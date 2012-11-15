#include "../cxxc.hh"
#include "fixture_firebird.hh"

using namespace informave::db::ex;


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, RollbackTransaction)
{
	dbc.directCmd("DELETE FROM alltypes");
	dbc.directCmd("INSERT INTO alltypes(t_smallint) VALUES(5)");
	Transaction trx = dbc.makeTrx(trx_read_committed, trx_default);
	DBMS::Statement stmt(dbc);
	try
	{
	stmt.execDirect("UPDATE alltypes SET t_smallint = 7", trx);
	stmt.close();
	stmt.execDirect("SELECT t_smallint FROM alltypes", trx);
	DBMS::Resultset rs(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.column(1).get<short>() == 7 );
	stmt.close();
	trx.rollback();
	stmt.execDirect("SELECT t_smallint FROM alltypes WHERE t_smallint = 5");
	DBMS::Resultset r2(stmt);
	r2.first();
	CXXC_CHECK( !r2.eof() );
	CXXC_CHECK( r2.column(1).get<short>() == 5 );
	}
	catch(std::runtime_error &e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(...)
	{
		std::cerr << "unknown" << std::endl;
	}
	stmt.close();
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, RollbackTransaction2)
{
	Transaction trx = dbc.makeTrx(trx_read_committed, trx_default);
	DBMS::Statement stmt(dbc);
	stmt.execDirect("SELECT * FROM RDB$DATABASE", trx);
	stmt.close();
	trx.commit();
	stmt.execDirect("ROLLBACK;");
	/*
	std::cerr << "foo" <<std::endl;
	stmt.close();
	stmt.execDirect("SET TRANSACTION;", trx);
	trx.rollback();
	*/
}




CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, RollbackTransaction4)
{
	DBMS::Statement stmt1(dbc);
	DBMS::Resultset rs;
	stmt1.execDirect("SELECT CURRENT_TRANSACTION FROM RDB$DATABASE");
	rs.attach(stmt1);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	int tr1 = rs.column(1).get<int>();
	stmt1.close();
	stmt1.execDirect("SELECT CURRENT_TRANSACTION FROM RDB$DATABASE");
	rs.attach(stmt1);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( tr1 != rs.column(1).get<int>() );
}



CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, RollbackTransaction7)
{
	DBMS::Statement stmt1(dbc);
	DBMS::Resultset rs;

	dbc.beginTrans(trx_read_committed, trx_default);
	stmt1.execDirect("SELECT CURRENT_TRANSACTION FROM RDB$DATABASE");
	rs.attach(stmt1);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	int tr1 = rs.column(1).get<int>();
	stmt1.close();

	stmt1.execDirect("SELECT CURRENT_TRANSACTION FROM RDB$DATABASE");
	rs.attach(stmt1);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( tr1 == rs.column(1).get<int>() );
	stmt1.close();

	stmt1.execDirect("COMMIT");
	stmt1.close();

	stmt1.execDirect("SET TRANSACTION");
	stmt1.close();

	stmt1.execDirect("SELECT CURRENT_TRANSACTION FROM RDB$DATABASE");
	rs.attach(stmt1);
	rs.first();
	tr1 = rs.column(1).get<int>();
	stmt1.close();
	
	stmt1.execDirect("SELECT CURRENT_TRANSACTION FROM RDB$DATABASE");
	rs.attach(stmt1);
	rs.first();
	CXXC_CHECK( tr1 == rs.column(1).get<int>() );
	stmt1.close();

	stmt1.execDirect("COMMIT RETAIN");
	stmt1.close();

	dbc.commit();
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, CloseAfterException)
{
	DBMS::Statement stmt(dbc);
	try
	{
		stmt.execDirect("SELECT unknown_columnname FROM RDB$DATABASE");
	}
	catch(DBMS::SQLSTATE_HY000 &e)
	{
	}
	stmt.close();
}

CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, SavepointTest)
{
	DBMS::Statement stmt(dbc);
	dbc.beginTrans(trx_read_committed);
	dbc.savepoint("foo");
	dbc.rollback("foo");
	dbc.directCmd("SAVEPOINT abc");
	dbc.directCmd("ROLLBACK TO abc");
	stmt.execDirect("ROLLBACK TO foo");
	dbc.commit();
	CXXC_CHECK_THROW( DBMS::SQLSTATE_25000, dbc.savepoint("bar") );
	dbc.directCmd("SAVEPOINT bar");
	CXXC_CHECK_THROW( DBMS::SQLSTATE_HY000, dbc.directCmd("ROLLBACK TO bar") );
}




CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, RollbackTransaction3)
{
        Transaction trx = dbc.makeTrx(trx_read_committed, trx_default);
        DBMS::Statement stmt(dbc);
        stmt.execDirect("SELECT * FROM RDB$DATABASE", trx);
        stmt.close();
        //stmt.execDirect("ROLLBACK;", trx);
        //stmt.close();
	//trx.rollback();
	//trx.rollback();
	try
	{
        //stmt.execDirect("SET TRANSACTION", trx);
	stmt.execDirect("COMMIT", trx);
	stmt.close();
	stmt.execDirect("SET TRANSACTION", trx);
	stmt.close();
	stmt.execDirect("UPDATE alltypes SET t_smallint = 6", trx);
	//stmt.execDirect("SET TRANSACTION", trx);
	stmt.close();
	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch(int&)
	{
		std::cerr << "UNKNOWNXX" << std::endl;
	}
	catch(...)
	{
		std::cerr << "UNKNOWN" << std::endl;
	}
	trx.commit();
        //stmt.execDirect("UPDATE alltypes SET t_smallint = 3", trx);
	//stmt.close();
	//trx.commit();
	std::cerr << "DONE" << std::endl;
        /*
        std::cerr << "foo" <<std::endl;
        stmt.close();
        stmt.execDirect("SET TRANSACTION;", trx);
        trx.rollback();
        */
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
	//rs.first();
	CXXC_CHECK_THROW( DBMS::SQLSTATE_24000, rs.eof() );
}

CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, DoubleResultset)
{
	dbc.directCmd("SET TRANSACTION");
/*
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;

	stmt.execDirect("SELECT 1 AS f1");
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( rs.column("f1").get<int>() == 1 ); ////// write get<char> test!

	stmt.close(); // test, should work without...
	stmt.execDirect("SELECT 2 AS f2");
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( rs.column("f2").get<int>() == 2 );
*/
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

