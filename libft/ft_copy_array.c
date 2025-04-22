/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_copy_array.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/22 10:29:56 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/22 10:30:25 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	**ft_copy_array(char **array)
{
	char	**copy;
	int		i;

	copy = NULL;
	if (!array)
		return (NULL);
	i = 0;
	while (array[i])
		i++;
	copy = (char **)malloc(sizeof(char *) * (i + 1));
	if (!copy)
		return (NULL);
	copy[i] = NULL;
	while (--i >= 0)
	{
		copy[i] = ft_strcopy(array[i]);
		if (!copy[i])
		{
			ft_free_array(copy);
			return (NULL);
		}
	}
	return (copy);
}
