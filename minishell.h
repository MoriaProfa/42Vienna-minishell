/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:55:34 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/17 20:33:09 by astehl           ###   ########.fr       */
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

void	free_token(void *content);
__attribute__((noreturn)) void	msh_quit(t_msh *msh, char *msg, int status);
void	msh_print(t_msh *msh, char *msg, int status);
void	print_num_error_exit(t_msh *msh, char *str);
char	*get_env_var_value(t_msh *msh, char *name);
void	print_env_vars(t_list *env_vars);
char	*extract_env_name(char *str, size_t *name_len);
char	*extract_env_value(char *str, size_t name_len);
bool	check_and_update_env_var_value(t_msh *msh, char *name, char *value);
void	prepare_name_and_add_to_env_vars(t_msh *msh, char *name, char *m_value);
void	add_to_env_vars(t_msh *msh, char *name, char *value);
void	build_envp_array(t_msh *msh);
void	free_env_var(void *content);
void	msh_free(t_msh *msh);
void    free_envp(t_msh *msh);
long	msh_atol(t_msh *msh, char *nptr);
void	print_num_error_exit(t_msh *msh, char *str);

// /src/redirection/redirection.c
int	set_redirections(t_msh *msh, int i);

#endif
