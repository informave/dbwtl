//
// parserapi.hh - C++ Wrapper from Lemon API
//
// Copyright (C)         informave.org
//   2012,               Daniel Vogelbacher <daniel@vogelbacher.name>
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


/// @file
/// @brief C++ wrapper for Lemon API
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_SQLPROXY_PARSERAPI_HH
#define INFORMAVE_DB_SQLPROXY_PARSERAPI_HH

#include <iosfwd>
#include <cstdlib>

#include "sqlproxy_fwd.hh"
#include "parser.h"
#include "ast.hh"
#include "token.hh"

#define ParseTOKENTYPE informave::db::sqlproxy::Token*
#define ParseARG_PDECL ,informave::db::sqlproxy::ParseTree *tree


void SqlProxyParser(void *,               // The parser
           int,                  // The major token code number
           ParseTOKENTYPE        // The value for the token
           ParseARG_PDECL);      // Optional %extra_argument parameter


void SqlProxyParserFree(void *p,                    // The parser to be deleted
               void (*freeProc)(void*) );  // Function used to reclaim memory


void *SqlProxyParserAlloc(void *(*mallocProc)(size_t));

void SqlProxyParserTrace(FILE *stream, char *zPrefix);


SQLPROXY_NAMESPACE_BEGIN



//--------------------------------------------------------------------------
/// Lemon Parser Wrapper
///
/// @since 0.0.1
/// @brief Lemon Parser Wrapper
class Parser
{
public:
    Parser(void) : m_parser(0)
    {
        m_parser = SqlProxyParserAlloc(::malloc);
    }

    inline void parse(int tid, ParseTOKENTYPE token, ParseTree *tree)
    {
        SqlProxyParser(m_parser, tid, token, tree);
    }

    inline void trace(FILE *stream, const char *prefix)
    {
        SqlProxyParserTrace(stream, const_cast<char*>(prefix));
    }

    ~Parser(void)
    {
        if(m_parser)
        {
            SqlProxyParserFree(m_parser, ::free);
            m_parser = 0;
        }
    }

protected:
    void *m_parser;

private:
    Parser(const Parser&);
    Parser& operator=(const Parser&);
};


SQLPROXY_NAMESPACE_END



#endif


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
