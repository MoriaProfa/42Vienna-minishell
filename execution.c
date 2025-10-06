/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 12:33:42 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/06 02:12:21 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_builtins(t_msh *msh, int i);

void	msh_pid(t_msh *msh)
{
	int	i;

	i = -1;
	msh->pid = (pid_t *)ft_calloc(msh->n_cmds + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_exit(msh, "pid malloc error", 1);
	while (++i < msh->n_cmds)
		msh->pid[i] = -1;
}

void	msh_execute(t_msh *msh, int i)
{
	if (!((t_cmd *)msh->cmd_table[i])->pathname)
	{
		msh->status = 127;
		msh_exit(msh, "command not found", 1);
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname
		&& access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == -1)
	{
		msh->status = 126;
		msh_exit(msh, "permission denied", 1);
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname
		&& access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == 0)
		execve(((t_cmd *)msh->cmd_table[i])->pathname,
			((t_cmd *)msh->cmd_table[i])->args, msh->envp);
	msh_exit(msh, "execve failed", 1);
}

void	close_fds(t_msh *msh, int fd1, int fd2)
{
	if (msh->fd_in != fd1)
		close(msh->fd_in);
	if (msh->fd_out != fd2)
		close(msh->fd_out);
	if (msh->fd[0] != fd1 && msh->fd[0] != -1)
		close(msh->fd[0]);
	if (msh->fd[1] != fd2 && msh->fd[1] != -1)
		close(msh->fd[1]);
}

void	msh_fork(t_msh *msh, int fd_in, int fd_out, int i)
{
	(void)fd_in;
	(void)fd_out;
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_exit(msh, "fork failed", 1);
	if (msh->pid[i] == 0)
	{
		if (((t_cmd *)msh->cmd_table[i])->builtin.builtin == false)
			msh_execute(msh, i);
		else
		{
			msh_builtins(msh, i);
			exit(0);
		}
	}
}

void	set_pipes(t_msh *msh)
{
	if (pipe(msh->fd) == -1)
		msh_exit(msh, "pipe failed", 1);
	dup2(msh->fd[1], STDOUT_FILENO);
	close(msh->fd[1]);
	if (msh->fd_pipe_read != STDIN_FILENO)
		close(msh->fd_pipe_read);
	msh->fd_pipe_read = msh->fd[0];
}

void	run_command(t_msh *msh, int i)
{
	// need to add signal handling in the child
	while (++i < msh->n_cmds)
	{
		if (i < msh->n_cmds - 1)
			set_pipes(msh);
		set_redirections(msh, i);
		if (msh->n_cmds == 1 && ((t_cmd *)msh->cmd_table[i])->builtin.builtin == true)
			msh_builtins(msh, i);
		else
			msh_fork(msh, msh->fd_in, msh->fd_out, i);
	}
	i = -1;
	while (++i < msh->n_cmds)
		waitpid(msh->pid[i], &msh->status, 0);
	close_fds(msh, STDIN_FILENO, STDOUT_FILENO);
}
