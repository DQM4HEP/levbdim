#include "exBuilder.hh"

exBuilder::exBuilder(std::string name,uint32_t port) 
{
  _fsm=new levbdim::fsmweb(name);

  // Register state
  _fsm->addState("CREATED");
  _fsm->addState("CONFIGURED");
  _fsm->addState("RUNNING");
  _fsm->addTransition("CONFIGURE","CREATED","CONFIGURED",boost::bind(&exBuilder::configure, this,_1));
  _fsm->addTransition("START","CONFIGURED","RUNNING",boost::bind(&exBuilder::start, this,_1));
  _fsm->addTransition("STOP","RUNNING","CONFIGURED",boost::bind(&exBuilder::stop, this,_1));
  _fsm->addTransition("HALT","RUNNING","CREATED",boost::bind(&exBuilder::halt, this,_1));
  _fsm->addTransition("HALT","CONFIGURED","CREATED",boost::bind(&exBuilder::halt, this,_1));

  _fsm->addCommand("LIST",boost::bind(&exBuilder::list, this,_1,_2));
  
  //Start DIM server
  std::stringstream s0;
  s0.str(std::string());
  s0<<"exBuilder-"<<name;
  DimServer::start(s0.str().c_str());
  // Start Web server
  _fsm->start(port);
}

void exBuilder::configure(levbdim::fsmmessage* m)
{
  std::cout<<"Received "<<m->command()<<std::endl;
  // Delet existing datasources
  for (std::vector<levbdim::datasocket*>::iterator it=_sources.begin();it!=_sources.end();it++)
      delete (*it);
  _sources.clear();

  // Add a data source
  // Parse the json message
  // {"command": "CONFIGURE", "content": {"memdir":"/dev/shm/lebdim","datadir":"/tmp",
  // 									  "detid": 100, "sourceid": [23, 24, 26]}}
  // Now create the builder
  Json::Value jc=m->content();
  std::string memdir=jc["memdir"].asString();
  _evb= new levbdim::shmdriver(memdir);
  _evb->createDirectories();
  _evb->cleanShm();

  // register Processor
  std::string datadir=jc["datadir"].asString();
  _writer= new levbdim::basicwriter(datadir);
  
  _evb->registerProcessor(_writer);
  
  // Register data sockets
  int32_t det=jc["detid"].asInt();
  const Json::Value& books = jc["sourceid"];
  for (Json::ValueConstIterator it = books.begin(); it != books.end(); ++it)
    {
      const Json::Value& book = *it;
      int32_t sid=(*it).asInt();
    // rest as before
      std::cout <<"Creating datatsource "<<det<<" "<<sid<<std::endl;
      levbdim::datasocket* ds= new levbdim::datasocket(det,sid,0x20000);
      ds->save2disk(memdir);
      _sources.push_back(ds);
      _evb->registerDataSource(det,sid);
    }
  
}
void exBuilder::start(levbdim::fsmmessage* m)
{
    std::cout<<"Received "<<m->command()<<std::endl;
	// Parse the json message
	// {"command": "START", "content": {"run":123456}}
    Json::Value jc=m->content();
    int32_t run=jc["run"].asInt();
    _evb->start(run);
}
void exBuilder::stop(levbdim::fsmmessage* m)
{
  
  
    std::cout<<"Received "<<m->command()<<std::endl;
    _evb->stop();
}
void exBuilder::halt(levbdim::fsmmessage* m)
{
    std::cout<<"Received "<<m->command()<<std::endl;
    this->stop(m);
    //stop data sources
    for (std::vector<levbdim::datasocket*>::iterator it=_sources.begin();it!=_sources.end();it++)
      delete (*it);
     _sources.clear();
}
void exBuilder::list(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  std::cout<<"list"<<request.getUrl()<<" "<<request.getMethod()<<" "<<request.getData()<<std::endl;
  Json::Value js;
  js["run"]=_evb->run();
  js["event"]=_evb->event();
  response["answer"]=js;
}
