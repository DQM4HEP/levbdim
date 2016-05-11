#ifndef _levbdim_datasource_h
#define _levbdim_datasource_h

#include <stdint.h>
#include <stdlib.h>
#include <dis.hxx>
#include <string>
#include "buffer.hh"
namespace levbdim {
  /**
    \class datasource
    \author    Laurent Mirabito
    \version   1.0
    \date      May 2016
    \copyright GNU Public License.
    \brief the datasource instantiates a buffer and the assoicated DimService
    \details The buffer is allocated and its identifaction is fixed. Access to
    the payload is given and at publication time the user provides event identification and
    payload size.
  */

  class datasource 
  {
  public:
    /**
       \brief constructor
       \param detid, Detector id
       \param sourceid, data source id
       \param max_size, maximum size of the buffer
       \details it allocates a buffer and creates a DimService with name
       /LEVBDIM/DS-#detid-#sourceid/DATA
     */
    datasource(uint32_t detid,uint32_t sourceid,uint32_t max_size);
    //! Destructor, it destroy the buffer and the DimService
    ~datasource();
    //! Pointer to the buffer payload
    void* payload();
    /**
       \brief publication of data
       \details it updates the buffer Event,bunch crossing, and payload size and then 
       publish the DimService
     */
    void publish(uint32_t evt,uint64_t bx,uint32_t payload_size);
 
    //! Access to the buffer
    levbdim::buffer* buffer();
  private:
    DimService* _service;
    levbdim::buffer* _buffer;
    uint32_t _detid,_sourceid;
    std::string _name;
    int32_t _fdOut;
  };
};
#endif
