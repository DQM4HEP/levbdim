#ifndef _levbdim_fsmmessage_h
#define _levbdim_fsmmessage_h

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <json/json.h>
namespace levbdim {

  class fsmmessage
  {
  public:

    fsmmessage(std::string s);
      
    std::string& value();
    std::string command();
    Json::Value content();
    void setValue(std::string s);
  private:
    std::string _sroot;
    Json::Value _jsroot;
  };
};
#endif
