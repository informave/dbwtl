#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>

#include "../cxxc.hh"


using namespace informave::db;


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


CXXC_TEST(NullVariantCopy)
{
	Variant v;
	CXXC_CHECK( v.isnull() );

	Variant y(v);
	CXXC_CHECK( y.isnull() );

	Variant z;
	z = v;
	CXXC_CHECK( z.isnull() );
}


CXXC_TEST(CtorAndAssign)
{
	int x = 5;

	Variant v1(&x);

	Variant v2(v1);
	x = 7;
	CXXC_CHECK( v2.get<int>() == 5 );

	Variant v3;
	v3.operator=(v1);
	x = 10;
	CXXC_CHECK( v3.get<int>() == 7 );
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

    v.set<bool>(true);
    CXXC_CHECK( v.datatype() == DAL_TYPE_BOOL );

    v.set<float>(1.2345);
    CXXC_CHECK( v.datatype() == DAL_TYPE_FLOAT );

    v.set<double>(1.2345);
    CXXC_CHECK( v.datatype() == DAL_TYPE_DOUBLE );

    v.set<signed int>(10);
    CXXC_CHECK( v.datatype() == DAL_TYPE_INT );

    v.set<unsigned int>(10);
    CXXC_CHECK( v.datatype() == DAL_TYPE_UINT );

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

    v.set<unsigned long long>(10);
    CXXC_CHECK( v.datatype() == DAL_TYPE_UBIGINT );

    v.set<String>("2");
    CXXC_CHECK( v.datatype() == DAL_TYPE_STRING );

    v.get<int>();

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



CXXC_TEST(BlobDeepCopy)
{
	std::stringstream ss;
	ss << "test";
	Variant v(Blob(ss.rdbuf()));

	std::stringstream aa;
	
	Blob x(v.get<Blob>());

	aa << x.rdbuf();

	CXXC_CHECK( ss.str() == "test" );
	CXXC_CHECK( ss.str() == aa.str() );

}


CXXC_TEST(MemoDeepCopy)
{
        std::wstringstream ss;
        ss << L"test";
        Variant v(Memo(ss.rdbuf()));

        std::wstringstream aa;

        Memo x(v.get<Memo>());

        aa << x.rdbuf();

        CXXC_CHECK( ss.str() == L"test" );
        CXXC_CHECK( ss.str() == aa.str() );
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
