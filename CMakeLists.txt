cmake_minimum_required(VERSION 3.30)

project(Websockets LANGUAGES C CXX VERSION 1.0.0)


set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.14)
set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")


# include and setup pd.cmake
set(PDCMAKE_FILE ${CMAKE_BINARY_DIR}/pd.cmake)
set(PDCMAKE_VERSION "v0.2.4")
if(NOT EXISTS "${PDCMAKE_FILE}")
    file(DOWNLOAD https://raw.githubusercontent.com/pure-data/pd.cmake/refs/tags/${PDCMAKE_VERSION}/pd.cmake ${PDCMAKE_FILE})
endif()
include(${PDCMAKE_FILE})


# add help files
pd_add_datafile(RabbitControl package/help/pd/wsclient-help.pd)
pd_add_datafile(RabbitControl package/help/pd/wsserver-help.pd)

#
include_directories(dependencies/scaryws)
add_subdirectory(dependencies/scaryws)


# include projects
include(wsclient.cmake)
include(wsserver.cmake)
