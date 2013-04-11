//
// sdi_libsdi.hh - SDI Backend libsdi
//
// Copyright (C)         informave.org
//   2012,               Daniel Vogelbacher <daniel@vogelbacher.name>
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
/// @brief SDI Backend libsdi
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_SDI_LIBSDI_HH
#define INFORMAVE_DB_DAL_SDI_LIBSDI_HH

#include "dbwtl/dal/engines/sdi_engine.hh"
#include "driver_libsdi.hh"

#include "../sqlproxy/sqlcursor.hh"
#include "../sqlproxy/rsproxy.hh"

//#include "../sqlproxy/sqlproxy.hh"

DAL_NAMESPACE_BEGIN

class SDIDataProvider_libsdi;
class SDIResult_libsdi;
class SDIStmt_libsdi;
class SDIDbc_libsdi;
class SDIEnv_libsdi;
class SDIData_libsdi;
class SDIDiag_libsdi;



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SDIDiag implementation for libsdi
class SDIDiag_libsdi : public SDIDiag
{
public:
    SDIDiag_libsdi(dalstate_t state,
               CodePosInfo pos,
               Variant what,
                   String description,
                   int sdi_code,
                   int sdi_excode);

    SDIDiag_libsdi(const SDIDiag_libsdi& ref);
    virtual ~SDIDiag_libsdi(void);
    
    virtual String str(void) const;
    virtual SDIDiag_libsdi* clone(void) const;    

protected:
    Variant m_sdi_code;
    Variant m_sdi_excode;
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SDIBlob implementation for libsdi
class SDIBlob_libsdi : public SDIBlob
{
public:
    SDIBlob_libsdi(const SDIData_libsdi& data);

    virtual ~SDIBlob_libsdi(void);

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

    const SDIData_libsdi& m_data;
    
    const unsigned char *m_buf;
    const unsigned char *m_buf_end;
    const unsigned char *m_cur;

private:
    SDIBlob_libsdi(const SDIBlob_libsdi&);
    SDIBlob_libsdi& operator=(const SDIBlob_libsdi&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SDIData implementation for libsdi
class SDIData_libsdi : public SDIData
{
private:
    SDIData_libsdi(const SDIData_libsdi&);
    SDIData_libsdi& operator=(const SDIData_libsdi&);
public:
    SDIData_libsdi(SDIDataProvider_libsdi& result, colnum_t colnum, bool locked);
    virtual ~SDIData_libsdi(void);



    virtual bool         isnull(void) const;
    virtual rowid_t      getCurrentRowID(void) const;


    virtual sditype_t sdi_type(void) const;

    virtual SDIBlob_libsdi*  getBlob(void) const;
    virtual UnicodeStreamBuf*  getMemo(void) const;


    virtual signed char getChar(void) const;
    virtual unsigned char getUChar(void) const;
    
    virtual signed short getSmallint(void) const;
    virtual unsigned short getUSmallint(void) const;
    
    virtual signed int getInt(void) const;
    virtual unsigned int getUInt(void) const;
    
    virtual signed long long getBigint(void) const;
    virtual unsigned long long getUBigint(void) const;

    virtual TNumeric getNumeric(void) const;
    virtual bool getBool(void) const;
    virtual String getString(void) const;
    virtual TDate getDate(void) const;
    virtual TTime getTime(void) const;
    virtual TTimestamp getTimestamp(void) const;
    virtual float getFloat(void) const;
    virtual double getReal(void) const;

    virtual void refresh(void);


    virtual daltype_t daltype(void) const;

    colnum_t colnum(void) const;

protected:
    SDIDataProvider_libsdi& m_resultset;

    colnum_t   m_colnum;
    mutable std::auto_ptr<SDIBlob_libsdi> m_blobbuf;
    //mutable std::auto_ptr<SDIMemo> m_memobuf;
    //mutable std::wstring m_memobuf_data;
    mutable std::wstringstream m_memo_stream;
    mutable void *m_buf;

};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SDIColumnDesc implementation for libsdi
class SDIColumnDesc_libsdi : public SDIColumnDesc
{
public:
    SDIColumnDesc_libsdi(colnum_t i, SDIDataProvider_libsdi &result);

    virtual ~SDIColumnDesc_libsdi(void)
    {}
};



//------------------------------------------------------------------------------
///
/// @internal
/// @brief SDI Resultset implementation for libsdi
class SDIDataProvider_libsdi : public SDIDataProvider
{
    friend class SDIStmt_libsdi;

public:

    virtual void open(open_mode mode,
                      const String &table,
	              const String &schema = String(),
		      const String &database = String());


    virtual void openObjects(void);



    SDIDataProvider_libsdi(SDIStmt_libsdi& stmt);
    virtual ~SDIDataProvider_libsdi(void);

    virtual void   first(void);
    virtual bool   next(void);
    virtual bool   eof(void) const;
    virtual void   close(void);

    virtual const SDIVariant&     column(colnum_t num);
    virtual const SDIVariant&     column(String name);

    // row methods
    virtual rowcount_t         rowCount(void) const;
    virtual rowcount_t         affectedRows(void) const;

    virtual Variant            lastInsertRowId(void);

    // column methods
    virtual size_t             columnCount(void) const;
    virtual colnum_t           columnID(String name) const;
    virtual String             columnName(colnum_t num) const;

    virtual rowid_t            getCurrentRowID(void) const;

    virtual SDIDrv*        drv(void) const;



    virtual const SDIColumnDesc& describeColumn(colnum_t num) const;

    virtual const SDIColumnDesc& describeColumn(String name) const;


    virtual SDIDbc_libsdi& getDbc(void) const;

    inline SDISTMT getHandle(void) const { return this->m_handle; }



    virtual bool   isBad(void) const;
		virtual bool isOpen(void) const;
    virtual bool      isPositioned(void) const;


protected:
    typedef std::map<colnum_t, SDIVariant*> VariantListT;

    virtual void         refreshMetadata(void);
//    virtual size_t       paramCount(void) const;
    

    SDIStmt_libsdi    &m_stmt;
    mutable SDISTMT  m_handle;
    rowid_t                  m_current_tuple;
    int                      m_last_row_status;
    bool                     m_isprepared, m_isopen, m_isbad;


    ///
    /// @brief Stores the type information of all columns in the resultset
    std::map<colnum_t, SDIColumnDesc_libsdi>     m_column_desc;

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
    std::vector<informave::utils::SmartPtr<SDIVariant,
                                          informave::utils::RefCounted,
                                          informave::utils::AllowConversion> > m_allocated_accessors;


private:
    SDIDataProvider_libsdi(const SDIDataProvider_libsdi&);
    SDIDataProvider_libsdi& operator=(const SDIDataProvider_libsdi&);
};




//------------------------------------------------------------------------------
///
/// @internal
/// @brief SDI Statement implementation for libsdi
class SDIStmt_libsdi : public SDIStmt
{
    typedef std::vector<SDIResult_libsdi*> ResultsetVectorT;
    typedef std::vector<SDIDataProvider_libsdi*> ProviderVectorT;
    


friend class SDIDataProvider_libsdi;

public:
	typedef std::auto_ptr<SDIStmt_libsdi> ptr;



    SDIStmt_libsdi(SDIDbc_libsdi& conn);
    virtual ~SDIStmt_libsdi(void);

    virtual SDIResult&        resultset(void);
    virtual const SDIResult&  resultset(void) const;

    virtual SDIDataProvider_libsdi*  newProvider(void);

     virtual SDIDataProvider* openObjectList(void);


    virtual void      prepare(String sql);
    virtual bool      isPrepared(void) const;
    virtual void      execute(void);
    virtual void      execDirect(String sql);
    //virtual void      execDirect(std::istream src) = 0;
    virtual void      close(void);

    virtual bool      nextResultset(void);
    virtual bool      moreResultsets(void) const;

    virtual size_t    paramCount(void) const;

    virtual rowid_t   affectedRows(void) const;

    virtual Variant   lastInsertRowId(void);

    virtual SDIDrv* drv(void) const;

    virtual SDIDbc_libsdi& getDbc(void) const; // interface

protected:
    SDIResult_libsdi* newResultset(void);

    SDIDbc_libsdi      &m_conn;
    ResultsetVectorT          m_resultsets;
    ProviderVectorT           m_providers;
    int                       m_currentResultset;
    bool                      m_isPrepared;

    //sqlproxy::Sqlproxy<SDIResult, SDIDataProvider_libsdi, SDIStmt_libsdi> m_proxy;
    //SDIResult_libsdi m_proxy;

private:
    SDIStmt_libsdi(const SDIStmt_libsdi&);
    SDIStmt_libsdi& operator=(const SDIStmt_libsdi&);
};





class SDIResult_libsdi : public sqlproxy::ResultsetProxy<SDIResult, sqlproxy::SqlCursor, SDIStmt>
{
public:
    typedef sqlproxy::ResultsetProxy<SDIResult, sqlproxy::SqlCursor, SDIStmt>   proxy_type;

    SDIResult_libsdi(SDIStmt_libsdi &stmt)
    : proxy_type(stmt),
      m_stmt(stmt)
    {
        this->setInternalResultset(new sqlproxy::SqlCursor(stmt));
    }

    virtual IDALDriver* drv() const
    {
        return this->m_stmt.drv();
    }

    SDIStmt_libsdi    &m_stmt;
};

/*
class SDIResult_libsdi : public sqlproxy::Sqlproxy<SDIResult, SDIDataProvider_libsdi, SDIStmt_libsdi>
{
public:
	typedef sqlproxy::Sqlproxy<SDIResult, SDIDataProvider_libsdi, SDIStmt_libsdi> proxy_type;

	SDIResult_libsdi(SDIStmt_libsdi& stmt) : proxy_type(stmt) {}
	virtual ~SDIResult_libsdi(void) {}
};
*/




//------------------------------------------------------------------------------
///
/// @internal
/// @brief SDI Connection implementation for libsdi
class SDIDbc_libsdi : public SDIDbc
{
public:
    SDIDbc_libsdi(SDIEnv_libsdi& env);
    virtual ~SDIDbc_libsdi(void);

    virtual SDIStmt_libsdi*    newStatement(void);

    virtual void           connect(String database,
                                   String user = String(),
                                   String password = String());
    virtual void           connect(IDbc::Options& options);
    virtual void           disconnect(void);
    virtual String         driverName(void) const;
    virtual String         dbmsName(void) const;

    virtual std::string    getDbcEncoding(void) const;

    virtual SDIDBC     getHandle(void) const;

    virtual SDIDrv*    drv(void) const;

TableList getTables(const ITableFilter& = EmptyTableFilter());


protected:
    virtual void           setDbcEncoding(std::string encoding);

    SDIEnv_libsdi    &m_env;
    SDIDrv           *m_lib; /* lib is stored in ENV */
    mutable SDIDBC    m_dbh;


private:
    SDIDbc_libsdi(const SDIDbc_libsdi&);
    SDIDbc_libsdi& operator=(const SDIDbc_libsdi&);
};






//------------------------------------------------------------------------------
///
/// @internal
/// @brief SDI Environment implementation for libsdi
class SDIEnv_libsdi : public SDIEnv
{
private:
    SDIEnv_libsdi(const SDIEnv_libsdi&);
    SDIEnv_libsdi& operator=(const SDIEnv_libsdi&);

public:
    SDIEnv_libsdi(String lib = String());
    virtual ~SDIEnv_libsdi(void);

    virtual SDIDbc_libsdi* newConnection(void);
    virtual SDIDrv* drv(void) const;

    virtual SDIENV getHandle(void);

protected:
    SDIENV                           m_env;
    mutable std::auto_ptr<SDIDrv>    m_lib;
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
