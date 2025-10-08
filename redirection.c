/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 10:48:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/08 19:41:21 by edarmaev         ###   ########.fr       */
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

static int	set_infile(t_msh *msh, t_cmd *cmd, int j)
{
	while (cmd->infiles[++j])
	{
		if (msh->fd_in != -1)
			close(msh->fd_in);
		if (check_access_read(cmd->infiles[j]) == 0)
			return (1);
		msh->fd_in = open(cmd->infiles[j], O_RDONLY);
		if (msh->fd_in == -1)
		{
			perror(cmd->infiles[j]);
			return (1); // or should we exit?
		}
		dup2(msh->fd_in, STDIN_FILENO);
		close(msh->fd_in);
	}
	return (0);
}

static int	set_outfile(t_msh *msh, t_cmd *cmd, int j)
{
	while (cmd->outfiles[++j].filename)
	{
		if (msh->fd_out != -1)
			close(msh->fd_out);
		if (check_access_write(cmd->outfiles[j].filename) == 0)
			return (1);
		if (cmd->outfiles[j].append == false)
			msh->fd_out = open(cmd->outfiles[j].filename,
					O_RDWR | O_CREAT | O_TRUNC, 0644);
		else
			msh->fd_out = open(cmd->outfiles[j].filename,
					O_RDWR | O_CREAT | O_APPEND, 0644);
		if (msh->fd_out == -1)
		{
			perror(cmd->outfiles[j].filename);
			return (1); // or should we exit?
		}
		dup2(msh->fd_out, STDOUT_FILENO);
		close(msh->fd_out);
	}
	return (0);
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

int	set_redirections(t_msh *msh, int i)
{
	if (msh->fd[0] != -1 && i < msh->n_cmds - 1)
		close(msh->fd[0]);
	if (msh->fd_pipe_read != -1)
	{
		dup2(msh->fd_pipe_read, STDIN_FILENO);
		close(msh->fd_pipe_read);
	}
	if (set_infile(msh, ((t_cmd *)msh->cmd_table[i]), -1) == 1)
		return (1);
	if (((t_cmd *)msh->cmd_table[i])->last_input == T_HEREDOC_REDIRECT)
	{
		dup2(((t_cmd *)msh->cmd_table[i])->heredoc_read, STDIN_FILENO); // add close
	}
	close_heredoc_fds(msh);
	if (msh->fd[1] != -1 && i < msh->n_cmds - 1)
	{
		dup2(msh->fd[1], STDOUT_FILENO);
		close(msh->fd[1]);
	}
	if (set_outfile(msh, ((t_cmd *)msh->cmd_table[i]), -1) == 1)
		return (1);
	return (0);
}
