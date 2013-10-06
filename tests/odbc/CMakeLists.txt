
set(DBWTL_TESTCONF_ODBC_DSN "DSN=dbwtl_test" CACHE STRING "Database name for
ODBC tests")

set(DBWTL_TESTCONF_ODBC_UID "sa" CACHE STRING "Database name for
ODBC tests")

set(DBWTL_TESTCONF_ODBC_PWD "foo" CACHE STRING "Database name for
ODBC tests")

set(DBWTL_TESTCONF_ODBC_ANSI_CS "ISO8859_1" CACHE STRING "Database name for
ODBC tests")

set(DBWTL_TESTCONF_ODBC_USE_UNICODE "yes" CACHE STRING "Database name for
ODBC tests")


configure_file("${CMAKE_CURRENT_SOURCE_DIR}/fixture_odbc.hh.in"
	        "${CMAKE_CURRENT_SOURCE_DIR}/fixture_odbc.hh"
)

FILE (GLOB DBWTL_TEST_FILES_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc )

foreach(t ${DBWTL_TEST_FILES_SRC})
	string(REGEX REPLACE "\\.cc$" "" TMP_TEST_NAME ${t})
	add_executable(odbc_${TMP_TEST_NAME}_test ${t})
	target_link_libraries (odbc_${TMP_TEST_NAME}_test dbwtl)
	add_test(odbc_${TMP_TEST_NAME} odbc_${TMP_TEST_NAME}_test)
	message("Building test: " odbc_${TMP_TEST_NAME})
endforeach(t)


