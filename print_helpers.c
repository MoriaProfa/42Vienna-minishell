
#include "minishell.h"

// valgrind command:
// valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --track-fds=all --trace-children=yes --suppressions=minishell.supp ./minishell
// pwd test < file1 > file2 >> file3 test2 << file4 << "file5" | cd hello | ls test3 test4 test5 test6 test7

void	print_tokens(t_list *tokens); // for testing only
void	print_cmd_table(t_msh *msh);  // for testing only

// just for testing purposes
void	print_tokens(t_list *tokens)
{
	t_list	*cur_node;
	t_token	*cur_token;
	int		i;

	cur_node = tokens;
	i = 1;
	printf("\nToken list:\n");
	while (cur_node)
	{
		cur_token = (t_token *)(cur_node->content);
		if (cur_token)
			printf("Token #%i:\n\tValue: %s, Type: %d, Quotes: %d, is adjacent:	%d\n", i, cur_token->value, cur_token->type, cur_token->quoted,
				cur_token->is_adjacent);
		cur_node = cur_node->next;
		i++;
	}
}

// just for testing purposes
void	print_cmd_table(t_msh *msh)
{
	int		i;
	t_cmd	*cmd;
	int		j;

	i = 0;
	if (!msh->cmd_table)
		return ;
	printf("\n=== Command Table (n = %i) ===\n", msh->n_cmds);
	while (msh->cmd_table[i] && i < msh->n_cmds)
	{
		cmd = msh->cmd_table[i];
		printf("Command %i:\n", i);
		printf("\targs (%i): ", cmd->n_args);
		j = 0;
		while (cmd->args[j] && j < cmd->n_args)
			printf("[%s] ", cmd->args[j++]);
		printf("\n\tBuiltin args[0]: %d\n", cmd->builtin.builtin);
		if (cmd->redirs)
		{
			j = -1;
			printf("\tredirs (%i): ", cmd->n_redirs);
			while (cmd->redirs[++j].filename && j < cmd->n_redirs)
				printf("[%s (type = %d)] ", cmd->redirs[j].filename,
					cmd->redirs[j].type);
			printf("\n");
		}
		if (cmd->heredocs)
		{
			j = -1;
			printf("\theredocs (%i): ", cmd->n_hd);
			while (cmd->heredocs[++j].delimiter && j < cmd->n_hd)
				printf("[%s (expand = %d)] ", cmd->heredocs[j].delimiter,
					cmd->heredocs[j].expand);
			printf("\n");
		}
		printf("\tlast_input: %i\n", cmd->last_input);
		// 0 if T_NONE, 3 if T_IN_REDIR, 6 if T_HEREDOC
		if (cmd->pathname)
			printf("\tpathname: %s\n", cmd->pathname);
		i++;
	}
}
