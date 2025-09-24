/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 12:33:42 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/24 16:13:49 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_pid(t_msh *msh)
{
	int	i;

	i = -1;
	msh->pid = (pid_t *)ft_calloc(msh->n_cmds + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_end(msh, "pid malloc error");
	while (++i < msh->n_cmds)
		msh->pid[i] = -1;
}

int	check_access_read(char *file)
{
	if (access(file, F_OK) == 0 && access(file, R_OK) != 0)
	{
		perror(file);
		return (0);
	}
	else
		return (1);
}

int	check_access_write(char *file)
{
	if (access(file, F_OK) == 0 && access(file, W_OK) != 0)
	{
		perror(file);
		return (0);
	}
	else
		return (1);
}

void	set_fd_in(t_msh *msh, int i, int j)
{
	msh->fd_in = msh->fd_in_previous;
	/*if (((t_cmd *)msh->cmd_table[i])->heredocs->delimiter)
	{
		if (msh->fd_in != STDIN_FILENO)
			close(msh->fd_in);
		if (pipe(msh->hd_fd) == -1)
			msh_end(msh, "pipe failed");
		write(msh->hd_fd[1], ((t_cmd *)msh->cmd_table[i])->heredocs->buffer,
			ft_strlen(((t_cmd *)msh->cmd_table[i])->heredocs->buffer));
		close(msh->hd_fd[1]);
		msh->fd_in = msh->hd_fd[0];
	}
	else */if (((t_cmd *)msh->cmd_table[i])->n_inf > 0)
	{
		while (j < ((t_cmd *)msh->cmd_table[i])->n_inf)
		{
			if (check_access_read(((t_cmd *)msh->cmd_table[i])->infiles[j]) == 0)
				return ;
			msh->fd_in = open(((t_cmd *)msh->cmd_table[i])->infiles[j],
					O_RDONLY);
			if (msh->fd_in == -1)
				perror(((t_cmd *)msh->cmd_table[i])->infiles[j]);
		}
	}
}

void	set_fd_out(t_msh *msh, int i, int j)
{
	while (((t_cmd *)msh->cmd_table[i])->n_outf > 0
		&& ++j < ((t_cmd *)msh->cmd_table[i])->n_outf)
	{
		if (((t_cmd *)msh->cmd_table[i])->outfiles[j].filename)
		{
			if (check_access_write(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename) == 0)
				return ;
			if (msh->fd_out != STDOUT_FILENO)
				close(msh->fd_out);
			if (((t_cmd *)msh->cmd_table[i])->outfiles[j].append == false)
				msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename,
						O_RDWR | O_CREAT | O_TRUNC, 0666);
			else
				msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename,
						O_RDWR | O_CREAT | O_APPEND, 0666);
			if (msh->fd_out == -1)
				perror(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename);
		}
	}
}

void	msh_execute(t_msh *msh, int i)
{
	if (!((t_cmd *)msh->cmd_table[i])->pathname)
	{
		msh->status = 127;
		msh_end(msh, "command not found");
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname
		&& access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == -1)
	{
		msh->status = 126;
		msh_end(msh, "permission denied");
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname
		&& access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == 0)
		execve(((t_cmd *)msh->cmd_table[i])->pathname,
			((t_cmd *)msh->cmd_table[i])->args, msh->envp);
	msh_end(msh, "execve failed");
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
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_end(msh, "fork failed");
	if (msh->pid[i] == 0)
	{
		close_fds(msh, fd_in, fd_out);
		if (fd_in != STDIN_FILENO)
		{
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);
		}
		if (fd_out != STDOUT_FILENO)
		{
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
		}
		msh_execute(msh, i);
	}
}

void	msh_pipeline(t_msh *msh, int i)
{
	// need to add signal handling in the child
	while (++i < msh->n_cmds)
	{
		set_fd_in(msh, i, -1);
		set_fd_out(msh, i, -1);
		if (i < msh->n_cmds - 1 && ((t_cmd *)msh->cmd_table[i])->n_outf == 0)
		{
			if (pipe(msh->fd) == -1)
				msh_end(msh, "pipe failed");
			msh->fd_out = msh->fd[1];
			msh->fd_in_previous = msh->fd[0];
		}
		msh_fork(msh, msh->fd_in, msh->fd_out, i);
		if (msh->fd_in != STDIN_FILENO)
			close(msh->fd_in);
		if (msh->fd_out != STDOUT_FILENO)
			close(msh->fd_out);
	}
	i = -1;
	while (++i < msh->n_cmds)
		waitpid(msh->pid[i], &msh->status, 0);
	close_fds(msh, STDIN_FILENO, STDOUT_FILENO);
}
