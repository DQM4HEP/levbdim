#ifndef _levbdim_fsm_h
#define _levbdim_fsm_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <json/json.h>
typedef boost::function<void (levbdim::fsmmessage*)> PFunctor;
namespace levbdim {
  
  class fsmTransition
  {
    public:
      fsmTransition(std::string istate,std::string fstate,PFunctor f) : _istate(istate),_fstate(fstate),_callback(f) {}
      std::string initialState(){return _istate;}
      std::string finalState(){return _fstate;}
      PFunctor callback(){return _callback;}
  }

  class rpcFsmMessage : DimRpc
  {
    public:
    RpcFsmMessage(levbdim::fsm* serv,std::string name);
    void rpcHandler(); 
 private:
    levbdim::fsm* _server;

  }
  class fsm
  {
  public:

    fsm();
    std::string processCommand(levbdim::fsmmessage* msg);
    void addState(std::string statename);
    void addTransition(std::string cmd,std::string istate,std::string fstate,PFunctor f);
      
    std::string value();
    std::string command();
    Json::Value content();
  private:
     std::vector<std::string> _states;
     std::string _state;
     
    levbdim::rpcFsmMessage* _rpc;
  };
};
#endif
