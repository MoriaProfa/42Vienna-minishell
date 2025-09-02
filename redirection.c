/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 14:37:10 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/02 17:10:23 by edarmaev         ###   ########.fr       */
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

int	check_access(char *file, t_msh *msh)
{
	if (access(file, F_OK) == 0 && access(file, R_OK) != 0)
	{
		printf("%s: ", file);
		msh_error(msh, "permission denied");
		return (0);
	}
	else
		return (1);
}

void	set_fd_in(t_msh *msh, int	i)
{
	if (((t_cmd *)msh->cmd_table[i])->infile)
	{
		if (check_access(((t_cmd *)msh->cmd_table[i])->infile, msh) == 1)
			msh->fd_in = open(((t_cmd *)msh->cmd_table[i])->infile, O_RDONLY);
		if (msh->fd_in == -1)
		{
			printf("%s: ", ((t_cmd *)msh->cmd_table[i])->infile);
			msh_error(msh, "no such file or directory");
		}
	}
	else
	{
		msh->fd_in = dup(STDIN_FILENO);
		if (msh->fd_in == -1)
			msh_error(msh, "dup(0) failed");
	}
	//printf("fd_in = %d\n", msh->fd_in);
}

void	set_fd_out(t_msh *msh, int	i)
{
	if (((t_cmd *)msh->cmd_table[i])->outfile && ((t_cmd *)msh->cmd_table[i])->append == 0)
	{
		if (check_access(((t_cmd *)msh->cmd_table[i])->outfile, msh) == 1)
			msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfile, O_RDWR, O_CREAT, O_TRUNC);
		if (msh->fd_out == -1)
		{
			printf("%s: ", ((t_cmd *)msh->cmd_table[i])->outfile);
			msh_error(msh, "no such file or directory");
		}
	}
	else if (((t_cmd *)msh->cmd_table[i])->outfile && ((t_cmd *)msh->cmd_table[i])->append == 1)
	{
		if (check_access(((t_cmd *)msh->cmd_table[i])->outfile, msh) == 1)
			msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfile, O_RDWR, O_CREAT, O_APPEND);
		if (msh->fd_out == -1)
		{
			printf("%s: ", ((t_cmd *)msh->cmd_table[i])->outfile);
			msh_error(msh, "no such file or directory");
		}
	}
	else
	{
		msh->fd_out = dup(STDOUT_FILENO);
		if (msh->fd_out == -1)
			msh_error(msh, "dup(1) failed");
	}
	//printf("fd_out = %d\n", msh->fd_out);
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
		set_fd_in(msh, i);
		set_fd_out(msh, i);
		dup2(((t_cmd *)msh->cmd_table[i])->fd[0], msh->fd_in);
		close(((t_cmd *)msh->cmd_table[i])->fd[0]);
		dup2(msh->fd_in, STDIN_FILENO);
		close(msh->fd_in);
		dup2(((t_cmd *)msh->cmd_table[i])->fd[1], msh->fd_out);
		close(((t_cmd *)msh->cmd_table[i])->fd[1]);
		dup2(msh->fd_out, STDOUT_FILENO);
		close(msh->fd_out);
		msh_execute(msh, i);
	}
}

void	msh_pipeline(t_msh *msh, int i)
{
	while (++i < msh->n_cmds)
	{
		if (i < msh->n_cmds - 1)
		{
			if (pipe(((t_cmd *)msh->cmd_table[i])->fd) == -1)
				msh_end(msh, "pipe failed");
			printf("fd[0] = %d\nfd[1] = %d\n", ((t_cmd *)msh->cmd_table[i])->fd[0], ((t_cmd *)msh->cmd_table[i])->fd[1]);
		}
		msh_fork(msh, i);
	}
	i = 0;
	while (i < msh->n_cmds)
	{
		waitpid(msh->pid[i], &msh->status, 0);
		i++;
	}
}
