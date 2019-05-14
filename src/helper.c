#include "traceroute.h"
#include "libft.h"

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

// char				*reverse_dns_lookup(char *ip_addr)
// {
// 	struct sockaddr_in	temp_addr;
// 	socklen_t			len;
// 	static char			ret_buf[NI_MAXHOST];

// 	temp_addr.sin_family = AF_INET;
// 	temp_addr.sin_addr.s_addr = inet_addr(ip_addr);
// 	len = sizeof(struct sockaddr_in);
// 	if (getnameinfo((struct sockaddr *)&temp_addr, len, ret_buf,
// 					sizeof(ret_buf), NULL, 0, NI_NAMEREQD))
// 	    return (ret_buf);
//     return (NULL);
// }

struct addrinfo		*host_to_addrinfo(char *host, int family, int socktype)
{
	struct addrinfo hints;
	struct addrinfo *res;

    if (!*host)
        return (NULL);
	bzero_(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;
	return (getaddrinfo(host, NULL, &hints, &res) == -1) 
        ? NULL
        : res;
}
