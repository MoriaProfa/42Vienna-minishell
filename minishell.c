/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:00:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/21 13:25:23 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_execute(t_cmd *cmd)
{
	if (cmd->pathname && access(cmd->pathname, X_OK) == -1)
		ft_printf("%s\n", "permission denied");
	if (cmd->pathname && access(cmd->pathname, X_OK) == 0)
		execve(cmd->pathname, cmd->cmd, cmd->envp);
	ft_printf("%s\n", "error");
}

char	*msh_pathname_r(char **paths, t_cmd *cmd)
{
	char	*pathname;
	char	*tmp;
	int		i;

	i = -1;
	while (paths[++i])
	{
		tmp = ft_strjoin(paths[i], "/");
		if (!tmp)
		{
			ft_printf("%s\n", "ft_strjoin failed");
		}
		pathname = ft_strjoin(tmp, cmd->cmd[0]);
		free(tmp);
		if (!pathname)
		{
			ft_printf("%s\n", "ft_strjoin failed");
		}
		if (access(pathname, F_OK) == 0)
			return (pathname);
		free(pathname);
	}
	return (NULL);
}

char	*msh_pathname_a(t_cmd *cmd)
{
	char	*pathname;

	pathname = ft_strcopy(cmd->cmd[0]);
	if (!pathname)
		ft_printf("%s\n", "strcopy failed");
	if (access(pathname, F_OK) == 0)
		return (pathname);
	free(pathname);
	return (NULL);
}

void	msh_pathname(t_cmd *cmd)
{
	char	**paths;

	cmd->pathname = NULL;
	if (cmd->cmd[0])
	{
		if (cmd->cmd[0][0] == '/' || (cmd->cmd[0][0] == '.'
				&& cmd->cmd[0][1] == '/') || (cmd->cmd[0][0] == '.'
				&& cmd->cmd[0][1] == '.' && cmd->cmd[0][2] == '/'))
			cmd->pathname = msh_pathname_a(cmd);
		else
		{
			while (*cmd->envp && ft_strncmp(*cmd->envp, "PATH=", 5) != 0)
				cmd->envp++;
			if (*cmd->envp)
			{
				paths = ft_split(*cmd->envp + 5, ':');
				if (!paths)
					ft_printf("%s\n", "ft_split failed");
				else 
					cmd->pathname = msh_pathname_r(paths, cmd);
				ft_free_array(paths);
			}
		}
	}
}

void	msh_cmd(t_cmd *cmd, char **argv, char **envp)
{
	cmd->cmd = ft_split(argv[1], ' ');
	if (!cmd->cmd)
		ft_printf("%s\n", "split failed");
	cmd->envp = envp;
	msh_pathname(cmd);
	if (!cmd->pathname)
		ft_printf("%s\n", "oopsie");
}

int	main(int argc, char **argv, char **envp)
{
	t_cmd cmd;

	if (argc < 2)
		ft_printf("%s\n", "command is missing");
	msh_cmd(&cmd, argv, envp);
	msh_execute(&cmd);
	return (0);
}