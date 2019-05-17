/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/14 08:13:25 by qpeng             #+#    #+#             */
/*   Updated: 2019/05/14 08:13:26 by qpeng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "traceroute.h"

bool                 g_alarmed;

void    sig_alrm(int signo)
{
    (void)signo;
    printf("kicks in!\n");
    g_alarmed = true;    
    return ;
}