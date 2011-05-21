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


#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_exceptions.hh"
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


Variant::Variant(const dal::EngineVariant& var) : m_storage()
{
    if(!var.isnull())
        //m_storage.reset(var.getStorageImpl()->clone()); // deep copy? no..
        m_storage.reset(var.deepcopy()); // deep copy? no..
        //this->m_storage.reset(var.getStorageImpl()->new_deepcopy());

    
}

/*
        Variant::Variant(const SqliteVariant& var)
        {
                if(!var.isnull())
                        this->m_storage.reset(var.getStorageImpl()->new_deepcopy());
        }
*/


bool DefaultVariant::isnull(void) const
{
    return this->m_isnull;
}


void DefaultVariant::setNull(bool mode)
{
    this->m_isnull = mode;
}





signed int      DefaultVariant::asInt(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_INT); }


unsigned int    DefaultVariant::asUInt(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_UINT); }


signed char     DefaultVariant::asChar(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_CHAR); }


unsigned char   DefaultVariant::asUChar(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_UCHAR); }


String   DefaultVariant::asStr(std::locale loc) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_STRING); }



bool            DefaultVariant::asBool(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_BOOL); }


signed short    DefaultVariant::asSmallint(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_SMALLINT); }


unsigned short  DefaultVariant::asUSmallint(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_USMALLINT); }


signed long long    DefaultVariant::asBigint(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_BIGINT); }


unsigned long long  DefaultVariant::asUBigint(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_UBIGINT); }




TNumeric        DefaultVariant::asNumeric(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_NUMERIC); }


float           DefaultVariant::asReal(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_FLOAT); }


double          DefaultVariant::asDouble(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_DOUBLE); }


TDate           DefaultVariant::asDate(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_DATE); }


TTime           DefaultVariant::asTime(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_TIME); }



TTimestamp      DefaultVariant::asTimestamp(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_TIMESTAMP); }


TInterval       DefaultVariant::asInterval(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_INTERVAL); }



ByteStreamBuf*         DefaultVariant::asBlob(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_BLOB); }


UnicodeStreamBuf*         DefaultVariant::asMemo(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_MEMO); }




//--------------------------------------------------------------------------
///
///
IStoredVariant*
Variant::getStorageImpl(void)
{
    /// do not use this->isnull() because it checks the m_storage, too!
    if(this->m_storage.get() == 0) throw db::ex::null_value(*this);
    return this->m_storage.get();
}



///
///
const IStoredVariant*
Variant::getStorageImpl(void) const
{
    if(this->m_storage.get() == 0) throw db::ex::null_value(*this);
    return this->m_storage.get();
}


IStoredVariant*
Variant::getStoredVar(void)
{
    if(this->getStorageImpl()->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl();
}



///
///
const IStoredVariant*
Variant::getStoredVar(void) const
{
    if(this->getStorageImpl()->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl();
}



///
const String&
Variant::getName(void) const
{
    return this->m_name;
}



//template<> int Variant::get<int>(void) const { return this->getStoredVar()->asInt(); }


TNumeric::TNumeric(std::string value)
    : m_value(0),
      m_frac(0)
{
}

TNumeric::TNumeric(void)
    : m_value(0),
      m_frac(0)
{
}

TNumeric::TNumeric(long long num, long long frac)
    : m_value(num),
      m_frac(frac)
{
/*
    std::stringstream ss;
    ss.imbue(std::locale("C"));
    ss << num
       << std::use_facet<std::numpunct<std::stringstream::char_type> >(std::locale("C")).decimal_point()
       << std::abs(frac);
    m_value = ss.str();

    std::cout << m_value << std::endl;
*/
}



///
signed int
Variant::asInt(void) const
{
    return this->getStoredVar()->asInt(); 
}

///
unsigned int
Variant::asUInt(void) const 
{ 
    return this->getStoredVar()->asUInt(); 
}

///
signed char
Variant::asChar(void) const 
{
    return this->getStoredVar()->asChar(); 
}

///
unsigned char
Variant::asUChar(void) const 
{ 
    return this->getStoredVar()->asUChar(); 
}

///
String 
Variant::asStr(std::locale loc) const
{
    return this->getStoredVar()->asStr(loc); 
}


///
bool
Variant::asBool(void) const 
{ 
    return this->getStoredVar()->asBool(); 
}

///
signed short 
Variant::asSmallint(void) const 
{ 
    return this->getStoredVar()->asSmallint(); 
}

///
unsigned short  
Variant::asUSmallint(void) const 
{
    return this->getStoredVar()->asUSmallint(); 
}

///
signed long long  
Variant::asBigint(void) const 
{
    return this->getStoredVar()->asBigint(); 
}

///
unsigned long long
Variant::asUBigint(void) const 
{
    return this->getStoredVar()->asUBigint(); 
}

///
TNumeric  
Variant::asNumeric(void) const 
{
    return this->getStoredVar()->asNumeric(); 
}


///
float         
Variant::asReal(void) const 
{
    return this->getStoredVar()->asReal(); 
}

///
double        
Variant::asDouble(void) const 
{
    return this->getStoredVar()->asDouble(); 
}

///
TDate        
Variant::asDate(void) const 
{
    return this->getStoredVar()->asDate(); 
}

///
TTime          
Variant::asTime(void) const 
{
    return this->getStoredVar()->asTime(); 
}

///
TTimestamp
Variant::asTimestamp(void) const 
{
    return this->getStoredVar()->asTimestamp(); 
}

///
//virtual TCustom&        asCustom(void) const = 0;


///
TInterval    
Variant::asInterval(void) const 
{
    return this->getStoredVar()->asInterval(); 
}

///
ByteStreamBuf*
Variant::asBlob(void) const 
{
    return this->getStoredVar()->asBlob(); 
}

///
UnicodeStreamBuf*
Variant::asMemo(void) const 
{
    return this->getStoredVar()->asMemo(); 
}











// OPERATORS
IVariant::operator bool                  (void) const { return this->asBool(); }
IVariant::operator signed int            (void) const { return this->asInt(); }
IVariant::operator unsigned int          (void) const { return this->asUInt(); }
IVariant::operator signed char           (void) const { return this->asChar(); }
IVariant::operator unsigned char         (void) const { return this->asUChar(); }
IVariant::operator signed short          (void) const { return this->asSmallint(); }
IVariant::operator unsigned short        (void) const { return this->asUSmallint(); }
IVariant::operator signed long long      (void) const { return this->asBigint(); }
IVariant::operator unsigned long long    (void) const { return this->asUBigint(); }
IVariant::operator float                 (void) const { return this->asReal(); }
IVariant::operator double                (void) const { return this->asDouble(); }
IVariant::operator String                (void) const { return this->asStr(); }
IVariant::operator std::string           (void) const { return std::string(this->asStr().to("UTF-8")); }
IVariant::operator ByteStreamBuf*        (void) const { return this->asBlob(); }
IVariant::operator UnicodeStreamBuf*     (void) const { return this->asMemo(); }

// IVariant::operator TNumeric              (void) const { return this->asNumeric(); }
// IVariant::operator TDate                 (void) const { return this->asDate(); }
// IVariant::operator TTime                 (void) const { return this->asTime(); }
// IVariant::operator TTimestamp            (void) const { return this->asTimestamp(); }
// IVariant::operator TInterval             (void) const { return this->asInterval(); }


///
daltype_t Variant::datatype(void) const
{
    // if unitialized, we can't determine the datatype
    if(this->m_storage.get() == 0)
        return DAL_TYPE_UNKNOWN;
    //throw db::ex::null_value(*this);
    return this->m_storage->datatype();
}


///
bool Variant::isnull(void) const
{
    return (this->m_storage.get() == 0 || this->m_storage->isnull());
}


/// If the internal storage points to an external value, it may
/// be required that this value is notified about the setNull() action.
void Variant::setNull(bool mode) 
{
    if(this->m_storage.get()) // only set null if storage is initialized!
    {
        this->m_storage->setNull(mode);
        this->m_storage.reset();
    }
}




//--------------------------------------------------------------------------
daltype_t TDate::datatype(void) const       {  return DAL_TYPE_DATE;       }
daltype_t TTime::datatype(void) const       {  return DAL_TYPE_TIME;       }
daltype_t TInterval::datatype(void) const   {  return DAL_TYPE_INTERVAL;   }
daltype_t TNumeric::datatype(void) const    {  return DAL_TYPE_NUMERIC;    }
daltype_t TTimestamp::datatype(void) const  {  return DAL_TYPE_TIMESTAMP;  }




//--------------------------------------------------------------------------
///
///



String
TDate::asStr(std::locale loc) const
{
    DAL_NOT_IMPL();
}

String
TTime::asStr(std::locale loc) const
{
    DAL_NOT_IMPL();
}

String
TInterval::asStr(std::locale loc) const
{
    DAL_NOT_IMPL();
}


String
TNumeric::asStr(std::locale loc) const
{
    DAL_NOT_IMPL();
}

String
TTimestamp::asStr(std::locale loc) const
{
    DAL_NOT_IMPL();
}


//////////////////////



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
    }
    throw ex::engine_error("Found BUG: daltype2string(): Given type is not handled");
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
    throw ex::engine_error("Found BUG: daltype2sqlname(): Given type is not handled");
}






//--------------------------------------------------------------------------
///
/// 
std::wostream&  operator<<(std::wostream& o, const IVariant &var)
{
    String str = var.asStr(o.getloc());
    o << str;
    return o;
}

///
///
std::ostream& operator<<(std::ostream& o,  const IVariant &var)
{
    String str = var.asStr(o.getloc());
    o << str;
    return o;
}






void throw_read_only(void)
{
    throw ex::read_only();
}




DB_NAMESPACE_END

