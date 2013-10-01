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
#include "dbwtl/dal/dal_interface.hh"
#include "../dal/dal_debug.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>

DB_NAMESPACE_BEGIN



//
//
Memo::Memo(const Memo& blob) : m_data()
{
    this->m_data << blob.rdbuf();
}

//
//
Memo::Memo(const Variant& data) : m_data()
{
    this->m_data << data.get<String>(this->m_data.getloc());
}



//
//
Memo& Memo::operator=(const Memo& memo)
{
    this->m_data.clear();
    memo.rdbuf()->pubseekpos(0);
    this->m_data << memo.rdbuf();
    return *this;
}


//
//
Memo::Memo(void) : m_data()
{}


//
//
Memo::Memo(UnicodeStreamBuf *buf) : m_data()
{
    this->m_data << buf;
}


//
//
Memo::~Memo(void)
{
}


//
//
std::wstreambuf*
Memo::rdbuf(void) const
{
    return this->m_data.rdbuf();
}




///
///
MemoStream::MemoStream(UnicodeStreamBuf *buf) : std::wistream(0),
                                                m_buf(buf)
{
    this->rdbuf(m_buf);
}



///
///
MemoStream::MemoStream(const MemoStream& m) : std::basic_ios<wchar_t>(),
                                              std::wistream(0),
                                              m_buf()
{
    this->operator=(m);
}


///
///
MemoStream&
MemoStream::operator=(const MemoStream& m)
{
    this->m_buf = m.m_buf;
    this->rdbuf(m_buf);
    return *this;
}




//
//
std::string
MemoStream::narrow_str(const char *charset) const
{
    return String(this->str()).to("UTF-8");
}


//
//
std::wstring
MemoStream::str() const
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




//
//
String
Memo::str(void) const
{
    return this->m_data.str();
}




//
//
UnicodeStreamBuf*
MemoStream::rdbuf(void) const
{
    return this->m_buf;
}


//
//
UnicodeStreamBuf*
MemoStream::rdbuf(UnicodeStreamBuf *buf)
{
    UnicodeStreamBuf* tmp = this->m_buf;
    this->m_buf = buf;
    return tmp;
}



//
//
MemoStream::~MemoStream(void)
{}



//
//
MemoStream
sv_accessor<Memo>::cast(MemoStream*, std::locale loc) const
{
    return MemoStream(this->get_value().rdbuf());
}



//
//
MemoStream
sv_accessor<MemoStream>::cast(MemoStream*, std::locale loc) const
{
    if(this->m_buffer.get())
    {
        this->m_buffer->rdbuf()->pubseekpos(0);
        return MemoStream(this->m_buffer->rdbuf());
    }
    else
        return MemoStream(this->get_value());
}



//
//
String
sv_accessor<MemoStream>::cast(String*, std::locale loc) const
{
    if(!this->m_buffer.get())
    {
        this->m_buffer.reset(new std::wstringstream());
        (*this->m_buffer.get()) << this->get_value().rdbuf();
    }
    this->m_buffer->rdbuf()->pubseekpos(0);
    return this->m_buffer->str();
}



//
//
String
sv_accessor<Memo>::cast(String*, std::locale loc) const
{
    this->get_value().rdbuf()->pubseekpos(0);
    return this->get_value().str();
}


//
//
Memo
sv_accessor<MemoStream>::cast(Memo*, std::locale loc) const
{
    if(!this->m_buffer.get())
    {
        this->m_buffer.reset(new std::wstringstream());
        (*this->m_buffer.get()) << this->get_value().rdbuf();
    }
    this->m_buffer->rdbuf()->pubseekpos(0);
    return Memo(this->m_buffer->rdbuf());
}


//
//
IVariantValue*
MemoStream::do_deepcopy(const IVariantValue *owner) const
{
    return new value_traits<Memo>::stored_type(use_cast<Memo>(owner));
}



DB_NAMESPACE_END


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
