/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: astehl <astehl@student.42vienna.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/26 21:25:30 by astehl            #+#    #+#             */
/*   Updated: 2025/10/18 16:18:46 by astehl           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

void	free_token(void *content)
{
	t_token	*cur_token;

	cur_token = (t_token *)content;
	if (!cur_token)
		return ;
	if (cur_token->value)
		free(cur_token->value);
	free(cur_token);
}

static bool	skip_whitespaces(t_msh *msh)
{
	bool	skip;

	skip = false;
	while (*msh->cpy_in_str == ' ' || *msh->cpy_in_str == '\t')
	{
		msh->cpy_in_str++;
		skip = true;
	}
	return (skip);
}

static int	set_quoted(char c)
{
	int	quoted;

	quoted = Q_NONE;
	if (c == '\'')
		quoted = Q_SINGLE;
	else if (c == '"')
		quoted = Q_DOUBLE;
	return (quoted);
}

static bool	check_adjacency(t_list *tokens)
{
	t_list	*last_node;
	t_token	*last_token;

	if (!tokens)
		return (false);
	last_node = ft_lstlast(tokens);
	last_token = (t_token *)last_node->content;
	if (last_token->type == T_WORD)
		return (true);
	return (false);
}

void	msh_tokenizer(t_msh *msh)
{
	char	*word;
	int		quoted_adjacent[2];
	int		i;
	int		skipped;

	i = -1;
	while (*msh->cpy_in_str)
	{
		ft_memset(quoted_adjacent, 0, sizeof(quoted_adjacent));
		skipped = skip_whitespaces(msh);
		if (++i != 0 && !skipped)
			quoted_adjacent[1] = check_adjacency(msh->tokens);
		if (!*msh->cpy_in_str)
			break ;
		if (handle_metacharacters(msh))
			continue ;
		else
		{
			quoted_adjacent[0] = set_quoted(*msh->cpy_in_str);
			word = create_word_token(msh);
			(fill_token_list(msh, word, T_WORD, quoted_adjacent), free(word));
		}
	}
	//	printf("\nraw tokens:");
	//	print_tokens(msh->tokens);
	remove_quotationmarks_in_tokens(msh->tokens);
	//	printf("\nafter quote removal:");
	//	print_tokens(msh->tokens);
	expand_word_tokens(msh->tokens, NULL, msh);
	//	printf("\nafter expansion:");
	//	print_tokens(msh->tokens);
	split_expanded_tokens(msh->tokens, msh);
	//	printf("\nafter variable splitting:");
	//	print_tokens(msh->tokens);
	merge_adjacent_tokens(&msh->tokens, msh);
	//	printf("\nafter adjacency handling:");
}
