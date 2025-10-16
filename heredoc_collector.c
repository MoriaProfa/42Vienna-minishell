/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_collector.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 00:10:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/16 15:18:26 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_signal(int signal);

char		*expand_value(char *value, char *result, t_msh *msh);
void		close_fds(t_msh *msh);
void		close_heredocs(t_msh *msh);
bool		compare_strings(char *s1, char *s2);
void		print_heredoc_warning(t_heredoc heredoc, size_t ln);
void	return_prompt(void);

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
/*
void	return_prompt(void);

void	return_heredoc_prompt(void)
{
	// printf("\n");
	// rl_on_new_line();
	// rl_replace_line("", 0);
	// rl_redisplay();
	// for heredoc (maybe also child processes?) -> old return_prompt function:
	rl_replace_line("", 0);
	ioctl(0, TIOCSTI, "\n");
	rl_on_new_line();
}
*/
void	handle_heredoc_signal(int signal)
{
	if (signal == SIGINT)
	{
		g_signal_flag = signal;
		return_prompt();
	}	
}

static int	collect_one_heredoc(t_msh *msh, t_heredoc heredoc, int fd)
{
	while (1)
	{
		heredoc.buffer = readline("> ");
		if (!heredoc.buffer)
		{
			print_heredoc_warning(heredoc, msh->line_number);
			return (0);
		}
		if (g_signal_flag)
		{
			free(heredoc.buffer);
			heredoc.buffer = NULL;
			close(fd);
			msh->status = 128 + g_signal_flag;
			g_signal_flag = 0;
			return (1);
		}
		if (heredoc.expand == true
			&& compare_strings(heredoc.buffer,
				heredoc.delimiter) == false)
			expand_heredoc_input(msh, &heredoc.buffer);
		if (compare_strings(heredoc.buffer,
				heredoc.delimiter) == true)
		{
			free(heredoc.buffer);
			heredoc.buffer = NULL;
			break ;
		}
		write(fd, heredoc.buffer, ft_strlen(heredoc.buffer));
		write(fd, "\n", 1);
		free(heredoc.buffer);
		heredoc.buffer = NULL;
	}
	close(fd);
	return (0);
}
void	collect_heredocs_in_child(t_msh *msh, int i, int j, int fd);

int	collect_all_heredocs(t_msh *msh, int i)
{
	int	j;
	int	fd;
	int	return_value;

	return_value = 0;
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
			return_value = collect_one_heredoc(msh, ((t_cmd *)msh->cmd_table[i])->heredocs[j], fd);
			close(fd);
		}
	}
	return (return_value);
}
/*
void	collect_heredocs_in_child(t_msh *msh, int i, int j, int fd)
{
	pid_t	pid;
	int		status;

	pid = fork();
	if (pid == -1)
		msh_quit(msh, "heredoc fork failed", 1);
	else if (pid == 0)
	{
		// signal(SIGINT, SIG_DFL);
		// signal(SIGQUIT, SIG_IGN); // ignore Ctrl+\ during heredoc
		collect_one_heredoc(msh, ((t_cmd *)msh->cmd_table[i])->heredocs[j], fd);
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
}*/
