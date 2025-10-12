/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 12:33:42 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/12 22:30:06 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_builtins(t_msh *msh, int i);
void	close_heredocs(t_msh *msh);

void	msh_pid(t_msh *msh) // could be put in cmd_table
{
	int i;

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
		msh_print(msh, "command not found", 127);
		return ;
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname
		&& access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == -1)
	{
		perror(((t_cmd *)msh->cmd_table[i])->args[0]);
		msh_print(msh, NULL, 126);
		return ;
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname
		&& access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == 0)
		execve(((t_cmd *)msh->cmd_table[i])->pathname,
			((t_cmd *)msh->cmd_table[i])->args, msh->envp);
	msh_print(msh, "execve failed", 1);
}

void	print_heredoc_fd(t_msh *msh);

void	print_fds(t_msh *msh, int i)
{
	printf("child[%d] msh->fd_in = %d\n", i, msh->fd_in);
	printf("child[%d] msh->fd_out = %d\n", i, msh->fd_out);
	printf("child[%d] msh->fd[0] = %d\n", i, msh->fd[0]);
	printf("child[%d] msh->fd[1] = %d\n", i, msh->fd[1]);
	printf("child[%d] msh->fd_pipe_read = %d\n", i, msh->fd_pipe_read);
	print_heredoc_fd(msh);
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
	if (msh->fd_pipe_read != -1)
		close(msh->fd_pipe_read);
	close_heredocs(msh);
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

void	child_process(t_msh *msh, int i)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	if (set_redirections(msh, i) != 0)
		msh_quit(msh, NULL, 1);
	if (msh->fd[0] != -1)
		close(msh->fd[0]);
	if (((t_cmd *)msh->cmd_table[i])->args[0]
		&& ((t_cmd *)msh->cmd_table[i])->builtin.builtin == false)
		msh_execute(msh, i);
	else if (((t_cmd *)msh->cmd_table[i])->args[0]
		&& ((t_cmd *)msh->cmd_table[i])->builtin.builtin == true)
		msh_builtins(msh, i);
	close_fds(msh);
	msh_quit(msh, NULL, msh->status);
}

void	msh_fork(t_msh *msh, int i)
{
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_quit(msh, "fork failed", 1);
	if (msh->pid[i] == 0)
	{
		child_process(msh, i);
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

void	msh_waitpid(t_msh *msh, int i)
{
	int	wstatus;
	int	sig;

	waitpid(msh->pid[i], &wstatus, 0);
	if (WIFEXITED(wstatus))
		msh->status = WEXITSTATUS(wstatus);
	else if (WIFSIGNALED(wstatus))
	{
		sig = WTERMSIG(wstatus);
		msh->status = 128 + sig;
		if (sig == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		else if (sig == SIGQUIT)
			write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
	}
}

void	run_command(t_msh *msh, int i)
{
	while (++i < msh->n_cmds)
	{
		if (msh->n_cmds == 1 && ((t_cmd *)msh->cmd_table[i])->args[0]
			&& ((t_cmd *)msh->cmd_table[i])->builtin.builtin == true)
			run_builtins(msh, i);
		else
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
	while (++i < msh->n_cmds && msh->pid && msh->pid[i] != -1)
	{
		msh_waitpid(msh, i);
	}
}
