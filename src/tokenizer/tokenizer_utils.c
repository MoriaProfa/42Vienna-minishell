/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 23:12:03 by astehl            #+#    #+#             */
/*   Updated: 2025/10/18 15:39:51 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static t_token	*create_token(t_msh *msh, char *value, t_token_type type,
		int *quoted_adjacent)
{
	t_token	*token;

	token = ft_calloc(1, sizeof(t_token));
	if (!token)
	{
		if (type == T_WORD)
			free(value);
		msh_quit(msh, "tokenizer: create token malloc failed", 1);
	}
	token->value = ft_strdup(value);
	if (!token->value)
	{
		if (type == T_WORD)
			free(value);
		free(token);
		msh_quit(msh, "tokenizer: create token->value malloc failed", 1);
	}
	token->type = type;
	token->quoted = quoted_adjacent[0];
	token->is_adjacent = quoted_adjacent[1];
	return (token);
}

void	fill_token_list(t_msh *msh, char *value, t_token_type type,
		int *quoted_adjacent)
{
	t_list	*cur_node;
	t_token	*token;

	token = create_token(msh, value, type, quoted_adjacent);
	cur_node = ft_lstnew(token);
	if (!cur_node)
	{
		if (type == T_WORD)
			free(value);
		free(token->value);
		free(token);
		msh_quit(msh, "tokenizer: node malloc failed", 1);
	}
	ft_lstadd_back(&msh->tokens, cur_node);
}

char	*create_word_token(t_msh *msh)
{
	char	*word;
	char	*start;
	int		len;

	len = 0;
	if (*msh->cpy_in_str == '"' || *msh->cpy_in_str == '\'')
		start = handle_quoted(msh, &len);
	else
		start = handle_unquoted(msh, &len);
	word = ft_calloc(len + 1, sizeof(char));
	if (!word)
		msh_quit(msh, "tokenizer: word malloc failed", 1);
	ft_memcpy(word, start, len);
	word[len] = '\0';
	return (word);
}

static void	remove_quotationmarks(char *str)
{
	size_t	i;
	char	quotationmark;
	char	*start;
	char	*end;

	i = 0;
	if (!str)
		return ;
	while (str[i] && str[i] != '\'' && str[i] != '"')
		i++;
	if (i >= ft_strlen(str))
		return ;
	quotationmark = str[i];
	start = ft_strchr(str, quotationmark);
	if (!start)
		return ;
	end = ft_strchr(start + 1, quotationmark);
	if (!end)
		return ;
	ft_memmove(start, start + 1, end - start - 1);
	ft_memmove(start + (end - start - 1), end + 1, ft_strlen(end + 1) + 1);
}

void	remove_quotationmarks_in_tokens(t_list *tokens)
{
	t_list	*cur_node;
	t_token	*cur_token;

	cur_node = tokens;
	while (cur_node)
	{
		cur_token = (t_token *)(cur_node->content);
		if (cur_token->type == T_WORD)
			remove_quotationmarks(cur_token->value);
		cur_node = cur_node->next;
	}
}
