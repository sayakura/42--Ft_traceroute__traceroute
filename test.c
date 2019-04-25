#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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


int         main(void)
{
    char                *str;

    str = reverse_dns_lookup("72.52.92.246");
    printf("%s\n", str);
    return (0);
}