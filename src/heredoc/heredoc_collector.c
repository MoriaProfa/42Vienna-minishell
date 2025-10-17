/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/17 22:14:38 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	expand_heredoc_input(t_msh *msh, char **value)
{
	char	*tmp;
	char	*expanded;

	tmp = ft_strdup("");
	if (!tmp)
	{
		close_heredocs(msh);
		msh_quit(msh, "heredoc expansion malloc failed", 1);
	}
	expanded = expand_value(NULL, *value, tmp, msh);
	free(*value);
	*value = expanded;
}

static char	*create_filename(int i, int j)
{
	char	*temp;
	char	*name;
	char	*index;

	index = ft_itoa(i);
	if (!index)
		return (NULL);
	temp = ft_strjoin("/tmp/.msh_temporary_hd_", index);
	free(index);
	if (!temp)
		return (NULL);
	index = ft_itoa(j);
	if (!index)
	{
		free(temp);
		return (NULL);
	}
	name = ft_strjoin(temp, index);
	free(temp);
	free(index);
	if (!name)
		return (NULL);
	return (name);
}

static int	collect_one_heredoc(t_msh *msh, t_heredoc heredoc, int fd)
{
	while (1)
	{
		heredoc.buffer = readline("> ");
		if (!heredoc.buffer)
			return (print_warning_heredoc(heredoc, msh->line_number));
		if (g_signal_flag)
		{
			close_heredocs(msh);
			return (handle_signal_heredoc(msh, &heredoc.buffer, fd));
		}
		if (heredoc.expand == true && compare_strings(heredoc.buffer,
				heredoc.delimiter) == false)
			expand_heredoc_input(msh, &heredoc.buffer);
		if (compare_strings(heredoc.buffer, heredoc.delimiter) == true)
		{
			ft_free(&heredoc.buffer);
			break ;
		}
		write(fd, heredoc.buffer, ft_strlen(heredoc.buffer));
		write(fd, "\n", 1);
		ft_free(&heredoc.buffer);
		msh->line_number++;
	}
	close(fd);
	return (0);
}

int	collect_all_heredocs(t_msh *msh, int i)
{
	int		j;
	t_cmd	**cmd;

	cmd = msh->cmd_table;
	while ((t_cmd *)msh->cmd_table[++i])
	{
		j = -1;
		while ((cmd[i])->heredocs[++j].delimiter)
		{
			(cmd[i])->heredocs[j].filename = create_filename(i, j);
			if (!(cmd[i])->heredocs[j].filename)
				msh_quit(msh, "failed to create temporary filename", 1);
			(cmd[i])->heredoc_read = open((cmd[i])->heredocs[j].filename,
					O_RDWR | O_CREAT | O_TRUNC, 0600);
			if ((cmd[i])->heredoc_read == -1)
				msh_quit(msh, "failed to create temporary heredoc file", 1);
			if (collect_one_heredoc(msh, (cmd[i])->heredocs[j],
					(cmd[i])->heredoc_read) == 1)
				return (1);
			close((cmd[i])->heredoc_read);
		}
	}
	return (0);
}
