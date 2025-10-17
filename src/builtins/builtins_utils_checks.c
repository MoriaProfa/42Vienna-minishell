/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_checks.c                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 20:52:58 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 21:03:22 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

bool	check_for_whitespace_end(char *str, int start)
{
	while(str[start])
	{
		if (!ft_strchr(" \t", str[start]))
			return (false);
		start++;
	}
	return (true);
}

bool	check_newline_skipping(char *str)
{
	int	i;
	if (str[0] != '-')
		return (false);
	i = 0;
	while (str[++i])
	{
		if (str[i] != 'n')
			return (false);
	}
	return (true);
}

bool	check_name_validity(t_msh *msh, char *str)
{
	int	i;

	i = 0;
	if (!ft_isalpha(str[0]) && str[0] != '_')
	{
		print_export_error(msh, str);
		return (false);
	}
	while (str[++i] && str[i] != '=')
	{
		if (!ft_isalnum(str[i]) && str[i] != '_')
		{
			print_export_error(msh, str);
			return (false);
		}
	}
	return (true);
}

void	check_exit_code_validity(t_msh *msh, char *str)
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
