/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 16:07:47 by edarmaev          #+#    #+#             */
/*   Updated: 2025/03/25 15:39:53 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*copy;
	size_t	size;

	size = ft_strlen(s) + 1;
	copy = (char *)malloc(size * sizeof(char));
	if (copy == NULL)
		return (NULL);
	else
	{
		ft_memmove(copy, s, size);
	}
	return (copy);
}
