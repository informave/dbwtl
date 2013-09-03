//
// sdi_engine.hh - SDI Engine
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

/// @file
/// @brief SDI Engine
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_SDI_ENGINE_HH
#define INFORMAVE_DB_DAL_SDI_ENGINE_HH

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_objects.hh"
#include "dbwtl/util/smartptr.hh"


#if !defined(DBWTL_WITH_SDI)
#error "DBWTL was compiled without SDI support!"
#endif

DAL_NAMESPACE_BEGIN



class SDIBlob;
class SDIData;
class SDIDataProvider;
class SDIResult;
class SDIStmt;
class SDIDbc;
class SDIEnv;
class SDIVariant;
class SDIEngineState;
class SDITable;
class SDIDiag;
class SDIDatatype;
class SDIColumnDesc;
class SDIMetadata;


struct sdi;
struct sdi_sqlstates;



//------------------------------------------------------------------------------
///
/// @brief SDI SQLSTATEs
struct sdi_sqlstates : public basic_states
{
    typedef enum 
    {
        DAL_SDI_SQLSTATE_08000 = DAL_SQLSTATE_08000, // connection exception
        DAL_SDI_SQLSTATE_08001 = DAL_SQLSTATE_08001, // SQL-client unable to establish SQL-connection
        DAL_SDI_SQLSTATE_0A000 = DAL_SQLSTATE_0A000, // feature not supported
        DAL_SDI_SQLSTATE_22000 = DAL_SQLSTATE_22000, // data exception
        DAL_SDI_SQLSTATE_23000 = DAL_SQLSTATE_23000, // integrity constraint violation
        DAL_SDI_SQLSTATE_25000 = DAL_SQLSTATE_25000, // invalid transaction state
        DAL_SDI_SQLSTATE_25001 = DAL_SQLSTATE_25001, // active SQL-transaction
        DAL_SDI_SQLSTATE_25006 = DAL_SQLSTATE_25006, // read-only SQL-transaction
        DAL_SDI_SQLSTATE_28000 = DAL_SQLSTATE_28000, // invalid authorization specification
        DAL_SDI_SQLSTATE_42000 = DAL_SQLSTATE_42000  // syntax error or access rule violation

        //DAL_SDI_SQLSTATE_XY000 = DAL_SQLSTATE_LAST + 1
    } engine_states_t;
    

// connection exception
    typedef sqlstate<DAL_SDI_SQLSTATE_08000>                         SQLSTATE_08000;
    typedef sqlstate<DAL_SDI_SQLSTATE_08000>                         SQLSTATE_connection_exception;

// SQL-client unable to establish SQL-connection
    typedef sqlstate<DAL_SDI_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_08001;
    typedef sqlstate<DAL_SDI_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_sql_client_unable_to_establish_sql_connection;

// feature not supported
    typedef sqlstate<DAL_SDI_SQLSTATE_0A000>                         SQLSTATE_0A000;
    typedef sqlstate<DAL_SDI_SQLSTATE_0A000>                         SQLSTATE_feature_not_supported;

// data exception
    typedef sqlstate<DAL_SDI_SQLSTATE_22000>                         SQLSTATE_22000;
    typedef sqlstate<DAL_SDI_SQLSTATE_22000>                         SQLSTATE_data_exception;

// integrity constraint violation
    typedef sqlstate<DAL_SDI_SQLSTATE_23000>                         SQLSTATE_23000;
    typedef sqlstate<DAL_SDI_SQLSTATE_23000>                         SQLSTATE_integrity_constraint_violation;

// invalid transaction state
    typedef sqlstate<DAL_SDI_SQLSTATE_25000>                         SQLSTATE_25000;
    typedef sqlstate<DAL_SDI_SQLSTATE_25000>                         SQLSTATE_invalid_transaction_state;

// active SQL-transaction
    typedef sqlstate<DAL_SDI_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_25001;
    typedef sqlstate<DAL_SDI_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_active_sql_transaction;

// read-only SQL-transaction
    typedef sqlstate<DAL_SDI_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_25006;
    typedef sqlstate<DAL_SDI_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_read_only_sql_transaction;

// invalid authorization specification
    typedef sqlstate<DAL_SDI_SQLSTATE_28000>                         SQLSTATE_28000;
    typedef sqlstate<DAL_SDI_SQLSTATE_28000>                         SQLSTATE_invalid_authorization_specification;

// syntax error or access rule violation
    typedef sqlstate<DAL_SDI_SQLSTATE_42000>                         SQLSTATE_42000;
    typedef sqlstate<DAL_SDI_SQLSTATE_42000>                         SQLSTATE_syntax_error_or_access_rule_violation;


//     typedef sqlstate<DAL_SDI_SQLSTATE_XY000>                         SQLSTATE_XY000;
};




//------------------------------------------------------------------------------
///
/// @brief SDI Diagnostic
class DBWTL_EXPORT SDIDiag : public DiagBase
{
public:
    virtual ~SDIDiag(void)
    {}


    virtual SDIDiag* clone(void) const = 0;
    virtual void raiseException(void) const;


protected:
    SDIDiag(dalstate_t state,
               CodePosInfo pos,
               Variant what,
               String description);
    
    SDIDiag(const SDIDiag& ref);

    sdi_sqlstates::engine_states_t m_sqlstate_id;
};



typedef DiagController<SDIDiag> SDIDiagController;



//------------------------------------------------------------------------------
///
/// @brief SDI BLOB
class DBWTL_EXPORT SDIBlob : public IBlobBuffer
{
    // no sdi specific extensions
};




//------------------------------------------------------------------------------
///
/// @brief SDI BLOB
/*
class DBWTL_EXPORT SDIMemo : public IMemoBuffer
{
    // no sdi specific extensions
};
*/




//------------------------------------------------------------------------------
///
/// @internal
/// @details 
/// Internal representation of the SDI data(types).
///
/// @todo How to deal with UTF-8 strings?
class SDIData : public custom_deepcopy
{
public:
    SDIData(void);
    virtual ~SDIData(void);


    virtual SDIBlob*  getBlob(void) const = 0;
    virtual UnicodeStreamBuf*  getMemo(void) const = 0;

    virtual signed char getChar(void) const = 0;
    virtual unsigned char getUChar(void) const = 0;
    
    virtual signed short getSmallint(void) const = 0;
    virtual unsigned short getUSmallint(void) const = 0;
    
    virtual signed int getInt(void) const = 0;
    virtual unsigned int getUInt(void) const = 0;
    
    virtual signed long long getBigint(void) const = 0;
    virtual unsigned long long getUBigint(void) const = 0;

    virtual TNumeric getNumeric(void) const = 0;
    virtual bool getBool(void) const = 0;
    virtual String getString(void) const = 0;
    virtual TDate getDate(void) const = 0;
    virtual TTime getTime(void) const = 0;
    virtual TTimestamp getTimestamp(void) const = 0;
    virtual float getFloat(void) const = 0;
    virtual double getReal(void) const = 0;
    
    virtual rowid_t getCurrentRowID(void) const = 0;


    virtual bool isnull(void) const = 0;

    virtual void refresh(void) = 0;

    virtual daltype_t daltype(void) const = 0;

    virtual IVariantValue* do_deepcopy(const IVariantValue* owner) const;
};



namespace sdi_ext
{

class DBWTL_EXPORT Text : public TCustomType
{
public:
    Text(void) { }
    Text(const SDIVariant&) { }
    Text& operator=(const SDIVariant&) { return *this; }


    virtual daltype_t  datatype(void) const { return DAL_TYPE_CUSTOM; }
    virtual String asStr(std::locale loc) const { return L"foo"; }
    //virtual std::wstring asWideStr() const { return L"foo"; }
};

}






//------------------------------------------------------------------------------
///
/// @brief SDI Variant
class DBWTL_EXPORT SDIVariant : public EngineVariant
{
public:
    SDIVariant(SDIData* data);

    virtual ~SDIVariant(void);

    virtual void refresh(void);

    virtual sdi_ext::Text asText(void) const
    { return sdi_ext::Text(); }

    SDIData *m_data;

private:
    SDIVariant(const SDIVariant& ref);
    SDIVariant(void);
    SDIVariant& operator=(const SDIVariant&);
};





//------------------------------------------------------------------------------
///
/// @brief SDI Table object
class DBWTL_EXPORT SDITable : public ITable
{
public:
    typedef utils::SmartPtr<SDITable,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    SDITable(String dbname, SDIDataProvider& src);
    
    virtual ~SDITable(void);


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
/// @brief SDI Index object
class DBWTL_EXPORT SDIIndex : public IIndex
{
public:
    typedef utils::SmartPtr<SDIIndex,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    SDIIndex(String dbname, SDIDataProvider& src);
    
    virtual ~SDIIndex(void);


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
/// @brief SDI View object
class DBWTL_EXPORT SDIView : public IView
{
public:
    typedef utils::SmartPtr<SDIView,
        utils::RefCounted,
        utils::AllowConversion> ptr;
    
    SDIView(String dbname, SDIDataProvider& src);
    
    virtual ~SDIView(void);


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
/// @brief SDI Catalog object
class DBWTL_EXPORT SDICatalog : public ICatalog
{
public:
    typedef utils::SmartPtr<SDICatalog,
                           utils::RefCounted,
                           utils::AllowConversion> ptr;
    
    SDICatalog(String dbname);
    
    virtual ~SDICatalog(void);


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
/// @brief SDI Column Descriptor
class DBWTL_EXPORT SDIColumnDesc : public ColumnDescBase
{
public:
    friend class SDIResult;

    SDIColumnDesc(void) : ColumnDescBase()
    {}

    virtual ~SDIColumnDesc(void)
    {}

};


class DBWTL_EXPORT SDIMetadata : public IMetadata
{
public:
	typedef utils::SmartPtr<SDIMetadata,
	                        utils::RefCounted,
				utils::AllowConversion> ptr;

	SDIMetadata(SDIDbc &dbc) : IMetadata(), m_dbc(dbc)
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

	virtual ~SDIMetadata(void)
	{}

	SDIDbc &m_dbc;
};

//------------------------------------------------------------------------------
///
/// @brief SDI Datatype
class DBWTL_EXPORT SDIDatatype : public DatatypeBase
{
public:
    friend class SDIDbc;

    typedef utils::SmartPtr<SDIDatatype,
        utils::RefCounted,
        utils::AllowConversion> ptr;

    SDIDatatype(void);

    virtual ~SDIDatatype(void);
};






//------------------------------------------------------------------------------
///
/// @brief SDI Data Provider
/// This provides data from one data object, for example a table.
/// Providers are used by SQL Proxy.
class DBWTL_EXPORT SDIDataProvider : public IDataProvider
{
public:
    typedef IDataProvider interface_type;
    typedef std::auto_ptr<SDIDataProvider> ptr;

    typedef size_t                   bookmark_type;
    typedef SDIVariant               value_type;

    SDIDataProvider(SDIDiagController &dc) : IDataProvider(),
        m_diag(dc)
        {}


	virtual void openObjects(void) = 0;
	virtual void openColumns(const String &catalog, const String &schema, const String &table) = 0;


    virtual const value_type&     column(colnum_t num) = 0;
    virtual const value_type&     column(String name) = 0;

    virtual const SDIColumnDesc& describeColumn(colnum_t num) const = 0;
    virtual const SDIColumnDesc& describeColumn(String name) const = 0;
  
protected:
    SDIDiagController &m_diag;
};


//------------------------------------------------------------------------------
/// 
/// @brief SDI Resultset
/// @detail
/// This class is not derived from ResultBase, because we do not need
/// cursor state etc. All calls are passed to the internal SqlCursor.
class DBWTL_EXPORT SDIResult : public IResult
{
public:
	typedef ResultBase interface_type;
    typedef std::auto_ptr<SDIResult> ptr;

    typedef size_t                bookmark_type;
    typedef Variant               value_type;

    SDIResult(void) : IResult()
        {}

    virtual const value_type&     column(colnum_t num) = 0;
    //virtual SDIVariant&     field(colnum_t num) = 0;
    virtual const value_type&     column(String name) = 0;
    //virtual SDIVariant&     field(std::wstring name) = 0;

    virtual const IColumnDesc& describeColumn(colnum_t num) const = 0;
    virtual const IColumnDesc& describeColumn(String name) const = 0;
};



//------------------------------------------------------------------------------
///
///  @brief SDI Statement
class DBWTL_EXPORT SDIStmt : public StmtBase
{
public:
    typedef std::auto_ptr<SDIStmt> ptr;

    SDIStmt(void)
        : StmtBase(),
        m_diag()
    {}

    virtual SDIResult&        resultset(void) = 0;
    virtual const SDIResult&  resultset(void) const = 0;

    virtual bool              diagAvail(void) const;
    virtual const SDIDiag&    fetchDiag(void);

    virtual SDIDataProvider*  newProvider(void) = 0;
    

protected:

//    virtual void openObjects(void) = 0;

    SDIDiagController m_diag;
};





//------------------------------------------------------------------------------
///
/// @brief SDI Connection
class DBWTL_EXPORT SDIDbc : public DbcBase
{
public:
    typedef std::auto_ptr<SDIDbc> ptr;

    SDIDbc(void)
        : DbcBase(),
        m_diag()
        {}

    virtual SDIStmt*    newStatement(void) = 0;

    //virtual TableList      getTables(const ITableFilter& = EmptyTableFilter());
    virtual ViewList       getViews(const IViewFilter& = EmptyViewFilter());
    virtual DatatypeList   getDatatypes(const IDatatypeFilter& = EmptyDatatypeFilter());
    virtual SchemaList     getSchemas(const ISchemaFilter& = EmptySchemaFilter());
    virtual CatalogList    getCatalogs(const ICatalogFilter& = EmptyCatalogFilter());
    virtual ProcedureList  getProcedures(const IProcedureFilter& = EmptyProcedureFilter());
    virtual IndexList      getIndices(const IIndexFilter& = EmptyIndexFilter());

    virtual void           beginTrans(trx_mode mode,
                                      access_mode access,
                                      String name = String());

    virtual void           commit(void);
 
    virtual void           savepoint(String name);

    virtual void           rollback(String name = String());


    virtual void           commit(Transaction trx);
    virtual void           rollback(Transaction trx);


    virtual void           directCmd(String cmd);


    virtual bool             diagAvail(void) const;
    virtual const SDIDiag&   fetchDiag(void);

	virtual SDIMetadata* newMetadata(void);

	virtual SDIStmt* getSDICatalogs(void) = 0;
	virtual SDIStmt* getSDISchemas(void) = 0;
	virtual SDIStmt* getSDITables(void) = 0;
	virtual SDIStmt* getSDIColumns(void) = 0;

protected:
    SDIDiagController m_diag;
};





//------------------------------------------------------------------------------
///
/// @brief SDI Environment
class DBWTL_EXPORT SDIEnv : public EnvBase
{
public:
    typedef std::auto_ptr<SDIEnv> ptr;

    SDIEnv(void);
    virtual ~SDIEnv(void);
    
    virtual SDIDbc* newConnection(void) = 0;


    virtual bool              diagAvail(void) const;
    virtual const SDIDiag&    fetchDiag(void);

    virtual dal_engine getEngineType(void) const { return DAL_ENGINE_SDI; }
        

protected:
    SDIDiagController m_diag;
};







//------------------------------------------------------------------------------
///
/// @brief Main SDI interface class 
struct sdi
{
    typedef SDIDbc          DBC;
    typedef SDIResult       RESULT;
    typedef SDIStmt         STMT;
    typedef SDIEnv          ENV;
    typedef Variant         VALUE;
    typedef SDIDiag         DIAG;
    typedef sdi_sqlstates   STATES;
    typedef SDITable        TABLE;
    typedef IColumnDesc     COLUMNDESC;
	typedef SDIMetadata     METADATA;



    //typedef STATES::engine_states_t 

    DBWTL_EXPORT static const char* sqlstate2string(STATES::engine_states_t id);


    ///
    /// @brief Create a new Environment
    ///
    /// Current supported drivers are:
    ///  - libsdi
    ///
    DBWTL_EXPORT static ENV* createEnv(String driver);
};


DAL_NAMESPACE_END



DB_NAMESPACE_BEGIN




template<>
struct value_traits<SDIData*>
{
    typedef raw_pointer<SDIData*>  stored_type;
    typedef SDIData*    value_type;
    typedef var_info<SDIData*>         info_type;
};



template<>
struct value_traits<const SDIData*>
{
/*
    typedef raw_pointer<const dal::SqliteData*>  stored_type;
    typedef const dal::SqliteData*    value_type;
    typedef var_info<const dal::SqliteData*>         info_type;
*/
};


#define SV_CAST_METHOD(type) virtual type cast(type*, std::locale loc) const

//------------------------------------------------------------------------------
///
/// @brief Variant storage accessor for SDIData 
template<>
class sv_accessor<SDIData*> : public virtual sa_base<SDIData*>,
                                   public supports<signed int>,
                                   public supports<unsigned int>,
                                   public supports<signed char>,
                                   public supports<unsigned char>,
                                   public supports<String>,
                                   public supports<bool>,
                                   public supports<signed short>,
                                   public supports<unsigned short>,
                                   public supports<signed long long>,
                                   public supports<unsigned long long>,
                                   public supports<TNumeric>,
                                   public supports<float>,
                                   public supports<double>,
                                   public supports<TDate>,
                                   public supports<TTime>,
                                   public supports<TTimestamp>,
                                   public supports<Blob>,
                                   public supports<Memo>
    
{
public:
    SV_CAST_METHOD(signed int);
    SV_CAST_METHOD(unsigned int);
    SV_CAST_METHOD(signed char);
    SV_CAST_METHOD(unsigned char);
    SV_CAST_METHOD(String);
    SV_CAST_METHOD(bool);
    SV_CAST_METHOD(signed short);
    SV_CAST_METHOD(unsigned short);
    SV_CAST_METHOD(signed long long);
    SV_CAST_METHOD(unsigned long long);
    SV_CAST_METHOD(TNumeric);
    SV_CAST_METHOD(float);
    SV_CAST_METHOD(double);
    SV_CAST_METHOD(TDate);
    SV_CAST_METHOD(TTime);
    SV_CAST_METHOD(TTimestamp);
    SV_CAST_METHOD(Blob);
    SV_CAST_METHOD(Memo);

    virtual bool valueNullCheck() const;
    virtual daltype_t datatype() const;
};




struct sdi_datatypes : public basic_datatypes
{
/*
    virtual ~sdi_datatypes(void)
    {}
*/
	typedef sdi_ext::Text Text;
};


template<typename tag>
class SDIEnvironment : public Environment<sdi, tag>
{
public:

    SDIEnvironment(String str)
        : Environment<sdi, tag>(str)
    {}

    virtual ~SDIEnvironment(void)
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
struct db_traits<sdi, tag>
{
    //typedef Environment<dal::sdi, tag>      environment_type;
    typedef SDIEnvironment<tag>             environment_type; /// @todo for tests only
    typedef Connection<sdi, tag>       connection_type;
    typedef Statement<sdi, tag>        statement_type;
    typedef Result<sdi, tag>           resultset_type;
    typedef CachedResult<sdi, tag>     cached_resultset_type;
    typedef Metadata<sdi, tag>		metadata_type;
    typedef sdi::VALUE                 value_type;

    typedef sdi::RESULT                dal_resultset_type;
    typedef sdi::STMT                  dal_stmt_type;
    typedef sdi::ENV                   dal_env_type;
    typedef sdi::DBC                   dal_dbc_type;
    typedef sdi::DIAG                  dal_diag_type;    
    typedef sdi::COLUMNDESC            dal_columndesc_type;
    typedef sdi::STATES                sqlstate_types;
    typedef sdi::METADATA		dal_metadata_type;

    typedef sdi_datatypes                   datatype_types;


    typedef Variant                            dal_variant_type;

    enum { DB_SYSTEM_ID = DAL_ENGINE_SDI };
};

struct sdi_v4 { };



class SDIConnection;
/*
  template<>
  struct db_traits<DAL::SDI, sdi_v4>
  {
  typedef Environment<DAL::SDI, sdi_v4>      environment_type;
  typedef SDIConnection                         connection_type;
  typedef int                                      statement_type;
  typedef Result<DAL::SDI, sdi_v4>           resultset_type;
  typedef CachedResult<DAL::SDI, sdi_v4>     cached_resultset_type;
  typedef int                                      variant_type;
  typedef DAL::SDI::RESULT                      dal_resultset_type;

  enum { DB_SYSTEM_ID = 0 };
  };


  class SDIConnection : public Connection<DAL::SDI, sdi_v4, DAL::SDIDbc>
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
