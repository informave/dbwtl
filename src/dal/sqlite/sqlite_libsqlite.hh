//
// sqlite_libsqlite.hh - SQLite Backend libsqlite
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
/// @brief SQLite Backend libsqlite
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_SQLITE_LIBSQLITE_HH
#define INFORMAVE_DB_DAL_SQLITE_LIBSQLITE_HH

#include "dbwtl/dal/engines/sqlite_engine.hh"
#include "driver_libsqlite.hh"


DAL_NAMESPACE_BEGIN

class SqliteResult_libsqlite;
class SqliteStmt_libsqlite;
class SqliteDbc_libsqlite;
class SqliteEnv_libsqlite;
class SqliteData_libsqlite;
class SqliteDiag_libsqlite;



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SqliteDiag implementation for libsqlite
class SqliteDiag_libsqlite : public SqliteDiag
{
public:
    SqliteDiag_libsqlite(dalstate_t state,
                         CodePosInfo pos,
                         Variant what,
                         String description,
                         int sqlite_code,
                         int sqlite_excode);

    SqliteDiag_libsqlite(const SqliteDiag_libsqlite& ref);
    virtual ~SqliteDiag_libsqlite(void);
    
    virtual String str(void) const;
    virtual SqliteDiag_libsqlite* clone(void) const;    

protected:
    Variant m_sqlite_code;
    Variant m_sqlite_excode;
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SqliteBlob implementation for libsqlite
class SqliteBlob_libsqlite : public SqliteBlob
{
public:
    SqliteBlob_libsqlite(const SqliteData_libsqlite& data);

    virtual ~SqliteBlob_libsqlite(void);

    virtual void setBufPtr(const unsigned char *buf, size_t size);

    virtual bool isNull(void) const;

protected:
    virtual int_type underflow();
    virtual int_type uflow();
    virtual int_type pbackfail(int_type ch);

    virtual std::streamsize showmanyc();

    virtual std::streamsize xsgetn(char_type *ch, std::streamsize n);

    virtual pos_type seekpos(pos_type p,
                             std::ios_base::openmode m = std::ios_base::in | std::ios_base::out);

    virtual pos_type seekoff(off_type off, std::ios_base::seekdir way,
                             std::ios_base::openmode m = std::ios_base::in | std::ios_base::out);

    const SqliteData_libsqlite& m_data;
    
    const unsigned char *m_buf;
    const unsigned char *m_buf_end;
    const unsigned char *m_cur;

private:
    SqliteBlob_libsqlite(const SqliteBlob_libsqlite&);
    SqliteBlob_libsqlite& operator=(const SqliteBlob_libsqlite&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SqliteData implementation for libsqlite
class SqliteData_libsqlite : public SqliteData
{
public:
    SqliteData_libsqlite(SqliteResult_libsqlite& result, colnum_t colnum, bool locked);
    virtual ~SqliteData_libsqlite(void);


    virtual SqliteBlob_libsqlite*       getBlob(void) const;
    virtual double       getDouble(void) const;
    virtual int          getInt(void) const;
    virtual int64_t      getInt64(void) const;
    virtual const char*  getText(void) const;
    virtual const void*  getText16(void) const;

    virtual bool         isnull(void) const;
    virtual rowid_t      getCurrentRowID(void) const;

    virtual void refresh(void);

    virtual String getString(void) const;

    virtual daltype_t daltype(void) const;

protected:
    mutable SqliteResult_libsqlite& m_resultset;

    colnum_t   m_colnum;
    mutable std::auto_ptr<SqliteBlob_libsqlite> m_blobbuf;

};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SqliteColumnDesc implementation for libsqlite
class SqliteColumnDesc_libsqlite : public SqliteColumnDesc
{
public:
    SqliteColumnDesc_libsqlite(colnum_t i, SqliteResult_libsqlite &result);

    virtual ~SqliteColumnDesc_libsqlite(void)
    {}
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SQLite Resultset implementation for libsqlite
class SqliteResult_libsqlite : public SqliteResult
{
    friend class SqliteStmt_libsqlite;

public:
    SqliteResult_libsqlite(SqliteStmt_libsqlite& stmt);
    virtual ~SqliteResult_libsqlite(void);

    virtual void   first(void);
    virtual bool   next(void);
    virtual bool   eof(void) const;
    virtual void   close(void);

    virtual const SqliteVariant&     column(colnum_t num);
    virtual const SqliteVariant&     column(String name);

    // row methods
    virtual rowcount_t         rowCount(void) const;
    virtual rowcount_t         affectedRows(void) const;

    virtual Variant            lastInsertRowId(void);

    // column methods
    virtual size_t             columnCount(void) const;
    virtual colnum_t           columnID(String name) const;
    virtual String             columnName(colnum_t num) const;

    virtual rowid_t            getCurrentRowID(void) const;

    virtual SQLite3Drv*        drv(void) const;


    virtual const SqliteColumnDesc& describeColumn(colnum_t num) const;

    virtual const SqliteColumnDesc& describeColumn(String name) const;


    virtual SqliteDbc_libsqlite& getDbc(void) const;

    inline ::sqlite3_stmt* getHandle(void) const { return this->m_handle; }

    virtual SqliteDiag& appendDiagRec(const SqliteDiag &diag);

    // sqlite specific
    virtual void   prepare(String sql);
    virtual void   execute(StmtBase::ParamMap& params);


protected:
    typedef std::map<colnum_t, SqliteVariant*> VariantListT;

    virtual void         refreshMetadata(void);
    virtual size_t       paramCount(void) const;
    

    SqliteStmt_libsqlite    &m_stmt;
    mutable ::sqlite3_stmt  *m_handle;
    rowid_t                  m_current_tuple;
    int                      m_last_row_status;
    bool                     m_isopen;


    ///
    /// @brief Stores the type information of all columns in the resultset
    std::map<colnum_t, SqliteColumnDesc_libsqlite>     m_column_desc;

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
    std::vector<informave::utils::SmartPtr<SqliteVariant,
                                          informave::utils::RefCounted,
                                          informave::utils::AllowConversion> > m_allocated_accessors;


private:
    SqliteResult_libsqlite(const SqliteResult_libsqlite&);
    SqliteResult_libsqlite& operator=(const SqliteResult_libsqlite&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SQLite Statement implementation for libsqlite
class SqliteStmt_libsqlite : public SqliteStmt
{
    typedef std::vector<SqliteResult_libsqlite*> ResultsetVectorT;
    
    friend class SqliteResult_libsqlite;

public:
    SqliteStmt_libsqlite(SqliteDbc_libsqlite& conn);
    virtual ~SqliteStmt_libsqlite(void);

    virtual SqliteResult&        resultset(void);
    virtual const SqliteResult&  resultset(void) const;

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

    virtual SQLite3Drv* drv(void) const;

    virtual SqliteDbc_libsqlite& getDbc(void) const; // interface

    virtual SqliteDiag& appendDiagRec(const SqliteDiag &diag);

protected:
    SqliteResult_libsqlite* newResultset(void);

    SqliteDbc_libsqlite      &m_conn;
    ResultsetVectorT          m_resultsets;
    int                       m_currentResultset;
    //bool                      m_isPrepared;


private:
    SqliteStmt_libsqlite(const SqliteStmt_libsqlite&);
    SqliteStmt_libsqlite& operator=(const SqliteStmt_libsqlite&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SQLite Connection implementation for libsqlite
class SqliteDbc_libsqlite : public SqliteDbc
{
public:
    SqliteDbc_libsqlite(SqliteEnv_libsqlite& env);
    virtual ~SqliteDbc_libsqlite(void);

    virtual SqliteStmt_libsqlite*    newStatement(void);

    virtual void           connect(String database,
                                   String user = String(),
                                   String password = String());
    virtual void           connect(IDbc::Options& options);
    virtual void           disconnect(void);
    virtual String         driverName(void) const;
    virtual String         dbmsName(void) const;

    virtual std::string    getDbcEncoding(void) const;

    virtual ::sqlite3*     getHandle(void) const;

    virtual SQLite3Drv*    drv(void) const;

    virtual SqliteDiag& appendDiagRec(const SqliteDiag &diag);


protected:
    virtual void           setDbcEncoding(std::string encoding);

    SQLite3Drv           *m_lib; /* lib is stored in ENV */
    mutable ::sqlite3    *m_dbh;


private:
    SqliteDbc_libsqlite(const SqliteDbc_libsqlite&);
    SqliteDbc_libsqlite& operator=(const SqliteDbc_libsqlite&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SQLite Environment implementation for libsqlite
class SqliteEnv_libsqlite : public SqliteEnv
{
public:
    SqliteEnv_libsqlite(String lib = String());
    virtual ~SqliteEnv_libsqlite(void);

    virtual SqliteDbc_libsqlite* newConnection(void);
    virtual SQLite3Drv* drv(void) const;

    virtual SqliteDiag& appendDiagRec(const SqliteDiag &diag);

protected:
    mutable std::auto_ptr<SQLite3Drv>    m_lib;
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
