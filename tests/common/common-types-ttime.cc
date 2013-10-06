//#include <dbwtl/dal/dalinterface>
//#include <dbwtl/dal/engines/sqlite>
//#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
//#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>

#include "../cxxc.hh"


using namespace informave::db;


CXXC_TEST(TTimeCreation)
{
    TTime t("23:14:59");
    CXXC_CHECK( t.hour() == 23 && t.minute() == 14 && t.second() == 59);

    TTime t2("01:00");

    CXXC_CHECK_THROW(ConvertException, TTime t2("234234234"));
    CXXC_CHECK_THROW(ConvertException, TTime t3("23:14:70"));
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
