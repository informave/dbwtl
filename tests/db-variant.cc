#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>

int test(void)
{
    //using namespace informave;
    using namespace informave::db::dal;
    using namespace informave::i18n;
    //using namespace informave::db::DAL;

    signed int a = 5;
    Variant v_a(a);

    Variant v_b(v_a);

    Variant v_c;
    v_c = v_b;


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

