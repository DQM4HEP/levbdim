#include "monitorProcessor.hh"
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
monitorProcessor::monitorProcessor(std::string dire) : _directory(dire),_run(0),_started(false),_event(0) {}
void monitorProcessor::start(uint32_t run)
{
  _run=run; 
  _event=0;
  _started=true;
}
void monitorProcessor::stop()
{
  _started=false;

}

uint32_t monitorProcessor::eventNumber(){return _event;}
uint32_t monitorProcessor::runNumber(){return _run;}
void monitorProcessor::processEvent(uint32_t key,std::vector<levbdim::buffer*> vbuf)
{

  if (!_started) return;
  uint32_t theNumberOfDIF=vbuf.size();
  std::vector<std::string> vnames;
  
  // list files in shm directory
  levbdim::shmdriver::ls(_directory,vnames);
  if (vnames.size()>20*theNumberOfDIF) return;
  for (std::vector<levbdim::buffer*>::iterator iv=vbuf.begin();iv!=vbuf.end();iv++) 
    {
      levbdim::buffer* b=(*iv);
      levbdim::shmdriver::store(b->detectorId(),b->dataSourceId(),b->eventId(),b->bxId(),b->ptr(),b->size(),_directory);
    }
                
                

  _event++;
   

}
