/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:08:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/17 03:12:35 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	print_env_vars(t_list *env_vars);
char	*extract_env_name(char *str, size_t *name_len);
char	*extract_env_value(char *str, size_t name_len);
bool	check_and_update_env_var_value(t_msh *msh, char *name, char *value);
void	prepare_name_and_add_to_env_vars(t_msh *msh, char *name, char *m_value);
void	add_to_env_vars(t_msh *msh, char *name, char *value);
void	build_envp_array(t_msh *msh);
void	free_env_var(void *content);
void	msh_free(t_msh *msh);
void    free_envp(t_msh *msh);
long	msh_atol(t_msh *msh, char *nptr);

char	*get_env_var_value(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var && cur_env_var->name && cur_env_var-> value && 
				!ft_strncmp(name, cur_env_var->name, ft_strlen(name) + 1))
			return (ft_strdup(cur_env_var->value));
		cur_node = cur_node->next;
	}
	return (NULL);
}

void	print_getcwd_error(char *builtin_name)
{
	ft_putstr_fd(builtin_name, STDERR_FILENO);
	ft_putstr_fd(": error retrieving current directory: ", STDERR_FILENO);
	ft_putstr_fd("getcwd: cannot access parent directories: ", STDERR_FILENO);
	perror("");
}

/*bool	check_pwd_validity(const char *pwd)
{
	int	len;

	if (!pwd || pwd[0] != '/')
		return (false);
	len = ft_strlen(pwd);
	if (len >= PATH_MAX)//PATH_MAX is included in limits.h even if vs code doesnt recognize this
		return (false);
	if (ft_strnstr(pwd, "/./", len) || ft_strnstr(pwd, "/../", len))
		return (false);
	if (len >= 2 && !ft_strncmp(pwd + (len - 2), "/.", 2))
		return (false);
	if (len >= 3 && !ft_strncmp(pwd + (len - 3), "/..", 3))
		return (false);
	return (true);
}*/

/*void	msh_pwd(t_msh *msh)
{
	char	*pwd;

	pwd = get_env_var_value(msh, "PWD");
	if (check_pwd_validity(pwd))
	{
		ft_putendl_fd(pwd, STDOUT_FILENO);
		free(pwd);
		return ;
	}
	if (pwd)
		free(pwd);
	pwd = getcwd(NULL, 0);
	if (!pwd)
	{
		print_getcwd_error("pwd");
		msh->status = 1;
		return ;
	}
	ft_putendl_fd(pwd, STDOUT_FILENO);
	free(pwd);
}*/

void	msh_pwd(t_msh *msh)
{
	char	*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		print_getcwd_error("pwd");
	ft_putendl_fd(pwd, STDOUT_FILENO);
	msh->status = 0;
	free(pwd);
}

char	*get_cd_path(t_msh *msh, char **args)
{
	char	*path;

	if (!args[1])
	{
		path = get_env_var_value(msh, "HOME");
		if (!path)
			msh_print(msh, "cd: HOME not set", 1);
	}
	else if (args[1][0] == '\0')
		return (NULL);
	else if (args[1][0] == '-' && args[1][1] == '\0')
	{
		path = get_env_var_value(msh, "OLDPWD");
		if (!path)
			msh_print(msh, "cd: OLDPWD not set", 1);
		else
			ft_putendl_fd(path, STDOUT_FILENO);
	}
	else
	{
		path = ft_strdup(args[1]);
		if (!path)
			msh_print(msh, "cd: path malloc failed", 1);
	}
	return (path);
}

void	handle_cd_env_var_update(t_msh *msh, char *oldpwd)
{
	char	*newpwd;

	if (!check_and_update_env_var_value(msh, "OLDPWD", oldpwd))
		prepare_name_and_add_to_env_vars(msh, "OLDPWD", oldpwd);
	newpwd = getcwd(NULL, 0);
	if (!newpwd)
		return (print_getcwd_error("cd"));
	if (!check_and_update_env_var_value(msh, "PWD", newpwd))
		prepare_name_and_add_to_env_vars(msh, "PWD", newpwd);
}

void	msh_cd(t_msh *msh, char **args)
{
	char	*path;
	char	*oldpwd;
	
	if (args[2])
		return (msh_print(msh, "cd: too many arguments", 1));
	path = get_cd_path(msh, args);
	if (!path)
		return ;
	oldpwd = get_env_var_value(msh, "PWD");
	if (!oldpwd)
	{
		oldpwd = getcwd(NULL, 0);
		if (!oldpwd)
			return (free(path), print_getcwd_error("cd"));
	}
	msh->status = chdir(path);
	if (msh->status != 0)
		return (msh_print(msh, NULL, 1), 
			ft_putstr_fd("minishell: cd: ", STDERR_FILENO), 
			perror(path), free(oldpwd), free(path));
	else
	{
		free(path);
		handle_cd_env_var_update(msh, oldpwd);
	}
}

void	print_num_error_exit(t_msh *msh, char *str)
{
	//ft_putendl_fd("exit", STDERR_FILENO);//tester doesnt accept "exit"
	ft_putstr_fd("exit: ", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	msh_quit(msh, ": numeric argument required", 2);
}

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
		//ft_putendl_fd("exit", STDERR_FILENO);//tester doesnt accept "exit"
		msh_print(msh, "exit: too many arguments", 1);
	}
	else
		msh_quit(msh, NULL/*"exit"*/, (code % 256 + 256) % 256);//tester doesnt accept "exit"
}

void	populate_names_array(t_msh *msh, char **names_array, size_t size)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;
	size_t		i;

	cur_node = msh->env_vars;
	i = 0;
	while (i < size && cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var && cur_env_var->name && ft_strncmp(cur_env_var->name, 
			"_", ft_strlen(cur_env_var->name) + 1))
		{
			names_array[i] = ft_strdup(cur_env_var->name);
			if (!names_array[i])
			{
				ft_array_free(names_array, i);
				msh_quit(msh, "env names array strdup malloc failed", 1);
			}
//			printf("names_array[%li] = %s\n", i, names_array[i]);//for testing
			i++;
		}
		cur_node = cur_node->next;
	}
}

void	sort_names_array(char **array, size_t size)
{
	char	*tmp;
	size_t	i;
	bool	swapped;

	if (!array || size < 2)
		return ;
	swapped = true;
	while (swapped)
	{
		swapped = false;
		i = 0;
		while (i + 1 < size && array[i] && array[i + 1])
		{
			if (ft_strncmp(array[i], array[i + 1], ft_strlen(array[i]) + 1) > 0)
			{
				tmp = array[i];
				array[i] = array[i + 1];
				array[i + 1] = tmp;
				swapped = true;
			}
			i++;
		}
	}
}

void	print_export_wo_args(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var && cur_env_var->name)
		{
			if (!ft_strncmp(name, cur_env_var->name, ft_strlen(name) + 1))
			{
				if (cur_env_var->value)
					printf("declare -x %s=\"%s\"\n", cur_env_var->name,
						cur_env_var->value);
				else
					printf("declare -x %s\n", cur_env_var->name);
				return ;
			}
		}
		cur_node = cur_node->next;
	}
}

void	handle_export_wo_args(t_msh *msh)
{
	char	**names_array;
	size_t	size;
	int		i;

	size = ft_lstsize(msh->env_vars);
	names_array = ft_calloc(size + 1, sizeof(char *));
	if (!names_array)
		msh_quit(msh, "env names array init malloc failed", 1);
	populate_names_array(msh, names_array, size);
	sort_names_array(names_array, size);
	i = -1;
	while (names_array[++i])
		print_export_wo_args(msh, names_array[i]);
	ft_array_free(names_array, size);
}

void	print_export_error(t_msh *msh, char *str)
{
	ft_putstr_fd("export: `", STDERR_FILENO);
	ft_putstr_fd(str, STDERR_FILENO);
	msh_print(msh, "': not a valid identifier", 1);
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

bool	check_and_update_env_var_value(t_msh *msh, char *name, char *value)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (!ft_strncmp(name, cur_env_var->name, ft_strlen(name) + 1))
		{
			if (value)
			{
				free(cur_env_var->value);
				cur_env_var->value = value;
			}
			return (true);
		}
		cur_node = cur_node->next;
	}
	return (false);
}

void	msh_export(t_msh *msh, char **args)
{
	char	*name;
	char	*value;
	size_t	name_len;
	int		i;

	if (!args[1])
		return(handle_export_wo_args(msh));
	i = 0;
	while (args[++i])
	{
		if (!check_name_validity(msh, args[i])/* || !ft_strchr(args[i], '=')*/)
			continue ;
		if (ft_strchr(args[i], '='))
		{
			name_len = 0;
			name = extract_env_name(args[i], &name_len);
			if (!name)
				msh_quit(msh, "export: name init malloc failed", 1);
			value = extract_env_value(args[i], name_len + 1);
			if (!value)
				(free(name), msh_quit(msh, "export: value init malloc failed", 1));
			if (check_and_update_env_var_value(msh, name, value))
				free(name);
			else
				add_to_env_vars(msh, name, value);
		}
		else
		{
			name = ft_strdup(args[i]);
			if (!name)
				msh_quit(msh, "export: name only init malloc failed", 1);
			if (!check_and_update_env_var_value(msh, name, NULL))
				add_to_env_vars(msh, name, NULL);
			else
				free(name);
		}
	}
}

void	check_and_delete_env_var(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)cur_node->content;
		if (!ft_strncmp(name, cur_env_var->name, ft_strlen(name) + 1))
		{
			if (cur_node->prev)
				cur_node->prev->next = cur_node->next;
			else
				msh->env_vars = cur_node->next;
			if (cur_node->next)
				cur_node->next->prev = cur_node->prev;
			ft_lstdelone(cur_node, free_env_var);
			break ;
		}
		cur_node = cur_node->next;
	}
}

void	msh_unset(t_msh *msh, char **args)
{
	int	i;

	i = 0;
	while (args[++i])
		check_and_delete_env_var(msh, args[i]);
	msh->status = 0;
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

void	print_echo_args(t_msh *msh, char **args, int start)
{
	bool	begin;

	begin = true;
	while (args[++start])
	{
		if (!begin)
			ft_putchar_fd(' ', STDOUT_FILENO);
		ft_putstr_fd(args[start], STDOUT_FILENO);
		msh->status = 0;
		begin = false;
	}
}
void	msh_echo(t_msh *msh, char **args)
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
	//ft_putstr_fd("\tnewline for better visibility\n", STDOUT_FILENO);//just for testing
}

void	msh_builtins(t_msh *msh, int i)
{
	if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_PWD)
		msh_pwd(msh);
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_CD)
	{
		msh_cd(msh, msh->cmd_table[i]->args);
		build_envp_array(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_EXIT)
		msh_exit(msh, msh->cmd_table[i]->args);
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_ENV)
		print_env_vars(msh->env_vars);
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
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_ECHO)
		msh_echo(msh, msh->cmd_table[i]->args);
	return ;
}
