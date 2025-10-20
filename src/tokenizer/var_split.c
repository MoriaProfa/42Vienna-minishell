/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_split.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:17:55 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 23:03:05 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	ft_lstinsert_after(t_list *cur_node, t_list *new_node)
{
	if (!cur_node || !new_node)
		return ;
	new_node->next = cur_node->next;
	new_node->prev = cur_node;
	if (cur_node->next)
		cur_node->next->prev = new_node;
	cur_node->next = new_node;
}

static t_list	*delete_current_node(t_list *tokens, t_list *cur_node)
{
	if (cur_node->prev)
		cur_node->prev->next = cur_node->next;
	if (cur_node->next)
		cur_node->next->prev = cur_node->prev;
	if (tokens == cur_node)
		tokens = cur_node->next;
	ft_lstdelone(cur_node, free_token);
	return (tokens);
}

static t_list	*create_new_node(char *value)
{
	t_token	*new_token;
	t_list	*new_node;

	new_token = ft_calloc(1, sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->value = ft_strdup(value);
	if (!new_token->value)
		return (free(new_token), NULL);
	new_token->type = T_WORD;
	new_token->is_expanded = true;
	new_node = ft_lstnew(new_token);
	if (!new_node)
		(free(new_token->value), free(new_token));
	return (new_node);
}

static void	split_and_insert_tokens(t_list **cur_node, t_token *cur_token,
		int i, t_msh *msh)
{
	char	**words;
	t_list	*new_node;

	words = ft_split_with_set(cur_token->value, " \t",
			count_words(cur_token->value, " \t"));
	if (!words)
		return ;
	if (!words[0])
		return (free(words));
	free(cur_token->value);
	cur_token->value = ft_strdup(words[0]);
	if (!cur_token->value)
		(ft_array_free(words, ft_array_size(words)), msh_quit(msh,
				"split and insert token value malloc failed", 1));
	while (words[++i])
	{
		new_node = create_new_node(words[i]);
		if (!new_node)
			(ft_array_free(words, ft_array_size(words)), msh_quit(msh,
					"split and insert token value malloc failed", 1));
		ft_lstinsert_after(*cur_node, new_node);
		*cur_node = new_node;
	}
	ft_array_free(words, ft_array_size(words));
}

void	split_expanded_tokens(t_list *tokens, t_msh *msh)
{
	t_list	*cur_node;
	t_list	*next_node;
	t_token	*cur_token;

	cur_node = tokens;
	while (cur_node)
	{
		cur_token = (t_token *)cur_node->content;
		next_node = cur_node->next;
		if (check_token_validity_for_split(cur_token))
		{
			if (check_for_whitespace_end(cur_token->value, 0))
			{
				correct_adjacency(cur_node, cur_token);
				tokens = delete_current_node(tokens, cur_node);
				cur_node = next_node;
				continue ;
			}
			if (ft_strchr(cur_token->value, ' ') || ft_strchr(cur_token->value,
					'\t'))
				(correct_adjacency(cur_node, cur_token),
					split_and_insert_tokens(&cur_node, cur_token, 0, msh));
		}
		cur_node = cur_node->next;
	}
}
