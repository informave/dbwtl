

add_subdirectory(other)

if(DBWTL_WITH_FIREBIRD)
	add_subdirectory(firebird)
endif(DBWTL_WITH_FIREBIRD)

if(DBWTL_WITH_ODBC)
	add_subdirectory(odbc)
endif(DBWTL_WITH_ODBC)

#add_subdirectory(sdi)

