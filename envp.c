/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   envp.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 02:39:40 by astehl            #+#    #+#             */
/*   Updated: 2025/10/08 14:36:19 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	free_envp(t_msh *msh)
{
	if (msh->envp)
	{
		ft_array_free(msh->envp, ft_array_size(msh->envp));
		msh->envp = NULL;
	}
}

static char	*create_envp_entry(char *name, char *value, t_msh *msh)
{
	char	*result;
	char	*tmp;

	if (!name | !value)
		return (NULL);
	tmp = ft_strjoin(name, "=");
	if (!tmp)
		msh_quit(msh, "(re)building envp entry 1st strjoin malloc failed", 1);
	result = ft_strjoin(tmp, value);
	free(tmp);
	if (!result)
		msh_quit(msh, "(re)building envp entry 2nd strjoin malloc failed", 1);
	return (result);
}

void	build_envp_array(t_msh *msh)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;
	size_t		size;
	size_t		i;

	if (msh->envp)
		free_envp(msh);
	size = ft_lstsize(msh->env_vars);
	msh->envp = ft_calloc(size + 1, sizeof(char *));
	if (!msh->envp)
		msh_quit(msh, "(re)building envp array malloc failed", 1);
	cur_node = msh->env_vars;
	i = -1;
	while (++i < size && cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var && cur_env_var->name && cur_env_var->value)
			msh->envp[i] = create_envp_entry(cur_env_var->name,
					cur_env_var->value, msh);
		if (!msh->envp[i])
			break ;
//      printf("msh->envp[%li] = %s\n", i, msh->envp[i]);//for testing
		cur_node = cur_node->next;
	}
}
