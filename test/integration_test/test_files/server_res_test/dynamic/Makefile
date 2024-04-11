NAME_CRRD			= client_redirect_res_doc.cgi
NAME_CRR			= client_redirect_res.cgi
CC					= gcc
CXX					= c++
CXXFLAGS			= -std=c++14
RM					= rm -rf

all: $(NAME_CRRD) $(NAME_CRR)

$(NAME_CRRD):
	$(CXX) $(CXXFLAGS) -o $@ client_redirect_res_doc.cpp

$(NAME_CRR):
	$(CC) -o $@ client_redirect_res.c

fclean: clean
	$(RM) $(NAME_CRRD) $(NAME_CRR)

re: fclean all

.PHONY: all clean fclean re test
