//
// firebird_engine.hh - Firebird Engine
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
/// @brief Firebird Engine
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_FIREBIRD_ENGINE_HH
#define INFORMAVE_DB_DAL_FIREBIRD_ENGINE_HH

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_objects.hh"
#include "dbwtl/util/smartptr.hh"

#if !defined(DBWTL_WITH_FIREBIRD)
#error "DBWTL was compiled without Firebird support!"
#endif


DAL_NAMESPACE_BEGIN



class FirebirdBlob;
class FirebirdData;
class FirebirdResult;
class FirebirdStmt;
class FirebirdDbc;
class FirebirdEnv;
class FirebirdVariant;
class FirebirdEngineState;
class FirebirdTable;
class FirebirdDiag;
class FirebirdDatatype;
class FirebirdColumnDesc;
class FirebirdParamDesc;
class FirebirdMetadata;


struct firebird;
struct firebird_sqlstates;


//..............................................................................
///////////////////////////////////////////////////////////// firebird_sqlstates
///
/// @since 0.0.1
/// @brief Firebird SQLSTATEs
struct firebird_sqlstates : public basic_states
{
    typedef enum 
    {
        DAL_FIREBIRD_SQLSTATE_08000 = DAL_SQLSTATE_08000, // connection exception
        DAL_FIREBIRD_SQLSTATE_08001 = DAL_SQLSTATE_08001, // SQL-client unable to establish SQL-connection
        DAL_FIREBIRD_SQLSTATE_08002 = DAL_SQLSTATE_08002, // connection name in use
        DAL_FIREBIRD_SQLSTATE_08004 = DAL_SQLSTATE_08004, // SQL-server rejected establishment of SQL-connection
        DAL_FIREBIRD_SQLSTATE_0A000 = DAL_SQLSTATE_0A000, // feature not supported
        DAL_FIREBIRD_SQLSTATE_22000 = DAL_SQLSTATE_22000, // data exception
        DAL_FIREBIRD_SQLSTATE_23000 = DAL_SQLSTATE_23000, // integrity constraint violation
        DAL_FIREBIRD_SQLSTATE_24000 = DAL_SQLSTATE_24000, // invalid cursor state
        DAL_FIREBIRD_SQLSTATE_25000 = DAL_SQLSTATE_25000, // invalid transaction state
        DAL_FIREBIRD_SQLSTATE_25001 = DAL_SQLSTATE_25001, // active SQL-transaction
        DAL_FIREBIRD_SQLSTATE_25006 = DAL_SQLSTATE_25006, // read-only SQL-transaction
        DAL_FIREBIRD_SQLSTATE_28000 = DAL_SQLSTATE_28000, // invalid authorization specification
        DAL_FIREBIRD_SQLSTATE_42000 = DAL_SQLSTATE_42000, // syntax error or access rule violation

        DAL_FIREBIRD_SQLSTATE_HY000 = DAL_SQLSTATE_HY000  // 

        //DAL_FIREBIRD_SQLSTATE_XY000 = DAL_SQLSTATE_LAST + 1
    } engine_states_t;
    

// connection exception
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_08000>                         SQLSTATE_08000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_08000>                         SQLSTATE_connection_exception;

// SQL-client unable to establish SQL-connection
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_08001;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_sql_client_unable_to_establish_sql_connection;

// connection name in use
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_08002, SQLSTATE_08000>         SQLSTATE_08002;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_08002, SQLSTATE_08000>         SQLSTATE_connection_name_in_use;

// SQL-server rejected establishment of SQL-connection
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_08004, SQLSTATE_08000>         SQLSTATE_08004;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_08004, SQLSTATE_08000>         SQLSTATE_server_rejected_connection;

// feature not supported
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_0A000>                         SQLSTATE_0A000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_0A000>                         SQLSTATE_feature_not_supported;

// data exception
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_22000>                         SQLSTATE_22000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_22000>                         SQLSTATE_data_exception;

// integrity constraint violation
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_23000>                         SQLSTATE_23000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_23000>                         SQLSTATE_integrity_constraint_violation;


// invalid cursor state
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_24000>                         SQLSTATE_24000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_24000>                         SQLSTATE_invalid_cursor_state;


// invalid transaction state
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_25000>                         SQLSTATE_25000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_25000>                         SQLSTATE_invalid_transaction_state;

// active SQL-transaction
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_25001;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_active_sql_transaction;

// read-only SQL-transaction
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_25006;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_read_only_sql_transaction;

// invalid authorization specification
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_28000>                         SQLSTATE_28000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_28000>                         SQLSTATE_invalid_authorization_specification;

// syntax error or access rule violation
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_42000>                         SQLSTATE_42000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_42000>                         SQLSTATE_syntax_error_or_access_rule_violation;

// syntax error or access rule violation
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_HY000>                         SQLSTATE_HY000;
    typedef sqlstate<DAL_FIREBIRD_SQLSTATE_HY000>                         SQLSTATE_general_error;


//     typedef sqlstate<DAL_FIREBIRD_SQLSTATE_XY000>                         SQLSTATE_XY000;
};



//..............................................................................
/////////////////////////////////////////////////////////////////// FirebirdDiag
///
/// @since 0.0.1
/// @brief Firebird Diagnostic
class DBWTL_EXPORT FirebirdDiag : public DiagBase
{
public:
    virtual ~FirebirdDiag(void)
    {}


    virtual FirebirdDiag* clone(void) const = 0;
    virtual void raiseException(void) const;


protected:
    FirebirdDiag(dalstate_t state,
                 CodePosInfo pos,
                 Variant what,
                 String description);

    
    FirebirdDiag(const FirebirdDiag& ref);

    firebird_sqlstates::engine_states_t m_sqlstate_id;
};



typedef DiagController<FirebirdDiag> FirebirdDiagController;



//..............................................................................
/////////////////////////////////////////////////////////////////// FirebirdBlob
///
/// @since 0.0.1
/// @brief Firebird BLOB
class DBWTL_EXPORT FirebirdBlob : public IBlobBuffer
{
    // no firebird specific extensions
};



//..............................................................................
/////////////////////////////////////////////////////////////////// FirebirdMemo
///
/// @since 0.0.1
/// @brief Firebird MEMO
class DBWTL_EXPORT FirebirdMemo : public IMemoBuffer
{
    // no firebird specific extensions
};



//..............................................................................
/////////////////////////////////////////////////////////////////// FirebirdData
///
/// @since 0.0.1
/// @internal
/// @details 
/// Internal representation of the Firebird data(types).
///
/// @todo How to deal with UTF-8 strings?
class FirebirdData : public custom_deepcopy
{
public:
    FirebirdData(void);
    virtual ~FirebirdData(void);

    virtual String       getText(void) const = 0;
    virtual TDate        getDate(void) const = 0;
    virtual short        getSmallint(void) const = 0;
    virtual int          getLong(void) const = 0;
    virtual double       getDouble(void) const = 0;
    virtual int64_t      getInt64(void) const = 0;
    virtual float        getFloat(void) const = 0;
    virtual TTime        getTime(void) const = 0;
    virtual TTimestamp   getTimestamp(void) const = 0;
    virtual TNumeric     getNumeric(void) const = 0;
    virtual TVarbinary   getVarbinary(void) const = 0;

    virtual FirebirdBlob*  getBlobStream(void) const = 0;
    virtual UnicodeStreamBuf* getMemoStream(void) const = 0;
    //virtual FirebirdMemo*  getMemo(void) const = 0;

	// Convert helpers for the accessor (because accessor must be stateless)
	virtual BlobStream cast2BlobStream(std::locale loc) const = 0;
	virtual MemoStream cast2MemoStream(std::locale loc) const = 0;
	virtual Blob cast2Blob(std::locale loc) const = 0;
	virtual Memo cast2Memo(std::locale loc) const = 0;

 
    virtual rowid_t getCurrentRowID(void) const = 0;


    virtual bool isnull(void) const = 0;

    virtual void refresh(void) = 0;

    virtual daltype_t daltype(void) const = 0;

    //virtual IVariantValue* do_deepcopy(const IVariantValue* owner) const;
};



namespace firebird_ext
{

class DBWTL_EXPORT Text : public TCustomType
{
public:
    Text(void) { }
    Text(const FirebirdVariant&) { }
    Text& operator=(const FirebirdVariant&) { return *this; }


    virtual daltype_t  datatype(void) const { return DAL_TYPE_CUSTOM; }
    virtual String asStr(std::locale loc) const { return L"foo"; }
    //virtual std::wstring asWideStr() const { return L"foo"; }
};

}



//..............................................................................
//////////////////////////////////////////////////////////////// FirebirdVariant
///
/// @since 0.0.1
/// @brief Firebird Variant
class DBWTL_EXPORT FirebirdVariant : public EngineVariant
{
public:
    FirebirdVariant(FirebirdData* data);

    virtual ~FirebirdVariant(void);

    virtual void refresh(void);

    virtual firebird_ext::Text asText(void) const
    { return firebird_ext::Text(); }


private:
    FirebirdData *m_data;

    FirebirdVariant(const FirebirdVariant& ref);
    FirebirdVariant(void);
    FirebirdVariant& operator=(const FirebirdVariant&);
};



//..............................................................................
////////////////////////////////////////////////////////////////// FirebirdTable
///
/// @since 0.0.1
/// @brief Firebird Table object
class DBWTL_EXPORT FirebirdTable : public ITable
{
public:
    typedef utils::SmartPtr<FirebirdTable,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    FirebirdTable(String dbname, FirebirdResult& src);
    
    virtual ~FirebirdTable(void);


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



//..............................................................................
////////////////////////////////////////////////////////////////// FirebirdIndex
///
/// @since 0.0.1
/// @brief Firebird Index object
class DBWTL_EXPORT FirebirdIndex : public IIndex
{
public:
    typedef utils::SmartPtr<FirebirdIndex,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    FirebirdIndex(String dbname, FirebirdResult& src);
    
    virtual ~FirebirdIndex(void);


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



//..............................................................................
/////////////////////////////////////////////////////////////////// FirebirdView
///
/// @since 0.0.1
/// @brief Firebird View object
class DBWTL_EXPORT FirebirdView : public IView
{
public:
    typedef utils::SmartPtr<FirebirdView,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    FirebirdView(String dbname, FirebirdResult& src);
    
    virtual ~FirebirdView(void);


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



//..............................................................................
//////////////////////////////////////////////////////////////// FirebirdCatalog
///
/// @since 0.0.1
/// @brief Firebird Catalog object
class DBWTL_EXPORT FirebirdCatalog : public ICatalog
{
public:
    typedef utils::SmartPtr<FirebirdCatalog,
                           utils::RefCounted,
                           utils::AllowConversion> ptr;
    
    FirebirdCatalog(String dbname);
    
    virtual ~FirebirdCatalog(void);


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




//..............................................................................
///////////////////////////////////////////////////////////// FirebirdColumnDesc
///
/// @since 0.0.1
/// @brief Firebird Column Descriptor
class DBWTL_EXPORT FirebirdColumnDesc : public ColumnDescBase
{
public:
    friend class FirebirdResult;

    FirebirdColumnDesc(void) : ColumnDescBase()
    {}

    virtual ~FirebirdColumnDesc(void)
    {}

};


//..............................................................................
///////////////////////////////////////////////////////////// FirebirdParamDesc
///
/// @since 0.0.1
/// @brief Firebird Param Descriptor
class DBWTL_EXPORT FirebirdParamDesc : public ParamDescBase
{
public:
    friend class FirebirdResult;

    FirebirdParamDesc(void) : ParamDescBase()
    {}

    virtual ~FirebirdParamDesc(void)
    {}

};



class DBWTL_EXPORT FirebirdMetadata : public IMetadata
{
public:
	typedef utils::SmartPtr<FirebirdMetadata,
	                        utils::RefCounted,
				utils::AllowConversion> ptr;

	FirebirdMetadata(FirebirdDbc &dbc) : IMetadata(), m_dbc(dbc)
	{}

        virtual RecordSet getCatalogs(const Variant &catalog = Variant(),
                const IMetadata::ObjectClass system = META_OBJECT_CLASS_USER,
                const DatasetFilter &filter = NoFilter());
        virtual RecordSet getSchemas(const Variant &catalog = Variant(),
                const Variant &schema = Variant(),
                const ObjectClass system = META_OBJECT_CLASS_USER,
                const DatasetFilter &filter = NoFilter());
        virtual RecordSet getTables(const Variant &schema = Variant(),
                const Variant &catalog = Variant(),
                const Variant &table = Variant(),
                const IMetadata::ObjectClass system = META_OBJECT_CLASS_USER,
                const DatasetFilter &filter = NoFilter());
        virtual RecordSet getColumns(const Variant &table = Variant(),
                const Variant &schema = Variant(),
                const Variant &catalog = Variant(),
                const Variant &column = Variant(),
                const IMetadata::ObjectClass system = META_OBJECT_CLASS_USER,
                const DatasetFilter &filter = NoFilter());

	virtual ~FirebirdMetadata(void)
	{}

	FirebirdDbc &m_dbc;
};

//..............................................................................
/////////////////////////////////////////////////////////////// FirebirdDatatype
///
/// @since 0.0.1
/// @brief Firebird Datatype
class DBWTL_EXPORT FirebirdDatatype : public DatatypeBase
{
public:
    friend class FirebirdDbc;

    typedef utils::SmartPtr<FirebirdDatatype,
        utils::RefCounted,
        utils::AllowConversion> ptr;

    FirebirdDatatype(void);

    virtual ~FirebirdDatatype(void);
};




//..............................................................................
///////////////////////////////////////////////////////////////// FirebirdResult
///
/// @since 0.0.1
/// @brief Firebird Resultset
class DBWTL_EXPORT FirebirdResult : public ResultBase
{
public:
    typedef std::auto_ptr<FirebirdResult> ptr;

    typedef size_t                      bookmark_type;
    typedef FirebirdVariant               value_type;

    FirebirdResult(FirebirdDiagController &dc) : ResultBase(),
        m_diag(dc)
        {}

    virtual const value_type&     column(colnum_t num) = 0;
    //virtual FirebirdVariant&     field(colnum_t num) = 0;
    virtual const value_type&     column(String name) = 0;
    //virtual FirebirdVariant&     field(std::wstring name) = 0;


    virtual const FirebirdColumnDesc& describeColumn(colnum_t num) const = 0;

    virtual const FirebirdColumnDesc& describeColumn(String name) const = 0;

    virtual const FirebirdParamDesc&   describeParam(int num) const = 0;
  
protected:
    FirebirdDiagController &m_diag;
};




//..............................................................................
/////////////////////////////////////////////////////////////////// FirebirdStmt
///
/// @since 0.0.1
///  @brief Firebird Statement
class DBWTL_EXPORT FirebirdStmt : public StmtBase
{
public:
    typedef std::auto_ptr<FirebirdStmt> ptr;

    FirebirdStmt(void)
        : StmtBase(),
        m_diag()
    {}

    virtual FirebirdResult&        resultset(void) = 0;
    virtual const FirebirdResult&  resultset(void) const = 0;

    virtual bool                 diagAvail(void) const;
    virtual const FirebirdDiag&   fetchDiag(void);

    virtual void       prepare(String sql) = 0;
    virtual void       prepare(String sql, Transaction trx) = 0; // firebird specific
    virtual void       execDirect(String sql) = 0;
    virtual void       execDirect(String sql, Transaction trx) = 0; // firebird specific


protected:
    mutable FirebirdDiagController m_diag;
};




//..............................................................................
//////////////////////////////////////////////////////////////////// FirebirdDbc
///
/// @since 0.0.1
/// @brief Firebird Connection
class DBWTL_EXPORT FirebirdDbc : public DbcBase
{
public:
    typedef std::auto_ptr<FirebirdDbc> ptr;

    FirebirdDbc(void)
        : DbcBase(),
        m_diag()
        {}

    virtual FirebirdStmt*  newStatement(void) = 0;

    virtual TableList      getTables(const ITableFilter& = EmptyTableFilter());
    virtual ViewList       getViews(const IViewFilter& = EmptyViewFilter());
    virtual DatatypeList   getDatatypes(const IDatatypeFilter& = EmptyDatatypeFilter());
    virtual SchemaList     getSchemas(const ISchemaFilter& = EmptySchemaFilter());
    virtual CatalogList    getCatalogs(const ICatalogFilter& = EmptyCatalogFilter());
    virtual ProcedureList  getProcedures(const IProcedureFilter& = EmptyProcedureFilter());
    virtual IndexList      getIndices(const IIndexFilter& = EmptyIndexFilter());

    virtual void           commit(void);
    virtual void           savepoint(String name);
    virtual void           rollback(String name = String());

    virtual void           directCmd(String cmd);

    virtual bool                  diagAvail(void) const;
    virtual const FirebirdDiag&   fetchDiag(void);

    
    // firebird specific

    virtual Transaction    makeTrx(trx_mode mode,
                                   access_mode access = trx_default,
                                   String name = String()) = 0;

    virtual bool           hasActiveTrx(void) = 0;
    virtual void           commit(Transaction trx) = 0;
    virtual void           rollback(Transaction trx) = 0;

	virtual FirebirdMetadata* newMetadata(void);

    virtual String         quoteIdentifier(const String &id);

protected:
    mutable FirebirdDiagController m_diag;
};




//..............................................................................
//////////////////////////////////////////////////////////////////// FirebirdEnv
///
/// @since 0.0.1
/// @brief Firebird Environment
class DBWTL_EXPORT FirebirdEnv : public EnvBase
{
public:
    typedef std::auto_ptr<FirebirdEnv> ptr;

    FirebirdEnv(void);
    virtual ~FirebirdEnv(void);
    
    virtual FirebirdDbc* newConnection(void) = 0;


    virtual bool                 diagAvail(void) const;
    virtual const FirebirdDiag&    fetchDiag(void);

    virtual dal_engine getEngineType(void) const { return DAL_ENGINE_FIREBIRD; }
        

protected:
    mutable FirebirdDiagController m_diag;
};




//..............................................................................
/////////////////////////////////////////////////////////////////////// firebird
///
/// @since 0.0.1
/// @brief Main Firebird interface class 
struct firebird
{
    typedef FirebirdDbc          DBC;
    typedef FirebirdResult       RESULT;
    typedef FirebirdStmt         STMT;
    typedef FirebirdEnv          ENV;
    typedef FirebirdVariant      VALUE;
    typedef FirebirdDiag         DIAG;
    typedef firebird_sqlstates   STATES;
    typedef FirebirdTable        TABLE;
    typedef FirebirdColumnDesc   COLUMNDESC;
	typedef FirebirdMetadata     METADATA;


    DBWTL_EXPORT static const char* sqlstate2string(STATES::engine_states_t id);


    ///
    /// @brief Create a new Environment
    ///
    /// Current supported drivers are:
    ///  - libfbclient
    ///
    DBWTL_EXPORT static ENV* createEnv(String driver);
};


DAL_NAMESPACE_END








DB_NAMESPACE_BEGIN

template<>
struct value_traits<FirebirdData*>
{
    typedef raw_pointer<FirebirdData*>   stored_type;
    typedef FirebirdData*                value_type;
    typedef var_info<FirebirdData*>      info_type;
};


template<>
struct value_traits<const FirebirdData*>
{ /* empty traits to prevent user mistakes */ };



//..............................................................................
//////////////////////////////////////////////////////////////////// sv_accessor
///
/// @since 0.0.1
/// @brief Variant storage accessor for FirebirdData 
template<>
struct sv_accessor<FirebirdData*> : public virtual sa_base<FirebirdData*>,
                                         public supports<signed int>,
                                         public supports<signed short>,
                                         public supports<signed long long>,
                                         public supports<float>,
                                         public supports<double>,
                                         public supports<bool>,
                                         public supports<BlobStream>,
                                         public supports<MemoStream>,
                                         public supports<Blob>,
                                         public supports<Memo>,
                                         public supports<TDate>,
                                         public supports<TTime>,
                                         public supports<TTimestamp>,
                                         public supports<TNumeric>,
                                         public supports<TVarbinary>,
                                         public supports<String>
{
    virtual signed int cast(signed int*, std::locale loc) const;
    virtual signed short cast(signed short*, std::locale loc) const;
    virtual signed long long cast(signed long long*, std::locale loc) const;
    virtual float cast(float*, std::locale loc) const;
    virtual double cast(double*, std::locale loc) const;
    virtual bool cast(bool*, std::locale loc) const;
    virtual BlobStream cast(BlobStream*, std::locale loc) const;
    virtual MemoStream cast(MemoStream*, std::locale loc) const;
    virtual Blob cast(Blob*, std::locale loc) const;
    virtual Memo cast(Memo*, std::locale loc) const;
    virtual TDate cast(TDate*, std::locale loc) const;
    virtual TTime cast(TTime*, std::locale loc) const;
    virtual TTimestamp cast(TTimestamp*, std::locale loc) const;
    virtual TNumeric cast(TNumeric*, std::locale loc) const;
    virtual TVarbinary cast(TVarbinary*, std::locale loc) const;
    virtual String cast(String*, std::locale loc) const;


    virtual bool valueNullCheck(void) const;
    //virtual bool isNull() const;

    virtual daltype_t datatype() const;

    virtual ~sv_accessor(void)
    {}

protected:
   sv_accessor(void) {}

   sv_accessor(const sv_accessor &orig)
   {}


private:    
    sv_accessor& operator=(const sv_accessor&);
};




struct firebird_datatypes : public basic_datatypes
{
    // firebird has no specific datatypes
};


template<typename tag>
class FirebirdConnection : public Connection<firebird, tag>
{
public:
	FirebirdConnection(typename db_traits<firebird, tag>::environment_type &env)
		: Connection<firebird, tag>(env)
	{}

	virtual ~FirebirdConnection(void)
	{}

	Transaction makeTrx(trx_mode mode,
                             access_mode access = trx_default,
                             String name = String())
	{
		return this->m_dbc->makeTrx(mode, access, name);
	}
};


template<typename tag>
class FirebirdStatement : public Statement<firebird, tag>
{
public:
	FirebirdStatement(typename db_traits<firebird, tag>::connection_type &dbc)
		: Statement<firebird, tag>(dbc)
	{}

	virtual ~FirebirdStatement(void)
	{}


	virtual void      prepare(String sql) { this->m_stmt->prepare(sql); }


	virtual void      prepare(String sql, Transaction trx)
	{
		this->m_stmt->prepare(sql, trx);
	}

	virtual void      execDirect(String sql) { this->m_stmt->execDirect(sql); }

	virtual void      execDirect(String sql, Transaction trx)
	{
		this->m_stmt->execDirect(sql, trx);
	}
};



//..............................................................................
////////////////////////////////////////////////////////////////////// db_traits
///
/// @since 0.0.1
template<typename tag>
struct db_traits<firebird, tag>
{
    typedef Environment<firebird, tag>      environment_type;
    typedef FirebirdConnection<tag>              connection_type;
    typedef FirebirdStatement<tag>               statement_type;
    typedef Result<firebird, tag>           resultset_type;
    typedef CachedResult<firebird, tag>     cached_resultset_type;
    typedef Metadata<firebird, tag>	metadata_type;
    typedef firebird::VALUE                 value_type;

    typedef firebird::RESULT                dal_resultset_type;
    typedef firebird::STMT                  dal_stmt_type;
    typedef firebird::ENV                   dal_env_type;
    typedef firebird::DBC                   dal_dbc_type;
    typedef firebird::DIAG                  dal_diag_type;    
    typedef firebird::COLUMNDESC            dal_columndesc_type;
    typedef firebird::STATES                sqlstate_types;
    typedef firebird::METADATA		dal_metadata_type;

    typedef firebird_datatypes                   datatype_types;


    typedef Variant                              dal_variant_type;

    enum { DB_SYSTEM_ID = DAL_ENGINE_FIREBIRD };
};


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
