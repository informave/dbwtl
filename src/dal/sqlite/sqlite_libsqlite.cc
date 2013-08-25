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
#include "dbwtl/variant.hh"
//#include "dbwtl/db_exceptions.hh"
#include "dbwtl/stdext/functional"
#include "../dal_debug.hh"
#include "../sqlutils.hh"
#include "../../utils.hh"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <sstream>



#define DRV_STATE(stateobj, dalstate, msg, drvcode, sqlstate)   \
    {                                                           \
        stateobj.setDALCode(dalstate);                          \
        stateobj.setMsg(msg);                                   \
        stateobj.setDrvCode(drvcode, "SQLITE_XYZ");            \
        stateobj.setSource(__FILE__, __FUNCTION__);             \
        stateobj.setSQLState(sqlstate);                         \
    }



#define DAL_SQLITE_LIBSQLITE_DIAG_ERROR(handle, msg, desc, code, excode) \
    {                                                                   \
        sqlite::DIAG *__dal__diag =                                     \
            new SqliteDiag_libsqlite(DAL_STATE_ERROR,                   \
                                     DBWTL_SPOS(),                      \
                                     String(msg), desc, code, excode);  \
        handle->m_diag.push_back(__dal__diag);                          \
        __dal__diag->raiseException();                                  \
    } 



DAL_NAMESPACE_BEGIN


/*
struct delete_object
{
    template <typename T>
    void operator()(T ptr){ delete ptr;}

    template<typename U, typename V>
    void operator()(std::pair<U, V> val){ delete val.second;}
};
*/


static void delete_resultset(SqliteResult_libsqlite* rs)
{
    delete rs;
}




SqliteDiag&
SqliteEnv_libsqlite::appendDiagRec(const SqliteDiag &diag)
{
    
    return *this->m_diag.push_back(diag.clone());
}

SqliteDiag&
SqliteResult_libsqlite::appendDiagRec(const SqliteDiag &diag)
{
    
    return *this->m_diag.push_back(diag.clone());
}

SqliteDiag&
SqliteDbc_libsqlite::appendDiagRec(const SqliteDiag &diag)
{
    
    return *this->m_diag.push_back(diag.clone());
}

SqliteDiag&
SqliteStmt_libsqlite::appendDiagRec(const SqliteDiag &diag)
{
    
    return *this->m_diag.push_back(diag.clone());
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//
//
SqliteBlob_libsqlite::SqliteBlob_libsqlite(const SqliteData_libsqlite& data)
    : SqliteBlob(),
      m_data(data),
      m_buf(0),
      m_buf_end(0),
      m_cur(0)
{}


//
//
SqliteBlob_libsqlite::~SqliteBlob_libsqlite(void)
{}


//
//
SqliteBlob_libsqlite::int_type
SqliteBlob_libsqlite::underflow()
{
    if(this->m_cur == this->m_buf_end)
        return traits_type::eof();
    else
        return *this->m_cur;
}


//
//
SqliteBlob_libsqlite::pos_type
SqliteBlob_libsqlite::seekpos(SqliteBlob_libsqlite::pos_type p, std::ios_base::openmode m)
{
    std::cout << "seek" << std::endl;
    if(m & std::ios_base::in)
    {
        this->m_cur = this->m_buf + p > this->m_buf_end ? this->m_buf_end : this->m_buf + p;
        return this->m_cur - this->m_buf;
    }
    return traits_type::eof();
}


//
//
SqliteBlob_libsqlite::pos_type
SqliteBlob_libsqlite::seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode m)
{
    if(m & std::ios_base::in)
    {
        switch(way)
        {
        case std::ios_base::beg:
            this->m_cur = this->m_buf + off > this->m_buf_end ? this->m_buf_end : this->m_buf + off;
            return this->m_cur - this->m_buf;

        case std::ios_base::cur:
            this->m_cur = this->m_cur + off > this->m_buf_end ? this->m_buf_end : this->m_cur + off;
            return this->m_cur - this->m_buf;

        case std::ios_base::end:
            this->m_cur = this->m_buf_end + off > this->m_buf_end ? this->m_buf_end : this->m_buf_end + off;
            return this->m_cur - this->m_buf;
        default:
            break;
            // catchall for GCC warnings (gcc defines more than the dree ways)
        }
    }
    return traits_type::eof();    
}


//
//
SqliteBlob_libsqlite::int_type
SqliteBlob_libsqlite::uflow()
{
    if(this->m_cur == this->m_buf_end)
        return traits_type::eof();
    else
        return *this->m_cur++;
}


//
//
SqliteBlob_libsqlite::int_type
SqliteBlob_libsqlite::pbackfail(int_type ch)
{
    if(this->m_cur == this->m_buf || (ch != traits_type::eof() && ch != this->m_cur[-1]))
        return traits_type::eof();
    else
        return *--this->m_cur;
}


//
//
std::streamsize
SqliteBlob_libsqlite::showmanyc(void)
{
    return this->m_buf_end - this->m_cur;
}


//
//
std::streamsize
SqliteBlob_libsqlite::xsgetn(char_type *ch, std::streamsize n)
{
    std::streamsize cp = n > showmanyc() ? showmanyc() : n;
    std::memcpy(ch, this->m_cur, cp * sizeof(traits_type::char_type));
    this->m_cur += cp;
    return cp;
}


///
///
void
SqliteBlob_libsqlite::setBufPtr(const unsigned char *buf, size_t size)
{
    this->m_buf = buf;
    this->m_buf_end = buf + size;
    this->m_cur = buf;
}


//
//
bool
SqliteBlob_libsqlite::isNull(void) const
{
    return (this->m_buf == this->m_buf_end);
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
SqliteData_libsqlite::SqliteData_libsqlite(SqliteResult_libsqlite& result, colnum_t colnum, bool locked)
    : SqliteData(),
      m_resultset(result),
      m_colnum(colnum),
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

    if(this->isnull())
        throw ex::null_value(String("SqliteData_libsqlite result column"));


    if(! this->m_blobbuf.get())
    {
        this->m_blobbuf.reset(new SqliteBlob_libsqlite(*this));
    }


    // the streambuffer is set to null on each fetch() (through refresh())
    if(this->m_blobbuf->isNull())
    {
        // get ptr if column is not null
        size = this->m_resultset.drv()->sqlite3_column_bytes(this->m_resultset.getHandle(),
                                                             this->m_colnum - 1);
        buf = this->m_resultset.drv()->sqlite3_column_blob(this->m_resultset.getHandle(),
                                                           this->m_colnum - 1);
        this->m_blobbuf->setBufPtr(static_cast<const unsigned char*>(buf), size);
    }
    return this->m_blobbuf.get();
}



//
double 
SqliteData_libsqlite::getDouble(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw ex::null_value(String("SqliteData_libsqlite result column"));
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
        throw ex::null_value(String("SqliteData_libsqlite result column"));
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
        throw ex::null_value(String("SqliteData_libsqlite result column"));
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
        throw ex::null_value(String("SqliteData_libsqlite result column"));
    else
    {
        const unsigned char *s = this->m_resultset.drv()->
            sqlite3_column_text(this->m_resultset.getHandle(), this->m_colnum - 1);
        if(!s)
            throw ex::null_value(String("SqliteData_libsqlite result column"));
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
        throw ex::null_value(String("SqliteData_libsqlite result column"));
    else
    {
        const void *s = this->m_resultset.drv()->
            sqlite3_column_text16(this->m_resultset.getHandle(), this->m_colnum - 1);
        if(!s)
            throw ex::null_value(String("SqliteData_libsqlite result column"));
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
void
SqliteData_libsqlite::refresh(void)
{
    if(this->m_blobbuf.get())
    {
        this->m_blobbuf->setBufPtr(0, 0); // reset the buffer
    }
}


//
String
SqliteData_libsqlite::getString(void) const
{
    const char* s = this->getText();

    return String(s, "UTF-8");
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
    : SqliteResult(stmt.m_diag),
      m_stmt(stmt),
      m_handle(NULL),
      m_current_tuple(DAL_TYPE_ROWID_NPOS),
      m_last_row_status(0),
      m_isopen(false),
      m_column_desc(),
      m_column_accessors(),
      m_allocated_accessors()
{ }



//
SqliteResult_libsqlite::~SqliteResult_libsqlite(void)
{
    this->m_column_accessors.clear();
    this->m_allocated_accessors.clear();
    this->close();
}



//
void
SqliteResult_libsqlite::prepare(String sql)
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->m_stmt.getDbc().isConnected())
        throw ex::engine_error("not connected");

    // if anything is currently open, we need to close.
    // This removes all binded vars, too.
    if(this->isPrepared())
        this->close();

    std::string sql_e = sql.to("UTF-8");

    int err = this->drv()->sqlite3_prepare_v2(this->m_stmt.getDbc().getHandle(),
                                              sql_e.c_str(), sql_e.size(), &this->m_handle,  NULL);
    switch(err)
    {
    case SQLITE_OK:
        this->m_cursorstate |= DAL_CURSOR_PREPARED;
        break;
    default:
        const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
        String u_msg(msg, "UTF-8");
        DAL_SQLITE_LIBSQLITE_DIAG_ERROR(this,
                                        String("Can not prepare query: ") + sql,
                                        u_msg,
                                        this->drv()->sqlite3_errcode(this->m_stmt.getDbc().getHandle()),
                                        this->drv()->sqlite3_extended_errcode(this->m_stmt.getDbc().getHandle()));
        break;
    };

    DALTRACE_LEAVE;
}



//
void
SqliteResult_libsqlite::execute(StmtBase::ParamMap& params)
{
    DALTRACE_ENTER;
    std::map<int, std::string> tmp_strings;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");
    
    /// If the resultset is already open, we cleanup all bindings
    /// because the current bindings are maintained by this class instead
    /// of SQLite.
    if(this->m_cursorstate & DAL_CURSOR_OPEN)
    {
        this->drv()->sqlite3_reset(this->getHandle());
        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_PREPARED);
    }

    StmtBase::ParamMapIterator param;
    for(param = params.begin(); param != params.end(); ++param)
    {
        Variant *var = param->second;
        std::stringstream tmp_stream;


        int err = SQLITE_OK;

        if(var->isnull())
        {
            err = this->drv()->sqlite3_bind_null(this->getHandle(), param->first);
        }
        else
        {
            switch(var->datatype())
            {
            case DAL_TYPE_INT:
            case DAL_TYPE_UINT:
            case DAL_TYPE_CHAR:
            case DAL_TYPE_UCHAR:
            case DAL_TYPE_BOOL:
            case DAL_TYPE_SMALLINT:
            case DAL_TYPE_USMALLINT:
                err = this->drv()->sqlite3_bind_int(this->getHandle(), param->first, var->asInt());
                break;

            case DAL_TYPE_BIGINT:
            case DAL_TYPE_UBIGINT:
                err = this->drv()->sqlite3_bind_int64(this->getHandle(), param->first, var->asBigint());
                break;
            
            case DAL_TYPE_FLOAT:
            case DAL_TYPE_DOUBLE:
                err = this->drv()->sqlite3_bind_double(this->getHandle(), param->first, var->asDouble());
                break;

            case DAL_TYPE_BLOB:
	    	DBWTL_BUGCHECK(var->get<BlobStream>().rdbuf());
                {
                    tmp_stream << var->get<BlobStream>().rdbuf();
                    
                    std::string& tmp_string = tmp_strings[param->first];
                    tmp_string = tmp_stream.str();
                    err = this->drv()->sqlite3_bind_blob(this->getHandle(), param->first, tmp_string.c_str(),
                                                         tmp_string.size(), NULL);
                }
                break;

            default:
                // all other types are passed as string
                err = this->drv()->sqlite3_bind_text(this->getHandle(), param->first,
                                                     var->asStr().to("UTF-8"), -1, 
                                                     SQLITE_TRANSIENT);
                break;
            }
        }
        switch(err)
        {
        case SQLITE_OK:
            break;

        case SQLITE_NOMEM:
            throw std::bad_alloc();

        case SQLITE_RANGE:
            throw ex::not_found("Parameter number out of range");

        default:
            const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
            String u_msg(msg, "UTF-8");
            DAL_SQLITE_LIBSQLITE_DIAG_ERROR(this,
                                            "Can not execute query",
                                            u_msg,
                                            this->drv()->sqlite3_errcode(this->m_stmt.getDbc().getHandle()),
                                            this->drv()->sqlite3_extended_errcode(this->m_stmt.getDbc().getHandle()));
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
        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_POSITIONED);
        this->refreshMetadata();
        break;

    default:
        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_BAD);
        const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
        String u_msg(msg, "UTF-8");
        DAL_SQLITE_LIBSQLITE_DIAG_ERROR(this,
                                        "Can not execute query",
                                        u_msg,
                                        this->drv()->sqlite3_errcode(this->m_stmt.getDbc().getHandle()),
                                        this->drv()->sqlite3_extended_errcode(this->m_stmt.getDbc().getHandle()));
        break;
    };
    DALTRACE_LEAVE;
}



//
size_t    
SqliteResult_libsqlite::paramCount(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");

    int count = this->drv()->sqlite3_bind_parameter_count(this->getHandle());
    return count;
}



//
void 
SqliteResult_libsqlite::first(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    if(this->m_current_tuple != 1)
    {
        throw ex::engine_error("can't scroll to first record");
    }
}



//
bool
SqliteResult_libsqlite::next(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    this->m_last_row_status = this->drv()->sqlite3_step(this->getHandle());

    switch(this->m_last_row_status)
    {   
    case SQLITE_ROW:
        // refresh all column accessors
        std::for_each(this->m_column_accessors.begin(),
                      this->m_column_accessors.end(),
                      [](VariantListT::value_type& p)
                      {
                          p.second->refresh();
                      });
        ++this->m_current_tuple;
        break;
    case SQLITE_DONE:
        break;
    case SQLITE_BUSY:
        //break;
    default:
        const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
        String u_msg(msg, "UTF_8");
        DAL_SQLITE_LIBSQLITE_DIAG_ERROR(this,
                                        "Can not prepare query",
                                        u_msg,
                                        this->drv()->sqlite3_errcode(this->m_stmt.getDbc().getHandle()),
                                        this->drv()->sqlite3_extended_errcode(this->m_stmt.getDbc().getHandle()));
        break;
    };

    return !this->eof();
}



//
bool  
SqliteResult_libsqlite::eof(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    return this->m_last_row_status != SQLITE_ROW;
}



//
void   
SqliteResult_libsqlite::close(void)
{
/*
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");
*/

    if(this->m_handle)
    {
        int err = this->drv()->sqlite3_finalize(this->m_handle);
        /*
        Do not check error code, because it returns the last error of step().

        switch(err)
        {
        case SQLITE_OK:
            this->m_handle = 0;
            break;
        default:

            const char *msg = this->drv()->sqlite3_errmsg(this->m_stmt.getDbc().getHandle());
            String u_msg(msg, "UTF-8");
            DAL_SQLITE_LIBSQLITE_DIAG_ERROR(this,
                                            "Can not prepare query",
                                            u_msg,
                                            this->drv()->sqlite3_errcode(this->m_stmt.getDbc().getHandle()),
                                            this->drv()->sqlite3_extended_errcode(this->m_stmt.getDbc().getHandle()));
            break;
        };
        */
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
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");

    int count = this->drv()->sqlite3_changes(this->m_stmt.getDbc().getHandle());
    return count;
}


//
Variant
SqliteResult_libsqlite::lastInsertRowId(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");

    long long id = this->drv()->sqlite3_last_insert_rowid(this->m_stmt.getDbc().getHandle());
    
    return Variant(id);
}




//
const SqliteVariant&
SqliteResult_libsqlite::column(String name)
{
    colnum_t num = this->columnID(name);
    return this->column(num);
}



//
const SqliteVariant&
SqliteResult_libsqlite::column(colnum_t num)
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    if(num > this->columnCount())
        throw ex::not_found("column number out of range");


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
rowid_t
SqliteResult_libsqlite::getCurrentRowID(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    return this->m_current_tuple;
}



//
size_t    
SqliteResult_libsqlite::columnCount(void) const
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    int c = this->drv()->sqlite3_column_count(this->getHandle());
    DAL_DEBUG("Query column count: " << c);
    DALTRACE_LEAVE;
    return c;
}



//
colnum_t
SqliteResult_libsqlite::columnID(String name) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    for(colnum_t i = 1; i <= this->columnCount(); ++i)
    {
        if(name == this->columnName(i))
            return i;
    }
    throw ex::not_found(US("Column '") + String::Internal(name) + US("' not found."));
}



//
String
SqliteResult_libsqlite::columnName(colnum_t num) const
{
    DALTRACE("VISIT");

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    return this->describeColumn(num).getName().asStr();
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
        this->m_daltype = DAL_TYPE_UNKNOWN;
        this->m_name.set(String("__DBWTL_BOOKMARK__"));
    }
    else
    {
        // set name
        const char *s = result.drv()->sqlite3_column_name(result.getHandle(), i-1);
        this->m_name.set(String(s ? s : "", "UTF-8"));

        // set type
        const char *type = result.drv()->sqlite3_column_decltype(result.getHandle(), i-1);

        SqlTypeParser pt;
        pt.registerType(DAL_TYPE_STRING, US("TEXT*"));
        pt.parse(String(type, "UTF-8"));
        this->m_type_name.set(daltype2sqlname(pt.getDaltype()));
        this->m_daltype = pt.getDaltype();
        this->m_size.set<signed int>(pt.getSize());
        this->m_precision.set<unsigned short>(pt.getPrecision());
        this->m_scale.set<unsigned short>(pt.getSize());
    }
}


//
const SqliteColumnDesc&
SqliteResult_libsqlite::describeColumn(colnum_t num) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");


    std::map<colnum_t, SqliteColumnDesc_libsqlite>::const_iterator i =
        this->m_column_desc.find(num);

    if(i == this->m_column_desc.end())
    {
        throw ex::not_found(US("Column '") + String::Internal(Variant(int(num)).asStr()) + US("' not found."));
    }
    else
        return i->second;
}


//
const SqliteColumnDesc&
SqliteResult_libsqlite::describeColumn(String name) const
{
    colnum_t num = this->columnID(name);
    return this->describeColumn(num);
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
    
    for(size_t i = 0; i <= colcount; ++i)
    {
        SqliteColumnDesc_libsqlite x(i, *this);
        this->m_column_desc.insert(std::pair<colnum_t, SqliteColumnDesc_libsqlite>(i, x));
    }
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
SqliteEnv_libsqlite::SqliteEnv_libsqlite(String lib)
    : SqliteEnv(),
      m_lib()
{
    this->m_lib.reset(new SQLite3Drv(lib));
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
      m_dbh(0),
	  m_env(env)
{ }

IEnv&
	SqliteDbc_libsqlite::getEnv(void)
{
	return this->m_env;
}

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
void
SqliteDbc_libsqlite::connect(String database,
                             String user,
                             String password)
{
    IDbc::Options options;
    
    options[ "database" ] = database;
    return this->connect(options);
}




static sqlite_sqlstates::engine_states_t sqlite3error_to_sqlstate(int code)
{
    switch(code)
    {
    case SQLITE_ERROR:         /* SQL error or missing database */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_INTERNAL:       /* Internal logic error in SQLite */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_PERM:           /* Access permission denied */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_28000;
    case SQLITE_ABORT:          /* Callback routine requested an abort */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_BUSY:           /* The database file is locked */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_25001;
    case SQLITE_LOCKED:         /* A table in the database is locked */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_NOMEM:          /* A malloc() failed */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_READONLY:       /* Attempt to write a readonly database */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_25006;
    case SQLITE_INTERRUPT:      /* Operation terminated by sqlite3_interrupt()*/
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_IOERR:          /* Some kind of disk I/O error occurred */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_CORRUPT:        /* The database disk image is malformed */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_NOTFOUND:       /* NOT USED. Table or record not found */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_FULL:           /* Insertion failed because database is full */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_CANTOPEN:       /* Unable to open the database file */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_08001;
    case SQLITE_PROTOCOL:       /* Database lock protocol error */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_EMPTY:          /* Database is empty */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_SCHEMA:         /* The database schema changed */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_TOOBIG:         /* String or BLOB exceeds size limit */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_22000;
    case SQLITE_CONSTRAINT:     /* Abort due to constraint violation */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_23000;
    case SQLITE_MISMATCH:       /* Data type mismatch */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_MISUSE:         /* Library used incorrectly */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_NOLFS:          /* Uses OS features not supported on host */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_0A000;
    case SQLITE_AUTH:           /* Authorization denied */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_28000;
    case SQLITE_FORMAT:         /* Auxiliary database format error */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_42000;
    case SQLITE_RANGE:          /* 2nd parameter to sqlite3_bind out of range */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_22000;
    case SQLITE_NOTADB:         /* File opened that is not a database file */
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_08001;
    default:
        return sqlite_sqlstates::DAL_SQLITE_SQLSTATE_08000;
        //throw sqlite::STATES::SQLSTATE_08006(*this);
    }
}


//
SqliteDiag_libsqlite::SqliteDiag_libsqlite(dalstate_t state,
                                           CodePosInfo pos,
                                           Variant what,
                                           String description,
                                           int sqlite_code,
                                           int sqlite_excode)
    : SqliteDiag(state, pos, what, description),
      m_sqlite_code(DAL_TYPE_STRING, "SqliteDiag::sqlite_code"),
      m_sqlite_excode(DAL_TYPE_STRING, "SqliteDiag::sqlite_excode")
{
    m_sqlite_code = sqlite_code;
    m_sqlite_excode = sqlite_excode;

    m_sqlstate_id = sqlite3error_to_sqlstate(sqlite_code);
    m_sqlstate.set(String(sqlite::sqlstate2string(m_sqlstate_id), "UTF-8"));
}


//
SqliteDiag_libsqlite*
SqliteDiag_libsqlite::clone(void) const
{
    return new SqliteDiag_libsqlite(*this);
}


//
SqliteDiag_libsqlite::SqliteDiag_libsqlite(const SqliteDiag_libsqlite& ref)
    : SqliteDiag(ref),
      m_sqlite_code(ref.m_sqlite_code),
      m_sqlite_excode(ref.m_sqlite_excode)
{}



//
SqliteDiag_libsqlite::~SqliteDiag_libsqlite(void)

{}



//
String
SqliteDiag_libsqlite::str(void) const
{
    std::wstringstream ss;

    ss << L"[SQLSTATE:" << ifnull<String>(this->m_sqlstate, L"fooo") << L"] "
       << ifnull<String>(this->m_message, L"No message") << std::endl
       << ifnull<String>(this->m_description, L"No description") << std::endl
       << L"SQLite errcode: " << ifnull<String>(this->m_sqlite_code, L"NULL")
       << L" (" << ifnull<String>(this->m_sqlite_excode, L"NULL") << L")" << std::endl
       << L"Raised at: " 
       << L" [" << this->m_codepos.str() << "]";

    return ss.str();
}



//
void
SqliteDbc_libsqlite::connect(IDbc::Options& options)
{
    DALTRACE_ENTER;


    std::string dbc_db(options[ "database" ].to("UTF-8"));

    int err = this->drv()->sqlite3_open_v2(dbc_db.c_str(), &this->m_dbh,
                                           SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE,
                                           NULL);
    // allocation error?
    if(this->m_dbh == NULL)
        throw std::bad_alloc();
    switch(err)
    {
    case SQLITE_OK:
        this->m_isConnected = true;
        break;
    default:
        const char *msg = this->drv()->sqlite3_errmsg(this->m_dbh);
        String::Internal u_msg = US("Database: ") + String::Internal(options[ "database" ])
            + US("\r\n") + String::Internal(String(msg, "UTF-8"));

        DAL_SQLITE_LIBSQLITE_DIAG_ERROR(this,
                                        "connection failure",
                                        u_msg,
                                        this->drv()->sqlite3_errcode(this->m_dbh),
                                        this->drv()->sqlite3_extended_errcode(this->m_dbh));

        break;
    };


    if(this->isConnected())
        this->setDbcEncoding("UTF-8");

    DALTRACE_LEAVE;
}



//
void
SqliteDbc_libsqlite::disconnect(void)
{
    DALTRACE_ENTER;
    
    if(this->m_dbh)
    {
        DALTRACE("is connected, disconnecting...");
        int err = this->drv()->sqlite3_close(this->m_dbh);
        switch(err)
        {
        case SQLITE_OK:
            this->m_dbh = 0;
            this->m_isConnected = false;
            break;

        case SQLITE_BUSY:            
        default:
            const char *msg = this->drv()->sqlite3_errmsg(this->m_dbh);
            String u_msg(msg, "UTF-8");
            DAL_SQLITE_LIBSQLITE_DIAG_ERROR(this,
                                            "connection failure",
                                            u_msg,
                                            this->drv()->sqlite3_errcode(this->m_dbh),
                                            this->drv()->sqlite3_extended_errcode(this->m_dbh));

            break;
        }
    }

    DALTRACE_LEAVE;
}



//
::sqlite3*
SqliteDbc_libsqlite::getHandle(void) const
{
    assert(this->m_dbh);
    return this->m_dbh;
}



//
String 
SqliteDbc_libsqlite::driverName(void) const
{
    return "SQLite (libsqlite)";
}



//
String 
SqliteDbc_libsqlite::dbmsName(void) const
{
    const char *s = this->drv()->sqlite3_libversion();
    return US("SQLite ") + String::Internal(String(s, "UTF-8"));
}



//
SQLite3Drv*
SqliteDbc_libsqlite::drv(void) const
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
      m_currentResultset(0)
{ }



//
SqliteStmt_libsqlite::~SqliteStmt_libsqlite(void)
{
    this->close();
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



/// @todo For now, prepare() only supports a single statement.
/// It is planned to support multiple statements.
void  
SqliteStmt_libsqlite::prepare(String sql)
{
    /// For SQLite, it is save to create a new resultset
    /// and execute the query inside of the resultset, because
    /// a query can never return more then one resultset.
    SqliteResult_libsqlite* rs = this->newResultset();
    rs->prepare(sql);
    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_PREPARED);
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
SqliteStmt_libsqlite::execDirect(String sql)
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

    /// Params and accessors are cleaned up by the base class.
    SqliteStmt::close();

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
Variant
SqliteStmt_libsqlite::lastInsertRowId(void)
{
    return this->resultset().lastInsertRowId();
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
