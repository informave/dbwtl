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


DAL_NAMESPACE_BEGIN

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
    case DAL_TYPE_BLOB: return new var_storage<i18n::UString>();
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
/// Deletes a object
struct delete_object
{
    template <typename T>
    void operator()(T ptr){ delete ptr;}

    template<typename U, typename V>
    void operator()(std::pair<U, V> val){ delete val.second;}
};



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
IBlob::~IBlob(void)
{}



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
IBlob&     
Variant::asBlob(void) const { return this->getStorageImpl()->asBlob(); }


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
Variant::setBlob(IBlob& value)         
{
    //init_if_null<IBlob>(this->m_storage);
    this->m_storage->setBlob(value); 
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
///
IEngineState::IEngineState(void)
{}


///
///
IEngineState::~IEngineState(void)
{}





//--------------------------------------------------------------------------
///
///
EngineState::EngineState(void) 
    : m_state(0), m_state_code(DALSTATE_OK)
{}


///
///
EngineState::EngineState(int code) 
    : m_state(0), m_state_code(code)
{}


///
///
EngineState& 
EngineState::operator=(int code)
{
    this->m_state.release();
    this->m_state_code = code;
    return *this;
}


///
///
bool 
EngineState::operator!=(int code)
{
    return ! this->operator==(code);
}


///
///
bool 
EngineState::operator==(int code)
{
    if(this->m_state.get())
        return this->m_state->getDALCode() == code;
    else
        return this->m_state_code == code;
}


///
///
EngineState::EngineState(const IEngineState& newstate) 
    : m_state(0), 
      m_state_code(0)
{
    this->m_state.reset( newstate.clone() );
}


///
///
EngineState::EngineState(const EngineState& newstate)
    : m_state(0), 
      m_state_code(newstate.m_state_code)
{
    if(this->m_state.get())
        this->m_state.reset( newstate.m_state->clone() );
}


///
///
EngineState&
EngineState::operator=(const EngineState& newstate)
{
    if(newstate.m_state.get())
        this->m_state.reset( newstate.m_state->clone() );
    this->m_state_code = newstate.m_state_code;
    return *this;
}



///
///
const IEngineState*
EngineState::getImpl(void) const
{
    return this->m_state.get();
}


///
///
IEngineState*
EngineState::getImpl(void)
{
    return this->m_state.get();
}


///
///
EngineState::~EngineState(void)
{}


///
///
void
EngineState::setMsg(i18n::UString msg)
{
    assert(this->m_state.get());
    this->m_state->setMsg(msg);
}


///
///
i18n::UString
EngineState::getMsg(void) const
{
    assert(this->m_state.get());
    return this->m_state->getMsg();
}



///
///
std::string
EngineState::getMsgUTF8(void) const
{
    assert(this->m_state.get());
    return i18n::conv_to(this->m_state->getMsg(), "UTF-8");
}



///
///
void 
EngineState::setDescription(i18n::UString desc)
{
    assert(this->m_state.get());
    this->m_state->setDescription(desc);
}


///
///
void 
EngineState::setSource(std::string file, std::string func)
{
    assert(this->m_state.get());
    this->m_state->setSource(file, func);    
}


///
///
void 
EngineState::setDALCode(int code)
{
    if(this->m_state.get())
        this->m_state->setDALCode(code);
    else
        this->m_state_code = code;
}


///
///
int
EngineState::getDALCode(void) const
{
    if(this->m_state.get())
        return this->m_state->getDALCode();
    else
        return this->m_state_code;
}


///
///
void 
EngineState::addUsedFile(std::string file)
{
    assert(this->m_state.get());
    this->m_state->addUsedFile(file);
}



///
///
void 
EngineState::setSQLState(std::string state)
{
    assert(this->m_state.get());
    this->m_state->setSQLState(state);
}


///
///
i18n::UString
EngineState::dump(void) const
{
    if(! this->m_state.get())
    {
        std::string s = "State code: ";
        s.append(dal_state_msg(this->m_state_code));
        return i18n::conv_from(s, "ASCII");
    }
    return this->m_state->dump();
}


///
///
EngineState*
EngineState::clone(void) const
{
    DBWTL_NOTIMPL();
}



///
///
std::ostream& operator<<(std::ostream& o, const informave::db::dal::IEngineState& state)
{
    o << i18n::conv_to(state.dump(), "UTF-8");
    return o;
}


///
///
std::wostream& operator<<(std::wostream& o, const informave::db::dal::IEngineState& state)
{
    o << state.dump();
    return o;
    /*
      o << std::endl
      << "(((DAL STATE)))" << std::endl
      << "[DAL_STATE = OK (Code: 0 / 0x0)]" << std::endl
      << "[SQL_STATE = HY0100]" << std::endl
      << "[DRV_CODE  = PG_INVALID_USER (Code: -298, Extended: -2938728)]" << std::endl
      << "[SOURCE    = DALInterface::Fooo]" << std::endl
      << "[......    = foobar.cc line 2343]" << std::endl
      << "[CATCHED   = foo.cc]" << std::endl
      << "[.......   = bar.cc]" << std::endl
      << "[.......   = bar.cc]" << std::endl
      << "[.......   = bar.cc]" << std::endl
      << "[FILES     = /usr/lib/libpq.so]" << std::endl
      << "The server refused the connection." << std::endl
      << std::endl;
    */
}







//--------------------------------------------------------------------------
struct dal_state_text_t
{
    int code;
    const char *text;
};

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
BaseEngineState::BaseEngineState(void)
    : m_msg(),
      m_desc(),
      m_srcfile(),
      m_srcfunc(),
      m_dalcode(DALSTATE_OK),
      m_files(),
      m_sqlstate("00000")
{}


///
///
BaseEngineState::~BaseEngineState(void)
{}


///
///
void 
BaseEngineState::setMsg(i18n::UString msg)
{
    this->m_msg = msg;
}


///
///
i18n::UString 
BaseEngineState::getMsg(void) const
{
    return this->m_msg;
}


///
///
void 
BaseEngineState::setDescription(i18n::UString desc)
{
    this->m_desc = desc;
}


///
///
void 
BaseEngineState::setSource(std::string file, std::string func)
{
    this->m_srcfile = file;
    this->m_srcfunc = func;
}


///
///
void 
BaseEngineState::setDALCode(int code)
{ 
    this->m_dalcode = code;
}


///
///
int
BaseEngineState::getDALCode(void) const
{ 
    return this->m_dalcode;
}


///
///
void 
BaseEngineState::addUsedFile(std::string file)
{ 
    this->m_files.push_back(file);
}


///
///
void 
BaseEngineState::setSQLState(std::string state)
{ 
    this->m_sqlstate = state;
}




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
ITypeInfo::ITypeInfo(void)
    : IDALObject(),
      m_typeid(DAL_TYPE_UNKNOWN)
{ }


///
///
ITypeInfo::~ITypeInfo(void)
{ }


///
///
i18n::UString
ITypeInfo::name(void) const
{
    switch(this->m_typeid)
    {
    case DAL_TYPE_UNKNOWN:
        return L"UNKNOWN";
    case  DAL_TYPE_INT:
        return L"INTEGER";
    case  DAL_TYPE_CHAR:
        return L"CHAR";
    case  DAL_TYPE_VARCHAR:
        return L"VARCHAR";
    case  DAL_TYPE_NVARCHAR:
        return L"NVARCHAR";
    case  DAL_TYPE_NCHAR:
        return L"NCHAR";
    case  DAL_TYPE_BOOL:
        return L"BOOL";
    case  DAL_TYPE_SMALLINT:
        return L"SMALLINT";
    case  DAL_TYPE_BIGINT:
        return L"BIGINT";
    case  DAL_TYPE_BLOB:
        return L"BLOB";
    case  DAL_TYPE_NUMERIC:
        return L"NUMERIC";
    case  DAL_TYPE_DECIMAL:
        return L"DECIMAL";
    case  DAL_TYPE_MONEY:
        return L"MONEY";
    case  DAL_TYPE_FLOAT:
        return L"FLOAT";
    case  DAL_TYPE_DOUBLE:
        return L"DOUBLE";
    case  DAL_TYPE_DATE:
        return L"DATE";
    case  DAL_TYPE_TIME:
        return L"TIME";
    case  DAL_TYPE_DATETIME:
        return L"DATETIME";
    case  DAL_TYPE_TIMESTAMP:
        return L"TIMESTAMP";
    case  DAL_TYPE_CUSTOM:
        return L"CUSTOM";
    default:
        throw std::runtime_error("unknow type");
        return L"TYPENOTIMPL";
    }
    throw std::runtime_error("unknow type");
}



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
StmtBase::close(void)
{
    
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
