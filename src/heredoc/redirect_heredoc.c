/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirect_heredoc.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 05:05:23 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/16 22:16:19 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	print_heredoc_warning(t_heredoc heredoc, size_t ln)
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
	while ((t_cmd *)msh->cmd_table[++i])
	{
		if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
			close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
	}
}

static int	set_heredocs(t_msh *msh, int i, int j)
{
	int	fd;

	fd = -1;
	while (((t_cmd *)msh->cmd_table[i])->heredocs[++j].delimiter)
	{
		if (fd != -1)
			close(fd);
		fd = open(((t_cmd *)msh->cmd_table[i])->heredocs[j].filename,
				O_RDONLY);
		if (fd == -1)
		{
			msh_print(msh, "failed to open temporary heredoc file", 1);
			return (-1);
		}
		else
			unlink(((t_cmd *)msh->cmd_table[i])->heredocs[j].filename);
	}
	return (fd);
}

int	redirect_heredoc(t_msh *msh, int i, int j)
{
	((t_cmd *)msh->cmd_table[i])->heredoc_read = set_heredocs(msh, i, j);
	if (((t_cmd *)msh->cmd_table[i])->heredoc_read == -1)
		return (1);
	else if (((t_cmd *)msh->cmd_table[i])->last_input == T_HEREDOC_REDIRECT)
		dup2(((t_cmd *)msh->cmd_table[i])->heredoc_read, STDIN_FILENO);
	return (0);
}
