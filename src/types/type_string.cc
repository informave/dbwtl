//
// type_string.cc - Type: STRING (definitions)
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
/// @brief Type: STRING (definitions)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/db_fwd.hh"
#include "dbwtl/variant.hh"
#include "dbwtl/types.hh"
#include "dbwtl/db_exceptions.hh"
#include "../dal/dal_debug.hh"

#include "dbwtl/dal/dal_interface.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>

DB_NAMESPACE_BEGIN




#define DAL_THROW_INVALID_CAST() throw std::runtime_error(__FUNCTION__)




signed int
read_accessor<String>::asInt(void) const
{
    signed int v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


unsigned int
read_accessor<String>::asUInt(void) const
{
    unsigned int v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


String
read_accessor<String>::asStr(std::locale loc) const
{
    return this->getValue();
}


bool
read_accessor<String>::asBool(void) const
{
    bool v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


signed short
read_accessor<String>::asSmallint(void) const
{
    signed short v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


unsigned short
read_accessor<String>::asUSmallint(void) const
{
    unsigned int v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


signed long long
read_accessor<String>::asBigint(void) const
{
    signed long long v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


unsigned long long
read_accessor<String>::asUBigint(void) const
{
    unsigned long long v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


float
read_accessor<String>::asReal(void) const
{
    float v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


double
read_accessor<String>::asDouble(void) const
{
    double v;
    if(!dal::convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}

UnicodeStreamBuf*
read_accessor<String>::asMemo(void) const
{
    std::wstringbuf *buf = new std::wstringbuf();
    buf->str(this->getValue());
    return buf;
}






DB_NAMESPACE_END
