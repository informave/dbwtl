//
// firebird_libfbclient.cc - Firebird Backend libfirebird (definition)
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
/// @brief Firebird Backend libfirebird (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "firebird_libfbclient.hh"
#include "dbwtl/variant.hh"
#include "dbwtl/db_exceptions.hh"
#include "dbwtl/stdext/functional"
#include "../dal_debug.hh"
#include "../../devutils.hh"
#include "../sqlutils.hh"
#include "../../utils.hh"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <ios>


DAL_NAMESPACE_BEGIN

#define DAL_FIREBIRD_BLOBBUF_SIZE (1024*4)

static inline ISC_DATE date2iscdate(TDate date, FBClientDrv *drv);
static inline ISC_TIME date2isctime(TTime time, FBClientDrv *drv);
static inline ISC_TIMESTAMP date2isctimestamp(TTimestamp timestamp, FBClientDrv *drv);
static inline ISC_SCHAR* allocspace(size_t len);
static firebird_sqlstates::engine_states_t gdscode2sqlstate(ISC_STATUS code);


//..............................................................................
///////////////////////////////////////////////////////////// Diagnostic helpers


/// @details
/// Creates a new Diagnostic record based on information from the ISC_STATUS
/// array.
static FirebirdDiag_libfbclient
handle_isc_error(FBClientDrv *drv, ISC_STATUS *sv, CodePosInfo pos, Variant v)
{
    DBWTL_BUGCHECK_SOFT(drv);
    DBWTL_BUGCHECK_SOFT(sv);
    DBWTL_BUGCHECK_SOFT(!v.isnull());
    DBWTL_BUGCHECK(sv[0] == 1 && sv[1] > 0);

    ISC_STATUS *svp = sv;
    char msg[512];
    std::stringstream ss;
    while(drv->isc_interprete(msg, &svp))
        ss << msg << std::endl;
    FirebirdDiag_libfbclient rec(DAL_STATE_ERROR, sv[1], pos, v, ss.str());
    return rec;
}


/// @details
/// 
#define THROW_ERROR(handle, sv, what)                           \
    {                                                           \
        FirebirdDiag_libfbclient diag =                         \
            handle_isc_error(handle->drv(), sv, DBWTL_SPOS(),   \
                             Variant(String(what)));            \
        handle->appendDiagRec(diag).raiseException();           \
        DBWTL_BUG_EX("Method raiseException() has not fired");  \
    }


/// @details
/// 
#define CREATE_DIAG(state, sqlstate, what, desc)                \
    FirebirdDiag_libfbclient(state, DBWTL_SPOS(),               \
                             firebird::STATES::                 \
                             DAL_FIREBIRD_SQLSTATE_##sqlstate,  \
                             what, desc)








//..............................................................................
/////////////////////////////////////////////////////// FirebirdBlob_libfbclient

/// @details
/// 
FBClientDrv*
FirebirdBlob_libfbclient::drv(void) const
{
    return this->m_data.m_resultset.drv();
}


/// @details
/// 
FirebirdDbc_libfbclient&
FirebirdBlob_libfbclient::getDbc(void) const
{
    return this->m_data.m_resultset.getDbc();
}


/// @details
/// 
FirebirdBlob_libfbclient::FirebirdBlob_libfbclient(const FirebirdData_libfbclient& data)
    : FirebirdBlob(),
      m_data(data),
      m_buf(),
      m_sv(), // only used for underflow()
      m_putback(DAL_STREAMBUF_PUTBACK),
      m_handle(0)
{
    ISC_STATUS sv[20];
    XSQLVAR *var = data.m_sqlvar;
    DBWTL_BUGCHECK_SOFT(var);

    ISC_QUAD id = *reinterpret_cast<ISC_QUAD*>(var->sqldata);
    ::isc_tr_handle *trans = this->m_data.m_resultset.getTrxHandle();

    this->m_data.m_resultset.drv()->isc_open_blob2(sv, this->getDbc().getHandle(),
                                                   trans, &this->m_handle, &id, 0, 0);
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR((&this->m_data.m_resultset), sv, "isc_open_blob2 failed");
    }

    // set streambuf pointers to end, so underflow() will handle the first fill
    char *end = m_buf + sizeof(m_buf);
    setg(end, end, end);
}


/// @details
/// 
FirebirdBlob_libfbclient::~FirebirdBlob_libfbclient(void)
{
    ISC_STATUS sv[20];

    if(this->m_handle)
    {
        this->drv()->isc_close_blob(sv, &this->m_handle);

        if(sv[0] == 1 && sv[1] > 0)
        {
            THROW_ERROR((&this->m_data.m_resultset), sv, "isc_close_blob failed");
        }
    }
}


/// @details
/// 
FirebirdBlob_libfbclient::int_type
FirebirdBlob_libfbclient::underflow()
{
    if (gptr() < egptr()) // buffer not exhausted
        return traits_type::to_int_type(*gptr());

    char *base = &m_buf[0];
    char *start = base;

    if (eback() == base) // true when this isn't the first fill
    {
        // Make arrangements for putback characters
        std::memmove(base, egptr() - m_putback, m_putback);
        start += m_putback;
    }

    // start is now the start of the buffer, proper.
    // Read from fptr_ in to the provided buffer
    unsigned short n = 0;
    
    DBWTL_BUGCHECK(this->m_handle);

    this->drv()->isc_get_segment(this->m_sv, &this->m_handle, &n,
                                 sizeof(m_buf) - (start - base), start);

    if(this->m_sv[1] == 0 ||
       this->m_sv[1] == isc_segstr_eof ||
       this->m_sv[1] == isc_segment)
    {}
    else
    {
        THROW_ERROR((&this->m_data.m_resultset), this->m_sv, "isc_get:segment failed");
    }
    
    if (n == 0)
        return traits_type::eof();

    // Set buffer pointers
    setg(base, start, start + n);
    return traits_type::to_int_type(*gptr());
}


/// @details
/// 
bool
FirebirdBlob_libfbclient::isNull(void) const
{
    // this object is only constructed when the field is not null
    return false;
}


//..............................................................................
/////////////////////////////////////////////////////// FirebirdBlob_libfbclient

/// @details
/// 
FirebirdData_libfbclient::FirebirdData_libfbclient(FirebirdResult_libfbclient& result,
                                                   colnum_t colnum, bool locked)
    : FirebirdData(),
      m_resultset(result),
      m_colnum(colnum),
      m_blobbuf(),
      m_memostream(),
      m_sqlvar(0)
{ 
    assert(result.m_osqlda);
    assert(m_colnum > 0);
    assert(m_colnum <= colnum_t(result.m_osqlda->sqln));
    m_sqlvar = &result.m_osqlda->sqlvar[m_colnum-1];
    assert(m_sqlvar);
}


/// @details
/// 
FirebirdData_libfbclient::~FirebirdData_libfbclient(void)
{ }


/// @details
/// 
IVariantValue*
FirebirdData_libfbclient::do_deepcopy(const IVariantValue *owner) const
{
    /// @bug What about NULL values?

    // Numeric columns need special handling
    if(this->daltype() == DAL_TYPE_NUMERIC)
    {
        return new typename value_traits<TNumeric>::stored_type(this->getNumeric());
    }

    if(this->daltype() == DAL_TYPE_BLOB)
    {
        return new typename value_traits<Blob>::stored_type(this->getBlob());
    }

    if(this->daltype() == DAL_TYPE_MEMO)
    {
        return new typename value_traits<Memo>::stored_type(this->getMemo());
    }

    if(this->daltype() == DAL_TYPE_VARBINARY)
    {
        return new typename value_traits<TVarbinary>::stored_type(this->getVarbinary());
    }

    switch(this->m_sqlvar->sqltype & ~1)
    {
    case SQL_TEXT:
    case SQL_VARYING:      return new typename value_traits<String>::stored_type(this->getText());
    case SQL_TYPE_DATE:    return new typename value_traits<TDate>::stored_type(this->getDate());
    case SQL_SHORT:        return new typename value_traits<signed short>::stored_type(this->getSmallint());
    case SQL_LONG:         return new typename value_traits<signed int>::stored_type(this->getLong());
    case SQL_DOUBLE:       return new typename value_traits<double>::stored_type(this->getDouble());
    case SQL_INT64:        return new typename value_traits<signed long long>::stored_type(this->getInt64());
    case SQL_FLOAT:        return new typename value_traits<float>::stored_type(this->getFloat());
    case SQL_TYPE_TIME:    return new typename value_traits<TTime>::stored_type(this->getTime());
    case SQL_TIMESTAMP:    return new typename value_traits<TTimestamp>::stored_type(this->getTimestamp());
    case SQL_BLOB:         DBWTL_BUG_EX("SQL_BLOB is alread handled");
    case SQL_ARRAY:
        DBWTL_NOTIMPL(); /// @todo Implement array type
    default:
        DBWTL_BUG_FMT("unknown sqltype: %hd", this->m_sqlvar->sqltype & ~1);
    }
}


/// @details
/// 
FirebirdBlob_libfbclient*
FirebirdData_libfbclient::getBlob(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_BLOB);

    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));

    if(! this->m_blobbuf.get())
    {
        this->m_blobbuf.reset(new FirebirdBlob_libfbclient(*this));
    }

    return this->m_blobbuf.get();
}



/// @details
/// 
UnicodeStreamBuf*
FirebirdData_libfbclient::getMemo(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_BLOB);

    FirebirdBlob_libfbclient *blob = this->getBlob();

    if(! this->m_memostream.get())
    {
        this->m_memostream.reset(new std::wstringstream());
    }

    std::stringstream ss;
    ss.imbue(std::locale("C"));
    ss << blob;

    *this->m_memostream << String(ss.str(), "UTF-8");

    return this->m_memostream->rdbuf();
}



/// @details
/// 
double 
FirebirdData_libfbclient::getDouble(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_DOUBLE);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        return *reinterpret_cast<double*>(this->m_sqlvar->sqldata);
    }
}


/// @details
/// 
int64_t
FirebirdData_libfbclient::getInt64(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_INT64);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        return *reinterpret_cast<ISC_INT64*>(this->m_sqlvar->sqldata);
    }
}


/// @details
/// 
float
FirebirdData_libfbclient::getFloat(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_FLOAT);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        return *reinterpret_cast<float*>(this->m_sqlvar->sqldata);
    }
}


/// @details
/// 
TTime
FirebirdData_libfbclient::getTime(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_TYPE_TIME);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        ::tm time;
        ISC_TIME dbtime = *reinterpret_cast<ISC_TIME*>(this->m_sqlvar->sqldata);
        this->m_resultset.drv()->isc_decode_sql_time(&dbtime, &time);
        return TTime(time);
    }
}


/// @details
/// 
TTimestamp
FirebirdData_libfbclient::getTimestamp(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_TIMESTAMP);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        ::tm ts;
        ISC_TIMESTAMP dbts = *reinterpret_cast<ISC_TIMESTAMP*>(this->m_sqlvar->sqldata);
        this->m_resultset.drv()->isc_decode_timestamp(&dbts, &ts);
        return TTimestamp(ts);
    }
}


/// @details
/// 
TNumeric
FirebirdData_libfbclient::getNumeric(void) const
{
    DALTRACE("VISIT");
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        ISC_INT64 llv = 0;

        // DO NOT USE the metadata scale
        //unsigned short scale = this->m_resultset.describeColumn(this->m_colnum).getScale();

        unsigned short scale = std::abs(this->m_sqlvar->sqlscale);
        switch(this->m_sqlvar->sqltype & ~1)
        {
        case SQL_SHORT:
            llv = *reinterpret_cast<ISC_SHORT*>(this->m_sqlvar->sqldata);
            return TNumeric(llv, scale);
        case SQL_LONG:
            llv = *reinterpret_cast<ISC_LONG*>(this->m_sqlvar->sqldata);
            return TNumeric(llv, scale);
        case SQL_INT64:
            llv = *reinterpret_cast<ISC_INT64*>(this->m_sqlvar->sqldata);
            return TNumeric(llv, scale);
        default:
            DBWTL_BUG_FMT("unknown sqltype: %hd", this->m_sqlvar->sqltype & ~1);
        }
    }
}


/// @details
/// 
String
FirebirdData_libfbclient::getText(void) const
{
    DALTRACE("VISIT");
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        short len = 0;
        switch(this->m_sqlvar->sqltype & ~1)
        {
        case SQL_VARYING:
            len = reinterpret_cast<short*>(this->m_sqlvar->sqldata)[0];
            return std::string(this->m_sqlvar->sqldata + 2, len);
        case SQL_TEXT:
            return std::string(this->m_sqlvar->sqldata, this->m_sqlvar->sqllen);
        default:
            DBWTL_BUG_FMT("unknown sqltype: %hd", this->m_sqlvar->sqltype & ~1);
        }
    }
}


/// @details
/// 
TDate
FirebirdData_libfbclient::getDate(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_TYPE_DATE);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        ::tm date;
        ISC_DATE dbdate = *reinterpret_cast<ISC_DATE*>(this->m_sqlvar->sqldata);
        this->m_resultset.drv()->isc_decode_sql_date(&dbdate, &date);
        return TDate(date);
    }
}


/// @details
/// 
short
FirebirdData_libfbclient::getSmallint(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_SHORT);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        return *reinterpret_cast<ISC_SHORT*>(this->m_sqlvar->sqldata);
    }
}


/// @details
/// 
int
FirebirdData_libfbclient::getLong(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK((this->m_sqlvar->sqltype & ~1) == SQL_LONG);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        return *reinterpret_cast<ISC_LONG*>(this->m_sqlvar->sqldata);
    }
}


/// @details
/// 
TVarbinary
FirebirdData_libfbclient::getVarbinary(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK(this->daltype() == DAL_TYPE_VARBINARY);
    if(this->isnull())
        throw ex::null_value(String("FirebirdData_libfbclient result column"));
    else
    {
        short len = 0;
        switch(this->m_sqlvar->sqltype & ~1)
        {
        case SQL_VARYING:
            len = reinterpret_cast<short*>(this->m_sqlvar->sqldata)[0];
            return TVarbinary(this->m_sqlvar->sqldata + 2, len);
        case SQL_TEXT:
            return TVarbinary(this->m_sqlvar->sqldata, this->m_sqlvar->sqllen);
        default:
            DBWTL_BUG_FMT("unknown sqltype: %hd", this->m_sqlvar->sqltype & ~1);
        }
    }
}


/// @details
/// 
bool 
FirebirdData_libfbclient::isnull(void) const
{
    DALTRACE("VISIT");

    assert(this->m_colnum > 0);
    assert(this->m_sqlvar);

    if(this->m_sqlvar->sqltype & 1) // using sqlind
    {
        assert(this->m_sqlvar->sqlind);
        return *this->m_sqlvar->sqlind == -1;        
    }
    else
        return false;
}


/// @details
/// 
rowid_t 
FirebirdData_libfbclient::getCurrentRowID(void) const
{
    DALTRACE("VISIT");
    return this->m_resultset.getCurrentRowID();
}


/// @details
/// 
void
FirebirdData_libfbclient::refresh(void)
{
    this->m_blobbuf.reset(0);
}


/// @details
/// 
daltype_t
FirebirdData_libfbclient::daltype(void) const
{
    return this->m_resultset.describeColumn(this->m_colnum).getDatatype();
}



//..............................................................................
///////////////////////////////////////////////////// FirebirdResult_libfbclient

/// @details
/// 
FirebirdResult_libfbclient::FirebirdResult_libfbclient(FirebirdStmt_libfbclient& stmt)
    : FirebirdResult(stmt.m_diag),
      m_stmt(stmt),
      m_handle(0),
      m_isqlda(0),
      m_osqlda(0),
      m_sql(),
      m_current_tuple(DAL_TYPE_ROWID_NPOS),
      m_last_row_status(100), // 100 signals EOF in isc API
      m_isopen(false),
      m_column_desc(),
      m_column_accessors(),
      m_allocated_accessors()
{}


/// @details
/// 
FirebirdResult_libfbclient::~FirebirdResult_libfbclient(void)
{
    this->m_column_accessors.clear();
    this->m_allocated_accessors.clear();
    this->close();
}


/// @details
/// 
FirebirdDiag&
FirebirdResult_libfbclient::appendDiagRec(const FirebirdDiag &diag) const
{
    
    return *this->m_diag.push_back(diag.clone());
}


/// @details
/// 
::isc_tr_handle*
FirebirdResult_libfbclient::getTrxHandle(void)
{
    return this->m_stmt.getDbc().getTrxHandle(this->m_stmt.getCurrentTrx());
}


/// @details
/// 
void
FirebirdResult_libfbclient::prepare(String sql, Transaction trx)
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

    // set new SQL statement
    this->m_sql = sql;

    ISC_STATUS sv[20];
    std::string sql_e = sql.to("UTF-8");

    this->drv()->isc_dsql_allocate_statement(sv, this->m_stmt.getDbc().getHandle(), &this->m_handle);
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_dsql_allocate_statement failed");
    }

    ::isc_tr_handle *trans = this->m_stmt.getDbc().getTrxHandle(trx);

    // Allocate OUTPUT XSQLDA
    free(m_osqlda);
    m_osqlda = 0;
    m_osqlda = (XSQLDA *)malloc(XSQLDA_LENGTH(1));
    assert(m_osqlda);
    m_osqlda->version = SQLDA_VERSION1;
    m_osqlda->sqln = 1;
    
    this->drv()->isc_dsql_prepare(sv, trans, &this->m_handle, 0, sql_e.c_str(),
                                  this->getDbc().getDialect(), m_osqlda);
    if(sv[0] == 1 && sv[1] > 0)
    {
        // isc_dsql_prepare() hasn't filled the osqlda structure, so
        // we drop it to avoid uninitialized values
        free(m_osqlda);
        m_osqlda = 0;
        THROW_ERROR(this, sv, DBWTL_FMT("prepare failed, SQL: %s", sql));
    }

    if (m_osqlda->sqld != m_osqlda->sqln) /* Need more XSQLVARS. */
    {
        int n = m_osqlda->sqld;
        free(m_osqlda);
        m_osqlda = 0;
        if(n > 0) // only allocate a new XSQLDA if there are any output columns
        {
            m_osqlda = (XSQLDA *)malloc(XSQLDA_LENGTH(n));
            assert(m_osqlda);
            m_osqlda->sqln = n;
            m_osqlda->version = SQLDA_VERSION1;
            this->drv()->isc_dsql_describe(sv, &this->m_handle, 1 , m_osqlda);
            if(sv[0] == 1 && sv[1] > 0)
            {
                // isc_dsql_decribe() hasn't filled the osqlda structure, so
                // we drop it to avoid uninitialized values
                free(m_osqlda);
                m_osqlda = 0;
                THROW_ERROR(this, sv, DBWTL_FMT("isc_dsql_describe failed, SQL: %s", sql));
            }   
        }

    }
    this->allocateVars(this->m_osqlda);


    // Allocate INPUT XSQLDA
    free(m_isqlda);
    m_isqlda = 0;
    m_isqlda = (XSQLDA *)malloc(XSQLDA_LENGTH(1));
    assert(m_isqlda);
    m_isqlda->version = SQLDA_VERSION1;
    m_isqlda->sqln = 1;

    this->drv()->isc_dsql_describe_bind(sv, &this->m_handle, 1 , m_isqlda);
    if(sv[0] == 1 && sv[1] > 0)
    {
        // isc_dsql_describe_bind() hasn't filled the isqlda structure, so
        // we drop it to avoid uninitialized values
        free(m_isqlda);
        m_isqlda = 0;
        THROW_ERROR(this, sv, DBWTL_FMT("isc_dsql_describe_bind failed, SQL: %s", sql));
    }

    if (m_isqlda->sqld != m_isqlda->sqln) /* Need more XSQLVARS. */
    {
        int n = m_isqlda->sqld;
        free(m_isqlda);
        m_isqlda = 0;
        if(n > 0) // only allocate a new XSQLDA if there are any input params
        {
            m_isqlda = (XSQLDA *)malloc(XSQLDA_LENGTH(n));
            assert(m_isqlda);
            m_isqlda->sqln = n;
            m_isqlda->version = SQLDA_VERSION1;
            this->drv()->isc_dsql_describe_bind(sv, &this->m_handle, 1 , m_isqlda);
            if(sv[0] == 1 && sv[1] > 0)
            {
                // isc_dsql_describe_bind() hasn't filled the isqlda structure, so
                // we drop it to avoid uninitialized values
                free(m_isqlda);
                m_isqlda = 0;
                THROW_ERROR(this, sv, DBWTL_FMT("isc_dsql_describe_bind failed, SQL: %s", sql));
            }
        }
    }
    this->allocateVars(this->m_isqlda);

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_PREPARED);

    DALTRACE_LEAVE;
}


/// @details
/// 
void
FirebirdResult_libfbclient::allocateVars(XSQLDA *sqlda)
{
    if(!sqlda)
        return; // nothing to do

    assert(sqlda->sqld > 0);
    assert(sqlda->sqln == sqlda->sqld); // prepare() should handle this

    XSQLVAR *var = 0;
    int i;
    for(i = 0, var = sqlda->sqlvar;
        i < sqlda->sqln;
        ++var, ++i)
    {
        switch(var->sqltype & ~1) // drop null flag for now
        {
        case SQL_VARYING:
            var->sqldata = allocspace(sizeof(char)*var->sqllen + 2);
            break;
        case SQL_TEXT:
            var->sqldata = allocspace(sizeof(char)*var->sqllen);
            break;
        case SQL_TYPE_DATE:
            var->sqldata = allocspace(sizeof(ISC_DATE));
            break;
        case SQL_SHORT:
            var->sqldata = allocspace(sizeof(short));
            break;
        case SQL_LONG:
            var->sqldata = allocspace(sizeof(ISC_LONG));
            break;
        case SQL_DOUBLE:
            var->sqldata = allocspace(sizeof(double));
            break;
        case SQL_INT64:
            var->sqldata = allocspace(sizeof(ISC_INT64));
            break;
        case SQL_FLOAT:
            var->sqldata = allocspace(sizeof(float));
            break;
        case SQL_TYPE_TIME:
            var->sqldata = allocspace(sizeof(ISC_TIME));
            break;
        case SQL_TIMESTAMP:
            var->sqldata = allocspace(sizeof(ISC_TIMESTAMP));
            break;
        case SQL_ARRAY:
        case SQL_BLOB:
            var->sqldata = allocspace(sizeof(ISC_QUAD));
            break;
        default:
            DBWTL_BUG_FMT("unknown sqltype: %hd", var->sqltype & ~1);
        }
        if(var->sqltype & 1)
        {
            var->sqlind = new short();
            *var->sqlind = -1;
        }
        else
            var->sqlind = 0;
    }
}


/// @details
/// 
void
FirebirdResult_libfbclient::freeVars(XSQLDA *sqlda)
{
    if(sqlda)
    {
        XSQLVAR *var = 0;
        int i;
        for(i = 0, var = sqlda->sqlvar;
            i < sqlda->sqln;
            ++var, ++i)
        {
            free(var->sqldata);
            var->sqldata = 0;
            //if(var->sqltype & 1)
            delete var->sqlind;
            //var->sqlind = 0;
        }
    }
}


/// @details
/// 
void
FirebirdResult_libfbclient::fillBindBuffers(StmtBase::ParamMap &params)
{
    XSQLDA *da = this->m_isqlda;

    if(!da)
        return; // no params;

    // We reset the input XSQLDA to drop old values
    freeVars(da);
    allocateVars(da);

    assert(da->sqln == da->sqld);

    for(int n = 1; n <= da->sqld; ++n)
    {
        StmtBase::ParamMapIterator p = params.find(n);
        if(p != params.end()) // found
        {
            XSQLVAR *sqlv = &da->sqlvar[n-1];
            Variant *v = p->second;
            if(v->isnull())
            {
                if(sqlv->sqltype & 1)
                {
                    assert(da->sqlvar[n-1].sqlind);
                    *da->sqlvar[n-1].sqlind = -1;
                }
                else
                {
                    // rewrite type
                    sqlv->sqltype |= 1;
                    sqlv->sqlind = new short();
                    *sqlv->sqlind = -1;
                }
            }
            else
            {
                int type = sqlv->sqltype & ~1;

                if(da->sqlvar[n-1].sqlind)
                    *da->sqlvar[n-1].sqlind = 0;

                // handle special conversions for NUMERIC
                if(v->datatype() == DAL_TYPE_NUMERIC &&
                   (type == SQL_SHORT || type == SQL_LONG || type == SQL_INT64))
                {
                    TNumeric num = v->get<TNumeric>();
                    int scale = num.scale();
                    if(num.scale())
                        num = num * TNumeric(std::pow(10, scale));
                    free(sqlv->sqldata);
                    sqlv->sqldata = 0;
                    sqlv->sqldata = allocspace(sizeof(ISC_INT64));
                    sqlv->sqltype = SQL_INT64 | (sqlv->sqltype & 1);
                    sqlv->sqlscale = -scale;
                    *reinterpret_cast<ISC_INT64*>(sqlv->sqldata) = num.asLongLong();
                    continue;
                }

                // handle special conversions for VARCHAR/TEXT
                if(type == SQL_TEXT || type == SQL_VARYING)
                {
                    if(type == SQL_VARYING) // do type coecering
                    {
                        sqlv->sqltype = SQL_TEXT | (sqlv->sqltype & 1);                        
                    }
                    if((da->sqlvar[n-1].sqlsubtype & 0xFF) == 1) // mask low byte for character set ID
                    {
                        TVarbinary p = v->get<TVarbinary>();
                        free(sqlv->sqldata);
                        sqlv->sqldata = 0;
                        sqlv->sqllen = p.size();
                        sqlv->sqldata = allocspace(sqlv->sqllen);
                        assert(sqlv->sqldata);
                        p.write(sqlv->sqldata, sqlv->sqllen);
                    }
                    else
                    {
                        std::string s = v->get<String>().to("UTF-8");
                        free(sqlv->sqldata);
                        sqlv->sqldata = 0;
                        sqlv->sqllen = s.length();
                        sqlv->sqldata = allocspace(sqlv->sqllen);
                        assert(sqlv->sqldata);
                        ::memcpy(sqlv->sqldata, s.c_str(), sqlv->sqllen);
                    }
                    continue;
                }

                void *buf = da->sqlvar[n-1].sqldata;
                switch(da->sqlvar[n-1].sqltype & ~1)
                {
                case SQL_TEXT:
                case SQL_VARYING:
                    DBWTL_BUG_EX("Both types are already handled few lines above.");
                case SQL_ARRAY:
                    DBWTL_NOTIMPL();

                case SQL_TYPE_DATE:
                    *reinterpret_cast<ISC_DATE*>(buf) = date2iscdate(v->get<TDate>(), this->drv());
                    break;
                case SQL_SHORT:
                    *reinterpret_cast<ISC_SHORT*>(buf) = v->get<short>();
                    break;
                case SQL_LONG:
                    *reinterpret_cast<ISC_LONG*>(buf) = v->get<signed int>();
                    break;
                case SQL_DOUBLE:
                    *reinterpret_cast<double*>(buf) = v->get<double>();
                    break;
                case SQL_INT64:
                    *reinterpret_cast<ISC_INT64*>(buf) = v->get<signed long long>();
                    break;
                case SQL_FLOAT:
                    *reinterpret_cast<float*>(buf) = v->get<float>();
                    break;
                case SQL_TYPE_TIME:
                    *reinterpret_cast<ISC_TIME*>(buf) = date2isctime(v->get<TTime>(), this->drv());
                    break;
                case SQL_TIMESTAMP:
                    *reinterpret_cast<ISC_TIMESTAMP*>(buf) = date2isctimestamp(v->get<TTimestamp>(), this->drv());
                    break;
                case SQL_BLOB:
                    this->fillBlob(&da->sqlvar[n-1], *v);
                    break;
                default:
                    DBWTL_BUG_FMT("unknown sqltype: %hd", da->sqlvar[n-1].sqltype & ~1);
                }
            }
        }
        else // set to NULL
        {
            if(da->sqlvar[n-1].sqltype & 1)
            {
                assert(da->sqlvar[n-1].sqlind);
                *da->sqlvar[n-1].sqlind = -1;
            }
            else
            {
                // rewrite type
                da->sqlvar[n-1].sqltype |= 1;
                da->sqlvar[n-1].sqlind = new short();
                *da->sqlvar[n-1].sqlind = -1;
            }
        }
    }
}


/// @details
/// 
void
FirebirdResult_libfbclient::fillBlob(XSQLVAR *var, Variant &data)
{
    ISC_STATUS sv[20];
    isc_blob_handle blobh = 0;

    DBWTL_BUGCHECK((var->sqltype & ~1) == SQL_BLOB);

    ::isc_tr_handle *trans = this->m_stmt.getDbc().getTrxHandle(this->m_stmt.getCurrentTrx());


    this->drv()->isc_create_blob2(sv, this->m_stmt.getDbc().getHandle(), trans, &blobh,
                                  (ISC_QUAD*)var->sqldata, 0, 0);
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_create_blob2 failed");
    }

    try
    {

        if(var->sqlsubtype == 0 || var->sqlsubtype != 1) // binary or anything other than text
        {
            Blob tmp_blob;
            ByteStreamBuf *buf = 0;
            if(data.can_convert<BlobStream>())
            {
                buf = data.get<BlobStream>().rdbuf();
            }
            else if(data.can_convert<Blob>())
            {
                tmp_blob = data.get<Blob>();
                buf = tmp_blob.rdbuf();
            }
            else
            {
                // error
            }

            std::vector<char> vec(DAL_FIREBIRD_BLOBBUF_SIZE);
        
            while(std::streamsize i = buf->sgetn(vec.data(), vec.size()))
            {
                this->drv()->isc_put_segment(sv, &blobh, i, vec.data());
                if(sv[0] == 1 && sv[1] > 0)
                {
                    THROW_ERROR(this, sv, "isc_put_segment failed");
                }
            }
            

        }
        else if(var->sqlsubtype == 1) // text
        {
            Memo tmp_memo;
            UnicodeStreamBuf *buf = 0;
            if(data.can_convert<MemoStream>())
            {
                buf = data.get<MemoStream>().rdbuf();
            }
            else if(data.can_convert<Memo>())
            {
                tmp_memo = data.get<Memo>();
                buf = tmp_memo.rdbuf();
            }
            else
            {
                // error
            }

            std::vector<char> vec(DAL_FIREBIRD_BLOBBUF_SIZE);

            std::stringstream ss;
            {
                std::wstringstream ws;
                ws << buf;
                String s(ws.str());
                ss << s.to(this->getDbc().getDbcEncoding());
            }
            std::streambuf *charbuf = ss.rdbuf();
        
            while(std::streamsize i = charbuf->sgetn(vec.data(), vec.size()))
            {
                this->drv()->isc_put_segment(sv, &blobh, i, vec.data());
                if(sv[0] == 1 && sv[1] > 0)
                {
                    THROW_ERROR(this, sv, "isc_put_segment failed");
                }
            }

        }
        else // should never happen
        {
            DBWTL_BUG();
        }
    }
    catch(...)
    {
        // if anything failed, close the blob handle
        this->drv()->isc_close_blob(sv, &blobh);
        if(sv[0] == 1 && sv[1] > 0)
        {
            THROW_ERROR(this, sv, "isc_close_blob failed after another error");
        }
        throw; 
    }

    this->drv()->isc_close_blob(sv, &blobh);
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_close_blob failed");
    }
}


/// @details
/// 
void
FirebirdResult_libfbclient::execute(StmtBase::ParamMap& params)
{
    DALTRACE_ENTER;
    std::map<int, std::string> tmp_strings;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");
    
    ISC_STATUS sv[20];

    ::isc_tr_handle *trans = this->m_stmt.getDbc().getTrxHandle(this->m_stmt.getCurrentTrx());


    if(this->getStatementType() == isc_info_sql_stmt_start_trans
       && *trans != 0
       && this->m_stmt.hasLocalTrx())
    {
        // check if the connection has an active transaction
        if(this->m_stmt.getDbc().hasActiveTrx())
        {
            this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 25000,
                                            String("Invalid transaction state"),
                                            "The SQL is a SET TRANSACTION statement, but"
                                            " the connecion already has an active transaction."))
                .raiseException();
        }

        // this commits the local trx and assign the connection trx
        // as the current trx.
        this->m_stmt.relinquishLocalTrx(this->m_stmt.getDbc().currentTrx());

        trans = this->m_stmt.getDbc().getTrxHandle(this->m_stmt.getCurrentTrx());
    }

/* seems useless, remove later
    if((*trans == 0) && this->getStatementType() != isc_info_sql_stmt_start_trans)
    {    
        char isc_tpb[] = { isc_tpb_version3, isc_tpb_write, isc_tpb_concurrency, isc_tpb_wait };

        this->drv()->isc_start_transaction(sv, trans, 1, this->m_stmt.getDbc().getHandle(),
                                           sizeof(isc_tpb), isc_tpb);
        if(sv[0] == 1 && sv[1] > 0)
        {
            THROW_ERROR(this, sv, "isc_start_transaction failed");
        }
    }
*/

    this->fillBindBuffers(params);

    this->drv()->isc_dsql_execute2(sv, trans, &this->m_handle, 1, this->m_isqlda, 0);
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, DBWTL_FMT("Error while executing the query. SQL: %s", this->m_sql));
    }

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);
    this->refreshMetadata();
    this->m_current_tuple = 0;
    //this->next();

    DALTRACE_LEAVE;
}


/// @details
/// 
size_t
FirebirdResult_libfbclient::paramCount(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");

    assert(this->m_isqlda);
    return this->m_isqlda->sqln;
}


/// @details
/// 
void 
FirebirdResult_libfbclient::first(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    if(!(this->m_cursorstate & DAL_CURSOR_POSITIONED))
        this->next();
    else if(this->eof() || this->m_current_tuple != 1)
    {
        throw ex::engine_error("can't scroll to first record");
    }
}


/// @details
/// 
bool
FirebirdResult_libfbclient::next(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    ISC_STATUS sv[20];

    assert(this->m_handle);

    if(!this->m_osqlda) // exists only if there are result coumns
        return false;

    assert(this->m_osqlda);

    this->m_last_row_status = this->drv()->isc_dsql_fetch(sv, &this->m_handle, 1, this->m_osqlda);
    switch(this->m_last_row_status)
    {
    case 0:
        // refresh all column accessors
        std::for_each(this->m_column_accessors.begin(),
                      this->m_column_accessors.end(),
                      [](VariantListT::value_type& p)
                      {
                          p.second->refresh();
                      });

        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_POSITIONED);
        ++this->m_current_tuple;
        break;
    case 100: // no more rows
        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);
        break;
    default:
        if(sv[0] == 1 && sv[1] > 0)
        {
            DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_BAD);
            THROW_ERROR(this, sv, "isc_dsql_fetch failed");
        }
    }
    return !this->eof();
}


/// @details
/// 
bool  
FirebirdResult_libfbclient::eof(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    if(! (this->m_cursorstate & DAL_CURSOR_POSITIONED))
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 24000,
                                        String("Dataset is not positioned."),
                                        "Hint: Use the first() method to position the cursor "
                                        "until calling eof()."))
            .raiseException();
    }

    return this->m_cursorstate & DAL_CURSOR_EOF;
    //return this->m_last_row_status != 0; // a valid row returns status 0
}


/// @details
/// 
void   
FirebirdResult_libfbclient::close(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(this->m_handle)
    {
        ISC_STATUS sv[20];
        
        this->drv()->isc_dsql_free_statement(sv, &this->m_handle, DSQL_drop);
        if(sv[0] == 1 && sv[1] > 0)
        {
            THROW_ERROR(this, sv, "isc_dsql_free_statement failed");
        }
        this->m_handle = 0;
    }
    this->freeVars(m_isqlda);
    free(m_isqlda);
    m_isqlda = 0;
    this->freeVars(m_osqlda);
    free(m_osqlda);
    m_osqlda = 0;
}


/// @details
/// 
rowcount_t
FirebirdResult_libfbclient::rowCount(void) const
{
    // Firebird can't tell you the row count
    return -1;
}


/// @details
/// The "documentation" for the isc_dsql_sql_info() function can
/// be found here:
/// http://tech.groups.yahoo.com/group/firebird-support/messages/37692?threaded=1&m=e&var=1&tidx=1
/// 
rowcount_t
FirebirdResult_libfbclient::affectedRows(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");

    ISC_STATUS sv[20];
    ISC_SCHAR type_item[] = { isc_info_sql_records };

    std::vector<ISC_SCHAR> res_buf(8);

again:
    this->drv()->isc_dsql_sql_info(sv, &this->m_handle, sizeof(type_item),
                                   type_item, res_buf.size(), res_buf.data());
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_dsql_sql_info failed");
    }

    // The Interbase 6 API Guide says isc_info_truncated is put on
    // the last byte in the buffer. But it seems that isn't true.
    // It's the first byte.
    if(res_buf[0] == isc_info_truncated)
    {
        res_buf.resize(res_buf.size() * 2);
        goto again;
    }

    DBWTL_BUGCHECK(res_buf[0] != isc_info_truncated);
    DBWTL_BUGCHECK(res_buf[0] == isc_info_sql_records);

    ISC_SCHAR *buf = &res_buf[0+1+2]; // first isc_info_sql_records entry
    ISC_LONG update_count = 0;
    ISC_LONG delete_count = 0;
    ISC_LONG select_count = 0;
    ISC_LONG insert_count = 0;

    while(buf[0] != isc_info_end)
    {
        ISC_LONG len = 0;
        switch(buf[0])
        {
        case isc_info_req_update_count:
            len = this->drv()->isc_vax_integer(&buf[1], 2);
            update_count = this->drv()->isc_vax_integer(&buf[1+2], len);            
            break;
        case isc_info_req_delete_count:
            len = this->drv()->isc_vax_integer(&buf[1], 2);
            delete_count = this->drv()->isc_vax_integer(&buf[1+2], len);            
            break;
        case isc_info_req_select_count:
            len = this->drv()->isc_vax_integer(&buf[1], 2);
            select_count = this->drv()->isc_vax_integer(&buf[1+2], len);            
            break;
        case isc_info_req_insert_count:
            len = this->drv()->isc_vax_integer(&buf[1], 2);
            insert_count = this->drv()->isc_vax_integer(&buf[1+2], len);            
            break;
        default:
            DBWTL_BUG_FMT("Invalid isc_info field: %hhd", buf[0]);
        }
        buf += 1+2+len; // skip id, len and data field
    }
    return update_count + delete_count + insert_count;
}


/// @details
/// 
Variant
FirebirdResult_libfbclient::lastInsertRowId(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");


    this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 0A000,
                                    String("Firebird has no support for lastInsertRowId"),
                                    "Hint: Instead, use RETURNING clause in your INSERT statement."))
        .raiseException();
    DBWTL_BUG();
}


/// @details
/// 
const FirebirdVariant&
FirebirdResult_libfbclient::column(String name)
{
    colnum_t num = this->columnID(name);
    return this->column(num);
}


/// @details
/// 
const FirebirdVariant&
FirebirdResult_libfbclient::column(colnum_t num)
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
        FirebirdVariant* v = new FirebirdVariant(new FirebirdData_libfbclient(*this, num, false));
        this->m_allocated_accessors.push_back(v); // smart ptr
        r = this->m_column_accessors.insert(VariantListT::value_type(num, v));
        p = r.first;
    }
    DALTRACE_LEAVE;
    return *(p->second);
}


/// @details
/// 
rowid_t
FirebirdResult_libfbclient::getCurrentRowID(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    return this->m_current_tuple;
}


/// @details
/// 
size_t    
FirebirdResult_libfbclient::columnCount(void) const
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    if(this->m_osqlda)
        return this->m_osqlda->sqln;
    else
        return 0; // no columns
}


/// @details
/// 
colnum_t
FirebirdResult_libfbclient::columnID(String name) const
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


/// @details
/// 
String
FirebirdResult_libfbclient::columnName(colnum_t num) const
{
    DALTRACE("VISIT");

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    return this->describeColumn(num).getName().asStr();
}


short
FirebirdResult_libfbclient::getStatementType(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isPrepared())
        throw ex::engine_error("Resultset is not prepared.");

    ISC_STATUS sv[20];
    ISC_SCHAR type_item[] = { isc_info_sql_stmt_type };

    std::vector<ISC_SCHAR> res_buf(8);

again:
    this->drv()->isc_dsql_sql_info(sv, &this->m_handle, sizeof(type_item),
                                   type_item, res_buf.size(), res_buf.data());
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_dsql_sql_info failed");
    }

    // The Interbase 6 API Guide says isc_info_truncated is put on
    // the last byte in the buffer. But it seems that isn't true.
    // It's the first byte.
    if(res_buf[0] == isc_info_truncated)
    {
        res_buf.resize(res_buf.size() * 2);
        goto again;
    }

    DBWTL_BUGCHECK(res_buf[0] != isc_info_truncated);
    DBWTL_BUGCHECK(res_buf[0] == isc_info_sql_stmt_type);

    ISC_LONG len = 0;
    ISC_SCHAR *buf = &res_buf[0+1]; // first isc_info_sql_stmt_type entry

    len = this->drv()->isc_vax_integer(&buf[0], 2);
    return this->drv()->isc_vax_integer(&buf[2], len);    
}


/// @details
/// 
FBClientDrv*
FirebirdResult_libfbclient::drv(void) const
{
    return this->getDbc().drv();
}


/// @details
/// 
FirebirdDbc_libfbclient& 
FirebirdResult_libfbclient::getDbc(void) const
{
    return this->m_stmt.getDbc();
}


/// @details
/// 
const FirebirdColumnDesc&
FirebirdResult_libfbclient::describeColumn(colnum_t num) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    std::map<colnum_t, FirebirdColumnDesc_libfbclient>::const_iterator i =
        this->m_column_desc.find(num);

    if(i == this->m_column_desc.end())
    {
        throw ex::not_found(US("Column '") + String::Internal(Variant(int(num)).asStr()) + US("' not found."));
    }
    else
        return i->second;
}


/// @details
/// 
const FirebirdColumnDesc&
FirebirdResult_libfbclient::describeColumn(String name) const
{
    colnum_t num = this->columnID(name);
    return this->describeColumn(num);
}


/// @details
/// 
void
FirebirdResult_libfbclient::refreshMetadata(void)
{
    DALTRACE_ENTER;
    this->m_column_desc.clear();
    if(! this->getHandle())
        return;
    
    size_t colcount = this->columnCount();
    FirebirdColumnDesc desc;
    
    for(size_t i = 0; i <= colcount; ++i)
    {
        FirebirdColumnDesc_libfbclient x(i, *this);
        this->m_column_desc.insert(std::pair<colnum_t, FirebirdColumnDesc_libfbclient>(i, x));
    }
}



//..............................................................................
///////////////////////////////////////////////// FirebirdColumnDesc_libfbclient


/// @details
/// 
FirebirdColumnDesc_libfbclient::FirebirdColumnDesc_libfbclient(colnum_t i,
                                                               FirebirdResult_libfbclient &result)
    : FirebirdColumnDesc()
{
    if(i == 0)
    {
        this->m_daltype = DAL_TYPE_UNKNOWN;
        this->m_name.set(String("__DBWTL_BOOKMARK__"));
    }
    else
    {
        assert(i <= colnum_t(result.m_osqlda->sqln));

        XSQLVAR *var = &result.m_osqlda->sqlvar[i-1];
        assert(var);

        this->m_name.set(String(std::string(var->aliasname, var->aliasname_length)));
        this->m_size.set(var->sqllen);
        switch(var->sqltype & ~1)
        {
        case SQL_VARYING:
	    // Firebird stores the charset ID in the low byte of the sqlsubtype field.
	    // The high byte contains the collation ID.
	    // The OCTETS character set has the ID 1, but it is also possible to
	    // check the RDB$CHARACTER_SETS system table against the ID.
	    if((var->sqlsubtype & 0xFF) == 1)
	    	this->m_daltype = DAL_TYPE_VARBINARY;
	    else
            	this->m_daltype = DAL_TYPE_STRING;
            break;
        case SQL_TEXT:
	    if((var->sqlsubtype & 0xFF) == 1)
	    	this->m_daltype = DAL_TYPE_VARBINARY;
            else
	    	this->m_daltype = DAL_TYPE_STRING;
            break;
        case SQL_TYPE_DATE:
            this->m_daltype = DAL_TYPE_DATE;
            break;
        case SQL_SHORT:
	    // For NUMERIC/DECIMAL values, sqlsubtype is set 1 or 2.
            if(var->sqlscale < 0 || var->sqlsubtype == 1)
            {
                this->m_daltype = DAL_TYPE_NUMERIC;
                this->m_scale.set<unsigned short>(::abs(var->sqlscale));
            }
	    else if(var->sqlscale < 0 || var->sqlsubtype == 2)
	    {
	    	this->m_daltype = DAL_TYPE_DECIMAL;
		this->m_scale.set<unsigned short>(::abs(var->sqlscale));
	    }
            else
                this->m_daltype = DAL_TYPE_SMALLINT;
            break;
        case SQL_LONG:
            if(var->sqlscale < 0 || var->sqlsubtype == 1)
            {
                this->m_daltype = DAL_TYPE_NUMERIC;
                this->m_scale.set<unsigned short>(::abs(var->sqlscale));
            }
	    else if(var->sqlscale < 0 || var->sqlsubtype == 2)
	    {
	    	this->m_daltype = DAL_TYPE_DECIMAL;
		this->m_scale.set<unsigned short>(::abs(var->sqlscale));
	    }
            else
                this->m_daltype = DAL_TYPE_INT;
            break;
        case SQL_DOUBLE:
            this->m_daltype = DAL_TYPE_DOUBLE;
            break;
        case SQL_INT64:
            if(var->sqlscale < 0 || var->sqlsubtype == 1)
            {
                this->m_daltype = DAL_TYPE_NUMERIC;
                this->m_scale.set<unsigned short>(::abs(var->sqlscale));
            }
	    else if(var->sqlscale < 0 || var->sqlsubtype == 2)
	    {
	    	this->m_daltype = DAL_TYPE_DECIMAL;
		this->m_scale.set<unsigned short>(::abs(var->sqlscale));
	    }
            else
                this->m_daltype = DAL_TYPE_BIGINT;
            break;
        case SQL_FLOAT:
            this->m_daltype = DAL_TYPE_FLOAT;
            break;
        case SQL_TYPE_TIME:
            this->m_daltype = DAL_TYPE_TIME;
            break;
        case SQL_TIMESTAMP:
            this->m_daltype = DAL_TYPE_TIMESTAMP;
            break;
        case SQL_ARRAY:
            this->m_daltype = DAL_TYPE_CUSTOM; // @todo introduce array type
            break;
        case SQL_BLOB:
            if(var->sqlsubtype == 0)
                this->m_daltype = DAL_TYPE_BLOB;
            else if(var->sqlsubtype == 1)
                this->m_daltype = DAL_TYPE_MEMO;
            else
                this->m_daltype = DAL_TYPE_BLOB; // BLR and other subtypes
            break;
        default:
            DBWTL_BUG_FMT("unknown sqltype: %hd", var->sqltype & ~1);
        }
      

        //DBWTL_NOTIMPL();
        // // set name
        // const char *s = result.drv()->firebird3_column_name(result.getHandle(), i-1);
        // this->m_name.set(String(s ? s : "", "UTF-8"));

        // // set type
        // const char *type = result.drv()->firebird3_column_decltype(result.getHandle(), i-1);

        // SqlTypeParser pt;
        // pt.registerType(DAL_TYPE_STRING, US("TEXT*"));
        // pt.parse(String(type, "UTF-8"));
        // this->m_type_name.set(daltype2sqlname(pt.getDaltype()));
        // this->m_daltype = pt.getDaltype();
        // this->m_size.set<signed int>(pt.getSize());
        // this->m_precision.set<unsigned short>(pt.getPrecision());
        // this->m_scale.set<unsigned short>(pt.getSize());
    }
}


//..............................................................................
//////////////////////////////////////////////////////// FirebirdEnv_libfbclient


/// @details
/// 
FirebirdEnv_libfbclient::FirebirdEnv_libfbclient(String lib)
    : FirebirdEnv(),
      m_lib()
{
    this->m_lib.reset(new FBClientDrv(lib));
}


/// @details
/// 
FirebirdEnv_libfbclient::~FirebirdEnv_libfbclient(void)
{ }


/// @details
/// 
FirebirdDiag&
FirebirdEnv_libfbclient::appendDiagRec(const FirebirdDiag &diag) const
{
    
    return *this->m_diag.push_back(diag.clone());
}


/// @details
/// 
FirebirdDbc_libfbclient*
FirebirdEnv_libfbclient::newConnection(void)
{
    return new FirebirdDbc_libfbclient(*this);
}


/// @details
/// 
FBClientDrv* 
FirebirdEnv_libfbclient::drv() const
{
    return this->m_lib.get();
}



//..............................................................................
//////////////////////////////////////////////////////// FirebirdDbc_libfbclient


/// @details
/// 
FirebirdDbc_libfbclient::FirebirdDbc_libfbclient(FirebirdEnv_libfbclient& env)
    : FirebirdDbc(),
      m_lib(env.drv()),
      m_dbh(0),
      m_dbc_trx(),
      m_trx_map(),
      m_trx_counter(0),
      m_dialect(SQL_DIALECT_V6)
{
    // create internal trx handle
    ::isc_tr_handle trh = 0;
    this->m_trx_map[++m_trx_counter] = trh;
    this->m_dbc_trx = Transaction(this, m_trx_counter);
}


/// @details
/// If there is an active transaction, we roll back it.
FirebirdDbc_libfbclient::~FirebirdDbc_libfbclient(void)
{
    if(this->hasActiveTrx())
        this->currentTrx().rollback();

    std::for_each(this->m_trx_map.begin(), this->m_trx_map.end(),
                  [this](typename trx_map_type::value_type &p)
                  {
                      Transaction(this, p.first).rollback();
                  });

    this->disconnect();
}


/// @details
/// 
FirebirdDiag&
FirebirdDbc_libfbclient::appendDiagRec(const FirebirdDiag &diag) const
{
    
    return *this->m_diag.push_back(diag.clone());
}


/// @details
/// 
FirebirdStmt_libfbclient*       
FirebirdDbc_libfbclient::newStatement(void)
{
    return new FirebirdStmt_libfbclient(*this);
}


/// @details
/// 
void
FirebirdDbc_libfbclient::connect(String database,
                                 String user,
                                 String password)
{
    IDbc::Options options;
    
    options[ "database" ] = database;
    options[ "username" ] = user;
    options[ "password" ] = password;
    
    return this->connect(options);
}


/// @details
/// 
char* add_dpb_param(std::vector<char> &dpb, char *p, char type, const Variant &v)
{
    if(v.isnull())
        return p;

    if(v.datatype() == DAL_TYPE_CHAR)
    {
        signed char d = v.get<signed char>();
        while(1)
        {
            size_t avail = (dpb.data() + dpb.size()) - p;
            if(sizeof(d) + 1+1 >= avail)
            {
                size_t consumed = p - dpb.data();
                dpb.resize(dpb.size() * 2);
                p = dpb.data() + consumed;
                continue;
            }
            else
            {
                *p++ = type;
                *p++ = sizeof(d);
                std::memcpy(p, &d, sizeof(d));
                p += sizeof(d);
                break;
            }
        }
    }
    else if(v.datatype() == DAL_TYPE_INT)
    {
        int d = v.get<int>();
        while(1)
        {
            size_t avail = (dpb.data() + dpb.size()) - p;
            if(sizeof(d) + 1+1 >= avail)
            {
                size_t consumed = p - dpb.data();
                dpb.resize(dpb.size() * 2);
                p = dpb.data() + consumed;
                continue;
            }
            else
            {
                *p++ = type;
                *p++ = sizeof(d);
                std::memcpy(p, &d, sizeof(d));
                p += sizeof(d);
                break;
            }
        }
    }
    else if(v.datatype() == DAL_TYPE_STRING)
    {
        std::string s = v.get<String>().to("UTF-8");
        assert(s.length() <= 256);
        while(1)
        {
            size_t avail = (dpb.data() + dpb.size()) - p;
            if(s.length() + 1+1 >= avail)
            {
                size_t consumed = p - dpb.data();
                dpb.resize(dpb.size() * 2);
                p = dpb.data() + consumed;
                continue;
            }
            else
            {
                *p++ = type;
                *p++ = s.length() & 0xff;
                std::memcpy(p, s.data(), s.length() & 0xff);
                p += s.length() & 0xff;
                break;
            }
        }
    }
    return p;
}


/// @details
/// 
void
FirebirdDbc_libfbclient::connect(IDbc::Options& options)
{
    DALTRACE_ENTER;

    if(this->isConnected())
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 08002,
                                        String("This connection is already active."),
                                        "Call disconnect() before making a new connection."))
            .raiseException();        
    }

    std::string database(options[ "database" ].to("UTF-8"));
    std::string username(options[ "username" ].to("UTF-8"));
    std::string password(options[ "password" ].to("UTF-8"));


    ISC_STATUS sv[20];
    std::vector<char> dpb(256);
    char *p = dpb.data();
    *p++ = isc_dpb_version1;

    p = add_dpb_param(dpb, p, isc_dpb_user_name, Variant(String(username)));
    p = add_dpb_param(dpb, p, isc_dpb_password, Variant(String(password)));
    p = add_dpb_param(dpb, p, isc_dpb_lc_ctype, Variant(String("UTF-8")));
    p = add_dpb_param(dpb, p, isc_dpb_sql_dialect, Variant((signed char)(SQL_DIALECT_V6)));
    //isc_dpb_connect_timeout
    //isc_dpb_sql_role_name
    //isc_dpb_set_db_readonly

    this->drv()->isc_attach_database(sv, 0, database.c_str(), &this->m_dbh, p - dpb.data(), dpb.data());
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_attach_database failed");
    }
    else
    {
        this->m_isConnected = true;
    }

    // if(this->isConnected())
    //     this->setDbcEncoding("UTF-8");

    DALTRACE_LEAVE;
}


/// @details
/// 
void
FirebirdDbc_libfbclient::disconnect(void)
{
    DALTRACE_ENTER;
    
    if(this->m_dbh)
    {
        DALTRACE("is connected, disconnecting...");

        ISC_STATUS sv[20];

        this->drv()->isc_detach_database(sv, &this->m_dbh);
        if(sv[0] == 1 && sv[1] > 0)
        {
            THROW_ERROR(this, sv, "isc_detach_database failed");
        }
        else
        {
            this->m_isConnected = false;
        }
    }

    DALTRACE_LEAVE;
}


/// @details
/// 
bool              
FirebirdDbc_libfbclient::hasActiveTrx(void)
{
    DBWTL_BUGCHECK(this->m_dbc_trx.id() > 0);

    return *this->getTrxHandle(this->m_dbc_trx) != 0;
}


/// @details
/// 
Transaction    
FirebirdDbc_libfbclient::currentTrx(void)
{
    DBWTL_BUGCHECK(this->m_dbc_trx.id() > 0);

    return this->m_dbc_trx;
/*
    if(!this->hasActiveTrx())
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 25000,
                                        String("Connection has no active transaction."),
                                        "currentTrx() failed because the connection has no explicit started transaction."))
            .raiseException();    
    }
    else
        return this->m_active_trx;
*/
}


/// @details
/// 
Transaction    
FirebirdDbc_libfbclient::makeTrx(trx_mode mode,
                                    access_mode access,
                                    String name)
{
    ::isc_tr_handle trh = this->makeTrxInternal(mode, access, name);

    this->m_trx_map[++m_trx_counter] = trh;
    return Transaction(this, m_trx_counter);
}


/// @details
/// 
::isc_tr_handle*
FirebirdDbc_libfbclient::getTrxHandle(const Transaction &trx)
{
    if(!trx.id())
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 25000,
                                        String("Invalid transaction ID (0)"),
                                        "The transaction handle was not initialized with a valid transaction."))
            .raiseException();
    }

    trx_map_type::iterator i = this->m_trx_map.find(trx.id());
    if(i == this->m_trx_map.end())
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 25000,
                                        DBWTL_FMT("Invalid transaction ID %s", trx.id()),
                                        "The transaction ID was not found."))
            .raiseException();
    }

    ::isc_tr_handle *h = &i->second;
    return h;
}


/// @details
/// 
unsigned short   
FirebirdDbc_libfbclient::getDialect(void) const
{
    return this->m_dialect;
}


/// @details
/// 
void
FirebirdDbc_libfbclient::beginTrans(trx_mode mode,
                                    access_mode access,
                                    String name)
{
    // check if the connection has an active transaction
    if(this->hasActiveTrx())
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 25000,
                                        String("Invalid transaction state"),
                                        "The SQL is a SET TRANSACTION statement, but"
                                        " the connecion already has an active transaction."))
            .raiseException();
    }

    ::isc_tr_handle trh = this->makeTrxInternal(mode, access, name);
    this->m_trx_map[this->m_dbc_trx.id()] = trh;
}


/// @details
/// 
::isc_tr_handle
FirebirdDbc_libfbclient::makeTrxInternal(trx_mode mode,
                                         access_mode access,
                                         String name)
{
    ISC_STATUS sv[20];
    ISC_STATUS *svp = sv;

    ::isc_tr_handle trh = 0;
    std::vector<char> tpb;
    tpb.resize(4);
    size_t i = 0;

    tpb[i++] = isc_tpb_version3;
    switch(access)
    {
    case trx_readwrite:    tpb[i++] = isc_tpb_write; break;
    case trx_readonly:     tpb[i++] = isc_tpb_read; break;
    case trx_default:      tpb[i++] = isc_tpb_write; break;
    }
    switch(mode)
    {
    case trx_read_uncommitted:
        tpb[i++] = isc_tpb_read_committed;
        tpb[i++] = isc_tpb_no_rec_version;
        break;
    case trx_read_committed:
        tpb[i++] = isc_tpb_read_committed;
        tpb[i++] = isc_tpb_rec_version;
        break;
    case trx_repeatable_read:
        tpb[i++] = isc_tpb_concurrency;
        break;
    case trx_serializable:
        tpb[i++] = isc_tpb_consistency;
        break;
    }

    this->drv()->isc_start_transaction(sv, &trh, 1, this->getHandle(), i, tpb.data());
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_start_transaction failed");
    }
    return trh;
}


/// @details
/// 
void
FirebirdDbc_libfbclient::commit(void)
{
    this->commit(this->m_dbc_trx);
}


/// @details
/// 
void
FirebirdDbc_libfbclient::commit(Transaction trx)
{
    ISC_STATUS sv[20];

    isc_tr_handle *trh = this->getTrxHandle(trx);
    if(!*trh) return; // already closed
    this->drv()->isc_commit_transaction(sv, trh);
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_commit_transaction failed");
    }
    *trh = 0;
}


/// @details
/// 
void
FirebirdDbc_libfbclient::rollback(String name)
{
    if(name.length() == 0)
        this->rollback(this->currentTrx());
    else // rollback to savepoint
    {
        String cmd;
        cmd = String("ROLLBACK TO ").append(name);
        
        FirebirdStmt::ptr stmt(this->newStatement());
        stmt->execDirect(cmd, this->currentTrx());
        stmt->close();        
    }
}



/// @details
/// 
void
FirebirdDbc_libfbclient::rollback(Transaction trx)
{
    ISC_STATUS sv[20];
    isc_tr_handle *trh = this->getTrxHandle(trx);
    DBWTL_BUGCHECK_SOFT(trh);
    if(!*trh) return; // already closed
    this->drv()->isc_rollback_transaction(sv, trh);
    if(sv[0] == 1 && sv[1] > 0)
    {
        THROW_ERROR(this, sv, "isc_rollback_transaction failed");
    }
    *trh = 0;
}



/// @details
/// 
void
FirebirdDbc_libfbclient::savepoint(String name)
{
    if(name.length() == 0)
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, HY000,
                                        DBWTL_FMT("Invalid savepoint name: %s", name),
                                        ""))
            .raiseException();
    }

    if(!this->hasActiveTrx())
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 25000,
                                        String("Invalid transaction state"),
                                        DBWTL_FMT("The connection has no active transaction"
                                                  " to create a savepoint (%s)", name)))
            .raiseException();
    }

    String cmd;
    cmd = String("SAVEPOINT ").append(name);

    FirebirdStmt::ptr stmt(this->newStatement());
    stmt->execDirect(cmd, this->currentTrx());
    stmt->close();
}



/// @details
/// 
void
FirebirdDbc_libfbclient::directCmd(String cmd)
{
    FirebirdStmt::ptr stmt(this->newStatement());
    stmt->execDirect(cmd); // if dbc has an active trx, stmt use it automatically
    stmt->close();

/*
    Transaction trx, trx_on_close;
    if(this->hasActiveTrx())
    {
        trx = this->m_dbc_trx;
    }
    else
    {
        trx = this->makeTrx(trx_read_committed, trx_default);
        trx_on_close = trx;
    }
    try
    {
        ISC_STATUS sv[20];

        std::string sql = cmd.to("UTF-8");

        this->drv()->isc_dsql_execute_immediate(sv, this->getHandle(), this->getTrxHandle(trx),
                                                0, sql.c_str(), this->getDialect(), 0);
        if(sv[0] == 1 && sv[1] > 0)
        {
            THROW_ERROR(this, sv, "isc_dsql_execute_immediate failed");
        }

        trx_on_close.commit();
    }
    catch(...)
    {
        trx_on_close.rollback();
        throw;
    }
*/
}


/// @details
/// 
::isc_db_handle*
FirebirdDbc_libfbclient::getHandle(void) const
{
    assert(this->m_dbh);
    return &this->m_dbh;
}


/// @details
/// 
String 
FirebirdDbc_libfbclient::driverName(void) const
{
    return "Firebird (libfirebird)";
}


/// @details
/// 
/// @todo Needs implementation
String 
FirebirdDbc_libfbclient::dbmsName(void) const
{
    return "Firebird";
}


/// @details
/// 
FBClientDrv*
FirebirdDbc_libfbclient::drv(void) const
{
    assert(this->m_lib);
    return this->m_lib;
}


/// @details
/// 
std::string  
FirebirdDbc_libfbclient::getDbcEncoding(void) const
{
    return "UTF-8";
}


/// @details
/// 
void    
FirebirdDbc_libfbclient::setDbcEncoding(std::string encoding)
{
    // nothing to do, Firebird only can handle UTF-8 and UTF-16
}





//..............................................................................
/////////////////////////////////////////////////////// FirebirdDiag_libfbclient


/// @details
/// 
FirebirdDiag_libfbclient::FirebirdDiag_libfbclient(dalstate_t state,
                                                   ISC_STATUS gdscode,
                                                   CodePosInfo pos,
                                                   Variant what,
                                                   String description)
    : FirebirdDiag(state, pos, what, description),
      m_gdscode(gdscode)
{
    m_sqlstate_id = gdscode2sqlstate(m_gdscode);
    m_sqlstate.set(String(firebird::sqlstate2string(m_sqlstate_id), "UTF-8"));

/*
  m_sqlstate_id = firebird3error_to_sqlstate(firebird_code);
  m_sqlstate.set(String(firebird::sqlstate2string(m_sqlstate_id), "UTF-8"));
*/
}


/// @details
/// 
FirebirdDiag_libfbclient::FirebirdDiag_libfbclient(dalstate_t state,
                                                   CodePosInfo pos,
                                                   firebird::STATES::engine_states_t sqlstate,
                                                   Variant what,
                                                   String description)
    : FirebirdDiag(state, pos, what, description),
      m_gdscode(0)
{
    m_sqlstate_id = sqlstate;
    m_sqlstate.set(String(firebird::sqlstate2string(m_sqlstate_id), "UTF-8"));

/*
  m_sqlstate_id = firebird3error_to_sqlstate(firebird_code);
  m_sqlstate.set(String(firebird::sqlstate2string(m_sqlstate_id), "UTF-8"));
*/
}


/// @details
/// 
FirebirdDiag_libfbclient*
FirebirdDiag_libfbclient::clone(void) const
{
    return new FirebirdDiag_libfbclient(*this);
}


/// @details
/// 
FirebirdDiag_libfbclient::FirebirdDiag_libfbclient(const FirebirdDiag_libfbclient& ref)
    : FirebirdDiag(ref),
      m_gdscode(ref.m_gdscode)
{}


/// @details
/// 
FirebirdDiag_libfbclient::~FirebirdDiag_libfbclient(void)
{}


/// @details
/// 
String
FirebirdDiag_libfbclient::str(void) const
{
    std::wstringstream ss;
    ss.imbue(std::locale("C"));

    ss << L"[SQLSTATE:" << ifnull<String>(this->m_sqlstate, L"UNSET") << L"] "
       << ifnull<String>(this->m_message, L"No message") << std::endl
       << ifnull<String>(this->m_description, L"No description") << std::endl
       << L"Firebird errcode: " << this->m_gdscode << std::endl
       << L"Raised at: "
       << L"[" << this->m_codepos.str() << "]";

    return ss.str();
}



//..............................................................................
/////////////////////////////////////////////////////// FirebirdStmt_libfbclient


/// @details
/// 
FirebirdStmt_libfbclient::FirebirdStmt_libfbclient(FirebirdDbc_libfbclient& conn)
    : FirebirdStmt(),
      m_conn(conn),
      m_resultsets(),
      m_currentResultset(0),
      m_localTrx(),
      m_currentTrx()
{}


/// @details
/// If there is a pending local transaction, the user has not called
/// close(), so we assume that something bad happens and we need
/// to rollback the transaction.
FirebirdStmt_libfbclient::~FirebirdStmt_libfbclient(void)
{
    if(this->hasLocalTrx())
        this->m_localTrx.rollback();
    this->close();
}


/// @details
/// 
FirebirdDiag&
FirebirdStmt_libfbclient::appendDiagRec(const FirebirdDiag &diag) const
{
    
    return *this->m_diag.push_back(diag.clone());
}


/// @details
/// 
FirebirdResult& 
FirebirdStmt_libfbclient::resultset(void)
{
    return *this->m_resultsets.at(this->m_currentResultset);
}


/// @details
/// 
const FirebirdResult& 
FirebirdStmt_libfbclient::resultset(void) const
{
    return *this->m_resultsets.at(this->m_currentResultset);
}


/// @details
/// 
bool
FirebirdStmt_libfbclient::hasLocalTrx(void) const
{
    return this->m_localTrx.id() != 0;
}


/// @details
/// This method is used when a user enters "SET TRANSACTION".
/// If the statement has a active local transaction, it must be committed
/// and the connection transaction is used instead.
/// 
/// @note This method is only used for special "SET TRANSACTION" handling
/// 
/// @warning USE WITH CAUTION
void
FirebirdStmt_libfbclient::relinquishLocalTrx(const Transaction &trx)
{
    DBWTL_BUGCHECK(this->hasLocalTrx());

    this->m_localTrx.commit();
    this->m_localTrx = Transaction(); // clear local transaction

    DBWTL_BUGCHECK(!this->hasLocalTrx());

    this->m_currentTrx = trx;
}


/// @details
/// Only a single statement is supported
void  
FirebirdStmt_libfbclient::prepare(String sql)
{
    if(this->m_cursorstate & DAL_CURSOR_PREPARED)
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 24000,
                                        String("Statement is already prepared."),
                                        "Hint: Call close() method and/or check for isPrepared()."))
            .raiseException();
    }

    DBWTL_BUGCHECK(this->m_currentTrx.id() == 0);


    Transaction trx;

    if(this->getDbc().hasActiveTrx())
        trx = this->getDbc().currentTrx();
    else // create local transaction
    {
        trx = this->getDbc().makeTrx(trx_read_committed, trx_default);
        m_localTrx = trx;
    }

    // prepare sets the current transaction for us
    this->prepare(sql, trx);
}

/// @details
/// Only a single statement is supported
void  
FirebirdStmt_libfbclient::prepare(String sql, Transaction trx)
{
    if(this->m_cursorstate & DAL_CURSOR_PREPARED)
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 24000,
                                        String("Statement is already prepared."),
                                        "Hint: Call close() method and/or check for isPrepared()."))
            .raiseException();
    }


    /// For Firebird, it is save to create a new resultset
    /// and execute the query inside of the resultset, because
    /// a query can never return more then one resultset.
    FirebirdResult_libfbclient* rs = this->newResultset();
    DBWTL_BUGCHECK(this->m_currentTrx.id() == 0);
    this->m_currentTrx = trx;
    rs->prepare(sql, trx);
    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_PREPARED);
}


/// @details
/// 
void   
FirebirdStmt_libfbclient::execute(void)
{
    DALTRACE_ENTER;

    if(! (this->m_cursorstate & DAL_CURSOR_PREPARED))
    {
        this->appendDiagRec(CREATE_DIAG(DAL_STATE_ERROR, 24000,
                                        String("Statement is not prepared."),
                                        "Hint: Use the prepare() method to prepare the statement."))
            .raiseException();
    }

    for(ResultsetVectorT::iterator i = this->m_resultsets.begin();
        i != this->m_resultsets.end();
        ++i)
    {             
        (*i)->execute(this->m_params);
    }
    this->m_currentResultset = 0;
    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);
    DALTRACE_LEAVE;
}


/// @details
/// 
void  
FirebirdStmt_libfbclient::execDirect(String sql)
{ 
    this->prepare(sql);
    this->execute();
}


/// @details
///
void
FirebirdStmt_libfbclient::execDirect(String sql, Transaction trx)
{
    this->prepare(sql, trx);
    this->execute();
}


/// @details
/// 
void   
FirebirdStmt_libfbclient::close(void)
{
    DALTRACE_ENTER;
    using std::for_each;
    using std::mem_fun;

    // clear resultsets from statement
    for_each(this->m_resultsets.begin(),
             this->m_resultsets.end(),
             delete_object());

    this->m_resultsets.clear();

    this->m_currentTrx = Transaction(); // reset current transaction

    if(this->hasLocalTrx())
    {
        if(this->isBad())
            this->m_localTrx.rollback();
        else
            this->m_localTrx.commit();
    }

    /// Params and accessors are cleaned up by the base class.
    FirebirdStmt::close();

    assert(!this->isPrepared());

    DALTRACE_LEAVE;
}


/// @details
/// 
bool 
FirebirdStmt_libfbclient::nextResultset(void)
{
    return false;
}


/// @details
/// 
bool  
FirebirdStmt_libfbclient::moreResultsets(void) const
{
    return false;
}


/// @details
/// 
size_t  
FirebirdStmt_libfbclient::paramCount(void) const
{
    return this->resultset().paramCount();
}


/// @details
/// 
rowid_t 
FirebirdStmt_libfbclient::affectedRows(void) const
{
    return this->resultset().affectedRows();
}


/// @details
/// 
Variant
FirebirdStmt_libfbclient::lastInsertRowId(void)
{
    return this->resultset().lastInsertRowId();
}


/// @details
/// 
FBClientDrv* 
FirebirdStmt_libfbclient::drv(void) const
{
    return this->getDbc().drv();
}


/// @details
/// 
FirebirdDbc_libfbclient& 
FirebirdStmt_libfbclient::getDbc(void) const
{
    return this->m_conn;
}


/// @details
/// 
FirebirdResult_libfbclient*
FirebirdStmt_libfbclient::newResultset(void)
{
    this->m_resultsets.push_back(new FirebirdResult_libfbclient(*this));
    return this->m_resultsets.at(this->m_resultsets.size()-1);
}




//..............................................................................
////////////////////////////////////////////////////////////////////////////////


/// @details
/// 
static inline ISC_DATE date2iscdate(TDate date, FBClientDrv *drv)
{
    ::tm d = date;
    ISC_DATE dbdate = 0;
    drv->isc_encode_sql_date(&d, &dbdate);
    return dbdate;
}


/// @details
/// 
static inline ISC_TIME date2isctime(TTime time, FBClientDrv *drv)
{
    ::tm t = time;
    ISC_TIME dbtime = 0;
    drv->isc_encode_sql_time(&t, &dbtime);
    return dbtime;
}


/// @details
/// 
static inline ISC_TIMESTAMP date2isctimestamp(TTimestamp timestamp, FBClientDrv *drv)
{
    ::tm t = timestamp;
    ISC_TIMESTAMP dbtimestamp;
    drv->isc_encode_timestamp(&t, &dbtimestamp);
    return dbtimestamp;
}


/// @details
/// 
static inline ISC_SCHAR* allocspace(size_t len)
{
    return (ISC_SCHAR*) malloc(len);
}



/// @details
/// 
static firebird_sqlstates::engine_states_t gdscode2sqlstate(ISC_STATUS code)
{
    switch(code)
    {
    case isc_arith_except:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_22000;
    case isc_bad_dbkey:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_db_format:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_db_handle:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_dpb_content:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_dpb_form:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_req_handle:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_segstr_handle:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_segstr_id:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_tpb_content:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_tpb_form:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_trans_handle:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bug_check:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_convert_error:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_db_corrupt:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_deadlock:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_excess_trans:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_from_no_match:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_infinap:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_infona:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_infunk:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_integ_fail:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_blr:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_error:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_08000;
    case isc_lock_conflict:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_metadata_corrupt:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_not_valid:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_cur_rec:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_dup:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_finish:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_meta_update:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_priv:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_recon:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_record:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_segstr_close:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_obsolete_metadata:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_open_trans:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_port_len:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_read_only_field:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_read_only_rel:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_read_only_trans:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_read_only_view:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_req_no_trans:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_req_sync:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_req_wrong_db:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_segment:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_segstr_eof:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_segstr_no_op:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_segstr_no_read:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_segstr_no_trans:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_segstr_no_write:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_segstr_wrong_db:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sys_request:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_stream_eof:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_unavailable:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_unres_rel:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_uns_ext:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wish_list:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wrong_ods:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wronumarg:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_imp_exc:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_random:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fatal_conflict:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badblk:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invpoolcl:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_nopoolids:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_relbadblk:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_blktoobig:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bufexh:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_syntaxerr:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bufinuse:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bdbincon:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_reqinuse:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badodsver:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_relnotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fldnotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dirtypage:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_waifortra:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_doubleloc:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_nodnotfnd:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dupnodfnd:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_locnotmar:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badpagtyp:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_corrupt:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badpage:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badindex:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dbbnotzer:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_tranotzer:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_trareqmis:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badhndcnt:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wrotpbver:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wroblrver:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wrodpbver:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_blobnotsup:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badrelation:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_nodetach:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_notremote:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_trainlim:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_notinlim:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_traoutsta:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_connect_reject:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dbfile:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_orphan:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_lock_mgr:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ctxinuse:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ctxnotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_datnotsup:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badmsgnum:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_badparnum:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_virmemexh:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_blocking_signal:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_lockmanerr:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_journerr:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_keytoobig:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_nullsegkey:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sqlerr:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wrodynver:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_funnotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_funmismat:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_msg_vec:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_detach:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_noargacc_read:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_noargacc_write:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_read_only:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ext_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_non_updatable:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_rollback:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_sec_info:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_sec_info:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_misc_interpreted:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_update_conflict:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_unlicensed:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_obj_in_use:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_nofilter:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_shadow_accessed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_sdl:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_out_of_bounds:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_dimension:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_rec_in_limbo:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_shadow_missing:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cant_validate:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cant_start_journal:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gennotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cant_start_logging:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_segstr_type:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_foreign_key:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_high_minor:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_tra_state:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_trans_invalid:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_buf_invalid:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_indexnotdefined:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_login:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_28000;
    case isc_invalid_bookmark:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_lock_level:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_relation_lock:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_record_lock:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_max_idx:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_jrn_enable:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_old_failure:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_old_in_progress:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_old_no_space:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_wal_no_jrn:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_num_old_files:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_file_open:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_stmt_handle:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_failure:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_walw_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logh_small:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logh_inv_version:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logh_open_flag:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logh_open_flag2:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logh_diff_dbname:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logf_unexpected_eof:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logr_incomplete:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logr_header_small:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_logb_small:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_illegal_attach:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_invalid_wpb:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_err_rollover:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_wal:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_drop_wal:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_stream_not_defined:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_subsys_error:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_subsys_corrupt:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_archive:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_shutinprog:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_range_in_use:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_range_not_found:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_charset_not_found:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_lock_timeout:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_prcnotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_prcmismat:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_bugcheck:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_cant_expand:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_codnotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_xcpnotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_except:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cache_restart:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_lock_handle:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_jrn_present:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_err_rollover2:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_err_logwrite:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_err_jrn_comm:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_err_expansion:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_err_setup:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_err_ww_sync:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_err_ww_start:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_shutdown:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_08004;
    case isc_existing_priv_mod:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_primary_key_ref:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_primary_key_notnull:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ref_cnstrnt_notfound:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_foreign_key_notfound:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ref_cnstrnt_update:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_check_cnstrnt_update:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_check_cnstrnt_del:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_integ_index_seg_del:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_integ_index_seg_mod:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_integ_index_del:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_integ_index_mod:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_check_trig_del:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_check_trig_update:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cnstrnt_fld_del:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cnstrnt_fld_rename:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_rel_cnstrnt_update:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_constaint_on_view:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invld_cnstrnt_type:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_primary_key_exists:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_systrig_update:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_not_rel_owner:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_grant_obj_notfound:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_grant_fld_notfound:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_grant_nopriv:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_nonsql_security_rel:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_nonsql_security_fld:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_cache_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_shutfail:                    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_check_constraint:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_svc_handle:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_shutwarn:                    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wrospbver:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_spb_form:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_svcnotdef:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_jrn:                      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_transliteration_failed:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_start_cm_for_wal:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_ovflow_log_required:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_text_subtype:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_error:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_command_err:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_constant_err:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_err:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_datatype_err:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_decl_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_update_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_open_err:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_close_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_field_err:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_internal_err:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_relation_err:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_procedure_err:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_request_err:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_sqlda_err:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_var_count_err:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_stmt_handle:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_function_err:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_blob_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_collation_not_found:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_collation_not_for_charset:   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_dup_option:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_tran_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_invalid_array:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_max_arr_dim_exceeded:   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_arr_range_error:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_trigger_err:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_subselect_err:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_crdb_prepare_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_specify_field_err:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_num_field_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_col_name_err:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_where_err:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_table_view_err:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_distinct_err:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_key_field_count_err:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_subquery_err:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_expression_eval_err:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_node_err:                    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_command_end_err:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_index_name:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_name:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_field_name:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_token_err:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_union_err:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_construct_err:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_field_aggregate_err:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_field_ref_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_order_by_err:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_return_mode_err:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_extern_func_err:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_alias_conflict_err:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_procedure_conflict_error:    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_relation_conflict_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_domain_err:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_idx_seg_err:                 return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_node_name_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_table_name:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_proc_name:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_idx_create_err:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_shadow_err:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dependency:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_idx_key_err:                 return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_file_length_err:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_shadow_number_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_token_unk_err:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_no_relation_alias:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_indexname:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_stream_plan:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_stream_twice:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_stream_not_found:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_collation_requires_text:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_domain_not_found:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_index_unused:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_self_join:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_stream_bof:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_stream_crack:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_db_or_file_exists:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_operator:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_conn_lost:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_checksum:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_page_type_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ext_readonly_err:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sing_select_err:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_psw_attach:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_psw_start_trans:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_direction:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_var_conflict:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_no_blob_array:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_base_table:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_duplicate_base_table:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_view_alias:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_index_root_page_full:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_blob_type_unknown:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_req_max_clones_exceeded:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_duplicate_spec:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_unique_key_violation:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_srvr_version_too_old:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_drdb_completed_with_errs:    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_procedure_use_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_count_mismatch:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_blob_idx_err:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_array_idx_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_key_field_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_delete:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_del_last_field:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sort_err:                    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sort_mem_err:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_version_err:                 return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_inval_key_posn:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_segments_err:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_crrp_data_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_rec_size_err:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_field_ref:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_req_depth_exceeded:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_field_access:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_dbkey:                    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_jrn_format_err:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_jrn_file_full:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_open_cursor_request:    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ib_error:                    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cache_redef:                 return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cache_too_small:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_log_redef:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_log_too_small:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_partition_too_small:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_partition_not_supp:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_log_length_spec:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_precision_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_scale_nogt:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_expec_short:                 return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_expec_long:                  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_expec_ushort:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_like_escape_invalid:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_svcnoexe:                    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_net_lookup_err:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_service_unknown:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_host_unknown:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_grant_nopriv_on_base:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_fld_ambiguous:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_agg_ref_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_complex_view:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_unprepared_stmt:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_expec_positive:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_sqlda_value_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_array_id:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_extfile_uns_op:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_svc_in_use:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_err_stack_limit:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_key:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_net_init_error:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_loadlib_failure:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_network_error:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_08001;
    case isc_net_connect_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_net_connect_listen_err:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_net_event_connect_err:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_net_event_listen_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_net_read_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_net_write_err:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_integ_index_deactivate:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_integ_deactivate_primary:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cse_not_supported:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_tra_must_sweep:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_unsupported_network_drive:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_create_err:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_open_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_close_err:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_read_err:                 return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_write_err:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_delete_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_access_err:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_udf_exception:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_lost_db_connection:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_write_user_priv:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_token_too_long:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_max_att_exceeded:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_login_same_as_role_name:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_reftable_requires_pk:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_usrname_too_long:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_password_too_long:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_usrname_required:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_password_required:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_protocol:                return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dup_usrname_found:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_usrname_not_found:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_error_adding_sec_record:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_error_modifying_sec_record:  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_error_deleting_sec_record:   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_error_updating_sec_db:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sort_rec_size_err:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_default_value:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_clause:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_too_many_handles:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_optimizer_blk_exc:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_string_constant:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_transitional_date:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_read_only_database:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_must_be_dialect_2_and_up:    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_blob_filter_exception:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_access_violation:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_datatype_missalignment:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_array_bounds_exceeded:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_float_denormal_operand:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_float_divide_by_zero:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_float_inexact_result:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_float_invalid_operand:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_float_overflow:    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_float_stack_check: return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_float_underflow:   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_integer_divide_by_zero:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_integer_overflow:  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_unknown:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_stack_overflow:    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_sigsegv:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_sigill:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_sigbus:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exception_sigfpe:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ext_file_delete:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ext_file_modify:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_adm_task_denied:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_extract_input_mismatch:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_insufficient_svc_privileges: return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_file_in_use:                 return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_service_att_err:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ddl_not_allowed_by_db_sql_dial:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cancelled:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_unexp_spb_form:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sql_dialect_datatype_unsupport:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_svcnouser:                   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_depend_on_uncommitted_rel:   return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_svc_name_missing:            return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_too_many_contexts:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_datype_notsup:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dialect_reset_warning:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dialect_not_changed:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_database_create_failed:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_inv_dialect_specified:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_valid_db_dialects:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sqlwarn:                     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dtype_renamed:               return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_extern_func_dir_error:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_date_range_exceeded:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_inv_client_dialect_specified:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_valid_client_dialects:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_optimizer_between_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_service_not_supported:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_generator_name:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_udf_name:                    return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_limit_param:             return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_skip_param:              return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_io_32bit_exceeded_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_savepoint:           return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_column_pos_err:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_agg_where_err:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_agg_group_err:          return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_agg_column_err:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_agg_having_err:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_agg_nested_err:         return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exec_sql_invalid_arg:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exec_sql_invalid_req:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exec_sql_invalid_var:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_exec_sql_max_call_exceeded:  return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_conf_access_denied:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wrong_backup_state:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_wal_backup_err:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cursor_not_open:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_shutdown_mode:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_concat_overflow:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_substring_offset:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_foreign_key_target_doesnt_exist:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_foreign_key_references_present:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_no_update:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_cursor_already_open:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_stack_trace:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ctx_var_not_found:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ctx_namespace_invalid:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ctx_too_big:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_ctx_bad_argument:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_identifier_too_long:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_except2:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_malformed_string:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_prc_out_param_mismatch:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_command_end_err2:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_partner_idx_incompat_type:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_substring_length:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_charset_not_installed:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_collation_not_installed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_att_shutdown:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_blobtoobig:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_must_have_phys_field:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_time_precision:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_blob_convert_error:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_array_convert_error:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_record_lock_not_supp:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_partner_idx_not_found:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_tra_num_exc:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_field_disappeared:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_met_wrong_gtt_scope:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_subtype_for_internal_use:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_illegal_prc_type:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_sort_datatype:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_collation_name:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_domain_name:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_domnotdef:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_array_max_dimensions:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_max_db_per_trans_allowed:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_debug_format:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_bad_proc_BLR:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_key_too_big:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_concurrent_transaction:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_not_valid_for_var:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_not_valid_for:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_need_difference:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_long_login:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fldnotdef2:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_db_name:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_invalid_sw:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_incmp_sw:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_replay_req:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_pgbuf_req:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_val_req:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_pval_req:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_trn_req:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_full_req:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_usrname_req:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_pass_req:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_subs_name:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_wal_req:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_sec_req:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_nval_req:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_type_shut:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_retry:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_retry_db:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_exceed_max:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_corrupt_pool:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_mem_exhausted:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_bad_pool:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_trn_not_valid:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_unexp_eoi:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_recon_fail:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_trn_unknown:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_mode_req:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_opt_SQL_dialect:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gfix_pzval_req:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_dbkey_from_non_table:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_transitional_numeric:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_dialect_warning_expr:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_sql_db_dialect_dtype_unsupport:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_isc_sql_dialect_conflict_num:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_warning_number_ambiguous:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_warning_number_ambiguous1:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_warn_precision_ambiguous:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_warn_precision_ambiguous1:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_warn_precision_ambiguous2:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_ambiguous_field_name:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_udf_return_pos_err:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_invalid_label:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_datatypes_not_comparable:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_invalid:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_redefined:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_not_found:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_exists:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_rel_ambiguous:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_rel_not_found:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cursor_not_open:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_type_not_supp_ext_tab:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_feature_not_supported_ods:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_primary_key_required:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_upd_ins_doesnt_match_pk:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_upd_ins_doesnt_match_matching:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_upd_ins_with_complex_view:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_incompatible_trigger_type:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_db_trigger_type_cant_change:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_role_does_not_exist:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_no_grant_admin_opt:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_user_not_role_member:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_delete_role_failed:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_grant_role_to_user:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_inv_sql_role_name:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_dup_sql_role:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_kywd_spec_for_role:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_roles_not_supported:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_domain_name_exists:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_field_name_exists:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_dependency_exists:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_dtype_invalid:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_char_fld_too_small:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_invalid_dtype_conversion:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_dtype_conv_invalid:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_zero_len_id:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_max_coll_per_charset:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_invalid_coll_attr:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_wrong_gtt_scope:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_scale_too_big:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_precision_too_small:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_miss_priv_warning:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dyn_ods_not_supp_feature:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_unknown_switch:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_page_size_missing:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_page_size_toobig:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_redir_ouput_missing:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_switches_conflict:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_unknown_device:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_no_protection:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_page_size_not_allowed:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_multi_source_dest:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_filename_missing:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_dup_inout_names:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_inv_page_size:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_db_specified:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_db_exists:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_unk_device:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_blob_info_failed:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_unk_blob_item:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_get_seg_failed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_close_blob_failed:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_open_blob_failed:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_put_blr_gen_id_failed:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_unk_type:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_comp_req_failed:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_start_req_failed:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_rec_failed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_rel_req_failed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_db_info_failed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_no_db_desc:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_db_create_failed:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_decomp_len_error:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_tbl_missing:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_blob_col_missing:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_create_blob_failed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_put_seg_failed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_rec_len_exp:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_inv_rec_len:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_exp_data_type:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_gen_id_failed:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_unk_rec_type:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_inv_bkup_ver:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_missing_bkup_desc:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_string_trunc:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_cant_rest_record:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_send_failed:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_no_tbl_name:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_unexp_eof:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_db_format_too_old:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_inv_array_dim:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_xdr_len_expected:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_open_bkup_error:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_open_error:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_missing_block_fac:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_inv_block_fac:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_block_fac_specified:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_missing_username:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_missing_password:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_missing_skipped_bytes:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_inv_skipped_bytes:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_err_restore_charset:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_err_restore_collation:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_read_error:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_write_error:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_db_in_use:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_sysmemex:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_restore_role_failed:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_role_op_missing:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_page_buffers_missing:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_page_buffers_wrong_param:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_page_buffers_restore:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_inv_size:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_file_outof_sequence:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_join_file_missing:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_stdin_not_supptd:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_stdout_not_supptd:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_bkup_corrupt:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_unk_db_file_spec:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_hdr_write_failed:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_disk_space_ex:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_size_lt_min:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_svc_name_missing:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_not_ownr:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_mode_req:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_just_data:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gbak_data_only:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_too_old_ods:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_table_not_found:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_view_not_found:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_line_col_error:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_unknown_pos:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_no_dup_name:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_too_many_values:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_no_array_computed:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_implicit_domain_name:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_only_can_subscript_array:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_max_sort_items:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_max_group_items:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_conflicting_sort_field:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_derived_table_more_columns:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_derived_table_less_columns:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_derived_field_unnamed:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_derived_field_dup_name:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_derived_alias_select:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_derived_alias_field:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_auto_field_bad_pos:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_wrong_reference:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_cycle:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_outer_join:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_mult_references:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_not_a_union:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_nonrecurs_after_recurs:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_wrong_clause:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_union_all:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_miss_nonrecursive:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_nested_with:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_col_more_than_once_using:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_unsupp_feature_dialect:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_dsql_cte_not_used:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_cant_open_db:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_switches_error:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_no_op_spec:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_no_usr_name:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_err_add:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_err_modify:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_err_find_mod:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_err_rec_not_found:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_err_delete:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_err_find_del:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_err_find_disp:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_inv_param:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_op_specified:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_pw_specified:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_uid_specified:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_gid_specified:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_proj_specified:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_org_specified:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_fname_specified:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_mname_specified:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_lname_specified:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_inv_switch:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_amb_switch:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_no_op_specified:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_params_not_allowed:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_incompat_switch:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_inv_username:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_inv_pw_length:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_db_specified:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_db_admin_specified:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_db_admin_pw_specified:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gsec_sql_role_specified:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_no_file:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_op_specified:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_op_missing:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_inv_switch:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_inv_switch_combo:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_inv_op_combo:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_amb_switch:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_inv_parameter:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_param_specified:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_param_req:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_syntx_error:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_dup_id:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_inv_id_key:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_err_remove:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_err_update:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_err_convert:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_err_unk:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_svc_err_add:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_svc_err_remove:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_license_eval_exists:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gstat_unknown_switch:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gstat_retry:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gstat_wrong_ods:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gstat_unexpected_eof:      return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gstat_open_err:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gstat_read_err:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_gstat_sysmemex:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fbsvcmgr_bad_am:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fbsvcmgr_bad_wm:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fbsvcmgr_bad_rs:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fbsvcmgr_info_err:     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fbsvcmgr_query_err:        return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_fbsvcmgr_switch_unknown:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;
    case isc_err_max:       return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000;

    default:
        DBWTL_NOTIMPL();
    }





    // switch(code)
    // {
    // case FIREBIRD_ERROR:         /* SQL error or missing database */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_INTERNAL:       /* Internal logic error in Firebird */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_PERM:           /* Access permission denied */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_28000;
    // case FIREBIRD_ABORT:          /* Callback routine requested an abort */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_BUSY:           /* The database file is locked */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_25001;
    // case FIREBIRD_LOCKED:         /* A table in the database is locked */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_NOMEM:          /* A malloc() failed */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_READONLY:       /* Attempt to write a readonly database */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_25006;
    // case FIREBIRD_INTERRUPT:      /* Operation terminated by firebird3_interrupt()*/
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_IOERR:          /* Some kind of disk I/O error occurred */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_CORRUPT:        /* The database disk image is malformed */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_NOTFOUND:       /* NOT USED. Table or record not found */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_FULL:           /* Insertion failed because database is full */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_CANTOPEN:       /* Unable to open the database file */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_08001;
    // case FIREBIRD_PROTOCOL:       /* Database lock protocol error */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_EMPTY:          /* Database is empty */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_SCHEMA:         /* The database schema changed */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_TOOBIG:         /* String or BLOB exceeds size limit */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_22000;
    // case FIREBIRD_CONSTRAINT:     /* Abort due to constraint violation */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_23000;
    // case FIREBIRD_MISMATCH:       /* Data type mismatch */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_MISUSE:         /* Library used incorrectly */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_NOLFS:          /* Uses OS features not supported on host */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_0A000;
    // case FIREBIRD_AUTH:           /* Authorization denied */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_28000;
    // case FIREBIRD_FORMAT:         /* Auxiliary database format error */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_42000;
    // case FIREBIRD_RANGE:          /* 2nd parameter to firebird3_bind out of range */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_22000;
    // case FIREBIRD_NOTADB:         /* File opened that is not a database file */
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_08001;
    // default:
    //     return firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_08000;
    //     //throw firebird::STATES::SQLSTATE_08006(*this);
    // }
}




DAL_NAMESPACE_END



//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
