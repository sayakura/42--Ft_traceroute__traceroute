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

struct addrinfo		*host_to_addrinfo(char *host, int family, int socktype)
{
	struct addrinfo hints;
	struct addrinfo *res;

	bzero_(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = family;
	hints.ai_socktype = socktype;
	return (getaddrinfo(host, NULL, &hints, &res) == -1) 
        ? NULL
        : res;
}
