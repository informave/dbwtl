#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/generic>
#include"../cxxc.hh"

#include "fixture_sqlite3.hh"


using namespace informave::db;

bool sortme(const ShrRecord& a, const ShrRecord& b)
{
	return a[1].get<int>() < b[1].get<int>();
}



CXXC_FIXTURE_TEST(SqliteMemoryFixture, CachedFoo)
{
    DBMS::Statement stmt(dbc);
    DBMS::Resultset rs;

    stmt.execDirect("CREATE TABLE test(id integer, name text)");
	stmt.close();
	stmt.prepare("INSERT INTO test(id, name) VALUES(?, ?)");
	stmt.bind(1, int(34));
	stmt.bind(2, String("Foo"));
	stmt.execute();
	stmt.bind(1, int(54));
	stmt.bind(2, String("Bar"));
	stmt.execute();
	stmt.bind(1, int(76));
	stmt.bind(2, String("Baz"));
	stmt.execute();
	stmt.close();
	
	stmt.execDirect("SELECT * FROM test");
    rs.attach(stmt);
    rs.first();
    CXXC_CHECK( rs.column("id").get<int>() == 34 ); ////// write get<char> test!
    
    stmt.close(); // test, should work without...
    
    
	//DBMS::CachedResult cr;
    DBMS::CachedResultset cr;
	stmt.execDirect("SELECT * FROM test");
	cr.attach(stmt);
	cr.open(); /// check if required!!!
	cr.first();
	CXXC_CHECK( cr.column("id").get<int>() == 34 );
	cr.next();
	CXXC_CHECK( cr.column("id").get<int>() == 54 );
	cr.prev();
	CXXC_CHECK( cr.column("id").get<int>() == 34 );
	cr.last();
	CXXC_CHECK( cr.column("id").get<int>() == 76 );
	cr.first();
	CXXC_CHECK( cr.column("id").get<int>() == 34 );
	stmt.close();

	//CachedResultBase xxx;


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

