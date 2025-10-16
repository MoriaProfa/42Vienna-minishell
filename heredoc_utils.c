/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 05:18:53 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/16 17:03:24 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
void	collect_heredocs_in_child(t_msh *msh, int i, int j, int fd)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		msh_quit(msh, "heredoc fork failed", 1);
	else if (pid == 0)
	{
		// signal(SIGINT, SIG_DFL);
		// signal(SIGQUIT, SIG_IGN); // ignore Ctrl+\ during heredoc
		collect_one_heredoc(msh, ((t_cmd *)msh->cmd_table[i])->heredocs[j], fd);
		close(fd);
		msh_quit(msh, NULL, 0);
	}
	else
	{
		close(fd);
		if (waitpid(pid, &status, 0) == -1) // protect from failing
			msh_quit(msh, "waitpid failed while collecting heredoc", 1);
		if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
		{
			msh->status = 130;
			write(1, "\n", STDOUT_FILENO);
			// msh_quit(msh, NULL, msh->status); // trying stuff
			return ; // stop heredoc collection & skip execution
		}
	}
}
void	print_heredoc_fd(t_msh *msh)
{
	int		i;
	t_cmd	*cmd;

	i = 0;
	if (!msh->cmd_table)
		return ;
	while (msh->cmd_table[i] && i < msh->n_cmds)
	{
		cmd = msh->cmd_table[i];
		printf("cmd [%d] heredoc_read = %d\n", i, cmd->heredoc_read);
		i++;
	}
}

*/