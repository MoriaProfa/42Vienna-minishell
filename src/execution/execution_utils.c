/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:44:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 17:41:59 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	close_and_exit(t_msh *msh, char *msg, int status)
{
	int	i;
	int	j;

	i = 0;
	while (i < msh->n_cmds)
	{
		j = 0;
		while (j < ((t_cmd *)msh->cmd_table[i])->n_hd)
		{
			if (((t_cmd *)msh->cmd_table[i])->heredocs[j].filename)
				unlink(((t_cmd *)msh->cmd_table[i])->heredocs->filename);
			j++;
		}
		i++;
	}
	close_heredocs(msh);
	msh_quit(msh, msg, status);
}

void	run_builtins(t_msh *msh, int i)
{
	save_stdin_stdout(msh, i);
	if (set_redirections(msh, i) == 0)
		msh_builtins(msh, i);
	else
		msh->status = 1;
	restore_stdin_stdout(msh);
}

bool	special_case_check(t_msh *msh, char *pathname, char *cmd)
{
	if (!pathname)
	{
		print_execution_error(msh, cmd, 1, 127);
		return (true);
	}
	if (compare_strings(cmd, ".") == true)
	{
		ft_putstr_fd(".: filename argument required", STDERR_FILENO);
		msh_print(msh, "\n.: usage: . filename [arguments]", 2);
		return (true);
	}
	if (compare_strings(cmd, "..") == true && compare_strings(pathname,
			"/..") == false)
	{
		print_execution_error(msh, cmd, 1, 127);
		return (true);
	}
	return (false);
}

int	check_if_directory(char *path)
{
	struct stat	mode;

	if (stat(path, &mode) != 0)
		return (0);
	else
		return (S_ISDIR(mode.st_mode));
}

void	print_execution_error(t_msh *msh, char *cmd, int option, int code)
{
	if (option == 1)
	{
		ft_putstr_fd(cmd, STDERR_FILENO);
		ft_putstr_fd(": ", STDERR_FILENO);
		return (msh_print(msh, "command not found", code));
	}
	if (option == 2)
	{
		perror(cmd);
		return (msh_print(msh, NULL, code));
	}
	if (option == 3)
	{
		ft_putstr_fd(cmd, STDERR_FILENO);
		return (msh_print(msh, ": Is a directory", code));
	}
	if (option == 4)
	{
		perror(cmd);
		return ((msh_print(msh, NULL, code)));
	}
}

// void	print_execution_error(t_msh *msh, char *cmd, int option, int code)
// {
// 	char	*msg;
// 	char	*tmp;

// 	if (option == 2 || option == 4)
// 	{
// 		ft_putstr_fd("minishell: ", STDERR_FILENO); 
// 		perror(cmd);
// 		msh_print(msh, NULL, code);
// 		return ;
// 	}
// 	tmp = ft_strjoin("minishell: ", cmd);
// 	if (!tmp)
// 		msh_quit(msh, "malloc failed composing an error message", 1);
// 	if (option == 1)
// 		msg = ft_strjoin(cmd, ": command not found");
// 	if (option == 3)
// 		msg = ft_strjoin(tmp, ": Is a directory");
// 	free(tmp);
// 	if (!msg)
// 		msh_quit(msh, "malloc failed composing an error message", 1);
// 	msh_print(msh, msg, code);
// 	free(msg);
// 	return ;
// }