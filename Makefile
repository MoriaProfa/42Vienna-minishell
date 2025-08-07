NAME = minishell

CC = cc

CFLAGS = -Wall -Wextra -Werror -MMD -MP -g

MY_SOURCES = minishell.c builtins.c

MY_OBJECTS = $(MY_SOURCES:.c=.o)

MY_DEPENDENCIES = $(MY_SOURCES:.c=.d)

all: $(NAME)
-include $(MY_DEPENDENCIES)

$(NAME): $(MY_OBJECTS)
		cd libft && $(MAKE) bonus
		$(CC) $(CFLAGS) $(MY_OBJECTS) libft/libft.a -o $(NAME) -lreadline

clean:
		cd libft && $(MAKE) clean
		rm -f $(MY_OBJECTS) $(MY_DEPENDENCIES)

fclean: clean
		cd libft && $(MAKE) fclean
		rm -f $(NAME)

re:		fclean $(NAME)

.PHONY: all clean fclean re
