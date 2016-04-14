#ifndef _levbdim_buffer_h
#define _levbdim_buffer_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <iostream>
namespace levbdim {

  class buffer
  {
  public:

    buffer(uint32_t max_size) : _allocate(true)
    {_ptr=new char[max_size];_iptr=(uint32_t*) _ptr;_i64ptr=(uint64_t*) &_ptr[3*sizeof(uint32_t)];_psize=0;}
    buffer(char* ptr,uint32_t offset) : _allocate(false)
    {_ptr=&ptr[offset]; _iptr=(uint32_t*) _ptr;_i64ptr=(uint64_t*) &_ptr[3*sizeof(uint32_t)];_psize=0;}
    
    ~buffer() { if (_allocate)
	{
	  delete _ptr;}
    }
    void setDetectorId(uint32_t id){ _iptr[0]=id;}
    void setDataSourceId(uint32_t id){ _iptr[1]=id;}
    void setEventId(uint32_t id){ _iptr[2]=id;}
    void setBxId(uint64_t id){ _i64ptr[0]=id;}
    void setPayload(void* payload,uint32_t payload_size)
    {
      memcpy(&_ptr[3*sizeof(uint32_t)+sizeof(uint64_t)],payload,payload_size);
      _psize=payload_size;
    }
  
    void setPayloadSize(uint32_t s){ _psize=s;}
    uint32_t detectorId(){return _iptr[0];}
    uint32_t dataSourceId(){return _iptr[1];}
    uint32_t eventId(){return _iptr[2];}
    uint64_t bxId(){return _i64ptr[0];}
    char* ptr(){return _ptr;}
    uint32_t size(){return _psize+3*sizeof(uint32_t)+sizeof(uint64_t);}
    char* payload(){return &_ptr[3*sizeof(uint32_t)+sizeof(uint64_t)];}
    uint32_t payloadSize(){return _psize;}
    void compress()
    {
      unsigned char obuf[0x20000];
      unsigned long ldest=0x20000;
      int rc=::compress(obuf,&ldest, (unsigned char*) payload(),payloadSize());
      //std::cout<<_psize<<" "<<ldest<<std::endl;
      memcpy(payload(),obuf,ldest);
      _psize=ldest;
    }
    void uncompress()
    {
      unsigned char obuf[0x20000];
      unsigned long ldest=0x20000;
      int rc=::uncompress(obuf,&ldest, (unsigned char*) payload(),payloadSize());
      //std::cout<<_psize<<" "<<ldest<<std::endl;
      memcpy(payload(),obuf,ldest);
      _psize=ldest;
    }
  private:
    bool _allocate;
    char* _ptr;
    uint32_t* _iptr;
    uint64_t* _i64ptr;
    uint32_t _psize;
  };
}
#endif
