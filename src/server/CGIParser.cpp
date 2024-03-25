#include "CGIParser.hpp"

#include <cctype>
#include <utility>
#include <functional>
#include <sstream>
#include <limits>

cgi::CGIParser::CGIParser() :
	headers_(NULL), body_(NULL), status_code_(NULL), status_code_line_(NULL), cri_(0) {}

cgi::CGIParser::~CGIParser() {}

void	cgi::CGIParser::init(HttpResponse& http_response)
{
	this->headers_ = &(http_response.headers_);
	this->body_ = &(http_response.body_);
	this->status_code_ = &(http_response.status_code_);
	this->status_code_line_ = &(http_response.cgi_status_code_line_);
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
		sw_nl,
		sw_value,
		sw_space_before_value,
		sw_status_code,
		sw_status_reason_phrase,
		sw_cl_value, // content-length
		sw_dup_value,
		sw_space_after_value,
		sw_header_almost_done,
		sw_header_done,
		sw_end
	};

	if (response.size() == 0)
	{
		this->state_ = PARSE_ERROR;
		return;
	}

	const static char	*kContentLength = "content-length";
	const static char	*kStatus = "status";
	const static char	*kContentType = "content-type";
	cri_ = 0;
	PARSE_HEADER_PHASE	state = sw_start;
	PARSE_HEADER_PHASE	next_state; // stateがsw_nl用の変数: \nの後にsetするstateを格納する
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
				next_state = sw_end;
				state = sw_nl;
				++cri_;
				break;
			case '\n':
				++cri_;
				state = sw_end;
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
				next_state = sw_end;
				state = sw_nl;
				++cri_;
				break;
			case '\n':
				state = sw_end;
				++cri_;
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
			const std::string	name_lowercase = Utils::toLower(cur_name);
			if (name_lowercase == kStatus)
			{
				state = sw_space_before_value;
				next_state = sw_status_code;
				break;
			}
			if (name_lowercase == kContentLength)
			{
				state = sw_space_before_value;
				next_state = sw_cl_value;
				break;
			}

			state = sw_value;
			break;
		}

		case sw_space_before_value:
			switch (ch)
			{
			case ' ':
				break;
			default:
				state = next_state;
				break;
			}
			++cri_;
			break;

		case sw_value:
			switch (ch)
			{
			case '\r':
				++cri_;
				next_state = sw_nl;
				state = sw_start;
				break;
			case '\n':
				state = sw_header_almost_done;
				break;
			default:
				if (std::isspace(ch))
				{
					++cri_;
					break;
				}
				cur_value += ch;
				break;
			}
		
		case sw_dup_value:
			switch (ch)
			{
			case '\r':
				next_state = sw_start;
				state = sw_nl;
				break;
			case '\n':
				state = sw_start;
				break;
			default:
				break;
			}
			++cri_;
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
			if (ch != '\n')
			{
				state = sw_error;
				break;
			}
			// statusがsetされていない場合
			if (cur_name == kStatus && (*(this->status_code_) == 0 && *(this->status_code_line_) == ""))
			{
				setStatusCode(cur_value);
				*(this->status_code_line_) = cur_value;
			}
			else if (!cur_name.empty())
				this->headers_->insert(std::make_pair(cur_name, cur_value));
			++cri_;
			state = sw_start;
			break;

		case sw_nl:
			if (ch != '\n')
			{
				state = sw_error;
				break;
			}
			state = next_state;
			++cri_;
			break;

		case sw_end:
			break;

		default:
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

	// Content-Typeが空の場合は、responseのheaderに追加しない
	const string_map_case_insensitive::iterator it = this->headers_->find(kContentType);
	if (it != this->headers_->end() && this->headers_->at(kContentType) == "")
		this->headers_->erase(it);
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
	// status codeのみの場合
	// ex: 200, 999など
	if (i == value.size() && i < 4)
	{
		std::istringstream	iss(value);
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
	return length > static_cast<unsigned long>(std::numeric_limits<long>::max());
}
