#ifndef _exBuilder_h
#define _exBuilder_h

#include "fsmweb.hh"
#include "datasocket.hh"
#include "shmdriver.hh"
#include "basicwriter.hh"

class exBuilder {
public:
  exBuilder(std::string name,uint32_t port);
  void configure(levbdim::fsmmessage* m);
  void start(levbdim::fsmmessage* m);
  void stop(levbdim::fsmmessage* m);
  void halt(levbdim::fsmmessage* m);
  void list(Mongoose::Request &request, Mongoose::JsonResponse &response);
private:
  fsmweb* _fsm;
  std::vector<levbdim::datasocket*> _sources;
  levbdim::shmdriver* _evb;
  levbdim::basicwriter* _writer;
};

#endif
