#include <gtest/gtest.h>
#include <gtest/gtest-param-test.h>

#include "TimerTree.hpp"
#include "Timer.hpp"

#include <vector>
#include <limits> 
#include <sys/time.h>

// timer testで許容する範囲: 3ms
const int kacceptable_timer_gap = 3;

namespace test
{
	void	testFdSet(const std::set<int> &fd_set, const int fd, const bool expect_found)
	{
		if (!expect_found) {
			EXPECT_TRUE(fd_set.find(fd) == fd_set.end());
			return;
		}
		EXPECT_TRUE(fd_set.find(fd) != fd_set.end());
	}

	void	testFdsSet(const std::set<int> &fd_set, const std::vector<int> &fds, const std::vector<bool> &expect_founds)
	{
		size_t	i = 0;
		for (std::vector<int>::const_iterator it = fds.begin(); it != fds.end(); ++it)
		{
			testFdSet(fd_set, *it, expect_founds[i]);
			++i;
		}
	}

	void	testValue(const TimerTree &timer_tree, const std::vector<unsigned long> &expect)
	{
		const std::multiset<Timer> &timers = timer_tree.getTimerTree();
		const std::set<int> &fd_set = timer_tree.getFdSet();
		size_t	i = 0;
		for (std::multiset<Timer>::const_iterator it = timers.begin();
			it != timers.end();
			++it
		)
		{
			// timerの誤差(3ms)は許容する
			const long	gap = (expect[i] + Timer::getCurrentTime()) - it->getTimeout();
			EXPECT_TRUE(gap <= kacceptable_timer_gap && gap >= -kacceptable_timer_gap);
			EXPECT_EQ(expect[i] + Timer::getCurrentTime(), it->getTimeout());
			testFdSet(fd_set, it->getFd(), true);
			++i;
		}
	}

	void	initTimerTree(TimerTree &timer_tree, const std::vector<unsigned long> &timers)
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
	
	void	deleteTimers(TimerTree &timer_tree, const std::vector<int> &fds)
	{
		for (std::vector<int>::const_iterator it = fds.begin();
			it != fds.end();
			++it
		)
		{
			timer_tree.deleteTimer(*it);
		}
	}

	void	expectTimer(const int actual, const int expect)
	{
		// timerの誤差(3ms)は許容する
		const long	gap = expect - actual;
		EXPECT_TRUE(gap <= kacceptable_timer_gap && gap >= -kacceptable_timer_gap);
		EXPECT_EQ(expect, actual);
	}
} // namespace test


TEST(timertree, addtimer)
{
	unsigned long	now = Timer::getCurrentTime();
	TimerTree	timer_tree;
	unsigned long	longmax = std::numeric_limits<long>::max();

	test::initTimerTree(timer_tree ,{
		(now + longmax),
		longmax,
		now,
		100,
		1,
		3,
		3,
		2,
		0,
		(longmax - now)
	});
	
	test::testValue(timer_tree, {
		0,
		1,
		2,
		3,
		3,
		100,
		now,
		(longmax - now),
		(longmax),
		(now + longmax)
	});
}

TEST(timertree, deletetimer)
{
	TimerTree	timer_tree;

	test::initTimerTree(timer_tree, {
		1, // fd 0
		2, // fd 1
		3, // fd 2
		4, // fd 3
		5, // fd 4
		6, // fd 5
		7, // fd 6
		8, // fd 7
		9, // fd 8
		10, // fd 9
		2, // fd 10
		3, // fd 11
		4, // fd 12
		5 // fd 13
	});

	test::deleteTimers(timer_tree, {0, 2, 3, 9, 12, 13});

	test::testValue(timer_tree, {
		2, // fd 1
		2, // fd 10
		3, // fd 11
		5, // fd 4
		6, // fd 5
		7, // fd 6
		8, // fd 7
		9, // fd 8
	});

	test::testFdsSet(timer_tree.getFdSet(),
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13},
		{false, true, false, false, true, true, true, true, true, false, true, true, false, false}
	);
}

TEST(timertree, findTimer_notimer)
{
	TimerTree	timer_tree;
	unsigned long	now = Timer::getCurrentTime();

	// timerが見つからない
	EXPECT_EQ(timer_tree.findTimer(), -1);

	test::initTimerTree(timer_tree, {now + 1, now + 100});
	test::deleteTimers(timer_tree, {0, 1});

	EXPECT_EQ(timer_tree.findTimer(), -1);
	test::testFdsSet(timer_tree.getFdSet(), {0, 1}, {false, false});
}

TEST(timertree, findTimer)
{
	TimerTree	timer_tree;
	unsigned long	longmax = std::numeric_limits<long>::max();

	test::initTimerTree(timer_tree, {
		3000, // fd 0
		4000, // fd 1
		1000, // fd 2
		longmax, // fd 3
	});

	test::expectTimer(timer_tree.findTimer(), 1000);

	timer_tree.deleteTimer(2);
	test::expectTimer(timer_tree.findTimer(), 3000);

	timer_tree.deleteTimer(0);
	test::expectTimer(timer_tree.findTimer(), 4000);

	timer_tree.deleteTimer(1);
	test::expectTimer(timer_tree.findTimer(), std::numeric_limits<int>::max());
}
