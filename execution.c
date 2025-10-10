/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 12:33:42 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/10 20:48:12 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_builtins(t_msh *msh, int i);

void	msh_pid(t_msh *msh) // could be put in cmd_table
{
	int	i;

	i = -1;
	msh->pid = (pid_t *)ft_calloc(msh->n_cmds + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_quit(msh, "pid malloc error", 1);
	while (++i < msh->n_cmds)
		msh->pid[i] = -1;
}

void	msh_execute(t_msh *msh, int i)
{
	if (!((t_cmd *)msh->cmd_table[i])->pathname)
	{
		ft_putstr_fd(((t_cmd *)msh->cmd_table[i])->args[0], STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		msh_quit(msh, "command not found", 127);
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname
		&& access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == -1)
	{
		printf("cmd_table[%d]\n", i);
		perror(((t_cmd *)msh->cmd_table[i])->args[0]);
		msh_quit(msh, NULL, 126);
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname
		&& access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == 0)
		execve(((t_cmd *)msh->cmd_table[i])->pathname,
			((t_cmd *)msh->cmd_table[i])->args, msh->envp);
	msh_quit(msh, "execve failed", 1);
}

void	close_fds(t_msh *msh)
{
	if (msh->fd_in != -1)
		close(msh->fd_in);
	if (msh->fd_out != -1)
		close(msh->fd_out);
	if (msh->fd[0] != -1)
		close(msh->fd[0]);
	if (msh->fd[1] != -1)
		close(msh->fd[1]);
	if (msh->fd_hd[0] != -1)
		close(msh->fd_hd[0]);
	if (msh->fd_hd[1] != -1)
		close(msh->fd_hd[1]);
}

void	save_stdin_stdout(t_msh *msh, int i)
{
	if (((t_cmd *)msh->cmd_table[i])->n_outf > 0)
		msh->saved_stdout = dup(STDOUT_FILENO);
	if (((t_cmd *)msh->cmd_table[i])->n_inf > 0
			|| ((t_cmd *)msh->cmd_table[i])->n_hd > 0)
		msh->saved_stdin = dup(STDIN_FILENO);
}

void	restore_stdin_stdout(t_msh *msh)
{
	if (msh->saved_stdin != -1)
	{
		dup2(msh->saved_stdin, STDIN_FILENO);
		close(msh->saved_stdin);
	}
	if (msh->saved_stdout != -1)
	{
		dup2(msh->saved_stdout, STDOUT_FILENO);
		close(msh->saved_stdout);
	}
}

void	msh_fork(t_msh *msh, int i)
{
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_quit(msh, "fork failed", 1);
	if (msh->pid[i] == 0)
	{
		if (set_redirections(msh, i) != 0)
			msh_quit(msh, NULL, 1);
		if (msh->fd[0] != -1)
			close(msh->fd[0]);
		if (((t_cmd *)msh->cmd_table[i])->builtin.builtin == false)
			msh_execute(msh, i);
		else
		{
			msh_builtins(msh, i);
			exit(0); // close everything fds
		}
	}
	if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
		close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
	if (msh->fd_pipe_read != -1)
		close(msh->fd_pipe_read);
	if (i < msh->n_cmds - 1)
	{
		if (msh->fd[1] != -1)
			close(msh->fd[1]);
		msh->fd_pipe_read = msh->fd[0];
	}
}

void	run_builtins(t_msh *msh, int i)
{
	save_stdin_stdout(msh, i);
	if (set_redirections(msh, i) == 0)
		msh_builtins(msh, i);
	restore_stdin_stdout(msh);
}

void	run_command(t_msh *msh, int i)
{
	// need to add signal handling in the child
	while (++i < msh->n_cmds)
	{
		if (((t_cmd *)msh->cmd_table[i])->args[0]
			&& ((t_cmd *)msh->cmd_table[i])->builtin.builtin == true)
			run_builtins(msh, i);
		else if (((t_cmd *)msh->cmd_table[i])->args[0])
		{
			if (i < msh->n_cmds - 1)
			{
				if (pipe(msh->fd) == -1)
					msh_quit(msh, "pipe failed", 1);
			}
			msh_fork(msh, i);
		}
	}
	i = -1;
	while (++i < msh->n_cmds && msh->pid[i] != -1)
		waitpid(msh->pid[i], &msh->status, 0);
}
