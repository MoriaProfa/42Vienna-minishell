/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/08 13:57:20 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*expand_value(char *value, char *result, t_msh *msh);

void	print_heredoc_fd(t_msh *msh)
{
	int		i;
	t_cmd	*cmd;

	i = 0;
	if (!msh->cmd_table)
		return ;
	printf("\n=== Command Table (n = %i) ===\n", msh->n_cmds);
	while (msh->cmd_table[i] && i < msh->n_cmds)
	{
		cmd = msh->cmd_table[i];
		printf("Command %i:\n", i);
		printf("\theredoc_read: %d\n", cmd->heredoc_read);
		i++;
	}
}

void	expand_heredoc_input(t_msh *msh, char **value)
{
	char	*result;

	result = ft_strdup("");
	if (!result)
		msh_quit(msh, "heredoc expansion malloc failed", 1);
	result = expand_value(*value, result, msh);
	free(*value);
	*value = result;
}

void	collect_one_heredoc(t_msh *msh, t_cmd *cmd, int j, int fd)
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
		write(fd, cmd->heredocs[j].buffer, ft_strlen(cmd->heredocs[j].buffer));
		write(fd, "\n", 1);
		free(cmd->heredocs[j].buffer);
		cmd->heredocs[j].buffer = NULL;
	}
}

void	collect_all_heredocs(t_msh *msh, int i)
{
	int	j;
	int	fd[2];

	while ((t_cmd *)msh->cmd_table[++i])
	{
		j = -1;
		while (((t_cmd *)msh->cmd_table[i])->heredocs[++j].delimiter)
		{
			if (pipe(fd) == -1)
				msh_quit(msh, "heredoc pipe failed", 1);
			collect_one_heredoc(msh, (t_cmd *)msh->cmd_table[i], j, fd[1]);
			if (fd[1] != -1)
				close(fd[1]);
			if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
				close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
			((t_cmd *)msh->cmd_table[i])->heredoc_read = fd[0];
		}
	}
}
