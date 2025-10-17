/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expansion.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/25 16:29:54 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 20:04:41 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

char	*get_env_var_value(t_msh *msh, char *name);
void	close_heredocs(t_msh *msh);
void	close_fds(t_msh *msh);

static void	add_to_result(char *tmp, char **result, size_t *i, t_msh *msh)
{
	char	*new_result;

	if (!tmp)
	{
		if (*result)
			free(*result);
		msh_quit(msh, "expander: tmp substr/itoa malloc failed", 1);
	}
	if (*result)
	{
		new_result = ft_strjoin(*result, tmp);
		free(*result);
		free(tmp);
		if (!new_result)
			msh_quit(msh, "expander: new_result strjoin malloc failed", 1);
		*result = new_result;
	}
	else
		*result = tmp;
	(*i)++;
}

void	close_and_exit(t_msh *msh, char *msg, int status)
{
	close_fds(msh);
	msh_quit(msh, msg, status);
}

static void	exp_getenv(t_token *token, char *var_name, char ***result, t_msh *msh)
{
	char		*env_value;
	char		*tmp;

	env_value = get_env_var_value(msh, var_name);
	free(var_name);
	if (env_value)
	{
		if (**result)
			tmp = ft_strjoin(**result, env_value);
		else
			tmp = ft_strdup(env_value);
		free(env_value);
		free(**result);
		if (!tmp)
			close_and_exit(msh, "expander: env_value join malloc failed", 1);
		**result = tmp;
	}
	else
	{
		if (**result && (**result[0] != '\0' || 
				(token && token->quoted == Q_DOUBLE)))
			return ;
		free(**result);
		**result = NULL;
	}	
}

static void	exp_getenv_value(t_token *token, char *value, char **result, size_t *i, t_msh *msh)
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
		msh_quit(msh, "expander: var_name substr malloc failed", 1);
	}
	exp_getenv(token, var_name, &result, msh);
}

char	*expand_value(t_token *token, char *value, char *result, t_msh *msh)
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
				exp_getenv_value(token, value, &result, &i, msh);
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
	char	*tmp;
	char	*expanded;

	cur_node = tokens;
	while (cur_node)
	{
		cur_token = (t_token *)cur_node->content;
		if (cur_token->type == T_WORD && cur_token->quoted != Q_SINGLE
			&& ft_strchr(cur_token->value, '$'))
		{
			if (!cur_node->prev || (cur_node->prev && 
				((t_token *)cur_node->prev->content)->type != T_HEREDOC_REDIRECT))
			{
				tmp = ft_strdup("");
				if (!tmp)
					msh_quit(msh, "expander: tmp init malloc failed", 1);
				expanded = expand_value(cur_token, cur_token->value, tmp, msh);
				free(cur_token->value);
				cur_token->value = expanded;
				cur_token->is_expanded = true;
			}
		}
		cur_node = cur_node->next;
	}
}
