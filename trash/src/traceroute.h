/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/03 20:51:22 by qpeng             #+#    #+#             */
/*   Updated: 2019/05/17 11:52:39 by qpeng            ###   ########.fr       */
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
# define IS_IPV4(ip) (ip == AF_INET)
# define PRINT_ERROR(msg) ({fprintf(stderr, "%s\n", msg);})
# define FETAL(msg) ({fprintf(stderr, "%s\n", msg); exit(EXIT_FAILURE);})
# define ERR_PROMPT(cond, msg)({if(cond)PRINT_ERROR(msg);})
# define ERR_QUIT(f) ({perror(f); exit(EXIT_FAILURE);})
# define ERR_CHECK(cond, f) ({if(cond)ERR_QUIT(f);})
# define RECV_SOCK_TIMEOUT -1
# define RECV_SUCCESS 1
# define RECV_TTLEXCEED 2
# define RECV_REACHED 3

typedef struct s_proto
{
    int             level;
    int             ttl_opt;
    int             icmp_proto;

}               t_proto;

typedef struct  s_ping
{
	struct sockaddr *sserv;
	int             sservlen;
    int             sockfd;
    pid_t             pid;
    void            (*send)(void);
    int             (*recv)(struct timeval *);
    char            sendbuff[BUF_SIZ];
    char	    *ip;
    char            this_ip[IP_SIZ];
    char            last_ip[IP_SIZ];
    t_proto         *proto;
}               t_ping;

// typedef struct  s_probe
// {
//     bool        status;
//     char        *host;
//     char        *ip;
//     double      rtt;
// }               t_probe;

// typedef struct  traceroute
// {
// 	struct sockaddr		*ssend;
// 	int					ssendlen;
// }               t_traceroute;

typedef struct  s_config
{
    int         max_ttl;
    int         max_probe;
    int         verbose;
}               t_config;

t_config    g_config;

t_ping      *g_ping;
// bool        g_SOCK_TIMEOUT;

t_proto     proto_v4 = {
    IPPROTO_IP,
    IP_TTL,
    IPPROTO_ICMP
};

t_proto     proto_v6 = {
    IPPROTO_IPV6,
    IPV6_UNICAST_HOPS,
    IPPROTO_ICMPV6
};
#endif
