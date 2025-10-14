/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 05:05:23 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/14 05:07:33 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	close_heredocs(t_msh *msh)
{
	int	i;

	i = -1;
	while ((t_cmd *)msh->cmd_table[++i])
	{
		if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
			close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
	}
}

static int	set_heredocs(t_msh *msh, int i, int j)
{
	int	fd;

	fd = -1;
	while (((t_cmd *)msh->cmd_table[i])->heredocs[++j].delimiter)
	{
		if (fd != -1)
			close(fd);
		fd = open(((t_cmd *)msh->cmd_table[i])->heredocs[j].filename,
				O_RDONLY);
		if (fd == -1)
		{
			msh_print(msh, "failed to open temporary heredoc file", 1);
			return (-1);
		}
		else
			unlink(((t_cmd *)msh->cmd_table[i])->heredocs[j].filename);
	}
	return (fd);
}

int	redirect_heredoc(t_msh *msh, int i, int j)
{
	((t_cmd *)msh->cmd_table[i])->heredoc_read = set_heredocs(msh, i, j);
	if (((t_cmd *)msh->cmd_table[i])->heredoc_read == -1)
		return (1);
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_HEREDOC_REDIRECT)
		dup2(((t_cmd *)msh->cmd_table[i])->heredoc_read, STDIN_FILENO);
	return (0);
}
