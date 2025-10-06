/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 10:48:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/06 22:41:44 by edarmaev         ###   ########.fr       */
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

static void	set_infile(t_msh *msh, t_cmd *cmd, int j)
{
	while (cmd->infiles[++j])
	{
		if (msh->fd_in != -1)
			close(msh->fd_in);
		if (check_access_read(cmd->infiles[j]) == 0)
			break ;
		msh->fd_in = open(cmd->infiles[j], O_RDONLY);
		if (msh->fd_in == -1)
		{
			perror(cmd->infiles[j]);
			break ;// msh_print(msh, NULL, 1); // or should we exit?
		}
	}
}

void	set_outfile(t_msh *msh, t_cmd *cmd, int j)
{
	while (cmd->outfiles[++j].filename)
	{
		if (msh->fd_out != -1)
			close(msh->fd_out);
		if (check_access_write(cmd->outfiles[j].filename) == 0)
			break ;
		if (cmd->outfiles[j].append == false)
			msh->fd_out = open(cmd->outfiles[j].filename,
					O_RDWR | O_CREAT | O_TRUNC, 0644);
		else
			msh->fd_out = open(cmd->outfiles[j].filename,
					O_RDWR | O_CREAT | O_APPEND, 0644);
		if (msh->fd_out == -1)
		{
			perror(cmd->outfiles[j].filename);
			break ;//msh_print(msh, NULL, 1); // maybe exit?
		}
		if (msh->fd_out != -1)
		{
			dup2(msh->fd_out, STDOUT_FILENO);
			close(msh->fd_out);
		}
	}
}

void	set_redirections(t_msh *msh, int i)
{
	if (msh->fd_pipe_read != STDIN_FILENO)
	{
		dup2(msh->fd_pipe_read, STDIN_FILENO);
		close(msh->fd_pipe_read);
	}
	set_infile(msh, ((t_cmd *)msh->cmd_table[i]), -1);
	if (((t_cmd *)msh->cmd_table[i])->last_input == T_IN_REDIRECT)
	{
		if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
			close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
		dup2(msh->fd_in, STDIN_FILENO);
		close(msh->fd_in);
	}
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_HEREDOC_REDIRECT)
	{
		if (msh->fd_in != STDIN_FILENO && msh->fd_in != -1)
			close(msh->fd_in);
		dup2(((t_cmd *)msh->cmd_table[i])->heredoc_read, STDIN_FILENO);
		close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
	}
	if (msh->fd[1] != -1)
	{
		dup2(msh->fd[1], STDOUT_FILENO);
		close(msh->fd[1]);
	}
	set_outfile(msh, ((t_cmd *)msh->cmd_table[i]), -1);
}
