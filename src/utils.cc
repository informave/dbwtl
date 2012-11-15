

#include "utils.hh"

#include <sstream>
#include <string>
#include <stdexcept>


DB_NAMESPACE_BEGIN

namespace utils
{

 void raise_bugcheck_exception(const char *cond, const char *what, int line, const char *file, const char *function)
 {
	std::stringstream ss;

	ss << ">>>INTERNAL BUGCHECK<<< Condition failed: " << cond << " at " << file << ":" << line << " {" << function << "}";

 	throw std::runtime_error(ss.str());
 }


}
DB_NAMESPACE_END


