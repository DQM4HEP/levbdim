#include "exBuilder.hh"
#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <stdint.h>
int main()
{
  std::stringstream sname;
    uint32_t port=46000;
    char* wp=getenv("WEBPORT");
    if (wp!=NULL)
    {
        port= atoi(wp);	
    }
    
	char hname[80];
	gethostname(hname,80);
	sname<<"EXB-"<<hname;
    exBuilder s(sname.str(),port);	
    
    while (1)
    {
        
        ::sleep(1);
    }
}