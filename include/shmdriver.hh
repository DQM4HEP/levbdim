#ifndef _levbdim_shmdriver_h
#define _levbdim_shmdriver_h

#include <stdint.h>
#include <stdlib.h>
#include "buffer.hh"
#include <vector>
#include <map>
#include <string>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#define dskey(d,s) ( (s&0xFFF) | ((d &0xFFF)<<12))
#define source_id(k) (k&0xFFF)
#define detector_id(k) ((k>>12)&0xFFF)
namespace levbdim {
  class shmprocessor
  {
  public:
    virtual void start(uint32_t run)=0;
    virtual void stop()=0;
    virtual  void processEvent(uint32_t key,std::vector<levbdim::buffer*> dss)=0;
  };

  class shmdriver 
  {
  public:

    shmdriver(std::string memdir, bool useevent=true);
    ~shmdriver();
    void cleanShm();
    void clear();
    void createDirectories();
    void registerProcessor(std::string name);
    void registerProcessor(levbdim::shmprocessor* p);
    void unregisterProcessor(levbdim::shmprocessor* p);
    void registerDataSource(uint32_t det,uint32_t ds);
    void unregisterDataSource(uint32_t det,uint32_t ds);
    uint32_t numberOfDataSource();
    uint32_t numberOfDataSource(uint32_t k);
    
    
    void start(uint32_t nr);
    void scanMemory();
    void processEvents();
    void processEvent(uint32_t id);
    void stop();
    
    static void purgeShm(std::string memdir);
    static std::string name(uint32_t detid,uint32_t sourceid,uint32_t eventid,uint64_t bxid);
    static uint32_t detId(std::string name);
    static uint32_t sourceId(std::string name);
    static uint32_t eventId(std::string name);
    static uint64_t bxId(std::string name);
    static void ls(std::string sourcedir,std::vector<std::string> &res);
    static void pull(std::string name,levbdim::buffer* buf,std::string sourcedir);
    static void store(uint32_t detid,uint32_t sourceid,uint32_t eventid,uint64_t bxid,void* ptr,uint32_t size,std::string destdir);

    inline uint32_t run(){return _run;}
    inline uint32_t event(){return _evt;}
  private:
    std::string _memdir;
    bool _useEventId;
    std::vector<uint32_t> _datasources;
    std::vector<shmprocessor* > _processors;
    std::map<uint64_t,std::vector<levbdim::buffer*> > _eventMap;
	
    boost::thread_group _gThread;
    bool _running;
    uint32_t _run,_evt;
  };
};
#endif
