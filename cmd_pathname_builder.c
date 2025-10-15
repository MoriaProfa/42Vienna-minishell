/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_pathname_builder.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:40:52 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/15 18:53:30 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	compare_strings(char *s1, char *s2);

char	*resolve_cmd_pathname(t_msh *msh, char **paths, char *cmd)
{
	char	*pathname;
	char	*tmp;
	int		j;

	j = -1;
	pathname = NULL;
	while (paths[++j] && cmd[0] != '\0')
	{
		tmp = ft_strjoin(paths[j], "/");
		if (!tmp)
			(ft_array_free(paths, ft_array_size(paths)),
				msh_quit(msh, "pathname tmp ft_strjoin malloc failed", 1));
		pathname = ft_strjoin(tmp, cmd);
		free(tmp);
		if (!pathname)
			(ft_array_free(paths, ft_array_size(paths)),
				msh_quit(msh, "pathname tmp ft_strjoin malloc failed", 1));
		if (access(pathname, F_OK) == 0)
			return (pathname);
		free(pathname);
	}
	return (NULL);
}

static char	*search_cmd_pathname(t_msh *msh, char *cmd, char *env)
{
	char	**paths;
	char	*pathname;

	pathname = NULL;
	if (env && env[5] != '\0')
	{
		paths = ft_split(env + 5, ':');
		if (!paths)
			msh_quit(msh, "paths ft_split malloc failed", 1);
		pathname = resolve_cmd_pathname(msh, paths, cmd);
		ft_array_free(paths, ft_array_size(paths));
	}
	else
	{
		if (access(cmd, F_OK) == 0 && compare_strings(cmd, "..") == false)
			pathname = ft_strdup(cmd);
		else
			pathname = ft_strjoin("/", cmd);
		if (!pathname)
			msh_quit(msh, "pathname malloc failed", 1);
	}
	return (pathname);
}

char	*create_cmd_pathname(t_msh *msh, char *cmd)
{
	char	*pathname;
	int		j;

	j = 0;
	pathname = NULL;
	if (ft_strchr(cmd, '/'))
	{
		pathname = ft_strdup(cmd);
		if (!pathname)
			msh_quit(msh, "pathname ft_strdup malloc failed", 1);
		return (pathname);
	}
	while (msh->envp[j] && ft_strncmp(msh->envp[j], "PATH=", 5) != 0)
		j++;
	pathname = search_cmd_pathname(msh, cmd, msh->envp[j]);	
	return (pathname);
}
