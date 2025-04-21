/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_uint.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 14:09:06 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/17 19:04:43 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	c_counter(unsigned int n)
{
	int	c;

	c = 1;
	while (n / 10 != 0)
	{
		n = n / 10;
		c++;
	}
	return (c);
}

static char	*ft_strfill(char *str, unsigned int n, int c)
{
	str[c] = '\0';
	while (c > 0)
	{
		str[c - 1] = n % 10 + 48;
		n = n / 10;
		c--;
	}
	return (str);
}

static char	*ft_utoa(unsigned int n)
{
	char	*str;
	int		c;

	c = c_counter(n);
	str = (char *)malloc(sizeof(char) * (c + 1));
	if (str == NULL)
		return (NULL);
	str = ft_strfill(str, n, c);
	return (str);
}

int	print_uint(unsigned int n, int fd)
{
	int		res;
	char	*str;

	str = ft_utoa(n);
	if (str == NULL)
		return (0);
	res = print_str(str, fd);
	return (free(str), res);
}
/* 
#include <stdio.h>
#include <string.h>

int	main(void)
{
	unsigned int	n = -4294967296;

	printf("unsigned int n 	= %u\n", n);
	printf("ft_utoa	: %s\n", ft_utoa(n));
	printf("len	: %zu\n", strlen(ft_utoa(n)));
	return (0);
} */