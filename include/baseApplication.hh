#ifndef _baseLevbdimApplication_hh
#define _baseLevbdimApplication_hh
#include "fsmweb.hh"
#include <string>
#include <vector>
#include <json/json.h>
namespace levbdim {
    /**
    \class baseApplication
    \brief FrameWork class for any DAQ application
    \details It is designed to work in conjonction with the job control JSON definition. A baseApplication instantiates a fsmweb object with 2 default states VOID and CREATED. At CREATE transition it will parse the JSON configuration file and store json PARAMETER assoicated. It may also call a userCreate virtual function.
      
    \author    Laurent Mirabito
    \version   1.0
    \date      May 2017
    \copyright GNU Public License.
  */
class baseApplication
{
public:
  /**
      \brief  Constructor
       \param name , service name of the fsmweb object
     */
  baseApplication(std::string name);
   /**
      \brief  Create transition, 
      it parses  the message and look for url or file JSON tag. 
      It parses the associated JSON buffer to a  and found parameters associated 
       \param name , service name of the fsmweb object
     */
  void  create(levbdim::fsmmessage* m);
  void c_getconfiguration(Mongoose::Request &request, Mongoose::JsonResponse &response);
  void c_getparameter(Mongoose::Request &request, Mongoose::JsonResponse &response);
  void c_setparameter(Mongoose::Request &request, Mongoose::JsonResponse &response);
  virtual void  userCreate(levbdim::fsmmessage* m);
  Json::Value configuration();
  Json::Value& parameters();
  fsmweb* fsm();
  uint32_t instance(){return _instance;}
  uint32_t port(){return _port;}
protected:
  fsmweb* _fsm;
  std::string _hostname;
  std::string _login;
  std::string _processName;
  Json::Value _jConfig;
  Json::Value _jParam;
  uint32_t _instance,_port;
  
};
};
#endif
