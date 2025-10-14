/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_outfile.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 00:52:04 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/14 05:07:08 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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

static int	set_outfile(t_cmd *cmd, int j)
{
	int	fd;

	fd = -1;
	while (cmd->outfiles[++j].filename)
	{
		if (fd != -1)
			close(fd);
		if (check_access_write(cmd->outfiles[j].filename) == 0)
			return (-1);
		if (cmd->outfiles[j].append == false)
			fd = open(cmd->outfiles[j].filename, O_RDWR | O_CREAT | O_TRUNC,
					0644);
		else
			fd = open(cmd->outfiles[j].filename, O_RDWR | O_CREAT | O_APPEND,
					0644);
		if (fd == -1)
		{
			perror(cmd->outfiles[j].filename);
			return (fd);
		}
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
