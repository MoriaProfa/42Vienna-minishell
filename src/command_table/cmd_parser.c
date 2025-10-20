/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_parser.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 22:10:32 by astehl            #+#    #+#             */
/*   Updated: 2025/10/20 19:30:00 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_builtin	builtin_check(char *s)
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

static t_token	*get_next_token(t_list **tokens)
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

static int	parse_heredoc_token(t_cmd *cmd, t_token *word_token, char *word,
		t_msh *msh)
{
	if (word_token->quoted == Q_NONE)
		cmd->heredocs[cmd->n_hd].expand = 1;
	cmd->heredocs[cmd->n_hd].delimiter = word;
	cmd->heredocs[cmd->n_hd].filename = create_filename(msh->n_cmds, cmd->n_hd);
	if (!cmd->heredocs[cmd->n_hd].filename)
		msh_quit(msh, "failed to create temporary filename", 1);
	if (cmd->heredoc_read == -1)
		close(cmd->heredoc_read);
	cmd->heredoc_read = open(cmd->heredocs[cmd->n_hd].filename, O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (cmd->heredoc_read == -1)
		msh_quit(msh, "failed to create temporary heredoc file", 1);
	if (set_heredoc(msh, cmd, cmd->heredocs[cmd->n_hd]) == 1)
		return (1);
	cmd->n_hd++;
	cmd->last_input = T_HEREDOC;
	return (0);
}

static void	parse_redirection_token(t_cmd *cmd, t_token *token, t_list **tokens,
		t_msh *msh)
{
	t_token	*word_token;
	char	*word;

	word_token = get_next_token(tokens);
	if (!word_token)
		return ;
	if (!word_token->value)
		return (msh_print(msh, "ambiguous redirect", 1));
	word = ft_strdup(word_token->value);
	if (!word)
		msh_quit(msh, "word_token->value ft_strdup malloc failed", 1);
	if (token->type == T_IN_REDIR)
	{
		cmd->redirs[cmd->n_redirs].filename = word;
		cmd->redirs[cmd->n_redirs++].type = T_IN_REDIR;
		cmd->last_input = T_IN_REDIR;
	}
	else if (token->type == T_OUT_REDIR || token->type == T_APPEND_REDIR)
	{
		cmd->redirs[cmd->n_redirs].filename = word;
		cmd->redirs[cmd->n_redirs++].type = token->type;
	}
	else if (token->type == T_HEREDOC)
	{
		if (parse_heredoc_token(cmd, word_token, word, msh) == 1)
			return ;
	}
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
			if (token->value)
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
