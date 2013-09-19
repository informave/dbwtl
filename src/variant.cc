//
// variant.cc - Variant (definition)
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
/// @brief Variant (definitions)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/exceptions.hh"
#include "dal/dal_debug.hh"

#include "dbwtl/dal/active_engines.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <locale>


DB_NAMESPACE_BEGIN


#define DAL_THROW_INVALID_CAST() throw std::runtime_error(__FUNCTION__)



//..............................................................................
//////////////////////////////////////////////////////////////////////// Variant

/// @details
/// 
const char*
Variant::get_typename(void) const
{
    return this->get_storage()->get_typename();
}

/// @details
/// 
void
Variant::replace_storage(IVariantValue* value)
{
    this->m_storage.reset(value);
}


/// @details
/// 
IVariantValue*
Variant::clone(void) const
{
    if(this->m_storage.get())
        return this->get_storage()->clone();
    else
        return 0;
}


/// @details
/// If we have a valid storage object, we ask the storage for the current
/// daltype. If the storage is not set, we use the daltype from the
/// constructor argument.
daltype_t 
Variant::datatype(void) const
{
    if(this->m_storage.get())
        return this->get_storage()->datatype();
    else
        return this->m_type_ifnull;
}

/// @details
/// 
bool
Variant::isnull(void) const
{
    return (this->m_storage.get() == 0 || this->m_storage->isNull());
}


/// @details
/// 
void
Variant::assign(const Variant& value)
{
    if(value.isnull())
    {
        this->m_storage.reset(0);
    }
    else
    {
        this->m_storage.reset(value.get_storage()->deepcopy());
    }
}


/// @details
/// 
void
Variant::setNull(void) { this->m_storage.reset(0); }


/// @details
/// 
IVariantValue*
Variant::get_storage(void)
{
    if(this->m_storage.get() == 0) throw db::NullException(*this);
    return this->m_storage.get();
}


/// @details
/// 
const IVariantValue*
Variant::get_storage(void) const
{
    if(this->m_storage.get() == 0) throw db::NullException(*this);
    return this->m_storage.get();
}





signed int Variant::asInt(void) const
{
    return this->get<signed int>();
}

unsigned int
Variant::asUInt(void) const
{
    return this->get<unsigned int>();
}

signed char
Variant::asChar(void) const
{
    return this->get<signed char>();
}

unsigned char
Variant::asUChar(void) const
{
    return this->get<unsigned char>();
}

String
Variant::asStr(std::locale loc) const
{
    return this->get<String>();
}

bool
Variant::asBool(void) const
{
    return this->get<bool>();
}

signed short
Variant::asSmallint(void) const
{
    return this->get<signed short>();
}

unsigned short
Variant::asUSmallint(void) const
{
    return this->get<unsigned short>();
}

signed long long
Variant::asBigint(void) const
{
    return this->get<signed long long>();
}

unsigned long long
Variant::asUBigint(void) const
{
    return this->get<unsigned long long>();
}

TNumeric    
Variant::asNumeric(void) const
{
    return this->get<TNumeric>();
}

float
Variant::asReal(void) const
{
    return this->get<float>();
}

double
Variant::asDouble(void) const
{
    return this->get<double>();
}

TDate              
Variant::asDate(void) const
{
    return this->get<TDate>();
}

TTime               
Variant::asTime(void) const
{
    return this->get<TTime>();
}

TTimestamp   
Variant::asTimestamp(void) const
{
    return this->get<TTimestamp>();
}

TInterval    
Variant::asInterval(void) const
{
    return this->get<TInterval>();
}

BlobStream
Variant::asBlob(void) const
{
    return this->get<BlobStream>();
}

MemoStream
Variant::asMemo(void) const
{
    return this->get<MemoStream>();
}




//..............................................................................
////////////////////////////////////////////////////////////////////////// TType

/// @details
/// 
bool
TType::isNull(void) const
{
    return this->m_isnull;
}




//--------------------------------------------------------------------------
///
/// Get type of variant as string
String daltype2string(daltype_t type)
{
    switch(type)
    {
    case DAL_TYPE_CUSTOM:     return "DAL_TYPE_CUSTOM";
    case DAL_TYPE_UNKNOWN:    return "DAL_TYPE_UNKNOWN";
    case DAL_TYPE_INT:        return "DAL_TYPE_INT";
    case DAL_TYPE_UINT:       return "DAL_TYPE_UINT";
    case DAL_TYPE_CHAR:       return "DAL_TYPE_CHAR";
    case DAL_TYPE_UCHAR:      return "DAL_TYPE_UCHAR";
    case DAL_TYPE_STRING:     return "DAL_TYPE_STRING";
    case DAL_TYPE_BOOL:       return "DAL_TYPE_BOOL";
    case DAL_TYPE_SMALLINT:   return "DAL_TYPE_SMALLINT";
    case DAL_TYPE_USMALLINT:  return "DAL_TYPE_USMALLINT";
    case DAL_TYPE_BIGINT:     return "DAL_TYPE_BIGINT";
    case DAL_TYPE_UBIGINT:    return "DAL_TYPE_UBIGINT";
    case DAL_TYPE_BLOB:       return "DAL_TYPE_BLOB";
    case DAL_TYPE_MEMO:       return "DAL_TYPE_MEMO";
    case DAL_TYPE_NUMERIC:    return "DAL_TYPE_NUMERIC";
    case DAL_TYPE_FLOAT:      return "DAL_TYPE_FLOAT";
    case DAL_TYPE_DOUBLE:     return "DAL_TYPE_DOUBLE";
    case DAL_TYPE_DATE:       return "DAL_TYPE_DATE";
    case DAL_TYPE_TIME:       return "DAL_TYPE_TIME";
    case DAL_TYPE_TIMESTAMP:  return "DAL_TYPE_TIMESTAMP";
    case DAL_TYPE_INTERVAL:   return "DAL_TYPE_INTERVAL";
    case DAL_TYPE_VARBINARY:  return "DAL_TYPE_VARBINARY";
    }
    DBWTL_BUG_EX("daltype2string(): Given type is not handled");
}


//--------------------------------------------------------------------------
///
/// Get type of variant as string
String daltype2sqlname(daltype_t type)
{
    switch(type)
    {
    case DAL_TYPE_CUSTOM:     return "CUSTOM";
    case DAL_TYPE_UNKNOWN:    return "UNKNOWN";
    case DAL_TYPE_INT:        return "INTEGER";
    case DAL_TYPE_UINT:       return "INTEGER";
    case DAL_TYPE_CHAR:       return "CHAR";
    case DAL_TYPE_UCHAR:      return "CHAR";
    case DAL_TYPE_STRING:     return "VARCHAR";
    case DAL_TYPE_BOOL:       return "BOOL";
    case DAL_TYPE_SMALLINT:   return "SMALLINT";
    case DAL_TYPE_USMALLINT:  return "SMALLINT";
    case DAL_TYPE_BIGINT:     return "BIGINT";
    case DAL_TYPE_UBIGINT:    return "BIGINT";
    case DAL_TYPE_BLOB:       return "BLOB";
    case DAL_TYPE_MEMO:       return "MEMO";
    case DAL_TYPE_NUMERIC:    return "NUMERIC";
    case DAL_TYPE_FLOAT:      return "FLOAT";
    case DAL_TYPE_DOUBLE:     return "DOUBLE";
    case DAL_TYPE_DATE:       return "DATE";
    case DAL_TYPE_TIME:       return "TIME";
    case DAL_TYPE_TIMESTAMP:  return "TIMESTAMP";
    case DAL_TYPE_INTERVAL:   return "INTERVAL";
    }
    DBWTL_BUG_EX("Found BUG: daltype2sqlname(): Given type is not handled");
}







///
/// 
std::wostream&  operator<<(std::wostream& o, const Variant &var)
{
    String str = var.asStr(o.getloc());
    o << str;
    return o;
}

///
///
std::ostream& operator<<(std::ostream& o,  const Variant &var)
{
    String str = var.asStr(o.getloc());
    o << str;
    return o;
}






void throw_read_only(void)
{
    throw ReadonlyException(); /// @todo include variant name for more information
}


void throw_convert_error(daltype_t src, daltype_t dest)
{
    throw ConvertException(src, dest);
}


Variant::Variant(const signed long int &value)
        : m_storage(),
                m_name_ptr(0),
                m_type_ifnull(DAL_TYPE_UNKNOWN)
        {
                this->m_storage.reset(new value_traits<signed int>::stored_type(value));
        }


Variant::Variant(const unsigned long int &value)
        : m_storage(),
                m_name_ptr(0),
                m_type_ifnull(DAL_TYPE_UNKNOWN)
        {
                this->m_storage.reset(new value_traits<unsigned int>::stored_type(value));
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
