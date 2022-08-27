#include "../inc/irondome.h"

char	*ft_strjoin(char const *s1, char const *s2)
{
	char	*ret;
	char	*ret_cpy;

	if (!s1 || !s2)
		return (NULL);
	ret = calloc(strlen(s1) + strlen(s2) + 1, 1);
	ret_cpy = ret;
	if (ret == NULL)
		return (NULL);
	while (*s1 != 0)
		*(ret++) = *(s1++);
	while (*s2 != 0)
		*(ret++) = *(s2++);
	return (ret_cpy);
}
