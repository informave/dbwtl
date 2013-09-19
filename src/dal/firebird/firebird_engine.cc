//
// firebird_engine.cc - Firebird engine (definition)
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
/// @brief Firebird engine (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/dal/engines/generic_engine.hh"
#include "dbwtl/dal/engines/firebird_engine.hh"
#include "firebird_libfbclient.hh"
#include "../dal_debug.hh"
#include "../../utils.hh"


#include <sstream>
#include <string>
#include <locale>
#include <fstream>



DB_NAMESPACE_BEGIN


FirebirdMetadata*
FirebirdDbc::newMetadata(void)
{
    return new FirebirdMetadata(*this);
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::








MetadataColumnDescriptor catalogsDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};

#define METADATA_DESC_COUNT(descs) (sizeof(descs)/sizeof(MetadataColumnDescriptor))

RecordSet
FirebirdMetadata::getCatalogs(const Variant &catalog,
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


    rs.open();

    ShrRecord rec(2);
    rec[0] = this->m_dbc.getCurrentCatalog();
    //rec[1] = rawRes.column("");
    rs.insert(rec);
    
    return rs;
}



MetadataColumnDescriptor schemaDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "SCHEMA_NAME",    DAL_TYPE_STRING, 0, true },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};


RecordSet
FirebirdMetadata::getSchemas(const Variant &catalog,
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


    rs.open();

    ShrRecord rec(3);
    rec[0] = this->m_dbc.getCurrentCatalog();
    //rec[1] = String("DEFAULT"); /// @bug set to null
    //rec[2] = rawRes.column("REMARKS");
    rs.insert(rec);

    return rs;
}



MetadataColumnDescriptor tableDescs[] = {
    { "CATALOG_NAME",   DAL_TYPE_STRING, 0, true },
    { "SCHEMA_NAME",    DAL_TYPE_STRING, 0, true },
    { "TABLE_NAME",     DAL_TYPE_STRING, 0, false },
    { "TABLE_TYPE",     DAL_TYPE_STRING, 0, false },
    { "COMMENT",        DAL_TYPE_STRING, 0, true }
};



RecordSet
FirebirdMetadata::getTables(const Variant &schema,
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

    std::shared_ptr<FirebirdStmt> rawStmt(this->m_dbc.newStatement());
    rawStmt->execDirect("SELECT NULL, NULL, RDB$RELATION_NAME, 'TABLE', NULL FROM RDB$RELATIONS WHERE (rdb$system_flag is null or rdb$system_flag = 0)");

    IResult &rawRes = rawStmt->resultset();

    const int columnsToCopy = 5;

    for(rawRes.first(); !rawRes.eof(); rawRes.next())
    {
        std::cerr << "FOUND FIREBIRD TABLE" << std::endl;
        tmp.close();
        tmp.clear();
        tmp.open();
        tmp.insert(ShrRecord(rawRes, std::mem_fun_ref(&IResult::columnByNumber), columnsToCopy));
        tmp.first();
        if(filter(tmp))
        {
            rs.insert(*tmp.begin());
        }
    }
    std::cerr << "RETURN OBDC TABLE" << std::endl;
    return rs;
}



MetadataColumnDescriptor columnDescs[] = {
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
FirebirdMetadata::getColumns(const Variant &table,
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

    std::shared_ptr<FirebirdStmt> rawStmt(this->m_dbc.newStatement());
    if(!table.isnull())
    {
        rawStmt->prepare("SELECT RDB$FIELD_NAME, RDB$RELATION_NAME FROM RDB$RELATION_FIELDS WHERE RDB$RELATION_NAME = ?");
        rawStmt->bind(1, Variant(table));
        rawStmt->execute();
    }
    else
        rawStmt->execDirect("SELECT RDB$FIELD_NAME, RDB$RELATION_NAME FROM RDB$RELATION_FIELDS");

    IResult &rawRes = rawStmt->resultset();


    for(rawRes.first(); !rawRes.eof(); rawRes.next())
    {
        tmp.close();
        tmp.clear();
        tmp.open();
        ShrRecord rec(10);
        rec[0] = this->m_dbc.getCurrentCatalog();
        //rec[1] = String("DEFAULT");
        rec[2] = rawRes.column("RDB$RELATION_NAME");
        rec[3] = rawRes.column("RDB$FIELD_NAME");
        //rec[4] = rawRes.column("COLUMN_TYPE");
        //rec[5] = rawRes.column("TYPE_NAME");
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
        }
    }
    return rs;
}


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


signed int
sv_accessor<FirebirdData*>::cast(signed int*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_INT)
        return this->get_value()->getLong();
    else
        return Variant(this->deepcopy()).get<signed int>();
}

signed short
sv_accessor<FirebirdData*>::cast(signed short*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_SMALLINT)
        return this->get_value()->getSmallint();
    else
        return Variant(this->deepcopy()).get<signed short>();
}

signed long long
sv_accessor<FirebirdData*>::cast(signed long long*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_BIGINT)
        return this->get_value()->getInt64();
    else
        return Variant(this->deepcopy()).get<signed long long>();
}

float
sv_accessor<FirebirdData*>::cast(float*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_FLOAT)
        return this->get_value()->getFloat();
    else
        return Variant(this->deepcopy()).get<float>();
}

double
sv_accessor<FirebirdData*>::cast(double*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_DOUBLE)
        return this->get_value()->getDouble();
    else
        return Variant(this->deepcopy()).get<double>();
}


bool
sv_accessor<FirebirdData*>::cast(bool*, std::locale loc) const
{
    return Variant(this->deepcopy()).get<bool>();
}


BlobStream
sv_accessor<FirebirdData*>::cast(BlobStream*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_BLOB || this->get_value()->daltype() == DAL_TYPE_MEMO)
        return BlobStream(this->get_value()->getBlob());
    else
        throw ConvertException(this->datatype(), value_traits<BlobStream>::info_type::type());
}


MemoStream
sv_accessor<FirebirdData*>::cast(MemoStream*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_MEMO)
        return this->get_value()->getMemo();
    else
        throw ConvertException(this->datatype(), value_traits<MemoStream>::info_type::type());

}

TDate
sv_accessor<FirebirdData*>::cast(TDate*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_DATE)
        return this->get_value()->getDate();
    else
        return Variant(this->deepcopy()).get<TDate>();
    return TDate(this->get_value()->getDate());
}

TTime
sv_accessor<FirebirdData*>::cast(TTime*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_TIME)
        return this->get_value()->getTime();
    else
        return Variant(this->deepcopy()).get<TTime>();
}


TTimestamp
sv_accessor<FirebirdData*>::cast(TTimestamp*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_TIMESTAMP)
        return this->get_value()->getTimestamp();
    else
        return Variant(this->deepcopy()).get<TTimestamp>();
}



TNumeric
sv_accessor<FirebirdData*>::cast(TNumeric*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_NUMERIC)
        return this->get_value()->getNumeric();
    else
        return Variant(this->deepcopy()).get<TNumeric>();
}


TVarbinary
sv_accessor<FirebirdData*>::cast(TVarbinary*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_VARBINARY)
        return this->get_value()->getVarbinary();
    else
        return Variant(this->deepcopy()).get<TVarbinary>();
}


String
sv_accessor<FirebirdData*>::cast(String*, std::locale loc) const
{
    if(this->get_value()->daltype() == DAL_TYPE_STRING)
        return this->get_value()->getText();
    else
        return Variant(this->deepcopy()).get<String>();
}

bool
sv_accessor<FirebirdData*>::valueNullCheck() const
{
    return this->get_value()->isnull();
}

daltype_t
sv_accessor<FirebirdData*>::datatype() const
{
    return this->get_value()->daltype();
}





DB_NAMESPACE_END




DAL_NAMESPACE_BEGIN







//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

bool
FirebirdEnv::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const FirebirdDiag&
FirebirdEnv::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}



bool
FirebirdStmt::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const FirebirdDiag&
FirebirdStmt::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}


bool
FirebirdDbc::diagAvail(void) const
{
    return this->m_diag.diagAvail();
}

const FirebirdDiag&
FirebirdDbc::fetchDiag(void)
{
    return this->m_diag.fetchDiag();
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
FirebirdData::FirebirdData(void)
{}



///
///
FirebirdData::~FirebirdData(void)
{}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
FirebirdTable::FirebirdTable(String dbname, FirebirdResult& src)
    : m_name(DAL_TYPE_STRING, "FirebirdTable::name"),
      m_catalog(DAL_TYPE_STRING, "FirebirdTable::catalog"),
      m_schema(DAL_TYPE_STRING, "FirebirdTable::schema"),
      m_comment(DAL_TYPE_STRING, "FirebirdTable::comment"),
      m_ddl(DAL_TYPE_STRING, "FirebirdTable::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "FirebirdTable::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}


 
///
///
FirebirdTable::~FirebirdTable(void)
{}


///
///
const ITable::value_type& FirebirdTable::getName(void) const
{
    return this->m_name;
}

///
///
const ITable::value_type& FirebirdTable::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const ITable::value_type& FirebirdTable::getSchema(void) const
{
    return this->m_schema;
}

///
///
const ITable::value_type& FirebirdTable::getComment(void) const
{
    return this->m_comment;
}

///
///
const ITable::value_type& FirebirdTable::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const ITable::value_type& FirebirdTable::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
FirebirdIndex::FirebirdIndex(String dbname, FirebirdResult& src)
    : m_name(DAL_TYPE_STRING, "FirebirdIndex::name"),
      m_table(DAL_TYPE_STRING, "FirebirdIndex::table"),
      m_catalog(DAL_TYPE_STRING, "FirebirdIndex::catalog"),
      m_schema(DAL_TYPE_STRING, "FirebirdIndex::schema"),
      m_comment(DAL_TYPE_STRING, "FirebirdIndex::comment"),
      m_ddl(DAL_TYPE_STRING, "FirebirdIndex::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "FirebirdIndex::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_table = src.column("tbl_name");
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}


 
///
///
FirebirdIndex::~FirebirdIndex(void)
{}


///
///
const IIndex::value_type& FirebirdIndex::getName(void) const
{
    return this->m_name;
}

///
///
const IIndex::value_type& FirebirdIndex::getTable(void) const
{
    return this->m_table;
}

///
///
const IIndex::value_type& FirebirdIndex::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const IIndex::value_type& FirebirdIndex::getSchema(void) const
{
    return this->m_schema;
}

///
///
const IIndex::value_type& FirebirdIndex::getComment(void) const
{
    return this->m_comment;
}

///
///
const IIndex::value_type& FirebirdIndex::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const IIndex::value_type& FirebirdIndex::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
FirebirdView::FirebirdView(String dbname, FirebirdResult& src)
    : m_name(DAL_TYPE_STRING, "FirebirdView::name"),
      m_catalog(DAL_TYPE_STRING, "FirebirdView::catalog"),
      m_schema(DAL_TYPE_STRING, "FirebirdView::schema"),
      m_comment(DAL_TYPE_STRING, "FirebirdView::comment"),
      m_ddl(DAL_TYPE_STRING, "FirebirdView::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "FirebirdView::systemobject")
{
    this->m_name = src.column("name");
    this->m_catalog = dbname;
    this->m_ddl = src.column("sql");
    this->m_systemobject.set<bool>(ifnull<bool>(src.column("sys"), false));
}


 
///
///
FirebirdView::~FirebirdView(void)
{}


///
///
const IView::value_type& FirebirdView::getName(void) const
{
    return this->m_name;
}

///
///
const IView::value_type& FirebirdView::getCatalog(void) const
{
    return this->m_catalog;
}

///
///
const IView::value_type& FirebirdView::getSchema(void) const
{
    return this->m_schema;
}

///
///
const IView::value_type& FirebirdView::getComment(void) const
{
    return this->m_comment;
}

///
///
const IView::value_type& FirebirdView::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const IView::value_type& FirebirdView::isSystemObject(void) const
{
    return this->m_systemobject;
}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
FirebirdCatalog::FirebirdCatalog(String dbname)
    : m_name(DAL_TYPE_STRING, "FirebirdCatalg::name"),
      m_comment(DAL_TYPE_STRING, "FirebirdCatalog::comment"),
      m_ddl(DAL_TYPE_STRING, "FirebirdCatalog::ddl"),
      m_systemobject(DAL_TYPE_BOOL, "FirebirdCatalog::systemobject")
{
    this->m_name = dbname;
    this->m_systemobject.set(false);
}


 
///
///
FirebirdCatalog::~FirebirdCatalog(void)
{}


///
///
const ICatalog::value_type& FirebirdCatalog::getName(void) const
{
    return this->m_name;
}

///
///
const ICatalog::value_type& FirebirdCatalog::getComment(void) const
{
    return this->m_comment;
}

///
///
const ICatalog::value_type& FirebirdCatalog::getDDL(void) const
{
    return this->m_ddl;
}

///
///
const ICatalog::value_type& FirebirdCatalog::isSystemObject(void) const
{
    return this->m_systemobject;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
///
FirebirdDatatype::FirebirdDatatype(void)
    : DatatypeBase()
{}


///
///
FirebirdDatatype::~FirebirdDatatype(void)
{}



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
TableList
FirebirdDbc::getTables(const ITableFilter&)
{
    TableList list;
    FirebirdStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, sql, CASE WHEN name IN ('firebird_stat1', 'firebird_sequence') THEN 1 ELSE 0 END AS sys")
            US(" FROM ")+dbname+US(".firebird_master WHERE type = 'table'")
            US(" UNION")
            US(" SELECT 'firebird_master', NULL, 1")
            US(" UNION")
            US(" SELECT 'firebird_temp_master', NULL, 1");


        //std::cout << i18n::conv_to(sql_column_query, "UTF-8") << std::endl;

        FirebirdStmt::ptr tables(this->newStatement());
        try
        {
            tables->prepare(sql_column_query);
            tables->execute();
        }
        catch(firebird::STATES::SQLSTATE_42000 &)
        {
            continue;
        }
        
        for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
        {
            list.push_back(new FirebirdTable(dbname, tables->resultset()));
        }
    }

    // load temp tables
    String::Internal sql_column_query =
        US(" SELECT name, sql, CASE WHEN name IN ('firebird_stat1', 'firebird_sequence') THEN 1 ELSE 0 END AS sys")
        US(" FROM firebird_temp_master")
        US(" WHERE type = 'table';");
    
    FirebirdStmt::ptr tables(this->newStatement());        
    tables->prepare(sql_column_query);
    tables->execute();
    
    for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
    {
        list.push_back(new FirebirdTable("temp", tables->resultset()));
    }
    
    return list;
}



//
IndexList
FirebirdDbc::getIndices(const IIndexFilter&)
{
    IndexList list;
    FirebirdStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();


    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, tbl_name, sql, 0 AS sys")
            US(" FROM ")+dbname+US(".firebird_master WHERE type = 'index'");



        FirebirdStmt::ptr indices(this->newStatement());
        try
        {
            indices->prepare(sql_column_query);
            indices->execute();
        }
        catch(firebird::STATES::SQLSTATE_42000 &)
        {
            continue;
        }


        
        for(indices->resultset().first(); ! indices->resultset().eof(); indices->resultset().next())
        {
            list.push_back(new FirebirdIndex(dbname, indices->resultset()));
        }


    }

    // load temp tables
    String::Internal sql_column_query =
        US(" SELECT name, tbl_name, sql, CASE WHEN name IN ('firebird_stat1', 'firebird_sequence') THEN 1 ELSE 0 END AS sys")
        US(" FROM firebird_temp_master")
        US(" WHERE type = 'index';");
    



    FirebirdStmt::ptr indices(this->newStatement());        
    indices->prepare(sql_column_query);
    indices->execute();
    
    for(indices->resultset().first(); ! indices->resultset().eof(); indices->resultset().next())
    {
        list.push_back(new FirebirdIndex("temp", indices->resultset()));
    }
    
    return list;
}


//
ProcedureList
FirebirdDbc::getProcedures(const IProcedureFilter&)
{
    ProcedureList list;
    return list;
}


//
SchemaList
FirebirdDbc::getSchemas(const ISchemaFilter&)
{
    SchemaList list;
    return list;
}



//
ViewList
FirebirdDbc::getViews(const IViewFilter&)
{
    ViewList list;
    FirebirdStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();
        
        String::Internal sql_column_query =
            US(" SELECT name, sql, 0 AS sys")
            US(" FROM ")+dbname+US(".firebird_master WHERE type = 'view'");

        FirebirdStmt::ptr views(this->newStatement());
        try
        {
            views->prepare(sql_column_query);
            views->execute();
        }
        catch(firebird::STATES::SQLSTATE_42000 &)
        {
            continue;
        }
        
        for(views->resultset().first(); ! views->resultset().eof(); views->resultset().next())
        {
            list.push_back(new FirebirdView(dbname, views->resultset()));
        }
    }

    // load temp views
/*
  String::Internal sql_column_query =
  US(" SELECT name, sql, CASE WHEN name IN ('firebird_stat1', 'firebird_sequence') THEN 1 ELSE 0 END AS sys")
  US(" FROM firebird_temp_master")
  US(" WHERE type = 'view';");
    
  FirebirdStmt::ptr tables(this->newStatement());        
  tables->prepare(sql_column_query);
  tables->execute();
    
  for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
  {
  list.push_back(new FirebirdTable("temp", tables->resultset()));
  }
*/  
    return list;
}


//
CatalogList
FirebirdDbc::getCatalogs(const ICatalogFilter&)
{
    CatalogList list;
    FirebirdStmt::ptr dblist(this->newStatement());
    dblist->prepare("PRAGMA database_list");
    dblist->execute();
    
    for(dblist->resultset().first(); ! dblist->resultset().eof(); dblist->resultset().next())
    {
        String::Internal dbname = dblist->resultset().column("name").asStr();

        list.push_back(new FirebirdCatalog(dbname));
    }

    // load temp views
/*
  String::Internal sql_column_query =
  US(" SELECT name, sql, CASE WHEN name IN ('firebird_stat1', 'firebird_sequence') THEN 1 ELSE 0 END AS sys")
  US(" FROM firebird_temp_master")
  US(" WHERE type = 'view';");
    
  FirebirdStmt::ptr tables(this->newStatement());        
  tables->prepare(sql_column_query);
  tables->execute();
    
  for(tables->resultset().first(); ! tables->resultset().eof(); tables->resultset().next())
  {
  list.push_back(new FirebirdTable("temp", tables->resultset()));
  }
*/  
    return list;
}



//
/// @todo use cache
DatatypeList
FirebirdDbc::getDatatypes(const IDatatypeFilter& filter)
{
    DatatypeList dtlist;
    FirebirdDatatype *dt = 0;
    
    dt = new FirebirdDatatype();
    dt->m_name.set(String("BLOB"));
    dt->m_size.set<signed int>(-1);
    dt->m_daltype = DAL_TYPE_BLOB;
    dtlist.push_back(dt);

    dt = new FirebirdDatatype();
    dt->m_name.set(String("INTEGER"));
    dt->m_size.set<signed int>(-1);
    dt->m_daltype = DAL_TYPE_BIGINT;
    dt->m_is_unsigned.set(false);
    dtlist.push_back(dt);

    dt = new FirebirdDatatype();
    dt->m_name.set(String("REAL"));
    dt->m_size.set<signed int>(sizeof(double));
    dt->m_daltype = DAL_TYPE_DOUBLE;
    dt->m_is_unsigned.set(false);
    dtlist.push_back(dt);

    dt = new FirebirdDatatype();
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
FirebirdDbc::commit(void)
{
    this->directCmd("COMMIT");
}



//
void    
FirebirdDbc::savepoint(String name)
{
    String s("SAVEPOINT ");
    s.append(name);
    this->directCmd(s);
}



//
void    
FirebirdDbc::rollback(String name)
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
FirebirdDbc::directCmd(String cmd)
{
    FirebirdStmt::ptr stmt(this->newStatement());
    stmt->execDirect(cmd);
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
FirebirdEnv::FirebirdEnv(void) : EnvBase(),
                                 m_diag()
{ }



//
FirebirdEnv::~FirebirdEnv(void)
{ }



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
FirebirdVariant::FirebirdVariant(FirebirdData* data)
    : EngineVariant(data), m_data(data)
{ }



//
FirebirdVariant::~FirebirdVariant(void)
{ 
    //this->m_storage->release_pointee();
    delete this->m_data;
}



//
void FirebirdVariant::refresh(void)
{
    //FirebirdData &data = dynamic_cast<sa_base<FirebirdData>&>(*this->getStorageImpl()).getValue();


    //FirebirdData *data = dynamic_cast<variant_value<FirebirdData*>&>(*this->getStorageImpl()).getRawValue();

    FirebirdData *data = this->get<FirebirdData*>();
    data->refresh();
}








//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
firebird::ENV*
firebird::createEnv(String driver)
{
    String::Internal drv;

    drv = parse_driver(driver)["driver"];
    if(! drv.length())
        goto err;

    else if(drv.compare(String("libfbclient")) == 0)
        return new FirebirdEnv_libfbclient(parse_driver(driver)["lib"]);

/*
  else if(drv.compare("odbc") == 0)
  return new FirebirdEnv_odbc(parse_driver(driver)["lib"]);
*/

    // nothing found..
err:
    throw EngineException(US("Driver '") + drv + US("' is not a valid driver name."));
}



#define DAL_NAMEOF_STATE(state) case firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_##state: return #state

const char*
firebird::sqlstate2string(STATES::engine_states_t id)
{
    switch(id)
    {
        DAL_NAMEOF_STATE(08000);
        DAL_NAMEOF_STATE(08001);
        DAL_NAMEOF_STATE(08002);
        DAL_NAMEOF_STATE(08004);
        DAL_NAMEOF_STATE(0A000);
        DAL_NAMEOF_STATE(22000);
        DAL_NAMEOF_STATE(23000);
        DAL_NAMEOF_STATE(24000);
        DAL_NAMEOF_STATE(25000);
        DAL_NAMEOF_STATE(25001);
        DAL_NAMEOF_STATE(25006);
        DAL_NAMEOF_STATE(28000);
        DAL_NAMEOF_STATE(42000);

        DAL_NAMEOF_STATE(HY000);

        //DAL_NAMEOF_STATE(XY000);
    }
    DBWTL_BUG_EX("Unhandled internal SQLSTATE. Please report this bug!");
}
#undef DAL_NAMEOF_STATE







/// @detail
/// As default SQLSTATE, we set HY000
FirebirdDiag::FirebirdDiag(dalstate_t state,
                           CodePosInfo pos,
                           Variant what,
                           String description)
    : DiagBase(state, pos, what, description),
      m_sqlstate_id(firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_HY000)
{}


//
FirebirdDiag::FirebirdDiag(const FirebirdDiag& ref)
    : DiagBase(ref),
      m_sqlstate_id(ref.m_sqlstate_id)
{}





#define DAL_THROW_STATE(state)                              \
    case firebird_sqlstates::DAL_FIREBIRD_SQLSTATE_##state: \
    throw firebird::STATES::SQLSTATE_##state(*this)

//
void
FirebirdDiag::raiseException(void) const
{
    switch(this->m_sqlstate_id)
    {
        DAL_THROW_STATE(08000);
        DAL_THROW_STATE(08001);
        DAL_THROW_STATE(08002);
        DAL_THROW_STATE(08004);
        DAL_THROW_STATE(0A000);
        DAL_THROW_STATE(22000);
        DAL_THROW_STATE(23000);
        DAL_THROW_STATE(24000);
        DAL_THROW_STATE(25000);
        DAL_THROW_STATE(25001);
        DAL_THROW_STATE(25006);
        DAL_THROW_STATE(28000);
        DAL_THROW_STATE(42000);
        DAL_THROW_STATE(HY000);

        //DAL_THROW_STATE(XY000);
    }
    DBWTL_BUG_EX("Unhandled internal SQLSTATE. Please report this bug!");
}

#undef DAL_THROW_STATE


DAL_NAMESPACE_END
