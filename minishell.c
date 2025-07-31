/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:00:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/05/07 15:18:17 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//valgrind command:
//valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=minishell.supp ./minishell

#include "minishell.h"

volatile sig_atomic_t   signal_flag = 0;

void	msh_cmd_free(char ***cmd)
{
	int	i;

	if (!cmd)
		return ;
	i = 0;
	while (cmd[i])
	{
		ft_array_free(cmd[i]);
		cmd[i++] = NULL;
	}
	free(cmd);
}

void	msh_free(t_msh *msh)
{
	if (msh->in_str)
		ft_free(&msh->in_str);
	if (msh->in_arr)
	{
		ft_array_free(msh->in_arr);
		msh->in_arr = NULL;
	}
	if (msh->cmd)
	{
		msh_cmd_free(msh->cmd);
		msh->cmd = NULL;
	}
	if (msh->pathname)
	{
		ft_array_free(msh->pathname);
		msh->pathname = NULL;
	}
	if (msh->pid)
	{
		free(msh->pid);
		msh->pid = NULL;
	}
}

void	msh_error(t_msh *msh, char *msg, int status)
{
	ft_putendl_fd(msg, 2);
	msh_free(msh);
	rl_clear_history();
	exit(status);
}

void	return_prompt()
{
	rl_replace_line("", 0);
	ioctl(0, TIOCSTI, "\n");
	rl_on_new_line();
}

void	msh_execute(t_msh *msh, int i)
{
	if (!msh->pathname[i])
	/*{
		//ft_putstr_fd(msh->pathname[i], 2);
		ft_putstr_fd("command not found", 2);
		return_prompt();
		return ;
	}*/
		msh_error(msh, "command not found", 127);//should not exit but display a new prompt after the error msg (Bernhard said this is the only error-msg that needs to be hardcoded)
	else if (msh->pathname[i] && access(msh->pathname[i], X_OK) == -1)
	/*{
		perror("Pathname");
		return_prompt();
		return ;
	}*/
		msh_error(msh, "permission denied", 126);//should also not exit at this point but display a new prompt after the error msg, maybe just use perror to print the errno-msg
	else if (msh->pathname[i] && access(msh->pathname[i], X_OK) == 0)
		execve(msh->pathname[i], msh->cmd[i], msh->envp);
	msh_error(msh, "execve failed", msh->status);//should also not exit at this point but display a new prompt after the error msg, maybe just use perror to print the errno-msg
}

char	*msh_pathname_r(t_msh *msh, char **paths, int i)
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
			msh_error(msh, "pathname tmp ft_strjoin failed", msh->status);
		pathname = ft_strjoin(tmp, msh->cmd[i][0]);
		free(tmp);
		if (!pathname)
			msh_error(msh, "pathname ft_strjoin failed", msh->status);
		if (access(pathname, F_OK) == 0)
			return (pathname);
		free(pathname);
	}
	return (NULL);
}

char	*msh_pathname(t_msh *msh, int i)
{
	char	**paths;
	char	*pathname;
	int		j;

	j = 0;
	pathname = NULL;
	if (access(msh->cmd[i][0], F_OK) == 0)
		pathname = msh->cmd[i][0];
	else
	{
		while (msh->envp[j] && ft_strncmp(msh->envp[j], "PATH=", 5) != 0)
			j++;
		if (msh->envp[j])
		{
			paths = ft_split(msh->envp[j] + 5, ':');
			if (!paths)
				msh_error(msh, "paths ft_split failed", msh->status);
			pathname = msh_pathname_r(msh, paths, i);
			ft_array_free(paths);
		}
	}
	return (pathname);
}
/* useless because of use of readline atm
char	*msh_read(ssize_t rr)
{
	char	buf[2];
	char	*tmp;
	char	*out;

	out = NULL;
	while (rr > 0)
	{
		rr = read(0, buf, 1);
		if (rr == -1)
			return (ft_free(&out), NULL);
		buf[rr] = 0;
		if (buf[0] == '\n')
			return (out);
		if (!out)
			out = ft_strcpy(buf);
		else
		{
			tmp = out;
			out = ft_strjoin(tmp, buf);
			ft_free(&tmp);
		}
	}
	return (out);
}
*/

void	msh_fork(t_msh *msh, int i, int fd_in, int fd_out)
{
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_error(msh, "fork failed", msh->status);
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

void	msh_initialize(t_msh *msh, char **envp)
{
	int	i;

	i = 0;
	msh->status = 1;
	msh->envp = envp; //todo: new function to iterate through original envp, count envp variables, ft_calloc msh->envp and copy all variables of original envp to msh->envp
	msh->in_arr = ft_split(msh->in_str, '|');
	if (!msh->in_arr)
		msh_error(msh, "in_arr split failed", msh->status);
	msh->n = (int)ft_array_size(msh->in_arr);
	msh->cmd = (char ***)ft_calloc(msh->n + 1, sizeof(char **));
	if (!msh->cmd)
		msh_error(msh, "cmd malloc failed", msh->status);
	msh->pathname = (char **)ft_calloc(msh->n + 1, sizeof(char *));
	if (!msh->pathname)
		msh_error(msh, "pathname malloc oopsie", msh->status);
	while (msh->in_arr[i])
	{
		msh->cmd[i] = ft_split(msh->in_arr[i], ' ');
		if (!msh->cmd[i])
			msh_error(msh, "cmd plit failed", msh->status);
		msh->pathname[i] = msh_pathname(msh, i);
		i++;
	}
	msh->pid = (pid_t *)ft_calloc(msh->n + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_error(msh, "pid malloc error", msh->status);
	i = -1;
	while (++i < msh->n)
		msh->pid[i] = -1;
	msh->fd[0] = -1;
	msh->fd[1] = -1;
	msh->fd_in = -1;
	msh->fd_out = -1;
}

void	msh_pipeline(t_msh *msh, int i)
{
	while (i < msh->n)
	{
		if (i < msh->n - 1)
		{
			if (pipe(msh->fd) == -1)
				msh_error(msh, "pipe failed", msh->status);
			msh->fd_out = msh->fd[1];
		}
		msh_fork(msh, i, msh->fd_in, msh->fd_out);
		if (msh->fd_in != -1)
			close(msh->fd_in);
		if (i < msh->n - 1)
		{
			close(msh->fd[1]);// close write end
			msh->fd_in = msh->fd[0];// keep read end for next command
		}
		i++;
	}
	i = 0;
	while (i < msh->n)
	{
		waitpid(msh->pid[i], &msh->status, 0);
		i++;
	}
}

void	handle_signal(int signal)
{
	if (signal == SIGINT)
	{
		signal_flag = signal + 128;
		return_prompt();
	}	
}

int	main(int argc, char **argv, char **envp)
{
	t_msh				msh;
	
	if (argc > 1)
		return (ft_putendl_fd("No arguments accepted.", 2), 1);
	(void)argv;
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, SIG_IGN);
	ft_bzero(&msh, sizeof(t_msh));
	while (1)
	{
		
		msh.in_str = readline("minishell> ");
		if (!msh.in_str) //by pressing CTRL+D
			break ;
		add_history(msh.in_str);
		if (signal_flag)
		{
			free(msh.in_str);
			msh.in_str = NULL;
			msh.status = signal_flag;
			signal_flag = 0;
			continue ;
		}
		msh_initialize(&msh, envp); //envp is also accessible with __environ (=global env variable)
		msh_pipeline(&msh, 0);
		msh_free(&msh);
	}
	msh_error(&msh, "exit", msh.status);
	return (0);
}
