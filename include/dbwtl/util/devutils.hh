#ifndef INFORMAVE_DB_UTIL_DEVUTILS_HH
#define INFORMAVE_DB_UTIL_DEVUTILS_HH

#include <iostream>
#include <cassert>


#define DAL_NEEDS_CHECK(msg)                                            \
    {                                                                   \
        std::cerr << "[*** NEED CHECK ***] " msg << " # " << __FILE__   \
                  << ": " << __FUNCTION__                               \
                  << std::endl;                                         \
    }

#define DAL_NOT_IMPL() { assert(!"not implemented"); }


#endif
