#include "CGIParser.hpp"

#include <cctype>
#include <utility>
#include <functional>

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
			++cri_;
			state = sw_value;
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
			if (!cur_name.empty())
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
	const static char	*kContentType = "Content-Type";
	const string_map_case_insensitive::iterator it = this->headers_->find(kContentType);
	if (it != this->headers_->end() && this->headers_->at(kContentType) == "")
		this->headers_->erase(it);
}