/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/05 15:54:43 by edarmaev          #+#    #+#             */
/*   Updated: 2025/03/25 15:33:20 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	c_counter(int n)
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

static char	*ft_strfill(char *str, int n, int c, int s)
{
	if (n < 0)
	{
		if (n == -2147483648)
			n = 2147483647;
		else
			n = -n;
	}
	str[c + s] = '\0';
	while (c > 0)
	{
		str[c + s - 1] = n % 10 + 48;
		n = n / 10;
		c--;
	}
	if (s == 1)
		str[0] = '-';
	return (str);
}

char	*ft_itoa(int n)
{
	char	*str;
	int		c;
	int		s;

	c = c_counter(n);
	s = 0;
	if (n < 0)
		s = 1;
	str = (char *)malloc(sizeof(char) * (c + s + 1));
	if (str == NULL)
		return (NULL);
	else
	{
		str = ft_strfill(str, n, c, s);
		if (n == -2147483648LL)
			str[c + s - 1] = '8';
	}
	return (str);
}
