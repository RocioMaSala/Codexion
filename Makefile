NAME = codexion
CC = cc
CFLAGS = -Wall -Wextra -Werror -pthread
SRC = cleanup.c coder_routine.c codexion.c monitor.c parsing.c
OBJ = $(SRC:.c=.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJ)

run: all
	$(MAKE) clean

%.o: %.c
	$(CC) $(CFLAGS) -I./coders -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re