/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:00:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/21 22:53:34 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_free(t_msh *msh)
{
	if (msh->cmd)
		ft_free_array(msh->cmd);
	if (msh->pathname)
		ft_free(&msh->pathname);
}

void	msh_exit(t_msh *msh, char *msg)
{
	ft_putendl_fd(msg, 2);
	msh_free(msh);
	exit(msh->status);
}

void	msh_execute(t_msh *msh)
{
	if (!msh->pathname)
	{
		msh->status = 127;
		msh_exit(msh, "command not found");
	}
	else if (msh->pathname && access(msh->pathname, X_OK) == -1)
	{
		msh->status = 126;
		msh_exit(msh, "permission denied");
	}
	else if (msh->pathname && access(msh->pathname, X_OK) == 0)
		execve(msh->pathname, msh->cmd, msh->envp);
	msh_exit(msh, "execve failed");
}

char	*msh_pathname_r(t_msh *msh, char **paths)
{
	char	*pathname;
	char	*tmp;
	int		i;

	i = -1;
	pathname = NULL;
	while (paths[++i])
	{
		tmp = ft_strjoin(paths[i], "/");
		if (!tmp)
			msh_exit(msh, "ft_strjoin failed");
		pathname = ft_strjoin(tmp, msh->cmd[0]);
		free(tmp);
		if (!pathname)
			msh_exit(msh, "ft_strjoin failed");
		if (access(pathname, F_OK) == 0)
			return (pathname);
		free(pathname);
	}
	return (NULL);
}

void	msh_pathname(t_msh *msh)
{
	char	**paths;

	if (access(msh->cmd[0], F_OK) == 0)
	{
		msh->pathname = msh->cmd[0];
	}
	else
	{
		msh->pathname = NULL;
		while (*msh->envp && ft_strncmp(*msh->envp, "PATH=", 5) != 0)
			msh->envp++;
		if (*msh->envp)
		{
			paths = ft_split(*msh->envp + 5, ':');
			if (!paths)
				msh_exit(msh, "ft_split failed");
			msh->pathname = msh_pathname_r(msh, paths);
			ft_free_array(paths);
			if (!msh->pathname)
			{
				msh->status = 127;
				msh_exit(msh, "command not found");
			}
		}
	}
}

void	msh_initialize(t_msh *msh, char **argv, char **envp)
{
	msh->status = 1;
	msh->cmd = ft_split(argv[1], ' ');
	if (!msh->cmd)
		msh_exit(msh, "split failed");
	msh->envp = envp;
	msh_pathname(msh);
}

int	main(int argc, char **argv, char **envp)
{
	t_msh	msh;

	if (argc < 2)
		ft_printf("%s\n", "enter command");
	msh_initialize(&msh, argv, envp);
	msh_execute(&msh);
	return (0);
}
