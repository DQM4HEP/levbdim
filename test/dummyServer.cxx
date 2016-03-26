#include "dummyServer.hh"

dummyServer::dummyServer(std::string name)
{
  _fsm=new levbdim::fsm(name);

  // Register state
  _fsm->addState("CREATED");
  _fsm->addState("CONFIGURED");
  _fsm->addState("RUNNING");
  _fsm->addTransition("CONFIGURE","CREATED","CONFIGURED",boost::bind(&dummyServer::configure, this,_1));
    _fsm->addTransition("CONFIGURE","CONFIGURED","CONFIGURED",boost::bind(&dummyServer::configure, this,_1));
  _fsm->addTransition("START","CONFIGURED","RUNNING",boost::bind(&dummyServer::start, this,_1));
  _fsm->addTransition("STOP","RUNNING","CONFIGURED",boost::bind(&dummyServer::stop, this,_1));
  _fsm->addTransition("HALT","RUNNING","CREATED",boost::bind(&dummyServer::stop, this,_1));

  //Start server
  std::stringstream s0;
  s0.str(std::string());
  s0<<"dummyServer-"<<name;
  DimServer::start(s0.str().c_str()); 

}

void dummyServer::configure(levbdim::fsmmessage* m)
{
  std::cout<<"Received "<<m->command()<<std::endl;
  // Delet existing datasources
  for (std::vector<levbdim::datasource*>::iterator it=_sources.begin();it!=_sources.end();it++)
      delete (*it);
  _sources.clear();
  // Add a data source
  // Parse the json message
  // {"command": "CONFIGURE", "content": {"detid": 100, "sourceid": [23, 24, 26]}}
  Json::Value jc=m->content();
  int32_t det=jc["detid"].asInt();
  const Json::Value& books = jc["sourceid"];
  for (Json::ValueConstIterator it = books.begin(); it != books.end(); ++it)
    {
      const Json::Value& book = *it;
      int32_t sid=(*it).asInt();
    // rest as before
      levbdim::datasource* ds= new levbdim::datasource(det,sid,0x20000);
      _sources.push_back(ds);

    }
}
void dummyServer::start(levbdim::fsmmessage* m)
{
    std::cout<<"Received "<<m->command()<<std::endl;

}
void dummyServer::stop(levbdim::fsmmessage* m)
{
  
    std::cout<<"Received "<<m->command()<<std::endl;

    //stop data sources
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
