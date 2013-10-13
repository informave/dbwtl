#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>

#include <iostream>
#include <fstream>
#include <memory>
#include <cstdlib>

#include "../cxxc.hh"
#include "fixture_sqlite3.hh"



CXXC_FIXTURE_TEST(SqliteGenericFixture, GenericTest)
{
    dbc.directCmd("CREATE TABLE test(id INTEGER PRIMARY KEY, data TEXT);");
    
    DBMS::Statement stmt(dbc);
    stmt.prepare("INSERT INTO test VALUES(?, ?)");
    stmt.bind(1, 1234);
    stmt.bind(2, String("testdata"));
    stmt.execute();

    stmt.close();
}


int main(void)
{
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    std::cerr.imbue(std::locale());
    std::clog.imbue(std::locale());
    std::wcout.imbue(std::locale());
    std::wcerr.imbue(std::locale());
    std::wclog.imbue(std::locale());

    return cxxc::runAll();
}

