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

SET(CMAKE_COLOR_MAKEFILE ON)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)


set(MEMORYCHECK_COMMAND_OPTIONS "--leak-check=full --show-possibly-lost=no --show-reachable=no --suppressions=${CMAKE_CURRENT_SOURCE_DIR}/dbwtl.supp")


if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR AND NOT MSVC_IDE)
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

#Charset converter
option(DBWTL_USE_ICONV "Use iconv." OFF)
option(DBWTL_USE_ICU "Use ICU." ON)

option(DBWTL_ENABLE_PEDANTIC "Compile with pedantic enabled." OFF)
option(DBWTL_ENABLE_WERROR "Fail and stop if a warning is triggered." OFF)

if(MSVC)
   option(DBWTL_ENABLE_WARNINGS "Enable compiler warnings." OFF)
else(MSVC)
   option(DBWTL_ENABLE_WARNINGS "Enable compiler warnings." ON)
endif()



option(DBWTL_CXX98_COMPATIBILITY
	"Use C++98 header and features instead of the current standard
	version" OFF)



set(DBWTL_DEFAULT_CHARSET "UTF-8" CACHE STRING "This charset is used for input 1-byte chars when converting to the internal wide charset")

set(DBWTL_INTERNAL_CHARTYPE "2" CACHE STRING "This char type is
internally used (only change if you really know what you are doing)")

option(DBWTL_USE_LOCAL_HEADERS
        "Use local installed header files instead of the files packed with
        dbwtl" OFF)

option(DBWTL_WITH_TESTS
        "Compile and run tests" OFF)

option(DBWTL_WITH_SQLITE
        "Compile with SQLite support" ON)

option(DBWTL_WITH_FIREBIRD
	"Compile with Firebird support" OFF)


option(DBWTL_WITH_SDI
	"Compile with SDI support" OFF)

option(DBWTL_WITH_ODBC
	"Compile with ODBC support" OFF)


#select plattform
if(WIN32)
  if(CYGWIN)
    set(DBWTL_ON_WIN32 0)
    set(DBWTL_ON_UNIX 1)
  else(CYGWIN)
    set(DBWTL_ON_WIN32 1)
    set(DBWTL_ON_UNIX 0)
  endif(CYGWIN)
  set(LTDL_SHLIB_EXT ".dll")
  set(EXEEXT ".exe")
else(WIN32)
  if(UNIX)
    set(DBWTL_ON_WIN32 0)
    set(DBWTL_ON_UNIX 1)
    if(APPLE)
      set(LTDL_SHLIB_EXT ".dylib")
    else(APPLE)
      set(LTDL_SHLIB_EXT ".so")
    endif(APPLE)
    set(EXEEXT "")
  else(UNIX)
    MESSAGE(SEND_ERROR "Unable to determine platform")
  endif(UNIX)
endif(WIN32)


#special options for MSVC
if(MSVC)
  # List of valid CRTs for MSVC
  set(MSVC_CRT
      MD
      MDd
      MT
      MTd)

  set(DBWTL_USE_CRT "" CACHE STRING "Specify VC++ CRT to use for debug/release configurations.")

  add_definitions("-D_CRT_SECURE_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS")
  add_definitions("-D_SCL_SECURE_NO_WARNINGS -DCRT_NONSTDC_NO_WARNINGS")
  add_definitions("-D_SCL_SECURE_NO_DEPRECATE")
  add_definitions("-wd4309 -wd4251 -wd4800 -wd4099 -wd4275 -wd4244 -wd4351")

  if(NOT ${DBWTL_USE_CRT} STREQUAL "")
    list(FIND MSVC_CRT ${DBWTL_USE_CRT} idx)
    if(idx LESS 0)
      message(FATAL_ERROR "Invalid value for DBWTL_USE_CRT: ${DBWTL_USE_CRT}. Valid options are one of: ${MSVC_CRT}")
    endif(idx LESS 0)
    add_definitions("/${DBWTL_USE_CRT}")
    message(STATUS "Using VC++ CRT: ${DBWTL_USE_CRT}")
  endif(NOT ${DBWTL_USE_CRT} STREQUAL "")

  # Enable warnings
  if(DBWTL_ENABLE_WARNINGS)
    add_definitions("/W4 /Wall")
    if(DBWTL_ENABLE_PEDANTIC)
      # No MSVC equivalent available
    endif(DBWTL_ENABLE_PEDANTIC)
  endif(DBWTL_ENABLE_WARNINGS)
  if(DBWTL_ENABLE_WERROR)
    add_definitions("/WX")
  endif(DBWTL_ENABLE_WERROR)

elseif(CMAKE_COMPILER_IS_GNUCXX)
	#add_definitions("-std=c++98 -Wno-non-virtual-dtor")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-virtual-dtor -std=c++0x")
	#add_definitions("-Wno-non-virtual-dtor -std=c++0x")
  if (DBWTL_ENABLE_WARNINGS)
	  add_definitions("-Wall -W -Wextra -Wno-unused-parameter
    -Wwrite-strings  -Wno-unused -Wno-switch-enum")
    if(DBWTL_ENABLE_PEDANTIC)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic -Wno-long-long -Weffc++ -Wno-non-virtual-dtor")
      #add_definitions("-pedantic -Wno-long-long -Weffc++ -Wno-non-virtual-dtor")
    endif(DBWTL_ENABLE_PEDANTIC)
  endif(DBWTL_ENABLE_WARNINGS)
  if(DBWTL_ENABLE_WERROR)
    add_definitions("-Werror")
  endif(DBWTL_ENABLE_WERROR)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-non-virtual-dtor -std=c++0x -Wno-tautological-constant-out-of-range-compare")
endif(MSVC)


#set(PACKAGE_NAME dbwtl)
#set(PACKAGE_VERSION 0.0.1)
#set(PACKAGE_STRING "${PACKAGE_NAME} ${PACKAGE_VERSION}")
#set(PACKAGE_BUGREPORT "bugs@informave.org")

# Directories
set(DBWTL_MAIN_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(DBWTL_MAIN_DOC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/doc)
set(DBWTL_MAIN_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DBWTL_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR})

# Include directories
include_directories( ${DBWTL_MAIN_INCLUDE_DIR} )
include_directories( ${DBWTL_BINARY_DIR} )


if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Release CACHE STRING
		"Choose the type of build, options are: None Debug Release."
		FORCE)
endif(NOT CMAKE_BUILD_TYPE)

string(TOUPPER "${CMAKE_BUILD_TYPE}" uppercase_CMAKE_BUILD_TYPE)


if(DBWTL_USE_LOCAL_HEADERS)
	set(DBWTL_LOCAL_HEADERS_INCLUDE_DIR
	"${DBWTL_MAIN_INCLUDE_DIR}/headers")
        include_directories(${DBWTL_MAIN_INCLUDE_DIR}/headers)
endif()



include(CheckIncludeFiles)


check_include_files("stdint.h" DBWTL_HAVE_STDINT_H)


if(DBWTL_WITH_SQLITE)
	if(DBWTL_USE_LOCAL_HEADERS)
		IF (EXISTS "${DBWTL_LOCAL_HEADERS_INCLUDE_DIR}/sqlite3.h")
			SET(DBWTL_HAVE_SQLITE3_H true)
		endif()
	else()
		check_include_files("sqlite3.h" DBWTL_HAVE_SQLITE3_H)
	endif()
	if(NOT DBWTL_HAVE_SQLITE3_H)
		message(FATAL_ERROR "Required header sqlite3.h not found.")
  	endif()
endif()





configure_file(
  "${DBWTL_MAIN_INCLUDE_DIR}/dbwtl/dbwtl_config.hh.in"
  "${PROJECT_BINARY_DIR}/dbwtl/dbwtl_config.hh"
  )
  
# Version info resource
if(MSVC)  
	configure_file(
 		"${DBWTL_MAIN_INCLUDE_DIR}/version.rc.in"
  		"${PROJECT_BINARY_DIR}/version.rc"
)  
endif(MSVC)




if(NOT MSVC)
	add_subdirectory(doc)
endif()
  

#include Debug information
if(CMAKE_COMPILER_IS_GNUCXX AND (uppercase_CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
	add_definitions("-O0 -g")
	add_definitions("-ftest-coverage -fprofile-arcs")
	link_libraries( gcov )
endif()

#add_definitions("-ftest-coverage -fprofile-arcs")

#SET(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} " -ftest-coverage -fprofile-arcs")
#SET(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} " -ftest-coverage -fprofile-arcs")

#add_definitions( -fprofile-arcs -ftest-coverage )
#link_libraries( gcov )
#set( CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} --coverage )

# Lemon parser
add_executable(lemon ${CMAKE_CURRENT_SOURCE_DIR}/lemon/lemon.c)


add_custom_command(
        OUTPUT ${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/parser.cc
        COMMAND lemon -p -s T=${CMAKE_CURRENT_SOURCE_DIR}/lemon/lempar.c ${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/parser.y
        DEPENDS lemon ${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/parser.y
        ${CMAKE_CURRENT_SOURCE_DIR}/lemon/lempar.c
        ${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/ast.hh # depend workaround cmake
)



# Source files
set(dbwtl_srcs
	${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/parser.cc
	${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/sqlparser.cc
	${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/token.cc
	${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/ast.cc
	${DBWTL_MAIN_SRC_DIR}/dal/sqlproxy/sqlcursor.cc
        ${DBWTL_MAIN_SRC_DIR}/dal/dal_interface.cc
	${DBWTL_MAIN_SRC_DIR}/utils.cc
	${DBWTL_MAIN_SRC_DIR}/recordset.cc
	${DBWTL_MAIN_SRC_DIR}/shrrecord.cc
	${DBWTL_MAIN_SRC_DIR}/cachedresult.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_bigint.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_bool.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_blob.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_int.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_float.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_ivariant.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_smallint.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_string.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_memo.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_date.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_time.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_timestamp.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_numeric.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_interval.cc
	${DBWTL_MAIN_SRC_DIR}/types/type_varbinary.cc
        ${DBWTL_MAIN_SRC_DIR}/dal/generic/generic_engine.cc
	${DBWTL_MAIN_SRC_DIR}/exceptions.cc
	${DBWTL_MAIN_SRC_DIR}/variant.cc
	${DBWTL_MAIN_SRC_DIR}/variant_expr.cc
	${DBWTL_MAIN_SRC_DIR}/variant_binary.cc
	${DBWTL_MAIN_SRC_DIR}/ustring.cc
	${DBWTL_MAIN_SRC_DIR}/debug.cc
	${DBWTL_MAIN_SRC_DIR}/dal/sqlutils.cc
)


if(DBWTL_WITH_SDI)
	set(dbwtl_srcs
		${DBWTL_MAIN_SRC_DIR}/dal/sdi/sdi_engine.cc
		${DBWTL_MAIN_SRC_DIR}/dal/sdi/sdi_libsdi.cc
		${dbwtl_srcs}
	)
endif(DBWTL_WITH_SDI)

if(DBWTL_WITH_ODBC)
	set(dbwtl_srcs
		${DBWTL_MAIN_SRC_DIR}/dal/odbc/odbc_libodbc.cc
		${DBWTL_MAIN_SRC_DIR}/dal/odbc/odbc_engine.cc
		${dbwtl_srcs}
	)
endif(DBWTL_WITH_ODBC)

if(DBWTL_WITH_SQLITE)
	set(dbwtl_srcs
        	${DBWTL_MAIN_SRC_DIR}/dal/sqlite/sqlite_engine.cc
        	${DBWTL_MAIN_SRC_DIR}/dal/sqlite/sqlite_libsqlite.cc
		${dbwtl_srcs}
	)
endif(DBWTL_WITH_SQLITE)


if(DBWTL_WITH_FIREBIRD)
        set(dbwtl_srcs
                ${DBWTL_MAIN_SRC_DIR}/dal/firebird/firebird_engine.cc
                ${DBWTL_MAIN_SRC_DIR}/dal/firebird/firebird_libfbclient.cc
                ${dbwtl_srcs}
        )
endif(DBWTL_WITH_FIREBIRD)


if(MSVC)  
	set(dbwtl_srcs
		"${PROJECT_BINARY_DIR}/version.rc"
  		${dbwtl_srcs})
endif(MSVC)


# library
add_library(dbwtl SHARED ${dbwtl_srcs})


#if(UNIX)
#	target_link_libraries(dbwtl dl)
#endif()


# ICU Library
if(DBWTL_USE_ICU)
	set(ICU_FIND_REQUIRED true)
	set(ICU_FIND_QUIETLY false)
endif()

include(FindICU)

if(DBWTL_USE_ICU)
	include_directories(${ICU_INCLUDE_DIRS})
	target_link_libraries(dbwtl ${ICU_LIBRARIES})
endif()



set_target_properties(dbwtl PROPERTIES
	VERSION ${${PROJECT_NAME}_VERSION}
	SOVERSION ${${PROJECT_NAME}_SOVERSION}
)


set(WITH_DOXYFILE_PDF false)

if(NOT MSVC)
	include(UseDoxygen)
endif()



if(UNIX)
	install(TARGETS dbwtl LIBRARY DESTINATION lib)
endif(UNIX)

if(NOT UNIX)
	install(TARGETS dbwtl RUNTIME DESTINATION lib)
	install(TARGETS dbwtl ARCHIVE DESTINATION lib LIBRARY DESTINATION lib)
endif(NOT UNIX)

install(DIRECTORY ${DBWTL_MAIN_INCLUDE_DIR}/dbwtl
  DESTINATION include
  FILES_MATCHING
  PATTERN "*.hh"
  REGEX "^[^\\.]*$"
  PATTERN ".git*" EXCLUDE
)

install(FILES "${PROJECT_BINARY_DIR}/dbwtl/dbwtl_config.hh"
	DESTINATION include/dbwtl)


#include Tests
if(DBWTL_WITH_TESTS)
	ENABLE_TESTING()
	include(CTest)
	add_subdirectory(tests)
endif(DBWTL_WITH_TESTS)

