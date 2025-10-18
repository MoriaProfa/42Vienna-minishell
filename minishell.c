/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:00:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 18:00:44 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

volatile sig_atomic_t	g_signal_flag = 0;

static void	msh_initialize(t_msh *msh)
{
	msh->error_check = false;
	msh->cpy_in_str = msh->in_str;
	msh->fd_pipe_read = -1;
	msh->fd_in = -1;
	msh->fd_out = -1;
	msh->fd[0] = -1;
	msh->fd[1] = -1;
	msh->saved_stdin = -1;
	msh->saved_stdout = -1;
}

static void	msh_pid(t_msh *msh)
{
	int	i;

	i = -1;
	msh->pid = (pid_t *)ft_calloc(msh->n_cmds + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_quit(msh, "pid malloc error", 1);
	while (++i < msh->n_cmds)
		msh->pid[i] = -1;
}

static void	init_beginning(t_msh *msh, char **envp)
{
	signal(SIGINT, handle_signal_parent);
	signal(SIGQUIT, SIG_IGN);
	ft_bzero(msh, sizeof(t_msh));
	init_env_vars(msh, envp);
	msh->line_number = 1;
}

static void	msh_handler(t_msh *msh)
{
	add_history(msh->in_str);
	msh_initialize(msh);
	check_quotationmarks(msh);
	build_envp_array(msh);
	if (!msh->error_check)
		(msh_tokenizer(msh));
	if (!msh->error_check)
		check_tokens(msh);
	if (!msh->error_check)
		(build_cmd_table(msh));
	if (!msh->error_check)
		msh_pid(msh);
	if (!msh->error_check && collect_all_heredocs(msh, -1) == 0)
		run_command(msh, -1);
	msh_free(msh);
}

int	main(int argc, char **argv, char **envp)
{
	t_msh	msh;

	if (argc > 1)
		return (ft_putendl_fd("No arguments accepted.", STDERR_FILENO), 1);
	(void)argv;
	init_beginning(&msh, envp);
	while (1)
	{
		if (g_signal_flag)
		{
			msh.status = 128 + g_signal_flag;
			g_signal_flag = 0;
		}
		msh.in_str = readline("minishell> ");
		if (!msh.in_str)
			break ;
		if (msh.in_str[0] == '\0' || check_for_whitespace_end(msh.in_str, 0))
		{
			free(msh.in_str);
			continue ;
		}
		msh_handler(&msh);
	}
	return (msh_quit(&msh, NULL, msh.status), 0);
}
