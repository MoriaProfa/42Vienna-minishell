/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:28:42 by astehl            #+#    #+#             */
/*   Updated: 2025/09/04 21:16:12 by astehl           ###   ########.fr       */
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
		j = -1;
		while (cmd->args[++j].value && j < cmd->argc)
			printf("[Value: %s, Quotes: %d] ", cmd->args[j].value, cmd->args[j].quote);
		printf("\n\tBuiltin args[0].value: %d\n", cmd->builtin);
		if (cmd->infile)
			printf("\tinfile: %s\n", cmd->infile);
		if (cmd->outfile)
			printf("\toutfile: %s (append = %d)\n", cmd->outfile, cmd->append);
		if (cmd->heredoc)
			printf("\theredoc: %s\n", cmd->heredoc);
		if (cmd->pathname)
			printf("\tpathname: %s\n", cmd->pathname);
		i++;
	}
}
void	free_cmd_table_arrays(t_cmd **cmds, int i)
{
	int	j;

	j = -1;
	while (cmds[i]->args[++j].value && j < cmds[i]->argc)
		free(cmds[i]->args[j].value);
	j = -1;
	while (cmds[i]->arg_array[++j] && j < cmds[i]->argc)
		free(cmds[i]->arg_array[j]);
}

void	free_cmd_table(t_cmd **cmds, int n)
{
	int	i;

	i = -1;
	if (!cmds)
		return ;
	while (cmds[++i] && i < n)
	{
		free_cmd_table_arrays(cmds, i);
		if (cmds[i]->arg_array)
			free(cmds[i]->arg_array);
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

void	*grow_array(void *old, int old_capacity, int new_capacity, size_t size)
{
	void	*new;

	new = ft_calloc(new_capacity, size);
	if (!new)
		return (NULL);//maybe msh_end
	ft_memcpy(new, old, old_capacity * size);
	free(old);
	return (new);
}

bool	builtin_check(char *s)
{
	bool	builtin;
	char	*builtin_list[8];
	int		i;
	
	builtin = false;
	builtin_list[0] = "pwd";
	builtin_list[1] = "cd";
	builtin_list[2] = "exit";
	builtin_list[3] = "env";
	builtin_list[4] = "export";
	builtin_list[5] = "unset";
	builtin_list[6] = "echo";
	builtin_list[7] = NULL;
	i = 0;
	while (builtin_list[i])
	{
		if (ft_strlen(s) == ft_strlen(builtin_list[i]))
		{
			if (!ft_strncmp(s, builtin_list[i], ft_strlen(s)))
				return (true);
		}
		i++;
	}
	return (builtin);
}

t_cmd	*init_cmd(void)
{
	t_cmd	*cmd;
	int		i;

	cmd = ft_calloc(1, sizeof(t_cmd));
	if (!cmd)
		return (NULL);//maybe msh_end
	cmd->args_capacity = 4;
	cmd->arg_array = ft_calloc(cmd->args_capacity, sizeof(char *));
	if (!cmd->arg_array)
	{
		free(cmd);
		return (NULL);//maybe msh_end
	}
	cmd->args = ft_calloc(cmd->args_capacity, sizeof(t_cmd_args));
	if (!cmd->args)
	{
		i = -1;
		while (cmd->arg_array[i])
			free(cmd->arg_array[i]);
		free(cmd);
		return (NULL);//maybe msh_end
	}
	cmd->argc = 0;
	cmd->pathname = NULL;
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

void	add_to_cmd_args(t_cmd *cmd, char *word, t_quote_type quote)
{
	int	new_capacity;

	if (cmd->argc + 1 >= cmd->args_capacity)
	{
		new_capacity = cmd->args_capacity * 2;
		cmd->args = grow_array(cmd->args, cmd->args_capacity, new_capacity, 
			sizeof(t_cmd_args));
		cmd->arg_array = grow_array(cmd->args, cmd->args_capacity, new_capacity, 
			sizeof(char *));
		cmd->args_capacity = new_capacity;
	}
	cmd->args[cmd->argc].value = ft_strdup(word);
	cmd->arg_array[cmd->argc] = ft_strdup(word);
	cmd->args[cmd->argc++].quote = quote;
	cmd->args[cmd->argc].value = NULL;//even if it should already be NULL because of using calloc in grow_array
	cmd->arg_array[cmd->argc] = NULL;//even if it should already be NULL because of using calloc in grow_array
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
			add_to_cmd_args(cmd, token->value, token->quoted);
		else if (token->type == T_IN_REDIRECT)
			cmd->infile = get_next_word_token(token_list);
		else if (token->type == T_OUT_REDIRECT)
			cmd->outfile = get_next_word_token(token_list);
		else if (token->type == T_APPEND_REDIRECT)
		{
			cmd->outfile = get_next_word_token(token_list);
			cmd->append = true;
		}
		else if (token->type == T_HEREDOC_REDIRECT)
			cmd->heredoc = get_next_word_token(token_list);
		*token_list = (*token_list)->next;
	}
	if ((*token_list) && ((t_token *)(*token_list)->content)->type == T_PIPE)
		*token_list = (*token_list)->next;
	if (builtin_check(cmd->args[0].value))
		cmd->builtin = true;
}

t_cmd	**ensure_cmd_capacity(t_cmd **cmds, int *capacity, int count)
{
	int	new_capacity;

	if (count < *capacity)
		return (cmds);
	new_capacity = (*capacity) * 2;
	cmds = grow_array(cmds, *capacity, new_capacity, sizeof(t_cmd *));
	*capacity = new_capacity;
	return (cmds);
}

void	build_cmd_table(t_msh *msh)
{
	int		capacity;
	int		count;
	t_cmd	**cmds;
	t_list	*token_list;

	capacity = 4;
	count = 0;
	cmds = ft_calloc(capacity, sizeof(t_cmd *));
	if (!cmds)
		msh_end(msh, "cmd table malloc failed");
	token_list = msh->tokens;
	while (token_list)
	{
		cmds = ensure_cmd_capacity(cmds, &capacity, count);
		if (!add_cmd(cmds, count, msh))
			return ;//maybe msh_end?
		parse_single_cmd_tokens(cmds[count], &token_list);
		cmds[count]->pathname = create_cmd_pathname(msh, cmds[count]->args[0].value);
		count++;
	}
	cmds = ensure_cmd_capacity(cmds, &capacity, count);
	cmds[count] = NULL;
	msh->cmd_table = cmds;
	msh->n_cmds = count;
}
