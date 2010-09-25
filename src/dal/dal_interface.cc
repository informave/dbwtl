//
// dal_interface.cc - DAL Interface (definitions)
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
/// @brief DAL Interface (definitions)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_exceptions.hh"
#include "dal_debug.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>

DAL_NAMESPACE_BEGIN


/**
 
@mainpage DBWTL - Database Wrapper Template Library


@section sec_intro Introduction

The DBWTL library is a wrapper around database management system APIs. It provides
an uniform interface to all components and supports database specific features.


@section sec_engines Supported engines

 - dal::generic - Generic layer
 - dal::sqlite - SQLite


@section sec_crash Crashcourse building apps with DBWTL
TODO

@subsection sec_crash_link Linking against the DBWTL library

Add the -ldbwtl option to g++


 */


std::string
IVariant::asStr(const std::string &charset, std::locale loc) const
{
    return i18n::conv_to(this->asWideStr(loc), charset.empty() ? "UTF-8" : charset.c_str());
}


void
IVariant::setStr(const std::string &data, const std::string &charset,
                 std::locale loc)
{
    this->setWideStr(i18n::conv_from(data, charset.empty() ? "UTF-8" : charset.c_str()), loc);
}


//--------------------------------------------------------------------------
//
Blob::Blob(std::streambuf *buf) : std::istream(0), m_buf(buf)
{
    this->rdbuf(buf);
}


///
///
Blob::Blob(const IVariant &variant) : std::istream(0), m_buf(variant.asBlob())
{
    this->rdbuf(m_buf);
}


//
//
Blob::~Blob(void)
{}


//--------------------------------------------------------------------------
//
Memo::Memo(std::wstreambuf *buf) : std::wistream(0), m_buf(buf)
{
    this->rdbuf(m_buf);
}

///
///
Memo::Memo(const IVariant &variant) : std::wistream(0), m_buf(variant.asMemo())
{
    this->rdbuf(m_buf);
}




//
//
Memo::~Memo(void)
{}



//
//
std::string
Memo::narrow_str(const char *charset) const
{
    return i18n::conv_to(this->str(), charset);
}


//
//
std::wstring
Memo::str() const
{
    try
    {
        return dynamic_cast<std::wstringbuf&>(*this->m_buf).str();
    }
    catch(std::bad_cast &)
    {
        std::wstringstream ss;
        ss << this->m_buf;
        return ss.str();
    }
}


//--------------------------------------------------------------------------
//
IHandle::IHandle(void)
  : IDALObject()
{}

/*
bool
IHandle::diagAvail(void) const
{
    return false;
}


const IDiagnostic&
IHandle::fetchDiag(void)
{
    throw std::range_error("no diagnostic records available");
}


void
IHandle::setDiagBufferSize(size_t n)
{
    this->m_diagbuffer_max = n;
}
*/


//--------------------------------------------------------------------------
//
/// If the type is unknown, we must return a NULL pointer.
IStoredVariant* new_default_storage(daltype_t type)
{
    switch(type)
    {
    case DAL_TYPE_CUSTOM: return 0;
    case DAL_TYPE_UNKNOWN: return 0;
    case DAL_TYPE_INT: return new var_storage<signed int>();
    case DAL_TYPE_UINT: return new var_storage<unsigned int>();
    case DAL_TYPE_CHAR: return new var_storage<signed char>();
    case DAL_TYPE_UCHAR: return new var_storage<unsigned char>();
    case DAL_TYPE_VARCHAR: return new var_storage<std::wstring>();
    case DAL_TYPE_NVARCHAR: return new var_storage<std::wstring>();
    case DAL_TYPE_NCHAR: return new var_storage<std::wstring>();
    case DAL_TYPE_BOOL: return new var_storage<bool>();
    case DAL_TYPE_SMALLINT: return new var_storage<signed short int>();
    case DAL_TYPE_USMALLINT: return new var_storage<unsigned short int>();
    case DAL_TYPE_BIGINT: return new var_storage<signed long long>();
    case DAL_TYPE_UBIGINT: return new var_storage<unsigned long long>();
    case DAL_TYPE_BLOB: return new var_storage<std::streambuf*>();
    case DAL_TYPE_MEMO: return new var_storage<std::wstreambuf*>();
    case DAL_TYPE_NUMERIC: return new var_storage<TNumeric>();
    case DAL_TYPE_FLOAT: return new var_storage<float>();
    case DAL_TYPE_DOUBLE: return new var_storage<double>();
    case DAL_TYPE_DATE: return new var_storage<TDate>();
    case DAL_TYPE_TIME: return new var_storage<TTime>();
    case DAL_TYPE_TIMESTAMP: return new var_storage<TTimestamp>();
    case DAL_TYPE_INTERVAL: return new var_storage<TInterval>();
    }
    return 0;
}




//--------------------------------------------------------------------------
//
DatatypeBase::DatatypeBase(void)
    : IDatatype(),
      m_name(DAL_TYPE_VARCHAR, L"DatatypeBase::name"),
      m_size(DAL_TYPE_INT, L"DatatypeBase::size"),
      m_literal_prefix(DAL_TYPE_VARCHAR, L"DatatypeBase::literal_prefix"),
      m_literal_suffix(DAL_TYPE_VARCHAR, L"DatatypeBase::literal_suffix"),
      m_is_unsigned(DAL_TYPE_BOOL, L"DatatypeBase::is_unsigned"),
      m_daltype(DAL_TYPE_UNKNOWN)
{}



//
//
DatatypeBase::~DatatypeBase(void)
{}


//
//
const IDatatype::value_type& DatatypeBase::getName(void) const            { return this->m_name; }
const IDatatype::value_type& DatatypeBase::getSize(void) const            { return this->m_size; }
const IDatatype::value_type& DatatypeBase::getLiteralPrefix(void) const   { return this->m_literal_prefix; }
const IDatatype::value_type& DatatypeBase::getLiteralSuffix(void) const   { return this->m_literal_suffix; }
const IDatatype::value_type& DatatypeBase::getIsUnsigned(void) const      { return this->m_is_unsigned; }
daltype_t DatatypeBase::daltype(void) const                               { return this->m_daltype; }






//--------------------------------------------------------------------------
//
ColumnDescBase::ColumnDescBase(void)
    : IColumnDesc(),
      m_name(DAL_TYPE_VARCHAR, L"ColumnDesc::name"),
      m_catalog_name(DAL_TYPE_VARCHAR, L"ColumnDesc::catalog_name"),
      m_schema_name(DAL_TYPE_VARCHAR, L"ColumnDesc::schema_name"),
      m_base_column_name(DAL_TYPE_VARCHAR, L"ColumnDesc::base_column_name"),
      m_type_name(DAL_TYPE_VARCHAR, L"ColumnDesc::type_name"),
      m_base_table_name(DAL_TYPE_VARCHAR, L"ColumnDesc::base_table_name"),
      m_size(DAL_TYPE_INT, L"ColumnDesc::size"),
      m_comment(DAL_TYPE_VARCHAR, L"ColumnDesc::comment"),
      m_is_nullable(DAL_TYPE_BOOL, L"ColumnDesc::is_nullable"),
      m_precision(DAL_TYPE_USMALLINT, L"ColumnDesc::precision"),
      m_scale(DAL_TYPE_USMALLINT, L"ColumnDesc::scale"),
      m_is_searchable(DAL_TYPE_BOOL, L"ColumnDesc::is_searchable"),
      m_daltype(DAL_TYPE_UNKNOWN)
{}


//
//
ColumnDescBase::~ColumnDescBase(void)
{}


const IColumnDesc::value_type& ColumnDescBase::getName(void) const           { return this->m_name; }
const IColumnDesc::value_type& ColumnDescBase::getCatalogName(void) const    { return this->m_catalog_name; }
const IColumnDesc::value_type& ColumnDescBase::getSchemaName(void) const     { return this->m_schema_name; }
const IColumnDesc::value_type& ColumnDescBase::getBaseColumnName(void) const { return this->m_base_column_name; }
const IColumnDesc::value_type& ColumnDescBase::getTypeName(void) const       { return this->m_type_name; }
const IColumnDesc::value_type& ColumnDescBase::getBaseTableName(void) const  { return this->m_base_table_name; }
const IColumnDesc::value_type& ColumnDescBase::getSize(void) const           { return this->m_size; }
const IColumnDesc::value_type& ColumnDescBase::getComment(void) const        { return this->m_comment; }
const IColumnDesc::value_type& ColumnDescBase::getIsNullable(void) const     { return this->m_is_nullable; }
const IColumnDesc::value_type& ColumnDescBase::getPrecision(void) const      { return this->m_precision; }
const IColumnDesc::value_type& ColumnDescBase::getScale(void) const          { return this->m_scale; }
const IColumnDesc::value_type& ColumnDescBase::getIsSearchable(void) const   { return this->m_is_searchable; }

//
//
daltype_t ColumnDescBase::getDatatype(void) const { return this->m_daltype; }





//--------------------------------------------------------------------------
///
/// 
std::wostream&  operator<<(std::wostream& o, const IVariant &var)
{
    o << var.asWideStr(o.getloc());
    return o;
}

///
///
std::ostream& operator<<(std::ostream& o,  const IVariant &var)
{
    o << var.asStr("UTF-8", o.getloc());
    return o;
}







//--------------------------------------------------------------------------
///
/// Get type of variant as string
std::wstring daltype2string(daltype_t type)
{
    switch(type)
    {
    case DAL_TYPE_CUSTOM: return std::wstring(L"DAL_TYPE_CUSTOM");
    case DAL_TYPE_UNKNOWN: return std::wstring(L"DAL_TYPE_UNKNOWN");
    case DAL_TYPE_INT: return std::wstring(L"DAL_TYPE_INT");
    case DAL_TYPE_UINT: return std::wstring(L"DAL_TYPE_UINT");
    case DAL_TYPE_CHAR: return std::wstring(L"DAL_TYPE_CHAR");
    case DAL_TYPE_UCHAR: return std::wstring(L"DAL_TYPE_UCHAR");
    case DAL_TYPE_VARCHAR: return std::wstring(L"DAL_TYPE_VARCHAR");
    case DAL_TYPE_NVARCHAR: return std::wstring(L"DAL_TYPE_NVARCHAR");
    case DAL_TYPE_NCHAR: return std::wstring(L"DAL_TYPE_NCHAR");
    case DAL_TYPE_BOOL: return std::wstring(L"DAL_TYPE_BOOL");
    case DAL_TYPE_SMALLINT: return std::wstring(L"DAL_TYPE_SMALLINT");
    case DAL_TYPE_USMALLINT: return std::wstring(L"DAL_TYPE_USMALLINT");
    case DAL_TYPE_BIGINT: return std::wstring(L"DAL_TYPE_BIGINT");
    case DAL_TYPE_UBIGINT: return std::wstring(L"DAL_TYPE_UBIGINT");
    case DAL_TYPE_BLOB: return std::wstring(L"DAL_TYPE_BLOB");
    case DAL_TYPE_MEMO: return std::wstring(L"DAL_TYPE_MEMO");
    case DAL_TYPE_NUMERIC: return std::wstring(L"DAL_TYPE_NUMERIC");
    case DAL_TYPE_FLOAT: return std::wstring(L"DAL_TYPE_FLOAT");
    case DAL_TYPE_DOUBLE: return std::wstring(L"DAL_TYPE_DOUBLE");
    case DAL_TYPE_DATE: return std::wstring(L"DAL_TYPE_DATE");
    case DAL_TYPE_TIME: return std::wstring(L"DAL_TYPE_TIME");
    case DAL_TYPE_TIMESTAMP: return std::wstring(L"DAL_TYPE_TIMESTAMP");
    case DAL_TYPE_INTERVAL: return std::wstring(L"DAL_TYPE_INTERVAL");
    }
    throw ex::engine_error(L"Found BUG: daltype2string(): Given type is not handled");
}


//--------------------------------------------------------------------------
///
/// Get type of variant as string
std::wstring daltype2sqlname(daltype_t type)
{
    switch(type)
    {
    case DAL_TYPE_CUSTOM: return std::wstring(L"CUSTOM");
    case DAL_TYPE_UNKNOWN: return std::wstring(L"UNKNOWN");
    case DAL_TYPE_INT: return std::wstring(L"INTEGER");
    case DAL_TYPE_UINT: return std::wstring(L"INTEGER");
    case DAL_TYPE_CHAR: return std::wstring(L"CHAR");
    case DAL_TYPE_UCHAR: return std::wstring(L"CHAR");
    case DAL_TYPE_VARCHAR: return std::wstring(L"VARCHAR");
    case DAL_TYPE_NVARCHAR: return std::wstring(L"NVARCHAR");
    case DAL_TYPE_NCHAR: return std::wstring(L"NCHAR");
    case DAL_TYPE_BOOL: return std::wstring(L"BOOL");
    case DAL_TYPE_SMALLINT: return std::wstring(L"SMALLINT");
    case DAL_TYPE_USMALLINT: return std::wstring(L"SMALLINT");
    case DAL_TYPE_BIGINT: return std::wstring(L"BIGINT");
    case DAL_TYPE_UBIGINT: return std::wstring(L"BIGINT");
    case DAL_TYPE_BLOB: return std::wstring(L"BLOB");
    case DAL_TYPE_MEMO: return std::wstring(L"MEMO");
    case DAL_TYPE_NUMERIC: return std::wstring(L"NUMERIC");
    case DAL_TYPE_FLOAT: return std::wstring(L"FLOAT");
    case DAL_TYPE_DOUBLE: return std::wstring(L"DOUBLE");
    case DAL_TYPE_DATE: return std::wstring(L"DATE");
    case DAL_TYPE_TIME: return std::wstring(L"TIME");
    case DAL_TYPE_TIMESTAMP: return std::wstring(L"TIMESTAMP");
    case DAL_TYPE_INTERVAL: return std::wstring(L"INTERVAL");
    }
    throw ex::engine_error(L"Found BUG: daltype2sqlname(): Given type is not handled");
}




//--------------------------------------------------------------------------
///
/// This method assigns the value from one variant to another.
/// If the source value is NULL, the destination value is just set to NULL.
void
IVariant::assign(const IVariant& var)
{
    if(var.isnull())
    {
        this->setNull();
    }
    else
    {
        switch(var.datatype())
        {
        case DAL_TYPE_CUSTOM:     this->setWideStr(var.asWideStr()); break;
        case DAL_TYPE_UNKNOWN:    this->setWideStr(var.asWideStr()); break;
            
        case DAL_TYPE_INT:        this->setInt(var.asInt()); break;
        case DAL_TYPE_UINT:       this->setUInt(var.asUInt()); break;
        case DAL_TYPE_CHAR:       this->setChar(var.asChar()); break;
        case DAL_TYPE_UCHAR:      this->setUChar(var.asUChar()); break;
        case DAL_TYPE_VARCHAR:    this->setWideStr(var.asWideStr()); break;
        case DAL_TYPE_NVARCHAR:   this->setWideStr(var.asWideStr()); break;
        case DAL_TYPE_NCHAR:      this->setWideStr(var.asWideStr()); break;
        case DAL_TYPE_BOOL:       this->setBool(var.asBool()); break;
        case DAL_TYPE_SMALLINT:   this->setSmallint(var.asSmallint()); break;
        case DAL_TYPE_USMALLINT:  this->setUSmallint(var.asUSmallint()); break;
        case DAL_TYPE_BIGINT:     this->setBigint(var.asBigint()); break;
        case DAL_TYPE_UBIGINT:    this->setUBigint(var.asUBigint()); break;
        case DAL_TYPE_BLOB:       this->setBlob(var.asBlob()); break;
        case DAL_TYPE_MEMO:       this->setMemo(var.asMemo()); break; /// @bug setMemo should COPY the content to own stream
        case DAL_TYPE_NUMERIC:    this->setNumeric(var.asNumeric()); break;
        case DAL_TYPE_FLOAT:      this->setReal(var.asReal()); break;
        case DAL_TYPE_DOUBLE:     this->setDouble(var.asDouble()); break;
        case DAL_TYPE_DATE:       this->setDate(var.asDate()); break;
        case DAL_TYPE_TIME:       this->setTime(var.asTime()); break;
        case DAL_TYPE_TIMESTAMP:  this->setTimestamp(var.asTimestamp()); break;
        case DAL_TYPE_INTERVAL:   this->setInterval(var.asInterval()); break;
        }
    }
}



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


///
const std::wstring&
Variant::getName(void) const
{
    return this->m_name;
}


///
signed int
Variant::asInt(void) const
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asInt(); 
}

///
unsigned int
Variant::asUInt(void) const 
{ 
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asUInt(); 
}

///
signed char
Variant::asChar(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asChar(); 
}

///
unsigned char
Variant::asUChar(void) const 
{ 
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asUChar(); 
}

///
std::wstring 
Variant::asWideStr(std::locale loc) const
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asWideStr(loc); 
}


///
bool
Variant::asBool(void) const 
{ 
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asBool(); 
}

///
signed short 
Variant::asSmallint(void) const 
{ 
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asSmallint(); 
}

///
unsigned short  
Variant::asUSmallint(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asUSmallint(); 
}

///
signed long long  
Variant::asBigint(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asBigint(); 
}

///
unsigned long long
Variant::asUBigint(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asUBigint(); 
}

///
TNumeric  
Variant::asNumeric(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asNumeric(); 
}


///
float         
Variant::asReal(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asReal(); 
}

///
double        
Variant::asDouble(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asDouble(); 
}

///
TDate        
Variant::asDate(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asDate(); 
}

///
TTime          
Variant::asTime(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asTime(); 
}

///
TTimestamp
Variant::asTimestamp(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asTimestamp(); 
}

///
//virtual TCustom&        asCustom(void) const = 0;


///
TInterval    
Variant::asInterval(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asInterval(); 
}

///
std::streambuf*
Variant::asBlob(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asBlob(); 
}

///
std::wstreambuf*
Variant::asMemo(void) const 
{
    if(this->isnull()) throw db::ex::null_value(*this);
    return this->getStorageImpl()->asMemo(); 
}


///
/// Initializes a variant storage smart pointer if it is NULL.
/// We first try to get a new StoredVariant with new_default_storage.
/// If the type is unknown, we use the given T type to get a valid
/// storage object.
template<class T>
inline void init_if_null(typename Variant::storage_type &storage, daltype_t type)
{
    IStoredVariant *sv = new_default_storage(type);
   
    if(storage.get() == 0)
        storage.reset(sv ? sv : new var_storage<T>());
}


#define DBWTL_VARIANT_SETTER(postfix, type)                     \
    void                                                        \
    Variant::set##postfix(const type& value)                    \
    {                                                           \
        init_if_null<type>(this->m_storage, this->m_type);      \
        try                                                     \
        {                                                       \
            return this->getStorageImpl()->set##postfix(value); \
        }                                                       \
        catch(ex::read_only &)                                  \
        {                                                       \
            throw ex::read_only(this->getName(), __FUNCTION__); \
        }                                                       \
    }

DBWTL_VARIANT_SETTER(Int, signed int)
DBWTL_VARIANT_SETTER(UInt, unsigned int)
DBWTL_VARIANT_SETTER(Char, signed char)
DBWTL_VARIANT_SETTER(UChar, unsigned char)


//DBWTL_VARIANT_SETTER(WideStr, std::wstring)

void                                                        
Variant::setWideStr(const std::wstring& value, std::locale loc)
{                                                           
    init_if_null<std::wstring>(this->m_storage, this->m_type);     
    try                                                     
    {                                                       
        return this->getStorageImpl()->setWideStr(value, loc); 
    }                                                       
    catch(ex::read_only &)                                  
    {                                                       
        throw ex::read_only(this->getName(), __FUNCTION__); 
    }                                                       
}



DBWTL_VARIANT_SETTER(Bool, bool)
DBWTL_VARIANT_SETTER(Smallint, signed short)
DBWTL_VARIANT_SETTER(USmallint, unsigned short)
DBWTL_VARIANT_SETTER(Bigint, signed long long)
DBWTL_VARIANT_SETTER(UBigint, unsigned long long)
DBWTL_VARIANT_SETTER(Numeric, TNumeric)
DBWTL_VARIANT_SETTER(Real, float)
DBWTL_VARIANT_SETTER(Double, double)
DBWTL_VARIANT_SETTER(Date, TDate)
DBWTL_VARIANT_SETTER(Time, TTime)
DBWTL_VARIANT_SETTER(Timestamp, TTimestamp)
DBWTL_VARIANT_SETTER(Interval, TInterval)


//virtual void        asCustom(void) const = 0;


IVariant::operator signed int            (void) const { return this->asInt(); }
IVariant::operator unsigned int          (void) const { return this->asUInt(); }
IVariant::operator signed char           (void) const { return this->asChar(); }
IVariant::operator unsigned char         (void) const { return this->asUChar(); }
IVariant::operator std::wstring         (void) const { return this->asWideStr(); }
IVariant::operator std::string           (void) const { return this->asStr("UTF-8"); }
IVariant::operator bool                  (void) const { return this->asBool(); }
IVariant::operator signed short          (void) const { return this->asSmallint(); }
IVariant::operator unsigned short        (void) const { return this->asUSmallint(); }
IVariant::operator signed long long      (void) const { return this->asBigint(); }
IVariant::operator unsigned long long    (void) const { return this->asUBigint(); }
IVariant::operator TNumeric              (void) const { return this->asNumeric(); }
IVariant::operator float                 (void) const { return this->asReal(); }
IVariant::operator double                (void) const { return this->asDouble(); }
/*
IVariant::operator TDate                 (void) const { return this->asDate(); }
IVariant::operator TTime                 (void) const { return this->asTime(); }
IVariant::operator TTimestamp            (void) const { return this->asTimestamp(); }
IVariant::operator TInterval             (void) const { return this->asInterval(); }
IVariant::operator std::wstreambuf*       (void) const { return this->asMemo(); }
IVariant::operator std::streambuf*      (void) const { return this->asBlob(); }
*/


///
void      
Variant::setBlob(std::streambuf* value)         
{
    /// @bug is init required?
    ///
    /// I think yes, because the Variant could be uninitialized.
    /// But what type need to be constructed? dal::Blob? Requires new test case!
    //init_if_null<IBlob>(this->m_storage);
    this->m_storage->setBlob(value); 
}


///
void
Variant::setMemo(std::wstreambuf* value)
{
    //init_if_null<IBlob>(this->m_storage);
    this->m_storage->setMemo(value);
}
   


///
daltype_t Variant::datatype(void) const
{
    // if unitialized, we can't determine the datatype
    if(this->m_storage.get() == 0)
        return this->m_type;
    //throw db::ex::null_value(*this);
    return this->m_storage->datatype();
}


///
bool Variant::isnull(void) const
{
    return (this->m_storage.get() == 0 || this->m_storage->isnull());
}


///
void Variant::setNull(void) 
{
    if(this->m_storage.get()) // only set null if storage is initialized!
        this->m_storage->setNull();
}









//--------------------------------------------------------------------------
///
/// @cond DEV_DOCS

/*
struct dal_state_text_t
{
    int code;
    const char *text;
};
/// @endcond

#define ADD_ENTRY(name) {name, #name}

#define DAL_STATE_MAX_CODE_ID 10
dal_state_text_t dal_state_text[] =
{
    {DAL_STATE_MAX_CODE_ID, NULL},
    ADD_ENTRY(DALSTATE_OK),
    ADD_ENTRY(DALSTATE_ERROR),
    ADD_ENTRY(DALSTATE_BAD_CONNECTION),
    ADD_ENTRY(DALSTATE_API_ERR),
    ADD_ENTRY(DALSTATE_DB_NOTFOUND),
    ADD_ENTRY(DALSTATE_AUTH_ERR),
    ADD_ENTRY(DALSTATE_SQL_ERROR),
    ADD_ENTRY(DALSTATE_PARAM_UNKNOW),
    ADD_ENTRY(DALSTATE_EPERM),
    ADD_ENTRY(DALSTATE_LIB_ERROR)
};


#undef ADD_ENTRY


///
///
const char* dal_state_msg(int code)
{
    const char* msg = NULL;
    for(int i = 1; i <= dal_state_text[0].code; ++i)
    {
        if(dal_state_text[i].code != code)
            continue;
        else
        {
            msg = dal_state_text[i].text;
            break;
        }
    }
    assert(msg);
    return msg;
}

*/




//--------------------------------------------------------------------------
daltype_t TDate::datatype(void) const       {  return DAL_TYPE_DATE;       }
daltype_t TTime::datatype(void) const       {  return DAL_TYPE_TIME;       }
daltype_t TInterval::datatype(void) const   {  return DAL_TYPE_INTERVAL;   }
daltype_t TNumeric::datatype(void) const    {  return DAL_TYPE_NUMERIC;    }
daltype_t TTimestamp::datatype(void) const  {  return DAL_TYPE_TIMESTAMP;  }




//--------------------------------------------------------------------------
///
///



std::wstring
TDate::asWideStr(std::locale loc) const
{
    return L"FIXME";
}

std::wstring
TTime::asWideStr(std::locale loc) const
{
    return L"FIXME";
}

std::wstring
TInterval::asWideStr(std::locale loc) const
{
    return L"FIXME";
}


std::wstring
TNumeric::asWideStr(std::locale loc) const
{
    return L"FIXME";
}

std::wstring
TTimestamp::asWideStr(std::locale loc) const
{
    return L"FIXME";
}


//////////////////////







//--------------------------------------------------------------------------
///
///
IResult::~IResult(void)
{ DALTRACE("VISIT"); }


//--------------------------------------------------------------------------
///
///
IStmt::~IStmt(void)
{ DALTRACE("VISIT"); }


//--------------------------------------------------------------------------
///
///
IDbc::~IDbc(void)
{ DALTRACE("VISIT"); }






//--------------------------------------------------------------------------
///
///


//
DiagBase::DiagBase(dalstate_t state,
                   const char *codepos,
                   const char *func,
                   std::wstring message,
                   std::wstring description)
    : IDiagnostic(),
      m_state(state),
      m_sqlstate(DAL_TYPE_VARCHAR, L"SqliteDiag::sqlstate"),
      m_codepos(DAL_TYPE_VARCHAR, L"SqliteDiag::codepos"),
      m_func(DAL_TYPE_VARCHAR, L"SqliteDiag::func"),
      m_message(DAL_TYPE_VARCHAR, L"SqliteDiag::message"),
      m_description(DAL_TYPE_VARCHAR, L"SqliteDiag::description")
      //m_sqlstate_id() // fix?
{
    m_codepos.setStr(codepos, "UTF-8");
    m_func.setStr(func, "UTF-8");
    m_message = message;
    m_description = description;
}


//
DiagBase::DiagBase(const DiagBase& ref)
    : IDiagnostic(),
      m_state(ref.m_state),
      m_sqlstate(ref.m_sqlstate),
      m_codepos(ref.m_codepos),
      m_func(ref.m_func),
      m_message(ref.m_message),
      m_description(ref.m_description)
      //m_sqlstate_id(ref.m_sqlstate_id)
{}




dalstate_t
DiagBase::getState(void) const
{
    return this->m_state;
}

/*
const Variant&
DiagBase::getQuery(void) const
{
    //return this->m_
}

const Variant&
DiagBase::getNativeErrorCode(void) const
{
    //ret
}
*/



const Variant&
DiagBase::getMsg(void) const
{
    return this->m_message;
}

const Variant&
DiagBase::getDescription(void) const
{
    return this->m_description;
}

const Variant&
DiagBase::getSqlstate(void) const
{
    return this->m_sqlstate;
}

const Variant&
DiagBase::getCodepos(void) const
{
    return this->m_codepos;
}


/*
const Variant&
DiagBase::getRowNumber(void) const
{
}


const Variant&
DiagBase::getServerName(void) const
{
}

const Variant&
DiagBase::getColumnNumber(void) const
{
}
*/



//--------------------------------------------------------------------------
///
///
EnvBase::EnvBase(void)
  : IEnv(),
    m_options()
{ 
    DAL_ADD_OPTION("env_library_path", DAL_TYPE_VARCHAR);
    DAL_ADD_OPTION("env_diag_maxsize", DAL_TYPE_UINT);
    
    this->m_options["env_diag_maxsize"].setUInt(50);
}


///
///
void
EnvBase::setOption(std::string name, Variant data)
{
    options_type::iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        i->second = data;
    }
    else
    {
        throw std::exception();
    }
}

///
///
const Variant&
EnvBase::getOption(std::string name) const
{
    options_type::const_iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        return i->second;
    }
    else
    {
        throw std::exception();
    }
}






//--------------------------------------------------------------------------
///
///
StmtBase::StmtBase(void)
  : IStmt(),
    m_params(),
    m_temp_params(),
    m_isPrepared(false),
    m_isBad(false),
    m_options()
{ 
    DAL_ADD_OPTION("env_diag_maxsize", DAL_TYPE_UINT);
    
    this->m_options["env_diag_maxsize"].setUInt(50);
}



///
///
void
StmtBase::setOption(std::string name, Variant data)
{
    options_type::iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        i->second = data;
    }
    else
    {
        throw std::exception();
    }
}


///
///
const Variant&
StmtBase::getOption(std::string name) const
{
    options_type::const_iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        return i->second;
    }
    else
    {
        throw std::exception();
    }
}



/// Support for binding POD types via an implicit constructor
/// call.
void 
StmtBase::bind(int num, Variant data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(data);
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
    //this->bind(num, tmp);
}



///
///
void
StmtBase::bind(int num, IVariant* data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(data);
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}


///
///
void
StmtBase::bind(int num, const IVariant* data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(data);
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}


///
///
void
StmtBase::bind(int num, std::streambuf *data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(data);
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}

void
StmtBase::bind(int num, std::wstreambuf *data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(data);
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}


///
///
void
StmtBase::close(void)
{
    this->m_isPrepared = false;
    this->m_isBad = false; 

    this->m_params.clear(); // better use smartobjects?
    for_each(this->m_temp_params.begin(),
             this->m_temp_params.end(),
             delete_object());

    this->m_temp_params.clear();
}


///
///
bool
StmtBase::isPrepared(void) const
{
    return this->m_isPrepared;
}


///
///
bool
StmtBase::isBad(void) const
{
    return this->m_isBad;
}



//--------------------------------------------------------------------------
///
///
bool
ResultBase::isPrepared(void) const
{
    return this->m_isPrepared;
}


///
///
bool
ResultBase::isBad(void) const
{
    return this->m_isBad;
}


///
///
bool
ResultBase::isOpen(void) const
{
    return this->m_isOpen;
}


//--------------------------------------------------------------------------
///
///
DbcBase::DbcBase(void)
  : IDbc(),
    m_isConnected(false),
    m_isBad(false),
    m_options()
{ 
    DAL_ADD_OPTION("env_diag_maxsize", DAL_TYPE_UINT);
    
    this->m_options["env_diag_maxsize"].setUInt(50);
}


///
///
void
DbcBase::setOption(std::string name, Variant data)
{
    options_type::iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        i->second = data;
    }
    else
    {
        throw std::exception();
    }
}


///
///
const Variant&
DbcBase::getOption(std::string name) const
{
    options_type::const_iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        return i->second;
    }
    else
    {
        throw std::exception();
    }
}


///
///
bool
DbcBase::isBad(void) const
{
    return this->m_isBad;
}


///
///
bool
DbcBase::isConnected(void) const
{
    return this->m_isConnected;
}




//--------------------------------------------------------------------------
///
///
#if defined(DBWTL_ON_UNIX) && (defined(DBWTL_DEBUG_MESSAGES) || defined(DBWTL_DEBUG_TRACE))
const char* RED="\e[1;31m";
const char* blue="\e[0;34m";
const char* NC="\e[0m";
#else
const char* RED="";
const char* blue="";
const char* NC="";
#endif


///
///
void
dal_trace(const char* func, const char* file, unsigned int line, const char* s)
{
#ifdef DBWTL_DEBUG_TRACE
    timespec x = { 0, 100000000/6};
    //nanosleep(&x, NULL);
    std::cerr << blue << "[TRACE] "
              << NC << s << " (" << func << "() in " << file << ":" << line << ")" << std::endl;
#endif
}

///
///
void
dal_debug(const char* func, const char* file, unsigned int line, const char* s)
{
#ifdef DBWTL_DEBUG_MESSAGES
    std::cerr << RED << "[DEBUG] "
              << NC << s << " (" << func << "() in " << file << ":" << line << ")" << std::endl;
#endif
}



DAL_NAMESPACE_END
