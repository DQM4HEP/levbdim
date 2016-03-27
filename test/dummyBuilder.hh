#ifndef _dummyBuilder_h
#define _dummyBuilder_h

#include "fsm.hh"
#include "datasocket.hh"
#include "shmdriver.hh"
#include "basicwriter.hh"

class dummyBuilder {
public:
  dummyBuilder(std::string memdir);
  void configure(levbdim::fsmmessage* m);
  void start(levbdim::fsmmessage* m);
  void stop(levbdim::fsmmessage* m);
  void halt(levbdim::fsmmessage* m);
  //void readdata(levbdim::datasource *ds);

private:
  levbdim::fsm* _fsm;
  std::vector<levbdim::datasocket*> _sources;
  levbdim::shmdriver* _evb;
  levbdim::basicwriter* _writer;
  //bool _running,_readout;
  //boost::thread_group _gthr;
};

#endif
