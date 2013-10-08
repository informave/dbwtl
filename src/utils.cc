



#include <sstream>
#include <string>
#include <stdexcept>

#include "dbwtl/exceptions.hh"

#include "utils.hh"

DB_NAMESPACE_BEGIN

namespace utils
{

 void raise_bugcheck_exception(const char *cond, const char *what, int line, const char *file, const char *function)
 {
	std::stringstream ss;

	ss << ">>>INTERNAL BUGCHECK<<< Condition failed: " << cond << " at " << file << ":" << line << " {" << function << "}";

 	throw Exception(ss.str());
 }


}
DB_NAMESPACE_END


