#include "traceroute.h"

void    sig_alrm(int signo)
{
    (void)signo;
    g_alarmed = true;    
    return ;
}