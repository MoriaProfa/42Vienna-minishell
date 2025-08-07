/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:08:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/08/06 18:42:27 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	msh_pwd(t_msh *msh)
{
	char	*pwd;

	(void)msh;
	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (1);
	printf("%s\n", pwd);
	return (0);
}

int	msh_cd(t_msh *msh, char *new_pathname)
{
	(void)msh;
	if (chdir(new_pathname) != 0)
		return (-1);
	return (0);
}
/*
int	msh_exit(t_msh *msh)
{
	
}

int	msh_env(t_msh *msh)
{
	
}

int	msh_export(t_msh *msh)
{
	
}

int	msh_unset(t_msh *msh)
{
	
}

int	msh_echo(t_msh *msh)
{
	
}
*/