#include "exBuilder.hh"
#include <stdlib.h>
#include <sstream>
#include <unistd.h>
#include <stdint.h>
int main (int argc,char *argv[])
{
    uint32_t port=45000;
    uint32_t detid;
    char tmp;
    /*if the program is ran witout options ,it will show the usgage and exit*/
    if(argc == 1)
    {
        std::cout<<"Please specify the detector with -d num"<<std::endl;
        exit(1);
    }
    /*use function getopt to get the arguments with option."hu:p:s:v" indicate 
     * that option h,v are the options without arguments while u,p,s are the
     * options with arguments*/
    while((tmp=getopt(argc,argv,"hd:p:"))!=-1)
    {
        switch(tmp)
        {
            /*option h show the help infomation*/
            case 'h':
                std::cout<<"you must specify the detector with -d num"<<std::endl;
                std::cout<<"you can specify the port with -p port or use WEBPORT variable"<<std::endl;
                break;
                /*option u present the username*/
                case 'd':
                    detid=atoi(optarg);
                    break;
                case 'p':
                    port=atoi(optarg);
                    break;
                default:
                    break;
        }
    }
    

  std::stringstream sname;

    char* wp=getenv("WEBPORT");
    if (wp!=NULL)
    {
        port= atoi(wp);	
    }
    
	char hname[80];
	gethostname(hname,80);
	sname<<"EXB-"<<hname;
    exBuilder s(sname.str(),port);	
    
    while (1)
    {
        
        ::sleep(1);
    }
}
