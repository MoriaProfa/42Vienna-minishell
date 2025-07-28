NAME = minishell

CC = cc

CFLAGS = -Wall -Wextra -Werror -lreadline -g

MY_SOURCES = minishell.c

MY_OBJECTS = $(MY_SOURCES:.c=.o)

all: $(NAME)

$(NAME): $(MY_OBJECTS)
		cd libft && $(MAKE) bonus
		$(CC) $(CFLAGS) $(MY_OBJECTS) libft/libft.a -o $(NAME)

clean:
		cd libft && $(MAKE) clean
		rm -f $(MY_OBJECTS)

fclean:
		cd libft && $(MAKE) fclean
		rm -f $(NAME) $(MY_OBJECTS)

re:		fclean $(NAME)

.PHONY: all clean fclean re
