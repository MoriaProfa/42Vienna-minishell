/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils_atol.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 18:33:57 by astehl            #+#    #+#             */
/*   Updated: 2025/10/17 23:46:27 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static bool	numcheck(char c)
{
	if (c >= 48 && c <= 57)
		return (true);
	else
		return (false);
}

static int	signcheck(char c)
{
	if (c == 45)
		return (-1);
	else
		return (1);
}

static bool	check_overflow(long result, char digit, int sign)
{
	if (result > LONG_MAX / 10 || \
		(result == LONG_MAX / 10 && \
			((sign == 1 && digit - '0' > LONG_MAX % 10) || \
			(sign == -1 && digit - '0' > -(LONG_MIN % 10)))))
		return (true);
	return (false);
}

long	msh_atol(t_msh *msh, char *nptr)
{
	long	result;
	int		i;
	int		sign;

	result = 0;
	i = 0;
	sign = 1;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if ((nptr[i] == 43 || nptr[i] == 45))
		sign = signcheck(nptr[i++]);
	while (numcheck(nptr[i]))
	{
		if (check_overflow(result, nptr[i], sign))
			print_num_error_exit(msh, nptr);
		result = result * 10 + (nptr[i] - '0');
		if (!numcheck(nptr[i + 1]))
			break ;
		i++;
	}
	return (result * sign);
}
