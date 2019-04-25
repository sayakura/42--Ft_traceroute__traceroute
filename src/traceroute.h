/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/03 20:51:22 by qpeng             #+#    #+#             */
/*   Updated: 2019/04/22 17:13:01 by qpeng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PING_H
# define PING_H

# include <stdio.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <string.h>
# include <stdlib.h>
# include <netinet/ip_icmp.h>
# include <time.h>
# include <fcntl.h>
# include <signal.h>
# include <stdbool.h>
# include <errno.h>
# include <sys/time.h>
# include <memory.h>
# include <ifaddrs.h>
# include <net/if.h>
# include <stdarg.h>
# include <resolv.h>
# include <netdb.h>
# include <netinet/in.h>
# include <netinet/icmp6.h>

# define HOP_LIMIT 30
# define DATALEN 56
# define BUF_SIZ 1024
# define IP_SIZ 100

typedef struct  s_ping
{
	struct sockaddr *sserv;
	int             sservlen;
    
    int             ttl;
    int             sockfd;
    int             pid;
    int             seq;
    int             protocol;
    void            (*send)(t_ping *);
    void            (*recv)(t_ping *, double *rtt);
    char            sendbuff[BUF_SIZ];
    char            ip[IP_SIZ];
}               t_ping;

typedef struct  s_probe
{
    bool        status;
    char        *host;
    char        *ip;
    double      rtt;
}               t_probe;

typedef struct  traceroute
{
	struct sockaddr		*ssend;
	int					ssendlen;
}               t_traceroute;


t_ping      g_ping;

#endif