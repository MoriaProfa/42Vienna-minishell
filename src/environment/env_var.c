/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_var.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/03 21:20:54 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 21:31:38 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	free_env_var(void *content)
{
	t_env_var	*cur_env_var;

	cur_env_var = (t_env_var *)content;
	if (!cur_env_var)
		return ;
	if (cur_env_var->name)
		free(cur_env_var->name);
	if (cur_env_var->value)
		free(cur_env_var->value);
	free(cur_env_var);
}

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

void	create_env(t_msh *msh)
{
	char	*cwd;

	if (!check_and_update_env_var_value(msh, "SHLVL", NULL))
		prepare_and_add_to_env_vars(msh, "SHLVL", "1");
	if (!check_and_update_env_var_value(msh, "PWD", NULL))
	{
		cwd = getcwd(NULL, 0);
		if (!cwd)
		{
			cwd = ft_strdup("/");
			if (!cwd)
				msh_quit(msh, "create env: cwd fallback malloc failed", 1);
		}
		prepare_name_and_add_to_env_vars(msh, "PWD", cwd);
	}
	if (!check_and_update_env_var_value(msh, "OLDPWD", NULL))
		prepare_and_add_to_env_vars(msh, "OLDPWD", NULL);//gets init in cd, if we do it here, cd - doesnt work always
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

void	init_env_vars(t_msh *msh, char **envp)
{
	char	*name;
	char	*value;
	size_t	name_len;
	int		i;

	i = -1;
	while (envp && envp[++i])
	{
		if (ft_strchr(envp[i], '='))
		{
			name_len = 0;
			name = extract_env_name(envp[i], &name_len);
			if (!name)
				msh_quit(msh, "env: name init malloc failed", 1);
			value = extract_env_value(envp[i], name_len + 1);
			if (!value)
				(free(name), msh_quit(msh, "env: value init malloc failed", 1));
			add_to_env_vars(msh, name, value);
			continue ;
		}
		name = ft_strdup(envp[i]);
		if (!name)
			msh_quit(msh, "env: name init malloc failed", 1);
		add_to_env_vars(msh, name, NULL);
	}
	create_env(msh);
}
