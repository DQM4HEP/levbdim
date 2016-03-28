#include "fsmmessage.hh"
using namespace levbdim;
fsmmessage::fsmmessage(std::string s) : _sroot(s)
{
	_jsroot.clear();
	Json::Reader reader;
    bool parsingSuccessful = reader.parse(s, _jsroot);
}
std::string fsmmessage::command() 
{
	return _jsroot["command"].asString();
}
std::string& fsmmessage::value() 
{
	return _sroot;
}
Json::Value fsmmessage::content()
{
	return _jsroot["content"];
}
void fsmmessage::setValue(std::string s)
{
  _sroot=s;
  _jsroot.clear();
	Json::Reader reader;
    bool parsingSuccessful = reader.parse(s, _jsroot);
}
void fsmmessage::setAnswer(Json::Value rep)
{
  Json::Value msg;
  msg["command"]=this->command();
  msg["content"]=this->content();
  msg["content"]["answer"]=rep;
  Json::FastWriter fastWriter;
  this->setValue(fastWriter.write(msg));
}
