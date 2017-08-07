#include "fsm.hh"
using namespace levbdim;

fsmClient::fsmClient(std::string name,int timeout) : DimRpcInfo((char*) name.c_str(),-1)
{_sem.lock();}
void fsmClient::execute(levbdim::fsmmessage* m)
{
  setData((char*) m->value().c_str(),m->value().size());_sem.lock();
}

void fsmClient::rpcInfoHandler()
{
  std::string s;

  s.assign((char*) getData(),getSize());
  //std::cout<<"fsmClient rec "<<getData()<<" et la string "<<s<<std::endl;
  _m.setValue(s);
  _sem.unlock();
}

rpcFsmMessage::rpcFsmMessage(levbdim::fsm* serv,std::string name) : DimRpc(name.c_str(),"C","C"),_server(serv) {}
void rpcFsmMessage::rpcHandler()
{
  char msgi[32768];
  memset(msgi,0,32768);
  memcpy(msgi,getData(),getSize());
  //printf("message %s \n",msgi);
   std::string smsg;smsg.assign(msgi);
   
   levbdim::fsmmessage msg(smsg);

   //std::cout<<"RECEIVED: "<<msg.value()<<std::endl;
   //std::cout<<msg.value().size()<<std::endl;
   
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
  if (_transitions.find(cmd)!=_transitions.end())
    {
      std::map<std::string,std::vector<levbdim::fsmTransition> >::iterator iv=_transitions.find(cmd);
      bool found=false;
      for (std::vector<levbdim::fsmTransition>::iterator it=iv->second.begin();it!=iv->second.end();it++)
	{
	  if (it->initialState().compare(istate)==0)
	    {/*already stor */ return;}
	}
      levbdim::fsmTransition t(istate,fstate,f);;
      iv->second.push_back(t);

    }
  else
    {
      levbdim::fsmTransition t(istate,fstate,f);
      std::vector<levbdim::fsmTransition> vp;
      vp.push_back(t);
      std::pair<std::string,std::vector<levbdim::fsmTransition> > p(cmd,vp);
      _transitions.insert(p);
    }
}

void fsm::setState(std::string s){ _state=s;}
std::string fsm::state(){return _state;}
void fsm::publishState() {_rpcState->updateService((char*) _state.c_str());}

std::string fsm::processCommand(levbdim::fsmmessage* msg)
{
  std::map<std::string,std::vector<levbdim::fsmTransition> >::iterator it=_transitions.find(msg->command());
  if (it==_transitions.end())
    {
      Json::Value jrep;
      jrep["command"]=msg->command();
      jrep["status"]="FAILED";
      std::stringstream s0;
      s0.str(std::string());  
      s0<<msg->command()<<" not found in transitions list ";
      jrep["content"]=msg->content();
      jrep["content"]["answer"]=s0.str();
      Json::FastWriter fastWriter;
      msg->setValue(fastWriter.write(jrep));
      return "ERROR";
    }
  else
  {
    // loop on vector of transition
    std::vector<levbdim::fsmTransition> &vp=it->second;
    for (std::vector<levbdim::fsmTransition>::iterator ift=vp.begin();ift!=vp.end();ift++)
      if (ift->initialState().compare(_state)==0)
	{
	  ift->callback()(msg);
	  _state=ift->finalState();
	  this->publishState();
	  Json::Value jrep;
	  std::stringstream s0;
	  s0.str(std::string());  
	  s0<<msg->command()<<"_DONE";

	  jrep["command"]=msg->command();
	  jrep["status"]="DONE";
	  jrep["content"]=msg->content();
	  //jrep["content"]["msg"]="OK";
	  Json::FastWriter fastWriter;
	  msg->setValue(fastWriter.write(jrep));
	  return _state;

	}
    // No initialState corresponding to _state
    //if (it->second.initialState().compare(_state)!=0)
    //  {
        Json::Value jrep;
	jrep["command"]=msg->command();
      jrep["status"]="FAILED";
      std::stringstream s0;
      s0.str(std::string());  
      s0<<"Current State="<<_state<<" is not an initial state of the command "<<msg->command();
      jrep["content"]=msg->content();
      jrep["content"]["answer"]=s0.str();
      Json::FastWriter fastWriter;
      msg->setValue(fastWriter.write(jrep));
      return "ERROR";
      /*  }
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
      */
  }
}
Json::Value fsm::transitionsList()
{
  Json::Value jrep;jrep.clear();
  for( std::map<std::string,std::vector<levbdim::fsmTransition> >::iterator it=_transitions.begin();it!=_transitions.end();it++)
    {
      Json::Value jc;
      jc["name"]=it->first;
      jrep.append(jc);
    }
  return jrep;
}
Json::Value fsm::allowList()
{
  Json::Value jrep;jrep.clear();
  for( std::map<std::string,std::vector<levbdim::fsmTransition> >::iterator it=_transitions.begin();it!=_transitions.end();it++)
    {
      bool allowed=false;
      std::vector<levbdim::fsmTransition> &vp=it->second;
      for (std::vector<levbdim::fsmTransition>::iterator ift=vp.begin();ift!=vp.end();ift++)
	if (ift->initialState().compare(_state)==0)
	  {allowed=true;break;}
      if (allowed)
	{
	  Json::Value jc;
	  jc["name"]=it->first;
	  jrep.append(jc);
	}
    }
  return jrep;
}
