/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 10:48:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/12 00:33:30 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_access_read(char *file)
{
	if (access(file, F_OK) == 0 && access(file, R_OK) != 0)
	{
		perror(file);
		return (0);
	}
	else
		return (1);
}

static int	check_access_write(char *file)
{
	if (access(file, F_OK) == 0 && access(file, W_OK) != 0)
	{
		perror(file);
		return (0);
	}
	else
		return (1);
}

static int	set_infile(t_cmd *cmd, int j)
{
	int	fd;

	fd = -1;
	while (cmd->infiles[++j])
	{
		if (fd != -1)
			close(fd);
		if (check_access_read(cmd->infiles[j]) == 0)
			return (-1);
		fd = open(cmd->infiles[j], O_RDONLY);
		if (fd == -1)
		{
			perror(cmd->infiles[j]);
			return (fd);
		}
	}
	return (fd);
}

static int	set_outfile(t_cmd *cmd, int j)
{
	int	fd;

	fd = -1;
	while (cmd->outfiles[++j].filename)
	{
		if (fd != -1)
			close(fd);
		if (check_access_write(cmd->outfiles[j].filename) == 0)
			return (-1);
		if (cmd->outfiles[j].append == false)
			fd = open(cmd->outfiles[j].filename, O_RDWR | O_CREAT | O_TRUNC,
					0644);
		else
			fd = open(cmd->outfiles[j].filename, O_RDWR | O_CREAT | O_APPEND,
					0644);
		if (fd == -1)
		{
			perror(cmd->outfiles[j].filename);
			return (fd);
		}
	}
	return (fd);
}

void	close_heredoc_fds(t_msh *msh)
{
	int	i;

	i = -1;
	while ((t_cmd *)msh->cmd_table[++i])
	{
		if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
			close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
	}
}

void	redirect_pipes(t_msh *msh, int i)
{
	if (i < msh->n_cmds - 1)
	{
		if (msh->fd[0] != -1)
			close(msh->fd[0]);
		if (msh->fd[1] != -1)
		{
			dup2(msh->fd[1], STDOUT_FILENO);
			close(msh->fd[1]);
		}
	}
	if (i > 0)
	{
		if (msh->fd_pipe_read != -1)
		{
			dup2(msh->fd_pipe_read, STDIN_FILENO);
			close(msh->fd_pipe_read);
		}
	}
}

static int	redirect_input(t_msh *msh, int i)
{
	if (((t_cmd *)msh->cmd_table[i])->last_input == T_IN_REDIRECT)
	{
		msh->fd_in = set_infile(((t_cmd *)msh->cmd_table[i]), -1);
		if (msh->fd_in == -1)
			return (1);
		dup2(msh->fd_in, STDIN_FILENO);
		close(msh->fd_in);
	}
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_HEREDOC_REDIRECT)
		dup2(((t_cmd *)msh->cmd_table[i])->heredoc_read, STDIN_FILENO);
	close_heredoc_fds(msh);
	return (0);
}

int	set_redirections(t_msh *msh, int i)
{
	int	return_value;

	redirect_pipes(msh, i);
	return_value = redirect_input(msh, i);
	if (((t_cmd *)msh->cmd_table[i])->n_outf > 0)
	{
		msh->fd_out = set_outfile(((t_cmd *)msh->cmd_table[i]), -1);
		if (msh->fd_out == -1)
			return (1);
		dup2(msh->fd_out, STDOUT_FILENO);
		close(msh->fd_out);
	}
	return (0);
}
