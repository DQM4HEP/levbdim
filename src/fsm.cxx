#include "fsm.hh"
using namespace levbdim;
rpcFsmMessage::rpcFsmMessage(levbdim::fsm* serv,std::string name) : DimRpc(name.c_str(),"C","C"),_server(serv) {}
void rpcFsmMessage::rpcHandler()
{
  char msgi[1024];
  memset(msgi,0,1024);
  memcpy(msgi,getData(),getSize());
  //printf("message %s \n",msgi);
   std::string smsg;smsg.assign(msgi);
   
   levbdim::fsmmessage msg(smsg);

    std::cout<<msg.value()<<std::endl;
   std::cout<<msg.value().size()<<std::endl;
   
   _server->processCommand(&msg);
  
   
   // memset(msgi,0,1024);
   // memcpy(msgi,msg.value().c_str(),msg.value().size());
   setData((char*) msg.value().c_str(),msg.value().size());
  
} 
fsm::fsm(std::string name) : _state("CREATED")
{
	_states.clear();
  _transitions.clear();
  std::stringstream s0;
  s0.str(std::string());
  s0<<"/FSM/"<<name<<"/CMD";  
	_rpc = new rpcFsmMessage(this,s0.str());
  s0.str(std::string());
  s0<<"/FSM/"<<name<<"/STATE";
   _rpcState=new DimService(s0.str().c_str(),(char*) _state.c_str());
   _rpcState->updateService();
}


void fsm::addState(std::string statename) 
{
  _states.push_back(statename);
}
  
void fsm::addTransition(std::string cmd,std::string istate,std::string fstate,PFunctor f)
{
  if (_transitions.find(cmd)!=_transitions.end()) return;
  levbdim::fsmTransition t(istate,fstate,f);
  std::pair<std::string,levbdim::fsmTransition> p(cmd,t);
  _transitions.insert(p);
}

void fsm::setState(std::string s){ _state=s;}

void fsm::publishState() {_rpcState->updateService((char*) _state.c_str());}

std::string fsm::processCommand(levbdim::fsmmessage* msg)
{
  std::map<std::string,levbdim::fsmTransition>::iterator it=_transitions.find(msg->command());
  if (it==_transitions.end())
    {
      Json::Value jrep;
      jrep["command"]="FAILED";
      std::stringstream s0;
      s0.str(std::string());  
      s0<<msg->command()<<" not found in transitions list ";
      jrep["content"]["msg"]=s0.str();
      Json::FastWriter fastWriter;
      msg->setValue(fastWriter.write(jrep));
      return "ERROR";
    }
  else
  {
    if (it->second.initialState().compare(_state)!=0)
      {
        Json::Value jrep;
      jrep["command"]="FAILED";
      std::stringstream s0;
      s0.str(std::string());  
      s0<<_state<<" is not ok for command "<<msg->command();
      jrep["content"]["msg"]=s0.str();
      Json::FastWriter fastWriter;
      msg->setValue(fastWriter.write(jrep));
      return "ERROR";
      }
      else
      {
	it->second.callback()(msg);
        _state=it->second.finalState();
        this->publishState();
	Json::Value jrep;
	std::stringstream s0;
	s0.str(std::string());  
	s0<<msg->command()<<"_DONE";
	jrep["command"]=s0.str();
	jrep["content"]["msg"]="OK";
	Json::FastWriter fastWriter;
	msg->setValue(fastWriter.write(jrep));
	return _state;
     }
  }
}
