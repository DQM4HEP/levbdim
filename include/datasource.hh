#ifndef _levbdim_datasource_h
#define _levbdim_datasource_h

#include <stdint.h>
#include <stdlib.h>
#include <dis.hxx>
#include <string>
#include "buffer.hh"
namespace levbdim {

  class datasource 
  {
  public:

    datasource(uint32_t detid,uint32_t sourceid,uint32_t max_size); 
    ~datasource();
    
    void* payload();
    void publish(uint32_t evt,uint64_t bx,uint32_t payload_size);
 

    levbdim::buffer* buffer();
  private:
    DimService* _service;
    levbdim::buffer* _buffer;
    uint32_t _detid,_sourceid;
    std::string _name;
    int32_t _fdOut;
  };
};
#endif
