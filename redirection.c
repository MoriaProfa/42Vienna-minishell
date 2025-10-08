/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/02 10:48:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/08 14:36:19 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	check_access_read(char *file)
{
	if (access(file, F_OK) == 0 && access(file, R_OK) != 0)
	{
		perror(file);
		return (0);
	}
	else
		return (1);
}

static int	check_access_write(char *file)
{
	if (access(file, F_OK) == 0 && access(file, W_OK) != 0)
	{
		perror(file);
		return (0);
	}
	else
		return (1);
}

static void	set_input(t_msh *msh, int i, int j)
{
	msh->fd_in = msh->fd_in_previous;
	if (((t_cmd *)msh->cmd_table[i])->n_hd > 0)
	{
		if (msh->fd_in != STDIN_FILENO)
			close(msh->fd_in);
		if (pipe(msh->hd_fd) == -1)
			msh_quit(msh, "pipe failed", 1);
		write(msh->hd_fd[1], ((t_cmd *)msh->cmd_table[i])->heredocs->buffer,
			ft_strlen(((t_cmd *)msh->cmd_table[i])->heredocs->buffer));
		close(msh->hd_fd[1]);
		msh->fd_in = msh->hd_fd[0];
	}
	else if (((t_cmd *)msh->cmd_table[i])->n_inf > 0)
	{
		while (++j < ((t_cmd *)msh->cmd_table[i])->n_inf)
		{
			if (check_access_read(((t_cmd *)msh->cmd_table[i])->infiles[j]) == 0)
				return ;
			msh->fd_in = open(((t_cmd *)msh->cmd_table[i])->infiles[j],
					O_RDONLY);
			if (msh->fd_in == -1)
				perror(((t_cmd *)msh->cmd_table[i])->infiles[j]);
		}
	}
}

static void	set_output(t_msh *msh, int i, int j)
{
	while (((t_cmd *)msh->cmd_table[i])->outfiles[++j].filename)
	{
		if (check_access_write(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename) == 0)
			return ;
		if (((t_cmd *)msh->cmd_table[i])->outfiles[j].append == false)
			msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename,
					O_RDWR | O_CREAT | O_TRUNC, 0644);
		else
			msh->fd_out = open(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename,
					O_RDWR | O_CREAT | O_APPEND, 0644);
		if (msh->fd_out == -1)
			perror(((t_cmd *)msh->cmd_table[i])->outfiles[j].filename);
		if (msh->fd_out != STDOUT_FILENO && msh->fd_out != -1)
		{
			dup2(msh->fd_out, STDOUT_FILENO);
			close(msh->fd_out);
		}
	}
}

void	set_redirections(t_msh *msh, int i)
{
	set_input(msh, i, -1);
	set_output(msh, i, -1);
	
}