/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_errorprints.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 20:47:21 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 21:15:48 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	print_getcwd_error(char *builtin_name, t_msh *msh)
{
	ft_putstr_fd(builtin_name, STDERR_FILENO);
	ft_putstr_fd(": error retrieving current directory: ", STDERR_FILENO);
	ft_putstr_fd("getcwd: cannot access parent directories: ", STDERR_FILENO);
	perror("");
	msh->status = 1;
}

void	print_num_error_exit(t_msh *msh, char *str)
{
	// ft_putendl_fd("exit", STDERR_FILENO);
	ft_putstr_fd("exit: ", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	msh_quit(msh, ": numeric argument required", 2);
}

void	print_export_error(t_msh *msh, char *str)
{
	ft_putstr_fd("export: `", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	msh_print(msh, "': not a valid identifier", 1);
}
