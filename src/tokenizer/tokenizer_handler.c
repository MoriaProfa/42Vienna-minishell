/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer_handler.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 23:05:58 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 23:10:34 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static int	handle_single_metacharacter(t_msh *msh, char c, t_token_type type)
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
