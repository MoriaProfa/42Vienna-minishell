/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:28:42 by astehl            #+#    #+#             */
/*   Updated: 2025/09/29 23:33:02 by astehl           ###   ########.fr       */
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
		printf("\targs (%i): ", cmd->n_args);
		j = 0;
		while (cmd->args[j] && j < cmd->n_args)
			printf("[%s] ", cmd->args[j++]);
		printf("\n\tBuiltin args[0]: %d\n", cmd->builtin);
		if (cmd->infiles)
		{
			j = 0;
			printf("\tinfiles (%i): ", cmd->n_inf);
			while (cmd->infiles[j] && j < cmd->n_inf)
				printf("[%s] ", cmd->infiles[j++]);
			printf("\n");
		}
		if (cmd->outfiles)
		{
			j = -1;
			printf("\toutfiles (%i): ", cmd->n_outf);
			while (cmd->outfiles[++j].filename && j < cmd->n_outf)
				printf("[%s (append = %d)] ", cmd->outfiles[j].filename, cmd->outfiles[j].append);
			printf("\n");
		}
		if (cmd->heredocs)
		{
			j = -1;
			printf("\theredocs (%i): ", cmd->n_hd);
			while (cmd->heredocs[++j].delimiter && j < cmd->n_hd)
				printf("[%s (expand = %d)] ", cmd->heredocs[j].delimiter, cmd->heredocs[j].expand);
			printf("\n");
		}
		printf("\tlast_input: %i\n", cmd->last_input);//0 if T_NONE, 3 if T_IN_REDIRECT, 6 if T_HEREDOC_REDIRECT
		if (cmd->pathname)
			printf("\tpathname: %s\n", cmd->pathname);
		i++;
	}
}

void	free_single_cmd_structs_data(t_cmd *cmd)
{
	int	j;

	j = -1;
	while (cmd->outfiles && ++j < cmd->n_outf)
		free(cmd->outfiles[j].filename);
	j = -1;
	while (cmd->heredocs && ++j < cmd->n_hd)
	{
		free(cmd->heredocs[j].delimiter);
		if (cmd->heredocs[j].buffer)
			free(cmd->heredocs[j].buffer);
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
		ft_array_free(cmds[i]->infiles, (size_t)cmds[i]->n_inf);
		free_single_cmd_structs_data(cmds[i]);
		if (cmds[i]->outfiles)
			free(cmds[i]->outfiles);
		if (cmds[i]->heredocs)
			free(cmds[i]->heredocs);
		if (cmds[i]->pathname)
			free(cmds[i]->pathname);
		free(cmds[i]);
	}
	free(cmds);
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
		msh_end(msh, "malloc for cmd failed", 1);
	cmd->capacity_args = 4;
	cmd->args = ft_calloc(cmd->capacity_args, sizeof(char *));
	if (!cmd->args)
		msh_end(msh, "malloc for cmd->args failed", 1);
	cmd->capacity_inf = 4;
	cmd->infiles = ft_calloc(cmd->capacity_inf, sizeof(char *));
	if (!cmd->infiles)
		msh_end(msh, "malloc for cmd->infiles failed", 1);
	cmd->capacity_outf = 4;
	cmd->outfiles = ft_calloc(cmd->capacity_outf, sizeof(t_outfile));
	if (!cmd->outfiles)
		msh_end(msh, "malloc for cmd->outfiles failed", 1);
	cmd->capacity_hd = 4;
	cmd->heredocs = ft_calloc(cmd->capacity_hd, sizeof(t_heredoc));
	if (!cmd->heredocs)
		msh_end(msh, "malloc for cmd->heredocs failed", 1);
	cmd->last_input = T_NONE;
	return (cmd);
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
	if (!ensure_capacity((void **)&cmd->args, &cmd->n_args, 
		&cmd->capacity_args, sizeof(char *)))
		msh_end(msh, "cmd args array capacity increase malloc failed", 1);
	if (!ensure_capacity((void **)&cmd->infiles, &cmd->n_inf, 
		&cmd->capacity_inf, sizeof(char *)))
		msh_end(msh, "cmd infiles array capacity increase malloc failed", 1);
	if (!ensure_capacity((void **)&cmd->outfiles, &cmd->n_outf, 
		&cmd->capacity_outf, sizeof(t_outfile)))
		msh_end(msh, "cmd outfiles array capacity increase malloc failed", 1);
	if (!ensure_capacity((void **)&cmd->heredocs, &cmd->n_hd, 
		&cmd->capacity_hd, sizeof(t_heredoc)))
		msh_end(msh, "cmd heredocs array capacity increase malloc failed", 1);
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
	word = ft_strdup(word_token->value);
	if (!word)
		msh_end(msh, "word_token->value ft_strdup malloc failed", 1);
	if (token->type == T_IN_REDIRECT)
	{
		cmd->infiles[cmd->n_inf++] = word;
		cmd->last_input = T_IN_REDIRECT;
	}
	else if (token->type == T_OUT_REDIRECT)
		cmd->outfiles[cmd->n_outf++].filename = word;
	else if (token->type == T_APPEND_REDIRECT)
	{
		cmd->outfiles[cmd->n_outf].filename = word;
		cmd->outfiles[cmd->n_outf++].append = true;
	}
	else if (token->type == T_HEREDOC_REDIRECT)
		parse_heredoc_token(cmd, word_token, word);
}

void	parse_single_cmd_tokens(t_cmd *cmd, t_list **tokens, t_msh *msh)
{
	t_token			*token;

	while (*tokens && ((t_token *)(*tokens)->content)->type != T_PIPE)
	{
		cmd_arrays_capacity_check(cmd, msh);
		token = (*tokens)->content;
		if (token->type == T_WORD)
		{
			cmd->args[cmd->n_args] = ft_strdup(token->value);
			if (!cmd->args[cmd->n_args++])
				msh_end(msh, "cmd args ft_strdup malloc failed", 1);
		}
		else
			parse_redirection_token(cmd, token, tokens, msh);
		*tokens = (*tokens)->next;
	}
	if ((*tokens) && ((t_token *)(*tokens)->content)->type == T_PIPE)
		*tokens = (*tokens)->next;
	if (cmd->args && cmd->args[0] && builtin_check(cmd->args[0]))
		cmd->builtin = true;
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
			msh_end(msh, "pathname tmp ft_strjoin malloc failed", 1);
		pathname = ft_strjoin(tmp, cmd);
		free(tmp);
		if (!pathname)
			msh_end(msh, "pathname ft_strjoin malloc failed", 1);
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
			msh_end(msh, "pathname ft_strdup malloc failed", 1);
		return (pathname);
	}
	while (msh->envp[j] && ft_strncmp(msh->envp[j], "PATH=", 5) != 0)
		j++;
	if (msh->envp[j])
	{
		paths = ft_split(msh->envp[j] + 5, ':');
		if (!paths)
			msh_end(msh, "paths ft_split malloc failed", 1);
		pathname = resolve_cmd_pathname(msh, paths, cmd);
		ft_array_free(paths, ft_array_size(paths));
	}
	return (pathname);
}

void	build_cmd_table(t_msh *msh)
{
	int		capacity;
	t_cmd	**cmds;
	t_list	*tokens;

	capacity = 4;
	cmds = ft_calloc(capacity, sizeof(t_cmd *));
	if (!cmds)
		msh_end(msh, "cmd table malloc failed", 1);
	tokens = msh->tokens;
	while (tokens)
	{
		if (!ensure_capacity((void **)&cmds, &msh->n_cmds, 
			&capacity, sizeof(t_cmd *)))
			msh_end(msh, "cmd table array capacity increase malloc failed", 1);
		cmds[msh->n_cmds] = init_cmd(msh);
		parse_single_cmd_tokens(cmds[msh->n_cmds], &tokens, msh);
		if (cmds[msh->n_cmds]->args[0])
			cmds[msh->n_cmds]->pathname = create_cmd_pathname(msh, 
				cmds[msh->n_cmds]->args[0]);
		msh->n_cmds++;
	}
	msh->cmd_table = cmds;
}
