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

// so we can free the token-list with ft_lstclear(&msh->tokens, free_token);
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

	token = malloc(sizeof(t_token));// needs to be freed in the end
	if (!token)
		return ;//maybe msh_end
	token->value = ft_strdup(value);// needs to be freed in the end
	token->type = type;
	node = ft_lstnew(token);
	if (!node)
	{
		free_token(token);
		return ;
	}
	ft_lstadd_back(&msh->tokens, node);
	// token list needs to be freed in the end
}

void	tokenizer(t_msh *msh)
{
	while (*msh->in_str)
	{
		while (*msh->in_str == ' ' || *msh->in_str == '\t')
			msh->in_str++;
		if (!*msh->in_str)
			break ;
		if (*msh->in_str == '<' && *(msh->in_str + 1) == '<')
			fill_token_list(&msh, "<<", T_HEREDOC_REDIRECT);
		else if (*msh->in_str == '>' && *(msh->in_str + 1) == '>')
			fill_token_list(&msh, ">>", T_APPEND_REDIRECT);
		else if (*msh->in_str == '<')
			fill_token_list(&msh, "<", T_IN_REDIRECT);
		else if (*msh->in_str == '>')
			fill_token_list(&msh, ">", T_OUT_REDIRECT);
		else if (*msh->in_str == '|')
			fill_token_list(&msh, "|", T_PIPE);
	// is a pipe sign really a token?
		/*else if (*msh->in_str == '"' || *msh->in_str == '\'')
			// read string in single/double quotation marks,
				add token string "T_WORD" to list
		else
			 // read string, add token string "T_WORD" to list*/
	}
}
