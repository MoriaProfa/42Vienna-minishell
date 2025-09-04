/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 12:33:42 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/04 20:57:55 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_execute(t_msh *msh, int i)
{
	if (!((t_cmd *)msh->cmd_table[i])->pathname)
	{
		msh->status = 127;
		msh_end(msh, "command not found");//should display a new prompt after the error msg (Bernhard said this is the only error-msg that needs to be hardcoded)
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname && access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == -1)
	{
		msh->status = 126;
		msh_end(msh, "permission denied");//maybe just use perror to print the errno-msg
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname && access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == 0)
		execve(((t_cmd *)msh->cmd_table[i])->pathname, /*((t_cmd *)msh->cmd_table[i])->args*/msh->cmd_table[i]->arg_array, msh->envp);
	msh_end(msh, "execve failed");//maybe just use perror to print the errno-msg
}

char	*resolve_cmd_pathname(t_msh *msh, char **paths, char *cmd)
{
	char	*pathname;
	char	*tmp;
	int		j;

	j = -1;
	pathname = NULL;
	while (paths[++j])
	{
		tmp = ft_strjoin(paths[j], "/");
		if (!tmp)
			msh_end(msh, "pathname tmp ft_strjoin failed");
		pathname = ft_strjoin(tmp, cmd);
		free(tmp);
		if (!pathname)
			msh_end(msh, "pathname ft_strjoin failed");
		if (access(pathname, F_OK) == 0)
			return (pathname);
		free(pathname);
	}
	return (NULL);
}

char	*create_cmd_pathname(t_msh *msh, char *cmd)
{
	char	**paths;
	char	*pathname;
	int		j;

	j = 0;
	pathname = NULL;
	if (access(cmd, F_OK) == 0)
	{
		pathname = ft_strdup(cmd);
		if (!pathname)
			msh_end(msh, "pathname malloc failed");
		return (pathname);
	}
	while (msh->envp[j] && ft_strncmp(msh->envp[j], "PATH=", 5) != 0)
		j++;
	if (msh->envp[j])
	{
		paths = ft_split(msh->envp[j] + 5, ':');
		if (!paths)
			msh_end(msh, "paths ft_split failed");
		pathname = resolve_cmd_pathname(msh, paths, cmd);
		ft_array_free(paths);
	}
	return (pathname);
}

void	msh_fork(t_msh *msh, int i, int fd_in, int fd_out)
{
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_end(msh, "fork failed");
	if (msh->pid[i] == 0)
	{
		if (fd_in != -1)
			dup2(fd_in, STDIN_FILENO);
		if (fd_out != -1)
			dup2(fd_out, STDOUT_FILENO);
		if (fd_in != -1)
			close(fd_in);
		if (fd_out != -1)
			close(fd_out);
		msh_execute(msh, i);
	}
}

void	msh_pathname_pid(t_msh *msh)
{
	int	i;

	i = -1;
	/*msh->pathname = (char **)ft_calloc(msh->n_cmds + 1, sizeof(char *));
	if (!msh->pathname)
		msh_end(msh, "pathname malloc oopsie");*/
	msh->pid = (pid_t *)ft_calloc(msh->n_cmds + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_end(msh, "pid malloc error");
	while (++i < msh->n_cmds)
	{
		//msh->pathname[i] = msh_pathname(msh, i);
		msh->pid[i] = -1;
	}
}
		

void	msh_pipeline(t_msh *msh, int i)
{
	while (i < msh->n_cmds)
	{
		if (i < msh->n_cmds - 1)
		{
			if (pipe(msh->fd) == -1)
				msh_end(msh, "pipe failed");
			msh->fd_out = msh->fd[1];
		}
		msh_fork(msh, i, msh->fd_in, msh->fd_out);
		if (msh->fd_in != -1)
			close(msh->fd_in);
		if (i < msh->n_cmds - 1)
		{
			close(msh->fd[1]);// close write end
			msh->fd_in = msh->fd[0];// keep read end for next command
		}
		i++;
	}
	i = 0;
	while (i < msh->n_cmds)
	{
		waitpid(msh->pid[i], &msh->status, 0);
		i++;
	}
}
