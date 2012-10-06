#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/sqlite>

#include <iostream>
#include <locale>
#include <cassert>

const char* names[] =
{
    "Dalibor",
    "Dejan",
    "Taisiya",
    "Fatmir",
    "Merideth",
    "Keely",
    "Voldemaras",
    "Drusa",
    NULL
};

int test(void)
{
    using namespace informave::db;
    typedef Database<sqlite> DBMS;

    DBMS::Environment env("sqlite:libsqlite");
    DBMS::Connection dbc(env);
    dbc.connect(L"sampledb.sqlitedb");

    // drop table if exists
    try
    {
        dbc.directCmd("DROP TABLE list_index_dummy");
    }
    catch(...)
    {
    }

    dbc.directCmd(
        " CREATE TEMP TABLE list_index_dummy("
        "  id     INTEGER PRIMARY KEY"
        " ,name   TEXT NOT NULL"
        " ,CONSTRAINT force_uniqe UNIQUE(name)"
        " )");

    dbc.directCmd(
        " CREATE INDEX list_index_idx ON list_index_dummy(id, name)"
        );

    dbc.beginTrans(trx_read_committed);

    std::string name;

    DBMS::Statement stmt(dbc);
    stmt.prepare("INSERT INTO list_index_dummy(name) VALUES(?)");
    stmt.bind(1, &name);

    for(int i = 0; names[i]; ++i)
    {
        name = names[i];
        std::cout << "Insert: " << name << std::endl;
        stmt.execute();
    }

    dbc.commit();


    IndexList list = dbc.getIndices();

    return 0;
    
    for(IndexList::const_iterator i = list.begin();
        i != list.end();
        ++i)
    {
        std::cout << std::boolalpha;
        std::cout << "DB  : " << (*i)->getCatalog() << std::endl;
        std::cout << "NAME: " << (*i)->getName() << std::endl;
        std::cout << "TABLE: " << (*i)->getTable() << std::endl;
        std::cout << "SYS : " << (*i)->isSystemObject().asBool() << std::endl;
        std::cout << "DLL : " << ifnull<String>((*i)->getDDL(),
                                                String("NULL"))
                  << std::endl;

        

    }



    return 0;
}



int main(void)
{
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());

    try
    {
        return test();
    }
    catch(informave::db::ex::exception &e)
    {
        std::cout << "error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

