/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:08:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/08/13 17:49:48 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	msh_pwd(t_msh *msh)
{
	char	*pwd;
	char	**array;

	array = ft_split(msh->in_str, ' ');
	if (!array)
		msh_end(msh, "pwd array split failed");
	if (array[1] != NULL)
	{
		printf("%s\n", "pwd: too many arguments");
		ft_array_free(array);
		return (1);
	}
	else
	{
		ft_array_free(array);
		pwd = getcwd(NULL, 0);
		if (!pwd)
			return (1);
		printf("%s\n", pwd);
	}
	return (0);
}
int	msh_cd_permissions(struct stat *stats)
{
	if (stats->st_mode & S_IRUSR & S_IWUSR & S_IXUSR & S_IRGRP & S_IWGRP & S_IXGRP & S_IROTH & S_IWOTH & S_IXOTH)
		return (0);
	return (1);
}

int	msh_cd(t_msh *msh)
{
	char		**array;
	int			value;
	struct stat	stats;

	value = 1;
	array = ft_split(msh->in_str, ' ');
	if (!array)
		msh_end(msh, "cd: array split failed");
	stat(array[1], &stats);
	//	msh_end(msh, "cd: stat failed");
	/*if (!S_ISDIR(stats.st_mode))
		printf("cd: %s: not a directory\n", array[1]);
	else */if (stats.st_mode & !F_OK)
		printf("cd: no such file or directory: %s\n", array[1]);
	else if (msh_cd_permissions(&stats) == 1)
		printf("cd: permission denied: %s\n", array[1]);
	else
		value = chdir(array[1]);
	ft_array_free(array);
	return (value);
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