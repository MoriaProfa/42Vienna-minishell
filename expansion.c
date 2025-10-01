/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 16:29:54 by astehl            #+#    #+#             */
/*   Updated: 2025/10/01 20:23:47 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	add_to_result(char *tmp, char **result, size_t *i, t_msh *msh)
{
	char	*new_result;

	if (!tmp)
		msh_end(msh, "expander: tmp substr/itoa malloc failed", 1);
	new_result = ft_strjoin(*result, tmp);
	free(*result);
	free(tmp);
	if (!new_result)
		msh_end(msh, "expander: new_result strjoin malloc failed", 1);
	*result = new_result;
	(*i)++;
}

static void	ft_getenv(char *var_name, size_t len, char ***result, t_msh *msh)
{
	char	*env_value;
	size_t	j;
	char	*tmp;

	env_value = NULL;
	j = 0;
	while (msh->envp[j])
	{
		if (ft_strncmp(msh->envp[j], var_name, len) == 0
			&& msh->envp[j][len] == '=')
		{
			env_value = msh->envp[j] + len + 1;
			break ;
		}
		j++;
	}
	free(var_name);
	if (env_value)
	{
		tmp = ft_strjoin(**result, env_value);
		free(**result);
		if (!tmp)
			msh_end(msh, "expander: env_value join malloc failed", 1);
		**result = tmp;
	}
}

static void	get_env_var_value(char *value, char **result, size_t *i, t_msh *msh)
{
	size_t	var_len;
	size_t	start;
	char	*var_name;

	var_len = 0;
	start = *i;
	while (value[*i] && (ft_isalnum(value[*i]) || value[*i] == '_'))
	{
		(*i)++;
		var_len++;
	}
	var_name = ft_substr(value, start, var_len);
	if (!var_name)
	{
		free(*result);
		msh_end(msh, "expander: var_name substr malloc failed", 1);
	}
	ft_getenv(var_name, ft_strlen(var_name), &result, msh);
}

char	*expand_value(char *value, char *result, t_msh *msh)
{
	size_t	i;

	i = 0;
	while (i < ft_strlen(value))
	{
		if (value[i] == '$')
		{
			if (value[i + 1] == '?')
			{
				add_to_result(ft_itoa(msh->status), &result, &i, msh);
				i++;
			}
			else if (ft_isalpha(value[i + 1]) || value[i + 1] == '_')
			{
				i++;
				get_env_var_value(value, &result, &i, msh);
			}
			else
				add_to_result(ft_substr(value, i, 1), &result, &i, msh);
		}
		else
			add_to_result(ft_substr(value, i, 1), &result, &i, msh);
	}
	return (result);
}

void	expand_word_tokens(t_list *tokens, t_msh *msh)
{
	t_list	*cur_node;
	t_token	*cur_token;
	char	*result;

	cur_node = tokens;
	while (cur_node)
	{
		cur_token = (t_token *)cur_node->content;
		if (cur_token->type == T_WORD && cur_token->quoted != Q_SINGLE
			&& ft_strchr(cur_token->value, '$'))
		{
			result = ft_strdup("");
			if (!result)
				msh_end(msh, "expander: result init malloc failed", 1);
			result = expand_value(cur_token->value, result, msh);
			free(cur_token->value);
			cur_token->value = result;
		}
		cur_node = cur_node->next;
	}
}
