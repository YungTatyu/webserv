#include <gtest/gtest.h>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

#include "Main.hpp"
#include "conf.hpp"
#include "directives_test.hpp"

const std::string kUse = "use";

TEST(UseTest, select) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/use/1.conf");
  ASSERT_NE(config, nullptr);

  const config::Events &events = config->events_;

  EXPECT_EQ(events.use_.getConnectionMethod(), config::CONNECTION_METHOD::SELECT);
  test::test_directives_set(events.directives_set_, kUse, true);
}

TEST(UseTest, poll) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/use/2.conf");
  ASSERT_NE(config, nullptr);

  const config::Events &events = config->events_;

  EXPECT_EQ(events.use_.getConnectionMethod(), config::CONNECTION_METHOD::POLL);
  test::test_directives_set(events.directives_set_, kUse, true);
}

TEST(UseTest, kqueue) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/use/3.conf");

// 環境によって挙動が変わる
#if defined(KQUEUE_AVAILABLE)
  ASSERT_NE(config, nullptr);
#else
  ASSERT_EQ(config, nullptr);
  return;
#endif

  const config::Events &events = config->events_;

  EXPECT_EQ(events.use_.getConnectionMethod(), config::CONNECTION_METHOD::KQUEUE);
  test::test_directives_set(events.directives_set_, kUse, true);
}

TEST(UseTest, epoll) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/use/4.conf");

// 環境によって挙動が変わる
#if defined(__linux__)
  ASSERT_NE(config, nullptr);
#else
  ASSERT_EQ(config, nullptr);
  return;
#endif

  const config::Events &events = config->events_;

  EXPECT_EQ(events.use_.getConnectionMethod(), config::CONNECTION_METHOD::EPOLL);
  test::test_directives_set(events.directives_set_, kUse, true);
}

TEST(UseTest, notFound) {
  const config::Main *config = config::initConfig("test/conf/directive_parser/only_context.conf");
  ASSERT_NE(config, nullptr);

  const config::Events &events = config->events_;

  test::test_directives_set(events.directives_set_, kUse, false);
}
