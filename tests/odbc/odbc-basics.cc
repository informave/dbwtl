
#include <fstream>

#include <dbwtl/dal/engines/generic>
#include "../cxxc.hh"
#include "fixture_odbc.hh"



CXXC_FIXTURE_TEST(OdbcPgFixture, Transaction)
{
        DBMS::Statement stmt(dbc);
        dbc.beginTrans(trx_read_committed);
	dbc.rollback();
	dbc.rollback();
	dbc.beginTrans(trx_read_committed);
	dbc.beginTrans(trx_read_committed);

}


CXXC_FIXTURE_TEST(OdbcPgFixture, MetaTables)
{
	DBMS::Metadata md(dbc);

	RecordSet rs = md.getTables(Variant(), Variant(), Variant(), IMetadata::META_OBJECT_CLASS_ALL);
	rs.first();
	while( !rs.eof() )
	{
		for(colnum_t i = 1; i <= rs.columnCount(); ++i)
		{
			String s =  ifnull<String>(rs.column(i), "<null>");
		}
		rs.next();
		//std::cout << std::endl;
	}
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

