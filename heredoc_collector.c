/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/14 05:21:56 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char		*expand_value(char *value, char *result, t_msh *msh);
void		close_fds(t_msh *msh);
void		close_heredocs(t_msh *msh);
bool		compare_strings(char *s1, char *s2);
void		print_heredoc_warning(t_cmd *cmd, int j);

static void	expand_heredoc_input(t_msh *msh, char **value)
{
	char	*tmp;
	char	*expanded;

	tmp = ft_strdup("");
	if (!tmp)
		msh_quit(msh, "heredoc expansion malloc failed", 1);
	expanded = expand_value(*value, tmp, msh);
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

static void	collect_one_heredoc(t_msh *msh, t_cmd *cmd, int j, int fd)
{
	while (1)
	{
		cmd->heredocs[j].buffer = readline("> ");
		if (!cmd->heredocs[j].buffer)
		{
			print_heredoc_warning(cmd, j);
			return ;
		}
		if (cmd->heredocs[j].expand == true
			&& compare_strings(cmd->heredocs[j].buffer,
				cmd->heredocs[j].delimiter) == false)
			expand_heredoc_input(msh, &cmd->heredocs[j].buffer);
		if (compare_strings(cmd->heredocs[j].buffer,
				cmd->heredocs[j].delimiter) == true)
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
	close(fd);
}

void	collect_heredocs_in_child(t_msh *msh, int i, int j, int fd)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		msh_quit(msh, "heredoc fork failed", 1);
	else if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		// need to add msh_free before terminatig heredoc?
		signal(SIGQUIT, SIG_IGN); // ignore Ctrl+\ during heredoc
		collect_one_heredoc(msh, (t_cmd *)msh->cmd_table[i], j, fd);
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

void	collect_all_heredocs(t_msh *msh, int i)
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
			collect_heredocs_in_child(msh, i, j, fd);
			close(fd);
		}
	}
}
