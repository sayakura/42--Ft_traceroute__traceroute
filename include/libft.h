/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/13 02:10:05 by qpeng             #+#    #+#             */
/*   Updated: 2019/05/13 02:11:38 by qpeng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
#define LIBFT_H

#include <errno.h>
#include <stdio.h>
#include <stddef.h>
#include <string.h>

void				perror_(const char *s);
int					strcmp_(const char *rdi, const char *rsi);
void				bzero_(void *rsi, size_t rcx);
char				*strcpy_(char *rdi, const char *rsi);
int                 atoi_(const char *str);

#endif