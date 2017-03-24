#include <cstdlib>

#ifndef __MDBREADER_HH
#include <iostream>
#include <sstream>
#include <fstream>
#include "mongo/client/dbclient.h"
#include "json/json.h"
using namespace mongo;
using namespace std;

class mdbReader
{
public:
  mdbReader(std::string dbname,std::string host,uint32_t port=27017); 
  void connect();
  std::vector<std::pair<std::string,uint32_t> > listDaqConfig(std::string coll);
   std::vector<std::pair<std::string,uint32_t> > listDaqParam();
  std::string queryDaqConfig(std::string coll,std::string name,uint32_t version);
  std::string queryDaqParam(std::string name,uint32_t version);
  void insertDaqConfig(std::string coll,std::string name,uint32_t version,std::string fname);
  void insertDaqParam(std::string name,uint32_t version,std::string fname);
private:
  std::string _db,_host;
  uint32_t _port;
  mongo::DBClientConnection _cl;

};

#endif
