/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_infile.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 05:03:32 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 15:10:00 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	set_infile(t_cmd *cmd, int j)
{
	int	fd;

	fd = -1;
	if (fd != -1)
		close(fd);
	fd = open(cmd->redirs[j].filename, O_RDONLY);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(cmd->redirs[j].filename);
		return (fd);
	}
	return (fd);
}

int	redirect_infile(t_msh *msh, int i, int j)
{
	if (msh->fd_in != -1)
		close(msh->fd_in);
	msh->fd_in = set_infile(((t_cmd *)msh->cmd_table[i]), j);
	if (msh->fd_in == -1)
		return (1);
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_IN_REDIR)
		dup2(msh->fd_in, STDIN_FILENO);
	return (0);
}
