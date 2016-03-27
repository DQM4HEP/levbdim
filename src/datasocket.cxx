#include "datasocket.hh"
#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include "shmdriver.hh"
using namespace levbdim;

datasocket::datasocket(uint32_t detid,uint32_t sourceid,uint32_t max_size) : _detid(detid),_sourceid(sourceid),_memdir(""),_count(0)
{ 
  _buffer=new levbdim::buffer(max_size);
  _buffer->setDataSourceId(_sourceid);
  _buffer->setDetectorId(_detid);
		
  std::stringstream s0;
  s0<<"/LEVBDIM/DS-"<<_detid<<"-"<<_sourceid<<"/DATA";
  _name=s0.str();
  std::cout<<s0.str()<<" info created"<<std::endl;
  _info=new DimInfo(s0.str().c_str(),_buffer->ptr(),_buffer->size(),this);
}
datasocket::datasocket(std::string name,uint32_t max_size) : _memdir(""),_count(0)
{ 
  sscanf(name.c_str(),"/LEVBDIM/DS-%d-%d/DATA",&_detid,&_sourceid);
  _buffer=new levbdim::buffer(max_size);
  _buffer->setDataSourceId(_sourceid);
  _buffer->setDetectorId(_detid);
		
  std::stringstream s0;
  s0<<"/LEVBDIM/DS-"<<_detid<<"-"<<_sourceid<<"/DATA";
  _info=new DimInfo(s0.str().c_str(),_buffer->ptr(),_buffer->size(),this);
}
datasocket::~datasocket()
{
  delete _buffer;
  delete _info;
}
void datasocket::save2disk(std::string memdir) {_memdir=memdir;}
void datasocket::infoHandler()
{

  _count++;
  DimInfo *curr = getInfo();
  //std::cout<<"data received "<<curr->getName()<<" "<<curr->getSize()<<std::endl;
  memcpy(_buffer->ptr(),curr->getData(),curr->getSize());
  _buffer->setPayloadSize(curr->getSize()-3*sizeof(uint32_t)-sizeof(uint64_t));
  if (_memdir.size()>4)
    levbdim::shmdriver::store(_buffer->detectorId(),_buffer->dataSourceId(),
			      _buffer->eventId(),_buffer->bxId(),_buffer->ptr(),_buffer->size(),_memdir);
}
void* datasocket::payload(){return _buffer->payload();}
	
bool datasocket::isAlive() {return _count>1;}
levbdim::buffer* datasocket::buffer(){return _buffer;}
