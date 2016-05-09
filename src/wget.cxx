using namespace std;
#include <curl/curl.h>
//#include <curl/types.h>
#include <curl/easy.h>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <json/json.h>

struct CurlBufferStruct
{
  char* buffer;
  size_t size;
};
 static size_t CurlCallback(void* ptr, size_t size, size_t nmemb, void* data)
{
  size_t realsize = size * nmemb;
  struct CurlBufferStruct* mem = (struct CurlBufferStruct*) data;
  mem->buffer = (char*)realloc(mem->buffer, mem->size + realsize + 1);
 
  if ( mem->buffer )
  {
    memcpy(&(mem->buffer[mem->size]), ptr, realsize );
    mem->size += realsize;
    mem->buffer[ mem->size ] = 0;
  }
 
  return realsize;
}



 
//Lecture de la page web
  char* CurlQuery(char* AddURL,char* Chaine)
  {
  curl_global_init(CURL_GLOBAL_ALL);
  
  CURL *myHandle;
  CURLcode result;
  struct CurlBufferStruct LectureLC;
  LectureLC.buffer = NULL;
  LectureLC.size = 0;
 
  myHandle = curl_easy_init();
  curl_easy_setopt(myHandle, CURLOPT_WRITEFUNCTION, CurlCallback);
  curl_easy_setopt(myHandle, CURLOPT_WRITEDATA, (void*)&LectureLC);
  curl_easy_setopt(myHandle, CURLOPT_URL, AddURL);
  result = curl_easy_perform(myHandle);  //voir la doc pour une gestion minimal des erreurs
  curl_easy_cleanup(myHandle);
 
  if(result!=0) LectureLC.size=1;

  strcpy(Chaine, LectureLC.buffer);  strcat(Chaine,"\0");
  if(LectureLC.buffer) free(LectureLC.buffer);
 
  return Chaine;
  }


string url_encode(const string &value) {
    ostringstream escaped;
    escaped.fill('0');
    escaped << hex;

    for (string::const_iterator i = value.begin(), n = value.end(); i != n; ++i) {
        string::value_type c = (*i);

        // Keep alphanumeric and other accepted characters intact
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
            continue;
        }

        // Any other characters are percent-encoded
        escaped << uppercase;
        escaped << '%' << std::setw(2) << int((unsigned char) c);
        escaped << nouppercase;
    }

    return escaped.str();
}

std::string wget(std::string url)
{
  char RC[50000];  //À changer selon vos besoin
  memset(RC,0,50000);
  CurlQuery((char*) url.c_str(),RC);
  
 
}
#ifdef WGET_MAIN
int main()
{

  std::string sfile=wget("http://lyosdhcal9/daq/levbdim_m3.json");
  std::string m_hostname;
  char hostn[80];
  gethostname(hostn,80);
  m_hostname.assign(hostn);
  //std::cout<<sfile<<std::endl;
    Json::Reader reader;
    Json::Value m_jfile;
    bool parsingSuccessful = reader.parse(sfile, m_jfile);
    //std::cout<<m_jfile;
    if (parsingSuccessful)
    {
      Json::Value m_jconf=m_jfile["HOSTS"][m_hostname];
      Json::StyledWriter styledWriter;
      std::cout << styledWriter.write(m_jconf) << std::endl;
    }
}
#endif
