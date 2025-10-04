/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 21:20:54 by astehl            #+#    #+#             */
/*   Updated: 2025/10/04 03:16:55 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_env_var(void *content)
{
    t_env_var   *cur_env_var;

    cur_env_var = (t_env_var *)content;
    if (!cur_env_var)
        return ;
    if (cur_env_var->name)
        free(cur_env_var->name);
    if (cur_env_var->value)
        free(cur_env_var->value);
    free(cur_env_var);
}

void	ft_printenv(t_list *env_vars)
{
	t_list	    *cur_node;
	t_env_var	*cur_env_var;

	cur_node = env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var)
			printf("%s=%s\n", cur_env_var->name, cur_env_var->value);
		cur_node = cur_node->next;
	}
}

void	fill_env_vars_list(t_msh *msh, const char *name, const char *value)
{
	t_list	    *cur_node;
    t_env_var	*cur_env_var;

	cur_env_var = ft_calloc(1, sizeof(t_env_var));
	if (!cur_env_var)
		msh_end(msh, "env: cur_env_var malloc failed", 1);
	cur_env_var->name = ft_strdup(name);
    if (!cur_env_var->name)
		msh_end(msh, "env: name malloc failed", 1);
   	cur_env_var->value = ft_strdup(value);
	if (!cur_env_var->value)
		msh_end(msh, "env: value malloc failed", 1);
	cur_node = ft_lstnew(cur_env_var);
	if (!cur_node)
		msh_end(msh, "env: node malloc failed", 1);
	ft_lstadd_back(&msh->env_vars, cur_node);
}

void    create_env(t_msh *msh)
{
    char    *cwd;
    
    fill_env_vars_list(msh, "SHLVL", "1");
    cwd = getcwd(NULL, 0);
    if (!cwd)
        cwd = ft_strdup("/");
    fill_env_vars_list(msh, "PWD", cwd);
    free(cwd);
    fill_env_vars_list(msh, "OLDPWD", "");
}

void	init_env_vars(t_msh *msh, char **envp, size_t len)
{
	char    *name;
    char    *value;
    int     i;
    int     j;

    if (!envp || !envp[0])
        create_env(msh);
    i = -1;
    while (envp[++i])
    {
        j = -1;
        len = 0;
        while (envp[i] && envp[i][++j] != '=')
            len++;
        name = ft_substr(envp[i], 0, len);
        if (!name)
            msh_end(msh, "env: name init malloc failed", 1);
        if (++len < ft_strlen(envp[i]))
            value = ft_substr(envp[i], len, ft_strlen(envp[i]) - len);
        else
            value = ft_strdup("");
        if (!value)
            (free(name), msh_end(msh, "env: value init malloc failed", 1));
        (fill_env_vars_list(msh, name, value), free(name), free(value));
    }
}
