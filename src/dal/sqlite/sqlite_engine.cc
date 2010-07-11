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
SqliteTable::SqliteTable(i18n::UString dbname, SqliteResult& src)
    : m_name(DAL_TYPE_VARCHAR, L"SqliteTable::name"),
      m_catalog(DAL_TYPE_VARCHAR, L"SqliteTable::catalog"),
      m_schema(DAL_TYPE_VARCHAR, L"SqliteTable::schema"),
      m_comment(DAL_TYPE_VARCHAR, L"SqliteTable::comment"),
      m_ddl(DAL_TYPE_VARCHAR, L"SqliteTable::ddl"),
      m_systemobject(DAL_TYPE_BOOL, L"SqliteTable::systemobject")
{
    this->m_name.assign(src.column(L"name")); /// @bug change to operator=
    this->m_catalog.setStr(dbname);
    this->m_schema.setStr(L"");
    //this->m_schema.setNull();
    this->m_comment.setStr(L"comment");
    this->m_ddl.assign(src.column(L"sql"));
    this->m_systemobject.setStr(L"sys");
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
SqliteDatatype::SqliteDatatype(SqliteDbc& dbc)
    : m_name(),
      m_daltype()
{}


///
///
SqliteDatatype::~SqliteDatatype(void)
{}


///
///
i18n::UString
SqliteDatatype::name(void) const
{
    return this->m_name;
}


///
///
daltype_t
SqliteDatatype::daltype(void) const
{
    return this->m_daltype;
}


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
        i18n::UString dbname = dblist->resultset().column(L"name").asStr();
        
        i18n::UString sql_column_query =
            L" SELECT name, sql FROM " + dbname + L".sqlite_master"
            L" WHERE type = 'table';";

        SqliteStmt::ptr tables(this->newStatement());
        try
        {
            tables->prepare(sql_column_query);
            tables->execute();
        }
        catch(ex::sql_error &)
        {
            continue;
        }
        
        for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
        {
            list.push_back(new SqliteTable(dbname, tables->resultset()));
        }
    }

    // load temp tables
    i18n::UString sql_column_query =
        L" SELECT name, sql FROM sqlite_temp_master"
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
    DatatypeList list;
    SqliteDatatype* var;

    var = new SqliteDatatype(*this);
    var->m_name = L"BLOB";
    var->m_daltype = DAL_TYPE_BLOB;
    list.push_back(var);

    var = new SqliteDatatype(*this);
    var->m_name = L"INTEGER";
    var->m_daltype = DAL_TYPE_BIGINT;
    list.push_back(var);

    var = new SqliteDatatype(*this);
    var->m_name = L"REAL";
    var->m_daltype = DAL_TYPE_DOUBLE;
    list.push_back(var);

    var = new SqliteDatatype(*this);
    var->m_name = L"TEXT";
    var->m_daltype = DAL_TYPE_VARCHAR;
    list.push_back(var);
    return list;
}



//
void
SqliteDbc::beginTrans(IDbc::trx_mode mode,
                      IDbc::access_mode access,
                      i18n::UString name)
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
SqliteDbc::savepoint(i18n::UString name)
{
    i18n::UString s(L"SAVEPOINT ");
    s.append(name);
    this->directCmd(s);
}



//
void    
SqliteDbc::rollback(i18n::UString name)
{
    i18n::UString s(L"ROLLBACK");
    if(! name.empty())
    {
        s.append(L" TO SAVEPOINT ");
        s.append(name);
    }
    this->directCmd(s);
}



//
void     
SqliteDbc::directCmd(i18n::UString cmd)
{
    SqliteStmt::ptr stmt(this->newStatement());
    stmt->execDirect(cmd);
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SqliteEnv::SqliteEnv(void)
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
SqliteBlob& SqliteVariant::asBlob(void) const
{
    return dynamic_cast<SqliteBlob&>(Variant::asBlob());
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SqliteEngineState::SqliteEngineState(void)
    : BaseEngineState(),
      m_drvcode(),
      m_drvcode_name()
{ }



//
SqliteEngineState::~SqliteEngineState(void)
{ }



//
SqliteEngineState*
SqliteEngineState::clone(void) const
{
    return new SqliteEngineState(*this);
}



//
void
SqliteEngineState::setDrvCode(int code, i18n::UString name)
{
    this->m_drvcode = code;
    this->m_drvcode_name = name;
}



//
i18n::UString
SqliteEngineState::dump(void) const
{
    std::wstringstream ss;

    ss << L"Message:" << std::endl 
       << this->m_msg << std::endl
       << L"[DAL_STATE = " << DAL_CODE(this->m_dalcode) 
       << L" (Code: " << this->m_dalcode << L" / 0x" << std::hex << this->m_dalcode << std::dec << ")]"
       << std::endl
       << L"[SQL_STATE = " << i18n::conv_from(&this->m_sqlstate[0], "ASCII") << L"]" << std::endl
       << L"[DRV_CODE  = " << this->m_drvcode_name << L" (Code: "<< this->m_drvcode << L")]" << std::endl;
  
    if( ! this->m_srcfunc.empty())
        ss << L"[SOURCE    = " << i18n::conv_from(this->m_srcfunc, "UTF-8") << L"]" << std::endl
           << L"[......    = " << i18n::conv_from(this->m_srcfile, "UTF-8") << L"]" << std::endl;


    for(std::vector<std::string>::const_iterator i = this->m_files.begin();
        i != this->m_files.end();
        ++i)
    {
        if(this->m_files.begin() == i)
            ss << L"[FILES     = " << i18n::conv_from(*i, "UTF-8") << L"]" << std::endl;
        else
            ss << L"[.....     = " << i18n::conv_from(*i, "UTF-8") << L"]" << std::endl;
    }

    ss << this->m_desc << std::endl << std::endl;
    return ss.str();
}






//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
sqlite::ENV*
sqlite::createEnv(i18n::UString driver)
{
    sqlite::STATE state;
    i18n::UString drv;

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
    state.setDALCode(DALSTATE_LIB_ERROR);
    state.setMsg(L"Driver \"" + drv + L"\" is not a valid driver name.");
    state.setSource(__FILE__, __FUNCTION__);
    throw ex::engine_error(state);
}













DAL_NAMESPACE_END
