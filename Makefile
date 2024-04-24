NAME				= webserv
CXX				= c++
CXXFLAGS			= -std=c++98 -Wall -Wextra -Werror
CXXDEBUG			= -fsanitize=address -g
DEPFLAGS			= -MMD -MP -MF $(DEPS_DIR)/$*.d
RM				= rm -rf

SRCS_DIR			= src
OBJS_DIR			= obj
DEPS_DIR			= dep
BUILD_DIR			= build
CONF_DIR			= conf
TEST_DIR			= test
TEST_CGI_DIR	= $(TEST_DIR)/cgi/cgi_files/executor

# ソースファイルの拡張子
SRC_EXT = cpp
# ソースファイルの検索パス
VPATH = $(SRCS_DIR) $(SRCS_DIR)/config $(SRCS_DIR)/server $(SRCS_DIR)/utils

# ソースファイルの取得
SRCS = $(wildcard $(addsuffix /*.$(SRC_EXT), $(VPATH)))

DEPS = $(patsubst $(SRCS_DIR)/%.cpp,$(DEPS_DIR)/%.d,$(SRCS))
OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))
INCLUDES			= -I$(SRCS_DIR) -I$(SRCS_DIR)/config/ -I$(SRCS_DIR)/server/ -I$(SRCS_DIR)/utils
# CONFIG				= $(CONF_DIR)/test.conf


all: $(DEPS_DIR) $(OBJS_DIR) $(NAME)

$(DEPS_DIR):
	@mkdir -p $@
	@mkdir -p dep/config dep/config/parser dep/server dep/utils

$(OBJS_DIR):
	@mkdir -p $(dir $@)

$(NAME): $(OBJS)
	@mkdir -p logs/
	$(CXX) $(CXXFLAGS) -o $@ $^

$(DEPS_DIR)/%.d: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS_DIR) $(DEPS_DIR) $(BUILD_DIR)

fclean: clean
	$(MAKE) fclean -C $(TEST_CGI_DIR)
	$(RM) $(NAME)

re: fclean all

TEST_FILTER ?= '*'

test:
	$(MAKE) -C $(TEST_CGI_DIR)
	@mkdir -p logs/
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	./$(BUILD_DIR)/webserv-googletest --gtest_filter=$(TEST_FILTER)

format:
	find $(SRCS_DIR) $(TEST_DIR) -name "*.cpp" -o -name "*.hpp" -o -name "*.c" | xargs clang-format -i
	black $(TEST_DIR)
	shfmt -w -l -i 2 $(TEST_DIR)

-include $(DEPS)

# run:
# 	./$(NAME) $(CONFIG)

# dev: CXXFLAGS += $(CXXDEBUG) all

# valgrind:
# 	valgrind --leak-check=full ./$(NAME)

.PHONY: all clean fclean re test
