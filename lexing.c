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

void	msh_end(t_msh *msh, char *msg);

#include "minishell.h"

void	check_quotation_marks(t_msh *msh)
{
	int		i;
	char	quotation_mark;
	char	str[2];

	i = -1;
	quotation_mark = 0;
	while (msh->in_str[++i])
	{
		if (msh->in_str[i] == '\'' || msh->in_str[i] == '"')
		{
			if (quotation_mark == 0)
				quotation_mark = msh->in_str[i]; // quote is opened
			else if (quotation_mark == msh->in_str[i])
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
		msh_end(&msh, "minishell: syntax error: unexpected end of file");
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

	if (*msh->in_str == c)
	{
		str[0] = c;
		str[1] = '\0';
		fill_token_list(msh, str, type);
		msh->in_str++;
		return (1);
	}
	return (0);
}

int	handle_metacharacters(t_msh *msh)
{
	if (*msh->in_str == '<' && *(msh->in_str + 1) == '<')
	{
		fill_token_list(msh, "<<", T_HEREDOC_REDIRECT);
		msh->in_str += 2;
		return (1);
	}
	if (*msh->in_str == '>' && *(msh->in_str + 1) == '>')
	{
		fill_token_list(msh, ">>", T_APPEND_REDIRECT);
		msh->in_str += 2;
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

void	tokenizer(t_msh *msh)
{
	while (*msh->in_str)
	{
		while (*msh->in_str == ' ' || *msh->in_str == '\t')
			msh->in_str++;
		if (!*msh->in_str)
			break ;
		if (handle_metacharacters(msh))
			continue ;
		/*else if (*msh->in_str == '"' || *msh->in_str == '\'')
			// read string in single/double quotation marks,
				add token string "T_WORD" to list
		else
			 // read string, add token string "T_WORD" to list*/
	}
}
