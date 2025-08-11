/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 20:48:16 by astehl            #+#    #+#             */
/*   Updated: 2025/08/05 20:48:21 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_end(t_msh *msh, char *msg);

//just for testing purposes
void print_tokens(t_list *tokens)
{
    t_list *current = tokens;
    while (current)
    {
        t_token *token = (t_token *)(current->content);
        if (token)
            printf("Value: %s, Type: %d\n", token->value, token->type);
        current = current->next;
    }
}

void	check_quotation_marks(t_msh *msh)
{
	int		i;
	char	quotation_mark;
	char	str[2];

	i = -1;
	quotation_mark = 0;
	while (msh->cpy_in_str[++i])
	{
		if (msh->cpy_in_str[i] == '\'' || msh->cpy_in_str[i] == '"')
		{
			if (quotation_mark == 0)
				quotation_mark = msh->cpy_in_str[i]; // quote is opened
			else if (quotation_mark == msh->cpy_in_str[i])
				quotation_mark = 0; // quote is closed again
		}
	}
	if (quotation_mark != 0)
	{
		str[0] = quotation_mark;
		str[1] = '\0';
		ft_putstr_fd("minishell: unexpected EOF while looking for matching `", \
			2);
		(ft_putstr_fd(str, 2), ft_putendl_fd("'", 2));
		msh_end(msh, "minishell: syntax error: unexpected end of file");
	}
}

//todo: free the token-list in msh_end with ft_lstclear(&msh->tokens, free_token);
void	free_token(void *content)
{
	t_token	*token;

	token = (t_token *)content;
	free(token->value);
	free(token);
}

void	fill_token_list(t_msh *msh, const char *value, t_token_type type)
{
	t_token	*token;
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
	if (*msh->cpy_in_str == '<' && *(msh->in_str + 1) == '<')
	{
		fill_token_list(msh, "<<", T_HEREDOC_REDIRECT);
		msh->cpy_in_str += 2;
		return (1);
	}
	if (*msh->cpy_in_str == '>' && *(msh->in_str + 1) == '>')
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
