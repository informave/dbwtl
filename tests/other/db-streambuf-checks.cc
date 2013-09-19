#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>

#include <fstream>
#include <sstream>
#include <iostream>

using namespace informave::db;
typedef Database<sqlite> DBMS;

int test(void)
{
    DBMS::Environment env(L"sqlite:libsqlite");
    DBMS::Connection dbc(env);
    dbc.connect("sampledb.sqlitedb");

    try
    {
        dbc.directCmd(L"DROP TABLE streambuf_checks");
    }
    catch(...)
    {}
    
    dbc.directCmd(
        " CREATE TABLE streambuf_checks("
        "  id       INTEGER PRIMARY KEY"
        " ,data     BLOB"
        " )");


    {
        std::ofstream tf("streambuf_checks_dummy.txt", std::ios::binary);
        tf << "Hello World";
    }

    dbc.beginTrans(trx_read_committed);

    DBMS::Statement stmt(dbc);
    stmt.prepare("INSERT INTO streambuf_checks(data) VALUES(?)");

    stmt.execute();


    std::ifstream in("streambuf_checks_dummy.txt", std::ios::binary);
    stmt.bind(1, in.rdbuf());
    stmt.execute();
    in.close();

    stmt.bind(1, DBMS::Variant());
    stmt.execute();

    stmt.close();

    dbc.commit();


    stmt.prepare("SELECT * from streambuf_checks WHERE data IS NOT NULL");
    stmt.execute();
    DBMS::Resultset rs(stmt);
    rs.first();

    assert(!rs.eof());

    const DBMS::Value &data = rs.column("data");    



    DBMS::BlobStream blob(data);


    //blob.exceptions ( DBMS::Blob::eofbit | DBMS::Blob::failbit | DBMS::Blob::badbit );
    std::stringstream ss;
    ss << "Data: " << data.asBlob().rdbuf();



    assert(ss.str() == std::string("Data: Hello World"));


    data.asBlob().rdbuf()->pubseekoff(-7, std::ios_base::cur);
    std::cout << data.asBlob().rdbuf() << std::endl;
    std::cout << data.asBlob().rdbuf() << std::endl;
    std::cout << data.asBlob().rdbuf() << std::endl;


    std::string tmp;
    blob >> tmp;
    assert(tmp.empty()); // this must now be empty

    assert(! data.isnull());
    rs.next();
    assert(rs.eof() == true); // query must return exactly one row
    
    assert(data.isnull()); // no row -> no value
    

    stmt.close();

    return 0;
}



int main(void)
{
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    std::cerr.imbue(std::locale());
    std::wcout.imbue(std::locale());
    std::wcerr.imbue(std::locale());

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

