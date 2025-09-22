/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/06 17:06:37 by edarmaev          #+#    #+#             */
/*   Updated: 2025/09/22 17:36:20 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static size_t	*ft_counter(const char *s, char c)
{
	size_t	*counter;
	size_t	i;
	size_t	j;
	size_t	k;

	counter = (size_t *)malloc(sizeof(size_t) * 2);
	if (counter == NULL)
		return (NULL);
	i = 0;
	j = 0;
	k = 0;
	while (s[i])
	{
		if (s[i] == c)
			j++;
		if (s[i] != c && (s[i + 1] == c || s[i + 1] == '\0'))
			k++;
		i++;
	}
	counter[0] = j;
	counter[1] = k;
	return (counter);
}

static char	*ft_stringcopy(char const *s, char c)
{
	char	*copy;
	int		i;

	i = 0;
	copy = (char *)malloc(sizeof(char) * (ft_strlen(s) + 1));
	if (copy == NULL)
		return (NULL);
	while (s[i])
	{
		copy[i] = s[i];
		i++;
	}
	copy[i] = '\0';
	while (i >= 0)
	{
		if (copy[i] == c)
			copy[i] = '\0';
		i--;
	}
	return (copy);
}

static char	**ft_fillarray(size_t counter, size_t len, char **arr, char *copy)
{
	size_t	k;
	size_t	j;
	size_t	i;

	i = 0;
	j = 0;
	if (copy == NULL)
		return (free(arr), NULL);
	while (j < counter)
	{
		k = 0;
		while (copy[i] == '\0' && i <= len)
			i++;
		arr[j] = (char *)malloc(sizeof(char) * (ft_strlen(&copy[i]) + 1));
		if (arr[j] == NULL)
		{
			ft_array_free(arr, sizeof(arr));
			return (free(copy), NULL);
		}
		while (copy[i] != '\0')
			arr[j][k++] = copy[i++];
		arr[j][k] = '\0';
		j++;
	}
	return (free(copy), arr[j] = NULL, arr);
}

char	**ft_split(char const *s, char c)
{
	char	**arr;
	size_t	*counter;

	if (!s)
		return (NULL);
	counter = ft_counter(s, c);
	if (counter == NULL)
		return (NULL);
	if (counter[0] == ft_strlen(s) || s[0] == '\0')
		counter[1] = 0;
	if (counter[0] == 0 && s[0] != '\0')
		counter[1] = 1;
	arr = (char **)malloc((counter[1] + 1) * sizeof(char *));
	if (arr == NULL)
		return (free(counter), NULL);
	if (counter[0] == ft_strlen(s) || s[0] == '\0')
		arr[0] = NULL;
	else
	{
		arr = ft_fillarray(counter[1], ft_strlen(s), arr, ft_stringcopy(s, c));
		if (arr == NULL)
			return (free(counter), NULL);
	}
	return (free(counter), arr);
}
