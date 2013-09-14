//
// dal_interface.hh - Public DAL types and classes
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
/// @brief Public DAL types and classes
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_INTERFACES_HH
#define INFORMAVE_DB_DAL_INTERFACES_HH


#define DAL_DEV_INCLUDE_DEVUTILS

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/variant.hh"
#include "dbwtl/types.hh"

#include "dbwtl/ustring.hh"
#include "dbwtl/ustreambuf.hh"
#include "dbwtl/util/smartptr.hh"

#include "dbwtl/exceptions.hh"


#ifdef DAL_DEV_INCLUDE_DEVUTILS
//#include "dbwtl/util/devutils.hh"
#endif

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <iosfwd>
#include <list>
#include <algorithm>
//#include <sstream>
#ifdef DBWTL_HAVE_STDINT_H
#include <stdint.h> // required until we get cstdint from c++0x
#else
typedef long long int64_t;
#endif

#ifndef DBWTL_CXX98_COMPATIBILITY
#include <type_traits>
#endif


DAL_NAMESPACE_BEGIN


typedef enum dal_engines_enum
{
    DAL_ENGINE_GENERIC=0,
    DAL_ENGINE_SQLITE=50,
    DAL_ENGINE_SDI=51,
    DAL_ENGINE_FIREBIRD=52,
    DAL_ENGINE_ODBC=53
} dal_engine;

///
/// @brief Type for row IDs
typedef signed long long        rowid_t;

#define DAL_TYPE_ROWID_FIRST_ROW   1      /// Row IDs starts at 1
#define DAL_TYPE_ROWID_NPOS        0      /// not positioned

typedef signed long long        rowcount_t;

typedef signed long long	rownum_t;

///
/// @brief Type for column numbers
typedef uint16_t             colnum_t;

#define DAL_TYPE_COLNUM_FIRST_COLUMN  1  /// Columns start at 1
#define DAL_TYPE_COLNUM_BOOKMARK      0  /// Bookmark columns


///
/// @brief DBMS dependend TYPE ID
typedef signed int systype_t;


/// 
/// @brief Transaction id
typedef unsigned int trxid_t;


#define DAL_MSG_SUCCESS                 L"Success."


/// 
/// Cursor states
#define DAL_CURSOR_CLOSED        0
#define DAL_CURSOR_PREPARED      0x01
#define DAL_CURSOR_OPEN          0x02
#define DAL_CURSOR_POSITIONED    0x04
#define DAL_CURSOR_EOF           0x08
#define DAL_CURSOR_BAD           0x10

typedef unsigned char cursorstate_t;

//#define DAL_MASK_LOW(mask) (mask | (mask-1))

#define DAL_SET_CURSORSTATE(cs, state) cs =             \
        state == DAL_CURSOR_BAD ? (cs | state) : (state | (state-1))


typedef enum
{
    DAL_STATE_OK,
    DAL_STATE_INFO,
    DAL_STATE_WARNING,
    DAL_STATE_ERROR
} dalstate_t;


///
/// @brief Get the name of the code as string
const char* dal_state_msg(int code);

#define DAL_CODE(codename) dal_state_msg(codename)


#define DAL_STREAMBUF_BUFSIZE 1024*4
#define DAL_STREAMBUF_PUTBACK 8



    /// @brief Connection transaction modes
    typedef enum trx_mode_enum
    {
        trx_read_uncommitted,   /// SQL:2003 TRX UNCOMMITTED
        trx_read_committed,     /// SQL:2003 TRX COMMITTED
        trx_repeatable_read,    /// SQL:2003 TRX REPEATABLE READ
        trx_serializable        /// SQL:2003 TRX SERIALIZABLE
    } trx_mode;

    
    /// @brief Transaction access mode
typedef enum access_mode_enum
    {
        trx_readwrite,
        trx_readonly,
        trx_default        /// default is the default of the database engine
    } access_mode;





typedef std::map<std::string, Variant> options_type;

struct CodePosInfo
{
    CodePosInfo(const char *file, int line, const char *func)
        : m_file(file),
          m_line(line),
          m_func(func)
        {}

    CodePosInfo(const CodePosInfo &pos)
    : m_file(pos.m_file),
      m_line(pos.m_line),
      m_func(pos.m_func)
        {}

    String str(void) const;


protected:
    const char *m_file;
    int m_line;
    const char *m_func;
};




template<typename T, typename U>
bool inline convertValue(const T& src, U& dest)
{
    std::wstringstream ss;
    ss << src;
    return ! (!(ss >> dest));
}

template<typename T, typename U>
void inline setValue(const T& src, U& dest)
{
    std::wstringstream ss;
    ss << src;
    dest = ss.str();
}


struct DBWTL_EXPORT ObjectName
{
	ObjectName(String table, String schema, String catalog) : m_table(table),
		m_schema(schema), m_catalog(catalog)
		{}

    ObjectName(void) : m_table(), m_schema(), m_catalog()
    {}

	inline const String& table(void) const { return m_table; }
	inline const String& schema(void) const { return m_schema; }
	inline const String& catalog(void) const { return m_catalog; }

	String m_table, m_schema, m_catalog;
};




//--------------------------------------------------------------------------
/// Controller for diagnostic records
///
/// @since 0.0.1
/// @brief Controller for diagnostic records
template<class T>
class DiagController
{
public:
    DiagController(void)
        : m_list(),
          m_cur()
    {
        m_cur = m_list.begin();
    }
    

    /// Free all records
    ~DiagController(void)
    {
        std::for_each(this->m_list.begin(), this->m_list.end(),
                      [](T* i)
                      {
                          delete i;
                      });
    }


    /// Push back a diagnostic record. The livetime is now controlled by
    /// this controller class.
    ///
    /// @todo Use an option variable for the max. size of the deque.
    T* push_back(T *diag)
    {
        this->m_list.push_back(diag);
        if(m_cur == m_list.end())
            m_cur = m_list.begin();

        // remove older records
        while(m_list.size() > 10)
        {
            typename std::list<T*>::iterator f = m_list.begin();
            if(f == this->m_cur)
                this->m_cur++;
            delete *f;
            m_list.pop_front();
        }
        return diag;
    }


    /// Checks if there are any records available
    bool diagAvail(void) const
    {
        return this->m_cur != this->m_list.end();
    }

    
    /// The reference keeps valid until no other method is called.
    const T& fetchDiag(void)
    {
        if(this->m_cur != this->m_list.end())
        {
            return **this->m_cur++;
        }
        else
            throw NotFoundException(L"No diagnostic records available");
    }

protected:
    std::list<T*>                     m_list;
    typename std::list<T*>::iterator  m_cur;
};




//------------------------------------------------------------------------------
/// Base class for all engine specific implementations of the Variant class.
///
/// @since 0.0.1
/// @brief Base class for engine specific variants
class DBWTL_EXPORT  EngineVariant : public Variant
{
public:
    template<class T>
        EngineVariant(const T& value, const String &name = String("<engine-variant>"))
        : Variant(value, name)
    {}


    virtual ~EngineVariant(void)
    {}


    /// This method refreshes internal states or resources
    /// which must be syncronised with the current row cursor position.
    /// If the cursor moves, data must be reloaded from the internal
    /// resultset handle or resources like streambuf objects (for BLOBs)
    /// must be invalidated.
    virtual void refresh(void) = 0;

};




//--------------------------------------------------------------------------
/// Interface for diagnostic records
///
/// @since 0.0.1
/// @brief Interface for diagnostic records
class DBWTL_EXPORT IDiagnostic
{
public:
    virtual ~IDiagnostic(void)
    {}

    virtual dalstate_t         getState(void) const = 0;
//     virtual const Variant&     getQuery(void) const = 0;
//     virtual const Variant&     getNativeErrorCode(void) const = 0;
    virtual const Variant&     getMsg(void) const = 0;
    virtual const Variant&     getDescription(void) const = 0;
    virtual const CodePosInfo&     getCodepos(void) const = 0;
    virtual const Variant&     getSqlstate(void) const = 0;

//     virtual const Variant&     getRowNumber(void) const = 0;
//     virtual const Variant&     getServerName(void) const = 0;
//     virtual const Variant&     getColumnNumber(void) const = 0;
//     virtual const Variant&     getUsedFiles(void) const = 0;


    /// SQLSTATE exceptions stores an own copy of a diagnostic record.
    /// It must be save to clone a complete record even if the handle that
    /// has initially created the record is destroyed.
    virtual IDiagnostic* clone(void) const = 0;

    /// Dumps the information as as string
    virtual String str(void) const = 0;

    /// Checks the stored information and error codes and raises a
    /// appropriate SQLSTATE exception.
    virtual void raiseException(void) const = 0;
};





//--------------------------------------------------------------------------
/// @brief Base class for Handles
class DBWTL_EXPORT IHandle : public IDALObject
{
public:
    IHandle(void);
    virtual ~IHandle(void) {}

    virtual bool                 diagAvail(void) const = 0;

    /// The reference keeps valid until no other method is called.
    virtual const IDiagnostic&   fetchDiag(void) = 0;


    virtual void             setOption(std::string name, const Variant &data) = 0;
    virtual const Variant&   getOption(std::string name) const = 0;
};


//--------------------------------------------------------------------------
/////// @brief Transaction Handle
class DBWTL_EXPORT Transaction
{
public:
    Transaction(void);
    Transaction(IHandle *handle, trxid_t trxid);
    Transaction(const Transaction& trx);
    ~Transaction(void);

    Transaction& operator=(const Transaction &trx);

    inline trxid_t  id(void)     const { return this->m_trxid;  }
    inline IHandle* handle(void) const { return this->m_handle; }

    void commit(void);
    void rollback(void);

protected:
    IHandle   *m_handle;
    trxid_t    m_trxid;
};



//--------------------------------------------------------------------------
/// @brief DAL Interface for drivers
class DBWTL_EXPORT IDALDriver : public IDALObject
{
public:
    virtual ~IDALDriver(void) { }
};




//--------------------------------------------------------------------------
/// @brief DAL Interface for BLOB buffers
class DBWTL_EXPORT IBlobBuffer : public ByteStreamBuf
{
public:
protected:
};



//--------------------------------------------------------------------------
/// @brief DAL Interface for MEMO buffers
class DBWTL_EXPORT IMemoBuffer : public UnicodeStreamBuf
{
public:
protected:
};





//--------------------------------------------------------------------------
/// @brief DAL Interface for column descriptors
class DBWTL_EXPORT IColumnDesc : public IDALObject
{
public:
    typedef Variant value_type;
    //typedef util::SmartPtr<ITable, util::RefCounted, util::AllowConversion> ptr;

    /// Empty virtual destructor
    virtual ~IColumnDesc(void) { }

    /// @brief Returns the name of the column
    virtual const value_type& getName(void) const = 0;
    virtual const value_type& getCatalogName(void) const = 0;
    virtual const value_type& getSchemaName(void) const = 0;
    virtual const value_type& getBaseColumnName(void) const = 0;
    virtual const value_type& getTypeName(void) const = 0;
    virtual const value_type& getBaseTableName(void) const = 0;
    virtual const value_type& getSize(void) const = 0;
    virtual const value_type& getComment(void) const = 0;
    virtual const value_type& getIsNullable(void) const = 0;
    virtual const value_type& getPrecision(void) const = 0;
    virtual const value_type& getScale(void) const = 0;
    virtual const value_type& getIsSearchable(void) const = 0;

    /// @todo datatype() should return IDatatype, which is get from the DBC method
    virtual daltype_t getDatatype(void) const = 0;
};




class DBWTL_EXPORT DatasetFilter
{
public:
    virtual ~DatasetFilter()
    {}

    virtual bool operator()(IDataset &ds) const = 0;
};

template<typename T>
class DBWTL_EXPORT ColumnMatchFilter : public DatasetFilter
{
public:
    ColumnMatchFilter(const String &column, const Variant &value) : DatasetFilter(),
                                       m_column(column),
									   m_value(value)
    {}
    virtual bool operator()(IDataset &ds) const;
protected:
	String m_column;
	Variant m_value;
};

class DBWTL_EXPORT MetadataFilter : public DatasetFilter
{
};

class DBWTL_EXPORT NoFilter : public DatasetFilter
{
public:
    virtual bool operator()(IDataset &ds) const
    {
        return true;
    }
};

class DBWTL_EXPORT TableFilter : public MetadataFilter
{
public:
    TableFilter(const String &table) : MetadataFilter(),
                                       m_table(table)
    {}

    virtual bool operator()(IDataset &ds) const;

    const String m_table;
};

class DBWTL_EXPORT ViewFilter : public MetadataFilter
{
};

class DBWTL_EXPORT ProcedureFilter : public MetadataFilter
{
};

class DBWTL_EXPORT ColumnFilter : public MetadataFilter
{
};


class DBWTL_EXPORT IMetadata : IDALObject
{
public:
    /*
    typedef enum
    {
        METADATA_FILTER_INPUT,
        METADATA_FILTER_OUTPUT
    } FilterDirection;
    */

    typedef enum
    {
    	META_OBJECT_CLASS_SYSTEM,
	META_OBJECT_CLASS_USER,
	META_OBJECT_CLASS_ALL
    } ObjectClass;

	typedef utils::SmartPtr<IMetadata, utils::RefCounted, utils::AllowConversion> ptr;

	virtual ~IMetadata(void) {}

	virtual RecordSet getCatalogs(const Variant &catalog = Variant(),
		const ObjectClass system = META_OBJECT_CLASS_USER,
		const DatasetFilter &filter = NoFilter()) = 0;
	virtual RecordSet getSchemas(const Variant &catalog = Variant(),
		const Variant &schema = Variant(),
		const ObjectClass system = META_OBJECT_CLASS_USER,
		const DatasetFilter &filter = NoFilter()) = 0;
	virtual RecordSet getTables(const Variant &schema = Variant(),
		const Variant &catalog = Variant(),
		const Variant &table = Variant(),
		const ObjectClass system = META_OBJECT_CLASS_USER,
		const DatasetFilter &filter = NoFilter()) = 0;
	virtual RecordSet getColumns(const Variant &table = Variant(),
		const Variant &schema = Variant(),
		const Variant &catalog = Variant(),
		const Variant &column = Variant(),
		const ObjectClass system = META_OBJECT_CLASS_USER,
		const DatasetFilter &filter = NoFilter()) = 0;

};


//------------------------------------------------------------------------------
/// The ITable interface represents a single table.
/// All values returned a variant types because some of them may be null.
///
/// @since 0.0.1
/// @brief DAL Interface for a single table
class DBWTL_EXPORT ITable : IDALObject
{
public:
    //typedef IVariant value_type;
    typedef Variant value_type;
    typedef utils::SmartPtr<ITable, utils::RefCounted, utils::AllowConversion> ptr;
    
    /// Empty virtual destructor
    virtual ~ITable(void) { }
    
    /// @brief Returns the name of the table
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the name of the catalog
    virtual const value_type& getCatalog(void) const = 0;

    /// @brief Returns the name of the schema
    virtual const value_type& getSchema(void) const = 0;

    /// @brief Returns the comment for the table
    virtual const value_type& getComment(void) const = 0;

    /// @brief Returns the DDL statement for the table
    virtual const value_type& getDDL(void) const = 0;

    /// @brief Returns true/false if the table is a system object
    /// @return a null value if is not determinable
    virtual const value_type& isSystemObject(void) const = 0;
};



//------------------------------------------------------------------------------
/// The IIndex interface represents a single index.
/// All values returned a variant types because some of them may be null.
///
/// @since 0.0.1
/// @brief DAL Interface for a single index
class DBWTL_EXPORT IIndex : IDALObject
{
public:
    typedef Variant value_type;
    typedef utils::SmartPtr<IIndex, utils::RefCounted, utils::AllowConversion> ptr;
    
    /// Empty virtual destructor
    virtual ~IIndex(void) { }
    
    /// @brief Returns the name of the index
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the name of the base table
    virtual const value_type& getTable(void) const = 0;

    /// @brief Returns the name of the catalog
    virtual const value_type& getCatalog(void) const = 0;

    /// @brief Returns the name of the schema
    virtual const value_type& getSchema(void) const = 0;

    /// @brief Returns the comment for the index
    virtual const value_type& getComment(void) const = 0;

    /// @brief Returns the DDL statement for the index
    virtual const value_type& getDDL(void) const = 0;

    /// @brief Returns true/false if the index is a system object
    /// @return a null value if is not determinable
    virtual const value_type& isSystemObject(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for types
class DBWTL_EXPORT IDatatype : IDALObject
{
public:
    typedef utils::SmartPtr<IDatatype,
                            utils::RefCounted,
                            utils::AllowConversion> ptr;

    typedef Variant value_type;

    /// @brief Returns the name of the table
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the maximum size for variable length types
    /// or the size for fixed length types (like INT)
    virtual const value_type& getSize(void) const = 0;

    virtual const value_type& getLiteralPrefix(void) const = 0;

    virtual const value_type& getLiteralSuffix(void) const = 0;

    virtual const value_type& getIsUnsigned(void) const = 0;

    virtual daltype_t         daltype(void) const = 0;
};



//------------------------------------------------------------------------------
///
/// @brief DAL Interface for views
class DBWTL_EXPORT IView : IDALObject
{
public:
    typedef Variant value_type;
    typedef utils::SmartPtr<IView, utils::RefCounted, utils::AllowConversion> ptr;
    
    /// Empty virtual destructor
    virtual ~IView(void) { }
    
    /// @brief Returns the name of the view
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the name of the catalog
    virtual const value_type& getCatalog(void) const = 0;

    /// @brief Returns the name of the schema
    virtual const value_type& getSchema(void) const = 0;

    /// @brief Returns the comment for the view
    virtual const value_type& getComment(void) const = 0;

    /// @brief Returns the DDL statement for the view
    virtual const value_type& getDDL(void) const = 0;

    /// @brief Returns true/false if the view is a system object
    /// @return a null value if is not determinable
    virtual const value_type& isSystemObject(void) const = 0;
};



//------------------------------------------------------------------------------
///
/// @brief DAL Interface for schemas
class DBWTL_EXPORT ISchema : IDALObject
{
public:
    typedef Variant value_type;
    typedef utils::SmartPtr<ISchema, utils::RefCounted, utils::AllowConversion> ptr;
    
    /// Empty virtual destructor
    virtual ~ISchema(void) { }
    
    /// @brief Returns the name of the schema
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the name of the catalog
    virtual const value_type& getCatalog(void) const = 0;

    /// @brief Returns the comment for the schema
    virtual const value_type& getComment(void) const = 0;

    /// @brief Returns the DDL statement for the schema
    virtual const value_type& getDDL(void) const = 0;

    /// @brief Returns true/false if the schema is a system object
    /// @return a null value if is not determinable
    virtual const value_type& isSystemObject(void) const = 0;
};




//------------------------------------------------------------------------------
///
/// @brief DAL Interface for catalogs
class DBWTL_EXPORT ICatalog : IDALObject
{
public:
    typedef Variant value_type;
    typedef utils::SmartPtr<ICatalog, utils::RefCounted, utils::AllowConversion> ptr;
    
    /// Empty virtual destructor
    virtual ~ICatalog(void) { }
    
    /// @brief Returns the name of the catalog
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the comment for the catalog
    virtual const value_type& getComment(void) const = 0;

    /// @brief Returns the DDL statement for the catalog
    virtual const value_type& getDDL(void) const = 0;

    /// @brief Returns true/false if the catalog is a system object
    /// @return a null value if is not determinable
    virtual const value_type& isSystemObject(void) const = 0;
};


//------------------------------------------------------------------------------
///
/// @brief DAL Interface for procedures
class DBWTL_EXPORT IProcedure : IDALObject
{
public:
    typedef Variant value_type;
    typedef utils::SmartPtr<IProcedure, utils::RefCounted, utils::AllowConversion> ptr;
    
    /// Empty virtual destructor
    virtual ~IProcedure(void) { }
    
    /// @brief Returns the name of the procedure
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the name of the catalog
    virtual const value_type& getCatalog(void) const = 0;

    /// @brief Returns the name of the schema
    virtual const value_type& getSchema(void) const = 0;

    /// @brief Returns the comment for the procedure
    virtual const value_type& getComment(void) const = 0;

    /// @brief Returns the DDL statement for the procedure
    virtual const value_type& getDDL(void) const = 0;

    /// @brief Returns true/false if the procedure is a system object
    /// @return a null value if is not determinable
    virtual const value_type& isSystemObject(void) const = 0;
};




typedef std::vector<ITable::ptr>      TableList;
typedef std::vector<IDatatype::ptr>   DatatypeList;
typedef std::vector<IView::ptr>       ViewList;
typedef std::vector<ISchema::ptr>     SchemaList;
typedef std::vector<ICatalog::ptr>    CatalogList;
typedef std::vector<IProcedure::ptr>  ProcedureList;
typedef std::vector<IIndex::ptr>      IndexList;





//------------------------------------------------------------------------------
///
/// @brief Interface for Table Filters
class DBWTL_EXPORT ITableFilter : public IDALObject
{
public:
    virtual ~ITableFilter(void) { }

//     virtual std::wstring simpleNameFilter(void) const = 0;
//     virtual std::wstring simpleCatalogFilter(void) const = 0;
//     virtual std::wstring simpleSchemaFilter(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief Empty Table Filter implementation
class DBWTL_EXPORT EmptyTableFilter : public ITableFilter
{
public:
    virtual ~EmptyTableFilter(void) { }
};



//------------------------------------------------------------------------------
///
/// @brief Interface for View Filters
class DBWTL_EXPORT IViewFilter : public IDALObject
{
public:
    virtual ~IViewFilter(void) { }
};


//------------------------------------------------------------------------------
///
/// @brief Empty View Filter implementation
class DBWTL_EXPORT EmptyViewFilter : public IViewFilter
{
public:
    virtual ~EmptyViewFilter(void) { }
};


//------------------------------------------------------------------------------
///
/// @brief Interface for Schema Filters
class DBWTL_EXPORT ISchemaFilter : public IDALObject
{
public:
    virtual ~ISchemaFilter(void) { }
};


//------------------------------------------------------------------------------
///
/// @brief Empty Schema Filter implementation
class DBWTL_EXPORT EmptySchemaFilter : public ISchemaFilter
{
public:
    virtual ~EmptySchemaFilter(void) { }
};


//------------------------------------------------------------------------------
///
/// @brief Interface for Catalog Filters
class DBWTL_EXPORT ICatalogFilter : public IDALObject
{
public:
    virtual ~ICatalogFilter(void) { }
};


//------------------------------------------------------------------------------
///
/// @brief Empty Catalog Filter implementation
class DBWTL_EXPORT EmptyCatalogFilter : public ICatalogFilter
{
public:
    virtual ~EmptyCatalogFilter(void) { }
};


//------------------------------------------------------------------------------
///
/// @brief Interface for Procedure Filters
class DBWTL_EXPORT IProcedureFilter : public IDALObject
{
public:
    virtual ~IProcedureFilter(void) { }
};


//------------------------------------------------------------------------------
///
/// @brief Empty Procedure Filter implementation
class DBWTL_EXPORT EmptyProcedureFilter : public IProcedureFilter
{
public:
    virtual ~EmptyProcedureFilter(void) { }
};



//------------------------------------------------------------------------------
///
/// @brief Interface for Index Filters
class DBWTL_EXPORT IIndexFilter : public IDALObject
{
public:
    virtual ~IIndexFilter(void) { }
};


//------------------------------------------------------------------------------
///
/// @brief Empty Index Filter implementation
class DBWTL_EXPORT EmptyIndexFilter : public IIndexFilter
{
public:
    virtual ~EmptyIndexFilter(void) { }
};




//------------------------------------------------------------------------------
///
/// @brief Interface for Datatype Filters
class DBWTL_EXPORT IDatatypeFilter : public IDALObject
{
public:
    virtual ~IDatatypeFilter(void) { }

//     virtual std::wstring simpleNameFilter(void) const = 0;
//     virtual std::wstring simpleCatalogFilter(void) const = 0;
//     virtual std::wstring simpleSchemaFilter(void) const = 0;
};



//------------------------------------------------------------------------------
///
/// @brief Empty Datatype Filter implementation
class DBWTL_EXPORT EmptyDatatypeFilter : public IDatatypeFilter
{
public:
    virtual ~EmptyDatatypeFilter(void) { }
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for Environments
class DBWTL_EXPORT IEnv : public IHandle
{
public:
    //typedef std::auto_ptr<IEnv> ptr;
    typedef utils::SmartPtr<IEnv, utils::RefCounted, utils::AllowConversion> ptr;

    virtual ~IEnv(void) { }

    virtual IDbc* newConnection(void) = 0;

    virtual dal_engine getEngineType(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for connections
class DBWTL_EXPORT IDbc : public IHandle
{
public:
    //typedef std::auto_ptr<IDbc>                      ptr;
    typedef utils::SmartPtr<IDbc, utils::RefCounted, utils::AllowConversion> ptr;
    typedef std::map<String, String>   Options;


    virtual ~IDbc(void);

	virtual IEnv& getEnv(void) = 0;

    ///
    /// @brief Connect to a database by name, user and password
    ///
    /// The value of database depends on the selected driver.
    /// Some drivers (filebased) requires a path to a directory or file
    /// and other drivers needs the name of the database.
    virtual void     connect(String database,
                             String user = String(),
                             String password = String()) = 0;


    ///
    /// @brief Connect to a database by options
    virtual void     connect(IDbc::Options& options) = 0;



    virtual bool           isConnected(void) const = 0;
    virtual void           disconnect(void) = 0;
    virtual String         driverName(void) const = 0;
    virtual String         dbmsName(void) const = 0;
    virtual IStmt*         newStatement(void) = 0;
    virtual IDALDriver*    drv(void) const = 0;

    virtual void           beginTrans(trx_mode mode,
                                      access_mode access = trx_default,
                                      String name = String()) = 0;
    virtual void           commit(void) = 0;
    virtual void           savepoint(String name) = 0;
    virtual void           rollback(String name = String()) = 0;

    virtual void           commit(Transaction trx) = 0;
    virtual void           rollback(Transaction trx) = 0;


    virtual void           directCmd(String cmd) = 0;
    virtual std::string    getDbcEncoding(void) const = 0;

    virtual IMetadata*     newMetadata(void) = 0;

    virtual TableList      getTables(const ITableFilter& = EmptyTableFilter()) = 0;
    virtual DatatypeList   getDatatypes(const IDatatypeFilter& = EmptyDatatypeFilter()) = 0;
    virtual ViewList       getViews(const IViewFilter& = EmptyViewFilter()) = 0;
    virtual SchemaList     getSchemas(const ISchemaFilter& = EmptySchemaFilter()) = 0;
    virtual CatalogList    getCatalogs(const ICatalogFilter& = EmptyCatalogFilter()) = 0;
    virtual ProcedureList  getProcedures(const IProcedureFilter& = EmptyProcedureFilter()) = 0;
    virtual IndexList      getIndices(const IIndexFilter& = EmptyIndexFilter()) = 0;
    //virtual ProcColumnList getProcColumns(const IProcColumnFilter& = EmptyProcColumnFilter()) = 0;

    virtual Variant        getCurrentCatalog(void) { return String("DBNAME-FIXME"); } /// @bug implement for all

protected:
    virtual void           setDbcEncoding(std::string encoding) = 0;
};



//------------------------------------------------------------------------------
///
/// @brief DAL Interface for datasets
class DBWTL_EXPORT IDataset : public IDALObject
{
public:
    //typedef std::auto_ptr<IResult> ptr;
    typedef utils::SmartPtr<IDataset, utils::RefCounted, utils::AllowConversion> ptr;
    typedef size_t                      bookmark_type;
    typedef Variant                     value_type;
    /// @todo check if row_type is required
    typedef std::vector<value_type*>    row_type;


    virtual ~IDataset(void) {}


    virtual bool   isBad(void) const = 0;

    virtual void   first(void) = 0;
    virtual bool   next(void) = 0;
    virtual bool   eof(void) const = 0;
    virtual bool   isOpen(void) const = 0;
    virtual bool   isPositioned(void) const = 0;
    virtual void   close(void) = 0;

    virtual rowcount_t       rowCount(void) const = 0;

    virtual const value_type&      column(colnum_t num) = 0;
    virtual const value_type&      column(String name) = 0;

    // alternate methods when overloaded methods are problematic
    const value_type& columnByNumber(colnum_t num);
    const value_type& columnByName(const String &name);

    // column methods
    virtual size_t           columnCount(void) const = 0;
    virtual colnum_t         columnID(String name) const = 0;
    virtual String           columnName(colnum_t num) const = 0;
    //virtual const ITypeInfo& datatype(colnum_t num) const = 0;

    /// @brief Returns the column descriptor for the given column number
    virtual const IColumnDesc& describeColumn(colnum_t num) const = 0;

    /// @brief Returns the column descriptor for the given column name
    virtual const IColumnDesc& describeColumn(String name) const = 0;


    //virtual IDALDriver* drv(void) const = 0;
};




class DBWTL_EXPORT IDataProvider : public IDataset
{
public:
	virtual ~IDataProvider(void) {}

    typedef enum { MODE_READ } open_mode;

    virtual void open(open_mode mode, 
                      const String &table,
                      const String &schema = String(),
	              const String &database = String()) = 0;

	//virtual void post(void) = 0;
    virtual IDALDriver* drv(void) const = 0;
 };
 
 
class DBWTL_EXPORT IResult : public IDataset
{
public:
	virtual ~IResult(void);
	typedef utils::SmartPtr<IResult, utils::RefCounted, utils::AllowConversion> ptr;

	virtual bool   isPrepared(void) const = 0;
	virtual bool   isBad(void) const = 0;
	virtual size_t paramCount(void) const = 0;

	virtual Variant          lastInsertRowId(void) = 0;

	virtual rowcount_t       affectedRows(void) const = 0;


	virtual IDALDriver* drv(void) const = 0;
};


//------------------------------------------------------------------------------
///
/// @brief DAL Interface for statements
class DBWTL_EXPORT IStmt : public IHandle
{
public:
    typedef std::map<int, Variant*> ParamMapT;
    //typedef std::auto_ptr<IStmt> ptr;
    typedef utils::SmartPtr<IStmt, utils::RefCounted, utils::AllowConversion> ptr;

    virtual ~IStmt(void);

    virtual bool      isBad(void) const = 0;
    virtual void      prepare(String sql) = 0;
    virtual bool      isPrepared(void) const = 0;
    virtual void      execute(void) = 0;
    virtual void      execDirect(String sql) = 0;
    /*
      virtual void      execBatch(std::istream src, const char *charset) = 0;
      virtual void      execBatch(std::wistream src) = 0;
    */
    virtual void      close(void) = 0;

    virtual IResult&        resultset(void) = 0;
    virtual const IResult&  resultset(void) const = 0;

    virtual IDataProvider*  newProvider(void) = 0;

    //virtual void      setResultsetMode(ResultsetMode mode) = 0;

    virtual bool      nextResultset(void) = 0;
    virtual bool      moreResultsets(void) const = 0;

    virtual size_t    paramCount(void) const = 0;
    virtual void      bind(int num, Variant* data) = 0;
    virtual void      bind(int num, const Variant* data) = 0;
    virtual void      bind(int num, const Variant &data) = 0;
    virtual void      bind(int num, ByteStreamBuf *data) = 0;
    virtual void      bind(int num, UnicodeStreamBuf *data) = 0;

    virtual rowcount_t  affectedRows(void) const = 0;

    virtual Variant   lastInsertRowId(void) = 0;


    virtual IDALDriver* drv(void) const = 0;
    //virtual IDbc& getConnection(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Factory
class DBWTL_EXPORT Factory
{
public:
    template<class T> DBWTL_EXPORT static typename T::ENV* create(String driver)
    {
        typename T::ENV* env = T::createEnv(driver);
        return env;
    }

    //static IEnv* createByName(std::wstring name, std::wstring driver); not needed
};





//--------------------------------------------------------------------------
/// Base implementation for IDiagnostic
///
/// @since 0.0.1
/// @brief Base implementation for IDiagnostic
class DBWTL_EXPORT DiagBase : public IDiagnostic
{
public:
    virtual ~DiagBase(void)
    {}

    DiagBase(dalstate_t state,
             CodePosInfo pos,
             Variant what,
             String description);
    
    DiagBase(const DiagBase& ref);



    virtual dalstate_t         getState(void) const;
//     virtual const Variant&     getQuery(void) const;
//     virtual const Variant&     getNativeErrorCode(void) const;
    virtual const Variant&     getMsg(void) const;
    virtual const Variant&     getDescription(void) const;
    virtual const CodePosInfo&     getCodepos(void) const;
    virtual const Variant&     getSqlstate(void) const;
//     virtual const Variant&     getRowNumber(void) const;
//     virtual const Variant&     getServerName(void) const;
//     virtual const Variant&     getColumnNumber(void) const;
//     virtual const Variant&     getUsedFiles(void) const = 0;
//     virtual const Variant&     getSourcePos(void) const = 0;

protected:
    dalstate_t m_state;
    Variant m_sqlstate;
    CodePosInfo m_codepos;
    Variant m_message;
    Variant m_description;
};




//------------------------------------------------------------------------------
///
/// @brief Base implementation for IStmt
class DBWTL_EXPORT StmtBase : public IStmt
{
public:
    typedef std::map<int, Variant*>    ParamMap;
    typedef ParamMap::iterator         ParamMapIterator;


public:
    virtual void      bind(int num, Variant* data);
    virtual void      bind(int num, const Variant* data);
    virtual void      bind(int num, const Variant &data);
    virtual void      bind(int num, ByteStreamBuf *data);
    virtual void      bind(int num, UnicodeStreamBuf *data);

    virtual bool      isPrepared(void) const;

    virtual bool      isBad(void) const;

    virtual IDataProvider*  newProvider(void);

    //virtual void      setResultsetMode(ResultsetMode mode);

    virtual void      close(void);

    //virtual void      bind(std::wstring name, IVariant* data);
    //virtual void      bind(std::wstring name, PodVariant data);
    //virtual int       getParamNumberByName(std::wstring name) const = 0;


    virtual void             setOption(std::string name, const Variant &data);
    virtual const Variant&   getOption(std::string name) const;

    virtual ~StmtBase(void)
    {}

protected:
    StmtBase(void);


    /** @brief Variants* passed by user */
    ParamMap                  m_params;
    /** @brief Temp. POD Variants, destroyed by StmtBase */
    std::vector<Variant*>    m_temp_params;

    cursorstate_t m_cursorstate;
    options_type m_options;
};





//------------------------------------------------------------------------------
///
/// @brief Base implementation for IResult
class DBWTL_EXPORT ResultBase : public IResult
{
public:
    virtual bool      isPrepared(void) const;
    virtual bool      isBad(void) const;
    virtual bool      isOpen(void) const;
    virtual bool      isPositioned(void) const;

    virtual ~ResultBase(void)
    {}

protected:

    ResultBase(void) : m_cursorstate(DAL_CURSOR_CLOSED)
    {}

    cursorstate_t m_cursorstate;
};





//------------------------------------------------------------------------------
///
/// 
class DBWTL_EXPORT EnvBase : public IEnv
{
public:
    virtual void             setOption(std::string name, const Variant &data);
    virtual const Variant&   getOption(std::string name) const;

protected:
    EnvBase(void);

    virtual ~EnvBase(void)
    {}
            

    options_type m_options;
};



//------------------------------------------------------------------------------
///
/// 
class DBWTL_EXPORT DbcBase : public IDbc
{
public:
    virtual bool      isConnected(void) const;
    virtual bool      isBad(void) const;

    virtual void             setOption(std::string name, const Variant &data);
    virtual const Variant&   getOption(std::string name) const;

    virtual IMetadata*     newMetadata(void) { throw std::runtime_error("not implemented"); }

    virtual ~DbcBase(void)
    {}

protected:

    DbcBase(void);

    bool    m_isConnected;
    bool    m_isBad;
    options_type m_options;
};




enum columndesc_entry_enum
{
	DBWTL_COLUMNDESC_NAME,
	DBWTL_COLUMNDESC_CATALOG_NAME,
	DBWTL_COLUMNDESC_SCHEMA_NAME,
	DBWTL_COLUMNDESC_BASE_COLUMN_NAME,
	DBWTL_COLUMNDESC_TYPE_NAME,
	DBWTL_COLUMNDESC_BASE_TABLE_NAME,
	DBWTL_COLUMNDESC_SIZE,
	DBWTL_COLUMNDESC_COMMENT,
	DBWTL_COLUMNDESC_IS_NULLABLE,
	DBWTL_COLUMNDESC_PRECISION,
	DBWTL_COLUMNDESC_SCALE,
	DBWTL_COLUMNDESC_IS_SEARCHABLE
};

typedef enum columndesc_entry_enum ColumnDescEntry;

//------------------------------------------------------------------------------
///
///
class DBWTL_EXPORT ColumnDescBase : public IColumnDesc
{
public:
    virtual const IColumnDesc::value_type& getName(void) const;
    virtual const IColumnDesc::value_type& getCatalogName(void) const;
    virtual const IColumnDesc::value_type& getSchemaName(void) const;
    virtual const IColumnDesc::value_type& getBaseColumnName(void) const;
    virtual const IColumnDesc::value_type& getTypeName(void) const;
    virtual const IColumnDesc::value_type& getBaseTableName(void) const;
    virtual const IColumnDesc::value_type& getSize(void) const;
    virtual const IColumnDesc::value_type& getComment(void) const;
    virtual const IColumnDesc::value_type& getIsNullable(void) const;
    virtual const IColumnDesc::value_type& getPrecision(void) const;
    virtual const IColumnDesc::value_type& getScale(void) const;
    virtual const IColumnDesc::value_type& getIsSearchable(void) const;

    virtual daltype_t getDatatype(void) const;



protected:
    ColumnDescBase(void);

    virtual ~ColumnDescBase(void);

    Variant m_name;
    Variant m_catalog_name;
    Variant m_schema_name;
    Variant m_base_column_name;
    Variant m_type_name;
    Variant m_base_table_name;
    Variant m_size;
    Variant m_comment;
    Variant m_is_nullable;
    Variant m_precision;
    Variant m_scale;
    Variant m_is_searchable;

    daltype_t m_daltype;
};


class DBWTL_EXPORT ColumnDesc : public ColumnDescBase
{
public:
	ColumnDesc(void);
	ColumnDesc(const IColumnDesc &orig);
	virtual ~ColumnDesc(void)
	{}

	virtual void changeEntry(ColumnDescEntry entry, const IColumnDesc::value_type &v);
	virtual void changeType(daltype_t daltype);
};


//------------------------------------------------------------------------------
///
///
class DBWTL_EXPORT DatatypeBase : public IDatatype
{
public:
    virtual const value_type& getName(void) const;
    virtual const value_type& getSize(void) const;
    virtual const value_type& getLiteralPrefix(void) const;
    virtual const value_type& getLiteralSuffix(void) const;
    virtual const value_type& getIsUnsigned(void) const;
    virtual daltype_t         daltype(void) const;

protected:
    DatatypeBase(void);

    virtual ~DatatypeBase(void);

    Variant m_name;
    Variant m_size;
    Variant m_literal_prefix;
    Variant m_literal_suffix;
    Variant m_is_unsigned;

    daltype_t m_daltype;
};


struct MetadataColumnDescriptor /// @todo transfer changes to other engines!
{
	std::string name;
	daltype_t daltype;
    int size;
    bool nullable;
};


template<typename T>
bool ColumnMatchFilter<T>::operator()(IDataset &ds) const
{
	if(m_value.isnull() || ds.column(this->m_column).isnull())
			return false;
	throw 1;
	//return m_value.get<T>() == ds.column(this->m_column).get<T>();
}


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
