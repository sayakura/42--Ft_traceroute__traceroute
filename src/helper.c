#include "traceroute.h"
#include "libft.h"

// uint16_t		in_cksum(uint16_t *addr, int len)
// {
// 	uint32_t	sum;
// 	uint16_t	answer;

// 	answer = 0;
// 	sum = 0;
// 	while (len > 1)
// 	{
// 		sum += *addr++;
// 		len -= 2;
// 	}
// 	if (len == 1)
// 	{
// 		*(unsigned char *)(&answer) = *(unsigned char *)addr;
// 		sum += answer;
// 	}
// 	sum = (sum >> 16) + (sum & 0xffff);
// 	sum += (sum >> 16);
// 	answer = ~sum;
// 	return (answer);
// }

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


char	*code_tostring(int code)
{
	static char *code_lookup[] = {
		"Net is unreachable",
		"Host is unreachable",
		"Protocol is unreachable",
		"Port is unreachable",
		"Fragmentation is needed and Don't Fragment was set",
		"Source route failed",
		"Destination network is unknown",
		"Destination host is unknown",
		"Source host is isolated",
		"Communication with destination network is administratively prohibited",
		"Communication with destination host is administratively prohibited",
		"Destination network is unreachable for type of service",
		"Destination host is unreachable for type of service",
		"Communication is administratively prohibited",
		"Host precedence violation",
		"Precedence cutoff is in effect"
	};

	if (code < 0 || code > 15)
		return ("unknown code");
	return (code_lookup[code]);
}