/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_infile.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 05:03:32 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/15 20:00:37 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
/*
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

int	redirect_infile(t_msh *msh, int i, int j)
{
	msh->fd_in = set_infile(((t_cmd *)msh->cmd_table[i]), j);
	if (msh->fd_in == -1)
		return (1);
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_IN_REDIRECT)
		dup2(msh->fd_in, STDIN_FILENO);
	return (0);
}*/
