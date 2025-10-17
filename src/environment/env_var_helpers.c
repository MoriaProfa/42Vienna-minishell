/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var_helpers.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 22:28:49 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 22:29:51 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	add_to_env_vars(t_msh *msh, char *name, char *value)
{
	t_list		*cur_node;
	t_env_var	*cur_env_var;

	cur_env_var = ft_calloc(1, sizeof(t_env_var));
	if (!cur_env_var)
	{
		free(name);
		free(value);
		msh_quit(msh, "env: cur_env_var malloc failed", 1);
	}
	cur_env_var->name = name;
	cur_env_var->value = value;
	cur_node = ft_lstnew(cur_env_var);
	if (!cur_node)
	{
		free(cur_env_var->name);
		free(cur_env_var->value);
		free(cur_env_var);
		msh_quit(msh, "env: new node malloc failed", 1);
	}
	ft_lstadd_back(&msh->env_vars, cur_node);
}

void	prepare_and_add_to_env_vars(t_msh *msh, char *name, char *value)
{
	char	*m_name;
	char	*m_value;

	m_name = ft_strdup(name);
	if (!m_name)
		msh_quit(msh, "prepare env: m_name malloc failed", 1);
	if (value)
	{
		m_value = ft_strdup(value);
		if (!m_value)
		{
			free(m_name);
			msh_quit(msh, "prepare env: m_value malloc failed", 1);
		}
	}
	else
		m_value = NULL;
	add_to_env_vars(msh, m_name, m_value);
}

void	prepare_name_and_add_to_env_vars(t_msh *msh, char *name, char *m_value)
{
	char	*m_name;

	m_name = ft_strdup(name);
	if (!m_name)
		msh_quit(msh, "prepare name env: m_name malloc failed", 1);
	add_to_env_vars(msh, m_name, m_value);
}

char	*extract_env_value(char *str, size_t name_len)
{
	char	*value;
	size_t	str_len;

	str_len = ft_strlen(str);
	if (name_len < str_len)
		value = ft_substr(str, name_len, str_len - name_len);
	else
		value = ft_strdup("");
	return (value);
}

char	*extract_env_name(char *str, size_t *name_len)
{
	int	i;

	i = -1;
	while (str[++i] && str[i] != '=')
		(*name_len)++;
	return (ft_substr(str, 0, *name_len));
}

