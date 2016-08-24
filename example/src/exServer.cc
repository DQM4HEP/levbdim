#include "exServer.hh"
#include <stdlib.h>

int main()
{
    uint32_t port=45000;
    char* wp=getenv("WEBPORT");
    if (wp!=NULL)
    {
        port=atoi(wp);	
    }
    
    exServer s("unessai",port);	
    
    while (1)
    {
        // For fun, Increment event number 10 times per second to trigger data sending
        // This is normally done by the hardware
        
        s.incrementEvent();
        ::usleep(100000);
    }
}