/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 21:20:54 by astehl            #+#    #+#             */
/*   Updated: 2025/10/18 01:56:29 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	free_env_var(void *content)
{
	t_env_var	*cur_env_var;

	cur_env_var = (t_env_var *)content;
	if (!cur_env_var)
		return ;
	if (cur_env_var->name)
		free(cur_env_var->name);
	if (cur_env_var->value)
		free(cur_env_var->value);
	free(cur_env_var);
}


static void	create_env(t_msh *msh)
{
	char	*cwd;

	if (!check_and_update_env_var_value(msh, "SHLVL", NULL))
		prepare_and_add_to_env_vars(msh, "SHLVL", "1");
	if (!check_and_update_env_var_value(msh, "PWD", NULL))
	{
		cwd = getcwd(NULL, 0);
		if (!cwd)
		{
			cwd = ft_strdup("/");
			if (!cwd)
				msh_quit(msh, "create env: cwd fallback malloc failed", 1);
		}
		prepare_name_and_add_to_env_vars(msh, "PWD", cwd);
	}
	if (!check_and_update_env_var_value(msh, "OLDPWD", NULL))
		prepare_and_add_to_env_vars(msh, "OLDPWD", NULL);//gets init in cd, if we do it here, cd - doesnt work always
}

void	init_env_vars(t_msh *msh, char **envp)
{
	char	*name;
	int		i;

	i = -1;
	while (envp && envp[++i])
	{
		if (ft_strchr(envp[i], '='))
			handle_export_with_value(envp[i], msh);
		else
		{
			name = ft_strdup(envp[i]);
			if (!name)
				msh_quit(msh, "env: name init malloc failed", 1);
			add_to_env_vars(msh, name, NULL);
		}
	}
	create_env(msh);
}
