/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:08:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/17 12:20:59 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	msh_pwd(t_msh *msh)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (msh->status);
	printf("%s\n", pwd);
	free(pwd);
	return (0);
}

int	msh_cd(t_msh *msh)
{
	char		**array;
	struct stat	stats;

	array = ft_split(msh->in_str, ' ');
	if (!array)
		msh_end(msh, "cd: array split failed");
	stat(array[1], &stats);
	//	msh_end(msh, "cd: stat failed");
	/*if (!S_ISDIR(stats.st_mode))
		printf("cd: %s: not a directory\n", array[1]);*/
	if (array[2])
		printf("%s\n", "cd: too many arguments");
	if (stats.st_mode & F_OK)
		msh->status = chdir(array[1]);
	if (!(stats.st_mode & F_OK))
		printf("cd: no such file or directory: %s\n", array[1]);
	if (access(array[1], X_OK) == 0)
		printf("cd: permission denied: %s\n", array[1]);
	ft_array_free(array);
	return (msh->status);
}
/*
int	msh_exit(t_msh *msh)
{
}

int	msh_env(t_msh *msh)
{
}

int	msh_export(t_msh *msh)
{
}

int	msh_unset(t_msh *msh)
{
}

int	msh_echo(t_msh *msh)
{
}
*/

void	msh_builtins(t_msh *msh)
{
	
}