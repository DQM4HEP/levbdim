#include "dummyBuilder.hh"

dummyBuilder::dummyBuilder(std::string name) 
{
  _fsm=new levbdim::fsm(name);

  // Register state
  _fsm->addState("CREATED");
  _fsm->addState("CONFIGURED");
  _fsm->addState("RUNNING");
  _fsm->addTransition("CONFIGURE","CREATED","CONFIGURED",boost::bind(&dummyBuilder::configure, this,_1));
  _fsm->addTransition("START","CONFIGURED","RUNNING",boost::bind(&dummyBuilder::start, this,_1));
  _fsm->addTransition("STOP","RUNNING","CONFIGURED",boost::bind(&dummyBuilder::stop, this,_1));
  _fsm->addTransition("HALT","RUNNING","CREATED",boost::bind(&dummyBuilder::halt, this,_1));
  _fsm->addTransition("HALT","CONFIGURED","CREATED",boost::bind(&dummyBuilder::halt, this,_1));

  //Start server
  std::stringstream s0;
  s0.str(std::string());
  s0<<"dummyBuilder-"<<name;
  DimServer::start(s0.str().c_str()); 

}

void dummyBuilder::configure(levbdim::fsmmessage* m)
{
  std::cout<<"Received "<<m->command()<<std::endl;
  // Delet existing datasources
  for (std::vector<levbdim::datasocket*>::iterator it=_sources.begin();it!=_sources.end();it++)
      delete (*it);
  _sources.clear();

  // Add a data source
  // Parse the json message
  // {"command": "CONFIGURE", "content": {"detid": 100, "sourceid": [23, 24, 26]}}
  // Now create the builder
  _evb= new levbdim::shmdriver("/dev/shm/levbdim");
  _evb->createDirectories();
  _evb->cleanShm();

  _writer= new levbdim::basicwriter("/tmp");
  
  _evb->registerProcessor(_writer);
  
  Json::Value jc=m->content();
  int32_t det=jc["detid"].asInt();
  const Json::Value& books = jc["sourceid"];
  for (Json::ValueConstIterator it = books.begin(); it != books.end(); ++it)
    {
      const Json::Value& book = *it;
      int32_t sid=(*it).asInt();
    // rest as before
      std::cout <<"Creating datatsource "<<det<<" "<<sid<<std::endl;
      levbdim::datasocket* ds= new levbdim::datasocket(det,sid,0x20000);
      ds->save2disk("/dev/shm/levbdim/");
      _sources.push_back(ds);
      _evb->registerDataSource(det,sid);
    }
  
}
void dummyBuilder::start(levbdim::fsmmessage* m)
{
    std::cout<<"Received "<<m->command()<<std::endl;
    Json::Value jc=m->content();
    int32_t run=jc["run"].asInt();
    //_writer->start(run);
    _evb->start(run);
    /*
    _running=true;

    for (std::vector<levbdim::datasource*>::iterator ids=_sources.begin();ids!=_sources.end();ids++)
      {
	_gthr.create_thread(boost::bind(&dummyBuilder::readdata, this,(*ids)));
	::usleep(500000);
      }
    */
}
void dummyBuilder::stop(levbdim::fsmmessage* m)
{
  
  
    std::cout<<"Received "<<m->command()<<std::endl;
    _evb->stop();
    //_writer->stop();
}
void dummyBuilder::halt(levbdim::fsmmessage* m)
{
  
  
    std::cout<<"Received "<<m->command()<<std::endl;
    this->stop(m);
    //stop data sources
    for (std::vector<levbdim::datasocket*>::iterator it=_sources.begin();it!=_sources.end();it++)
      delete (*it);
     _sources.clear();
}

int main()
{
  dummyBuilder s("unessaiB");
  while (1)
    {
      ::sleep(1);
    }
}
