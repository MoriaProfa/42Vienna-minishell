NAME = minishell

CC = cc

CFLAGS = -Wall -Wextra -Werror -MMD -MP -g

MY_SOURCES = minishell.c \
			src/builtins/builtins_cd.c \
			src/builtins/builtins_export.c \
			src/builtins/builtins_helpers_atol.c \
			src/builtins/builtins_helpers_checks.c \
			src/builtins/builtins_helpers_env.c \
			src/builtins/builtins_helpers_errorprints.c \
			src/builtins/builtins_helpers_prints.c \
			src/builtins/builtins.c \
			src/command_table/cmd_capacity.c \
			src/command_table/cmd_free.c \
			src/command_table/cmd_parser.c \
			src/command_table/cmd_pathname_builder.c \
			src/command_table/cmd_table.c \
			src/environment/env_var.c \
			src/environment/envp.c \
			src/execution/execution_utils.c \
			src/execution/execution.c \
			src/heredoc/heredoc_collector.c \
			src/heredoc/heredoc_redirect.c \
			src/redirection/redirect_infile.c \
			src/redirection/redirect_outfile.c \
			src/redirection/redirection.c \
			src/tokenizer/expansion.c \
			src/tokenizer/lexing.c \
			src/tokenizer/tokenizer.c \
			src/tokenizer/var_split.c			

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
