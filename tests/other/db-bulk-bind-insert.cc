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
        dbc.directCmd("DROP TABLE bulk_bind_insert");
    }
    catch(...)
    {}

    dbc.directCmd(
        " CREATE TABLE bulk_bind_insert("
        "  id     INTEGER PRIMARY KEY"
        " ,name   TEXT NOT NULL"
        " ,CONSTRAINT force_uniqe UNIQUE(name)"
        " ,CONSTRAINT force_length CHECK(length(name) > 0)"
        " )");

    dbc.beginTrans(trx_read_committed);

    String name;

    DBMS::Statement stmt(dbc);
    stmt.prepare("INSERT INTO bulk_bind_insert(name) VALUES(? || ?)");
    //stmt.bind(1, &name);
    stmt.bind(1, static_cast<const String*>(&name));

    for(int i = 0; names[i]; ++i)
    {
        if(i == 3)
            dbc.savepoint("sp1");

        stmt.bind(2, name);
        name = std::string(names[i]);
        std::cout << "Insert: " << name << std::endl;
        stmt.execute();
    }
    dbc.savepoint("end");
    
    dbc.rollback("sp1");
    dbc.commit();

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

