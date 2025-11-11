NAME		=	npuzzle

SRCDIR		=	src
INCDIR		=	includes
BINDIR		=	bin

SRCS		=	$(wildcard $(SRCDIR)/*.cpp)
OBJS		=	$(patsubst $(SRCDIR)/%.cpp,$(BINDIR)/%.o,$(SRCS))

CXX			=	c++
CXXFLAGS	=	-O3 -march=native -flto -DNDEBUG -Wall -Wextra -Werror -I$(INCDIR)
RM			=	rm -rf

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CXX) $(CXXFLAGS) $^ -o $@

$(BINDIR)/%.o:	$(SRCDIR)/%.cpp | $(BINDIR)
				$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

$(BINDIR):
				mkdir -p $(BINDIR)

clean:
				$(RM) $(BINDIR)/*.d
				$(RM) $(OBJS)

fclean:			clean
				$(RM) $(BINDIR)
				$(RM) $(NAME)

re:				fclean all

.PHONY:			all re clean fclean

-include $(BINDIR)/*.d
