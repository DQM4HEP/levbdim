#include "exServer.hh"
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

    sname<<"EXS-"<<hname;
    exServer s(sname.str(),port);	
    while (1)
    {
        // For fun, Increment event number 10 times per second to trigger data sending
        // This is normally done by the hardware
        if (s.running())
		  s.incrementEvent();
        ::usleep(100000);
    }
}