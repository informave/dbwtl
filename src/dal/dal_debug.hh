//
// dal_debug.hh - DAL debugging
//
// Copyright (C)         informave.org
//   2010,               Daniel Vogelbacher <daniel@vogelbacher.name>
//
// BSD License
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the
// distribution.
//
// Neither the name of the copyright holders nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

/// @file
/// @brief DAL debugging
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_DEBUG_HH
#define INFORMAVE_DB_DAL_DEBUG_HH

#include "dbwtl/dal/dal_fwd.hh"
#include "../devutils.hh"

#include <cassert>

/// Enable this macros if required during development
/// or testing.

//#define DBWTL_DEBUG_MESSAGES
//#define DBWTL_DEBUG_TRACE





DAL_NAMESPACE_BEGIN


/*
struct DebugValue2Str
{
	virtual String operator()(void) const = 0;
};
*/

template<typename T>
struct DebugValue2Str
{
	String operator()(const T& var) const
	{
		std::stringstream ss;
		ss << var;
		return String(ss.str(), "utf-8");
	}
};


template<> struct DebugValue2Str<String> { String operator()(const String& var) const { return String("\"") + var + String("\""); } };


template<typename T>
inline String debugWrapper(const T &var)
{
	DebugValue2Str<T> dvs;
	return dvs(var);
}

struct DebugTraceGuard
{
	DebugTraceGuard(const char *fun, int line, const char *file)
	: data(String("[ENTER] ") +  String(fun) + String("()")), fun(fun)
	{
		writeData();
	}

	DebugTraceGuard(const char *fun, int line, const char *file, const char *an, const String &a)
	: data(String("[ENTER] ") +  String(fun) + String("()")), fun(fun)
	{
		data += String(", [");
		data += String(an) + String("=") + a;
		data += "]";
		writeData();
	}

        DebugTraceGuard(const char *fun, int line, const char *file, const char *an, const String &a,
	const char *bn, const String &b)
        : data(String("[ENTER] ") +  String(fun) + String("()")), fun(fun)
        {
                data += String(", [");
                data += String(an) + String("=") + a;
		data += String(", ") + String(bn) + String("=") + b;
                data += "]";
		writeData();
        }


	~DebugTraceGuard(void)
	{
		std::cerr << "[LEAVE] " << fun << "()" << std::endl;
	}

	void writeData(void) const
	{
		std::cerr << data << std::endl;
	}

	String data;
	String fun;
};

template<typename T>
void trace_value_helper(const char *name, const T& value)
{
	std::cerr << "[VALUE] " << name << ": " << debugWrapper(value) << std::endl;
}


#ifdef DBWTL_TRACE_ENABLE
#define DBWTL_TRACE0() DebugTraceGuard _foobar(__FUNCTION__, __LINE__, __FILE__)


#define DBWTL_TRACE_VALUE(value) trace_value_helper(#value, value)

#define DBWTL_TRACE1(arg1) DebugTraceGuard _foobar(__FUNCTION__, __LINE__, __FILE__, #arg1, debugWrapper(arg1))

#define DBWTL_TRACE2(a, b) DebugTraceGuard _foobar(__FUNCTION__, __LINE__, __FILE__, \
	#a, debugWrapper(a), \
	#b, debugWrapper(b) )


#define DBWTL_TRACE3(a, b, c)

#else
#define DBWTL_TRACE0()
#define DBWTL_TRACE1(a)
#define DBWTL_TRACE2(a, b)
#define DBWTL_TRACE_VALUE(a)
#endif

std::string string_format(const std::string &fmt, ...);


class debug_error : public std::runtime_error
{
public:
    debug_error(const std::string &what) : std::runtime_error(what)
        {}

    virtual ~debug_error(void) throw()
        {}
};


/// 
#define DBWTL_UNHANDLED(str)                                            \
    throw debug_error(std::string("[ERR][").append(DBWTL_CODE_POS) \
                             .append("][")                              \
                             .append(DBWTL_FUNC_NAME)                   \
                             .append("] ==>>> ")                        \
                             .append(str))



/// Throws if function is not implemented
#define DBWTL_NOTIMPL()                                             \
    throw debug_error(std::string("[ERR][")                  \
                             .append(DBWTL_CODE_POS)              \
                             .append("][")                          \
                             .append(DBWTL_FUNC_NAME)               \
                             .append("] ==>>> ")                    \
                             .append("Function not implemented"))



#define DBWTL_BUGCHECK(cond) if(!(cond)) throw std::runtime_error("BUG on " DBWTL_CODE_POS ": Expression failed: " #cond )

#define DBWTL_BUGCHECK_SOFT(cond) if(!(cond)) throw std::runtime_error("BUG on " DBWTL_CODE_POS ": Expression failed: " #cond )

#define DBWTL_BUGCHECK_EX(cond, msg) if(!(cond)) throw std::runtime_error(msg)

#define DBWTL_BUG() throw std::runtime_error("BUG on " DBWTL_CODE_POS )


#define DBWTL_BUG_FMT(fmt, ...) throw std::runtime_error(std::string("BUG at ") \
                                                        .append(DBWTL_CODE_POS) \
                                                        .append(": ")   \
                                                        .append(string_format(fmt, ##__VA_ARGS__)))


#define DBWTL_BUG_EX(what) throw std::runtime_error(std::string("BUG at ") \
                                                        .append(DBWTL_CODE_POS) \
                                                        .append(": ")   \
                                                        .append(what))

/// @brief Standard debug macro
#ifdef DBWTL_DEBUG_MESSAGES
#define DAL_DEBUG(message)                                              \
    {                                                                   \
        std::stringstream __dal_internal_ss;                            \
        __dal_internal_ss << message;                                   \
        dal_debug(__FUNCTION__, __FILE__, __LINE__, __dal_internal_ss.str().c_str()); \
    }
#else
#define DAL_DEBUG(message)
#endif

/// @brief Trace and print a message
#define DALTRACE(message) { dal_trace(__FUNCTION__, __FILE__, __LINE__, message); }


///
#define DALTRACE_ENTER { dal_trace(__FUNCTION__, __FILE__, __LINE__, "ENTER"); }
#define DALTRACE_LEAVE { dal_trace(__FUNCTION__, __FILE__, __LINE__, "LEAVE"); }
#define DALTRACE_VISIT { dal_trace(__FUNCTION__, __FILE__, __LINE__, "VISIT"); }


void dal_trace(const char* func, const char* file, unsigned int line, const char* s);
void dal_debug(const char* func, const char* file, unsigned int line, const char* s);


DAL_NAMESPACE_END


#endif
