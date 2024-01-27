NAME				= cgi_exe
CXX					= c++
CXXFLAGS			= -std=c++98 -Wall -Wextra -Werror
RM					= rm -rf

SRCS_DIR			= ./src
OBJS_DIR			= ./obj
INCS_DIR			= ./include

SRCS				= $(SRCS_DIR)/cgi.cpp
OBJS				= $(patsubst $(SRCS_DIR)/%.cpp,$(OBJS_DIR)/%.o,$(SRCS))
INCLUDES			= -I$(INCS_DIR)


all: $(OBJS_DIR) $(NAME)	

$(OBJS_DIR):
	@mkdir -p $@

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJS_DIR)/%.o: $(SRCS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
