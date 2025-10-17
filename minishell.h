/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:55:34 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/17 22:24:17 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft/libft.h" 
//<stdint.h>
//<stdlib.h> (malloc, free, getenv)
//<unistd.h> (write, access, read, close, fork, exit, getcwd, chdir, stat, 
//lstat, fstat, unlink, execve, dup, dup2, pipe, isatty, ttyname, ttyslot, 
//tcsetattr, tcgetattr)
# include <errno.h>
# include <fcntl.h>
# include <sys/ioctl.h>
# include <limits.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <signal.h>
# include <stdbool.h>
# include <stdio.h>
# include <sys/stat.h>
//stat, lstat, fstat
# include <sys/types.h>
//fork, wait, waitpid, wait3, wait4, kill, stat, lstat, fstat, opendir, closedir
# include <sys/wait.h>
//wait, waitpid, wait3, wait4

extern volatile sig_atomic_t   g_signal_flag;

typedef enum e_quote_type
{
	Q_NONE,
	Q_SINGLE,
	Q_DOUBLE,
	Q_MIXED
}	t_quote_type;

typedef enum e_token_type
{
	T_NONE,
	T_WORD,
	T_PIPE,
	T_IN_REDIRECT,
	T_OUT_REDIRECT,
	T_APPEND_REDIRECT,
	T_HEREDOC_REDIRECT //maybe renaming all "redirect" to "redir" to shorten line lengths
}	t_token_type;

typedef enum e_builtin_type
{
	B_NOT_BUILTIN,
	B_PWD,
	B_CD,
	B_EXIT,
	B_ENV,
	B_EXPORT,
	B_UNSET,
	B_ECHO,
}	t_builtin_type;

typedef struct s_env_var
{
	char	*name;
	char	*value;
}			t_env_var;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
	t_quote_type	quoted;
	bool			is_adjacent;
	bool			is_expanded;
}					t_token;

typedef struct s_builtin
{
	bool			builtin;
	t_builtin_type	type;
}					t_builtin;

typedef struct s_redir
{
	char			*filename;
	t_token_type	type;
}					t_redir;

typedef struct s_heredoc
{
	char	*delimiter;
	char	*buffer;
	char	*filename;
	bool	expand;
}			t_heredoc;

typedef struct s_cmd
{
	char			**args;
	int				n_args;
	int				capacity_args;
	t_builtin		builtin;
	t_redir			*redirs;
	int				n_redirs;
	int				capacity_redirs;
	t_heredoc		*heredocs;
	int				heredoc_read;
	int				n_hd;
	int				capacity_hd;
	t_token_type	last_input;
	char			*pathname;
}					t_cmd;

typedef struct s_msh
{
	bool			error_check;
	char			*in_str;
	char			*cpy_in_str;
	t_list			*env_vars;
	char			**envp;
	t_list			*tokens;
	t_cmd			**cmd_table;
	int				n_cmds;
	int				status;
	pid_t			*pid;
	int				fd[2];
	int				fd_pipe_read;
	int				fd_in;
	int				fd_out;
	int				saved_stdout;
	int				saved_stdin;
	size_t			line_number;
}					t_msh;

void	add_to_env_vars(t_msh *msh, char *name, char *value);
void	build_cmd_table(t_msh *msh);
void	build_envp_array(t_msh *msh);
void	check_quotationmarks(t_msh *msh);
void	check_tokens(t_msh *msh);
int		collect_all_heredocs(t_msh *msh, int i);
bool	compare_strings(char *s1, char *s2);
void	expand_word_tokens(t_list *tokens, char *tmp, t_msh *msh);
char	*extract_env_name(char *str, size_t *name_len);
char	*extract_env_value(char *str, size_t name_len);
void    free_cmd_table(t_cmd **cmds, int n);
void	free_env_var(void *content);
void    free_envp(t_msh *msh);
void	free_token(void *content);
void	heredoc_collector(t_msh *msh, t_cmd **cmds);
void	init_env_vars(t_msh *msh, char **envp);
void	msh_execute(t_msh *msh, int i);
void	msh_free(t_msh *msh);
void	msh_pid(t_msh *msh);
void	msh_print(t_msh *msh, char *msg, int status);
__attribute__((noreturn)) void	msh_quit(t_msh *msh, char *msg, int status);
void	msh_tokenizer(t_msh *msh);
void	prepare_name_and_add_to_env_vars(t_msh *msh, char *name, char *m_value);
void	run_command(t_msh *msh, int i);
void    split_expanded_tokens(t_list *tokens, t_msh *msh);
void	heredoc_collector(t_msh *msh, t_cmd **cmds);
bool	compare_strings(char *s1, char *s2);
char	*create_cmd_pathname(t_msh *msh, char *cmd);
void	close_and_exit(t_msh *msh, char *msg, int status);


// minishell.c

// src/builtins/builtins_cd.c
void	msh_cd(t_msh *msh, char **args);

// src/builtins/builtins_export.c
void	msh_export(t_msh *msh, char **args);

// src/builtins/builtins_helpers_atol.c
long	msh_atol(t_msh *msh, char *nptr);

// src/builtins/builtins_helpers_checks.c
bool	check_for_whitespace_end(char *str, int start);
bool	check_newline_skipping(char *str);
bool	check_name_validity(t_msh *msh, char *str);
void	check_exit_code_validity(t_msh *msh, char *str);

// src/builtins/builtins_helpers_env.c
char	*get_env_var_value(t_msh *msh, char *name);
bool	check_and_update_env_var_value(t_msh *msh, char *name, char *value);
void	check_and_delete_env_var(t_msh *msh, char *name);

// src/builtins/builtins_helpers_errorprints.c
void	print_getcwd_error(char *builtin_name);
void	print_num_error_exit(t_msh *msh, char *str);
void	print_export_error(t_msh *msh, char *str);

// src/builtins/builtins_helpers_prints.c
void	print_env_vars(t_list *env_vars);
void	print_export_wo_args(t_msh *msh, char *name);
void	print_echo_args(t_msh *msh, char **args, int start);

// src/builtins/builtins.c
void	msh_builtins(t_msh *msh, int i);

// src/command_table/cmd_pathname_builder.c
char	*create_cmd_pathname(t_msh *msh, char *cmd);

// src/command_table/cmd_capacity.c
bool	ensure_capacity(void **array, int *count, int *capacity, size_t size);
void	cmd_arrays_capacity_check(t_cmd *cmd, t_msh *msh);

// src/command_table/cmd_free.c
void	free_cmd_table(t_cmd **cmds, int n);

// src/command_table/cmd_parser.c
void	parse_single_cmd_tokens(t_cmd *cmd, t_list **tokens, t_msh *msh);

// src/command_table/cmd_table.c
void	build_cmd_table(t_msh *msh);

// src/environment/env_var.c

// src/environment/envp.c

// src/execution/execution_utils.c

// src/execution/execution.c

// src/heredoc/heredoc_collector.c

// src/heredoc/heredoc_utils.c

// src/heredoc/redirect_heredoc.c

// src/redirection/redirect_infile.c

// src/redirection/redirect_outfile.c

// src/redirection/redirection.c
void	restore_stdin_stdout(t_msh *msh);
int		set_redirections(t_msh *msh, int i);

// src/tokenizer/expansion.c

// src/tokenizer/lexing.c

// src/tokenizer/tokenizer.c

// src/tokenizer/var_split.c

// src/signals/signals.c
void	handle_signal_parent(int signal);

#endif
