/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:00:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 01:19:17 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

//valgrind command:
//valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=all --trace-children=yes --suppressions=minishell.supp ./minishell
//pwd test < file1 > file2 >> file3 test2 << file4 << "file5" | cd hello | ls test3 test4 test5 test6 test7

#include "minishell.h"

void	print_tokens(t_list *tokens);//for testing only
void	print_cmd_table(t_msh *msh);//for testing only

volatile sig_atomic_t   g_signal_flag = 0;

void	msh_print(t_msh *msh, char *msg, int status)
{
	int	fd;

	msh->status = status;
	if (msh->status != 0)
		fd = STDERR_FILENO;
	else
		fd = STDOUT_FILENO;
	if (msg)
		ft_putendl_fd(msg, fd);
	if (msh->status != 0)
		msh->error_check = true;
}

void	msh_free(t_msh *msh)
{
	if (msh->in_str)
		free(msh->in_str);
	ft_lstclear(&msh->tokens, free_token);
	free_cmd_table(msh->cmd_table, msh->n_cmds);
	msh->cmd_table = NULL;
	msh->n_cmds = 0;	
	if (msh->pid)
		free(msh->pid);
	msh->pid = NULL;
}

void	msh_quit(t_msh *msh, char *msg, int status)
{
	int	fd;

	msh->status = status;
	if (msh->status != 0)
		fd = STDERR_FILENO;
	else
		fd = STDOUT_FILENO;
	if (msg)
		ft_putendl_fd(msg, fd);
	restore_stdin_stdout(msh);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	ft_lstclear(&msh->env_vars, free_env_var);
	free_envp(msh);
	msh_free(msh);
	rl_clear_history();
	exit(msh->status);
}

void	msh_initialize(t_msh *msh)
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

void	msh_pid(t_msh *msh)
{
	int	i;

	i = -1;
	msh->pid = (pid_t *)ft_calloc(msh->n_cmds + 1, sizeof(pid_t));
	if (!msh->pid)
		msh_quit(msh, "pid malloc error", 1);
	while (++i < msh->n_cmds)
		msh->pid[i] = -1;
}

int	main(int argc, char **argv, char **envp)
{
	t_msh				msh;
	
	if (argc > 1)
		return (ft_putendl_fd("No arguments accepted.", STDERR_FILENO), 1);
	(void)argv;
	signal(SIGINT, handle_signal_parent);
	signal(SIGQUIT, SIG_IGN);
	ft_bzero(&msh, sizeof(t_msh));
	init_env_vars(&msh, envp); //envp is also accessible with __environ (=global env variable)
	//print_env_vars(msh.env_vars);//just for testing if init_env_var worked correctly
	msh.line_number = 1;
	while (1)
	{
		if (g_signal_flag)
		{
			//free(msh.in_str);
			msh.in_str = NULL;
			msh.status = 128 + g_signal_flag;
			g_signal_flag = 0;
			continue ;
		}
		msh.in_str = readline("minishell> ");
		if (!msh.in_str) //by pressing CTRL+D
			break ;//msh_quit(&msh, "exit", 0);//didnt work as intended for not yet known reason?!
		if (msh.in_str[0] == '\0' || check_for_whitespace_end(msh.in_str, 0))
		{
			free(msh.in_str);
			continue ;
		}
		add_history(msh.in_str); //original bash doesnt add it when its starting with a whitespace
		msh_initialize(&msh);
		check_quotationmarks(&msh);
		build_envp_array(&msh);
		if (!msh.error_check)
			(msh_tokenizer(&msh)/* , print_tokens(msh.tokens) */); //print just for testing
		if (!msh.error_check)
			check_tokens(&msh);
		if (!msh.error_check)
			(build_cmd_table(&msh)/* , print_cmd_table(&msh) */); //print just for testing
		if (!msh.error_check)
			msh_pid(&msh); //allocates for msh.pid
/*		if (!msh.error_check)
		{
			if (((t_cmd *)msh.cmd_table[0])->builtin.builtin == true && msh.n_cmds == 1)//why only if n_cmds is 1?
			msh_builtins(&msh, 0);
		}
*/		if (!msh.error_check/* && collect_all_heredocs(&msh, -1) == 0*/)
			run_command(&msh, -1);
		msh_free(&msh);
	}
	msh_quit(&msh, NULL, msh.status);
	return (0);
}
