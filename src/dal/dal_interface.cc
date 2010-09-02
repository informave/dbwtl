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




//--------------------------------------------------------------------------
//
Blob::Blob(std::streambuf *buf) : std::istream(), m_buf(buf)
{
    this->rdbuf(buf);
}


//
//
Blob::~Blob(void)
{}


//--------------------------------------------------------------------------
//
Memo::Memo(std::wstreambuf *buf) : std::wistream(), m_buf(buf)
{
    this->rdbuf(buf);
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
    case DAL_TYPE_VARCHAR: return new var_storage<i18n::UString>();
    case DAL_TYPE_NVARCHAR: return new var_storage<i18n::UString>();
    case DAL_TYPE_NCHAR: return new var_storage<i18n::UString>();
    case DAL_TYPE_BOOL: return new var_storage<bool>();
    case DAL_TYPE_SMALLINT: return new var_storage<signed short int>();
    case DAL_TYPE_USMALLINT: return new var_storage<unsigned short int>();
    case DAL_TYPE_BIGINT: return new var_storage<signed long long>();
    case DAL_TYPE_UBIGINT: return new var_storage<unsigned long long>();
    case DAL_TYPE_BLOB: return new var_storage<std::streambuf*>();
    case DAL_TYPE_MEMO: return new var_storage<std::wstreambuf*>();
    case DAL_TYPE_NUMERIC: return new var_storage<TNumeric>();
    case DAL_TYPE_DECIMAL: return new var_storage<TDecimal>();
    case DAL_TYPE_MONEY: return new var_storage<TMoney>();
    case DAL_TYPE_FLOAT: return new var_storage<float>();
    case DAL_TYPE_DOUBLE: return new var_storage<double>();
    case DAL_TYPE_DATE: return new var_storage<TDate>();
    case DAL_TYPE_TIME: return new var_storage<TTime>();
    case DAL_TYPE_DATETIME: return new var_storage<TDatetime>();
    case DAL_TYPE_TIMESTAMP: return new var_storage<TTimestamp>();
    case DAL_TYPE_CIDR: return new var_storage<TCidr>();
    case DAL_TYPE_INTERVAL: return new var_storage<TInterval>();
    case DAL_TYPE_MACADDR: return new var_storage<TMacaddr>();
    case DAL_TYPE_INETADDR: return new var_storage<TInetaddr>();
    case DAL_TYPE_UUID: return new var_storage<TUuid>();
    case DAL_TYPE_XML: return new var_storage<TXml>();
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
    o << var.asStr(o.getloc());
    return o;
}

///
///
std::ostream& operator<<(std::ostream& o,  const IVariant &var)
{
    o << var.asNarrowStr("UTF-8", o.getloc());
    return o;
}







//--------------------------------------------------------------------------
///
/// Get type of variant as string
i18n::UString daltype2string(daltype_t type)
{
    switch(type)
    {
    case DAL_TYPE_CUSTOM: return i18n::UString(L"DAL_TYPE_CUSTOM");
    case DAL_TYPE_UNKNOWN: return i18n::UString(L"DAL_TYPE_UNKNOWN");
    case DAL_TYPE_INT: return i18n::UString(L"DAL_TYPE_INT");
    case DAL_TYPE_UINT: return i18n::UString(L"DAL_TYPE_UINT");
    case DAL_TYPE_CHAR: return i18n::UString(L"DAL_TYPE_CHAR");
    case DAL_TYPE_UCHAR: return i18n::UString(L"DAL_TYPE_UCHAR");
    case DAL_TYPE_VARCHAR: return i18n::UString(L"DAL_TYPE_VARCHAR");
    case DAL_TYPE_NVARCHAR: return i18n::UString(L"DAL_TYPE_NVARCHAR");
    case DAL_TYPE_NCHAR: return i18n::UString(L"DAL_TYPE_NCHAR");
    case DAL_TYPE_BOOL: return i18n::UString(L"DAL_TYPE_BOOL");
    case DAL_TYPE_SMALLINT: return i18n::UString(L"DAL_TYPE_SMALLINT");
    case DAL_TYPE_USMALLINT: return i18n::UString(L"DAL_TYPE_USMALLINT");
    case DAL_TYPE_BIGINT: return i18n::UString(L"DAL_TYPE_BIGINT");
    case DAL_TYPE_UBIGINT: return i18n::UString(L"DAL_TYPE_UBIGINT");
    case DAL_TYPE_BLOB: return i18n::UString(L"DAL_TYPE_BLOB");
    case DAL_TYPE_MEMO: return i18n::UString(L"DAL_TYPE_MEMO");
    case DAL_TYPE_NUMERIC: return i18n::UString(L"DAL_TYPE_NUMERIC");
    case DAL_TYPE_DECIMAL: return i18n::UString(L"DAL_TYPE_DECIMAL");
    case DAL_TYPE_MONEY: return i18n::UString(L"DAL_TYPE_MONEY");
    case DAL_TYPE_FLOAT: return i18n::UString(L"DAL_TYPE_FLOAT");
    case DAL_TYPE_DOUBLE: return i18n::UString(L"DAL_TYPE_DOUBLE");
    case DAL_TYPE_DATE: return i18n::UString(L"DAL_TYPE_DATE");
    case DAL_TYPE_TIME: return i18n::UString(L"DAL_TYPE_TIME");
    case DAL_TYPE_DATETIME: return i18n::UString(L"DAL_TYPE_DATETIME");
    case DAL_TYPE_TIMESTAMP: return i18n::UString(L"DAL_TYPE_TIMESTAMP");
    case DAL_TYPE_CIDR: return i18n::UString(L"DAL_TYPE_CIDR");
    case DAL_TYPE_INTERVAL: return i18n::UString(L"DAL_TYPE_INTERVAL");
    case DAL_TYPE_MACADDR: return i18n::UString(L"DAL_TYPE_MACADDR");
    case DAL_TYPE_INETADDR: return i18n::UString(L"DAL_TYPE_INETADDR");
    case DAL_TYPE_UUID: return i18n::UString(L"DAL_TYPE_UUID");
    case DAL_TYPE_XML: return i18n::UString(L"DAL_TYPE_XML");
    }
    return L"<UNKNOWN_TYPE_ID>"; /// @todo throw exception
}


//--------------------------------------------------------------------------
///
/// Get type of variant as string
i18n::UString daltype2sqlname(daltype_t type)
{
    switch(type)
    {
    case DAL_TYPE_CUSTOM: return L"<CUSTOM_TYPE_ID>"; /// @todo throw exception
    case DAL_TYPE_UNKNOWN: return L"<UNKNOWN_TYPE_ID>"; /// @todo throw exception
    case DAL_TYPE_INT: return i18n::UString(L"INTEGER");
    case DAL_TYPE_UINT: return i18n::UString(L"INTEGER");
    case DAL_TYPE_CHAR: return i18n::UString(L"CHAR");
    case DAL_TYPE_UCHAR: return i18n::UString(L"CHAR");
    case DAL_TYPE_VARCHAR: return i18n::UString(L"VARCHAR");
    case DAL_TYPE_NVARCHAR: return i18n::UString(L"NVARCHAR");
    case DAL_TYPE_NCHAR: return i18n::UString(L"NCHAR");
    case DAL_TYPE_BOOL: return i18n::UString(L"BOOL");
    case DAL_TYPE_SMALLINT: return i18n::UString(L"SMALLINT");
    case DAL_TYPE_USMALLINT: return i18n::UString(L"SMALLINT");
    case DAL_TYPE_BIGINT: return i18n::UString(L"BIGINT");
    case DAL_TYPE_UBIGINT: return i18n::UString(L"BIGINT");
    case DAL_TYPE_BLOB: return i18n::UString(L"BLOB");
    case DAL_TYPE_MEMO: return i18n::UString(L"MEMO");
    case DAL_TYPE_NUMERIC: return i18n::UString(L"NUMERIC");
    case DAL_TYPE_DECIMAL: return i18n::UString(L"DECIMAL");
    case DAL_TYPE_MONEY: return i18n::UString(L"MONEY");
    case DAL_TYPE_FLOAT: return i18n::UString(L"FLOAT");
    case DAL_TYPE_DOUBLE: return i18n::UString(L"DOUBLE");
    case DAL_TYPE_DATE: return i18n::UString(L"DATE");
    case DAL_TYPE_TIME: return i18n::UString(L"TIME");
    case DAL_TYPE_DATETIME: return i18n::UString(L"DATETIME");
    case DAL_TYPE_TIMESTAMP: return i18n::UString(L"TIMESTAMP");
    case DAL_TYPE_CIDR: return i18n::UString(L"CIDR");
    case DAL_TYPE_INTERVAL: return i18n::UString(L"INTERVAL");
    case DAL_TYPE_MACADDR: return i18n::UString(L"MACADDR");
    case DAL_TYPE_INETADDR: return i18n::UString(L"INETADDR");
    case DAL_TYPE_UUID: return i18n::UString(L"UUID");
    case DAL_TYPE_XML: return i18n::UString(L"XML");
    }
    return L"<UNKNOWN_TYPE_ID>"; /// @todo throw exception
}




//--------------------------------------------------------------------------
///
///
void
IVariant::assign(const IVariant& var)
{
    switch(var.datatype())
    {
    case DAL_TYPE_CUSTOM:     this->setStr(var.asStr()); break;
    case DAL_TYPE_UNKNOWN:    this->setStr(var.asStr()); break;

    case DAL_TYPE_INT:        this->setInt(var.asInt()); break;
    case DAL_TYPE_UINT:       this->setUInt(var.asUInt()); break;
    case DAL_TYPE_CHAR:       this->setChar(var.asChar()); break;
    case DAL_TYPE_UCHAR:      this->setUChar(var.asUChar()); break;
    case DAL_TYPE_VARCHAR:    this->setStr(var.asStr()); break;
    case DAL_TYPE_NVARCHAR:   this->setStr(var.asStr()); break;
    case DAL_TYPE_NCHAR:      this->setStr(var.asStr()); break;
    case DAL_TYPE_BOOL:       this->setBool(var.asBool()); break;
    case DAL_TYPE_SMALLINT:   this->setSmallint(var.asSmallint()); break;
    case DAL_TYPE_USMALLINT:  this->setUSmallint(var.asUSmallint()); break;
    case DAL_TYPE_BIGINT:     this->setBigint(var.asBigint()); break;
    case DAL_TYPE_UBIGINT:    this->setUBigint(var.asUBigint()); break;
    case DAL_TYPE_BLOB:       this->setBlob(var.asBlob()); break;
    case DAL_TYPE_MEMO:       this->setMemo(var.asMemo()); break; /// @bug setMemo should COPY the content to own stream
    case DAL_TYPE_NUMERIC:    this->setNumeric(var.asNumeric()); break;
    case DAL_TYPE_DECIMAL:    this->setNumeric(var.asNumeric()); break; /// @todo ok?
    case DAL_TYPE_MONEY:      this->setMoney(var.asMoney()); break;
    case DAL_TYPE_FLOAT:      this->setReal(var.asReal()); break;
    case DAL_TYPE_DOUBLE:     this->setDouble(var.asDouble()); break;
    case DAL_TYPE_DATE:       this->setDate(var.asDate()); break;
    case DAL_TYPE_TIME:       this->setTime(var.asTime()); break;
    case DAL_TYPE_DATETIME:   this->setDatetime(var.asDatetime()); break;
    case DAL_TYPE_TIMESTAMP:  this->setTimestamp(var.asTimestamp()); break;
    case DAL_TYPE_CIDR:       this->setCIDR(var.asCIDR()); break;
    case DAL_TYPE_INTERVAL:   this->setInterval(var.asInterval()); break;
    case DAL_TYPE_MACADDR:    this->setMacaddr(var.asMacaddr()); break;
    case DAL_TYPE_INETADDR:   this->setInetaddr(var.asInetaddr()); break;
    case DAL_TYPE_UUID:       this->setUUID(var.asUUID()); break;
    case DAL_TYPE_XML:        this->setXML(var.asXML()); break;
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
const i18n::UString&
Variant::getName(void) const
{
    return this->m_name;
}


///
signed int
Variant::asInt(void) const {  return this->getStorageImpl()->asInt(); }

///
unsigned int
Variant::asUInt(void) const { return this->getStorageImpl()->asUInt(); }

///
signed char
Variant::asChar(void) const { return this->getStorageImpl()->asChar(); }

///
unsigned char
Variant::asUChar(void) const { return this->getStorageImpl()->asUChar(); }

///
i18n::UString 
Variant::asStr(void) const { return this->getStorageImpl()->asStr(); }

///
i18n::UString
Variant::asStr(std::locale loc) const { return this->getStorageImpl()->asStr(loc); }

///
std::string
Variant::asNarrowStr(const char *charset) const { return this->getStorageImpl()->asNarrowStr(charset); }

///
std::string
Variant::asNarrowStr(const char *charset, std::locale loc) const
{ return this->getStorageImpl()->asNarrowStr(charset, loc); }

///
bool
Variant::asBool(void) const { return this->getStorageImpl()->asBool(); }

///
signed short 
Variant::asSmallint(void) const { return this->getStorageImpl()->asSmallint(); }

///
unsigned short  
Variant::asUSmallint(void) const { return this->getStorageImpl()->asUSmallint(); }

///
signed long long  
Variant::asBigint(void) const { return this->getStorageImpl()->asBigint(); }

///
unsigned long long
Variant::asUBigint(void) const { return this->getStorageImpl()->asUBigint(); }

///
TNumeric  
Variant::asNumeric(void) const { return this->getStorageImpl()->asNumeric(); }

///
TMoney   
Variant::asMoney(void) const { return this->getStorageImpl()->asMoney(); }

///
float         
Variant::asReal(void) const { return this->getStorageImpl()->asReal(); }

///
double        
Variant::asDouble(void) const { return this->getStorageImpl()->asDouble(); }

///
TDate        
Variant::asDate(void) const { return this->getStorageImpl()->asDate(); }

///
TTime          
Variant::asTime(void) const { return this->getStorageImpl()->asTime(); }

///
TDatetime      
Variant::asDatetime(void) const { return this->getStorageImpl()->asDatetime(); }

///
signed int     
Variant::asTimestamp(void) const { return this->getStorageImpl()->asTimestamp(); }

///
//virtual TCustom&        asCustom(void) const = 0;

///
TCidr        
Variant::asCIDR(void) const { return this->getStorageImpl()->asCIDR(); }

///
TInterval    
Variant::asInterval(void) const { return this->getStorageImpl()->asInterval(); }

///
TMacaddr    
Variant::asMacaddr(void) const { return this->getStorageImpl()->asMacaddr(); }

///
TInetaddr    
Variant::asInetaddr(void) const { return this->getStorageImpl()->asInetaddr(); }

///
TUuid      
Variant::asUUID(void) const { return this->getStorageImpl()->asUUID(); }

///
TXml     
Variant::asXML(void) const { return this->getStorageImpl()->asXML(); }

///
std::streambuf*
Variant::asBlob(void) const { return this->getStorageImpl()->asBlob(); }

///
std::wstreambuf*
Variant::asMemo(void) const { return this->getStorageImpl()->asMemo(); }


///
template<class T>
inline void init_if_null(typename Variant::storage_type &storage)
{
    if(storage.get() == 0)
        storage.reset(new var_storage<T>());
}


#define DBWTL_VARIANT_SETTER(postfix, type)                     \
    void                                                        \
    Variant::set##postfix(const type& value)                    \
    {                                                           \
        init_if_null<type>(this->m_storage);                    \
        try                                                     \
        {                                                       \
            return this->getStorageImpl()->set##postfix(value); \
        }                                                       \
        catch(ex::read_only &e)                                 \
        {                                                       \
            throw ex::read_only(this->getName(), __FUNCTION__); \
        }                                                       \
    }

DBWTL_VARIANT_SETTER(Int, signed int)
DBWTL_VARIANT_SETTER(UInt, unsigned int)
DBWTL_VARIANT_SETTER(Char, signed char)
DBWTL_VARIANT_SETTER(UChar, unsigned char)
DBWTL_VARIANT_SETTER(Str, i18n::UString)


///
void     
Variant::setStr(const char* data, std::size_t len, const char* charset) 
{
    init_if_null<i18n::UString>(this->m_storage);
    try
    {
        this->getStorageImpl()->setStr(data, len, charset);
    }
    catch(ex::read_only &e)
    {
        throw ex::read_only(this->getName(), __FUNCTION__);
    }
}


///
void        
Variant::setStr(const std::string& value, const char* charset)
{ 
    init_if_null<i18n::UString>(this->m_storage);
    try
    {
        this->getStorageImpl()->setStr(value, charset); 
    }
    catch(ex::read_only &e)
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
DBWTL_VARIANT_SETTER(Money, TMoney)
DBWTL_VARIANT_SETTER(Real, float)
DBWTL_VARIANT_SETTER(Double, double)
DBWTL_VARIANT_SETTER(Date, TDate)
DBWTL_VARIANT_SETTER(Time, TTime)
DBWTL_VARIANT_SETTER(Datetime, TDatetime)
DBWTL_VARIANT_SETTER(Timestamp, signed int) /// @bug sure? TTimestamp
DBWTL_VARIANT_SETTER(CIDR, TCidr)
DBWTL_VARIANT_SETTER(Interval, TInterval)
DBWTL_VARIANT_SETTER(Macaddr, TMacaddr)
DBWTL_VARIANT_SETTER(Inetaddr, TInetaddr)
DBWTL_VARIANT_SETTER(UUID, TUuid)
DBWTL_VARIANT_SETTER(XML, TXml)


//virtual void        asCustom(void) const = 0;





///
void      
Variant::setBlob(std::streambuf* value)         
{
    /// @bug is init required?
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
daltype_t TCidr::datatype(void) const       {  return DAL_TYPE_CIDR;       }
daltype_t TDate::datatype(void) const       {  return DAL_TYPE_DATE;       }
daltype_t TTime::datatype(void) const       {  return DAL_TYPE_TIME;       }
daltype_t TInterval::datatype(void) const   {  return DAL_TYPE_INTERVAL;   }
daltype_t TMacaddr::datatype(void) const    {  return DAL_TYPE_MACADDR;    }
daltype_t TNumeric::datatype(void) const    {  return DAL_TYPE_NUMERIC;    }
daltype_t TDecimal::datatype(void) const    {  return DAL_TYPE_NUMERIC;    }
daltype_t TMoney::datatype(void) const      {  return DAL_TYPE_MONEY;      }
daltype_t TInetaddr::datatype(void) const   {  return DAL_TYPE_INETADDR;   }
daltype_t TUuid::datatype(void) const       {  return DAL_TYPE_UUID;       }
daltype_t TXml::datatype(void) const        {  return DAL_TYPE_XML;        }
daltype_t TTimestamp::datatype(void) const  {  return DAL_TYPE_TIMESTAMP;  }
daltype_t TDatetime::datatype(void) const   {  return DAL_TYPE_DATETIME;   }




//--------------------------------------------------------------------------
///
///


i18n::UString
TCidr::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TDate::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TTime::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TInterval::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TMacaddr::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TNumeric::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TMoney::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TDecimal::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TInetaddr::asStr(void) const
{
    return L"FIXME";
}


i18n::UString
TUuid::asStr(void) const
{
    return L"FIXME";
}


i18n::UString
TXml::asStr(void) const
{
    return L"FIXME";
}

i18n::UString
TTimestamp::asStr(void) const
{
    return L"FIXME";
}


i18n::UString
TDatetime::asStr(void) const
{
    return L"FIXME";
}



///////////////////////



i18n::UString
TCidr::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TDate::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TTime::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TInterval::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TMacaddr::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TNumeric::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TMoney::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TDecimal::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TInetaddr::asStr(std::locale loc) const
{
    return L"FIXME";
}


i18n::UString
TUuid::asStr(std::locale loc) const
{
    return L"FIXME";
}


i18n::UString
TXml::asStr(std::locale loc) const
{
    return L"FIXME";
}

i18n::UString
TTimestamp::asStr(std::locale loc) const
{
    return L"FIXME";
}


i18n::UString
TDatetime::asStr(std::locale loc) const
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
i18n::UString 
EmptyTableFilter::simpleNameFilter(void) const
{
    DBWTL_NOTIMPL(); /// @bug implement me
}


///
///
i18n::UString
EmptyTableFilter::simpleCatalogFilter(void) const
{
    DBWTL_NOTIMPL(); /// @bug implement me
}


///
///
i18n::UString 
EmptyTableFilter::simpleSchemaFilter(void) const
{
    DBWTL_NOTIMPL(); /// @bug implement me
}






//--------------------------------------------------------------------------
///
///
i18n::UString 
EmptyDatatypeFilter::simpleNameFilter(void) const
{
    DBWTL_NOTIMPL(); /// @bug implement me
}


///
///
i18n::UString
EmptyDatatypeFilter::simpleCatalogFilter(void) const
{
    DBWTL_NOTIMPL(); /// @bug implement me
}


///
///
i18n::UString 
EmptyDatatypeFilter::simpleSchemaFilter(void) const
{
    DBWTL_NOTIMPL(); /// @bug implement me
}



//--------------------------------------------------------------------------
///
///


//
DiagBase::DiagBase(dalstate_t state,
                   const char *codepos,
                   const char *func,
                   i18n::UString message,
                   i18n::UString description)
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
