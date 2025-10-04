/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_array_copy.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 10:29:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/23 05:23:52 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	**ft_array_copy(char **array)
{
	char	**copy;
	size_t	i;
	size_t	size;

	if (!array)
		return (NULL);
	i = 0;
	size = 0;
	while (array[size])
		size++;
	copy = (char **)ft_calloc(size + 1, sizeof(char *));
	if (!copy)
		return (NULL);
	while (i < size)
	{
		copy[i] = ft_strcpy(array[i]);
		if (!copy[i])
		{
			ft_array_free(copy, i);
			return (NULL);
		}
		i++;
	}
	return (copy);
}
