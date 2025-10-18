/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 20:48:16 by astehl            #+#    #+#             */
/*   Updated: 2025/10/18 15:39:24 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	handle_quotationmark_error(t_msh *msh, char quotationmark)
{
	char	str[2];

	str[0] = quotationmark;
	str[1] = '\0';
	//	ft_putstr_fd("minishell: ", STDERR_FILENO);//tester doesnt accept it
	ft_putstr_fd("unexpected EOF while looking for matching `", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	ft_putendl_fd("'", STDERR_FILENO);
	msh_print(msh, "syntax error: unexpected end of file", 2);
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

static void	check_middle_tokens(t_msh *msh)
{
	t_list	*cur_node;
	t_token	*cur_token;
	t_token	*next_token;

	cur_node = msh->tokens;
	while (!msh->error_check && cur_node && cur_node->next)
	{
		cur_token = cur_node->content;
		next_token = cur_node->next->content;
		if (cur_token->type != T_WORD && cur_token->type != T_PIPE
			&& next_token->type != T_WORD)
		{
			//ft_putstr_fd("minishell: ", STDERR_FILENO);//tester doesnt accept it
			ft_putstr_fd("syntax error near unexpected token `", STDERR_FILENO);
			ft_putstr_fd(next_token->value, STDERR_FILENO);
			ft_putendl_fd("'", STDERR_FILENO);
			msh_print(msh, NULL, 2);
		}
		if (cur_token->type == T_PIPE && next_token->type == T_PIPE)
		{
			//ft_putstr_fd("minishell: ", STDERR_FILENO);//tester doesnt accept it
			msh_print(msh, "syntax error near unexpected token `|'", 2);
		}
		cur_node = cur_node->next;
	}
}

void	check_tokens(t_msh *msh)
{
	t_list	*cur_node;
	t_token	*first_token;
	t_token	*last_token;

	cur_node = msh->tokens;
	first_token = cur_node->content;
	last_token = ft_lstlast(cur_node)->content;
	if (first_token->type == T_PIPE)
		msh_print(msh, "syntax error near unexpected token `|'", 2);// tester doesnt accept printing minishell: in front
	check_middle_tokens(msh);
	if (!msh->error_check && last_token->type == T_PIPE)
		msh_print(msh, "syntax error: unexpected end of file", 2);// tester doesnt accept printing minishell: in front
	if (!msh->error_check && last_token->type != T_WORD
		&& last_token->type != T_PIPE)
		msh_print(msh, "syntax error near unexpected token `newline'", 2);// tester doesnt accept printing minishell: in front
}
