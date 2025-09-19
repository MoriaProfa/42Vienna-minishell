/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_array_free.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/25 10:59:06 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/19 23:15:26 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

void	ft_array_free(char **arr, size_t size)
{
	size_t	i;

	i = 0;
	if (!arr)
		return ;
	while (i < size)
		free(arr[i++]);
	free(arr);
}

/*void	ft_array_free(char **arr)
{
	int	i;

	i = -1;
	while (arr[++i])
		free(arr[i]);
	free(arr);
}*/
