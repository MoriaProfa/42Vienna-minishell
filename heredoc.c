/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/05 12:35:32 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	collect_one_heredoc(t_msh *msh, t_cmd *cmd, int hd_fd[2], int j)
{
	(void)msh;
	while (1)
	{
		cmd->heredocs[j].buffer = readline("> ");
		if (!cmd->heredocs[j].buffer)
			break ;
		// if ((cmd->heredocs[j].expand == true)
		// 	printf("ATTENTION to expansion!\n"); // add expansion-> should I tokenize the input to expand?
		if (ft_strlen(cmd->heredocs[j].buffer) == ft_strlen(cmd->heredocs[j].delimiter)
			&& ft_strncmp(cmd->heredocs[j].buffer, cmd->heredocs[j].delimiter,
				ft_strlen(cmd->heredocs[j].buffer)) == 0)
		{
			free(cmd->heredocs[j].buffer);
			cmd->heredocs[j].buffer = NULL;
			break ;
		}
		write(hd_fd[1], cmd->heredocs[j].buffer,
			ft_strlen(cmd->heredocs[j].buffer));
		write(hd_fd[1], "\n", 1);
		free(cmd->heredocs[j].buffer);
		cmd->heredocs[j].buffer = NULL;
	}
}

void	collect_all_heredocs(t_msh *msh, int i, int j)
{
	int	hd_fd[2];

	while (++i < msh->n_cmds)
	{
		while (++j < ((t_cmd *)msh->cmd_table[i])->n_hd)
		{
			if (((t_cmd *)msh->cmd_table[i])->hd_fd != -1)
				close(((t_cmd *)msh->cmd_table[i])->hd_fd);
			if (pipe(hd_fd) == -1)
				msh_end(msh, "heredoc pipe failed", 1);
			collect_one_heredoc(msh, (t_cmd *)msh->cmd_table[i], hd_fd, j);
			close(hd_fd[1]);
			if (((t_cmd *)msh->cmd_table[i])->hd_fd != -1)
				close(((t_cmd *)msh->cmd_table[i])->hd_fd);
			((t_cmd *)msh->cmd_table[i])->hd_fd = hd_fd[0];
		}
	}
}
