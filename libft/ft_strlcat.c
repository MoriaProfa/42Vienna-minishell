/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/06/14 10:51:32 by edarmaev          #+#    #+#             */
/*   Updated: 2025/03/25 15:41:26 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t size)
{
	size_t	i;
	size_t	j;
	size_t	k;
	size_t	m;

	i = ft_strlen(src);
	if (size == 0 && dst == NULL)
		return (i);
	j = ft_strlen(dst);
	k = 0;
	m = size - j - 1;
	if (size > j)
	{
		while (k < m && src[k] != '\0')
		{
			dst[j + k] = src[k];
			k++;
		}
		dst[j + k] = '\0';
		return (j + i);
	}
	else
		return (i + size);
}
