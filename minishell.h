/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:55:34 by edarmaev          #+#    #+#             */
/*   Updated: 2025/08/12 17:39:37 by edarmaev         ###   ########.fr       */
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
# include <curses.h>
//tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
# include <dirent.h>
//opendir, readdir, closedir
# include <errno.h>
//perror
# include <fcntl.h>
//open
# include <limits.h>
//INT_MAX, INT_MIN
# include <readline/readline.h>
//readline, rl_clear_history, rl_on_new_line, rl_replace_line, 
//rl_redisplay, add_history
# include <readline/history.h>
//readline, rl_clear_history, rl_on_new_line, rl_replace_line,
//rl_redisplay, add_history
# include <signal.h>
//signal, sigaction, sigemptyset, sigaddset, kill
# include <stdio.h>
//printf, perror
# include <string.h>
//strerror
# include <sys/ioctl.h>
//ioctl
# include <sys/resource.h>
//wait3, wait4
# include <sys/stat.h>
//stat, lstat, fstat
# include <sys/time.h>
//wait3, wait4
# include <sys/types.h>
//fork, wait, waitpid, wait3, wait4, kill, stat, lstat, fstat, opendir, closedir
# include <sys/wait.h>
//wait, waitpid, wait3, wait4
# include <term.h>
//tgetent, tgetflag, tgetnum, tgetstr, tgoto, tputs
# include <termios.h>
//tcsetattr, tcgetattr

//t_list in libft.h maybe changed to double linked list -> add *previous
//ft_lstnew + ft_lstadd_back + ft_lstadd_front to be modified then

typedef enum e_token_type
{
	T_WORD,
	T_PIPE,
	T_IN_REDIRECT,
	T_OUT_REDIRECT,
	T_APPEND_REDIRECT,
	T_HEREDOC_REDIRECT
}	t_token_type;

typedef struct s_token
{
	char			*value;
	t_token_type	type;
}					t_token;

typedef struct s_msh
{
	char	*in_str;
	char	*cpy_in_str;
	t_list	*tokens;
	char	**in_arr; //token_list instead?
	char	***cmd;
	char	**pathname;
	int		n;
	char	**envp;
	int		status;
	pid_t	*pid;
	int		fd_in;
	int		fd_out;
	int		fd[2];
}			t_msh;

void	msh_end(t_msh *msh, char *msg);
int	msh_pwd(t_msh *msh);
int	msh_cd(t_msh *msh);

#endif