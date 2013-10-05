
FILE (GLOB DBWTL_TEST_FILES_SRC RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cc )

foreach(t ${DBWTL_TEST_FILES_SRC})
	string(REGEX REPLACE "\\.cc$" "" TMP_TEST_NAME ${t})
	add_executable(odbc_${TMP_TEST_NAME}_test ${t})
	target_link_libraries (odbc_${TMP_TEST_NAME}_test dbwtl)
	add_test(odbc_${TMP_TEST_NAME} odbc_${TMP_TEST_NAME}_test)
	message("Building test: " odbc_${TMP_TEST_NAME})
endforeach(t)


