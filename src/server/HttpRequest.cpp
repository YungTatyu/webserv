#include "HttpRequest.hpp"

#include "LimitExcept.hpp"

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

void HttpRequest::parseRequest(std::string &rawRequest, HttpRequest &oldRequest) {
  if (oldRequest.parseState == PARSE_HEADER_DONE)
  {
    HttpRequest::doParseChunked(rawRequest, oldRequest);
    return;
  }
  oldRequest = HttpRequest::doParseRequest(rawRequest, oldRequest);
}

HttpRequest HttpRequest::doParseRequest(std::string &rawRequest, HttpRequest &oldRequest) {
  HttpRequest newRequest;
  ParseState state = oldRequest.parseState;
  while (state != PARSE_COMPLETE) {
    ParseState state_before = state;
    switch (state) {
      case PARSE_BEFORE:
        state = HttpRequest::parseRequestLine(rawRequest, newRequest);
        break;
      case PARSE_REQUEST_LINE_DONE:
        state = HttpRequest::parseHeaders(rawRequest, newRequest);
        if (state == PARSE_ERROR) break;
        if (newRequest.headers.find(kTransferEncoding) == newRequest.headers.end() && newRequest.headers.find(kContentLength) == newRequest.headers.end())
          state = PARSE_COMPLETE;
        break;
      case PARSE_HEADER_DONE:
      case PARSE_INPROGRESS:
        if (newRequest.headers.find(kTransferEncoding) != newRequest.headers.end())
          state = HttpRequest::doParseChunked(rawRequest, newRequest);
        else
          state = HttpRequest::parseBody(rawRequest, newRequest);
        break;
      default:
        break;
    }
    if (state == PARSE_ERROR) break;
    if (state == state_before || state == PARSE_INPROGRESS) // parse未完了：引き続きクライアントからのrequestを待つ
      break;
  }
  newRequest.state_ = state;
  return newRequest;
}

HttpRequest::ParseState HttpRequest::doParseChunked(std::string &rawRequest, HttpRequest &oldRequest) {
  std::string byteSize = rawRequest.substr(0, rawRequest.find('\r'));
  ParseState state;
  if (byteSize == "0")
    state = HttpRequest::PARSE_COMPLETE;
  else
    state = HttpRequest::PARSE_INPROGRESS;
  std::string chunkBody = rawRequest.substr(rawRequest.find('\n') + 1);
  oldRequest.body += chunkBody.substr(0, chunkBody.find('\r'));
  return state;
}

HttpRequest::ParseState HttpRequest::parseMethod(std::string &rawRequest, HttpRequest &newRequest) {
  enum ParseMethodPhase {
    sw_method_start = 0,
    sw_method_mid,
    sw_method_almost_end,
    sw_method_end,
  } state;

  std::string method = newRequest.key_buf_;

  state = static_cast<ParseMethodPhase>(newRequest.state_);
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
          return HttpRequest::PARSE_ERROR;
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
    newRequest.key_buf_ = method;
    newRequest.state_ = state;
    return newRequest.parseState;
  }
  newRequest.state_ = 0; // reset;

  switch (method.size()) {
    case 3:
      if (method == "GET") {
        newRequest.method = config::GET;
        break;
      }
      return HttpRequest::PARSE_ERROR;
    case 4:
      if (method == "HEAD") {
        newRequest.method = config::HEAD;
        break;

      } else if (method == "POST") {
        newRequest.method = config::POST;
        break;
      }
      return HttpRequest::PARSE_ERROR;
    case 6:
      if (method == "DELETE") {
        newRequest.method = config::DELETE;
        break;
      }
      break;
    default:
      return HttpRequest::PARSE_ERROR;  // 501 Not Implemented (SHOULD)
  }
  return HttpRequest::PARSE_METHOD_DONE;
}

/*
 * URLのパース
 * URIかと思っていた、、
 * URLからスキーマ、ポート、パス、クエリーに分解する？
 */
HttpRequest::ParseState HttpRequest::parseUri(std::string &rawRequest, HttpRequest &newRequest) {
  enum parseUriPhase {
    sw_start = 0,
    sw_slash_before_uri,
    sw_schema,
    sw_almost_end,
    sw_end
  } state;

  state = static_cast<parseUriPhase>(newRequest.state_);
  size_t i = 0;
  while (state != sw_end && i < rawRequest.size()) {
    switch (state) {
      case sw_start:
        state = sw_slash_before_uri;
        break;
      case sw_slash_before_uri:
        if (rawRequest[i] != '/') return HttpRequest::PARSE_ERROR;
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
    newRequest.state_ = state;
    return newRequest.parseState;
  }
  newRequest.state_ = 0; // reset

  std::string tmp = rawRequest.substr(0, rawRequest.find(' '));
  tmp = urlDecode(tmp);
  newRequest.uri = tmp.substr(0, tmp.find('?'));
  size_t qindex = tmp.find('?');
  if (qindex != std::string::npos) newRequest.queries = tmp.substr(tmp.find('?') + 1);
  rawRequest = rawRequest.substr(rawRequest.find(' ') + 1);

  return HttpRequest::PARSE_URI_DONE;
}

HttpRequest::ParseState HttpRequest::parseVersion(std::string &rawRequest, HttpRequest &newRequest) {
  enum parseVersionPhase {
    sw_start = 0,
    sw_H,
    sw_HT,
    sw_HTT,
    sw_HTTP,
    sw_HTTP1,
    sw_HTTP1dot,
    sw_HTTP1dot1,
    sw_almost_end,
    sw_end
  } state;

  state = static_cast<parseVersionPhase>(newRequest.state_);
  size_t i = 0;
  std::string version;

  while (state != sw_end || i < rawRequest.size())
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
        return HttpRequest::PARSE_VERSION_ERROR;
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
        return HttpRequest::PARSE_VERSION_ERROR;
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
        return HttpRequest::PARSE_VERSION_ERROR;
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
        return HttpRequest::PARSE_VERSION_ERROR;
      }
      break;
    /* ver 1.1 のみ */
    case sw_HTTP:
      switch (ch)
      {
      case '1':
        version += ch;
        state = sw_HTTP1;
        break;
      default:
        return HttpRequest::PARSE_VERSION_ERROR;
      }
      break;
    case sw_HTTP1:
      switch (ch)
      {
      case '.':
        version += ch;
        state = sw_HTTP1dot;
        break;
      default:
        return HttpRequest::PARSE_VERSION_ERROR;
      }
      break;
    case sw_HTTP1dot:
      switch (ch)
      {
      case '1':
        version += ch;
        state = sw_HTTP1dot1;
        break;
      default:
        return HttpRequest::PARSE_VERSION_ERROR;
      }
      break;
    case sw_HTTP1dot1:
      switch (ch)
      {
      case '\r':
        state = sw_almost_end;
        break;
      case '\n':
        state = sw_end;
        break;
      default:
        return HttpRequest::PARSE_VERSION_ERROR;
      }
      break;
    case sw_almost_end:
      switch (ch)
      {
      case '\n':
        state = sw_end;
        break;
      default:
        return HttpRequest::PARSE_VERSION_ERROR;
      }
      break;
    case sw_end:
      break;
    }
    ++i;
  }

  if (state != sw_end) {
    newRequest.state_ = state;
    return newRequest.parseState;
  }
  newRequest.state_ = 0; // reset
  newRequest.version = version;  
  rawRequest = rawRequest.substr(i);
  return HttpRequest::PARSE_VERSION_DONE;
}

HttpRequest::ParseState HttpRequest::parseRequestLine(std::string &rawRequest, HttpRequest &newRequest) {
  enum parseRequestLineState {
    sw_start = 0,
    sw_method,
    sw_uri,
    sw_version,
    sw_end
  } state;

  state = static_cast<parseRequestLineState>(newRequest.state_);
  size_t i = 0;
  if (rawRequest.empty()) return HttpRequest::PARSE_ERROR;
  while (state != sw_end && i < rawRequest.size()) {
    switch (state) {
      case sw_start:
        state = sw_method;
        break;
      case sw_method:
        if (HttpRequest::parseMethod(rawRequest, newRequest) == HttpRequest::PARSE_ERROR)
          return HttpRequest::PARSE_ERROR;
        state = sw_uri;
        break;
      case sw_uri:
        if (HttpRequest::parseUri(rawRequest, newRequest) == HttpRequest::PARSE_ERROR)
          return HttpRequest::PARSE_ERROR;
        state = sw_version;
        break;
      case sw_version:
        if (HttpRequest::parseVersion(rawRequest, newRequest) == HttpRequest::PARSE_ERROR)
          return HttpRequest::PARSE_ERROR;
        state = sw_end;
        break;
      case sw_end:
        break;
    };
    ++i;
  }
  if (state != sw_end) // parse未完了：引き続きクライアントからのrequestを待つ
  {
    newRequest.state_ = state;
    return newRequest.parseState;
  }
  newRequest.state_ = 0;
  return HttpRequest::PARSE_REQUEST_LINE_DONE;
}

/*
 * read each into hash??
 */
HttpRequest::ParseState HttpRequest::parseHeaders(std::string &rawRequest, HttpRequest &newRequest) {
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

  std::string cur_name = newRequest.key_buf_;
  std::string cur_value = newRequest.val_buf_;
  state = sw_start;
  size_t i = 0;
  while (state != sw_end && i < rawRequest.size()) {
    unsigned char ch = rawRequest[i];
    switch (state) {
      case sw_start:
        if (ch == '\r') {
          ++i;
          if (rawRequest[i] != '\n') return HttpRequest::PARSE_ERROR;
          break;
        }
        if (ch == '\n') {
          ++i;
          state = sw_end;
          break;
        }
        if (!std::isalnum(ch))
          state = sw_end;
        else
          state = sw_name;
        break;
      case sw_name:
        if (!cur_name.empty() && ch == ':') {
          newRequest.headers[cur_name];
          state = sw_colon;
        } else {
          cur_name += ch;
          ++i;
        }
        break;
      case sw_colon:
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
        // if (!std::isprint(ch) && ch < 128) {
        if (ch == '\r' || ch == '\n') {  // TODO: logic考える必要あり
          newRequest.headers[cur_name] = cur_value;
          cur_name.clear();
          cur_value.clear();
          state = sw_space_after_value;
        } else {
          cur_value += ch;
          ++i;
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
        if (ch != '\r') return HttpRequest::PARSE_ERROR;
        state = sw_header_done;
        ++i;
        break;
      case sw_header_done:
        if (ch != '\n') return HttpRequest::PARSE_ERROR;
        state = sw_start;
        ++i;
        break;
      case sw_end:
        break;
    }
  }

  if (state != sw_end) {
    newRequest.state_ = state;
    newRequest.key_buf_ = cur_name;
    newRequest.val_buf_ = cur_value;
    return newRequest.parseState;
  }
  newRequest.state_ = 0; // reset
  if (newRequest.headers.find("Host") == newRequest.headers.end()) return HttpRequest::PARSE_ERROR;
  rawRequest = rawRequest.substr(i);
  return HttpRequest::PARSE_HEADER_DONE;
}

HttpRequest::ParseState HttpRequest::parseBody(std::string &body, HttpRequest &newRequest) { 
  size_t  body_size = body.size();
  size_t  content_length = Utils::strToSizet(newRequest.headers.find(kContentLength)->second);
  newRequest.body = body.substr(0, content_length);
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
