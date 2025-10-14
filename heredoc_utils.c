/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 05:18:53 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/14 05:21:27 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// for testing purposes
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

bool	compare_strings(char *s1, char *s2)
{
	if (ft_strlen(s1) == ft_strlen(s2) && ft_strncmp(s1, s2,
			ft_strlen(s1)) == 0)
		return (true);
	else
		return (false);
}

void	print_heredoc_warning(t_cmd *cmd, int j)
{
	ft_putstr_fd("minishell: warning: here-document delimeted by end-of-file (wanted ",
		STDERR_FILENO);
	ft_putstr_fd(cmd->heredocs[j].delimiter, STDERR_FILENO);
	ft_putstr_fd(")\n", STDERR_FILENO);
}

