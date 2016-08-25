
#ifndef EXSERVER_HH
# define EXSERVER_HH

# include "datasource.hh"
# include "fsmweb.hh"

class exServer {
public:
  exServer(std::string name,uint32_t port);
  void configure(levbdim::fsmmessage* m);
  void start(levbdim::fsmmessage* m);
  void stop(levbdim::fsmmessage* m);
  void halt(levbdim::fsmmessage* m);
  void readdata(levbdim::datasource *ds);
  void download(Mongoose::Request &request, Mongoose::JsonResponse &response);
  void list(Mongoose::Request &request, Mongoose::JsonResponse &response);
  void incrementEvent() {_event++;_bx++;}
  bool running(){return _running;}
  inline void setDetectorId(uint32_t id) {_detid=id;}
  inline uint32_t getDetectorId() {return _detid;}
private:
  fsmweb* _fsm;
  uint32_t _detid;
  std::vector<levbdim::datasource*> _sources;
  std::map<uint32_t,uint32_t> _stat;
  bool _running,_readout;
  boost::thread_group _gthr;
  uint32_t _event;
  uint32_t _bx;
 };

#endif

