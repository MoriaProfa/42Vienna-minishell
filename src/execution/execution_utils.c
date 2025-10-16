/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 20:44:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/16 22:16:09 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

int	check_if_directory(char *path)
{
	struct stat	mode;

	if (stat(path, &mode) != 0)
		return (0);
	else
		return (S_ISDIR(mode.st_mode));
}

// void	print_execution_error(t_msh *msh, char *cmd, int option, int code)
// {
// 	char	*msg;
// 	char	*tmp;

// 	if (option == 2 || option == 4)
// 	{
// 		ft_putstr_fd("minishell: ", STDERR_FILENO); // commented out to pass the tester for option 4, bash prints "bash: err message"
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
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		perror(cmd);
		return (msh_print(msh, NULL, code));
	}
	if (option == 3)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd, STDERR_FILENO);
		return (msh_print(msh, ": Is a directory", code));
	}
	if (option == 4)
	{
		//ft_putstr_fd("minishell: ", STDERR_FILENO); // commented out to pass the tester, bash prints "bash: err message"
		perror(cmd);
		return ((msh_print(msh, NULL, code)));
	}
}