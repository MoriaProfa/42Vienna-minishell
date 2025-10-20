/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_prints.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 20:47:21 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 21:16:42 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	print_env_vars(t_list *env_vars)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var && cur_env_var->name && cur_env_var->value)
		{
			ft_putstr_fd(cur_env_var->name, STDOUT_FILENO);
			ft_putchar_fd('=', STDOUT_FILENO);
			ft_putendl_fd(cur_env_var->value, STDOUT_FILENO);
		}
		cur_node = cur_node->next;
	}
}

void	print_export_wo_args(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var && cur_env_var->name)
		{
			if (!ft_strncmp(name, cur_env_var->name, ft_strlen(name) + 1))
			{
				if (cur_env_var->value)
					printf("declare -x %s=\"%s\"\n", cur_env_var->name,
						cur_env_var->value);
				else
					printf("declare -x %s\n", cur_env_var->name);
				return ;
			}
		}
		cur_node = cur_node->next;
	}
}

void	print_echo_args(t_msh *msh, char **args, int start)
{
	bool	begin;

	begin = true;
	while (args[++start])
	{
		if (!begin)
			ft_putchar_fd(' ', STDOUT_FILENO);
		ft_putstr_fd(args[start], STDOUT_FILENO);
		msh->status = 0;
		begin = false;
	}
}
