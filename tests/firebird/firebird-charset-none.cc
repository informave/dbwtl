#include "../cxxc.hh"
#include "fixture_firebird.hh"



CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, ReadBinary)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.execDirect("SELECT CAST('foobar' as VARCHAR(10) CHARACTER SET OCTETS) as c1, "
			"CAST(_ISO8859_1 'fäö' as VARCHAR(10) CHARACTER SET NONE) as c2 FROM RDB$DATABASE");
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.column(1).datatype() == DAL_TYPE_VARBINARY);
	CXXC_CHECK( rs.column(2).datatype() == DAL_TYPE_STRING);
	CXXC_ECHO( rs.column(2).get<String>() );
	CXXC_CHECK( rs.column(2).get<String>() == String("fäö") );
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

