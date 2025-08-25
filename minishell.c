/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:00:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/08/25 18:29:32 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//valgrind command:
//valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --suppressions=minishell.supp ./minishell

#include "minishell.h"

void print_tokens(t_list *tokens);
void print_cmd_table(t_msh *msh);
void	check_quotationmarks(t_msh *msh);
void	free_token(void *content);
void	msh_tokenizer(t_msh *msh);
void   build_cmd_table(t_msh *msh);
void    free_cmd_table(t_cmd **cmds, int n);

volatile sig_atomic_t   g_signal_flag = 0;

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

void	msh_set_null(t_msh *msh)
{
	msh->in_arr = NULL;
	msh->cmd = NULL;
	msh->pathname = NULL;
	msh->pid = NULL;
	msh->cmd_table = NULL;
	msh->n_cmds = 0;
}

void	msh_free(t_msh *msh)
{
	if (msh->in_str)
		ft_free(&msh->in_str);
	if (msh->in_arr)
		ft_array_free(msh->in_arr);
	if (msh->cmd)
		msh_cmd_free(msh->cmd);
	if (msh->pathname)
		ft_array_free(msh->pathname);
	if (msh->pid)
		free(msh->pid);
	ft_lstclear(&msh->tokens, free_token);
	free_cmd_table(msh->cmd_table, msh->n_cmds);
	msh_set_null(msh);
}

void	envp_free(t_msh *msh)
{
	if (msh->envp)
	{
		ft_array_free(msh->envp);
		msh->envp = NULL;
	}
}

void	msh_end(t_msh *msh, char *msg)
{
	int	fd;

	if (msh->status != 0)
		fd = 2;
	else
		fd = 1;
	ft_putendl_fd(msg, fd);
	envp_free(msh);
	msh_free(msh);
	rl_clear_history();
	exit(msh->status);
}

void	return_prompt(void)
{
	rl_replace_line("", 0);
	ioctl(0, TIOCSTI, "\n");
	rl_on_new_line();
}

void	msh_execute(t_msh *msh, int i)
{
	if (!msh->pathname[i])
	{
		msh->status = 127;
		msh_end(msh, "command not found");//should display a new prompt after the error msg (Bernhard said this is the only error-msg that needs to be hardcoded)
	}
	else if (msh->pathname[i] && access(msh->pathname[i], X_OK) == -1)
	{
		msh->status = 126;
		msh_end(msh, "permission denied");//maybe just use perror to print the errno-msg
	}
	else if (msh->pathname[i] && access(msh->pathname[i], X_OK) == 0)
		execve(msh->pathname[i], ((t_cmd *)msh->cmd_table[i])->args, msh->envp);
	msh_end(msh, "execve failed");//maybe just use perror to print the errno-msg
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
			msh_end(msh, "pathname tmp ft_strjoin failed");
		pathname = ft_strjoin(tmp, ((t_cmd *)msh->cmd_table[i])->args[0]);
		free(tmp);
		if (!pathname)
			msh_end(msh, "pathname ft_strjoin failed");
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
	if (access(((t_cmd *)msh->cmd_table[i])->args[0], F_OK) == 0)
		pathname = ((t_cmd *)msh->cmd_table[i])->args[0];
	else
	{
		while (msh->envp[j] && ft_strncmp(msh->envp[j], "PATH=", 5) != 0)
			j++;
		if (msh->envp[j])
		{
			paths = ft_split(msh->envp[j] + 5, ':');
			if (!paths)
				msh_end(msh, "paths ft_split failed");
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
void	envp_initialize(t_msh *msh, char **envp)
{
	msh->envp = ft_array_copy(envp);
	if (!msh->envp)
		msh_end(msh, "envp copy malloc failed");
}

void	msh_initialize(t_msh *msh)
{
	msh->cpy_in_str = msh->in_str;
	msh->status = 1;
	msh->fd[0] = -1;
	msh->fd[1] = -1;
	msh->fd_in = -1;
	msh->fd_out = -1;
}

void	msh_parser(t_msh *msh)
{
	int	i;

	i = 0;
	/*msh->in_arr = ft_split(msh->in_str, '|');
	if (!msh->in_arr)
		msh_end(msh, "in_arr split failed");
	msh->n = (int)ft_array_size(msh->in_arr);
	msh->cmd = (char ***)ft_calloc(msh->n + 1, sizeof(char **));
	if (!msh->cmd)
		msh_end(msh, "cmd malloc failed");*/
	msh->pathname = (char **)ft_calloc(msh->n_cmds + 1, sizeof(char *));
	if (!msh->pathname)
		msh_end(msh, "pathname malloc oopsie");
	while (i < msh->n_cmds)
	{
		/*msh->cmd[i] = ft_split(msh->in_arr[i], ' ');
		if (!msh->cmd[i])
			msh_end(msh, "cmd plit failed");*/
		msh->pathname[i] = msh_pathname(msh, i);
		i++;
	}
	msh->pid = (pid_t *)ft_calloc(msh->n + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_end(msh, "pid malloc error");
	i = -1;
	while (++i < msh->n_cmds)
		msh->pid[i] = -1;
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

void	handle_signal(int signal)
{
	if (signal == SIGINT)
	{
		g_signal_flag = signal + 128;
		return_prompt();
	}	
}

t_cmd   *init_cmd(void);
void    add_to_args(t_cmd *cmd, char *word);

int	main(int argc, char **argv, char **envp)
{
	t_msh				msh;
	
	if (argc > 1)
		return (ft_putendl_fd("No arguments accepted.", 2), 1);
	(void)argv;
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, SIG_IGN);
	ft_bzero(&msh, sizeof(t_msh));
	envp_initialize(&msh, envp); //envp is also accessible with __environ (=global env variable)
	while (1)
	{
		msh.in_str = readline("minishell> ");
		if (!msh.in_str) //by pressing CTRL+D
			break ;
		add_history(msh.in_str);
		if (g_signal_flag)
		{
			free(msh.in_str);
			msh.in_str = NULL;
			msh.status = g_signal_flag;
			g_signal_flag = 0;
			continue ;
		}
		msh_initialize(&msh);
		check_quotationmarks(&msh);
		msh_tokenizer(&msh);
		//print_tokens(msh.tokens); //just for testing
		build_cmd_table(&msh);
		//print_cmd_table(&msh); //just for testing
		msh_parser(&msh); //doesnt do anything with tokens yet
		int i = 0;
		while (i < msh.n_cmds)
		{
			printf("msh.table[%d].args[0] = %s\n", i, ((t_cmd *)msh.cmd_table[i])->args[0]);
			printf("msh.pathname[%d] = %s\n", i, msh.pathname[i]);
			i++;
		}
		msh_pipeline(&msh, 0);
		msh_free(&msh);
	}
	msh_end(&msh, "exit");
	return (0);
}
