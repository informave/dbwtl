//
// firebird_libfbclient.hh - Firebird Backend libfirebird
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
/// @brief Firebird Backend libfirebird
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_FIREBIRD_LIBFBCLIENT_HH
#define INFORMAVE_DB_DAL_FIREBIRD_LIBFBCLIENT_HH

#include "dbwtl/dal/engines/firebird_engine.hh"
#include "driver_libfbclient.hh"
#include "../../devutils.hh"

DAL_NAMESPACE_BEGIN

class FirebirdResult_libfbclient;
class FirebirdStmt_libfbclient;
class FirebirdDbc_libfbclient;
class FirebirdEnv_libfbclient;
class FirebirdData_libfbclient;
class FirebirdDiag_libfbclient;



//------------------------------------------------------------------------------
///
/// @internal
/// @brief FirebirdDiag implementation for libfirebird
class FirebirdDiag_libfbclient : public FirebirdDiag
{
public:
    FirebirdDiag_libfbclient(dalstate_t state,
                             ISC_STATUS gdscode,
                             CodePosInfo pos,
                             Variant what,
                             String description);

    FirebirdDiag_libfbclient(dalstate_t state,
                             CodePosInfo pos,
                             firebird::STATES::engine_states_t sqlstate,
                             Variant what,
                             String description);

    FirebirdDiag_libfbclient(const FirebirdDiag_libfbclient& ref);
    virtual ~FirebirdDiag_libfbclient(void);
    
    virtual String str(void) const;
    virtual FirebirdDiag_libfbclient* clone(void) const;    

protected:
    ISC_STATUS m_gdscode;
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief FirebirdBlob implementation for libfirebird
class FirebirdBlob_libfbclient : public FirebirdBlob
{
public:
    FirebirdBlob_libfbclient(const FirebirdData_libfbclient& data);

    virtual ~FirebirdBlob_libfbclient(void);

    virtual bool isNull(void) const;

    virtual FBClientDrv* drv(void) const;

    virtual FirebirdDbc_libfbclient& getDbc(void) const;

protected:
    virtual int_type underflow();

    const FirebirdData_libfbclient& m_data;

    char_type           m_buf[DAL_STREAMBUF_BUFSIZE + DAL_STREAMBUF_PUTBACK];
    ISC_STATUS          m_sv[20];
    const std::size_t   m_putback;
    isc_blob_handle     m_handle;

private:
    FirebirdBlob_libfbclient(const FirebirdBlob_libfbclient&);
    FirebirdBlob_libfbclient& operator=(const FirebirdBlob_libfbclient&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief FirebirdData implementation for libfirebird
class FirebirdData_libfbclient : public FirebirdData
{
    friend class FirebirdBlob_libfbclient;
public:
    FirebirdData_libfbclient(FirebirdResult_libfbclient& result, colnum_t colnum, bool locked);
    virtual ~FirebirdData_libfbclient(void);

    virtual String       getText(void) const;
    virtual TDate        getDate(void) const;
    virtual short        getSmallint(void) const;
    virtual int          getLong(void) const;
    virtual double       getDouble(void) const;
    virtual int64_t      getInt64(void) const;
    virtual float        getFloat(void) const;
    virtual TTime        getTime(void) const;
    virtual TTimestamp   getTimestamp(void) const;
    virtual TNumeric     getNumeric(void) const;
    virtual TVarbinary   getVarbinary(void) const;

    virtual FirebirdBlob_libfbclient*       getBlob(void) const;
    virtual UnicodeStreamBuf* getMemo(void) const;


    virtual IVariantValue* do_deepcopy(const IVariantValue *owner) const;

    virtual bool         isnull(void) const;
    virtual rowid_t      getCurrentRowID(void) const;

    virtual void refresh(void);

    virtual daltype_t daltype(void) const;

protected:
    FirebirdResult_libfbclient& m_resultset;

    colnum_t   m_colnum;
    mutable std::auto_ptr<FirebirdBlob_libfbclient> m_blobbuf;
    mutable std::auto_ptr<std::wstringstream> m_memostream;
    XSQLVAR *m_sqlvar;

private:
    FirebirdData_libfbclient(const FirebirdData_libfbclient&);
    FirebirdData_libfbclient& operator=(const FirebirdData_libfbclient&);
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief FirebirdColumnDesc implementation for libfirebird
class FirebirdColumnDesc_libfbclient : public FirebirdColumnDesc
{
public:
    FirebirdColumnDesc_libfbclient(colnum_t i, FirebirdResult_libfbclient &result);

    virtual ~FirebirdColumnDesc_libfbclient(void)
    {}
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief Firebird Resultset implementation for libfirebird
class FirebirdResult_libfbclient : public FirebirdResult
{
    friend class FirebirdStmt_libfbclient;
    friend class FirebirdColumnDesc_libfbclient;
    friend class FirebirdData_libfbclient;

public:
    FirebirdResult_libfbclient(FirebirdStmt_libfbclient& stmt);
    virtual ~FirebirdResult_libfbclient(void);

    virtual void   first(void);
    virtual bool   next(void);
    virtual bool   eof(void) const;
    virtual void   close(void);

    virtual const FirebirdVariant&     column(colnum_t num);
    virtual const FirebirdVariant&     column(String name);

    // row methods
    virtual rowcount_t         rowCount(void) const;
    virtual rowcount_t         affectedRows(void) const;

    virtual Variant            lastInsertRowId(void);

    // column methods
    virtual size_t             columnCount(void) const;
    virtual colnum_t           columnID(String name) const;
    virtual String             columnName(colnum_t num) const;

    virtual rowid_t            getCurrentRowID(void) const;

    virtual FBClientDrv*        drv(void) const;

    virtual short getStatementType(void);

    virtual const FirebirdColumnDesc& describeColumn(colnum_t num) const;

    virtual const FirebirdColumnDesc& describeColumn(String name) const;


    virtual FirebirdDbc_libfbclient& getDbc(void) const;

    inline ::isc_stmt_handle getHandle(void) const { return this->m_handle; }

    virtual ::isc_tr_handle* getTrxHandle(void);

    // firebird specific
    virtual void   prepare(String sql, Transaction trx);
    virtual void   execute(StmtBase::ParamMap& params);

    virtual FirebirdDiag& appendDiagRec(const FirebirdDiag &diag) const;

protected:
    void allocateVars(XSQLDA *sqlda);
    void freeVars(XSQLDA *sqlda);
    void fillBindBuffers(StmtBase::ParamMap& params);
    void fillBlob(XSQLVAR *var, Variant &data);


    typedef std::map<colnum_t, FirebirdVariant*> VariantListT;

    virtual void         refreshMetadata(void);
    virtual size_t       paramCount(void) const;
    

    FirebirdStmt_libfbclient    &m_stmt;
    mutable ::isc_stmt_handle  m_handle;
    mutable XSQLDA            *m_isqlda;
    mutable XSQLDA            *m_osqlda;
    String                   m_sql;
    rowid_t                  m_current_tuple;
    int                      m_last_row_status;
    bool                     m_isopen;


    ///
    /// @brief Stores the type information of all columns in the resultset
    std::map<colnum_t, FirebirdColumnDesc_libfbclient>     m_column_desc;

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
    std::vector<informave::utils::SmartPtr<FirebirdVariant,
                                          informave::utils::RefCounted,
                                          informave::utils::AllowConversion> > m_allocated_accessors;


private:
    FirebirdResult_libfbclient(const FirebirdResult_libfbclient&);
    FirebirdResult_libfbclient& operator=(const FirebirdResult_libfbclient&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief Firebird Statement implementation for libfirebird
class FirebirdStmt_libfbclient : public FirebirdStmt
{
    typedef std::vector<FirebirdResult_libfbclient*> ResultsetVectorT;
    typedef std::list<Transaction>                   TransactionListT;
    
    friend class FirebirdResult_libfbclient;

public:
    FirebirdStmt_libfbclient(FirebirdDbc_libfbclient& conn);
    virtual ~FirebirdStmt_libfbclient(void);

    virtual FirebirdResult&        resultset(void);
    virtual const FirebirdResult&  resultset(void) const;

    virtual void      prepare(String sql);
    virtual void      prepare(String sql, Transaction trx); // firebird specific
    virtual void      execute(void);
    virtual void      execDirect(String sql);
    virtual void      execDirect(String sql, Transaction trx); // firebird specific
    //virtual void      execDirect(std::istream src) = 0;
    virtual void      close(void);

    virtual bool      nextResultset(void);
    virtual bool      moreResultsets(void) const;

    virtual size_t    paramCount(void) const;

    virtual rowid_t   affectedRows(void) const;

    virtual Variant   lastInsertRowId(void);

    virtual FBClientDrv* drv(void) const;

    virtual FirebirdDbc_libfbclient& getDbc(void) const; // interface

    virtual Transaction getCurrentTrx(void) { return this->m_currentTrx; }

    virtual FirebirdDiag& appendDiagRec(const FirebirdDiag &diag) const;

protected:
    FirebirdResult_libfbclient* newResultset(void);

    virtual bool hasLocalTrx(void) const;
    virtual void relinquishLocalTrx(const Transaction& trx);

    FirebirdDbc_libfbclient      &m_conn;
    ResultsetVectorT          m_resultsets;
    int                       m_currentResultset;
    //TransactionListT          m_transactionsToCommit;
    Transaction               m_localTrx;
    Transaction               m_currentTrx;


private:
    FirebirdStmt_libfbclient(const FirebirdStmt_libfbclient&);
    FirebirdStmt_libfbclient& operator=(const FirebirdStmt_libfbclient&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief Firebird Connection implementation for libfirebird
class FirebirdDbc_libfbclient : public FirebirdDbc
{
public:
    typedef std::map<trxid_t, ::isc_tr_handle>   trx_map_type;

    FirebirdDbc_libfbclient(FirebirdEnv_libfbclient& env);
    virtual ~FirebirdDbc_libfbclient(void);

    virtual FirebirdStmt_libfbclient*    newStatement(void);

    virtual void           connect(String database,
                                   String user = String(),
                                   String password = String());
    virtual void           connect(IDbc::Options& options);
    virtual void           disconnect(void);
    virtual String         driverName(void) const;
    virtual String         dbmsName(void) const;

    virtual std::string    getDbcEncoding(void) const;

    virtual ::isc_db_handle*     getHandle(void) const;

    virtual FBClientDrv*    drv(void) const;

    virtual void           directCmd(String cmd);

    virtual void           beginTrans(trx_mode mode,
                                      access_mode access = trx_default,
                                      String name = String());
    virtual void           commit(void);
    virtual void           savepoint(String name);
    virtual void           rollback(String name = String());
    virtual void           commit(Transaction trx);
    virtual void           rollback(Transaction trx);


    virtual bool              hasActiveTrx(void);
    virtual Transaction       currentTrx(void);
    virtual Transaction       makeTrx(trx_mode mode,
                                      access_mode access = trx_default,
                                      String name = String());
    virtual ::isc_tr_handle*  getTrxHandle(const Transaction &trx);
    virtual unsigned short    getDialect(void) const;


    virtual FirebirdDiag& appendDiagRec(const FirebirdDiag &diag) const;

protected:
    virtual void           setDbcEncoding(std::string encoding);

    virtual ::isc_tr_handle           makeTrxInternal(trx_mode mode,
                                           access_mode access,
                                           String name);

    FBClientDrv               *m_lib; /* lib is stored in ENV */
    mutable ::isc_db_handle    m_dbh;
    Transaction                m_dbc_trx; // internal dbc transaction
    trx_map_type               m_trx_map;
    trxid_t                    m_trx_counter;
    unsigned char              m_dialect;


private:
    FirebirdDbc_libfbclient(const FirebirdDbc_libfbclient&);
    FirebirdDbc_libfbclient& operator=(const FirebirdDbc_libfbclient&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief Firebird Environment implementation for libfirebird
class FirebirdEnv_libfbclient : public FirebirdEnv
{
public:
    FirebirdEnv_libfbclient(String lib = String());
    virtual ~FirebirdEnv_libfbclient(void);

    virtual FirebirdDbc_libfbclient* newConnection(void);
    virtual FBClientDrv* drv(void) const;

    virtual FirebirdDiag& appendDiagRec(const FirebirdDiag &diag) const;

protected:
    mutable std::auto_ptr<FBClientDrv>    m_lib;
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
