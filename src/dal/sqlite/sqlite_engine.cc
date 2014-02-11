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
#include "../../utils.hh"

#include <sstream>
#include <string>



DB_NAMESPACE_BEGIN


SqliteMetadata*
SqliteDbc::newMetadata(void)
{
    return new SqliteMetadata(*this);
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::





static MetadataColumnDescriptor catalogsDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};

#define METADATA_DESC_COUNT(descs) (sizeof(descs)/sizeof(MetadataColumnDescriptor))

RecordSet
SqliteMetadata::getCatalogs(const Variant &catalog,
                            const ObjectClass system,
                            const DatasetFilter &filter)
{   
    RecordSet rs;
    rs.setColumnCount(2);

    assert(sizeof(catalogsDescs) / sizeof(MetadataColumnDescriptor) == 2);

    for(size_t i = 1; i <= METADATA_DESC_COUNT(catalogsDescs); ++i)
    {
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, String(catalogsDescs[i-1].name));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_SIZE, int(catalogsDescs[i-1].size));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_IS_NULLABLE, bool(catalogsDescs[i-1].nullable));
        rs.setDatatype(i, catalogsDescs[i-1].daltype);
    }

    RecordSet tmp(rs);
    assert(tmp.columnCount() == rs.columnCount());
    rs.open();



    SqliteStmt::ptr dblist(this->m_dbc.newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        std::cout << "read cat data" << std::endl;
        tmp.close();
        tmp.clear();
        tmp.open();
        ShrRecord rec(2);
        rec[0] = dblist->resultset().column("name");
        //rec[1] = rawRes.column("");
        tmp.insert(rec);
        tmp.first();
        if(filter(tmp))
        {
            rs.insert(*tmp.begin());
        }
    }
    return rs;
}



static MetadataColumnDescriptor schemaDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "SCHEMA_NAME",    DAL_TYPE_STRING, 0, true },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};


RecordSet
SqliteMetadata::getSchemas(const Variant &catalog,
                           const Variant &schema,
                           const ObjectClass system,
                           const DatasetFilter &filter)
{   
    RecordSet rs;
    rs.setColumnCount(3);

    assert(sizeof(schemaDescs) / sizeof(MetadataColumnDescriptor) == 3);

    for(size_t i = 1; i <= METADATA_DESC_COUNT(schemaDescs); ++i)
    {
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, String(schemaDescs[i-1].name));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_SIZE, int(schemaDescs[i-1].size));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_IS_NULLABLE, bool(schemaDescs[i-1].nullable));
        rs.setDatatype(i, schemaDescs[i-1].daltype);
    }

    RecordSet tmp(rs);
    
    assert(tmp.columnCount() == rs.columnCount());
    rs.open();

    //std::shared_ptr<OdbcStmt> rawStmt(this->m_dbc.getOdbcSchemas(catalog));
    //IResult &rawRes = rawStmt->resultset();



    ShrRecord rec(3);
    rec[0] = this->m_dbc.getCurrentCatalog();
    rec[1] = String("DEFAULT");
    //rec[2] = rawRes.column("REMARKS");
    tmp.open();
    tmp.insert(rec);
    tmp.first();
    if(filter(tmp))
    {
        rs.insert(*tmp.begin());
    }

    return rs;
}



static MetadataColumnDescriptor tableDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "SCHEMA_NAME",    DAL_TYPE_STRING, 0, true },
    { "TABLE_NAME",     DAL_TYPE_STRING, 0, false },
    { "TABLE_TYPE",     DAL_TYPE_STRING, 0, false },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};



RecordSet
SqliteMetadata::getTables(const Variant &schema,
                          const Variant &catalog,
                          const Variant &table,
                          const ObjectClass system,
                          const DatasetFilter &filter)
{   
    RecordSet rs;
    rs.setColumnCount(5);

    assert(sizeof(tableDescs) / sizeof(MetadataColumnDescriptor) == 5);

    for(size_t i = 1; i <= METADATA_DESC_COUNT(tableDescs); ++i)
    {
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, String(tableDescs[i-1].name));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_SIZE, int(tableDescs[i-1].size));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_IS_NULLABLE, bool(tableDescs[i-1].nullable));
        rs.setDatatype(i, tableDescs[i-1].daltype);
    }

    RecordSet tmp(rs);
    assert(tmp.columnCount() == rs.columnCount());
    rs.open();





    TableList list;
    SqliteStmt::ptr dblist(this->m_dbc.newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys")
            US(" FROM ")+dbname+US(".sqlite_master WHERE type = 'table'")
            US(" UNION")
            US(" SELECT 'sqlite_master', NULL, 1")
            US(" UNION")
            US(" SELECT 'sqlite_temp_master', NULL, 1");


        //std::cout << i18n::conv_to(sql_column_query, "UTF-8") << std::endl;

        SqliteStmt::ptr tables(this->m_dbc.newStatement());
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
            std::cerr << "FOUND TABLE" << std::endl;
            tmp.close();
            tmp.clear();
            tmp.open();
            ShrRecord rec(5);
            rec[0] = String(dbname);
            //rec[1] = NULL;
            rec[2] = tables->resultset().column("name");
            rec[3] = String("TABLE");
            tmp.insert(rec);
            tmp.first();
            if(filter(tmp))
            {
                rs.insert(*tmp.begin());
            }
        }
    }

    // load temp tables
    String::Internal sql_column_query =
        US(" SELECT name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys")
        US(" FROM sqlite_temp_master")
        US(" WHERE type = 'table';");
    
    SqliteStmt::ptr tables(this->m_dbc.newStatement());        
    tables->prepare(sql_column_query);
    tables->execute();
    
    for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
    {
        std::cerr << "FOUND OBDC TABLE" << std::endl;
        tmp.close();
        tmp.clear();
        tmp.open();
        ShrRecord rec(5);
        rec[0] = String("temp");
        //rec[1] = NULL;
        rec[2] = tables->resultset().column("name");
        rec[3] = String("TABLE");
        tmp.insert(rec);
        tmp.first();
        if(filter(tmp))
        {
            rs.insert(*tmp.begin());
        }
    }
    
    return rs;
}



static MetadataColumnDescriptor columnDescs[] = {
    { "CATALOG_NAME",     DAL_TYPE_STRING,  0, true  },
    { "SCHEMA_NAME",      DAL_TYPE_STRING,  0, true  },
    { "TABLE_NAME",       DAL_TYPE_STRING,  0, true  },
    { "COLUMN_NAME",      DAL_TYPE_STRING,  0, true  },
    { "COLUMN_TYPE",      DAL_TYPE_INT,     0, true  },
    { "TYPE_NAME",        DAL_TYPE_STRING,  0, true  },
    { "COLUMN_SIZE",      DAL_TYPE_INT,     0, true  },
    { "NULLABLE",         DAL_TYPE_BOOL,    0, true  },
    { "ORDINAL_POSITION", DAL_TYPE_INT,     0, true  },
    { "COMMENT",          DAL_TYPE_STRING,  0, true  }
};


RecordSet
SqliteMetadata::getColumns(const Variant &table,
                           const Variant &schema,
                           const Variant &catalog,
                           const Variant &column,
                           const ObjectClass system,
                           const DatasetFilter &filter)
{   
    RecordSet rs;
    rs.setColumnCount(10);

    assert(sizeof(columnDescs) / sizeof(MetadataColumnDescriptor) == 10);

    for(size_t i = 1; i <= METADATA_DESC_COUNT(columnDescs); ++i)
    {
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_NAME, String(columnDescs[i-1].name));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_SIZE, int(columnDescs[i-1].size));
        rs.modifyColumnDesc(i, DBWTL_COLUMNDESC_IS_NULLABLE, bool(columnDescs[i-1].nullable));
        rs.setDatatype(i, columnDescs[i-1].daltype);
    }

    RecordSet tmp(rs);
    assert(tmp.columnCount() == rs.columnCount());
    rs.open();

    RecordSet tables = this->getTables();
    for(tables.first(); !tables.eof(); tables.next())
    {
        if(!table.isnull())
        {
            std::cerr << "TABFILTER: \"" << tables.column("TABLE_NAME").asStr().utf8() << "\"/\"" << table.get<String>().utf8() << "\"" << std::endl;
            if(std::wstring(tables.column("TABLE_NAME").asStr()) != std::wstring(table.get<String>()))
                continue;
            else
                std::cerr << "match" << std::endl;
        }


        SqliteStmt::ptr columns(this->m_dbc.newStatement());        
        columns->prepare(String("PRAGMA table_info(") + tables.column("TABLE_NAME").asStr() + ")");
        columns->execute();
        IResult &rawRes = columns->resultset();


        for(rawRes.first(); !rawRes.eof(); rawRes.next())
        {
            tmp.close();
            tmp.clear();
            tmp.open();
            ShrRecord rec(10);
            rec[0] = this->m_dbc.getCurrentCatalog();
            rec[1] = String("DEFAULT");
            rec[2] = tables.column("TABLE_NAME");
            rec[3] = rawRes.column("name");
            //rec[4] = rawRes.column("COLUMN_TYPE");
            rec[5] = rawRes.column("type");
            //rec[6] = rawRes.column("COLUMN_SIZE");
            //rec[7] = rawRes.column("NULLABLE");
            //rec[8] = rawRes.column("ORDINAL_POSITION");
            //rec[9] = rawRes.column("REMARKS");
            //tmp.insert(ShrRecord(rawRes, std::mem_fun_ref(&IResult::columnByNumber), columnsToCopy));
            tmp.insert(rec);
            tmp.first();
            if(filter(tmp))
            {
        
                rs.insert(*tmp.begin());
//            skipit:;
            }
        }
    }
    return rs;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


signed int
sv_accessor<SqliteData*>::cast(signed int*, std::locale loc) const
{
    return this->get_value()->getInt(); 
}

bool
sv_accessor<SqliteData*>::cast(bool*, std::locale loc) const
{
    return this->get_value()->getInt() > 0;
}


BlobStream
sv_accessor<SqliteData*>::cast(BlobStream*, std::locale loc) const
{
    if(this->m_blob_buffer.get())
    {
        this->m_blob_buffer->seekg(0);
        return BlobStream(this->m_blob_buffer->rdbuf());
    }
    else
        return BlobStream(this->get_value()->getBlob());
}


Blob
sv_accessor<SqliteData*>::cast(Blob*, std::locale loc) const
{
    if(!this->m_blob_buffer.get())
    {
        this->m_blob_buffer.reset(new std::stringstream());
        (*this->m_blob_buffer.get()) << this->get_value()->getBlob();
    }
    this->m_blob_buffer->rdbuf()->pubseekpos(0);
    return Blob(this->m_blob_buffer->rdbuf());
}


TDate
sv_accessor<SqliteData*>::cast(TDate*, std::locale loc) const
{
    return TDate(this->get_value()->getString());

    //String s = this->asStr(std::locale("C"));
/*
  String s;
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
sv_accessor<SqliteData*>::cast(TTime*, std::locale loc) const
{
    return TTime(this->get_value()->getString());
/*
  String s = this->get_value()->getText();
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
sv_accessor<SqliteData*>::cast(TTimestamp*, std::locale loc) const
{
    return TTimestamp(this->get_value()->getString());
/*
  String s = this->get_value()->getText();
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
sv_accessor<SqliteData*>::cast(TNumeric*, std::locale loc) const
{
    std::string s = this->get_value()->getString();
    return TNumeric(s, loc);
}




String
sv_accessor<SqliteData*>::cast(String*, std::locale loc) const
{
    return this->get_value()->getString();
}

bool
sv_accessor<SqliteData*>::valueNullCheck() const
{
    return this->get_value()->isnull();
}

daltype_t
sv_accessor<SqliteData*>::datatype() const
{
    return this->get_value()->daltype();
}





DB_NAMESPACE_END




DAL_NAMESPACE_BEGIN




///
//
IVariantValue*
SqliteData::do_deepcopy(const IVariantValue *owner) const
{
    // The sv_accesor<SqliteData+> class uses buffers for BlobStreams.
    // We can't use clone() because this would create new buffers for the tmp object.
    if(owner->datatype() == DAL_TYPE_BLOB)
    {
    	return new value_traits<Blob>::stored_type(use_cast<Blob>(owner));
    }

    // we make a copy of the variant_storage owner for this instance, so we can use
    // the get<T> method to reuse cast definitions.
    Variant tmp(owner->clone());

    switch(tmp.datatype())
    {
    case DAL_TYPE_CUSTOM:     return new value_traits<String>::stored_type(tmp.get<String>());
    case DAL_TYPE_UNKNOWN:    return new value_traits<String>::stored_type(tmp.get<String>());
    case DAL_TYPE_INT:        return new value_traits<signed int>::stored_type(tmp.get<signed int>());       
    case DAL_TYPE_UINT:       return new value_traits<unsigned int>::stored_type(tmp.get<unsigned int>());
    case DAL_TYPE_CHAR:       return new value_traits<signed char>::stored_type(tmp.get<signed char>());
    case DAL_TYPE_UCHAR:      return new value_traits<unsigned char>::stored_type(tmp.get<unsigned char>());
    case DAL_TYPE_STRING:     return new value_traits<String>::stored_type(tmp.get<String>());
    case DAL_TYPE_BOOL:       return new value_traits<bool>::stored_type(tmp.get<bool>());
    case DAL_TYPE_SMALLINT:   return new value_traits<signed short>::stored_type(tmp.get<signed short>());
    case DAL_TYPE_USMALLINT:  return new value_traits<unsigned short>::stored_type(tmp.get<unsigned short>());
    case DAL_TYPE_BIGINT:     return new value_traits<signed long long>::stored_type(tmp.get<signed long long>());
    case DAL_TYPE_UBIGINT:    return new value_traits<unsigned long long>::stored_type(tmp.get<unsigned long long>());
    case DAL_TYPE_DATE:       return new value_traits<TDate>::stored_type(tmp.get<TDate>());
    case DAL_TYPE_TIME:       return new value_traits<TTime>::stored_type(tmp.get<TTime>());
    case DAL_TYPE_TIMESTAMP:  return new value_traits<TTimestamp>::stored_type(tmp.get<TTimestamp>());
    case DAL_TYPE_NUMERIC:    return new value_traits<TNumeric>::stored_type(tmp.get<TNumeric>());
    //case DAL_TYPE_BLOB:       return new value_traits<BlobStream>::stored_type(tmp.get<BlobStream>());
    //case DAL_TYPE_MEMO:       return new value_traits<MemoStream>::stored_type(tmp.get<MemoStream>());
    case DAL_TYPE_FLOAT:      return new value_traits<float>::stored_type(tmp.get<float>());
    case DAL_TYPE_DOUBLE:     return new value_traits<double>::stored_type(tmp.get<double>());
    case DAL_TYPE_INTERVAL:   return new value_traits<TInterval>::stored_type(tmp.get<TInterval>());

    default:
        throw EngineException(format("Unhandled datatype(%d)") % int(tmp.datatype()));
    }
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

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
///
SqliteTable::SqliteTable(String dbname, SqliteResult& src)
    : m_name(DAL_TYPE_STRING, "SqliteTable::name"),
      m_catalog(DAL_TYPE_STRING, "SqliteTable::catalog"),
      m_schema(DAL_TYPE_STRING, "SqliteTable::schema"),
      m_comment(DAL_TYPE_STRING, "SqliteTable::comment"),
      m_ddl(DAL_TYPE_STRING, "SqliteTable::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "SqliteTable::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
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
SqliteIndex::SqliteIndex(String dbname, SqliteResult& src)
    : m_name(DAL_TYPE_STRING, "SqliteIndex::name"),
      m_table(DAL_TYPE_STRING, "SqliteIndex::table"),
      m_catalog(DAL_TYPE_STRING, "SqliteIndex::catalog"),
      m_schema(DAL_TYPE_STRING, "SqliteIndex::schema"),
      m_comment(DAL_TYPE_STRING, "SqliteIndex::comment"),
      m_ddl(DAL_TYPE_STRING, "SqliteIndex::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "SqliteIndex::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_table = src.column("tbl_name");
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}


 
///
///
SqliteIndex::~SqliteIndex(void)
{}


///
///
const IIndex::value_type& SqliteIndex::getName(void) const
{
    return this->m_name;
}

///
///
const IIndex::value_type& SqliteIndex::getTable(void) const
{
    return this->m_table;
}

///
///
const IIndex::value_type& SqliteIndex::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const IIndex::value_type& SqliteIndex::getSchema(void) const
{
    return this->m_schema;
}

///
///
const IIndex::value_type& SqliteIndex::getComment(void) const
{
    return this->m_comment;
}

///
///
const IIndex::value_type& SqliteIndex::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const IIndex::value_type& SqliteIndex::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SqliteView::SqliteView(String dbname, SqliteResult& src)
    : m_name(DAL_TYPE_STRING, "SqliteView::name"),
      m_catalog(DAL_TYPE_STRING, "SqliteView::catalog"),
      m_schema(DAL_TYPE_STRING, "SqliteView::schema"),
      m_comment(DAL_TYPE_STRING, "SqliteView::comment"),
      m_ddl(DAL_TYPE_STRING, "SqliteView::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "SqliteView::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}


 
///
///
SqliteView::~SqliteView(void)
{}


///
///
const IView::value_type& SqliteView::getName(void) const
{
    return this->m_name;
}

///
///
const IView::value_type& SqliteView::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const IView::value_type& SqliteView::getSchema(void) const
{
    return this->m_schema;
}

///
///
const IView::value_type& SqliteView::getComment(void) const
{
    return this->m_comment;
}

///
///
const IView::value_type& SqliteView::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const IView::value_type& SqliteView::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
SqliteCatalog::SqliteCatalog(String dbname)
    : m_name(DAL_TYPE_STRING, "SqliteCatalg::name"),
      m_comment(DAL_TYPE_STRING, "SqliteCatalog::comment"),
      m_ddl(DAL_TYPE_STRING, "SqliteCatalog::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "SqliteCatalog::systemobject")
{
    this->m_name = dbname;
    this->m_systemobject.set(false);
}


 
///
///
SqliteCatalog::~SqliteCatalog(void)
{}


///
///
const ICatalog::value_type& SqliteCatalog::getName(void) const
{
    return this->m_name;
}

///
///
const ICatalog::value_type& SqliteCatalog::getComment(void) const
{
    return this->m_comment;
}

///
///
const ICatalog::value_type& SqliteCatalog::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const ICatalog::value_type& SqliteCatalog::isSystemObject(void) const
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
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys")
            US(" FROM ")+dbname+US(".sqlite_master WHERE type = 'table'")
            US(" UNION")
            US(" SELECT 'sqlite_master', NULL, 1")
            US(" UNION")
            US(" SELECT 'sqlite_temp_master', NULL, 1");


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
    String::Internal sql_column_query =
        US(" SELECT name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys")
        US(" FROM sqlite_temp_master")
        US(" WHERE type = 'table';");
    
    SqliteStmt::ptr tables(this->newStatement());        
    tables->prepare(sql_column_query);
    tables->execute();
    
    for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
    {
        list.push_back(new SqliteTable("temp", tables->resultset()));
    }
    
    return list;
}



//
IndexList
SqliteDbc::getIndices(const IIndexFilter&)
{
    IndexList list;
    SqliteStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();


    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, tbl_name, sql, 0 AS sys")
            US(" FROM ")+dbname+US(".sqlite_master WHERE type = 'index'");



        SqliteStmt::ptr indices(this->newStatement());
        try
        {
            indices->prepare(sql_column_query);
            indices->execute();
        }
        catch(sqlite::STATES::SQLSTATE_42000 &)
        {
            continue;
        }


        
        for(indices->resultset().first(); ! indices->resultset().eof(); indices->resultset().next())
        {
            list.push_back(new SqliteIndex(dbname, indices->resultset()));
        }


    }

    // load temp tables
    String::Internal sql_column_query =
        US(" SELECT name, tbl_name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys")
        US(" FROM sqlite_temp_master")
        US(" WHERE type = 'index';");
    



    SqliteStmt::ptr indices(this->newStatement());        
    indices->prepare(sql_column_query);
    indices->execute();
    
    for(indices->resultset().first(); ! indices->resultset().eof(); indices->resultset().next())
    {
        list.push_back(new SqliteIndex("temp", indices->resultset()));
    }
    
    return list;
}


//
ProcedureList
SqliteDbc::getProcedures(const IProcedureFilter&)
{
    ProcedureList list;
    return list;
}


//
SchemaList
SqliteDbc::getSchemas(const ISchemaFilter&)
{
    SchemaList list;
    return list;
}



//
ViewList
SqliteDbc::getViews(const IViewFilter&)
{
    ViewList list;
    SqliteStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, sql, 0 AS sys")
            US(" FROM ")+dbname+US(".sqlite_master WHERE type = 'view'");

        SqliteStmt::ptr views(this->newStatement());
        try
        {
            views->prepare(sql_column_query);
            views->execute();
        }
        catch(sqlite::STATES::SQLSTATE_42000 &)
        {
            continue;
        }
        
        for(views->resultset().first(); ! views->resultset().eof(); views->resultset().next())
        {
            list.push_back(new SqliteView(dbname, views->resultset()));
        }
    }

    // load temp views
/*
  String::Internal sql_column_query =
  US(" SELECT name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys")
  US(" FROM sqlite_temp_master")
  US(" WHERE type = 'view';");
    
  SqliteStmt::ptr tables(this->newStatement());        
  tables->prepare(sql_column_query);
  tables->execute();
    
  for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
  {
  list.push_back(new SqliteTable("temp", tables->resultset()));
  }
*/  
    return list;
}


//
CatalogList
SqliteDbc::getCatalogs(const ICatalogFilter&)
{
    CatalogList list;
    SqliteStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();

        list.push_back(new SqliteCatalog(dbname));
    }

    // load temp views
/*
  String::Internal sql_column_query =
  US(" SELECT name, sql, CASE WHEN name IN ('sqlite_stat1', 'sqlite_sequence') THEN 1 ELSE 0 END AS sys")
  US(" FROM sqlite_temp_master")
  US(" WHERE type = 'view';");
    
  SqliteStmt::ptr tables(this->newStatement());        
  tables->prepare(sql_column_query);
  tables->execute();
    
  for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
  {
  list.push_back(new SqliteTable("temp", tables->resultset()));
  }
*/  
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
    dt->m_name.set(String("BLOB"));
    dt->m_size.set<signed int>(-1);
    dt->m_daltype = DAL_TYPE_BLOB;
    dtlist.push_back(dt);

    dt = new SqliteDatatype();
    dt->m_name.set(String("INTEGER"));
    dt->m_size.set<signed int>(-1);
    dt->m_daltype = DAL_TYPE_BIGINT;
    dt->m_is_unsigned.set(false);
    dtlist.push_back(dt);

    dt = new SqliteDatatype();
    dt->m_name.set(String("REAL"));
    dt->m_size.set<signed int>(sizeof(double));
    dt->m_daltype = DAL_TYPE_DOUBLE;
    dt->m_is_unsigned.set(false);
    dtlist.push_back(dt);

    dt = new SqliteDatatype();
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
SqliteDbc::beginTrans(trx_mode mode,
                      access_mode access,
                      String name)
{
    if(mode == trx_read_uncommitted)
    {
        this->directCmd("PRAGMA read_uncommitted=true;");
    }
    else
        this->directCmd("PRAGMA read_uncommitted=false;");

    std::string s_cmd("BEGIN TRANSACTION;");
    this->directCmd(s_cmd);
}



//
void      
SqliteDbc::commit(void)
{
    this->directCmd("COMMIT");
}


void
SqliteDbc::commit(Transaction trx)
{
    DBWTL_NOTIMPL();
}

void
SqliteDbc::rollback(Transaction trx)
{
    DBWTL_NOTIMPL();
}


//
void    
SqliteDbc::savepoint(String name)
{
    String s("SAVEPOINT ");
    s.append(name);
    this->directCmd(s);
}



//
void    
SqliteDbc::rollback(String name)
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
SqliteDbc::directCmd(String cmd)
{
    SqliteStmt::ptr stmt(this->newStatement());
    stmt->execDirect(cmd);
}


String
SqliteDbc::quoteIdentifier(const String &id)
{
    return String("\"") + id + String("\"");
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SqliteEnv::SqliteEnv(void) : EnvBase(),
                             m_diag()
{ }



//
SqliteEnv::~SqliteEnv(void)
{ }



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SqliteVariant::SqliteVariant(SqliteData* data)
    : EngineVariant(data), m_data(data)
{ }



//
SqliteVariant::~SqliteVariant(void)
{ 
    //this->m_storage->release_pointee();
    delete this->m_data;
}



//
void SqliteVariant::refresh(void)
{
    //SqliteData &data = dynamic_cast<sa_base<SqliteData>&>(*this->getStorageImpl()).getValue();


    //SqliteData *data = dynamic_cast<variant_value<SqliteData*>&>(*this->getStorageImpl()).getRawValue();

    SqliteData *data = this->get<SqliteData*>();
    data->refresh();
}








//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
sqlite::ENV*
sqlite::createEnv(String driver)
{
    String::Internal drv;

    drv = parse_driver(driver)["driver"];
    if(! drv.length())
        goto err;

    else if(drv.compare(String("libsqlite")) == 0)
        return new SqliteEnv_libsqlite(parse_driver(driver)["lib"]);

/*
  else if(drv.compare("odbc") == 0)
  return new SqliteEnv_odbc(parse_driver(driver)["lib"]);
*/

    // nothing found..
err:
    throw EngineException(US("Driver '") + drv + US("' is not a valid driver name."));
}



#define DAL_NAMEOF_STATE(state) case sqlite_sqlstates::DAL_SQLITE_SQLSTATE_##state: return #state

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
    DBWTL_BUG_EX("Unhandled internal SQLSTATE. Please report this bug!");
}
#undef DAL_NAMEOF_STATE







//
SqliteDiag::SqliteDiag(dalstate_t state,
                       CodePosInfo pos,
                       Variant what,
                       String description)
    : DiagBase(state, pos, what, description),
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





#define DAL_THROW_STATE(state)                          \
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
    DBWTL_BUG_EX("Unhandled internal SQLSTATE. Please report this bug!");
}

#undef DAL_THROW_STATE


DAL_NAMESPACE_END
