/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_files.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 00:52:04 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 19:35:10 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	redirect_infile(t_msh *msh, int i, int j)
{
	if (msh->fd_in != -1)
		close(msh->fd_in);
	msh->fd_in = open(((t_cmd *)msh->cmd_table[i])->redirs[j].filename,
			O_RDONLY);
	if (msh->fd_in == -1)
	{
		perror(((t_cmd *)msh->cmd_table[i])->redirs[j].filename);
		return (1);
	}
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_IN_REDIR)
		dup2(msh->fd_in, STDIN_FILENO);
	return (0);
}

int	redirect_outfile(t_msh *msh, int i, int j)
{
	if (msh->fd_out != -1)
		close(msh->fd_out);
	if (((t_cmd *)msh->cmd_table[i])->redirs[j].type == T_OUT_REDIR)
		msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->redirs[j].filename,
				O_RDWR | O_CREAT | O_TRUNC, 0644);
	else if (((t_cmd *)msh->cmd_table[i])->redirs[j].type == T_APPEND_REDIR)
		msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->redirs[j].filename,
				O_RDWR | O_CREAT | O_APPEND, 0644);
	if (msh->fd_out == -1)
	{
		perror(((t_cmd *)msh->cmd_table[i])->redirs[j].filename);
		return (1);
	}
	dup2(msh->fd_out, STDOUT_FILENO);
	close(msh->fd_out);
	return (0);
}
