/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexing.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/05 20:48:16 by astehl            #+#    #+#             */
/*   Updated: 2025/08/27 14:14:47 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	handle_quotationmark_error(t_msh *msh, char quotationmark)
{
	char	str[2];

	str[0] = quotationmark;
	str[1] = '\0';
	ft_putstr_fd("minishell: unexpected EOF while looking for matching `", 2);
	(ft_putstr_fd(str, 2), ft_putendl_fd("'", 2));
	msh_end(msh, "minishell: syntax error: unexpected end of file");
}

void	check_quotationmarks(t_msh *msh)
{
	int		i;
	char	quotationmark;

	i = -1;
	quotationmark = 0;
	while (msh->in_str[++i])
	{
		if (msh->in_str[i] == '\'' || msh->in_str[i] == '"')
		{
			if (i > 0 && msh->in_str[i - 1] == '\\')
				continue ;
			if (quotationmark == 0)
				quotationmark = msh->in_str[i];
			else if (quotationmark == msh->in_str[i])
				quotationmark = 0;
		}
	}
	if (quotationmark != 0)
		handle_quotationmark_error(msh, quotationmark);
}
