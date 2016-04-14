#ifndef _levbdim_datasocket_h
#define _levbdim_datasocket_h

#include <stdint.h>
#include <stdlib.h>
#include <dic.hxx>
#include "buffer.hh"
#include <string>
namespace levbdim {

  class datasocket : public DimClient
  {
  public:

    datasocket(uint32_t detid,uint32_t sourceid,uint32_t max_size) ;
    datasocket(std::string name,uint32_t max_size);
    ~datasocket();
    void save2disk(std::string memdir);
    void infoHandler();
    void* payload();
	
    bool isAlive();
    levbdim::buffer* buffer();
  private:
    uint32_t _count;
    DimInfo* _info;
    levbdim::buffer* _buffer;
    uint32_t _sourceid,_detid;
    std::string _name,_memdir;
  };
}
#endif
