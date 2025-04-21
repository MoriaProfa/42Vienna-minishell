/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_ptr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/20 19:14:14 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/21 12:06:54 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_counter(long unsigned int n)
{
	int	counter;

	counter = 1;
	while (n / 16 != 0)
	{
		n = n / 16;
		counter++;
	}
	return (counter);
}

static char	*ft_convert(long unsigned int n)
{
	char		*str;
	int			c;

	c = ft_counter(n) + 2;
	str = (char *)malloc(sizeof(char) * (c + 3));
	if (str == NULL)
		return (NULL);
	str[c--] = '\0';
	if (n == 0)
		str[c] = '0';
	while (n)
	{
		if (n % 16 == 0)
			str[c] = '0';
		else if (n % 16 > 0 && n % 16 < 10)
			str[c] = n % 16 + '0';
		else if (n % 16 > 9)
			str[c] = n % 16 + 'a' - 10;
		n = n / 16;
		c--;
	}
	str[c--] = 'x';
	str[c] = '0';
	return (str);
}

int	print_ptr(void *p, int fd)
{
	int					res;
	char				*str;
	unsigned long int	uli;

	if (p != NULL)
	{
		uli = (unsigned long int)p;
		str = ft_convert(uli);
		if (str == NULL)
			return (0);
		res = print_str(str, fd);
		free(str);
	}
	else
	{
		str = "(nil)";
		res = print_str(str, fd);
	}
	return (res);
}
/* 
#include <stdio.h>
#include <string.h>

int	main(void)
{
	printf("\nog = %d\n", printf("%p", "ptr"));
	printf("\nmy = %d\n", print_ptr("ptr", 1));
	return (0);
} */