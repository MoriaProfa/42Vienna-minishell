/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/04 20:51:09 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/20 13:15:48 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	f_counter(char const *s1, char const *set)
{
	size_t	f;
	size_t	i;

	f = 0;
	i = 0;
	while (i < ft_strlen(s1) && ft_strchr(set, s1[i]) != NULL)
	{
		i++;
		f++;
	}
	return (f);
}

static size_t	b_counter(char const *s1, char const *set)
{
	size_t	b;
	size_t	i;

	b = 0;
	i = 0;
	while (i < ft_strlen(s1) && ft_strrchr(set, s1[ft_strlen(s1) - 1
				- i]) != NULL)
	{
		i++;
		b++;
	}
	return (b);
}

char	*ft_strtrim(char const *s1, char const *set)
{
	char	*strtrim;
	size_t	i;
	size_t	a;

	i = 0;
	if (!s1 || !set)
		return (NULL);
	if (f_counter(s1, set) == ft_strlen(s1) || b_counter(s1,
			set) == ft_strlen(s1))
		a = 1;
	else
		a = ft_strlen(s1) - f_counter(s1, set) - b_counter(s1, set) + 1;
	strtrim = (char *)ft_calloc(a, sizeof(char));
	if (strtrim == NULL)
		return (NULL);
	else
	{
		while (i < a - 1)
		{
			strtrim[i] = s1[f_counter(s1, set) + i];
			i++;
		}
		strtrim[i] = '\0';
	}
	return (strtrim);
}
