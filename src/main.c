/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/17 14:51:58 by qpeng             #+#    #+#             */
/*   Updated: 2019/04/27 18:38:50 by qpeng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "traceroute.h"

bool	g_timeout;

void	sig_alarm(int sig)
{
	g_timeout = true;		
	return ;
}


int		get_ip_v(char *src)
{
	char buf[100];

	if (inet_pton(AF_INET, src, buf))
		return (4);
	else if (inet_pton(AF_INET6, src, buf))
		return (6);
	return (0);
}

void	         tv_sub(struct timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

uint16_t		in_cksum(uint16_t *addr, int len)
{
	uint32_t	sum;
	uint16_t	answer;

	answer = 0;
	sum = 0;
	while (len > 1)
	{
		sum += *addr++;
		len -= 2;
	}
	if (len == 1)
	{
		*(unsigned char *)(&answer) = *(unsigned char *)addr;
		sum += answer;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	answer = ~sum;
	return (answer);
}

void		send_v4()
{
	int				len;
	struct icmp		*icmp;

	icmp = (struct icmp *)g_ping->sendbuff;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_id = g_ping->pid;
	icmp->icmp_code = 0;
	icmp->icmp_seq = 0;
	memset(icmp->icmp_data, 0x77, DATALEN);
	ERR_PROMPT(gettimeofday((struct timeval *)icmp->icmp_data, NULL) == -1,
		"Failed to get timestamp for icmp packet.");
	len = 8 + DATALEN;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = in_cksum((u_short *)icmp, len);
	sendto(g_ping->sockfd, g_ping->sendbuff, len, 0, g_ping->sserv, g_ping->sservlen);
}

void		send_v6()
{
	int					len;
	struct icmp6_hdr	*icmp;

	icmp = (struct icmp6_hdr *)g_ping->sendbuff;
	icmp->icmp6_type = ICMP6_ECHO_REQUEST;
	icmp->icmp6_id = g_ping->pid;
	icmp->icmp6_code = 0;
	icmp->icmp6_seq = 0;
	memset((icmp + 1), 0x77, DATALEN);
	ERR_PROMPT(gettimeofday((struct timeval *)(icmp + 1), NULL) == -1,
		"Failed to get timestamp for icmp6 packet.");
	len = 8 + DATALEN;
	sendto(g_ping->sockfd, g_ping->sendbuff, len, 0, g_ping->sserv, g_ping->sservlen);
}
/*
void		readmsg_v6(int b_read, char *recvbuff)
{
	struct icmp6_hdr	*icmp6;
	struct timeval		tvrecv;
	double				rrt;
	int					hlim;

	gettimeofday(&tvrecv, NULL);
	icmp6 = (struct icmp6_hdr *)recvbuff;
	if (b_read < 8)
		return ;
	if (icmp6->icmp6_type == ICMP6_ECHO_REPLY)
	{
		if (icmp6->icmp6_id != gl.pid || b_read < 16)
			return ;
		tv_sub(&tvrecv, (struct timeval *)(icmp6 + 1));
		rrt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec / 1000.0;
		return (rrt);
	}
    return (-1);
}
*/
int			readmsg_v4(struct timeval *tvrecv)
{
	struct ip		*iphdr;
	struct icmp		*icmp;
	char			*tmp_ip;
	char			recvbuff[BUF_SIZ];
	int				len;
	int				hdrlen;
	int 			b_read;
	int 			ret;
	double			rrt;

	while (true)
	{
		len = g_ping->sservlen;
		b_read = recvfrom(g_ping->sockfd, recvbuff, sizeof(recvbuff), 0, g_ping->sserv, &len);
		if (b_read < 0)
		{
			if (errno == EINTR)
				continue ;
			else if (errno == ETIMEDOUT)
				return (RECV_TIMEOUT);
			else 
				ERR_QUIT("recvfrom");
		}
		iphdr = (struct ip *)recvbuff;
		hdrlen = iphdr->ip_hl << 2;
		if (iphdr->ip_p != IPPROTO_ICMP || (b_read - hdrlen) < 8)
			continue ;
		icmp = (struct icmp *)(recvbuff + hdrlen);
		if (icmp->icmp_type == ICMP_ECHOREPLY || 
			(icmp->icmp_type == ICMP_TIMXCEED && icmp->icmp_code == ICMP_TIMXCEED_INTRANS))
		{
			if (icmp->icmp_id != g_ping->pid || (b_read - hdrlen) < 16)
				continue ;
			tv_sub(tvrecv, (struct timeval *)icmp->icmp_data);
			if (icmp->icmp_type == ICMP_ECHOREPLY)
				ret = RECV_REACHED;
			else
			{	
				tmp_ip = inet_ntoa(iphdr->ip_src);
				strcmp(g_ping->this_ip, tmp_ip);
				ret = RECV_TTLEXCEED;
			}
		}
	}
	gettimeofday(tvrecv, NULL);
	return (ret);
}

void                _recv(t_ping *ping, double *rrt)
{
    int             b_recv;
    int             slen;
    char		    recvbuff[BUF_SIZ];

    slen = ping->sservlen;
    b_recv = recvfrom(ping->sockfd, recvbuff, sizeof(recvbuff), 0, ping->sserv, &slen);
    
}

char				*reverse_dns_lookup(char *ip_addr)
{
	struct sockaddr_in	temp_addr;
	socklen_t			len;
	char				buf[100];
	static char			ret_buf[200];

	temp_addr.sin_family = AF_INET;
	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
	len = sizeof(struct sockaddr_in);
	if (getnameinfo((struct sockaddr *)&temp_addr, len, buf,
					sizeof(buf), NULL, 0, NI_NAMEREQD))
		return (ip_addr);
	strcpy(ret_buf, buf);
	return (ret_buf);
}

struct addrinfo		*host_to_addrinfo(char *host,\
						const char *serv, int family, int socktype)
{
	struct addrinfo hints;
	struct addrinfo *res;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;
	if (getaddrinfo(host, serv, &hints, &res))
	{
		fprintf(stderr, "ping: %s: Name or service not known\n", host);
		exit(EXIT_FAILURE);
	}
	return (res);
}

void				ping_init(char *host)
{
	struct addrinfo		*ret;
	void				*ptr;
	int					version;
    double              rtt;

	version = get_ip_v(host);
	ret = host_to_addrinfo(host, NULL, (version == 4 || version == 0) ?
		AF_INET : AF_INET6, SOCK_STREAM);
    if (ret->ai_family == AF_INET)
		ptr = &((struct sockaddr_in *)ret->ai_addr)->sin_addr;
	else
		ptr = &((struct sockaddr_in6 *)ret->ai_addr)->sin6_addr;
	g_ping->sserv = ret->ai_addr;
	g_ping->sservlen = ret->ai_addrlen;
	g_ping->pid = getpid();
	g_ping->send = IS_IPV4(ret->ai_family) ? send_v4 : send_v6;
	g_ping->recv = IS_IPV4(ret->ai_family) ? readmsg_v4 : NULL;
	g_ping->proto = IS_IPV4(ret->ai_family) ? &proto_v4 : &proto_v6;
	inet_ntop(ret->ai_family, ptr, g_ping->ip, 100);
	printf("%s\n", g_ping->ip);
}

int				sock_init()
{
    struct timeval  timeout;

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    g_ping->sockfd = socket(g_ping->sserv->sa_family, SOCK_RAW, g_ping->proto->icmp_proto);
	ERR_CHECK(g_ping->sockfd == -1, "socket");
    ERR_CHECK(setsockopt(g_ping->sockfd, SOL_SOCKET, SO_RCVTIMEO,\
		(void *)&timeout, sizeof(timeout)) == -1, "setsockopt");
}

void                readloop(void)
{
	struct 	timeval tvrecv;
	double			rtt;
	int				ret;
	bool 			printed;

	for (int ttl = 1; ttl <= g_config.max_ttl; ttl++)
	{
		ERR_PROMPT(setsockopt(g_ping->sockfd, g_ping->proto->level,\
					g_ping->proto->ttl_opt, &ttl, sizeof(ttl)) == -1,
					"Failed to set ttl value for the socket.");
		printf("%2d ", ttl);
		fflush(stdout);
		printed = false;
		bzero(g_ping->last_ip, IP_SIZ);
		for (int probe = 0; probe < g_config.max_probe; probe++)
		{
			g_ping->send();
			ret = g_ping->recv(&tvrecv);
			if (ret == RECV_TIMEOUT)
				printf(" *");
			else if(ret == RECV_TTLEXCEED ||
					ret == RECV_REACHED)
			{
				if (!printed)
				{
					printf("%s ", g_ping->this_ip);
					strcpy(g_ping->last_ip, g_ping->this_ip);
					printed = 1;
				}
				else
				{
					if (strcmp(g_ping->this_ip, g_ping->last_ip) != 0)
						printf("\n    %s ", g_ping->this_ip);	
				}
				rtt = tvrecv.tv_sec * 1000.0 * tvrecv.tv_usec / 1000.0;
				printf(" %3f", rtt);
			}
		}
		fflush(stdout);
		write(1, "\n", 1);
		if (strcmp(g_ping->ip, g_ping->this_ip) == 0)
			return ;
	}

	// host = reverse_dns_lookup(ping->ip);
	// g_ping->send(&g_ping);
    // g_ping->recv(&g_ping, &rtt);
}

void				env_init(void)
{

	// gl.times = INT32_MAX;
	// gl.duration = 1;
	// gl.r_ns_lookup = true;
	// gl.pid = getpid() & 0xffff;
	// gl.ttl = 64;
	// signal(SIGALRM, sig_alrm);
	// signal(SIGINT, sig_int);
	//
	g_config.max_ttl = 30;
	g_config.max_probe = 3;

}

int					main(int ac, char **av)
{
	t_ping	ping;

	g_ping = &ping;
	env_init();
	//readopt(ac, av);
	ping_init(av[1]);
	sock_init();
	readloop();
	return (0);
}
