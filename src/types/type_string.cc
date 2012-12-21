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
#include "dbwtl/dal/dal_interface.hh"
#include "../dal/dal_debug.hh"

#include "dbwtl/dal/dal_interface.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>

DB_NAMESPACE_BEGIN



/// @todo include value in error message
#define DAL_THROW_INVALID_CAST() throw ex::convert_error("Invalid format")


signed int
    sv_accessor<String>::cast(signed int*, std::locale loc) const
{
    signed int v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


unsigned int
sv_accessor<String>::cast(unsigned int*, std::locale loc) const
{
    unsigned int v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}



bool
sv_accessor<String>::cast(bool*, std::locale loc) const
{
    bool v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


signed short
sv_accessor<String>::cast(signed short*, std::locale loc) const
{
    signed short v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


unsigned short
sv_accessor<String>::cast(unsigned short*, std::locale loc) const
{
    unsigned int v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


signed long long
sv_accessor<String>::cast(signed long long*, std::locale loc) const
{
    signed long long v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


unsigned long long
sv_accessor<String>::cast(unsigned long long*, std::locale loc) const
{
    unsigned long long v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


float
sv_accessor<String>::cast(float*, std::locale loc) const
{
    float v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


double
sv_accessor<String>::cast(double*, std::locale loc) const
{
    double v;
    if(!convertValue(this->get_value(), v)) DAL_THROW_INVALID_CAST();
    else return v;
}


TDate
sv_accessor<String>::cast(TDate*, std::locale loc) const
{
    return TDate(this->get_value());
}

TTime
sv_accessor<String>::cast(TTime*, std::locale loc) const
{
    return TTime(this->get_value());
}

TTimestamp
sv_accessor<String>::cast(TTimestamp*, std::locale loc) const
{
    return TTimestamp(this->get_value());
}


Memo
sv_accessor<String>::cast(Memo*, std::locale loc) const
{
    return Memo(this->get_value());
/*
    std::wstringbuf *buf = new std::wstringbuf();
    buf->str(this->get_value());
    return MemoStream(buf);
*/
}



DB_NAMESPACE_END
