/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:28:42 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 21:31:48 by astehl           ###   ########.fr       */
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
		// 0 if T_NONE, 3 if T_IN_REDIRECT, 6 if T_HEREDOC_REDIRECT
		if (cmd->pathname)
			printf("\tpathname: %s\n", cmd->pathname);
		i++;
	}
}

void	free_single_cmd_structs_data(t_cmd *cmd)
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

t_builtin	builtin_check(char *s)
{
	t_builtin	builtin;
	char		*builtin_list[8];
	int			i;

	builtin.builtin = false;
	builtin.type = B_NOT_BUILTIN;
	builtin_list[0] = "pwd";
	builtin_list[1] = "cd";
	builtin_list[2] = "exit";
	builtin_list[3] = "env";
	builtin_list[4] = "export";
	builtin_list[5] = "unset";
	builtin_list[6] = "echo";
	builtin_list[7] = NULL;
	i = -1;
	while (builtin_list[++i])
	{
		if (ft_strlen(s) == ft_strlen(builtin_list[i]) && !ft_strncmp(s,
				builtin_list[i], ft_strlen(s)))
		{
			builtin.builtin = true;
			builtin.type = i + 1;
		}
	}
	return (builtin);
}

void	*grow_array(void *old_array, int old_capacity, int new_capacity,
		size_t size)
{
	void	*new_array;

	new_array = ft_calloc(new_capacity, size);
	if (!new_array)
		return (NULL);
	ft_memcpy(new_array, old_array, old_capacity * size);
	free(old_array);
	return (new_array);
}

bool	ensure_capacity(void **array, int *count, int *capacity, size_t size)
{
	int		new_capacity;
	void	*new_array;

	if (*count + 1 >= *capacity)
	{
		new_capacity = (*capacity) * 2;
		new_array = grow_array(*array, *capacity, new_capacity, size);
		if (!new_array)
			return (false);
		*array = new_array;
		*capacity = new_capacity;
	}
	return (true);
}

void	cmd_arrays_capacity_check(t_cmd *cmd, t_msh *msh)
{
	if (!ensure_capacity((void **)&cmd->args, &cmd->n_args, &cmd->capacity_args,
			sizeof(char *)))
		msh_quit(msh, "cmd args array capacity increase malloc failed", 1);
	if (!ensure_capacity((void **)&cmd->redirs, &cmd->n_redirs,
			&cmd->capacity_redirs, sizeof(t_redir)))
		msh_quit(msh, "cmd redirs array capacity increase malloc failed", 1);
	if (!ensure_capacity((void **)&cmd->heredocs, &cmd->n_hd, &cmd->capacity_hd,
			sizeof(t_heredoc)))
		msh_quit(msh, "cmd heredocs array capacity increase malloc failed", 1);
}

t_token	*get_next_token(t_list **tokens)
{
	t_token	*token;

	if ((*tokens)->next
		&& ((t_token *)(*tokens)->next->content)->type == T_WORD)
	{
		*tokens = (*tokens)->next;
		token = (t_token *)(*tokens)->content;
		return (token);
	}
	return (NULL);
}

void	parse_heredoc_token(t_cmd *cmd, t_token *word_token, char *word)
{
	if (word_token->quoted == Q_NONE)
		cmd->heredocs[cmd->n_hd].expand = 1;
	cmd->heredocs[cmd->n_hd++].delimiter = word;
	cmd->last_input = T_HEREDOC_REDIRECT;
}

void	parse_redirection_token(t_cmd *cmd, t_token *token, t_list **tokens,
		t_msh *msh)
{
	t_token	*word_token;
	char	*word;

	word_token = get_next_token(tokens);
	if (!word_token)
		return ;
	if (!word_token->value)
		return (msh_print(msh, "minishell: ambiguous redirect", 1));
	word = ft_strdup(word_token->value);
	if (!word)
		msh_quit(msh, "word_token->value ft_strdup malloc failed", 1);
	if (token->type == T_IN_REDIRECT)
	{
		cmd->redirs[cmd->n_redirs].filename = word;
		cmd->redirs[cmd->n_redirs++].type = T_IN_REDIRECT;
		cmd->last_input = T_IN_REDIRECT;
	}
	else if (token->type == T_OUT_REDIRECT || token->type == T_APPEND_REDIRECT)
	{
		cmd->redirs[cmd->n_redirs].filename = word;
		cmd->redirs[cmd->n_redirs++].type = token->type;
	}
	else if (token->type == T_HEREDOC_REDIRECT)
		parse_heredoc_token(cmd, word_token, word);
}

void	parse_single_cmd_tokens(t_cmd *cmd, t_list **tokens, t_msh *msh)
{
	t_token	*token;

	while (*tokens && ((t_token *)(*tokens)->content)->type != T_PIPE)
	{
		cmd_arrays_capacity_check(cmd, msh);
		token = (*tokens)->content;
		if (token->type == T_WORD)
		{
			if (token->value/* && token->value[0] != '\0'*/)
			{
				cmd->args[cmd->n_args] = ft_strdup(token->value);
				if (!cmd->args[cmd->n_args++])
					msh_quit(msh, "cmd args ft_strdup malloc failed", 1);
			}
		}
		else
			parse_redirection_token(cmd, token, tokens, msh);
		*tokens = (*tokens)->next;
	}
	if ((*tokens) && ((t_token *)(*tokens)->content)->type == T_PIPE)
		*tokens = (*tokens)->next;
	if (cmd->args && cmd->args[0])
		cmd->builtin = builtin_check(cmd->args[0]);
}

t_cmd	*init_cmd_data(t_msh *msh, t_cmd *cmd)
{
	cmd->capacity_args = 4;
	cmd->args = ft_calloc(cmd->capacity_args, sizeof(char *));
	if (!cmd->args)
		(free(cmd), msh_quit(msh, "malloc for cmd->args failed", 1));
	cmd->capacity_redirs = 4;
	cmd->redirs = ft_calloc(cmd->capacity_redirs, sizeof(t_redir));
	if (!cmd->redirs)
		(free(cmd->args), free(cmd), 
			msh_quit(msh, "malloc for cmd->redirs failed", 1));
	cmd->capacity_hd = 4;
	cmd->heredocs = ft_calloc(cmd->capacity_hd, sizeof(t_heredoc));
	if (!cmd->heredocs)
		(free(cmd->redirs), free(cmd->args), free(cmd), 
			msh_quit(msh, "malloc for cmd->heredocs failed", 1));
	cmd->last_input = T_NONE;//should be set to that in the beginning already?
	return (cmd);
}

t_cmd	*init_cmd(t_msh *msh)
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

