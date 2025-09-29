/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:25:30 by astehl            #+#    #+#             */
/*   Updated: 2025/09/29 23:52:21 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	merge_adjacent_tokens(t_list **tokens, t_msh *msh);
void    remove_quotationmarks(char *str);

// just for testing purposes
void	print_tokens(t_list *tokens)
{
	t_list	*current;
	int		i;
	t_token	*token;

	current = tokens;
	i = 1;
	printf("\nToken list:\n");
	while (current)
	{
		token = (t_token *)(current->content);
		if (token)
			printf("Token #%i:\n\tValue: %s, Type: %d, Quotes: %d, is adjacent: %d\n", i, token->value,
				token->type, token->quoted, token->is_adjacent);
		current = current->next;
		i++;
	}
}

void	free_token(void *content)
{
	t_token	*token;

	token = (t_token *)content;
	free(token->value);
	free(token);
}

void	fill_token_list(t_msh *msh, const char *value, t_token_type type, 
	int *quoted_adjacent)
{
	t_token	*token;
	t_list	*node;

	token = ft_calloc(1, sizeof(t_token));
	if (!token)
		msh_end(msh, "tokenizer: token malloc failed", 1);
	token->value = ft_strdup(value);
	if (!token->value)
		msh_end(msh, "tokenizer: value malloc failed", 1);
	token->type = type;
	token->quoted = quoted_adjacent[0];
	token->is_adjacent = quoted_adjacent[1];
	node = ft_lstnew(token);
	if (!node)
		msh_end(msh, "tokenizer: node malloc failed", 1);
	ft_lstadd_back(&msh->tokens, node);
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
		msh_end(msh, "tokenizer: word malloc failed", 1);
	ft_memcpy(word, start, len);
	word[len] = '\0';
	return (word);
}

int	set_quoted(char c)
{
	int		quoted;

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

void	remove_quotationmarks_in_tokens(t_list *tokens)
{
	t_list	*current;
	t_token	*token;

	current = tokens;
	while (current)
	{
		token = (t_token *)(current->content);
		if (token->type == T_WORD)
			remove_quotationmarks(token->value);
		current = current->next;
	}
}

void	msh_tokenizer(t_msh *msh)
{
	char	*word;
	int		quoted_adjacent[2];
	int		i;

	i = -1;
	while (*msh->cpy_in_str)
	{
		i++;
		ft_memset(quoted_adjacent, 0, sizeof(quoted_adjacent));
		if (i != 0 && !skip_whitespaces(msh))
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
	//printf("\nraw tokens:");
	//print_tokens(msh->tokens);
	//expansion
	remove_quotationmarks_in_tokens(msh->tokens);
	//printf("\nafter quote removal:");
	//print_tokens(msh->tokens);
	merge_adjacent_tokens(&msh->tokens, msh);
	//printf("\nafter adjacency handling:");
}
