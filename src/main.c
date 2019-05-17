#include <stdio.h>
#include "traceroute.h"

char				*g_hostname;
int					g_sendfd;
 uint16_t 			g_dport;
 uint16_t			g_sport;
uint8_t 			g_inital_ttl;
uint8_t 			g_max_ttl;
uint8_t 			g_probes;
uint8_t 			g_waittime;
uint16_t			g_port;
bool                g_SOCK_TIMEOUT;
char				g_rhostname[NI_MAXHOST];
struct addrinfo		*g_addrinfo;
struct sockaddr		g_serrecv;
int                 g_recvfd;

void			readopt(int ac, char **av);

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
	int					sendfd;
	int 				recvfd;
	struct sockaddr_in	serbind; 
    struct timeval      SOCK_TIMEOUT;

    SOCK_TIMEOUT.tv_sec = 3;
    SOCK_TIMEOUT.tv_usec = 0;
	if ((uid = getuid()) != 0)
		FATAL("Need run premission to create raw socket.");

	recvfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	ERR_CHECK(recvfd == -1, "socket");
	sendfd = socket(AF_INET, SOCK_DGRAM, 0);
	ERR_CHECK(sendfd == -1, "socket");

    if (setsockopt (recvfd, SOL_SOCKET, SO_RCVTIMEO, (char *)&SOCK_TIMEOUT,
                sizeof(SOCK_TIMEOUT)) < 0)
        FATAL("setsockopt failed\n");
	//setuid(uid);
	
	serbind.sin_family = AF_INET;
	g_sport = (getpid() & 0xffff) | 0x8000;
    //printf("g_sport2 = %d\n", htons(g_sport));
	serbind.sin_port = htons(g_sport);
	if (bind(sendfd, (struct sockaddr *)(&serbind), sizeof(serbind)) == -1)
		INFO("Bind failed.");
	g_sendfd = sendfd;
	g_recvfd = recvfd;
}

void 	init(void)
{
	if (!g_hostname)
	{
		printf("usage.\n");
		exit(EXIT_SUCCESS);
	}
    signal(SIGALRM, sig_alrm);
	g_addrinfo = host_to_addrinfo(g_hostname, AF_INET, SOCK_DGRAM);
	ERR_CHECK(g_addrinfo == NULL, "host_to_addrinfo");
	if (getnameinfo(g_addrinfo->ai_addr, g_addrinfo->ai_addrlen, g_rhostname,\
													NI_MAXHOST, NULL, 0, 0) != 0)
		perror_("getnameinfo");
    g_dport = DEFAULT_PORT_NUM;
	g_inital_ttl = INITIAL_TTL;
	g_max_ttl = MAX_TTL;
	g_probes = DEFAULT_PROBE;
	g_waittime = DEFAULT_WAIT;
	g_port = DEFAULT_PORT_NUM;
}

void 	clean_up(void)
{
	close(g_recvfd);
	close(g_sendfd);
	freeaddrinfo(g_addrinfo);
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

	// g_SOCK_TIMEOUT = false;
    ret = 0;
    // alarm(2);
	while (true)
	{
		// if (g_SOCK_TIMEOUT)
		// 	return (SOCK_TIMEOUT);// expired
		//printf("run! alarm? %s\n", g_SOCK_TIMEOUT ? "true" : "false");
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

        //printf("outer ip len: %d\n", ip_out_len);
		if ((icmplen = b_read - ip_out_len) < ICMP_HDR_LEN)
			continue ; 
		if (icmplen < ICMP_HDR_LEN + sizeof(struct ip))
			continue ;
        //printf("icmplen: %d\n", icmplen);
		icmp_hdr = (struct icmp *)(recvbuf + ip_out_len);
		if (icmp_hdr->icmp_type == ICMP_TIMXCEED ||icmp_hdr->icmp_type == ICMP_UNREACH) 
		{
            //printf("icmp_hdr->icmp_type == ICMP_TIMXCEED ?: %d\n", icmp_hdr->icmp_type == ICMP_TIMXCEED);
			ip_hdr_in = (struct ip *)(recvbuf + ip_out_len + ICMP_HDR_LEN);
			ip_in_len = ip_hdr_in->ip_hl << 2;
            //printf("inner ip len: %d\n", ip_in_len);
			if (icmplen < ICMP_HDR_LEN + ip_in_len + (UDP_HDR_LEN / 2))
				continue ;

			udp_hdr = (struct udphdr *)(recvbuf + ip_out_len + ICMP_HDR_LEN + ip_in_len);
            // printf("[udp_hdr->uh_sport = %d] \n", udp_hdr->uh_sport);
            //  printf("[htons(g_sport) = %d] \n",  htons(g_sport));
            // printf("[udp_hdr->uh_dport = %d] \n", udp_hdr->uh_dport);
            // printf("g_dport: %d %d\n", g_dport, seq);
            //  printf("[htons(g_dport + seq) = %d] \n",  htons(g_dport + seq));
            // printf("[icmp_hdr->icmp_code = %d] \n", icmp_hdr->icmp_code);
            // printf("ip_hdr_in->ip_p == IPPROTO_UDP ? %d\n", ip_hdr_in->ip_p == IPPROTO_UDP);
           
        //    printf("%d\n", ip_hdr_in->ip_p == IPPROTO_UDP);
        //    printf("%d\n", udp_hdr->uh_sport == htons(g_sport));
        //    printf("%d\n", udp_hdr->uh_dport == htons(g_dport + seq));
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
			//verbose
			continue ;
		}
	}
    //alarm(0);
    gettimeofday(tv, NULL);
    return (ret);
	return (UNREACHABLE);
}


void 	readloop(void)
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

    
    seq = 0;
	sig_alrm(SIGALRM);
    for (u_int8_t ttl = g_inital_ttl; ttl < g_max_ttl; ttl++)
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
			printf("b_sent: %d\n", b_sent);
            code = wait_and_recv(seq, &recvtv);
			if (code == SOCK_TIMEOUT)
				printf (" *");
			else 
			{
				if (serlast.sin_addr.s_addr != ((struct sockaddr_in *)&g_serrecv)->sin_addr.s_addr)
				{
					if (getnameinfo(&g_serrecv, sizeof(struct sockaddr), hostname, NI_MAXHOST, NULL, 0, 0) == 0)
						printf("%s (%s)", hostname, inet_ntoa(((struct sockaddr_in *)&g_serrecv)->sin_addr));
					else
						printf(" %s", inet_ntoa(((struct sockaddr_in *)&g_serrecv)->sin_addr));
				}
				memcpy(&serlast, &g_serrecv, sizeof(struct sockaddr_in));
				tv_sub(&recvtv , &data->recv_time);
				rtt = recvtv.tv_sec * 1000.0 + recvtv.tv_usec / 1000.0;
				printf(" %.3f ms", rtt);
			}
			if (code == ICMP_UNREACH_PORT)
				break ;
			else if (code != ICMP_TIMXCEED_INTRANS && code != SOCK_TIMEOUT)
				printf (" (ICMP %d)", code);
		}
		fflush(stdout);
        printf("\n");
	}
}

int		main(int ac, char **av)
{
	readopt(ac, av);
	init();
	creat_sock();
	readloop();
	clean_up();
	return (0);
}