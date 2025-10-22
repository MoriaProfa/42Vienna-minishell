/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_merger.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 23:07:36 by astehl            #+#    #+#             */
/*   Updated: 2025/10/22 17:39:11 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	check_adjacency(t_list *tokens)
{
	t_list	*last_node;
	t_token	*last_token;

	if (!tokens)
		return (false);
	last_node = ft_lstlast(tokens);
	last_token = (t_token *)last_node->content;
	if (last_token->type == T_WORD)
		return (true);
	return (false);
}

static t_quote_type	merge_quote_types(t_quote_type q1, t_quote_type q2)
{
	if (q1 == q2)
		return (q1);
	if ((q1 == Q_NONE && q2 == Q_SINGLE) || (q1 == Q_SINGLE && q2 == Q_NONE))
		return (Q_SINGLE);
	if ((q1 == Q_NONE && q2 == Q_DOUBLE) || (q1 == Q_DOUBLE && q2 == Q_NONE))
		return (Q_DOUBLE);
	return (Q_MIXED);
}

static void	join_token_content(t_token *previous, t_token *current, t_msh *msh)
{
	char	*joined;

	joined = NULL;
	if (previous->value && current->value)
	{
		joined = ft_strjoin(previous->value, current->value);
		if (!joined)
			msh_quit(msh, "token value merging ft_strjoin malloc failed", 1);
	}
	else if (!previous->value && current->value)
	{
		joined = ft_strdup(current->value);
		if (!joined)
			msh_quit(msh, "token value merging ft_strjoin malloc failed", 1);
	}
	free(previous->value);
	previous->value = joined;
	previous->quoted = merge_quote_types(previous->quoted, current->quoted);
	if (current->is_split)
        previous->is_split = true;
}

void	merge_adjacent_tokens(t_list **tokens, t_msh *msh)
{
	t_list	*cur_node;
	t_token	*cur_token;
	t_token	*prev_token;
	t_list	*prev_node;

	cur_node = *tokens;
	while (cur_node)
	{
		cur_token = (t_token *)cur_node->content;
		if (cur_token->type == T_WORD && cur_token->value
			&& cur_token->is_adjacent && cur_node->prev
			&& ((t_token *)cur_node->prev->content)->type == T_WORD)
		{
			prev_node = cur_node->prev;
			prev_token = (t_token *)prev_node->content;
			join_token_content(prev_token, cur_token, msh);
			prev_node->next = cur_node->next;
			if (cur_node->next)
				cur_node->next->prev = prev_node;
			ft_lstdelone(cur_node, free_token);
			cur_node = prev_node->next;
		}
		else
			cur_node = cur_node->next;
	}
}
