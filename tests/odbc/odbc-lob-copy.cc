
#include <fstream>

#include <dbwtl/dal/engines/generic>
#include "../cxxc.hh"
#include "fixture_odbc.hh"



CXXC_FIXTURE_TEST(OdbcPgFixture, BlobCopy)
{
	DBMS::Statement stmt(dbc);
	stmt.prepare("SELECT data FROM blobstream_test");
	//stmt.prepare("SELECT data from bigdata");
	stmt.execute();
	DBMS::Resultset rs;
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.describeColumn(1).getDatatype() == DAL_TYPE_BLOB );
	CXXC_ECHO( rs.column(1).get<String>() + "foo" );
	CXXC_ECHO( rs.column(1).get<String>() + "foo" );
	//CXXC_ECHO( rs.column(1).get<String>() );
	//CXXC_CHECK( rs.column(1).get<String>() == "7382936F728A37BCD90D" );
	ShrRecord x(1);
	x[0] = Variant();
	x[0] = (const Variant&)rs.column(1); /// test
	CXXC_CHECK( x[0].get<String>() == String("738200936F728A37BCD90D") );
	//CXXC_ECHO( Variant((const Variant&)rs.column(1)).get<String>() );
	//CXXC_ECHO( Variant((const Variant&)rs.column(1)).get<String>() );
	CXXC_ECHO( x[0].get<String>() );
	RecordSet cr;
	cr.setColumnCount(1);
	cr.open();
	cr.insert(x);
	cr.insert(x);
	//CXXC_CHECK( x[0].get<String>() == String("738200936F728A37BCD90D") );
	cr.first();
	cr.setpos(2);
	cr.setpos(1);
	if(cr.column(1).isnull())
		return;

	CXXC_CHECK( cr.column(1).get<String>() == String("738200936F728A37BCD90D") );

    x[0].get<BlobStream>();
    x[0].get<Blob>();

    CXXC_ECHO( x[0].get<String>() );
    CXXC_CHECK( x[0].get<String>() == String("738200936F728A37BCD90D") );
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

