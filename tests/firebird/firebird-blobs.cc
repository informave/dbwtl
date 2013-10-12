
#include <dbwtl/dal/engines/generic>
#include "../cxxc.hh"
#include "fixture_firebird.hh"



CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, DatabaseVersion)
{
	std::cout << dbc.dbmsName() << std::endl;
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, WriteMemoStream)
{
	dbc.beginTrans(trx_read_committed);
        dbc.directCmd("DELETE FROM alltypes");
        DBMS::Statement stmt(dbc);
        stmt.prepare("INSERT INTO alltypes(t_text) VALUES(?)");
	std::wstringstream ss;
	ss << "Teststring";
        stmt.bind(1, ss.rdbuf());
        stmt.execute();
        stmt.close();
	dbc.commit();
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, WriteMemo)
{
        dbc.beginTrans(trx_read_committed);
        dbc.directCmd("DELETE FROM alltypes");
        DBMS::Statement stmt(dbc);
        stmt.prepare("INSERT INTO alltypes(t_text) VALUES(?)");
        std::wstringstream ss;
        ss << L"TeststringÖÄÜ";
	Memo m(ss.rdbuf());
        stmt.bind(1, m);
        stmt.execute();
        stmt.close();
	stmt.execDirect("SELECT CAST(t_text AS VARCHAR(30)) FROM alltypes");
	DBMS::Resultset rs;
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.column(1).isnull() );
	CXXC_ECHO(  rs.column(1).get<String>() );
	CXXC_CHECK( rs.column(1).get<String>() == "TeststringÖÄÜ" );
	stmt.close();
        dbc.commit();
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, ReadMemo)
{
        dbc.beginTrans(trx_read_committed, trx_readonly);
        DBMS::Statement stmt(dbc);
        stmt.execDirect("SELECT t_text FROM alltypes");
        DBMS::Resultset rs;
        rs.attach(stmt);
        rs.first();
        CXXC_CHECK( rs.column(1).get<String>() == "TeststringÖÄÜ" );
        stmt.close();
        dbc.commit();
}

CXXC_FIXTURE_TEST(FirebirdTestbaseIso88591Fixture, ReadMemoIso88591)
{
        dbc.beginTrans(trx_read_committed, trx_readonly);
        DBMS::Statement stmt(dbc);
        stmt.execDirect("SELECT t_text FROM alltypes");
        DBMS::Resultset rs;
        rs.attach(stmt);
        rs.first();
        CXXC_CHECK( rs.column(1).get<String>() == "TeststringÖÄÜ" );
        stmt.close();
        dbc.commit();
}



CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, WriteStringToMemo)
{
	dbc.beginTrans(trx_read_committed);
	dbc.directCmd("DELETE FROM alltypes");
	DBMS::Statement stmt(dbc);
	stmt.prepare("INSERT INTO alltypes(t_text) VALUES(?)");
	stmt.bind(1, String("Ghe ÖÄÜ"));
	stmt.execute();
	stmt.close();
	dbc.commit();
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

