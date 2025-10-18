/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_export.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 20:43:19 by astehl            #+#    #+#             */
/*   Updated: 2025/10/18 01:53:48 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	populate_names_array(t_msh *msh, char **names_array, size_t size)
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
			i++;
		}
		cur_node = cur_node->next;
	}
}

static void	sort_names_array(char **array, size_t size)
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

static void	handle_export_wo_args(t_msh *msh)
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

void	handle_export_with_value(char *arg, t_msh *msh)
{
	size_t	name_len;
	char	*value;
	char	*name;

	name_len = 0;
	name = extract_env_name(arg, &name_len);
	if (!name)
		msh_quit(msh, "export: name init malloc failed", 1);
	value = extract_env_value(arg, name_len + 1);
	if (!value)
		(free(name), msh_quit(msh, "export: value init malloc failed", 1));
	if (check_and_update_env_var_value(msh, name, value))
		free(name);
	else
		add_to_env_vars(msh, name, value);
}

void	msh_export(t_msh *msh, char **args)
{
	char	*name;
	int		i;

	if (!args[1])
		return(handle_export_wo_args(msh));
	i = 0;
	while (args[++i])
	{
		if (!check_name_validity(msh, args[i]))
			continue ;
		if (ft_strchr(args[i], '='))
			handle_export_with_value(args[i], msh);
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
