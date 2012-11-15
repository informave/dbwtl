#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>

#include "../cxxc.hh"
#include "fixtures/sqlite_memory.hh"


using namespace informave::db::ex;



CXXC_FIXTURE_TEST(SqliteMemoryFixture, DoubleResultset)
{
	DBMS::Statement stmt(dbc);
	DBMS::Resultset rs;

	stmt.execDirect("SELECT 1 AS f1");
	rs.attach(stmt),
	rs.first();
	CXXC_CHECK( rs.column("f1").get<int>() == 1 ); ////// write get<char> test!

	stmt.close(); // test, should work without...
	stmt.execDirect("SELECT 2 AS f2");
	rs.attach(stmt),
	rs.first();
	CXXC_CHECK( rs.column("f2").get<int>() == 2 );
}


CXXC_FIXTURE_TEST(SqliteMemoryFixture, SqliteVariantConversion)
{
	DBMS::Statement stmt(dbc);
	stmt.execDirect("SELECT date('now'), datetime('now'), NULL");
	DBMS::Resultset rs;
	rs.attach(stmt);
	rs.first();
	TDate x = rs.column(1).get<TDate>();

	std::cout << "VAL: " << std::string(x.str()) << std::endl;
	std::cout << "VAL: " << std::string(TTime(13,37, 15).str()) << std::endl;
	std::cout << "VAL: " << std::string(TTimestamp(2011, 12, 31, 10, 35, 23, 888).str()) << std::endl;

	CXXC_CHECK( ! rs.column(1).isnull() );
	CXXC_CHECK( rs.column(1).get<TDate>() == TDate("now") );
	CXXC_CHECK( rs.column(2).get<TDate>() == TDate("now") );
	CXXC_CHECK_THROW( NullException, rs.column(3).get<TDate>() );

	stmt.close();
	stmt.execDirect("SELECT time('now'), datetime('now'), NULL");
	rs.attach(stmt);
	rs.first();
	TTime y = rs.column(1).get<TTime>();
	CXXC_CHECK_THROW( NullException, rs.column(3).get<TTime>() );
}


CXXC_FIXTURE_TEST(SqliteMemoryFixture, SqliteCompare)
{
        DBMS::Statement stmt(dbc);
        stmt.prepare("SELECT date(?), date(?) || 'T' || time(?), date(?), NULL");
	stmt.bind(1, TDate(2011, 4, 5));
	stmt.bind(2, TDate(2011, 4, 6));
	stmt.bind(3, TTime("12:45:01"));
	stmt.bind(4, TTimestamp(2011, 12, 31, 10, 35, 23, 888));
	stmt.execute();
        DBMS::Resultset rs;
        rs.attach(stmt);
        rs.first();
	CXXC_CHECK( rs.column(1).get<TDate>() == TDate(2011, 4, 5) );
	CXXC_CHECK( rs.column(2).get<TTime>() == TTime(12, 45, 1) );
	CXXC_CHECK( rs.column(3).get<TDate>() == TDate(2011, 12, 31) );
}



CXXC_FIXTURE_TEST(SqliteMemoryFixture, SqliteVariantTest)
{
    DBMS::Statement stmt(dbc);
    stmt.execDirect("SELECT 1, 2, 'foobar' UNION ALL SELECT 4, 5, NULL");
    DBMS::Resultset rs;
    rs.attach(stmt);
    rs.first();
   
    CXXC_CHECK( rs.column(1).get<int>() == 1 );
    CXXC_CHECK( rs.column(2).get<int>() == 2 );
    CXXC_CHECK( rs.column(3).get<String>() == "foobar" );
    CXXC_CHECK( rs.column(1).get<String>() == "1" );

    Variant c1copy(rs.column(1).clone());
    CXXC_CHECK( c1copy.get<int>() == 1 );
    //c1copy.set<int>(10);

    Variant c1;
    c1 = rs.column(1);
    CXXC_CHECK( c1.get<int>() == 1 );

    Variant c2;
    c2 = rs.column(2);

    rs.next();
    CXXC_CHECK( c1copy.get<int>() == 4 );
    CXXC_CHECK( c1.get<int>() == 1 );
    CXXC_CHECK( c2.get<int>() == 2 );

    CXXC_CHECK( rs.column(1).get<int>() == 4 );
    CXXC_CHECK( rs.column(3).isnull() );
}




CXXC_FIXTURE_TEST(SqliteMemoryFixture, SqliteNumeric)
{
    DBMS::Statement stmt(dbc);
    int v1 = 5;
    TNumeric v2(7);
    stmt.prepare("SELECT ?, CAST(? AS REAL) + 1.534001 - 5, NULL");
	stmt.bind(1, &v1);
    stmt.bind(2, &v2);
    v2 += TNumeric("0.1");
	stmt.execute();
    DBMS::Resultset rs;
    rs.attach(stmt);
    rs.first();
	CXXC_CHECK( rs.column(1).get<TNumeric>() == TNumeric(5) );
	CXXC_CHECK( rs.column(2).get<TNumeric>() == (TNumeric("8.634001") - v1) );
}



CXXC_TEST(SimpleNewVariant)
{
    Variant v;
    CXXC_CHECK( v.isnull() );

    v = 5;
    CXXC_CHECK( ! v.isnull() );
    CXXC_CHECK( v.get<int>() == 5 );

    v.setNull();
    CXXC_CHECK( v.isnull() );
}


CXXC_TEST(SimpleAssign)
{
    Variant v;
    
    v.set<signed int>(10);
    CXXC_CHECK( ! v.isnull() );
    CXXC_CHECK( v.get<signed int>() == 10 );

    v.set<signed int>(30);
    CXXC_CHECK( ! v.isnull() );
    CXXC_CHECK( v.get<signed int>() == 30 );
}

CXXC_TEST(TypeCheck)
{
    Variant v;

    v.set<signed int>(10);
    CXXC_CHECK( v.datatype() == DAL_TYPE_INT );

    v.set<unsigned int>(10);
    CXXC_CHECK( v.datatype() == DAL_TYPE_UINT );
/*
  v.set<signed short>(10);
  CXXC_CHECK( v.datatype() == DAL_TYPE_SMALLINT );

  v.set<unsigned short>(10);
  CXXC_CHECK( v.datatype() == DAL_TYPE_USMALLINT );

  v.set<signed char>(10);
  CXXC_CHECK( v.datatype() == DAL_TYPE_CHAR );

  v.set<unsigned char>(10);
  CXXC_CHECK( v.datatype() == DAL_TYPE_UCHAR );

  v.set<signed long long>(10);
  CXXC_CHECK( v.datatype() == DAL_TYPE_BIGINT );

  v.set<unsigned long>(10);
  CXXC_CHECK( v.datatype() == DAL_TYPE_UBIGINT );
*/
}



CXXC_TEST(TypeConversionUChar)
{
    Variant v;
    
    v.set<signed int>(10);
    CXXC_CHECK( ! v.isnull() );
    CXXC_CHECK( v.get<unsigned char>() == 10 );
}



CXXC_TEST(PointerStorage)
{
    int x = 10;
    int *ptr = &x;
    const int *const_ptr = &x;

    Variant v1(ptr);
    Variant v2(const_ptr);
    Variant v3(x);

    x = 20;
    CXXC_CHECK( v1.get<int>() == 20 );
    CXXC_CHECK( v2.get<int>() == 20 );
    CXXC_CHECK( v3.get<int>() == 10 );

    v1.set<int>(30);
    CXXC_CHECK( x == 30 );
    CXXC_CHECK( v1.get<int>() == 30 );
    CXXC_CHECK( v3.get<int>() == 10 );
    
    CXXC_CHECK_THROW( ReadonlyException, v2.set<int>(40) );
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
