/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: qpeng <qpeng@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/04/17 14:51:58 by qpeng             #+#    #+#             */
/*   Updated: 2019/04/22 19:51:39 by qpeng            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "traceroute.h"

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

void		send_v4(t_ping *ping)
{
	int			len;
	struct icmp	*icmp;

	icmp = (struct icmp *)ping->sendbuff;
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_id = ping->pid;
	icmp->icmp_seq = ping->seq++;
	memset(icmp->icmp_data, 0xa5, DATALEN);
	gettimeofday((struct timeval *)icmp->icmp_data, NULL);
	len = 8 + DATALEN;
	icmp->icmp_cksum = 0;
	icmp->icmp_cksum = in_cksum((u_short *)icmp, len);
	sendto(ping->sockfd, ping->sendbuff, len, 0, ping->sserv, ping->sservlen);
}

void		send_v6(t_ping *ping)
{
	int					len;
	struct icmp6_hdr	*icmp;

	icmp = (struct icmp6_hdr *)ping->sendbuff;
	icmp->icmp6_type = ICMP6_ECHO_REQUEST;
	icmp->icmp6_code = 0;
	icmp->icmp6_id = ping->pid;
	icmp->icmp6_seq = ping->seq++;
	memset((icmp + 1), 0x77, DATALEN);
	gettimeofday((struct timeval *)(icmp + 1), NULL);
	len = 8 + DATALEN;
	sendto(ping->sockfd, ping->sendbuff, len, 0, ping->sserv, ping->sservlen);
}

void		readmsg_v6(int b_read, char *recvbuff)
{
	struct icmp6_hdr	*icmp6;
	struct timeval		tvrecv;
	int					hlim;
	double				rrt;

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

double		readmsg_v4(int b_read, char *recvbuff)
{
	struct ip		*iphdr;
	struct icmp		*icmp;
	struct timeval	tvrecv;
	int				hdrlen;
	double			rrt;

	gettimeofday(&tvrecv, NULL);
	iphdr = (struct ip *)recvbuff;
	hdrlen = iphdr->ip_hl << 2;
	if (iphdr->ip_p != IPPROTO_ICMP || (b_read - hdrlen) < 8)
		return ;
	icmp = (struct icmp *)(recvbuff + hdrlen);
	if (icmp->icmp_type == ICMP_ECHOREPLY)
	{
		if (icmp->icmp_id != gl.pid || (b_read - hdrlen) < 16)
			return ;
		tv_sub(&tvrecv, (struct timeval *)icmp->icmp_data);
		rrt = tvrecv.tv_sec * 1000.0 + tvrecv.tv_usec / 1000.0;
		return (rrt);
	}
    return (-1);
}

void                _recv(t_ping *ping, double *rrt)
{
    int             b_recv;
    int             slen;
    char		    recvbuff[BUFF_SIZ];

  
    slen = ping->sservlen;
    b_recv = recvfrom(ping->sockfd, recvbuff, sizeof(recvbuff), 0, ping->sserv, &slen);
    *rrt = ping->recv(b_recv, recvbuff);
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

void				ping(int sockfd, char host, int ttl)
{
	struct addrinfo		*ret;
	void				*ptr;
    t_ping              ping;
    t_probe             probe;
	int					version;
    double              rtt;

	version = get_ip_v(host);
	ret = host_to_addrinfo(host, NULL, (version == 4 || version == 0) ?
		AF_INET : AF_INET6, SOCK_STREAM);
    if (ret->ai_family == AF_INET)
		ptr = &((struct sockaddr_in *)ret->ai_addr)->sin_addr;
	else
		ptr = &((struct sockaddr_in6 *)ret->ai_addr)->sin6_addr;
	ping.sserv = ret->ai_addr;
	ping.sservlen = ret->ai_addrlen;
	ping.send = (ret->ai_family == AF_INET) ? send_v4 : send_v6;
	ping.recv = (ret->ai_family == AF_INET) ? readmsg_v4 : readmsg_v6;
	ping.protocol = (ret->ai_family == AF_INET) ? IPPROTO_ICMP : IPPROTO_ICMPV6;
	inet_ntop(ret->ai_family, ptr, ping.ip, 100);
	probe.host = reverse_dns_lookup(ping.ip);
	ping.send(&ping);
    ping.recv(&ping, &rtt);
}

void                readloop(char *host)
{
    int             sockfd;
    struct timeval  timeout;

    timeout.tv_sec = 3;
    timeout.tv_usec = 0;
    sockfd = socket(AF_INET, SOCK_RAW, 0);
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, sizeof(timeout)) < 0)
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    ping(sockfd, host, 1);
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
}

int					main(int ac, char **av)
{
	env_init();
	readopt(ac, av);
	ping_init();
	readloop();
	return (0);
}