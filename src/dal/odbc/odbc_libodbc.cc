//
// odbc_libodbc.cc - SQLite Backend libodbc (definition)
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
/// @brief SQLite Backend libodbc (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "odbc_libodbc.hh"
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
        stateobj.setDrvCode(drvcode, "ODBC_XYZ");               \
        stateobj.setSource(__FILE__, __FUNCTION__);             \
        stateobj.setSQLState(sqlstate);                         \
    }



#define DAL_ODBC_LIBODBC_DIAG_ERROR(handle, msg, desc, code, excode)    \
    {                                                                   \
        odbc::DIAG *__dal__diag =                                       \
            new OdbcDiag_libodbc(DAL_STATE_ERROR,                       \
                                 DBWTL_SPOS(),                          \
                                 String(msg), desc, code, excode);      \
        handle->m_diag.push_back(__dal__diag);                          \
        __dal__diag->raiseException();                                  \
    }



DAL_NAMESPACE_BEGIN


static inline daltype_t sqltype2daltype(SQLSMALLINT sqltype, bool is_unsigned);

bool sqlgetfunction(const OdbcDbc_libodbc &dbc, SQLUSMALLINT functionId)
{
    SQLRETURN ret;
    SQLUSMALLINT val = SQL_FALSE;
    ret = dbc.drv()->SQLGetFunctions(dbc.getHandle(), functionId, &val);
    if(SQL_SUCCEEDED(ret))
        return val == SQL_TRUE;
    else
        throw EngineException("SQLGetFunctions() failed");
}

template<typename T>
T sqlgetinfo(const OdbcDbc_libodbc &dbc, SQLSMALLINT info)
{
    T value = T();
    SQLRETURN ret = dbc.drv()->SQLGetInfoW(dbc.getHandle(), info, &value, sizeof(value), 0);
    if(!SQL_SUCCEEDED(ret))
        throw EngineException("SQLGetInfo() returns an unexpected error"); /// @todo
    else
        return value;
}


template<>
String sqlgetinfo<String>(const OdbcDbc_libodbc &dbc, SQLSMALLINT info)
{
    if(dbc.usingUnicode())
    {
        OdbcStrW s(256);
        SQLSMALLINT ind = SQL_NULL_DATA;
        SQLRETURN ret = dbc.drv()->SQLGetInfoW(dbc.getHandle(), info, s.ptr(), s.size()*sizeof(SQLWCHAR), &ind);
        if(!SQL_SUCCEEDED(ret))
            throw EngineException("SQLGetInfo() returns an unexpected error"); /// @todo
        else
            return s.str(ind/sizeof(SQLWCHAR));
    }
    else
    {
        OdbcStrA s(256);
        SQLSMALLINT ind = SQL_NULL_DATA;
        SQLRETURN ret = dbc.drv()->SQLGetInfoA(dbc.getHandle(), info, s.ptr(), s.size()*sizeof(SQLCHAR), &ind);
        if(!SQL_SUCCEEDED(ret))
            throw EngineException("SQLGetInfo() returns an unexpected error"); /// @todo
        else
            return s.str(ind/sizeof(SQLCHAR), dbc.getDbcEncoding());
    }
}


template<typename T>
inline void copy_diag_and_throw(const OdbcDbc_libodbc &dbc, T &obj, const CodePosInfo &pos, SQLHANDLE handle, SQLSMALLINT htype, const String &desc)
{

    SQLINTEGER i = 0;
    SQLINTEGER native = 0;
    SQLSMALLINT len = 0;
    SQLRETURN ret;
    OdbcDiag *first_diag = 0;

    if(dbc.usingUnicode())
    {
        OdbcStrW state(6);
        OdbcStrW text(512+1);
        do
        {
            ret = dbc.drv()->SQLGetDiagRecW(htype, handle, ++i, state.ptr(), &native, text.ptr(), text.size(), &len);
            if(SQL_SUCCEEDED(ret))
            {
                OdbcDiag_libodbc diag = OdbcDiag_libodbc(DAL_STATE_ERROR, pos,
                                                         text.str(len), desc, native, state.str(5).utf8(), native);
                OdbcDiag &d = obj.appendDiagRec(diag);
                if(!first_diag)
                    first_diag = &d;
            }
            else if(ret != SQL_NO_DATA)
            {
                throw EngineException("SQLGetDiagRec() returns an unexpected error");
            }
        }
        while(SQL_SUCCEEDED(ret));

    }
    else
    {
        OdbcStrA state(6);
        OdbcStrA text(512+1);
        do
        {
            ret = dbc.drv()->SQLGetDiagRecA(htype, handle, ++i, state.ptr(), &native, text.ptr(), text.size(), &len);
            if(SQL_SUCCEEDED(ret))
            {
                OdbcDiag_libodbc diag = OdbcDiag_libodbc(DAL_STATE_ERROR, pos,
                                                         text.str(len, dbc.getDbcEncoding()), desc, native, state.str(5, "ASCII").utf8(), native);
                OdbcDiag &d = obj.appendDiagRec(diag);
                if(!first_diag)
                    first_diag = &d;
            }
            else if(ret != SQL_NO_DATA)
            {
                throw EngineException("SQLGetDiagRec() returns an unexpected error");
            }
        }
        while(SQL_SUCCEEDED(ret));
    }
    if(first_diag)
        first_diag->raiseException();
    else
        throw EngineException("Last function returns SQL_ERROR, but no DIAG avail.");
}





inline std::string diag_sqlstate(const CodePosInfo &info, const OdbcDbc_libodbc &dbc, SQLHANDLE handle, SQLSMALLINT htype)
{
    SQLINTEGER native = 0;
    SQLSMALLINT len = 0;
    SQLRETURN ret;

    std::string s;

    if(dbc.usingUnicode())
    {
        OdbcStrW state(6);
        OdbcStrW text(512+1);


        ret = dbc.drv()->SQLGetDiagRecW(htype, handle, 1, state.ptr(), &native, text.ptr(), text.size(), &len);
        s = state.str(5);
    }
    else
    {
        OdbcStrA state(6);
        OdbcStrA text(512+1);
        ret = dbc.drv()->SQLGetDiagRecA(htype, handle, 1, state.ptr(), &native, text.ptr(), text.size(), &len);
        s = state.str(5, "ASCII");
    }


    if(SQL_SUCCEEDED(ret))
    {
        return s;
    }
    else if(ret == SQL_NO_DATA)
    {
		throw EngineException(std::string("SQLGetDiagRec() returns SQL_NO_DATA when asking for SQLSTATE at ") + info.str().utf8());
    }
    else
    {
        throw EngineException("SQLGetDiagRec() returns an unexpected error");
    }

}


/*
  void extract_error(
  ODBC30Drv *drv,
  const char *fn,
  SQLHANDLE handle,
  SQLSMALLINT type)
  {
  SQLINTEGER   i = 0;
  SQLINTEGER   native;
  SQLWCHAR      state[ 7 ];
  OdbcStrW  text(512);
  SQLSMALLINT  len;
  SQLRETURN    ret;

  fprintf(stderr,
  "\n"
  "The driver reported the following diagnostics whilst running "
  "%s\n\n",
  fn);

  do
  {
  ret = drv->SQLGetDiagRecW(type, handle, ++i, state, &native, text.ptr(),
  text.size(), &len );
  if (SQL_SUCCEEDED(ret))
  {
  std::wcout << text.str(len) << std::endl;
  }
//            printf("%s:%d:%d:%s\n", state, i, native, text);
}
while( ret == SQL_SUCCESS );
}
*/
#define THROW_ODBC_DIAG_ERROR(env, obj, handle, htype, desc)            \
    copy_diag_and_throw(env, obj, DBWTL_SPOS(), handle, htype, String(desc))


/*
  struct delete_object
  {
  template <typename T>
  void operator()(T ptr){ delete ptr;}

  template<typename U, typename V>
  void operator()(std::pair<U, V> val){ delete val.second;}
  };
*/


static void delete_resultset(OdbcResult_libodbc* rs)
{
    delete rs;
}




OdbcDiag&
OdbcEnv_libodbc::appendDiagRec(const OdbcDiag &diag)
{

    return *this->m_diag.push_back(diag.clone());
}

OdbcDiag&
OdbcResult_libodbc::appendDiagRec(const OdbcDiag &diag)
{

    return *this->m_diag.push_back(diag.clone());
}

OdbcDiag&
OdbcDbc_libodbc::appendDiagRec(const OdbcDiag &diag)
{

    return *this->m_diag.push_back(diag.clone());
}

OdbcDiag&
OdbcStmt_libodbc::appendDiagRec(const OdbcDiag &diag)
{

    return *this->m_diag.push_back(diag.clone());
}





//..............................................................................
/////////////////////////////////////////////////////////////// OdbcBlob_libodbc

/// @details
///
ODBC30Drv*
OdbcBlob_libodbc::drv(void) const
{
    return this->m_data.m_resultset.drv();
}


/// @details
///
SQLHSTMT
OdbcBlob_libodbc::getHandle(void) const
{
    return this->m_data.m_resultset.getHandle();
}


/// @details
///
OdbcBlob_libodbc::OdbcBlob_libodbc(const OdbcData_libodbc& data, SQLLEN &ind)
    : OdbcBlob(),
      m_data(data),
      m_buf(),
	  m_ind(ind),
      m_putback(DAL_STREAMBUF_PUTBACK)
{
/*
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
*/

    // set streambuf pointers to end, so underflow() will handle the first fill
    char *end = m_buf + sizeof(m_buf);
    setg(end, end, end);
}


/// @details
///
OdbcBlob_libodbc::~OdbcBlob_libodbc(void)
{
/*
  ISC_STATUS sv[20];

  if(this->m_handle)
  {
  this->drv()->isc_close_blob(sv, &this->m_handle);

  if(sv[0] == 1 && sv[1] > 0)
  {
  THROW_ERROR((&this->m_data.m_resultset), sv, "isc_close_blob failed");
  }
  }
*/
}


/// @details
///
OdbcBlob_libodbc::int_type
OdbcBlob_libodbc::underflow()
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

    SQLRETURN ret =
        this->drv()->SQLGetData(this->getHandle(), this->m_data.m_colnum,
                                SQL_C_BINARY, start, sizeof(m_buf) - (start - base), &m_ind);


    if(ret == SQL_NO_DATA)
        return traits_type::eof();

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->m_data.m_resultset.getDbc(), this->m_data.m_resultset.getStmt(),
                              this->getHandle(), SQL_HANDLE_STMT,
                              "GetData failed");
    }

    //std::cout << "IND: " << ind << std::endl;

    if(ret == SQL_SUCCESS_WITH_INFO)
    {
        //std::cout << "TRUNC" << std::endl;
        std::string state = diag_sqlstate(DBWTL_CPI, this->m_data.m_resultset.getDbc(),
                                          this->getHandle(), SQL_HANDLE_STMT);
        if(state != "01004")
        {
            THROW_ODBC_DIAG_ERROR(this->m_data.m_resultset.getDbc(), this->m_data.m_resultset.getStmt(),
                                  this->getHandle(), SQL_HANDLE_STMT,
                                  "GetData failed");
        }
        setg(base, start, start + ( sizeof(m_buf) - (start - base)));
        return traits_type::to_int_type(*gptr());
    }

    if(m_ind == SQL_NULL_DATA || m_ind == 0)
        return traits_type::eof();
    else
    {
        assert(m_ind > 0); // there mus be data avail!
        setg(base, start, start + m_ind);
        return traits_type::to_int_type(*gptr());
    }
}


/// @details
///
bool
OdbcBlob_libodbc::isNull(void) const
{
	return this->m_ind == SQL_NULL_DATA;
}





















//..............................................................................
/////////////////////////////////////////////////////////////// OdbcMemo_libodbc

/// @details
///
ODBC30Drv*
OdbcMemo_libodbc::drv(void) const
{
    return this->m_data.m_resultset.drv();
}


/// @details
///
SQLHSTMT
OdbcMemo_libodbc::getHandle(void) const
{
    return this->m_data.m_resultset.getHandle();
}


/// @details
///
OdbcMemo_libodbc::OdbcMemo_libodbc(const OdbcData_libodbc& data, SQLLEN &ind)
    : OdbcMemo(),
      m_data(data),
      m_buf(),
	  m_ind(ind),
      m_putback(DAL_STREAMBUF_PUTBACK)
{
/*
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
*/

    // set streambuf pointers to end, so underflow() will handle the first fill
    char_type *end = m_buf + sizeof(m_buf);
    setg(end, end, end);
}


/// @details
///
OdbcMemo_libodbc::~OdbcMemo_libodbc(void)
{
/*
  ISC_STATUS sv[20];

  if(this->m_handle)
  {
  this->drv()->isc_close_blob(sv, &this->m_handle);

  if(sv[0] == 1 && sv[1] > 0)
  {
  THROW_ERROR((&this->m_data.m_resultset), sv, "isc_close_blob failed");
  }
  }
*/
}


/// @details
///
OdbcMemo_libodbc::int_type
OdbcMemo_libodbc::underflow()
{
    if (gptr() < egptr()) // buffer not exhausted
        return traits_type::to_int_type(*gptr());

    char_type *base = &m_buf[0];
    char_type *start = base;

    if (eback() == base) // true when this isn't the first fill
    {
        // Make arrangements for putback characters
        std::memmove(base, egptr() - m_putback, m_putback);
        start += m_putback;
    }

    // start is now the start of the buffer, proper.
    // Read from fptr_ in to the provided buffer
    

    SQLRETURN ret;


    if(sizeof(char_type) == sizeof(SQLWCHAR))
    {
        ret = this->drv()->SQLGetData(this->getHandle(), this->m_data.m_colnum,
                                      SQL_C_WCHAR, start, sizeof(m_buf) - (start - base), &m_ind);

//  std::wcout << "DATA: " << std::wstring(start, start + (sizeof(m_buf) - (start - base))) << std::endl;
    }
    else
    {
        std::vector<SQLWCHAR> tmp((sizeof(m_buf)/sizeof(wchar_t) - (start - base)));
        ret = this->drv()->SQLGetData(this->getHandle(), this->m_data.m_colnum,
                                      SQL_C_WCHAR, tmp.data(), tmp.size()*sizeof(SQLWCHAR), &m_ind);
        
		if(SQL_SUCCEEDED(ret))
		{
			for(size_t i = 0; i < (sizeof(m_buf)/sizeof(wchar_t) - (start - base)); ++i)
			{
				assert(i < (DAL_STREAMBUF_BUFSIZE + DAL_STREAMBUF_PUTBACK));

				assert(sizeof(m_buf)/sizeof(wchar_t) ==  (DAL_STREAMBUF_BUFSIZE + DAL_STREAMBUF_PUTBACK));

				//std::cout << i << " " << (sizeof(m_buf) << " " << (start - base))  << std::endl;
				start[i] = tmp[i];
			}
		}
    }


	if(ret == SQL_NO_DATA)
		return traits_type::eof();


    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->m_data.m_resultset.getDbc(), this->m_data.m_resultset.getStmt(),
                              this->getHandle(), SQL_HANDLE_STMT,
                              "GetData failed");
    }

    //std::cout << "IND: " << ind << std::endl;



    if(ret == SQL_SUCCESS_WITH_INFO)
    {
        //std::cout << "TRUNC" << std::endl;
		std::string state;
		try
		{
			state = diag_sqlstate(DBWTL_CPI, this->m_data.m_resultset.getDbc(),
                                          this->getHandle(), SQL_HANDLE_STMT);
		}
		catch(...) {}
        if(state != "01004")
        {
            THROW_ODBC_DIAG_ERROR(this->m_data.m_resultset.getDbc(), this->m_data.m_resultset.getStmt(),
                                  this->getHandle(), SQL_HANDLE_STMT,
                                  "GetData failed");
        }
        setg(base, start, start + ( sizeof(m_buf) - (start - base)));
        return traits_type::to_int_type(*gptr());
    }

    if(m_ind == SQL_NULL_DATA || m_ind == 0)
        return traits_type::eof();
    else
    {
        assert(m_ind > 0); // there mus be data avail!
        setg(base, start, start + (m_ind/sizeof(SQLWCHAR)));
        return traits_type::to_int_type(*gptr());
    }
}


/// @details
///
bool
OdbcMemo_libodbc::isNull(void) const
{
    return this->m_ind == SQL_NULL_DATA;
}





OdbcResult_libodbc&
OdbcData_libodbc::getResultset(void) const
{
    return this->m_resultset;
}

OdbcStmt_libodbc&
OdbcData_libodbc::getStmt(void) const { return this->getResultset().getStmt(); }
ODBC30Drv*
OdbcData_libodbc::drv(void) const {  return this->getResultset().drv(); }

SQLHSTMT
OdbcData_libodbc::getHandle(void) const
{
    return this->getResultset().getHandle();
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


void
OdbcData_libodbc::bindIndicator(colnum_t colnum, SQLLEN *len)
{
    DBWTL_TRACE2(colnum, *len);
    //SQLRETURN ret;


    assert(!"do not call");
#if 0

    SQLHDESC desc = SQL_NULL_HANDLE;

    ret = this->drv()->SQLGetStmtAttrW(this->getHandle(), SQL_ATTR_APP_ROW_DESC,
                                       &desc, sizeof(desc), NULL);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "SQLGetStmtAttrW failed");
    }


    SQLSMALLINT count = 0;

    ret = this->drv()->SQLGetDescFieldW(desc, 0, SQL_DESC_COUNT, &count, SQL_IS_SMALLINT, NULL);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "SQLGetDescFieldW failed");
    }


    if((count < 0 ? 0 : count) < colnum)
    {


        ret = this->drv()->SQLSetDescFieldW(desc, 0, SQL_DESC_COUNT, reinterpret_cast<void*>(count),
                                            SQL_IS_SMALLINT);


        if(! SQL_SUCCEEDED(ret))
        {
            THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "SQLSetDescFieldW failed");
        }

    }



    ret = this->drv()->SQLSetDescFieldW(desc, colnum, SQL_DESC_TYPE, (void*)SQL_C_WCHAR,
                                        SQL_IS_SMALLINT);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "SQLSetDescFieldW failed");
    }




    ret = this->drv()->SQLSetDescFieldW(desc, colnum, SQL_DESC_DATA_PTR, (void*)1,
                                        SQL_IS_POINTER);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "SQLSetDescFieldW failed");
    }

/*
  ret = this->drv()->SQLSetDescFieldW(desc, colnum, SQL_DESC_OCTET_LENGTH, (void*)0,
  SQL_IS_INTEGER);

  if(! SQL_SUCCEEDED(ret))
  {
  THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "SQLSetDescFieldW failed");
  }
*/

    ret = this->drv()->SQLSetDescFieldW(desc, colnum, SQL_DESC_INDICATOR_PTR, len,
                                        SQL_IS_POINTER);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "SQLSetDescFieldW failed");
    }

/*

  if(! SQL_SUCCEEDED(ret))
  {
  THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "DescribeCol failed");
  }

  SQLGetStmtAttr
  SQLGetDescField

  SQLSetDescField
  SQL_DESC_INDICATOR_PTR
*/
#endif
}


///
OdbcData_libodbc::OdbcData_libodbc(OdbcResult_libodbc& result, colnum_t colnum, bool locked)
    : OdbcData(),
      m_resultset(result),
      m_colnum(colnum),
      m_blobbuf(),
      m_memobuf(),
	  m_blob_cache(),
	  m_memo_cache(), 
      m_memostream(),
      m_bufsize(),
      m_value(),
	  m_is_bound(false)
{
    DBWTL_TRACE2(colnum, locked);
    assert(result.isOpen());


	if(colnum != 0)
		this->initValue();

}



//
OdbcData_libodbc::~OdbcData_libodbc(void)
{ }




//
UnicodeStreamBuf*
OdbcData_libodbc::getMemoStream(void) const
{
    DALTRACE("VISIT");
    DBWTL_BUGCHECK(this->daltype() == DAL_TYPE_MEMO);

	/*
      if(this->isnull())
      throw NullException(String("OdbcData_libodbc result column"));
	*/

	if(/*this->m_resultset.getDbc().usingUnicode() && */ this->m_value.sqltype == SQL_WLONGVARCHAR)
    {
        if(! this->m_memobuf.get())
        {
			this->m_memobuf.reset(new OdbcMemo_libodbc(*this, this->m_value.ind));
        }
        return this->m_memobuf.get();
    }
    else
    {
        OdbcBlob_libodbc *blob = this->getBlobStream();

        if(! this->m_memostream.get())
        {
            this->m_memostream.reset(new std::wstringstream());
        }

        this->m_memostream->clear();

        std::stringstream ss;
        ss.imbue(std::locale("C"));
        ss << blob;

        *this->m_memostream << String(ss.str(), this->m_resultset.getDbc().getDbcEncoding());

        return this->m_memostream->rdbuf();
    }
}



//
OdbcBlob_libodbc*
OdbcData_libodbc::getBlobStream(void) const
{
    DALTRACE("VISIT");
    //DBWTL_BUGCHECK(this->daltype() == DAL_TYPE_BLOB || (this->daltype() == DAL_TYPE_MEMO && !this->m_resultset.getDbc().usingUnicode()));

	/*
      if(this->isnull())
      throw NullException(String("OdbcData_libodbc result column"));
	*/

    if(! this->m_blobbuf.get())
    {
        this->m_blobbuf.reset(new OdbcBlob_libodbc(*this, this->m_value.ind));
    }

    return this->m_blobbuf.get();
}



String OdbcData_libodbc::getString(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
		DBWTL_BUGCHECK(this->m_value.ind >= 0);
        if(m_value.strbufA.size())
        {
            return m_value.strbufA.str(this->m_value.ind/sizeof(SQLCHAR), this->m_resultset.getDbc().getDbcEncoding());
        }
        else
            return m_value.strbufW.str(this->m_value.ind/(signed)sizeof(SQLWCHAR) > m_value.strbufW.size() ?
                                       m_value.strbufW.size()-1 : this->m_value.ind/sizeof(SQLWCHAR));
    }
}

signed short int OdbcData_libodbc::getSShort(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.sShortInt;
    }
}

unsigned short int OdbcData_libodbc::getUShort(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.uShortInt;
    }
}


//
signed long int
OdbcData_libodbc::getSLong(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.sLongInt;
    }
}




unsigned long int OdbcData_libodbc::getULong(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.uLongInt;
    }
}

float OdbcData_libodbc::getFloat(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.float_;
    }
}

double OdbcData_libodbc::getDouble(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.double_;
    }
}

unsigned char OdbcData_libodbc::getBit(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.bit;
    }
}
signed char OdbcData_libodbc::getSTinyint(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.sTinyInt;
    }
}
unsigned char OdbcData_libodbc::getUTinyint(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.uTinyInt;
    }
}

signed long long OdbcData_libodbc::getSBigint(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.sBigInt;
    }
}
unsigned long long OdbcData_libodbc::getUBigint(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return this->m_value.data.uBigInt;
    }
}

TVarbinary OdbcData_libodbc::getBinary(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return TVarbinary(this->m_value.varbinary.data(), this->m_value.ind);
    }
}
unsigned long int OdbcData_libodbc::getBookmark(void) const
{
    throw FeatureUnsuppException("Bookmark support for ODBC is not implemented yet.");
}
const  unsigned char* OdbcData_libodbc::getVarbookmark(void) const
{
    throw FeatureUnsuppException("Bookmark support for ODBC is not implemented yet.");
}
TDate OdbcData_libodbc::getDate(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return TDate(this->m_value.data.date.year,
                     this->m_value.data.date.month,
                     this->m_value.data.date.day);
    }
}

TTime OdbcData_libodbc::getTime(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return TTime(this->m_value.data.time.hour,
                     this->m_value.data.time.minute,
                     this->m_value.data.time.second);
    }
}
TTimestamp OdbcData_libodbc::getTimestamp(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        return TTimestamp(this->m_value.data.timestamp.year,
                          this->m_value.data.timestamp.month,
                          this->m_value.data.timestamp.day,
                          this->m_value.data.timestamp.hour,
                          this->m_value.data.timestamp.minute,
                          this->m_value.data.timestamp.second,
                          this->m_value.data.timestamp.fraction);
    }
}

TNumeric OdbcData_libodbc::getNumeric(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);
    if(this->isnull())
        throw NullException(String("OdbcData_libodbc result column"));
    else
    {
        const SQL_NUMERIC_STRUCT &num = this->m_value.data.numeric;

        TNumeric res(0);
        TNumeric denom(1);

        for(size_t i = 0; i < SQL_MAX_NUMERIC_LEN; ++i)
        {
            res = res + num.val[i] * denom;
            denom = denom * 256;
        }

        res.rescale(num.scale);
        // NOTE: The ODBC 3.0 spec required drivers to return the sign as
        // 1 for positive numbers and 2 for negative number. This was changed in the
        // ODBC 3.5 spec to return 0 for negative instead of 2.
        if(num.sign == 0 || num.sign == 2)
        {
            res = res * -1;
        }

        return res;
    }
}


BlobStream
OdbcData_libodbc::cast2BlobStream(std::locale loc) const
{
    if(this->m_blob_cache.get())
    {
        this->m_blob_cache->seekg(0);
        return BlobStream(this->m_blob_cache->rdbuf());
    }
    else
        return BlobStream(this->getBlobStream());
}

MemoStream
OdbcData_libodbc::cast2MemoStream(std::locale loc) const
{
    if(this->m_memo_cache.get())
    {
        this->m_memo_cache->seekg(0);
        return MemoStream(this->m_memo_cache->rdbuf());
    }
    else
        return MemoStream(this->getMemoStream());
}

Blob
OdbcData_libodbc::cast2Blob(std::locale loc) const
{
	if(!this->m_blob_cache.get())
    {
        this->m_blob_cache.reset(new std::stringstream());
        (*this->m_blob_cache.get()) << this->getBlobStream();
    }
    this->m_blob_cache->rdbuf()->pubseekpos(0);
    return Blob(this->m_blob_cache->rdbuf());
}

Memo
OdbcData_libodbc::cast2Memo(std::locale loc) const
{
    if(!this->m_memo_cache.get())
    {
        this->m_memo_cache.reset(new std::wstringstream());
        (*this->m_memo_cache.get()) << this->getMemoStream();
    }
    this->m_memo_cache->rdbuf()->pubseekpos(0);
    return Memo(this->m_memo_cache->rdbuf());
}


//
bool
OdbcData_libodbc::isnull(void) const
{
    return this->m_value.ind == SQL_NULL_DATA;
}


//
void
OdbcData_libodbc::fetchParts(void)
{
    DBWTL_TRACE0();

	return;

   // if(this->m_value.ind == SQL_NULL_DATA || this->m_value.ind == SQL_NTS)
   //     return;

   // SQLRETURN ret;
   // SQLSMALLINT sqltype = SQL_UNKNOWN_TYPE;
   // SQLULEN size = 0;

   // if(this->m_resultset.getDbc().usingUnicode())
   //     ret = this->drv()->SQLDescribeColW(this->getHandle(), m_colnum, NULL, 0, NULL,
   //                                        &sqltype, &size, NULL, NULL);
   // else
   //     ret = this->drv()->SQLDescribeColA(this->getHandle(), m_colnum, NULL, 0, NULL,
   //                                        &sqltype, &size, NULL, NULL);

   // if(! SQL_SUCCEEDED(ret))
   // {
   //     THROW_ODBC_DIAG_ERROR(this->m_resultset.getStmt().getDbc(),
   //                           this->m_resultset.getStmt(), this->m_resultset.getHandle(),
   //                           SQL_HANDLE_STMT, "DescribeCol failed");
   // }

   // // UNICODE
   // if(sqltype == SQL_WCHAR || sqltype == SQL_WVARCHAR)
   // {
   //     if(this->m_value.ind > 0 && (unsigned)this->m_value.ind <= this->m_value.strbufW.size()*sizeof(SQLWCHAR))
   //     {
   //         return; // nothing to do.
   //     }
   //     else if(this->m_value.ind > 0 && (unsigned)this->m_value.ind > this->m_value.strbufW.size()*sizeof(SQLWCHAR))
   //     {
   //         // we know the exact size of the data, everything could
   //         // be read at once.
   //         if((unsigned)this->m_value.ind > this->m_value.strbufW.max_size())
   //             throw EngineException(FORMAT2("Unable to fetch %d bytes, max string size is %d",
   //                                           this->m_value.ind, this->m_value.strbufW.max_size()));
   //         m_value.strbufW.resize( (this->m_value.ind/sizeof(SQLWCHAR))+1); // add null-terminator
   //         this->m_value.ind = SQL_NULL_DATA; // reset indicator
   //         ret = this->drv()->SQLGetData(this->getHandle(), this->m_colnum,
   //                                       SQL_C_WCHAR, m_value.strbufW.ptr(),
   //                                       m_value.strbufW.size()*sizeof(SQLWCHAR), &this->m_value.ind);
   //         if(ret == SQL_NO_DATA)
   //         {
   //             m_value.ind = SQL_NULL_DATA;
   //         }
   //         else if(! SQL_SUCCEEDED(ret))
   //             THROW_ODBC_DIAG_ERROR(this->m_resultset.getDbc(), this->m_resultset.getStmt(),
   //                                   this->getHandle(), SQL_HANDLE_STMT,
   //                                   "SQLGetData() failed");
   //     }
   //     else if(this->m_value.ind == SQL_NO_TOTAL)
   //     {
   //         SQLLEN bufsize = DBWTL_ODBC_LOB_BUFSIZE;
   //         m_value.strbufW.resize(bufsize);
   //         SQLWCHAR *ptr = m_value.strbufW.ptr();
   //         do
   //         {
   //             this->m_value.ind = SQL_NULL_DATA; // reset indicator
   //             ret = this->drv()->SQLGetData(this->getHandle(), this->m_colnum,
   //                                           SQL_C_WCHAR, ptr,
   //                                           bufsize*sizeof(SQLWCHAR), &this->m_value.ind);
   //             if(ret == SQL_SUCCESS_WITH_INFO
   //                && diag_sqlstate(this->getStmt().getDbc(), this->getHandle(), SQL_HANDLE_STMT) == "01004")
   //             {
   //                 m_value.strbufW.resize((m_value.strbufW.size()-1)+bufsize); // remove null terminator
   //                 ptr = m_value.strbufW.ptr() + (m_value.strbufW.size()-bufsize);
   //             }
   //             else if(ret == SQL_NO_DATA)
   //             {
   //                 m_value.ind = SQL_NULL_DATA;
   //             }
   //             else if(ret == SQL_SUCCESS) // final part
   //             {
   //                 assert(this->m_value.ind != SQL_NO_TOTAL);
   //                 assert(this->m_value.ind != 0);
   //                 m_value.strbufW.resize(m_value.strbufW.size()-(bufsize-((this->m_value.ind/sizeof(SQLWCHAR)))));
   //                 m_value.ind = m_value.strbufW.size()*sizeof(SQLWCHAR);
   //                 break;
   //             }
   //             else
   //             {
   //                 THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(),
   //                                       this->getHandle(), SQL_HANDLE_STMT, "SQLGetData() failed");
   //             }
   //         }
   //         while(ret == SQL_SUCCESS_WITH_INFO);
   //     }
   //     else
   //     {
   //         throw EngineException(FORMAT1("Invalid indicator value at fetchParts(): %d", this->m_value.ind));
   //     }
   // }
   // // ANSI
   // else if(sqltype == SQL_CHAR || sqltype == SQL_VARCHAR)
   // {
   //     if(this->m_value.ind > 0 && (unsigned)this->m_value.ind <= this->m_value.strbufA.size()*sizeof(SQLCHAR))
   //     {
   //         return; // nothing to do.
   //     }
   //     else if(this->m_value.ind > 0 && (unsigned)this->m_value.ind > this->m_value.strbufA.size()*sizeof(SQLCHAR))
   //     {
   //         // we know the exact size of the data, everything could
   //         // be read at once.
   //         if((unsigned)this->m_value.ind > this->m_value.strbufA.max_size())
   //             throw EngineException(FORMAT2("Unable to fetch %d bytes, max string size is %d",
   //                                           this->m_value.ind, this->m_value.strbufA.max_size()));
   //         m_value.strbufA.resize( (this->m_value.ind/sizeof(SQLCHAR))+1); // add null-terminator
   //         this->m_value.ind = SQL_NULL_DATA; // reset indicator
   //         ret = this->drv()->SQLGetData(this->getHandle(), this->m_colnum,
   //                                       SQL_C_CHAR, m_value.strbufA.ptr(),
   //                                       m_value.strbufA.size()*sizeof(SQLCHAR), &this->m_value.ind);
   //         if(ret == SQL_NO_DATA)
   //         {
   //             m_value.ind = SQL_NULL_DATA;
   //         }
			//else if(! SQL_SUCCEEDED(ret))
   //             THROW_ODBC_DIAG_ERROR(this->m_resultset.getDbc(), this->m_resultset.getStmt(),
   //                                   this->getHandle(), SQL_HANDLE_STMT,
   //                                   "SQLGetData() failed");
   //     }
   //     else if(this->m_value.ind == SQL_NO_TOTAL)
   //     {
   //         SQLLEN bufsize = DBWTL_ODBC_LOB_BUFSIZE;
   //         m_value.strbufA.resize(bufsize);
   //         SQLCHAR *ptr = m_value.strbufA.ptr();
   //         do
   //         {
   //             this->m_value.ind = SQL_NULL_DATA; // reset indicator
   //             ret = this->drv()->SQLGetData(this->getHandle(), this->m_colnum,
   //                                           SQL_C_CHAR, ptr,
   //                                           bufsize*sizeof(SQLCHAR), &this->m_value.ind);
   //             if(ret == SQL_SUCCESS_WITH_INFO
   //                && diag_sqlstate(this->getStmt().getDbc(), this->getHandle(), SQL_HANDLE_STMT) == "01004")
   //             {
   //                 m_value.strbufA.resize((m_value.strbufA.size()-1)+bufsize); // remove null terminator
   //                 ptr = m_value.strbufA.ptr() + (m_value.strbufA.size()-bufsize);
   //             }
			//	else if(ret == SQL_NO_DATA)
   //             {
   //                 m_value.ind = SQL_NULL_DATA;
   //             }
   //             else if(ret == SQL_SUCCESS) // final part
   //             {
   //                 assert(this->m_value.ind != SQL_NO_TOTAL);
   //                 assert(this->m_value.ind != 0);
   //                 m_value.strbufA.resize(m_value.strbufA.size()-(bufsize-((this->m_value.ind/sizeof(SQLCHAR)))));
   //                 m_value.ind = m_value.strbufA.size()*sizeof(SQLCHAR);
   //                 break;
   //             }
   //             else
   //             {
   //                 THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(),
   //                                       this->getHandle(), SQL_HANDLE_STMT, "SQLGetData() failed");
   //             }
   //         }
   //         while(ret == SQL_SUCCESS_WITH_INFO);
   //     }
   //     else
   //     {
   //         throw EngineException(FORMAT1("Invalid indicator value at fetchParts(): %d", this->m_value.ind));
   //     }
   // }
   // // BINARY
   // else if(sqltype == SQL_BINARY || sqltype == SQL_VARBINARY)
   // {
   //     DBWTL_NOTIMPL();
   // }
   // else
   // {
   //     // other types ignored
   // }
}


//
rowid_t
OdbcData_libodbc::getCurrentRowID(void) const
{
    DBWTL_TRACE;
    return this->m_resultset.getCurrentRowID();
}


//
void
OdbcData_libodbc::refresh(void)
{
    this->m_blobbuf.reset(0);
	this->m_memobuf.reset(0);
	this->m_memostream.reset(0);

	this->m_blob_cache.reset();
    this->m_memo_cache.reset();


	this->getdata();
}



void
OdbcData_libodbc::initValue(void)
{
	

    SQLRETURN ret;

	///

    if(this->m_resultset.getDbc().usingUnicode())
        ret = this->drv()->SQLDescribeColW(this->getHandle(), m_colnum, NULL, 0, NULL,
                                           &this->m_value.sqltype, &this->m_value.size, NULL, NULL);
    else
        ret = this->drv()->SQLDescribeColA(this->getHandle(), m_colnum, NULL, 0, NULL,
                                           &this->m_value.sqltype, &this->m_value.size, NULL, NULL);


    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(),
                              this->getHandle(), SQL_HANDLE_STMT, "DescribeCol failed");
    }


	///

    if(this->m_resultset.getDbc().usingUnicode())
        ret = this->drv()->SQLColAttributeW(this->getHandle(), m_colnum, SQL_DESC_UNSIGNED,
                                            NULL, 0, NULL, &this->m_value.is_unsigned);
    else
        ret = this->drv()->SQLColAttributeA(this->getHandle(), m_colnum, SQL_DESC_UNSIGNED,
                                            NULL, 0, NULL, &this->m_value.is_unsigned);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(),
                              this->getHandle(), SQL_HANDLE_STMT, "SQLColAttribute failed");
    }

	OdbcValue &val = this->m_value;


	// This is used for all datatypes except char and binary types
	val.buf = &this->m_value.data;

    switch(this->m_value.sqltype)
    {
    case SQL_LONGVARCHAR:
		val.ctype = SQL_C_CHAR;
		val.buf = 0;
		val.buflen = 0;
        break;
    case SQL_WLONGVARCHAR:
		val.ctype = SQL_C_WCHAR;
		val.buf = 0;
		val.buflen = 0;
        break;		
	case SQL_LONGVARBINARY:
		val.ctype = SQL_C_BINARY;
		val.buf = 0;
		val.buflen = 0;
        break;

    case SQL_CHAR:
    case SQL_VARCHAR:
        val.strbufA.resize((val.size == 0 || val.size > DBWTL_ODBC_MAX_STRING_SIZE) ? DBWTL_ODBC_MAX_STRING_SIZE : val.size+1); // size = chars
		val.ctype = SQL_C_CHAR;
		val.buf = m_value.strbufA.ptr();
		val.buflen = m_value.strbufA.size()*sizeof(SQLCHAR);
        break;

    case SQL_WCHAR:
    case SQL_WVARCHAR:
        val.strbufW.resize((val.size == 0 || val.size > DBWTL_ODBC_MAX_STRING_SIZE) ? DBWTL_ODBC_MAX_STRING_SIZE : val.size+1); // size = chars
		val.ctype = SQL_C_WCHAR;
		val.buf = m_value.strbufW.ptr();
		val.buflen = m_value.strbufW.size()*sizeof(SQLWCHAR);
        break;

    case SQL_DECIMAL:
    case SQL_NUMERIC:
		val.ctype = SQL_C_NUMERIC;
		val.buflen = sizeof(this->m_value.data.numeric);
        break;

    case SQL_SMALLINT:
		val.ctype = this->m_value.is_unsigned ? SQL_C_USHORT : SQL_C_SSHORT;
		val.buflen = this->m_value.is_unsigned ? sizeof(this->m_value.data.uShortInt) : sizeof(this->m_value.data.sShortInt);
        break;

	case SQL_INTEGER:
		val.ctype = this->m_value.is_unsigned ? SQL_C_ULONG : SQL_C_SLONG;
		val.buflen = this->m_value.is_unsigned ? sizeof(this->m_value.data.uLongInt) : sizeof(this->m_value.data.sLongInt);
		break;

    case SQL_REAL:
    case SQL_FLOAT:
		val.ctype = SQL_C_FLOAT;
		val.buflen = sizeof(this->m_value.data.float_);
        break;

    case SQL_DOUBLE:
		val.ctype = SQL_C_DOUBLE;
		val.buflen = sizeof(this->m_value.data.double_);
        break;

    case SQL_BIT:
		val.ctype = SQL_C_BIT;
		val.buflen = sizeof(this->m_value.data.bit);
        break;

    case SQL_TINYINT:
		val.ctype = this->m_value.is_unsigned ? SQL_C_UTINYINT : SQL_C_STINYINT;
		val.buflen = this->m_value.is_unsigned ? sizeof(this->m_value.data.uTinyInt) : sizeof(this->m_value.data.sTinyInt);
        break;

    case SQL_BIGINT:
		val.ctype = this->m_value.is_unsigned ? SQL_C_UBIGINT : SQL_C_SBIGINT;
		val.buflen = this->m_value.is_unsigned ? sizeof(this->m_value.data.uBigInt) : sizeof(this->m_value.data.sBigInt);
        break;

    case SQL_BINARY:
    case SQL_VARBINARY:
        val.varbinary.resize((val.size == 0 || val.size > DBWTL_ODBC_MAX_VARBINARY_SIZE) ? DBWTL_ODBC_MAX_VARBINARY_SIZE : val.size+1); // size = chars
		val.ctype = SQL_C_BINARY;
		val.buf = m_value.varbinary.data();
		val.buflen = m_value.varbinary.size();
        break;

    case SQL_TYPE_DATE:
		val.ctype = SQL_C_TYPE_DATE;
		val.buflen = sizeof(this->m_value.data.date);
        break;

    case SQL_TYPE_TIME:
		val.ctype = SQL_C_TYPE_TIME;
		val.buflen = sizeof(this->m_value.data.time);
        break;

    case SQL_TYPE_TIMESTAMP:
		val.ctype = SQL_C_TYPE_TIMESTAMP;
		val.buflen = sizeof(this->m_value.data.timestamp);
        break;

	/// @bug GUID is an ODBC 3.5 type. Needs handling via version. for now, we use string conversion
	case SQL_GUID:
        val.strbufA.resize(100+1); // size = chars
		val.ctype = SQL_C_CHAR;
		val.buf = m_value.strbufA.ptr();
		val.buflen = m_value.strbufA.size()*sizeof(SQLCHAR);
        break;

//case SQL_TYPE_UTCDATETIME:
//case SQL_TYPE_UTCTIME:
    case SQL_INTERVAL_MONTH:
    case SQL_INTERVAL_YEAR:
    case SQL_INTERVAL_YEAR_TO_MONTH:
    case SQL_INTERVAL_DAY:
    case SQL_INTERVAL_HOUR:
    case SQL_INTERVAL_MINUTE:
    case SQL_INTERVAL_SECOND:
    case SQL_INTERVAL_DAY_TO_HOUR:
    case SQL_INTERVAL_DAY_TO_MINUTE:
    case SQL_INTERVAL_DAY_TO_SECOND:
    case SQL_INTERVAL_HOUR_TO_MINUTE:
    case SQL_INTERVAL_HOUR_TO_SECOND:
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        /// @todo Implement Interval types
        throw FeatureUnsuppException("Interval types for ODBC are not implemented yet.");

	default:
		// raise exception!!!
		//this->m_resultset.columnName(this->m_colnum)
		throw std::runtime_error(FORMAT2("unhandled sqltype for %s %d", this->m_resultset.columnName(this->m_colnum), this->m_value.sqltype));
	};
}

bool
OdbcData_libodbc::getdata(void)
{
	SQLRETURN ret;

    if(m_colnum == 0)
    {
        /// @todo Implement bookmark support for ODBC
        return true;
    }
	
	if(this->m_is_bound)
		return true;


	if(this->m_value.sqltype == SQL_WLONGVARCHAR || this->m_value.sqltype == SQL_LONGVARCHAR)
	{
		bool fetch_later = this->m_resultset.getDbc().getOption(DBWTL_ODBC_DEFERRED_LOB_FETCH).get<bool>();

		if(!fetch_later)
		{
			this->m_memo_cache.reset(new std::wstringstream());
			(*this->m_memo_cache.get()) << this->getMemoStream();
			this->m_memo_cache->rdbuf()->pubseekpos(0);
			return true;
		}
		return false;
	}
	else if(this->m_value.sqltype == SQL_LONGVARBINARY)
	{
		bool fetch_later = this->m_resultset.getDbc().getOption(DBWTL_ODBC_DEFERRED_LOB_FETCH).get<bool>();

		if(!fetch_later)
		{
			this->m_blob_cache.reset(new std::stringstream());
			(*this->m_blob_cache.get()) << this->getBlobStream();
			this->m_blob_cache->rdbuf()->pubseekpos(0);
			return true;
		}
		return false;
	}
	else
	{

		ret = this->drv()->SQLGetData(this->getHandle(), this->m_colnum, this->m_value.ctype, this->m_value.buf, this->m_value.buflen, &this->m_value.ind);

		if(ret == SQL_NO_DATA)
		{
			throw EngineException("SQLGetData() returns SQL_NO_DATA, this is unhandled here!");
		}
		else if(SQL_SUCCEEDED(ret))
		{
			return true;
		}
		else
		{
			THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(),
                                  this->getHandle(), SQL_HANDLE_STMT, "SQLGetData() failed!!!");
		}
	}
}

//
bool
OdbcData_libodbc::bindcol(void)
{
	SQLRETURN ret;

	if(m_colnum == 0)
	{
		/// @todo Implement bookmark support for ODBC
		return true;
	}





	if(this->m_value.sqltype == SQL_LONGVARCHAR
       || this->m_value.sqltype == SQL_LONGVARBINARY
       || this->m_value.sqltype == SQL_WLONGVARCHAR)
	{
		
		bool fetch_later = this->m_resultset.getDbc().getOption(DBWTL_ODBC_DEFERRED_LOB_FETCH).get<bool>();
		if(fetch_later)
		{
			ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, this->m_value.ctype,
                                          &this->m_value.data.wchar_dummy, sizeof(SQLWCHAR), &m_value.ind);
			if(! SQL_SUCCEEDED(ret))
			{
				THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "BindCol failed");
			}
			
			return false;
		} 
		else
			
			return false;
	}


	ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, this->m_value.ctype,
                                  this->m_value.buf, this->m_value.buflen, &m_value.ind);

	if(! SQL_SUCCEEDED(ret))
	{
		THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "BindCol failed");
	}

	this->m_is_bound = true;
	return true;

	/*


      SQLSMALLINT sqltype = SQL_UNKNOWN_TYPE;
      SQLULEN size = 0;
      SQLLEN is_unsigned = SQL_FALSE;

    

      if(this->m_resultset.getDbc().usingUnicode())
      ret = this->drv()->SQLDescribeColW(this->getHandle(), m_colnum, NULL, 0, NULL,
      &sqltype, &size, NULL, NULL);
      else
      ret = this->drv()->SQLDescribeColA(this->getHandle(), m_colnum, NULL, 0, NULL,
      &sqltype, &size, NULL, NULL);


      if(! SQL_SUCCEEDED(ret))
      {
      THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(),
      this->getHandle(), SQL_HANDLE_STMT, "DescribeCol failed");
      }

      if(this->m_resultset.getDbc().usingUnicode())
      ret = this->drv()->SQLColAttributeW(this->getHandle(), m_colnum, SQL_DESC_UNSIGNED,
      NULL, 0, NULL, &is_unsigned);
      else
      ret = this->drv()->SQLColAttributeA(this->getHandle(), m_colnum, SQL_DESC_UNSIGNED,
      NULL, 0, NULL, &is_unsigned);

      if(! SQL_SUCCEEDED(ret))
      {
      THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(),
      this->getHandle(), SQL_HANDLE_STMT, "SQLColAttribute failed");
      }

      // check for getinfo getdata extensions!!!!!
      if(sqltype == SQL_LONGVARCHAR
      || sqltype == SQL_LONGVARBINARY
      || sqltype == SQL_WLONGVARCHAR)
      {
      return false;
      }

      if(sqltype == SQL_WCHAR && !this->m_resultset.getDbc().usingUnicode())
      sqltype = SQL_CHAR;
      if(sqltype == SQL_WVARCHAR && !this->m_resultset.getDbc().usingUnicode())
      sqltype = SQL_VARCHAR;
      if(sqltype == SQL_WLONGVARCHAR && !this->m_resultset.getDbc().usingUnicode())
      sqltype = SQL_LONGVARCHAR;




      switch(sqltype)
      {
      case SQL_CHAR:
      case SQL_VARCHAR:
      std::cout << "ERRRRRRRR" << std::endl;
      //m_value.strbufA.resize(2);
      m_value.strbufA.resize(size == 0 || size > DBWTL_ODBC_MAX_STRING_SIZE ? 256 : size+1); // size = chars
      ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_CHAR,
      m_value.strbufA.ptr(), m_value.strbufA.size()*sizeof(SQLCHAR), &m_value.ind);
      break;
      case SQL_LONGVARCHAR:
      // We bind a dummy buffer, just for getting the indicator.
      // The real fetch is done by SQLGetData()
      m_value.strbufA.resize(5);

      // This is very tricky. We must set a valid data pointer, but the size is 0.
      // If data pointer is NULL, the indicator variable is ignored by drivers (WTF?)
      ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_CHAR,
      m_value.strbufA.ptr(), m_value.strbufA.size()*sizeof(SQLCHAR), &m_value.ind);
      break;
      case SQL_WCHAR:
      case SQL_WVARCHAR:
      m_value.strbufW.resize(size == 0 || size > DBWTL_ODBC_MAX_STRING_SIZE ? 256 : size+1); // size = chars
      m_value.strbufW.resize(4); // size = chars
      ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_WCHAR,
      m_value.strbufW.ptr(), m_value.strbufW.size()*sizeof(SQLWCHAR), &m_value.ind);
      break;
      case SQL_WLONGVARCHAR:
      // We bind a dummy buffer, just for getting the indicator.
      // The real fetch is done by SQLGetData()
      m_value.strbufW.resize(5);

      // This is very tricky. We must set a valid data pointer, but the size is 0.
      // If data pointer is NULL, the indicator variable is ignored by drivers (WTF?)
      ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_WCHAR,
      m_value.strbufW.ptr(), 0, &m_value.ind);


//        this->bindIndicator(m_colnum, &m_value.ind);

break;
case SQL_DECIMAL:
case SQL_NUMERIC:
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_NUMERIC,
&m_value.data.numeric, sizeof(m_value.data.numeric), &m_value.ind);
break;
case SQL_SMALLINT:
if(is_unsigned == SQL_TRUE)
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_USHORT,
&m_value.data.uShortInt, sizeof(m_value.data.uShortInt), &m_value.ind);
else
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_SSHORT,
&m_value.data.sShortInt, sizeof(m_value.data.sShortInt), &m_value.ind);

break;
case SQL_INTEGER:
if(is_unsigned == SQL_TRUE)
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_ULONG,
&m_value.data.uLongInt, sizeof(m_value.data.uLongInt), &m_value.ind);
else
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_SLONG,
&m_value.data.sLongInt, sizeof(m_value.data.sLongInt), &m_value.ind);

break;
case SQL_REAL:
case SQL_FLOAT:
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_FLOAT,
&m_value.data.float_, sizeof(m_value.data.float_), &m_value.ind);
break;
case SQL_DOUBLE:
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_DOUBLE,
&m_value.data.double_, sizeof(m_value.data.double_), &m_value.ind);
break;
case SQL_BIT:
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_BIT,
&m_value.data.bit, sizeof(m_value.data.bit), &m_value.ind);
break;
case SQL_TINYINT:
if(is_unsigned == SQL_TRUE)
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_UTINYINT,
&m_value.data.uTinyInt, sizeof(m_value.data.uTinyInt), &m_value.ind);
else
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_STINYINT,
&m_value.data.sTinyInt, sizeof(m_value.data.sTinyInt), &m_value.ind);
break;
case SQL_BIGINT:
if(is_unsigned == SQL_TRUE)
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_UBIGINT,
&m_value.data.uBigInt, sizeof(m_value.data.uBigInt), &m_value.ind);
else
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_SBIGINT,
&m_value.data.sBigInt, sizeof(m_value.data.sBigInt), &m_value.ind);

break;
case SQL_BINARY:
case SQL_VARBINARY:
assert(size > 0);
m_value.varbinary.resize(size);
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_BINARY,
m_value.varbinary.data(), m_value.varbinary.size(), &m_value.ind);
break;
case SQL_LONGVARBINARY:
// We bind a dummy buffer, just for getting the indicator.
// The real fetch is done by SQLGetData()
m_value.varbinary.resize(8);
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_BINARY,
m_value.varbinary.data(), 0, &m_value.ind);
break; /// we read longvarbinary data via SQLGetData()
case SQL_TYPE_DATE:
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_TYPE_DATE,
&m_value.data.date, sizeof(m_value.data.date), &m_value.ind);
break;
case SQL_TYPE_TIME:
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_TYPE_TIME,
&m_value.data.time, sizeof(m_value.data.time), &m_value.ind);
break;
case SQL_TYPE_TIMESTAMP:
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_TYPE_TIMESTAMP,
&m_value.data.timestamp, sizeof(m_value.data.timestamp), &m_value.ind);
break;
//case SQL_TYPE_UTCDATETIME:
//case SQL_TYPE_UTCTIME:
case SQL_INTERVAL_MONTH:
case SQL_INTERVAL_YEAR:
case SQL_INTERVAL_YEAR_TO_MONTH:
case SQL_INTERVAL_DAY:
case SQL_INTERVAL_HOUR:
case SQL_INTERVAL_MINUTE:
case SQL_INTERVAL_SECOND:
case SQL_INTERVAL_DAY_TO_HOUR:
case SQL_INTERVAL_DAY_TO_MINUTE:
case SQL_INTERVAL_DAY_TO_SECOND:
case SQL_INTERVAL_HOUR_TO_MINUTE:
case SQL_INTERVAL_HOUR_TO_SECOND:
case SQL_INTERVAL_MINUTE_TO_SECOND:
/// @todo Implement Interval types
throw FeatureUnsuppException("Interval types for ODBC are not implemented yet.");
//case SQL_GUID:
default:
/// @note Other types: fallback to string
if(this->getStmt().getDbc().usingUnicode())
{
m_value.strbufW.resize(size == 0 ? 256 : size+1); // size = chars
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_WCHAR,
m_value.strbufW.ptr(), m_value.strbufW.size()*sizeof(SQLWCHAR), &m_value.ind);
}
else
{
m_value.strbufA.resize(size == 0 ? 256 : size+1); // size = chars
ret = this->drv()->SQLBindCol(this->getHandle(), m_colnum, SQL_C_CHAR,
m_value.strbufA.ptr(), m_value.strbufA.size()*sizeof(SQLCHAR), &m_value.ind);
}
}


//m_buf = new int();
//m_bufsize = sizeof(int);
m_value.ind = SQL_NULL_DATA;


if(! SQL_SUCCEEDED(ret))
{
THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "BindCol failed");
}

this->m_is_bound = true;
return true;
	*/
}


//
daltype_t
OdbcData_libodbc::daltype(void) const
{
    return this->m_resultset.describeColumn(this->m_colnum).getDatatype();
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
OdbcResult_libodbc::OdbcResult_libodbc(OdbcStmt_libodbc& stmt)
    : OdbcResult(stmt.m_diag),
      m_stmt(stmt),
      //m_handle(NULL),
      m_current_tuple(DAL_TYPE_ROWID_NPOS),
      m_last_row_status(0),
      m_isopen(false),
      m_cached_resultcol_count(-1),
      m_param_data(),
      m_column_desc(),
      m_column_accessors(),
      m_allocated_accessors()
{ }



//
OdbcResult_libodbc::~OdbcResult_libodbc(void)
{
    this->m_column_accessors.clear();
    this->m_allocated_accessors.clear();
    this->close();
}


class Sqlstate2Exception
{
public:
	Sqlstate2Exception(int(*mapping_function)(int), const IDiagnosticRec &rec, IHandle &handle)
	{
		handle.writeDiagnostic(DiagnosticRec(DBWTL_CPI, DAL_STATE_DEBUG, SQLSTATE("00000"), 0, "not impl"));
	}
};


int odbcSqlstateMapping(int)
{
		/// @bug implement me
		return 0;
}

//
void
OdbcResult_libodbc::prepare(String sql)
{

    DBWTL_TRACE1(sql);

    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->m_stmt.getDbc().isConnected())
        throw EngineException("not connected");

    // if anything is currently open, we need to close.
    // This removes all binded vars, too.
    if(this->isPrepared())
        this->close();

    SQLRETURN ret;

    if(this->getDbc().usingUnicode())
    {
        OdbcStrW text(sql);
        ret = this->drv()->SQLPrepareW(this->getHandle(),  text.ptr(), text.size());
    }
    else
    {
        OdbcStrA text(sql, this->getDbc().getDbcEncoding());
        ret = this->drv()->SQLPrepareA(this->getHandle(),  text.ptr(), text.size());
    }

    if(! SQL_SUCCEEDED(ret))
    {
		std::list<OdbcDiagnosticRec_libodbc> list = this->m_stmt.getDiagRecs(DBWTL_CPI);

		std::for_each(list.begin(), list.end(), [this](OdbcDiagnosticRec_libodbc &i){ this->m_stmt.writeDiagnostic(i); });
		

		this->m_stmt.writeDiagnostic(DiagnosticRec(DBWTL_CPI, DAL_STATE_DEBUG, SQLSTATE("00000"), 0, "fooMessage"));

		DiagnosticRec myrec(DBWTL_CPI, DAL_STATE_ERROR, SQLSTATE("HY000"), 0, "bla");

		//throw map_sqlstate2exception();

		throw Sqlstate2Exception(odbcSqlstateMapping, myrec, this->m_stmt);


		throw EngineException(this->m_stmt, DiagnosticRec(DBWTL_CPI, DAL_STATE_ERROR, SQLSTATE("XXXXX"), 0, "newFoo"));
		

	// @bug Debug & Dev code, remove after implementation!
		/*
	this->m_stmt.writeDiagnostic(DiagnosticRec(DBWTL_CPI, DAL_STATE_DEBUG, SQLSTATE("HY000"), 34,
		"SQLPrepare() failed, invalid SQL",
		23412, 2,
		"The query failed."));
		*/

        THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt, this->getHandle(), SQL_HANDLE_STMT, "Prepare failed");
    }


    assert(SQL_SUCCEEDED(ret));

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_PREPARED);

    DALTRACE_LEAVE;
}


std::list<OdbcDiagnosticRec_libodbc>
readDiagnosticRecs(const CodePosInfo &cpi, bool use_unicode, const std::string &charset, ODBC30Drv* drv, SQLHANDLE handle, SQLSMALLINT htype)
{
	std::list<OdbcDiagnosticRec_libodbc> lst;
    SQLINTEGER i = 0;
    SQLINTEGER native = 0;
    SQLSMALLINT len = 0;
    SQLRETURN ret;
	
    if(use_unicode)
    {
        OdbcStrW state(6);
        OdbcStrW text(512+1);
        do
        {
            ret = drv->SQLGetDiagRecW(htype, handle, ++i, state.ptr(), &native, text.ptr(), text.size(), &len);
            if(SQL_SUCCEEDED(ret))
            {
				/// @bug check error class instead of hardcoding DAL_STATE_ERROR
				lst.push_back(OdbcDiagnosticRec_libodbc(cpi, DAL_STATE_ERROR, state.str(5).utf8(), native, text.str(len)));
            }
            else if(ret != SQL_NO_DATA)
            {
                throw EngineException("SQLGetDiagRec() returns an unexpected error");
            }
        }
        while(SQL_SUCCEEDED(ret));

    }
    else
    {
        OdbcStrA state(6);
        OdbcStrA text(512+1);
        do
        {
            ret = drv->SQLGetDiagRecA(htype, handle, ++i, state.ptr(), &native, text.ptr(), text.size(), &len);
            if(SQL_SUCCEEDED(ret))
            {
				lst.push_back(OdbcDiagnosticRec_libodbc(cpi, DAL_STATE_ERROR, state.str(5, "ASCII").utf8(), native, text.str(len, charset)));
            }
            else if(ret != SQL_NO_DATA)
            {
                throw EngineException("SQLGetDiagRec() returns an unexpected error");
            }
        }
        while(SQL_SUCCEEDED(ret));
    }
	return lst;
}



std::list<OdbcDiagnosticRec_libodbc>
OdbcStmt_libodbc::getDiagRecs(const CodePosInfo & cpi) const
{
	return readDiagnosticRecs(cpi, this->getDbc().usingUnicode(), this->getDbc().getDbcEncoding(), this->drv(), 
		this->getHandle(), SQL_HANDLE_STMT);
}

std::list<OdbcDiagnosticRec_libodbc>
OdbcDbc_libodbc::getDiagRecs(const CodePosInfo & cpi) const
{
	return std::list<OdbcDiagnosticRec_libodbc>();
}


std::list<OdbcDiagnosticRec_libodbc>
OdbcEnv_libodbc::getDiagRecs(const CodePosInfo & cpi) const
{
	return std::list<OdbcDiagnosticRec_libodbc>();
}


OdbcDiagnosticRec_libodbc::OdbcDiagnosticRec_libodbc(const CodePosInfo & cpi,
													 dalstate_t dalstate,
		SQLSTATE sqlstate,
		const Variant &nativeCode,
		const String &msg,
		rownum_t rowNum,
		colnum_t colNum,
		const String &desc,
		const Variant &data)
	: OdbcDiagnosticRec(cpi, dalstate, sqlstate, nativeCode, msg, rowNum, colNum, desc, data)
{
    
}


OdbcDiagnosticRec_libodbc*
OdbcDiagnosticRec_libodbc::clone(void) const
{
	DBWTL_NOTIMPL();
}


static void tdate2odbc(const TDate &tdate, SQL_DATE_STRUCT &odbc)
{
    odbc.year = tdate.year();
    odbc.month = tdate.month();
    odbc.day = tdate.day();
}


static void ttime2odbc(const TTime &ttime, SQL_TIME_STRUCT &odbc)
{
    odbc.hour = ttime.hour();
    odbc.minute = ttime.minute();
    odbc.second = ttime.second();
}

static void ttimestamp2odbc(const TTimestamp &ttimestamp, SQL_TIMESTAMP_STRUCT &odbc)
{
    odbc.year = ttimestamp.year();
    odbc.month = ttimestamp.month();
    odbc.day = ttimestamp.day();
    odbc.hour = ttimestamp.hour();
    odbc.minute = ttimestamp.minute();
    odbc.second = ttimestamp.second();
    odbc.fraction = ttimestamp.fraction();
}



void
OdbcResult_libodbc::bindParamBlob(StmtBase::ParamMapIterator param)
{
    std::shared_ptr<OdbcValue> pdata = this->m_param_data[param->first];
    assert(pdata.get());

    SQLRETURN ret;

    bool needLength = sqlgetinfo<String>(this->getDbc(), SQL_NEED_LONG_DATA_LEN) == "Y";

    //std::cout << sqlgetinfo<String>(this->getDbc(), SQL_NEED_LONG_DATA_LEN) << std::endl;

    if(needLength)
    {
        pdata->varbinary = Blob(param->second->get<BlobStream>().rdbuf()).toVarbinary();
        assert(pdata->varbinary.size());
		// SQL_LEN_DATA_AT_EXEC requires a signed sized type
        pdata->ind = SQL_LEN_DATA_AT_EXEC(static_cast<SQLINTEGER>(pdata->varbinary.size()));
    }
    else // no length required
    {
        pdata->ind = SQL_LEN_DATA_AT_EXEC(1); // any non-negative integer is ok
    }

    ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                        SQL_C_BINARY,
                                        SQL_LONGVARBINARY,
                                        0,
                                        0,
                                        reinterpret_cast<void*>(param->first),
                                        0, // len 0
                                        &pdata->ind);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                              this->getHandle(), SQL_HANDLE_STMT,
                              "SQLBindParameter() failed");
    }
}



void
OdbcResult_libodbc::bindParamMemo(StmtBase::ParamMapIterator param)
{
    std::shared_ptr<OdbcValue> pdata = this->m_param_data[param->first];
    assert(pdata.get());

    SQLRETURN ret;

    bool needLength = sqlgetinfo<String>(this->getDbc(), SQL_NEED_LONG_DATA_LEN) == "Y";


    if(this->getDbc().usingUnicode())
    {

        //std::cout << sqlgetinfo<String>(this->getDbc(), SQL_NEED_LONG_DATA_LEN) << std::endl;

        if(needLength)
        {
            pdata->strbufW = OdbcStrW(Memo(param->second->get<MemoStream>().rdbuf()).str());
			// SQL_LEN_DATA_AT_EXEC requires a signed sized type
            pdata->ind = SQL_LEN_DATA_AT_EXEC(static_cast<SQLINTEGER>(pdata->strbufW.size()*sizeof(SQLWCHAR)));
        }
        else // no length required
        {
            pdata->ind = SQL_LEN_DATA_AT_EXEC(1); // any non-negative integer is ok
        }

        ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                            SQL_C_WCHAR,
                                            SQL_WLONGVARCHAR,
                                            0,
                                            0,
                                            reinterpret_cast<void*>(param->first),
                                            0, // len 0
                                            &pdata->ind);
    }
    else
    {
        // we ignore needLength because we need to transform the whole data in-memory.
        pdata->strbufA = OdbcStrA(Memo(param->second->get<MemoStream>().rdbuf()).str(), this->getDbc().getDbcEncoding());
		// SQL_LEN_DATA_AT_EXEC requires a signed sized type
        pdata->ind = SQL_LEN_DATA_AT_EXEC(static_cast<SQLINTEGER>(pdata->strbufA.size()*sizeof(SQLCHAR)));
        ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                            SQL_C_CHAR,
                                            SQL_LONGVARCHAR,
                                            0,
                                            0,
                                            reinterpret_cast<void*>(param->first),
                                            0,
                                            &pdata->ind);
    }
    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                              this->getHandle(), SQL_HANDLE_STMT,
                              "SQLBindParameter() failed");
    }
}





void
OdbcResult_libodbc::bindParamString(StmtBase::ParamMapIterator param)
{
    std::shared_ptr<OdbcValue> pdata = this->m_param_data[param->first];
    assert(pdata.get());
    Variant *var = param->second;
    SQLRETURN ret;
    if(this->getDbc().usingUnicode())
    {
        pdata->strbufW = OdbcStrW(var->get<String>());
        pdata->ind = SQL_NTS;
        ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                            SQL_C_WCHAR,
                                            SQL_WVARCHAR,
                                            pdata->strbufW.size(),
                                            0,
                                            pdata->strbufW.ptr(),
                                            pdata->strbufW.size()*sizeof(SQLWCHAR),
                                            &pdata->ind);
    }
    else
    {
        pdata->strbufA = OdbcStrA(var->get<String>(), this->getDbc().getDbcEncoding());
        pdata->ind = SQL_NTS;
        ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                            SQL_C_CHAR,
                                            SQL_VARCHAR,
                                            pdata->strbufA.size(),
                                            0,
                                            pdata->strbufA.ptr(),
                                            pdata->strbufA.size(),
                                            &pdata->ind);
    }
    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                              this->getHandle(), SQL_HANDLE_STMT,
                              "SQLBindParameter() failed");
    }
}

void
OdbcResult_libodbc::bindParamNumeric(StmtBase::ParamMapIterator param)
{
    std::shared_ptr<OdbcValue> pdata = this->m_param_data[param->first];
    assert(pdata.get());
    Variant *var = param->second;

    SQLRETURN ret;

    SQLHDESC desc = SQL_NULL_HANDLE;

    // ANSI API should be save, no character strings
    ret = this->drv()->SQLGetStmtAttrA(this->getHandle(), SQL_ATTR_APP_PARAM_DESC,
                                       &desc, sizeof(desc), NULL);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(),
                              SQL_HANDLE_STMT, "SQLGetStmtAttrW failed");
    }


    SQL_NUMERIC_STRUCT &num = pdata->data.numeric;
    TNumeric src = var->get<TNumeric>();

    //src = TNumeric("123.456");

    SQLSMALLINT scale = src.scale();
    SQLSMALLINT prec = src.precision();

    num.sign = src.sign() ? 1 : 2;   /* 1 if positive, 2 if negative */

    src.zeroscale();
    src = std::abs(src);


    ::memset (num.val, 0, 16);

    TNumeric q(1);

    int i;
    for(i = 0; i < SQL_MAX_NUMERIC_LEN; ++i)
    {
        //  std::cout << i << std::endl;
        TNumeric r = q;
        q = q * 256;
//  std::cout << x << " " << q << " " << r << std::endl;
//  std::cout << "DAT: " << (x % q) << std::endl;
//  std::cout << "DAT: " << divi(x % q, r) << std::endl;
        num.val[i] = divi(src % q, r).convert<unsigned short>();
//  std::cout << "VAL: " << int(num.val[i]) << std::endl;
        src = src - (src % q);

        if(src == 0) break;
    }

//   x / 256;

//   num.val [0] = 0x0;
//   num.val [1] = 0x2;

    // Some documentation states that this information fields are ignored.
    // Lots of tests shows that they are required...
    num.precision = prec;
    num.scale = scale;

/*
  for(size_t i = 0; i < SQL_MAX_NUMERIC_LEN; ++i)
  {
  res = res + num.val[i] * denom;
  denom = denom * 256;
  }
*/


    //pdata->data.sLongInt = 45;
    //pdata->strbufW = OdbcStrW(src.str());
//    std::cout << "DATA: " << src.str() << std::endl;
    //   std::cout << "PREC: " << src.precision() << std::endl;
    //  std::cout << "PREC: " << src.scale() << std::endl;
    //pdata->ind = pdata->strbufW.size()*sizeof(SQLWCHAR);
    pdata->ind = sizeof(num);

    ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                        SQL_C_NUMERIC,
                                        SQL_NUMERIC,
                                        num.precision,
                                        num.scale,
                                        &num,
                                        0,
                                        &pdata->ind);
/*

  ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
  SQL_C_WCHAR,
  SQL_NUMERIC,
  src.precision(),
  src.scale(),
  pdata->strbufW.ptr(),
  pdata->strbufW.size()*sizeof(SQLWCHAR),
  &pdata->ind);
*/
/*
  ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
  SQL_C_NUMERIC,
  SQL_NUMERIC,
  8,
  num.scale,
  &num,
  sizeof(num),
  &pdata->ind);
*/
    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                              this->getHandle(), SQL_HANDLE_STMT,
                              "SQLBindParameter() failed");
    }


    ret = this->drv()->SQLSetDescFieldA(desc, param->first, SQL_DESC_PRECISION,
                                        reinterpret_cast<void*>(num.precision), SQL_IS_SMALLINT);


    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(),
                              SQL_HANDLE_STMT, "SQLSetDescFieldW failed");
    }


    ret = this->drv()->SQLSetDescFieldA(desc, param->first, SQL_DESC_SCALE,
                                        reinterpret_cast<void*>(num.scale), SQL_IS_SMALLINT);


    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(),
                              SQL_HANDLE_STMT, "SQLSetDescFieldW failed");
    }



    // after calling SQLSetDescField(), the params DATA_PTR is set to NULL by
    // the driver. We need to re-set the pointer.
    ret = this->drv()->SQLSetDescFieldA(desc, param->first, SQL_DESC_DATA_PTR,
                                        &num, SQL_IS_POINTER);
    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getStmt().getDbc(), this->getStmt(), this->getHandle(),
                              SQL_HANDLE_STMT, "SQLSetDescFieldW failed");
    }



}


//
void
OdbcResult_libodbc::execute(StmtBase::ParamMap& params)
{
    DALTRACE_ENTER;
    std::map<int, std::string> tmp_strings;

    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isPrepared())
        throw EngineException("Resultset is not prepared.");


/// If the resultset is already open, we cleanup all bindings
/// because the current bindings are maintained by this class instead
/// of SQLite.
    if(this->m_cursorstate & DAL_CURSOR_OPEN)
    {
        this->reset();
        this->m_param_data.clear();

        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_PREPARED);
    }


    SQLRETURN ret;


    StmtBase::ParamMapIterator param;
    for(param = params.begin(); param != params.end(); ++param)
    {
        Variant *var = param->second;
        std::stringstream tmp_stream;


        std::shared_ptr<OdbcValue> pdata(new OdbcValue());
        this->m_param_data[param->first] = pdata;

        pdata->ind = SQL_NULL_DATA;


        if(var->isnull())
        {
            /// @todo Setting VARCHAR is just a workaround.
            SQLSMALLINT dtype = SQL_VARCHAR;
			SQLULEN plen = 1;
			SQLSMALLINT decdigits = 1;

            if(sqlgetfunction(this->getDbc(), SQL_API_SQLDESCRIBEPARAM))
            {
                SQLRETURN ret = this->drv()->SQLDescribeParam(this->getHandle(), param->first,
                                                              &dtype, &plen, &decdigits, NULL);
				if(ret == SQL_SUCCESS) {}
				// Invalid Descriptor Index
				else if(ret == SQL_SUCCESS_WITH_INFO && diag_sqlstate(DBWTL_CPI, this->getDbc(), this->getHandle(), SQL_HANDLE_STMT) == "07009")
				{
					// ignore
				}
				else
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLDescribeParam() for NULL value failed");
            }

            ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                SQL_C_DEFAULT,
                                                dtype,
                                                plen,
                                                decdigits,
                                                NULL,
                                                0,
                                                &pdata->ind);

            if(! SQL_SUCCEEDED(ret))
            {
                THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                      this->getHandle(), SQL_HANDLE_STMT,
                                      "SQLBindParameter() for NULL value failed");
            }

        }
        else
        {
            switch(var->datatype())
            {
            case DAL_TYPE_INT:
                pdata->data.sLongInt = var->get<signed int>();
                //pdata->data.sLongInt = 66;
                //pdata->ind = sizeof(pdata->data.sLongInt);
                //std::cout << "DATA: " << var->get<signed int>() << std::endl;
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_SLONG,
                                                    SQL_INTEGER,
                                                    10,
                                                    0,
                                                    &pdata->data.sLongInt,
                                                    sizeof(pdata->data.sLongInt),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;

            case DAL_TYPE_UINT:
                pdata->data.uLongInt = var->get<unsigned int>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_ULONG,
                                                    SQL_INTEGER,
                                                    10,
                                                    0,
                                                    &pdata->data.uLongInt,
                                                    sizeof(pdata->data.uLongInt),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_CHAR:
                pdata->data.sTinyInt = var->get<signed char>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_STINYINT,
                                                    SQL_TINYINT,
                                                    3,
                                                    0,
                                                    &pdata->data.sTinyInt,
                                                    sizeof(pdata->data.sTinyInt),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_UCHAR:
                pdata->data.uTinyInt = var->get<unsigned char>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_UTINYINT,
                                                    SQL_TINYINT,
                                                    3,
                                                    0,
                                                    &pdata->data.uTinyInt,
                                                    sizeof(pdata->data.uTinyInt),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_BOOL:
                pdata->data.bit = var->get<bool>() ? 1 : 0;
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_BIT,
                                                    SQL_BIT,
                                                    1,
                                                    0,
                                                    &pdata->data.bit,
                                                    sizeof(pdata->data.bit),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_SMALLINT:
                pdata->data.sShortInt = var->get<signed short>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_SSHORT,
                                                    SQL_SMALLINT,
                                                    5,
                                                    0,
                                                    &pdata->data.sShortInt,
                                                    sizeof(pdata->data.sShortInt),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_USMALLINT:
                pdata->data.uShortInt = var->get<unsigned short>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_USHORT,
                                                    SQL_SMALLINT,
                                                    5,
                                                    0,
                                                    &pdata->data.uShortInt,
                                                    sizeof(pdata->data.uShortInt),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_BIGINT:
                pdata->data.sBigInt = var->get<signed long long>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_SBIGINT,
                                                    SQL_BIGINT,
                                                    19,
                                                    0,
                                                    &pdata->data.sBigInt,
                                                    sizeof(pdata->data.sBigInt),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_UBIGINT:
                pdata->data.uBigInt = var->get<unsigned long long>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_UBIGINT,
                                                    SQL_BIGINT,
                                                    20,
                                                    0,
                                                    &pdata->data.uBigInt,
                                                    sizeof(pdata->data.uBigInt),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_FLOAT:
                pdata->data.float_ = var->get<float>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_FLOAT,
                                                    SQL_FLOAT,
                                                    15,
                                                    0,
                                                    &pdata->data.float_,
                                                    sizeof(pdata->data.float_),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_DOUBLE:
                pdata->data.double_ = var->get<double>();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_DOUBLE,
                                                    SQL_DOUBLE,
                                                    15,
                                                    0,
                                                    &pdata->data.double_,
                                                    sizeof(pdata->data.double_),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;
            case DAL_TYPE_DATE:
                tdate2odbc(var->get<TDate>(), pdata->data.date);
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_DATE,
													SQL_TYPE_DATE,
                                                    10,
                                                    0,
                                                    &pdata->data.date,
                                                    sizeof(pdata->data.date),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;

            case DAL_TYPE_TIME:
                ttime2odbc(var->get<TTime>(), pdata->data.time);
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_TIME,
                                                    SQL_TYPE_TIME,
                                                    8,
                                                    0,
                                                    &pdata->data.time,
                                                    sizeof(pdata->data.time),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;

            case DAL_TYPE_TIMESTAMP:
                ttimestamp2odbc(var->get<TTimestamp>(), pdata->data.timestamp);
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_TIMESTAMP,
                                                    SQL_TYPE_TIMESTAMP,
                                                    19,
                                                    0,
                                                    &pdata->data.timestamp,
                                                    sizeof(pdata->data.timestamp),
                                                    NULL);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;

            case DAL_TYPE_VARBINARY:
                //pdata->varbinary.resize(var->get<TVarbinary>().size());
                pdata->varbinary = var->get<TVarbinary>();
                //var->get<TVarbinary>().write(pdata->varbinary.data(), pdata->varbinary.size());
                //pdata->ind = pdata->varbinary.size();
                pdata->ind = pdata->varbinary.size();
                ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                                                    SQL_C_BINARY,
                                                    SQL_VARBINARY,
                                                    pdata->ind,
                                                    0,
                                                    pdata->varbinary.data(),
                                                    pdata->varbinary.size(),
                                                    &pdata->ind);

                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLBindParameter() failed");
                }
                break;


            case DAL_TYPE_BLOB:
                this->bindParamBlob(param);
                break;

            case DAL_TYPE_MEMO:
                this->bindParamMemo(param);
                break;

            case DAL_TYPE_NUMERIC:
                this->bindParamNumeric(param);
                break;


            default:
                this->bindParamString(param);
                break;
                /*
                  pdata->strbufW = var->get<String>();
                  pdata->ind = SQL_NTS;
                  ret = this->drv()->SQLBindParameter(this->getHandle(), param->first, SQL_PARAM_INPUT,
                  SQL_C_WCHAR,
                  SQL_WVARCHAR,
                  pdata->strbufW.size(),
                  0,
                  pdata->strbufW.ptr(),
                  pdata->strbufW.size(),
                  &pdata->ind);

                  if(! SQL_SUCCEEDED(ret))
                  {
                  THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                  this->getHandle(), SQL_HANDLE_STMT,
                  "SQLBindParameter() failed");
                  }
                  break;
                */
            }
        }
    }


    ret = this->drv()->SQLExecute(this->getHandle());

    // If params exists with SQL_DATA_AT_EXEC, we must supply this data now.
    if(ret == SQL_NEED_DATA)
    {
        SQLRETURN ret;
        SQLINTEGER param_num = 0;
        while((ret = this->drv()->SQLParamData(this->getHandle(), (SQLPOINTER*)&param_num))
              == SQL_NEED_DATA)
        {
            param = params.find(param_num);
            if(param == params.end() || param->second->isnull())
            {
                SQLRETURN ret = this->drv()->SQLPutData(this->getHandle(), NULL, SQL_NULL_DATA);
                if(! SQL_SUCCEEDED(ret))
                {
                    THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                          this->getHandle(), SQL_HANDLE_STMT,
                                          "SQLPutData failed");
                }
            }
            else
            {
                Variant &data =  *param->second;

                if(data.datatype() == DAL_TYPE_BLOB)
                {
                    Blob tmp_blob;
                    ByteStreamBuf *buf = 0;
                    if(sqlgetinfo<String>(this->getDbc(), SQL_NEED_LONG_DATA_LEN) == "Y")
                    {
                        tmp_blob = m_param_data[param->first]->varbinary.toBlob();
                        //std::cout << "final size: " << tmp_blob.toVarbinary().size() << std::endl;
                        buf = tmp_blob.rdbuf();
                    }
                    else if(data.can_convert<BlobStream>())
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
                        DBWTL_BUG();
                    }

                    std::vector<char> vec(DBWTL_ODBC_LOB_BUFSIZE);

                    while(std::streamsize i = buf->sgetn(vec.data(), vec.size()))
                    {
                        SQLRETURN ret = this->drv()->SQLPutData(this->getHandle(), vec.data(), i);
                        if(! SQL_SUCCEEDED(ret))
                        {
                            THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                                  this->getHandle(), SQL_HANDLE_STMT,
                                                  "SQLPutData failed");
                        }
                    }
                }
                else if(data.datatype() == DAL_TYPE_MEMO)
                {
                    Memo tmp_memo;
                    UnicodeStreamBuf *buf = 0;
                    Variant &data =  *param->second;

                    if(this->getDbc().usingUnicode() && sqlgetinfo<String>(this->getDbc(), SQL_NEED_LONG_DATA_LEN) == "Y")
                    {
                        // strbufW/A is filled with the whole MEMO data and reduced to the
                        // correct size. We can put all data at once.
                        SQLRETURN ret;
                        OdbcStrW &str = m_param_data[param->first]->strbufW;
                        ret = this->drv()->SQLPutData(this->getHandle(),
                                                      str.ptr(), str.size()*sizeof(SQLWCHAR));
                        if(! SQL_SUCCEEDED(ret))
                            THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                                  this->getHandle(), SQL_HANDLE_STMT,
                                                  "SQLPutData failed");
                    }
                    else if(!this->getDbc().usingUnicode())
                    {
                        // strbufW/A is filled with the whole MEMO data and reduced to the
                        // correct size. We can put all data at once.
                        SQLRETURN ret;
                        OdbcStrA &str = m_param_data[param->first]->strbufA;
                        ret = this->drv()->SQLPutData(this->getHandle(),
                                                      str.ptr(), str.size()*sizeof(SQLCHAR));
                        if(! SQL_SUCCEEDED(ret))
                            THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                                  this->getHandle(), SQL_HANDLE_STMT,
                                                  "SQLPutData failed");
                    }
                    else // no pre-filled buffers, we read directly from the source
                    {
                        Memo tmp_memo;
                        UnicodeStreamBuf *buf = 0;
                        Variant &data = *param->second;

                        if(data.can_convert<MemoStream>())
                        {
                            buf = data.get<MemoStream>().rdbuf();
                        }
                        else if(data.can_convert<Memo>())
                        {
                            tmp_memo = data.get<Memo>();
                            buf = tmp_memo.rdbuf();
                        }

                        // copy buffer
                        std::vector<wchar_t> vec(DBWTL_ODBC_LOB_BUFSIZE);

                        while(std::streamsize i = buf->sgetn(vec.data(), vec.size()))
                        {
                            SQLRETURN ret;
                            if(sizeof(wchar_t) == sizeof(SQLWCHAR))
                            {
                                // charsize is compatible, we can put data directly
                                ret = this->drv()->SQLPutData(this->getHandle(), vec.data(), i*sizeof(wchar_t));
                            }
                            else
                            {
                                // we must convert chars to DM char size
                                std::vector<SQLWCHAR> vec2(vec.size());
                                std::copy(vec.begin(), vec.begin()+i, vec2.begin());
                                ret = this->drv()->SQLPutData(this->getHandle(), vec2.data(), i*sizeof(SQLWCHAR));
                            }
                            if(! SQL_SUCCEEDED(ret))
                            {
                                THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                                      this->getHandle(), SQL_HANDLE_STMT,
                                                      "SQLPutData failed");
                            }
                        }
                    }
                } // endif memo
            }
        }

        if(! SQL_SUCCEEDED(ret))
        {
            THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                                  this->getHandle(), SQL_HANDLE_STMT,
                                  "SQLParamData failed");
        }
    }
	else if(ret == SQL_NO_DATA_FOUND)
	{
		DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);
		this->refreshMetadata();
		this->m_current_tuple = 0;
		DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);
	}
    else if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->m_stmt.getDbc(), this->m_stmt,
                              this->getHandle(), SQL_HANDLE_STMT,
                              "Execute failed");
    }
	else
	{
		DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);
		this->refreshMetadata();
		this->m_current_tuple = 0;
	}
    //this->next();

    DALTRACE_LEAVE;
}



//
size_t
OdbcResult_libodbc::paramCount(void) const
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isPrepared())
        throw EngineException("Resultset is not prepared.");

    SQLSMALLINT count;

    SQLRETURN ret = this->drv()->SQLNumParams(this->getHandle(), &count);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getDbc(), this->getStmt(),
                              this->getHandle(), SQL_HANDLE_STMT,
                              "Execute failed");
    }
    return count;
}



//
void
OdbcResult_libodbc::first(void)
{

    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    if(this->m_current_tuple > 1)
    {
        throw EngineException("can't scroll to first record");
    }

    if(this->m_current_tuple == 1)
        return; // nothing to do...


    SQLRETURN ret = this->drv()->SQLFetch(this->getHandle());
    assert(ret != SQL_INVALID_HANDLE);

    if(SQL_SUCCEEDED(ret))
    {
        this->m_current_tuple = 1;
        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_POSITIONED);
        std::for_each(this->m_column_accessors.begin(),
                      this->m_column_accessors.end(),
                      [](VariantListT::value_type &item)
                      {
                          item.second->refresh();
                      });
    }
    else if(ret == SQL_NO_DATA)
    {
        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);
    }
    else
    {
        THROW_ODBC_DIAG_ERROR(this->getDbc(), this->getStmt(),
                              this->getHandle(), SQL_HANDLE_STMT, "Fetch first failed");
    }

    // Fetch parts
    //if(ret == SQL_SUCCESS_WITH_INFO)
    //{
    //    if(diag_sqlstate(DBWTL_CPI, this->getDbc(), this->getHandle(), SQL_HANDLE_STMT) == "01004")
    //    {
    //        // std::for_each(this->m_column_accessors.begin(),
    //        //               this->m_column_accessors.end(),
    //        //               stdext::compose1(std::mem_fun(&OdbcVariant::fetchParts),
    //        //                                stdext::select2nd<VariantListT::value_type>())
    //        //  );

    //        std::for_each(this->m_column_accessors.begin(),
    //                      this->m_column_accessors.end(),
    //                      [](VariantListT::value_type &item)
    //                      {
    //                          item.second->fetchParts();
    //                      });
    //    }
    //}

}



//
bool
OdbcResult_libodbc::next(void)
{

    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    SQLRETURN ret = this->drv()->SQLFetch(this->getHandle());
    assert(ret != SQL_INVALID_HANDLE);

    if(SQL_SUCCEEDED(ret))
    {
        ++this->m_current_tuple;
        std::for_each(this->m_column_accessors.begin(),
                      this->m_column_accessors.end(),
                      [](VariantListT::value_type &item)
                      {
                          item.second->refresh();
                      });
    }
    else if(ret == SQL_NO_DATA)
    {
        DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_EOF);
        return false;
    }
    else
    {
        THROW_ODBC_DIAG_ERROR(this->getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "Fetch failed");
    }

    // Fetch parts
    //if(ret == SQL_SUCCESS_WITH_INFO)
    //{
    //    if(diag_sqlstate(DBWTL_CPI, this->getDbc(), this->getHandle(), SQL_HANDLE_STMT) == "01004")
    //    {
    //        // std::for_each(this->m_column_accessors.begin(),
    //        //               this->m_column_accessors.end(),
    //        //               stdext::compose1(std::mem_fun(&OdbcVariant::fetchParts),
    //        //                                stdext::select2nd<VariantListT::value_type>())
    //        //  );

    //        std::for_each(this->m_column_accessors.begin(),
    //                      this->m_column_accessors.end(),
    //                      [](VariantListT::value_type &item)
    //                      {
    //                          item.second->fetchParts();
    //                      });
    //    }
    //}

    return !this->eof();
}



//
bool
OdbcResult_libodbc::eof(void) const
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    if(! (this->m_cursorstate & DAL_CURSOR_POSITIONED))
    {
        throw EngineException("Dataset is not positioned.");
    }

    return this->m_cursorstate & DAL_CURSOR_EOF;

/*
  if(this->isBad())
  throw EngineException("Resultset is in bad state.");

  if(! this->isOpen())
  throw EngineException("Resultset is not open.");

  return this->m_last_row_status != ODBC_ROW;
*/
}



//
void
OdbcResult_libodbc::close(void)
{
    this->reset(); /// @todo this may conflict with multiple resultsets.
}



//
void
OdbcResult_libodbc::reset(void)
{
    SQLRETURN ret;

    ret = this->drv()->SQLFreeStmt(this->getHandle(), SQL_CLOSE);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getDbc(), *this,
                              this->getHandle(), SQL_HANDLE_STMT,
                              "SQLFreeStmt() failed");
    }


    //this->m_column_desc.clear();

    ret = this->drv()->SQLFreeStmt(this->getHandle(), SQL_UNBIND);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getDbc(), *this,
                              this->getHandle(), SQL_HANDLE_STMT,
                              "SQLFreeStmt() failed");
    }
    this->m_column_accessors.clear();
    this->m_allocated_accessors.clear();


    ret = this->drv()->SQLFreeStmt(this->getHandle(), SQL_RESET_PARAMS);


    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getDbc(), *this,
                              this->getHandle(), SQL_HANDLE_STMT,
                              "SQLFreeStmt() failed");
    }
    this->m_param_data.clear();

    this->m_cached_resultcol_count = -1;
}




//
rowcount_t
OdbcResult_libodbc::rowCount(void) const
{
    // ODBC can't tell you the row count
    return -1;
}



rowcount_t
OdbcResult_libodbc::affectedRows(void) const
{

    SQLLEN c = 0;

    SQLRETURN ret = this->drv()->SQLRowCount(this->getHandle(), &c);

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT,
                              "SQLRowCount() failed");
    }
    return c;
}


///
/// @note ODBC has no method for getting the last insert row ID
Variant
OdbcResult_libodbc::lastInsertRowId(void)
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isPrepared())
        throw EngineException("Resultset is not prepared.");

    throw FeatureUnsuppException("LastInsertRowID not supported by ODBC API");
}




//
const OdbcVariant&
OdbcResult_libodbc::column(String name)
{
    colnum_t num = this->columnID(name);
    return this->column(num);
}



///
/// @note For ODBC, we must preallocate each column beacuse we need
/// to call SQLBindCol() for each output column. The Output buffer
/// is stored in the Data object for each column.
const OdbcVariant&
OdbcResult_libodbc::column(colnum_t num)
{
    DBWTL_TRACE1(num);

    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    if(num > this->columnCount())
    {
        throw NotFoundException(FORMAT2("Column %d not found, column count is %d", num, this->columnCount()));
    }

    VariantListT::iterator p = this->m_column_accessors.find(num);
    if(this->m_column_accessors.end() == p)
    {
        DBWTL_BUG_EX(FORMAT2("Column %d not found, column count is %d", num, this->columnCount()));
    }
    return *(p->second);
}



//
rowid_t
OdbcResult_libodbc::getCurrentRowID(void) const
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    return this->m_current_tuple;
}



//
size_t
OdbcResult_libodbc::columnCount(void) const
{

    DBWTL_TRACE0();

    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    SQLSMALLINT c = 0;

    if(this->m_cached_resultcol_count == -1)
    {
        SQLRETURN ret = this->drv()->SQLNumResultCols(this->getHandle(), &c);

        if(! SQL_SUCCEEDED(ret))
        {
            THROW_ODBC_DIAG_ERROR(this->getDbc(), this->getStmt(), this->getHandle(), SQL_HANDLE_STMT, "NumResultCols");
        }
		this->m_cached_resultcol_count = c;
    }

    DAL_DEBUG("Query column count: " << this->m_cached_resultcol_count);
    DALTRACE_LEAVE;
    return this->m_cached_resultcol_count;
}



//
colnum_t
OdbcResult_libodbc::columnID(String name) const
{
    DBWTL_TRACE1(name);
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    for(colnum_t i = 1; i <= this->columnCount(); ++i)
    {
        if(name == this->columnName(i))
            return i;
    }
    throw NotFoundException(US("Column '") + String::Internal(name) + US("' not found."));
}



//
String
OdbcResult_libodbc::columnName(colnum_t num) const
{
    DBWTL_TRACE1(num);

    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");

    return this->describeColumn(num).getName().asStr();
}



//
ODBC30Drv*
OdbcResult_libodbc::drv(void) const
{
    return this->getDbc().drv();
}



//
OdbcDbc_libodbc&
OdbcResult_libodbc::getDbc(void) const
{
    return this->m_stmt.getDbc();
}



//
OdbcStmt_libodbc&
OdbcResult_libodbc::getStmt(void) const
{
    return this->m_stmt;
}


static inline daltype_t sqltype2daltype(SQLSMALLINT sqltype, bool is_unsigned)
{
	switch(sqltype)
    {
    case SQL_CHAR:
    case SQL_VARCHAR:
    case SQL_WCHAR:
    case SQL_WVARCHAR:
        return DAL_TYPE_STRING;

    case SQL_LONGVARCHAR:
    case SQL_WLONGVARCHAR:
        return DAL_TYPE_MEMO;

    case SQL_DECIMAL:
    case SQL_NUMERIC:
        return DAL_TYPE_NUMERIC;
            
    case SQL_SMALLINT:
        return is_unsigned ? DAL_TYPE_USMALLINT : DAL_TYPE_SMALLINT;

    case SQL_INTEGER:
        return is_unsigned ? DAL_TYPE_UINT : DAL_TYPE_INT;

    case SQL_REAL:
    case SQL_FLOAT:
        return DAL_TYPE_FLOAT;
            
    case SQL_DOUBLE:
        return DAL_TYPE_DOUBLE;
            
    case SQL_BIT:
        return DAL_TYPE_BOOL;
            
    case SQL_TINYINT:
        return is_unsigned  ? DAL_TYPE_UCHAR : DAL_TYPE_CHAR;
            
    case SQL_BIGINT:
        return is_unsigned ? DAL_TYPE_UBIGINT : DAL_TYPE_BIGINT;
            
    case SQL_BINARY:
    case SQL_VARBINARY:
        return DAL_TYPE_VARBINARY;
            
    case SQL_LONGVARBINARY:
        return DAL_TYPE_BLOB;

    case SQL_TYPE_DATE:
        return DAL_TYPE_DATE;
            
    case SQL_TYPE_TIME:
        return DAL_TYPE_TIME;
            
    case SQL_TYPE_TIMESTAMP:
        return DAL_TYPE_TIMESTAMP;
            
//case SQL_TYPE_UTCDATETIME:
//case SQL_TYPE_UTCTIME:
    case SQL_INTERVAL_MONTH:
    case SQL_INTERVAL_YEAR:
    case SQL_INTERVAL_YEAR_TO_MONTH:
    case SQL_INTERVAL_DAY:
    case SQL_INTERVAL_HOUR:
    case SQL_INTERVAL_MINUTE:
    case SQL_INTERVAL_SECOND:
    case SQL_INTERVAL_DAY_TO_HOUR:
    case SQL_INTERVAL_DAY_TO_MINUTE:
    case SQL_INTERVAL_DAY_TO_SECOND:
    case SQL_INTERVAL_HOUR_TO_MINUTE:
    case SQL_INTERVAL_HOUR_TO_SECOND:
    case SQL_INTERVAL_MINUTE_TO_SECOND:
        /// @todo Implement Interval support for ODBC
        throw FeatureUnsuppException("Interval types are not supported yet.");
        break;
    default:
        return DAL_TYPE_UNKNOWN;
    }
}

//
//
OdbcColumnDesc_libodbc::OdbcColumnDesc_libodbc(colnum_t i, OdbcResult_libodbc &result)
    : OdbcColumnDesc()
{
    DBWTL_TRACE1(i);

    SQLRETURN ret;

    OdbcStrW nameW(256);
    OdbcStrA nameA(256);
    SQLSMALLINT sqltype = SQL_UNKNOWN_TYPE;
    SQLSMALLINT namelen = 0;
    SQLSMALLINT digits = 0;
    SQLSMALLINT nullable = SQL_NULLABLE_UNKNOWN;
    SQLLEN is_unsigned = SQL_FALSE;
    SQLLEN scale = 0;
    SQLLEN precision = 0;
    SQLULEN size = 0;

    if(i == 0)
    {
        this->m_daltype = DAL_TYPE_UNKNOWN;
        this->m_name.set(String("__DBWTL_BOOKMARK__"));
    }
    else
    {
        if(result.getDbc().usingUnicode())
        {
            ret = result.drv()->SQLDescribeColW(result.getHandle(), i, nameW.ptr(), nameW.size(), &namelen,
                                                &sqltype, &size, &digits, &nullable);
        }
        else
        {
            ret = result.drv()->SQLDescribeColA(result.getHandle(), i, nameA.ptr(), nameA.size(), &namelen,
                                                &sqltype, &size, &digits, &nullable);
        }

        if(! SQL_SUCCEEDED(ret))
        {
            THROW_ODBC_DIAG_ERROR(result.getStmt().getDbc(), result.getStmt(), result.getHandle(),
                                  SQL_HANDLE_STMT, "DescribeCol failed");
        }


        // we do not ask for char data, so ANSI API should be fine
        ret = result.drv()->SQLColAttributeA(result.getHandle(), i, SQL_DESC_UNSIGNED,
                                             NULL, 0, NULL, &is_unsigned);
        if(! SQL_SUCCEEDED(ret))
            THROW_ODBC_DIAG_ERROR(result.getStmt().getDbc(), result.getStmt(),
                                  result.getHandle(), SQL_HANDLE_STMT, "SQLColAttribute failed");


        // we do not ask for char data, so ANSI API should be fine
        ret = result.drv()->SQLColAttributeA(result.getHandle(), i, SQL_DESC_PRECISION,
                                             NULL, 0, NULL, &precision);
        if(! SQL_SUCCEEDED(ret))
            THROW_ODBC_DIAG_ERROR(result.getStmt().getDbc(), result.getStmt(),
                                  result.getHandle(), SQL_HANDLE_STMT, "SQLColAttribute failed");

        // we do not ask for char data, so ANSI API should be fine
        ret = result.drv()->SQLColAttributeA(result.getHandle(), i, SQL_DESC_SCALE,
                                             NULL, 0, NULL, &scale);
        if(! SQL_SUCCEEDED(ret))
            THROW_ODBC_DIAG_ERROR(result.getStmt().getDbc(), result.getStmt(),
                                  result.getHandle(), SQL_HANDLE_STMT, "SQLColAttribute failed");



        this->m_name.set(result.getDbc().usingUnicode() ? nameW.str(namelen)
                         : nameA.str(namelen, result.getDbc().getDbcEncoding()));
        this->m_size.set<signed int>(size);
        this->m_is_nullable.set<bool>(nullable == SQL_NULLABLE);

		this->m_daltype = sqltype2daltype(sqltype, is_unsigned == SQL_TRUE);

		this->m_precision.set<unsigned short>(precision);
        this->m_scale.set<unsigned short>(scale);
    }
}


//
const OdbcColumnDesc&
OdbcResult_libodbc::describeColumn(colnum_t num) const
{
    if(this->isBad())
        throw EngineException("Resultset is in bad state.");

    if(! this->isOpen())
        throw EngineException("Resultset is not open.");


    std::map<colnum_t, OdbcColumnDesc_libodbc>::const_iterator i =
        this->m_column_desc.find(num);

    if(i == this->m_column_desc.end())
    {
        throw NotFoundException(US("Column '") + String::Internal(Variant(int(num)).asStr()) + US("' not found."));
    }
    else
        return i->second;
}


//
const OdbcColumnDesc&
OdbcResult_libodbc::describeColumn(String name) const
{
    DBWTL_TRACE1(name);
    colnum_t num = this->columnID(name);
    return this->describeColumn(num);
}


void
OdbcResult_libodbc::refreshMetadata(void)
{
    DBWTL_TRACE0();
    this->m_column_desc.clear();

    this->m_column_accessors.clear();
    this->m_allocated_accessors.clear();

    if(! this->getHandle())
        return;


    // We use SQLFetch() and column binding for fetching data, so
    // we need to supply a buffer ptr for each output column.
    // This is automatically done by OdbcData_libodbc.
    size_t colcount = this->columnCount();
    OdbcColumnDesc desc;

	bool can_bind = true;

	SQLUINTEGER gd_ext = sqlgetinfo<SQLUINTEGER>(this->getDbc(), SQL_GETDATA_EXTENSIONS);


    for(size_t i = 0; i <= colcount; ++i)
    {
        OdbcColumnDesc_libodbc x(i, *this);
        this->m_column_desc.insert(std::pair<colnum_t, OdbcColumnDesc_libodbc>(i, x));


        {
            std::pair<VariantListT::iterator,bool> r;
			OdbcData_libodbc *data = new OdbcData_libodbc(*this, i, false);
			OdbcVariant* v = new OdbcVariant(data);
			this->m_allocated_accessors.push_back(v); // smart ptr
			if(can_bind || gd_ext & SQL_GD_ANY_COLUMN)
				can_bind = data->bindcol();
				
            
            r = this->m_column_accessors.insert(VariantListT::value_type(i, v));
        }

    }
}







//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
OdbcEnv_libodbc::OdbcEnv_libodbc(String lib)
    : OdbcEnv(),
      m_lib(),
      m_handle(SQL_NULL_HANDLE)
{
    this->m_lib.reset(new ODBC30Drv(lib));
    SQLRETURN ret = this->m_lib->SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &this->m_handle);
    assert(ret == SQL_SUCCESS);
    ret = this->m_lib->SQLSetEnvAttr(this->m_handle, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
    assert(ret == SQL_SUCCESS);
}



//
OdbcEnv_libodbc::~OdbcEnv_libodbc(void)
{
    SQLRETURN ret = this->m_lib->SQLFreeHandle(SQL_HANDLE_ENV, this->m_handle);
    assert(ret == SQL_SUCCESS);
    this->m_handle = SQL_NULL_HANDLE;
}


SQLHENV
OdbcEnv_libodbc::getHandle(void) const
{
    return this->m_handle;
}

//
OdbcDbc_libodbc*
OdbcEnv_libodbc::newConnection(void)
{
    return new OdbcDbc_libodbc(*this);
}



//
ODBC30Drv*
OdbcEnv_libodbc::drv() const
{
    return this->m_lib.get();
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
OdbcDbc_libodbc::OdbcDbc_libodbc(OdbcEnv_libodbc& env)
    : OdbcDbc(),
      m_lib(env.drv()),
      m_env(env),
      m_dbh(SQL_NULL_HANDLE),
      m_useUnicode(true),
      m_ansics("UNICODE")
{
    assert(env.getHandle() != SQL_NULL_HANDLE);
    SQLRETURN ret = this->drv()->SQLAllocHandle(SQL_HANDLE_DBC, env.getHandle(), &this->m_dbh);
    assert(ret == SQL_SUCCESS);

	this->m_options[DBWTL_ODBC_DEFERRED_LOB_FETCH] = bool(false);
}

IEnv&
OdbcDbc_libodbc::getEnv(void)
{
    return this->m_env;
}

//
OdbcDbc_libodbc::~OdbcDbc_libodbc(void)
{
    this->disconnect();
    SQLRETURN ret = this->drv()->SQLFreeHandle(SQL_HANDLE_DBC, this->m_dbh);
    assert(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
    this->m_dbh = SQL_NULL_HANDLE;
}



//
OdbcStmt_libodbc*
OdbcDbc_libodbc::newStatement(void)
{
    return new OdbcStmt_libodbc(*this);
}



//
void
OdbcDbc_libodbc::connect(String database,
                         String user,
                         String password)
{
    IDbc::Options options;

    options[ "DSN" ] = database;
    options[ "UID" ] = user;
    options[ "PWD" ] = password;
    return this->connect(options);
}


//
OdbcDiag_libodbc::OdbcDiag_libodbc(dalstate_t state,
                                   CodePosInfo pos,
                                   Variant what,
                                   String description,
                                   int odbc_code,
                                   const std::string &sqlstate,
                                   int nativecode)
    : OdbcDiag(state, pos, what, description, sqlstate, nativecode)
{
//    m_sqlstate_id = odbc3error_to_sqlstate(odbc_code);
    //m_sqlstate.set(String(odbc::sqlstate2string(m_sqlstate_id), "UTF-8"));
}


//
OdbcDiag_libodbc*
OdbcDiag_libodbc::clone(void) const
{
    return new OdbcDiag_libodbc(*this);
}


//
OdbcDiag_libodbc::OdbcDiag_libodbc(const OdbcDiag_libodbc& ref)
    : OdbcDiag(ref)
{}



//
OdbcDiag_libodbc::~OdbcDiag_libodbc(void)

{}



//
String
OdbcDiag_libodbc::str(void) const
{
    std::wstringstream ss;

    ss << L"[SQLSTATE:" << ifnull<String>(this->m_sqlstate, L"-----")  << L"] "
       << ifnull<String>(this->m_message, L"No message") << std::endl
       << ifnull<String>(this->m_description, L"No description") << std::endl
       << L"ODBC native error: " << this->m_nativecode << std::endl
       << L"Raised at: "
       << L" [" << this->m_codepos.str() << "]";

    return ss.str();
}



//
void
OdbcDbc_libodbc::connect(IDbc::Options& options)
{
    assert(this->m_isConnected == false);

    assert(this->getHandle() != SQL_NULL_HANDLE);

    if(options["charset"].empty())
    {
		
	this->writeDiagnostic(DiagnosticRec(DBWTL_CPI, DAL_STATE_DEBUG, SQLSTATE("HY000"), 34,
		"connect() failed, invalid arguments",
		23412, 2,
		"connect() failed. You must specify the 'charset' option"
        " with a valid character set"
        " like \"utf-8\" or \"iso-8859-1\"."));

	

	//throw EngineException(DiagnosticRec(OdbcDiagRecXXX));

        throw EngineException("connect() failed. You must specify the 'charset' option"
                              " with a valid character set"
                              " like \"utf-8\" or \"iso-8859-1\".");
    }
    this->m_ansics = options["charset"];

    if(options["unicode"].empty())
    {
        throw EngineException("connect() failed. You must specify the 'unicode' option"
                              " with either 'yes' or 'no' to switch between the ODBC ANSI and UNICODE API.");
    }
    else if(options["unicode"].upper() == "YES")
        this->m_useUnicode = true;
    else if(options["unicode"].upper() == "NO")
        this->m_useUnicode = false;
    else
        throw EngineException(FORMAT1("Invalid value for option \"unicode\": %s", options["unicode"]));


    SQLRETURN ret;
    if(usingUnicode())
    {

        OdbcStrW dsn(options[ "datasource" ]);
        OdbcStrW uid(options[ "username" ]);
        OdbcStrW pwd(options[ "password" ]);

        ret = this->drv()->SQLConnectW(this->getHandle(),
                                       dsn.ptr(), dsn.size(), uid.ptr(), uid.size(), pwd.ptr(), pwd.size());
    }
    else
    {
        OdbcStrA dsn(options[ "datasource" ], this->m_ansics);
        OdbcStrA uid(options[ "username" ], this->m_ansics);
        OdbcStrA pwd(options[ "password" ], this->m_ansics);
        ret = this->drv()->SQLConnectA(this->getHandle(),
                                       dsn.ptr(), dsn.size(), uid.ptr(), uid.size(), pwd.ptr(), pwd.size());
    }

    assert(ret != SQL_INVALID_HANDLE);

    if(! SQL_SUCCEEDED(ret))
    {
        if(usingUnicode())
        {

            OdbcStrW constr(options[ "datasource" ]);
            OdbcStrW outstr(1024);
            SQLSMALLINT outlen = 0;
            ret = this->drv()->SQLDriverConnectW(this->getHandle(), 0,
                                                 constr.ptr(), constr.size(), outstr.ptr(), outstr.size(),
                                                 &outlen, SQL_DRIVER_NOPROMPT);
        }
        else
        {
            OdbcStrA constr(options[ "datasource" ], this->m_ansics);
            OdbcStrA outstr(1024);
            SQLSMALLINT outlen = 0;
            ret = this->drv()->SQLDriverConnectA(this->getHandle(), 0,
                                                 constr.ptr(), constr.size(), outstr.ptr(), outstr.size(),
                                                 &outlen, SQL_DRIVER_NOPROMPT);
        }
    }

    if(! SQL_SUCCEEDED(ret))
    {
        THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC, "Connect failed");
    }
    assert(ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO);
    this->m_isConnected = true;

	SQLUINTEGER gd_ext = sqlgetinfo<SQLUINTEGER>(*this, SQL_GETDATA_EXTENSIONS);

	this->setOption(DBWTL_ODBC_DEFERRED_LOB_FETCH, bool((gd_ext & SQL_GD_ANY_COLUMN) && (gd_ext & SQL_GD_ANY_ORDER) && (gd_ext & SQL_GD_BOUND)));

/*
  DALTRACE_ENTER;


  std::string dbc_db(options[ "database" ].to("UTF-8"));

  int err = this->drv()->odbc3_open_v2(dbc_db.c_str(), &this->m_dbh,
  ODBC_OPEN_READWRITE | ODBC_OPEN_CREATE,
  NULL);
  // allocation error?
  if(this->m_dbh == NULL)
  switch(err)
  {
  case ODBC_OK:
  this->m_isConnected = true;
  break;
  default:
  const char *msg = this->drv()->odbc3_errmsg(this->m_dbh);
  String::Internal u_msg = US("Database: ") + String::Internal(options[ "database" ])
  + US("\r\n") + String::Internal(String(msg, "UTF-8"));

  DAL_ODBC_LIBODBC_DIAG_ERROR(this,
  "connection failure",
  u_msg,
  this->drv()->odbc3_errcode(this->m_dbh),
  this->drv()->odbc3_extended_errcode(this->m_dbh));

  break;
  };


  if(this->isConnected())
  this->setDbcEncoding("UTF-8");

  DALTRACE_LEAVE;
*/
}



//
void
OdbcDbc_libodbc::disconnect(void)
{
    if(this->m_dbh && this->m_isConnected)
    {
        SQLRETURN ret = this->drv()->SQLDisconnect(this->getHandle());
        if(! SQL_SUCCEEDED(ret))
        {
            THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC, "Disconnect failed");
        }

        this->m_isConnected = false;
    }
/*
  DALTRACE_ENTER;

  if(this->m_dbh)
  {
  DALTRACE("is connected, disconnecting...");
  int err = this->drv()->odbc3_close(this->m_dbh);
  switch(err)
  {
  case ODBC_OK:
  this->m_dbh = 0;
  this->m_isConnected = false;
  break;

  case ODBC_BUSY:
  default:
  const char *msg = this->drv()->odbc3_errmsg(this->m_dbh);
  String u_msg(msg, "UTF-8");
  DAL_ODBC_LIBODBC_DIAG_ERROR(this,
  "connection failure",
  u_msg,
  this->drv()->odbc3_errcode(this->m_dbh),
  this->drv()->odbc3_extended_errcode(this->m_dbh));

  break;
  }
  }

  DALTRACE_LEAVE;
*/
}


void
OdbcDbc_libodbc::beginTrans(trx_mode mode,
                            access_mode access,
                            String name)
{
    SQLRETURN ret = SQL_ERROR;
    ret = this->drv()->SQLSetConnectAttrA(this->getHandle(), SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_OFF, SQL_IS_UINTEGER);
    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC, "Set autocommit off failed");
    switch(mode)
    {
    case trx_read_uncommitted:
        ret = this->drv()->SQLSetConnectAttrA(this->getHandle(), SQL_ATTR_TXN_ISOLATION, (void*)SQL_TXN_READ_UNCOMMITTED, SQL_IS_UINTEGER);
        break;
    case trx_read_committed:
        ret = this->drv()->SQLSetConnectAttrA(this->getHandle(), SQL_ATTR_TXN_ISOLATION, (void*)SQL_TXN_READ_COMMITTED, SQL_IS_UINTEGER);
        break;
    case trx_repeatable_read:
        ret = this->drv()->SQLSetConnectAttrA(this->getHandle(), SQL_ATTR_TXN_ISOLATION, (void*)SQL_TXN_REPEATABLE_READ, SQL_IS_UINTEGER);
        break;
    case trx_serializable:
        ret = this->drv()->SQLSetConnectAttrA(this->getHandle(), SQL_ATTR_TXN_ISOLATION, (void*)SQL_TXN_SERIALIZABLE, SQL_IS_UINTEGER);
        break;
    };
    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC, "Set transaction mode failed");
}

void
OdbcDbc_libodbc::commit(void)
{
    SQLRETURN ret = SQL_ERROR;
    ret = this->drv()->SQLEndTran(SQL_HANDLE_DBC, this->getHandle(), SQL_COMMIT);
    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC, "commit failed");
    ret = this->drv()->SQLSetConnectAttrA(this->getHandle(), SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_ON, SQL_IS_UINTEGER);
    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC, "Set autocommit on failed");
}

void
OdbcDbc_libodbc::savepoint(String name)
{
    throw FeatureUnsuppException("ODBC has no support for savepoints");
}

void
OdbcDbc_libodbc::rollback(String name)
{
    if(!name.empty())
        throw FeatureUnsuppException(FORMAT1("ODBC has no support for ROLLACK TO %s", name));

    SQLRETURN ret = SQL_ERROR;
    ret = this->drv()->SQLEndTran(SQL_HANDLE_DBC, this->getHandle(), SQL_ROLLBACK);
    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC, "commit failed");
    ret = this->drv()->SQLSetConnectAttrA(this->getHandle(), SQL_ATTR_AUTOCOMMIT, (void*)SQL_AUTOCOMMIT_ON, SQL_IS_UINTEGER);
    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC, "Set autocommit on failed");
}




SQLHSTMT
OdbcResult_libodbc::getHandle(void) const
{
    return this->m_stmt.getHandle();
}


SQLHSTMT
OdbcStmt_libodbc::getHandle(void) const
{
    return this->m_handle;
}



//
SQLHDBC
OdbcDbc_libodbc::getHandle(void) const
{
    assert(this->m_dbh);
    return this->m_dbh;
}



//
String
OdbcDbc_libodbc::driverName(void) const
{
    return "ODBC 3.0 (libodbc)";
}




/*
 * template<typename T>
 T sqlgetinfo(const OdbcDbc_libodbc &dbc, SQLSMALLINT info)
 {
 T value = T();
 SQLRETURN ret = dbc.drv()->SQLGetInfoW(dbc.getHandle(), info, &value, sizeof(value), 0);
 if(!SQL_SUCCEEDED(ret))
 throw EngineException("SQLGetInfo() returns an unexpected error"); /// @todo
 else
 return value;
 }
*/



//
String
OdbcDbc_libodbc::dbmsName(void) const
{

    SQLSMALLINT ind;

    if(this->usingUnicode())
    {
        OdbcStrW strbufW(256);
        SQLRETURN ret = this->drv()->SQLGetInfoW(this->getHandle(), SQL_DBMS_NAME,
                                                 strbufW.ptr(), strbufW.size()*sizeof(SQLWCHAR), &ind);


        if(! SQL_SUCCEEDED(ret))
        {
            assert(!"failed");
            /*
              THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC,
              "SQLGetInfo() failed");
            */
        }


        return strbufW.str(ind/sizeof(SQLWCHAR));
    }
    else
    {
        OdbcStrA strbufA(256);
        SQLRETURN ret = this->drv()->SQLGetInfoA(this->getHandle(), SQL_DBMS_NAME,
                                                 strbufA.ptr(), strbufA.size()*sizeof(SQLCHAR), &ind);


        if(! SQL_SUCCEEDED(ret))
        {
            assert(!"failed");
            /*
              THROW_ODBC_DIAG_ERROR(this->m_env, *this, this->getHandle(), SQL_HANDLE_DBC,
              "SQLGetInfo() failed");
            */
        }


        return strbufA.str(ind/sizeof(SQLCHAR), this->getDbcEncoding());

    }
}



//
ODBC30Drv*
OdbcDbc_libodbc::drv(void) const
{
    assert(this->m_lib);
    return this->m_lib;
}


OdbcStmt*
OdbcDbc_libodbc::getOdbcCatalogs(void)
{
    OdbcStmt_libodbc *stmt = newStatement();
    stmt->openOdbcCatalogs();
    return stmt;
}

OdbcStmt*
OdbcDbc_libodbc::getOdbcSchemas(const Variant &catalog)
{
    OdbcStmt_libodbc *stmt = newStatement();
    stmt->openOdbcSchemas(catalog);
    return stmt;
}

OdbcStmt*
OdbcDbc_libodbc::getOdbcTables(const Variant &catalog, const Variant &schema, const Variant &type)
{
    OdbcStmt_libodbc *stmt = newStatement();
    stmt->openOdbcTables(catalog, schema, type);
    return stmt;
}


OdbcStmt*
OdbcDbc_libodbc::getOdbcColumns(const Variant &catalog, const Variant &schema, const Variant &table)
{
    /// @todo possible leak on exception
    OdbcStmt_libodbc *stmt = newStatement();
    stmt->openOdbcColumns(catalog, schema, table);
    return stmt;
}


void
OdbcStmt_libodbc::openOdbcTables(const Variant &catalog, const Variant &schema, const Variant &type)
{
    SQLRETURN ret;

    OdbcResult_libodbc* rs = this->newResultset();

    if(this->getDbc().usingUnicode())
    {

// unixODBC seems to have an issue with catalog names here.
// We leave these empty for the moment...
#ifdef DBWTL_ON_UNIX
        OdbcStrW str_catalog;
#else
        OdbcStrW str_catalog(catalog);
#endif
        OdbcStrW str_schema(schema);
        OdbcStrW str_type(type);

        ret = this->drv()->SQLTablesW(this->getHandle(),
#ifdef DBWTL_ON_LINUX
                                      NULL, 0,
#else
                                      catalog.isnull() ? 0 : str_catalog.ptr(),
                                      str_catalog.size(),
#endif
                                      schema.isnull() ? 0 : str_schema.ptr(),
                                      str_schema.size(),
                                      NULL, 0,
                                      type.isnull() ? 0 : str_type.ptr(),
                                      str_type.size());
    }
    else
    {
        OdbcStrA str_catalog(catalog, this->getDbc().getDbcEncoding());
        OdbcStrA str_schema(schema, this->getDbc().getDbcEncoding());
        OdbcStrA str_type(type, this->getDbc().getDbcEncoding());

        ret = this->drv()->SQLTablesA(this->getHandle(),
                                      catalog.isnull() ? 0 : str_catalog.ptr(),
                                      str_catalog.size(),
                                      schema.isnull() ? 0 : str_schema.ptr(),
                                      str_schema.size(),
                                      NULL, 0,
                                      type.isnull() ? 0 : str_type.ptr(),
                                      str_type.size());
    }


    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->getDbc(), *this, this->getHandle(), SQL_HANDLE_STMT,
                              "SQLTables() failed");

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);

    DAL_SET_CURSORSTATE(rs->m_cursorstate, DAL_CURSOR_OPEN);

    rs->refreshMetadata();
}


void
OdbcStmt_libodbc::openOdbcCatalogs(void)
{
    SQLRETURN ret;

    OdbcResult_libodbc* rs = this->newResultset();

    if(this->getDbc().usingUnicode())
    {
        OdbcStrW catalog(String(SQL_ALL_CATALOGS));
        OdbcStrW schema(String(""));
        OdbcStrW table(String(""));
        OdbcStrW type;
        ret = this->drv()->SQLTablesW(this->getHandle(),
                                      catalog.ptr(), catalog.size(),
                                      schema.ptr(), schema.size(),
                                      table.ptr(), table.size(),
                                      type.ptr(), type.size());
    }
    else
    {
        OdbcStrA catalog(String(SQL_ALL_CATALOGS), std::string("ASCII"));
        OdbcStrA schema(String(""), std::string("ASCII"));
        OdbcStrA table(String(""), std::string("ASCII"));
        OdbcStrA type;
        ret = this->drv()->SQLTablesA(this->getHandle(),
                                      catalog.ptr(), catalog.size(),
                                      schema.ptr(), schema.size(),
                                      table.ptr(), table.size(),
                                      type.ptr(), type.size());
    }


    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->getDbc(), *this, this->getHandle(), SQL_HANDLE_STMT,
                              "SQLTables() failed");

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);

    DAL_SET_CURSORSTATE(rs->m_cursorstate, DAL_CURSOR_OPEN);

    rs->refreshMetadata();
}



void
OdbcStmt_libodbc::openOdbcSchemas(const Variant &catalog)
{
    SQLRETURN ret;

    OdbcResult_libodbc* rs = this->newResultset();
    /// @todo catalog is ignored, because only the current catalog is used
    if(this->getDbc().usingUnicode())
    {
        OdbcStrW str_catalog(String(""));
        OdbcStrW str_schema(String(SQL_ALL_SCHEMAS));
        OdbcStrW str_table(String(""));
        OdbcStrW str_type;
        ret = this->drv()->SQLTablesW(this->getHandle(),
                                      str_catalog.ptr(), 0,
                                      str_schema.ptr(), str_schema.size(),
                                      str_table.ptr(), 0,
                                      NULL, 0);
    }
    else
    {
        OdbcStrA str_catalog(String(""), std::string("ASCII"));
        OdbcStrA str_schema(String(SQL_ALL_SCHEMAS), std::string("ASCII"));
        OdbcStrA str_table(String(""), std::string("ASCII"));
        OdbcStrA str_type;
        ret = this->drv()->SQLTablesA(this->getHandle(),
                                      str_catalog.ptr(), 0,
                                      str_schema.ptr(), str_schema.size(),
                                      str_table.ptr(), 0,
                                      NULL, 0);
    }


    if(! SQL_SUCCEEDED(ret))
    {
        //HYC00; -> schemas not supported!
        THROW_ODBC_DIAG_ERROR(this->getDbc(), *this, this->getHandle(), SQL_HANDLE_STMT,
                              "SQLTables() failed");
    }

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);

    DAL_SET_CURSORSTATE(rs->m_cursorstate, DAL_CURSOR_OPEN);

    rs->refreshMetadata();
}



void
OdbcStmt_libodbc::openOdbcColumns(const Variant &catalog, const Variant &schema, const Variant &table)
{
    SQLRETURN ret;

    OdbcResult_libodbc* rs = this->newResultset();

    if(this->getDbc().usingUnicode())
    {
        OdbcStrW str_catalog(catalog);
        OdbcStrW str_schema(schema);
        OdbcStrW str_table(table);

        ret = this->drv()->SQLColumnsW(this->getHandle(),
                                       catalog.isnull()  ? 0 : str_catalog.ptr(),
                                       str_catalog.size(),
                                       schema.isnull() ? 0 : str_schema.ptr(),
                                       str_schema.size(),
                                       table.isnull() ? 0 : str_table.ptr(),
                                       str_table.size(),
                                       0, 0);
    }
    else
    {
        OdbcStrA str_catalog(catalog, this->getDbc().getDbcEncoding());
        OdbcStrA str_schema(schema, this->getDbc().getDbcEncoding());
        OdbcStrA str_table(table, this->getDbc().getDbcEncoding());

        ret = this->drv()->SQLColumnsA(this->getHandle(),
                                       catalog.isnull()  ? 0 : str_catalog.ptr(),
                                       str_catalog.size(),
                                       schema.isnull() ? 0 : str_schema.ptr(),
                                       str_schema.size(),
                                       table.isnull() ? 0 : str_table.ptr(),
                                       str_table.size(),
                                       0, 0);
    }


    if(! SQL_SUCCEEDED(ret))
        THROW_ODBC_DIAG_ERROR(this->getDbc(), *this, this->getHandle(), SQL_HANDLE_STMT,
                              "SQLColumns() failed");

    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_OPEN);

    DAL_SET_CURSORSTATE(rs->m_cursorstate, DAL_CURSOR_OPEN);

    rs->refreshMetadata();
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
OdbcStmt_libodbc::OdbcStmt_libodbc(OdbcDbc_libodbc& conn)
    : m_conn(conn),
      m_resultsets(),
      m_currentResultset(0),
      m_handle(SQL_NULL_HANDLE)
{

    assert(conn.getHandle() != SQL_NULL_HANDLE);
    SQLRETURN ret = this->drv()->SQLAllocHandle(SQL_HANDLE_STMT, conn.getHandle(), &this->m_handle);
    assert(ret == SQL_SUCCESS);
}



//
OdbcStmt_libodbc::~OdbcStmt_libodbc(void)
{
    this->close();
    if(this->m_handle != SQL_NULL_HANDLE)
    {
        SQLRETURN ret = this->drv()->SQLFreeHandle(SQL_HANDLE_STMT, this->m_handle);
        assert(ret == SQL_SUCCESS);
        this->m_handle = SQL_NULL_HANDLE;
    }
}



//
OdbcResult&
OdbcStmt_libodbc::resultset(void)
{
    return *this->m_resultsets.at(this->m_currentResultset);
}



//
const OdbcResult&
OdbcStmt_libodbc::resultset(void) const
{
    return *this->m_resultsets.at(this->m_currentResultset);
}



/// @todo For now, prepare() only supports a single statement.
/// It is planned to support multiple statements.
void
OdbcStmt_libodbc::prepare(String sql)
{
    /// For SQLite, it is save to create a new resultset
    /// and execute the query inside of the resultset, because
    /// a query can never return more then one resultset.
    OdbcResult_libodbc* rs = this->newResultset();
    rs->prepare(sql);
    DAL_SET_CURSORSTATE(this->m_cursorstate, DAL_CURSOR_PREPARED);
}



//
void
OdbcStmt_libodbc::execute(void)
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
OdbcStmt_libodbc::execDirect(String sql)
{
    this->prepare(sql);
    this->execute();
}



//
void
OdbcStmt_libodbc::close(void)
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
    OdbcStmt::close();

    DALTRACE_LEAVE;
}



//
bool
OdbcStmt_libodbc::nextResultset(void)
{
    return false;
}



//
bool
OdbcStmt_libodbc::moreResultsets(void) const
{
    return false;
}



//
size_t
OdbcStmt_libodbc::paramCount(void) const
{
    return this->resultset().paramCount();
}



//
rowid_t
OdbcStmt_libodbc::affectedRows(void) const
{
    return this->resultset().affectedRows();
}


//
Variant
OdbcStmt_libodbc::lastInsertRowId(void)
{
    return this->resultset().lastInsertRowId();
}



//
ODBC30Drv*
OdbcStmt_libodbc::drv(void) const
{
    return this->getDbc().drv();
}



//
OdbcDbc_libodbc&
OdbcStmt_libodbc::getDbc(void) const
{
    return this->m_conn;
}



//
OdbcResult_libodbc*
OdbcStmt_libodbc::newResultset(void)
{
    this->m_resultsets.push_back(new OdbcResult_libodbc(*this));
    return this->m_resultsets.at(this->m_resultsets.size()-1);
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




std::string
OdbcDbc_libodbc::getDbcEncoding(void) const
{
    return this->m_ansics;
}

void
OdbcDbc_libodbc::setDbcEncoding(std::string encoding)
{
    this->m_ansics = encoding;
}



bool
OdbcDbc_libodbc::usingUnicode(void) const
{
    return this->m_useUnicode;
    //return this->m_ansics == "UNICODE";
}



Variant
OdbcDbc_libodbc::getCurrentCatalog(void)
{
    return sqlgetinfo<String>(*this, SQL_DATABASE_NAME);
}



String
OdbcDbc_libodbc::quoteIdentifier(const String &id)
{
    String q = sqlgetinfo<String>(*this, SQL_IDENTIFIER_QUOTE_CHAR);

    return q + id + q;
}



DAL_NAMESPACE_END
