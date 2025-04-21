NAME = minishell

CC = cc

CFLAGS = -Wall -Wextra -Werror

MY_SOURCES = minishell.c

MY_OBJECTS = $(MY_SOURCES:.c=.o)

all: $(NAME)

$(NAME): $(MY_OBJECTS)
		cd ft_printf && $(MAKE)
		$(CC) $(CFLAGS) $(MY_OBJECTS) ft_printf/libftprintf.a -o $(NAME)

clean:
		cd ft_printf && $(MAKE) clean
		rm -f $(MY_OBJECTS)

fclean:
		cd ft_printf && $(MAKE) fclean
		rm -f $(NAME) $(MY_OBJECTS)

re:		fclean $(NAME)

.PHONY: all clean fclean re