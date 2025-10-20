/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:28:42 by astehl            #+#    #+#             */
/*   Updated: 2025/10/20 18:01:24 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_cmd	*init_cmd_data(t_msh *msh, t_cmd *cmd)
{
	cmd->capacity_args = 4;
	cmd->args = ft_calloc(cmd->capacity_args, sizeof(char *));
	if (!cmd->args)
		(free(cmd), msh_quit(msh, "malloc for cmd->args failed", 1));
	cmd->capacity_redirs = 4;
	cmd->redirs = ft_calloc(cmd->capacity_redirs, sizeof(t_redir));
	if (!cmd->redirs)
		(free(cmd->args), free(cmd), msh_quit(msh,
				"malloc for cmd->redirs failed", 1));
	cmd->capacity_hd = 4;
	cmd->heredocs = ft_calloc(cmd->capacity_hd, sizeof(t_heredoc));
	if (!cmd->heredocs)
		(free(cmd->redirs), free(cmd->args), free(cmd), msh_quit(msh,
				"malloc for cmd->heredocs failed", 1));
	return (cmd);
}

static t_cmd	*init_cmd(t_msh *msh)
{
	t_cmd	*cmd;

	cmd = ft_calloc(1, sizeof(t_cmd));
	if (!cmd)
		msh_quit(msh, "malloc for cmd failed", 1);
	return (init_cmd_data(msh, cmd));
}

void	build_cmd_table(t_msh *msh)
{
	int		capacity;
	t_list	*tokens;

	capacity = 4;
	msh->cmd_table = ft_calloc(capacity, sizeof(t_cmd *));
	if (!msh->cmd_table)
		msh_quit(msh, "cmd table malloc failed", 1);
	tokens = msh->tokens;
	while (tokens)
	{
		if (!ensure_capacity((void **)&msh->cmd_table, &msh->n_cmds, &capacity,
				sizeof(t_cmd *)))
			msh_quit(msh, "cmd table array capacity increase malloc failed", 1);
		msh->cmd_table[msh->n_cmds] = init_cmd(msh);
		msh->n_cmds++;
		msh->cmd_table[msh->n_cmds - 1]->heredoc_read = -1;
		parse_single_cmd_tokens(msh->cmd_table[msh->n_cmds - 1], &tokens, msh);
		if (msh->cmd_table[msh->n_cmds - 1]->args[0])
			msh->cmd_table[msh->n_cmds - 1]->pathname = create_cmd_pathname(msh,
					msh->cmd_table[msh->n_cmds - 1]->args[0]);
		msh->cmd_table[msh->n_cmds - 1]->heredoc_read = -1;
	}
}
