/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edarmaev <edarmaev@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/21 11:00:15 by edarmaev          #+#    #+#             */
/*   Updated: 2025/04/22 21:06:24 by edarmaev         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	msh_free(t_msh *msh)
{
	if (msh->in_str)
		ft_free(&msh->in_str);
	if (msh->cmd.cmd)
		ft_free_array(msh->cmd.cmd);
	if (msh->cmd.pathname)
		ft_free(&msh->cmd.pathname);
}

void	msh_exit(t_msh *msh, char *msg)
{
	ft_putendl_fd(msg, 2);
	msh_free(msh);
	exit(msh->status);
}

void	msh_execute(t_msh *msh)
{
	if (!msh->cmd.pathname)
	{
		msh->status = 127;
		msh_exit(msh, "command not found");
	}
	else if (msh->cmd.pathname && access(msh->cmd.pathname, X_OK) == -1)
	{
		msh->status = 126;
		msh_exit(msh, "permission denied");
	}
	else if (msh->cmd.pathname && access(msh->cmd.pathname, X_OK) == 0)
		execve(msh->cmd.pathname, msh->cmd.cmd, msh->envp);
	msh_exit(msh, "execve failed");
}

char	*msh_pathname_r(t_msh *msh, char **paths)
{
	char	*pathname;
	char	*tmp;
	int		i;

	i = -1;
	pathname = NULL;
	while (paths[++i])
	{
		tmp = ft_strjoin(paths[i], "/");
		if (!tmp)
			msh_exit(msh, "ft_strjoin failed");
		pathname = ft_strjoin(tmp, msh->cmd.cmd[0]);
		free(tmp);
		if (!pathname)
			msh_exit(msh, "ft_strjoin failed");
		if (access(pathname, F_OK) == 0)
			return (pathname);
		free(pathname);
	}
	return (NULL);
}

char	*msh_pathname(t_msh *msh)
{
	char	**paths;
	char	*pathname;

	pathname = NULL;
	if (access(msh->cmd.cmd[0], F_OK) == 0)
		pathname = msh->cmd.cmd[0];
	else
	{
		while (*msh->envp && ft_strncmp(*msh->envp, "PATH=", 5) != 0)
			msh->envp++;
		if (*msh->envp)
		{
			paths = ft_split(*msh->envp + 5, ':');
			if (!paths)
				msh_exit(msh, "ft_split failed");
			pathname = msh_pathname_r(msh, paths);
			ft_free_array(paths);
		}
	}
	return (pathname);
}

char	*msh_read(ssize_t rr)
{
	char	buf[2];
	char	*tmp;
	char	*out;

	out = NULL;
	while (rr > 0)
	{
		rr = read(0, buf, 1);
		if (rr == -1)
			return (ft_free(&out), NULL);
		buf[rr] = 0;
		if (buf[0] == '\n')
			return (out);
		if (!out)
			out = ft_strcopy(buf);
		else
		{
			tmp = out;
			out = ft_strjoin(tmp, buf);
			ft_free(&tmp);
		}
	}
	return (out);
} 

void	msh_fork(t_msh *msh)
{
	msh->pid.pid = fork();
	if (msh->pid.pid == -1)
		msh_exit(msh, "fork failed");
	if (msh->pid.pid == 0)
	{
		msh_execute(msh);
	}
}

t_cmd	*msh_new_cmd(t_msh *msh, char *content)
{
	t_cmd	*new;

	new = (t_cmd *)malloc(sizeof(t_cmd));
	if (!new)
		msh_exit(msh, "malloc failed");
	new->cmd = ft_split(content, ' ');
	if (!new->cmd)
		msh_exit(msh, "split failed");
	new->pathname = msh_pathname(msh);
	new->next = NULL;
	return (new);
}

t_cmd	*msh_add_cmd(t_msh *msh, char *content)
{
	t_cmd	*new;
	t_cmd	*current;

	new = msh_new_cmd(msh, content);
	if (!new->cmd)
		msh_exit(msh, "couldn't create new entry");
	if (!&msh->cmd)
		msh->cmd = *new;
	else
	{
		current = &msh->cmd;
		while (current->next)
			current = current->next;
		current->next = new;
	}
	return (new);
}

void	msh_initialize(t_msh *msh, char **envp)
{
	t_cmd	tmp;

	msh->status = 1;
	msh->in_str = msh_read(1);
	if (!msh->in_str)
		msh_exit(msh, "couldn't read");
	msh->in_arr = ft_split(msh->in_str, '|');
	while (*msh->in_arr)
	{
		ft_printf("%s\n", *msh->in_arr++);
	}
	// exit(42);
	if (!msh->in_arr)
		msh_exit(msh, "split failed");
	while (*msh->in_arr)
	{
		tmp = *msh_new_cmd(msh, *msh->in_arr++);
		msh->cmd = tmp;
	}
	msh->envp = envp;
	// msh->cmd.cmd = ft_split(msh->in_arr[0], ' ');
	// if (!msh->cmd.cmd)
	// 	msh_exit(msh, "split failed");
	// msh->envp = ft_copy_array(envp);
	// if (!msh->envp)
	// 	msh_exit(msh, "array copy failed");
	// msh->cmd.pathname = msh_pathname(msh);
}

int	main(int argc, char **argv, char **envp)
{
	t_msh	msh;

	if (argc > 2)
		ft_printf("%s\n", "oops");
	while (argv[0])
	{
		msh_initialize(&msh, envp);
		msh_fork(&msh);
		msh_free(&msh);
	}
	return (0);
}
