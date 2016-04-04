

#include "shmdriver.hh"
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/dir.h>  
#include <sys/param.h>  
#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include "datasource.hh"
#include "datasocket.hh"
extern  int alphasort(); //Inbuilt sorting function  
#define FALSE 0  
#define TRUE !FALSE      
int file_select_2(const struct direct *entry)  
{  
  if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))  
    return (FALSE);  
  else  
    return (TRUE);  
}  
using namespace levbdim;
shmdriver::shmdriver(std::string memdir, bool useevent) : _memdir(memdir),_useEventId(useevent),_running(false)
{
  _eventMap.clear();
  _processors.clear();
  _datasources.clear();
}
shmdriver::~shmdriver()
{
  this->clear();
}
void shmdriver::clear()
{
  for (std::map<uint64_t,std::vector<levbdim::buffer*> >::iterator it=_eventMap.begin();it!=_eventMap.end();it++)
    {
      for (std::vector<levbdim::buffer*>::iterator jt=it->second.begin();jt!=it->second.end();jt++) delete (*jt);
    }
  _eventMap.clear();
  _processors.clear();
  _datasources.clear();
}
void shmdriver::cleanShm()
{
  shmdriver::purgeShm(_memdir);
}
void shmdriver::purgeShm(std::string memory_dir)
{
  int count,i;  
  struct direct **files;
  std::stringstream sc;
  sc.str(std::string());
  sc<<memory_dir<<"/closed/";
  
  count = scandir(sc.str().c_str(), &files, file_select_2, alphasort);  
	
  /* If no files found, make a non-selectable menu item */  
  for (i=1; i<count+1; ++i)
    {
      //uint32_t dtc,gtc,dif;
      //unsigned long long abcid;
      //sscanf(files[i-1]->d_name,"%lld_%d_%d_%d",&abcid,&dtc,&gtc,&dif);
      //printf("dif %d DTC %d GTC %d \n",dif,dtc,gtc);
		
      char fname[256];
      memset(fname,0,256);
      sprintf(fname,"%s/%s",memory_dir.c_str(),files[i-1]->d_name);
      unlink(fname);
      
      memset(fname,0,256);
      sprintf(fname,"%s/%s",sc.str().c_str(),files[i-1]->d_name);
      unlink(fname);
      free(files[i-1]);
    }
	
}

void shmdriver::createDirectories()
{
  std::stringstream sc;
  sc.str(std::string());
  sc<<_memdir<<"/closed";
  int status = mkdir(sc.str().c_str(), S_IRWXU | S_IRWXG | S_IRWXO );
  std::cout<<sc.str().c_str()<<" is created "<<status<<std::endl;
}

void shmdriver::registerProcessor(levbdim::shmprocessor* p)
{
  _processors.push_back(p);
}
void shmdriver::unregisterProcessor(levbdim::shmprocessor* p)
{
  std::vector<levbdim::shmprocessor*>::iterator it=std::find(_processors.begin(),_processors.end(),p);
  if (it!=_processors.end())
    _processors.erase(it);
}
void shmdriver::registerDataSource(uint32_t det,uint32_t ds)
{
  _datasources.push_back(dskey(det,ds));
}
void shmdriver::unregisterDataSource(uint32_t det,uint32_t ds)
{
  std::vector<uint32_t>::iterator it=std::find(_datasources.begin(),_datasources.end(),ds);
  if (it!=_datasources.end())
    _datasources.erase(it);
}

uint32_t shmdriver::numberOfDataSource()
{
  return _datasources.size();
}

uint32_t shmdriver::numberOfDataSource(uint32_t k)
{
  std::map<uint64_t,std::vector<levbdim::buffer*> >::iterator it=_eventMap.find(k);
  if (it!=_eventMap.end())
    return it->second.size();
  else
    return 0;
}

void shmdriver::processEvents()
{
  while (_running)
    {
      //if (_eventMap.size())
      //	std::cout<<"Map size "<<_eventMap.size()<<std::endl;
      for ( std::map<uint64_t,std::vector<levbdim::buffer*> >::iterator it=_eventMap.begin();it!=_eventMap.end();it++)
	{
	  if (it->second.size()!=numberOfDataSource()) continue;
	  if (it->first==0) continue; // do not process event 0
	  _evt=it->first;
	  //std::cout<<"full  event find " <<it->first<<std::endl;
	  for (std::vector<levbdim::shmprocessor*>::iterator itp=_processors.begin();itp!=_processors.end();itp++)
	    {
	      (*itp)->processEvent(it->first,it->second);
	    }
  
	}
      // remove completed events
      for (std::map<uint64_t,std::vector<levbdim::buffer*> >::iterator it=_eventMap.begin();it!=_eventMap.end();)
	{
	  
	  if (it->second.size()==numberOfDataSource())
	    {
	      //std::cout<<"Deleting Event "<<it->first<<std::endl; 
	      for (std::vector<levbdim::buffer*>::iterator iv=it->second.begin();iv!=it->second.end();iv++) delete (*iv);
	      it->second.clear();
	      _eventMap.erase(it++);
	    }
	  else
	    it++;
	}
      ::usleep(500);
    }
  
}
    
void shmdriver::start(uint32_t nr)
{
  // Do the start of the the processors
  _run=nr;
  _evt=0;
  for (std::vector<levbdim::shmprocessor*>::iterator itp=_processors.begin();itp!=_processors.end();itp++)
    {
      (*itp)->start(nr);
    }

  _running=true;
  _gThread.create_thread(boost::bind(&levbdim::shmdriver::scanMemory, this));
  _gThread.create_thread(boost::bind(&levbdim::shmdriver::processEvents, this));

}
void shmdriver::scanMemory()
{
  //levbdim::buffer* b=new levbdim::buffer(0x80000);
  std::vector<std::string> vnames;
  while (_running)
    {
      
      levbdim::shmdriver::ls(_memdir,vnames);
      if (vnames.size()==0) {::sleep(1);continue;}
      //continue;
      for ( std::vector<std::string>::iterator it=vnames.begin();it!=vnames.end();it++)
	{
	  levbdim::buffer* b=new levbdim::buffer(0x80000);
	  levbdim::shmdriver::pull((*it),b,_memdir);
	  //continue;
	  uint64_t idx_storage=b->eventId(); // usually abcid

	  std::map<uint64_t,std::vector<levbdim::buffer*> >::iterator it_gtc=_eventMap.find(idx_storage);
	  if (it_gtc!=_eventMap.end())
	    it_gtc->second.push_back(b);
	  else
	    {
	      std::vector<levbdim::buffer*> v;
	      v.clear();
	      v.push_back(b);
          
	      std::pair<uint64_t,std::vector<levbdim::buffer*> > p(idx_storage,v);
	      _eventMap.insert(p);
	      it_gtc=_eventMap.find(idx_storage);
	    }
	  if (it_gtc->second.size()==this->numberOfDataSource() && it_gtc->first%100==0)
	    printf("GTC %lu %lu  %d\n",it_gtc->first,it_gtc->second.size(),this->numberOfDataSource());
	}
      usleep(500);	

    }
}
void shmdriver::stop()
{
  _running=false;
  _gThread.join_all();

  // Do the stop of the the processors
  for (std::vector<levbdim::shmprocessor*>::iterator itp=_processors.begin();itp!=_processors.end();itp++)
    {
      (*itp)->stop();
    }

}
    
std::string shmdriver::name(uint32_t detid,uint32_t sourceid,uint32_t eventid,uint64_t bxid)
{
  std::stringstream s;
  s<<"Event_"<<detid<<"_"<<sourceid<<"_"<<eventid<<"_"<<bxid;
  return s.str();
}
uint32_t shmdriver::detId(std::string name)
{
  uint32_t d,s,e;
  uint64_t b;
  sscanf(name.c_str(),"Event_%u_%u_%u_%lu",&d,&s,&e,&b);
  return d;
}
uint32_t shmdriver::sourceId(std::string name)
{
  uint32_t d,s,e;
  uint64_t b;
  sscanf(name.c_str(),"Event_%u_%u_%u_%lu",&d,&s,&e,&b);
  return s;
}
uint32_t shmdriver::eventId(std::string name)
{
  uint32_t d,s,e;
  uint64_t b;
  sscanf(name.c_str(),"Event_%u_%u_%u_%lu",&d,&s,&e,&b);
  return e;
}
uint64_t shmdriver::bxId(std::string name)
{
  uint32_t d,s,e;
  uint64_t b;
  sscanf(name.c_str(),"Event_%u_%u_%u_%lu",&d,&s,&e,&b);
  return b;
}
void shmdriver::ls(std::string sourcedir,std::vector<std::string>& res)
{
 
  res.clear();
  int count,i;  
  struct direct **files;  
  std::stringstream sc;
  sc.str(std::string());
  sc<<sourcedir<<"/closed/";
  
  count = scandir(sc.str().c_str(), &files, file_select_2, alphasort); 		
  /* If no files found, make a non-selectable menu item */  
  if(count <= 0)    {return ;}
       
  std::stringstream sd;		
  //printf("Number of files = %d\n",count);  
  for (i=1; i<count+1; ++i)  
    {
      // file name
      std::string fName;
      fName.assign(files[i-1]->d_name);
      res.push_back(fName);
       
      /* sc.str(std::string());
       sd.str(std::string());
       sc<<sourcedir<<"/closed/"<<files[i-1]->d_name;
       sd<<sourcedir<<"/"<<files[i-1]->d_name;
       ::unlink(sc.str().c_str());
       ::unlink(sd.str().c_str());
      */
       free(files[i-1]);
    }
  
  return;
}

void shmdriver::pull(std::string name,levbdim::buffer* buf,std::string sourcedir)
{
  std::stringstream sc,sd;
  sc.str(std::string());
  sd.str(std::string());
  sc<<sourcedir<<"/closed/"<<name;
  sd<<sourcedir<<"/"<<name;
  int fd=::open(sd.str().c_str(),O_RDONLY);
  if (fd<0) 
    {
      printf("%s  Cannot open file %s : return code %d \n",__PRETTY_FUNCTION__,sd.str().c_str(),fd);
      //LOG4CXX_FATAL(_logShm," Cannot open shm file "<<fname);
      return ;
    }
  int size_buf=::read(fd,buf->ptr(),0x20000);
  buf->setPayloadSize(size_buf-20);
  //printf("%d bytes read %x %d \n",size_buf,cbuf[0],cbuf[1]);
  ::close(fd);
  ::unlink(sc.str().c_str());
  ::unlink(sd.str().c_str());
}
void shmdriver::store(uint32_t detid,uint32_t sourceid,uint32_t eventid,uint64_t bxid,void* ptr,uint32_t size,std::string destdir)
{
  std::stringstream sc,sd;
  sc.str(std::string());
  sc<<destdir<<"/closed/";
  char name[512];
  memset(name,0,512);
  sprintf(name,"%s/Event_%u_%u_%u_%lu",destdir.c_str(),detid,sourceid,eventid,bxid);
  int fd= ::open(name,O_CREAT| O_RDWR | O_NONBLOCK,S_IRWXU);
  if (fd<0)
    {
   
      //LOG4CXX_FATAL(_logShm," Cannot open shm file "<<s.str());
      perror("No way to store to file :");
      //std::cout<<" No way to store to file"<<std::endl;
      return;
    }
  int ier=write(fd,ptr,size);
  if (ier!=size) 
    {
      std::cout<<"pb in write "<<ier<<std::endl;
      return;
    }
  ::close(fd);
  memset(name,0,512);
  sprintf(name,"%s/closed/Event_%u_%u_%u_%lu",destdir.c_str(),detid,sourceid,eventid,bxid);
  fd= ::open(name,O_CREAT| O_RDWR | O_NONBLOCK,S_IRWXU);
  //std::cout<<st.str().c_str()<<" "<<fd<<std::endl;
  //write(fd,b,1);
  ::close(fd);

}
