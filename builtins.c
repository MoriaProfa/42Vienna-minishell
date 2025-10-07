/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:08:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/07 12:22:31 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	ft_printenv(t_list *env_vars);
char	*get_env_name(char *str, size_t *name_len);
char	*get_env_value(char *str, size_t name_len);
void	add_to_env_vars(t_msh *msh, const char *name, const char *value);
void	build_envp_array(t_msh *msh);
void	free_env_var(void *content);
void	msh_free(t_msh *msh);

/*
//not sure if it does what it should and tester is failing 
//path_check 11 with it:
// env -i ./minishell
//cd /bin/
//ls -S

char	*get_env_var_value(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;
	size_t		max_size;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		max_size = get_max_size(name, cur_env_var->name);
		if (cur_env_var && !ft_strncmp(name, cur_env_var->name, max_size))
			return (ft_strdup(cur_env_var->value));
		cur_node = cur_node->next;
	}
	return (NULL);
}

void	msh_pwd(t_msh *msh) //if !pwd, get value of PWD in env_var, if there is no PWD too, write error msg
{
	char		*pwd;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		pwd = get_env_var_value(msh, "PWD");
	if (!pwd)
	{
		ft_putstr_fd("pwd: error retrieving ", STDERR_FILENO);
		ft_putstr_fd("current directory: ", STDERR_FILENO);
		msh_exit(msh, "getcwd: cannot access parent directories", 1);
	}
	ft_putendl_fd(pwd, msh->fd_out);
	free(pwd);
}
*/

int	msh_pwd(t_msh *msh) // if !pwd, get value of PWD in env_var, if there is no PWD too, write error msg
{
	char	*pwd;
 
	pwd = getcwd(NULL, 0);
	if (!pwd)
		return (msh->status);//where is this return value catched? what should it do?
	ft_putendl_fd(pwd, STDOUT_FILENO);
	free(pwd);
	return (0);
}

int	msh_cd(t_msh *msh, int i) // also needs to change the value in OLDPWD to
// what has been the value of PWD before
{
	// struct stat	stats;
	size_t len;

	if (((t_cmd *)msh->cmd_table[i])->args[2])
		return (printf("%s\n", "cd: too many arguments"), 1);
	len = ft_strlen(((t_cmd *)msh->cmd_table[i])->args[1]);
	if (!((t_cmd *)msh->cmd_table[i])->args[1])
		msh->status = chdir("/home/edarmaev/");
	(void)len;
	/*stat(((t_cmd *)msh->cmd_table[i])->args[1], &stats);
	//	msh_exit(msh, "cd: stat failed");
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
		if (cur_env_var && cur_env_var->name)
		{
			names_array[i] = ft_strdup(cur_env_var->name);
			if (!names_array[i])
			{
				ft_array_free(names_array, i);
				msh_exit(msh, "env names array strdup malloc failed", 1);
			}
//			printf("names_array[%li] = %s\n", i, names_array[i]);//for testing
			i++;
		}
		cur_node = cur_node->next;
	}
	names_array[size] = NULL;
}

size_t	get_max_size(char *str1, char *str2)
{
	size_t	len1;
	size_t	len2;

	len1 = ft_strlen(str1);
	len2 = ft_strlen(str2);
	if (len1 > len2)
		return (len1);
	return (len2);
}

void	sort_names_array(char **array, size_t size)
{
	char	*tmp;
	size_t	i;
	size_t	max_size;
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
			max_size = get_max_size(array[i], array[i + 1]);
			if (ft_strncmp(array[i], array[i + 1], max_size) > 0)
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

void	print_env_var(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;
	size_t		max_size;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		if (cur_env_var && cur_env_var->name)
		{
			max_size = get_max_size(name, cur_env_var->name);
			if (!ft_strncmp(name, cur_env_var->name, max_size))
			{
				if (cur_env_var->value && ft_strlen(cur_env_var->value))
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
		msh_exit(msh, "env names array init malloc failed", 1);
	populate_names_array(msh, names_array, size);
	sort_names_array(names_array, size);
	i = -1;
	while (names_array[++i])
		print_env_var(msh, names_array[i]);
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
	size_t		max_size;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)(cur_node->content);
		max_size = get_max_size(name, cur_env_var->name);
		if (!ft_strncmp(name, cur_env_var->name, max_size))
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

	i = 0;
	while (args[++i])
	{
		if (!check_name_validity(msh, args[i]) || !ft_strchr(args[i], '='))
			continue ;
		name_len = 0;
		name = get_env_name(args[i], &name_len);
		if (!name)
			msh_exit(msh, "export: name init malloc failed", 1);
		value = get_env_value(args[i], name_len + 1);
		if (!value)
			(free(name), msh_exit(msh, "export: value init malloc failed", 1));
		if (!check_and_update_env_var_value(msh, name, value))
			(add_to_env_vars(msh, name, value), free(value));
		free(name);
	}
	if (!args[1])
		handle_export_wo_args(msh);
}

void	check_and_delete_env_var(t_msh *msh, char *name)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_node = msh->env_vars;
	while (cur_node)
	{
		cur_env_var = (t_env_var *)cur_node->content;
		if (!ft_strncmp(name, cur_env_var->name, get_max_size(name,
					cur_env_var->name)))
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
			ft_putchar_fd(' ', msh->fd_out);
		ft_putstr_fd(args[start], msh->fd_out);
		msh->status = 0;
		begin = false;
	}
}
void	msh_echo(t_msh *msh, char **args)
{
	int		start;
	bool	skip_newline;

	if (!args[1])
	{
		ft_putchar_fd('\n', msh->fd_out);
		return ;
	}
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
		ft_putchar_fd('\n', msh->fd_out);
	//ft_putstr_fd("\tnewline for better visibility\n", msh->fd_out);//just for testing
}

void	msh_builtins(t_msh *msh, int i)
{
	if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_PWD)
		msh_pwd(msh);
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_CD)
	{
		msh_cd(msh, i);
		build_envp_array(msh);
	}
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_EXIT)
		msh_exit(msh, "exit", 0); // if there are more entries in the args-array than just "exit" it should print an error (only cares for args[1], args[1+] are ignored) and change msh->status to 2 before ending
	else if (((t_cmd *)msh->cmd_table[i])->builtin.type == B_ENV)
		ft_printenv(msh->env_vars);
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
