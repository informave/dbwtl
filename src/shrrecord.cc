//
// shrrecord.cc - ShrRecord (definition)
//
// Copyright (C)         informave.org
//   2013,               Daniel Vogelbacher <daniel@vogelbacher.name>
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
/// @brief ShrRecord (definitions)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/db_objects.hh"
#include "dbwtl/exceptions.hh"
#include "dal/dal_debug.hh"
#include "utils.hh"

#include <ctime>
 #include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <locale>

#define IS_UNIMPL() assert(!"unimpl"); throw 1

DB_NAMESPACE_BEGIN


ShrRecord::ShrRecord(void)
    : m_data(new ShrRecord::ColumnBuffer())
{
}
    
ShrRecord::ShrRecord(size_t columns)
    : m_data(new ShrRecord::ColumnBuffer(columns))
{
}


ShrRecord::ShrRecord(const ShrRecord& orig)
    : m_data(orig.m_data)
{}

ShrRecord::ShrRecord(const std::initializer_list<Variant> &values)
	: m_data(new ShrRecord::ColumnBuffer(values))
{
}

ShrRecord::ShrRecord(const ShrRecord::ColumnBuffer &buf)
    : m_data(new ShrRecord::ColumnBuffer(buf))
{
}

const Variant&
ShrRecord::operator[](size_t index) const
{
    return this->m_data->at(index);
}
 
Variant&
ShrRecord::operator[](size_t index)
{
    return this->m_data->at(index);
}

/*
const Variant&
Record::operator[](size_t num) const
{
	assert(num > 0);
    return this->m_data->at(num);
}
 
Variant&
Record::operator[](size_t num)
{
	assert(num > 0);
    return this->m_data->at(num);
}
*/

ShrRecord&
ShrRecord::operator=(const ShrRecord& rec)
{
    if(this == &rec) return *this;

	if(this->m_data->size() == 0 || this->m_data->size() == rec.m_data->size())
    {
		this->m_data = rec.m_data;
        return *this;
    }
	else
		throw EngineException(FORMAT2("ShrRecord assignment failed, field count mismatch: %d vs. %d",
			this->m_data->size(), rec.m_data->size()));
}


ShrRecord
ShrRecord::clone(void) const
{
    return ShrRecord(this->getbuf());
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
