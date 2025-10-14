/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:44:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/14 21:59:47 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
		ft_putstr_fd("minishell: ", STDOUT_FILENO);
		perror(cmd);
		return (msh_print(msh, NULL, code));
	}
	if (option == 3)
	{
		ft_putstr_fd("minishell: ", STDOUT_FILENO);
		ft_putstr_fd(cmd, STDOUT_FILENO);
		return (msh_print(msh, ": Is a directory", code));
	}
	if (option == 4)
	{
		ft_putstr_fd("minishell: ", STDOUT_FILENO);
		perror(cmd);
		return ((msh_print(msh, NULL, code)));
	}
}
