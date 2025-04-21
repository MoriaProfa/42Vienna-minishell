/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 14:22:17 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/21 22:52:54 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ft_printf.h"

static int	print_chr(char c, int fd)
{
	write(fd, &c, 1);
	return (1);
}

int	print_str(char *s, int fd)
{
	int	i;

	i = 0;
	if (s == NULL)
		s = "(null)";
	while (s[i] != '\0')
		write(fd, &s[i++], 1);
	return (i);
}

static int	print_int(int n, int fd)
{
	int		res;
	char	*str;

	str = ft_itoa(n);
	if (str == NULL)
		return (0);
	res = print_str(str, fd);
	return (free(str), res);
}

static int	format_specifier(char n, va_list arguments)
{
	if (n == 'c')
		return (print_chr(va_arg(arguments, int), 1));
	if (n == '%')
		return (print_chr('%', 1));
	if (n == 's')
		return (print_str(va_arg(arguments, char *), 1));
	if (n == 'd' || n == 'i')
		return (print_int(va_arg(arguments, int), 1));
	if (n == 'p')
		return (print_ptr(va_arg(arguments, void *), 1));
	if (n == 'u')
		return (print_uint(va_arg(arguments, unsigned int), 1));
	if (n == 'x')
		return (print_lhex(va_arg(arguments, int), 1));
	if (n == 'X')
		return (print_uhex(va_arg(arguments, int), 1));
	return (0);
}

int	ft_printf(const char *s, ...)
{
	int		i;
	int		f;
	va_list	arguments;

	if (s == NULL)
		return (-1);
	va_start(arguments, s);
	i = 0;
	f = 0;
	while (s[i])
	{
		if (s[i] == '%')
		{
			f = f + format_specifier(s[++i], arguments);
			i++;
		}
		else
			f = f + print_chr(s[i++], 1);
	}
	va_end(arguments);
	return (f);
}
