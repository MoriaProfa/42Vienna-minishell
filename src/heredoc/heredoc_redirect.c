/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_redirect.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 05:05:23 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 15:40:13 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	print_warning_heredoc(t_heredoc heredoc, size_t ln)
{
	ft_putstr_fd("minishell: warning: here-document at line ", STDERR_FILENO);
	ft_putnbr_fd(ln, STDERR_FILENO);
	ft_putstr_fd(" delimited by end-of-file (wanted `", STDERR_FILENO);
	ft_putstr_fd(heredoc.delimiter, STDERR_FILENO);
	ft_putstr_fd("')\n", STDERR_FILENO);
	return (0);
}

bool	compare_strings(char *s1, char *s2)
{
	if (ft_strlen(s1) == ft_strlen(s2) && ft_strncmp(s1, s2,
			ft_strlen(s1)) == 0)
		return (true);
	else
		return (false);
}

void	close_heredocs(t_msh *msh)
{
	int	i;

	i = -1;
	if (msh->cmd_table)
	{
		while ((t_cmd *)msh->cmd_table[++i])
		{
			if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
				close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
		}
	}
}

int	set_heredoc(t_msh *msh, t_cmd *cmd, t_heredoc heredoc)
{
	if (collect_one_heredoc(msh, heredoc, cmd->heredoc_read) == 1)
		return (1);
	close(cmd->heredoc_read);
	cmd->heredoc_read = open(heredoc.filename, O_RDONLY);
	if (cmd->heredoc_read == -1)
		return (1);
	else
		unlink(heredoc.filename);
	return (0);
}

int	redirect_heredoc(t_msh *msh, int i)
{
	if (((t_cmd *)msh->cmd_table[i])->heredoc_read == -1)
		return (1);
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_HEREDOC)
	{
		dup2(((t_cmd *)msh->cmd_table[i])->heredoc_read, STDIN_FILENO);
		close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
	}
	return (0);
}
