/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_outfile.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 00:52:04 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/16 19:24:33 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// static int	check_access_write(char *file)
// {
// 	if (access(file, F_OK) == 0 && access(file, W_OK) != 0)
// 	{
// 		ft_putstr_fd("minishell: ", STDERR_FILENO);
// 		perror(file);
// 		return (0);
// 	}
// 	else
// 		return (1);
// }

static int	set_outfile(t_cmd *cmd, int j)
{
	int	fd;

	fd = -1;
	if (fd != -1)
		close(fd);
	// if (check_access_write(cmd->redirs[j].filename) == 0)
	// 	return (-1);
	if (cmd->redirs[j].type == T_OUT_REDIRECT)
		fd = open(cmd->redirs[j].filename, O_RDWR | O_CREAT | O_TRUNC,
				0644);
	else if (cmd->redirs[j].type == T_APPEND_REDIRECT)
		fd = open(cmd->redirs[j].filename, O_RDWR | O_CREAT | O_APPEND,
					0644);
	if (fd == -1)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(cmd->redirs[j].filename);
		return (fd);
	}
	return (fd);
}

int	redirect_outfile(t_msh *msh, int i, int j)
{
	msh->fd_out = set_outfile(((t_cmd *)msh->cmd_table[i]), j);
	if (msh->fd_out == -1)
		return (1);
	dup2(msh->fd_out, STDOUT_FILENO);
	close(msh->fd_out);
	return (0);
}
