/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mde-arpe <mde-arpe@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/03/23 01:53:51 by mde-arpe          #+#    #+#             */
/*   Updated: 2022/09/01 22:18:41 by mde-arpe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/irondome.h"

static int	count_words(char const *s, char c)
{
	int		num_words;
	int		in_word;

	num_words = 0;
	in_word = 0;
	while (*s)
	{
		if (*s != c)
		{
			if (!in_word)
				num_words++;
			in_word = 1;
		}
		else
			in_word = 0;
		s++;
	}
	return (num_words);
}

static int	len_till_next__word(char const *s, char c)
{
	int	counter;

	counter = 0;
	while (*s && *s != c)
	{	
		counter++;
		s++;
	}
	return (counter);
}

static char	**norminette_while(char **ret, char const *s, char c)
{
	int	inner_counter;
	int	counter;

	inner_counter = 0;
	counter = -1;
	while (*s)
	{
		if (*s != c)
		{
			inner_counter = 0;
			ret[++counter] = calloc(len_till_next__word(s, c) + 1, 1);
			if (!(ret[counter]))
			{	
				while (counter >= 0)
					free(ret[counter--]);
				return (NULL);
			}
			while (*s != c && *s)
				ret[counter][inner_counter++] = *(s++);
		}
		else
			s++;
	}
	ret[++counter] = NULL;
	return (ret);
}

char	**ft_split(char const *s, char c)
{
	char	**ret;

	if (!s)
		return (NULL);
	ret = calloc(count_words(s, c) + 1, sizeof(char *));
	if (!ret)
		return (NULL);
	ret = norminette_while(ret, s, c);
	if (!ret)
	{	
		free(ret);
		return (NULL);
	}
	return (ret);
}
