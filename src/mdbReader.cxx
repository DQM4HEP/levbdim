
#include "mdbReader.hh"
  mdbReader::mdbReader(std::string dbname,std::string host,uint32_t port) : _db(dbname),_host(host),_port(port)
  {;}
  void mdbReader::connect()
  {
    mongo::HostAndPort hp(_host,_port);
    
    std::string errmsg;
    _cl.connect(hp,errmsg);
  }
  std::vector<std::pair<std::string,uint32_t> > mdbReader::listDaqConfig(std::string coll)
  {
    std::vector<std::pair<std::string,uint32_t> > list;
    this->connect();
    stringstream s;
    s<<_db<<"."<<coll;

    auto cursor=_cl.query(s.str(),BSONObj());
    while (cursor->more())
      {
	BSONObj b=cursor->next();
	mongo::OID theid=b["_id"].OID();
	//cout<<theid.asTimeT()<<endl;
	
	uint8_t bb[12];
	for (int i=0;i<12;i++) 
	  {
	    int val;
	    sscanf(theid.toString().substr(i*2,2).c_str(),"%x",&val);
	    bb[i]=val&0XFF;
	    
	  }
	uint32_t cnt=bb[11]|(bb[10]<<8)|(bb[9]<<16);
	//cout<<hex<<cnt<<dec<<" "<<cnt<<" "<<theid.toString()<<endl;

	std::string sn=b["name"].toString(false);
	uint32_t ver=b["version"].Int();
	cout<<theid.toString()<<" "<<cnt<<" "<<sn<<" "<<ver<<endl;
	list.push_back(std::pair<std::string,uint32_t>(sn,ver));

      }
    BSONObj p=BSON("_id"<<mongo::OID("58c8fc686dc98a2aa86832b0"));
     auto cursor1=_cl.query(s.str(),p);
    while (cursor1->more())
      {
	BSONObj b=cursor1->next();
	cout<<b["name"].toString()<<endl;
      }
    return list;
  }
   std::vector<std::pair<std::string,uint32_t> > mdbReader::listDaqParam()
  {
    std::vector<std::pair<std::string,uint32_t> > list;
    this->connect();
    stringstream s;
    s<<_db<<".daqparam";

    auto cursor=_cl.query(s.str(),BSONObj());
    while (cursor->more())
      {
	BSONObj b=cursor->next();
	mongo::OID theid=b["_id"].OID();
	//cout<<theid.asTimeT()<<endl;

		
	uint8_t bb[12];
	for (int i=0;i<12;i++) 
	  {
	    int val;
	    sscanf(theid.toString().substr(i*2,2).c_str(),"%x",&val);
	    bb[i]=val&0XFF;
	    
	  }
	uint32_t cnt=bb[11]|(bb[10]<<8)|(bb[9]<<16);
	//cout<<hex<<cnt<<dec<<" "<<cnt<<" "<<theid.toString()<<endl;

	std::string sn=b["name"].toString(false);
	uint32_t ver=b["version"].Int();
	cout<<theid.toString()<<" "<<cnt<<" "<<sn<<" "<<ver<<endl;
	list.push_back(std::pair<std::string,uint32_t>(sn,ver));

      }
    return list;
  }  
  std::string mdbReader::queryDaqConfig(std::string coll,std::string name,uint32_t version)
  {
   this->connect();
    stringstream s;
    s<<_db<<"."<<coll;
    BSONObj p = BSON( "name" << name << "version" << version );
    auto cursor=_cl.query(s.str(),p);
    while (cursor->more())
      {
	BSONObj b=cursor->next();
	std::string sc=b["configuration"].jsonString(mongo::JsonStringFormat::Strict,false,1);
	if (sc.size()<=3) continue;
	return sc;
      }
    return "{}";
  }
  std::string mdbReader::queryDaqParam(std::string name,uint32_t version)
  {
   this->connect();
    stringstream s;
    s<<_db<<".daqparam";
    BSONObj p = BSON( "name" << name << "version" << version );
    auto cursor=_cl.query(s.str(),p);
    while (cursor->more())
      {
	BSONObj b=cursor->next();
	std::string sc=b["configuration"].jsonString(mongo::JsonStringFormat::Strict,false,1);
	if (sc.size()<=3) continue;
	return sc;
      }
    return "{}";
  }
  void mdbReader::insertDaqConfig(std::string coll,std::string name,uint32_t version,std::string fname)
  {
     this->connect();
     BSONObjBuilder bb;
     bb.append("name", name);
     bb.append("version", version);
     ifstream inFile;
     inFile.open(fname);
  // /opt/dhcal/etc/levbdim_small.json");//open the input file

     stringstream strStream;
     strStream << inFile.rdbuf();//read the file
     string str = strStream.str();
     inFile.close();
     bb.append("configuration",mongo::fromjson(str));
     BSONObj p = bb.obj();
     stringstream s;
     s<<_db<<"."<<coll;
     _cl.insert(s.str(), p);
  }
  void mdbReader::insertDaqParam(std::string name,uint32_t version,std::string fname)
  {
     this->connect();
     BSONObjBuilder bb;
     bb.append("name", name);
     bb.append("version", version);
     ifstream inFile;
     inFile.open(fname);
  // /opt/dhcal/etc/levbdim_small.json");//open the input file

     stringstream strStream;
     strStream << inFile.rdbuf();//read the file
     string str = strStream.str();
     inFile.close();
     bb.append("configuration",mongo::fromjson(str));
     BSONObj p = bb.obj();
     stringstream s;
     s<<_db<<".daqparam";
     _cl.insert(s.str(), p);
  }
