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
/// @brief Database traits class
template<typename Engine, typename tag = default_tag>
struct db_traits
{ };





//------------------------------------------------------------------------------
///
///  @brief Database Environment
template<typename Engine,
         typename tag,
         typename EnvInterface = dal::IEnv>
class Environment : public EnvInterface
{
    typedef typename db_traits<Engine, tag>::dal_dbc_type    dal_dbc_type;
    typedef typename db_traits<Engine, tag>::dal_env_type    dal_env_type;

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

    virtual dal_env_type* getImpl(void) { return this->m_env.get(); }

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
    typedef typename db_traits<Engine, tag>::dal_dbc_type     dal_dbc_type;
    typedef typename db_traits<Engine, tag>::dal_stmt_type    dal_stmt_type;

public:
    Connection( typename db_traits<Engine, tag>::environment_type &env )
        : ConnectionInterface(),
          m_dbc( env.newConnection() )
        {}

    virtual ~Connection(void)
        {}


    virtual dal::dalstate_t   connect(i18n::UString database,
                                      i18n::UString user = i18n::UString(),
                                      i18n::UString password = i18n::UString())
        { return this->m_dbc->connect(database, user, password); }


    virtual dal::dalstate_t   connect(dal::IDbc::Options& options)
        { return this->m_dbc->connect(options); }


    virtual bool              isConnected(void) const       { return this->m_dbc->isConnected(); }

    virtual dal::dalstate_t   disconnect(void)              { return this->m_dbc->disconnect(); }

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

    virtual dal_dbc_type*     getImpl(void)   { return this->m_dbc.get(); }

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
    typedef typename db_traits<Engine, tag>::dal_resultset_type  dal_resultset_type;
    typedef typename db_traits<Engine, tag>::dal_stmt_type       dal_stmt_type;


public:
    Statement( typename db_traits<Engine, tag>::connection_type &dbc )
        : StmtInterface(),
          m_stmt( dbc.newStatement() )
        {}

    virtual ~Statement(void)
        {}


    virtual bool      isBad(void) const              { return this->m_stmt->isBad(); }

    virtual void      prepare(i18n::UString sql)     { return this->m_stmt->prepare(sql); }

    virtual bool      isPrepared(void) const         { return this->m_stmt->isPrepared(); }

    virtual void      execute(void)                  { this->m_stmt->execute(); }

    virtual void      execDirect(i18n::UString sql)  { this->m_stmt->execDirect(sql); }

    virtual void      close(void)                    { this->m_stmt->close(); }

    virtual dal_resultset_type&        resultset(void)          { return this->m_stmt->resultset(); }

    virtual const dal_resultset_type&  resultset(void) const    { return this->m_stmt->resultset(); }

    //virtual void      setResultsetMode(dal::ResultsetMode mode) { } // delete me

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


    virtual dal_stmt_type*     getImpl(void)   { return this->m_stmt.get(); }

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
public:
    typedef int iterator;
    typedef int const_iterator;

    typedef typename db_traits<Engine, tag>::statement_type      statement_type;
    typedef typename db_traits<Engine, tag>::dal_resultset_type  dal_resultset_type;
    typedef typename db_traits<Engine, tag>::dal_columndesc_type dal_columndesc_type;
    typedef typename db_traits<Engine, tag>::value_type          value_type;

public:

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
//GEXAMPLE.001 dalstate_t state = dbc.connect(L"sampledb.sqlitedb");
//GEXAMPLE.001 if(! dbc.isConnected() )
//GEXAMPLE.001    throw std::runtime_error("not connected");
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


//GEXAMPLE.001 With the methods 'begin()', 'next()' and 'eof()' you can iterate over the
//GEXAMPLE.001 records.
//GEXAMPLE.001 [source,cpp]
//GEXAMPLE.001 ------------------------------------------------------------------------------
//GEXAMPLE.001 for(rs.begin(); !rs.eof(); rs.next()) { //for each record }
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
//GEXAMPLE.001 for(rs.begin(); !rs.eof(); rs.next())
//GEXAMPLE.001 {
//GEXAMPLE.001 	for(int i = 1; i <= rs.columnCount(); ++i)
//GEXAMPLE.001    	{
//GEXAMPLE.001 		if(rs.column(i).isNull())
//GEXAMPLE.001 			std::cout << "<NULL>";
//GEXAMPLE.001 		else
//GEXAMPLE.001 			std::cout << rs.column(i).asNarrowStr("UTF-8");
//GEXAMPLE.001 	}
//GEXAMPLE.001 	std::cout << std::endl;
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
//GCORE.001 If the underlaying column type is of type BLOB (Variant::daltype() == DAL_TYPE_BLOB),
//GCORE.001 a convert_error exception is raised.
//GCORE.001 All other types are converted into a std::wstreambuf.
//GCORE.001 
//GCORE.001 If you have obtained a buffer pointer with asBlob() or asMemo(), the pointer
//GCORE.001 is valid (for all rows) until you close the resultset or statement.
//GCORE.001 Scrolling through the dataset (e.g. next()) only resets the buffer to the new
//GCORE.001 row data. If the new field is NULL, the buffer is marked as bad (std::ios::bad).
//GCORE.001 

//GCORE.001 Value
//GCORE.001 ~~~~~
//GCORE.001 ColumnDesc
//GCORE.001 ~~~~~~~~~~
//GCORE.001 Variant
//GCORE.001 ~~~~~~~
//GCORE.001 
//GCORE.001 Blob
//GCORE.001 ~~~~
//GCORE.001 
//GCORE.001 Memo
//GCORE.001 ~~~~
//GCORE.001 
//GCORE.001 
//GCORE.001 Exceptions
//GCORE.001 ----------
//GCORE.001 
//GCORE.001 
//GCORE.001 Internals
//GCORE.001 ---------
//GCORE.001 
//GCORE.001 DAL Layer
//GCORE.001 ~~~~~~~~~
//GCORE.001 


//------------------------------------------------------------------------------
///
/// @brief Defines the main types
template<typename Engine, typename tag = default_tag>
struct Database
{
    typedef typename db_traits<Engine, tag>::environment_type         Environment;
    typedef typename db_traits<Engine, tag>::connection_type          Connection;
    typedef typename db_traits<Engine, tag>::statement_type           Statement;
    typedef typename db_traits<Engine, tag>::resultset_type           Resultset;
    typedef typename db_traits<Engine, tag>::cached_resultset_type    CachedResultset;
    typedef typename db_traits<Engine, tag>::value_type               Value;
    typedef typename db_traits<Engine, tag>::dal_columndesc_type      ColumnDesc;
    typedef dal::Variant                                              Variant;
    typedef dal::Blob                                                 Blob;
    typedef dal::Memo                                                 Memo;
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
