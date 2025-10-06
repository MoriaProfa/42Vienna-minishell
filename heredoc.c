/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/06 19:35:06 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_value(char *value, char *result, t_msh *msh);

void	expand_heredoc_input(t_msh *msh, char **value)
{
	char	*result;

	result = ft_strdup("");
	if (!result)
		msh_exit(msh, "heredoc expansion malloc failed", 1);
	result = expand_value(*value, result, msh);
	free(*value);
	*value = result;
}

void	collect_one_heredoc(t_msh *msh, t_cmd *cmd, int j)
{
	while (1)
	{
		cmd->heredocs[j].buffer = readline("> ");
		if (!cmd->heredocs[j].buffer)
			break ;
		if (cmd->heredocs[j].expand == true)
			expand_heredoc_input(msh, &cmd->heredocs[j].buffer);
		if (ft_strlen(cmd->heredocs[j].buffer) == ft_strlen(cmd->heredocs[j].delimiter)
			&& ft_strncmp(cmd->heredocs[j].buffer, cmd->heredocs[j].delimiter,
				ft_strlen(cmd->heredocs[j].buffer)) == 0)
		{
			free(cmd->heredocs[j].buffer);
			cmd->heredocs[j].buffer = NULL;
			break ;
		}
		write(msh->fd_hd[1], cmd->heredocs[j].buffer,
			ft_strlen(cmd->heredocs[j].buffer));
		write(msh->fd_hd[1], "\n", 1);
		free(cmd->heredocs[j].buffer);
		cmd->heredocs[j].buffer = NULL;
	}
}

void	collect_all_heredocs(t_msh *msh, int i, int j)
{
	while (++i < msh->n_cmds)
	{
		while (++j < ((t_cmd *)msh->cmd_table[i])->n_hd)
		{
			if (((t_cmd *)msh->cmd_table[i])->fd_heredoc != -1)
				close(((t_cmd *)msh->cmd_table[i])->fd_heredoc);
			if (pipe(msh->fd_hd) == -1)
				msh_exit(msh, "heredoc pipe failed", 1);
			collect_one_heredoc(msh, (t_cmd *)msh->cmd_table[i], j);
			close(msh->fd_hd[1]);
			((t_cmd *)msh->cmd_table[i])->fd_heredoc = msh->fd_hd[0];
		}
	}
}
