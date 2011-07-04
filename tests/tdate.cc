//#include <dbwtl/dal/dalinterface>
//#include <dbwtl/dal/engines/sqlite>
//#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
//#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>

#include "cxxc.hh"


using namespace informave::db;


CXXC_TEST(TDateCreation)
{
    TDate d("2010-12-04");
    CXXC_CHECK( d.day() == 4 && d.month() == 12 && d.day() == 4);

    CXXC_CHECK_THROW(ex::convert_error, TDate d2("234234234"));
    CXXC_CHECK_THROW(ex::convert_error, TDate d3("2011-13-04"));

    TDate d4("2011-12-04 13:12:04");
    TDate d5("2011-12-04T13:12:04Z");
    CXXC_CHECK( d5.day() == 4 && d5.month() == 12 && d5.year() == 2011 );
    TDate d6("now");
}

CXXC_TEST(Compare)
{
    CXXC_CHECK( TDate("2011-12-31") < TDate("2012-01-01") );
    CXXC_CHECK( TDate("2011-12-31") > TDate("2011-12-30") );
    CXXC_CHECK( ! (TDate("2011-12-31") > TDate("2011-12-31")) );
    CXXC_CHECK( ! (TDate("2011-12-31") < TDate("2011-12-31")) );
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
