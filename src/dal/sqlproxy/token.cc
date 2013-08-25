//
// token.cc - Token class (definition)
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

/// @file
/// @brief Token class used by the Tokenizer (definition)
/// @author Daniel Vogelbacher
/// @since 0.1

#include "tokenizer.hh"
#include "token.hh"
//#include "tokenizer.hh"

#include <sstream>
#include <cassert>
#include <iostream>


SQLPROXY_NAMESPACE_BEGIN


InvalidToken::InvalidToken(wchar_t ch, const SourceInfo &si)
	: EngineException()
{
	std::wstringstream ss;
	ss << L"Invalid Token >" << ch << L"<, at ";
	ss << si.str();
	this->setMessage(ss.str());
}

//..............................................................................
///////////////////////////////////////////////////////////////////// SourceInfo

/// @details
/// 
SourceInfo::SourceInfo(String file, std::streamsize chpos, size_t len, size_t line)
    : m_file(file), 
      m_offset(chpos),
      m_len(len),
      m_line(line)
{}


/// @details
/// 
void
SourceInfo::expand(const SourceInfo &info)
{
    /// @todo uncomment asserts if all AST nodes implemented
    // assert(info.m_line > 0);
    // assert(info.m_len > 0);

    if(m_line == 0 || m_line > info.m_line)
        m_line = info.m_line == 0 ? m_line : info.m_line;

    if(m_file.empty())
        m_file = info.m_file;

    if(m_offset == 0 || info.m_offset < m_offset)
        m_offset = info.m_offset;

    if(m_offset + m_len < info.m_offset + info.m_len)
        this->m_len = (info.m_offset + info.m_len) - m_offset;
}


//..............................................................................
////////////////////////////////////////////////////////////////////////// Token

/// @details
/// 
Token::Token(void)
    : m_id(0),
      m_data(), 
      m_info()
{}


/// @details
/// 
Token::Token(int tid, SourceInfo info, String data)
    : m_id(tid),
      m_data(data),
      m_info(info)
{}


/// @details
/// 
Token::Token(const Token& t)
    : m_id(t.m_id),
      m_data(t.m_data),
      m_info(t.m_info)
{}


/// @details
/// 
void 
Token::setData(const String &str)
{
    this->m_data = str;
}


/// @details
/// 
String 
Token::data(void) const
{
    return this->m_data;
}


/// @details
/// 
int 
Token::id(void) const
{
    return this->m_id;
}


/// @details
/// 
const SourceInfo&
Token::getSourceInfo(void) const
{
    return this->m_info;
}


/// @details
/// 
std::string 
Token::str(void) const
{
    std::stringstream tmp;
          
    tmp << "<"
        << this->m_id
        << "> "
        << "Start: "
        << this->getSourceInfo().offset()
        << " Line: "
        << this->getSourceInfo().linenum()
        << " DATA: "
        << this->m_data;
            
    return tmp.str();
}


/// @details
///  
Token 
Token::eof(void)
{
    return Token(0);
}


/// @details
/// 
bool 
Token::operator !=(const Token &tok) const
{
    return m_id != tok.m_id;
}

SQLPROXY_NAMESPACE_END


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
