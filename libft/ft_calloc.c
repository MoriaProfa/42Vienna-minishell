/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_calloc.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/02 16:07:29 by edarmaev          #+#    #+#             */
/*   Updated: 2025/03/25 15:32:20 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_calloc(size_t nmemb, size_t size)
{
	void	*s;

	if (nmemb * size > SIZE_MAX)
		return (NULL);
	s = malloc(nmemb * size);
	if (s == NULL)
		return (NULL);
	else
		ft_bzero(s, nmemb * size);
	return (s);
}
