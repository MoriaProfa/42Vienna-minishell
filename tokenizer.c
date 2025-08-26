/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:25:30 by astehl            #+#    #+#             */
/*   Updated: 2025/08/26 21:38:09 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//just for testing purposes
void print_tokens(t_list *tokens)
{
    t_list *current = tokens;
	int i = 1;
	
	printf("Token list:\n");
    while (current)
    {
        t_token *token = (t_token *)(current->content);
        if (token)
            printf("Token #%i:\n\tValue: %s, Type: %d\n", i, token->value, token->type);
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

void	fill_token_list(t_msh *msh, const char *value, t_token_type type)
{
	t_token		*token;
	t_list	*node;

	token = malloc(sizeof(t_token));
	if (!token)
		return ;//maybe msh_end
	token->value = ft_strdup(value);
	token->type = type;
	node = ft_lstnew(token);
	if (!node)
	{
		free_token(token);
		return ;//maybe msh_end
	}
	ft_lstadd_back(&msh->tokens, node);
}

int	handle_single_metacharacter(t_msh *msh, char c, t_token_type type)
{
	char str[2];

	if (*msh->cpy_in_str == c)
	{
		str[0] = c;
		str[1] = '\0';
		fill_token_list(msh, str, type);
		msh->cpy_in_str++;
		return (1);
	}
	return (0);
}

int	handle_metacharacters(t_msh *msh)
{
	if (*msh->cpy_in_str == '<' && *(msh->cpy_in_str + 1) == '<')
	{
		fill_token_list(msh, "<<", T_HEREDOC_REDIRECT);
		msh->cpy_in_str += 2;
		return (1);
	}
	if (*msh->cpy_in_str == '>' && *(msh->cpy_in_str + 1) == '>')
	{
		fill_token_list(msh, ">>", T_APPEND_REDIRECT);
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

void	skip_whitespaces(t_msh *msh)
{
	while (*msh->cpy_in_str == ' ' || *msh->cpy_in_str == '\t')
			msh->cpy_in_str++;
}

char	*handle_quoted(t_msh *msh, int *len)
{
	char	*start;
	char	quotationmark;

	quotationmark = *msh->cpy_in_str;
	msh->cpy_in_str++;
	start = msh->cpy_in_str;
	*len = 0;
	while (*msh->cpy_in_str && *msh->cpy_in_str != quotationmark)
		{
			msh->cpy_in_str++;
			(*len)++;
		}
	if (*msh->cpy_in_str == quotationmark)
		msh->cpy_in_str++;
	return (start);
}

char	*handle_unquoted(t_msh *msh, int *len)
{
	char	*start;

	start = msh->cpy_in_str;
	*len = 0;
	while (*msh->cpy_in_str && !ft_strchr(" \t<>|", *msh->cpy_in_str))
	{
		msh->cpy_in_str++;
		(*len)++;
	}
	return (start);
}

char	*create_word(t_msh *msh)
{

	char	*word;
	char	*start;
	int		len;

	len = 0;
	skip_whitespaces(msh);
	if (*msh->cpy_in_str == '"' || *msh->cpy_in_str == '\'')
		start = handle_quoted(msh, &len);
	else
		start = handle_unquoted(msh, &len);
	word = ft_calloc(len + 1, sizeof(char));
	if (!word)
		return (NULL);
	ft_memcpy(word, start, len);
	word[len] = '\0';
	return (word);
}

void	msh_tokenizer(t_msh *msh)
{
	char	*word;

	while (*msh->cpy_in_str)
	{
		skip_whitespaces(msh);
		if (!*msh->cpy_in_str)
			break ;
		if (handle_metacharacters(msh))
			continue ;
		else
		{
			word = create_word(msh);
			if (!word)
				break ;
			fill_token_list(msh, word, T_WORD);
			free(word);
		}
	}
}
