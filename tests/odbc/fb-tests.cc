
#include <fstream>

#include <dbwtl/dal/engines/generic>
#include "../cxxc.hh"
#include "fixture_fbodbc.hh"

using namespace informave::db::ex;


CXXC_TEST(CreateDbc)
{
	DBMS::Environment env("odbc:libodbc");
	DBMS::Connection db(env);
	IDbc::Options opts;
	opts["datasource"] = "myfbsql";
	opts["username"] = "SYSDBA";
	opts["password"] = "masterkey";
	opts["charset"] = "utf8";
	opts["unicode"] = "no";
	db.connect(opts);
	CXXC_CHECK( db.isConnected() );
	db.disconnect();
}


CXXC_FIXTURE_TEST(OdbcTestbaseFixture, Transaction1)
{
        DBMS::Statement stmt(dbc);
        dbc.beginTrans(trx_read_committed);
	dbc.rollback();
	dbc.rollback();
	dbc.beginTrans(trx_read_committed);
	dbc.beginTrans(trx_read_committed);

}


CXXC_FIXTURE_TEST(OdbcTestbaseFixture, Meta)
{
	DBMS::Metadata md(dbc);

	RecordSet rs = md.getTables(Variant(), Variant(), Variant(), IMetadata::META_OBJECT_CLASS_ALL, TableFilter("RDB$DATABASE"));
	rs.first();
	while( !rs.eof() )
	{
		for(colnum_t i = 1; i <= rs.columnCount(); ++i)
		{
			std::cout << ifnull<String>(rs.column(i), "<null>") << "\t|\t";
		}
		rs.next();
		std::cout << std::endl;
	}
}


CXXC_FIXTURE_TEST(OdbcTestbaseFixture, Connect)
{
	DBMS::Statement stmt(dbc);
	//stmt.prepare("SELECT 1+1 UNION ALL SELECT 3 UNION ALL SELECT 5");
	stmt.prepare("SELECT CAST('foobar' as varchar(30)) FROM RDB$DATABASE UNION ALL SELECT 'bla' FROM RDB$DATABASE;");
	stmt.execute();

        DBMS::Resultset rs;
        rs.attach(stmt);
        rs.first();
        CXXC_CHECK( !rs.eof() );

	while( !rs.eof() )
	{
		CXXC_ECHO( rs.column(1).get<String>() );
		rs.next();
	}

}

/*
CXXC_FIXTURE_TEST(OdbcTestbaseFixture, ParamCount)
{
	DBMS::Statement stmt(dbc);
	stmt.prepare("SELECT 'foo' || ? || 'bar' FROM RDB$DATABASE");
	//stmt.prepare("SELECT ?, ?+1");
	CXXC_CHECK(stmt.paramCount() == 2);
	String a("foo");
	String b("bar");
	//stmt.bind(1, Variant());
	//stmt.bind(1, int(5));
	stmt.bind(1, String("hallo"));
	stmt.execute();

	DBMS::Resultset rs;
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	//CXXC_CHECK( rs.column(1).isnull() );
	//CXXC_CHECK( rs.column(1).get<int>() == 6 );
	CXXC_ECHO( rs.column(1).get<String>() );
	CXXC_CHECK( rs.column(1).get<String>() == String("foohallobar") );

	CXXC_ECHO( dbc.dbmsName() );
}
*/

CXXC_FIXTURE_TEST(OdbcTestbaseFixture, Numeric)
{
	DBMS::Statement stmt(dbc);
	//stmt.prepare("SELECT CAST(-34.1234 as NUMERIC(8,6)) FROM RDB$DATABASE;");
	stmt.prepare("SELECT CAST(-257.6 as NUMERIC(8,1)) FROM RDB$DATABASE;");
	stmt.execute();
	DBMS::Resultset rs;
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.describeColumn(1).getDatatype() == DAL_TYPE_NUMERIC );
	CXXC_ECHO( rs.column(1).get<TNumeric>() );
	//CXXC_CHECK( rs.column(1).get<TNumeric>() == TNumeric("-34.1234") );
}

/*
CXXC_TEST(Exceptions)
{
        DBMS::Environment env("odbc:libodbc");
        DBMS::Connection db(env);
        CXXC_CHECK_THROW( DBMS::SQLSTATE_28000, db.connect("mypgsql", "postgres", "tesdf") );
}
*/

/*
CXXC_FIXTURE_TEST(OdbcTestbaseFixture, BlobReader)
{
	DBMS::Statement stmt(dbc);
	//stmt.prepare("SELECT 'fooo'::text, CAST('OTTO' as bytea)");
	stmt.prepare("SELECT * FROM bigdata WHERE id = 1");
	stmt.execute();
	DBMS::Resultset rs;
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.describeColumn(3).getDatatype() == DAL_TYPE_MEMO );
	//Blob blob = 
	std::ofstream of("/tmp/out.bin", std::ios::out | std::ios::binary);
	BlobStream stream = rs.column(2).get<BlobStream>();
	of << stream.rdbuf();
}
*/

CXXC_FIXTURE_TEST(OdbcTestbaseFixture, MemoReader)
{
	DBMS::Statement stmt(dbc);
	stmt.prepare("SELECT cast('foobarbaz' as blob sub_type text) FROM RDB$DATABASE");
	stmt.execute();
	DBMS::Resultset rs;
	rs.attach(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_CHECK( rs.describeColumn(1).getDatatype() == DAL_TYPE_MEMO );
	//CXXC_ECHO( rs.column(1).get<String>() + "foo" );
	CXXC_CHECK( rs.column(1).get<String>() == "foobarbaz" );
	//CXXC_CHECK( rs.column(1).get<String>() == "foobarbaz" ); /// @bug blob stability
}



CXXC_FIXTURE_TEST(OdbcTestbaseFixture, BlobWriter)
{
	DBMS::Statement stmt(dbc);
	stmt.prepare("INSERT INTO bigdata(data, data2) VALUES(?, ?)");
	CXXC_CHECK( stmt.paramCount() == 2 );
	std::ifstream in1("/etc/fstab", std::ios::in | std::ios::binary);
	std::ifstream in2("/etc/mtab", std::ios::in | std::ios::binary);

	stmt.bind(1, in1.rdbuf());
	stmt.bind(2, Variant());
	//stmt.bind(2, in2.rdbuf());
	stmt.execute();
}



CXXC_FIXTURE_TEST(OdbcTestbaseFixture, MemoWriter)
{
        DBMS::Statement stmt(dbc);
	DBMS::Statement query(dbc);
        stmt.prepare("INSERT INTO bigdata(mem) VALUES(?) RETURNING id;");
        CXXC_CHECK( stmt.paramCount() == 2 );
        std::wifstream in1("/etc/fstab", std::ios::in);

        //stmt.bind(1, in1.rdbuf());
        stmt.bind(1, in1.rdbuf());
        stmt.execute();

	DBMS::Resultset rs(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );

	int id = rs.column(1).get<int>();
	stmt.close();

	query.prepare("SELECT mem FROM bigdata WHERE id = ?");
	//query.bind(1, rs.column(1));
	query.bind(1, id);
	query.execute();
	//rs.close(); /// @bug valgrind errors if I close it here, may conflict with reset()
	rs.attach(query);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	std::wstring s = rs.column(1).get<String>();
	in1.seekg (0, in1.beg);
	std::wstring orig((std::istreambuf_iterator<wchar_t>(in1)), std::istreambuf_iterator<wchar_t>());
	CXXC_CHECK( s == orig );
}




CXXC_FIXTURE_TEST(OdbcTestbaseFixture, MultiExecute)
{
        DBMS::Statement stmt(dbc);
	stmt.prepare("SELECT cast(? as integer) FROM RDB$DATABASE");
	stmt.bind(1, Variant(5));
	stmt.execute();
	stmt.bind(1, Variant(6));
	stmt.execute();

	stmt.close();
	stmt.prepare("SELECT 5 from RDB$DATABASE");
	stmt.execute();
	stmt.execute();

	stmt.close();
	stmt.prepare("SELECT cast(? as integer) FROM RDB$DATABASE");
	stmt.bind(1, Variant(5));

	DBMS::Statement st2(dbc);
	st2.prepare("SELECT 5 FROM RDB$DATABASE");
	st2.execute();
}

/*
CXXC_FIXTURE_TEST(OdbcTestbaseFixture, AllTypes)
{
        DBMS::Statement stmt(dbc);
	stmt.prepare("SELECT 'Foobar'::varchar(10), CURRENT_DATE, CURRENT_TIME, CURRENT_TIMESTAMP, -9223372036854775807::bigint, "
			"-0.0003873::float, false::bool, true::bool");
	stmt.execute();
	DBMS::Resultset rs(stmt);
	rs.first();
	CXXC_ECHO( rs.column(1).get<String>() );
	CXXC_ECHO( rs.column(2).get<TDate>() );
	CXXC_ECHO( rs.column(3).get<TTime>() );
	CXXC_ECHO( rs.column(4).get<TTimestamp>() );
	CXXC_ECHO( rs.column(5).get<signed long long>() );
	CXXC_ECHO( rs.column(6).get<float>() );
	CXXC_ECHO( rs.column(7).get<bool>() );
	CXXC_ECHO( rs.column(8).get<bool>() );
	for(int i = 1; i <= rs.columnCount(); ++i)
	{
		CXXC_ECHO( daltype2string(rs.describeColumn(i).getDatatype()) );
		CXXC_ECHO( rs.column(i).get<String>() );
	}


}

CXXC_FIXTURE_TEST(OdbcTestbaseFixture, Interval)
{
	DBMS::Statement stmt(dbc);
	stmt.prepare("SELECT cast('1 day 12 hours' as interval minute)");
	stmt.execute();
	DBMS::Resultset rs(stmt);
	rs.first();
	CXXC_ECHO( rs.column(1).get<String>() );
}
*/


CXXC_FIXTURE_TEST(OdbcTestbaseFixture, BindInteger)
{
	DBMS::Statement stmt(dbc);
	stmt.prepare("INSERT INTO ALLTYPES(id, T_SMALLINT, T_INTEGER, T_BIGINT, T_FLOAT) VALUES(NULL, ?, ?, NULL, NULL);");
	CXXC_CHECK( stmt.paramCount() == 2 );
	stmt.bind(1, (signed short)(-92));
	stmt.bind(2, (signed int)(-34));
	//stmt.bind(3, (signed long long)(489232));
	//stmt.bind(3, (float)(0.0007855));
	stmt.execute();
}


CXXC_FIXTURE_TEST(OdbcTestbaseFixture, AllTypesBinding)
{
	DBMS::Statement stmt(dbc);
	stmt.prepare("INSERT INTO ALLTYPES VALUES(NULL, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
	/*
		"RETURNING T_SMALLINT, T_INTEGER, T_BIGINT, T_FLOAT, T_DOUBLE, T_NUMERIC, T_DECIMAL, T_DATE, T_TIME, T_TIMESTAMP,"
		" T_CHAR, T_VARCHAR, T_BLOB, T_TEXT");
		*/
	stmt.bind(1, (signed short)(-92));
	stmt.bind(2, (signed int)(-34));
	stmt.bind(3, (signed long long)(-8390126317123926256));
	stmt.bind(4, (float)(0.0007855));
	stmt.bind(5, (double)(12.00234987125));
	stmt.bind(6, TNumeric("-826.0028715"));
	stmt.bind(7, TNumeric("-826.0028715"));
	stmt.bind(8, TDate(2013, 3, 31));
	stmt.bind(9, TTime(18, 59, 34));
	stmt.bind(10, TTimestamp(2013, 3, 30, 17, 58, 33, 5000000));
	stmt.bind(11, String("Hallo"));
	stmt.bind(12, String("Unicode test: ÖÄÜ…"));
	stmt.bind(13, TVarbinary({0x03, 0x00, 0xf3, 0x85}));
	stmt.bind(14, String("blobtest"));
	stmt.execute();
	return;
	DBMS::Resultset rs(stmt);
	rs.first();
	CXXC_CHECK( !rs.eof() );
	CXXC_ECHO( "Not output..." );
	CXXC_CHECK( rs.column(1).get<signed short>() == -92 );
	//CXXC_CHECK( rs.column(2).get<signed int>() == -34 );
	//return;
	CXXC_CHECK( rs.column(3).get<signed long long>() == -8390126317123926256 );
	return;
	//CXXC_CHECK( rs.column(7).get<signed char>() == -120 );
	//CXXC_CHECK( rs.column(8).get<unsigned char>() == 204 );
//	CXXC_CHECK( rs.column(9).get<bool>() == true );
//	CXXC_CHECK( rs.column(10).get<bool>() == false );
	CXXC_CHECK( rs.column(4).get<float>() == float(0.0007855) );
	CXXC_CMP_DOUBLE( rs.column(5).get<double>(), double(12.00234987125), double(0.0000001) );

//	CXXC_ECHO( rs.column(13).get<String>() );
	
//	CXXC_CHECK( rs.column(6).get<TNumeric>() == TNumeric("-826.0028715") );
	CXXC_CHECK( rs.column(11).get<String>() == String("Hallo") );
	CXXC_CHECK( rs.column(12).get<String>() == String("Unicode test: ÖÄÜ…") );
	CXXC_CHECK( rs.column(8).get<TDate>() == TDate(2013, 3, 31) );
	CXXC_CHECK( rs.column(9).get<TTime>() == TTime(18, 59, 34) );
	CXXC_CHECK( rs.column(10).get<TTimestamp>() == TTimestamp(2013, 3, 30, 17, 58, 33, 0) );
	
//	CXXC_ECHO( rs.column(18).get<TVarbinary>().str() );

	CXXC_CHECK( rs.column(13).get<TVarbinary>() == TVarbinary({0x03, 0x00, 0xf3, 0x85}) );

	// date, time, timestamp
}


/*
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

*/

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

