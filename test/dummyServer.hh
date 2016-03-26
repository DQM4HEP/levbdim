#ifndef _dummyServer_h
#define _dummyServer_h

#include "fsm.hh"
#include "datasource.hh"

class dummyServer {
public:
  dummyServer(std::string name);
  void configure(levbdim::fsmmessage* m);
  void start(levbdim::fsmmessage* m);
  void stop(levbdim::fsmmessage* m);
  void halt(levbdim::fsmmessage* m);
  void readdata(levbdim::datasource *ds);

private:
  levbdim::fsm* _fsm;
  std::vector<levbdim::datasource*> _sources;
  bool _running,_readout;
  boost::thread_group _gthr;
};

#endif
