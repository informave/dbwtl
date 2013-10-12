#include "../cxxc.hh"
#include "fixture_firebird.hh"


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, DatabaseVersion)
{
	std::cout << dbc.dbmsName() << std::endl;
}

CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, ScalarValueSelect)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.execDirect("SELECT 5 FROM RDB$DATABASE");
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.column(1).get<int>() == 5 );
}

CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, BindScalarValueSelect)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.prepare("SELECT 10 FROM RDB$DATABASE WHERE 5 = ?");
	short a = 0;
	stmt.bind(1, &a);
	a = 5;
	stmt.execute();
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.column(1).get<int>() == 10 );
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, UnknownBind)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.prepare("SELECT coalesce(?, 10) FROM RDB$DATABASE");
	stmt.bind(1, 20);
	stmt.execute();
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.column(1).get<int>() == 20 );
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, StmtReuse)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.prepare("SELECT 1 FROM RDB$DATABASE WHERE 10 = ?");
	stmt.bind(1, 10);
	stmt.execute();
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.column(1).get<int>() == 1 );
	stmt.close();
	//stmt.clearBindings(); // @bug
	stmt.prepare("SELECT 1 FROM RDB$DATABASE WHERE 10 = ?");
	//stmt.bind(1, 20);
	stmt.execute();
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( rs.eof() );
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

