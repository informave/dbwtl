#include <dbwtl/dal/dalinterface>
#include <dbwtl/ustring>

#include "../src/dal/sqlutils.hh"

#include <iostream>
#include <memory>
#include <cstdlib>


int test(void)
{
    //using namespace informave;
    //using namespace informave::db::sqlutils;
//    using namespace informave::i18n;
    using namespace informave::db;
    //using namespace informave::db::DAL;

    
    SqlTypeParser p;

    //p.parse(L"VARCHAR(50)");

    p.parse(String("INT"));
    if(p.getDaltype() != DAL_TYPE_INT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("INTEGER"));
    if(p.getDaltype() != DAL_TYPE_INT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("int"));
    if(p.getDaltype() != DAL_TYPE_INT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("CHAR"));
    if(p.getDaltype() != DAL_TYPE_CHAR)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;


    p.parse(String("VARCHAR"));
    if(p.getDaltype() != DAL_TYPE_STRING)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("VARCHAR(10)"));
    if(p.getDaltype() != DAL_TYPE_STRING)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;
    //assert(p.getSize() == 10);
    std::wcout << L"Parsed Size: " << p.getSize() << std::endl;


    p.parse(String("BOOL"));
    if(p.getDaltype() != DAL_TYPE_BOOL)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("SHORTINT"));
    if(p.getDaltype() != DAL_TYPE_SMALLINT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("BIGINT"));
    if(p.getDaltype() != DAL_TYPE_BIGINT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("BLOB"));
    if(p.getDaltype() != DAL_TYPE_BLOB)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("NUMERIC(4,2)"));
    if(p.getDaltype() != DAL_TYPE_NUMERIC)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("DECIMAL"));
    if(p.getDaltype() != DAL_TYPE_NUMERIC)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("MONEY"));
    if(p.getDaltype() != DAL_TYPE_NUMERIC)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("FLOAT"));
    if(p.getDaltype() != DAL_TYPE_FLOAT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("REAL"));
    if(p.getDaltype() != DAL_TYPE_FLOAT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("DOUBLE"));
    if(p.getDaltype() != DAL_TYPE_DOUBLE)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("DATE"));
    if(p.getDaltype() != DAL_TYPE_DATE)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(String("TIME"));
    if(p.getDaltype() != DAL_TYPE_TIME)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;


    p.parse(String("TIMESTAMP"));
    if(p.getDaltype() != DAL_TYPE_TIMESTAMP)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;


    return 0;
}



int main(void)
{
    try
    {
        return test();
    }
    catch(informave::db::ex::exception &e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}



