/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 14:22:17 by edarmaev          #+#    #+#             */
/*   Updated: 2024/11/22 17:12:16 by edarmaev         ###   ########.fr       */
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

static int	format_specifier(char n, va_list	arguments)
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
/*  
#include <stdio.h>
#include <stddef.h>

int	main(void)
{
	printf("og = %d\n", printf(\
	"he%s w%crld %%%%%i %d %X %x %p %p\n", \
	"llo", 'o', 42, -2147483647, -42, 42, "&ptr", NULL));
	ft_printf("og = %d\n", printf(\
	"he%s w%crld %%%%%i %d %X %x %p %p\n", \
	"llo", 'o', 42, -2147483647, -42, 42, "&ptr", NULL));
	ft_printf("my = %d\n", ft_printf(\
	"he%s w%crld %%%%%i %d %X %x %p %p\n", \
	"llo", 'o', 42, -2147483647, -42, 42, "&ptr", NULL));
	// tests by https://github.com/catherine21550
	// tests for strings
	printf("\n string tests:\n");
	//
	printf("\nlength is: %d\n", \
	ft_printf("test %s, %s, another test %s, after", \
	"string here", "str\0teere", NULL));
	printf("\nlength is: %d\n", \
	printf("test %s, %s, another test %s, after", \
	"string here", "str\0teere", (char *) NULL));
	// int tests
	printf("\n int tests:\n");
	//
	printf("\nlength is: %d\n", \
	ft_printf("test %d, Max %d, Min %d, after", 123, 2147483647, -2147483648));
	printf("\nlength is: %d\n", \
	printf("test %d, Max %d, Min %d, after", \
	123, (int) 2147483647, (int) -2147483648));
	printf("\nlength is: %d\n", \
	ft_printf("test %d, Max %d, Min %d, after", 123, 2147483648, -2147483649));
	printf("\nlength is: %d\n", \
	printf("test %d, Max %d, Min %d, after", \
	123, (int) 2147483648, (int) -2147483649));
	printf("\nlength is: %d\n", \
	ft_printf("test %i, Max %i, Min %i, after", 123, 2147483647, -2147483648));
	printf("\nlength is: %d\n", \
	printf("test %i, Max %i, Min %i, after", \
	123, (int) 2147483647, (int) -2147483648));
	printf("\nlength is: %d\n", \
	ft_printf("test %i, Max %i, Min %i, after", 123, 2147483648, -2147483649));
	printf("\nlength is: %d\n", \
	printf("test %i, Max %i, Min %i, after", \
	123, (int) 2147483648, (int) -2147483649));
	// unsigned int tests
	printf("\n unsigned int tests:\n");
	//
	printf("\nlength is: %d\n", ft_printf("test %u, Max %u, Min %u, after", \
	123, (unsigned int) 4294967295, 0));
	printf("\nlength is: %d\n", printf("test %u, Max %u, Min %u, after", \
	123, (unsigned int) 4294967295, 0));
	printf("\nlength is: %d\n", \
	ft_printf("test %u, Max %u, Min %u, after", \
	123, (unsigned int) 4294967296, (unsigned int) -1));
	printf("\nlength is: %d\n", \
	printf("test %u, Max %u, Min %u, after", \
	123, (unsigned int) 4294967296, (unsigned int) -1));
	// char tests
	printf("\n pointer tests:\n");
	//
	char	*test = "blabla";
	int		i = 0;
	printf("\nlength is: %d\n", \
	ft_printf("test %p, str %p, str %p, after int", test, &test[2], &test[4]));
	printf("\nlength is: %d\n", \
	printf("test %p, str %p, str %p, after int", test, &test[2], &test[4]));
	printf("\nlength is: %d\n", \
	ft_printf("test %p, null %p, after int", NULL, &i));
	printf("\nlength is: %d\n", \
	printf("test %p, null %p, after int", NULL, &i));
	// hex tests
	printf("\n hex tests:\n");
	//
	printf("\nlength is: %d\n", \
	ft_printf("test %x, Max %x, Min %x, after", \
	123, (unsigned int) 4294967295, 0));
	printf("\nlength is: %d\n", \
	printf("test %x, Max %x, Min %x, after", \
	123, (unsigned int) 4294967295, 0));
	printf("\nlength is: %d\n", \
	ft_printf("test %x, Max %x, Min %x, after", \
	123, (unsigned int) 4294967296, (unsigned int) -1));
	printf("\nlength is: %d\n", \
	printf("test %x, Max %x, Min %x, after", \
	123, (unsigned int) 4294967296, (unsigned int) -1));
	printf("\nlength is: %d\n", \
	ft_printf("test %x, Max %x, Min %x, after", \
	873, (unsigned int) 42949, (unsigned int) 9999));
	printf("\nlength is: %d\n", \
	printf("test %x, Max %x, Min %x, after", \
	873, (unsigned int) 42949, (unsigned int) 9999));
	ft_printf("\n");
	printf("\nlength is: %d\n", \
	ft_printf("test %X, Max %X, Min %X, after", \
	123, (unsigned int) 4294967295, 0));
	printf("\nlength is: %d\n", \
	printf("test %X, Max %X, Min %X, after", \
	123, (unsigned int) 4294967295, 0));
	printf("\nlength is: %d\n", \
	ft_printf("test %X, Max %X, Min %X, after", \
	123, (unsigned int) 4294967296, (unsigned int) -1));
	printf("\nlength is: %d\n", \
	printf("test %X, Max %X, Min %X, after", \
	123, (unsigned int) 4294967296, (unsigned int) -1));
	printf("\nlength is: %d\n", \
	ft_printf("test %X, Max %X, Min %X, after", \
	873, (unsigned int) 42949, (unsigned int) 9999));
	printf("\nlength is: %d\n", \
	printf("test %X, Max %X, Min %X, after", \
	873, (unsigned int) 42949, (unsigned int) 9999));
	// null check
	printf("\n NULL tests:\n");
	//
	printf("\nlength is: %d\n", ft_printf(NULL));
	printf("hey");
	printf("\nlength is: %d\n", printf(NULL));
	// character checks
	printf("\n character tests:\n");
	//
	printf("\nlength is: %d\n", \
	ft_printf("test %c, %c, another test %c, after", '?', 2, 200));
	printf("\nlength is: %d\n", \
	printf("test %c, %c, another test %c, after", '?', 2, 200));
	printf("\nlength is: %d\n", \
	ft_printf("test %c, %c, another test %c, after", 'a', 'b', '1'));
	printf("\nlength is: %d\n", \
	printf("test %c, %c, another test %c, after", 'a', 'b', '1'));
	// percentage checks
	printf("\n percentage%% tests:\n");
	//
	printf("\nlength is: %d\n", \
	ft_printf("test %%, %%, another test %%, after"));
	printf("\nlength is: %d\n", \
	printf("test %%, %%, another test %%, after"));
	// invalid format specifiers
	printf("\n invalid format specifiers");
	//
	printf("\nhey\n");
	ft_printf("%");
	printf("\nhey");
	printf("hey\n");
	printf("%");
	printf("\nhey\n");
	//
	printf("\nlength is: %d\n", \
	ft_printf("test %, %%, another test %%, after", 12));
	printf("\nlength is: %d\n", \
	printf("test %, %%, another test %%, after", 12));
	printf("\nlength is: %d\n", ft_printf("%%% %c"));
	printf("\nlength is: %d\n", printf("%%% %c"));
	// space " " is a flag (sub-specifier), handled only in the bonus part 
	return (0);
} */
