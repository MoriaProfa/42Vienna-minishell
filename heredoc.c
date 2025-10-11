/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/11 21:50:57 by edarmaev         ###   ########.fr       */
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
	char	*tmp;
	char	*expanded;

	tmp = ft_strdup("");
	if (!tmp)
		msh_quit(msh, "heredoc expansion malloc failed", 1);
	expanded = expand_value(*value, tmp, msh);
	free(*value);
	*value = expanded;
}
/*
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
	int		j;
	int		fd[2];

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
}*/
// temporary heredoc file

char	*create_filename(int i, int j)
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
	close(fd);
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
			collect_one_heredoc(msh, (t_cmd *)msh->cmd_table[i], j, fd);
			if (((t_cmd *)msh->cmd_table[i])->heredoc_read != -1)
				close(((t_cmd *)msh->cmd_table[i])->heredoc_read);
			((t_cmd *)msh->cmd_table[i])->heredoc_read = open(((t_cmd *)msh->cmd_table[i])->heredocs[j].filename,
					O_RDONLY);
			if (((t_cmd *)msh->cmd_table[i])->heredoc_read == -1)
				msh_quit(msh, "failed to open temporary heredoc file", 1);
			unlink(((t_cmd *)msh->cmd_table[i])->heredocs[j].filename);
		}
	}
}
