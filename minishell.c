/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:00:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/05/07 15:18:17 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//valgrind command:
//valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=readline.supp ./minishell

#define _POSIX_C_SOURCE 200809L //for vscode only so signals work!

#include "minishell.h"

volatile sig_atomic_t   signal_flag = 0;

void	msh_cmd_free(char ***cmd)
{
	int	i;

	i = 0;
	while (cmd[i])
	{
		ft_array_free(cmd[i]);
		cmd[i++] = NULL;
	}
	free(cmd);
	cmd = NULL;
}

void	msh_free(t_msh *msh)
{
	if (msh->in_str)
		ft_free(&msh->in_str);
	if (msh->in_arr)
	{
		ft_array_free(msh->in_arr);
		msh->in_arr = NULL;
	}
	if (msh->cmd)
	{
		msh_cmd_free(msh->cmd);
		msh->cmd = NULL;
	}
	if (msh->pathname)
	{
		ft_array_free(msh->pathname);
		msh->pathname = NULL;
	}
	if (msh->pid)
	{
		free(msh->pid);
		msh->pid = NULL;
	}
}

void	msh_error(t_msh *msh, char *msg)
{
	ft_putendl_fd(msg, 2);
	msh_free(msh);
	rl_clear_history();
	exit(msh->status);
}

void	msh_execute(t_msh *msh, int i)
{
	if (!msh->pathname[i])
	{
		msh->status = 127;
		msh_error(msh, "command not found");//should not exit but display a new prompt
	}
	else if (msh->pathname[i] && access(msh->pathname[i], X_OK) == -1)
	{
		msh->status = 126;
		msh_error(msh, "permission denied");//should also not exit at this point I guess?
	}
	else if (msh->pathname[i] && access(msh->pathname[i], X_OK) == 0)
		execve(msh->pathname[i], msh->cmd[i], msh->envp);
	msh_error(msh, "execve failed");//dunno if it should exit here...
}

char	*msh_pathname_r(t_msh *msh, char **paths, int i)
{
	char	*pathname;
	char	*tmp;
	int		j;

	j = -1;
	pathname = NULL;
	while (paths[++j])
	{
		tmp = ft_strjoin(paths[j], "/");
		if (!tmp)
			msh_error(msh, "pathname tmp ft_strjoin failed");
		pathname = ft_strjoin(tmp, msh->cmd[i][0]);
		free(tmp);
		if (!pathname)
			msh_error(msh, "pathname ft_strjoin failed");
		if (access(pathname, F_OK) == 0)
			return (pathname);
		free(pathname);
	}
	return (NULL);
}

char	*msh_pathname(t_msh *msh, int i)
{
	char	**paths;
	char	*pathname;
	int		j;

	j = 0;
	pathname = NULL;
	if (access(msh->cmd[i][0], F_OK) == 0)
		pathname = msh->cmd[i][0];
	else
	{
		while (msh->envp[j] && ft_strncmp(msh->envp[j], "PATH=", 5) != 0)
			j++;
		if (msh->envp[j])
		{
			paths = ft_split(msh->envp[j] + 5, ':');
			if (!paths)
				msh_error(msh, "paths ft_split failed");
			pathname = msh_pathname_r(msh, paths, i);
			ft_array_free(paths);
		}
	}
	return (pathname);
}

char	*msh_read(ssize_t rr) //useless now?!
{
	char	buf[2];
	char	*tmp;
	char	*out;

	out = NULL;
	while (rr > 0)
	{
		rr = read(0, buf, 1);
		if (rr == -1)
			return (ft_free(&out), NULL);
		buf[rr] = 0;
		if (buf[0] == '\n')
			return (out);
		if (!out)
			out = ft_strcpy(buf);
		else
		{
			tmp = out;
			out = ft_strjoin(tmp, buf);
			ft_free(&tmp);
		}
	}
	return (out);
}

void	msh_fork(t_msh *msh, int i, int fd_in, int fd_out)
{
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_error(msh, "fork failed");
	if (msh->pid[i] == 0)
	{
		if (fd_in != -1)
			dup2(fd_in, STDIN_FILENO);
		if (fd_out != -1)
			dup2(fd_out, STDOUT_FILENO);
		if (fd_in != -1)
			close(fd_in);
		if (fd_out != -1)
			close(fd_out);
		msh_execute(msh, i);
	}
}

void	msh_initialize(t_msh *msh, char **envp)
{
	int	i;

	i = 0;
	msh->status = 1;
	msh->envp = envp;
	msh->in_arr = ft_split(msh->in_str, '|');
	if (!msh->in_arr)
		msh_error(msh, "in_arr split failed");
	msh->n = (int)ft_array_size(msh->in_arr);
	msh->cmd = (char ***)ft_calloc(msh->n + 1, sizeof(char **));
	if (!msh->cmd)
		msh_error(msh, "cmd malloc failed");
	msh->pathname = (char **)ft_calloc(msh->n + 1, sizeof(char *));
	if (!msh->pathname)
		msh_error(msh, "pathname malloc oopsie");
	while (msh->in_arr[i])
	{
		msh->cmd[i] = ft_split(msh->in_arr[i], ' ');
		if (!msh->cmd[i])
			msh_error(msh, "cmd plit failed");
		msh->pathname[i] = msh_pathname(msh, i);
		i++;
	}
	msh->pid = (pid_t *)ft_calloc(msh->n + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_error(msh, "pid malloc error");
	i = -1;
	while (++i < msh->n)
		msh->pid[i] = -1;
	msh->fd[0] = -1;
	msh->fd[1] = -1;
	msh->fd_in = -1;
	msh->fd_out = -1;
}

void	msh_pipeline(t_msh *msh, int i)
{
	while (i < msh->n)
	{
		if (i < msh->n - 1)
		{
			if (pipe(msh->fd) == -1)
				msh_error(msh, "pipe failed");
			msh->fd_out = msh->fd[1];
		}
		msh_fork(msh, i, msh->fd_in, msh->fd_out);
		if (msh->fd_in != -1)
			close(msh->fd_in);
		if (i < msh->n - 1)
		{
			close(msh->fd[1]);// close write end
			msh->fd_in = msh->fd[0];// keep read end for next command
		}
		i++;
	}
	i = 0;
	while (i < msh->n)
	{
		waitpid(msh->pid[i], &msh->status, 0);
		i++;
	}
}

void	handle_signal(int signal, siginfo_t *info, void *context)
{
	(void)*context;
	(void)*info;
	if (signal == SIGINT)
	{
		signal_flag = signal + 128;
		write(1, "\n", 1);
		rl_replace_line("", 0);
		rl_on_new_line();
		rl_redisplay();
	}
	else if (signal == SIGQUIT)
		signal_flag = signal + 128;
}

int	main(int argc, char **argv, char **envp)
{
	t_msh				msh;
	struct sigaction	sa;

	if (argc > 2)
		ft_putendl_fd("Not enough arguments", 2);
	(void)argv;
	rl_catch_signals = 0;
	sa.sa_sigaction = handle_signal;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGQUIT, &sa, NULL);
	while (1)
	{
		msh.in_str = readline("minishell: "); //msh_read(1);
		if (!msh.in_str) //by pressing CTRL+D
			break ;
		if (signal_flag)
		{
			free(msh.in_str);
			msh.in_str = NULL;//bash prints ^C when ctrl+c is pressed, ignores ctrl+\ .
			msh.status = signal_flag;
			signal_flag = 0;
		}
		add_history(msh.in_str);
		msh_initialize(&msh, envp);
		msh_pipeline(&msh, 0);
		msh_free(&msh);
	}
	ft_putendl_fd("exit", 1);
	rl_clear_history();//somehow we still have leaks from add_history?
	msh_free(&msh);
	return (0);
}
