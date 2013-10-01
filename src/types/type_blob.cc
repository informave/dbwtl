//
// type_blob.cc - Type: BLOB (definitions)
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
/// @brief Type: BLOB (definitions)
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
Blob::Blob(const Blob& blob) : m_data()
{
    blob.rdbuf()->pubseekpos(0);
    this->m_data << blob.rdbuf();
}


Blob::Blob(const void *ptr, size_t size) : m_data()
{
    assert(ptr);
    this->m_data.rdbuf()->sputn(static_cast<const char*>(ptr), size);
}


//
//
Blob& Blob::operator=(const Blob& blob)
{
    this->m_data.clear();
    assert(blob.rdbuf());
    blob.rdbuf()->pubseekpos(0);
    this->m_data << blob.rdbuf();
    return *this;
}



//
//
Blob::Blob(void) : m_data()
{
}


TVarbinary
Blob::toVarbinary(void)
{
    std::string s(this->m_data.str());
    return TVarbinary(s.data(), s.size());
}


//
//
Blob::Blob(ByteStreamBuf *buf) : m_data()
{
    this->m_data << buf;
}


//
//
Blob::~Blob(void)
{
}


//
//
std::streambuf*
Blob::rdbuf(void) const
{
    
    return this->m_data.rdbuf();
}




///
///
BlobStream::BlobStream(ByteStreamBuf *buf) : std::istream(0), m_buf(buf)
{
    this->rdbuf(buf);
}


///
///
BlobStream::BlobStream(const Variant &variant) : std::istream(0),
                                                 m_buf()
{
    this->operator=(variant.get<BlobStream>());
}


///
///
BlobStream::BlobStream(const BlobStream& b) : std::basic_ios<char>(),
                                              std::istream(0),
                                              m_buf()
{
    this->operator=(b);
}


///
///
BlobStream&
BlobStream::operator=(const BlobStream& b)
{
    this->m_buf = b.m_buf;
    this->rdbuf(m_buf);
    return *this;
}




//
//
ByteStreamBuf*
BlobStream::rdbuf(void) const
{
    return this->m_buf;
}


//
//
ByteStreamBuf*
BlobStream::rdbuf(ByteStreamBuf *buf)
{
    ByteStreamBuf* tmp = this->m_buf;
    this->m_buf = buf;
    return tmp;
}


//
//
BlobStream::~BlobStream(void)
{}





//
//
BlobStream
sv_accessor<Blob>::cast(BlobStream*, std::locale loc) const
{
    return BlobStream(this->get_value().rdbuf());
}


//
//
BlobStream
sv_accessor<BlobStream>::cast(BlobStream*, std::locale loc) const
{
    if(this->m_buffer.get())
    {
        this->m_buffer->rdbuf()->pubseekpos(0);
        return BlobStream(this->m_buffer->rdbuf());
    }
    else
        return BlobStream(this->get_value());
}


//
//
String
sv_accessor<BlobStream>::cast(String*, std::locale loc) const
{
    if(!this->m_buffer.get())
    {
        this->m_buffer.reset(new std::stringstream());
        (*this->m_buffer.get()) << this->get_value().rdbuf();
    }
    this->m_buffer->rdbuf()->pubseekpos(0);
    std::string tmp(this->m_buffer->str());

    return TVarbinary(tmp.c_str(), tmp.size()).str();
}


//
//
String
sv_accessor<Blob>::cast(String*, std::locale loc) const
{
    std::stringstream ss;
    this->get_value().rdbuf()->pubseekpos(0);
    ss << this->get_value().rdbuf();
    std::string tmp(ss.str());

    return TVarbinary(tmp.c_str(), tmp.size()).str();
}



//
//
Blob
sv_accessor<BlobStream>::cast(Blob*, std::locale loc) const
{
    if(!this->m_buffer.get())
    {
        this->m_buffer.reset(new std::stringstream());
        (*this->m_buffer.get()) << this->get_value().rdbuf();
    }
    this->m_buffer->rdbuf()->pubseekpos(0);
    return Blob(this->m_buffer->rdbuf());
}


//
//
IVariantValue*
BlobStream::do_deepcopy(const IVariantValue *owner) const
{
    return new value_traits<Blob>::stored_type(use_cast<Blob>(owner));
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
