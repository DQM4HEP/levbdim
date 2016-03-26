#include "dummyServer.hh"

dummyServer::dummyServer(std::string name) : _running(false)
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
  _fsm->addTransition("HALT","RUNNING","CREATED",boost::bind(&dummyServer::halt, this,_1));
  _fsm->addTransition("HALT","CONFIGURED","CREATED",boost::bind(&dummyServer::halt, this,_1));

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
      std::cout <<"Creating datatsource "<<det<<" "<<sid<<std::endl;
      levbdim::datasource* ds= new levbdim::datasource(det,sid,0x20000);
      _sources.push_back(ds);

    }
}
void dummyServer::readdata(levbdim::datasource *ds)
{
  uint32_t evt=0,bx=0;
  while (_running)
    {
      ::usleep(10000);
      if (!_running) break;
      std::cout<<"Thread of "<<ds->buffer()->dataSourceId()<<" is running "<<evt<<" "<<_running<<std::endl;
      // Just fun , ds is publishing a buffer containing sourceid X int of value sourceid
      uint32_t psi=ds->buffer()->dataSourceId();
      
      uint32_t* pld=(uint32_t*) ds->payload();
      for (int i=0;i<psi;i++) pld[i]=psi;
      pld[0]=evt;
      pld[psi-1]=evt;
    
      ds->publish(evt,bx,psi*sizeof(uint32_t));
    
     evt++;
     bx++;
    }
 
  std::cout<<"Thread of "<<ds->buffer()->dataSourceId()<<" is exiting"<<std::endl;
}
void dummyServer::start(levbdim::fsmmessage* m)
{
    std::cout<<"Received "<<m->command()<<std::endl;
    _running=true;

    for (std::vector<levbdim::datasource*>::iterator ids=_sources.begin();ids!=_sources.end();ids++)
      {
	_gthr.create_thread(boost::bind(&dummyServer::readdata, this,(*ids)));
	::usleep(500000);
      }

}
void dummyServer::stop(levbdim::fsmmessage* m)
{
  
  
    std::cout<<"Received "<<m->command()<<std::endl;
    // Stop running
    _running=false;
    ::sleep(1);
    std::cout<<"joining"<<std::endl;
    _gthr.join_all();
}
void dummyServer::halt(levbdim::fsmmessage* m)
{
  
  
    std::cout<<"Received "<<m->command()<<std::endl;
    if (_running)
      this->stop(m);
    std::cout<<"Destroying"<<std::endl;
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
