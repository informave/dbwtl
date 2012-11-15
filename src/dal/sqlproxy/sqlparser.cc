//
// sqlparser.cc - SQL Parser
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
//

///
/// @cond DEV_DOCS
/// @file
/// @brief SQL Parser
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "sqlparser.hh"
#include "dbwtl/dal/dal_interface.hh"

#include <cassert>

SQLPROXY_NAMESPACE_BEGIN


/// @details
/// 
ParseTree*
SqlParser::parse(String s)
{

    std::wstringstream ss;
    ss << s;

    return load(ss, "SQLINPUT");


}

/// @details
/// 
ParseTree*
SqlParser::load(std::istreambuf_iterator<wchar_t> in, String sourcename)
{
    Token t;
    Parser p;
    Tokenizer<wchar_t> tz(in);
    tz.setSourceName(sourcename);

    ParseTree *m_tree = new ParseTree();



    //p.trace(stdout, "[LEMON] ");
    do
    {
        t = tz.next();
        //std::cout << "found token: " << t.getSourceInfo() << std::endl;
        if(t.id() != 0)
        {
            assert(! t.data().empty());
            assert(t.getSourceInfo().linenum() > 0);
            
            Token *tp = m_tree->newToken(t);

            p.parse(t.id(), tp, m_tree);
        }
        else
            p.parse(0, NULL, m_tree);
    }
    while(t != Token::eof());

    return m_tree;
}


SQLPROXY_NAMESPACE_END


