#include "exServer.hh"
#include <iostream>
#include <sstream>


exServer::exServer(std::string name,uint32_t port) : _running(false),_event(0),_bx(0)
{
  _fsm=new fsmweb(name);
  
  // Register state
  _fsm->addState("CREATED");
  _fsm->addState("CONFIGURED");
  _fsm->addState("RUNNING");
  _fsm->addTransition("CONFIGURE","CREATED","CONFIGURED",boost::bind(&exServer::configure, this,_1));
  _fsm->addTransition("CONFIGURE","CONFIGURED","CONFIGURED",boost::bind(&exServer::configure, this,_1));
  _fsm->addTransition("START","CONFIGURED","RUNNING",boost::bind(&exServer::start, this,_1));
  _fsm->addTransition("STOP","RUNNING","CONFIGURED",boost::bind(&exServer::stop, this,_1));
  _fsm->addTransition("HALT","RUNNING","CREATED",boost::bind(&exServer::halt, this,_1));
  _fsm->addTransition("HALT","CONFIGURED","CREATED",boost::bind(&exServer::halt, this,_1));
  
  _fsm->addCommand("DOWNLOAD",boost::bind(&exServer::download, this,_1,_2));
  _fsm->addCommand("LIST",boost::bind(&exServer::list, this,_1,_2));
  
  //Start server
  std::stringstream s0;
  s0.str(std::string());
  s0<<"exServer-"<<name;
  DimServer::start(s0.str().c_str()); 
  _fsm->start(port);
}

void exServer::configure(levbdim::fsmmessage* m)
{
  std::cout<<"Received "<<m->command()<<std::endl;
  std::cout<<"Received "<<m->value()<<std::endl;
  
  // Delet existing datasources
  for (std::vector<levbdim::datasource*>::iterator it=_sources.begin();it!=_sources.end();it++)
	delete (*it);
  _sources.clear();
  // Clear statistics
  _stat.clear();
  // Add a data source
  // Parse the json message
  // {"command": "CONFIGURE", "content": {"detid": 100, "sourceid": [23, 24, 26]}}
  Json::Value jc=m->content();
  int32_t det=jc["detid"].asInt();
  const Json::Value& books = jc["sourceid"];
  Json::Value array_keys;
  for (Json::ValueConstIterator it = books.begin(); it != books.end(); ++it)
  {
	const Json::Value& book = *it;
	int32_t sid=(*it).asInt();
	// rest as before
	std::cout <<"Creating data source "<<det<<" "<<sid<<std::endl;
	array_keys.append((det<<16)|sid);
	levbdim::datasource* ds= new levbdim::datasource(det,sid,0x20000);
	_sources.push_back(ds);
	_stat.insert(std::pair<uint32_t,uint32_t>((det<<16)|sid,0));
	
  }
  
  // Overwrite msg
  //Prepare complex answer
  m->setAnswer(array_keys);
  
}
/**
 * Thread process per datasource
 */
void exServer::readdata(levbdim::datasource *ds)
{
  uint32_t last_evt=0;
  std::srand(std::time(0));
  while (_running)
  {
	::usleep(10000);
	if (!_running) break;
	if (_event%100==0)
	  std::cout<<"Thread of "<<ds->buffer()->dataSourceId()<<" is running "<<_event<<" "<<_running<<std::endl;
	if (_event == last_evt) continue;
	// Just fun 
	// Create a dummy buffer of fix length depending on source id and random data
	// 
	uint32_t psi=ds->buffer()->dataSourceId();
	// Payload address
	uint32_t* pld=(uint32_t*) ds->payload();
	// Random data with tags at start and end of data payload 
	for (int i=1;i<psi-1;i++) pld[i]= std::rand();
	pld[0]=_event;
	pld[psi-1]=_event;
	// Publish the data source
	ds->publish(_event,_bx,psi*sizeof(uint32_t));
	// Update statistics
	std::map<uint32_t,uint32_t>::iterator its=_stat.find((ds->buffer()->detectorId()<<16)|ds->buffer()->dataSourceId());
	if (its!=_stat.end())
	  its->second=_event;
	
	last_evt=_event;
	
  }
  std::cout<<"Thread of "<<ds->buffer()->dataSourceId()<<" is exiting after "<<last_evt<<"events"<<std::endl;
}
/**
 * Transition from CONFIGURED to RUNNING, starts one thread per data source
 */
void exServer::start(levbdim::fsmmessage* m)
{
  std::cout<<"Received "<<m->command()<<std::endl;
  
  _running=true;
  
  for (std::vector<levbdim::datasource*>::iterator ids=_sources.begin();ids!=_sources.end();ids++)
  {
	_gthr.create_thread(boost::bind(&exServer::readdata, this,(*ids)));
	::usleep(500000);
  }
  
}
/**
 * RUNNING to CONFIGURED, Stop threads 
 */
void exServer::stop(levbdim::fsmmessage* m)
{
  
  
  std::cout<<"Received "<<m->command()<<std::endl;
  
  // Stop running
  _running=false;
  ::sleep(1);
  std::cout<<"joining"<<std::endl;
  _gthr.join_all();
}
/**
 * go back to CREATED, call stop and destroy sources
 */
void exServer::halt(levbdim::fsmmessage* m)
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

/**
 * Standalone command DOWNLOAD, unused but it might be used to download data to 
 * configure the hardware
 */
void exServer::download(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  std::cout<<"download"<<request.getUrl()<<" "<<request.getMethod()<<" "<<request.getData()<<std::endl;
  response["answer"]="download called";
}
/**
 * Standalone command LIST to get the statistics of each data source 
 */
void exServer::list(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
  std::cout<<"list"<<request.getUrl()<<" "<<request.getMethod()<<" "<<request.getData()<<std::endl;
  Json::Value array_keys;
  for (std::map<uint32_t,uint32_t>::iterator it=_stat.begin();it!=_stat.end();it++)
  {
	Json::Value js;
	js["detid"]=(it->first>>16)&0xFFFF;
	js["sourceid"]=it->first&0xFFFF;
	js["event"]=it->second;
	array_keys.append(js);
  }
  response["answer"]=array_keys;
}

