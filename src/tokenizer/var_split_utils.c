/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_split_utils.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 22:58:38 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 23:03:18 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

ssize_t	count_words(const char *s, const char *set)
{
	ssize_t	word_count;
	ssize_t	i;

	word_count = 0;
	i = 0;
	while (s[i])
	{
		while (s[i] && ft_strchr(set, s[i]))
			i++;
		if (s[i])
			word_count++;
		while (s[i] && !ft_strchr(set, s[i]))
			i++;
	}
	return (word_count);
}

char	**ft_split_with_set(const char *s, const char *set, ssize_t word_count)
{
	char	**array;
	size_t	start;
	size_t	end;
	ssize_t	i;

	if (!s || !set)
		return (NULL);
	start = 0;
	i = -1;
	array = ft_calloc(word_count + 1, sizeof(char *));
	if (!array)
		return (NULL);
	while (s[start] && ++i < word_count)
	{
		while (s[start] && ft_strchr(set, s[start]))
			start++;
		end = start;
		while (s[end] && !ft_strchr(set, s[end]))
			end++;
		array[i] = ft_substr(s, start, end - start);
		if (!array[i])
			return (ft_array_free(array, i), NULL);
		start = end;
	}
	return (array);
}

void	correct_adjacency(t_list *cur_node, t_token *cur_token)
{
	t_list	*next_node;
	t_token	*next_token;

	if (ft_strchr(" \t", cur_token->value[0]))
		cur_token->is_adjacent = false;
	next_node = cur_node->next;
	if (!next_node)
		return ;
	next_token = (t_token *)next_node->content;
	if (ft_strchr(" \t", cur_token->value[ft_strlen(cur_token->value) - 1]))
		next_token->is_adjacent = false;
}

bool	check_token_validity_for_split(t_token *cur_token)
{
	if (cur_token && cur_token->value && cur_token->value[0] != '\0'
		&& cur_token->quoted == Q_NONE && cur_token->is_expanded)
		return (true);
	return (false);
}
