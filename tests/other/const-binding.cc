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
    using namespace informave;
    using namespace informave::db;


    // auto_ptr for scope deletion
    SqliteEnv::ptr env(Factory::create<sqlite>(L"libsqlite"));



    // db connection
    SqliteDbc::ptr dbc(env->newConnection());


    // now connect
    dbc->connect(L"sampledb.sqlitedb");


    // print current state
    //std::cout << Sqlite::engine_state(state);


    std::cout << "Driver name: " << dbc->driverName().to("ISO-8859-1") << std::endl;
    std::cout << "DBMS name: " << dbc->dbmsName().to("ISO-8859-1") << std::endl;


    

    SqliteStmt::ptr stmt(dbc->newStatement());

    dbc->directCmd(L"CREATE TEMP TABLE test(id INTEGER, name TEXT);");

    stmt->prepare(L"INSERT INTO test(id, name) VALUES(?, 'foo');");
    const int v = 104;
    stmt->bind(1, &v);

    stmt->execute();
    stmt->close();

    stmt->prepare(L"SELECT * from test;");
    stmt->execute();

    SqliteResult& res = stmt->resultset();    

    {
        const Variant& var = res.column(2);
    }

    stmt->close();

    dbc->disconnect();
    //std::cout << Sqlite::engine_state(state);

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

