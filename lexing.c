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
	int	i;
	int	single_quotation_marks;
	int	double_quotation_marks;

	i = 0;
	single_quotation_marks = 0;
	double_quotation_marks = 0;
	while (msh->in_str[i])
	{
		if (msh->in_str[i] == '\'')
			single_quotation_marks++;
		else if (msh->in_str[i] == '"')
			double_quotation_marks++;
		i++;
	}
    if (single_quotation_marks % 2 != 0 && double_quotation_marks % 2 != 0)
        msh_end(&msh, "Invalid input: uneven amount of quotation marks");
	if (single_quotation_marks % 2 != 0)
		msh_end(&msh, "Invalid input: uneven amount of single quotation marks");
	if (double_quotation_marks % 2 != 0)
		msh_end(&msh, "Invalid input: uneven amount of double quotation marks");
}

void    free_token(void *content) //so we can free the token-list with ft_lstclear(&msh->tokens, free_token);
{
    t_token *token;

    token = (t_token *)content;
    free(token->value);
    free(token);
}

void    fill_token_list(t_msh *msh, const char *value, t_token_type type)
{
    t_token *token;
    t_list  *node;

    token = malloc(sizeof(t_token)); //needs to be freed in the end
    if (!token)
        return ; //maybe msh_end?
    token->value = ft_strdup(value); //needs to be freed in the end
    token->type = type;
    node = ft_lstnew(token); 
    if (!node)
    {
        free_token(token);
        return ;
    }   
    ft_lstadd_back(&msh->tokens, node); //token list needs to be freed in the end
}

void	tokenizer(t_msh *msh, const char *input)
{
	while (*input)
	{
		while (*input == ' ' || *input == '\t')
            input++;
		if (!*input)
			break ;
		if (*input == '<' && *(input + 1) == '<')
			fill_token_list(&msh, "<<", T_HEREDOC_REDIRECT);
		else if (*input == '>' && *(input + 1) == '>')
			fill_token_list(&msh, ">>", T_APPEND_REDIRECT);
		else if (*input == '<')
			fill_token_list(&msh, "<", T_IN_REDIRECT);
		else if (*input == '>')
			fill_token_list(&msh, ">", T_OUT_REDIRECT);
		else if (*input == '|')
			fill_token_list(&msh, "|", T_PIPE); //is a pipe sign really a token?
		/*else if (*input == '"' || *input == '\'')
			// read string in single/double quotation marks, add token string "T_WORD" to list
		else
		    // read string, add token string "T_WORD" to list
	*/}
}