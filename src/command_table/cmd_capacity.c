/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_capacity.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 22:09:25 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 22:12:49 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	*grow_array(void *old_array, int old_capacity, int new_capacity,
		size_t size)
{
	void	*new_array;

	new_array = ft_calloc(new_capacity, size);
	if (!new_array)
		return (NULL);
	ft_memcpy(new_array, old_array, old_capacity * size);
	free(old_array);
	return (new_array);
}

bool	ensure_capacity(void **array, int *count, int *capacity, size_t size)
{
	int		new_capacity;
	void	*new_array;

	if (*count + 1 >= *capacity)
	{
		new_capacity = (*capacity) * 2;
		new_array = grow_array(*array, *capacity, new_capacity, size);
		if (!new_array)
			return (false);
		*array = new_array;
		*capacity = new_capacity;
	}
	return (true);
}

void	cmd_arrays_capacity_check(t_cmd *cmd, t_msh *msh)
{
	if (!ensure_capacity((void **)&cmd->args, &cmd->n_args, &cmd->capacity_args,
			sizeof(char *)))
		msh_quit(msh, "cmd args array capacity increase malloc failed", 1);
	if (!ensure_capacity((void **)&cmd->redirs, &cmd->n_redirs,
			&cmd->capacity_redirs, sizeof(t_redir)))
		msh_quit(msh, "cmd redirs array capacity increase malloc failed", 1);
	if (!ensure_capacity((void **)&cmd->heredocs, &cmd->n_hd, &cmd->capacity_hd,
			sizeof(t_heredoc)))
		msh_quit(msh, "cmd heredocs array capacity increase malloc failed", 1);
}
