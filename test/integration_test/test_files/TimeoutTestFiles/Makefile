NAME_TIMEOUT0			= timeout0
NAME_TIMEOUT5			= timeout5
NAME_TIMEOUT10			= timeout10
CC					= gcc
CXX					= c++
CXXFLAGS			= -std=c++14
RM					= rm -rf

all: $(NAME_TIMEOUT0) $(NAME_TIMEOUT5) $(NAME_TIMEOUT10)

$(NAME_TIMEOUT0):
	$(CXX) $(CXXFLAGS) -o $@ client_timeout0.cpp

$(NAME_TIMEOUT5):
	$(CXX) $(CXXFLAGS) -o $@ client_timeout5.cpp

$(NAME_TIMEOUT10):
	$(CXX) $(CXXFLAGS) -o $@ client_timeout10.cpp

fclean: clean
	$(RM) $(NAME_TIMEOUT0) $(NAME_TIMEOUT5) $(NAME_TIMEOUT10)

re: fclean all

.PHONY: all clean fclean re test
