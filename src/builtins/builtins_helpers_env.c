/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_helpers_env.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 20:45:23 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 20:46:51 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*get_env_var_value(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var && cur_env_var->name && cur_env_var-> value && 
				!ft_strncmp(name, cur_env_var->name, ft_strlen(name) + 1))
			return (ft_strdup(cur_env_var->value));
		cur_node = cur_node->next;
	}
	return (NULL);
}

bool	check_and_update_env_var_value(t_msh *msh, char *name, char *value)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (!ft_strncmp(name, cur_env_var->name, ft_strlen(name) + 1))
		{
			if (value)
			{
				free(cur_env_var->value);
				cur_env_var->value = value;
			}
			return (true);
		}
		cur_node = cur_node->next;
	}
	return (false);
}

void	check_and_delete_env_var(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)cur_node->content;
		if (!ft_strncmp(name, cur_env_var->name, ft_strlen(name) + 1))
		{
			if (cur_node->prev)
				cur_node->prev->next = cur_node->next;
			else
				msh->env_vars = cur_node->next;
			if (cur_node->next)
				cur_node->next->prev = cur_node->prev;
			ft_lstdelone(cur_node, free_env_var);
			break ;
		}
		cur_node = cur_node->next;
	}
}
