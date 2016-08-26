#!/usr/bin/env python
import os
import socks
import socket
import httplib, urllib,urllib2
import json
from copy import deepcopy

import time
import argparse

def parseReturn(command,sr):
    if (command=="jobStatus"):
        #s=r1.read()
        #print s["answer"]
        sj=json.loads(sr)
        #sj=s
        #ssj=json.loads(sj["answer"]["ANSWER"])
        ssj=sj["answer"]["ANSWER"]
        print "\033[1m %6s %15s %25s %20s \033[0m" % ('PID','NAME','HOST','STATUS')
        for x in ssj:
            if (x['DAQ']=='Y'):
                print "%6d %15s %25s %20s" % (x['PID'],x['NAME'],x['HOST'],x['STATUS'])
    if (command=="hvStatus"):
        sj=json.loads(sr)
        ssj=sj["answer"]["ANSWER"]
        #print ssj
        print "\033[1m %5s %10s %10s %10s %10s \033[0m" % ('Chan','VSET','ISET','VOUT','IOUT')
        for x in ssj:
            print "#%.4d %10.2f %10.2f %10.2f %10.2f" % (x['channel'],x['vset'],x['iset'],x['vout'],x['iout'])
    if (command=="LVStatus"):
        sj=json.loads(sr)
        
        ssj=sj["answer"]["LVSTATUS"]
        print "\033[1m %10s %10s %10s \033[0m" % ('VSET','VOUT','IOUT')
        print " %10.2f %10.2f %10.2f" % (ssj['vset'],ssj['vout'],ssj['iout'])
    if (command=="status" and not results.verbose):

        sj=json.loads(sr)
        ssj=sj["answer"]["diflist"]
        print "\033[1m %4s %5s %6s %12s %12s %15s  %s \033[0m" % ('DIF','SLC','EVENT','BCID','BYTES','SERVER','STATUS')

        for d in ssj:
            #print d
            #for d in x["difs"]:
            print '#%4d %5x %6d %12d %12d %15s %s ' % (d["id"],d["slc"],d["gtc"],d["bcid"],d["bytes"],d["host"],d["state"])
    if (command=="dbStatus" ):
        sj=json.loads(sr)
        ssj=sj["answer"]
        print "\033[1m %10s %10s \033[0m" % ('Run','State')
        print " %10d %s " % (ssj['run'],ssj['state'])
    if (command=="shmStatus" ):
        sj=json.loads(sr)
        ssj=sj["answer"]
        print "\033[1m %10s %10s \033[0m" % ('Run','Event')
        print " %10d %10d " % (ssj['run'],ssj['event'])
    if (command=="state"):
        sj=json.loads(sr)
        print "\033[1m State \033[0m :",sj["STATE"]
        scm=""
        for z in sj["CMD"]:
            scm=scm+"%s:" % z["name"]
        scf=""
        for z in sj["FSM"]:
            scf=scf+"%s:" % z["name"]

        print "\033[1m Commands \033[0m :",scm
        print "\033[1m F S M \033[0m :",scf
    if (command=="triggerStatus"):
          
        sj=json.loads(sr)
        ssj=sj["answer"]["COUNTERS"]
        print "\033[1m %10s %10s %10s %10s %12s %12s %10s %10s %10s \033[0m" % ('Spill','Busy1','Busy2','Busy3','SpillOn','SpillOff','Beam','Mask','EcalMask')
        print " %10d %10d %10d %10d  %12d %12d %12d %10d %10d " % (ssj['spill'],ssj['busy1'],ssj['busy2'],ssj['busy3'],ssj['spillon'],ssj['spilloff'],ssj['beam'],ssj['mask'],ssj['ecalmask'])
    if (command=="difLog" or command=="cccLog" or command=="mdccLog" or command =="zupLog" or command=="jobLog"):
          
        sj=json.loads(sr)
        print  "\033[1m %s \033[0m" % sj["answer"]["FILE"]
        ssj=sj["answer"]["LINES"]
        print ssj
        #print "\033[1m %10s %10s %10s %10s %12s %12s %10s %10s %10s \033[0m" % ('Spill','Busy1','Busy2','Busy3','SpillOn','SpillOff','Beam','Mask','EcalMask')
        #print " %10d %10d %10d %10d  %12d %12d %12d %10d %10d " % (ssj['spill'],ssj['busy1'],ssj['busy2'],ssj['busy3'],ssj['spillon'],ssj['spilloff'],ssj['beam'],ssj['mask'],ssj['ecalmask'])

        
def executeFSM(host,port,prefix,cmd,params):
   if (params!=None):
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       #if (name!=None):
       #    lq['name']=name
       #if (value!=None):
       #    lq['value']=value
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       lq={}
       
       lq["content"]=json.dumps(params,sort_keys=True)
       #for x,y in params.iteritems():
       #    lq["content"][x]=y
       lq["command"]=cmd           
       lqs=urllib.urlencode(lq)
       #print lqs
       saction = '/%s/FSM?%s' % (prefix,lqs)
       myurl=myurl+saction
       #print myurl
       req=urllib2.Request(myurl)
       r1=urllib2.urlopen(req)
       return r1.read()

def executeCMD(host,port,prefix,cmd,params):
   if (params!=None and cmd!=None):
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       #if (name!=None):
       #    lq['name']=name
       #if (value!=None):
       #    lq['value']=value
       myurl = "http://"+host+ ":%d" % (port)
       #conn = httplib.HTTPConnection(myurl)
       lq={}
       lq["name"]=cmd
       for x,y in params.iteritems():
           lq[x]=y
       lqs=urllib.urlencode(lq)
       saction = '/%s/CMD?%s' % (prefix,lqs)
       myurl=myurl+saction
       #print myurl
       req=urllib2.Request(myurl)
       r1=urllib2.urlopen(req)
       return r1.read()
   else:
       myurl = "http://"+host+ ":%d/%s/" % (port,prefix)
       #conn = httplib.HTTPConnection(myurl)
       #print myurl
       req=urllib2.Request(myurl)
       r1=urllib2.urlopen(req)
       return r1.read()
    
class levProcess:
    ' handling '
    def Dump(self):
        print self.host,self.port,self.prefix
        for pname,vpar in sorted(self.params.iteritems()):
            print "params[",pname,"]=",vpar
            
    def sendTransition(self,cmd):
        self.rc=executeFSM(self.host,self.port,self.prefix,cmd,self.params)
        return self.rc
    def sendCommand(self,cmd):
        self.rc=executeCMD(self.host,self.port,self.prefix,cmd,self.params)
        return self.rc
    def setParameter(self,pname,pval):
        """
        Change internal parameter pname to pval value (both are strings)
        """
        self.params[pname]=pval

    def getParameter(self, pname):
        """
        get the stored value of the parameter pname
        """
        return self.params[pname]

    def clear(self):
        self.params.clear()

    def __init__(self,host,port,prefix):
        """
        Constructor
        """
        self.host=host
        self.port=port
        self.prefix=prefix
        self.params={}

class exServer(levProcess):
    def __init__(self,host,port,detid,sources):
        prf="EXS-"+host
        levProcess.__init__(self,host,port,prf)
        self.clear()
        self.setParameter("detid",detid)
        self.setParameter("sourceid",sources)

class exBuilder(levProcess):
    def __init__(self,host,port,mempath,datapath):
        prf="EXB-"+host
        levProcess.__init__(self,host,port,prf)
        self.clear()
        self.setParameter("memdir",mempath)
        self.setParameter("datadir",datapath)

class exSetup:
    def __init__(self,jsonfile):
        self.jfile=jsonfile
        self.json=None
        self.builder=None
        self.servers=[]
    def parse(self):
        self.builder=None
        self.servers=[]
        req=urllib2.Request(self.jfile)
        try:
            r1=urllib2.urlopen(req)
        except URLError, e:
            print "Cannot retrieve file ",self.jfile,e
            return 
        else:
            sfile=r1.read();
            #print sfile
            self.json=json.loads(sfile)
        # EXSERVERS
        x=self.json["HARDWARE"]
        for sv in x["EXSERVER"]:
            exs=exServer(sv["host"],sv["port"],sv["detid"],sv["sources"])
            self.servers.append(exs)
        sb=x["EXBUILDER"]
        self.builder=exBuilder(sb["host"],sb["port"],sb["mempath"],sb["datapath"])
    def initJobs(self):
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            sr=executeFSM(x,9999,"LJC-%s" % x,"INITIALISE",lcgi)
    def startJobs(self):
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            sr=executeFSM(x,9999,"LJC-%s" % x,"START",lcgi) 
    def killJobs(self):
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            sr=executeFSM(x,9999,"LJC-%s" % x,"KILL",lcgi) 
    def statusJobs(self):
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            sr=json.loads(executeCMD(x,9999,"LJC-%s" % x,"STATUS",lcgi))
            print x
            print sr

    def restartJob(self,host,name):
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            if (x!=host):
                continue;
            sr=json.loads(executeCMD(x,9999,"LJC-%s" % x,"STATUS",lcgi))
            for p in sr['answer']['JOBS']:
                if (p['NAME']==name):
                    pid=p['PID']
                    lcgi['pid']=pid
                    lcgi['processname']=name
                    sr=executeCMD(x,9999,"LJC-%s" % x,"RESTARTJOB",lcgi)
                    print sr
                    return
    def killJob(self,host,name):
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            if (x!=host):
                continue;
            sr=json.loads(executeCMD(x,9999,"LJC-%s" % x,"STATUS",lcgi))
            for p in sr['answer']['JOBS']:
                if (p['NAME']==name):
                    pid=p['PID']
                    lcgi['pid']=pid
                    lcgi['processname']=name
                    sr=executeCMD(x,9999,"LJC-%s" % x,"KILLJOB",lcgi)
                    print sr
                    return
    def jobLog(self,host,name,lines=100):
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            if (x!=host):
                continue;
            srs=executeCMD(x,9999,"LJC-%s" % x,"STATUS",lcgi)
            sr=json.loads(srs)
            #print sr
            for p in sr['answer']['JOBS']:
                if (p['NAME']==name):
                    pid=p['PID']
                    lcgi['pid']=pid
                    lcgi['processname']=name
                    lcgi['lines']=lines
                    lrs=executeCMD(x,9999,"LJC-%s" % x,"JOBLOG",lcgi)
                    #print lrs
                    lr=json.loads(lrs)
                    #print lr
                    print lr['answer']['FILE']
                    print lr['answer']['LINES']
                    return
    def configure(self):
        if (self.json==None):
            print "No json file parsed"
            return
        for x in self.servers:
            x.sendTransition("CONFIGURE")
        self.builder.sendTransition("CONFIGURE")
        # Now add sources to the builder
        x=self.json["HARDWARE"]
        for sv in x["EXSERVER"]:
            self.builder.setParameter("detid",sv["detid"])
            self.builder.setParameter("sourceid",sv["sources"])
            self.builder.sendTransition("ADDSOURCES")
        self.builder.sendTransition("REGISTER")
    def start(self,run):
        if (self.json==None):
            print "No json file parsed"
            return
        self.builder.setParameter("run",run)
        self.builder.sendTransition("START")
        for x in self.servers:
            x.sendTransition("START")
    def stop(self):
        for x in self.servers:
            x.sendTransition("STOP")
        self.builder.sendTransition("STOP")
    def status(self):
        print self.builder.sendCommand("LIST")
        for x in self.servers:
            print x.sendCommand("LIST")

#parser = argparse.ArgumentParser()

## configure all the actions
#grp_action = parser.add_mutually_exclusive_group()

#grp_action.add_argument('--ljc-register',action='store_true',help='register job  specified with --job=name in a host with --host=name  if name ids ALL , all jobs/host are considered')
#grp_action.add_argument('--ljc-initialise',action='store_true',help='initialise the LJC on host --host=name with a local file --file=name or an url file --url=name')
#grp_action.add_argument('--ljc-register-start',action='store_true',help='start registration on --host=name')
#grp_action.add_argument('--ljc-register-stop',action='store_true',help='ends registration on --host=name')
#grp_action.add_argument('--ljc-start',action='store_true',help='start processes on --host=name')
#grp_action.add_argument('--ljc-kill',action='store_true',help='kill processes on --host=name')
#grp_action.add_argument('--ljc-destroy',action='store_true',help='destroy registration on --host=name')
#grp_action.add_argument('--ljc-status',action='store_true',help='status of processes on --host=name')
#grp_action.add_argument('--ljc-kill-job',action='store_true',help='stop process with --processname=name or --pid=pid with signal --signal=X on --host=name')
#grp_action.add_argument('--ljc-restart-job',action='store_true',help='restart process with --processname=name or --pid=pid --signal=X on --host=name')
#grp_action.add_argument('--ljc-job-log',action='store_true',help='log of the process with --processname=name or --pid=pid on --host=name')




#grp_action.add_argument('--srv-configure',action='store_true',help='configure the exServer with --host=name --port=num --detid=id --sources=[x,y,z]')
#grp_action.add_argument('--srv-start',action='store_true',help='start the exServer with --host=name --port=num')
#grp_action.add_argument('--srv-stop',action='store_true',help='stop the exServer with --host=name --port=num')
#grp_action.add_argument('--srv-status',action='store_true',help='stop the exBuilder with --host=name --port=num')
#grp_action.add_argument('--srv-halt',action='store_true',help='halt the exServer with --host=name --port=num')

#grp_action.add_argument('--bd-configure',action='store_true',help='configure the exBuilder with --host=name --port=num --memory=path --data=path')
#grp_action.add_argument('--bd-add-sources',action='store_true',help='add sources to the exBuilder --host=name --port=num --det=id --sources=[x,y,z]')
#grp_action.add_argument('--bd-register-done',action='store_true',help='close registration on the exBuilder with --host=name --port=num')

#grp_action.add_argument('--bd-start',action='store_true',help='start the exBuilder with --host=name --port=num --run=number')
#grp_action.add_argument('--bd-stop',action='store_true',help='stop the exBuilder with --host=name --port=num')
#grp_action.add_argument('--bd-status',action='store_true',help='status of the exBuilder with --host=name --port=num')

#grp_action.add_argument('--bd-halt',action='store_true',help='halt the exBuilder with --host=name --port=num')
#grp_action.add_argument('--bd-destroy',action='store_true',help='destroy the exServer with --host=name --port=num')





## Arguments
#parser.add_argument('--job', action='store', dest='job',default=None,help='job name in job control')
#parser.add_argument('--processname', action='store', dest='processname',default=None,help='job name in job control')
#parser.add_argument('--file', action='store', dest='configjson',default=None,help='remote file name in job control initialise')
#parser.add_argument('--url', action='store', dest='url',default=None,help='remote file name in job control initialise')
#parser.add_argument('--pid', action='store', dest='pid',type=int,default=None,help='job pid in job control')
#parser.add_argument('--signal', action='store', dest='signal',type=int,default=15,help='signal to kill process in job control')

#parser.add_argument('--host', action='store', dest='host',default=None,help='host of the application')
#parser.add_argument('--port', action='store', type=int,dest='port',default=None,help='port of the application ')
#parser.add_argument('--det', action='store', type=int,dest='detid',default=None,help='detector id ')
#parser.add_argument('--sources', action='store',dest='sources',default=None,help='List of sources id ')
#parser.add_argument('--memory', action='store', type=str,default=None,dest='memorypath',help='memory path')
#parser.add_argument('--data', action='store', type=str,default=None,dest='datapath',help='data path')

#parser.add_argument('-v','--verbose',action='store_true',default=False,help='set the mysql account')

#results = parser.parse_args()

#print results
#exit(0)
# Analyse results
#if (results.config==None):
    #dc=os.getenv("DAQCONFIG","Not Found")
    #if (dc=="Not Found"):
        #print "please specify a configuration with --config=conf_name"
        #exit(0)
    #else:
        #results.config=dc

## import the configuration
#try:
    #exec("import %s  as conf" % results.config)
#except ImportError:
    #raise Exception("cannot import")

## fill parameters 
#p_par={}

#p_par['dbstate']=conf.dbstate
#p_par['zupdevice']=conf.zupdevice
#p_par['zupport']=conf.zupport
#p_par['filepath']=conf.filepath
#p_par['memorypath']=conf.memorypath
#p_par['proclist']=conf.proclist
#p_par['ctrlreg']=conf.ctrlreg
#p_par['dccname']=conf.dccname
#p_par['mdccname']=conf.mdccname
#p_par['daqhost']=conf.daqhost
#p_par['daqport']=conf.daqport
#p_par['json']=conf.jsonfile

#l_par=json.dumps(p_par,sort_keys=True)

# set the connection mode
#if (results.sockport==None):
    #sp=os.getenv("SOCKPORT","Not Found")
    #if (sp!="Not Found"):
        #results.sockport=int(sp)


#if (results.sockport !=None):
    #socks.setdefaultproxy(socks.PROXY_TYPE_SOCKS5, "127.0.0.1", results.sockport)
    #socket.socket = socks.socksocket
    ##print "on utilise sock",results.sockport
## analyse the command
#lcgi={}
#r_cmd=None
#if(results.ljc_initialise):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=name'
        #exit(0)
    #if (results.configjson==None and results.url==None):
        #print 'Please specify the file with --file=name or --url=name'
        #exit(0)
    #if (results.configjson!=None):
        #lcgi['file']=results.config;
    #elif (results.url!=None):
        #lcgi['url']=results.url;
    #port=9999
    #if (results.port!=None):
        #port=results.port
    
    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeFSM(x,9999,"LJC-%s" % x,"INITIALISE",lcgi)
            #print sr
    #exit(0)

#elif(results.ljc_register_start):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=STATE'
        #exit(0)
    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeFSM(x,9999,"LJC-%s" % x,"REGISTRATION",lcgi)
            
    #exit(0)
#elif(results.ljc_register_stop):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=STATE'
        #exit(0)
    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeFSM(x,9999,"LJC-%s" % x,"ENDREGISTRATION",lcgi)

    #exit(0)

#elif(results.ljc_register):
    #lcgi.clear();
    
    #if (results.host==None):
        #print 'Please specify the state --host=STATE'
        #exit(0)
    #if (results.job==None):
        #print 'Please specify the job --job=STATE'
        #exit(0)
    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #for z in y:
                #if (z["NAME"]==results.job or results.job =="ALL"):
                    #lcgi["processname"]=z["NAME"]
                    #lcgi["processargs"]=json.dumps(z["ARGS"])
                    #lcgi["processenv"]=json.dumps(z["ENV"])
                    #lcgi["processbin"]=json.dumps(z["PROGRAM"])
                    ##print lcgi
                    #sr=executeFSM(x,9999,"LJC-%s" % x,"REGISTERJOB",lcgi)
                    #print x,z["NAME"],"==>",sr
    #exit(0)


#elif(results.ljc_start):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=name'
        #exit(0)
    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeFSM(x,9999,"LJC-%s" % x,"START",lcgi)
            #print sr
    #exit(0)
#elif(results.ljc_kill):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=name'
        #exit(0)
    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeFSM(x,9999,"LJC-%s" % x,"KILL",lcgi)
            #print sr
    #exit(0)
#elif(results.ljc_destroy):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=name'
        #exit(0)
    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeFSM(x,9999,"LJC-%s" % x,"DESTROY",lcgi)
            #print sr
    #exit(0)
#elif(results.ljc_status):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=name'
        #exit(0)
    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeCMD(x,9999,"LJC-%s" % x,"STATUS",lcgi)
            #print sr
    #exit(0)
#elif(results.ljc_kill_job):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=name'
        #exit(0)
    #if (results.processname==None and results.pid==None):
        #print 'Please specify the process with --processname=name or --pid=id'
        #exit(0)
    #if (results.processname!=None):
        #lcgi['processname']=results.processname;
    #elif (results.pid!=None):
        #lcgi['pid']=results.pid;
    #if (results.signal!=None):
        #lcgi['signal']=results.signal;

    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeCMD(x,9999,"LJC-%s" % x,"KILLJOB",lcgi)
            
    #exit(0)
#elif(results.ljc_restart_job):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=name'
        #exit(0)
    #if (results.processname==None and results.pid==None):
        #print 'Please specify the process with --processname=name or --pid=id'
        #exit(0)
    #if (results.processname!=None):
        #lcgi['processname']=results.processname;
    #elif (results.pid!=None):
        #lcgi['pid']=results.pid;
    #if (results.signal!=None):
        #lcgi['signal']=results.signal;

    ##print conf.jsonfile
    #sc=json.load(open(conf.jsonfile))
    #for  x,y in sc["HOSTS"].iteritems():
        #if (x==results.host or results.host=="ALL"):
            #sr=executeCMD(x,9999,"LJC-%s" % x,"RESTARTJOB",lcgi)
            
    #exit(0)
#elif(results.ljc_job_log):
    #lcgi.clear();
    #if (results.host==None):
        #print 'Please specify the state --host=name'
        #exit(0)
    #if (results.processname==None and results.pid==None):
        #print 'Please specify the process with --processname=name or --pid=id'
        #exit(0)
    #if (results.processname!=None):
        #lcgi['processname']=results.processname;
    #elif (results.pid!=None):
        #lcgi['pid']=results.pid;
    #lines=100
    #if (results.lines!=None):
        #lines=results.lines
    #lcgi["lines"]=lines

    #sr=executeCMD(results.host,9999,"LJC-%s" % results.host,"JOBLOG",lcgi)
    #r_cmd='jobLog'
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)

    #exit(0)

#elif(results.daq_state):
    #r_cmd='state'
    #lcgi.clear();
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ",None,None)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)

    #exit(0)
#elif(results.daq_discover):
    #r_cmd='Discover'
    #lcgi.clear()
    #sr=executeFSM(conf.daqhost,conf.daqport,"WDAQ","DISCOVER",lcgi)
    #print sr
    #lcgi.clear()
    #lcgi["params"]=l_par
    #srp=executeCMD(conf.daqhost,conf.daqport,"WDAQ","SETPAR",lcgi)
    #exit(0)
#elif(results.daq_setparameters):
    #r_cmd='setParameters'
    #lcgi.clear()
    #lcgi["params"]=l_par
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","SETPAR",lcgi)
    #print sr
    #exit(0)
#elif(results.daq_getparameters):
    #r_cmd='getParameters'
    #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","GETPAR",lcgi)
    #print sr
    #exit(0)


#elif(results.daq_forceState):
    #r_cmd='forceState'
    #if (results.fstate!=None):
        #lcgi['name']=results.fstate
    #else:
        #print 'Please specify the state --state=STATE'
        #exit(0)
#elif(results.daq_services):
    #r_cmd='prepareServices'
    #lcgi.clear()
    #sr=executeFSM(conf.daqhost,conf.daqport,"WDAQ","PREPARE",p_par)
    #print sr
    #exit(0)
#elif(results.daq_lvon):
    #r_cmd='LVON'
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","LVON",lcgi)
    #print sr
    #exit(0)

#elif(results.daq_lvoff):
    #r_cmd='LVOFF'
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","LVOFF",lcgi)
    #print sr
    #exit(0)
#elif(results.daq_lvstatus):
    #r_cmd='LVStatus'
    #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","LVSTATUS",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)
#elif(results.daq_initialise):
    #r_cmd='initialise'
    #lcgi.clear()
    #sr=executeFSM(conf.daqhost,conf.daqport,"WDAQ","INITIALISE",p_par)
    #print sr
    #exit(0)

#elif(results.daq_configure):
    #r_cmd='configure'
    #lcgi.clear()
    #sr=executeFSM(conf.daqhost,conf.daqport,"WDAQ","CONFIGURE",p_par)
    #print sr
    #exit(0)

#elif(results.daq_status):
    #r_cmd='status'
    #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","DIFSTATUS",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)
#elif(results.daq_evbstatus):
    #r_cmd='shmStatus'
    #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","EVBSTATUS",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)
#elif(results.daq_diflog):
    #r_cmd='difLog'
    #lcgi.clear()
    #if (results.host==None):
        #print 'Please specify the host --host=name'
        #exit(0)
    #lines=100
    #if (results.lines!=None):
        #lines=results.lines
    #lcgi["lines"]=lines
    #sr=executeCMD(results.host,40000,"DIF-%s" % results.host,"JOBLOG",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)
#elif(results.daq_ccclog):
    #r_cmd='cccLog'
    #lcgi.clear()
    #if (results.host==None):
        #print 'Please specify the host --host=name'
        #exit(0)
    #lines=100
    #if (results.lines!=None):
        #lines=results.lines
    #lcgi["lines"]=lines
    #sr=executeCMD(results.host,42000,"Ccc-%s" % results.host,"JOBLOG",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)

#elif(results.daq_mdcclog):
    #r_cmd='mdccLog'
    #lcgi.clear()
    #if (results.host==None):
        #print 'Please specify the host --host=name'
        #exit(0)
    #lines=100
    #if (results.lines!=None):
        #lines=results.lines
    #lcgi["lines"]=lines
    #sr=executeCMD(results.host,41000,"Mdcc-%s" % results.host,"JOBLOG",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)

#elif(results.daq_zuplog):
    #r_cmd='zupLog'
    #lcgi.clear()
    #if (results.host==None):
        #print 'Please specify the host --host=name'
        #exit(0)
    #lines=100
    #if (results.lines!=None):
        #lines=results.lines
    #lcgi["lines"]=lines
    #sr=executeCMD(results.host,43000,"Zup-%s" % results.host,"JOBLOG",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)

#elif(results.daq_startrun):
    #r_cmd='start'
    #lcgi.clear()
    #sr=executeFSM(conf.daqhost,conf.daqport,"WDAQ","START",p_par)
    #print sr
    #exit(0)

#elif(results.daq_stoprun):
    #r_cmd='stop'
    #lcgi.clear()
    #sr=executeFSM(conf.daqhost,conf.daqport,"WDAQ","STOP",p_par)
    #print sr
    #exit(0)
#elif(results.daq_destroy):
    #r_cmd='destroy'
    #lcgi.clear()
    #sr=executeFSM(conf.daqhost,conf.daqport,"WDAQ","DESTROY",p_par)
    #print sr
    #exit(0)
#elif(results.daq_dbstatus):
    #r_cmd='dbStatus'
    #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","DBSTATUS",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)

#elif(results.daq_downloaddb):
    #r_cmd='downloadDB'
    #p_par['dbstate']=results.dbstate
    #lcgi.clear()
    #if (results.dbstate!=None):
        #lcgi['state']=results.dbstate
    #else:
        #print 'Please specify the state --dbstate=STATE'
        #exit(0)
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","DOWNLOADDB",lcgi)
    #print sr
    #exit(0)
#elif(results.daq_ctrlreg):
    #r_cmd='setControlRegister'
    #if (results.ctrlreg!=None):
        #lcgi['value']=int(results.ctrlreg,16)
    #else:
        #print 'Please specify the value --ctrlreg=0xX######'
        #exit(0)
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","CTRLREG",lcgi)
    #print sr
    #exit(0)

#elif(results.trig_status):
    #r_cmd='triggerStatus'
    #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","TRIGGERSTATUS",lcgi)
    ##print "WHAHAHAHA",sr
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)

#elif(results.trig_beam):
    #r_cmd='triggerBeam'
    #if (results.clock!=None):
        #lcgi['clock']=results.clock
    #else:
        #print 'Please specify the number of clock --clock=xx'
        #exit(0)
        #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","BEAMON",lcgi)
    #print sr
    #exit(0)


#elif(results.trig_spillon):
    #r_cmd='triggerSpillOn'
    #if (results.clock!=None):
        #lcgi['clock']=results.clock
    #else:
        #print 'Please specify the number of clock --clock=xx'
        #exit(0)
        #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","SPILLON",lcgi)
    #print sr
    #exit(0)

#elif(results.trig_spilloff):
    #r_cmd='triggerSpillOff'
    #if (results.clock!=None):
        #lcgi['clock']=results.clock
    #else:
        #print 'Please specify the number of clock --clock=xx'
        #exit(0)
        #lcgi.clear()
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","SPILLON",lcgi)
    #print sr
    #exit(0)

#elif(results.ecal_pause):
    #r_cmd='pauseEcal'
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","ECALPAUSE",lcgi)
    #print sr
    #exit(0)

#elif(results.ecal_resume):
    #r_cmd='resumeEcal'
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","ECALRESUME",lcgi)
    #print sr
    #exit(0)
#elif(results.trig_reset):
    #r_cmd='resetTrigger'
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","RESETCOUNTERS",lcgi)
    #print sr
    #exit(0)
#elif(results.trig_pause):
    #r_cmd='pause'
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","PAUSE",lcgi)
    #print sr
    #exit(0)
#elif(results.trig_resume):
    #r_cmd='resume'
    #sr=executeCMD(conf.daqhost,conf.daqport,"WDAQ","RESUME",lcgi)
    #print sr
    #exit(0)
#elif(results.slc_create):
    #r_cmd='createSlowControl'
    ##lcgi['jsonfile']=conf.jsonfile
    #lcgi.clear()
    #sr=executeFSM(conf.slowhost,conf.slowport,"WSLOW","DISCOVER",lcgi)
    #print sr
    #exit(0)

#elif(results.slc_initialisesql):
    #r_cmd='initialiseDB'
    #if (results.account!=None):
        #lcgi['account']=results.account
    #else:
        #print 'Please specify the MYSQL account --account=log/pwd@host:base'
        #exit(0)
    #sr=executeFSM(conf.slowhost,conf.slowport,"WSLOW","INITIALISE",lcgi)
    #print sr
    
#elif(results.slc_loadreferences):
    #r_cmd='loadReferences'
    #lcgi.clear()
    #sr=executeCMD(conf.slowhost,conf.slowport,"WSLOW","LOADREFERENCES",lcgi)
    #print sr
    #exit(0)
#elif(results.slc_hvstatus):
    #r_cmd='hvStatus'
    #lcgi.clear()
    #lcgi['channel']=99;
    #sr=executeCMD(conf.slowhost,conf.slowport,"WSLOW","HVREADCHANNEL",lcgi)
    #if (results.verbose):
        #print sr
    #else:
        #parseReturn(r_cmd,sr)
    #exit(0)

#elif(results.slc_ptstatus):
    #r_cmd='PT'
    #lcgi.clear()
    ##lcgi['channel']=99;
    #sr=executeCMD(conf.slowhost,conf.slowport,"WSLOW","PTREAD",lcgi)
    #print sr
    #exit(0)
#elif(results.slc_setperiod):
    #r_cmd='setReadoutPeriod'
    #lcgi.clear()
    #if (results.period!=None):
        #lcgi['period']=results.period
    #else:
        #print 'Please specify the period --period=second(s)'
        #exit(0)
    #sr=executeCMD(conf.slowhost,conf.slowport,"WSLOW","SETPERIOD",lcgi)
    #print sr
    #exit(0)
#elif(results.slc_setvoltage):
    #r_cmd='setVoltage'
    #lcgi.clear()
    #if (results.first!=None):
        #lcgi['first']=results.first
    #else:
        #print 'Please specify the channels --first=# --last=#'
        #exit(0)
    #if (results.last!=None):
        #lcgi['last']=results.last
    #else:
        #print 'Please specify the channels --first=# --last=#'
        #exit(0)
    #if (results.voltage!=None):
        #lcgi['voltage']=results.voltage
    #else:
        #print 'Please specify the voltage --voltage=V'
        #exit(0)
    #sr=executeCMD(conf.slowhost,conf.slowport,"WSLOW","SETVOLTAGE",lcgi)
    #print sr
    #exit(0)

#elif(results.slc_setcurrent):
    #r_cmd='setCurrentLimit'
    #lcgi.clear()
    #if (results.first!=None):
        #lcgi['first']=results.first
    #else:
        #print 'Please specify the channels --first=# --last=#'
        #exit(0)
    #if (results.last!=None):
        #lcgi['last']=results.last
    #else:
        #print 'Please specify the channels --first=# --last=#'
        #exit(0)
    #if (results.current!=None):
        #lcgi['current']=results.current
    #else:
        #print 'Please specify the current limit --current=V'
        #exit(0)
    #sr=executeCMD(conf.slowhost,conf.slowport,"WSLOW","SETCURRENTLIMIT",lcgi)
    #print sr
    #exit(0)
    
#elif(results.slc_hvon):
    #r_cmd='HVON'
    #lcgi.clear()
    #if (results.first!=None):
        #lcgi['first']=results.first
    #else:
        #print 'Please specify the channels --first=# --last=#'
        #exit(0)
    #if (results.last!=None):
        #lcgi['last']=results.last
    #else:
        #print 'Please specify the channels --first=# --last=#'
        #exit(0)
    #sr=executeCMD(conf.slowhost,conf.slowport,"WSLOW","HVON",lcgi)
    #print sr
    #exit(0)

#elif(results.slc_hvoff):
    #r_cmd='HVOFF'
    #lcgi.clear()
    #if (results.first!=None):
        #lcgi['first']=results.first
    #else:
        #print 'Please specify the channels --first=# --last=#'
        #exit(0)
    #if (results.last!=None):
        #lcgi['last']=results.last
    #else:
        #print 'Please specify the channels --first=# --last=#'
        #exit(0)
    #sr=executeCMD(conf.slowhost,conf.slowport,"WSLOW","HVOFF",lcgi)
    #print sr
    #exit(0)
#elif(results.slc_store):
    #r_cmd='startStorage'
    #if (results.period!=None):
        #lcgi['period']=results.period
    #else:
        #print 'Please specify the period --period=second(s)'
        #exit(0)
    #sr=executeFSM(conf.slowhost,conf.slowport,"WSLOW","STARTMONITOR",lcgi)
    #print sr
    #exit(0)
#elif(results.slc_check):
    #r_cmd='startCheck'
    #if (results.period!=None):
        #lcgi['period']=results.period
    #else:
        #print 'Please specify the period --period=second(s)'
        #exit(0)
    #sr=executeFSM(conf.slowhost,conf.slowport,"WSLOW","STARTCHECK",lcgi)
    #print sr
    #exit(0)

#elif(results.slc_store_stop):
    #r_cmd='stopStorage'
    #sr=executeFSM(conf.slowhost,conf.slowport,"WSLOW","STOPMONITOR",lcgi)
    #print sr
    #exit(0)

#elif(results.slc_check_stop):
    #r_cmd='stopCheck'
    #sr=executeFSM(conf.slowhost,conf.slowport,"WSLOW","STOPCHECK",lcgi)
    #print sr
    #exit(0)

##print r_cmd
##print lcgi


#def sendcommand2(command,host=p_par["daqhost"],port=p_par['daqport'],lq=None):
   #global results 
   #if (lq!=None):
       #if (len(lq)!=0):
           #myurl = "http://"+host+ ":%d" % (port)
           ##conn = httplib.HTTPConnection(myurl)
           ##if (name!=None):
           ##    lq['name']=name
           ##if (value!=None):
           ##    lq['value']=value
           #lqs=urllib.urlencode(lq)
           #saction = '/%s?%s' % (command,lqs)
           #myurl=myurl+saction
           ##print myurl
           #req=urllib2.Request(myurl)
           #r1=urllib2.urlopen(req)

           #return r1.read()
   #else:
       #myurl = "http://"+host+ ":%d" % (port)
       ##conn = httplib.HTTPConnection(myurl)
       #saction = '/%s' % command
       #myurl=myurl+saction
       ##print myurl
       #req=urllib2.Request(myurl)
       #r1=urllib2.urlopen(req)
       #if (command=="status" and not results.verbose):
           #s=r1.read()
           #sj=json.loads(s)
           #ssj=json.loads(sj["statusResponse"]["statusResult"][0])
##           for x in ssj:
##             for d in x["difs"]:
##                print '#%4d %5x %6d %12d %12d %s %s ' % (d["id"],d["slc"],d["gtc"],d["bcid"],d["bytes"],d["state"],x["name"])
           #print "\033[1m %4s %5s %6s %12s %12s %15s  %s \033[0m" % ('DIF','SLC','EVENT','BCID','BYTES','SERVER','STATUS')

           #for d in ssj:
               ##print d
               ##for d in x["difs"]:
               #print '#%4d %5x %6d %12d %12d %15s %s ' % (d["id"],d["slc"],d["gtc"],d["bcid"],d["bytes"],d["host"],d["state"])
       #elif (command=="jobStatus" and not results.verbose ):
           #s=r1.read()
           #sj=json.loads(s)
           #ssj=json.loads(sj["jobStatusResponse"]["jobStatusResult"][0])
           #print "\033[1m %6s %15s %25s %20s \033[0m" % ('PID','NAME','HOST','STATUS')
           #for x in ssj:
               #if (x['DAQ']=='Y'):
                   #print "%6d %15s %25s %20s" % (x['PID'],x['NAME'],x['HOST'],x['STATUS'])
       #elif (command=="hvStatus" and not results.verbose):
           #s=r1.read()
           #sj=json.loads(s)
           #ssj=json.loads(sj["hvStatusResponse"]["hvStatusResult"][0])
           #print "\033[1m %5s %10s %10s %10s %10s \033[0m" % ('Chan','VSET','ISET','VOUT','IOUT')
           #for x in ssj:
               #print "#%.4d %10.2f %10.2f %10.2f %10.2f" % (x['channel'],x['vset'],x['iset'],x['vout'],x['iout'])

       #elif (command=="LVStatus" and not results.verbose ):
           #s=r1.read()
           #sj=json.loads(s)
           #ssj=json.loads(sj["LVStatusResponse"]["LVStatusResult"][0])
           #print "\033[1m %10s %10s %10s \033[0m" % ('VSET','VOUT','IOUT')
           #print " %10.2f %10.2f %10.2f" % (ssj['vset'],ssj['vout'],ssj['iout'])
           ##for x in ssj:
           ##    print "#%.4d %10.2f %10.2f %10.2f %10.2f" % (x['channel'],x['vset'],x['iset'],x['vout'],x['iout'])
       #elif (command=="shmStatus" and not results.verbose):
           #s=r1.read()
           #sj=json.loads(s)
           #ssj=json.loads(sj["shmStatusResponse"]["shmStatusResult"][0])
           #print "\033[1m %10s %10s \033[0m" % ('Run','Event')
           #print " %10d %10d " % (ssj['run'],ssj['event'])
       #elif (command=="triggerStatus" and not results.verbose):
           #s=r1.read()
           #sj=json.loads(s)
           #ssj=json.loads(sj["triggerStatusResponse"]["triggerStatusResult"][0])
           #print "\033[1m %10s %10s %10s %10s %12s %12s %10s %10s %10s \033[0m" % ('Spill','Busy1','Busy2','Busy3','SpillOn','SpillOff','Beam','Mask','EcalMask')
           #print " %10d %10d %10d %10d  %12d %12d %12d %10d %10d " % (ssj['spill'],ssj['busy1'],ssj['busy2'],ssj['busy3'],ssj['spillon'],ssj['spilloff'],ssj['beam'],ssj['mask'],ssj['ecalmask'])

       #else:
          #print r1.read()
          #return r1.read()

       
       ##print r1.status, r1.reason

#if (r_cmd==None):
    #print "No command given"
    #exit(0)
#if (len(lcgi)!=0):
    #sendcommand2(r_cmd,lq=lcgi)
#else:
    #sendcommand2(r_cmd)
#exit(0)
