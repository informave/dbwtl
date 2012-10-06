
#include <dbwtl/dal/engines/generic>
#include "../cxxc.hh"
#include "fixture_firebird.hh"

using namespace informave::db::ex;


CXXC_TEST(InvalidDb)
{
	DBMS::Environment env("firebird:libfbclient");
	DBMS::Connection db(env);
	CXXC_CHECK_THROW(DBMS::SQLSTATE_08000, db.connect("localhost:/var/lib/firebird/dbwtldbX.fdb", "SYSDBA", "masterkey"));
}

CXXC_TEST(InvalidServer)
{
        DBMS::Environment env("firebird:libfbclient");
        DBMS::Connection db(env);
        CXXC_CHECK_THROW(DBMS::SQLSTATE_08001, db.connect("informave.org:/var/lib/firebird/dbwtldb.fdb", "SYSDBA", "masterkey"));
}

CXXC_TEST(InvalidUser)
{
        DBMS::Environment env("firebird:libfbclient");
        DBMS::Connection db(env);
        CXXC_CHECK_THROW(DBMS::SQLSTATE_28000, db.connect("localhost:/var/lib/firebird/dbwtldb.fdb", "INVUSER", "masterkey"));

}

CXXC_TEST(InvalidPasswd)
{
        DBMS::Environment env("firebird:libfbclient");
        DBMS::Connection db(env);
        CXXC_CHECK_THROW(DBMS::SQLSTATE_28000, db.connect("localhost:/var/lib/firebird/dbwtldb.fdb", "SYSDBA", "INVPASS"));

}

CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, ConnectionInUse)
{
	CXXC_CHECK_THROW(DBMS::SQLSTATE_08002, dbc.connect("localhost:not_existing_database.fdb", "X", "Y"));
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, DatabaseVersion)
{
	std::cout << dbc.dbmsName() << std::endl;
}



CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, NumericTest)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.execDirect("SELECT CAST(5 AS NUMERIC(3,0)) FROM RDB$DATABASE");
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.column(1).datatype() == DAL_TYPE_NUMERIC );
	stmt.close();
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, OctetString)
{
        DBMS::Statement stmt(dbc);
        DBMS::Resultset rs;
        stmt.execDirect("SELECT CAST('abc' AS VARCHAR(5) CHARACTER SET OCTETS) FROM RDB$DATABASE");
        rs.attach(stmt);
        rs.first();
        CXXC_CHECK( !rs.eof() );
        CXXC_CHECK( rs.column(1).datatype() == DAL_TYPE_VARBINARY );
        stmt.close();
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, OctetStringReadWrite)
{
	dbc.directCmd("RECREATE TABLE octet_test(name VARCHAR(20), data VARCHAR(10) CHARACTER SET OCTETS)");
	const char data[] = { 'A', 'B', 'C' };
        DBMS::Statement stmt(dbc);
        DBMS::Resultset rs;
        stmt.prepare("INSERT INTO octet_test VALUES(?, ?)");
	stmt.bind(1, String("CDA"));
	stmt.bind(2, TVarbinary(data, sizeof(data)));
	stmt.execute();
	stmt.close();
	stmt.execDirect("SELECT * FROM octet_test WHERE data = 'ABC'");
        rs.attach(stmt);
        rs.first();
        CXXC_CHECK( !rs.eof() );
        CXXC_CHECK( rs.column(2).datatype() == DAL_TYPE_VARBINARY );
	CXXC_CHECK( rs.column(2).get<TVarbinary>().size() == 3 );
	CXXC_CHECK( rs.column(2).get<TVarbinary>()[3-1] == 'C' );
        stmt.close();
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, NullValueDeepcopy)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.execDirect("SELECT NULL FROM RDB$DATABASE");
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	Variant x(6);
	CXXC_CHECK(! x.isnull() );
	x.assign(rs.column(1)); // deepcopy
	CXXC_CHECK( x.isnull() );
	stmt.close();
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
	stmt.bind(1, 20);
	stmt.execute();
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( rs.eof() );
}

CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, BindAllTypesToNull)
{
    dbc.directCmd("DELETE FROM alltypes");
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.prepare(" INSERT INTO alltypes(t_smallint, t_integer, t_bigint, t_float,"
                 " t_double, t_numeric, t_decimal, t_date, t_time, t_timestamp,"
                 " t_char, t_varchar, t_blob, t_text)"
                 " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");

    CXXC_CHECK(stmt.paramCount() == 14);
    for(size_t i = 1; i <= stmt.paramCount(); ++i)
        stmt.bind(i, Variant());
    stmt.execute();
    stmt.close();

    stmt.execDirect("SELECT * FROM alltypes");
    rs.attach(stmt);
    rs.first();
    CXXC_CHECK( !rs.eof() );
    rs.first();
    for(size_t i = 2; i <= rs.columnCount(); ++i) // skip first ID column
    {
        CXXC_CHECK( rs.column(i).isnull() );
    }
    rs.next();
    CXXC_CHECK( rs.eof() );
}

CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, BindAllTypesToValues)
{
    dbc.directCmd("DELETE FROM alltypes");
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.prepare(" INSERT INTO alltypes(t_smallint, t_integer, t_bigint, t_float,"
                 " t_double, t_numeric, t_decimal, t_date, t_time, t_timestamp,"
                 " t_char, t_varchar, t_blob, t_text)"
                 " VALUES(?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");

    CXXC_CHECK(stmt.paramCount() == 14);

    std::vector<DBMS::Variant> values(14);
    values[0]  = short(-23);
    values[1]  = int(-2926713);
    values[2]  = -8346824617420560033LL;
    values[3]  = float(34.0018276);
    values[4]  = double(-0.0817278762781);
    values[5]  = TNumeric("0.123456789011");
    values[6]  = TNumeric("-0.123456789011");
    values[7]  = TDate("2012-08-31");
    values[8]  = TTime("23:49:22");
    values[9]  = TTimestamp("2012-12-24T23:00:03.005");
    values[10] = String("X");
    values[11] = String("John Doe");
    values[12] = Variant();
    values[13] = Variant();

    for(size_t i = 1; i <= stmt.paramCount(); ++i)
        stmt.bind(i, values[i-1]);
    stmt.execute();
    stmt.close();

    stmt.execDirect("SELECT * FROM alltypes");
    rs.attach(stmt);
    rs.first();
    CXXC_CHECK( !rs.eof() );

    CXXC_CHECK( rs.column("T_SMALLINT").get<short>() == -23 );
    CXXC_CHECK( rs.column("T_INTEGER").get<int>() == -2926713 );
    CXXC_CHECK( rs.column("T_BIGINT").get<long long>() == -8346824617420560033LL);
    CXXC_CHECK( rs.column("T_FLOAT").get<float>() == float(34.0018276));
    CXXC_CHECK( rs.column("T_DOUBLE").get<double>() == double(-0.0817278762781));
    CXXC_CHECK( rs.column("T_NUMERIC").get<TNumeric>() == TNumeric("0.123"));
    CXXC_CHECK( rs.column("T_DECIMAL").get<TNumeric>() == TNumeric("-0.123456789011"));
    CXXC_CHECK( rs.column("T_DATE").get<TDate>() == TDate("2012-08-31"));
    CXXC_CHECK( rs.column("T_TIME").get<TTime>() == TTime("23:49:22"));
    CXXC_CHECK( rs.column("T_TIMESTAMP").get<TTimestamp>() == TTimestamp("2012-12-24T23:00:03"));
    CXXC_CHECK( rs.column("T_CHAR").get<String>() == "X                   ");
    CXXC_CHECK( rs.column("T_VARCHAR").get<String>() == "John Doe");
    
    rs.next();
    CXXC_CHECK( rs.eof() );
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, ReturningClause)
{
    dbc.directCmd("DELETE FROM alltypes");
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.prepare(" INSERT INTO alltypes(t_smallint) VALUES(?) RETURNING id, t_smallint");

    CXXC_CHECK(stmt.paramCount() == 1);
    stmt.bind(1, 782);
    stmt.execute();

    rs.attach(stmt);
    rs.first();
    CXXC_CHECK( !rs.eof() );
    CXXC_CHECK( rs.column("T_SMALLINT").get<int>() == 782 );
    CXXC_EVAL( rs.column("ID").get<int>() );
}


CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, AffectedRows)
{
    dbc.directCmd("DELETE FROM alltypes");
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.prepare(" INSERT INTO alltypes(t_smallint) VALUES(?)");

    CXXC_CHECK(stmt.paramCount() == 1);
    stmt.bind(1, 782);
    stmt.execute();
    CXXC_CHECK( stmt.affectedRows() == 1 );
    stmt.bind(1, 123);
    stmt.execute();
    CXXC_CHECK( stmt.affectedRows() == 1 );
    stmt.close();

    stmt.prepare("UPDATE alltypes SET t_smallint = NULL");
    stmt.execute();

    CXXC_EVAL( stmt.affectedRows() );
    CXXC_CHECK( stmt.affectedRows() == 2 );
}



CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, InsertValueTooLarge)
{
    dbc.directCmd("DELETE FROM alltypes");
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
	stmt.prepare(" INSERT INTO alltypes(t_varchar) VALUES(?)");

    CXXC_CHECK(stmt.paramCount() == 1);
    stmt.bind(1, String("ThisStringIsTooLargeForTheColumn"));
    CXXC_CHECK_THROW( DBMS::SQLSTATE_22000, stmt.execute() );
    CXXC_CHECK_THROW( DBMS::SQLSTATE_feature_not_supported, stmt.lastInsertRowId() );
}

CXXC_FIXTURE_TEST(FirebirdTestbaseFixture, ExecuteBeforePrepare)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;
    //stmt.execute();
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

