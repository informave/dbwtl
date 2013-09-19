#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <fstream>
#include <memory>
#include <cstdlib>


int test(void)
{
    using namespace informave::db;
    typedef Database<sqlite> DBMS;

    DBMS::Environment env("sqlite:libsqlite");

    DBMS::Connection dbc(env);
    dbc.connect(":memory:");

    dbc.directCmd("CREATE TABLE test(id INTEGER PRIMARY KEY, data TEXT);");
    
    DBMS::Statement stmt(dbc);
    stmt.prepare("INSERT INTO test VALUES(?, ?)");
    stmt.bind(1, 1234);
    stmt.bind(2, String("testdata"));
    stmt.execute();

    DBMS::Variant id = stmt.lastInsertRowId();
    
    stmt.close();

    return (id.asBigint() == 1234) ? 0 : -1;
}



int main(void)
{
    try
    {
        return test();
    }
    catch(informave::db::Exception &e)
    {
        std::cout << "error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

