#include "HttpRequest.hpp"

#include <cstddef>
#include <limits>

#include "LimitExcept.hpp"
#include "WebServer.hpp"

static const char *kHost = "Host";
static const char *kContentLength = "Content-Length";
static const char *kTransferEncoding = "Transfer-Encoding";
static const char *kChunk = "chunked";

HttpRequest::HttpRequest(config::REQUEST_METHOD method, const std::string &uri, const std::string &version,
                         const std::map<std::string, std::string, utils::CaseInsensitiveCompare> &headers,
                         const std::string &queries, const std::string &body, const std::string &port,
                         ParseState state)
    : method_(method),
      uri_(uri),
      version_(version),
      headers_(headers),
      queries_(queries),
      body_(body),
      port_in_host_(port),
      parse_state_(state),
      state_(0) {}

HttpRequest::HttpRequest(const HttpRequest &other) { *this = other; }

HttpRequest::~HttpRequest() {}

HttpRequest &HttpRequest::operator=(const HttpRequest &other) {
  if (this != &other) {
    this->method_ = other.method_;
    this->uri_ = other.uri_;
    this->version_ = other.version_;
    this->headers_ = other.headers_;
    this->queries_ = other.queries_;
    this->body_ = other.body_;
    this->port_in_host_ = other.port_in_host_;
    this->parse_state_ = other.parse_state_;
    this->key_buf_ = other.key_buf_;
    this->val_buf_ = other.val_buf_;
    this->spc_buf_ = other.spc_buf_;
    this->state_ = other.state_;
  }
  return *this;
}

void HttpRequest::parseRequest(std::string &raw_request, HttpRequest &request) {
  // 新たなリクエストの場合は初期化する
  if (!isParsePending(request)) {
    HttpRequest::clear(request);
  }
  ParseState &state = request.parse_state_;
  while (state != PARSE_COMPLETE) {
    ParseState state_before = state;
    switch (state) {
      case PARSE_BEFORE:
      case PARSE_METHOD_DONE:
      case PARSE_URI_DONE:
      case PARSE_VERSION_DONE:
        state = HttpRequest::parseRequestLine(raw_request, request);
        // errorもしくはparse未完了：引き続きクライアントからのrequestを待つ
        if (state != PARSE_REQUEST_LINE_DONE) return;
        break;
      case PARSE_REQUEST_LINE_DONE:
        state = HttpRequest::parseHeaders(raw_request, request);
        break;
      case PARSE_HEADER_DONE:
      case PARSE_INPROGRESS:
        if (request.headers_.find(kTransferEncoding) != request.headers_.end())
          state = HttpRequest::parseChunkedBody(raw_request, request);
        else
          state = HttpRequest::parseBody(raw_request, request);
        break;
      default:
        break;
    }
    if (state == PARSE_ERROR || state == PARSE_ERROR_BODY_TOO_LARGE || state == PARSE_NOT_IMPLEMENTED) return;
    // parse未完了：引き続きクライアントからのrequestを待つ
    if (state == state_before || state == PARSE_INPROGRESS) return;
  }
}

HttpRequest::ParseState HttpRequest::parseChunkedBody(std::string &raw_request, HttpRequest &request) {
  enum parseChunkPhase {
    sw_chunk_start = 0,
    sw_chunk_size,
    // sw_chunk_extension,
    sw_chunk_extension_almost_done,
    sw_chunk_data,
    sw_after_data,
    sw_after_data_almost_done,
    sw_last_chunk_extension,
    sw_last_chunk_extension_almost_done,
    sw_last_chunk_extension_done,
    sw_chunk_almost_end,
    sw_chunk_end
  } state;

  state = static_cast<parseChunkPhase>(request.state_);
  static const size_t kMaxChunkSize = std::numeric_limits<long>::max();
  size_t i = 0;
  size_t bytes;
  std::string chunk_bytes = request.key_buf_;
  std::string total_bytes = request.val_buf_.empty() ? "0" : request.val_buf_;
  const ConfigHandler &config_handler = WebServer::getConfigHandler();
  unsigned long cli_max_body_size = config_handler.searchCliMaxBodySize();
  while (state != sw_chunk_end && i < raw_request.size()) {
    unsigned char ch, c;
    ch = raw_request[i];

    switch (state) {
      case sw_chunk_start:
        if (std::isdigit(ch)) {
          chunk_bytes = ch;
          state = sw_chunk_size;
          break;
        }
        c = static_cast<unsigned char>(ch | 0x20);  // 小文字に変換
        if (c >= 'a' && c <= 'f') {
          chunk_bytes = c;
          state = sw_chunk_size;
          break;
        }
        return PARSE_ERROR;

      case sw_chunk_size:
        if (utils::hexToDec(chunk_bytes) > (kMaxChunkSize / 16)) return PARSE_ERROR;
        if (std::isdigit(ch)) {
          chunk_bytes += ch;
          break;
        }
        c = static_cast<unsigned char>(ch | 0x20);  // 小文字に変換
        if (c >= 'a' && c <= 'f') {
          chunk_bytes += c;
          break;
        }
        if (chunk_bytes == "0") {
          switch (ch) {
            case '\r':
              state = sw_last_chunk_extension_almost_done;
              break;
            case '\n':
              state = sw_last_chunk_extension_done;
              break;
            default:
              return PARSE_ERROR;
          }
          break;
        }
        switch (ch) {
          case '\r':
            state = sw_chunk_extension_almost_done;
            break;
          case '\n':
            state = sw_chunk_data;
            bytes = utils::hexToDec(chunk_bytes);
            if (cli_max_body_size != 0 &&
                isChunkBytesBiggerThanCliMaxBodySize(bytes, total_bytes, cli_max_body_size))
              return PARSE_ERROR_BODY_TOO_LARGE;
            chunk_bytes = utils::toStr(bytes);  // 10進数に変換
            break;
          default:
            return PARSE_ERROR;
        }
        break;

      case sw_chunk_extension_almost_done:
        if (ch != '\n') return PARSE_ERROR;
        state = sw_chunk_data;
        bytes = utils::hexToDec(chunk_bytes);
        if (cli_max_body_size != 0 &&
            isChunkBytesBiggerThanCliMaxBodySize(bytes, total_bytes, cli_max_body_size))
          return PARSE_ERROR_BODY_TOO_LARGE;
        chunk_bytes = utils::toStr(bytes);  // 10進数に変換
        break;

      case sw_chunk_data:
        bytes = utils::strToSizet(chunk_bytes);
        request.body_ += ch;
        --bytes;
        if (bytes == 0) {
          state = sw_after_data;
          chunk_bytes.clear();
          break;
        }
        chunk_bytes = utils::toStr(bytes);
        break;

      case sw_after_data:
        switch (ch) {
          case '\r':
            state = sw_after_data_almost_done;
            break;
          case '\n':
            state = sw_chunk_start;
            break;
          default:
            return PARSE_ERROR;
        }
        break;

      case sw_after_data_almost_done:
        switch (ch) {
          case '\n':
            state = sw_chunk_start;
            break;
          default:
            return PARSE_ERROR;
        }
        break;

      case sw_last_chunk_extension:
        switch (ch) {
          case '\r':
            state = sw_last_chunk_extension_almost_done;
            break;
          case '\n':
            state = sw_last_chunk_extension_done;
            break;
          default:
            return PARSE_ERROR;
        }
        break;

      case sw_last_chunk_extension_almost_done:
        switch (ch) {
          case '\n':
            state = sw_last_chunk_extension_done;
            break;
          default:
            return PARSE_ERROR;
        }
        break;

      case sw_last_chunk_extension_done:
        switch (ch) {
          case '\r':
            state = sw_chunk_almost_end;
            break;
          case '\n':
            state = sw_chunk_end;
            break;
          default:
            return PARSE_ERROR;
        }
        break;

      case sw_chunk_almost_end:
        switch (ch) {
          case '\n':
            state = sw_chunk_end;
            break;
          default:
            return PARSE_ERROR;
        }
        break;

      case sw_chunk_end:
        break;
    }
    ++i;
  }

  if (state != sw_chunk_end) {
    request.key_buf_ = chunk_bytes;
    request.val_buf_ = total_bytes;
    request.state_ = state;
    raw_request.clear();
    return PARSE_INPROGRESS;
  }
  resetBufs(request);
  raw_request = raw_request.substr(i);
  return PARSE_COMPLETE;
}

HttpRequest::ParseState HttpRequest::parseMethod(std::string &raw_request, HttpRequest &request) {
  enum ParseMethodPhase {
    sw_method_start = 0,
    sw_method_mid,
    sw_method_end
  } state;

  std::string method = request.key_buf_;

  state = static_cast<ParseMethodPhase>(request.state_);
  size_t i = 0;
  while (state != sw_method_end && i < raw_request.size()) {
    unsigned char ch = raw_request[i];
    switch (state) {
      case sw_method_start:
        if (ch < 'A' || ch > 'Z') return PARSE_ERROR;
        method += ch;
        state = sw_method_mid;
        break;
      case sw_method_mid:
        if (ch == ' ') {
          state = sw_method_end;
          break;
        }
        if (ch < 'A' || ch > 'Z') return PARSE_ERROR;
        method += ch;
        break;
      case sw_method_end:
        break;
    }
    ++i;
  }

  if (state != sw_method_end) {
    request.key_buf_ = method;
    request.state_ = state;
    raw_request.clear();
    return request.parse_state_;
  }

  switch (method.size()) {
    case 3:
      if (method == "GET") {
        request.method_ = config::GET;
        break;
      }
      return PARSE_ERROR;
    case 4:
      if (method == "HEAD") {
        request.method_ = config::HEAD;
        break;

      } else if (method == "POST") {
        request.method_ = config::POST;
        break;
      }
      return PARSE_ERROR;
    case 6:
      if (method == "DELETE") {
        request.method_ = config::DELETE;
        break;
      }
      break;
    default:
      return PARSE_ERROR;  // 501 Not Implemented (SHOULD)
  }
  resetBufs(request);
  raw_request = raw_request.substr(i);
  return PARSE_METHOD_DONE;
}

/**
 * @brief
 *
 * @param raw_request
 * @param request
 * @return HttpRequest::ParseState
 *
 * URLからスキーマ、ポート、パス、クエリーはparseしない
 *
 */
HttpRequest::ParseState HttpRequest::parseUri(std::string &raw_request, HttpRequest &request) {
  enum parseUriPhase {
    sw_start = 0,
    sw_slash_before_uri,
    sw_after_slash_in_uri,
    sw_uri,
    sw_end
  } state;

  state = static_cast<parseUriPhase>(request.state_);
  size_t i = 0;
  std::string uri = request.key_buf_;
  while (state != sw_end && i < raw_request.size()) {
    unsigned char ch = raw_request[i];
    switch (state) {
      case sw_start:
        state = sw_slash_before_uri;
        break;
      case sw_slash_before_uri:
        if (raw_request[i] != '/') return PARSE_ERROR;
        uri = ch;
        ++i;
        state = sw_after_slash_in_uri;
        break;
      case sw_after_slash_in_uri:
        state = sw_uri;
        break;
      case sw_uri:
        ++i;
        if (ch == ' ') {
          state = sw_end;
          break;
        }
        if (isInvalidLetter(ch)) return PARSE_ERROR;
        uri += ch;
        break;
      case sw_end:
        break;
    }
  }

  if (state != sw_end)  // parse未完了：引き続きクライアントからのrequestを待つ
  {
    request.state_ = state;
    request.key_buf_ = uri;
    raw_request.clear();
    return request.parse_state_;
  }

  uri = urlDecode(uri);
  size_t qindex = uri.find('?');
  std::string uri_part = uri.substr(0, qindex);
  if (!isValidUri(uri_part)) return PARSE_ERROR;
  request.uri_ = utils::normalizePath(uri_part);
  if (qindex != std::string::npos) request.queries_ = uri.substr(uri.find('?') + 1);

  resetBufs(request);
  raw_request = raw_request.substr(i);
  return PARSE_URI_DONE;
}

HttpRequest::ParseState HttpRequest::parseVersion(std::string &raw_request, HttpRequest &request) {
  enum parseVersionPhase {
    sw_start = 0,
    sw_H,
    sw_HT,
    sw_HTT,
    sw_HTTP,
    sw_HTTP_SL,  // slash
    sw_HTTP_SL1,
    sw_HTTP_SL1dot,
    sw_HTTP_SL1dot1,
    sw_almost_end,
    sw_end
  } state;

  state = static_cast<parseVersionPhase>(request.state_);
  size_t i = 0;
  std::string version = request.key_buf_;

  while (state != sw_end && i < raw_request.size()) {
    unsigned char ch = raw_request[i];
    switch (state) {
      case sw_start:
        switch (ch) {
          case 'H':
            version += ch;
            state = sw_H;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_H:
        switch (ch) {
          case 'T':
            version += ch;
            state = sw_HT;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_HT:
        switch (ch) {
          case 'T':
            version += ch;
            state = sw_HTT;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_HTT:
        switch (ch) {
          case 'P':
            version += ch;
            state = sw_HTTP;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_HTTP:
        switch (ch) {
          case '/':
            version += ch;
            state = sw_HTTP_SL;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      /* ver 1.1 のみ */
      case sw_HTTP_SL:
        switch (ch) {
          case '1':
            version += ch;
            state = sw_HTTP_SL1;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_HTTP_SL1:
        switch (ch) {
          case '.':
            version += ch;
            state = sw_HTTP_SL1dot;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_HTTP_SL1dot:
        switch (ch) {
          case '1':
            version += ch;
            state = sw_HTTP_SL1dot1;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_HTTP_SL1dot1:
        switch (ch) {
          case '\r':
            state = sw_almost_end;
            break;
          case '\n':
            state = sw_end;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_almost_end:
        switch (ch) {
          case '\n':
            state = sw_end;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_end:
        break;
    }
    ++i;
  }

  if (state != sw_end) {
    request.key_buf_ = version;
    request.state_ = state;
    raw_request.clear();
    return request.parse_state_;
  }
  request.version_ = version;
  resetBufs(request);
  raw_request = raw_request.substr(i);
  return PARSE_VERSION_DONE;
}

HttpRequest::ParseState HttpRequest::parseRequestLine(std::string &raw_request, HttpRequest &request) {
  ParseState &state = request.parse_state_;
  while (state != PARSE_REQUEST_LINE_DONE) {
    ParseState state_before = state;
    switch (state) {
      case PARSE_BEFORE:
        state = HttpRequest::parseMethod(raw_request, request);
        break;
      case PARSE_METHOD_DONE:
        state = HttpRequest::parseUri(raw_request, request);
        break;
      case PARSE_URI_DONE:
        state = HttpRequest::parseVersion(raw_request, request);
        break;
      case PARSE_VERSION_DONE:
        state = PARSE_REQUEST_LINE_DONE;
        break;
      case PARSE_REQUEST_LINE_DONE:
        break;
      default:
        break;
    };
    if (state == PARSE_ERROR) break;
    if (state == state_before) break;  // parse未完
  }
  return state;
}

/*
 * read each into hash??
 */
HttpRequest::ParseState HttpRequest::parseHeaders(std::string &raw_request, HttpRequest &request) {
  enum parseHeaderPhase {
    sw_start = 0,
    sw_name,
    sw_colon,
    sw_space_before_value,
    sw_value,
    sw_space_after_value,
    sw_header_almost_done,
    sw_header_done,
    sw_almost_end,
    sw_end
  } state;

  std::string cur_name = request.key_buf_;
  std::string cur_value = request.val_buf_;
  std::string cur_space = request.spc_buf_;  // header valueのspaceを格納
  state = static_cast<parseHeaderPhase>(request.state_);
  size_t i = 0;
  while (state != sw_end && i < raw_request.size()) {
    unsigned char ch = raw_request[i];
    switch (state) {
      case sw_start:
        if (ch == '\r') {
          ++i;
          state = sw_almost_end;
          break;
        }
        if (ch == '\n') {
          state = sw_almost_end;
          break;
        }
        // space以下, del, :はエラー
        if (isInvalidLetter(ch) || ch == ':') return PARSE_ERROR;
        state = sw_name;
        break;
      case sw_name:
        switch (ch) {
          case ':':
            state = sw_colon;
            break;
          case '\r':
            state = sw_header_almost_done;
            break;
          case '\n':
            state = sw_header_done;
            break;
          default:
            if (isInvalidLetter(ch)) return PARSE_ERROR;
            cur_name += ch;
            ++i;
            break;
        }
        break;
      case sw_colon:
        if (isUniqHeaderDup(request, cur_name)) return PARSE_ERROR;
        state = sw_space_before_value;
        ++i;
        break;
      case sw_space_before_value:
        if (ch != ' ')
          state = sw_value;
        else
          ++i;
        break;
      case sw_value:
        switch (ch) {
          case '\r':
            state = sw_header_almost_done;
            break;
          case '\n':
            state = sw_header_done;
            break;
          case ' ':
            state = sw_space_after_value;
            break;
          default:
            cur_value += ch;
            ++i;
            break;
        }
        break;

      /**
       * valueは複数ある場合がある
       * ex) ex1: v1, v2, v3
       * ex) ex2: v1 v2  v3
       */
      case sw_space_after_value:
        switch (ch) {
          case ' ':
            cur_space += ch;
            ++i;
            break;
          case '\r':
            state = sw_header_almost_done;
            break;
          case '\n':
            state = sw_header_done;
            break;
          default:
            cur_value += cur_space;
            cur_space.clear();
            state = sw_value;
            break;
        }
        break;
      case sw_header_almost_done:
        switch (ch) {
          case '\r':
            state = sw_header_done;
            ++i;
            break;
          case '\n':
            state = sw_header_done;
            break;
          default:
            return PARSE_ERROR;
        }
        break;
      case sw_header_done:
        if (ch != '\n') return PARSE_ERROR;
        if (isUniqHeaderDup(request, cur_name)) return PARSE_ERROR;
        if (utils::compareIgnoreCase(kHost, cur_name) && !parseHost(cur_value, request)) return PARSE_ERROR;
        if (utils::compareIgnoreCase(kContentLength, cur_name) && !isValidContentLength(cur_value))
          return PARSE_ERROR;
        // headerが重複している場合は、一番初めに登場したものを優先する
        if (request.headers_.find(cur_name) == request.headers_.end()) request.headers_[cur_name] = cur_value;
        cur_name.clear();
        cur_value.clear();
        cur_space.clear();
        state = sw_start;
        ++i;
        break;
      case sw_almost_end:
        if (ch != '\n') return PARSE_ERROR;
        state = sw_end;
        ++i;
        break;
      case sw_end:
        break;
    }
  }

  if (state != sw_end) {
    request.state_ = state;
    request.key_buf_ = cur_name;
    request.val_buf_ = cur_value;
    request.spc_buf_ = cur_space;
    raw_request.clear();
    return request.parse_state_;
  }
  resetBufs(request);
  raw_request = raw_request.substr(i);

  std::map<std::string, std::string, utils::CaseInsensitiveCompare>::const_iterator end_it, cl_it, te_it;
  end_it = request.headers_.end();
  if (request.headers_.find(kHost) == end_it) return PARSE_ERROR;
  te_it = request.headers_.find(kTransferEncoding);
  cl_it = request.headers_.find(kContentLength);
  if (te_it != end_it && !utils::compareIgnoreCase(kChunk, te_it->second))
    return PARSE_NOT_IMPLEMENTED;  // chunk以外は対応しない
  if (te_it != end_it && cl_it != end_it)
    return PARSE_ERROR;  // content-length, transfer-encoding: chunkedの二つが揃ってはいけない

  // client max body sizeをチェック
  if (cl_it != end_it) {
    const ConfigHandler &config_handler = WebServer::getConfigHandler();
    unsigned long cli_max_body_size = config_handler.searchCliMaxBodySize();
    unsigned long cl = utils::strToT<unsigned long>(cl_it->second);
    if (cli_max_body_size != 0 && cl >= cli_max_body_size) return PARSE_ERROR_BODY_TOO_LARGE;
  }

  if (te_it == end_it && cl_it == end_it) return PARSE_COMPLETE;  // bodyなし
  return PARSE_HEADER_DONE;
}

/**
 * @brief host headerをhost部とport部に分けて、parseする
 *
 * ex) host: localhost:8000  -> localhost, :8000
 */
bool HttpRequest::parseHost(std::string &host, HttpRequest &request) {
  if (!isValidHost(host)) return false;
  // すでにhostをparse済みの場合は、何もしない
  if (request.headers_.find(kHost) != request.headers_.end()) return true;
  size_t i = host.find(':');
  if (i != std::string::npos) {
    request.port_in_host_ = host.substr(i);  // :も含んだ値で保持する
    host = host.substr(0, i);
  }
  return true;
}

/**
 * @brief content-lengthのサイズ分bodyをparseする
 *
 * @param raw_request
 * @param request
 * @return HttpRequest::ParseState
 */
HttpRequest::ParseState HttpRequest::parseBody(std::string &raw_request, HttpRequest &request) {
  size_t content_length = utils::strToSizet(request.headers_.find(kContentLength)->second);
  if (content_length == 0) return PARSE_COMPLETE;
  std::string body = raw_request.substr(0, content_length);
  request.body_ += body;
  size_t parsed_body_size = body.size();
  raw_request = raw_request.substr(parsed_body_size);  // bodyからはみ出た部分は次のリクエストに追加される
  if (parsed_body_size == content_length) return PARSE_COMPLETE;
  // parse未完了：引き続きbodyを待つ
  request.headers_[kContentLength] =
      utils::toStr(content_length - parsed_body_size);  // 残りのbodyのsizeをupdate
  return PARSE_INPROGRESS;
}

/*
 * https://www.techieclues.com/blogs/convert-url-encoding-to-string-in-cpp
 */
std::string HttpRequest::urlDecode(const std::string &encoded) {
  std::string decoded;
  for (std::size_t i = 0; i < encoded.size(); ++i) {
    if (encoded[i] == '%' && i + 2 < encoded.size()) {
      int hexValue = std::strtol(encoded.substr(i + 1, 2).c_str(), NULL, 16);
      decoded += static_cast<unsigned char>(hexValue);
      i += 2;
    } else {
      decoded += encoded[i];
    }
  }
  return decoded;
}

/**
 * @brief parse用変数を初期化
 *
 * @param request
 */
void HttpRequest::resetBufs(HttpRequest &request) {
  request.state_ = 0;
  request.key_buf_.clear();
  request.val_buf_.clear();
  request.spc_buf_.clear();
}

/**
 * @brief host, content-length, transfer-encodingが重複しているかチェック
 *
 * @param request
 * @param header
 * @return true
 * @return false
 */
bool HttpRequest::isUniqHeaderDup(const HttpRequest &request, const std::string &header) {
  if (!utils::compareIgnoreCase(header, kHost) && !utils::compareIgnoreCase(header, kContentLength) &&
      !utils::compareIgnoreCase(header, kTransferEncoding))
    return false;
  return request.headers_.find(header) != request.headers_.end();
}

/**
 * @brief ascii: space以下, delはheaderの名前, uriに受け付けない
 *
 * @param ch
 * @return true
 * @return false
 */
bool HttpRequest::isInvalidLetter(unsigned char ch) { return ch <= ' ' || ch == 127; }

/**
 * @brief hostの値が正当かチェック
 * 値が空はerror
 * spaceが含まれているとerror
 * 値が:で始まるとerror
 *
 * @param str
 * @return true
 * @return false
 */
bool HttpRequest::isValidHost(const std::string &str) {
  return !str.empty() && str.find(' ') == std::string::npos && str.find(':') != 0;
}

bool HttpRequest::isValidContentLength(const std::string &str) {
  if (!utils::isNumeric(str)) return false;
  unsigned long length;
  std::istringstream iss(str);
  iss >> length;

  if (iss.fail() || iss.bad() || iss.peek() != EOF) return false;
  return length <= static_cast<unsigned long>(std::numeric_limits<long>::max());
}

/**
 * @brief rootを遡ろうとするuriはerrorとする
 * ex) /..  /test/../..
 */
bool HttpRequest::isValidUri(const std::string &str) {
  std::vector<std::string> components;
  std::istringstream ss(str);
  std::string token;

  while (std::getline(ss, token, '/')) {
    if (token.empty() || token == ".") {
      continue;
    }
    if (token == "..") {
      if (components.empty()) return false;
      components.pop_back();
      continue;
    }
    components.push_back(token);
  }
  return true;
}

/**
 * @brief chunked bodyがclient max body sizeを超えるかチェック
 * total_bytesの値を更新
 */
bool HttpRequest::isChunkBytesBiggerThanCliMaxBodySize(size_t chunk_bytes, std::string &total_bytes,
                                                       size_t cli_max_body_size) {
  if (chunk_bytes >= cli_max_body_size) return true;
  size_t tb = utils::strToSizet(total_bytes);
  // check overflow
  if (std::numeric_limits<size_t>::max() - tb < chunk_bytes) return true;
  size_t total = chunk_bytes + tb;
  // total bytesの値を更新
  total_bytes = utils::toStr(total);
  return total >= cli_max_body_size;
}

bool HttpRequest::isParsePending(const HttpRequest &request) {
  enum ParseState state = request.parse_state_;
  return state != PARSE_COMPLETE && state != PARSE_ERROR && state != PARSE_ERROR_BODY_TOO_LARGE &&
         state != PARSE_NOT_IMPLEMENTED;
}
void HttpRequest::clear(HttpRequest &request) {
  request.method_ = config::UNKNOWN;
  request.uri_.clear();
  request.queries_.clear();
  request.version_.clear();
  request.headers_.clear();
  request.body_.clear();
  request.port_in_host_.clear();
  request.resetBufs(request);
  request.parse_state_ = PARSE_BEFORE;
}
