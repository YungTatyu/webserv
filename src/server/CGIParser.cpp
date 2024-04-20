#include "CGIParser.hpp"

#include <cctype>
#include <utility>
#include <functional>
#include <sstream>
#include <limits>
#include <algorithm>

const static char	*kContentLength = "content-length";
const static char	*kStatus = "status";

cgi::CGIParser::CGIParser() :
	headers_(NULL), body_(NULL), status_code_(NULL), status_code_line_(NULL), ri_(0) {}

cgi::CGIParser::~CGIParser() {}

void	cgi::CGIParser::init(HttpResponse& http_response)
{
	this->headers_ = &(http_response.headers_);
	this->body_ = &(http_response.body_);
	this->status_code_ = &(http_response.status_code_);
	this->status_code_line_ = &(http_response.cgi_status_code_line_);

	this->headers_->clear();
	this->body_->clear();
	*(this->status_code_) = 0;
	this->status_code_line_->clear();
}

bool	cgi::CGIParser::parse(
	HttpResponse& http_response,
	const std::string& cgi_response,
	const PARSE_STATE init_state
)
{
	init(http_response);
	this->state_ = init_state;
	while (this->state_ != cgi::PARSE_COMPLETE)
	{
		switch (this->state_)
		{
		case PARSE_BEFORE:
			parseHeaders(cgi_response);
			break;
		case PARSE_HEADER_DONE:
			parseBody(cgi_response);
			break;
		case PARSE_BODY_DONE:
			this->state_ = PARSE_COMPLETE;
			break;
		default:
			break;
		}

		if (this->state_ == PARSE_ERROR)
			return false;
	}
	return true;
}

void	cgi::CGIParser::parseHeaders(const std::string& response)
{
	enum PARSE_HEADER_PHASE
	{
		sw_start,
		sw_error,
		sw_name,
		sw_colon,
		sw_value,
		sw_space_before_value,
		sw_status_code,
		sw_status_reason_phrase,
		sw_cl_value, // content-length
		sw_dup_value,
		sw_space_after_value,
		sw_header_almost_done,
		sw_header_done,
		sw_almost_end,
		sw_end
	};

	if (response.size() == 0)
	{
		this->state_ = PARSE_ERROR;
		return;
	}

	const static char	*kContentType = "content-type";
	ri_ = 0;
	PARSE_HEADER_PHASE	state = sw_start;
	std::string	cur_name;
	std::string	cur_value;
	while (state != sw_end)
	{
		const unsigned char	ch = response[ri_];
		switch (state)
		{
		case sw_start:
			cur_name.clear();
			cur_value.clear();
			switch (ch)
			{
			case '\r':
				state = sw_almost_end;
				++ri_;
				break;
			case '\n':
				state = sw_almost_end;
				break;
			default:
				if (std::isspace(ch))
				{
					state = sw_error;
					break;
				}
				state = sw_name;
				break;
			}
			break;
		
		case sw_name:
			switch (ch)
			{
			case ':':
				if (cur_name.empty())
				{
					state = sw_error;
					break;
				}
				state = sw_colon;
				break;
			case '\r':
			case '\n':
				state = sw_header_almost_done;
				break;
			default:
				// 記号で始まるheaderはerrorにする
				if ((cur_name.empty() && !std::isalnum(ch))
					|| !std::isprint(ch))
				{
					state = sw_error;
					break;
				}
				cur_name += ch;
				++ri_;
				break;
			}
			break;

		case sw_colon:
		{
			++ri_;
			/**
			 * headerが重複している場合は、syntaxを見ない
			 * 一番初めに設定されたheaderの値が適応される
			 */
			if (this->headers_->find(cur_name) != this->headers_->end())
			{
				state = sw_dup_value;
				break;
			}
			state = sw_space_before_value;
			break;
		}

		case sw_space_before_value:
			if (ch == ' ')
			{
				++ri_;
				break;
			}
			if (Utils::compareIgnoreCase(cur_name, kStatus))
			{
				state = sw_status_code;
				break;
			}
			if (Utils::compareIgnoreCase(cur_name, kContentLength))
			{
				state = sw_cl_value;
				break;
			}
			state = sw_value;
			break;

		case sw_value:
			switch (ch)
			{
			case '\r':
			case '\n':
				state = sw_header_almost_done;
				break;
			default:
				cur_value += ch;
				++ri_;
				break;
			}
			break;
		
		case sw_dup_value:
			switch (ch)
			{
			case '\r':
			case '\n':
				state = sw_header_almost_done;
				break;
			default:
				++ri_;
				break;
			}
			break;

		case sw_status_code:
			switch (ch)
			{
			case '\r':
			case '\n':
				if (!isValidStatusCode(cur_value))
				{
					state = sw_error;
					break;
				}
				state = sw_header_almost_done;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				cur_value += ch;
				++ri_;
				break;
			default:
				if (!isValidStatusCode(cur_value))
				{
					state = sw_error;
					break;
				}
				state = sw_status_reason_phrase;
				break;
			}
			break;
		
		case sw_status_reason_phrase:
			switch (ch)
			{
			case '\r':
			case '\n':
				state = sw_header_almost_done;
				break;
			default:
				cur_value += ch;
				++ri_;
				break;
			}
			break;

		case sw_cl_value:
			switch (ch)
			{
			case '\r':
			case '\n':
				if (!isValidContentLength(cur_value))
				{
					state = sw_error;
					break;
				}
				state = sw_header_almost_done;
				break;
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				cur_value += ch;
				++ri_;
				break;
			case ' ':
				if (!isValidContentLength(cur_value))
				{
					state = sw_error;
					break;
				}
				state = sw_space_after_value;
				break;
			default:
				state = sw_error;
				break;
			}
			break;

		case sw_space_after_value:
			switch (ch)
			{
			case '\r':
			case '\n':
				state = sw_header_almost_done;
				break;
			case ' ':
				++ri_;
				break;
			default:
				state = sw_error;
				break;
			}
			break;

		case sw_header_almost_done:
			switch (ch)
			{
			case '\r':
				state = sw_header_done;
				++ri_;
				break;
			case '\n':
				state = sw_header_done;
				break;
			default:
				state = sw_error;
				break;
			}
			break;
		
		case sw_header_done:
		{
			if (ch != '\n')
			{
				state = sw_error;
				break;
			}

			const string_map_case_insensitive::const_iterator	it = this->headers_->find(cur_name);
			if (Utils::compareIgnoreCase(cur_name, kContentLength) && it == this->headers_->end() && cur_value.empty())
			{
				state = sw_error;
				break;
			}
			// headerが重複している場合は、一番初めのものが適応される
			if (Utils::compareIgnoreCase(cur_name, kStatus) && it == this->headers_->end())
			{
				setStatusCode(cur_value);
				this->headers_->insert(std::make_pair(cur_name, cur_value));
			}
			else if (!cur_name.empty() && it == this->headers_->end())
				this->headers_->insert(std::make_pair(cur_name, cur_value));
			++ri_;
			state = sw_start;
			break;
		}

		case sw_almost_end:
			if (ch != '\n')
			{
				state = sw_error;
				break;
			}
			++ri_;
			state = sw_end;
			break;

		case sw_end:
		case sw_error:
			break;
		}

		if (state == sw_error || 
			(ri_ >= response.size() && state != sw_end)
		)
		{
			this->state_ = PARSE_ERROR;
			return;
		}
	}

	// Content-Typeが空の場合は、responseのheaderから削除する
	const string_map_case_insensitive::iterator it = this->headers_->find(kContentType);
	if (it != this->headers_->end() && this->headers_->at(kContentType).empty())
		eraseHeader(kContentType);
	finalizeStatusCode();

	this->state_ = PARSE_HEADER_DONE;
}

void	cgi::CGIParser::parseBody(const std::string& response)
{
	// content lengthが設定されている場合は、bodyの長さを調節する
	if (this->headers_->find(kContentLength) != this->headers_->end())
	{
		const std::string& content_length = this->headers_->at(kContentLength);
		std::istringstream	iss(content_length);
		size_t	length;
		iss >> length;

		*(this->body_) = response.substr(ri_, length);
		this->state_ = PARSE_BODY_DONE;
		return;
	}
	*(this->body_) = response.substr(ri_);
	this->state_ = PARSE_BODY_DONE;
}

/**
 * @brief status codeは100以上である必要がある
 * 
 * @param status_code 
 * @return true 
 * @return false 
 */
bool	cgi::CGIParser::isValidStatusCode(const std::string& status_code) const
{
	if (status_code.size() < 3)
		return false;
	std::istringstream	iss(status_code.substr(0, 3));
	int	num;
	iss >> num;
	if (iss.fail())
		return false;
	return num >= 100;
}

void	cgi::CGIParser::setStatusCode(const std::string& value)
{
	std::string	tmp = value;
	// status code以降がspaceのみの場合は、status codeの値のみを保持する
	tmp.erase(std::remove_if(tmp.begin(), tmp.end(), Utils::isSpace), tmp.end());
	// status codeのみの場合
	// ex: 200, 999など
	if (tmp.size() < 4)
	{
		std::istringstream	iss(tmp);
		long status;
		iss >> status;
		*(this->status_code_) = status;
		return;
	}
	*(this->status_code_line_) = value;
}

/**
 * @brief status codeが設定されていない場合は、設定する
 * 
 * headerにlocationが存在する場合：302
 * それ以外：200
 * 
 */
void	cgi::CGIParser::finalizeStatusCode()
{
	// statusのみ違うメンバ変数で管理しているので、削除する
	if (this->headers_->find(kStatus) != this->headers_->end())
		return eraseHeader(kStatus);
	if (this->headers_->find("location") != this->headers_->end())
	{
		*(this->status_code_) = 302;
		return;
	}
	*(this->status_code_) = 200;
}

bool	cgi::CGIParser::isValidContentLength(std::string cl) const
{
	unsigned long	length;
	std::istringstream iss(cl);
	iss >> length;

	if (iss.fail())
		return false;
	return length <= static_cast<unsigned long>(std::numeric_limits<long>::max());
}

void	cgi::CGIParser::eraseHeader(const std::string& header)
{
	const string_map_case_insensitive::iterator it = this->headers_->find(header);
	if (it == this->headers_->end())
		return;
	this->headers_->erase(it);
}
