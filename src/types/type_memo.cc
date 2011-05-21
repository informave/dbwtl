//
// type_memo.cc - Type: MEMO (definitions)
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
/// @brief Type: MEMO (definitions)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/db_fwd.hh"
#include "dbwtl/variant.hh"
#include "dbwtl/types.hh"
#include "dbwtl/db_exceptions.hh"
#include "../dal/dal_debug.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>

DB_NAMESPACE_BEGIN


///
///
Memo::Memo(UnicodeStreamBuf *buf) : std::wistream(0),
                                    m_buf(buf)
{
    this->rdbuf(m_buf);
}

///
///
Memo::Memo(const IVariant &variant) : std::wistream(0),
                                      m_buf()
{
    this->operator=(variant.asMemo());
}


///
///
Memo::Memo(const Memo& m) : std::basic_ios<wchar_t>(),
                            std::wistream(0),
                            m_buf()
{
    this->operator=(m);
}


///
///
Memo&
Memo::operator=(const Memo& m)
{
    this->m_buf = m.m_buf;
    this->rdbuf(m_buf);
    return *this;
}



//
//
Memo::~Memo(void)
{}



//
//
std::string
Memo::narrow_str(const char *charset) const
{
    return String(this->str()).to("UTF-8");
}


//
//
std::wstring
Memo::str() const
{
    try
    {
        return dynamic_cast<const std::wstringbuf&>(*this->m_buf).str();
    }
    catch(std::bad_cast &)
    {
        std::wstringstream ss;
        ss << this->m_buf;
        return ss.str();
    }
}



///
/// 
Memo
read_accessor<Memo>::asMemo(void) const
{
    return this->getValue();
}


DB_NAMESPACE_END
