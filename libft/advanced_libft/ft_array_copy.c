/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_array_copy.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 10:29:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/22 17:37:24 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	**ft_array_copy(char **array)
{
	char	**copy;
	int		i;

	copy = NULL;
	if (!array)
		return (NULL);
	i = 0;
	while (array[i])
		i++;
	copy = (char **)ft_calloc(i + 1, sizeof(char *));
	if (!copy)
		return (NULL);
	while (--i >= 0)
	{
		copy[i] = ft_strcpy(array[i]);
		if (!copy[i])
		{
			ft_array_free(copy, sizeof(copy));
			return (NULL);
		}
	}
	return (copy);
}
