/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 16:07:29 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/20 13:07:11 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*s;

	if (nmemb * size > INT_MAX)
		return (NULL);
	s = malloc(nmemb * size);
	if (s == NULL)
		return (NULL);
	else
		ft_bzero(s, nmemb * size);
	return (s);
}
