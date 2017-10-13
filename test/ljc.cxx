#include "fsmjob.hh"
#include <unistd.h>

int main()
{
  char hostn[80];
  gethostname(hostn,80);
  std::stringstream s;
  s<<"LJC-"<<hostn;
  levbdim::fsmjob fs(s.str(),9999);
  while (1)
    {
      ::sleep(1);
    }
}
