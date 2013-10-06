
set(DBWTL_TESTCONF_SQLITE_DATABASE "/tmp/dbwtl_test.sqlite3" CACHE STRING "Database name for
SQLite3 tests")

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/fixture_sqlite3.hh.in"
	"${CMAKE_CURRENT_SOURCE_DIR}/fixture_sqlite3.hh"
)


FILE (GLOB DBWTL_TEST_FILES_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc )

foreach(t ${DBWTL_TEST_FILES_SRC})
	string(REGEX REPLACE "\\.cc$" "" TMP_TEST_NAME ${t})
	add_executable(${TMP_TEST_NAME}_test ${t})
	target_link_libraries (${TMP_TEST_NAME}_test dbwtl)
	add_test(${TMP_TEST_NAME} ${TMP_TEST_NAME}_test)
	message("Building test: " ${TMP_TEST_NAME})
endforeach(t)


