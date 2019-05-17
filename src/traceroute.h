/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   traceroute.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/05/13 01:10:34 by qpeng             #+#    #+#             */
/*   Updated: 2019/05/17 09:08:16 by qpeng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef TRACEROUTE_H
#define TRACEROUTE_H

#include <stdlib.h>
#include <stdbool.h> // bool
#include <inttypes.h> // portable integer types
#include <sys/time.h> // gettimeofday
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h> // inet_ntoa
#include <netinet/udp.h>
#include <sys/types.h>
#include <netdb.h>

#include <signal.h>
#include "libft.h"
#include <unistd.h>

// macros
#define GET_BIT(X,N) (((X) >> (N)) & 1)
#define SET_BIT(X,N) ((X) | (1 << (N)))
#define RST_BIT(X,N) ((X) & ~(1 << (N)))
#define NUM_OF(x) (sizeof (x) / sizeof (*x))

// consts
#define INITIAL_TTL 1
#define MAX_TTL 30
#define DEFAULT_PROBE 3
#define DEFAULT_WAIT 5
#define DEFAULT_PORT_NUM 33434
#define UNREACHABLE 2147483647
#define ICMP_HDR_LEN 8
#define UDP_HDR_LEN 8
#define ALARMED -2

enum    e_flags
{
	F_FLAGS = 0b00000001,
	M_FLAGS = 0b00000010,
	P_FLAGS = 0b00000100,
	S_FLAGS = 0b00001000,
	W_FLAGS = 0b00010000
};

#define HAS_FLAG(name) name##_FLAGS & g_flags

# define FMT "usage\n"
# define INFO(msg) printf(msg)
# define PRINT_USAGE ({printf(FMT); exit(EXIT_SUCCESS);})
# define FATAL(msg) ({fprintf(stderr, "%s\n", msg); exit(EXIT_FAILURE);})
# define ERR_QUIT(f) ({perror_(f); exit(EXIT_FAILURE);})
# define ERR_CHECK(cond, f) ({if(cond)ERR_QUIT(f);})

// data content 
struct  s_content
{
		uint16_t		seq;
		uint16_t		ttl;
		struct timeval	recv_time;
};
/**
 *  global variable for storing all the flags info.
 *  0 = not used
 *  |0|0|0|w|S|p|m|f|
 *   8 7 6 5 4 3 2 1
 */
extern uint8_t              g_flags;

extern uint8_t              g_inital_ttl;
extern uint8_t              g_max_ttl;
extern uint8_t              g_probes;
extern uint8_t              g_waittime;
extern uint16_t             g_port;
extern char *               g_hostname;
extern char                 g_rhostname[NI_MAXHOST];
extern struct addrinfo      *g_addrinfo;
extern int                  g_sendfd;
extern int                  g_recvfd;
extern bool                 g_alarmed;
extern uint16_t				g_sport;
extern uint16_t 			g_dport;
extern struct sockaddr		g_serrecv;

struct addrinfo		        *host_to_addrinfo(char *host, int family, int socktype);
char						*code_tostring(int code);
void                        sig_alrm(int signo);
#endif