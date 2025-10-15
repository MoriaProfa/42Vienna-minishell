/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_infile.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 05:03:32 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/15 21:25:22 by edarmaev         ###   ########.fr       */
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

static int	set_infile(t_cmd *cmd, int j)
{
	int	fd;

	fd = -1;
	if (fd != -1)
		close(fd);
	if (check_access_read(cmd->redirs[j].filename) == 0)
		return (-1);
	fd = open(cmd->redirs[j].filename, O_RDONLY);
	if (fd == -1)
	{
		perror(cmd->redirs[j].filename);
		return (fd);
	}
	return (fd);
}

int	redirect_infile(t_msh *msh, int i, int j)
{
	msh->fd_in = set_infile(((t_cmd *)msh->cmd_table[i]), j);
	if (msh->fd_in == -1)
		return (1);
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_IN_REDIRECT)
		dup2(msh->fd_in, STDIN_FILENO);
	return (0);
}
