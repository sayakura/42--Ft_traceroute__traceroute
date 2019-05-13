#include "traceroute.h"
#include "libft.h"


uint8_t         g_flags;
char*	        g_hostname;
void			readopt(int ac, char **av)
{
    static char*    avaliable_flags[] = {
        "-f", "-m", "-p", "-S", "-w"
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
                g_flags = SET_BIT(g_flags, poi);
        }
    }
    g_hostname = av[ac - 1][0] == '-' ? NULL : av[ac - 1];
}