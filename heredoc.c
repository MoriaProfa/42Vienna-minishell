/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/03 02:03:51 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	collect_one_heredoc(t_msh *msh, int i, int j)
{
	if (pipe(msh->hd_fd) == -1)
		msh_end(msh, "heredoc pipe failed", 1);
	printf("fd[0] = %d fd[1] = %d\n", msh->hd_fd[0], msh->hd_fd[1]);
	while (1)
	{
		((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer = readline("> ");
		if (!((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer)
			break ;
		if (ft_strncmp(((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer,
				((t_cmd *)msh->cmd_table[i])->heredocs[j].delimiter,
				ft_strlen(((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer)) == 0)
		{
			free(((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer);
			((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer = NULL;
			break ;
		}
		write(msh->hd_fd[1], ((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer,
			ft_strlen(((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer));
		write(msh->hd_fd[1], "\n", 1);
		free(((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer);
		((t_cmd *)msh->cmd_table[i])->heredocs[j].buffer = NULL;
	}
	close(msh->hd_fd[1]);
}

void	collect_all_heredocs(t_msh *msh, int i, int j)
{
	while (++i < msh->n_cmds)
	{
		while (++j < ((t_cmd *)msh->cmd_table[i])->n_hd)
		{
			if (msh->hd_fd[0] != -1)
				close(msh->hd_fd[0]);
			collect_one_heredoc(msh, i, j);
		}
	}
}
