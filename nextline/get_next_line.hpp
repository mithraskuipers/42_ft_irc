/* ************************************************************************** */
/*                                                                            */
/*                                                        ::::::::            */
/*   get_next_line.h                                    :+:    :+:            */
/*                                                     +:+                    */
/*   By: bde-meij <bde-meij@student.codam.nl>         +#+                     */
/*                                                   +#+                      */
/*   Created: 2022/01/18 15:30:18 by bde-meij      #+#    #+#                 */
/*   Updated: 2022/02/27 16:10:22 by bde-meij      ########   odam.nl         */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_HPP
# define GET_NEXT_LINE_HPP

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 15
# endif

# include	<stdlib.h>
# include	<unistd.h>

char	*get_next_line(int fd);
void	ft_bzero(char *s, size_t n);
size_t	ft_strlen(const char *string);
char	*ft_calloc(size_t count, size_t size);
char	*ft_strjoin(char const *s1, char const *s2);
char	*ft_substr(char const *s, unsigned int start, size_t len);

#endif
