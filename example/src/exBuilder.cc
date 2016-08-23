#include "exBuilder.hh"
#include <stdlib.h>

int main()
{
  char* wp=getenv("WEBPORT");
  if (wp!=NULL)
  {
	std::cout<<"Service "<<name<<" started on port "<<atoi(wp)<<std::endl;
	_fsm->start(atoi(wp));
	exBuilder s("unessai",atoi(wp));	
  }
  else
	exBuilder s("unessai",46000);	
  
  while (1)
  {
	
	::sleep(1);
  }
}