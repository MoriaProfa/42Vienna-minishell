/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 16:29:54 by astehl            #+#    #+#             */
/*   Updated: 2025/09/30 17:51:30 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


void    expand_word_tokens(t_list *tokens, t_msh *msh)
{
    t_list  *cur_node;
    t_token *cur_token;
    char    *str;
    int     i;

    cur_node = tokens;
    while (cur_node)
    {
        i = -1;
        cur_token = (t_token *)cur_node->content;
        if (cur_token->type == T_WORD && cur_token->quoted != Q_SINGLE && 
            ft_strchr(cur_token->value, '$'))
        {//this needs to have some recursive logic to be able to handle more than one expansion in one string
            while(cur_token->value[++i])
            {
                if (cur_token->value[i] == '$' && (ft_isalpha(cur_token->value[i + 1]) || ft_strchr("_?", cur_token->value[i])))
                {
                    str = ft_substr(cur_token->value, 0, i);
                }
            }
        }
        cur_node = cur_node->next;
    }
}
//letters _ ? <- allowed chars after $ to expand it

void    remove_quotationmarks(char *str)
{
    size_t  i;
    char    quotationmark;
    char    *start;
    char    *end;

    i = 0;
    if (!str)
        return ;
    while (str[i] && str[i] != '\'' && str[i] != '"')
        i++;
    if (i >= ft_strlen(str))
        return ;
    quotationmark = str[i];
    start = ft_strchr(str, quotationmark);
    if (!start)
        return ;
    end = ft_strchr(start + 1, quotationmark);
    if (!end)
        return ;
    ft_memmove(start, start + 1, end - start - 1);
    ft_memmove(start + (end - start - 1), end + 1, ft_strlen(end + 1) + 1);
}

t_quote_type    merge_quote_types(t_quote_type q1, t_quote_type q2)
{
    if (q1 == q2)
        return (q1);
    if ((q1 == Q_NONE && q2 == Q_SINGLE) || (q1 == Q_SINGLE && q2 == Q_NONE))
        return (Q_SINGLE);
    if ((q1 == Q_NONE && q2 == Q_DOUBLE) || (q1 == Q_DOUBLE && q2 == Q_NONE))
        return (Q_DOUBLE);
    return (Q_MIXED);
}

void    join_token_content(t_token *previous, t_token *current, t_msh *msh)
{
    char    *joined;

    joined = ft_strjoin(previous->value, current->value);
    if (!joined)
        msh_end(msh, "token value merging ft_strjoin malloc failed", 1);
    free(previous->value);
    previous->value = joined;
    previous->quoted = merge_quote_types(previous->quoted, current->quoted);
}

void    merge_adjacent_tokens(t_list **tokens, t_msh *msh)
{
    t_list  *cur_node;
    t_token *cur_token;
    t_token *prev_token;
    t_list  *prev_node;

    cur_node = *tokens;
    while (cur_node)
    {
        cur_token = (t_token *)cur_node->content;
        if (cur_token->type == T_WORD 
            && cur_token->is_adjacent && cur_node->prev 
            && ((t_token *)cur_node->prev->content)->type == T_WORD)
        {
            prev_node = cur_node->prev;
            prev_token = (t_token *)prev_node->content;
            join_token_content(prev_token, cur_token, msh);
            prev_node->next = cur_node->next;
            if (cur_node->next)
                cur_node->next->prev = prev_node;
            ft_lstdelone(cur_node, free_token);
            cur_node = prev_node->next;       
        }
        else
            cur_node = cur_node->next;
    }
}
