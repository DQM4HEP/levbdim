#include "datasource.hh"
#include <iostream>
#include <sstream>
using namespace levbdim;
datasource::datasource(uint32_t detid,uint32_t sourceid,uint32_t max_size) : _detid(detid),_sourceid(sourceid) 
{ 
  _buffer=new levbdim::buffer(max_size);
  _buffer->setDataSourceId(_sourceid);
  _buffer->setDetectorId(_detid);
		
  std::stringstream s0;
  s0<<"/LEVBDIM/DS-"<<_detid<<"-"<<_sourceid<<"/DATA";
  _service=new DimService(s0.str().c_str(),"I:5;C",_buffer->ptr(),_buffer->size());
}
   
datasource::~datasource()
{
  delete _buffer;
  delete _service;
}
void* datasource::payload(){return _buffer->payload();}
void datasource::publish(uint32_t evt,uint64_t bx,uint32_t payload_size)
{
  _buffer->setBxId(bx);
  _buffer->setEventId(evt);
  _buffer->setPayloadSize(payload_size);
  _service->updateService(_buffer->ptr(),_buffer->size());
}

levbdim::buffer* datasource::buffer(){return _buffer;}
