#include "CGIParser.hpp"

#include <cctype>
#include <utility>
#include <functional>
#include <sstream>
#include <limits>
#include <algorithm>

const static char	*kContentLength = "content-length";

cgi::CGIParser::CGIParser() :
	headers_(NULL), body_(NULL), status_code_(NULL), status_code_line_(NULL), cri_(0) {}

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

	const static char	*kStatus = "status";
	const static char	*kContentType = "content-type";
	cri_ = 0;
	PARSE_HEADER_PHASE	state = sw_start;
	std::string	cur_name;
	std::string	cur_value;
	while (state != sw_end)
	{
		const unsigned char	ch = response[cri_];
		switch (state)
		{
		case sw_start:
			cur_name.clear();
			cur_value.clear();
			switch (ch)
			{
			case '\r':
				state = sw_almost_end;
				++cri_;
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
				++cri_;
				break;
			}
			break;

		case sw_colon:
		{
			++cri_;
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
		{
			if (ch == ' ')
			{
				++cri_;
				break;
			}
			const std::string	name_lowercase = Utils::toLower(cur_name);
			if (name_lowercase == kStatus)
			{
				state = sw_status_code;
				break;
			}
			if (name_lowercase == kContentLength)
			{
				state = sw_cl_value;
				break;
			}
			state = sw_value;
			break;
		}

		case sw_value:
			switch (ch)
			{
			case '\r':
			case '\n':
				state = sw_header_almost_done;
				break;
			default:
				cur_value += ch;
				++cri_;
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
				++cri_;
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
				++cri_;
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
				++cri_;
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
				++cri_;
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
				++cri_;
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
				++cri_;
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
			if (cur_name == kContentLength && it == this->headers_->end() && cur_value.empty())
			{
				state = sw_error;
				break;
			}
			// headerが重複している場合は、一番初めのものが適応される
			if (cur_name == kStatus && it == this->headers_->end())
			{
				setStatusCode(cur_value);
				this->headers_->insert(std::make_pair(cur_name, cur_value));
			}
			else if (!cur_name.empty() && it == this->headers_->end())
				this->headers_->insert(std::make_pair(cur_name, cur_value));
			++cri_;
			state = sw_start;
			break;
		}

		case sw_almost_end:
			if (ch != '\n')
			{
				state = sw_error;
				break;
			}
			++cri_;
			state = sw_end;
			break;

		case sw_end:
		case sw_error:
			break;
		}

		if (state == sw_error || 
			(cri_ >= response.size() && (state != sw_end && state != sw_start))
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
	// statusはheaderから削除する
	eraseHeader(kStatus);

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

		*(this->body_) = response.substr(cri_, length);
		this->state_ = PARSE_BODY_DONE;
		return;
	}
	*(this->body_) = response.substr(cri_);
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
	// status code <= 100
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
	size_t	i = 0;
	while (i < value.size())
	{
		const unsigned char	ch = value[i];
		if (!std::isdigit(ch))
			break;
		i++;
	}
	std::string	tmp = value;
	// status code移行がspaceのみの場合は、status codeの値のみを保持する
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
