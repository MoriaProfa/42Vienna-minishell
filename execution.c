/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 12:33:42 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/19 15:53:33 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_builtins(t_msh *msh, int i);

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

int	check_access_read(char *file)
{
	if (access(file, F_OK) == 0 && access(file, R_OK) != 0)
	{
		printf("%s: %s\n", file, "permission denied");
		return (0);
	}
	else
		return (1);
}

int	check_access_write(char *file)
{
	if (access(file, F_OK) == 0 && access(file, W_OK) != 0)
	{
		printf("%s: %s\n", file, "permission denied");
		return (0);
	}
	else
		return (1);
}

void	set_fd_in(t_msh *msh, int	i)
{
	msh->fd_in = msh->fd_in_previous;
	if (((t_cmd *)msh->cmd_table[i])->heredoc)
	{
		if (msh->fd_in != STDIN_FILENO)
			close(msh->fd_in);
		if (pipe(msh->hd_fd) == -1)
			msh_end(msh, "pipe failed");
		write(msh->hd_fd[1], ((t_cmd *)msh->cmd_table[i])->heredoc, ft_strlen(((t_cmd *)msh->cmd_table[i])->heredoc));
		close(msh->hd_fd[1]);
		msh->fd_in = msh->hd_fd[0];
	}
	else if (((t_cmd *)msh->cmd_table[i])->infile)
	{
		if (check_access_read(((t_cmd *)msh->cmd_table[i])->infile) == 0)
			return ;
		msh->fd_in = open(((t_cmd *)msh->cmd_table[i])->infile, O_RDONLY);
		if (msh->fd_in == -1)
			printf("%s: %s\n", ((t_cmd *)msh->cmd_table[i])->infile, "no such file or directory");
	}
	
}

void	set_fd_out(t_msh *msh, int	i)
{
	if (((t_cmd *)msh->cmd_table[i])->outfile && ((t_cmd *)msh->cmd_table[i])->append == 0)
	{
		if (check_access_write(((t_cmd *)msh->cmd_table[i])->outfile) == 0)
			return ;
		msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfile, O_RDWR | O_CREAT | O_TRUNC, 0666);
		if (msh->fd_out == -1)
			printf("%s: %s\n", ((t_cmd *)msh->cmd_table[i])->outfile, "no such file or directory1");
	}
	else if (((t_cmd *)msh->cmd_table[i])->outfile && ((t_cmd *)msh->cmd_table[i])->append == 1)
	{
		if (check_access_write(((t_cmd *)msh->cmd_table[i])->outfile) == 0)
			return ;
		msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfile, O_RDWR | O_CREAT | O_APPEND, 0666);
		if (msh->fd_out == -1)
			printf("%s: %s\n", ((t_cmd *)msh->cmd_table[i])->outfile, "no such file or directory2");
	}
}

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
		execve(((t_cmd *)msh->cmd_table[i])->pathname, ((t_cmd *)msh->cmd_table[i])->args, msh->envp);
	msh_end(msh, "execve failed");//maybe just use perror to print the errno-msg
}

void	close_fds(t_msh *msh, int fd1, int fd2)
{
	if (msh->fd_in != fd1)
		close(msh->fd_in);
	if (msh->fd_out != fd2)
		close(msh->fd_out);
	if (msh->fd[0] != fd1 && msh->fd[0] != -1)
		close(msh->fd[0]);
	if (msh->fd[1] != fd2 && msh->fd[1] != -1)
		close(msh->fd[1]);
}

void	msh_fork(t_msh *msh, int fd_in, int fd_out, int i)
{
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_end(msh, "fork failed");;
	if (msh->pid[i] == 0)
	{
		close_fds(msh, fd_in, fd_out);
		if (fd_in != STDIN_FILENO)
		{
			dup2(fd_in, STDIN_FILENO);
			close(fd_in);
		}
		if (fd_out != STDOUT_FILENO)
		{
			dup2(fd_out, STDOUT_FILENO);
			close(fd_out);
		}
		if (((t_cmd *)msh->cmd_table[i])->builtin.builtin == false)
			msh_execute(msh, i);
		else
		{
			msh_builtins(msh, i);
			exit (0);
		}
	}
}

void	msh_pipeline(t_msh *msh, int i)
{
	while (++i < msh->n_cmds)
	{
		set_fd_in(msh, i);
		set_fd_out(msh, i);
		if (i < msh->n_cmds - 1 && !((t_cmd *)msh->cmd_table[i])->outfile)
		{
			if (pipe(msh->fd) == -1)
				msh_end(msh, "pipe failed");
			msh->fd_out = msh->fd[1];
			msh->fd_in_previous = msh->fd[0];
		}
		if (((t_cmd *)msh->cmd_table[i])->builtin.builtin == true && msh->n_cmds == 1)
			msh_builtins(msh, i);
		else
			msh_fork(msh, msh->fd_in, msh->fd_out, i);
		if (msh->fd_in != STDIN_FILENO)
			close(msh->fd_in);
		if (msh->fd_out != STDOUT_FILENO)
			close(msh->fd_out);
	}
	i = -1;
	while (++i < msh->n_cmds)
		waitpid(msh->pid[i], &msh->status, 0);
	close_fds(msh, STDIN_FILENO, STDOUT_FILENO);
}
