#include "exServer.hh"
#include <stdlib.h>

int main()
{
  char* wp=getenv("WEBPORT");
  if (wp!=NULL)
  {
	std::cout<<"Service "<<name<<" started on port "<<atoi(wp)<<std::endl;
	_fsm->start(atoi(wp));
	exServer s("unessai",atoi(wp));	
  }
  else
	exServer s("unessai",45000);	
  
  while (1)
  {
	// For fun, Increment event number 10 times per second to trigger data sending
	// This is normally done by the hardware
	
	s.incrementEvent();
	::usleep(100000);
  }
}