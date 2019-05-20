#include "traceroute.h"
#include "libft.h"

void	tv_sub(struct timeval *out, struct timeval *in)
{
	if ((out->tv_usec -= in->tv_usec) < 0)
	{
		--out->tv_sec;
		out->tv_usec += 1000000;
	}
	out->tv_sec -= in->tv_sec;
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

void     print_usage(void)
{
    printf("Usage:\ntraceroute [ -f first_ttl ] [ -m max_ttl ] [ -S %% of lost packets ] [ -w wait time ] [ -q nprobes ] host\n");
    printf("\t-f started from this hop, default 1\n");
    printf("\t-m set the max hop limits, default 30, up to 255\n");
    printf("\t-S print out a message indicating how many packets were lost\n");
    printf("\t-w set the wait time between probes, default 3 seconds\n");
    printf("\t-q set how many probes per hop, default 3\n");
}