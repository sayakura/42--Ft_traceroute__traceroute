#include "traceroute.h"
#include "libft.h"


uint8_t         g_flags;
char*	        g_hostname;
uint8_t         g_inital_ttl;
uint8_t         g_max_ttl;
uint16_t        g_dport;

void			readopt(int ac, char **av)
{
    static char*    avaliable_flags[] = {
        "-f", "-m", "-q", "-S", "-w"
    };
    uint8_t         num_of_flags;

    if (ac < 2)
        return ;
    num_of_flags = NUM_OF(avaliable_flags);
    for (int i = 1; i < ac; i++)
    {
        for (uint8_t poi = 0; poi < num_of_flags; poi++)
        {
            if (strcmp_(avaliable_flags[poi], av[i]) == 0)
            {
                g_flags = SET_BIT(g_flags, poi);
                
               
                        
                
            }
            
        }
    }
    g_hostname = av[ac - 1][0] == '-' ? NULL : av[ac - 1];
    if (!g_hostname)
	{
		printf("usage.\n");
		exit(EXIT_SUCCESS);
	}
	g_addrinfo = host_to_addrinfo(g_hostname, AF_INET, SOCK_DGRAM);
	ERR_CHECK(g_addrinfo == NULL, "host_to_addrinfo");
    if (g_inital_ttl > g_max_ttl || g_inital_ttl == 0)
        FATAL("first hop out of range");
}

    // case 1:
                    //     if (i < ac - 1)
                    //         val = atol(av[i + 1]);
                    //     if (val > 255)
                    //         FATAL("max hops cannot be more than 255");
                    //     if (val < 0)
                    //         FATAL("max hops must be a non-negative number");
                    //     g_max_ttl = val;
                    //     break ;
                    // case 2:
                    //     if (i < ac - 1)
                    //         val = atol(av[i + 1]);
                    //     if (val > 10)
                    //         FATAL("no more than 10 probes per hop");
                    //     g_probes = val;
                    //     break ;
                    // case 4:
                    //     if (i < ac - 1)
                    //         val = atol(av[i + 1]);
                    //     if (val < 0 || val > INT32_MAX)
                    //         FATAL("waittime must be between 0 ~ 2147483647");
                    //     g_waittime = val;
                    //     break ;
                //     default: 
                //         break ;
                // }