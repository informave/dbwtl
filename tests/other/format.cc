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


#define CXXC_CHECK_EQUAL(arg1, arg2) CXXC_CHECK(arg1 == arg2)


CXXC_TEST(SimpleNewVariant)
{
    CXXC_CHECK_EQUAL(std::string(format(" %% ")), " % ");

    CXXC_CHECK_EQUAL(std::string(format("%s") % "foo"), "foo");

    CXXC_CHECK_EQUAL(std::string(format("%d") % 5), "5");

    CXXC_CHECK_EQUAL(std::string(format("%04d") % 5), "0005");

    CXXC_CHECK_EQUAL(std::string(format("%+04d") % 5), "+005");

    CXXC_CHECK_EQUAL(std::string(format("%+04.2d") % 5), "+005");

    CXXC_CHECK_EQUAL(std::string(format("%+-04.2d") % 5), "+5  ");

    CXXC_CHECK_EQUAL(std::string(format("%+04.2f") % 2.34f), "+2.34");

    CXXC_CHECK_EQUAL(std::string(format("%+0*.2f") % 10 % 2.347f), "+000002.35");

    CXXC_CHECK_EQUAL(std::string(format("%+0*.*f") % 10 % 4 % 2.34f), "+0002.3400");

    CXXC_CHECK_THROW(format::format_error, std::string xx = format("foo: %d %d") % 1);
}


CXXC_TEST(ArgTest)
{
	CXXC_CHECK_EQUAL(std::string(format("%0*d") % 4 % 5), "0005");
}




CXXC_TEST(foo)
{


	std::string x;
	x = format("fooo %d %s %s") % 23000234  % "foo" % 24;
	std::cout << x << std::endl;

	//x = format("Test: %+#0- 123.5d") % 23;
	//std::cout << x << std::endl;

	std::cout << std::string(format("%+04d") % 5) << std::endl;
	std::cout << std::string(format("%+010.5d") % 5) << std::endl;
	std::cout << std::string(format("%+10.5d") % 5) << std::endl;
	std::cout << std::string(format("%+010.5d") % 5) << std::endl;
	std::cout << std::string(format("%+0*.3f") % 10 % 5.123456f) << std::endl;
	std::cout << std::string(format("%+0*.2f") % 10 % 2.34f) << std::endl;


}



int main(void)
{
    std::locale::global(std::locale("C"));
    std::cout.imbue(std::locale());
    std::cerr.imbue(std::locale());
    std::clog.imbue(std::locale());
    std::wcout.imbue(std::locale());
    std::wcerr.imbue(std::locale());
    std::wclog.imbue(std::locale());

    return cxxc::runAll();
}
