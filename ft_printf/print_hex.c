/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_hex.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/19 13:22:24 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/17 21:32:42 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	ft_counter(unsigned int n)
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

static char	*ft_convert(int n)
{
	char			*str;
	int				c;
	unsigned int	neg;

	c = ft_counter(n);
	neg = n;
	str = (char *)malloc(sizeof(char) * (c + 1));
	if (str == NULL)
		return (NULL);
	str[c--] = '\0';
	if (neg == 0)
		str[c] = '0';
	while (neg)
	{
		if (neg % 16 == 0)
			str[c] = '0';
		else if (neg % 16 > 0 && neg % 16 < 10)
			str[c] = neg % 16 + '0';
		else if (neg % 16 > 9)
			str[c] = neg % 16 + 'A' - 10;
		neg = neg / 16;
		c--;
	}
	return (str);
}

int	print_uhex(int n, int fd)
{
	int		res;
	char	*str;

	str = ft_convert(n);
	if (str == NULL)
		return (0);
	res = print_str(str, fd);
	return (free(str), res);
}

int	print_lhex(int n, int fd)
{
	int		i;
	int		res;
	char	*str;

	i = 0;
	str = ft_convert(n);
	if (!str)
		return (0);
	while (str && str[i])
	{
		str[i] = ft_tolower(str[i]);
		i++;
	}
	res = print_str(str, fd);
	return (free(str), res);
}

/* #include <stdio.h>

int	main(void)
{
	int	n = -154;
	int	p;

	printf("\nprint_lhex = %d\n", print_lhex(n, 1));
	printf("%d in HEX is 	%X\n", n, n);
	printf("%d in hex is 	%x\n", n, n);
	p = printf("%x", n);
	printf("\nprintf(n in x) is %d\n", p);
	return (0);
} */