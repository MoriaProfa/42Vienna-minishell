/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:28:42 by astehl            #+#    #+#             */
/*   Updated: 2025/09/23 06:55:55 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		printf("\n\tBuiltin args[0]: %d\n", cmd->builtin);
		if (cmd->infiles)
		{
			j = 0;
			printf("\tinfiles: ");
			while (cmd->infiles[j] && j < cmd->n_inf)
				printf("[%s] ", cmd->infiles[j++]);
			printf("\n");
		}
		if (cmd->outfiles)
		{
			j = -1;
			printf("\toutfiles: ");
			while (cmd->outfiles[++j].filename && j < cmd->n_outf)
				printf("[%s (append = %d)] ", cmd->outfiles[j].filename, cmd->outfiles[j].append);
			printf("\n");
		}
		if (cmd->heredocs)
		{
			j = 0;
			printf("\theredocs: ");
			while (cmd->heredocs[j].delimiter && j < cmd->n_hd)
				printf("[delimiter: %s] ", cmd->heredocs[j++].delimiter);
			printf("\n");
		}		
		if (cmd->pathname)
			printf("\tpathname: %s\n", cmd->pathname);
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
		ft_array_free(cmds[i]->args, (size_t)cmds[i]->argc);
		ft_array_free(cmds[i]->infiles, (size_t)cmds[i]->n_inf);
		j = -1;
		while (cmds[i]->outfiles && ++j < cmds[i]->n_outf)
			free(cmds[i]->outfiles[j].filename);
		if (cmds[i]->outfiles)
			free(cmds[i]->outfiles);
		j = -1;
		while (cmds[i]->heredocs && ++j < cmds[i]->n_hd)
		{
			free(cmds[i]->heredocs[j].delimiter);
			if (cmds[i]->heredocs[j].buffer)
				free(cmds[i]->heredocs[j].buffer);
		}
		if (cmds[i]->heredocs)
			free(cmds[i]->heredocs);
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
		return (NULL);//maybe msh_end -> not possible, cannot add msh to parameters... norm ftw?
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

t_cmd	*init_cmd(t_msh *msh)
{
	t_cmd	*cmd;

	cmd = ft_calloc(1, sizeof(t_cmd)); //all counts get set to 0 this way, pathname to NULL, builtin to false
	if (!cmd)
		msh_end(msh, "malloc for cmd failed");
	cmd->args_capacity = 4;
	cmd->args = ft_calloc(cmd->args_capacity, sizeof(char *));
	if (!cmd->args)
		msh_end(msh, "malloc for cmd->args failed");
	cmd->capacity_inf = 4;
	cmd->infiles = ft_calloc(cmd->capacity_inf, sizeof(char *));
	if (!cmd->infiles)
		msh_end(msh, "malloc for cmd->infiles failed");
	cmd->capacity_outf = 4;
	cmd->outfiles = ft_calloc(cmd->capacity_outf, sizeof(t_outfile));
	if (!cmd->outfiles)
		msh_end(msh, "malloc for cmd->outfiles failed");
	cmd->capacity_hd = 4;
	cmd->heredocs = ft_calloc(cmd->capacity_hd, sizeof(t_heredoc));
	if (!cmd->heredocs)
		msh_end(msh, "malloc for cmd->heredocs failed");
	return (cmd);
}

int	add_cmd(t_cmd **cmds, int count, t_msh *msh)
{
	cmds[count] = init_cmd(msh);
	if (!cmds[count])
	{
		free_cmd_table(msh->cmd_table, count);
		perror("single cmd malloc failed");
		return (0);//maybe msh_end
	}
	return (1);
}

void	add_to_cmd_args(t_cmd *cmd, char *word)
{
	int	new_capacity;

	if (cmd->argc + 1 >= cmd->args_capacity)
	{
		new_capacity = cmd->args_capacity * 2;
		cmd->args = grow_array(cmd->args, cmd->args_capacity, new_capacity, 
			sizeof(char *));
		cmd->args_capacity = new_capacity;
	}
	cmd->args[cmd->argc++] = ft_strdup(word);
	cmd->args[cmd->argc] = NULL;
}

char	*get_next_word(t_list **tokens)
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
	t_token			*token;

	while (*token_list && ((t_token *)(*token_list)->content)->type != T_PIPE)
	{
		token = (*token_list)->content;
		if (token->type == T_WORD)
			add_to_cmd_args(cmd, token->value);
		else if (token->type == T_IN_REDIRECT)
			cmd->infiles[cmd->n_inf++] = get_next_word(token_list);
		else if (token->type == T_OUT_REDIRECT)
			cmd->outfiles[cmd->n_outf++].filename = get_next_word(token_list);
		else if (token->type == T_APPEND_REDIRECT)
		{
			cmd->outfiles[cmd->n_outf].filename = get_next_word(token_list);
			cmd->outfiles[cmd->n_outf++].append = true;
		}
		else if (token->type == T_HEREDOC_REDIRECT)
			cmd->heredocs[cmd->n_hd++].delimiter = get_next_word(token_list);
		*token_list = (*token_list)->next;
	}
	if ((*token_list) && ((t_token *)(*token_list)->content)->type == T_PIPE)
		*token_list = (*token_list)->next;
	if (cmd->args && cmd->args[0] && builtin_check(cmd->args[0]))
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

char	*resolve_cmd_pathname(t_msh *msh, char **paths, char *cmd)
{
	char	*pathname;
	char	*tmp;
	int		j;

	j = -1;
	pathname = NULL;
	while (paths[++j])
	{
		tmp = ft_strjoin(paths[j], "/");
		if (!tmp)
			msh_end(msh, "pathname tmp ft_strjoin failed");
		pathname = ft_strjoin(tmp, cmd);
		free(tmp);
		if (!pathname)
			msh_end(msh, "pathname ft_strjoin failed");
		if (access(pathname, F_OK) == 0)
			return (pathname);
		free(pathname);
	}
	return (NULL);
}

char	*create_cmd_pathname(t_msh *msh, char *cmd)
{
	char	**paths;
	char	*pathname;
	int		j;

	j = 0;
	pathname = NULL;
	if (access(cmd, F_OK) == 0)
	{
		pathname = ft_strdup(cmd);
		if (!pathname)
			msh_end(msh, "pathname malloc failed");
		return (pathname);
	}
	while (msh->envp[j] && ft_strncmp(msh->envp[j], "PATH=", 5) != 0)
		j++;
	if (msh->envp[j])
	{
		paths = ft_split(msh->envp[j] + 5, ':');
		if (!paths)
			msh_end(msh, "paths ft_split failed");
		pathname = resolve_cmd_pathname(msh, paths, cmd);
		ft_array_free(paths, ft_array_size(paths));
	}
	return (pathname);
}

void	build_cmd_table(t_msh *msh)
{
	int		capacity;
	int		count;
	t_cmd	**cmds;
	t_list	*token_list;

	capacity = 4;
	count = 0; //use msh->n_cmds directly instead?
	cmds = ft_calloc(capacity, sizeof(t_cmd *));
	if (!cmds)
		msh_end(msh, "cmd table malloc failed");
	token_list = msh->tokens;
	while (token_list)
	{
		cmds = ensure_cmd_capacity(cmds, &capacity, count);
		if (!add_cmd(cmds, count, msh))//add_cmd could be reduced to 3 lines - maybe adding it here instead
			return ;//maybe msh_end?
		parse_single_cmd_tokens(cmds[count], &token_list);
		cmds[count]->pathname = create_cmd_pathname(msh, cmds[count]->args[0]);//cmds[count]->args[0]->value -->array of struct containing value = string + information about quotationmarks
		count++;
	}
	cmds = ensure_cmd_capacity(cmds, &capacity, count);
	cmds[count] = NULL;
	msh->cmd_table = cmds;
	msh->n_cmds = count;
}
