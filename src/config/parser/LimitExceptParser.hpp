#ifndef CONFIG_LIMIT_EXCEPT_PARSER_HPP
#define CONFIG_LIMIT_EXCEPT_PARSER_HPP

namespace config
{
class LimitExceptParser
{
	private:
		LimitExceptParser();
	public:
		~LimitExceptParser();
		static unsigned int	num_; // event contextの出現回数
		const static bool	is_unique_ = true; // unique（重複不可）なcontextか
};
} // namespace config

#endif
