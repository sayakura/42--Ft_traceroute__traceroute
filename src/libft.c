/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/13 02:09:22 by qpeng             #+#    #+#             */
/*   Updated: 2019/05/13 02:11:16 by qpeng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void		perror_(const char *s)
{
	if (errno < 0)
		fprintf(stderr, "Unknown error : %d\n", errno);
	else
		fprintf(stderr, "%s: %s\n", s,\
									strerror(errno));
}

int			strcmp_(const char *rdi, const char *rsi)
{
	while (*rdi == *rsi++)
		if (*rdi++ == 0)
			return (0);
	return (*(unsigned char *)rdi - *(unsigned char *)--rsi);
}

void		bzero_(void *rsi, size_t rcx)
{
	char	*rdi;

	rdi = (char *)rsi;
	while (rcx--)
	{
		*rdi = 0;
		rdi++;
	}
}

char		*strcpy_(char *rdi, const char *rsi)
{
	char	*rax;

	rax = rdi;
	while ((*rdi++ = *rsi++))
		;
	return (rax);
}

int		isdigit_(char c)
{
	return (c >= '0' && c <= '9');
}

int 	isspace_(char c)
{
	return (c == ' ' ||
			c == '\t' ||
			c == '\n' ||
			c == '\f' ||
			c == '\v' ||
			c == '\r' ||
			c == '\t');
}

int     atoi_(const char *str)
{
	unsigned long long	result;
	unsigned int		index;
	char				sign;

	index = 0;
	result = 0;
	sign = 1;
	while (isspace_(str[index]))
		index++;
	if (str[index] == '+' || str[index] == '-')
	{
		if (str[index] == '-')
			sign = 0;
		index++;
	}
	while (isdigit_(str[index]))
	{
		result = result * 10 + (str[index] - '0');
		if (result > 9223372036854775807)
		{
            if (sign)
				return (-1);
			else
				return (0);
        }
		index++;
	}
	return (int)((sign) ? (result) : (-result));
}