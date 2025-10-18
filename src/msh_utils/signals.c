/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42vienna.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/17 13:54:05 by edarmaev          #+#    #+#             */
/*   Updated: 2025/10/18 04:22:54 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../minishell.h"

static void	return_prompt(void)
{
	rl_replace_line("", 0);
	ioctl(0, TIOCSTI, "\n");
	rl_on_new_line();
}

void	handle_signal_parent(int signal)
{
	if (signal == SIGINT)
	{
		g_signal_flag = signal;
		return_prompt();
	}	
}

void	handle_signal_child(int signal)
{
	if (signal == SIGINT)
		g_signal_flag = signal;
	rl_on_new_line();
	rl_replace_line("", 0);
}

int	handle_signal_heredoc(t_msh *msh, char **buffer)
{
	ft_free(buffer);
	close_heredocs(msh);
	msh->status = 128 + g_signal_flag;
	g_signal_flag = 0;
	return (1);	
}
