/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 14:37:10 by edarmaev          #+#    #+#             */
/*   Updated: 2025/08/28 17:40:32 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_pid(t_msh *msh)
{
	int	i;

	i = -1;
	msh->pid = (pid_t *)ft_calloc(msh->n_cmds + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_end(msh, "pid malloc error");
	while (++i < msh->n_cmds)
	{
		msh->pid[i] = -1;
	}
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

void	msh_execute(t_msh *msh, int i)
{
	printf("in execution");
	if (!((t_cmd *)msh->cmd_table[i])->pathname)
	{
		msh->status = 127;
		msh_error(msh, "command not found");//should display a new prompt after the error msg (Bernhard said this is the only error-msg that needs to be hardcoded)
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname && access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == -1)
	{
		msh->status = 126;
		msh_error(msh, "permission denied");//maybe just use perror to print the errno-msg
	}
	else if (((t_cmd *)msh->cmd_table[i])->pathname && access(((t_cmd *)msh->cmd_table[i])->pathname, X_OK) == 0)
		execve(((t_cmd *)msh->cmd_table[i])->pathname, ((t_cmd *)msh->cmd_table[i])->args, msh->envp);
	msh_end(msh, "execve failed");//maybe just use perror to print the errno-msg
}

void	msh_fork(t_msh *msh, int i)
{
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_end(msh, "fork failed");;
	if (msh->pid[i] == 0)
	{
		msh_execute(msh, i);
	}
}

void	set_fd_in(t_msh *msh, int	i)
{
	if ((t_cmd *)msh->cmd_table[i]->infile)
	{
		msh->fd_in = open(((t_cmd *)msh->cmd_table[i])->infile, O_RDONLY);
		if (msh->fd_in == -1)
			msh_error(msh, "open(infile) failed");
	}
	else
	{
		msh->fd_in = dup(0);
		if (msh->fd_in == -1)
			msh_error(msh, "dup(0) failed");
	}
	printf("fd_in = %d\n", msh->fd_in);
}

void	set_fd_out(t_msh *msh, int	i)
{
	if ((t_cmd *)msh->cmd_table[i]->outfile)
	{
		msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfile, O_RDWR, O_CREAT);
		if (msh->fd_out == -1)
			msh_error(msh, "open(outfile) failed");
	}
	else
	{
		msh->fd_out = dup(1);
		if (msh->fd_out == -1)
			msh_error(msh, "dup(1) failed");
	}
	printf("fd_out = %d\n", msh->fd_out);
}

void	msh_pipeline(t_msh *msh, int i)
{
	// int	tmp_in = dup(0);
	// int	tmp_out = dup(1);
	set_fd_in(msh, i);
	while (i < msh->n_cmds)
	{
		if (dup2(msh->fd_in, STDIN_FILENO) == -1)
			msh_error(msh, "dup2 stdin failed");
		close(msh->fd_in);
		if (i == msh->n_cmds - 1)
			set_fd_out(msh, i);
		else
		{
			if (pipe(msh->fd) == -1)
				msh_end(msh, "pipe failed");
			msh->fd_in = msh->fd[0];
			msh->fd_out = msh->fd[1];
		}
		if (dup2(msh->fd_out, STDOUT_FILENO) == -1)
			msh_error(msh, "dup2 stdin failed");
		close(msh->fd_out);
		//msh_fork(msh, i);
		msh->pid[i] = fork();
		if (msh->pid[i] == -1)
			msh_end(msh, "fork failed");;
		if (msh->pid[i] == 0)
		{
			msh_execute(msh, i);
		}
		waitpid(msh->pid[i], &msh->status, 0);
		i++;
	}
}
