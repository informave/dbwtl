#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>

#include "cxxc.hh"
#include "fixtures/sqlite_memory.hh"


using namespace informave::db::dal;
using namespace informave::db::ex;




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
    
    CXXC_CHECK_THROW( read_only, v2.set<int>(40) );
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
