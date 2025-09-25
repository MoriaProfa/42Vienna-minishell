/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 16:29:54 by astehl            #+#    #+#             */
/*   Updated: 2025/09/25 17:14:36 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void    expand_word_tokens(t_list *tokens)
{
    t_token *token;
    int     i;
    char    *str;

    while (tokens->next)
    {
        token = (t_token *)tokens->content;
        i = 0;
        if (token->type == T_WORD && token->quoted != Q_SINGLE)
        {
            str = ft_strchr(token->value, '$');
            
        }
        tokens = tokens->next;
    }
}