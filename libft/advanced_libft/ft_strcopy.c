/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcopy.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 20:34:06 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/17 19:07:19 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	*ft_strcopy(char *str)
{
	char	*copy;
	int		i;
	size_t	size;

	i = 0;
	if (str)
	{
		size = ft_strlen(str) + 1;
		copy = (char *)malloc(size * sizeof(char));
		if (copy == NULL)
			return (NULL);
		while (str[i])
		{
			copy[i] = str[i];
			i++;
		}
		copy[i] = '\0';
		return (copy);
	}
	return (NULL);
}
