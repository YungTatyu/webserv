NAME				= webserv
CXX				= c++
CXXFLAGS			= -std=c++98 -Wall -Wextra -Werror -Wpedantic
DEPFLAGS			= -MMD -MP -MF $(DEPS_DIR)/$*.d
RM				= rm -rf

SRCS_DIR			= src
OBJS_DIR			= obj
DEPS_DIR			= dep
ROOT_DIR			= $(shell pwd)
BUILD_DIR			= build
CONF_DIR			= conf
LOG_DIR				= logs
LOG_FILE			= $(LOG_DIR)/error.log $(LOG_DIR)/access.log
TEST_DIR			= test
TEST_CGI_DIR	= $(TEST_DIR)/cgi/cgi_files/executor

DOCKERFILE_PTEST	= Dockerfile.ptest
PTEST_IMG_NAME	= test-image
DOCKERFILE_FORMATTER	= Dockerfile.formatter
FORMATTER_IMG_NAME	= formatter-image

# ソースファイルの拡張子
SRC_EXT = cpp
# ソースファイルの検索パス
VPATH = $(SRCS_DIR) $(SRCS_DIR)/config $(SRCS_DIR)/server $(SRCS_DIR)/http $(SRCS_DIR)/cgi $(SRCS_DIR)/utils

# ソースファイルの取得
SRCS = $(wildcard $(addsuffix /*.$(SRC_EXT), $(VPATH)))
DEPS = $(patsubst $(SRCS_DIR)/%.cpp,$(DEPS_DIR)/%.d,$(SRCS))
OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))
INCLUDES = -I$(SRCS_DIR) -I$(SRCS_DIR)/config -I$(SRCS_DIR)/server -I$(SRCS_DIR)/http -I$(SRCS_DIR)/cgi -I$(SRCS_DIR)/utils


all: $(DEPS_DIR) $(OBJS_DIR) $(NAME)

$(DEPS_DIR):
	@mkdir -p $@
	@mkdir -p dep/config dep/server dep/http dep/cgi dep/utils

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
	$(RM) $(NAME) $(LOG_FILE)

re: fclean all

ptest:
	docker build -t $(PTEST_IMG_NAME) . -f $(DOCKERFILE_PTEST)
	docker run --rm $(PTEST_IMG_NAME)

TEST_FILTER ?= '*'

gtest:
	$(MAKE) -C $(TEST_CGI_DIR)
	@mkdir -p logs/
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	$(BUILD_DIR)/webserv-googletest --gtest_filter=$(TEST_FILTER)

test: gtest ptest

format:
	docker build -t $(FORMATTER_IMG_NAME) . -f $(DOCKERFILE_FORMATTER)
	docker run --rm -v "$(ROOT_DIR):$(ROOT_DIR)" -w "$(ROOT_DIR)" $(FORMATTER_IMG_NAME)

debug: CXXFLAGS += -fsanitize=address -g 
debug: all

-include $(DEPS)


.PHONY: all clean fclean re test ptest gtest format debug
