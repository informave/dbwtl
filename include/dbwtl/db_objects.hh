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


#include "dbwtl/dbwtl_config.hh"
#include "db_fwd.hh"
//#include "db_exceptions.hh"
#include "dal/dal_interface.hh"

#include <iostream>

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
//MANPAGE.010 typedef Database<dal::Generic> DBMS;     // define new database type
//MANPAGE.010 
//MANPAGE.010 DBMS::Environment env("sqlite:libsqlite");
//MANPAGE.010 
//MANPAGE.010 DBMS::Connection dbc(env);
//MANPAGE.010 
//MANPAGE.010 dbc.connect(L"dummy.db");
//MANPAGE.010 
//MANPAGE.010 DBMS::Statement stmt(dbc);
//MANPAGE.010 
//MANPAGE.010 stmt.execDirect(L"SELECT * from test");
//MANPAGE.010 
//MANPAGE.010 DBMS::Resultset res(stmt);
//MANPAGE.010 
//MANPAGE.010 res.first();    // seek to first record
//MANPAGE.010 res.next();     // seek to next record
//MANPAGE.010 res.eof();      // check for end-of-resultset
//MANPAGE.010 
//MANPAGE.010 // get a column value
//MANPAGE.010 res.column(L"mycolumn");
//MANPAGE.010
//MANPAGE.010 -------------------------------------------------
//MANPAGE.010



DB_NAMESPACE_BEGIN


struct default_tag { };
struct extended_api_tag { };




//------------------------------------------------------------------------------
///
/// @brief SQLSTATE template
template<int T, class U = informave::db::ex::sqlstate_exception>
struct sqlstate : public U
{
    sqlstate(const dal::IDiagnostic& diag_to_clone)
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
    ifnull(const dal::IVariant &var, value_type def) : m_var(var), m_def(def)
    {}

    /// cast operator to the requested type
    operator value_type() const
    { return m_var.isnull() ? m_def : dal::variant_dispatch_method<value_type>(m_var)(); }

private:
    const dal::IVariant &m_var;
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
         typename EnvInterface = dal::IEnv>
class Environment : public EnvInterface
{
private:
    typedef typename db_traits<Engine, tag>::dal_dbc_type     dal_dbc_type;
    typedef typename db_traits<Engine, tag>::dal_env_type     dal_env_type;
    typedef typename db_traits<Engine, tag>::dal_diag_type    dal_diag_type;
    typedef typename db_traits<Engine, tag>::dal_variant_type dal_variant_type;

public:
    Environment(i18n::UString str)
        : EnvInterface(),
          m_env( dal::Factory::create<Engine>(str) )
    {}

    Environment(std::string str) 
        : EnvInterface(),
          m_env( dal::Factory::create<Engine>(i18n::conv_from(str, "UTF-8") ) )
    {}

    virtual ~Environment(void)
    {}


    virtual dal_dbc_type* newConnection(void) { return this->m_env->newConnection(); }

    virtual void                    setOption(std::string name, dal_variant_type data) { this->m_env->setOption(name, data); }
    virtual const dal_variant_type& getOption(std::string name) const                  { return this->m_env->getOption(name); }

    virtual bool                    diagAvail(void) const   { return this->m_env->diagAvail(); }
    virtual const dal_diag_type&    fetchDiag(void)         { return this->m_env->fetchDiag(); }

    virtual dal_env_type*           getImpl(void)           { return this->m_env.get(); }

protected:
    typename dal_env_type::ptr m_env;

private:
    Environment(const Environment&);
    Environment& operator=(const Environment&);
};





//------------------------------------------------------------------------------
///
/// @brief Database Connection
template<typename Engine, typename tag, typename ConnectionInterface = dal::IDbc>
class Connection : public ConnectionInterface
{
private:
    typedef typename db_traits<Engine, tag>::dal_dbc_type      dal_dbc_type;
    typedef typename db_traits<Engine, tag>::dal_stmt_type     dal_stmt_type;
    typedef typename db_traits<Engine, tag>::dal_diag_type     dal_diag_type;
    typedef typename db_traits<Engine, tag>::dal_variant_type  dal_variant_type;

public:
    Connection( typename db_traits<Engine, tag>::environment_type &env )
        : ConnectionInterface(),
          m_dbc( env.newConnection() )
    {}

    virtual ~Connection(void)
    {}


    virtual void   connect(i18n::UString database,
                           i18n::UString user = i18n::UString(),
                           i18n::UString password = i18n::UString())
    { this->m_dbc->connect(database, user, password); }


    virtual void   connect(dal::IDbc::Options& options)
    { this->m_dbc->connect(options); }


    virtual bool              isConnected(void) const       { return this->m_dbc->isConnected(); }

    virtual void              disconnect(void)              { this->m_dbc->disconnect(); }

    virtual i18n::UString     driverName(void) const        { return this->m_dbc->driverName(); }

    virtual i18n::UString     dbmsName(void) const          { return this->m_dbc->dbmsName(); }

    virtual dal_stmt_type*    newStatement(void)            { return this->m_dbc->newStatement(); }

    virtual dal::IDALDriver*  getDriver(void) const         { return this->m_dbc->getDriver(); }

    virtual void              commit(void)                  { this->m_dbc->commit(); }

    virtual void              savepoint(i18n::UString name) { this->m_dbc->savepoint(name); }

    virtual void              directCmd(i18n::UString cmd)  { this->m_dbc->directCmd(cmd); }

    virtual std::string       getDbcEncoding(void) const    { return this->m_dbc->getDbcEncoding(); }

    virtual void              rollback(i18n::UString name = i18n::UString()) { this->m_dbc->rollback(name); }

    virtual void              beginTrans(dal::IDbc::trx_mode mode,
                                         dal::IDbc::access_mode access = dal::IDbc::trx_default,
                                         i18n::UString name = i18n::UString())
    { this->m_dbc->beginTrans(mode, access, name); }


    virtual dal::TableList    getTables(const dal::ITableFilter& filter = dal::EmptyTableFilter())
    { return this->m_dbc->getTables(filter); }

    virtual dal::DatatypeList getDatatypes(const dal::IDatatypeFilter& filter = dal::EmptyDatatypeFilter())
    { return this->m_dbc->getDatatypes(filter); }

    /*
      virtual ColumnList     getTables(const IColumnFilter& = EmptyColumnFilter()) = 0;
      virtual SchemaList     getSchemas(const ISchemaFilter& = EmptySchemaFilter()) = 0;
      virtual CatalogList    getCatalogs(const ICatalogFilter& = EmptyCatalogFilter()) = 0;
      virtual ViewList       getViews(const IViewFilter& = EmptyViewFilter()) = 0;
      virtual ProcedureList  getProcedures(const IProcedureFilter& = EmptyProcedureFilter()) = 0;
      virtual ProcColumnList getProcColumns(const IProcColumnFilter& = EmptyProcColumnFilter()) = 0;
    */


    virtual bool                   diagAvail(void) const    { return this->m_dbc->diagAvail(); }
    virtual const dal_diag_type&   fetchDiag(void)          { return this->m_dbc->fetchDiag(); }


    virtual void                    setOption(std::string name, dal_variant_type data) { this->m_dbc->setOption(name, data); }
    virtual const dal_variant_type& getOption(std::string name) const                  { return this->m_dbc->getOption(name); }


    virtual dal_dbc_type*           getImpl(void)   { return this->m_dbc.get(); }

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
template<typename Engine, typename tag, typename StmtInterface = dal::IStmt>
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
          m_stmt( dbc.newStatement() )
    {}

    virtual ~Statement(void)
    {}


    virtual bool      isBad(void) const                         { return this->m_stmt->isBad(); }

    virtual void      prepare(i18n::UString sql)                { return this->m_stmt->prepare(sql); }

    virtual bool      isPrepared(void) const                    { return this->m_stmt->isPrepared(); }

    virtual void      execute(void)                             { this->m_stmt->execute(); }

    virtual void      execDirect(i18n::UString sql)             { this->m_stmt->execDirect(sql); }

    virtual void      close(void)                               { this->m_stmt->close(); }

    virtual dal_resultset_type&        resultset(void)          { return this->m_stmt->resultset(); }

    virtual const dal_resultset_type&  resultset(void) const    { return this->m_stmt->resultset(); }

    virtual bool      nextResultset(void)                       { return this->m_stmt->nextResultset(); }

    virtual bool      moreResultsets(void) const                { return this->m_stmt->moreResultsets();}

    virtual size_t    paramCount(void) const                    { return this->m_stmt->paramCount(); }

    virtual void      bind(int num, dal::IVariant* data)        { this->m_stmt->bind(num, data); }

    virtual void      bind(int num, const dal::IVariant* data)  { this->m_stmt->bind(num, data); }

    virtual void      bind(int num, dal::Variant data)          { this->m_stmt->bind(num, data); }

    virtual void      bind(int num, std::streambuf *data)       { this->m_stmt->bind(num, data); }

    virtual void      bind(int num, std::wstreambuf *data)      { this->m_stmt->bind(num, data); }

    virtual dal::rowcount_t  affectedRows(void) const           { return this->m_stmt->affectedRows(); }

    virtual dal::IDALDriver* getDriver(void) const              { return this->m_stmt->getDriver(); }

    // Diagnostic
    virtual bool                   diagAvail(void) const        { return this->m_stmt->diagAvail(); }
    virtual const dal_diag_type&   fetchDiag(void)              { return this->m_stmt->fetchDiag(); }

    // Options
    virtual void                    setOption(std::string name, dal_variant_type data) { this->m_stmt->setOption(name, data); }
    virtual const dal_variant_type& getOption(std::string name) const                  { return this->m_stmt->getOption(name); }


    virtual dal_stmt_type*          getImpl(void)               { return this->m_stmt.get(); }

protected:
    typename dal_stmt_type::ptr m_stmt;

private:
    Statement(const Statement&);
    Statement operator=(const Statement&);
};





//------------------------------------------------------------------------------
///
/// @brief Cached resultset
///
/// @bug implement me 
template<typename ResultInterface, typename ResultSource = ResultInterface>
class CachedResult : public ResultInterface
{
};





//------------------------------------------------------------------------------
///
/// @brief Database Resultset
template<typename Engine, typename tag, typename ResultInterface = dal::IResult>
class Result : public ResultInterface
{
private:
    typedef typename db_traits<Engine, tag>::statement_type      statement_type;
    typedef typename db_traits<Engine, tag>::dal_resultset_type  dal_resultset_type;
    typedef typename db_traits<Engine, tag>::dal_columndesc_type dal_columndesc_type;
    typedef typename db_traits<Engine, tag>::value_type          value_type;

public:
    typedef int iterator;
    typedef int const_iterator;



    Result(void)
        : ResultInterface(),
          m_result()
    {}

    Result(statement_type &stmt)
        : ResultInterface(),
          m_result(*stmt.resultset())
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

    virtual void   next(void)              { this->m_result->next(); }

    virtual bool   eof(void) const         { return this->m_result->eof(); }

    virtual bool   isOpen(void) const      { return this->m_result->isOpen(); }

    virtual void   close(void)             { this->m_result->close(); }


    // row methods
    virtual dal::rowcount_t    rowCount(void) const            { return this->m_result->rowCount(); }

    virtual size_t             paramCount(void) const          { return this->m_result->paramCount(); }

    virtual dal::rowcount_t    affectedRows(void) const        { return this->m_result->affectedRows(); }

    virtual const value_type&      column(dal::colnum_t num)   { return this->m_result->column(num); }

    //virtual variant_type&      field(dal::colnum_t num)      { return this->m_result->field(num); }

    virtual const value_type&      column(i18n::UString name)  { return this->m_result->column(name); }

    //virtual variant_type&      field(i18n::UString name)     { return this->m_result->field(name); }


    //virtual IBlob&           getBlob(colnum_t num) = 0;

    // column methods
    virtual size_t                 columnCount(void) const             { return this->m_result->columnCount(); }

    virtual dal::colnum_t          columnID(i18n::UString name) const  { return this->m_result->columnID(name); }

    virtual i18n::UString          columnName(dal::colnum_t num) const { return this->m_result->columnName(num); }


    virtual const dal_columndesc_type& describeColumn(dal::colnum_t num) const { return this->m_result->describeColumn(num); }

    virtual const dal_columndesc_type& describeColumn(i18n::UString name) const { return this->m_result->describeColumn(name); }


    //virtual const dal::ITypeInfo&  datatype(dal::colnum_t num) const   { return this->m_result->datatype(num); }

    virtual dal::IDALDriver*       getDriver(void) const               { return this->m_result->getDriver(); }


    virtual dal_resultset_type*    getImpl(void)   { return this->m_result; }



    iterator begin(void) 
    { 
        throw std::runtime_error("not implemented"); /// @bug fixme
    }

protected:
    dal_resultset_type *m_result;

private:
    Result(const Result&);
    Result& operator=(const Result&);
};




//MANPAGE.020 DATABASES
//MANPAGE.020 ---------
//MANPAGE.020 
//MANPAGE.020 Before you can start, you have to choose wether you want to work with
//MANPAGE.020 the generic interface or with a special database system.
//MANPAGE.020 
//MANPAGE.020 It is recommended that you define a C++ type, for example:
//MANPAGE.020 
//MANPAGE.020     typedef Database<dal::generic> DBMS;
//MANPAGE.020 
//MANPAGE.020 for the generic interface, or
//MANPAGE.020 
//MANPAGE.020     typedef Database<dal::the-engine-name> DBMS;
//MANPAGE.020 
//MANPAGE.020 for a specific system. The engine classes are defined in the
//MANPAGE.020 dal namespace.
//MANPAGE.020 
//MANPAGE.020 
//MANPAGE.020 The DBWTL library supports the following database systems:
//MANPAGE.020 
//MANPAGE.020 Generic interface (generic)
//MANPAGE.020 ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//MANPAGE.020 
//MANPAGE.020 The generic interface works with all supported engines. The format of
//MANPAGE.020 the driver is:
//MANPAGE.020 
//MANPAGE.020        <engine>:<driver>[:<lib>]
//MANPAGE.020 
//MANPAGE.020 
//MANPAGE.020 
//MANPAGE.020 SQLite (sqlite)
//MANPAGE.020 ~~~~~~~~~~~~~~~
//MANPAGE.020 Available drivers:
//MANPAGE.020 
//MANPAGE.020 *libsqlite*::
//MANPAGE.020   This driver uses the libsqlite.so driver or sqlite3.dll.
//MANPAGE.020 *odbc*:: 
//MANPAGE.020   Uses an ODBC connection.
//MANPAGE.020 
//MANPAGE.020 PostgreSQL (postgres)
//MANPAGE.020 ~~~~~~~~~~~~~~~~~~~~~
//MANPAGE.020 Available drivers:
//MANPAGE.020 
//MANPAGE.020 *libpq*::
//MANPAGE.020   This driver uses the libpq.so driver or libpq.dll.
//MANPAGE.020 *odbc*:: 
//MANPAGE.020   Uses an ODBC connection.
//MANPAGE.020 





//GEXAMPLE.001 Introduction with a step by step example
//GEXAMPLE.001 ----------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 All important declarations are provided by the inclusion of the
//GEXAMPLE.001 'dbobjects' header file.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 #include <dbwtl/dbobjects>
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 You have to choose at least one engine and include them.
//GEXAMPLE.001 If you want a generic API instead of a specific engine, you
//GEXAMPLE.001 can just include the 'generic' engine.
//GEXAMPLE.001 The generic engine allows the configuration of a
//GEXAMPLE.001 specific engine at runtime.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 #include <dbwtl/dal/engines/sqlite>
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 The next line is not required, but makes life easier.
//GEXAMPLE.001 All needed classes are declared inside the 'informave::db' namespace.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 using namespace informave::db;
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 Now we have to declare our own type to tell the dbwtl library
//GEXAMPLE.001 which database/engine we want to use.
//GEXAMPLE.001 The 'Database' template members declares the right types for connections,
//GEXAMPLE.001 resultsets etc. depending on the given engine.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 typedef Database<dal::sqlite> DBMS;
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 The environment is the first thing we have to create. The argument to the
//GEXAMPLE.001 constructor tells the environment which backend should be used.
//GEXAMPLE.001 For example, you can connect to a SQLite database via the libsqlite.dll C-Library,
//GEXAMPLE.001 through ODBC or ADO. 
//GEXAMPLE.001 
//GEXAMPLE.001 If you are using the generic engine, the first part of the string is important.
//GEXAMPLE.001 The generic Environment class instantiates an environment of the given
//GEXAMPLE.001 engine "in the background". This makes it possible to select
//GEXAMPLE.001 the used engine at runtime.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 DBMS::Environment env("sqlite:libsqlite");
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 Now we can check if everything is correct and the dbwtl library
//GEXAMPLE.001 is correctly installed.
//GEXAMPLE.001 [source,shell]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 gcc -o dbtest dbtest.c -ldbwtl
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 
//GEXAMPLE.001 Creating a connection
//GEXAMPLE.001 ~~~~~~~~~~~~~~~~~~~~~
//GEXAMPLE.001 In the next step, we create a new connection to a SQLite database.
//GEXAMPLE.001 The 'Connection' constructor takes the environment object as a reference.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 DBMS::Connection dbc(env);
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 Now we can open the database:
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 dbc.connect(L"sampledb.sqlitedb");
//GEXAMPLE.001 std::cout << i18n::conv_to(dbc.dbmsName(), "ISO-8859-1") << std::endl;
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 Run a SQL query
//GEXAMPLE.001 ~~~~~~~~~~~~~~~
//GEXAMPLE.001 To run a query, we need to create a Statement object.
//GEXAMPLE.001 The constructor takes the connection object as a reference.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 DBMS::Statement stmt(dbc);
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 
//GEXAMPLE.001 The direct execution of a SQL statement is possible via 'execDirect()':
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 stmt.execDirect(L"SELECT * from customers");
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 Iterate over the resultset
//GEXAMPLE.001 ^^^^^^^^^^^^^^^^^^^^^^^^^^
//GEXAMPLE.001 After execution of the statement, you can create a new DBMS::Resultset object
//GEXAMPLE.001 and attach it to the internal statement resultset:
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 DBMS::Resultset rs(stmt);
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 [NOTE]
//GEXAMPLE.001 If the statement contains no internal resultset, the DBMS::Resultset
//GEXAMPLE.001 object is marked as 'bad'. This state can only be changed by re-attaching
//GEXAMPLE.001 the statement via 'rs.attach(stmt)'.
//GEXAMPLE.001 


//GEXAMPLE.001 With the methods 'first()', 'next()' and 'eof()' you can iterate over the
//GEXAMPLE.001 records.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 for(rs.first(); !rs.eof(); rs.next()) { //for each record }
//GEXAMPLE.001 ------------------------------------------------------------------------------
//
//GEXAMPLE.001 Access the record fields
//GEXAMPLE.001 ^^^^^^^^^^^^^^^^^^^^^^^^
//GEXAMPLE.001 You can access the fields by name or by position (first field has position 1).
//GEXAMPLE.001 To determine the number of available fields, use 'columnCount()'.
//GEXAMPLE.001 The 'column()' method returns a 'const DBMS::Value' reference which provides
//GEXAMPLE.001 many getter methods like 'asStr()', 'asInt()' or 'asDate()' to get the value.
//GEXAMPLE.001 To check if a field is null, call isNull() on the value reference.

//GEXAMPLE.001 
//GEXAMPLE.001 .Print all records and fields
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 for(rs.first(); !rs.eof(); rs.next())
//GEXAMPLE.001 {
//GEXAMPLE.001  for(int i = 1; i <= rs.columnCount(); ++i)
//GEXAMPLE.001      {
//GEXAMPLE.001      if(rs.column(i).isNull())
//GEXAMPLE.001          std::cout << "<NULL>";
//GEXAMPLE.001      else
//GEXAMPLE.001          std::cout << rs.column(i).asNarrowStr("UTF-8");
//GEXAMPLE.001  }
//GEXAMPLE.001  std::cout << std::endl;
//GEXAMPLE.001 }
//GEXAMPLE.001 ------------------------------------------------------------------------------
//
//GEXAMPLE.001 Access the column descriptor
//GEXAMPLE.001 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//GEXAMPLE.001 The 'describeColumn()' method provides a getter for all column descriptors.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 for(int i = 1; i <= rs.columnCount(); i++)
//GEXAMPLE.001 {
//GEXAMPLE.001      const DBMS::ColumnDesc &desc = rs.describeColumn(i);
//GEXAMPLE.001      std::cout << desc.catalogName() << std::endl;
//GEXAMPLE.001      std::cout << desc.columnName() << std::endl;
//GEXAMPLE.001 }
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 A DMBS::ColumnDesc (Interface: 'dal::IColumnDesc') object has
//GEXAMPLE.001 several getter methods all returning
//GEXAMPLE.001 a const DBMS::Variant (Interface: 'dal::IVariant') reference.


//GSUPPDB.001 
//GSUPPDB.001 Supported database systems and drivers
//GSUPPDB.001 --------------------------------------
//GSUPPDB.001 SQLite
//GSUPPDB.001 ~~~~~~
//GSUPPDB.001 libsqlite:: Uses the libsqlite3.so or sqlite3.dll to access
//GSUPPDB.001 a database file.
//GSUPPDB.001 odbc:: Uses a installed ODBC driver to connect to a SQLite database.
//GSUPPDB.001 




//GCORE.001 The Database<> template
//GCORE.001 -----------------------
//GCORE.001 
//GCORE.001 The 'Database<>' template defines all important types required to work
//GCORE.001 with a database, depending on the selected engine.
//GCORE.001 The engine is a class name from the 'dal' namespace, e.g. 'dal::sqlite'.
//GCORE.001 If you want to choose the engine at runtime, you can use 'dal::generic' and
//GCORE.001 set the engine later.
//GCORE.001 


//GCORE.001 
//GCORE.001 Handles
//GCORE.001 ~~~~~~~
//GCORE.001 There are three handles: Environment, Connection and Statement.
//GCORE.001 
//GCORE.001 
//GCORE.001 Setting options for handles
//GCORE.001 ^^^^^^^^^^^^^^^^^^^^^^^^^^^
//GCORE.001 Each handle provides a 'setOption()' and 'getOption()' method.
//GCORE.001 
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ------------------------------------------------------------------------------
//GCORE.001 DBMS::Environment env("sqlite:libsqlite");
//GCORE.001 
//GCORE.001 env.setOption("env_library_path", std::wstring(L"/path/to/libs"));
//GCORE.001 
//GCORE.001 std::string path = ifnull<std::string>(env.getOption("env_library_path"), ".");
//GCORE.001 ------------------------------------------------------------------------------
//GCORE.001 




//GCORE.001 Environment
//GCORE.001 ~~~~~~~~~~~
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ---------------------------------------------
//GCORE.001 Database<dal::engine_name>::Environment
//GCORE.001 ---------------------------------------------
//GCORE.001 



//GCORE.001 
//GCORE.001 Connection
//GCORE.001 ~~~~~~~~~~
//GCORE.001 [source,cpp]
//GCORE.001 ---------------------------------------------
//GCORE.001 Database<dal::engine_name>::Connection
//GCORE.001 ---------------------------------------------
//GCORE.001 
//GCORE.001 
//GCORE.001 Statement
//GCORE.001 ~~~~~~~~~
//GCORE.001 [source,cpp]
//GCORE.001 ---------------------------------------------
//GCORE.001 Database<dal::engine_name>::Statement
//GCORE.001 ---------------------------------------------
//GCORE.001 


//GCORE.001 Binding BLOB and MEMO types
//GCORE.001 ^^^^^^^^^^^^^^^^^^^^^^^^^^^
//GCORE.001 
//GCORE.001 A DBMS::Statement object provides two overloaded bind() methods for
//GCORE.001 std::streambuf* and std::wstreambuf*.
//GCORE.001 
//GCORE.001 Binding std::streambuf*::
//GCORE.001 The data is threated as binary data.
//GCORE.001 
//GCORE.001 Binding std::wstreambuf*::
//GCORE.001 A character set conversion is performed (from UCS-2 or UCS-4 to connection encoding)
//GCORE.001 before the data is sent to the database.
//GCORE.001 
//GCORE.001 
//GCORE.001 The following examples shows how stream buffers can be bound to a statement:
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ------------------------------------------------------------------------------
//GCORE.001 std::ifstream binFile("photo.jpg", std::ios::binary);
//GCORE.001 stmt.bind(1, binFile.rdbuf()); // binary data
//GCORE.001 
//GCORE.001 std::wifstream textFile("example.txt");
//GCORE.001 stmt.bind(2, textFile.rdbuf()); // text data
//GCORE.001 
//GCORE.001 std::ifstream utf8File("utf8.txt");
//GCORE.001 std::wstreambuf *wbuf = new std::wbuffer_convert<std::codecvt_utf8>(utf8File.rdbuf());
//GCORE.001 stmt.bind(3, wbuf); // text data
//GCORE.001 ------------------------------------------------------------------------------
//GCORE.001 The last one is a special case where a char stream buffer is first converted
//GCORE.001 to a wide char stream buffer. 
//GCORE.001 





//GCORE.001 
//GCORE.001 Resultset
//GCORE.001 ~~~~~~~~~
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ---------------------------------------------
//GCORE.001 Database<dal::engine_name>::Resultset
//GCORE.001 ---------------------------------------------
//GCORE.001 

//GCORE.001 Wokring with BLOB and MEMO types
//GCORE.001 ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
//GCORE.001 For receiving BLOB or MEMO fields, the Variant class provides two
//GCORE.001 methods: 'asBlob()' and 'asMemo()'.
//GCORE.001

//GCORE.001 asBlob::
//GCORE.001 Returns a pointer to std::streambuf and performs no charset conversion.
//GCORE.001 You just get the raw data from the database.
//GCORE.001 
//GCORE.001 asMemo::
//GCORE.001 Returns a pointer to a std::wstreambuf and performs charset conversion (from
//GCORE.001 connection encoding to UCS-2 or UCS-4 [depends on sizeof(wchar_t]).
//GCORE.001 If the underlaying column type is of type BLOB (Variant::daltype() == 'DAL_TYPE_BLOB'),
//GCORE.001 a convert_error exception is raised.
//GCORE.001 All other types are converted into a std::wstreambuf.
//GCORE.001 
//GCORE.001 If you have obtained a buffer pointer with asBlob() or asMemo(), the pointer
//GCORE.001 is valid (for all rows) until you close the resultset or statement.
//GCORE.001 Scrolling through the dataset (e.g. next()) only resets the buffer to the new
//GCORE.001 row data. If the new field is NULL, the buffer is marked as bad (std::ios::bad).
//GCORE.001 



//GCORE.001 General supported types
//GCORE.001 ~~~~~~~~~~~~~~~~~~~~~~~
//GCORE.001 The 'Database<>' template provides for every selected engine a basic set
//GCORE.001 of types conforming to the SQL standard. An engine can extend this set
//GCORE.001 with system specific types supported by the database.
//GCORE.001 
//GCORE.001 `--------------------------`----------------------
//GCORE.001 *SQL:2008 Type*             *DBWTL Type Mapping*
//GCORE.001 CHARACTER                   DBMS::Char
//GCORE.001 CHARACTER VARING            DBMS::Char
//GCORE.001 CHARACTER LARGE OBJECT      DBMS::Memo
//GCORE.001 BINARY                      DBMS::Blob
//GCORE.001 BINARY VARYING              DBMS::Blob
//GCORE.001 BINARY LARGE OBJECT         DBMS::Blob
//GCORE.001 NUMERIC                     DBMS::Numeric
//GCORE.001 DECIMAL                     DBMS::Numeric
//GCORE.001 SMALLINT                    DBMS::Smallint
//GCORE.001 INTEGER                     DBMS::Integer
//GCORE.001 BIGINT                      DBMS::Bigint
//GCORE.001 FLOAT                       DBMS::Float
//GCORE.001 REAL                        DBMS::Float
//GCORE.001 DOUBLE PRECISION            DBMS::Double
//GCORE.001 BOOLEAN                     DBMS::Boolean
//GCORE.001 DATE                        DBMS::Date
//GCORE.001 TIME                        DBMS::Time
//GCORE.001 TIMESTAMP                   DBMS::Timestamp
//GCORE.001 INTERVAL                    unsupported
//GCORE.001 --------------------------------------------------
//GCORE.001 
//GCORE.001 
//GCORE.001 Values from resultsets are automatically converted to the variable type.
//GCORE.001 So the following statement ist valid:
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ---------------------------------------------
//GCORE.001 const DBMS::Numeric val = rs.column(i); // auto conversion
//GCORE.001 ---------------------------------------------
//GCORE.001 



//GCORE.001 Value
//GCORE.001 ~~~~~
//GCORE.001 The 'Value' type is for column values returned from the engine. You can't create
//GCORE.001 new objects of type 'Value', you can only get (mostly const) references
//GCORE.001 from a 'Resultset' object.
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ---------------------------------------------
//GCORE.001 const DBMS::Value &val = rs.column(i);
//GCORE.001 ---------------------------------------------
//GCORE.001 
//GCORE.001 It is a requirement that the 'Value' type implements the IVariant interface.
//GCORE.001 This allows you to pass column values to all methods which  accepts IVariant references,
//GCORE.001 for example Statement::bind()
//GCORE.001 
//GCORE.001 Variant
//GCORE.001 ~~~~~~~
//GCORE.001 The 'Variant' type can be used to create own variant objects.
//GCORE.001 This type can store all the POD types and pointers to such types.
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ------------------------------------------------------
//GCORE.001 int myint = 5;
//GCORE.001 DBMS::Variant var(&myint);
//GCORE.001 var.setInt(10);
//GCORE.001 assert(myint == 10); // myint was changed through setInt()
//GCORE.001 ------------------------------------------------------
//GCORE.001 

//GCORE.001 ColumnDesc
//GCORE.001 ~~~~~~~~~~

//GCORE.001 
//GCORE.001 Blob
//GCORE.001 ~~~~
//GCORE.001 
//GCORE.001 Memo
//GCORE.001 ~~~~
//GCORE.001 

//GCORE.001 
//GCORE.001 Diagnostic and error handling
//GCORE.001 -----------------------------
//GCORE.001 
//GCORE.001 Each handle (Environment, Connection and Statement) logs warnings and errors
//GCORE.001 as a diagnostic record to an internal buffer. If a buffer reaches DBWTL_DIAG_SIZE,
//GCORE.001 older records are overriden (it works like a circular buffer).
//GCORE.001 
//GCORE.001 With 'diagAvail()' you can check if there are any diagnostic records available and
//GCORE.001 with 'fetchDiag()' you can fetch them which moves an internal cursor to the next record. 
//GCORE.001 
//GCORE.001 If an error occours, the DBWTL performs two steps: all information about
//GCORE.001 the error are collected and written to a diagnostic record, and second, a SQLSTATE
//GCORE.001 exception with a copy of the diagnostic record is thrown.
//GCORE.001 
//GCORE.001 Hints and warnings are just logged, no exception is raised.
//GCORE.001 
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ------------------------------------------------------
//GCORE.001 while(dbc.diagAvail())
//GCORE.001 {
//GCORE.001     const DBMS::Diag &diag = dbc.fetchDiag();
//GCORE.001     // print information
//GCORE.001 }
//GCORE.001 ------------------------------------------------------
//GCORE.001 
//GCORE.001 
//GCORE.001 Exception handling
//GCORE.001 ~~~~~~~~~~~~~~~~~~
//GCORE.001 There are two categories of exceptions implemented in DBWTL.
//GCORE.001 
//GCORE.001 Engine exceptions::
//GCORE.001 	Exceptions inside the DBWTL or DAL code like unknown driver strings,
//GCORE.001 	reading NULL values and so on.
//GCORE.001 
//GCORE.001 SQLSTATE exceptions::
//GCORE.001 	Exceptions for SQLSTATEs which are classified as errors, for example
//GCORE.001 	SQL syntax erros, constraint violations or authentification errors.
//GCORE.001 
//GCORE.001 There are only a small set of engine errors:
//GCORE.001 
//GCORE.001  * 'ex::exception': Base class for exceptions
//GCORE.001  * 'ex::convert_error': A variant value could not be converted to the requested type.
//GCORE.001  * 'ex::null_value': A variant value is set to NULL and you have tried to read the value
//GCORE.001  * 'ex::engine_error': Base class for all exceptions raised in DAL code
//GCORE.001  * 'ex::sqlstate_exception': Base class for all SQLSATE error conditions
//GCORE.001  * 'ex::not_found': Raised if you have requested a column or parameter which does not exists
//GCORE.001  * 'ex::charset_error': The source string could not be converted to the requested charset.
//GCORE.001  * 'ex::read_only': Raised if you try to write to a read-only variant
//GCORE.001  * 'ex::missing_function': A required function in the external driver library could not be found
//GCORE.001  * 'ex::engine_busy': Raised if the engine is busy and your request could not be handled
//GCORE.001 
//GCORE.001 The SQLSTATE exceptions are far more complex. The list of SQLSTATE exceptions
//GCORE.001 you can catch depends on the selected engine. Each engine defines their own
//GCORE.001 list of supported ISO 9075 SQLSATE exceptions (and maybe engine-dependend custom SQLSTATES).
//GCORE.001 Only the generic engine defines the full set of ISO SQLSTATE error codes, specific
//GCORE.001 engines defines only thoose where are supported (plus their own states).
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ------------------------------------------------------------------------------
//GCORE.001 DBMS::Statement stmt(dbc);
//GCORE.001 try
//GCORE.001 {
//GCORE.001 	stmt.prepare(L"INSERT INTO customers VALUES(1, 'Dave Miller');");
//GCORE.001 	stmt.execute();
//GCORE.001 }
//GCORE.001 catch(DBMS::SQLSTATE_23000 &state)
//GCORE.001 {
//GCORE.001 	std::cout << "Constraint violation" << std::endl;
//GCORE.001 }
//GCORE.001 catch(DBMS::SQLSTATE_22000 &state)
//GCORE.001 {
//GCORE.001 	std::cout << "General data exception or other 22xxx state" << std::endl;
//GCORE.001 }
//GCORE.001 catch(DBMS::SQLSTATE_42000 &state)
//GCORE.001 {
//GCORE.001 	std::cout << "Syntax error or access violation" << std::endl;
//GCORE.001 }
//GCORE.001 catch(ex::engine_error &err)
//GCORE.001 {
//GCORE.001 	std::cout << "Other errors" << std::endl;
//GCORE.001 }
//GCORE.001 ------------------------------------------------------------------------------
//GCORE.001 
//GCORE.001 A SQLSTATE subclass (last 3 chars) is derived from the SQLSTATE class (first
//GCORE.001 2 chars) which makes it possible to catch a specific subclass SQLSTATE or all
//GCORE.001 subclasses by catching the class (e.g. SQLSTATE_22000).
//GCORE.001 
//GCORE.001 An engine can define additional SQLSTATES which can be catched by
//GCORE.001 the same way as regular ISO SQLSTATEs. The only exception is when you are
//GCORE.001 using the generic engine: only the ISO SQLSTATEs are defined.
//GCORE.001 If a specific engine defines for example SQLSTATE_XYZ01, you can catch them
//GCORE.001 only with ex::sqlstate_error which is the base class for all SQLSTATE exceptions.
//GCORE.001 Inside the exception handler you can fetch the SQLSTATE as a string to make
//GCORE.001 your decisions:
//GCORE.001 
//GCORE.001 
//GCORE.001 
//GCORE.001 [source,cpp]
//GCORE.001 ------------------------------------------------------------------------------
//GCORE.001 DBMS::Statement stmt(dbc);
//GCORE.001 try
//GCORE.001 {
//GCORE.001 	stmt.prepare(L"INSERT INTO customers VALUES(1, 'Dave Miller');");
//GCORE.001 	stmt.execute();
//GCORE.001 }
//GCORE.001 catch(DBMS::SQLSTATE_42000 &state)
//GCORE.001 {
//GCORE.001 	// catched ISO defined SQLSTATE
//GCORE.001 }
//GCORE.001 catch(ex::sqlstate_error &state)
//GCORE.001 {
//GCORE.001     // catched engine specific SQLSTATE
//GCORE.001 	std::string s = state.diag().sqlstate().asNarrowStr("ASCII");
//GCORE.001 	if(s.compare("XYZ01") == 0)
//GCORE.001 	{
//GCORE.001 		// handle XYZ001
//GCORE.001 	}
//GCORE.001 	else if(s.compare("ZZZ55") == 0)
//GCORE.001 	{
//GCORE.001 		// handle ZZZ55
//GCORE.001 	}
//GCORE.001 	else
//GCORE.001 	{
//GCORE.001 		// other states
//GCORE.001 	}
//GCORE.001 }
//GCORE.001 ------------------------------------------------------------------------------
//GCORE.001 
//GCORE.001 This provides a powerful method if you want to use the generic layer, but also
//GCORE.001 have to handle engine specific SQLSTATEs.
//GCORE.001 







struct basic_datatypes
{
//     typedef signed char           Char; // these two types are covered by String!
//     typedef unsigned char         UChar;
    typedef i18n::UString         String;
/*
    typedef std::wstreambuf*      Memo;
    typedef std::streambuf*       Blob; // better use dal::Blob with implicit ctor?
*/
    typedef dal::Blob             Blob;
    typedef dal::Memo             Memo;
    typedef dal::TNumeric         Numeric;
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
    typedef dal::TDate            Date;
    typedef dal::TTime            Time;
    typedef dal::TTimestamp       Timestamp;
    typedef dal::TInterval        Interval;


    /*
    typedef dal::TCidr            CIDR;
    typedef dal::TMacaddr         MACAddr;
    typedef dal::TInetaddr        INETAddr;
    typedef dal::TUuid            UUID;
    typedef dal::TXml             XML;
    typedef dal::TDatetime        Datetime;
    */

};


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
    typedef typename db_traits<Engine, tag>::cached_resultset_type    CachedResultset;
    typedef typename db_traits<Engine, tag>::value_type               Value;
    typedef typename db_traits<Engine, tag>::dal_columndesc_type      ColumnDesc;
    typedef typename db_traits<Engine, tag>::dal_diag_type            Diag;
    typedef typename db_traits<Engine, tag>::dal_variant_type         Variant;
/*
    typedef dal::Blob                                                 Blob;
    typedef dal::Memo                                                 Memo;
*/
};

//GSUPP.001 
//GSUPP.001 Supported systems
//GSUPP.001 -----------------
//GSUPP.001 
//GSUPP.001 [format="csv",cols="^1,4*2",separator=";"]
//GSUPP.001 |===================================================
//GSUPP.001 DBMS;Version;Status;Write Support;Version introduced
//GSUPP.001 include::../../doc/dbms_support.csv[]
//GSUPP.001 |===================================================
//GSUPP.001 
//GSUPP.001 Drivers implemented as SDA connector
//GSUPP.001 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//GSUPP.001 [format="csv",cols="^1,4*2",separator=";"]
//GSUPP.001 |===================================================
//GSUPP.001 DBMS;Version;Status;Write Support;Version introduced
//GSUPP.001 include::../../doc/sda_drivers.csv[]
//GSUPP.001 |===================================================
//GSUPP.001 



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
