#include "HttpRequest.hpp"

#include "LimitExcept.hpp"

const static char*  kHost = "Host";
const static char*  kContentLength = "Content-Length";
const static char*  kTransferEncoding = "Transfer-Encoding";

HttpRequest::HttpRequest(const config::REQUEST_METHOD &method, const std::string &uri,
                         const std::string &version,
                         const std::map<std::string, std::string, Utils::CaseInsensitiveCompare> &headers,
                         const std::string &queries, const std::string &body, const ParseState parseState)
    : method(method),
      uri(uri),
      version(version),
      headers(headers),
      queries(queries),
      body(body),
      parseState(parseState),
      state_(0) {}

HttpRequest::~HttpRequest() {}

void HttpRequest::parseRequest(std::string &rawRequest, HttpRequest &request) {
  ParseState state = request.parseState;
  while (state != PARSE_COMPLETE) {
    ParseState state_before = state;
    switch (state) {
      case PARSE_BEFORE:
      case PARSE_METHOD_DONE:
      case PARSE_URI_DONE:
      case PARSE_VERSION_DONE:
        state = HttpRequest::parseRequestLine(rawRequest, request);
        if (state != PARSE_REQUEST_LINE_DONE) { // errorもしくはparse未完了：引き続きクライアントからのrequestを待つ
          request.parseState = state;
          return;
        }
        break;
      case PARSE_REQUEST_LINE_DONE:
        state = HttpRequest::parseHeaders(rawRequest, request);
        if (state == PARSE_ERROR) break;
        if (request.headers.find(kTransferEncoding) == request.headers.end() && request.headers.find(kContentLength) == request.headers.end())
          state = PARSE_COMPLETE;
        break;
      case PARSE_HEADER_DONE:
      case PARSE_INPROGRESS:
        if (request.headers.find(kTransferEncoding) != request.headers.end())
          state = HttpRequest::doParseChunked(rawRequest, request);
        else
          state = HttpRequest::parseBody(rawRequest, request);
        break;
      default:
        break;
    }
    if (state == PARSE_ERROR) break;
    if (state == state_before || state == PARSE_INPROGRESS) // parse未完了：引き続きクライアントからのrequestを待つ
      break;
  }
  request.parseState = state;
}

HttpRequest::ParseState HttpRequest::doParseChunked(std::string &rawRequest, HttpRequest &request) {
  std::string byteSize = rawRequest.substr(0, rawRequest.find('\r'));
  ParseState state;
  if (byteSize == "0")
    state = HttpRequest::PARSE_COMPLETE;
  else
    state = HttpRequest::PARSE_INPROGRESS;
  std::string chunkBody = rawRequest.substr(rawRequest.find('\n') + 1);
  request.body += chunkBody.substr(0, chunkBody.find('\r'));
  return state;
}

HttpRequest::ParseState HttpRequest::parseMethod(std::string &rawRequest, HttpRequest &request) {
  enum ParseMethodPhase {
    sw_method_start = 0,
    sw_method_mid,
    sw_method_almost_end,
    sw_method_end,
  } state;

  std::string method = request.key_buf_;

  state = static_cast<ParseMethodPhase>(request.state_);
  size_t i = 0;
  while (state != sw_method_end && i < rawRequest.size()) {
    unsigned char ch = rawRequest[i];
    switch (state) {
      case sw_method_start:
        method += ch;
        state = sw_method_mid;
        break;
      case sw_method_mid:
        if (std::isalpha(ch)) {
          method += ch;
        } else if (ch == ' ') {
          state = sw_method_almost_end;
        } else {
          return PARSE_ERROR;
        }
        break;
      case sw_method_almost_end:
        rawRequest = rawRequest.substr(i);
        state = sw_method_end;
        break;
      case sw_method_end:
        break;
    }
    ++i;
  }

  if (state != sw_method_end) {
    request.key_buf_ = method;
    request.state_ = state;
    return request.parseState;
  }
  request.state_ = 0; // reset;

  switch (method.size()) {
    case 3:
      if (method == "GET") {
        request.method = config::GET;
        break;
      }
      return PARSE_ERROR;
    case 4:
      if (method == "HEAD") {
        request.method = config::HEAD;
        break;

      } else if (method == "POST") {
        request.method = config::POST;
        break;
      }
      return PARSE_ERROR;
    case 6:
      if (method == "DELETE") {
        request.method = config::DELETE;
        break;
      }
      break;
    default:
      return PARSE_ERROR;  // 501 Not Implemented (SHOULD)
  }
  request.key_buf_.clear();
  return HttpRequest::PARSE_METHOD_DONE;
}

/*
 * URLのパース
 * URIかと思っていた、、
 * URLからスキーマ、ポート、パス、クエリーに分解する？
 */
HttpRequest::ParseState HttpRequest::parseUri(std::string &rawRequest, HttpRequest &request) {
  enum parseUriPhase {
    sw_start = 0,
    sw_slash_before_uri,
    sw_schema,
    sw_almost_end,
    sw_end
  } state;

  state = static_cast<parseUriPhase>(request.state_);
  size_t i = 0;
  while (state != sw_end && i < rawRequest.size()) {
    switch (state) {
      case sw_start:
        state = sw_slash_before_uri;
        break;
      case sw_slash_before_uri:
        if (rawRequest[i] != '/') return PARSE_ERROR;
        ++i;
        state = sw_schema;
        break;
      case sw_schema:
        state = sw_almost_end;
        break;
      case sw_almost_end:
        state = sw_end;
        break;
      case sw_end:
        break;
    }
  }

  if (state != sw_end) // parse未完了：引き続きクライアントからのrequestを待つ
  {
    request.state_ = state;
    return request.parseState;
  }
  request.state_ = 0; // reset

  std::string tmp = rawRequest.substr(0, rawRequest.find(' '));
  tmp = urlDecode(tmp);
  request.uri = tmp.substr(0, tmp.find('?'));
  size_t qindex = tmp.find('?');
  if (qindex != std::string::npos) request.queries = tmp.substr(tmp.find('?') + 1);
  rawRequest = rawRequest.substr(rawRequest.find(' ') + 1);

  return HttpRequest::PARSE_URI_DONE;
}

HttpRequest::ParseState HttpRequest::parseVersion(std::string &rawRequest, HttpRequest &request) {
  enum parseVersionPhase {
    sw_start = 0,
    sw_H,
    sw_HT,
    sw_HTT,
    sw_HTTP,
    sw_HTTP_SL, // slash
    sw_HTTP_SL1,
    sw_HTTP_SL1dot,
    sw_HTTP_SL1dot1,
    sw_almost_end,
    sw_end
  } state;

  state = static_cast<parseVersionPhase>(request.state_);
  size_t i = 0;
  std::string version = request.key_buf_;

  while (state != sw_end && i < rawRequest.size())
  {
    unsigned char ch = rawRequest[i];
    switch (state)
    {
    case sw_start:
      switch (ch)
      {
      case 'H':
        version += ch;
        state = sw_H;
        break;
      default:
        return PARSE_ERROR;
      }
      break;
    case sw_H:
      switch (ch)
      {
      case 'T':
        version += ch;
        state = sw_HT;
        break;
      default:
        return PARSE_ERROR;
      }
      break;
    case sw_HT:
      switch (ch)
      {
      case 'T':
        version += ch;
        state = sw_HTT;
        break;
      default:
        return PARSE_ERROR;
      }
      break;
    case sw_HTT:
      switch (ch)
      {
      case 'P':
        version += ch;
        state = sw_HTTP;
        break;
      default:
        return PARSE_ERROR;
      }
      break;
    case sw_HTTP:
      switch (ch)
      {
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
      switch (ch)
      {
      case '1':
        version += ch;
        state = sw_HTTP_SL1;
        break;
      default:
        return PARSE_ERROR;
      }
      break;
    case sw_HTTP_SL1:
      switch (ch)
      {
      case '.':
        version += ch;
        state = sw_HTTP_SL1dot;
        break;
      default:
        return PARSE_ERROR;
      }
      break;
    case sw_HTTP_SL1dot:
      switch (ch)
      {
      case '1':
        version += ch;
        state = sw_HTTP_SL1dot1;
        break;
      default:
        return PARSE_ERROR;
      }
      break;
    case sw_HTTP_SL1dot1:
      switch (ch)
      {
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
      switch (ch)
      {
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
    return request.parseState;
  }
  request.version = version;
  request.state_ = 0; // reset
  request.key_buf_.clear();
  rawRequest = rawRequest.substr(i);
  return HttpRequest::PARSE_VERSION_DONE;
}

HttpRequest::ParseState HttpRequest::parseRequestLine(std::string &rawRequest, HttpRequest &request) {
  ParseState state = request.parseState;
  while (state != PARSE_REQUEST_LINE_DONE) {
    ParseState state_before = state;
    switch (state) {
      case PARSE_BEFORE:
        state = HttpRequest::parseMethod(rawRequest, request);
        break;
      case PARSE_METHOD_DONE:
        state = HttpRequest::parseUri(rawRequest, request);
        break;
      case PARSE_URI_DONE:
        state = HttpRequest::parseVersion(rawRequest, request);
        break;
      case PARSE_VERSION_DONE:
        state = PARSE_REQUEST_LINE_DONE;
        break;
      case PARSE_REQUEST_LINE_DONE:
        break;
      default:
        break;
    };
    if (state == PARSE_ERROR) return state;
    if (state == state_before) break;
  }
  return state;
}

/*
 * read each into hash??
 */
HttpRequest::ParseState HttpRequest::parseHeaders(std::string &rawRequest, HttpRequest &request) {
  enum parseHeaderPhase {
    sw_start = 0,
    sw_name,
    sw_colon,
    sw_space_before_value,
    sw_value,
    sw_space_after_value,
    sw_header_almost_done,
    sw_header_done,
    sw_end
  } state;

  std::string cur_name = request.key_buf_;
  std::string cur_value = request.val_buf_;
  state = static_cast<parseHeaderPhase>(request.state_);
  size_t i = 0;
  while (state != sw_end && i < rawRequest.size()) {
    unsigned char ch = rawRequest[i];
    switch (state) {
      case sw_start:
        if (ch == '\r') {
          ++i;
          if (rawRequest[i] != '\n') return PARSE_ERROR;
          break;
        }
        if (ch == '\n') {
          ++i;
          state = sw_end;
          break;
        }
        // space以下, del, :はエラー
        if (isInvalidHeaderLetter(ch) || ch == ':') return PARSE_ERROR;
        state = sw_name;
        break;
      case sw_name:
        switch (ch)
        {
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
          if (isInvalidHeaderLetter(ch)) return PARSE_ERROR;
          cur_name += ch;
          ++i;
          break;
        }
        break;
      case sw_colon:
        if (isUniqueHeaderDup(request, cur_name)) return PARSE_ERROR;
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
        switch (ch)
        {
        case '\r':
          state = sw_header_almost_done;
          break;
        case '\n':
          state = sw_header_done;
          break;
        case ' ':
          state = sw_space_after_value;
        default:
          cur_value += ch;
          ++i;
          break;
        }
        break;
      case sw_space_after_value:
        if (ch != ' ') {
          state = sw_header_almost_done;
        } else {
          ++i;
        }
        break;
      case sw_header_almost_done:
        if (ch != '\r') return PARSE_ERROR;
        state = sw_header_done;
        ++i;
        break;
      case sw_header_done:
        if (ch != '\n') return PARSE_ERROR;
        if (isUniqueHeaderDup(request, cur_name)) return PARSE_ERROR;
        if (Utils::compareIgnoreCase(kHost, cur_name) && !isValidHost(cur_value)) return PARSE_ERROR;
        if (Utils::compareIgnoreCase(kContentLength, cur_name) && !isValidContentLength(cur_value)) return PARSE_ERROR;
        // headerが重複している場合は、一番初めに登場したものを優先する
        if (request.headers.find(cur_name) == request.headers.end())
          request.headers[cur_name] = cur_value;
        cur_name.clear();
        cur_value.clear();
        state = sw_start;
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
    return request.parseState;
  }
  request.state_ = 0; // reset
  if (request.headers.find(kHost) == request.headers.end()) return PARSE_ERROR;
  rawRequest = rawRequest.substr(i);
  clearBuf(request);
  return HttpRequest::PARSE_HEADER_DONE;
}

HttpRequest::ParseState HttpRequest::parseBody(std::string &rawRequest, HttpRequest &request) { 
  size_t  body_size = rawRequest.size();
  size_t  content_length = Utils::strToSizet(request.headers.find(kContentLength)->second);
  request.body = rawRequest.substr(0, content_length);
  rawRequest = rawRequest.substr(content_length); // bodyからはみ出た部分は次のリクエストに追加される
  if (content_length > body_size)
    return PARSE_INPROGRESS;
  return PARSE_COMPLETE;
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

void HttpRequest::clearBuf(HttpRequest &request) {
  request.key_buf_.clear();
  request.val_buf_.clear();
}

/**
 * @brief host, content-length, transfer-encodingが重複しているかチェック
 * 
 * @param request 
 * @param header 
 * @return true 
 * @return false 
 */
bool HttpRequest::isUniqueHeaderDup(const HttpRequest &request, const std::string &header) {
  if (!Utils::compareIgnoreCase(header, kHost) && !Utils::compareIgnoreCase(header, kContentLength) && !Utils::compareIgnoreCase(header, kContentLength)) return false;
  return request.headers.find(header) != request.headers.end();
}

/**
 * @brief ascii: space以下, delはheaderの名前に受け付けない
 * 
 * @param ch 
 * @return true 
 * @return false 
 */
bool HttpRequest::isInvalidHeaderLetter(unsigned char ch) {
  return ch <= ' ' || ch == 127;  
}

bool HttpRequest::isValidHost(const std::string &str) {
  return !str.empty();
}

bool HttpRequest::isValidContentLength(const std::string &str) {
  unsigned long length;
  std::istringstream iss(str);
  iss >> length;

  if (iss.fail()) return false;
  return length <= static_cast<unsigned long>(std::numeric_limits<long>::max());
}
