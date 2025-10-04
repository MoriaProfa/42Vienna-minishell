/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:08:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/04 19:52:23 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	envp_free(t_msh *msh);
void	msh_free(t_msh *msh);

int	msh_pwd(t_msh *msh)
{
	char	*pwd;
 
	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (msh->status);
	ft_putendl_fd(pwd, msh->fd_out);
	free(pwd);
	return (0);
}

int	msh_cd(t_msh *msh, int i)
{
	// struct stat	stats;
	size_t	len;

	if (((t_cmd *)msh->cmd_table[i])->args[2])
		return (printf("%s\n", "cd: too many arguments"), 1);
	len = ft_strlen(((t_cmd *)msh->cmd_table[i])->args[1]);
	if (!((t_cmd *)msh->cmd_table[i])->args[1])
		msh->status = chdir("/home/edarmaev/");
	(void)len;
	/*stat(((t_cmd *)msh->cmd_table[i])->args[1], &stats);
	//	msh_end(msh, "cd: stat failed");
	if (!S_ISDIR(stats.st_mode))
		printf("cd: %s: not a directory\n", array[1]);
	if (stats.st_mode & F_OK)
		msh->status = chdir(((t_cmd *)msh->cmd_table[i])->args[1]);
	if (!(stats.st_mode & F_OK))
		printf("cd: %s: no such file or directory\n", ((t_cmd *)msh->cmd_table[i])->args[1]);
	if (access(((t_cmd *)msh->cmd_table[i])->args[1], X_OK) == 0)
		printf("cd: permission denied: %s\n", ((t_cmd *)msh->cmd_table[i])->args[1]);*/
	return (msh->status);
}

int	msh_env(t_msh *msh)
{
	(void)msh;
	printf("env\n");
	return (0);
}

int	msh_export(t_msh *msh)
{
	(void)msh;
	printf("export\n");
	return (0);
}

int	msh_unset(t_msh *msh)
{
	(void)msh;
	printf("unset\n");
	return (0);
}

int	msh_echo(t_msh *msh)
{
	(void)msh;
	printf("echo\n");
	return (0);
}

void	msh_builtins(t_msh *msh, int i)
{
	if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_PWD)
	{
		msh_pwd(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_CD)
	{
		msh_cd(msh, i);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_EXIT)
		msh_end(msh, "exit", 0);
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_ENV)
	{
		msh_env(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_EXPORT)
	{
		msh_export(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_UNSET)
	{
		msh_unset(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_ECHO)
	{
		msh_echo(msh);
	}
	return ;
}
