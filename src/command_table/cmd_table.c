/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:28:42 by astehl            #+#    #+#             */
/*   Updated: 2025/10/18 15:10:00 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

// just for testing purposes
void	print_cmd_table(t_msh *msh)
{
	int		i;
	t_cmd	*cmd;
	int		j;

	i = 0;
	if (!msh->cmd_table)
		return ;
	printf("\n=== Command Table (n = %i) ===\n", msh->n_cmds);
	while (msh->cmd_table[i] && i < msh->n_cmds)
	{
		cmd = msh->cmd_table[i];
		printf("Command %i:\n", i);
		printf("\targs (%i): ", cmd->n_args);
		j = 0;
		while (cmd->args[j] && j < cmd->n_args)
			printf("[%s] ", cmd->args[j++]);
		printf("\n\tBuiltin args[0]: %d\n", cmd->builtin.builtin);
		if (cmd->redirs)
		{
			j = -1;
			printf("\tredirs (%i): ", cmd->n_redirs);
			while (cmd->redirs[++j].filename && j < cmd->n_redirs)
				printf("[%s (type = %d)] ", cmd->redirs[j].filename,
					cmd->redirs[j].type);
			printf("\n");
		}
		if (cmd->heredocs)
		{
			j = -1;
			printf("\theredocs (%i): ", cmd->n_hd);
			while (cmd->heredocs[++j].delimiter && j < cmd->n_hd)
				printf("[%s (expand = %d)] ", cmd->heredocs[j].delimiter,
					cmd->heredocs[j].expand);
			printf("\n");
		}
		printf("\tlast_input: %i\n", cmd->last_input);
		// 0 if T_NONE, 3 if T_IN_REDIR, 6 if T_HEREDOC
		if (cmd->pathname)
			printf("\tpathname: %s\n", cmd->pathname);
		i++;
	}
}

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
	cmd->last_input = T_NONE; // should be set to that in the beginning already?
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
		parse_single_cmd_tokens(msh->cmd_table[msh->n_cmds - 1], &tokens, msh);
		if (msh->cmd_table[msh->n_cmds - 1]->args[0])
			msh->cmd_table[msh->n_cmds - 1]->pathname = create_cmd_pathname(msh,
					msh->cmd_table[msh->n_cmds - 1]->args[0]);
		msh->cmd_table[msh->n_cmds - 1]->heredoc_read = -1;
	}
}
