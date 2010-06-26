#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>


int main(void)
{
    using namespace informave;
    using namespace informave::db::dal;


    // auto_ptr for scope deletion
    SqliteEnv::ptr env(Factory::create<sqlite>(L"libsqlite"));
    // db connection
    SqliteDbc::ptr dbc(env->newConnection());


    // now connect
    dalstate_t state = dbc->connect(L"sampledb.sqlitedb");
    if(state != DALSTATE_OK)
    {
        i18n::UString s = state.dump();
        //throw std::runtime_error(state.getMsgUTF8());
        throw std::runtime_error(i18n::conv_to(s, "UTF-8"));
    }

    // print current state
    //std::cout << Sqlite::engine_state(state);
    
    std::cout << "Driver name: " << i18n::conv_to(dbc->driverName(), "ISO-8859-1") << std::endl;
    std::cout << "DBMS name: " << i18n::conv_to(dbc->dbmsName(), "ISO-8859-1") << std::endl;


    

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
        const IVariant& var = res.column(2);
    }

    stmt->close();

    state = dbc->disconnect();
    //std::cout << Sqlite::engine_state(state);

    return state == DALSTATE_OK ? 0 : -1;
}
