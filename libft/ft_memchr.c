/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 14:25:41 by edarmaev          #+#    #+#             */
/*   Updated: 2025/03/25 15:33:39 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t			i;
	unsigned char	chr;

	chr = c;
	i = 0;
	while (i < n)
	{
		if (((unsigned char *)s)[i] != chr)
			i++;
		else
			return (&((unsigned char *)s)[i]);
	}
	return (NULL);
}
