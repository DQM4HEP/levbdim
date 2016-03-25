#include "dummyServer.hh"

dummyServer::dummyServer(std::string name)
{
  _fsm=new levbdim::fsm(name);

  // Register state
  _fsm->addState("CREATED");
  _fsm->addState("CONFIGURED");
  _fsm->addState("RUNNING");
  _fsm->addTransition("CONFIGURE","CREATED","CONFIGURED",boost::bind(&dummyServer::configure, this,_1));
  _fsm->addTransition("START","CONFIGURED","RUNNING",boost::bind(&dummyServer::start, this,_1));
  _fsm->addTransition("STOP","RUNNING","CREATED",boost::bind(&dummyServer::stop, this,_1));

  //Start server
  std::stringstream s0;
  s0.str(std::string());
  s0<<"dummyServer-"<<name;
  DimServer::start(s0.str().c_str()); 

}

void dummyServer::configure(levbdim::fsmmessage* m)
{
  std::cout<<"Received "<<m->command()<<std::endl;
  _sources.clear();
   levbdim::datasource* ds= new levbdim::datasource(1,100,0x20000);
  _sources.push_back(ds);
}
void dummyServer::start(levbdim::fsmmessage* m)
{
    std::cout<<"Received "<<m->command()<<std::endl;

}
void dummyServer::stop(levbdim::fsmmessage* m)
{
    std::cout<<"Received "<<m->command()<<std::endl;
    for (std::vector<levbdim::datasource*>::iterator it=_sources.begin();it!=_sources.end();it++)
      delete (*it);
    _sources.clear();
}

int main()
{
  dummyServer s("unessai");
  while (1)
    {
      ::sleep(1);
    }
}
