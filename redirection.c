/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 10:48:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/14 18:02:05 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int			redirect_infile(t_msh *msh, int i, int j);
int			redirect_outfile(t_msh *msh, int i, int j);
int			redirect_heredoc(t_msh *msh, int i, int j);
void		close_heredocs(t_msh *msh);

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

static void	redirect_pipes(t_msh *msh, int i)
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

int	set_redirections(t_msh *msh, int i)
{
	int	return_value;

	return_value = 0;
	if (msh->n_cmds > 0)
		redirect_pipes(msh, i);
	if (return_value == 0 && ((t_cmd *)msh->cmd_table[i])->n_inf > 0)
		return_value += redirect_infile(msh, i, -1);
	if (return_value == 0 && ((t_cmd *)msh->cmd_table[i])->n_hd > 0)
		return_value += redirect_heredoc(msh, i, -1);
	if (msh->fd_in != -1)
		close(msh->fd_in);
	if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
		close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
	if (return_value == 0 && ((t_cmd *)msh->cmd_table[i])->n_outf > 0)
		return_value += redirect_outfile(msh, i, -1);
	return (return_value);
}
