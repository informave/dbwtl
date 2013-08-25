//
// type_varbinary.cc - Type: VARBINARY (definitions)
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
/// @brief Type: VARBINARY (definitions)
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



/// @details
/// 
TVarbinary::TVarbinary(const void *buf, size_t n)
    : m_data()
{
    const uint8_t *p = static_cast<const uint8_t*>(buf);
    if(p && n > 0)
        this->m_data.assign(p, p+n);
    else
        this->m_data.clear();
}


TVarbinary::TVarbinary(const std::initializer_list<unsigned char> &values)
    : m_data(values)
{
}


/// @details
/// 
String
TVarbinary::str(void) const
{
    static const char hexmap[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    };

    if(this->size() == 0)
        return String();
    std::string s;
    s.resize(this->size()*2);

    std::string::iterator i = s.begin();

    std::for_each(this->m_data.begin(), this->m_data.end(), [&](uint8_t v)
                  {
                      *i++ = hexmap[(v >> 4) & 0x0F];
                      *i++ = hexmap[v & 0x0F];
                  });
    return s;
}


/// @details
/// 
size_t
TVarbinary::write(void *buf, size_t size) const
{
    DBWTL_BUGCHECK(buf);
    if(!size || !this->size()) return 0;
    size_t c = this->size() > size ? size : this->size();
    ::memcpy(buf, this->m_data.data(), c);
    return c;
}


/// @details
/// 
size_t
TVarbinary::size(void) const
{
    return this->m_data.size();
}


/// @details
/// 
String
sv_accessor<TVarbinary>::cast(String*, std::locale loc) const
{
    return this->get_value().str();
}



/// @details
/// 
Blob
sv_accessor<TVarbinary>::cast(Blob*, std::locale loc) const
{
	return this->get_value().toBlob();
}


Blob
TVarbinary::toBlob(void) const
{
	return Blob(this->m_data.data(), this->m_data.size());
}


DB_NAMESPACE_END
