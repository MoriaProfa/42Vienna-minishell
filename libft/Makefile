NAME = libft.a

CC = cc

CFLAGS = -Wall -Wextra -Werror -MMD -MP

MY_SOURCES = ft_atoi.c \
				ft_bzero.c \
				ft_calloc.c \
				ft_isalnum.c \
				ft_isalpha.c \
				ft_isascii.c \
				ft_isdigit.c \
				ft_isprint.c \
				ft_memchr.c \
				ft_memcmp.c \
				ft_memcpy.c \
				ft_memmove.c \
				ft_memset.c \
				ft_strchr.c \
				ft_strdup.c \
				ft_strlcat.c \
				ft_strlcpy.c \
				ft_strlen.c \
				ft_strncmp.c \
				ft_strnstr.c \
				ft_strrchr.c \
				ft_tolower.c \
				ft_toupper.c \
				ft_substr.c \
				ft_strjoin.c \
				ft_strtrim.c \
				ft_itoa.c \
				ft_strmapi.c \
				ft_striteri.c \
				ft_putchar_fd.c \
				ft_putstr_fd.c \
				ft_putendl_fd.c \
				ft_putnbr_fd.c \
				ft_split.c \
				advanced_libft/ft_array_free.c \
				advanced_libft/ft_array_copy.c \
				advanced_libft/ft_array_size.c \
				advanced_libft/ft_atol.c \
				advanced_libft/ft_strcpy.c \
				advanced_libft/ft_strjoin_with_space.c \
				advanced_libft/ft_free.c

BONUS_SOURCES = bonus/ft_lstadd_back.c \
				bonus/ft_lstadd_front.c \
				bonus/ft_lstclear.c \
				bonus/ft_lstdelone.c \
				bonus/ft_lstiter.c \
				bonus/ft_lstlast.c \
				bonus/ft_lstmap.c \
				bonus/ft_lstnew.c \
				bonus/ft_lstsize.c

MY_OBJECTS = $(MY_SOURCES:.c=.o)
BONUS_OBJECTS = $(BONUS_SOURCES:.c=.o)

MY_DEPENDENCIES = $(MY_SOURCES:.c=.d)
BONUS_DEPENDENCIES = $(BONUS_SOURCES:.c=.d)

all: $(NAME)
-include $(MY_DEPENDENCIES) $(BONUS_DEPENDENCIES)

$(NAME): $(MY_OBJECTS)
	ar rcs $(NAME) $(MY_OBJECTS)

bonus: $(MY_OBJECTS) $(BONUS_OBJECTS)
	ar rcs $(NAME) $(MY_OBJECTS) $(BONUS_OBJECTS)

clean:
	rm -f $(MY_OBJECTS) $(BONUS_OBJECTS) $(MY_DEPENDENCIES) $(BONUS_DEPENDENCIES)

fclean: clean
	rm -f $(NAME)

re:	fclean all

.PHONY: all clean fclean re bonus