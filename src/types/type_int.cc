//
// type_int.cc - Type: INT (definitions)
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
/// @brief Type: INT (definitions)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/db_fwd.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/variant.hh"
#include "dbwtl/types.hh"
#include "../dal/dal_debug.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <limits>

DB_NAMESPACE_BEGIN


unsigned int         
sv_accessor<signed int>::cast(unsigned int*, std::locale loc) const
{
    const signed int &v = this->get_value();
    if(v < 0)
        throw ex::convert_error(format("Negative number can't converted to unsigned int: %d") % this->get_value());
    else
        return this->get_value();


}

signed char          
sv_accessor<signed int>::cast(signed char*, std::locale loc) const
{
	const signed int &v = this->get_value();
	if(v > std::numeric_limits<signed char>::max() || v < std::numeric_limits<signed char>::min())
        throw ex::convert_error(format("Number can't converted to signed char: %d") % this->get_value());
	else
	    return this->get_value();
}

unsigned char        
sv_accessor<signed int>::cast(unsigned char*, std::locale loc) const
{
        const signed int &v = this->get_value();
        if(v > std::numeric_limits<unsigned char>::max() || v < std::numeric_limits<unsigned char>::min())
            throw ex::convert_error(format("Number can't converted to unsigned char: %d") % this->get_value());
        else
            return this->get_value();
}


signed short         
sv_accessor<signed int>::cast(signed short*, std::locale loc) const
{
        const signed int &v = this->get_value();
        if(v > std::numeric_limits<signed short>::max() || v < std::numeric_limits<signed short>::min())
            throw ex::convert_error(format("Number can't converted to signed short: %d") % this->get_value());
        else
            return this->get_value();
}

unsigned short       
sv_accessor<signed int>::cast(unsigned short*, std::locale loc) const
{
        const signed int &v = this->get_value();
        if(v > std::numeric_limits<unsigned short>::max() || v < std::numeric_limits<unsigned short>::min())
            throw ex::convert_error(format("Number can't converted to unsigned short: %d") % this->get_value());
        else
            return this->get_value();
}

unsigned long long   
sv_accessor<signed int>::cast(unsigned long long*, std::locale loc) const
{
         const signed int &v = this->get_value();
        if(v < 0)
            throw ex::convert_error(format("Number can't converted to unsigned long long: %d") % this->get_value());
        else
            return this->get_value();
 
}



String               
sv_accessor<signed int>::cast(String*, std::locale loc) const
{
	std::wstringstream ss;
	ss.imbue(loc);
	ss << this->get_value();
	return ss.str();
}




String               
sv_accessor<unsigned int>::cast(String*, std::locale loc) const
{
	std::wstringstream ss;
	ss.imbue(loc);
	ss << this->get_value();
	return ss.str();
}



String               
sv_accessor<signed char>::cast(String*, std::locale loc) const
{
	std::wstringstream ss;
	ss.imbue(loc);
	ss << this->get_value();
	return ss.str();
}


String               
sv_accessor<unsigned char>::cast(String*, std::locale loc) const
{
	std::wstringstream ss;
	ss.imbue(loc);
	ss << this->get_value();
	return ss.str();
}






TTimestamp           
sv_accessor<signed int>::cast(TTimestamp*, std::locale loc) const
{
	return TTimestamp(this->get_value());
}




DB_NAMESPACE_END
