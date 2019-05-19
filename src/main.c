#include <stdio.h>
#include "traceroute.h"

char				*g_hostname;
int					g_sendfd;
uint16_t 			g_dport;
uint16_t			g_sport;
uint8_t 			g_inital_ttl;
uint8_t 			g_max_ttl;
uint8_t 			g_probes;
uint32_t 			g_waittime;
struct addrinfo		*g_addrinfo;
struct sockaddr		g_serrecv;
int                 g_recvfd;

void			readopt(int ac, char **av);

void 	clean_up(void)
{
    if (g_recvfd >=0 )
	    close(g_recvfd);
    if (g_sendfd >=0 )
	    close(g_sendfd);
    if (g_addrinfo)
	    freeaddrinfo(g_addrinfo);
}


void	tv_sub(struct timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
}

void 	creat_sock(void)
{
	uid_t				uid;
	struct sockaddr_in	serbind; 
    struct timeval      timeout;

	if ((uid = getuid()) != 0)
		FATAL("Need run premission to create raw socket.");
	if ((g_recvfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1
        || (g_sendfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        clean_up();
        ERR_QUIT("socket");
    }

    timeout.tv_sec = g_waittime;
    timeout.tv_usec = 0;
    if (setsockopt(g_recvfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
                sizeof(timeout)) < 0)
    {
        clean_up();
        ERR_QUIT("setsockopt");
    }

	serbind.sin_family = AF_INET;
	g_sport = (getpid() & 0xffff) | 0x8000;
	serbind.sin_port = htons(g_sport);
	if (bind(g_sendfd, (struct sockaddr *)(&serbind), sizeof(serbind)) == -1)
	{
        clean_up();
        INFO("Bind failed.");
        ERR_QUIT("bind");
    }
}

void 	init(void)
{
    g_dport = DEFAULT_PORT_NUM;
	g_inital_ttl = INITIAL_TTL;
	g_max_ttl = MAX_TTL;
	g_probes = DEFAULT_PROBE;
	g_waittime = DEFAULT_WAIT;
    g_recvfd = g_sendfd = -1;
}

int		wait_and_recv(int seq, struct timeval *tv)
{
	int				ret;
	ssize_t			b_read;
	uint16_t		ip_out_len, ip_in_len, icmplen;
	char			recvbuf[777];
	struct ip 		*ip_hdr_out, *ip_hdr_in;
	struct icmp 	*icmp_hdr;
	struct udphdr 	*udp_hdr;

    ret = 0;
	while (true)
	{
        b_read = recvfrom(g_recvfd, recvbuf, sizeof(recvbuf), 0, &g_serrecv, (unsigned []){sizeof(g_serrecv)});
		if (b_read < 0)
		{
			if (errno == EAGAIN)
                return (SOCK_TIMEOUT);
			else
				perror_("recvfrom");
		}
		ip_hdr_out = (struct ip *)recvbuf;
		ip_out_len = ip_hdr_out->ip_hl << 2;

		if ((icmplen = b_read - ip_out_len) < ICMP_HDR_LEN)
			continue ; 
		if (icmplen < ICMP_HDR_LEN + sizeof(struct ip))
			continue ;

		icmp_hdr = (struct icmp *)(recvbuf + ip_out_len);
		if (icmp_hdr->icmp_type == ICMP_TIMXCEED ||icmp_hdr->icmp_type == ICMP_UNREACH) 
		{
			ip_hdr_in = (struct ip *)(recvbuf + ip_out_len + ICMP_HDR_LEN);
			ip_in_len = ip_hdr_in->ip_hl << 2;
			if (icmplen < ICMP_HDR_LEN + ip_in_len + (UDP_HDR_LEN / 2))
				continue ;

			udp_hdr = (struct udphdr *)(recvbuf + ip_out_len + ICMP_HDR_LEN + ip_in_len);
      
			if (ip_hdr_in->ip_p == IPPROTO_UDP
				&& udp_hdr->uh_sport == htons(g_sport)
				&& udp_hdr->uh_dport == htons(g_dport + seq))
			{
                ret = (icmp_hdr->icmp_code);
                break ;
            }
		}
		else 
		{
			continue ;
		}
	}
    gettimeofday(tv, NULL);
    return (ret);
	return (UNREACHABLE);
}


void 	traceroute(void)
{
	struct sockaddr_in 		serlast;
	struct s_content		*data;
	struct timeval			recvtv;
	double					rtt;
	u_int16_t				seq;
	int						code;
    int                     b_sent;
	char					sendbuf[777];
	char					hostname[NI_MAXHOST];
    bool                    arrived;
    
    printf("traceroute to %s (%s), %d hops max, %ld byte packets\n",
                            g_hostname, 
                            inet_ntoa(((struct sockaddr_in *)g_addrinfo->ai_addr)->sin_addr),
                            g_max_ttl,
                            sizeof(struct s_content));
    
    seq = 0;
    arrived = false;
    (void)b_sent;
    for (u_int8_t ttl = g_inital_ttl; ttl <= g_max_ttl; ttl++)
	{
		ERR_CHECK(setsockopt(g_sendfd, IPPROTO_IP, IP_TTL, (int[]){(int)ttl}, sizeof(int)) == -1,\
			"setsockopt");
		bzero_(&serlast, sizeof(serlast));
		printf("%2d ", ttl);
		fflush(stdout);
		for (uint8_t probe = 0; probe < g_probes; probe++)
		{
			data = (struct s_content *)(sendbuf);
			data->seq = ++seq;
			data->ttl = ttl;
			gettimeofday(&data->recv_time, NULL);
			((struct sockaddr_in *)g_addrinfo->ai_addr)->sin_port = htons(g_dport + seq);
			b_sent = sendto(g_sendfd, sendbuf, sizeof(struct s_content), 0, g_addrinfo->ai_addr, g_addrinfo->ai_addrlen);
            code = wait_and_recv(seq, &recvtv);
			if (code == SOCK_TIMEOUT)
				printf (" *");
			else 
			{
				if (serlast.sin_addr.s_addr != ((struct sockaddr_in *)&g_serrecv)->sin_addr.s_addr)
				{
					if (getnameinfo(&g_serrecv, sizeof(struct sockaddr), hostname, NI_MAXHOST, NULL, 0, 0) == 0)
						printf(" %s (%s)", hostname, inet_ntoa(((struct sockaddr_in *)&g_serrecv)->sin_addr));
					else
						printf(" %s", inet_ntoa(((struct sockaddr_in *)&g_serrecv)->sin_addr));
				}
				memcpy(&serlast, &g_serrecv, sizeof(struct sockaddr_in));
				tv_sub(&recvtv , &data->recv_time);
				rtt = recvtv.tv_sec * 1000.0 + recvtv.tv_usec / 1000.0;
				printf(" %.3f ms", rtt);
			}
            if (code == ICMP_UNREACH_PORT)
                arrived = true;
            else if (code != ICMP_TIMXCEED_INTRANS && code != SOCK_TIMEOUT)
				printf (" (ICMP %d)", code);
            fflush(stdout);
		}
        printf("\n");
        if (arrived)
				break ;
	}
}

int		main(int ac, char **av)
{
    init();
	readopt(ac, av);
	creat_sock();
	traceroute();
	clean_up();
	return (0);
}