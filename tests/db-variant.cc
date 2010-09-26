#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>

#define PRINT(x) std::cout << x << std::endl;

int test(void)
{
    using namespace informave::db;
    using namespace informave::db::dal;
    using namespace informave::i18n;
    //using namespace informave::db::DAL;


    {
        Variant v(DAL_TYPE_STRING, L"varchar");
        v.setWideStr(L"1");        
        assert(v.asBool() == true);
    }

    {
        Variant v(DAL_TYPE_STRING, L"varchar");
        std::wstringstream ss(L"foobar");
        v.setMemo(ss.rdbuf());
        assert(v.asWideStr() == L"foobar");
    }


    {
        Variant v(DAL_TYPE_STRING, L"varchar");
        v.setWideStr(L"123");
        assert(v.asInt() == 123);
        assert(v.asUInt() == 123);
//         assert(v.asBool() == true);
        assert(v.asSmallint() == 123);
        assert(v.asUSmallint() == 123);
        assert(v.asBigint() == 123);
        assert(v.asUBigint() == 123);
//         assert(v.asNumeric() == 123);
        assert(v.asReal() == 123);
        assert(v.asDouble() == 123);
//         assert(v.asDate() == 123);
//         assert(v.asTime() == 123);
//         assert(v.asTimestamp() == 123);
//         assert(v.asInternval() == 123);
//         assert(v.asBlob() == 123);
//         assert(v.asMemo() == 123);
        v.setWideStr(L"123.456");
        assert(v.asInt() == 123);
        v.setWideStr(L"-123.456");
        assert(v.asInt() == -123);

    }




    signed int a = 5;
    Variant v_a(a);

    Variant v_b(v_a);

    Variant v_c;
    v_c = v_b;


    // add assign and type check


    Variant v_x(DAL_TYPE_STRING, L"foobar");
    //v_x.setNull();
    try
    {
        v_x.asWideStr();
        assert(!"variant is not null");
    }
    catch(ex::null_value&)
    {}
    catch(...)
    {
        assert(!"unknown exception");
    }


    std::cout << v_c << std::endl;
    std::wcout << v_c << std::endl;

    //if(v_c == v_b) { } 

    return (v_c.asInt() == v_a.asInt()) ? 0 : -1;

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

