/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 10:48:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/06 02:11:08 by edarmaev         ###   ########.fr       */
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

static void	set_infile(t_msh *msh, int i, int j)
{
	while (((t_cmd *)msh->cmd_table[i])->infiles[++j])
	{
		if (msh->fd_in != STDIN_FILENO)
			close(msh->fd_in);
		if (check_access_read(((t_cmd *)msh->cmd_table[i])->infiles[j]) == 0)
			return ;
		msh->fd_in = open(((t_cmd *)msh->cmd_table[i])->infiles[j],
				O_RDONLY);
		if (msh->fd_in == -1)
			perror(((t_cmd *)msh->cmd_table[i])->infiles[j]);
	}
}

static void	set_outfile(t_msh *msh, int i, int j)
{
	while (((t_cmd *)msh->cmd_table[i])->outfiles[++j].filename)
	{
		if (check_access_write(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename) == 0)
			break ;
		if (((t_cmd *)msh->cmd_table[i])->outfiles[j].append == false)
			msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename,
					O_RDWR | O_CREAT | O_TRUNC, 0644);
		else
			msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename,
					O_RDWR | O_CREAT | O_APPEND, 0644);
		if (msh->fd_out == -1)
			perror(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename);
		if (msh->fd_out != STDOUT_FILENO && msh->fd_out != -1)
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
	set_infile(msh, i, -1);
	if (((t_cmd *)msh->cmd_table[i])->last_input == T_IN_REDIRECT)
		dup2(msh->fd_in, STDIN_FILENO);
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_HEREDOC_REDIRECT)
		dup2(((t_cmd *)msh->cmd_table[i])->fd_heredoc, STDIN_FILENO);
	if (msh->fd_in != STDIN_FILENO)
		close(msh->fd_in);
	if (((t_cmd *)msh->cmd_table[i])->fd_heredoc != -1)
		close(((t_cmd *)msh->cmd_table[i])->fd_heredoc);
	set_outfile(msh, i, -1);
}
