/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   msh_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 23:38:20 by astehl            #+#    #+#             */
/*   Updated: 2025/10/18 16:11:36 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

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
	close_fds(msh);
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	ft_lstclear(&msh->env_vars, free_env_var);
	free_envp(msh);
	msh_free(msh);
	rl_clear_history();
	exit(msh->status);
}
