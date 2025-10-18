/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:08:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 16:16:11 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	msh_echo(t_msh *msh, char **args)
{
	int		start;
	bool	skip_newline;

	if (!args[1])
		return (msh_print(msh, "", 0));
	if (!ft_strncmp(args[1], "-", ft_strlen("-") + 1))
		return (msh_print(msh, "-", 0));
	skip_newline = check_newline_skipping(args[1]);
	start = 1;
	while (skip_newline && args[++start])
	{
		if (!check_newline_skipping(args[start]))
			break ;
	}
	if (!skip_newline)
		start = 0;
	else
		start--;
	print_echo_args(msh, args, start);
	if (!skip_newline)
		msh_print(msh, "", 0);
}

static void	msh_pwd(t_msh *msh)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		print_getcwd_error("pwd", msh);
	else
	{
		ft_putendl_fd(pwd, STDOUT_FILENO);
		msh->status = 0;
		free(pwd);
	}
}

static void	msh_unset(t_msh *msh, char **args)
{
	int	i;

	i = 0;
	while (args[++i])
		check_and_delete_env_var(msh, args[i]);
	msh->status = 0;
}

static void	msh_exit(t_msh *msh, char **args)
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
		// ft_putendl_fd("exit", STDERR_FILENO);
		msh_print(msh, "exit: too many arguments", 1);
	}
	else
		msh_quit(msh, NULL /*, "exit"*/, (code % 256 + 256) % 256);
}

void	msh_builtins(t_msh *msh, int i)
{
	if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_ECHO)
		msh_echo(msh, msh->cmd_table[i]->args);
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_CD)
	{
		msh_cd(msh, msh->cmd_table[i]->args);
		build_envp_array(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_PWD)
		msh_pwd(msh);
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_EXPORT)
	{
		msh_export(msh, msh->cmd_table[i]->args);
		build_envp_array(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_UNSET)
	{
		msh_unset(msh, msh->cmd_table[i]->args);
		build_envp_array(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_ENV)
		print_env_vars(msh->env_vars);
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_EXIT)
		msh_exit(msh, msh->cmd_table[i]->args);
}
