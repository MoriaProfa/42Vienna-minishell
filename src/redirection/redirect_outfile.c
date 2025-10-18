/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_outfile.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 00:52:04 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 16:22:32 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	set_outfile(t_cmd *cmd, int j)
{
	int	fd;

	fd = -1;
	if (fd != -1)
		close(fd);
	if (cmd->redirs[j].type == T_OUT_REDIR)
		fd = open(cmd->redirs[j].filename, O_RDWR | O_CREAT | O_TRUNC,
				0644);
	else if (cmd->redirs[j].type == T_APPEND_REDIR)
		fd = open(cmd->redirs[j].filename, O_RDWR | O_CREAT | O_APPEND,
				0644);
	if (fd == -1)
	{
		perror(cmd->redirs[j].filename);
		return (fd);
	}
	return (fd);
}

int	redirect_outfile(t_msh *msh, int i, int j)
{
	if (msh->fd_out != -1)
		close(msh->fd_out);
	msh->fd_out = set_outfile(((t_cmd *)msh->cmd_table[i]), j);
	if (msh->fd_out == -1)
		return (1);
	dup2(msh->fd_out, STDOUT_FILENO);
	close(msh->fd_out);
	return (0);
}
