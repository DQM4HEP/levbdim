#include "fsmweb.hh"
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <unistd.h>

fsmweb::fsmweb(std::string name) : levbdim::fsm(name),_running(false),_name(name),_webState(NULL)
{
  _commands.clear();
  _service= new FSMMongo(name,this);
}
void fsmweb::serving(uint32_t port)
{
  Mongoose::Server server(port);
  server.registerController(_service);
  server.setOption("enable_directory_listing", "true");
  server.start();
  
  cout << "Server started, routes:" << endl;
  _service->dumpRoutes();
  
  while (_running) {
    ::sleep(1);
  }
  
  server.stop();
}
void fsmweb::start(uint32_t port)
{
  _running=true;
  std::stringstream s0;
  std::stringstream sw;
  s0.str(std::string());
  sw.str(std::string());
  s0<<"/FSM/"<<_name<<"/WEB";
  char hostname[80];
  gethostname(hostname,80);
  sw<<"http://"<<hostname<<":"<<port<<"/"<<_name;
  _webName=sw.str();
  _webState=new DimService(s0.str().c_str(),(char*) _webName.c_str());
  _webState->updateService();
  g_d.create_thread(boost::bind(&fsmweb::serving,this,port));
}

std::string fsmweb::processCommand(levbdim::fsmmessage* msg)
{
  if (_webState!=NULL && msg->command().compare("?")==0)
    {
      _webState->updateService();
      return this->state();
    }
  else
    return levbdim::fsm::processCommand(msg);
}
void fsmweb::stop()
{
    _running=false;
    g_d.join_all();
}
void fsmweb::addCommand(std::string s,MGRFunctor f)
{
  std::pair<std::string,MGRFunctor> p(s,f);
  _commands.insert(p);
}

void fsmweb::handleRequest(Request &request, JsonResponse &response)
{

  std::cout<<"cmdProcess"<<request.getUrl()<<" "<<request.getMethod()<<" "<<request.getData()<<std::endl;
  // Get the command and content
  Json::Value v; v.clear();
  std::string scommand=request.get("name", "NOTSET");
  if (scommand.compare("NOTSET")==0)
    {
      response["status"]="No command or content given";
      return;
    }
  else
    {
      std::map<std::string,MGRFunctor >::iterator icmd=_commands.find(scommand);
      if (icmd==_commands.end())
	{
	  response["status"]="Unknown Command";
	  return;
	  
	}
      JsonResponse pRep;
      icmd->second(request,pRep);
      response["answer"]=pRep;
      response["status"]="OK";
      return;
    }
}

Json::Value fsmweb::commandsList()
{
  Json::Value jrep;jrep.clear();
  for (std::map<std::string,MGRFunctor >::iterator ic=_commands.begin();ic!=_commands.end();ic++)
    {
      Json::Value jc;
      jc["name"]=ic->first;
      jrep.append(jc);
    }
  return jrep;
}




FSMMongo::FSMMongo(std::string name,fsmweb* f) : _name(name), _fsm(f)
{
  
}

void FSMMongo::fsmProcess(Request &request, JsonResponse &response)
{
   std::cout<<"fsmProcess"<<request.getUrl()<<" "<<request.getMethod()<<" "<<request.getData()<<std::endl;
  // Get the command and content
  Json::Value v; v.clear();
  std::string scommand=request.get("command", "NOTSET");
  std::string scontent=request.get("content", "NOTSET");
  std::cout<<"command=>"<<scommand<<std::endl;
  std::cout<<"content=>"<<scontent<<std::endl;
  if (scommand.compare("NOTSET")==0 || scontent.compare("NOTSET")==0)
    {
      response["status"]="No command or content given";
      return;
    }
  else
    {
      v["command"]=scommand;
      Json::Reader reader;Json::Value vc;
      bool parsing =reader.parse(scontent,vc);
      
      v["content"]=vc;
      Json::FastWriter fastWriter;
      std::cout<<"JSON send "<<v<<std::endl;
      std::cout<<"JSON send "<<fastWriter.write(v)<<std::endl;

      levbdim::fsmmessage m;
      m.setValue(fastWriter.write(v));
      std::cout<<"CALLING FSM "<<m.value();
      std::cout<<"CALLING FSM CONTENT "<<m.content();
      std::string res=_fsm->processCommand(&m);
      std::cout<<"RC FSM "<<res<<"==>"<<m.value();
      parsing =reader.parse(m.value(),response);
      return;
    }
}
void FSMMongo::cmdProcess(Request &request, JsonResponse &response)
{
  _fsm->handleRequest(request,response);
}

void FSMMongo::List(Request &request, JsonResponse &response)
{
  response["PREFIX"]=_name;
  response["FSM"]=_fsm->transitionsList();
  response["CMD"]=_fsm->commandsList();
  response["STATE"]=_fsm->state();
  response["PID"]=getpid();
}

void FSMMongo::setup()
{
  // Example of prefix, putting all the urls into "/api"
  addRouteResponse("GET", "/", FSMMongo,List, JsonResponse);
  std::stringstream os;
  os<<"/"<<_name;
  setPrefix(os.str());
  
  // Command handling
  addRouteResponse("GET", "/", FSMMongo,List, JsonResponse);
  //addRouteResponse("GET", "/", FSMMongo, fsmProcess, JsonResponse);
  addRouteResponse("GET", "/FSM",FSMMongo,fsmProcess, JsonResponse);
  addRouteResponse("GET", "/CMD",FSMMongo,cmdProcess, JsonResponse);
}
#ifdef TEST_MAIN
#include "datasource.hh"

class wummyServer {
public:
  wummyServer(std::string name,uint32_t port);
  void configure(levbdim::fsmmessage* m);
  void start(levbdim::fsmmessage* m);
  void stop(levbdim::fsmmessage* m);
  void halt(levbdim::fsmmessage* m);
  void readdata(levbdim::datasource *ds);
  void download(Mongoose::Request &request, Mongoose::JsonResponse &response);
  void list(Mongoose::Request &request, Mongoose::JsonResponse &response);

private:
  fsmweb* _fsm;
  std::vector<levbdim::datasource*> _sources;
  bool _running,_readout;
  boost::thread_group _gthr;
};



wummyServer::wummyServer(std::string name,uint32_t port) : _running(false)
{
  _fsm=new fsmweb(name);

  // Register state
  _fsm->addState("CREATED");
  _fsm->addState("CONFIGURED");
  _fsm->addState("RUNNING");
  _fsm->addTransition("CONFIGURE","CREATED","CONFIGURED",boost::bind(&wummyServer::configure, this,_1));
    _fsm->addTransition("CONFIGURE","CONFIGURED","CONFIGURED",boost::bind(&wummyServer::configure, this,_1));
  _fsm->addTransition("START","CONFIGURED","RUNNING",boost::bind(&wummyServer::start, this,_1));
  _fsm->addTransition("STOP","RUNNING","CONFIGURED",boost::bind(&wummyServer::stop, this,_1));
  _fsm->addTransition("HALT","RUNNING","CREATED",boost::bind(&wummyServer::halt, this,_1));
  _fsm->addTransition("HALT","CONFIGURED","CREATED",boost::bind(&wummyServer::halt, this,_1));

  _fsm->addCommand("DOWNLOAD",boost::bind(&wummyServer::download, this,_1,_2));
  _fsm->addCommand("LIST",boost::bind(&wummyServer::list, this,_1,_2));

  //Start server
  std::stringstream s0;
  s0.str(std::string());
  s0<<"wummyServer-"<<name;
  DimServer::start(s0.str().c_str()); 
  _fsm->start(port);
}

void wummyServer::configure(levbdim::fsmmessage* m)
{
  std::cout<<"Received "<<m->command()<<std::endl;
  std::cout<<"Received "<<m->value()<<std::endl;

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
  Json::Value array_keys;
  for (Json::ValueConstIterator it = books.begin(); it != books.end(); ++it)
    {
      const Json::Value& book = *it;
      int32_t sid=(*it).asInt();
    // rest as before
      std::cout <<"Creating datatsource "<<det<<" "<<sid<<std::endl;
      array_keys.append((det<<16)|sid);
      levbdim::datasource* ds= new levbdim::datasource(det,sid,0x20000);
      _sources.push_back(ds);

    }

  // Overwrite msg
    //Prepare complex answer
  m->setAnswer(array_keys);
  /*
  Json::Value rep;
  rep["command"]=m->command();
  rep["content"]=m->content();
  rep["content"]["answer"]=array_keys;
  Json::FastWriter fastWriter;
  m->setValue(fastWriter.write(rep));
  */
}
void wummyServer::readdata(levbdim::datasource *ds)
{
  uint32_t evt=0,bx=0;
  std::srand(std::time(0));
  while (_running)
    {
      ::usleep(10000);
      //      ::sleep(1);
      if (!_running) break;
      if (evt%100==0)
	std::cout<<"Thread of "<<ds->buffer()->dataSourceId()<<" is running "<<evt<<" "<<_running<<std::endl;
      // Just fun , ds is publishing a buffer containing sourceid X int of value sourceid
      uint32_t psi=ds->buffer()->dataSourceId();
      
      uint32_t* pld=(uint32_t*) ds->payload();
      for (int i=0;i<psi;i++) pld[i]= std::rand();
      pld[0]=evt;
      pld[psi-1]=evt;
    
      ds->publish(evt,bx,psi*sizeof(uint32_t));
    
     evt++;
     bx++;
    }
 
  std::cout<<"Thread of "<<ds->buffer()->dataSourceId()<<" is exiting"<<std::endl;
}
void wummyServer::start(levbdim::fsmmessage* m)
{
    std::cout<<"Received "<<m->command()<<std::endl;

    _running=true;

    for (std::vector<levbdim::datasource*>::iterator ids=_sources.begin();ids!=_sources.end();ids++)
      {
	_gthr.create_thread(boost::bind(&wummyServer::readdata, this,(*ids)));
	::usleep(500000);
      }

}
void wummyServer::stop(levbdim::fsmmessage* m)
{
  
  
    std::cout<<"Received "<<m->command()<<std::endl;
  
    // Stop running
    _running=false;
    ::sleep(1);
    std::cout<<"joining"<<std::endl;
    _gthr.join_all();
}
void wummyServer::halt(levbdim::fsmmessage* m)
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
void wummyServer::download(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
    std::cout<<"dowmnload"<<request.getUrl()<<" "<<request.getMethod()<<" "<<request.getData()<<std::endl;
    response["answer"]="download called";
}

void wummyServer::list(Mongoose::Request &request, Mongoose::JsonResponse &response)
{
    std::cout<<"list"<<request.getUrl()<<" "<<request.getMethod()<<" "<<request.getData()<<std::endl;
    response["answer"]="list called";
}


int main()
{
  wummyServer s("unessai",45000);
  while (1)
    {
      ::sleep(1);
    }
}



#endif

