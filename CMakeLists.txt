#
# Project file for DBWTL
#

cmake_minimum_required(VERSION 2.6.1)

# project name
project(dbwtl)

# versions
set(${PROJECT_NAME}_MAJOR_VERSION 0)
set(${PROJECT_NAME}_MINOR_VERSION 1)
set(${PROJECT_NAME}_PATCH_LEVEL 0)


if( CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE )
  message(FATAL_ERROR "In-source builds are not allowed.
  Please create a directory and run cmake from there, passing the path
  to this source directory as the last argument.
  This process created the file `CMakeCache.txt' and the directory
  `CMakeFiles'.
  Please delete them.")
endif()


# library Unix style versioning
set(${PROJECT_NAME}_SOVERSION
	"${${PROJECT_NAME}_MAJOR_VERSION}")
set(${PROJECT_NAME}_VERSION
	"${${PROJECT_NAME}_MAJOR_VERSION}.${${PROJECT_NAME}_MINOR_VERSION}.${${PROJECT_NAME}_PATCH_LEVEL}")

#set(CMAKE_VERBOSE_MAKEFILE true)


#set(PACKAGE_NAME dbwtl)
#set(PACKAGE_VERSION 0.0.1)
#set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
#set(PACKAGE_BUGREPORT "bugs@informave.org")


set(DBWTL_MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(DBWTL_MAIN_DOC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc)
set(DBWTL_MAIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DBWTL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})



INCLUDE_DIRECTORIES( ${DBWTL_MAIN_INCLUDE_DIR} )
INCLUDE_DIRECTORIES( ${DBWTL_BINARY_DIR} )


#IF(NOT CMAKE_BUILD_TYPE)
#	SET(CMAKE_BUILD_TYPE Release CACHE STRING
#		"Choose the type of build, options are: None Debug Release."
#		FORCE)
#ENDIF(NOT CMAKE_BUILD_TYPE)

string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)

OPTION(USE_LOCAL_HEADERS
	"Use local installed header files instead of the files packed with
	dbwtl" OFF)

OPTION(WITH_TESTS
	"Compile and run tests" OFF)

OPTION(WITH_SQLITE
	"Compile with SQLite support" ON)



configure_file (
  "${DBWTL_MAIN_INCLUDE_DIR}/dbwtl/dbwtl_config.hh.in"
  "${PROJECT_BINARY_DIR}/dbwtl/dbwtl_config.hh"
  )

ADD_SUBDIRECTORY(doc)
  
IF(uppercase_CMAKE_BUILD_TYPE STREQUAL "RELEASE")
	ADD_DEFINITIONS("-std=c++98 -Wno-unused -O2 -DNDEBUG")
ELSEIF(uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG")
	#ADD_DEFINITIONS("-std=c++98 -Wmissing-braces -g -fmessage-length=1000 -Wno-unused -O0")
	ADD_DEFINITIONS("-O1 -W -Wall -Wno-old-style-cast -Woverloaded-virtual
	-Wsign-promo -Wctor-dtor-privacy -std=c++98 -Wmissing-braces
	-Wswitch-default -Wuninitialized -Wsign-compare -g -pedantic
	-fmessage-length=1000 -Wextra -Weffc++ -Wno-unused -Wno-switch-enum")
	#set(CMAKE_VERBOSE_MAKEFILE true)
ELSE()
	ADD_DEFINITIONS("-std=c++98")
ENDIF()


SET(dbwtl_srcs
        ${DBWTL_MAIN_SRC_DIR}/dal/dal_interface.cc
        ${DBWTL_MAIN_SRC_DIR}/dal/generic/generic_engine.cc
	${DBWTL_MAIN_SRC_DIR}/db_exceptions.cc
	${DBWTL_MAIN_SRC_DIR}/ustring.cc
)


IF(WITH_SQLITE)
SET(dbwtl_srcs
        ${DBWTL_MAIN_SRC_DIR}/dal/sqlite/sqlite_engine.cc
        ${DBWTL_MAIN_SRC_DIR}/dal/sqlite/sqlite_libsqlite.cc
	${dbwtl_srcs}
)
ENDIF(WITH_SQLITE)




ADD_LIBRARY(dbwtl SHARED ${dbwtl_srcs})

target_link_libraries(dbwtl dl)



SET_TARGET_PROPERTIES(dbwtl PROPERTIES
	VERSION ${${PROJECT_NAME}_VERSION}
	SOVERSION ${${PROJECT_NAME}_SOVERSION}
)


set(WITH_DOXYFILE_PDF false)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include(UseDoxygen)



install(DIRECTORY ${DBWTL_MAIN_INCLUDE_DIR}
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.def"
  PATTERN "*.h"
  PATTERN "*.td"
  PATTERN "*.inc"
  PATTERN ".svn" EXCLUDE
  )


IF(WITH_TESTS)
	INCLUDE(CTest)
	ADD_SUBDIRECTORY(tests)
ENDIF(WITH_TESTS)



