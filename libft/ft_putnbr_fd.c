/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 16:51:19 by edarmaev          #+#    #+#             */
/*   Updated: 2025/03/25 15:35:05 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_putnbr_fd(int n, int fd)
{
	char	c;

	if (n < 0)
	{
		if (n == -2147483648LL)
		{
			write(fd, "-2147483648", 11);
			return ;
		}
		else
		{
			n = n * -1;
			write(fd, "-", 1);
		}
	}
	if (n >= 0 && n < 10)
		c = n + '0';
	if (n > 9)
	{
		ft_putnbr_fd(n / 10, fd);
		c = n % 10 + '0';
	}
	write (fd, &c, 1);
}
