/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sminna <sminna@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/07 01:21:04 by sminna            #+#    #+#             */
/*   Updated: 2021/11/07 18:52:06 by sminna           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "pipex.h"

char	*check_patch(char **path, char *command)
{
	int		i;
	char	*path_l;
	char	*path_l_com;
	int		check;

	i = 0;
	while (path[i])
	{
		path_l = ft_strjoin(path[i], "/");
		path_l_com = ft_strjoin(path_l, command);
		free(path_l);
		check = open(path_l_com, O_RDONLY, 0777);
        /* Close fd*/
		if (check < 0)
			free(path_l_com);
		else
			return (path_l_com);
		i++;
	}
	return (NULL);
}

char	**find_path(char **envp)
{
	int		i;
	char	**path;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
		{
			path = ft_split(envp[i] + 5, ':');
			return (path);
		}
		i++;
	}
	perror("PATH not found");
	exit(0);
}

void	fork_child(int pipefd[2], char **envp, char **argv)
{
	int		file_one;
	char	**path;
	char	*exist;
	char	**comand;

	file_one = open (argv[1], O_RDONLY, 0);
	if (file_one < 0)
		perror("file not found");
	if (file_one < 0)
		exit (0);
	dup2(file_one, 0);
	dup2(pipefd[1], 1);
	close(pipefd[0]);
	path = find_path(envp);
	comand = ft_split(argv[2], ' ');
	exist = check_patch(path, comand[0]);
	if (!exist)
		perror("command not found");
	execve(exist, comand, envp);
}

void	fork_parent(int pipefd[2], char **envp, char **argv)
{
	int		file_two;
	char	**path;
	char	*exist;
	char	**comand;

	wait(0);
	file_two = open (argv[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (file_two < 0)
		perror("file not found");
	if (file_two < 0)
		exit (0);
	dup2(pipefd[0], 0);
	dup2(file_two, 1);
	close(pipefd[1]);
	path = find_path(envp);
	comand = ft_split(argv[3], ' ');
	exist = check_patch(path, comand[0]);
	if (!exist)
		perror("command not found");
	execve(exist, comand, envp);
}

int	main(int argc, char **argv, char **envp)
{
	int	pipefd[2];
	int	fo;

	if (argc != 5)
		return (0);
	if (pipe(pipefd) == -1)
		return (0);
	fo = fork();
	if (fo == -1)
		return (0);
	if (fo == 0)
		fork_child(pipefd, envp, argv);
	else
		fork_parent(pipefd, envp, argv);
	return (0);
}
