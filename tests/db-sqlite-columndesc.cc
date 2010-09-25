#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>


int test(void)
{
    //using namespace informave;
    using namespace informave::db;
    using namespace informave::i18n;
    //using namespace informave::db::DAL;

    
    //typedef Database<DAL::sqlite> DBMS;

    typedef Database<dal::sqlite> DBMS;


    DBMS::Environment env("sqlite:libsqlite");
    //env.create("sqlite:libsqlite");

    DBMS::Connection dbc(env);
    dbc.connect(L"sampledb.sqlitedb");

    std::cout << conv_to(dbc.dbmsName(), "ISO-8859-1") << std::endl;



    dbc.directCmd(L"create temp table foo(id integer primary key, "
                  L"name VARCHAR(50), bday DATE, married BOOL);");



    DBMS::Statement stmt(dbc);
    stmt.execDirect(L"SELECT * from foo;");

    DBMS::Resultset res;
    res.attach(stmt);


    for(dal::colnum_t i = 0; i <= res.columnCount(); ++i)
    {
    	const DBMS::ColumnDesc &desc = res.describeColumn(i);
        std::cout << "name:      " << (desc.getName().isnull() ? "<null>" :
                                       desc.getName().asStr("UTF-8")) << std::endl;
        std::cout << "catalog:   " << (desc.getCatalogName().isnull() ? "<null>" :
                                       desc.getCatalogName().asStr("UTF-8")) << std::endl;
        std::cout << "schema:    " << (desc.getSchemaName().isnull() ? "<null>" :
                                       desc.getSchemaName().asStr("UTF-8")) << std::endl;
        std::cout << "base name  " << (desc.getBaseColumnName().isnull() ? "<null>" :
                                       desc.getBaseColumnName().asStr("UTF-8")) << std::endl;
        std::cout << "typename:  " << (desc.getTypeName().isnull() ? "<null>" :
                                       desc.getTypeName().asStr("UTF-8")) << std::endl;
        std::cout << "table:     " << (desc.getBaseTableName().isnull() ? "<null>" :
                                       desc.getBaseTableName().asStr("UTF-8")) << std::endl;
        std::cout << "size:      " << (desc.getSize().isnull() ? "<null>" :
                                       desc.getSize().asStr("UTF-8")) << std::endl;
        std::cout << "comment    " << (desc.getComment().isnull() ? "<null>" :
                                       desc.getComment().asStr("UTF-8")) << std::endl; 
        std::cout << "nullable:  " << (desc.getIsNullable().isnull() ? "<null>" :
                                       desc.getIsNullable().asStr("UTF-8")) << std::endl;
        std::cout << "precision  " << (desc.getPrecision().isnull() ? "<null>" :
                                       desc.getPrecision().asStr("UTF-8")) << std::endl;
        std::cout << "scale:     " << (desc.getScale().isnull() ? "<null>" :
                                       desc.getScale().asStr("UTF-8")) << std::endl;
        std::cout << "searchable " << (desc.getIsSearchable().isnull() ? "<null>" :
                                       desc.getIsSearchable().asStr("UTF-8")) << std::endl;

        std::cout << "type:      " << conv_to(daltype2string(desc.getDatatype()), "UTF-8") << std::endl;
        std::cout << std::endl;


    }


    return 0;
}



int main(void)
{
    try
    {
        test();
    }
    catch(informave::db::ex::exception &e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}

