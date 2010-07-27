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
    using informave::db::dal::dalstate_t;
    //using namespace informave::db::DAL;


    typedef Database<dal::sqlite> DBMS;

    DBMS::Environment env("sqlite:libsqlite");

    DBMS::Connection dbc(env);
    dalstate_t state = dbc.connect(L"sampledb.sqlitedb");
    if(! dbc.isConnected() )
        throw std::runtime_error("not connected");

    std::cout << conv_to(dbc.dbmsName(), "ISO-8859-1") << std::endl;


    //dbc.directCmd(L"create table foo(id int, data BLOB);");

    DBMS::Statement stmt(dbc);
    stmt.prepare(L"INSERT INTO foo VALUES(?, ?, ?);");

    std::ifstream ss("/home/cytrinox/informave/dbwtl/build/testimg.jpg", std::ios::binary);
    std::ifstream ss2("/home/cytrinox/informave/dbwtl/build/test2.jpg", std::ios::binary);

    int c = 100;
    stmt.bind(1, &c);
    stmt.bind(2, ss.rdbuf());
    stmt.bind(3, ss2.rdbuf());

    for(int cc = c; c <= cc+10; ++c)
    {
        ss.seekg(0, std::ios::beg);
        ss2.seekg(0, std::ios::beg);
        stmt.execute();
    }

    stmt.close();

    DBMS::Statement s(dbc);
    s.prepare(L"SELECT * FROM foo;");
    s.execute();


    DBMS::Resultset rs;
    rs.attach(s);
   
    for(rs.first(); !rs.eof(); rs.next())
    {
        std::cout << "row ";
        std::cout << rs.column(1).asNarrowStr("UTF-8") << std::endl;
        //std::cout << conv_to(stmt.resultset().column(2).asStr(), "UTF-8") << std::endl;
        if(! rs.column(2).isnull())
        {
            DBMS::Blob bl(rs.column(2).asBlob());
            //DBMS::Memo me(rs.column(2).asMemo());
            //std::cout << bl.rdbuf() << std::endl;
        }
        std::cout << std::endl << std::flush;
    }

    return 0;
}



int main(void)
{
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

