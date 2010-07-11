//
// sqlite_engine.hh - SQLite Engine
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
/// @brief SQLite Engine
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_SQLITE_ENGINE_HH
#define INFORMAVE_DB_DAL_SQLITE_ENGINE_HH

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_objects.hh"
#include "dbwtl/util/smartptr.hh"

#if !defined(DBWTL_WITH_SQLITE)
#error "DBWTL was compiled without SQLite support!"
#endif

#include <cstdlib>


DAL_NAMESPACE_BEGIN



class SqliteBlob;
class SqliteData;
class SqliteResult;
class SqliteStmt;
class SqliteDbc;
class SqliteEnv;
class SqliteVariant;
class SqliteEngineState;
class SqliteTable;
class SqliteDatatype;

class SqliteColumnDesc;

struct sqlite;




//------------------------------------------------------------------------------
///
/// @brief SQLite BLOB
class DBWTL_EXPORT SqliteBlob : public IBlob
{
    // no sqlite specific extensions
};





//------------------------------------------------------------------------------
///
/// @internal
/// @details 
/// Internal representation of the SQLite data(types).
///
/// @todo How to deal with UTF-8 strings?
class SqliteData
{
public:
    SqliteData(void);
    virtual ~SqliteData(void);

    virtual SqliteBlob&  getBlob(void) const = 0;
    virtual double       getDouble(void) const = 0;
    virtual int          getInt(void) const = 0;
    virtual int64_t      getInt64(void) const = 0;
    virtual const char*  getText(void) const = 0;
    virtual const void*  getText16(void) const = 0;
 
    virtual rowid_t getCurrentRowID(void) const = 0;

    virtual i18n::UString getString(void) const = 0;

    virtual bool isnull(void) const = 0;

    virtual daltype_t daltype(void) const = 0;
};




//------------------------------------------------------------------------------
///
/// @brief SQLite Variant
class DBWTL_EXPORT SqliteVariant : public Variant
{
public:
    SqliteVariant(SqliteData* data);

    virtual ~SqliteVariant(void);

    virtual SqliteBlob& asBlob(void) const;

private:
	SqliteVariant(const SqliteVariant& ref);
    SqliteVariant(void);
    SqliteVariant& operator=(const SqliteVariant&);
};





//------------------------------------------------------------------------------
///
/// @brief SQLite Table object
class DBWTL_EXPORT SqliteTable : public ITable
{
public:
    typedef util::SmartPtr<SqliteTable,
                           util::RefCounted,
                           util::AllowConversion> ptr;
    
    SqliteTable(i18n::UString dbname, SqliteResult& src);
    
    virtual ~SqliteTable(void);


    virtual const ITable::value_type& getName(void) const;
    virtual const ITable::value_type& getCatalog(void) const;
    virtual const ITable::value_type& getSchema(void) const;
    virtual const ITable::value_type& getComment(void) const;
    virtual const ITable::value_type& getDDL(void) const;
    virtual const ITable::value_type& isSystemObject(void) const;


protected:
    Variant  m_name;
    Variant  m_catalog;
    Variant  m_schema;
    Variant  m_comment;
    Variant  m_ddl;
    Variant  m_systemobject;

/*
    i18n::UString   m_tablename;
    i18n::UString   m_schemaname;
    i18n::UString   m_catalogname;
    i18n::UString   m_typename;
    bool            m_notnull;
    bool            m_systemflag;
    int             m_typelen;
*/
};



class DBWTL_EXPORT SqliteColumnDesc : public ColumnDescBase
{
public:
    friend class SqliteResult;

    SqliteColumnDesc(void) : ColumnDescBase()
    {}

    virtual ~SqliteColumnDesc(void)
    {}

};



//------------------------------------------------------------------------------
///
/// @brief SQLite Datatype
class DBWTL_EXPORT SqliteDatatype : public IDatatype
{
public:
    friend class SqliteDbc;

    typedef util::SmartPtr<SqliteDatatype,
                           util::RefCounted,
                           util::AllowConversion> ptr;

    SqliteDatatype(SqliteDbc& dbc);

    virtual ~SqliteDatatype(void);

    virtual i18n::UString        name(void) const;
    virtual daltype_t            daltype(void) const;


protected:
    i18n::UString     m_name;
    daltype_t         m_daltype;
};






//------------------------------------------------------------------------------
///
/// @brief SQLite Resultset
class DBWTL_EXPORT SqliteResult : public ResultBase
{
public:
    typedef std::auto_ptr<SqliteResult> ptr;

    typedef size_t                      bookmark_type;
    typedef SqliteVariant               value_type;
    typedef std::vector<value_type*>    row_type; /// @todo required?

    SqliteResult(void) : ResultBase()
    {}

    virtual const value_type&     column(colnum_t num) = 0;
    //virtual SqliteVariant&     field(colnum_t num) = 0;
    virtual const value_type&     column(i18n::UString name) = 0;
    //virtual SqliteVariant&     field(i18n::UString name) = 0;


    virtual const SqliteColumnDesc& describeColumn(colnum_t num) const = 0;

    virtual const SqliteColumnDesc& describeColumn(i18n::UString name) const = 0;
   
};





//------------------------------------------------------------------------------
///
///  @brief SQLite Statement
class DBWTL_EXPORT SqliteStmt : public StmtBase
{
public:
    typedef std::auto_ptr<SqliteStmt> ptr;

    SqliteStmt(void) : StmtBase()
    {}

    virtual SqliteResult&        resultset(void) = 0;
    virtual const SqliteResult&  resultset(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief SQLite Connection
class DBWTL_EXPORT SqliteDbc : public DbcBase
{
public:
    typedef std::auto_ptr<SqliteDbc> ptr;

    virtual SqliteStmt*    newStatement(void) = 0;

    virtual TableList      getTables(const ITableFilter& = EmptyTableFilter());

    virtual DatatypeList   getDatatypes(const IDatatypeFilter& = EmptyDatatypeFilter());

    virtual void           beginTrans(IDbc::trx_mode mode,
                                      IDbc::access_mode access,
                                      i18n::UString name = i18n::UString());

    virtual void           commit(void);
 
    virtual void           savepoint(i18n::UString name);

    virtual void           rollback(i18n::UString name = i18n::UString());

    virtual void           directCmd(i18n::UString cmd);
};





//------------------------------------------------------------------------------
///
/// @brief SQLite Environment
class DBWTL_EXPORT SqliteEnv : public IEnv
{
public:
    typedef std::auto_ptr<SqliteEnv> ptr;

    SqliteEnv(void);
    virtual ~SqliteEnv(void);
    
    virtual SqliteDbc* newConnection(void) = 0;
};






//------------------------------------------------------------------------------
///
/// @brief SQLite Engine State
class DBWTL_EXPORT SqliteEngineState : public BaseEngineState
{
public:
    SqliteEngineState(void);

    virtual ~SqliteEngineState(void);


    inline operator EngineState(void) const { return EngineState(*this); }


    /** @brief Clones the current State object */
    virtual SqliteEngineState* clone(void) const;


    /** @brief Set the driver-dependend status code */
    virtual void setDrvCode(int code, i18n::UString name);


    /** @brief Dumps the State content into a string */
    virtual i18n::UString dump(void) const;


protected:
    int              m_drvcode;
    i18n::UString    m_drvcode_name;
};





//------------------------------------------------------------------------------
///
/// @brief Main SQLite interface class 
struct sqlite
{
	typedef SqliteDbc          DBC;
	typedef SqliteResult       RESULT;
	typedef SqliteStmt         STMT;
	typedef SqliteEnv          ENV;
	typedef SqliteVariant      VALUE;
	typedef SqliteEngineState  STATE;
	typedef SqliteTable        TABLE;
	typedef SqliteColumnDesc   COLUMNDESC;

	static inline const STATE& engine_state(dalstate_t& state)
	{
        const IEngineState* es = state.getImpl();
        return dynamic_cast<const STATE&>(*es);
	}

    ///
    /// @brief Create a new Environment
    ///
    /// Current supported drivers are:
    ///  - libsqlite
    ///
    DBWTL_EXPORT static ENV* createEnv(i18n::UString driver);
};





//------------------------------------------------------------------------------
///
/// @brief Variant storage accessor for SqliteData 
template<>
class storage_accessor<SqliteData*> : public BaseVariantImplFor<sa_base<SqliteData*> >
{
public:
    DAL_VARIANT_ACCESSOR;

    virtual ~storage_accessor(void) { }

    virtual int asInt() const
        { return this->getValue()->getInt(); }


    virtual SqliteBlob& asBlob(void) const
        {
            return this->getValue()->getBlob();
        }


    virtual i18n::UString asStr() const
        {
            return this->getValue()->getString();
        }


    virtual i18n::UString asStr(std::locale loc) const
        {
            throw std::runtime_error("not implemented"); /// @bug fixme
            ///return this->getValue()->getString();
        }


    virtual bool isnull() const
        {
            return this->getValue()->isnull();
        }


    virtual daltype_t datatype() const
        {
            return this->getValue()->daltype();
        }
};





DAL_NAMESPACE_END







DB_NAMESPACE_BEGIN



//------------------------------------------------------------------------------
///
/// 
template<typename tag>
struct db_traits<dal::sqlite, tag>
{
    typedef Environment<dal::sqlite, tag>      environment_type;
    typedef Connection<dal::sqlite, tag>       connection_type;
    typedef Statement<dal::sqlite, tag>        statement_type;
    typedef Result<dal::sqlite, tag>           resultset_type;
    typedef CachedResult<dal::sqlite, tag>     cached_resultset_type;
    typedef dal::sqlite::VALUE                 value_type;

    typedef dal::sqlite::RESULT                dal_resultset_type;
    typedef dal::sqlite::STMT                  dal_stmt_type;
    typedef dal::sqlite::ENV                   dal_env_type;
    typedef dal::sqlite::DBC                   dal_dbc_type;

    typedef dal::sqlite::COLUMNDESC            dal_columndesc_type;
    
    enum { DB_SYSTEM_ID = 0 };

};

struct sqlite_v4 { };



class SqliteConnection;
/*
template<>
struct db_traits<DAL::Sqlite, sqlite_v4>
{
    typedef Environment<DAL::Sqlite, sqlite_v4>      environment_type;
	typedef SqliteConnection  connection_type;
    typedef int                                statement_type;
    typedef Result<DAL::Sqlite, sqlite_v4>           resultset_type;
    typedef CachedResult<DAL::Sqlite, sqlite_v4>     cached_resultset_type;
    typedef int                                variant_type;

    typedef DAL::Sqlite::RESULT                dal_resultset_type;

    enum { DB_SYSTEM_ID = 0 };
};


class SqliteConnection : public Connection<DAL::Sqlite, sqlite_v4, DAL::SqliteDbc>
{ };
*/



DB_NAMESPACE_END


#endif
