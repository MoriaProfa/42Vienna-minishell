/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/17 15:59:15 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void		handle_signal(int signal);

char		*expand_value(t_token *token, char *value, char *result, t_msh *msh);
void		close_fds(t_msh *msh);
void		close_heredocs(t_msh *msh);
bool		compare_strings(char *s1, char *s2);
int			print_warning_heredoc(t_heredoc heredoc, size_t ln);
void		return_prompt(void);
int			handle_signal_heredoc(t_msh *msh, char **buffer, int fd);

static void	expand_heredoc_input(t_msh *msh, char **value)
{
	char	*tmp;
	char	*expanded;

	tmp = ft_strdup("");
	if (!tmp)
		msh_quit(msh, "heredoc expansion malloc failed", 1);
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
			return (handle_signal_heredoc(msh, &heredoc.buffer, fd));
		if (heredoc.expand == true && compare_strings(heredoc.buffer,
				heredoc.delimiter) == false)
			expand_heredoc_input(msh, &heredoc.buffer);
		msh->line_number++;
		if (compare_strings(heredoc.buffer, heredoc.delimiter) == true)
		{
			ft_free(&heredoc.buffer);
			break ;
		}
		write(fd, heredoc.buffer, ft_strlen(heredoc.buffer));
		write(fd, "\n", 1);
		ft_free(&heredoc.buffer);
	}
	close(fd);
	return (0);
}

int	collect_all_heredocs(t_msh *msh, int i)
{
	int	j;
	int	fd;

	while ((t_cmd *)msh->cmd_table[++i])
	{
		j = -1;
		while (((t_cmd *)msh->cmd_table[i])->heredocs[++j].delimiter)
		{
			((t_cmd *)msh->cmd_table[i])->heredocs[j].filename = create_filename(i,
					j);
			if (!((t_cmd *)msh->cmd_table[i])->heredocs[j].filename)
				msh_quit(msh, "failed to create temporary filename", 1);
			fd = open(((t_cmd *)msh->cmd_table[i])->heredocs[j].filename,
					O_RDWR | O_CREAT | O_TRUNC, 0600);
			if (fd == -1)
				msh_quit(msh, "failed to create temporary heredoc file", 1);
			if (collect_one_heredoc(msh,
					((t_cmd *)msh->cmd_table[i])->heredocs[j], fd) == 1)
				return (1);
			close(fd);
		}
	}
	return (0);
}
