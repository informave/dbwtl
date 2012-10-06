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
class SqliteDiag;
class SqliteDatatype;
class SqliteColumnDesc;


struct sqlite;
struct sqlite_sqlstates;



//------------------------------------------------------------------------------
///
/// @brief SQLite SQLSTATEs
struct sqlite_sqlstates : public basic_states
{
    typedef enum 
    {
        DAL_SQLITE_SQLSTATE_08000 = DAL_SQLSTATE_08000, // connection exception
        DAL_SQLITE_SQLSTATE_08001 = DAL_SQLSTATE_08001, // SQL-client unable to establish SQL-connection
        DAL_SQLITE_SQLSTATE_0A000 = DAL_SQLSTATE_0A000, // feature not supported
        DAL_SQLITE_SQLSTATE_22000 = DAL_SQLSTATE_22000, // data exception
        DAL_SQLITE_SQLSTATE_23000 = DAL_SQLSTATE_23000, // integrity constraint violation
        DAL_SQLITE_SQLSTATE_25000 = DAL_SQLSTATE_25000, // invalid transaction state
        DAL_SQLITE_SQLSTATE_25001 = DAL_SQLSTATE_25001, // active SQL-transaction
        DAL_SQLITE_SQLSTATE_25006 = DAL_SQLSTATE_25006, // read-only SQL-transaction
        DAL_SQLITE_SQLSTATE_28000 = DAL_SQLSTATE_28000, // invalid authorization specification
        DAL_SQLITE_SQLSTATE_42000 = DAL_SQLSTATE_42000  // syntax error or access rule violation

        //DAL_SQLITE_SQLSTATE_XY000 = DAL_SQLSTATE_LAST + 1
    } engine_states_t;
    

// connection exception
    typedef sqlstate<DAL_SQLITE_SQLSTATE_08000>                         SQLSTATE_08000;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_08000>                         SQLSTATE_connection_exception;

// SQL-client unable to establish SQL-connection
    typedef sqlstate<DAL_SQLITE_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_08001;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_sql_client_unable_to_establish_sql_connection;

// feature not supported
    typedef sqlstate<DAL_SQLITE_SQLSTATE_0A000>                         SQLSTATE_0A000;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_0A000>                         SQLSTATE_feature_not_supported;

// data exception
    typedef sqlstate<DAL_SQLITE_SQLSTATE_22000>                         SQLSTATE_22000;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_22000>                         SQLSTATE_data_exception;

// integrity constraint violation
    typedef sqlstate<DAL_SQLITE_SQLSTATE_23000>                         SQLSTATE_23000;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_23000>                         SQLSTATE_integrity_constraint_violation;

// invalid transaction state
    typedef sqlstate<DAL_SQLITE_SQLSTATE_25000>                         SQLSTATE_25000;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_25000>                         SQLSTATE_invalid_transaction_state;

// active SQL-transaction
    typedef sqlstate<DAL_SQLITE_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_25001;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_active_sql_transaction;

// read-only SQL-transaction
    typedef sqlstate<DAL_SQLITE_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_25006;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_read_only_sql_transaction;

// invalid authorization specification
    typedef sqlstate<DAL_SQLITE_SQLSTATE_28000>                         SQLSTATE_28000;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_28000>                         SQLSTATE_invalid_authorization_specification;

// syntax error or access rule violation
    typedef sqlstate<DAL_SQLITE_SQLSTATE_42000>                         SQLSTATE_42000;
    typedef sqlstate<DAL_SQLITE_SQLSTATE_42000>                         SQLSTATE_syntax_error_or_access_rule_violation;


//     typedef sqlstate<DAL_SQLITE_SQLSTATE_XY000>                         SQLSTATE_XY000;
};




//------------------------------------------------------------------------------
///
/// @brief SQLite Diagnostic
class DBWTL_EXPORT SqliteDiag : public DiagBase
{
public:
    virtual ~SqliteDiag(void)
    {}


    virtual SqliteDiag* clone(void) const = 0;
    virtual void raiseException(void) const;


protected:
    SqliteDiag(dalstate_t state,
               CodePosInfo pos,
               Variant what,
               String description);
    
    SqliteDiag(const SqliteDiag& ref);

    sqlite_sqlstates::engine_states_t m_sqlstate_id;
};



typedef DiagController<SqliteDiag> SqliteDiagController;



//------------------------------------------------------------------------------
///
/// @brief SQLite BLOB
class DBWTL_EXPORT SqliteBlob : public IBlobBuffer
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
class SqliteData : public custom_deepcopy
{
public:
    SqliteData(void);
    virtual ~SqliteData(void);

    virtual SqliteBlob*  getBlob(void) const = 0;
    virtual double       getDouble(void) const = 0;
    virtual int          getInt(void) const = 0;
    virtual int64_t      getInt64(void) const = 0;
    virtual const char*  getText(void) const = 0;
    virtual const void*  getText16(void) const = 0;
 
    virtual rowid_t getCurrentRowID(void) const = 0;

    virtual String getString(void) const = 0;

    virtual bool isnull(void) const = 0;

    virtual void refresh(void) = 0;

    virtual daltype_t daltype(void) const = 0;

    virtual IVariantValue* do_deepcopy(const IVariantValue* owner) const;
};



namespace sqlite_ext
{

class DBWTL_EXPORT Text : public TCustomType
{
public:
    Text(void) { }
    Text(const SqliteVariant&) { }
    Text& operator=(const SqliteVariant&) { return *this; }


    virtual daltype_t  datatype(void) const { return DAL_TYPE_CUSTOM; }
    virtual String asStr(std::locale loc) const { return L"foo"; }
    //virtual std::wstring asWideStr() const { return L"foo"; }
};

}






//------------------------------------------------------------------------------
///
/// @brief SQLite Variant
class DBWTL_EXPORT SqliteVariant : public EngineVariant
{
public:
    SqliteVariant(SqliteData* data);

    virtual ~SqliteVariant(void);

    virtual void refresh(void);

    virtual sqlite_ext::Text asText(void) const
    { return sqlite_ext::Text(); }


private:
    SqliteData *m_data;

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
    typedef utils::SmartPtr<SqliteTable,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    SqliteTable(String dbname, SqliteResult& src);
    
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
};


//------------------------------------------------------------------------------
///
/// @brief SQLite Index object
class DBWTL_EXPORT SqliteIndex : public IIndex
{
public:
    typedef utils::SmartPtr<SqliteIndex,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    SqliteIndex(String dbname, SqliteResult& src);
    
    virtual ~SqliteIndex(void);


    virtual const ITable::value_type& getName(void) const;
    virtual const ITable::value_type& getTable(void) const;
    virtual const ITable::value_type& getCatalog(void) const;
    virtual const ITable::value_type& getSchema(void) const;
    virtual const ITable::value_type& getComment(void) const;
    virtual const ITable::value_type& getDDL(void) const;
    virtual const ITable::value_type& isSystemObject(void) const;


protected:
    Variant  m_name;
    Variant  m_table;
    Variant  m_catalog;
    Variant  m_schema;
    Variant  m_comment;
    Variant  m_ddl;
    Variant  m_systemobject;
};



//------------------------------------------------------------------------------
///
/// @brief SQLite View object
class DBWTL_EXPORT SqliteView : public IView
{
public:
    typedef utils::SmartPtr<SqliteView,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    SqliteView(String dbname, SqliteResult& src);
    
    virtual ~SqliteView(void);


    virtual const IView::value_type& getName(void) const;
    virtual const IView::value_type& getCatalog(void) const;
    virtual const IView::value_type& getSchema(void) const;
    virtual const IView::value_type& getComment(void) const;
    virtual const IView::value_type& getDDL(void) const;
    virtual const IView::value_type& isSystemObject(void) const;


protected:
    Variant  m_name;
    Variant  m_catalog;
    Variant  m_schema;
    Variant  m_comment;
    Variant  m_ddl;
    Variant  m_systemobject;
};



//------------------------------------------------------------------------------
///
/// @brief SQLite Catalog object
class DBWTL_EXPORT SqliteCatalog : public ICatalog
{
public:
    typedef utils::SmartPtr<SqliteCatalog,
                           utils::RefCounted,
                           utils::AllowConversion> ptr;
    
    SqliteCatalog(String dbname);
    
    virtual ~SqliteCatalog(void);


    virtual const ICatalog::value_type& getName(void) const;
    virtual const ICatalog::value_type& getComment(void) const;
    virtual const ICatalog::value_type& getDDL(void) const;
    virtual const ICatalog::value_type& isSystemObject(void) const;


protected:
    Variant  m_name;
    Variant  m_comment;
    Variant  m_ddl;
    Variant  m_systemobject;
};




//------------------------------------------------------------------------------
///
/// @brief SQLite Column Descriptor
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
class DBWTL_EXPORT SqliteDatatype : public DatatypeBase
{
public:
    friend class SqliteDbc;

    typedef utils::SmartPtr<SqliteDatatype,
        utils::RefCounted,
        utils::AllowConversion> ptr;

    SqliteDatatype(void);

    virtual ~SqliteDatatype(void);
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

    SqliteResult(SqliteDiagController &dc) : ResultBase(),
        m_diag(dc)
        {}

    virtual const value_type&     column(colnum_t num) = 0;
    //virtual SqliteVariant&     field(colnum_t num) = 0;
    virtual const value_type&     column(String name) = 0;
    //virtual SqliteVariant&     field(std::wstring name) = 0;


    virtual const SqliteColumnDesc& describeColumn(colnum_t num) const = 0;

    virtual const SqliteColumnDesc& describeColumn(String name) const = 0;
  
protected:
    SqliteDiagController &m_diag;
};





//------------------------------------------------------------------------------
///
///  @brief SQLite Statement
class DBWTL_EXPORT SqliteStmt : public StmtBase
{
public:
    typedef std::auto_ptr<SqliteStmt> ptr;

    SqliteStmt(void)
        : StmtBase(),
        m_diag()
    {}

    virtual SqliteResult&        resultset(void) = 0;
    virtual const SqliteResult&  resultset(void) const = 0;

    virtual bool                 diagAvail(void) const;
    virtual const SqliteDiag&   fetchDiag(void);

protected:
    SqliteDiagController m_diag;
};





//------------------------------------------------------------------------------
///
/// @brief SQLite Connection
class DBWTL_EXPORT SqliteDbc : public DbcBase
{
public:
    typedef std::auto_ptr<SqliteDbc> ptr;

    SqliteDbc(void)
        : DbcBase(),
        m_diag()
        {}

    virtual SqliteStmt*    newStatement(void) = 0;

    virtual TableList      getTables(const ITableFilter& = EmptyTableFilter());
    virtual ViewList       getViews(const IViewFilter& = EmptyViewFilter());
    virtual DatatypeList   getDatatypes(const IDatatypeFilter& = EmptyDatatypeFilter());
    virtual SchemaList     getSchemas(const ISchemaFilter& = EmptySchemaFilter());
    virtual CatalogList    getCatalogs(const ICatalogFilter& = EmptyCatalogFilter());
    virtual ProcedureList  getProcedures(const IProcedureFilter& = EmptyProcedureFilter());
    virtual IndexList      getIndices(const IIndexFilter& = EmptyIndexFilter());

    virtual void           beginTrans(trx_mode mode,
                                      access_mode access = trx_default,
                                      String name = String());

    virtual void           commit(void);
 
    virtual void           savepoint(String name);

    virtual void           rollback(String name = String());

    virtual void           directCmd(String cmd);

    virtual void           commit(Transaction trx);
    virtual void           rollback(Transaction trx);

    virtual bool                 diagAvail(void) const;
    virtual const SqliteDiag&    fetchDiag(void);


protected:
    SqliteDiagController m_diag;
};





//------------------------------------------------------------------------------
///
/// @brief SQLite Environment
class DBWTL_EXPORT SqliteEnv : public EnvBase
{
public:
    typedef std::auto_ptr<SqliteEnv> ptr;

    SqliteEnv(void);
    virtual ~SqliteEnv(void);
    
    virtual SqliteDbc* newConnection(void) = 0;


    virtual bool                 diagAvail(void) const;
    virtual const SqliteDiag&    fetchDiag(void);

    virtual dal_engine getEngineType(void) const { return DAL_ENGINE_SQLITE; }
        

protected:
    SqliteDiagController m_diag;
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
    typedef SqliteDiag         DIAG;
    typedef sqlite_sqlstates   STATES;
    typedef SqliteTable        TABLE;
    typedef SqliteColumnDesc   COLUMNDESC;



    //typedef STATES::engine_states_t 

    DBWTL_EXPORT static const char* sqlstate2string(STATES::engine_states_t id);


    ///
    /// @brief Create a new Environment
    ///
    /// Current supported drivers are:
    ///  - libsqlite
    ///
    DBWTL_EXPORT static ENV* createEnv(String driver);
};


DAL_NAMESPACE_END



DB_NAMESPACE_BEGIN




template<>
struct value_traits<SqliteData*>
{
    typedef raw_pointer<SqliteData*>  stored_type;
    typedef SqliteData*    value_type;
    typedef var_info<SqliteData*>         info_type;
};



template<>
struct value_traits<const SqliteData*>
{
/*
    typedef raw_pointer<const SqliteData*>  stored_type;
    typedef const SqliteData*    value_type;
    typedef var_info<const SqliteData*>         info_type;
*/
};



//------------------------------------------------------------------------------
///
/// @brief Variant storage accessor for SqliteData 
template<>
struct sv_accessor<SqliteData*> : public virtual sa_base<SqliteData*>,
                                       public supports<signed int>,
                                       public supports<bool>,
                                       public supports<BlobStream>,
                                       public supports<TDate>,
                                       public supports<TTime>,
                                       public supports<TTimestamp>,
                                       public supports<TNumeric>,
                                       public supports<String>
{
    virtual signed int cast(signed int*, std::locale loc) const;
    virtual bool cast(bool*, std::locale loc) const;
    virtual BlobStream cast(BlobStream*, std::locale loc) const;
    virtual TDate cast(TDate*, std::locale loc) const;
    virtual TTime cast(TTime*, std::locale loc) const;
    virtual TTimestamp cast(TTimestamp*, std::locale loc) const;
    virtual TNumeric cast(TNumeric*, std::locale loc) const;
    virtual String cast(String*, std::locale loc) const;


    virtual bool valueNullCheck(void) const;
    //virtual bool isNull() const;

    virtual daltype_t datatype() const;
};


/*
template<>
class read_accessor<sqlite_ext::Text> : public default_accessor<sqlite_ext::Text>
{
    DAL_VARIANT_ACCESSOR;

    virtual ~read_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_CUSTOM; }    
};
*/


/*
template<>
struct assign_value<SqliteData*>
{
    void operator()(SqliteData *value, const Variant &src)
    {
        // *value = src;
        //value->assign(src);
    }
};
*/


/*
template<>
struct DeepCopyPtr<SqliteData*>
{
    Variant operator()(const SqliteData* ptr)
    {
    }
};
*/




struct sqlite_datatypes : public basic_datatypes
{
/*
    virtual ~sqlite_datatypes(void)
    {}
*/
	typedef sqlite_ext::Text Text;
};


template<typename tag>
class SqliteEnvironment : public Environment<sqlite, tag>
{
public:

    SqliteEnvironment(String str)
        : Environment<sqlite, tag>(str)
    {}

    virtual ~SqliteEnvironment(void)
    {}

    virtual void specialFunc(void)
    {
    	this->m_env;
    }

};


//------------------------------------------------------------------------------
///
/// 
template<typename tag>
struct db_traits<sqlite, tag>
{
    //typedef Environment<sqlite, tag>      environment_type;
    typedef SqliteEnvironment<tag>             environment_type; /// @todo for tests only
    typedef Connection<sqlite, tag>       connection_type;
    typedef Statement<sqlite, tag>        statement_type;
    typedef Result<sqlite, tag>           resultset_type;
    typedef CachedResult<sqlite, tag>     cached_resultset_type;
    typedef sqlite::VALUE                 value_type;

    typedef sqlite::RESULT                dal_resultset_type;
    typedef sqlite::STMT                  dal_stmt_type;
    typedef sqlite::ENV                   dal_env_type;
    typedef sqlite::DBC                   dal_dbc_type;
    typedef sqlite::DIAG                  dal_diag_type;    
    typedef sqlite::COLUMNDESC            dal_columndesc_type;
    typedef sqlite::STATES                sqlstate_types;

    typedef sqlite_datatypes                   datatype_types;


    typedef Variant                            dal_variant_type;

    enum { DB_SYSTEM_ID = DAL_ENGINE_SQLITE };
};

struct sqlite_v4 { };



class SqliteConnection;
/*
  template<>
  struct db_traits<DAL::Sqlite, sqlite_v4>
  {
  typedef Environment<DAL::Sqlite, sqlite_v4>      environment_type;
  typedef SqliteConnection                         connection_type;
  typedef int                                      statement_type;
  typedef Result<DAL::Sqlite, sqlite_v4>           resultset_type;
  typedef CachedResult<DAL::Sqlite, sqlite_v4>     cached_resultset_type;
  typedef int                                      variant_type;
  typedef DAL::Sqlite::RESULT                      dal_resultset_type;

  enum { DB_SYSTEM_ID = 0 };
  };


  class SqliteConnection : public Connection<DAL::Sqlite, sqlite_v4, DAL::SqliteDbc>
  { };
*/


DB_NAMESPACE_END


#endif


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
