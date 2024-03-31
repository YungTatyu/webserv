NAME_BODY			= body.cgi
NAME_CRRD			= client_redirect_res_doc.cgi
NAME_CRR			= client_redirect_res.cgi
CC					= gcc
CXX					= c++
CXXFLAGS			= -std=c++14
RM					= rm -rf

all: $(NAME_BODY) $(NAME_CRRD) $(NAME_CRR)

$(NAME_BODY):
	$(CXX) $(CXXFLAGS) -o $@ body.cpp

$(NAME_CRRD):
	$(CXX) $(CXXFLAGS) -o $@ client_redirect_res_doc.cpp

$(NAME_CRR):
	$(CC) -o $@ client_redirect_res.c

fclean: clean
	$(RM) $(NAME_BODY) $(NAME_CRRD) $(NAME_CRR)

re: fclean all

.PHONY: all clean fclean re test
