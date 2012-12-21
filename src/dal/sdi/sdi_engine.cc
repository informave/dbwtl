//
// sdi_engine.cc - SDI engine (definition)
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
/// @brief SDI engine (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/dal/engines/generic_engine.hh"
#include "dbwtl/dal/engines/sdi_engine.hh"
#include "sdi_libsdi.hh"
#include "../dal_debug.hh"
#include "../../utils.hh"

//#include "../../macros.hh"

#include <sstream>
#include <string>

//#include "../sqlproxy/sqlproxy.hh"

DB_NAMESPACE_BEGIN




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

signed int
sv_accessor<SDIData*>::cast(signed int*, std::locale loc) const
{ 
    return this->get_value()->getInt(); 
}

unsigned int
sv_accessor<SDIData*>::cast(unsigned int*, std::locale loc) const
{ 
    return this->get_value()->getUInt(); 
}


signed long long
sv_accessor<SDIData*>::cast(signed long long*, std::locale loc) const
{ 
    return this->get_value()->getBigint(); 
}

unsigned long long
sv_accessor<SDIData*>::cast(unsigned long long*, std::locale loc) const
{ 
    return this->get_value()->getUBigint(); 
}



signed short
sv_accessor<SDIData*>::cast(signed short*, std::locale loc) const
{ 
    return this->get_value()->getSmallint(); 
}

unsigned short
sv_accessor<SDIData*>::cast(unsigned short*, std::locale loc) const
{ 
    return this->get_value()->getUSmallint(); 
}



signed char
sv_accessor<SDIData*>::cast(signed char*, std::locale loc) const
{ 
    return this->get_value()->getChar(); 
}

unsigned char
sv_accessor<SDIData*>::cast(unsigned char*, std::locale loc) const
{ 
    return this->get_value()->getUChar(); 
}



float
sv_accessor<SDIData*>::cast(float*, std::locale loc) const
{ 
    return this->get_value()->getReal(); 
}


double
sv_accessor<SDIData*>::cast(double*, std::locale loc) const
{ 
    return this->get_value()->getReal(); 
}



bool
sv_accessor<SDIData*>::cast(bool*, std::locale loc) const
{
    return this->get_value()->getInt() > 0;
}


Blob
sv_accessor<SDIData*>::cast(Blob*, std::locale loc) const
{
    return Blob(this->get_value()->getBlob());
}


Memo
sv_accessor<SDIData*>::cast(Memo*, std::locale loc) const
{
    return Memo(this->get_value()->getMemo());
}



TDate
sv_accessor<SDIData*>::cast(TDate*, std::locale loc) const
{
    return TDate(this->cast((String*)0, loc));
/*
    String s = this->asStr(std::locale("C"));
    try
    {
        int a = Variant(s).get<int>();
        return TDate(TTimestamp(a));
    }
    catch(ex::convert_error &)
    {
        return TDate(s);
    }
*/
}

TTime
sv_accessor<SDIData*>::cast(TTime*, std::locale loc) const
{
    return TTime(this->cast((String*)0, loc));
/*
    String s = this->asStr(std::locale("C"));
    try
    {
        int a = Variant(s).get<int>();
        return TTime(TTimestamp(a));
    }
    catch(ex::convert_error &)
    {
        return TTime(std::string(s));
    }
*/
}


TTimestamp
sv_accessor<SDIData*>::cast(TTimestamp*, std::locale loc) const
{
    return TTimestamp(this->cast((String*)0, loc));
/*
    String s = this->asStr(std::locale("C"));
    try
    {
        int a = Variant(s).get<int>();
        return TTimestamp(a);
    }
    catch(ex::convert_error &)
    {
        return TTimestamp(std::string(s));
    }
*/
}



TNumeric
sv_accessor<SDIData*>::cast(TNumeric*, std::locale loc) const
{
    return TNumeric(this->cast((String*)0, loc));
}




String
sv_accessor<SDIData*>::cast(String*, std::locale loc) const
{

    std::wstringstream ss;

    switch(this->get_value()->daltype())
    {
    case DAL_TYPE_CHAR: return Variant(this->get_value()->getChar()).asStr(loc);
    case DAL_TYPE_UCHAR: return Variant(this->get_value()->getUChar()).asStr(loc);
    case DAL_TYPE_SMALLINT: return Variant(this->get_value()->getSmallint()).asStr(loc);
    case DAL_TYPE_USMALLINT: return Variant(this->get_value()->getUSmallint()).asStr(loc);
    case DAL_TYPE_INT: return Variant(this->get_value()->getInt()).asStr(loc);
    case DAL_TYPE_UINT: return Variant(this->get_value()->getUInt()).asStr(loc);
    case DAL_TYPE_BIGINT: return Variant(this->get_value()->getBigint()).asStr(loc);
    case DAL_TYPE_UBIGINT: return Variant(this->get_value()->getUBigint()).asStr(loc);
    case DAL_TYPE_NUMERIC: return this->get_value()->getNumeric().str();
    case DAL_TYPE_BOOL: return Variant(this->get_value()->getBool()).asStr(loc);
    case DAL_TYPE_STRING: return this->get_value()->getString();
    case DAL_TYPE_DATE: return Variant(this->get_value()->getDate()).asStr(loc);
    case DAL_TYPE_TIME: return Variant(this->get_value()->getTime()).asStr(loc);
    case DAL_TYPE_TIMESTAMP: return Variant(this->get_value()->getTimestamp()).asStr(loc);
    case DAL_TYPE_FLOAT: return Variant(this->get_value()->getFloat()).asStr(loc);
    case DAL_TYPE_DOUBLE: return Variant(this->get_value()->getReal()).asStr(loc);

    case DAL_TYPE_BLOB:
        return Variant(Blob(this->get_value()->getBlob())).asStr(loc);
	case DAL_TYPE_MEMO:
        return Memo(this->get_value()->getMemo()).str();

    default:
        return "UNKNOWN";
        assert(!"unknown type");
    }
}

bool
sv_accessor<SDIData*>::valueNullCheck() const
{
    return this->get_value()->isnull();
}

daltype_t
sv_accessor<SDIData*>::datatype() const
{
    return this->get_value()->daltype();
}



/*


void
variant_assign<SDIData*>::set_new_value(SDIData*& dest, const Variant &src)
{
    throw ex::read_only("SDIData", DBWTL_MACRO_SRCPOS);
}
*/


///
///
IVariantValue*
SDIData::do_deepcopy(const IVariantValue *owner) const
{
    // we make a copy of the variant_storage owner for this instance, so we can use
    // the get<T> method to reuse cast definitions.
    Variant tmp(owner->clone());

    Variant *var = &tmp;

    switch(tmp.datatype())
    {
    case DAL_TYPE_CUSTOM:     return new variant_value<String>(var->asStr());
    case DAL_TYPE_UNKNOWN:    return new variant_value<String>(var->asStr()); break;
    case DAL_TYPE_INT:        return new variant_value<signed int>(var->asInt()); break;
    case DAL_TYPE_UINT:       return new variant_value<unsigned int>(var->asUInt()); break;
    case DAL_TYPE_CHAR:       return new variant_value<signed char>(var->asChar()); break;
    case DAL_TYPE_UCHAR:      return new variant_value<unsigned char>(var->asUChar()); break;
    case DAL_TYPE_STRING:     return new variant_value<String>(var->asStr()); break;
    case DAL_TYPE_BOOL:       return new variant_value<bool>(var->asBool()); break;
    case DAL_TYPE_SMALLINT:   return new variant_value<signed short>(var->asSmallint()); break;
    case DAL_TYPE_USMALLINT:  return new variant_value<unsigned short>(var->asUSmallint()); break;
    case DAL_TYPE_BIGINT:     return new variant_value<signed long long>(var->asBigint()); break;
    case DAL_TYPE_UBIGINT:    return new variant_value<unsigned long long>(var->asUBigint()); break;
    case DAL_TYPE_BLOB:       return new variant_value<BlobStream>(var->asBlob()); break;
    case DAL_TYPE_MEMO:       return new variant_value<MemoStream>(var->asMemo()); break;
    case DAL_TYPE_NUMERIC:    return new variant_value<TNumeric>(var->asNumeric()); break;
    case DAL_TYPE_FLOAT:      return new variant_value<float>(var->asReal()); break;
    case DAL_TYPE_DOUBLE:     return new variant_value<double>(var->asDouble()); break;
    case DAL_TYPE_DATE:       return new variant_value<TDate>(var->asDate()); break;
    case DAL_TYPE_TIME:       return new variant_value<TTime>(var->asTime()); break;
    case DAL_TYPE_TIMESTAMP:  return new variant_value<TTimestamp>(var->asTimestamp()); break;
    case DAL_TYPE_INTERVAL:   return new variant_value<TInterval>(var->asInterval()); break;
    default:
        DBWTL_BUG_FMT("Unhandled datatype(%d)", int(var->datatype()));
    }
    DBWTL_BUG();
}



DB_NAMESPACE_END




DAL_NAMESPACE_BEGIN


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool
SDIEnv::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const SDIDiag&
SDIEnv::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}



bool
SDIStmt::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const SDIDiag&
SDIStmt::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}


bool
SDIDbc::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const SDIDiag&
SDIDbc::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SDIData::SDIData(void)
{}



///
///
SDIData::~SDIData(void)
{}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SDITable::SDITable(String dbname, SDIDataProvider& src)
    : m_name(DAL_TYPE_STRING, "SDITable::name"),
      m_catalog(DAL_TYPE_STRING, "SDITable::catalog"),
      m_schema(DAL_TYPE_STRING, "SDITable::schema"),
      m_comment(DAL_TYPE_STRING, "SDITable::comment"),
      m_ddl(DAL_TYPE_STRING, "SDITable::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "SDITable::systemobject")
{
    this->m_name = src.column("NAME");
    this->m_catalog = dbname;
    this->m_schema = src.column("SCHEMA");
    //this->m_ddl = src.column("sql");
    //this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
    this->m_systemobject.set<bool>(false);
}


 
///
///
SDITable::~SDITable(void)
{}


///
///
const ITable::value_type& SDITable::getName(void) const
{
    return this->m_name;
}

///
///
const ITable::value_type& SDITable::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const ITable::value_type& SDITable::getSchema(void) const
{
    return this->m_schema;
}

///
///
const ITable::value_type& SDITable::getComment(void) const
{
    return this->m_comment;
}

///
///
const ITable::value_type& SDITable::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const ITable::value_type& SDITable::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SDIIndex::SDIIndex(String dbname, SDIDataProvider& src)
    : m_name(DAL_TYPE_STRING, "SDIIndex::name"),
      m_table(DAL_TYPE_STRING, "SDIIndex::table"),
      m_catalog(DAL_TYPE_STRING, "SDIIndex::catalog"),
      m_schema(DAL_TYPE_STRING, "SDIIndex::schema"),
      m_comment(DAL_TYPE_STRING, "SDIIndex::comment"),
      m_ddl(DAL_TYPE_STRING, "SDIIndex::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "SDIIndex::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_table = src.column("tbl_name");
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}


 
///
///
SDIIndex::~SDIIndex(void)
{}


///
///
const IIndex::value_type& SDIIndex::getName(void) const
{
    return this->m_name;
}

///
///
const IIndex::value_type& SDIIndex::getTable(void) const
{
    return this->m_table;
}

///
///
const IIndex::value_type& SDIIndex::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const IIndex::value_type& SDIIndex::getSchema(void) const
{
    return this->m_schema;
}

///
///
const IIndex::value_type& SDIIndex::getComment(void) const
{
    return this->m_comment;
}

///
///
const IIndex::value_type& SDIIndex::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const IIndex::value_type& SDIIndex::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SDIView::SDIView(String dbname, SDIDataProvider& src)
    : m_name(DAL_TYPE_STRING, "SDIView::name"),
      m_catalog(DAL_TYPE_STRING, "SDIView::catalog"),
      m_schema(DAL_TYPE_STRING, "SDIView::schema"),
      m_comment(DAL_TYPE_STRING, "SDIView::comment"),
      m_ddl(DAL_TYPE_STRING, "SDIView::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "SDIView::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}


 
///
///
SDIView::~SDIView(void)
{}


///
///
const IView::value_type& SDIView::getName(void) const
{
    return this->m_name;
}

///
///
const IView::value_type& SDIView::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const IView::value_type& SDIView::getSchema(void) const
{
    return this->m_schema;
}

///
///
const IView::value_type& SDIView::getComment(void) const
{
    return this->m_comment;
}

///
///
const IView::value_type& SDIView::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const IView::value_type& SDIView::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SDICatalog::SDICatalog(String dbname)
    : m_name(DAL_TYPE_STRING, "SDICatalg::name"),
      m_comment(DAL_TYPE_STRING, "SDICatalog::comment"),
      m_ddl(DAL_TYPE_STRING, "SDICatalog::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "SDICatalog::systemobject")
{
    this->m_name = dbname;
    this->m_systemobject.set(false);
}


 
///
///
SDICatalog::~SDICatalog(void)
{}


///
///
const ICatalog::value_type& SDICatalog::getName(void) const
{
    return this->m_name;
}

///
///
const ICatalog::value_type& SDICatalog::getComment(void) const
{
    return this->m_comment;
}

///
///
const ICatalog::value_type& SDICatalog::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const ICatalog::value_type& SDICatalog::isSystemObject(void) const
{
    return this->m_systemobject;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SDIDatatype::SDIDatatype(void)
    : DatatypeBase()
{}


///
///
SDIDatatype::~SDIDatatype(void)
{}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
IndexList
SDIDbc::getIndices(const IIndexFilter&)
{
    IndexList list;
    SDIStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();


    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, tbl_name, sql, 0 AS sys")
            US(" FROM ")+dbname+US(".sdi_master WHERE type = 'index'");



        SDIStmt::ptr indices(this->newStatement());
        try
        {
            indices->prepare(sql_column_query);
            indices->execute();
        }
        catch(sdi::STATES::SQLSTATE_42000 &)
        {
            continue;
        }


        
        for(indices->resultset().first(); ! indices->resultset().eof(); indices->resultset().next())
        {
            //FIXME list.push_back(new SDIIndex(dbname, indices->resultset()));
        }


    }

    // load temp tables
    String::Internal sql_column_query =
        US(" SELECT name, tbl_name, sql, CASE WHEN name IN ('sdi_stat1', 'sdi_sequence') THEN 1 ELSE 0 END AS sys")
        US(" FROM sdi_temp_master")
        US(" WHERE type = 'index';");
    



    SDIStmt::ptr indices(this->newStatement());        
    indices->prepare(sql_column_query);
    indices->execute();
    
    for(indices->resultset().first(); ! indices->resultset().eof(); indices->resultset().next())
    {
        //FIXME list.push_back(new SDIIndex("temp", indices->resultset()));
    }
    
    return list;
}


//
ProcedureList
SDIDbc::getProcedures(const IProcedureFilter&)
{
    ProcedureList list;
    return list;
}


//
SchemaList
SDIDbc::getSchemas(const ISchemaFilter&)
{
    SchemaList list;
    return list;
}



//
ViewList
SDIDbc::getViews(const IViewFilter&)
{
    ViewList list;
    SDIStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, sql, 0 AS sys")
            US(" FROM ")+dbname+US(".sdi_master WHERE type = 'view'");

        SDIStmt::ptr views(this->newStatement());
        try
        {
            views->prepare(sql_column_query);
            views->execute();
        }
        catch(sdi::STATES::SQLSTATE_42000 &)
        {
            continue;
        }
        
        for(views->resultset().first(); ! views->resultset().eof(); views->resultset().next())
        {
            //FIXME list.push_back(new SDIView(dbname, views->resultset()));
        }
    }

    // load temp views
/*
  String::Internal sql_column_query =
  US(" SELECT name, sql, CASE WHEN name IN ('sdi_stat1', 'sdi_sequence') THEN 1 ELSE 0 END AS sys")
  US(" FROM sdi_temp_master")
  US(" WHERE type = 'view';");
    
  SDIStmt::ptr tables(this->newStatement());        
  tables->prepare(sql_column_query);
  tables->execute();
    
  for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
  {
  list.push_back(new SDITable("temp", tables->resultset()));
  }
*/  
    return list;
}


//
CatalogList
SDIDbc::getCatalogs(const ICatalogFilter&)
{
    CatalogList list;
    SDIStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();

        list.push_back(new SDICatalog(dbname));
    }

    // load temp views
/*
  String::Internal sql_column_query =
  US(" SELECT name, sql, CASE WHEN name IN ('sdi_stat1', 'sdi_sequence') THEN 1 ELSE 0 END AS sys")
  US(" FROM sdi_temp_master")
  US(" WHERE type = 'view';");
    
  SDIStmt::ptr tables(this->newStatement());        
  tables->prepare(sql_column_query);
  tables->execute();
    
  for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
  {
  list.push_back(new SDITable("temp", tables->resultset()));
  }
*/  
    return list;
}



//
/// @todo use cache
DatatypeList
SDIDbc::getDatatypes(const IDatatypeFilter& filter)
{
    DatatypeList dtlist;
    SDIDatatype *dt = 0;
    
    dt = new SDIDatatype();
    dt->m_name.set(String("BLOB"));
    dt->m_size.set<signed int>(-1);
    dt->m_daltype = DAL_TYPE_BLOB;
    dtlist.push_back(dt);

    dt = new SDIDatatype();
    dt->m_name.set(String("INTEGER"));
    dt->m_size.set<signed int>(-1);
    dt->m_daltype = DAL_TYPE_BIGINT;
    dt->m_is_unsigned.set(false);
    dtlist.push_back(dt);

    dt = new SDIDatatype();
    dt->m_name.set(String("REAL"));
    dt->m_size.set<signed int>(sizeof(double));
    dt->m_daltype = DAL_TYPE_DOUBLE;
    dt->m_is_unsigned.set(false);
    dtlist.push_back(dt);

    dt = new SDIDatatype();
    dt->m_name.set(String("TEXT"));
    dt->m_size.set<signed int>(65000);
    dt->m_daltype = DAL_TYPE_STRING;
    dt->m_literal_prefix.set(String("'"));
    dt->m_literal_suffix.set(String("'"));
    dtlist.push_back(dt);
    
   
    return dtlist;
}



//
void
SDIDbc::beginTrans(trx_mode mode,
                      access_mode access,
                      String name)
{
    std::string s_cmd("BEGIN TRANSACTION ");
    this->directCmd(s_cmd);
}



//
void      
SDIDbc::commit(void)
{
    this->directCmd("COMMIT");
}


//
void      
SDIDbc::commit(Transaction trx)
{
    this->directCmd("COMMIT");
}



//
void    
SDIDbc::savepoint(String name)
{
    String s("SAVEPOINT ");
    s.append(name);
    this->directCmd(s);
}



//
void    
SDIDbc::rollback(String name)
{
    String s("ROLLBACK");
    if(! name.empty())
    {
        s.append(" TO SAVEPOINT ");
        s.append(name);
    }
    this->directCmd(s);
}


//
void      
SDIDbc::rollback(Transaction trx)
{
    this->directCmd("ROLLBACK");
}



//
void     
SDIDbc::directCmd(String cmd)
{
    SDIStmt::ptr stmt(this->newStatement());
    stmt->execDirect(cmd);
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SDIEnv::SDIEnv(void) : EnvBase(),
                             m_diag()
{ }



//
SDIEnv::~SDIEnv(void)
{ }



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SDIVariant::SDIVariant(SDIData* data)
    : EngineVariant(data), m_data(data)
{ }



//
SDIVariant::~SDIVariant(void)
{ 
    //this->m_storage->releasePointee();
    delete this->m_data;
}



//
void SDIVariant::refresh(void)
{
    //SDIData &data = dynamic_cast<sa_base<SDIData>&>(*this->getStorageImpl()).getValue();
    //SDIData *data = dynamic_cast<variant_value<SDIData*>&>(*this->getStorageImpl()).getRawValue();

    SDIData *data = this->get<SDIData*>();
    data->refresh();
}








//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
sdi::ENV*
sdi::createEnv(String driver)
{
    String::Internal drv;

    drv = parse_driver(driver)["driver"];
    if(! drv.length())
        goto err;

    else if(drv.compare(String("libsdi")) == 0)
        return new SDIEnv_libsdi(parse_driver(driver)["lib"]);

/*
  else if(drv.compare("odbc") == 0)
  return new SDIEnv_odbc(parse_driver(driver)["lib"]);
*/

    // nothing found..
err:
    throw ex::engine_error(US("Driver '") + drv + US("' is not a valid driver name."));
}



#define DAL_NAMEOF_STATE(state) case sdi_sqlstates::DAL_SDI_SQLSTATE_##state: return #state

const char*
sdi::sqlstate2string(STATES::engine_states_t id)
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
    throw ex::engine_error("Found BUG: Unhandled internal SQLSTATE. Please report this bug!");
}
#undef DAL_NAMEOF_STATE







//
SDIDiag::SDIDiag(dalstate_t state,
                 CodePosInfo pos,
                 Variant what,
                 String description)
    : DiagBase(state, pos, what, description),
      m_sqlstate_id() // fix?
{
    //m_sqlstate_id = sdi3error_to_sqlstate(sdi_code);
    //m_sqlstate.setWideStr(sqlstate_to_name(m_sqlstate_id), "UTF-8");
}


//
SDIDiag::SDIDiag(const SDIDiag& ref)
    : DiagBase(ref),
      m_sqlstate_id(ref.m_sqlstate_id)
{}





#define DAL_THROW_STATE(state)                          \
    case sdi_sqlstates::DAL_SDI_SQLSTATE_##state: \
    throw sdi::STATES::SQLSTATE_##state(*this)

//
void
SDIDiag::raiseException(void) const
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
    throw ex::engine_error("Found BUG: Unhandled internal SQLSTATE. Please report this bug!");
}

#undef DAL_THROW_STATE


DAL_NAMESPACE_END