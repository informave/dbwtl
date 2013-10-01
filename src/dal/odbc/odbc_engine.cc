//
// odbc_engine.cc - SQLite engine (definition)
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
/// @brief SQLite engine (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/dal/engines/generic_engine.hh"
#include "dbwtl/dal/engines/odbc_engine.hh"
#include "odbc_libodbc.hh"
#include "../dal_debug.hh"
#include "../../utils.hh"

#include <sstream>
#include <string>



DB_NAMESPACE_BEGIN




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


signed char
sv_accessor<OdbcData*>::cast(signed char*, std::locale loc) const
{

    typedef signed char output_type;
/*
  if(this->get_value()->daltype() == DAL_TYPE_TINYINT)
  return this->get_value()->getSTinyint();
  else if(this->get_value()->daltype() == DAL_TYPE_UTINYINT)
  return this->get_value()->getUTinyint();
  else
  return this->deepcopy().get<output_type>();
  {

  }
*/

    return Variant(this->deepcopy()).get<int>();
/*
  switch(this->get_value()->daltype())
  {
  case DAL_TYPE_INT:
  return this->get_value()->getSLong();
  case DAL_TYPE_UINT:
  return this->get_value()->getULong();
  case DAL_TYPE_SMALLINT:
  return this->get_value()->getSShort();
  case DAL_TYPE_USMALLINT:
  return this->get_value()->getUShort();
  case DAL_TYPE_BIGINT:
  return this->get_value()->getSBigint();
  case DAL_TYPE_UBIGINT:
  return this->get_value()->getUBigint();
  case DAL_TYPE_BOOL:
  return this->get_value()->getBit();
  case DAL_TYPE_FLOAT:
  return this->get_value()->getFloat();
  case DAL_TYPE_DOUBLE:
  return this->get_value()->getDouble();
  case DAL_TYPE_NUMERIC:
  return Variant(this->get_value()->getNumeric()).get<output_type>();
  case DAL_TYPE_STRING:
  return Variant(this->get_value()->getString()).get<output_type>();
  default:
  DBWTL_BUG();
  }
*/
}

unsigned char
sv_accessor<OdbcData*>::cast(unsigned char*, std::locale loc) const
{
    DBWTL_NOTIMPL();
/*
  typedef signed char output_type;
  switch(this->get_value()->daltype())
  {
  case DAL_TYPE_INT:
  return this->get_value()->getSLong();
  case DAL_TYPE_UINT:
  return this->get_value()->getULong();
  case DAL_TYPE_SMALLINT:
  return this->get_value()->getSShort();
  case DAL_TYPE_USMALLINT:
  return this->get_value()->getUShort();
  case DAL_TYPE_BIGINT:
  return this->get_value()->getSBigint();
  case DAL_TYPE_UBIGINT:
  return this->get_value()->getUBigint();
  case DAL_TYPE_BOOL:
  return this->get_value()->getBit();
  case DAL_TYPE_FLOAT:
  return this->get_value()->getFloat();
  case DAL_TYPE_DOUBLE:
  return this->get_value()->getDouble();
  case DAL_TYPE_NUMERIC:
  return Variant(this->get_value()->getNumeric()).get<output_type>();
  case DAL_TYPE_STRING:
  return Variant(this->get_value()->getString()).get<output_type>();
  default:
  DBWTL_BUG();
  }
*/
}

signed int
sv_accessor<OdbcData*>::cast(signed int*, std::locale loc) const
{
    typedef signed int output_type;
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_INT:
        return this->get_value()->getSLong();
    case DAL_TYPE_UINT:
        return this->get_value()->getULong();
    case DAL_TYPE_SMALLINT:
        return this->get_value()->getSShort();
    case DAL_TYPE_USMALLINT:
        return this->get_value()->getUShort();
    case DAL_TYPE_BIGINT:
        return this->get_value()->getSBigint();
    case DAL_TYPE_UBIGINT:
        return this->get_value()->getUBigint();
    case DAL_TYPE_BOOL:
        return this->get_value()->getBit();
    case DAL_TYPE_FLOAT:
        return this->get_value()->getFloat();
    case DAL_TYPE_DOUBLE:
        return this->get_value()->getDouble();
    case DAL_TYPE_NUMERIC:
        return Variant(this->get_value()->getNumeric()).get<output_type>();
    case DAL_TYPE_STRING:
        return Variant(this->get_value()->getString()).get<output_type>();
    default:
        DBWTL_BUG();
    }
}


signed short
sv_accessor<OdbcData*>::cast(signed short*, std::locale loc) const
{
    typedef signed short output_type;
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_INT:
        return this->get_value()->getSLong();
    case DAL_TYPE_UINT:
        return this->get_value()->getULong();
    case DAL_TYPE_SMALLINT:
        return this->get_value()->getSShort();
    case DAL_TYPE_USMALLINT:
        return this->get_value()->getUShort();
    case DAL_TYPE_BIGINT:
        return this->get_value()->getSBigint();
    case DAL_TYPE_UBIGINT:
        return this->get_value()->getUBigint();
    case DAL_TYPE_BOOL:
        return this->get_value()->getBit();
    case DAL_TYPE_FLOAT:
        return this->get_value()->getFloat();
    case DAL_TYPE_DOUBLE:
        return this->get_value()->getDouble();
    case DAL_TYPE_NUMERIC:
        return Variant(this->get_value()->getNumeric()).get<output_type>();
    case DAL_TYPE_STRING:
        return Variant(this->get_value()->getString()).get<output_type>();
    default:
        DBWTL_BUG();
    }
}

signed long long
sv_accessor<OdbcData*>::cast(signed long long*, std::locale loc) const
{
    typedef signed long long output_type;
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_INT:
        return this->get_value()->getSLong();
    case DAL_TYPE_UINT:
        return this->get_value()->getULong();
    case DAL_TYPE_SMALLINT:
        return this->get_value()->getSShort();
    case DAL_TYPE_USMALLINT:
        return this->get_value()->getUShort();
    case DAL_TYPE_BIGINT:
        return this->get_value()->getSBigint();
    case DAL_TYPE_UBIGINT:
        return this->get_value()->getUBigint();
    case DAL_TYPE_BOOL:
        return this->get_value()->getBit();
    case DAL_TYPE_FLOAT:
        return this->get_value()->getFloat();
    case DAL_TYPE_DOUBLE:
        return this->get_value()->getDouble();
    case DAL_TYPE_NUMERIC:
        return Variant(this->get_value()->getNumeric()).get<output_type>();
    case DAL_TYPE_STRING:
        return Variant(this->get_value()->getString()).get<output_type>();
    default:
        DBWTL_BUG();
    }
}

unsigned int
sv_accessor<OdbcData*>::cast(unsigned int*, std::locale loc) const
{
    typedef unsigned int output_type;
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_INT:
        return this->get_value()->getSLong();
    case DAL_TYPE_UINT:
        return this->get_value()->getULong();
    case DAL_TYPE_SMALLINT:
        return this->get_value()->getSShort();
    case DAL_TYPE_USMALLINT:
        return this->get_value()->getUShort();
    case DAL_TYPE_BIGINT:
        return this->get_value()->getSBigint();
    case DAL_TYPE_UBIGINT:
        return this->get_value()->getUBigint();
    case DAL_TYPE_BOOL:
        return this->get_value()->getBit();
    case DAL_TYPE_FLOAT:
        return this->get_value()->getFloat();
    case DAL_TYPE_DOUBLE:
        return this->get_value()->getDouble();
    case DAL_TYPE_NUMERIC:
        return Variant(this->get_value()->getNumeric()).get<output_type>();
    case DAL_TYPE_STRING:
        return Variant(this->get_value()->getString()).get<output_type>();
    default:
        DBWTL_BUG();
    }
}

unsigned short
sv_accessor<OdbcData*>::cast(unsigned short*, std::locale loc) const
{
    typedef unsigned short output_type;
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_INT:
        return this->get_value()->getSLong();
    case DAL_TYPE_UINT:
        return this->get_value()->getULong();
    case DAL_TYPE_SMALLINT:
        return this->get_value()->getSShort();
    case DAL_TYPE_USMALLINT:
        return this->get_value()->getUShort();
    case DAL_TYPE_BIGINT:
        return this->get_value()->getSBigint();
    case DAL_TYPE_UBIGINT:
        return this->get_value()->getUBigint();
    case DAL_TYPE_BOOL:
        return this->get_value()->getBit();
    case DAL_TYPE_FLOAT:
        return this->get_value()->getFloat();
    case DAL_TYPE_DOUBLE:
        return this->get_value()->getDouble();
    case DAL_TYPE_NUMERIC:
        return Variant(this->get_value()->getNumeric()).get<output_type>();
    case DAL_TYPE_STRING:
        return Variant(this->get_value()->getString()).get<output_type>();
    default:
        DBWTL_BUG();
    }
}

unsigned long long
sv_accessor<OdbcData*>::cast(unsigned long long*, std::locale loc) const
{
    typedef unsigned long long output_type;
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_INT:
        return this->get_value()->getSLong();
    case DAL_TYPE_UINT:
        return this->get_value()->getULong();
    case DAL_TYPE_SMALLINT:
        return this->get_value()->getSShort();
    case DAL_TYPE_USMALLINT:
        return this->get_value()->getUShort();
    case DAL_TYPE_BIGINT:
        return this->get_value()->getSBigint();
    case DAL_TYPE_UBIGINT:
        return this->get_value()->getUBigint();
    case DAL_TYPE_BOOL:
        return this->get_value()->getBit();
    case DAL_TYPE_FLOAT:
        return this->get_value()->getFloat();
    case DAL_TYPE_DOUBLE:
        return this->get_value()->getDouble();
    case DAL_TYPE_NUMERIC:
        return Variant(this->get_value()->getNumeric()).get<output_type>();
    case DAL_TYPE_STRING:
        return Variant(this->get_value()->getString()).get<output_type>();
    default:
        DBWTL_BUG();
    }
}


float
sv_accessor<OdbcData*>::cast(float*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_FLOAT:
        return this->get_value()->getFloat();
    case DAL_TYPE_DOUBLE:
        return this->get_value()->getDouble();
        //case DAL_TYPE_NUMERIC:
        //return this->get_value()->getNumeric();
    default:
        DBWTL_BUG();
    }
}


double
sv_accessor<OdbcData*>::cast(double*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_FLOAT:
        return this->get_value()->getFloat();
    case DAL_TYPE_DOUBLE:
        return this->get_value()->getDouble();
    default:
        DBWTL_BUG();
    }
}



bool
sv_accessor<OdbcData*>::cast(bool*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_BOOL:
        return this->get_value()->getBit() != 0;
    case DAL_TYPE_INT:
        return this->get_value()->getSLong();
    case DAL_TYPE_STRING:
        return Variant(this->get_value()->getString()).get<bool>();
    default:
        DBWTL_BUG();
    }
}


BlobStream
sv_accessor<OdbcData*>::cast(BlobStream*, std::locale loc) const
{
    return BlobStream(this->get_value()->getBlob());
}

MemoStream
sv_accessor<OdbcData*>::cast(MemoStream*, std::locale loc) const
{
    return MemoStream(this->get_value()->getMemo());
}

TDate
sv_accessor<OdbcData*>::cast(TDate*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_DATE:
        return this->get_value()->getDate();
    case DAL_TYPE_TIMESTAMP:
        return this->get_value()->getTimestamp();
    case DAL_TYPE_STRING:
        return TDate(this->get_value()->getString());
    default:
        std::cout << daltype2string(this->get_value()->daltype()) << std::endl;
        DBWTL_BUG();
    }
    //String s = this->asStr(std::locale("C"));
/*
  String s;
  try
  {
  int a = Variant(s).get<int>();
  return TDate(TTimestamp(a));
  }
  catch(ex::convert_error &)
  {
  return TDate(s);
  }
*/
}

TTime
sv_accessor<OdbcData*>::cast(TTime*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_TIME:
        return this->get_value()->getTime();
    case DAL_TYPE_TIMESTAMP:
        return this->get_value()->getTimestamp();
    case DAL_TYPE_STRING:
        return TTime(this->get_value()->getString());
    default:
        std::cout << daltype2string(this->get_value()->daltype()) << std::endl;
        std::cout << daltype2string(this->get_value()->daltype()) << std::endl;
        DBWTL_BUG();
    }
/*
  String s = this->get_value()->getText();
  try
  {
  int a = Variant(s).get<int>();
  return TTime(TTimestamp(a));
  }
  catch(ex::convert_error &)
  {
  return TTime(std::string(s));
  }
*/
}


TTimestamp
sv_accessor<OdbcData*>::cast(TTimestamp*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_DATE:
        return this->get_value()->getTimestamp();
    case DAL_TYPE_TIMESTAMP:
        return this->get_value()->getTimestamp();
    case DAL_TYPE_STRING:
        return TTimestamp(this->get_value()->getString());
    default:
        std::cout << daltype2string(this->get_value()->daltype()) << std::endl;
        DBWTL_BUG();
    }
/*
  String s = this->get_value()->getText();
  try
  {
  int a = Variant(s).get<int>();
  return TTimestamp(a);
  }
  catch(ex::convert_error &)
  {
  return TTimestamp(std::string(s));
  }
*/
}



TNumeric
sv_accessor<OdbcData*>::cast(TNumeric*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_NUMERIC:
        return this->get_value()->getNumeric();
    case DAL_TYPE_STRING:
        return TNumeric(this->get_value()->getString().utf8());
    default:
        std::cout << daltype2string(this->get_value()->daltype()) << std::endl;
        DBWTL_BUG();
    }
}



TVarbinary
sv_accessor<OdbcData*>::cast(TVarbinary*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_VARBINARY:
        return this->get_value()->getBinary();
    case DAL_TYPE_BLOB:
        return Blob(this->get_value()->getBlob()).toVarbinary();
    default:
        std::cout << daltype2string(this->get_value()->daltype()) << std::endl;
        DBWTL_BUG();
    }
}




String
sv_accessor<OdbcData*>::cast(String*, std::locale loc) const
{
    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_INT:
        return Variant((signed int)(this->get_value()->getSLong())).get<String>();
    case DAL_TYPE_UINT:
        return Variant((unsigned int)(this->get_value()->getULong())).get<String>();
//    case DAL_TYPE_CHAR:
//    case DAL_TYPE_UCHAR:
    case DAL_TYPE_BOOL:
        return Variant(this->get_value()->getBit() ? true : false).get<String>();
    case DAL_TYPE_SMALLINT:
        return Variant(this->get_value()->getSShort()).get<String>();
    case DAL_TYPE_USMALLINT:
        return Variant(this->get_value()->getUShort()).get<String>();
    case DAL_TYPE_BIGINT:
        return Variant(this->get_value()->getSBigint()).get<String>();
    case DAL_TYPE_UBIGINT:
        return Variant(this->get_value()->getUBigint()).get<String>();
    case DAL_TYPE_NUMERIC:
        return Variant(this->get_value()->getNumeric()).get<String>();
//    case DAL_TYPE_VARBINARY:
    case DAL_TYPE_MEMO:
        return Memo(this->get_value()->getMemo()).str();
    case DAL_TYPE_BLOB:
        return Blob(this->get_value()->getBlob()).toVarbinary().str();
    case DAL_TYPE_FLOAT:
        return Variant(this->get_value()->getFloat()).get<String>();
    case DAL_TYPE_DOUBLE:
        return Variant(this->get_value()->getDouble()).get<String>();
    case DAL_TYPE_DATE:
        return this->get_value()->getDate().str();
    case DAL_TYPE_TIME:
        return this->get_value()->getTime().str();
    case DAL_TYPE_TIMESTAMP:
        return this->get_value()->getTimestamp().str();
    case DAL_TYPE_STRING:
        return this->get_value()->getString();
    default:
        std::cout << daltype2string(this->get_value()->daltype()) << std::endl;
        DBWTL_BUG();
    }
}

bool
sv_accessor<OdbcData*>::valueNullCheck() const
{
    return this->get_value()->isnull();
}

daltype_t
sv_accessor<OdbcData*>::datatype() const
{
    return this->get_value()->daltype();
}





DB_NAMESPACE_END




DAL_NAMESPACE_BEGIN




///
//
IVariantValue*
OdbcData::do_deepcopy(const IVariantValue *owner) const
{
    // we make a copy of the variant_storage owner for this instance, so we can use
    // the get<T> method to reuse cast definitions.
    Variant tmp(owner->clone());

    switch(tmp.datatype())
    {
    case DAL_TYPE_CUSTOM:     return new value_traits<String>::stored_type(tmp.get<String>());
    case DAL_TYPE_UNKNOWN:    return new value_traits<String>::stored_type(tmp.get<String>());
    case DAL_TYPE_INT:        return new value_traits<signed int>::stored_type(tmp.get<signed int>());
    case DAL_TYPE_UINT:       return new value_traits<unsigned int>::stored_type(tmp.get<unsigned int>());
    case DAL_TYPE_CHAR:       return new value_traits<signed char>::stored_type(tmp.get<signed char>());
    case DAL_TYPE_UCHAR:      return new value_traits<unsigned char>::stored_type(tmp.get<unsigned char>());
    case DAL_TYPE_STRING:     return new value_traits<String>::stored_type(tmp.get<String>());
    case DAL_TYPE_BOOL:       return new value_traits<bool>::stored_type(tmp.get<bool>());
    case DAL_TYPE_SMALLINT:   return new value_traits<signed short>::stored_type(tmp.get<signed short>());
    case DAL_TYPE_USMALLINT:  return new value_traits<unsigned short>::stored_type(tmp.get<unsigned short>());
    case DAL_TYPE_BIGINT:     return new value_traits<signed long long>::stored_type(tmp.get<signed long long>());
    case DAL_TYPE_UBIGINT:    return new value_traits<unsigned long long>::stored_type(tmp.get<unsigned long long>());
    case DAL_TYPE_DATE:       return new value_traits<TDate>::stored_type(tmp.get<TDate>());
    case DAL_TYPE_TIME:       return new value_traits<TTime>::stored_type(tmp.get<TTime>());
    case DAL_TYPE_TIMESTAMP:  return new value_traits<TTimestamp>::stored_type(tmp.get<TTimestamp>());
    case DAL_TYPE_NUMERIC:    return new value_traits<TNumeric>::stored_type(tmp.get<TNumeric>());
    case DAL_TYPE_BLOB:       return new value_traits<BlobStream>::stored_type(tmp.get<BlobStream>());
    case DAL_TYPE_MEMO:       return new value_traits<MemoStream>::stored_type(tmp.get<MemoStream>());
    case DAL_TYPE_FLOAT:      return new value_traits<float>::stored_type(tmp.get<float>());
    case DAL_TYPE_DOUBLE:     return new value_traits<double>::stored_type(tmp.get<double>());
    case DAL_TYPE_INTERVAL:   return new value_traits<TInterval>::stored_type(tmp.get<TInterval>());
    case DAL_TYPE_VARBINARY:  return new value_traits<TVarbinary>::stored_type(tmp.get<TVarbinary>());
/*
  default:
  throw ex::exception(format("Unhandled datatype(%d) at %s") % int(this->daltype()) % DBWTL_MACRO_SRCPOS);
*/
    }
    assert(false);
    return 0;
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool
OdbcEnv::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const OdbcDiag&
OdbcEnv::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}



bool
OdbcStmt::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const OdbcDiag&
OdbcStmt::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}


bool
OdbcDbc::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const OdbcDiag&
OdbcDbc::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
OdbcData::OdbcData(void)
{}



///
///
OdbcData::~OdbcData(void)
{}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
OdbcTable::OdbcTable(String dbname, OdbcResult& src)
    : m_name(DAL_TYPE_STRING, "OdbcTable::name"),
      m_catalog(DAL_TYPE_STRING, "OdbcTable::catalog"),
      m_schema(DAL_TYPE_STRING, "OdbcTable::schema"),
      m_comment(DAL_TYPE_STRING, "OdbcTable::comment"),
      m_ddl(DAL_TYPE_STRING, "OdbcTable::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "OdbcTable::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}



///
///
OdbcTable::~OdbcTable(void)
{}


///
///
const ITable::value_type& OdbcTable::getName(void) const
{
    return this->m_name;
}

///
///
const ITable::value_type& OdbcTable::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const ITable::value_type& OdbcTable::getSchema(void) const
{
    return this->m_schema;
}

///
///
const ITable::value_type& OdbcTable::getComment(void) const
{
    return this->m_comment;
}

///
///
const ITable::value_type& OdbcTable::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const ITable::value_type& OdbcTable::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
OdbcIndex::OdbcIndex(String dbname, OdbcResult& src)
    : m_name(DAL_TYPE_STRING, "OdbcIndex::name"),
      m_table(DAL_TYPE_STRING, "OdbcIndex::table"),
      m_catalog(DAL_TYPE_STRING, "OdbcIndex::catalog"),
      m_schema(DAL_TYPE_STRING, "OdbcIndex::schema"),
      m_comment(DAL_TYPE_STRING, "OdbcIndex::comment"),
      m_ddl(DAL_TYPE_STRING, "OdbcIndex::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "OdbcIndex::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_table = src.column("tbl_name");
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}



///
///
OdbcIndex::~OdbcIndex(void)
{}


///
///
const IIndex::value_type& OdbcIndex::getName(void) const
{
    return this->m_name;
}

///
///
const IIndex::value_type& OdbcIndex::getTable(void) const
{
    return this->m_table;
}

///
///
const IIndex::value_type& OdbcIndex::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const IIndex::value_type& OdbcIndex::getSchema(void) const
{
    return this->m_schema;
}

///
///
const IIndex::value_type& OdbcIndex::getComment(void) const
{
    return this->m_comment;
}

///
///
const IIndex::value_type& OdbcIndex::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const IIndex::value_type& OdbcIndex::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
OdbcView::OdbcView(String dbname, OdbcResult& src)
    : m_name(DAL_TYPE_STRING, "OdbcView::name"),
      m_catalog(DAL_TYPE_STRING, "OdbcView::catalog"),
      m_schema(DAL_TYPE_STRING, "OdbcView::schema"),
      m_comment(DAL_TYPE_STRING, "OdbcView::comment"),
      m_ddl(DAL_TYPE_STRING, "OdbcView::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "OdbcView::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}



///
///
OdbcView::~OdbcView(void)
{}


///
///
const IView::value_type& OdbcView::getName(void) const
{
    return this->m_name;
}

///
///
const IView::value_type& OdbcView::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const IView::value_type& OdbcView::getSchema(void) const
{
    return this->m_schema;
}

///
///
const IView::value_type& OdbcView::getComment(void) const
{
    return this->m_comment;
}

///
///
const IView::value_type& OdbcView::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const IView::value_type& OdbcView::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
OdbcCatalog::OdbcCatalog(String dbname)
    : m_name(DAL_TYPE_STRING, "OdbcCatalg::name"),
      m_comment(DAL_TYPE_STRING, "OdbcCatalog::comment"),
      m_ddl(DAL_TYPE_STRING, "OdbcCatalog::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "OdbcCatalog::systemobject")
{
    this->m_name = dbname;
    this->m_systemobject.set(false);
}



///
///
OdbcCatalog::~OdbcCatalog(void)
{}


///
///
const ICatalog::value_type& OdbcCatalog::getName(void) const
{
    return this->m_name;
}

///
///
const ICatalog::value_type& OdbcCatalog::getComment(void) const
{
    return this->m_comment;
}

///
///
const ICatalog::value_type& OdbcCatalog::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const ICatalog::value_type& OdbcCatalog::isSystemObject(void) const
{
    return this->m_systemobject;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
OdbcDatatype::OdbcDatatype(void)
    : DatatypeBase()
{}


///
///
OdbcDatatype::~OdbcDatatype(void)
{}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



OdbcMetadata*
OdbcDbc::newMetadata(void)
{
    return new OdbcMetadata(*this);
}




static MetadataColumnDescriptor catalogsDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};

#define METADATA_DESC_COUNT(descs) (sizeof(descs)/sizeof(MetadataColumnDescriptor))

RecordSet
OdbcMetadata::getCatalogs(const Variant &catalog,
                          const ObjectClass system,
                          const DatasetFilter &filter)
{
    RecordSet rs;
    rs.setColumnCount(2);

    assert(sizeof(catalogsDescs) / sizeof(MetadataColumnDescriptor) == 2);

    for(size_t i = 1; i <= METADATA_DESC_COUNT(catalogsDescs); ++i)
    {
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, String(catalogsDescs[i-1].name));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_SIZE, int(catalogsDescs[i-1].size));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_IS_NULLABLE, bool(catalogsDescs[i-1].nullable));
        rs.setDatatype(i, catalogsDescs[i-1].daltype);
    }

    RecordSet tmp(rs);
    assert(tmp.columnCount() == rs.columnCount());
    rs.open();

    std::shared_ptr<OdbcStmt> rawStmt(this->m_dbc.getOdbcCatalogs());
    IResult &rawRes = rawStmt->resultset();


    for(rawRes.first(); !rawRes.eof(); rawRes.next())
    {
        std::cout << "read cat data" << std::endl;
        tmp.close();
        tmp.clear();
        tmp.open();
        ShrRecord rec(2);
        rec[0] = rawRes.column("TABLE_CAT");
        //rec[1] = rawRes.column("");
        tmp.insert(rec);
        tmp.first();
        if(filter(tmp))
        {
            rs.insert(*tmp.begin());
        }
    }
    return rs;
}



static MetadataColumnDescriptor schemaDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "SCHEMA_NAME",    DAL_TYPE_STRING, 0, true },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};


RecordSet
OdbcMetadata::getSchemas(const Variant &catalog,
                         const Variant &schema,
                         const ObjectClass system,
                         const DatasetFilter &filter)
{
    RecordSet rs;
    rs.setColumnCount(3);

    assert(sizeof(schemaDescs) / sizeof(MetadataColumnDescriptor) == 3);

    for(size_t i = 1; i <= METADATA_DESC_COUNT(schemaDescs); ++i)
    {
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, String(schemaDescs[i-1].name));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_SIZE, int(schemaDescs[i-1].size));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_IS_NULLABLE, bool(schemaDescs[i-1].nullable));
        rs.setDatatype(i, schemaDescs[i-1].daltype);
    }

    RecordSet tmp(rs);
    assert(tmp.columnCount() == rs.columnCount());
    rs.open();


    OdbcStmt::ptr rawStmt;


    try
    {
        rawStmt.reset(this->m_dbc.getOdbcSchemas(catalog));

    }
    catch(odbc::STATES::SQLSTATE_HYC00 &)
    {
        tmp.open();
        ShrRecord rec(3);
        rec[0] = this->m_dbc.getCurrentCatalog();
        rec[1] = Variant();
        rec[2] = String("NULL schema, driver did not support SCHEMA");
        tmp.insert(rec);
        tmp.first();
        if(filter(tmp)) rs.insert(*tmp.begin());
        return rs;
    }

    IResult &rawRes = rawStmt->resultset();

    for(rawRes.first(); !rawRes.eof(); rawRes.next())
    {
        tmp.close();
        tmp.clear();
        tmp.open();
        ShrRecord rec(3);
        rec[0] = this->m_dbc.getCurrentCatalog();
        rec[1] = rawRes.column("TABLE_SCHEM");
        rec[2] = rawRes.column("REMARKS");
        tmp.insert(rec);
        tmp.first();
        if(filter(tmp))
        {
            rs.insert(*tmp.begin());
        }
    }
    return rs;
}



static MetadataColumnDescriptor tableDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "SCHEMA_NAME",    DAL_TYPE_STRING, 0, true },
    { "TABLE_NAME",     DAL_TYPE_STRING, 0, false },
    { "TABLE_TYPE",     DAL_TYPE_STRING, 0, false },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};



RecordSet
OdbcMetadata::getTables(const Variant &schema,
                        const Variant &catalog,
                        const Variant &table,
                        const ObjectClass system,
                        const DatasetFilter &filter)
{
    RecordSet rs;
    rs.setColumnCount(5);

    assert(sizeof(tableDescs) / sizeof(MetadataColumnDescriptor) == 5);

    for(size_t i = 1; i <= METADATA_DESC_COUNT(tableDescs); ++i)
    {
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, String(tableDescs[i-1].name));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_SIZE, int(tableDescs[i-1].size));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_IS_NULLABLE, bool(tableDescs[i-1].nullable));
        rs.setDatatype(i, tableDescs[i-1].daltype);
    }

    RecordSet tmp(rs);
    assert(tmp.columnCount() == rs.columnCount());
    rs.open();


    String typeStr;

    switch(system)
    {
    case META_OBJECT_CLASS_SYSTEM:
        typeStr = "SYSTEM TABLE"; break;
    case META_OBJECT_CLASS_USER:
        typeStr = "TABLE"; break;
    case META_OBJECT_CLASS_ALL:
        typeStr = "TABLE, SYSTEM TABLE"; break;
    }


    std::shared_ptr<OdbcStmt> rawStmt(this->m_dbc.getOdbcTables(catalog, schema, typeStr));
    IResult &rawRes = rawStmt->resultset();

    const int columnsToCopy = 5;

    for(rawRes.first(); !rawRes.eof(); rawRes.next())
    {
        tmp.close();
        tmp.clear();
        tmp.open();
        tmp.insert(ShrRecord(rawRes, std::mem_fun_ref(&IResult::columnByNumber), columnsToCopy));
        tmp.first();
        if(filter(tmp))
        {
            rs.insert(*tmp.begin());
        }
    }
    return rs;
}



static MetadataColumnDescriptor columnDescs[] = {
    { "CATALOG_NAME",     DAL_TYPE_STRING,  0, true  },
    { "SCHEMA_NAME",      DAL_TYPE_STRING,  0, true  },
    { "TABLE_NAME",       DAL_TYPE_STRING,  0, true  },
    { "COLUMN_NAME",      DAL_TYPE_STRING,  0, true  },
    { "COLUMN_TYPE",      DAL_TYPE_INT,     0, true  },
    { "TYPE_NAME",        DAL_TYPE_STRING,  0, true  },
    { "COLUMN_SIZE",      DAL_TYPE_INT,     0, true  },
    { "NULLABLE",         DAL_TYPE_BOOL,    0, true  },
    { "ORDINAL_POSITION", DAL_TYPE_INT,     0, true  },
    { "COMMENT",          DAL_TYPE_STRING,  0, true  }
};


RecordSet
OdbcMetadata::getColumns(const Variant &table,
                         const Variant &schema,
                         const Variant &catalog,
                         const Variant &column,
                         const ObjectClass system,
                         const DatasetFilter &filter)
{
    RecordSet rs;
    rs.setColumnCount(10);

    assert(sizeof(columnDescs) / sizeof(MetadataColumnDescriptor) == 10);

    for(size_t i = 1; i <= METADATA_DESC_COUNT(columnDescs); ++i)
    {
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, String(columnDescs[i-1].name));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_SIZE, int(columnDescs[i-1].size));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_IS_NULLABLE, bool(columnDescs[i-1].nullable));
        rs.setDatatype(i, columnDescs[i-1].daltype);
    }

    RecordSet tmp(rs);
    assert(tmp.columnCount() == rs.columnCount());
    rs.open();

    std::shared_ptr<OdbcStmt> rawStmt(this->m_dbc.getOdbcColumns(catalog, schema, table));
    IResult &rawRes = rawStmt->resultset();


    for(rawRes.first(); !rawRes.eof(); rawRes.next())
    {
        tmp.close();
        tmp.clear();
        tmp.open();
        ShrRecord rec(10);
        rec[0] = rawRes.column("TABLE_CAT");
        rec[1] = rawRes.column("TABLE_SCHEM");
        rec[2] = rawRes.column("TABLE_NAME");
        rec[3] = rawRes.column("COLUMN_NAME");
        //rec[4] = rawRes.column("COLUMN_TYPE");
        rec[5] = rawRes.column("TYPE_NAME");
        rec[6] = rawRes.column("COLUMN_SIZE");
        rec[7] = rawRes.column("NULLABLE");
        rec[8] = rawRes.column("ORDINAL_POSITION");
        rec[9] = rawRes.column("REMARKS");
        //tmp.insert(ShrRecord(rawRes, std::mem_fun_ref(&IResult::columnByNumber), columnsToCopy));
        tmp.insert(rec);
        tmp.first();
        if(filter(tmp))
        {
            rs.insert(*tmp.begin());
        }
    }
    return rs;
}



//
TableList
OdbcDbc::getTables(const ITableFilter&)
{
    TableList list;
    OdbcStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();

    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();

        String::Internal sql_column_query =
            US(" SELECT name, sql, CASE WHEN name IN ('odbc_stat1', 'odbc_sequence') THEN 1 ELSE 0 END AS sys")
            US(" FROM ")+dbname+US(".odbc_master WHERE type = 'table'")
            US(" UNION")
            US(" SELECT 'odbc_master', NULL, 1")
            US(" UNION")
            US(" SELECT 'odbc_temp_master', NULL, 1");


        //std::cout << i18n::conv_to(sql_column_query, "UTF-8") << std::endl;

        OdbcStmt::ptr tables(this->newStatement());
        try
        {
            tables->prepare(sql_column_query);
            tables->execute();
        }
        catch(odbc::STATES::SQLSTATE_42000 &)
        {
            continue;
        }

        for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
        {
            list.push_back(new OdbcTable(dbname, tables->resultset()));
        }
    }

    // load temp tables
    String::Internal sql_column_query =
        US(" SELECT name, sql, CASE WHEN name IN ('odbc_stat1', 'odbc_sequence') THEN 1 ELSE 0 END AS sys")
        US(" FROM odbc_temp_master")
        US(" WHERE type = 'table';");

    OdbcStmt::ptr tables(this->newStatement());
    tables->prepare(sql_column_query);
    tables->execute();

    for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
    {
        list.push_back(new OdbcTable("temp", tables->resultset()));
    }

    return list;
}



//
IndexList
OdbcDbc::getIndices(const IIndexFilter&)
{
    IndexList list;
    OdbcStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();



    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();

        String::Internal sql_column_query =
            US(" SELECT name, tbl_name, sql, 0 AS sys")
            US(" FROM ")+dbname+US(".odbc_master WHERE type = 'index'");



        OdbcStmt::ptr indices(this->newStatement());
        try
        {
            indices->prepare(sql_column_query);
            indices->execute();
        }
        catch(odbc::STATES::SQLSTATE_42000 &)
        {
            continue;
        }



        for(indices->resultset().first(); ! indices->resultset().eof(); indices->resultset().next())
        {
            list.push_back(new OdbcIndex(dbname, indices->resultset()));
        }


    }

    // load temp tables
    String::Internal sql_column_query =
        US(" SELECT name, tbl_name, sql, CASE WHEN name IN ('odbc_stat1', 'odbc_sequence') THEN 1 ELSE 0 END AS sys")
        US(" FROM odbc_temp_master")
        US(" WHERE type = 'index';");




    OdbcStmt::ptr indices(this->newStatement());
    indices->prepare(sql_column_query);
    indices->execute();

    for(indices->resultset().first(); ! indices->resultset().eof(); indices->resultset().next())
    {
        list.push_back(new OdbcIndex("temp", indices->resultset()));
    }

    return list;
}


//
ProcedureList
OdbcDbc::getProcedures(const IProcedureFilter&)
{
    ProcedureList list;
    return list;
}


//
SchemaList
OdbcDbc::getSchemas(const ISchemaFilter&)
{
    SchemaList list;
    return list;
}



//
ViewList
OdbcDbc::getViews(const IViewFilter&)
{
    ViewList list;
    OdbcStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();

    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();

        String::Internal sql_column_query =
            US(" SELECT name, sql, 0 AS sys")
            US(" FROM ")+dbname+US(".odbc_master WHERE type = 'view'");

        OdbcStmt::ptr views(this->newStatement());
        try
        {
            views->prepare(sql_column_query);
            views->execute();
        }
        catch(odbc::STATES::SQLSTATE_42000 &)
        {
            continue;
        }

        for(views->resultset().first(); ! views->resultset().eof(); views->resultset().next())
        {
            list.push_back(new OdbcView(dbname, views->resultset()));
        }
    }

    // load temp views
/*
  String::Internal sql_column_query =
  US(" SELECT name, sql, CASE WHEN name IN ('odbc_stat1', 'odbc_sequence') THEN 1 ELSE 0 END AS sys")
  US(" FROM odbc_temp_master")
  US(" WHERE type = 'view';");

  OdbcStmt::ptr tables(this->newStatement());
  tables->prepare(sql_column_query);
  tables->execute();

  for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
  {
  list.push_back(new OdbcTable("temp", tables->resultset()));
  }
*/
    return list;
}


//
CatalogList
OdbcDbc::getCatalogs(const ICatalogFilter&)
{
    CatalogList list;
    OdbcStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();

    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();

        list.push_back(new OdbcCatalog(dbname));
    }

    // load temp views
/*
  String::Internal sql_column_query =
  US(" SELECT name, sql, CASE WHEN name IN ('odbc_stat1', 'odbc_sequence') THEN 1 ELSE 0 END AS sys")
  US(" FROM odbc_temp_master")
  US(" WHERE type = 'view';");

  OdbcStmt::ptr tables(this->newStatement());
  tables->prepare(sql_column_query);
  tables->execute();

  for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
  {
  list.push_back(new OdbcTable("temp", tables->resultset()));
  }
*/
    return list;
}



//
/// @todo use cache
DatatypeList
OdbcDbc::getDatatypes(const IDatatypeFilter& filter)
{
    DatatypeList dtlist;
    OdbcDatatype *dt = 0;

    dt = new OdbcDatatype();
    dt->m_name.set(String("BLOB"));
    dt->m_size.set<signed int>(-1);
    dt->m_daltype = DAL_TYPE_BLOB;
    dtlist.push_back(dt);

    dt = new OdbcDatatype();
    dt->m_name.set(String("INTEGER"));
    dt->m_size.set<signed int>(-1);
    dt->m_daltype = DAL_TYPE_BIGINT;
    dt->m_is_unsigned.set(false);
    dtlist.push_back(dt);

    dt = new OdbcDatatype();
    dt->m_name.set(String("REAL"));
    dt->m_size.set<signed int>(sizeof(double));
    dt->m_daltype = DAL_TYPE_DOUBLE;
    dt->m_is_unsigned.set(false);
    dtlist.push_back(dt);

    dt = new OdbcDatatype();
    dt->m_name.set(String("TEXT"));
    dt->m_size.set<signed int>(65000);
    dt->m_daltype = DAL_TYPE_STRING;
    dt->m_literal_prefix.set(String("'"));
    dt->m_literal_suffix.set(String("'"));
    dtlist.push_back(dt);


    return dtlist;
}



//



void
OdbcDbc::commit(Transaction trx)
{
    throw FeatureUnsuppException("ODBC has not support for transaction handles in commit()");
}

void
OdbcDbc::rollback(Transaction trx)
{
    throw FeatureUnsuppException("ODBC has not support for transaction handles in rollback()");
}


//


//
void
OdbcDbc::directCmd(String cmd)
{
    OdbcStmt::ptr stmt(this->newStatement());
    stmt->execDirect(cmd);
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
OdbcEnv::OdbcEnv(void) : EnvBase(),
                         m_diag()
{ }



//
OdbcEnv::~OdbcEnv(void)
{ }



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
OdbcVariant::OdbcVariant(OdbcData* data)
    : EngineVariant(data), m_data(data)
{ }



//
OdbcVariant::~OdbcVariant(void)
{
    //this->m_storage->release_pointee();
    delete this->m_data;
}



//
void OdbcVariant::refresh(void)
{
    //OdbcData &data = dynamic_cast<sa_base<OdbcData>&>(*this->getStorageImpl()).getValue();


    //OdbcData *data = dynamic_cast<variant_value<OdbcData*>&>(*this->getStorageImpl()).getRawValue();

    OdbcData *data = this->get<OdbcData*>();
    data->refresh();
}

void OdbcVariant::fetchParts(void)
{
    this->m_data->fetchParts();
}






//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
odbc::ENV*
odbc::createEnv(String driver)
{
    String::Internal drv;

    drv = parse_driver(driver)["driver"];
    if(! drv.length())
        goto err;

    else if(drv.compare(String("libodbc")) == 0)
        return new OdbcEnv_libodbc(parse_driver(driver)["lib"]);

/*
  else if(drv.compare("odbc") == 0)
  return new OdbcEnv_odbc(parse_driver(driver)["lib"]);
*/

    // nothing found..
err:
    throw EngineException(US("Driver '") + drv + US("' is not a valid driver name."));
}



#define DAL_NAMEOF_STATE(state) case odbc_sqlstates::DAL_ODBC_SQLSTATE_##state: return #state

const char*
odbc::sqlstate2string(STATES::engine_states_t id)
{
    switch(id)
    {
        DAL_NAMEOF_STATE(07000);
        DAL_NAMEOF_STATE(07001);
        DAL_NAMEOF_STATE(07002);
        DAL_NAMEOF_STATE(07005);
        DAL_NAMEOF_STATE(07006);
        DAL_NAMEOF_STATE(07009);
        DAL_NAMEOF_STATE(08000);
        DAL_NAMEOF_STATE(08001);
        DAL_NAMEOF_STATE(08002);
        DAL_NAMEOF_STATE(08003);
        DAL_NAMEOF_STATE(08004);
        DAL_NAMEOF_STATE(08007);
        DAL_NAMEOF_STATE(21000);
        DAL_NAMEOF_STATE(22000);
        DAL_NAMEOF_STATE(22001);
        DAL_NAMEOF_STATE(22002);
        DAL_NAMEOF_STATE(22003);
        DAL_NAMEOF_STATE(22007);
        DAL_NAMEOF_STATE(22008);
        DAL_NAMEOF_STATE(22012);
        DAL_NAMEOF_STATE(22015);
        DAL_NAMEOF_STATE(22018);
        DAL_NAMEOF_STATE(22019);
        DAL_NAMEOF_STATE(22025);
        DAL_NAMEOF_STATE(22026);
        DAL_NAMEOF_STATE(23000);
        DAL_NAMEOF_STATE(24000);
        DAL_NAMEOF_STATE(25000);
        DAL_NAMEOF_STATE(28000);
        DAL_NAMEOF_STATE(34000);
        DAL_NAMEOF_STATE(3C000);
        DAL_NAMEOF_STATE(3D000);
        DAL_NAMEOF_STATE(3F000);
        DAL_NAMEOF_STATE(42000);
        DAL_NAMEOF_STATE(44000);
        DAL_NAMEOF_STATE(HY000);
        DAL_NAMEOF_STATE(07S01);
        DAL_NAMEOF_STATE(08S01);
        DAL_NAMEOF_STATE(21S01);
        DAL_NAMEOF_STATE(21S02);
        DAL_NAMEOF_STATE(25S01);
        DAL_NAMEOF_STATE(25S02);
        DAL_NAMEOF_STATE(25S03);
        DAL_NAMEOF_STATE(42S01);
        DAL_NAMEOF_STATE(42S02);
        DAL_NAMEOF_STATE(42S11);
        DAL_NAMEOF_STATE(42S12);
        DAL_NAMEOF_STATE(42S21);
        DAL_NAMEOF_STATE(42S22);
        DAL_NAMEOF_STATE(40000);
        DAL_NAMEOF_STATE(40001);
        DAL_NAMEOF_STATE(40002);
        DAL_NAMEOF_STATE(40003);
        DAL_NAMEOF_STATE(HY001);
        DAL_NAMEOF_STATE(HY003);
        DAL_NAMEOF_STATE(HY004);
        DAL_NAMEOF_STATE(HY007);
        DAL_NAMEOF_STATE(HY008);
        DAL_NAMEOF_STATE(HY009);
        DAL_NAMEOF_STATE(HY010);
        DAL_NAMEOF_STATE(HY011);
        DAL_NAMEOF_STATE(HY012);
        DAL_NAMEOF_STATE(HY013);
        DAL_NAMEOF_STATE(HY014);
        DAL_NAMEOF_STATE(HY015);
        DAL_NAMEOF_STATE(HY016);
        DAL_NAMEOF_STATE(HY017);
        DAL_NAMEOF_STATE(HY018);
        DAL_NAMEOF_STATE(HY019);
        DAL_NAMEOF_STATE(HY020);
        DAL_NAMEOF_STATE(HY021);
        DAL_NAMEOF_STATE(HY024);
        DAL_NAMEOF_STATE(HY090);
        DAL_NAMEOF_STATE(HY091);
        DAL_NAMEOF_STATE(HY092);
        DAL_NAMEOF_STATE(HY095);
        DAL_NAMEOF_STATE(HY096);
        DAL_NAMEOF_STATE(HY097);
        DAL_NAMEOF_STATE(HY098);
        DAL_NAMEOF_STATE(HY099);
        DAL_NAMEOF_STATE(HY100);
        DAL_NAMEOF_STATE(HY101);
        DAL_NAMEOF_STATE(HY103);
        DAL_NAMEOF_STATE(HY104);
        DAL_NAMEOF_STATE(HY105);
        DAL_NAMEOF_STATE(HY106);
        DAL_NAMEOF_STATE(HY107);
        DAL_NAMEOF_STATE(HY109);
        DAL_NAMEOF_STATE(HY110);
        DAL_NAMEOF_STATE(HY111);
        DAL_NAMEOF_STATE(HYC00);
        DAL_NAMEOF_STATE(HYT00);
        DAL_NAMEOF_STATE(HYT01);
        DAL_NAMEOF_STATE(IM000);
        DAL_NAMEOF_STATE(IM001);
        DAL_NAMEOF_STATE(IM002);
        DAL_NAMEOF_STATE(IM003);
        DAL_NAMEOF_STATE(IM004);
        DAL_NAMEOF_STATE(IM005);
        DAL_NAMEOF_STATE(IM006);
        DAL_NAMEOF_STATE(IM007);
        DAL_NAMEOF_STATE(IM008);
        DAL_NAMEOF_STATE(IM009);
        DAL_NAMEOF_STATE(IM010);
        DAL_NAMEOF_STATE(IM011);
        DAL_NAMEOF_STATE(IM012);
        DAL_NAMEOF_STATE(IM013);
        DAL_NAMEOF_STATE(IM014);
        DAL_NAMEOF_STATE(IM015);
    }
    DBWTL_BUG_EX("Unhandled internal SQLSTATE. Please report this bug!");
}
#undef DAL_NAMEOF_STATE





static odbc_sqlstates::engine_states_t sqlstate2id(std::string sqlstate)
{
    if(sqlstate == "07000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_07000;
    else if(sqlstate == "07001") return odbc_sqlstates::DAL_ODBC_SQLSTATE_07001;
    else if(sqlstate == "07002") return odbc_sqlstates::DAL_ODBC_SQLSTATE_07002;
    else if(sqlstate == "07005") return odbc_sqlstates::DAL_ODBC_SQLSTATE_07005;
    else if(sqlstate == "07006") return odbc_sqlstates::DAL_ODBC_SQLSTATE_07006;
    else if(sqlstate == "07009") return odbc_sqlstates::DAL_ODBC_SQLSTATE_07009;
    else if(sqlstate == "08000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_08000;
    else if(sqlstate == "08001") return odbc_sqlstates::DAL_ODBC_SQLSTATE_08001;
    else if(sqlstate == "08002") return odbc_sqlstates::DAL_ODBC_SQLSTATE_08002;
    else if(sqlstate == "08003") return odbc_sqlstates::DAL_ODBC_SQLSTATE_08003;
    else if(sqlstate == "08004") return odbc_sqlstates::DAL_ODBC_SQLSTATE_08004;
    else if(sqlstate == "08007") return odbc_sqlstates::DAL_ODBC_SQLSTATE_08007;
    else if(sqlstate == "21000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_21000;
    else if(sqlstate == "22000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22000;
    else if(sqlstate == "22001") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22001;
    else if(sqlstate == "22002") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22002;
    else if(sqlstate == "22003") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22003;
    else if(sqlstate == "22007") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22007;
    else if(sqlstate == "22008") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22008;
    else if(sqlstate == "22012") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22012;
    else if(sqlstate == "22015") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22015;
    else if(sqlstate == "22018") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22018;
    else if(sqlstate == "22019") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22019;
    else if(sqlstate == "22025") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22025;
    else if(sqlstate == "22026") return odbc_sqlstates::DAL_ODBC_SQLSTATE_22026;
    else if(sqlstate == "23000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_23000;
    else if(sqlstate == "24000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_24000;
    else if(sqlstate == "25000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_25000;
    else if(sqlstate == "28000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_28000;
    else if(sqlstate == "34000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_34000;
    else if(sqlstate == "3C000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_3C000;
    else if(sqlstate == "3D000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_3D000;
    else if(sqlstate == "3F000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_3F000;
    else if(sqlstate == "42000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_42000;
    else if(sqlstate == "44000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_44000;
    else if(sqlstate == "HY000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY000;
    else if(sqlstate == "07S01") return odbc_sqlstates::DAL_ODBC_SQLSTATE_07S01;
    else if(sqlstate == "08S01") return odbc_sqlstates::DAL_ODBC_SQLSTATE_08S01;
    else if(sqlstate == "21S01") return odbc_sqlstates::DAL_ODBC_SQLSTATE_21S01;
    else if(sqlstate == "21S02") return odbc_sqlstates::DAL_ODBC_SQLSTATE_21S02;
    else if(sqlstate == "25S01") return odbc_sqlstates::DAL_ODBC_SQLSTATE_25S01;
    else if(sqlstate == "25S02") return odbc_sqlstates::DAL_ODBC_SQLSTATE_25S02;
    else if(sqlstate == "25S03") return odbc_sqlstates::DAL_ODBC_SQLSTATE_25S03;
    else if(sqlstate == "42S01") return odbc_sqlstates::DAL_ODBC_SQLSTATE_42S01;
    else if(sqlstate == "42S02") return odbc_sqlstates::DAL_ODBC_SQLSTATE_42S02;
    else if(sqlstate == "42S11") return odbc_sqlstates::DAL_ODBC_SQLSTATE_42S11;
    else if(sqlstate == "42S12") return odbc_sqlstates::DAL_ODBC_SQLSTATE_42S12;
    else if(sqlstate == "42S21") return odbc_sqlstates::DAL_ODBC_SQLSTATE_42S21;
    else if(sqlstate == "42S22") return odbc_sqlstates::DAL_ODBC_SQLSTATE_42S22;
    else if(sqlstate == "40001") return odbc_sqlstates::DAL_ODBC_SQLSTATE_40001;
    else if(sqlstate == "40002") return odbc_sqlstates::DAL_ODBC_SQLSTATE_40002;
    else if(sqlstate == "40003") return odbc_sqlstates::DAL_ODBC_SQLSTATE_40003;
    else if(sqlstate == "HY001") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY001;
    else if(sqlstate == "HY003") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY003;
    else if(sqlstate == "HY004") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY004;
    else if(sqlstate == "HY007") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY007;
    else if(sqlstate == "HY008") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY008;
    else if(sqlstate == "HY009") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY009;
    else if(sqlstate == "HY010") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY010;
    else if(sqlstate == "HY011") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY011;
    else if(sqlstate == "HY012") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY012;
    else if(sqlstate == "HY013") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY013;
    else if(sqlstate == "HY014") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY014;
    else if(sqlstate == "HY015") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY015;
    else if(sqlstate == "HY016") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY016;
    else if(sqlstate == "HY017") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY017;
    else if(sqlstate == "HY018") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY018;
    else if(sqlstate == "HY019") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY019;
    else if(sqlstate == "HY020") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY020;
    else if(sqlstate == "HY021") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY021;
    else if(sqlstate == "HY024") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY024;
    else if(sqlstate == "HY090") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY090;
    else if(sqlstate == "HY091") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY091;
    else if(sqlstate == "HY092") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY092;
    else if(sqlstate == "HY095") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY095;
    else if(sqlstate == "HY096") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY096;
    else if(sqlstate == "HY097") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY097;
    else if(sqlstate == "HY098") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY098;
    else if(sqlstate == "HY099") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY099;
    else if(sqlstate == "HY100") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY100;
    else if(sqlstate == "HY101") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY101;
    else if(sqlstate == "HY103") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY103;
    else if(sqlstate == "HY104") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY104;
    else if(sqlstate == "HY105") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY105;
    else if(sqlstate == "HY106") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY106;
    else if(sqlstate == "HY107") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY107;
    else if(sqlstate == "HY109") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY109;
    else if(sqlstate == "HY110") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY110;
    else if(sqlstate == "HY111") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY111;
    else if(sqlstate == "HYC00") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HYC00;
    else if(sqlstate == "HYT00") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HYT00;
    else if(sqlstate == "HYT01") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HYT01;
    else if(sqlstate == "IM000") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM000;
    else if(sqlstate == "IM001") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM001;
    else if(sqlstate == "IM002") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM002;
    else if(sqlstate == "IM003") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM003;
    else if(sqlstate == "IM004") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM004;
    else if(sqlstate == "IM005") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM005;
    else if(sqlstate == "IM006") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM006;
    else if(sqlstate == "IM007") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM007;
    else if(sqlstate == "IM008") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM008;
    else if(sqlstate == "IM009") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM009;
    else if(sqlstate == "IM010") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM010;
    else if(sqlstate == "IM011") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM011;
    else if(sqlstate == "IM012") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM012;
    else if(sqlstate == "IM013") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM013;
    else if(sqlstate == "IM014") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM014;
    else if(sqlstate == "IM015") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM015;
    else
    {
        if(sqlstate.substr(0, 2) == "28") return odbc_sqlstates::DAL_ODBC_SQLSTATE_28000;
        else if(sqlstate.substr(0, 2) == "HY") return odbc_sqlstates::DAL_ODBC_SQLSTATE_HY000;
        else if(sqlstate.substr(0, 2) == "IM") return odbc_sqlstates::DAL_ODBC_SQLSTATE_IM000;
        else
            throw EngineException(FORMAT1("Unknown SQLSTATE for exception mapping: %s", String(sqlstate)));
    }
}




//
OdbcDiag::OdbcDiag(dalstate_t state,
                   CodePosInfo pos,
                   Variant what,
                   String description,
                   const std::string &sqlstate,
                   int nativecode)
    : DiagBase(state, pos, what, description),
      m_sqlstate_id(),
      m_sqlstate(sqlstate),
      m_nativecode(nativecode)
{
    m_sqlstate_id = sqlstate2id(sqlstate);
}


//
OdbcDiag::OdbcDiag(const OdbcDiag& ref)
    : DiagBase(ref),
      m_sqlstate_id(ref.m_sqlstate_id),
      m_sqlstate(ref.m_sqlstate),
      m_nativecode(ref.m_nativecode)
{}





#define DAL_THROW_STATE(state)                      \
    case odbc_sqlstates::DAL_ODBC_SQLSTATE_##state: \
    throw odbc::STATES::SQLSTATE_##state(*this)

//
void
OdbcDiag::raiseException(void) const
{
    switch(this->m_sqlstate_id)
    {
        DAL_THROW_STATE(07000);
        DAL_THROW_STATE(07001);
        DAL_THROW_STATE(07002);
        DAL_THROW_STATE(07005);
        DAL_THROW_STATE(07006);
        DAL_THROW_STATE(07009);
        DAL_THROW_STATE(08000);
        DAL_THROW_STATE(08001);
        DAL_THROW_STATE(08002);
        DAL_THROW_STATE(08003);
        DAL_THROW_STATE(08004);
        DAL_THROW_STATE(08007);
        DAL_THROW_STATE(21000);
        DAL_THROW_STATE(22000);
        DAL_THROW_STATE(22001);
        DAL_THROW_STATE(22002);
        DAL_THROW_STATE(22003);
        DAL_THROW_STATE(22007);
        DAL_THROW_STATE(22008);
        DAL_THROW_STATE(22012);
        DAL_THROW_STATE(22015);
        DAL_THROW_STATE(22018);
        DAL_THROW_STATE(22019);
        DAL_THROW_STATE(22025);
        DAL_THROW_STATE(22026);
        DAL_THROW_STATE(23000);
        DAL_THROW_STATE(24000);
        DAL_THROW_STATE(25000);
        DAL_THROW_STATE(28000);
        DAL_THROW_STATE(34000);
        DAL_THROW_STATE(3C000);
        DAL_THROW_STATE(3D000);
        DAL_THROW_STATE(3F000);
        DAL_THROW_STATE(42000);
        DAL_THROW_STATE(44000);
        DAL_THROW_STATE(HY000);
        DAL_THROW_STATE(07S01);
        DAL_THROW_STATE(08S01);
        DAL_THROW_STATE(21S01);
        DAL_THROW_STATE(21S02);
        DAL_THROW_STATE(25S01);
        DAL_THROW_STATE(25S02);
        DAL_THROW_STATE(25S03);
        DAL_THROW_STATE(42S01);
        DAL_THROW_STATE(42S02);
        DAL_THROW_STATE(42S11);
        DAL_THROW_STATE(42S12);
        DAL_THROW_STATE(42S21);
        DAL_THROW_STATE(42S22);
        DAL_THROW_STATE(40000);
        DAL_THROW_STATE(40001);
        DAL_THROW_STATE(40002);
        DAL_THROW_STATE(40003);
        DAL_THROW_STATE(HY001);
        DAL_THROW_STATE(HY003);
        DAL_THROW_STATE(HY004);
        DAL_THROW_STATE(HY007);
        DAL_THROW_STATE(HY008);
        DAL_THROW_STATE(HY009);
        DAL_THROW_STATE(HY010);
        DAL_THROW_STATE(HY011);
        DAL_THROW_STATE(HY012);
        DAL_THROW_STATE(HY013);
        DAL_THROW_STATE(HY014);
        DAL_THROW_STATE(HY015);
        DAL_THROW_STATE(HY016);
        DAL_THROW_STATE(HY017);
        DAL_THROW_STATE(HY018);
        DAL_THROW_STATE(HY019);
        DAL_THROW_STATE(HY020);
        DAL_THROW_STATE(HY021);
        DAL_THROW_STATE(HY024);
        DAL_THROW_STATE(HY090);
        DAL_THROW_STATE(HY091);
        DAL_THROW_STATE(HY092);
        DAL_THROW_STATE(HY095);
        DAL_THROW_STATE(HY096);
        DAL_THROW_STATE(HY097);
        DAL_THROW_STATE(HY098);
        DAL_THROW_STATE(HY099);
        DAL_THROW_STATE(HY100);
        DAL_THROW_STATE(HY101);
        DAL_THROW_STATE(HY103);
        DAL_THROW_STATE(HY104);
        DAL_THROW_STATE(HY105);
        DAL_THROW_STATE(HY106);
        DAL_THROW_STATE(HY107);
        DAL_THROW_STATE(HY109);
        DAL_THROW_STATE(HY110);
        DAL_THROW_STATE(HY111);
        DAL_THROW_STATE(HYC00);
        DAL_THROW_STATE(HYT00);
        DAL_THROW_STATE(HYT01);
        DAL_THROW_STATE(IM000);
        DAL_THROW_STATE(IM001);
        DAL_THROW_STATE(IM002);
        DAL_THROW_STATE(IM003);
        DAL_THROW_STATE(IM004);
        DAL_THROW_STATE(IM005);
        DAL_THROW_STATE(IM006);
        DAL_THROW_STATE(IM007);
        DAL_THROW_STATE(IM008);
        DAL_THROW_STATE(IM009);
        DAL_THROW_STATE(IM010);
        DAL_THROW_STATE(IM011);
        DAL_THROW_STATE(IM012);
        DAL_THROW_STATE(IM013);
        DAL_THROW_STATE(IM014);
        DAL_THROW_STATE(IM015);
    }
    DBWTL_BUG_EX("Unhandled internal SQLSTATE. Please report this bug!");
}

#undef DAL_THROW_STATE


DAL_NAMESPACE_END
