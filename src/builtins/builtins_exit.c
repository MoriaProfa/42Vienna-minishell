/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_exit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 20:31:06 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 20:33:49 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	print_num_error_exit(t_msh *msh, char *str)
{
	// ft_putendl_fd("exit", STDERR_FILENO);//tester doesnt accept "exit"
	ft_putstr_fd("exit: ", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	msh_quit(msh, ": numeric argument required", 2);
}

static void	check_exit_code_validity(t_msh *msh, char *str)
{
	int	i;

	if (str[0] == '\0')
		print_num_error_exit(msh, str);
	i = 0;
	while (ft_strchr(" \t", str[i]))
		i++;
	if (str[i] != '+' && str[i] != '-')
		i--;
	while (str[++i])
	{
		if (ft_strchr(" \t", str[i]) && check_for_whitespace_end(str, i))
			break ;
		if (!ft_isdigit(str[i]))
			print_num_error_exit(msh, str);
	}
}

void	msh_exit(t_msh *msh, char **args)
{
	long	code;

	if (!args[1])
	{
		ft_putendl_fd("exit", STDERR_FILENO);
		msh_quit(msh, NULL, msh->status);
	}
	check_exit_code_validity(msh, args[1]);
	code = msh_atol(msh, args[1]);
	if (args[2])
	{
		// ft_putendl_fd("exit", STDERR_FILENO);//tester doesnt accept "exit"
		msh_print(msh, "exit: too many arguments", 1);
	}
	else
		msh_quit(msh, NULL /*"exit"*/, (code % 256 + 256) % 256);// tester doesnt accept "exit"
}
