/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_cd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 20:22:27 by astehl            #+#    #+#             */
/*   Updated: 2025/10/18 15:51:56 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static char	*get_cd_path(t_msh *msh, char **args)
{
	char	*path;

	if (!args[1])
	{
		path = get_env_var_value(msh, "HOME");
		if (!path)
			msh_print(msh, "cd: HOME not set", 1);
	}
	else if (args[1][0] == '\0')
		return (NULL);
	else if (args[1][0] == '-' && args[1][1] == '\0')
	{
		path = get_env_var_value(msh, "OLDPWD");
		if (!path)
			msh_print(msh, "cd: OLDPWD not set", 1);
	}
	else
	{
		path = ft_strdup(args[1]);
		if (!path)
			msh_print(msh, "cd: path malloc failed", 1);
	}
	return (path);
}

static void	handle_cd_env_var_update(t_msh *msh, char *oldpwd)
{
	char	*newpwd;

	if (!check_and_update_env_var_value(msh, "OLDPWD", oldpwd))
		prepare_name_and_add_to_env_vars(msh, "OLDPWD", oldpwd);
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		return (print_getcwd_error("cd", msh));
	if (!check_and_update_env_var_value(msh, "PWD", newpwd))
		prepare_name_and_add_to_env_vars(msh, "PWD", newpwd);
}

void	msh_cd(t_msh *msh, char **args)
{
	char	*path;
	char	*oldpwd;

	if (args[2])
		return (msh_print(msh, "cd: too many arguments", 1));
	path = get_cd_path(msh, args);
	if (!path)
		return ;
	oldpwd = get_env_var_value(msh, "PWD");
	if (!oldpwd)
	{
		oldpwd = getcwd(NULL, 0);
		if (!oldpwd)
			return (free(path), print_getcwd_error("cd", msh));
	}
	msh->status = chdir(path);
	if (msh->status != 0)
		return (msh_print(msh, NULL, 1), ft_putstr_fd("minishell: cd: ",
				STDERR_FILENO), perror(path), free(oldpwd), free(path));
	else
	{
		if (args[1] && (args[1][0] == '-' && args[1][1] == '\0'))
			ft_putendl_fd(path, STDOUT_FILENO);
		(free(path), handle_cd_env_var_update(msh, oldpwd));
	}
}
