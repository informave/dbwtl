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

#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_exceptions.hh"
#include "../dal_debug.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <limits>

DAL_NAMESPACE_BEGIN




signed int           
read_accessor<signed int>::asInt(void) const
{
    return this->getValue();
}

unsigned int         
read_accessor<signed int>::asUInt(void) const
{
        const signed int &v = this->getValue();
        if(v < 0)
                throw std::runtime_error("negative number can not be converted to unsigned int");
        else
            return this->getValue();


}

signed char          
read_accessor<signed int>::asChar(void) const
{
	const signed int &v = this->getValue();
	if(v > std::numeric_limits<signed char>::max() || v < std::numeric_limits<signed char>::min())
		throw std::runtime_error("to big or small");
	else
	    return this->getValue();
}

unsigned char        
read_accessor<signed int>::asUChar(void) const
{
        const signed int &v = this->getValue();
        if(v > std::numeric_limits<unsigned char>::max() || v < std::numeric_limits<unsigned char>::min())
                throw std::runtime_error("to big or small");
        else
            return this->getValue();
}

String               
read_accessor<signed int>::asStr(std::locale loc) const
{
	std::wstringstream ss;
	ss.imbue(loc);
	ss << this->getValue();
	return ss.str();
}

bool                 
read_accessor<signed int>::asBool(void) const
{
	return this->getValue() > 0 ? true : false;
}

signed short         
read_accessor<signed int>::asSmallint(void) const
{
        const signed int &v = this->getValue();
        if(v > std::numeric_limits<signed short>::max() || v < std::numeric_limits<signed short>::min())
                throw std::runtime_error("to big or small");
        else
            return this->getValue();
}

unsigned short       
read_accessor<signed int>::asUSmallint(void) const
{
        const signed int &v = this->getValue();
        if(v > std::numeric_limits<unsigned short>::max() || v < std::numeric_limits<unsigned short>::min())
                throw std::runtime_error("to big or small");
        else
            return this->getValue();
}

signed long long     
read_accessor<signed int>::asBigint(void) const
{
        const signed int &v = this->getValue();
        if(v > std::numeric_limits<signed long long>::max() || v < std::numeric_limits<signed long long>::min())
                throw std::runtime_error("to big or small");
        else
            return this->getValue();
}

unsigned long long   
read_accessor<signed int>::asUBigint(void) const
{
        const signed int &v = this->getValue();
        if(v < 0)
                throw std::runtime_error("to big or small");
        else
            return this->getValue();
}

TNumeric             
read_accessor<signed int>::asNumeric(void) const
{
    return TNumeric(this->getValue());
}

float                
read_accessor<signed int>::asReal(void) const
{
	return this->getValue();
}

double               
read_accessor<signed int>::asDouble(void) const
{
	return this->getValue();
}


TTimestamp           
read_accessor<signed int>::asTimestamp(void) const
{
	return TTimestamp(this->getValue());
}



ByteStreamBuf*       
read_accessor<signed int>::asBlob(void) const
{
	DAL_NOT_IMPL();
    //return this->getValue();
}

UnicodeStreamBuf*    
read_accessor<signed int>::asMemo(void) const
{
	DAL_NOT_IMPL();
    //return this->getValue();
}





DAL_NAMESPACE_END
