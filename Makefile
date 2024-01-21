NAME				= webserv
CXX					= c++
CXXFLAGS			= -std=c++98 -Wall -Wextra -Werror
CXXDEBUG			= -fsanitize=address -g
DEPFLAGS			= -MMD -MP -MF $(DEPS_DIR)/$*.d
RM					= rm -rf

SRCS_DIR			= src
INCS_DIR			= $(SRCS_DIR)/config
OBJS_DIR			= obj
DEPS_DIR			= dep
BUILD_DIR			= build
CGI_DIR				= cgi
CONF_DIR			= conf

# SRCS				= $(SRCS_DIR)/main.cpp $(SRCS_DIR)/ConnectionManager.cpp $(SRCS_DIR)/EchoServer.cpp $(SRCS_DIR)/NetworkIOHandler.cpp $(SRCS_DIR)/RequestHandler.cpp $(SRCS_DIR)/ServerConfig.cpp

SRCS = $(wildcard $(SRCS_DIR)/*.cpp)

# SERVER_DIR			= server
# SRCS				+= $(SRCS_DIR)/$(SERVER_DIR)

# CONFIG_DIR			= config
# SRCS				+= $(SRCS_DIR)/$(CONFIG_DIR)

DEPS = $(patsubst $(SRCS_DIR)/%.cpp,$(DEPS_DIR)/%.d,$(SRCS))
OBJS = $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))
INCLUDES			= -I$(INCS_DIR)
CGI_EXEFILE			= $(CGI_DIR)/cgi_exe
# CONFIG				= $(CONF_DIR)/test.conf


all: $(CGI_EXEFILE) $(DEPS_DIR) $(OBJS_DIR) $(NAME)

$(DEPS_DIR):
	@mkdir -p $@
# mkdir -p dep/server dep/config

$(OBJS_DIR):
	@mkdir -p $(dir $@)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(DEPS_DIR)/%.d: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $(INCLUDES) -c $< -o $@

$(CGI_EXEFILE):
	$(MAKE) -C $(CGI_DIR)

clean:
	$(MAKE) clean -C $(CGI_DIR)
	$(RM) $(OBJS_DIR) $(DEPS_DIR) $(BUILD_DIR)

fclean: clean
	$(MAKE) fclean -C $(CGI_DIR)
	$(RM) $(NAME)

re: fclean all

TEST_FILTER ?= '*'

test:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR)
	./$(BUILD_DIR)/webserv-googletest --gtest_filter=$(TEST_FILTER)

-include $(DEPS)

# run:
# 	./$(NAME) $(CONFIG)

# dev: CXXFLAGS += $(CXXDEBUG) all

# valgrind:
# 	valgrind --leak-check=full ./$(NAME)

.PHONY: all clean fclean re test
