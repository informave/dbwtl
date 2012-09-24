#ifndef ACTIVE_ENGINES
#define ACTIVE_ENGINES

#include "dbwtl/dbwtl_config.hh"

#include "engines/generic_engine.hh"

#if defined(DBWTL_WITH_SQLITE)
#include "engines/sqlite_engine.hh"
#endif


#if defined(DBWTL_WITH_FIREBIRD)
#include "engines/firebird_engine.hh"
#endif

#endif

