#################################################
# cmake file for building levbdim package
# @author Eté Rémi
CMAKE_MINIMUM_REQUIRED( VERSION 2.6 FATAL_ERROR )
#################################################

# project name
PROJECT( levbdim )

SET( ${PROJECT_NAME}_VERSION_MAJOR 1 )
SET( ${PROJECT_NAME}_VERSION_MINOR 0 )
SET( ${PROJECT_NAME}_VERSION_PATCH 0 )

# --------------- ilcsoft dependencies -----------
LIST( APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake )

# load default settings
INCLUDE( default_settings )

###############################
# Build option
OPTION( BUILD_TESTS "Whether to build tests" OFF )

###############################
# Find Threads or multithreading.
FIND_PACKAGE( Threads REQUIRED )

###############################
# Find packages
FIND_PACKAGE( DIM REQUIRED SHARED )   # for networking
FIND_PACKAGE( JSONCPP REQUIRED )      # io support
FIND_PACKAGE( ZLIB REQUIRED )         # buffer compression
FIND_PACKAGE( Boost COMPONENTS system thread REQUIRED  ) # generic support libraries

FOREACH( pkg DIM JSONCPP ZLIB Boost )
	
	INCLUDE_DIRECTORIES( ${${pkg}_INCLUDE_DIRS} )
	LINK_LIBRARIES( ${${pkg}_LIBRARIES} )
	
	LIST( APPEND ${PROJECT_NAME}_DEPENDS_INCLUDE_DIRS  ${${pkg}_INCLUDE_DIRS} )
	LIST( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARY_DIRS  ${${pkg}_LIBRARY_DIRS} )
	LIST( APPEND ${PROJECT_NAME}_DEPENDS_LIBRARIES     ${${pkg}_LIBRARIES} )
	
ENDFOREACH()

###############################
# Generate DIMJCConfig.h 
CONFIGURE_FILE( "${PROJECT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.h.cmake.in"
    "${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}Config.hh" @ONLY )

###############################
# include directories
INCLUDE_DIRECTORIES( include )

# require proper c++
ADD_DEFINITIONS( "-pedantic -Wunused-value -O2" )
ADD_DEFINITIONS("-Wno-long-long -Wreturn-type")

FILE( GLOB
    SRC_FILES
    src/*.cxx
)

ADD_SHARED_LIBRARY( ${PROJECT_NAME} ${SRC_FILES} )

INSTALL( 
    TARGETS ${PROJECT_NAME}
    LIBRARY DESTINATION lib
)

IF( BUILD_TESTS )

	INCLUDE_DIRECTORIES( test )
	
	ADD_EXECUTABLE( dummyServer test/dummyServer.cxx )
	TARGET_LINK_LIBRARIES( dummyServer ${PROJECT_NAME} )
	
	INSTALL( 
	    TARGETS dummyServer
	    RUNTIME DESTINATION bin
	)
	
	ADD_EXECUTABLE( dummyBuilder test/dummyBuilder.cxx )
	TARGET_LINK_LIBRARIES( dummyBuilder ${PROJECT_NAME} )
	
	INSTALL( 
	    TARGETS dummyBuilder
	    RUNTIME DESTINATION bin
	)

ENDIF()


###############################
# display some variables and write them to cache
DISPLAY_STD_VARIABLES()

###############################
# generate and install following configuration files
GENERATE_PACKAGE_CONFIGURATION_FILES( ${PROJECT_NAME}Config.cmake ${PROJECT_NAME}ConfigVersion.cmake ${PROJECT_NAME}LibDeps.cmake )

