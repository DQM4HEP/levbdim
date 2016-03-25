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
std::string fsmmessage::value() 
{
	return _jsroot.asString();
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
