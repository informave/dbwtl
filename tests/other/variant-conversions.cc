#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>
#include <dbwtl/exceptions.hh>

#include <iostream>
#include <memory>
#include <cstdlib>

#include "../cxxc.hh"


using namespace informave::db;
using namespace informave::db::ex;


CXXC_TEST(SignedInt)
{
	Variant v((signed int)(34));
	CXXC_CHECK( v.datatype() == DAL_TYPE_INT );
	CXXC_CHECK( v.get<signed int>() == 34 );
	CXXC_CHECK( v.get<unsigned int>() == 34 );
	CXXC_CHECK( v.get<signed short>() == 34 );
	CXXC_CHECK( v.get<unsigned short>() == 34 );
	CXXC_CHECK( v.get<signed long long>() == 34 );
	CXXC_CHECK( v.get<unsigned long long>() == 34 );
	CXXC_CHECK( v.get<signed char>() == 34 );
	CXXC_CHECK( v.get<unsigned char>() == 34 );
	CXXC_CHECK( v.get<float>() == 34 );
	CXXC_CHECK( v.get<double>() == 34 );
}

CXXC_TEST(UnsignedInt)
{
        Variant v((unsigned int)(34));
        CXXC_CHECK( v.datatype() == DAL_TYPE_UINT );
        CXXC_CHECK( v.get<signed int>() == 34 );
        CXXC_CHECK( v.get<unsigned int>() == 34 );
        CXXC_CHECK( v.get<signed short>() == 34 );
        CXXC_CHECK( v.get<unsigned short>() == 34 );
        CXXC_CHECK( v.get<signed long long>() == 34 );
        CXXC_CHECK( v.get<unsigned long long>() == 34 );
        CXXC_CHECK( v.get<signed char>() == 34 );
        CXXC_CHECK( v.get<unsigned char>() == 34 );
        CXXC_CHECK( v.get<float>() == 34 );
        CXXC_CHECK( v.get<double>() == 34 );

	//CXXC_CHECK( (unsigned int)(34) > std::numeric_limits<signed short>::min() );
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
