cmake_minimum_required (VERSION 2.6)
project (PiloteMyBoxServer)

set(SOURCES 
        ../src/main.cpp
	)

	
# add the executable
add_executable(PiloteMyBoxServer ${SOURCES})

#-- libs
target_link_libraries(PiloteMyBoxServer psapi ws2_32 wsock32 powrprof)

# The version number.
set (PiloteMyBoxServer_VERSION_MAJOR 1)
set (PiloteMyBoxServer_VERSION_MINOR 0)

#-- library
SET(BOOST_ROOT "C:/boost_1_53_0")
find_package(Boost 1.53 COMPONENTS filesystem thread system REQUIRED)
set(Boost_USE_STATIC_LIBS ON) 
message(status "** Boost Include: ${Boost_INCLUDE_DIR}")
message(status "** Boost Libraries: ${Boost_LIBRARY_DIRS}")
message(status "** Boost Libraries: ${Boost_LIBRARIES}")
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})
TARGET_LINK_LIBRARIES( PiloteMyBoxServer ${Boost_LIBRARIES} )

set(GNUtoMS 0)
set(CMAKE_INSTALL_PREFIX $PWD/../install)
SET(CMAKE_BUILD_TYPE None)
