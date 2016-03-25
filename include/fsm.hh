#ifndef _levbdim_fsm_h
#define _levbdim_fsm_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <json/json.h>
#include "dis.hxx"
#include "dic.hxx"
#include <sstream>
#include <map>
#include <vector>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include "fsmmessage.hh"
using namespace levbdim;
typedef boost::function<void (levbdim::fsmmessage*)> PFunctor;
namespace levbdim {
  
  class fsmTransition
  {
    public:
      fsmTransition(std::string istate,std::string fstate,PFunctor f) : _istate(istate),_fstate(fstate),_callback(f) {}
      std::string initialState(){return _istate;}
      std::string finalState(){return _fstate;}
      PFunctor callback(){return _callback;}
    private:
      std::string _istate,_fstate;
      PFunctor _callback;
  };
  class fsm;
  class rpcFsmMessage : DimRpc
  {
    public:
    rpcFsmMessage(levbdim::fsm* serv,std::string name);
    void rpcHandler(); 
 private:
    levbdim::fsm* _server;

  };
  class fsm
  {
  public:

    fsm(std::string name);
    std::string processCommand(levbdim::fsmmessage* msg);
    void addState(std::string statename);
    void addTransition(std::string cmd,std::string istate,std::string fstate,PFunctor f);
    void setState(std::string s);
    void publishState();
  private:
    std::vector<std::string> _states;
    std::string _state;
    std::map<std::string,levbdim::fsmTransition> _transitions;
    levbdim::rpcFsmMessage* _rpc;
    DimService* _rpcState;
  };
};
#endif
