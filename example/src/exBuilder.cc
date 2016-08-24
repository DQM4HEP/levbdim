#include "exBuilder.hh"
#include <stdlib.h>
#include <sstream>
int main()
{
  std::stringstream sname;
    uint32_t port=46000;
    char* wp=getenv("WEBPORT");
    if (wp!=NULL)
    {
        port= atoi(wp);	
    }
    char* hn=getenv("HOSTNAME");
	sname<<"EXB-"<<hn;
    exBuilder s(sname.str(),port);	
    
    while (1)
    {
        
        ::sleep(1);
    }
}