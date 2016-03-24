#ifndef _levbdim_basicwriter_h
#define _levbdim_basicwriter_h

#include <stdint.h>
#include <stdlib.h>
#include "buffer.hh"
#include "shmdriver.hh"
#include <vector>
#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>
namespace levbdim {

  class basicwriter : public shmprocessor
  {
  public:
    basicwriter(std::string dire="/tmp");
    void start(uint32_t run);
    void stop();
    virtual  void processEvent(uint32_t key,std::vector<levbdim::buffer*> dss);
    uint32_t totalSize();
    uint32_t eventNumber();
  private:
    std::string _directory;
    uint32_t _run,_event,_totalSize;
    int32_t _fdOut;
    bool _started;
  };
};
#endif
