/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:55:34 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/21 12:28:28 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_h

# include "libft/libft.h"
# include "ft_printf/ft_printf.h"

typedef struct s_cmd
{
	char	**cmd;
	char	**envp;
	char	*pathname;
}			t_cmd;

#endif
