#!/usr/bin/env python
"""
Module to control an exServer+exBuilder example

Typical run:

import jobc as jc
# Create a setup with a description file
st=jc.exSetup("http://lyopc252/daq/levbdim_example.json")
# parse the file and create builder and server controllers
st.parse()
# start the process remotely
st.startJobs()
# check they are running
st.statusJobs()
# configure 
st.configure()
# check the states
st.state()
# Start a run 3421
st.start(3421)
# check the running
st.status()
# stop the run
st.stop()
# kill the jobs
st.killJobs()



This script can be run using SOCKS if the environmental variable SOCKPORT is set
"""
import os
import socks
import socket
import httplib, urllib,urllib2
from urllib2 import URLError, HTTPError

import json
from copy import deepcopy

import time

# Check SOCKPORT, if defined use SOCKS
sockport=None
sp=os.getenv("SOCKPORT","Not Found")
if (sp!="Not Found"):
   sockport=int(sp)
   print sockport
if (sockport !=None):
   socks.setdefaultproxy(socks.PROXY_TYPE_SOCKS5, "127.0.0.1", sockport)
   socket.socket = socks.socksocket

def parseReturn(command,sr):
    """ Parsing the command return.
    Args:
    command: command name
    sr: the return lines (in json or string format)
    """
    if (command=="statusJobs"):
      print "\033[1m %6s %15s %25s %20s \033[0m" % ('PID','NAME','HOST','STATUS')
      for x in sr['JOBS']:
        print "%6d %15s %25s %20s" % (x['PID'],x['NAME'],x['HOST'],x['STATUS'])
      
    if (command=="status" ):
      ssj=sr["answer"]["answer"]["dataSources"]
      print "\033[1m %12s %12s %12s \033[0m" % ('DetID','SourceId','EVENT')

      for d in ssj:
        print '#%12d %12d %12d  ' % (d["detid"],d["sourceid"],d["event"])
        
    if (command=="builderStatus" ):
      sj=sr['answer']
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
    if (command=="jobLog"):   
      print  "\033[1m %s \033[0m" % sr["answer"]["FILE"]
      ssj=sr["answer"]["LINES"]
      print ssj
 
def discover(host,port):
   """ Dump the page of a fsmweb  http://host:port.
   Args:
    host: hostname
    port: port of the fsmweb
   """
   myurl = "http://"+host+ ":%d" % (port)
   req=urllib2.Request(myurl)
   r1=urllib2.urlopen(req)
   return r1.read()
 
def executeFSM(host,port,prefix,cmd,params):
   """ Call an fsmweb transition
   Args:
    host: hostname
    port: port of the fsmweb
    prefix: the fsmweb url prefix
    cmd: the transition name
    params: array of parameters to be encoded in the content field
   Returns:
    the json encoded page or None 
   """
   if (params!=None):
       myurl = "http://"+host+ ":%d" % (port)
       lq={} 
       lq["content"]=json.dumps(params,sort_keys=True)
       lq["command"]=cmd           
       lqs=urllib.urlencode(lq)
       saction = '/%s/FSM?%s' % (prefix,lqs)
       myurl=myurl+saction
       #print myurl
       req=urllib2.Request(myurl)
       r1=urllib2.urlopen(req)
       return r1.read()
   else:
       return None

def executeCMD(host,port,prefix,cmd,params):
   """ Call an fsmweb command.
   Args:
    host: hostname
    port: port of the fsmweb
    prefix: the fsmweb url prefix
    cmd: the command name
    params: array of parameters to be encoded in the url
   Returns:
    the json encoded page or None 
   """
   if (params!=None and cmd!=None):
       myurl = "http://"+host+ ":%d" % (port)
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
       #print myurl
       req=urllib2.Request(myurl)
       r1=urllib2.urlopen(req)
       return r1.read()
    
class levProcess:
    """ Class handling fsmweb interactions.
    """
    def Dump(self):
        " Print object info"
        print self.host,self.port,self.prefix
        for pname,vpar in sorted(self.params.iteritems()):
            print "params[",pname,"]=",vpar
    def discover(self):
        " Print Process capabilities"
        print "\033[1m levProcess: http://%s:%d \033[0m" % (self.host,self.port)
        parseReturn("state",discover(self.host,self.port))
    def sendTransition(self,cmd):
        " Send a transition with the parameters stroed in the object"
        self.rc=executeFSM(self.host,self.port,self.prefix,cmd,self.params)
        return self.rc
    def sendCommand(self,cmd):
        " send a command with the parameters stroed in the object"
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
        " Clear the parameters list"
        self.params.clear()

    def __init__(self,host,port,prefix):
        """
        Constructor.
        Args:
         host: the hostname
         port: the fsmweb port
         prefix: the prefix url of FSM or CMD 
        """
        self.host=host
        self.port=port
        self.prefix=prefix
        self.params={}

class exServer(levProcess):
    " exServer handling class"
    def __init__(self,host,port,detid,sources):
        """ Constructor.
        Args: 
         host,port: the levProcess parameters (prefix='EXS-host')
         detid: the detector id
         sources: an array of sourceids
        """
        prf="EXS-"+host
        levProcess.__init__(self,host,port,prf)
        self.clear()
        self.setParameter("detid",detid)
        self.setParameter("sourceid",sources)

class exBuilder(levProcess):
    " exBuilder handling class"
    def __init__(self,host,port,mempath,datapath):
        """ Constructor.
        Args: 
         host,port: the levProcess parameters (prefix='EXB-host')
         mempath: the /dev/shm path 
         datapath: teh directroy where data are written
        """
        prf="EXB-"+host
        levProcess.__init__(self,host,port,prf)
        self.clear()
        self.setParameter("memdir",mempath)
        self.setParameter("datadir",datapath)

class exSetup:
    "Dummy example of a run + job control"
    def __init__(self,jsonfile):
        """
        Constructor.
        Args:
        jsonfile: the url of the configuration file (may be local file=///xxxxx.json)
         
        """
        self.jfile=jsonfile
        self.json=None
        self.builder=None
        self.servers=[]
    def parse(self):
        """ Parse the configuration file.
        it find in the HARWARE section, the list of exServer and creates the corresponding exServer controllers

        it also find the exBuilder and creates its controller
        """
        self.builder=None
        self.servers=[]
        req=urllib2.Request(self.jfile)
        try:
            r1=urllib2.urlopen(req)
        except URLError,  e:
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
        "Loop on the HOSTS section of the configuration file and register all processes"
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            sr=executeFSM(x,9999,"LJC-%s" % x,"INITIALISE",lcgi)
    def startJobs(self):
        " Loop on the HOSTS section of the configuration file and creates all registered processes"
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            sr=executeFSM(x,9999,"LJC-%s" % x,"START",lcgi) 
    def killJobs(self):
        " Loop on the HOSTS section of the configuration file and kill all registered processes"
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            sr=executeFSM(x,9999,"LJC-%s" % x,"KILL",lcgi) 
    def statusJobs(self):
        "Loop on the HOSTS section of the configuration file and dump status of all registered processes"
        if (self.json==None):
            print "No json file parsed"
            return
        lcgi={}
        #lcgi['url']=self.jfile
        for  x,y in self.json["HOSTS"].iteritems():
            sr=json.loads(executeCMD(x,9999,"LJC-%s" % x,"STATUS",lcgi))
            print "Worker :",x
            parseReturn('statusJobs',sr['answer'])

    def restartJob(self,host,name):
        " restart the process named name on host"
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
        " kill the process named name on host"
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
        " dump the last lines of the log of the process named name on host" 
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
                    parseReturn('jobLog',lr)
                    #print lr
                    #print lr['answer']['FILE']
                    #print lr['answer']['LINES']
                    return
    def configure(self):
        " Send a configure transition to all exServers and to the exBuilder"
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
        " Start the run " 
        if (self.json==None):
            print "No json file parsed"
            return
        self.builder.setParameter("run",run)
        self.builder.sendTransition("START")
        for x in self.servers:
            x.sendTransition("START")
    def stop(self):
        " stop the run"
        for x in self.servers:
            x.sendTransition("STOP")
        self.builder.sendTransition("STOP")
    def state(self):
        " state and capabilities of all exServers and exBuilder"
        for x in self.servers:
            x.discover()
        self.builder.discover()
    def status(self):
        " Send a LIST command to the builder and to all exServers"
        parseReturn("builderStatus",json.loads(self.builder.sendCommand("LIST")))
        for x in self.servers:
            parseReturn("status",json.loads(x.sendCommand("LIST")))

