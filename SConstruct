import sys,os,commands
import  os
import re
import sys

def SWIGSharedLibrary(env, library, sources, **args):
  swigre = re.compile('(.*).i')
  if env.WhereIs('swig') is None:
    sourcesbis = []
    for source in sources:
      cName = swigre.sub(r'\1_wrap.c', source)
      cppName = swigre.sub(r'\1_wrap.cc', source)
      if os.path.exists(cName):
        sourcesbis.append(cName)
      elif os.path.exists(cppName):
        sourcesbis.append(cppName)
      else:
        sourcesbis.append(source)
  else:
    sourcesbis = sources
 
  if 'SWIGFLAGS' in args:
    args['SWIGFLAGS'] += ['-python']
  else:
    args['SWIGFLAGS'] = ['-python'] + env['SWIGFLAGS']
  args['SHLIBPREFIX']=""
  if sys.version >= '2.5':
    args['SHLIBSUFFIX']=".pyd"
 
  cat=env.SharedLibrary(library, sourcesbis, **args)
  return cat
 


# environment
#import xdaq
#print xdaq.INCLUDES
#print xdaq.LIBRARY_PATHS
#print xdaq.LIBRARIES

#print "----------------------------------------------"
Decider('MD5-timestamp')
XDAQ_ROOT="/opt/xdaq"
DHCAL_ROOT=os.path.abspath("..")
Use_Dim=True
#os.environ.has_key("DIM_DNS_NODE")
fres=os.popen('uname -r')
kl=fres.readline().split(".")

platform="UBUNTU"
if (kl[len(kl)-1][0:3] == 'el5'):
    platform="SLC5"

if (kl[len(kl)-2][0:3] == 'el6'):
    platform="SLC6"
print kl[len(kl)-2][0:3]
fres=os.popen('uname -p')
kp=fres.readline()
osv=kp[0:len(kp)-1]

print platform,osv

Bit64=False
Bit64=os.uname()[4]=='x86_64'

Arm=os.uname()[4]=='armv7l'

if Arm or platform=="UBUNTU":
  boostsystem='boost_system'
  boostthread='boost_thread'
else:
  boostsystem='boost_system-mt'
  boostthread='boost_thread-mt'
  
# includes
INCLUDES=['include',"/usr/include/boost141/","/usr/include/jsoncpp","/usr/local/include","/usr/local/include/mongoose"]


INCLUDES.append(commands.getoutput("python -c 'import distutils.sysconfig as conf; print conf.get_python_inc()'"))

CPPFLAGS=["-DLINUX", "-DREENTRANT" ,"-Dlinux", "-DLITTLE_ENDIAN__ ", "-Dx86",  "-DXERCES=2", "-DDAQ_VERSION_2"]

#Library ROOT + some of XDAQ + DB 



LIBRARIES=['pthread',  'm', 'stdc++','log4cxx','jsoncpp','z','mongoose',boostsystem,boostthread]



#Library path XDAQ,DHCAL and ROOT + Python
if (Bit64):
	LIBRARY_PATHS=["/usr/lib64","/usr/local/lib","/usr/lib64/boost141"]
else:
  LIBRARY_PATHS=["/usr/lib","/usr/local/lib","/usr/lib/boost141"]
LIBRARY_PATHS.append(commands.getoutput("python -c 'import distutils.sysconfig as conf; print conf.PREFIX'")+"/lib")


if Use_Dim:
   CPPFLAGS.append("-DUSE_DIM")
   INCLUDES.append("/opt/dhcal/dim/dim")
   LIBRARIES.append("dim")
   LIBRARY_PATHS.append("/opt/dhcal/dim/linux")


#link flags
#LDFLAGS=["-fPIC","-dynamiclib"]
LDFLAGS=["-fPIC","-dynamiclib"]


# SWIG
SWIGSF=["-c++","-classic"]

for i in INCLUDES:
    SWIGSF.append("-I"+i)
print SWIGSF

# Create the Environment
env = Environment(CPPPATH=INCLUDES,CPPFLAGS=CPPFLAGS,LINKFLAGS=LDFLAGS, LIBS=LIBRARIES,LIBPATH=LIBRARY_PATHS,SWIGFLAGS=SWIGSF)

#print "CC is:",env.subst('$CPPPATH')

env['BUILDERS']['PythonModule'] = SWIGSharedLibrary


# Library source
LIBRARY_SOURCES=Glob("#src/*.cxx")

#Shared library
levbdim=env.SharedLibrary("#lib/levbdim",LIBRARY_SOURCES)

#Python module
#MYLIBS=["levbdim",'pthread',  'm', 'stdc++',boostsystem,boostthread]
#_levbdim=env.PythonModule('_Llevbdim', ['Llevbdim.i'],LIBPATH="#lib",LIBS=MYLIBS)

#env.Install(DHCAL_ROOT+"/opt/dhcal/lib",levbdim)
###env.Install("/opt/dhcal/lib",levbdim)
###env.Install("/opt/dhcal/include/readout",myinc)

###env.Alias('install', [DHCAL_ROOT+"/opt/dhcal/lib","/opt/dhcal/lib","/opt/dhcal/include/rpiccc"])
#env.Alias('install', [DHCAL_ROOT+"/opt/dhcal/lib"])

EXE_LIBPATH=LIBRARY_PATHS
EXE_LIBPATH.append("#lib")
EXE_LIBS=LIBRARIES
EXE_LIBS.append("levbdim")
dums=env.Program("bin/dummyS",source="test/dummyServer.cxx",LIBPATH=EXE_LIBPATH,LIBS=EXE_LIBS)
dumb=env.Program("bin/dummyB",source="test/dummyBuilder.cxx",LIBPATH=EXE_LIBPATH,LIBS=EXE_LIBS)

