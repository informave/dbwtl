//
// db_objects.hh - Public DB Objects
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
/// @brief Public DB Objects
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DBOBJECTS_HH
#define INFORMAVE_DB_DBOBJECTS_HH


#include "db_fwd.hh"
#include "ustring.hh"
#include "ustreambuf.hh"

#include "variant.hh"


//#include "db_exceptions.hh"
#include "dal/dal_interface.hh"

#include <iosfwd>




DB_NAMESPACE_BEGIN


struct default_tag { };
struct extended_api_tag { };




//------------------------------------------------------------------------------
///
/// @brief SQLSTATE template
template<int T, class U = informave::db::ex::sqlstate_exception>
struct sqlstate : public U
{
    sqlstate(const IDiagnostic& diag_to_clone)
        : U(diag_to_clone)
    {}
    
    sqlstate(const sqlstate& orig) : U(orig)
    {}

    virtual ~sqlstate(void) throw()
    {}
};




//------------------------------------------------------------------------------
///
/// @brief Basic SQLSTATE's (SQL:2008)
struct basic_states
{
    virtual ~basic_states(void)
    {}

    typedef enum
    {
        DAL_SQLSTATE_07000, // dynamic SQL error
        DAL_SQLSTATE_07001, // using clause does not match dynamic parameter specification
        DAL_SQLSTATE_07002, // using clause does not match target specification
        DAL_SQLSTATE_07003, // cursor specification cannot be executed
        DAL_SQLSTATE_07004, // using clause required for dynamic parameters
        DAL_SQLSTATE_07005, // prepared statement not a cursor specification
        DAL_SQLSTATE_07006, // restricted data type attribute violation
        DAL_SQLSTATE_07007, // using clause required for result fields
        DAL_SQLSTATE_07008, // invalid descriptor count
        DAL_SQLSTATE_07009, // invalid descriptor index
        DAL_SQLSTATE_0700B, // data type transform function violation
        DAL_SQLSTATE_0700C, // undefined DATA value
        DAL_SQLSTATE_0700D, // invalid DATA target
        DAL_SQLSTATE_0700E, // invalid LEVEL value
        DAL_SQLSTATE_0700F, // invalid DATETIME_INTERVAL_CODE
        DAL_SQLSTATE_08000, // connection exception
        DAL_SQLSTATE_08001, // SQL-client unable to establish SQL-connection
        DAL_SQLSTATE_08002, // connection name in use
        DAL_SQLSTATE_08003, // connection does not exist
        DAL_SQLSTATE_08004, // SQL-server rejected establishment of SQL-connection
        DAL_SQLSTATE_08006, // connection failure
        DAL_SQLSTATE_08007, // transaction resolution unknown
        DAL_SQLSTATE_09000, // trigger action exception
        DAL_SQLSTATE_0A000, // feature not supported
        DAL_SQLSTATE_0A001, // multiple server transactions
        DAL_SQLSTATE_0D000, // invalid target type specification
        DAL_SQLSTATE_0E000, // invalid schema name list specification
        DAL_SQLSTATE_0F000, // locator exception
        DAL_SQLSTATE_0F001, // invalid specification
        DAL_SQLSTATE_0L000, // invalid grantor
        DAL_SQLSTATE_0M000, // invalid SQL-invoked procedure reference
        DAL_SQLSTATE_0P000, // invalid role specification
        DAL_SQLSTATE_0S000, // invalid transform group name specification
        DAL_SQLSTATE_0T000, // target table disagrees with cursor specification
        DAL_SQLSTATE_0U000, // attempt to assign to non-updatable column
        DAL_SQLSTATE_0V000, // attempt to assign to ordering column
        DAL_SQLSTATE_0W000, // prohibited statement encountered during trigger execution
        DAL_SQLSTATE_0Z000, // diagnostics exception
        DAL_SQLSTATE_0Z001, // maximum number of stacked diagnostics areas exceeded
        DAL_SQLSTATE_21000, // cardinality violation
        DAL_SQLSTATE_22000, // data exception
        DAL_SQLSTATE_22001, // string data, right truncation
        DAL_SQLSTATE_22002, // null value, no indicator parameter
        DAL_SQLSTATE_22003, // numeric value out of range
        DAL_SQLSTATE_22004, // null value not allowed
        DAL_SQLSTATE_22005, // error in assignment
        DAL_SQLSTATE_22006, // invalid interval format
        DAL_SQLSTATE_22007, // invalid datetime format
        DAL_SQLSTATE_22008, // datetime field overflow
        DAL_SQLSTATE_22009, // invalid time zone displacement value
        DAL_SQLSTATE_2200B, // escape character conflict
        DAL_SQLSTATE_2200C, // invalid use of escape character
        DAL_SQLSTATE_2200D, // invalid escape octet
        DAL_SQLSTATE_2200E, // null value in array target
        DAL_SQLSTATE_2200F, // zero-length character string
        DAL_SQLSTATE_2200G, // most specific type mismatch
        DAL_SQLSTATE_2200H, // sequence generator limit exceeded
        DAL_SQLSTATE_2200P, // interval value out of range
        DAL_SQLSTATE_2200Q, // multiset value overflow
        DAL_SQLSTATE_22010, // invalid indicator parameter value
        DAL_SQLSTATE_22011, // substring error
        DAL_SQLSTATE_22012, // division by zero
        DAL_SQLSTATE_22013, // invalid preceding or following size in window function
        DAL_SQLSTATE_22014, // invalid argument for NTILE function
        DAL_SQLSTATE_22015, // interval field overflow
        DAL_SQLSTATE_22016, // invalid argument for NTH_VALUE function
        DAL_SQLSTATE_22018, // invalid character value for cast
        DAL_SQLSTATE_22019, // invalid escape character
        DAL_SQLSTATE_2201B, // invalid regular expression
        DAL_SQLSTATE_2201C, // null row not permitted in table
        DAL_SQLSTATE_2201E, // invalid argument for natural logarithm
        DAL_SQLSTATE_2201F, // invalid argument for power function
        DAL_SQLSTATE_2201G, // invalid argument for width bucket function
        DAL_SQLSTATE_2201S, // invalid XQuery regular expression
        DAL_SQLSTATE_2201T, // invalid XQuery option flag
        DAL_SQLSTATE_2201U, // attempt to replace a zero-length string
        DAL_SQLSTATE_2201V, // invalid XQuery replacement string
        DAL_SQLSTATE_2201W, // invalid row count in fetch first clause
        DAL_SQLSTATE_2201X, // invalid row count in result offset clause
        DAL_SQLSTATE_22021, // character not in repertoire
        DAL_SQLSTATE_22022, // indicator overflow
        DAL_SQLSTATE_22023, // invalid parameter value
        DAL_SQLSTATE_22024, // unterminated C string
        DAL_SQLSTATE_22025, // invalid escape sequence
        DAL_SQLSTATE_22026, // string data, length mismatch
        DAL_SQLSTATE_22027, // trim error
        DAL_SQLSTATE_22029, // noncharacter in UCS string
        DAL_SQLSTATE_2202D, // null value substitued for mutator subject parameter
        DAL_SQLSTATE_2202E, // array element error
        DAL_SQLSTATE_2202F, // array data, right truncation
        DAL_SQLSTATE_2202G, // invalid repeat argument in a sample clause
        DAL_SQLSTATE_2202H, // invalid sample size
        DAL_SQLSTATE_23000, // integrity constraint violation
        DAL_SQLSTATE_23001, // restrict violation
        DAL_SQLSTATE_24000, // invalid cursor state
        DAL_SQLSTATE_25000, // invalid transaction state
        DAL_SQLSTATE_25001, // active SQL-transaction
        DAL_SQLSTATE_25002, // branch transaction already active
        DAL_SQLSTATE_25003, // inappropriate access mode for branch transaction
        DAL_SQLSTATE_25004, // inappropriate isolation level for branch transaction
        DAL_SQLSTATE_25005, // no active SQL-transaction for branch transaction
        DAL_SQLSTATE_25006, // read-only SQL-transaction
        DAL_SQLSTATE_25007, // schema and data statement mixing not supported
        DAL_SQLSTATE_25008, // held cursor requires same isolation level
        DAL_SQLSTATE_26000, // invalid SQL statement name
        DAL_SQLSTATE_27000, // riggered data change violation
        DAL_SQLSTATE_28000, // invalid authorization specification
        DAL_SQLSTATE_2B000, // dependent privilege descriptors still exist
        DAL_SQLSTATE_2C000, // invalid character set name
        DAL_SQLSTATE_2D000, // invalid transaction termination
        DAL_SQLSTATE_2E000, // invalid connection name
        DAL_SQLSTATE_2F000, // SQL routine exception
        DAL_SQLSTATE_2F002, // modifying SQL-data not permitted
        DAL_SQLSTATE_2F003, // prohibited SQL-statement attempted
        DAL_SQLSTATE_2F004, // reading SQL-data not permitted
        DAL_SQLSTATE_2F005, // function executed no return statement
        DAL_SQLSTATE_2H000, // invalid collation name
        DAL_SQLSTATE_30000, // invalid SQL statement identifier
        DAL_SQLSTATE_33000, // invalid SQL descriptor name
        DAL_SQLSTATE_34000, // invalid cursor name
        DAL_SQLSTATE_35000, // invalid condition number
        DAL_SQLSTATE_36000, // cursor sensitivity exception
        DAL_SQLSTATE_36001, // request rejected
        DAL_SQLSTATE_36002, // request failed
        DAL_SQLSTATE_38000, // external routine exception
        DAL_SQLSTATE_38001, // containg SQL not permitted
        DAL_SQLSTATE_38002, // modifying SQL-data not permitted
        DAL_SQLSTATE_38003, // prohibited SQL-statement attempted
        DAL_SQLSTATE_38004, // reading SQL-data not permitted
        DAL_SQLSTATE_39000, // external routine invocation exception
        DAL_SQLSTATE_39004, // null value not allowed
        DAL_SQLSTATE_3B000, // savepoint exception
        DAL_SQLSTATE_3B001, // invalid specification
        DAL_SQLSTATE_3B002, // too many savepoints
        DAL_SQLSTATE_3C000, // ambigous cursor name
        DAL_SQLSTATE_3D000, // invalid catalog name
        DAL_SQLSTATE_3F000, // invalid schema name
        DAL_SQLSTATE_42000, // syntax error or access rule violation
        DAL_SQLSTATE_44000, // with check option violation
        DAL_SQLSTATE_50000, // transaction rollback
        DAL_SQLSTATE_50001, // serialization failure
        DAL_SQLSTATE_50002, // integrity constraint violation
        DAL_SQLSTATE_50003, // statement completion unknown
        DAL_SQLSTATE_50004, // triggered action exception

        DAL_SQLSTATE_HY000, // 
        DAL_SQLSTATE_LAST
    } states_t;

};



//------------------------------------------------------------------------------
///
/// @brief get the value from variant or (if null) return second argument
template<typename T>
struct ifnull
{
    /// requested type
    typedef T value_type;

    /// @param[in] var A IVariant reference
    /// @param[in] def Default value which is returned if var is null
    ifnull(const Variant &var, value_type def) : m_var(var), m_def(def)
    {}

    /// cast operator to the requested type
    operator value_type() const
    { return m_var.isnull() ? m_def : m_var.get<value_type>(); }

private:
    const Variant       &m_var;
    value_type           m_def;
};



/// @brief Helper operator for ifnull
template<typename T>
DBWTL_EXPORT inline std::wostream&  operator<<(std::wostream& o, const ifnull<T> &value)
{
    T tmp = value;
    return o << tmp;
}

/// @brief Helper operator for ifnull
template<typename T>
DBWTL_EXPORT inline std::ostream&  operator<<(std::ostream& o, const ifnull<T> &value)
{
    T tmp = value;
    return o << tmp;
}








struct basic_datatypes
{
    virtual ~basic_datatypes(void)
    {}

//     typedef signed char           Char; // these two types are covered by String!
//     typedef unsigned char         UChar;

/*
    typedef std::wstreambuf*      Memo;
    typedef std::streambuf*       Blob; // better use Blob with implicit ctor?
*/
    typedef db::String            String;
    typedef db::BlobStream             BlobStream;
    typedef db::MemoStream             MemoStream;
    typedef db::Blob		Blob;
    typedef db::Memo		Memo;
    typedef TNumeric              Numeric;
    typedef signed short          Smallint;
    typedef unsigned short        USmallint;
    typedef signed int            Integer;
    typedef unsigned int          UInteger;
    typedef signed long long      Bigint;
    typedef unsigned long long    UBigint;
    typedef float                 Float;
    // real
    typedef double                Double;
    typedef bool                  Boolean;
    typedef TDate                 Date;
    typedef TTime                 Time;
    typedef TTimestamp            Timestamp;
    typedef TInterval             Interval;


    /*
    typedef TCidr            CIDR;
    typedef TMacaddr         MACAddr;
    typedef TInetaddr        INETAddr;
    typedef TUuid            UUID;
    typedef TXml             XML;
    typedef TDatetime        Datetime;
    */

};





//------------------------------------------------------------------------------
///
/// @brief Database traits class
template<typename Engine, typename tag = default_tag>
struct db_traits {};





//------------------------------------------------------------------------------
///
///  @brief Database Environment
template<typename Engine,
         typename tag,
         typename EnvInterface = IEnv>
class Environment : public EnvInterface
{
private:
    typedef typename db_traits<Engine, tag>::dal_dbc_type     dal_dbc_type;
    typedef typename db_traits<Engine, tag>::dal_env_type     dal_env_type;
    typedef typename db_traits<Engine, tag>::dal_diag_type    dal_diag_type;
    typedef typename db_traits<Engine, tag>::dal_variant_type dal_variant_type;

public:
    Environment(String str)
        : EnvInterface(),
          m_env( Factory::create<Engine>(str) )
    {}
/*
    Environment(std::string str) 
        : EnvInterface(),
          m_env( Factory::create<Engine>(i18n::conv_from(str, "UTF-8") ) )
    {}
   */

    virtual ~Environment(void)
    {}


    virtual dal_dbc_type* newConnection(void) { return this->m_env->newConnection(); }

    virtual void setOption(std::string name, const dal_variant_type &data)
    { this->m_env->setOption(name, data); }

    virtual const dal_variant_type& getOption(std::string name) const
    { return this->m_env->getOption(name); }

    virtual bool                    diagAvail(void) const   { return this->m_env->diagAvail(); }
    virtual const dal_diag_type&    fetchDiag(void)         { return this->m_env->fetchDiag(); }

    virtual dal_env_type*           getImpl(void)           { return this->m_env.get(); }

    virtual dal_engine getEngineType(void) const       { return this->m_env->getEngineType(); }

protected:
    typename dal_env_type::ptr m_env;

private:
    Environment(const Environment&);
    Environment& operator=(const Environment&);
};





//------------------------------------------------------------------------------
///
/// @brief Database Connection
template<typename Engine, typename tag, typename ConnectionInterface = IDbc>
class Connection : public ConnectionInterface
{
private:
    typedef typename db_traits<Engine, tag>::dal_dbc_type      dal_dbc_type;
    typedef typename db_traits<Engine, tag>::dal_stmt_type     dal_stmt_type;
    typedef typename db_traits<Engine, tag>::dal_metadata_type     dal_metadata_type;
    typedef typename db_traits<Engine, tag>::dal_diag_type     dal_diag_type;
    typedef typename db_traits<Engine, tag>::dal_variant_type  dal_variant_type;



public:
    Connection( typename db_traits<Engine, tag>::environment_type &env )
        : ConnectionInterface(),
          m_dbc( env.newConnection() )
    {}

    virtual ~Connection(void)
    {}

    typedef IDbc::Options     Options;

	virtual IEnv& getEnv(void)
	{
		return this->m_dbc->getEnv();
	}


    virtual void   connect(String database,
                           String user = String(),
                           String password = String())
    { this->m_dbc->connect(database, user, password); }


    virtual void   connect(IDbc::Options& options)
    { this->m_dbc->connect(options); }


    virtual bool              isConnected(void) const       { return this->m_dbc->isConnected(); }

    virtual void              disconnect(void)              { this->m_dbc->disconnect(); }

    virtual String            driverName(void) const        { return this->m_dbc->driverName(); }

    virtual String            dbmsName(void) const          { return this->m_dbc->dbmsName(); }

    virtual dal_stmt_type*    newStatement(void)            { return this->m_dbc->newStatement(); }

    virtual IDALDriver*  drv(void) const               { return this->m_dbc->drv(); }

    virtual void              commit(void)                  { this->m_dbc->commit(); }

    virtual void              commit(Transaction trx)  { this->m_dbc->commit(trx); }

    virtual void              savepoint(String name)        { this->m_dbc->savepoint(name); }

    virtual void              directCmd(String cmd)         { this->m_dbc->directCmd(cmd); }

    virtual std::string       getDbcEncoding(void) const    { return this->m_dbc->getDbcEncoding(); }

    virtual void              rollback(String name = String()) { this->m_dbc->rollback(name); }

    virtual void              rollback(Transaction trx){ this->m_dbc->rollback(trx); }

    virtual void              beginTrans(trx_mode mode,
                                         access_mode access = trx_default,
                                         String name = String())
    { this->m_dbc->beginTrans(mode, access, name); }


    virtual dal_metadata_type* newMetadata(void) { return this->m_dbc->newMetadata(); }

    virtual TableList    getTables(const ITableFilter& filter = EmptyTableFilter())
    { return this->m_dbc->getTables(filter); }

    virtual ViewList     getViews(const IViewFilter& filter = EmptyViewFilter())
    { return this->m_dbc->getViews(filter); }

    virtual DatatypeList getDatatypes(const IDatatypeFilter& filter = EmptyDatatypeFilter())
    { return this->m_dbc->getDatatypes(filter); }

    virtual SchemaList   getSchemas(const ISchemaFilter& filter = EmptySchemaFilter())
    { return this->m_dbc->getSchemas(filter); }

    virtual CatalogList  getCatalogs(const ICatalogFilter& filter = EmptyCatalogFilter())
    { return this->m_dbc->getCatalogs(filter); }

    virtual ProcedureList getProcedures(const IProcedureFilter& filter = EmptyProcedureFilter())
    { return this->m_dbc->getProcedures(filter); }

    virtual IndexList    getIndices(const IIndexFilter& filter = EmptyIndexFilter())
    { return this->m_dbc->getIndices(filter); }


    virtual bool                   diagAvail(void) const    { return this->m_dbc->diagAvail(); }
    virtual const dal_diag_type&   fetchDiag(void)          { return this->m_dbc->fetchDiag(); }


    virtual void                    setOption(std::string name, const dal_variant_type &data)
    { this->m_dbc->setOption(name, data); }

    virtual const dal_variant_type& getOption(std::string name) const
    { return this->m_dbc->getOption(name); }


    virtual dal_dbc_type*           getImpl(void)   { return this->m_dbc.get(); }

	virtual Variant         getCurrentCatalog(void) { return this->m_dbc->getCurrentCatalog(); }

protected:
    virtual void           setDbcEncoding(std::string encoding)
    {
        //return this->m_dbc->setDbcEncoding(encoding); // FIX Protcted error
    }


protected:
    typename dal_dbc_type::ptr m_dbc;

private:
    Connection(const Connection&);
    Connection& operator=(const Connection&);
};









//------------------------------------------------------------------------------
///
/// @brief Database Statement
template<typename Engine, typename tag, typename StmtInterface = IStmt>
class Statement : public StmtInterface
{
private:
    typedef typename db_traits<Engine, tag>::dal_resultset_type  dal_resultset_type;
    typedef typename db_traits<Engine, tag>::dal_stmt_type       dal_stmt_type;
    typedef typename db_traits<Engine, tag>::dal_diag_type       dal_diag_type;
    typedef typename db_traits<Engine, tag>::dal_variant_type    dal_variant_type;


public:
    Statement( typename db_traits<Engine, tag>::connection_type &dbc )
        : StmtInterface(),
          m_stmt( dbc.newStatement() ),
          m_bind_strings()
    {}

    virtual ~Statement(void)
    {
    	this->close();
    }


    virtual bool      isBad(void) const                   { return this->m_stmt->isBad(); }

    virtual void      prepare(String sql)                 { return this->m_stmt->prepare(sql); }

    virtual bool      isPrepared(void) const              { return this->m_stmt->isPrepared(); }

    virtual void      execute(void)                       { this->m_stmt->execute(); }

    virtual void      execDirect(String sql)              { this->m_stmt->execDirect(sql); }

    virtual void      close(void)                              
    { 
    	this->m_stmt->close(); 
        for(std::vector<String*>::iterator i(this->m_bind_strings.begin());
            i != this->m_bind_strings.end();
            ++i)
		{
			delete *i;
			*i = 0;
		}
		this->m_bind_strings.clear();
    }

    virtual IDataProvider*  newProvider(void)                   { return this->m_stmt->newProvider(); }

    virtual dal_resultset_type&        resultset(void)          { return this->m_stmt->resultset(); }

    virtual const dal_resultset_type&  resultset(void) const    { return this->m_stmt->resultset(); }

    virtual bool      nextResultset(void)                       { return this->m_stmt->nextResultset(); }

    virtual bool      moreResultsets(void) const                { return this->m_stmt->moreResultsets();}

    virtual size_t    paramCount(void) const                    { return this->m_stmt->paramCount(); }

    
    template<class A, class B, class C> void bind(int num, const std::basic_string<A, B, C>& data)
    {
        this->bind(num, Variant(String(data)));
    }

    template<class A, class B, class C> void bind(int num, std::basic_string<A, B, C>* data)
    {
        //std::cout << "binding non-const" << std::endl;
        String *s = new String();
        this->m_bind_strings.push_back(s);
        s->sync_with(data);
        this->bind(num, Variant(s));
    }
    
    template<class A, class B, class C> void bind(int num, const std::basic_string<A, B, C>* data)
    {
        //std::cout << "binding const" << std::endl;
        String *s = new String();
        this->m_bind_strings.push_back(s);
        s->sync_with(data);
        this->bind(num, Variant(static_cast<const String*>(s)));
    }



    virtual void      bind(int num, ByteStreamBuf *data)       { this->m_stmt->bind(num, data); }

    virtual void      bind(int num, UnicodeStreamBuf *data)    { this->m_stmt->bind(num, data); }
    
    
    // template<class A, class B> void bind(int num, std::basic_streambuf<A, B>* data)
    // {
    //     streambuf_convert<char> *cvtbuf = new streambuf_convert<char>(data);
    //     std::wstreambuf *wsb = cvtbuf;
    //     this->m_stmt->bind(num, data); 
    // }


    // template<class B> void bind(int num, std::basic_streambuf<char, B>* data)
    // {}


    virtual void      bind(int num, Variant* data)         { this->m_stmt->bind(num, data); }

    virtual void      bind(int num, const Variant* data)   { this->m_stmt->bind(num, data); }

    virtual void      bind(int num, const Variant &data)   { this->m_stmt->bind(num, data); }



    virtual rowcount_t  affectedRows(void) const           { return this->m_stmt->affectedRows(); }

    virtual dal_variant_type lastInsertRowId(void)              { return this->m_stmt->lastInsertRowId(); }

    virtual IDALDriver* drv(void) const                    { return this->m_stmt->drv(); }

    // Diagnostic
    virtual bool                   diagAvail(void) const        { return this->m_stmt->diagAvail(); }
    virtual const dal_diag_type&   fetchDiag(void)              { return this->m_stmt->fetchDiag(); }

    // Options
    virtual void                    setOption(std::string name, const dal_variant_type &data)
    { this->m_stmt->setOption(name, data); }

    virtual const dal_variant_type& getOption(std::string name) const
    { return this->m_stmt->getOption(name); }


    virtual dal_stmt_type*          getImpl(void)               { return this->m_stmt.get(); }

protected:
    typename dal_stmt_type::ptr m_stmt;
	
	std::vector<String*> m_bind_strings;
//	std::vector<UnicodeStreamBuf>

private:
    Statement(const Statement&);
    Statement operator=(const Statement&);
};



class Bookmark
{
public:
    Bookmark(void) : x(0)
    {}

    Bookmark(int a) : x(a)
    {}

    bool operator<(const Bookmark &bm) const
    {
        return x < bm.x;

    }
    int x;
};



    template<typename T>
    class ValueFunctor
    {
    public:
        ValueFunctor(T p)
        {
        }
    };


   template<typename T>
   ValueFunctor<T> make_functor(T p)
   {
	return ValueFunctor<T>(p);
   }


//..............................................................................
////////////////////////////////////////////////////////////////////// ShrRecord
///
/// @since 0.0.1
/// @brief ShrRecord class
/// A ShrRecord object stores all column data into an internal buffer.
/// This buffer may be shared between other ShrRecord objects.
/// If you want a full independent copy of a ShrRecord, use the clone() method.
///
/// Implementation notes:
/// A ShrRecord object has no Bookmark information.
/// Managing bookmarks is part of a dataset implementation or database engine.
/// A single record object could not provide any useful with a Bookmark.
class DBWTL_EXPORT ShrRecord
{
public:
    typedef std::vector<Variant>     ColumnBuffer;

    /// @brief Construct empty Record
    ShrRecord(void);
    
    /// @brief Construct Record with ncol columns
    ShrRecord(size_t ncol);

    /// @brief Create a duplicate of the origin record.
    /// @note Both ShrRecord objects shares the same column buffer!
    ShrRecord(const ShrRecord& orig);

    /// @brief Construct Record from a list of Variant values (C++0x)
    #ifdef DBWTL_WITH_INIT_LISTS_CPP0X
    ShrRecord(const std::initializer_list<Variant> &values);
    #endif

    template<typename T, typename U>
    ShrRecord(T &obj, U fun, colnum_t lastColnum, colnum_t firstColnum = 1)
    	: m_data(new ShrRecord::ColumnBuffer())
    {
    	this->allocate(lastColnum);
    	for(colnum_t n = firstColnum; n <= lastColnum; ++n)
	{
		assert(n > 0); /// @bug throw exception
		(*this)[n-1] = fun(obj, n);
		//(*this)[n] = r(obj);
	}
    }

    /// @brief Construct Record from a column buffer
    /// @note The buffer data is copied into the record.
    ShrRecord(const ColumnBuffer &buf);

    /// @brief Clones a Record including the whole column buffer
    ShrRecord clone(void) const;

    /// @brief Returns a reference to the column data
    Variant&       operator[](size_t index);

    /// @brief Returns a const reference to the column data
    const Variant& operator[](size_t index) const;

    /// @brief Copy the raw data from rec
    ShrRecord& operator=(const ShrRecord& rec);
    
    /// @brief Allocate space for ncol columns
    inline void allocate(size_t ncol)
    {
    	this->m_data->resize(ncol);
    }

    /// @brief Returns the number of columns allocated
    inline size_t size(void) const
    {
    	return this->m_data->size();
    }

    /// @brief Clear data buffer, release allocated column space
    inline void clear(void)
    {
        this->m_data->clear();
    }

    /// @brief Returns the internal column buffer
    inline ColumnBuffer& getbuf(void)
    {
        return *this->m_data;
    }

    /// @brief  Returns the internal column buffer
    inline const ColumnBuffer& getbuf(void) const
    {
        return *this->m_data;
    }

protected:
    std::shared_ptr<ColumnBuffer>          m_data;
};



class DBWTL_EXPORT ScrollableDataset : public IDataset
{
public:
	virtual ~ScrollableDataset(void)
	{}

    virtual bool   last(void) = 0;
    virtual bool   setpos(rownum_t row) = 0;
    virtual bool   prev(void) = 0;
    
	virtual rowcount_t fetchMore(void) = 0;
	virtual bool moreAvail(void) = 0;

    // already declared by base class:
/*
    virtual void   first(void) = 0;
    virtual bool   next(void) = 0;
    virtual bool   eof(void) const = 0;
*/
};


//..............................................................................
////////////////////////////////////////////////////////////////////// RecordSet
///
/// @since 0.0.1
/// @brief RecordSet class
/// A RecordSet holds multiple ShrRecord objects. There are two typical
/// usecases:
///  * Create a new RecordSet and insert/update/delete records.
///    This is useful for independent Datasets like Metadata information
///    returned by a database engine implementation.
///  * Using a CachedResult for fetching records from a database.
///    Every fetched row gets stored by an internal RecordSet. After reading
///    all data, you can disconnect from the database. See CachedResult for
///    more information.
///
/// A standalone RecordSet object can be used to store values as a dataset.
/// This is sometime called a in-memory or temporary dataset.
class DBWTL_EXPORT RecordSet : public ScrollableDataset
{
public:
    RecordSet(void);
    virtual ~RecordSet(void);

    virtual bool   isBad(void) const;

    virtual void   open(void);
    virtual void   first(void);
    virtual bool   next(void);
    virtual bool   eof(void) const;
    virtual bool   isOpen(void) const;
    virtual bool   isPositioned(void) const;
    virtual void   close(void);

    // ScrollableDataset methods:
    virtual bool   last(void);
    virtual bool   setpos(rownum_t row);
    virtual bool   prev(void);
    virtual rowcount_t fetchMore(void) { return 0; }
    virtual bool moreAvail(void) { return false; }

    virtual rowcount_t       rowCount(void) const;

    virtual const value_type&      column(colnum_t num);
    virtual const value_type&      column(String name);

    // column methods
    virtual size_t           columnCount(void) const;
    virtual colnum_t         columnID(String name) const;
    virtual String           columnName(colnum_t num) const;
    //virtual const ITypeInfo& datatype(colnum_t num) const;

    /// @brief Returns the column descriptor for the given column number
    virtual const IColumnDesc& describeColumn(colnum_t num) const;

    /// @brief Returns the column descriptor for the given column name
    virtual const IColumnDesc& describeColumn(String name) const;


    typedef std::vector<ShrRecord>                   storage_type;
    typedef storage_type::iterator          iterator;
    typedef storage_type::const_iterator    const_iterator;


    typedef std::map<Bookmark, ShrRecord>            bookmark_map_type;

    storage_type        m_records;
    bookmark_map_type   m_bm_records;


    size_t m_count;

    cursorstate_t m_cursorstate;

    void clear(void);
    void reset(void);

    void setColumnCount(size_t num)
    {
        m_count = num;
	this->updateDescriptors();
    }

    void modifyColumnDesc(colnum_t num, ColumnDescEntry entry, const IColumnDesc::value_type &v);
    void setDatatype(colnum_t num, daltype_t daltype);



    //Record& allocRecord(const Bookmark &bm);

    iterator begin(void)
    {
    	return this->m_records.begin();
    }

    iterator end(void)
    {
    	return this->m_records.end();
    }

    const_iterator begin(void) const
    {
    	return this->m_records.begin();
    }

    const_iterator end(void) const
    {
    	return this->m_records.end();
    }


    void insert(const ShrRecord &rec);

protected:
    RecordSet::iterator m_cursor;
   
    std::vector<ColumnDesc>	m_column_descriptors;

    inline ColumnDesc& findDescriptor(colnum_t num)
    {
    	assert(num > 0);
	return this->m_column_descriptors.at(num-1);
    }


    inline const ColumnDesc& findDescriptor(colnum_t num) const
    {
    	assert(num > 0);
	return this->m_column_descriptors.at(num-1);
    }

    void updateDescriptors(void);


    inline storage_type& rows(void)
    {
        return m_records;
    }

    inline const storage_type& rows(void) const
    {
        return m_records;
    }
};


template<typename T>
struct DBWTL_EXPORT ColumnSortByNumber
{
    ColumnSortByNumber(size_t num)
        : m_num(num)
    {
    	assert(num > 0);
    }

    bool operator()(const ShrRecord &a, const ShrRecord &b)
    {
        return a[m_num-1].get<T>() < b[m_num-1].get<T>();
    }

    const size_t m_num;
};





class DBWTL_EXPORT CachedResultBase : public ScrollableDataset
{

private:
    typedef IStmt      statement_type;
    typedef  IResult  dal_resultset_type;
//    typedef typename db_traits<generic>::dal_columndesc_type dal_columndesc_type;
//    typedef typename db_traits<generic>::value_type          value_type;
//    typedef typename db_traits<generic>::dal_variant_type    dal_variant_type;


public:
    typedef int iterator;
    typedef int const_iterator;

    void begin();
    void end();


    CachedResultBase(void);
    virtual ~CachedResultBase(void);


    //Result(Statement &stmt);

    CachedResultBase(IStmt *result);


    //void attach(Statement &statement);//  { this->m_result = &statement.resultset(); }

    void attach(IStmt &statement);
    //{ this->m_source = &statement.resultset(); }

    bool sourceAvail(void) const;



    virtual bool   isBad(void) const;

    virtual void   open(void);
    virtual void   first(void);
    virtual bool   next(void);
    virtual bool   eof(void) const;
    virtual bool   isOpen(void) const;
    virtual bool   isPositioned(void) const;
    virtual void   close(void);

    // ScrollableDataset methods:
    virtual bool   last(void);
    virtual bool   setpos(rownum_t row);
    virtual bool   prev(void);
    virtual rowcount_t   fetchMore(void);
    virtual bool moreAvail(void);

    virtual rowcount_t       rowCount(void) const;

    virtual const value_type&      column(colnum_t num);
    virtual const value_type&      column(String name);

    // column methods
    virtual size_t           columnCount(void) const;
    virtual colnum_t         columnID(String name) const;
    virtual String           columnName(colnum_t num) const;
    //virtual const ITypeInfo& datatype(colnum_t num) const;

    /// @brief Returns the column descriptor for the given column number
    virtual const IColumnDesc& describeColumn(colnum_t num) const;

    /// @brief Returns the column descriptor for the given column name
    virtual const IColumnDesc& describeColumn(String name) const;




    virtual dal_resultset_type*    getSource(void)   { return this->m_source; }

    void fetchAll(void);
    void detach(void);


protected:
    dal_resultset_type *m_source;
    RecordSet m_rscache;

    rowcount_t m_current_row; // 1-base // 1-basedd

    void resetInternal(void);
    void copyMetadata(void);
    void copyRecord(void);

private:
    CachedResultBase(const CachedResultBase&);
    CachedResultBase& operator=(const CachedResultBase&);


};




//------------------------------------------------------------------------------
///
/// @brief Cached resultset
///
template<typename ResultInterface, typename ResultSource = ResultInterface>
class CachedResult : public CachedResultBase
{
public:
    virtual ~CachedResult(void)
    {}
};




//------------------------------------------------------------------------------
///
/// @brief Database Resultset
template<typename Engine, typename tag, typename ResultInterface = IResult>
class Result : public ResultInterface
{
private:
    typedef typename db_traits<Engine, tag>::statement_type      statement_type;
    typedef typename db_traits<Engine, tag>::dal_resultset_type  dal_resultset_type;
    typedef typename db_traits<Engine, tag>::dal_columndesc_type dal_columndesc_type;
    typedef typename db_traits<Engine, tag>::value_type          value_type;
    typedef typename db_traits<Engine, tag>::dal_variant_type    dal_variant_type;

public:

    Result(void)
        : ResultInterface(),
          m_result()
    {}

    Result(statement_type &stmt)
        : ResultInterface(),
          m_result(&stmt.resultset())
    {}

    Result(dal_resultset_type *result)
        : ResultInterface(),
          m_result(result)
    {}


    void attach(statement_type &statement)  { this->m_result = &statement.resultset(); }

    void attach(dal_resultset_type &result) { this->m_result = &result; }


    virtual bool   isPrepared(void) const  { return this->m_result->isPrepared(); }

    virtual bool   isBad(void) const       { return this->m_result->isBad(); }

    virtual void   first(void)             { this->m_result->first(); }

    virtual bool   next(void)              { return this->m_result->next(); }

    virtual bool   eof(void) const         { return this->m_result->eof(); }

    virtual bool   isOpen(void) const      { return this->m_result->isOpen(); }

    virtual bool   isPositioned(void) const      { return this->m_result->isPositioned(); }

    virtual void   close(void)             { this->m_result->close(); }


    // row methods
    virtual rowcount_t     rowCount(void) const            { return this->m_result->rowCount(); }

    virtual size_t              paramCount(void) const          { return this->m_result->paramCount(); }

    virtual rowcount_t     affectedRows(void) const        { return this->m_result->affectedRows(); }

    virtual dal_variant_type lastInsertRowId(void)              { return this->m_result->lastInsertRowId(); }

    virtual const value_type&   column(colnum_t num)       { return this->m_result->column(num); }

    //virtual variant_type&      field(colnum_t num)      { return this->m_result->field(num); }

    virtual const value_type&   column(String name)             { return this->m_result->column(name); }

    //virtual variant_type&      field(String name)     { return this->m_result->field(name); }


    //virtual IBlob&           getBlob(colnum_t num) = 0;

    // column methods
    virtual size_t                 columnCount(void) const             { return this->m_result->columnCount(); }

    virtual colnum_t          columnID(String name) const         { return this->m_result->columnID(name); }

    virtual String                 columnName(colnum_t num) const { return this->m_result->columnName(num); }


    virtual const dal_columndesc_type& describeColumn(colnum_t num) const
    { return this->m_result->describeColumn(num); }

    virtual const dal_columndesc_type& describeColumn(String name) const
    { return this->m_result->describeColumn(name); }


    //virtual const ITypeInfo&  datatype(colnum_t num) const   { return this->m_result->datatype(num); }

    virtual IDALDriver*       drv(void) const               { return this->m_result->drv(); }


    virtual dal_resultset_type*    getImpl(void)   { return this->m_result; }



protected:
    dal_resultset_type *m_result;

private:
    Result(const Result&);
    Result& operator=(const Result&);
};


#define DBWTL_FOREACH_ROW(result) for(result.first(); !result.eof(); result.next())





template<typename Engine, typename tag, typename MetadataInterface = IMetadata>
class Metadata : public MetadataInterface
{
private:
    //typedef typename db_traits<Engine, tag>::dal_resultset_type  dal_resultset_type;
    //typedef typename db_traits<Engine, tag>::dal_stmt_type       dal_stmt_type;
    //typedef typename db_traits<Engine, tag>::dal_diag_type       dal_diag_type;
    //typedef typename db_traits<Engine, tag>::dal_variant_type    dal_variant_type;

    typedef typename db_traits<Engine, tag>::dal_metadata_type    dal_metadata_type;

    //typedef typename dal_metadata_type::FilterDirection         FilterDirection;

public:
    Metadata( typename db_traits<Engine, tag>::connection_type &dbc )
        : MetadataInterface(),
          m_metadata( dbc.newMetadata() )
    {}

    virtual ~Metadata(void)
    {
    }


    virtual RecordSet getCatalogs(const Variant &catalog = Variant(),
                    const IMetadata::ObjectClass system = IMetadata::META_OBJECT_CLASS_USER,
		                    const DatasetFilter &filter = NoFilter())
    { return getImpl()->getCatalogs(catalog, system, filter); }
    virtual RecordSet getSchemas(const Variant &catalog = Variant(),
                    const Variant &schema = Variant(),
		                    const IMetadata::ObjectClass system = IMetadata::META_OBJECT_CLASS_USER,
				                    const DatasetFilter &filter = NoFilter())
    { return getImpl()->getSchemas(catalog, schema, system, filter); }
    virtual RecordSet getTables(const Variant &schema = Variant(),
                    const Variant &catalog = Variant(),
		                    const Variant &table = Variant(),
				                    const IMetadata::ObjectClass system = IMetadata::META_OBJECT_CLASS_USER,
						                    const DatasetFilter &filter = NoFilter())
    { return getImpl()->getTables(schema, catalog, table, system, filter); }
    virtual RecordSet getColumns(const Variant &table = Variant(),
                    const Variant &schema = Variant(),
		                    const Variant &catalog = Variant(),
				                    const Variant &column = Variant(),
						                    const IMetadata::ObjectClass system = IMetadata::META_OBJECT_CLASS_USER,
								                    const DatasetFilter &filter = NoFilter())
    { return getImpl()->getColumns(table, schema, catalog, column, system, filter); }


    virtual dal_metadata_type*          getImpl(void)               { return this->m_metadata.get(); }

protected:
    typename dal_metadata_type::ptr m_metadata;
	
private:
    Metadata(const Metadata&);
    Metadata& operator=(const Metadata&);

};



class cfmt_header
{
public:
    cfmt_header(IDataset &ds);

    void write(std::ostream &os) const;
protected:
    IDataset &m_ds;
};

class cfmt
{
public:
    cfmt(IDataset &ds, colnum_t colnum);
    cfmt(IDataset &ds, const String &name);

    void write(std::ostream &os) const;
protected:
    IDataset &m_ds;
    colnum_t m_cn;
    const String m_cs;
    bool m_byname;
};


std::ostream& operator<<(std::ostream &os, const cfmt &cf);
std::ostream& operator<<(std::ostream &os, const cfmt_header &cf);


//------------------------------------------------------------------------------
///
/// @brief Defines the main types
template<typename Engine, typename tag = default_tag>
struct Database : public db_traits<Engine, tag>::sqlstate_types,
                  public db_traits<Engine, tag>::datatype_types
{
    typedef typename db_traits<Engine, tag>::environment_type         Environment;
    typedef typename db_traits<Engine, tag>::connection_type          Connection;
    typedef typename db_traits<Engine, tag>::statement_type           Statement;
    typedef typename db_traits<Engine, tag>::resultset_type           Resultset;
    typedef typename db_traits<Engine, tag>::metadata_type            Metadata;
    typedef typename db_traits<Engine, tag>::cached_resultset_type    CachedResultset;
    typedef typename db_traits<Engine, tag>::value_type               Value;
    typedef typename db_traits<Engine, tag>::dal_columndesc_type      ColumnDesc;
    typedef typename db_traits<Engine, tag>::dal_diag_type            Diag;
    typedef typename db_traits<Engine, tag>::dal_variant_type         Variant;
/*
    typedef Blob                                                 Blob;
    typedef Memo                                                 Memo;
*/
};



DB_NAMESPACE_END

#endif


//MANPAGE.001 Informave DBWTL(3)
//MANPAGE.001 ==================
//MANPAGE.001 
//MANPAGE.001 NAME
//MANPAGE.001 ----
//MANPAGE.001 dbwtl - C++ Database Wrapper Template Library
//MANPAGE.001 
//MANPAGE.001 
//MANPAGE.005 DESCRIPTION
//MANPAGE.005 -----------
//MANPAGE.005 The DBWTL library is a C++ wrapper library for database
//MANPAGE.005 access.
//MANPAGE.005 
//MANPAGE.010 SYNOPIS
//MANPAGE.010 -------
//MANPAGE.010 [source,cpp]
//MANPAGE.010 -------------------------------------------------
//MANPAGE.010 #include <dbwtl/dbobjects>
//MANPAGE.010 #include <dbwtl/dal/generic/engine>
//MANPAGE.010 
//MANPAGE.010 typedef Database<Generic> DBMS;     // define new database type
//MANPAGE.010 
//MANPAGE.010 DBMS::Environment env("sqlite:libsqlite");
//MANPAGE.010 
//MANPAGE.010 DBMS::Connection dbc(env);
//MANPAGE.010 
//MANPAGE.010 dbc.connect("dummy.db");
//MANPAGE.010 
//MANPAGE.010 DBMS::Statement stmt(dbc);
//MANPAGE.010 
//MANPAGE.010 stmt.execDirect("SELECT * from test");
//MANPAGE.010 
//MANPAGE.010 DBMS::Resultset res(stmt);
//MANPAGE.010 
//MANPAGE.010 res.first();    // seek to first record
//MANPAGE.010 res.next();     // seek to next record
//MANPAGE.010 res.eof();      // check for end-of-resultset
//MANPAGE.010 
//MANPAGE.010 // get a column value
//MANPAGE.010 res.column("mycolumn");
//MANPAGE.010
//MANPAGE.010 -------------------------------------------------
//MANPAGE.010



//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
