/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_printf.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/18 14:46:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/21 12:06:46 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef FT_PRINTF_H
# define FT_PRINTF_H
# include <stdarg.h>
# include "../libft/libft.h"

int	ft_printf(const char *s, ...);
int	print_uint(unsigned int n, int fd);
int	print_str(char *s, int fd);
int	print_lhex(int n, int fd);
int	print_uhex(int n, int fd);
int	print_ptr(void *p, int fd);

#endif
