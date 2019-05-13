#include <stdio.h>
#include "traceroute.h"

uint8_t			g_flags = 0;
void			readopt(int ac, char **av);

int main(int ac, char **av)
{
	readopt(ac, av);
	if (g_flags & F_FLAGS)
		printf("f!\n");
	if (g_flags & M_FLAGS)
		printf("m!\n");
	if (g_flags & P_FLAGS)
		printf("p!\n");
	if (g_flags & S_FLAGS)
		printf("S!\n");
	if (g_flags & W_FLAGS)
		printf("w!\n");
	printf("%d\n", g_flags);
	return (0);
}
