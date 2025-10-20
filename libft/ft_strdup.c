/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 16:07:47 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/20 13:14:48 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*copy;
	size_t	size;

	size = ft_strlen(s) + 1;
	copy = (char *)ft_calloc(size, sizeof(char));
	if (copy == NULL)
		return (NULL);
	else
	{
		ft_memmove(copy, s, size);
	}
	return (copy);
}
