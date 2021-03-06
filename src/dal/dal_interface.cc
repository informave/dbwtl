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
//#include "dbwtl/db_exceptions.hh"
#include "dal_debug.hh"

#include "dbwtl/dal/active_engines.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>
#include <locale>
#include <iomanip>

DAL_NAMESPACE_BEGIN


/** 
@mainpage DBWTL - Database Wrapper Template Library


@section sec_intro Introduction

The DBWTL library is a wrapper around database management system APIs. It provides
an uniform interface to all components and supports database specific features.


@section sec_engines Supported engines

 - dal::generic - Generic layer
 - dal::sqlite - SQLite


@section sec_crash Building apps with DBWTL
TODO

@subsection sec_crash_link Linking against the DBWTL library

Add the -ldbwtl option to g++
*/



#define DAL_THROW_INVALID_CAST() throw std::runtime_error(__FUNCTION__)



const IDataset::value_type&
IDataset::columnByNumber(colnum_t num)
{
	return this->column(num);
}

const IDataset::value_type&
IDataset::columnByName(const String &name)
{
	return this->column(name);
}



String
CodePosInfo::str(void) const
{
    std::stringstream ss;
    ss.imbue(std::locale("C"));
    ss << m_file << ":" << m_line << " (" << m_func << ")";
    return ss.str();
}



//--------------------------------------------------------------------------
//
IHandle::IHandle(void)
  : IDALObject(),
    m_writer(0),
    m_arg(0)
{}


bool
IHandle::writeDiagnostic(const IDiagnosticRec &rec) const
{
	if(this->m_writer)
	{
		this->m_writer(rec, this->m_arg);
	}
	return true;
}


void
IHandle::setDiagnosticWriter(void (*writer)(const IDiagnosticRec&, void*), void *arg)
{
	this->m_writer = writer;
	this->m_arg = arg;
}




Transaction::Transaction(void)
	: m_handle(0),
	  m_trxid(0)
{}

Transaction::Transaction(IHandle *handle, trxid_t trxid)
	: m_handle(handle),
	  m_trxid(trxid)
{}

Transaction::Transaction(const Transaction& trx)
	: m_handle(trx.m_handle),
	m_trxid(trx.m_trxid)
{}


Transaction::~Transaction(void)
{}


Transaction& Transaction::operator=(const Transaction &trx)
{
	m_handle = trx.m_handle;
	m_trxid = trx.m_trxid;
    return *this;
}


void
Transaction::commit(void)
{
    if(IDbc *dbc = dynamic_cast<IDbc*>(m_handle))
    {
        dbc->commit(*this);
    }
}


void
Transaction::rollback(void)
{
    if(IDbc *dbc = dynamic_cast<IDbc*>(m_handle))
    {
        dbc->rollback(*this);
    }
}



bool
TableFilter::operator()(IDataset &ds) const
{
    return ds.column("TABLE_NAME").get<String>() == m_table;
}



//--------------------------------------------------------------------------
//
/// If the type is unknown, we must return a NULL pointer.
/*
IStoredVariant* new_default_storage(daltype_t type)
{
    switch(type)
    {
    case DAL_TYPE_CUSTOM:        return 0;
    case DAL_TYPE_UNKNOWN:       return 0;
    case DAL_TYPE_INT:           return new var_storage< signed int            >();
    case DAL_TYPE_UINT:          return new var_storage< unsigned int          >();
    case DAL_TYPE_CHAR:          return new var_storage< signed char           >();
    case DAL_TYPE_UCHAR:         return new var_storage< unsigned char         >();
    case DAL_TYPE_BOOL:          return new var_storage< bool                  >();
    case DAL_TYPE_SMALLINT:      return new var_storage< signed short int      >();
    case DAL_TYPE_USMALLINT:     return new var_storage< unsigned short int    >();
    case DAL_TYPE_BIGINT:        return new var_storage< signed long long      >();
    case DAL_TYPE_UBIGINT:       return new var_storage< unsigned long long    >();
    case DAL_TYPE_FLOAT:         return new var_storage< float                 >();
    case DAL_TYPE_DOUBLE:        return new var_storage< double                >();
    case DAL_TYPE_STRING:        return new var_storage< String                >();
    case DAL_TYPE_BLOB:          return new var_storage< ByteStreamBuf*        >();
    case DAL_TYPE_MEMO:          return new var_storage< UnicodeStreamBuf*     >();
    case DAL_TYPE_NUMERIC:       return new var_storage< TNumeric              >();
    case DAL_TYPE_DATE:          return new var_storage< TDate                 >();
    case DAL_TYPE_TIME:          return new var_storage< TTime                 >();
    case DAL_TYPE_TIMESTAMP:     return new var_storage< TTimestamp            >();
    case DAL_TYPE_INTERVAL:      return new var_storage< TInterval             >();
    }
    return 0;
}
*/



//--------------------------------------------------------------------------
//
DatatypeBase::DatatypeBase(void)
    : IDatatype(),
      m_name             (DAL_TYPE_STRING, "DatatypeBase::name"),
      m_size             (DAL_TYPE_INT,    "DatatypeBase::size"),
      m_literal_prefix   (DAL_TYPE_STRING, "DatatypeBase::literal_prefix"),
      m_literal_suffix   (DAL_TYPE_STRING, "DatatypeBase::literal_suffix"),
      m_is_unsigned      (DAL_TYPE_BOOL,   "DatatypeBase::is_unsigned"),
      m_daltype          (DAL_TYPE_UNKNOWN)
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
      m_name              (DAL_TYPE_STRING,    "ColumnDesc::name"),
      m_catalog_name      (DAL_TYPE_STRING,    "ColumnDesc::catalog_name"),
      m_schema_name       (DAL_TYPE_STRING,    "ColumnDesc::schema_name"),
      m_base_column_name  (DAL_TYPE_STRING,    "ColumnDesc::base_column_name"),
      m_type_name         (DAL_TYPE_STRING,    "ColumnDesc::type_name"),
      m_base_table_name   (DAL_TYPE_STRING,    "ColumnDesc::base_table_name"),
      m_size              (DAL_TYPE_INT,       "ColumnDesc::size"),
      m_comment           (DAL_TYPE_STRING,    "ColumnDesc::comment"),
      m_is_nullable       (DAL_TYPE_BOOL,      "ColumnDesc::is_nullable"),
      m_precision         (DAL_TYPE_USMALLINT, "ColumnDesc::precision"),
      m_scale             (DAL_TYPE_USMALLINT, "ColumnDesc::scale"),
      m_is_searchable     (DAL_TYPE_BOOL,      "ColumnDesc::is_searchable"),
      m_daltype           (DAL_TYPE_UNKNOWN)
{}

/*
ColumnDesc::ColumnDesc(const IColumnDesc &orig)
{
	assert(!"fixme");
}
*/


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




ColumnDesc::ColumnDesc(const IColumnDesc &orig)
	: ColumnDescBase()
{
	assert(!"fixme");
}

ColumnDesc::ColumnDesc(void)
	: ColumnDescBase()
{
}


void
ColumnDesc::changeType(daltype_t daltype)
{
	this->m_daltype = daltype;
}


void
ColumnDesc::changeEntry(ColumnDescEntry entry, const IColumnDesc::value_type &v)
{
	switch(entry)
	{
        case DBWTL_COLUMNDESC_NAME:
		this->m_name = v;
		break;
        case DBWTL_COLUMNDESC_CATALOG_NAME:
		this->m_catalog_name = v;
		break;
        case DBWTL_COLUMNDESC_SCHEMA_NAME:
		this->m_schema_name = v;
		break;
        case DBWTL_COLUMNDESC_BASE_COLUMN_NAME:
		this->m_base_column_name = v;
		break;
        case DBWTL_COLUMNDESC_TYPE_NAME:
		this->m_type_name = v;
		break;
        case DBWTL_COLUMNDESC_BASE_TABLE_NAME:
		this->m_base_table_name = v;
		break;
        case DBWTL_COLUMNDESC_SIZE:
		this->m_size = v;
		break;
        case DBWTL_COLUMNDESC_COMMENT:
		this->m_comment = v;
		break;
        case DBWTL_COLUMNDESC_IS_NULLABLE:
		this->m_is_nullable = v;
		break;
        case DBWTL_COLUMNDESC_PRECISION:
		this->m_precision = v;
		break;
        case DBWTL_COLUMNDESC_SCALE:
		this->m_scale = v;
		break;
        case DBWTL_COLUMNDESC_IS_SEARCHABLE:
		this->m_is_searchable = v;
		break;
	};
}




//--------------------------------------------------------------------------
//
ParamDescBase::ParamDescBase(void)
    : IParamDesc(),
      m_name              (DAL_TYPE_STRING,    "ParamDesc::name"),
      m_type_name         (DAL_TYPE_STRING,    "ParamDesc::type_name"),
      m_size              (DAL_TYPE_INT,       "ParamDesc::size"),
      m_is_nullable       (DAL_TYPE_BOOL,      "ParamDesc::is_nullable"),
      m_precision         (DAL_TYPE_USMALLINT, "ParamDesc::precision"),
      m_scale             (DAL_TYPE_USMALLINT, "ParamDesc::scale"),
      m_daltype           (DAL_TYPE_UNKNOWN)
{}

/*
ParamDesc::ParamDesc(const IParamDesc &orig)
{
	assert(!"fixme");
}
*/


//
//
ParamDescBase::~ParamDescBase(void)
{}


const IParamDesc::value_type& ParamDescBase::getName(void) const           { return this->m_name; }
const IParamDesc::value_type& ParamDescBase::getTypeName(void) const       { return this->m_type_name; }
const IParamDesc::value_type& ParamDescBase::getSize(void) const           { return this->m_size; }
const IParamDesc::value_type& ParamDescBase::getIsNullable(void) const     { return this->m_is_nullable; }
const IParamDesc::value_type& ParamDescBase::getPrecision(void) const      { return this->m_precision; }
const IParamDesc::value_type& ParamDescBase::getScale(void) const          { return this->m_scale; }

//
//
daltype_t ParamDescBase::getDatatype(void) const { return this->m_daltype; }




ParamDesc::ParamDesc(const IParamDesc &orig)
	: ParamDescBase()
{
	assert(!"fixme");
}

ParamDesc::ParamDesc(void)
	: ParamDescBase()
{
}



//--------------------------------------------------------------------------
///
/// This method assigns the value from one variant to another.
/// If the source value is NULL, the destination value is just set to NULL.
/*
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
        case DAL_TYPE_CUSTOM:     this->setStr(var.asStr()); break;
        case DAL_TYPE_UNKNOWN:    this->setStr(var.asStr()); break;
        case DAL_TYPE_INT:        this->setInt(var.asInt()); break;
        case DAL_TYPE_UINT:       this->setUInt(var.asUInt()); break;
        case DAL_TYPE_CHAR:       this->setChar(var.asChar()); break;
        case DAL_TYPE_UCHAR:      this->setUChar(var.asUChar()); break;
        case DAL_TYPE_STRING:     this->setStr(var.asStr()); break;
        case DAL_TYPE_BOOL:       this->setBool(var.asBool()); break;
        case DAL_TYPE_SMALLINT:   this->setSmallint(var.asSmallint()); break;
        case DAL_TYPE_USMALLINT:  this->setUSmallint(var.asUSmallint()); break;
        case DAL_TYPE_BIGINT:     this->setBigint(var.asBigint()); break;
        case DAL_TYPE_UBIGINT:    this->setUBigint(var.asUBigint()); break;
        case DAL_TYPE_BLOB:       this->setBlob(var.asBlob()); break;
        case DAL_TYPE_MEMO:       this->setMemo(var.asMemo()); break;
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
*/

///
///
 /*
Variant
IVariant::value(void) const
{
    Variant tmp(this->datatype(), "created by deepcopy");
    tmp = *this; // assign value
    return tmp;
}
 */


  

/// Initializes a variant storage smart pointer if it is NULL.
/// We first try to get a new StoredVariant with new_default_storage.
/// If the type is unknown, we use the given T type to get a valid
/// storage object.
/*
template<class T>
inline void init_if_null(typename Variant::storage_type &storage, daltype_t type)
{
    if(storage.get() == 0)
    {
        IStoredVariant *sv = new_default_storage(type);
        storage.reset(sv ? sv : new var_storage<T>());
    }
}



/// Helper for identical setter methods
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

DBWTL_VARIANT_SETTER(Bool,          bool)
DBWTL_VARIANT_SETTER(Char,          signed    char)
DBWTL_VARIANT_SETTER(UChar,         unsigned  char)
DBWTL_VARIANT_SETTER(Int,           signed    int)
DBWTL_VARIANT_SETTER(UInt,          unsigned  int)
DBWTL_VARIANT_SETTER(Smallint,      signed    short)
DBWTL_VARIANT_SETTER(USmallint,     unsigned  short)
DBWTL_VARIANT_SETTER(Bigint,        signed    long long)
DBWTL_VARIANT_SETTER(UBigint,       unsigned  long long)
DBWTL_VARIANT_SETTER(Real,          float)
DBWTL_VARIANT_SETTER(Double,        double)
DBWTL_VARIANT_SETTER(Numeric,       TNumeric)
DBWTL_VARIANT_SETTER(Date,          TDate)
DBWTL_VARIANT_SETTER(Time,          TTime)
DBWTL_VARIANT_SETTER(Timestamp,     TTimestamp)
DBWTL_VARIANT_SETTER(Interval,      TInterval)

#undef DBWTL_VARIANT_SETTER
*/

/// Copies the given string to the variant storage,
/// using the locale if conversion to another type (e.g. float)
/// is required.
/*
void                                                        
Variant::setStr(const String& value, std::locale loc)
{                                                           
    init_if_null<String>(this->m_storage, this->m_type);     
    try                                                     
    {                                                       
        return this->getStorageImpl()->setStr(value, loc); 
    }                                                       
    catch(ex::read_only &)                                  
    {                                                       
        throw ex::read_only(this->getName(), __FUNCTION__); 
    }                                                       
}


/// Copies byte data from the given byte stream to the
/// internal variant storage.
void      
Variant::setBlob(ByteStreamBuf* value)         
{
    /// If the storage is empty, we initialize it with a dummy
    /// ByteStreamBuf*, but there's no chance to set a real buffer.
    /// This method was introduced to get data from a stream and
    /// write it into an _existing_ variant value.
    ///
    /// Mostly used when the user has bound a ofstream byte stream
    /// as an output parameter and the DBWTL library writes the data
    /// from the internal BLOB variant to the bound variant.
    ///
    /// Another possible requirement for this method is if a
    /// user has a BLOB variant and assigns a resultset column
    /// which contains a BLOB.
    init_if_null<ByteStreamBuf*>(this->m_storage, this->m_type);     
    try                                                     
    {
        return this->getStorageImpl()->setBlob(value);
    }
    catch(ex::read_only &)
    {
        throw ex::read_only(this->getName(), __FUNCTION__); 
    }
}


/// Copies the unicode data from the given stream to the
/// internal variant storage.
void
Variant::setMemo(UnicodeStreamBuf* value)
{
    init_if_null<UnicodeStreamBuf*>(this->m_storage, this->m_type);     
    try                                                     
    {
        return this->getStorageImpl()->setMemo(value);
    }
    catch(ex::read_only &)
    {
        throw ex::read_only(this->getName(), __FUNCTION__); 
    }
}
*/
   




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







DiagnosticRec::DiagnosticRec(const CodePosInfo &cpi,
                dalstate_t dalstate,
                SQLSTATE sqlstate,
                const Variant &nativeCode,
                const String &msg,
                rownum_t rowNum,
                colnum_t colNum,
                const String &desc,
                const Variant &data)
      : m_dalstate(dalstate),
       m_nativeCode(nativeCode),
        m_msg(msg),
         m_description(desc),
    m_data(data),
       m_codepos(cpi),
          m_sqlstate(sqlstate),
       m_rownum(rowNum),
        m_colnum(colNum)

{
}

    
DiagnosticRec::DiagnosticRec(const DiagnosticRec &o)
      : m_dalstate(o.m_dalstate),
       m_nativeCode(o.m_nativeCode),
        m_msg(o.m_msg),
         m_description(o.m_description),
    m_data(o.m_data),
       m_codepos(o.m_codepos),
          m_sqlstate(o.m_sqlstate),
       m_rownum(o.m_rownum),
        m_colnum(o.m_colnum)
{
}


DiagnosticRec::DiagnosticRec(const CodePosInfo &cpi)
      : m_dalstate(),
       m_nativeCode(),
        m_msg(),
         m_description(),
    m_data(),
       m_codepos(cpi),
          m_sqlstate("00000"),
       m_rownum(),
        m_colnum()
{
}

 dalstate_t       
DiagnosticRec::getState(void) const
{
	return this->m_dalstate;
}
const Variant&        
DiagnosticRec::getNativeErrorCode(void) const
{
return this->m_nativeCode;
}

const String&         
DiagnosticRec::getMsg(void) const
{
	return this->m_msg;
}

const String&          
DiagnosticRec::getDescription(void) const
{
	return this->m_description;
}

 const Variant&        
DiagnosticRec::getData(void) const
{
	return this->m_data;
}

   const CodePosInfo&    
DiagnosticRec::getCodepos(void) const
{
	return this->m_codepos;
}

   const SQLSTATE        
DiagnosticRec::getSqlstate(void) const
{
	return this->m_sqlstate;
}

  rownum_t     
DiagnosticRec::getRowNumber(void) const
{
	return this->m_rownum;
}

  colnum_t     
DiagnosticRec::getColumnNumber(void) const
{
	return this->m_colnum;
}


String
DiagnosticRec::str(void) const
{
	String serv;
	switch(this->getState())
	{
	case DAL_STATE_DEBUG:
		serv = "DEBUG"; break;
	case DAL_STATE_WARNING:
		serv = "WARNING"; break;
	case DAL_STATE_ERROR:
		serv = "ERROR"; break;
	case DAL_STATE_INFO:
		serv = "INFO"; break;
	}

	std::wstringstream ss;
	ss << serv << L" [" << this->getSqlstate().str() << L"] " << this->getMsg();
	ss << std::endl;
	ss << L"----------" << std::endl;
	ss << L"column (" << this->getColumnNumber() << L"), row (" << this->getRowNumber() << L"), nativecode (" << this->getNativeErrorCode() << L")";
	ss << std::endl;
	if(!this->getDescription().empty())
	{
		ss << L"" << this->getDescription();
		ss << std::endl;
	}
	if(!this->getData().isnull())
	{
		ss << L"RAW: [" << this->getData().asStr() << L"]";
		ss << std::endl;
	}
	ss << L"Source: " << this->getCodepos().str() << L"";
	ss << std::endl << L"----------" << std::endl;
	return ss.str();
}

IDiagnosticRec*
DiagnosticRec::clone(void) const
{
		return NULL; /// @bug fixme
}


SQLSTATE::SQLSTATE(const char *sqlstate)
	: state(sqlstate)
{
}


SQLSTATE::SQLSTATE(SQLSTATE const &o)
	: state(o.state)
{
}

String
SQLSTATE::str(void) const
{
	return String(this->state);
}


//--------------------------------------------------------------------------
///
///


//
DiagBase::DiagBase(dalstate_t state,
                   CodePosInfo pos,
                   Variant what,
                   String description)
    : IDiagnostic(),
      m_state        (state),
      m_sqlstate     (DAL_TYPE_STRING, "SqliteDiag::sqlstate"),
      m_codepos      (pos),
      m_message      (DAL_TYPE_STRING, "SqliteDiag::message"),
      m_description  (DAL_TYPE_STRING, "SqliteDiag::description")
      //m_sqlstate_id() // fix?
{
    m_message = what;
    m_description = description;
}


//
DiagBase::DiagBase(const DiagBase& ref)
    : IDiagnostic(),
      m_state(ref.m_state),
      m_sqlstate(ref.m_sqlstate),
      m_codepos(ref.m_codepos),
      m_message(ref.m_message),
      m_description(ref.m_description)
      //m_sqlstate_id(ref.m_sqlstate_id)
{}




dalstate_t
DiagBase::getState(void) const
{
    return this->m_state;
}


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

const CodePosInfo&
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


std::string replace_all(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos)
	{
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void defaultDiagnosticWriter(const IDiagnosticRec &rec, void*)
{
	static size_t errc = 0;
	++errc;
	std::string utf8_msg = rec.getMsg().utf8();
	utf8_msg = replace_all(utf8_msg, "\r\n", " ");
	utf8_msg = replace_all(utf8_msg, "\n", " ");
	std::cerr << "[ERR " << errc << "]: " << utf8_msg << std::endl;
	std::cerr << "[ERR " << errc << "]: " << "SQLSTATE " << rec.getSqlstate().str() << std::endl;
	std::cerr << "[ERR " << errc << "]: " << "at " << rec.getCodepos().str() << std::endl;	
	//std::cerr << "[DIAG] " << rec.str() << std::endl;
	std::cerr << std::endl;
}

//--------------------------------------------------------------------------
///
///
EnvBase::EnvBase(void)
  : IEnv(),
    m_options()
{ 
    DAL_ADD_OPTION("env_library_path", DAL_TYPE_STRING);
    DAL_ADD_OPTION("env_diag_maxsize", DAL_TYPE_UINT);
    
    this->m_options["env_diag_maxsize"].set(50);

	this->setDiagnosticWriter(defaultDiagnosticWriter);
}


///
///
void
EnvBase::setOption(std::string name, const Variant &data)
{
    options_type::iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        i->second = data;
    }
    else
    {
        throw NotFoundException(format("The option '%s' is invalid or not supported by this engine.") % String(name));
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
        throw NotFoundException(format("The option '%s' is invalid or not supported by this engine.") % String(name));
    }
}






//--------------------------------------------------------------------------
///
///
StmtBase::StmtBase(void)
  : IStmt(),
    m_params(),
    m_temp_params(),
    m_cursorstate(DAL_CURSOR_CLOSED),
    m_options()
{ 
    DAL_ADD_OPTION("env_diag_maxsize", DAL_TYPE_UINT);
    
    this->m_options["env_diag_maxsize"].set(50);

	this->setDiagnosticWriter(defaultDiagnosticWriter);
}



///
///
void
StmtBase::setOption(std::string name, const Variant &data)
{
    options_type::iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        i->second = data;
    }
    else
    {
        throw NotFoundException(format("The option '%s' is invalid or not supported by this engine.") % String(name));
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
        throw NotFoundException(format("The option '%s' is invalid or not supported by this engine.") % String(name));
    }
}



/// Support for binding POD types via an implicit constructor
/// call.
void 
StmtBase::bind(int num, const Variant &data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(data.clone());
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}



///
///
void
StmtBase::bind(int num, Variant* data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(data->clone());
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}


///
///
void
StmtBase::bind(int num, const Variant* data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(data->clone());
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}


///
///
void
StmtBase::bind(int num, ByteStreamBuf *data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(BlobStream(data));
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}

void
StmtBase::bind(int num, UnicodeStreamBuf *data)
{
    DALTRACE_VISIT;
    Variant* tmp = new Variant(MemoStream(data));
    this->m_temp_params.push_back(tmp);
    this->m_params[num] = tmp;
}


const IParamDesc&
StmtBase::describeParam(int num) const
{
	throw EngineException("Unsupported method: describeParam()");
}

///
///
void
StmtBase::close(void)
{
    this->m_cursorstate = DAL_CURSOR_CLOSED;

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
    return this->m_cursorstate & DAL_CURSOR_PREPARED;
}


///
///
bool
StmtBase::isBad(void) const
{
    return this->m_cursorstate & DAL_CURSOR_BAD;
}


///
///
IDataProvider*
StmtBase::newProvider(void)
{
	throw EngineException("Unsupported method: newProvider()");
}


//--------------------------------------------------------------------------
///
///
bool
ResultBase::isPrepared(void) const
{
    return this->m_cursorstate & DAL_CURSOR_PREPARED;
}


///
///
bool
ResultBase::isBad(void) const
{
    return this->m_cursorstate & DAL_CURSOR_BAD;
}


///
///
bool
ResultBase::isOpen(void) const
{
    return this->m_cursorstate & DAL_CURSOR_OPEN;
}

///
///
bool
ResultBase::isPositioned(void) const
{
    return this->m_cursorstate & DAL_CURSOR_POSITIONED;
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
    
    this->m_options["env_diag_maxsize"].set(50);

	this->setDiagnosticWriter(defaultDiagnosticWriter);
}


///
///
void
DbcBase::setOption(std::string name, const Variant &data)
{
    options_type::iterator i = this->m_options.find(name);
    if(i != this->m_options.end())
    {
        i->second = data;
    }
    else
    {
        throw NotFoundException(format("The option '%s' is invalid or not supported by this engine.") % String(name));
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
        throw NotFoundException(format("The option '%s' is invalid or not supported by this engine.") % String(name));
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




cfmt_header::cfmt_header(IDataset &ds)
    : m_ds(ds)
{
}


void
cfmt_header::write(std::ostream &os) const
{
    for(colnum_t i = 1; i <= m_ds.columnCount(); ++i)
    {
        os << std::setw(ifnull<int>(m_ds.describeColumn(i).getSize(), 30))
           << std::left
           << ifnull<String>(m_ds.describeColumn(i).getName(), String("<NULL>"))
           << (i == m_ds.columnCount() ? "" : "|");   
    }
}


cfmt::cfmt(IDataset &ds, colnum_t colnum)
    : m_ds(ds),
      m_cn(colnum),
      m_cs(),
      m_byname(false)
{
}


cfmt::cfmt(IDataset &ds, const String &name)
    : m_ds(ds),
      m_cn(0),
      m_cs(name),
      m_byname(true)
{
}

void
cfmt::write(std::ostream &os) const
{
    const Variant *v = 0;
    const IColumnDesc *desc = 0;
    if(m_byname)
    {
        v = &this->m_ds.column(m_cs);
        desc = &this->m_ds.describeColumn(m_cs);
    }
    else
    {
        v = &this->m_ds.column(m_cn);
        desc = &this->m_ds.describeColumn(m_cn);
    }

    size_t width = desc->getSize().isnull() || desc->getSize().get<int>() <= 0 ? 30 : desc->getSize().get<int>();
    if(width < desc->getName().get<String>().length())
        width = desc->getName().get<String>().length();
    if(ifnull<bool>(desc->getIsNullable(), true))
    {
        if(width < strlen("<NULL>"))
            width = strlen("<NULL>");
    }

    os << std::setw(width);
    if(desc->getDatatype() == DAL_TYPE_STRING)
        os << std::left;
    if(!v->isnull())
        os << *v;
    else
        os << "<NULL>";

}


std::ostream& operator<<(std::ostream &os, const cfmt &cf)
{
    cf.write(os);
    return os;
}

std::ostream& operator<<(std::ostream &os, const cfmt_header &cf)
{
    cf.write(os);
    return os;
}



DAL_NAMESPACE_END
