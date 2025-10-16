/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 12:33:42 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/16 21:37:37 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_builtins(t_msh *msh, int i);
void	close_heredocs(t_msh *msh);
void	print_heredoc_fd(t_msh *msh);
void	save_stdin_stdout(t_msh *msh, int i);
void	restore_stdin_stdout(t_msh *msh);
void	close_fds(t_msh *msh);
int		check_if_directory(char *path);
void	print_execution_error(t_msh *msh, char *cmd, int option, int code);
void	msh_print(t_msh *msh, char *msg, int status);
bool	compare_strings(char *s1, char *s2);
void	handle_signal(int signal);

void	msh_pid(t_msh *msh) // could be put in cmd_table
{
	int i;

	i = -1;
	msh->pid = (pid_t *)ft_calloc(msh->n_cmds + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_quit(msh, "pid malloc error", 1);
	while (++i < msh->n_cmds)
		msh->pid[i] = -1;
}

bool	special_case_check(t_msh *msh, char *pathname, char *cmd)
{
	if (!pathname)
	{
		print_execution_error(msh, cmd, 1, 127);
		return (true);
	}
	if (compare_strings(cmd, ".") == true)
	{
		msh_print(msh,
			"minishell: .: filename argument required\n.: usage: . filename [arguments]",
			2);
		return (true);
	}
	if (compare_strings(cmd, "..") == true && compare_strings(pathname,
			"/..") == false)
	{
		print_execution_error(msh, cmd, 1, 127);
		return (true);
	}
	return (false);
}

void	msh_execute(t_msh *msh, int i)
{
	char	*pathname;
	char	*cmd;

	pathname = ((t_cmd *)msh->cmd_table[i])->pathname;
	cmd = ((t_cmd *)msh->cmd_table[i])->args[0];
	if (special_case_check(msh, pathname, cmd) == true)
		return ;
	execve(pathname, ((t_cmd *)msh->cmd_table[i])->args, msh->envp);
	if (errno == ENOENT)
		return (print_execution_error(msh, cmd, 2, 127));
	else if (errno == EACCES)
	{
		if (check_if_directory(pathname))
			return (print_execution_error(msh, cmd, 3, 126));
		else if (access(pathname, X_OK) == -1)
			return (print_execution_error(msh, cmd, 4, 126));
	}
	else
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(((t_cmd *)msh->cmd_table[i])->args[0]);
		msh_print(msh, NULL, 126);
		return ;
	}
}
void	handle_signal_child(int sig)
{
	if (sig == SIGINT)
		g_signal_flag = sig;
	rl_on_new_line();
	rl_replace_line("", 0);
}

void	child_process(t_msh *msh, int i)
{
	signal(SIGINT, handle_signal_child); // need to check signal handling in a child
	signal(SIGQUIT, handle_signal_child);
	if (set_redirections(msh, i) != 0)
		msh_quit(msh, NULL, 1);
	if (msh->fd[0] != -1)
		close(msh->fd[0]);
	if (((t_cmd *)msh->cmd_table[i])->args[0]
		&& ((t_cmd *)msh->cmd_table[i])->builtin.builtin == false)
		msh_execute(msh, i);
	else if (((t_cmd *)msh->cmd_table[i])->args[0]
		&& ((t_cmd *)msh->cmd_table[i])->builtin.builtin == true)
	{
		signal(SIGPIPE, SIG_IGN);
		msh_builtins(msh, i);
	}
	close_fds(msh);
	msh_quit(msh, NULL, msh->status);
}

void	msh_fork(t_msh *msh, int i)
{
	msh->pid[i] = fork();
	if (msh->pid[i] == -1)
		msh_quit(msh, "fork failed", 1);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	if (msh->pid[i] == 0)
		child_process(msh, i);
	if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
		close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
	if (msh->fd_pipe_read != -1)
		close(msh->fd_pipe_read);
	if (i < msh->n_cmds - 1)
	{
		if (msh->fd[1] != -1)
			close(msh->fd[1]);
		msh->fd_pipe_read = msh->fd[0];
	}
}

void	msh_waitpid(t_msh *msh, int i)
{
	int	wstatus;
	int	signal;

	if (waitpid(msh->pid[i], &wstatus, 0) == -1)
		msh_quit(msh, "child waitpid failed", 1);
	if (WIFEXITED(wstatus))
		msh->status = WEXITSTATUS(wstatus);
	else if (WIFSIGNALED(wstatus))
	{
		signal = WTERMSIG(wstatus);
		msh->status = 128 + signal;
		if (signal == SIGINT)
			write(STDOUT_FILENO, "\n", 1);
		else if (signal == SIGQUIT)
			write(STDOUT_FILENO, "Quit (core dumped)\n", 19);
	}
}

void	run_builtins(t_msh *msh, int i)
{
	save_stdin_stdout(msh, i);
	if (set_redirections(msh, i) == 0)
		msh_builtins(msh, i);
	else
		msh->status = 1;
	restore_stdin_stdout(msh);
}

void	run_command(t_msh *msh, int i)
{
	while (++i < msh->n_cmds)
	{
		if (msh->n_cmds == 1 && ((t_cmd *)msh->cmd_table[i])->args[0]
			&& ((t_cmd *)msh->cmd_table[i])->builtin.builtin == true)
			run_builtins(msh, i);
		else
		{
			if (i < msh->n_cmds - 1)
			{
				if (pipe(msh->fd) == -1)
					msh_quit(msh, "pipe failed", 1);
			}
			msh_fork(msh, i);
		}
	}
	i = -1;
	while (++i < msh->n_cmds && msh->pid && msh->pid[i] != -1)
		msh_waitpid(msh, i);
	signal(SIGINT, handle_signal);
	signal(SIGQUIT, SIG_IGN);
}
