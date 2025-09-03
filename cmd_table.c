/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:28:42 by astehl            #+#    #+#             */
/*   Updated: 2025/09/03 15:18:37 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*create_cmd_pathname(t_msh *msh, char *cmd);

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
		printf("\targs: ");
		j = 0;
		while (cmd->args[j] && j < cmd->argc)
			printf("[%s] ", cmd->args[j++]);
		printf("\n");
		if (cmd->infile)
			printf("\tinfile: %s\n", cmd->infile);
		if (cmd->outfile)
			printf("\toutfile: %s (append = %i)\n", cmd->outfile, cmd->append);
		if (cmd->heredoc)
			printf("\theredoc: %s\n", cmd->heredoc);
		if (cmd->pathname)
			printf("\tpathname: %s\n", cmd->pathname);
		printf("\tfd[0] = %d\n\tfd[1] = %d\n", cmd->fd[0], cmd->fd[1]);
		i++;
	}
}

void	free_cmd_table(t_cmd **cmds, int n)
{
	int	i;
	int	j;

	i = -1;
	if (!cmds)
		return ;
	while (cmds[++i] && i < n)
	{
		j = -1;
		while (cmds[i]->args[++j] && j < cmds[i]->argc)
			free(cmds[i]->args[j]);
		if (cmds[i]->args)
			free(cmds[i]->args);
		if (cmds[i]->infile)
			free(cmds[i]->infile);
		if (cmds[i]->outfile)
			free(cmds[i]->outfile);
		if (cmds[i]->heredoc)
			free(cmds[i]->heredoc);
		if (cmds[i]->pathname)
			free(cmds[i]->pathname);
		free(cmds[i]);
	}
	free(cmds);
}

void	*grow_array(void *old, int old_cap, int new_cap, size_t size)
{
	void	*new;

	new = ft_calloc(new_cap, size);
	if (!new)
		return (NULL);//maybe msh_end
	ft_memcpy(new, old, old_cap * size);
	free(old);
	return (new);
}

t_cmd	*init_cmd(void)
{
	t_cmd	*cmd;

	cmd = ft_calloc(1, sizeof(t_cmd));
	if (!cmd)
		return (NULL);//maybe msh_end
	cmd->args_cap = 4;
	cmd->args = ft_calloc(cmd->args_cap, sizeof(char *));
	if (!cmd->args)
	{
		free(cmd);
		return (NULL);//maybe msh_end
	}
	cmd->argc = 0;
	cmd->pathname = NULL;
	cmd->fd[0] = -1;
	cmd->fd[1] = -1;
	cmd->fd_in = -1;
	cmd->fd_out = -1;
	return (cmd);
}

int	add_cmd(t_cmd **cmds, int count, t_msh *msh)
{
	cmds[count] = init_cmd();
	if (!cmds[count])
	{
		free_cmd_table(msh->cmd_table, count);
		perror("cmd calloc");
		return (0);//maybe msh_end
	}
	return (1);
}

void	add_to_cmd_args(t_cmd *cmd, char *word)
{
	int	new_cap;

	if (cmd->argc + 1 >= cmd->args_cap)
	{
		new_cap = cmd->args_cap * 2;
		cmd->args = grow_array(cmd->args, cmd->args_cap, new_cap, 
			sizeof(char *));
		cmd->args_cap = new_cap;
	}
	cmd->args[cmd->argc++] = ft_strdup(word);
	cmd->args[cmd->argc] = NULL;
}

char	*get_next_word_token(t_list **tokens)
{
	char	*value;
	
	if ((*tokens)->next
		&& ((t_token *)(*tokens)->next->content)->type == T_WORD)
	{
		*tokens = (*tokens)->next;
		value = ft_strdup(((t_token *)(*tokens)->content)->value); //malloc check missing... if !value = msh_end?
		return (value);
	}
	return (NULL);
}

void	parse_single_cmd_tokens(t_cmd *cmd, t_list **token_list)
{
	t_token	*token;

	while (*token_list && ((t_token *)(*token_list)->content)->type != T_PIPE)
	{
		token = (*token_list)->content;
		if (token->type == T_WORD)
			add_to_cmd_args(cmd, token->value);
		else if (token->type == T_IN_REDIRECT)
			cmd->infile = get_next_word_token(token_list);
		else if (token->type == T_OUT_REDIRECT)
			cmd->outfile = get_next_word_token(token_list);
		else if (token->type == T_APPEND_REDIRECT)
		{
			cmd->outfile = get_next_word_token(token_list);
			cmd->append = 1;
		}
		else if (token->type == T_HEREDOC_REDIRECT)
			cmd->heredoc = get_next_word_token(token_list);
		*token_list = (*token_list)->next;
	}
	if ((*token_list) && ((t_token *)(*token_list)->content)->type == T_PIPE)
		*token_list = (*token_list)->next;
}

t_cmd	**ensure_cmd_capacity(t_cmd **cmds, int *cap, int count)
{
	int	new_cap;

	if (count < *cap)
		return (cmds);
	new_cap = (*cap) * 2;
	cmds = grow_array(cmds, *cap, new_cap, sizeof(t_cmd *));
	*cap = new_cap;
	return (cmds);
}

void	build_cmd_table(t_msh *msh)
{
	int		cap;
	int		count;
	t_cmd	**cmds;
	t_list	*token_list;

	cap = 4;
	count = 0;
	cmds = ft_calloc(cap, sizeof(t_cmd *));
	if (!cmds)
		msh_end(msh, "cmd table malloc failed");
	token_list = msh->tokens;
	while (token_list)
	{
		cmds = ensure_cmd_capacity(cmds, &cap, count);
		if (!add_cmd(cmds, count, msh))
			return ;//maybe msh_end?
		parse_single_cmd_tokens(cmds[count], &token_list);
		cmds[count]->pathname = create_cmd_pathname(msh, cmds[count]->args[0]);
		count++;
	}
	cmds = ensure_cmd_capacity(cmds, &cap, count);
	cmds[count] = NULL;
	msh->cmd_table = cmds;
	msh->n_cmds = count;
}
