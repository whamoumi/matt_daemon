CXXFLAGS	=	-Wall -Wextra -Werror -std=c++17 
NAME		=	MattDaemon
SRC		=	$(wildcard ./*.cpp)

OBJ		=	$(SRC:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) $(OBJ) -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
