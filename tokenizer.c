/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:25:30 by astehl            #+#    #+#             */
/*   Updated: 2025/10/15 01:15:11 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void			expand_word_tokens(t_list *tokens, t_msh *msh);

// just for testing purposes
void	print_tokens(t_list *tokens)
{
	t_list	*cur_node;
	t_token	*cur_token;
	int		i;

	cur_node = tokens;
	i = 1;
	printf("\nToken list:\n");
	while (cur_node)
	{
		cur_token = (t_token *)(cur_node->content);
		if (cur_token)
			printf("Token #%i:\n\tValue: %s, Type: %d, Quotes: %d, is adjacent: %d\n", i, cur_token->value, cur_token->type, cur_token->quoted,
				cur_token->is_adjacent);
		cur_node = cur_node->next;
		i++;
	}
}

void	free_token(void *content)
{
	t_token	*cur_token;

	cur_token = (t_token *)content;
	if (!cur_token)
		return ;
	if (cur_token->value)
		free(cur_token->value);
	free(cur_token);
}

t_token	*create_token(t_msh *msh, char *value, t_token_type type, 
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

int	handle_single_metacharacter(t_msh *msh, char c, t_token_type type)
{
	char	str[2];
	int		quoted_adjacent[2];

	ft_memset(quoted_adjacent, 0, sizeof(quoted_adjacent));
	if (*msh->cpy_in_str == c)
	{
		str[0] = c;
		str[1] = '\0';
		fill_token_list(msh, str, type, quoted_adjacent);
		msh->cpy_in_str++;
		return (1);
	}
	return (0);
}

int	handle_metacharacters(t_msh *msh)
{
	int	quoted_adjacent[2];

	ft_memset(quoted_adjacent, 0, sizeof(quoted_adjacent));
	if (*msh->cpy_in_str == '<' && *(msh->cpy_in_str + 1) == '<')
	{
		fill_token_list(msh, "<<", T_HEREDOC_REDIRECT, quoted_adjacent);
		msh->cpy_in_str += 2;
		return (1);
	}
	if (*msh->cpy_in_str == '>' && *(msh->cpy_in_str + 1) == '>')
	{
		fill_token_list(msh, ">>", T_APPEND_REDIRECT, quoted_adjacent);
		msh->cpy_in_str += 2;
		return (1);
	}
	if (handle_single_metacharacter(msh, '<', T_IN_REDIRECT))
		return (1);
	if (handle_single_metacharacter(msh, '>', T_OUT_REDIRECT))
		return (1);
	if (handle_single_metacharacter(msh, '|', T_PIPE))
		return (1);
	return (0);
}

bool	skip_whitespaces(t_msh *msh)
{
	bool	skip;

	skip = false;
	while (*msh->cpy_in_str == ' ' || *msh->cpy_in_str == '\t')
	{
		msh->cpy_in_str++;
		skip = true;
	}
	return (skip);
}

char	*handle_quoted(t_msh *msh, int *len)
{
	char	*start;
	char	quotationmark;

	quotationmark = *msh->cpy_in_str;
	start = msh->cpy_in_str;
	msh->cpy_in_str++;
	*len = 1;
	while (*msh->cpy_in_str && *msh->cpy_in_str != quotationmark)
	{
		msh->cpy_in_str++;
		(*len)++;
	}
	if (*msh->cpy_in_str == quotationmark)
	{
		msh->cpy_in_str++;
		(*len)++;
	}
	return (start);
}

char	*handle_unquoted(t_msh *msh, int *len)
{
	char	*start;

	start = msh->cpy_in_str;
	*len = 0;
	while (*msh->cpy_in_str && !ft_strchr(" \t<>|\"\'", *msh->cpy_in_str))
	{
		msh->cpy_in_str++;
		(*len)++;
	}
	return (start);
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

int	set_quoted(char c)
{
	int	quoted;

	quoted = Q_NONE;
	if (c == '\'')
		quoted = Q_SINGLE;
	else if (c == '"')
		quoted = Q_DOUBLE;
	return (quoted);
}

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

void	remove_quotationmarks(char *str)
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

t_quote_type	merge_quote_types(t_quote_type q1, t_quote_type q2)
{
	if (q1 == q2)
		return (q1);
	if ((q1 == Q_NONE && q2 == Q_SINGLE) || (q1 == Q_SINGLE && q2 == Q_NONE))
		return (Q_SINGLE);
	if ((q1 == Q_NONE && q2 == Q_DOUBLE) || (q1 == Q_DOUBLE && q2 == Q_NONE))
		return (Q_DOUBLE);
	return (Q_MIXED);
}

void	join_token_content(t_token *previous, t_token *current, t_msh *msh)
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
		if (cur_token->type == T_WORD && cur_token->is_adjacent
			&& cur_node->prev
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

void	msh_tokenizer(t_msh *msh)
{
	char	*word;
	int		quoted_adjacent[2];
	int		i;
	int		skipped;

	i = -1;
	while (*msh->cpy_in_str)
	{
		ft_memset(quoted_adjacent, 0, sizeof(quoted_adjacent));
		skipped = skip_whitespaces(msh);
		if (++i != 0 && !skipped)
			quoted_adjacent[1] = check_adjacency(msh->tokens);
		if (!*msh->cpy_in_str)
			break ;
		if (handle_metacharacters(msh))
			continue ;
		else
		{
			quoted_adjacent[0] = set_quoted(*msh->cpy_in_str);
			word = create_word_token(msh);
			fill_token_list(msh, word, T_WORD, quoted_adjacent);
			free(word);
		}
	}
//	printf("\nraw tokens:");
//	print_tokens(msh->tokens);
	remove_quotationmarks_in_tokens(msh->tokens);
//	printf("\nafter quote removal:");
//	print_tokens(msh->tokens);
	expand_word_tokens(msh->tokens, msh);
//	printf("\nafter expansion:");
//	print_tokens(msh->tokens);
	merge_adjacent_tokens(&msh->tokens, msh);
//	printf("\nafter adjacency handling:");
}
