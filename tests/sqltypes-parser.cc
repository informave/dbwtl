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
    using namespace informave::i18n;
    using namespace informave::db::dal;
    //using namespace informave::db::DAL;

    
    SqlTypeParser p;

    //p.parse(L"VARCHAR(50)");

    p.parse(L"INT");
    if(p.getDaltype() != DAL_TYPE_INT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"INTEGER");
    if(p.getDaltype() != DAL_TYPE_INT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"int");
    if(p.getDaltype() != DAL_TYPE_INT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"CHAR");
    if(p.getDaltype() != DAL_TYPE_CHAR)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;


    p.parse(L"VARCHAR");
    if(p.getDaltype() != DAL_TYPE_VARCHAR)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"VARCHAR(50)");
    if(p.getDaltype() != DAL_TYPE_VARCHAR)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;


    p.parse(L"BOOL");
    if(p.getDaltype() != DAL_TYPE_BOOL)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"SHORTINT");
    if(p.getDaltype() != DAL_TYPE_SMALLINT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"BIGINT");
    if(p.getDaltype() != DAL_TYPE_BIGINT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"BLOB");
    if(p.getDaltype() != DAL_TYPE_BLOB)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"NUMERIC(4,2)");
    if(p.getDaltype() != DAL_TYPE_NUMERIC)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"DECIMAL");
    if(p.getDaltype() != DAL_TYPE_DECIMAL)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"MONEY");
    if(p.getDaltype() != DAL_TYPE_MONEY)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"FLOAT");
    if(p.getDaltype() != DAL_TYPE_FLOAT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"REAL");
    if(p.getDaltype() != DAL_TYPE_FLOAT)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"DOUBLE");
    if(p.getDaltype() != DAL_TYPE_DOUBLE)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"DATE");
    if(p.getDaltype() != DAL_TYPE_DATE)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"TIME");
    if(p.getDaltype() != DAL_TYPE_TIME)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"DATETIME");
    if(p.getDaltype() != DAL_TYPE_DATETIME)
        return -1;
    std::wcout << L"Parsed Type: " << daltype2string(p.getDaltype()) << std::endl;

    p.parse(L"TIMESTAMP");
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



