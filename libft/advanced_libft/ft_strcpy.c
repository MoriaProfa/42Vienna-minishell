/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strcpy.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 20:34:06 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/20 13:16:12 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../libft.h"

char	*ft_strcpy(char *str)
{
	char	*copy;
	int		i;
	size_t	size;

	i = 0;
	if (str)
	{
		size = ft_strlen(str) + 1;
		copy = (char *)ft_calloc(size, sizeof(char));
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
