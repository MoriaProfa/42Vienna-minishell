/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:55:34 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 05:21:55 by edarmaev         ###   ########.fr       */
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
	T_HEREDOC_REDIRECT
}	t_token_type;
//maybe renaming all "redirect" to "redir" to shorten line lengths
typedef enum e_builtin_type
{
	B_NOT_BUILTIN,
	B_PWD,
	B_CD,
	B_EXIT,
	B_ENV,
	B_EXPORT,
	B_UNSET,
	B_ECHO
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

// src/builtins/builtins_cd.c
void	msh_cd(t_msh *msh, char **args);

// src/builtins/builtins_export.c
void	handle_export_with_value(char *arg, t_msh *msh);
void	msh_export(t_msh *msh, char **args);

// src/builtins/builtins_utils_atol.c
long	msh_atol(t_msh *msh, char *nptr);

// src/builtins/builtins_utils_checks.c
bool	check_for_whitespace_end(char *str, int start);
bool	check_newline_skipping(char *str);
bool	check_name_validity(t_msh *msh, char *str);
void	check_exit_code_validity(t_msh *msh, char *str);

// src/builtins/builtins_utils_env.c
char	*get_env_var_value(t_msh *msh, char *name);
bool	check_and_update_env_var_value(t_msh *msh, char *name, char *value);
void	check_and_delete_env_var(t_msh *msh, char *name);

// src/builtins/builtins_utils_errorprints.c
void	print_getcwd_error(char *builtin_name);
void	print_num_error_exit(t_msh *msh, char *str);
void	print_export_error(t_msh *msh, char *str);

// src/builtins/builtins_utils_prints.c
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

// src/environment/env_var_utils.c
void	add_to_env_vars(t_msh *msh, char *name, char *value);
void	prepare_and_add_to_env_vars(t_msh *msh, char *name, char *value);
void	prepare_name_and_add_to_env_vars(t_msh *msh, char *name, char *m_value);
char	*extract_env_value(char *str, size_t name_len);
char	*extract_env_name(char *str, size_t *name_len);

// src/environment/env_var.c
void	free_env_var(void *content);
void	init_env_vars(t_msh *msh, char **envp);

// src/environment/envp.c
void    free_envp(t_msh *msh);
void	build_envp_array(t_msh *msh);

// src/execution/execution_utils.c
void	close_and_exit(t_msh *msh, char *msg, int status);
void	run_builtins(t_msh *msh, int i);
bool	special_case_check(t_msh *msh, char *pathname, char *cmd);
int		check_if_directory(char *path);
void	print_execution_error(t_msh *msh, char *cmd, int option, int code);

// src/execution/execution.c
void	run_command(t_msh *msh, int i);

// src/heredoc/heredoc_collector.c
int		collect_one_heredoc(t_msh *msh, t_heredoc heredoc, int fd);
int		collect_all_heredocs(t_msh *msh, int i);

// src/heredoc/heredoc_redirect.c
int		print_warning_heredoc(t_heredoc heredoc, size_t ln);
bool	compare_strings(char *s1, char *s2);
void	close_heredocs(t_msh *msh);
int		set_heredoc(t_msh *msh, t_cmd *cmd, t_heredoc heredoc);
int		redirect_heredoc(t_msh *msh, int i/*, int j*/);

// src/msh_utils/msh_utils.c
void	msh_print(t_msh *msh, char *msg, int status);
void	msh_free(t_msh *msh);
__attribute__((noreturn)) void	msh_quit(t_msh *msh, char *msg, int status);

// src/msh_utils/signals.c
void	handle_signal_parent(int signal);
void	handle_signal_child(int signal);
int		handle_signal_heredoc(t_msh *msh, char **buffer);

// src/redirection/redirect_infile.c
int		redirect_infile(t_msh *msh, int i, int j);

// src/redirection/redirect_outfile.c
int		redirect_outfile(t_msh *msh, int i, int j);

// src/redirection/redirection.c
void	close_fds(t_msh *msh);
void	save_stdin_stdout(t_msh *msh, int i);
void	restore_stdin_stdout(t_msh *msh);
int		set_redirections(t_msh *msh, int i);

// src/tokenizer/expansion.c
char	*expand_value(t_token *token, char *value, char *result, t_msh *msh);
void	expand_word_tokens(t_list *tokens, char *tmp, t_msh *msh);

// src/tokenizer/lexing.c
void	check_quotationmarks(t_msh *msh);
void	check_tokens(t_msh *msh);

// src/tokenizer/tokenizer_handler.c
int		handle_metacharacters(t_msh *msh);
char	*handle_quoted(t_msh *msh, int *len);
char	*handle_unquoted(t_msh *msh, int *len);

// src/tokenizer/tokenizer_merger.c
void	merge_adjacent_tokens(t_list **tokens, t_msh *msh);

// src/tokenizer/tokenizer_utils.c
void	fill_token_list(t_msh *msh, char *value, t_token_type type,	int *quoted_adjacent);
char	*create_word_token(t_msh *msh);
void	remove_quotationmarks_in_tokens(t_list *tokens);

// src/tokenizer/tokenizer.c
void	free_token(void *content);
void	msh_tokenizer(t_msh *msh);

// src/tokenizer/var_split_utils.c
ssize_t	count_words(const char *s, const char *set);
char	**ft_split_with_set(const char *s, const char *set, ssize_t word_count);
void	correct_adjacency(t_list *cur_node, t_token *cur_token);
bool	check_token_validity_for_split(t_token *cur_token);

// src/tokenizer/var_split.c
void    split_expanded_tokens(t_list *tokens, t_msh *msh);

#endif
