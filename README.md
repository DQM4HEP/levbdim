# LEVBDIM A Light EVent Builder based on DIM
## Introduction

DIM[1] is an HEP acquisition framework developped in the DELPHI experiement. It embeds binary packet exchange in messages published by server and subscribed by client.
It is light and TCP/IP based anc can be installed on nearly all currently used OS nowdays.

LEVBDIM is a simple acquisition framework where DIM is used to exchange structured binary buffer. It provides several functionnalities oftenly used in modern data acquisition:

  - Predefine structure of buffer
  - publication of data source
  - collection of data source
  - Event building
  - Finite State Machine
  - Web access
  - process control

All those functionnalities are mainly independant and can be used on their own.
The last section of this documenation details a full example using all the capabilities of LEVBDIM.

## Installation

### Needed packages

Few standard packages are needed and can be found on any linux distributions: boost, jsoncpp, curl, git and scons

#### Example on Debian

     sudo apt-get -y install libboost-dev libboost-system-dev libboost-filesystem-dev libboost-thread-dev libjsoncpp-dev libcurl4-gnutls-dev git scons

Two network libraries should be installed. The first one is DIM that can be download and compile from https://dim.web.cern.ch/dim/. The second one is used to give web access to the application, 
it's Mongoose-cpp [2] based on Mongoose[3]. One version is distributed with levbdim and can be compile before the installation.

### LEVBDIM installation

#### Download the software

     git clone http://github.com/mirabitl/levbdim.git

#### Install mongoose-cpp

    cd levbdim
    source web/mongoose.install

### Compile the code

    cd levbdim
    scons 

## Developper's guide

### The event builder

Event building consists of merging various data source that collect event fragment at the same time. Each data source should consequently have a localisation tag and a time tag for each data fragment 
it provides. This fragment are published by a DimService and is centrally collected to build an event, i.e a collection of data fragment with an identical time tag. 
### Data encapsulation and exchange

#### Buffer structure
