/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_free.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 22:06:29 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 22:14:31 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	free_single_cmd_structs_data(t_cmd *cmd)
{
	int	j;

	j = -1;
	while (cmd->redirs && ++j < cmd->n_redirs)
		free(cmd->redirs[j].filename);
	j = -1;
	while (cmd->heredocs && ++j < cmd->n_hd)
	{
		free(cmd->heredocs[j].delimiter);
		if (cmd->heredocs[j].buffer)
			free(cmd->heredocs[j].buffer);
		if (cmd->heredocs[j].filename)
			free(cmd->heredocs[j].filename);
	}
}

void	free_cmd_table(t_cmd **cmds, int n)
{
	int	i;

	i = -1;
	if (!cmds)
		return ;
	while (cmds[++i] && i < n)
	{
		ft_array_free(cmds[i]->args, (size_t)cmds[i]->n_args);
		free_single_cmd_structs_data(cmds[i]);
		if (cmds[i]->redirs)
			free(cmds[i]->redirs);
		if (cmds[i]->heredocs)
			free(cmds[i]->heredocs);
		if (cmds[i]->pathname)
			free(cmds[i]->pathname);
		free(cmds[i]);
	}
	free(cmds);
}
