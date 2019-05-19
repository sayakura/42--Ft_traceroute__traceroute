#include "traceroute.h"
#include "libft.h"

bool            g_loss;
uint8_t         g_flags;
char*	        g_hostname;
uint8_t         g_inital_ttl;
uint8_t         g_max_ttl;

void			readopt(int ac, char **av)
{
    static char*    avaliable_flags[] = {
        "-f", "-m", "-q", "-S", "-w"
    };
    uint8_t         num_of_flags;
    uint32_t        val;
    uint32_t        host_poi;

    if (ac < 2)
    {
        printf("usage!\n");
        exit(EXIT_SUCCESS);
    }
    val = 0;
    host_poi = ac - 1;
    num_of_flags = NUM_OF(avaliable_flags);
    for (int i = 1; i < ac; i++)
    {
        for (uint8_t poi = 0; poi < num_of_flags; poi++)
        {
            if (strcmp_(avaliable_flags[poi], av[i]) == 0)
            {
                if (!GET_BIT(g_flags, poi))
                {
                    g_flags = SET_BIT(g_flags, poi);
                    val ^= val;
                    switch(poi)
                    {
                        case 0:
                        case 1:
                        case 2:
                        case 4:
                            if (i < ac - 1)
                                val = atoi(av[i + 1]);
                            host_poi = ++i + 1;
                            if (poi == 0)
                            {
                                if (val > g_max_ttl)
                                    FATAL("first hop out of range");
                                g_inital_ttl = val;
                            }
                            else if (poi == 1)
                            {
                                if (val > 255)
                                    FATAL("max hops cannot be more than 255");
                                else if (val < 0)
                                    FATAL("max hops must be a non-negative number");
                                g_max_ttl = val;
                            }
                            else if (poi == 2)
                            {
                                if (val > 10)
                                    FATAL("no more than 10 probes per hop");
                                else if (val == 0)
                                    FATAL("probe must be a non-zero number");
                                g_probes = val;
                            }
                            else if (poi == 4)
                            {
                                if (val <= 0 || val > INT32_MAX)
                                    FATAL("waittime must be between 1 ~ 2147483647");
                                g_waittime = val;
                            }
                            break ;
                        case 3:
                            g_loss = true;
                            break ;
                    }
                    break ;
                }
            }
        }
    }
    if (host_poi >= ac)
        FATAL("Specify \"host\" missing argument.");
    g_hostname = av[ac - 1][0] == '-' ? NULL : av[ac - 1];
    if (!g_hostname)
	{
		printf("usage.\n");
		exit(EXIT_SUCCESS);
	}
	g_addrinfo = host_to_addrinfo(g_hostname, AF_INET, SOCK_DGRAM);
	if (!g_addrinfo)
    {
        fprintf(stderr, "%s: Name or service not known\n", g_hostname);
        exit(EXIT_SUCCESS);
    }
    if (g_inital_ttl > g_max_ttl || g_inital_ttl == 0)
        FATAL("first hop out of range");
}

