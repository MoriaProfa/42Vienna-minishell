/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 20:48:16 by astehl            #+#    #+#             */
/*   Updated: 2025/09/29 22:48:59 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_quotationmark_error(t_msh *msh, char quotationmark)
{
	char	str[2];

	str[0] = quotationmark;
	str[1] = '\0';
	ft_putstr_fd("minishell: unexpected EOF while looking for matching `", 2);
	(ft_putstr_fd(str, 2), ft_putendl_fd("'", 2));
	msh_error(msh, "minishell: syntax error: unexpected end of file", 2);
}

void	check_quotationmarks(t_msh *msh)
{
	int		i;
	char	quotationmark;

	i = -1;
	quotationmark = 0;
	while (msh->in_str[++i])
	{
		if (msh->in_str[i] == '\'' || msh->in_str[i] == '"')
		{
			if (i > 0 && msh->in_str[i - 1] == '\\')
				continue ;
			if (quotationmark == 0)
				quotationmark = msh->in_str[i];
			else if (quotationmark == msh->in_str[i])
				quotationmark = 0;
		}
	}
	if (quotationmark != 0)
		handle_quotationmark_error(msh, quotationmark);
}

void	check_tokens(t_msh *msh)
{
	t_list	*token_list;
	t_token *current_token;
	//t_token *next_token;
	t_token	*first_token;
	t_token	*last_token;

	token_list = msh->tokens;
	current_token = token_list->content;
	first_token = current_token;
	last_token = ft_lstlast(token_list)->content;
	if (first_token->type == T_PIPE)
		msh_error(msh, "minishell: syntax error near unexpected token `|'", 2);
	if (last_token->type == T_PIPE)
		msh_error(msh, "minishell: syntax error: unexpected end of file", 2);
		//bash opens an input line
		//if ctrl+c is pressed it gives the prompt back without error msg 
		//but if ctrl+D is pressed it exits with error message "bash: syntax error: unexpected end of file" and in next line "exit"
	if (last_token->type != T_WORD && last_token->type != T_PIPE)
		msh_error(msh, "minishell: syntax error near unexpected token `newline'", 2);
	/*while (token_list->next && !msh->error_check)
	{
		current_token = token_list->content;
		next_token = token_list->next->content;
		if (current_token->type != T_WORD && next_token->type != T_WORD)
		{
			msh_error(msh, "minishell: syntax error near unexpected token `", 2);
			(ft_putstr_fd(current_token->value, 2), ft_putendl_fd("'", 2));
		}
	}*/
}
