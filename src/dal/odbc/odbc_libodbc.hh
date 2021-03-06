//
// odbc_libodbc.hh - SQLite Backend libodbc
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

///
/// @cond DEV_DOCS
/// @file
/// @brief SQLite Backend libodbc
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_ODBC_LIBODBC_HH
#define INFORMAVE_DB_DAL_ODBC_LIBODBC_HH

#include "dbwtl/dal/engines/odbc_engine.hh"
#include "driver_odbc.hh"


DAL_NAMESPACE_BEGIN


#define DBWTL_ODBC_LOB_BUFSIZE 512

#define DBWTL_ODBC_MAX_STRING_SIZE (256*256)
#define DBWTL_ODBC_MAX_VARBINARY_SIZE (256*256)


class OdbcResult_libodbc;
class OdbcStmt_libodbc;
class OdbcDbc_libodbc;
class OdbcEnv_libodbc;
class OdbcData_libodbc;
class OdbcDiag_libodbc;



//------------------------------------------------------------------------------
///
/// @internal
/// @brief OdbcDiag implementation for libodbc
class DBWTL_EXPORT OdbcDiagnosticRec_libodbc : public OdbcDiagnosticRec
{
public:
	OdbcDiagnosticRec_libodbc(const CodePosInfo & cpi,
		dalstate_t dalstate,
		SQLSTATE sqlstate,
		const Variant &nativeCode,
		const String &msg,
		rownum_t rowNum = 0,
		colnum_t colNum = 0,
		const String &desc = String(),
		const Variant &data = Variant());

    virtual ~OdbcDiagnosticRec_libodbc(void)
    {}


    virtual OdbcDiagnosticRec_libodbc* clone(void) const;
};

//------------------------------------------------------------------------------
///
/// @internal
/// @brief OdbcDiag implementation for libodbc
class OdbcDiag_libodbc : public OdbcDiag
{
public:
    OdbcDiag_libodbc(dalstate_t state,
                     CodePosInfo pos,
                     Variant what,
                     String description,
                     int odbc_code,
                     const std::string &sqlstate,
                     int nativecode);

    OdbcDiag_libodbc(const OdbcDiag_libodbc& ref);
    virtual ~OdbcDiag_libodbc(void);
    
    virtual String str(void) const;
    virtual OdbcDiag_libodbc* clone(void) const;    


};





//------------------------------------------------------------------------------
///
/// @internal
/// @brief OdbcBlob implementation
class OdbcBlob_libodbc : public OdbcBlob
{
public:
    OdbcBlob_libodbc(const OdbcData_libodbc& data, SQLLEN &ind);

    virtual ~OdbcBlob_libodbc(void);

    virtual bool isNull(void) const;

    virtual ODBC30Drv* drv(void) const;

    virtual SQLHSTMT getHandle(void) const;

protected:
    virtual int_type underflow();

    const OdbcData_libodbc& m_data;

    char_type           m_buf[DAL_STREAMBUF_BUFSIZE + DAL_STREAMBUF_PUTBACK];
	SQLLEN			   &m_ind;
    const std::size_t   m_putback;

private:
    OdbcBlob_libodbc(const OdbcBlob_libodbc&);
    OdbcBlob_libodbc& operator=(const OdbcBlob_libodbc&);
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief OdbcMemo implementation
class OdbcMemo_libodbc : public OdbcMemo
{
public:
    OdbcMemo_libodbc(const OdbcData_libodbc& data, SQLLEN &ind);

    virtual ~OdbcMemo_libodbc(void);

    virtual bool isNull(void) const;

    virtual ODBC30Drv* drv(void) const;

    virtual SQLHSTMT getHandle(void) const;

protected:
    virtual int_type underflow();

    const OdbcData_libodbc& m_data;

    char_type           m_buf[DAL_STREAMBUF_BUFSIZE + DAL_STREAMBUF_PUTBACK];
	SQLLEN			   &m_ind;
    const std::size_t   m_putback;

private:
    OdbcMemo_libodbc(const OdbcMemo_libodbc&);
    OdbcMemo_libodbc& operator=(const OdbcMemo_libodbc&);
};




template<int T>
struct basic_odbcstr
{
};


template<>
struct basic_odbcstr<1>
{
    basic_odbcstr(size_t size = 0)
        : m_string()
	{
        m_string.resize(size);
    }

    explicit basic_odbcstr(const Variant &var, const std::string &charset)
        : m_string()
    {
        if(!var.isnull())
        {
            std::string bs = var.asStr().to(charset);
            std::copy(bs.begin(), bs.end(), std::back_inserter(m_string));
        }
        // unixODBC tracer ignores the length of a string but
        // assumes SQL_NTS. For this and other broken implementations
        // we always add a null-terminator.
        // m_string.push_back(0);
    }


	explicit basic_odbcstr(const String &str, const std::string &charset)
		: m_string()
	{
		std::string bs = str.to(charset);
		std::copy(bs.begin(), bs.end(), std::back_inserter(m_string));

        // unixODBC tracer ignores the length of a string but
        // assumes SQL_NTS. For this and other broken implementations
        // we always add a null-terminator.
        // m_string.push_back(0);
	}

    SQLCHAR* ptr(void) const
    {
        return const_cast<SQLCHAR*>(m_string.data());
    }

    SQLLEN size(void) const
    {
		return this->m_string.size();// ? this->m_string.size()-1 : 0;
    }

	bool empty(void) const
	{
		return this->m_string.empty();// ||
        //(this->m_string.size() == 1 && this->m_string[0] == 0);
	}

    size_t max_size(void) const
    {
        return this->m_string.max_size();
    }

	void resize(SQLLEN len) // in characters
	{
		this->m_string.resize(len);
	}

	String str(SQLLEN len, const std::string &charset) const
	{
		return str(len, charset.c_str());
	}

    String str(SQLLEN len, const char *charset) const
    {
        if(len == SQL_NTS)
        {
            len = 0;
            while(m_string[len]) ++len;
        }

        assert(len >= 0 && (unsigned)len <= m_string.size());
        if((unsigned)len > m_string.size())
            throw std::out_of_range("length for str() is bigger than current string"); // should never happen

        std::string s(reinterpret_cast<const char*>(m_string.data()), len);
        return String(s, charset);
    }

protected:
    mutable std::basic_string<unsigned char> m_string;
};


template<>
struct basic_odbcstr<2>
{
	basic_odbcstr(size_t size = 0)
        : m_string()
	{
		m_string.resize(size);
	}


    explicit basic_odbcstr(const Variant &var)
        : m_string()
    {
        if(!var.isnull())
        {
            std::wstring ws = var.asStr();
            std::copy(ws.begin(), ws.end(), std::back_inserter(m_string));
        }
        // unixODBC tracer ignores the length of a string but
        // assumes SQL_NTS. For this and other broken implementations
        // we always add a null-terminator.
        //m_string.push_back(0);
    }

	explicit basic_odbcstr(const String &str)
        : m_string()
	{
		std::wstring ws = str;
		std::copy(ws.begin(), ws.end(), std::back_inserter(m_string));

        // unixODBC tracer ignores the length of a string but
        // assumes SQL_NTS. For this and other broken implementations
        // we always add a null-terminator.
        // m_string.push_back(0);
	}

	SQLWCHAR* ptr(void) const
	{
		return const_cast<SQLWCHAR*>(m_string.data());
	}

	SQLLEN size(void) const
	{
		return this->m_string.size();// ? this->m_string.size()-1 : 0;
	}

	bool empty(void) const
	{
		return this->m_string.empty();// ||
        //(this->m_string.size() == 1 && this->m_string[0] == 0);
	}

    size_t max_size(void) const
    {
        return this->m_string.max_size();
    }

	void resize(SQLLEN len) // in characters
	{
		this->m_string.resize(len);
	}

	String str(SQLLEN len) const
	{
		std::wstring ws;
        if(len == SQL_NTS)
        {
            len = 0;
            while(m_string[len]) ++len;
        }
		std::copy(m_string.begin(), m_string.begin()+len, std::back_inserter(ws));
		return String(ws);
	}

	String str(void) const
	{
		std::wstring ws;
		std::copy(m_string.begin(), m_string.end(), std::back_inserter(ws));
		return String(ws);
	}

protected:
#ifdef _WIN32
	mutable std::basic_string<wchar_t> m_string;
#else
	mutable std::basic_string<unsigned short> m_string;
#endif
};


typedef basic_odbcstr<sizeof(SQLWCHAR)> OdbcStrW;
typedef basic_odbcstr<sizeof(SQLCHAR)> OdbcStrA;


struct OdbcValue
{
    OdbcValue(void)
        : data(),
          ind(SQL_NULL_DATA),
          ctype(SQL_C_DEFAULT),
		  sqltype(SQL_UNKNOWN_TYPE),
		  size(0),
		  buf(0),
		  buflen(0),
		  is_unsigned(SQL_FALSE),
          strbufW(),
          strbufA(),
          varbinary()
    {}

    union
    {
        SQLSCHAR sTinyInt;
        SQLCHAR uTinyInt;
        SQLINTEGER sLongInt;
        SQLUINTEGER uLongInt;
        SQLSMALLINT sShortInt;
        SQLUSMALLINT uShortInt;
        SQLBIGINT sBigInt;
        SQLUBIGINT uBigInt;
        SQLCHAR bit;
        SQLREAL float_;
        SQLDOUBLE double_;

        SQL_DATE_STRUCT date;
        SQL_TIME_STRUCT time;
        SQL_TIMESTAMP_STRUCT timestamp;
        SQL_NUMERIC_STRUCT numeric;
        //SQLGUID  guid;
        SQL_INTERVAL_STRUCT interval;
		SQLWCHAR wchar_dummy;
    } data;
    //char padding[128];
    SQLLEN ind;
    SQLSMALLINT ctype;
	SQLSMALLINT sqltype;
	SQLULEN size;
	SQLPOINTER buf;
	SQLINTEGER buflen;
    SQLLEN is_unsigned;
    OdbcStrW strbufW;
    OdbcStrA strbufA;
    TVarbinary varbinary; // used for parameter data

private:
    OdbcValue(const OdbcValue &);
    OdbcValue& operator=(const OdbcValue &);
};


//------------------------------------------------------------------------------
///
/// @internal
/// @brief OdbcData implementation for libodbc
class OdbcData_libodbc : public OdbcData
{
public:
    OdbcData_libodbc(OdbcResult_libodbc& result, colnum_t colnum, bool locked);
    virtual ~OdbcData_libodbc(void);

    friend class OdbcBlob_libodbc;
    friend class OdbcMemo_libodbc;

    virtual OdbcBlob_libodbc*       getBlobStream(void) const;
    virtual UnicodeStreamBuf*       getMemoStream(void) const;

    virtual String getString(void) const;
    virtual signed short int getSShort(void) const;
    virtual unsigned short int getUShort(void) const;
    virtual signed long int getSLong(void) const;
    virtual unsigned long int getULong(void) const;
    virtual float getFloat(void) const;
    virtual double getDouble(void) const;
    virtual unsigned char getBit(void) const;
    virtual signed char getSTinyint(void) const;
    virtual unsigned char getUTinyint(void) const;
    virtual signed long long getSBigint(void) const;
    virtual unsigned long long getUBigint(void) const;
    virtual TVarbinary getBinary(void) const;
    virtual unsigned long int getBookmark(void) const;
    virtual const unsigned char* getVarbookmark(void) const;
    virtual TDate getDate(void) const;
    virtual TTime getTime(void) const;
    virtual TTimestamp getTimestamp(void) const;
    virtual TNumeric getNumeric(void) const;



    virtual bool         isnull(void) const;
    virtual rowid_t      getCurrentRowID(void) const;

    virtual void refresh(void);

	virtual bool bindcol(void);
	virtual bool getdata(void);

	virtual void initValue(void);

    virtual daltype_t daltype(void) const;

    virtual void fetchParts(void);

	virtual BlobStream cast2BlobStream(std::locale loc) const;
	virtual MemoStream cast2MemoStream(std::locale loc) const;
	virtual Blob cast2Blob(std::locale loc) const;
	virtual Memo cast2Memo(std::locale loc) const;

protected:
    OdbcResult_libodbc& getResultset(void) const;
    OdbcStmt_libodbc& getStmt(void) const;
    SQLHSTMT getHandle(void) const;
    ODBC30Drv* drv(void) const;

    void bindIndicator(colnum_t colnum, SQLLEN *ind);


    OdbcResult_libodbc& m_resultset;

    colnum_t   m_colnum;
    mutable std::auto_ptr<OdbcBlob_libodbc> m_blobbuf;
    mutable std::auto_ptr<OdbcMemo_libodbc> m_memobuf;

	// This caches are used for MemoStream -> Memo conversions
	// and must be reset if the cursor moves.
    mutable std::shared_ptr<std::stringstream> m_blob_cache;
    mutable std::shared_ptr<std::wstringstream> m_memo_cache;


    /// @todo Used for ANSI memo data conversion
    mutable std::auto_ptr<std::wstringstream> m_memostream;

    //SQLSMALLINT m_type;
    SQLLEN m_bufsize;
    //void *m_buf;
    //SQLLEN m_ind;

    mutable OdbcValue m_value;
	bool m_is_bound;
    //std::vector<SQLWCHAR> m_strbufW;
    //std::vector<SQLCHAR> m_strbufA;
    //OdbcStrW m_strbufW;
    //OdbcStrA m_strbufA;
    //std::vector<unsigned char> m_binarybuf;
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief OdbcColumnDesc implementation for libodbc
class OdbcColumnDesc_libodbc : public OdbcColumnDesc
{
public:
    OdbcColumnDesc_libodbc(colnum_t i, OdbcResult_libodbc &result);

    virtual ~OdbcColumnDesc_libodbc(void)
    {}
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SQLite Resultset implementation for libodbc
class OdbcResult_libodbc : public OdbcResult
{
    friend class OdbcStmt_libodbc;

public:
    OdbcResult_libodbc(OdbcStmt_libodbc& stmt);
    virtual ~OdbcResult_libodbc(void);

    virtual void   first(void);
    virtual bool   next(void);
    virtual bool   eof(void) const;
    virtual void   close(void);

    virtual const OdbcVariant&     column(colnum_t num);
    virtual const OdbcVariant&     column(String name);

    // row methods
    virtual rowcount_t         rowCount(void) const;
    virtual rowcount_t         affectedRows(void) const;

    virtual Variant            lastInsertRowId(void);

    // column methods
    virtual size_t             columnCount(void) const;
    virtual colnum_t           columnID(String name) const;
    virtual String             columnName(colnum_t num) const;

    virtual rowid_t            getCurrentRowID(void) const;

    virtual ODBC30Drv*        drv(void) const;


    virtual const OdbcColumnDesc& describeColumn(colnum_t num) const;

    virtual const OdbcColumnDesc& describeColumn(String name) const;


    virtual OdbcDbc_libodbc& getDbc(void) const;
    virtual OdbcStmt_libodbc& getStmt(void) const;

    virtual SQLHSTMT getHandle(void) const;

    virtual OdbcDiag& appendDiagRec(const OdbcDiag &diag);

    // odbc specific
    virtual void   prepare(String sql);
    virtual void   execute(StmtBase::ParamMap& params);


protected:
    typedef std::map<colnum_t, OdbcVariant*> VariantListT;

    virtual void         refreshMetadata(void);
    virtual size_t       paramCount(void) const;
    void                 reset(void);

    void                 bindParamBlob(StmtBase::ParamMapIterator param);
    void		 bindParamMemo(StmtBase::ParamMapIterator param);
    void		 bindParamNumeric(StmtBase::ParamMapIterator param);
    void		 bindParamString(StmtBase::ParamMapIterator param);

    OdbcStmt_libodbc    &m_stmt;
    rowid_t                  m_current_tuple;
    int                      m_last_row_status;
    bool                     m_isopen;
    mutable SQLSMALLINT      m_cached_resultcol_count;


    std::map<colnum_t, std::shared_ptr<OdbcValue> > m_param_data;


    ///
    /// @brief Stores the type information of all columns in the resultset
    std::map<colnum_t, OdbcColumnDesc_libodbc>     m_column_desc;

    ///
    /// @brief Stores all column accessors (IVariants) requested by an user for
    /// the current resultset.
    ///
    /// @details The column accessors are created when an user requested one
    /// and only get destroyed when the resultset becomes invalid (closed, destroyed..)
    /// We use this container only for lookups if a reqeusted accessor is already allocated.
    /// The accessors are stored in m_allocted_accessors, too - which cleanup
    /// the accessors if thy are not needed anymore.
    ///
    /// @note Do not delete (free) any of the elements, this task is
    /// done by m_allocated_accessors's  destructor.
    VariantListT           m_column_accessors;

    
    ///
    /// @brief Stores all allocated accessors (IVariants) requested by an user for
    /// the current resultset.
    ///
    /// @note all objects (this container stores pointer to these) are free'd if
    /// the user closes the resultset.
    std::vector<informave::utils::SmartPtr<OdbcVariant,
                                           informave::utils::RefCounted,
                                           informave::utils::AllowConversion> > m_allocated_accessors;


private:
    OdbcResult_libodbc(const OdbcResult_libodbc&);
    OdbcResult_libodbc& operator=(const OdbcResult_libodbc&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SQLite Statement implementation for libodbc
class OdbcStmt_libodbc : public OdbcStmt
{
    typedef std::vector<OdbcResult_libodbc*> ResultsetVectorT;
    
    friend class OdbcResult_libodbc;

public:
    OdbcStmt_libodbc(OdbcDbc_libodbc& conn);
    virtual ~OdbcStmt_libodbc(void);

    virtual OdbcResult&        resultset(void);
    virtual const OdbcResult&  resultset(void) const;

    virtual void      prepare(String sql);
    virtual void      execute(void);
    virtual void      execDirect(String sql);
    //virtual void      execDirect(std::istream src) = 0;
    virtual void      close(void);

    virtual bool      nextResultset(void);
    virtual bool      moreResultsets(void) const;

    virtual size_t    paramCount(void) const;

    virtual rowid_t   affectedRows(void) const;

    virtual Variant   lastInsertRowId(void);

    virtual ODBC30Drv* drv(void) const;

    virtual OdbcDbc_libodbc& getDbc(void) const; // interface

    virtual OdbcDiag& appendDiagRec(const OdbcDiag &diag);

    virtual SQLHSTMT getHandle(void) const;

	// exttensions
	virtual void openOdbcCatalogs(void);
	virtual void openOdbcSchemas(const Variant &catalog);
	virtual void openOdbcTables(const Variant &catalog, const Variant &schema, const Variant &type);
	virtual void openOdbcColumns(const Variant &catalog, const Variant &schema, const Variant &table);

	std::list<OdbcDiagnosticRec_libodbc> getDiagRecs(const CodePosInfo & cpi) const;

protected:
    OdbcResult_libodbc* newResultset(void);

    OdbcDbc_libodbc      &m_conn;
    ResultsetVectorT          m_resultsets;
    int                       m_currentResultset;
    mutable SQLHSTMT  m_handle;
    //bool                      m_isPrepared;


private:
    OdbcStmt_libodbc(const OdbcStmt_libodbc&);
    OdbcStmt_libodbc& operator=(const OdbcStmt_libodbc&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SQLite Connection implementation for libodbc
class OdbcDbc_libodbc : public OdbcDbc
{
public:
    OdbcDbc_libodbc(OdbcEnv_libodbc& env);
    virtual ~OdbcDbc_libodbc(void);

    virtual OdbcStmt_libodbc*    newStatement(void);

	virtual IEnv& getEnv(void);

    virtual void           connect(String database,
                                   String user = String(),
                                   String password = String());
    virtual void           connect(IDbc::Options& options);
    virtual void           disconnect(void);
    virtual String         driverName(void) const;
    virtual String         dbmsName(void) const;

    virtual std::string    getDbcEncoding(void) const;

    virtual SQLHDBC     getHandle(void) const;

    virtual ODBC30Drv*    drv(void) const;

    virtual OdbcDiag& appendDiagRec(const OdbcDiag &diag);

    virtual void           beginTrans(trx_mode mode,
                                      access_mode access = trx_default,
                                      String name = String());
    virtual void           commit(void);
    virtual void           savepoint(String name);
    virtual void           rollback(String name = String());


    // ODBC extensions
    virtual bool usingUnicode(void) const;

    virtual const OdbcEnv_libodbc& getEnv(void) const { return this->m_env; }

    virtual OdbcStmt*      getOdbcCatalogs(void);
    virtual OdbcStmt*      getOdbcSchemas(const Variant &catalog);
    virtual OdbcStmt*      getOdbcTables(const Variant &catalog, const Variant &schema, const Variant &type);
    virtual OdbcStmt*      getOdbcColumns(const Variant &catalog, const Variant &schema, const Variant &table);

    virtual Variant        getCurrentCatalog(void);

    virtual String         quoteIdentifier(const String &id);

	std::list<OdbcDiagnosticRec_libodbc> getDiagRecs(const CodePosInfo & cpi) const;

protected:
    virtual void           setDbcEncoding(std::string encoding);

    ODBC30Drv           *m_lib; /* lib is stored in ENV */
    OdbcEnv_libodbc   &m_env;
    mutable SQLHDBC    m_dbh;
    bool               m_useUnicode;
    std::string        m_ansics;


private:
    OdbcDbc_libodbc(const OdbcDbc_libodbc&);
    OdbcDbc_libodbc& operator=(const OdbcDbc_libodbc&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SQLite Environment implementation for libodbc
class OdbcEnv_libodbc : public OdbcEnv
{
public:
    OdbcEnv_libodbc(String lib = String());
    virtual ~OdbcEnv_libodbc(void);

    virtual OdbcDbc_libodbc* newConnection(void);
    virtual ODBC30Drv* drv(void) const;

    virtual OdbcDiag& appendDiagRec(const OdbcDiag &diag);

    virtual SQLHENV getHandle(void) const;

	std::list<OdbcDiagnosticRec_libodbc> getDiagRecs(const CodePosInfo & cpi) const;


protected:
    mutable std::auto_ptr<ODBC30Drv>    m_lib;
    SQLHENV	m_handle;

private:
    OdbcEnv_libodbc(const OdbcEnv_libodbc&);
    OdbcEnv_libodbc& operator=(const OdbcEnv_libodbc&);
};




/// @endcond


DAL_NAMESPACE_END

#endif


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
