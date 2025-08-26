/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_table.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/21 14:28:42 by astehl            #+#    #+#             */
/*   Updated: 2025/08/26 21:42:05 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
char	*create_cmd_pathname(t_msh *msh, char *cmd);

//just for testing purposes
void print_cmd_table(t_msh *msh)
{
    int i = 0;
    if (!msh->cmd_table)
        return ;
    printf("\n=== Command Table (n = %i) ===\n", msh->n_cmds);
    while (msh->cmd_table[i] && i < msh->n_cmds)
    {
        t_cmd *cmd = msh->cmd_table[i];
        printf("Command %i:\n", i);
        printf("\targs: ");
        int j = 0;
        while (cmd->args[j] && j < cmd->argc)
            printf("[%s] ", cmd->args[j++]);
        printf("\n");
        if (cmd->infile)
            printf("\tinfile: %s\n", cmd->infile);
        if (cmd->outfile)
            printf("\toutfile: %s (append = %i)\n", cmd->outfile, cmd->append);
        if (cmd->heredoc)
            printf("\theredoc: %s\n", cmd->heredoc);
        if (cmd->pathname)
            printf("\tpathname: %s\n", cmd->pathname);
        i++;
    }
}

void    free_cmd_table(t_cmd **cmds, int n)
{
    int i;
    int j;

    i = -1;
    if (!cmds)
        return ;
    while (cmds[++i] && i < n)
    {
        j = -1;
        while (cmds[i]->args[++j] && j < cmds[i]->argc)
            free(cmds[i]->args[j]);
        if (cmds[i]->args)
            free(cmds[i]->args);
        if (cmds[i]->infile)
            free(cmds[i]->infile);
        if (cmds[i]->outfile)
            free(cmds[i]->outfile);
        if (cmds[i]->heredoc)
            free(cmds[i]->heredoc);
        if (cmds[i]->pathname)
            free(cmds[i]->pathname);
        free(cmds[i]);
    }
    free(cmds);
}

void    *grow_array(void *old, int old_cap, int new_cap, size_t size)
{
    void    *new;

    new = ft_calloc(new_cap, size);
    if (!new)
        return (NULL);
    ft_memcpy(new, old, old_cap * size);
    free(old);
    return (new);
}

t_cmd   *init_cmd(void)
{
    t_cmd   *cmd;

    cmd = ft_calloc(1, sizeof(t_cmd));
    if (!cmd)
        return (NULL);
    cmd->args_cap = 4;
    cmd->args = ft_calloc(cmd->args_cap, sizeof(char *));
    if (!cmd->args)
    {
        free(cmd);
        return (NULL);
    }
    cmd->argc = 0;
    /*cmd->pathname = (char *)ft_calloc(1, sizeof(char *));
	if (!cmd->pathname)
	{
        free(cmd);
        return (NULL);
    }*/ 
//why allocate when setting it to NULL afterwards? this creates a leak and it seems to work without it
    cmd->pathname = NULL;
    return (cmd);
}

void    add_to_args(t_cmd *cmd, char *word)
{
    int new_cap;

    if (cmd->argc + 1 >= cmd->args_cap)
    {
        new_cap = cmd->args_cap * 2;
        cmd->args = grow_array(cmd->args, cmd->args_cap, new_cap, sizeof(char *));
        cmd->args_cap = new_cap;
    }
    cmd->args[cmd->argc++] = ft_strdup(word);
    cmd->args[cmd->argc] = NULL;
}

char    *get_next_word_token(t_list **tokens)
{
    if ((*tokens)->next && ((t_token *)(*tokens)->next->content)->type == T_WORD)
    {
        *tokens = (*tokens)->next;
        return (ft_strdup(((t_token *)(*tokens)->content)->value));
    }
    return (NULL);
}

void   build_cmd_table(t_msh *msh)
{
    int     cap;
    int     new_cap;
    int     count;
    t_cmd   **cmds;
    t_list  *token_list;
    t_token *token;

    cap = 4;
    count = 0;
    cmds = ft_calloc(cap, sizeof(t_cmd*));
    if (!cmds)
        msh_end(msh, "cmd table malloc failed");
    token_list = msh->tokens;
    while(token_list)
    {
        if (count >= cap)
        {
            new_cap = cap * 2;
            cmds = grow_array(cmds, cap, new_cap, sizeof(t_cmd *));
            cap = new_cap;
        }
        cmds[count] = init_cmd();
        if (!cmds[count])
        {
            free_cmd_table(msh->cmd_table, count);
            perror("calloc");
            return ;
        }
        while (token_list && ((t_token *)token_list->content)->type != T_PIPE)
        {
            token = token_list->content;
            
            if (token->type == T_WORD)
                add_to_args(cmds[count], token->value);
            else if (token->type == T_IN_REDIRECT)
                cmds[count]->infile = get_next_word_token(&token_list);
            else if (token->type == T_OUT_REDIRECT)
                cmds[count]->outfile = get_next_word_token(&token_list);
            else if (token->type == T_APPEND_REDIRECT)
            {
                cmds[count]->outfile = get_next_word_token(&token_list);
                cmds[count]->append = 1;
            }
            else if (token->type == T_HEREDOC_REDIRECT)
                cmds[count]->heredoc = get_next_word_token(&token_list);
            token_list = token_list->next;
        }
        cmds[count]->pathname = create_cmd_pathname(msh, cmds[count]->args[0]);
        //check if pathname was created correctly? if not, msh_end?
        if (token_list && ((t_token *)token_list->content)->type == T_PIPE)
            token_list = token_list->next;
        count++;
    }
    if (count >= cap)
        cmds = grow_array(cmds, cap, count + 1, sizeof(t_cmd *));
    cmds[count] = NULL;
    msh->cmd_table = cmds;
    msh->n_cmds = count;
}
