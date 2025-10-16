/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_split.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:17:55 by astehl            #+#    #+#             */
/*   Updated: 2025/10/16 16:39:09 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	check_for_whitespace_end(char *str, int start);
t_token	*create_token(t_msh *msh, char *value, t_token_type type, 
	int *quoted_adjacent);

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

void	ft_lstinsert_after(t_list *cur_node, t_list *new_node)
{
	if (!cur_node || !new_node)
		return ;
	new_node->next = cur_node->next;
	new_node->prev = cur_node;
	if (cur_node->next)
		cur_node->next->prev = new_node;
	cur_node->next = new_node;
}

t_list	*delete_current_node(t_list *tokens, t_list *cur_node)
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

void	split_and_insert_tokens(t_list **cur_node_ptr, t_token *cur_token, 
	t_msh *msh)
{
	char	**words;
	int		quoted_adjacent[2];
	int		i;
	t_list	*new_node;
	t_token	*new_token;

	ft_memset(quoted_adjacent, 0, sizeof(quoted_adjacent));
	i = 0;
	words = ft_split_with_set(cur_token->value, " \t", 
		count_words(cur_token->value, " \t"));
	if (!words || !words[0])
		return ;
	free(cur_token->value);
	cur_token->value = ft_strdup(words[0]);
	if (!cur_token->value)
		msh_quit(msh, "split and insert token value malloc failed", 1);
	while (words[++i])
	{
		new_token = create_token(msh, words[i], T_WORD, quoted_adjacent);//check for malloc protection
		new_token->is_expanded = true;
		new_node = ft_lstnew(new_token);//check for malloc protection
		ft_lstinsert_after(*cur_node_ptr, new_node);
		*cur_node_ptr = new_node;
	}
	ft_array_free(words, ft_array_size(words));
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

void    split_expanded_tokens(t_list *tokens, t_msh *msh)
{
	t_list  *cur_node;
	t_list	*next_node;
	t_token *cur_token;

	cur_node = tokens;
	while (cur_node)
	{
		cur_token = (t_token *)cur_node->content;
		next_node = cur_node->next;
		if (cur_token && cur_token->value && cur_token->value[0] != '\0' && 
			cur_token->quoted == Q_NONE && cur_token->is_expanded)
		{
			if (check_for_whitespace_end(cur_token->value, 0))
			{
				correct_adjacency(cur_node, cur_token);
				tokens = delete_current_node(tokens, cur_node);
				cur_node = next_node;
				continue ;
			}
			if (ft_strchr(cur_token->value, ' ') || 
					ft_strchr(cur_token->value, '\t'))
				(correct_adjacency(cur_node, cur_token), 
					split_and_insert_tokens(&cur_node, cur_token, msh));
		}
		cur_node = cur_node->next;
	}
}
