NAME = libftprintf.a

CC = cc

CFLAGS = -Wall -Wextra -Werror

MY_SOURCES = ft_printf.c print_ptr.c print_hex.c print_uint.c

MY_OBJECTS = $(MY_SOURCES:.c=.o)

all: $(NAME)

$(NAME): $(MY_OBJECTS)
	cd ../libft && $(MAKE)
	cp ../libft/libft.a .
	mv libft.a $(NAME)
	ar -rcs $(NAME) $(MY_OBJECTS)

clean:
	cd ../libft && $(MAKE) clean
	rm -f $(MY_OBJECTS)

fclean: 
	cd ../libft && $(MAKE) fclean
	rm -f $(NAME) $(MY_OBJECTS)

re:	fclean $(NAME)

.PHONY: all clean fclean re