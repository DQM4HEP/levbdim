#include "basicwriter.hh"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/dir.h>  
#include <sys/param.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
using namespace levbdim;
basicwriter::basicwriter(std::string dire) : _directory(dire),_run(0),_started(false),_fdOut(-1),_totalSize(0),_event(0) {}
void basicwriter::start(uint32_t run)
{
  _run=run; 
  std::stringstream filename("");    
  char dateStr [64];
            
  time_t tm= time(NULL);
  strftime(dateStr,20,"SMM_%d%m%y_%H%M%S",localtime(&tm));
  filename<<_directory<<"/"<<dateStr<<"_"<<run<<".dat";
  _fdOut= ::open(filename.str().c_str(),O_CREAT| O_RDWR | O_NONBLOCK,S_IRWXU);
  if (_fdOut<0)
    {
      perror("No way to store to file :");
      //std::cout<<" No way to store to file"<<std::endl;
      return;
    }  
  _event=0;
  _started=true;
}
void basicwriter::stop()
{
  _started=false;
  ::sleep(1);
  if (_fdOut>0)
    {
      ::close(_fdOut);
      _fdOut=-1;
    }


}
uint32_t basicwriter::totalSize(){return _totalSize;}
uint32_t basicwriter::eventNumber(){return _event;}
void basicwriter::processEvent(uint32_t key,std::vector<levbdim::buffer*> vbuf)
{

  if (!_started) return;
  uint32_t theNumberOfDIF=vbuf.size();
  if (_event%100==0) 
    std::cout<<"Standard completion "<<_event<<" GTC "<<key<<" size "<<_totalSize<<std::endl;
  // To be implemented
  if (_fdOut>0)
    {
      int ier=write(_fdOut,&_event,sizeof(uint32_t));
      ier=write(_fdOut,&theNumberOfDIF,sizeof(uint32_t));
      for (std::vector<levbdim::buffer*>::iterator iv=vbuf.begin();iv!=vbuf.end();iv++) 
	{
	  //printf("\t writing %d bytes",idata[SHM_BUFFER_SIZE]);
	  uint32_t bsize=(*iv)->size();
	  _totalSize+=bsize;
	  ier=write(_fdOut,&bsize,sizeof(uint32_t));
	  ier=write(_fdOut,(*iv)->ptr(),bsize);
	}
                
                

      _event++;
    }

  if (_totalSize>1900*1024*1024)
    {
      ::close(_fdOut);
      _totalSize=0;
      this->start(_run);
    }


}
