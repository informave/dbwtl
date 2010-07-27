//
// sqlite_libsqlite.cc - SQLite Backend libsqlite (definition)
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
/// @brief SQLite Backend libsqlite (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "sqlite_libsqlite.hh"
#include "dbwtl/db_exceptions.hh"
#include "dbwtl/stdext/functional"
#include "../dal_debug.hh"
#include "../sqlutils.hh"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <sstream>



#define DRV_STATE(stateobj, dalstate, msg, drvcode, sqlstate)   \
    {                                                           \
        stateobj.setDALCode(dalstate);                          \
        stateobj.setMsg(msg);                                   \
        stateobj.setDrvCode(drvcode, L"SQLITE_XYZ");            \
        stateobj.setSource(__FILE__, __FUNCTION__);             \
        stateobj.setSQLState(sqlstate);                         \
    }


DAL_NAMESPACE_BEGIN



struct delete_object
{
    template <typename T>
    void operator()(T ptr){ delete ptr;}

    template<typename U, typename V>
    void operator()(std::pair<U, V> val){ delete val.second;}
};



static void delete_resultset(SqliteResult_libsqlite* rs)
{
    delete rs;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//
//
SqliteBlob_libsqlite::SqliteBlob_libsqlite(const SqliteData_libsqlite& data,
                                           const void *buf, size_t size)
    : SqliteBlob(),
      m_data(data),
      m_buf(static_cast<const unsigned char*>(buf)),
      m_size(size)
{
    char* ptr = const_cast<char*>(static_cast<const char*>(buf));/// @bug test workaround, remove me!
    this->setg(ptr, ptr, ptr+size);
}


//
//
SqliteBlob_libsqlite::~SqliteBlob_libsqlite(void)
{
}

///
/// @todo move to base class
SqliteBlob_libsqlite::int_type
SqliteBlob_libsqlite::underflow()
{
    
    if (gptr() < egptr())
        return *gptr();
    else
        return traits_type::eof();
}


///
///
void
SqliteBlob_libsqlite::reset_ptr(const void *buf, size_t size)
{
    this->m_buf = static_cast<const unsigned char*>(buf);
    this->m_size = size;
    char* ptr = const_cast<char*>(static_cast<const char*>(buf));/// @bug test workaround, remove me!
    this->setg(ptr, ptr, ptr+size);
}






//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
/// @bug locked is not used, remove it
SqliteData_libsqlite::SqliteData_libsqlite(SqliteResult_libsqlite& result, colnum_t colnum, bool locked)
    : SqliteData(),
      m_resultset(result),
      m_colnum(colnum),
      m_locked(locked), /// @todo is not longer required
      m_rowid(DAL_TYPE_ROWID_NPOS), /// @todo is not longer required
      m_blobbuf()
{ }



//
SqliteData_libsqlite::~SqliteData_libsqlite(void)
{ }



//
SqliteBlob_libsqlite*
SqliteData_libsqlite::getBlob(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);

    size_t size = 0;
    const void *buf = NULL;

    // get ptr if column is not null
    if(! this->isnull())
    {
        size_t size = this->m_resultset.drv()->sqlite3_column_bytes(this->m_resultset.getHandle(),
                                                                    this->m_colnum - 1);
        const void* buf = this->m_resultset.drv()->sqlite3_column_blob(this->m_resultset.getHandle(),
                                                                       this->m_colnum - 1);
    }

    // prepare or reset stream buffer
    if(this->m_blobbuf.get())
    {
        this->m_blobbuf->reset_ptr(buf, size);
    }
    else
    {
        this->m_blobbuf.reset(new SqliteBlob_libsqlite(*this, buf, size));
    }

    // everything is up-to-date now...
    if(this->isnull())
        throw ex::null_value(i18n::UString(L"null value in SqliteData_libsqlite"));
    else
        return this->m_blobbuf.get();
}



//
double 
SqliteData_libsqlite::getDouble(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw ex::null_value(i18n::UString(L"null value in SqliteData_libsqlite"));
    else
        return this->m_resultset.drv()->sqlite3_column_double(this->m_resultset.getHandle(), this->m_colnum - 1);
}



//
int
SqliteData_libsqlite::getInt(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw ex::null_value(i18n::UString(L"null value in SqliteData_libsqlite"));
    else
        return this->m_resultset.drv()->sqlite3_column_int(this->m_resultset.getHandle(), this->m_colnum - 1);
}



//
int64_t  
SqliteData_libsqlite::getInt64(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw ex::null_value(i18n::UString(L"null value in SqliteData_libsqlite"));
    else
        return this->m_resultset.drv()->sqlite3_column_int64(this->m_resultset.getHandle(), this->m_colnum - 1);
}



//
const char*
SqliteData_libsqlite::getText(void) const
{
    DALTRACE("VISIT");

    assert(this->m_colnum > 0);
    if(this->isnull())
        throw ex::null_value(i18n::UString(L"null value in SqliteData_libsqlite"));
    else
    {
        const unsigned char *s = this->m_resultset.drv()->
            sqlite3_column_text(this->m_resultset.getHandle(), this->m_colnum - 1);
        if(!s)
            throw ex::null_value(i18n::UString(L"null value in SqliteData_libsqlite"));
        return reinterpret_cast<const char*>(s);
    }
}



//
const void*
SqliteData_libsqlite::getText16(void) const
{
    DALTRACE("VISIT");

    assert(this->m_colnum > 0);
    if(this->isnull())
        throw ex::null_value(i18n::UString(L"null value in SqliteData_libsqlite"));
    else
    {
        const void *s = this->m_resultset.drv()->
            sqlite3_column_text16(this->m_resultset.getHandle(), this->m_colnum - 1);
        if(!s)
            throw ex::null_value(i18n::UString(L"null value in SqliteData_libsqlite"));
        return s;
    }
}



//
bool 
SqliteData_libsqlite::isnull(void) const
{
    DALTRACE("VISIT");

    assert(this->m_colnum > 0);
    return this->m_resultset.drv()->sqlite3_column_type(this->m_resultset.getHandle(),
                                                        this->m_colnum - 1)
        == SQLITE_NULL ? true : false;
}



//
rowid_t 
SqliteData_libsqlite::getCurrentRowID(void) const
{
    DALTRACE("VISIT");
    return this->m_resultset.getCurrentRowID();
}



//
i18n::UString
SqliteData_libsqlite::getString(void) const
{
    const char* s = this->getText();

    return i18n::conv_from(s, "UTF-8");
}


//
daltype_t
SqliteData_libsqlite::daltype(void) const
{
    return this->m_resultset.describeColumn(this->m_colnum).getDatatype();
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SqliteResult_libsqlite::SqliteResult_libsqlite(SqliteStmt_libsqlite& stmt)
    : SqliteResult(),
      m_stmt(stmt),
      m_handle(NULL),
      m_current_tuple(DAL_TYPE_ROWID_NPOS),
      m_last_row_status(0),
      m_isopen(false),
      m_column_desc(),
      m_column_accessors(),
      m_field_accessors(),
      m_allocated_accessors()
{ }



//
SqliteResult_libsqlite::~SqliteResult_libsqlite(void)
{
    this->m_column_accessors.clear();
    this->m_field_accessors.clear();
    this->m_allocated_accessors.clear();
    this->close();
}



//
void
SqliteResult_libsqlite::prepare(i18n::UString sql)
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->m_stmt.getDbc().isConnected())
        throw ex::engine_error(L"not connected");

    // if anything is currently open, we need to close.
    // This removes all binded vars, too.
    if(this->isPrepared())
        this->close();

    std::string sql_e = i18n::conv_to(sql, "UTF-8");

    int err = this->drv()->sqlite3_prepare_v2(this->m_stmt.getDbc().getHandle(),
                                              sql_e.c_str(), sql_e.size(), &this->m_handle,  NULL);
    switch(err)
    {
    case SQLITE_OK:
        this->m_isPrepared = true;
        break;
    default:
        sqlite::STATE state;
        const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
        i18n::UString u_msg = i18n::conv_from(msg, "UTF-8");
        DRV_STATE(state, DALSTATE_SQL_ERROR, u_msg, err, "HY000");
        throw ex::sql_error(state, sql, u_msg);
        break;
    };

    DALTRACE_LEAVE;
}



//
void
SqliteResult_libsqlite::execute(StmtBase::ParamMap& params)
{
    DALTRACE_ENTER;
    sqlite::STATE state;
    std::map<int, std::string> tmp_strings;

    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error(L"Resultset is not prepared.");

    /// @bug fix this, required for bind() exec(), exec(), ... 
    /// the state muss be resetted
    this->drv()->sqlite3_reset(this->getHandle());

    StmtBase::ParamMapIterator param;
    for(param = params.begin(); param != params.end(); ++param)
    {
        IVariant *var = param->second;
        std::stringstream tmp_stream;


        int err;

        if(var->isnull())
            continue;

        switch(var->datatype())
        {
        case DAL_TYPE_SMALLINT:
        case DAL_TYPE_INT:
        case DAL_TYPE_BOOL:
            err = this->drv()->sqlite3_bind_int(this->getHandle(), param->first, var->asInt());
            break;
            
            /*
              case DAL_TYPE_BITINT:
              err = this->drv()->sqlite3_bind_int64(this->m_stmt.getHandle(), pcount, var->asBigint());
              break;            
              case DAL_TYPE_FLOAT:
              case DAL_TYPE_DOUBLE:
              err = this->drv()->sqlite3_bind_double(this->m_stmt.getHandle(), pcount, var->asDouble());
              break;
            */
        case DAL_TYPE_BLOB:
            if(var->asBlob())
            {
                tmp_stream << var->asBlob();
                
                std::string& tmp_string = tmp_strings[param->first];
                tmp_string = tmp_stream.str();
                err = this->drv()->sqlite3_bind_blob(this->getHandle(), param->first, tmp_string.c_str(),
                                                     tmp_string.size(), NULL);
            }
            break;
        default:
            err = this->drv()->sqlite3_bind_text(this->getHandle(), param->first,
                                                 var->asNarrowStr("UTF-8").c_str(), -1, SQLITE_TRANSIENT);
        }
        switch(err)
        {
        case SQLITE_OK:
            break;
        case SQLITE_NOMEM:
            throw std::bad_alloc();
        case SQLITE_RANGE:
            throw ex::not_found(L"param not found");
            //break; // throw exception::RangeError;
        default:
            const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
            i18n::UString u_msg = i18n::conv_from(msg, "UTF-8");
            DRV_STATE(state, DALSTATE_SQL_ERROR, u_msg, err, "HY000");
            throw ex::engine_error(state);
            break;
        };
    }

    this->m_last_row_status = this->drv()->sqlite3_step(this->m_handle);
    switch(this->m_last_row_status)
    {
    case SQLITE_OK:
    case SQLITE_ROW:
    case SQLITE_DONE:
        this->m_current_tuple = 1;
        this->m_isOpen = true;
        this->refreshMetadata();
        break;
    case SQLITE_ERROR:
    case SQLITE_INTERNAL:
    case SQLITE_PERM:
    case SQLITE_ABORT:
    case SQLITE_BUSY:
    case SQLITE_LOCKED:
    case SQLITE_NOMEM:
    case SQLITE_READONLY:
    case SQLITE_INTERRUPT:
    case SQLITE_IOERR:
    case SQLITE_CORRUPT:
    case SQLITE_NOTFOUND:
    case SQLITE_FULL:
    case SQLITE_EMPTY:
    case SQLITE_SCHEMA:
    case SQLITE_TOOBIG:
    case SQLITE_CONSTRAINT:
    case SQLITE_MISMATCH:
    case SQLITE_MISUSE:
    case SQLITE_NOLFS:
    case SQLITE_AUTH:
    case SQLITE_FORMAT:
    case SQLITE_RANGE:
        
    default:
        const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
        i18n::UString u_msg = i18n::conv_from(msg, "UTF-8");
        DRV_STATE(state, DALSTATE_SQL_ERROR, u_msg, this->m_last_row_status, "HY000");
        throw ex::engine_error(state);
        break;
    };
    DALTRACE_LEAVE;
}



//
size_t    
SqliteResult_libsqlite::paramCount(void) const
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error(L"Resultset is not prepared.");

    int count = this->drv()->sqlite3_bind_parameter_count(this->getHandle());
    return count;
}



//
void 
SqliteResult_libsqlite::first(void)
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");

    if(this->m_current_tuple != 1)
    {
        throw std::runtime_error("can't scroll to first record");
    }
}



//
void
SqliteResult_libsqlite::next(void)
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");

    this->m_last_row_status = this->drv()->sqlite3_step(this->getHandle());

    switch(this->m_last_row_status)
    {   
    case SQLITE_ROW:
        std::for_each(this->m_column_accessors.begin(),
                      this->m_column_accessors.end(),
                      informave::stdext::compose1(std::mem_fun(&SqliteVariant::refresh),
                                                  informave::stdext::select2nd<VariantListT::value_type>()));
        ++this->m_current_tuple;
        break;
    case SQLITE_DONE:
        break;
    case SQLITE_BUSY:
        //break;
    default:
        sqlite::STATE state;
        const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
        i18n::UString u_msg = i18n::conv_from(msg, "UTF-8");
        DRV_STATE(state, DALSTATE_SQL_ERROR, u_msg, this->m_last_row_status, "HY000");
        throw ex::engine_error(state);
        break;
    };
}



//
bool  
SqliteResult_libsqlite::eof(void) const
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");

    return this->m_last_row_status != SQLITE_ROW;
}



//
void   
SqliteResult_libsqlite::close(void)
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(this->m_handle)
    {
        int err = this->drv()->sqlite3_finalize(this->m_handle);
        switch(err)
        {
        case SQLITE_OK:
            this->m_handle = 0;
            break;
        default:
            sqlite::STATE state;
            const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
            i18n::UString u_msg = i18n::conv_from(msg, "UTF-8");
            DRV_STATE(state, DALSTATE_ERROR, u_msg, err, "HY000");
            throw ex::engine_error(state);
            break;            
        };
    }
}



//
rowcount_t
SqliteResult_libsqlite::rowCount(void) const
{
    // SQLite can't tell you the row count
    return -1;
}



rowcount_t
SqliteResult_libsqlite::affectedRows(void) const
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error(L"Resultset is not prepared.");

    int count = this->drv()->sqlite3_changes(this->m_stmt.getDbc().getHandle());
    return count;
}



//
const SqliteVariant&
SqliteResult_libsqlite::column(i18n::UString name)
{
    colnum_t num = this->columnID(name);
    return this->column(num);
}



//
/*
  const SqliteVariant&
  SqliteResult_libsqlite::field(i18n::UString name)
  {
  colnum_t num = this->columnID(name);
  return this->field(num);
  }
*/


//
const SqliteVariant&
SqliteResult_libsqlite::column(colnum_t num)
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");

    if(num > this->columnCount())
        throw ex::not_found(L"column number out of range");


    VariantListT::iterator p = this->m_column_accessors.find(num);
    if(this->m_column_accessors.end() == p)
    {
        std::pair<VariantListT::iterator,bool> r;
        SqliteVariant* v = new SqliteVariant(new SqliteData_libsqlite(*this, num, false));
        this->m_allocated_accessors.push_back(v); // smart ptr
        r = this->m_column_accessors.insert(VariantListT::value_type(num, v));
        p = r.first;
    }
    DALTRACE_LEAVE;
    return *(p->second);
}



//
/*
  SqliteVariant&
  SqliteResult_libsqlite::field(colnum_t num)
  {
  DALTRACE_ENTER;

  if(this->isBad())
  throw ex::engine_error(L"Resultset is in bad state.");

  if(! this->isOpen())
  throw ex::engine_error(L"Resultset is not open.");

  VariantListT::iterator p = this->m_field_accessors.find(num);
  if(this->m_field_accessors.end() == p)
  {
  std::pair<VariantListT::iterator,bool> r;
  SqliteVariant* v = new SqliteVariant(new SqliteData_libsqlite(*this, num, true));
  this->m_allocated_accessors.push_back(v);
  r = this->m_field_accessors.insert(VariantListT::value_type(num, v));
  p = r.first;
  }
  DALTRACE_LEAVE;
  return *(p->second);
  }
*/




//
rowid_t
SqliteResult_libsqlite::getCurrentRowID(void) const
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");

    return this->m_current_tuple;
}



//
size_t    
SqliteResult_libsqlite::columnCount(void) const
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");

    int c = this->drv()->sqlite3_column_count(this->getHandle());
    DAL_DEBUG("Query column count: " << c);
    DALTRACE_LEAVE;
    return c;
}



//
colnum_t
SqliteResult_libsqlite::columnID(i18n::UString name) const
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");

    for(colnum_t i = 1; i <= this->columnCount(); ++i)
    {
        if(name == this->columnName(i))
            return i;
    }
    throw std::runtime_error("column not found");
}



//
i18n::UString
SqliteResult_libsqlite::columnName(colnum_t num) const
{
    DALTRACE("VISIT");

    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");

    return this->describeColumn(num).getName().asStr(); /// @todo return empty string if null
}



//
SQLite3Drv*
SqliteResult_libsqlite::drv(void) const
{
    return this->getDbc().drv();
}



//
SqliteDbc_libsqlite& 
SqliteResult_libsqlite::getDbc(void) const
{
    return this->m_stmt.getDbc();
}




//
//
SqliteColumnDesc_libsqlite::SqliteColumnDesc_libsqlite(colnum_t i, SqliteResult_libsqlite &result)
    : SqliteColumnDesc()
{
    if(i == 0)
    {
        //this->m_daltype = DAL_TYPE_BOOKMARK;
        this->m_name.setStr(L"__DBWTL_BOOKMARK__");
    }
    else
    {
        // set name
        const char *s = result.drv()->sqlite3_column_name(result.getHandle(), i-1);
        this->m_name.setStr(s ? s : "", "UTF-8");

        // set type
        const char *type = result.drv()->sqlite3_column_decltype(result.getHandle(), i-1);
        SqlTypeParser pt;
        pt.registerType(DAL_TYPE_VARCHAR, L"TEXT*");
        pt.parse(i18n::conv_from(type, "UTF-8"));
        this->m_type_name.setStr(daltype2sqlname(pt.getDaltype()));
        this->m_daltype = pt.getDaltype();
        this->m_size.setInt(pt.getSize());
        this->m_precision.setUSmallint(pt.getPrecision());
        this->m_scale.setUSmallint(pt.getSize());
    }
}


//
const SqliteColumnDesc&
SqliteResult_libsqlite::describeColumn(colnum_t num) const
{
    if(this->isBad())
        throw ex::engine_error(L"Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error(L"Resultset is not open.");


    std::map<colnum_t, SqliteColumnDesc_libsqlite>::const_iterator i =
        this->m_column_desc.find(num);

    if(i == this->m_column_desc.end())
        throw ex::not_found(L"foo");
    else
        return i->second;
   
   
}


//
const SqliteColumnDesc&
SqliteResult_libsqlite::describeColumn(i18n::UString name) const
{
    DBWTL_NOTIMPL();
}


void
SqliteResult_libsqlite::refreshMetadata(void)
{
    DALTRACE_ENTER;
    this->m_column_desc.clear();
    if(! this->getHandle())
        return;
    
    size_t colcount = this->columnCount();
    SqliteColumnDesc desc;
    //desc.m_name.setStr(L"foo");
    //for(size_t i = 0; i < colCount; ++i)
    //
    
    for(size_t i = 0; i <= colcount; ++i)
    {
        SqliteColumnDesc_libsqlite x(i, *this);
        //x = y;
        this->m_column_desc.insert(std::pair<colnum_t, SqliteColumnDesc_libsqlite>(i, x));
    }
}

//
#if 0
void      
SqliteResult_libsqlite::refreshMetadata(void)
{
    DALTRACE_ENTER;
    this->m_column_meta.clear();
    if( ! this->getHandle())
        return;

    size_t colCount = this->columnCount();
    // add bookmark column
    this->m_column_meta.push_back(SqliteTypeInfo(DAL_TYPE_CHAR)); // change to TYPE_BOOKMARK
    for(size_t i = 0; i < colCount; ++i)
    {
        daltype_t daltype = DAL_TYPE_CUSTOM;
        const char *type = this->drv()->sqlite3_column_decltype(this->getHandle(), i);
        if(type)
        {
            std::string s(type);
            if(s == "INTEGER") daltype = DAL_TYPE_INT;
            else if(s == "VARCHAR") daltype = DAL_TYPE_VARCHAR;
            else if(s == "BLOB") daltype = DAL_TYPE_BLOB;
            else daltype = DAL_TYPE_CUSTOM;
        }        
        SqliteTypeInfo info(daltype);

        this->m_column_meta.push_back(info);
    }
    DALTRACE_LEAVE;
}
#endif


// delete me
SQLite3Drv* 
SqliteResult_libsqlite::getDriver(void) const
{
    DBWTL_NOTIMPL();
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
SqliteEnv_libsqlite::SqliteEnv_libsqlite(i18n::UString lib)
    : SqliteEnv(),
      m_lib()
{
    this->m_lib.reset(new SQLite3Drv()); /// @todo pass lib path
}



//
SqliteEnv_libsqlite::~SqliteEnv_libsqlite(void)
{ }



//
SqliteDbc_libsqlite*
SqliteEnv_libsqlite::newConnection(void)
{
    return new SqliteDbc_libsqlite(*this);
}



//
SQLite3Drv* 
SqliteEnv_libsqlite::drv() const
{
    return this->m_lib.get();
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
SqliteDbc_libsqlite::SqliteDbc_libsqlite(SqliteEnv_libsqlite& env)
    : SqliteDbc(),
      m_lib(env.drv()),
      m_dbh(0)
{ }



//
SqliteDbc_libsqlite::~SqliteDbc_libsqlite(void)
{
    this->disconnect();
}



//
SqliteStmt_libsqlite*       
SqliteDbc_libsqlite::newStatement(void)
{
    return new SqliteStmt_libsqlite(*this);
}



//
dalstate_t
SqliteDbc_libsqlite::connect(i18n::UString database,
                             i18n::UString user,
                             i18n::UString password)
{
    IDbc::Options options;
    
    options[ L"database" ] = database;
    return this->connect(options);
}



//
dalstate_t    
SqliteDbc_libsqlite::connect(IDbc::Options& options)
{
    DALTRACE_ENTER;
    sqlite::STATE state;

    try
    {
        std::string dbc_db = i18n::conv_to(options[ L"database" ], "UTF-8");

        int err = this->drv()->sqlite3_open_v2(dbc_db.c_str(), &this->m_dbh,
                                               SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                               NULL);
        // allocation error?
        if(this->m_dbh == NULL)
            throw std::bad_alloc();
        switch(err)
        {
        case SQLITE_OK:
            DRV_STATE(state, DALSTATE_OK, DAL_MSG_SUCCESS, err, "00000");
            this->m_isConnected = true;
            break;
        default:
            const char *msg = this->drv()->sqlite3_errmsg(this->m_dbh);
            i18n::UString u_msg = i18n::conv_from(msg, "UTF-8");
            DRV_STATE(state, DALSTATE_BAD_CONNECTION, u_msg, err, "HY000");
            break;
        };
    }
    catch(ex::exception& e)
    {
        DRV_STATE(state, DALSTATE_API_ERR, e.getMessage(), 0, "HY000");
    }

    if(this->isConnected())
        this->setDbcEncoding("UTF-8");

    DALTRACE_LEAVE;
    return state;
}



//
dalstate_t
SqliteDbc_libsqlite::disconnect(void)
{
    DALTRACE_ENTER;
    sqlite::STATE state;
    
    if(this->isConnected())
    {
        DALTRACE("is connected, disconnecting...");
        int err = this->drv()->sqlite3_close(this->m_dbh);
        switch(err)
        {
        case SQLITE_OK:
            this->m_dbh = 0;
            this->m_isConnected = false;
            DRV_STATE(state, DALSTATE_OK, DAL_MSG_SUCCESS, err, "00000");
            break;
        case SQLITE_BUSY:
            
        default:
            const char *msg = this->drv()->sqlite3_errmsg(this->m_dbh);
            i18n::UString u_msg = i18n::conv_from(msg, "UTF-8");
            DRV_STATE(state, DALSTATE_BAD_CONNECTION, u_msg, err, "HY000");
            break;            
        }
    }
    DALTRACE_LEAVE;
    return state;
}



//
::sqlite3*
SqliteDbc_libsqlite::getHandle(void) const
{
    assert(this->m_dbh);
    return this->m_dbh;
}



//
i18n::UString 
SqliteDbc_libsqlite::driverName(void) const
{
    return L"SQLite (libsqlite)";
}



//
i18n::UString 
SqliteDbc_libsqlite::dbmsName(void) const
{
    const char *s = this->drv()->sqlite3_libversion();
    return L"SQLite " + i18n::conv_from(s, "UTF-8");
}



//
SQLite3Drv*
SqliteDbc_libsqlite::getDriver(void) const
{
    assert(this->m_lib);
    return this->m_lib;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
SqliteStmt_libsqlite::SqliteStmt_libsqlite(SqliteDbc_libsqlite& conn)
    : m_conn(conn),
      m_resultsets(),
      m_currentResultset(0),
      m_isPrepared(false)
{ }



//
SqliteStmt_libsqlite::~SqliteStmt_libsqlite(void)
{
    using std::for_each;
    using std::mem_fun;

    for_each(this->m_resultsets.begin(),
             this->m_resultsets.end(),
             delete_resultset);
    this->m_resultsets.clear();
}



//
SqliteResult& 
SqliteStmt_libsqlite::resultset(void)
{
    return *this->m_resultsets.at(this->m_currentResultset);
}



//
const SqliteResult& 
SqliteStmt_libsqlite::resultset(void) const
{
    return *this->m_resultsets.at(this->m_currentResultset);
}



//
/// @todo split sql
void  
SqliteStmt_libsqlite::prepare(i18n::UString sql)
{
    SqliteResult_libsqlite* rs = this->newResultset();
    rs->prepare(sql);
    this->m_isPrepared = true;
}



bool   
SqliteStmt_libsqlite::isPrepared(void) const
{
    return this->m_isPrepared;
}



//
void   
SqliteStmt_libsqlite::execute(void)
{
    DALTRACE_ENTER;

    for(ResultsetVectorT::iterator i = this->m_resultsets.begin();
        i != this->m_resultsets.end();
        ++i)
    {             
        (*i)->execute(this->m_params);
    }
    this->m_currentResultset = 0;
    DALTRACE_LEAVE;
}



//
void  
SqliteStmt_libsqlite::execDirect(i18n::UString sql)
{ 
    this->prepare(sql);
    this->execute();
}



//
void   
SqliteStmt_libsqlite::close(void)
{
    DALTRACE_ENTER;
    using std::for_each;
    using std::mem_fun;

    // clear resultsets from statement
    for_each(this->m_resultsets.begin(),
             this->m_resultsets.end(),
             delete_object());

    this->m_resultsets.clear();


    

    // clear params from prepared statements
    // clean only PodVariant types with special delete flag?!
    /*
      for_each(this->m_paramlist.begin(),
      this->m_paramlist.end(),
      delete_object());
    */


    SqliteStmt::close();

    /* now done in StmtBase?
       this->m_paramlist.clear(); // better use smartobjects?
       for_each(this->m_paramlistPod.begin(),
       this->m_paramlistPod.end(),
       delete_object());

       this->m_paramlistPod.clear();
    */

    DALTRACE_LEAVE;
}



//
bool 
SqliteStmt_libsqlite::nextResultset(void)
{
    return false;
}



//
bool  
SqliteStmt_libsqlite::moreResultsets(void) const
{
    return false;
}



//
size_t  
SqliteStmt_libsqlite::paramCount(void) const
{
    return this->resultset().paramCount();
}



//
rowid_t 
SqliteStmt_libsqlite::affectedRows(void) const
{
    return this->resultset().affectedRows();
}



//
SQLite3Drv* 
SqliteStmt_libsqlite::drv(void) const
{
    return this->getDbc().drv();
}



//
SqliteDbc_libsqlite& 
SqliteStmt_libsqlite::getDbc(void) const
{
    return this->m_conn;
}



//
SqliteResult_libsqlite*
SqliteStmt_libsqlite::newResultset(void)
{
    this->m_resultsets.push_back(new SqliteResult_libsqlite(*this));
    return this->m_resultsets.at(this->m_resultsets.size()-1);
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




std::string  
SqliteDbc_libsqlite::getDbcEncoding(void) const
{
    return "UTF-8";
}

void    
SqliteDbc_libsqlite::setDbcEncoding(std::string encoding)
{
    // nothing to do, SQLite only can handle UTF-8 and UTF-16
}






DAL_NAMESPACE_END
