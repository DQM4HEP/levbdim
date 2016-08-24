#include "exBuilder.hh"
#include <stdlib.h>

int main()
{
    uint32_t port=46000;
    char* wp=getenv("WEBPORT");
    if (wp!=NULL)
    {
        port= atoi(wp);	
    }
    
    exBuilder s("unessai",port);	
    
    while (1)
    {
        
        ::sleep(1);
    }
}