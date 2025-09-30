/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 16:29:54 by astehl            #+#    #+#             */
/*   Updated: 2025/09/30 21:15:02 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


void    expand_word_tokens(t_list *tokens, t_msh *msh)
{
    t_list  *cur_node;
    t_token *cur_token;
    char    *str;
    char    *value_cpy;
    char    *env_var;
    char    *env_var_tmp;
    char    *str_tmp;
    int     i;
    int     len;
    int     j;

    cur_node = tokens;
    while (cur_node)
    {
        cur_token = (t_token *)cur_node->content;
        value_cpy = cur_token->value;
        if (cur_token->type == T_WORD && cur_token->quoted != Q_SINGLE && 
            ft_strchr(value_cpy, '$'))
        {
            i = -1;
            while(value_cpy[++i])
            {
                if (value_cpy[i] == '$' && (ft_isalpha(value_cpy[i + 1]) || ft_strchr("_?", value_cpy[i + 1])))
                {
                    if (i > 1 && !str)
                    {
                        str = ft_substr(value_cpy, 0, i - 1);
                        if (!str)
                            msh_end(msh, "expand str substr malloc failed", 1);
                    }
                    len = i + 1;
                    while (value_cpy[len] != ' ' && value_cpy[len] != '$')
                        len++;
                    len -= i;
                    env_var_tmp = ft_substr(value_cpy, 0, len);
                    if (!env_var_tmp)
                        msh_end(msh, "expand env_var_tmp substr malloc failed", 1);
                    env_var = ft_strjoin(env_var_tmp, "=");
                    if (!env_var)
                        msh_end(msh, "expand env_var strjoin malloc failed", 1);
                    while (msh->envp[j] && ft_strncmp(msh->envp[j], env_var, ft_strlen(env_var)) != 0)
                        j++;
                    if (!str)
                        str = msh->envp[j];
                    else
                    {
                        str_tmp = ft_strjoin(str, msh->envp[j]);
                        if (!str_tmp)
                            msh_end(msh, "expand tmp strjoin malloc failed", 1);
                        free(str);
                        free(env_var);
                        str = str_tmp;
                    }
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
