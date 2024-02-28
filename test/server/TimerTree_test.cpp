#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "TimerTree.hpp"
#include "Time.hpp"

#include <vector>
#include <limits> 
#include <sys/time.h>

namespace test
{
	unsigned long	getCurrentTime()
	{
		struct timeval	t;

		gettimeofday(&t, NULL);
		return (t.tv_sec * 1000) + (t.tv_usec / 1000);
	}
	void	test_value(const std::multiset<Timer>	&timer_tree, const std::vector<unsigned long> &expect)
	{
		size_t	i = 0;
		for (std::multiset<Timer>::const_iterator it = timer_tree.begin();
			it != timer_tree.end();
			++it
		)
		{
			EXPECT_EQ(it->getTimeout(), expect[i]);
			++i;
		}
	}
	void	initTimerTree(TimerTree	&timer_tree, const std::vector<unsigned long> &timers)
	{
		size_t	i = 0;
		for (std::vector<unsigned long>::const_iterator it = timers.begin();
			it != timers.end();
			++it
		)
		{
			timer_tree.addTimer(Timer(i, config::Time(*it)));
			i++;
		}
	}
} // namespace test


TEST(timertree, add)
{
	unsigned long	now = test::getCurrentTime();
	TimerTree	timer_tree;
	unsigned long	longmax = std::numeric_limits<long>::max();

	test::initTimerTree(timer_tree ,{
		(now + longmax),
		longmax,
		now,
		100,
		1,
		2,
		0,
		(longmax - now)
	});
	
	test::test_value(timer_tree.getTimerTree(), {
		0,
		1,
		2,
		100,
		now,
		(longmax - now),
		(longmax),
		(now + longmax)
	});
}