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

#include <cassert>

//#define INFORMAVE_DAL_DEBUG
//#define INFORMAVE_DAL_TRACE


#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

#define CODE_POS() __FILE__ ":"                 \
    TOSTRING(__LINE__)

#define ShowMessage(msg) std::wcerr << L">>>>> " << msg << std::endl;

#define UNHANDLED(str) throw std::runtime_error(std::string("[ERR][").append(CODE_POS()).append("][").append(__PRETTY_FUNCTION__).append("] ==>>> ").append(str))

#define NOT_IMPLEMENTED "Function not implemented"


#define NOT_IMPL() throw std::runtime_error(std::string("[ERR][").append(CODE_POS()).append("][").append(__PRETTY_FUNCTION__).append("] ==>>> ").append(NOT_IMPLEMENTED))



DAL_NAMESPACE_BEGIN

#define DALDEBUG(message)                                               \
    {                                                                   \
        std::stringstream __dal_internal_ss;                            \
        __dal_internal_ss << message;                                   \
        dal_debug(__FUNCTION__, __FILE__, __LINE__, __dal_internal_ss.str().c_str()); \
    }
#define DALTRACE(message) { dal_trace(__FUNCTION__, __FILE__, __LINE__, message); }
#define DALTRACE_ENTER { dal_trace(__FUNCTION__, __FILE__, __LINE__, "ENTER"); }
#define DALTRACE_LEAVE { dal_trace(__FUNCTION__, __FILE__, __LINE__, "LEAVE"); }


void dal_trace(const char* func, const char* file, unsigned int line, const char* s);
void dal_debug(const char* func, const char* file, unsigned int line, const char* s);


DAL_NAMESPACE_END

#endif
