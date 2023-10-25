/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.c                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bde-meij <bde-meij@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/18 15:35:24 by bde-meij      #+#    #+#                 */
/*   Updated: 2022/04/14 19:29:00 by bde-meij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.hpp"

void	initiate_save(int fd, char **saved, int *phase)
{
	int	bytes_read;

	*saved = ft_calloc(BUFFER_SIZE, sizeof(char *));
	*phase += 1;
	bytes_read = read(fd, *saved, BUFFER_SIZE);
	if (bytes_read == -1)
	{
		*phase = 4;
		if (*saved)
			free(*saved);
		*saved = NULL;
		return ;
	}
}

int	find_nline(char *saved)
{
	int	count;

	count = 0;
	if (saved)
	{	
		while (saved[count])
		{
			if (saved[count] == '\n')
				return (count + 1);
			count++;
		}
	}
	return (-1);
}

int	read_and_alloc(int fd, char **saved, char **tmp_save)
{
	static char	buffer[BUFFER_SIZE + 1];

	ft_bzero(buffer, BUFFER_SIZE);
	read(fd, buffer, BUFFER_SIZE);
	*tmp_save = *saved;
	*saved = ft_strjoin(*saved, buffer);
	free(*tmp_save);
	if (!*saved)
		return (4);
	if (!*buffer)
		return (3);
	return (2);
}

char	*get_next_line(int fd)
{
	static char	*saved;
	char		*next_line;
	char		*tmp_save;
	static int	phase = 1;

	next_line = NULL;
	if (phase == 1)
		initiate_save(fd, &saved, &phase);
	while ((find_nline(saved) == -1) && (phase == 2) && (saved))
		phase = read_and_alloc(fd, &saved, &tmp_save);
	if (phase == 3)
	{
		phase += 1;
		if (*saved)
			return (saved);
		else
			free(saved);
	}
	if (phase == 4 || saved == NULL)
		return (NULL);
	tmp_save = saved;
	next_line = ft_substr(saved, 0, find_nline(saved));
	saved = ft_substr(saved, find_nline(saved), ft_strlen(saved));
	free(tmp_save);
	return (next_line);
}
