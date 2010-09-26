//
// sqlite_engine.cc - SQLite engine (definition)
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
/// @brief SQLite engine (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/dal/engines/generic_engine.hh"
#include "dbwtl/dal/engines/sqlite_engine.hh"
#include "sqlite_libsqlite.hh"
#include "../dal_debug.hh"

#include <sstream>
#include <string>



DAL_NAMESPACE_BEGIN


bool
SqliteEnv::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const SqliteDiag&
SqliteEnv::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}



bool
SqliteStmt::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const SqliteDiag&
SqliteStmt::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}


bool
SqliteDbc::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const SqliteDiag&
SqliteDbc::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SqliteData::SqliteData(void)
{}



///
///
SqliteData::~SqliteData(void)
{}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
/// @todo implement other values, not only name
SqliteTable::SqliteTable(std::wstring dbname, SqliteResult& src)
    : m_name(DAL_TYPE_STRING, L"SqliteTable::name"),
      m_catalog(DAL_TYPE_STRING, L"SqliteTable::catalog"),
      m_schema(DAL_TYPE_STRING, L"SqliteTable::schema"),
      m_comment(DAL_TYPE_STRING, L"SqliteTable::comment"),
      m_ddl(DAL_TYPE_STRING, L"SqliteTable::ddl"),
      m_systemobject(DAL_TYPE_BOOL, L"SqliteTable::systemobject")
{
    this->m_name = src.column(L"name");
    this->m_catalog = dbname;
    this->m_ddl = src.column(L"sql");
    this->m_systemobject.setBool(ifnull<bool>(src.column(L"sys"), false));
}


 
///
///
SqliteTable::~SqliteTable(void)
{}


///
///
const ITable::value_type& SqliteTable::getName(void) const
{
    return this->m_name;
}

///
///
const ITable::value_type& SqliteTable::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const ITable::value_type& SqliteTable::getSchema(void) const
{
    return this->m_schema;
}

///
///
const ITable::value_type& SqliteTable::getComment(void) const
{
    return this->m_comment;
}

///
///
const ITable::value_type& SqliteTable::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const ITable::value_type& SqliteTable::isSystemObject(void) const
{
    return this->m_systemobject;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SqliteDatatype::SqliteDatatype(void)
    : DatatypeBase()
{}


///
///
SqliteDatatype::~SqliteDatatype(void)
{}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
TableList
SqliteDbc::getTables(const ITableFilter&)
{
    TableList list;
    SqliteStmt::ptr dblist(this->newStatement());
    dblist->prepare(L"PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        std::wstring dbname = dblist->resultset().column(L"name").asWideStr();
        
        std::wstring sql_column_query =
            L" SELECT name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys"
            L" FROM "+dbname+L".sqlite_master WHERE type = 'table'"
            L" UNION"
            L" SELECT 'sqlite_master', NULL, 1"
            L" UNION"
            L" SELECT 'sqlite_temp_master', NULL, 1";


        //std::cout << i18n::conv_to(sql_column_query, "UTF-8") << std::endl;

        SqliteStmt::ptr tables(this->newStatement());
        try
        {
            tables->prepare(sql_column_query);
            tables->execute();
        }
        catch(sqlite::STATES::SQLSTATE_42000 &)
        {
            continue;
        }
        
        for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
        {
            list.push_back(new SqliteTable(dbname, tables->resultset()));
        }
    }

    // load temp tables
    std::wstring sql_column_query =
        L" SELECT name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys"
        L" FROM sqlite_temp_master"
        L" WHERE type = 'table';";
    
    SqliteStmt::ptr tables(this->newStatement());        
    tables->prepare(sql_column_query);
    tables->execute();
    
    for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
    {
        list.push_back(new SqliteTable(L"temp", tables->resultset()));
    }
    
    return list;
}



//
/// @todo use cache
DatatypeList
SqliteDbc::getDatatypes(const IDatatypeFilter& filter)
{
    DatatypeList dtlist;
    SqliteDatatype *dt = 0;
    
    dt = new SqliteDatatype();
    dt->m_name.setWideStr(L"BLOB");
    dt->m_size.setInt(-1);
    dt->m_daltype = DAL_TYPE_BLOB;
    dtlist.push_back(dt);

    dt = new SqliteDatatype();
    dt->m_name.setWideStr(L"INTEGER");
    dt->m_size.setInt(-1);
    dt->m_daltype = DAL_TYPE_BIGINT;
    dt->m_is_unsigned.setBool(false);
    dtlist.push_back(dt);

    dt = new SqliteDatatype();
    dt->m_name.setWideStr(L"REAL");
    dt->m_size.setInt(sizeof(double));
    dt->m_daltype = DAL_TYPE_DOUBLE;
    dt->m_is_unsigned.setBool(false);
    dtlist.push_back(dt);

    dt = new SqliteDatatype();
    dt->m_name.setWideStr(L"TEXT");
    dt->m_size.setInt(65000);
    dt->m_daltype = DAL_TYPE_STRING;
    dt->m_literal_prefix.setWideStr(L"'");
    dt->m_literal_suffix.setWideStr(L"'");
    dtlist.push_back(dt);
    
   
    return dtlist;
}



//
void
SqliteDbc::beginTrans(IDbc::trx_mode mode,
                      IDbc::access_mode access,
                      std::wstring name)
{
    std::string s_cmd("BEGIN TRANSACTION ");
    this->directCmd(i18n::conv_from(s_cmd, "UTF-8"));
}



//
void      
SqliteDbc::commit(void)
{
    this->directCmd(L"COMMIT");
}



//
void    
SqliteDbc::savepoint(std::wstring name)
{
    std::wstring s(L"SAVEPOINT ");
    s.append(name);
    this->directCmd(s);
}



//
void    
SqliteDbc::rollback(std::wstring name)
{
    std::wstring s(L"ROLLBACK");
    if(! name.empty())
    {
        s.append(L" TO SAVEPOINT ");
        s.append(name);
    }
    this->directCmd(s);
}



//
void     
SqliteDbc::directCmd(std::wstring cmd)
{
    SqliteStmt::ptr stmt(this->newStatement());
    stmt->execDirect(cmd);
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SqliteEnv::SqliteEnv(void) : EnvBase()
{ }



//
SqliteEnv::~SqliteEnv(void)
{ }



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SqliteVariant::SqliteVariant(SqliteData* data)
    : Variant(data)

{ }



//
SqliteVariant::~SqliteVariant(void)
{ 
    this->m_storage->releasePointee();
}



//
void SqliteVariant::refresh(void)
{
	/// @bug implement refresh()
}








//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
sqlite::ENV*
sqlite::createEnv(std::wstring driver)
{
    std::wstring drv;

    drv = parse_driver(driver)["driver"];
    if(! drv.length())
        goto err;

    else if(drv.compare(L"libsqlite") == 0)
        return new SqliteEnv_libsqlite(parse_driver(driver)["lib"]);

/*
  else if(drv.compare(L"odbc") == 0)
  return new SqliteEnv_odbc(parse_driver(driver)["lib"]);
*/

    // nothing found..
err:
    throw ex::engine_error(L"Driver \"" + drv + L"\" is not a valid driver name.");
}



#define DAL_NAMEOF_STATE(state)	case sqlite_sqlstates::DAL_SQLITE_SQLSTATE_##state: return #state

const char*
sqlite::sqlstate2string(STATES::engine_states_t id)
{
	switch(id)
    {
        DAL_NAMEOF_STATE(08000);
        DAL_NAMEOF_STATE(08001);
        DAL_NAMEOF_STATE(0A000);
        DAL_NAMEOF_STATE(22000);
        DAL_NAMEOF_STATE(23000);
        DAL_NAMEOF_STATE(25000);
        DAL_NAMEOF_STATE(25001);
        DAL_NAMEOF_STATE(25006);
        DAL_NAMEOF_STATE(28000);
        DAL_NAMEOF_STATE(42000);

        //DAL_NAMEOF_STATE(XY000);
    }
    throw ex::engine_error(L"Found BUG: Unhandled internal SQLSTATE. Please report this bug!");
}
#undef DAL_NAMEOF_STATE







//
SqliteDiag::SqliteDiag(dalstate_t state,
                       const char *codepos,
                       const char *func,
                       std::wstring message,
                       std::wstring description)
    : DiagBase(state, codepos, func, message, description),
      m_sqlstate_id() // fix?
{
    //m_sqlstate_id = sqlite3error_to_sqlstate(sqlite_code);
    //m_sqlstate.setWideStr(sqlstate_to_name(m_sqlstate_id), "UTF-8");
}


//
SqliteDiag::SqliteDiag(const SqliteDiag& ref)
    : DiagBase(ref),
      m_sqlstate_id(ref.m_sqlstate_id)
{}





#define DAL_THROW_STATE(state)                              \
        case sqlite_sqlstates::DAL_SQLITE_SQLSTATE_##state: \
            throw sqlite::STATES::SQLSTATE_##state(*this)

//
void
SqliteDiag::raiseException(void) const
{
	switch(this->m_sqlstate_id)
    {
        DAL_THROW_STATE(08000);
        DAL_THROW_STATE(08001);
        DAL_THROW_STATE(0A000);
        DAL_THROW_STATE(22000);
        DAL_THROW_STATE(23000);
        DAL_THROW_STATE(25000);
        DAL_THROW_STATE(25001);
        DAL_THROW_STATE(25006);
        DAL_THROW_STATE(28000);
        DAL_THROW_STATE(42000);

        //DAL_THROW_STATE(XY000);
    }
    throw ex::engine_error(L"Found BUG: Unhandled internal SQLSTATE. Please report this bug!");
}

#undef DAL_THROW_STATE


DAL_NAMESPACE_END
