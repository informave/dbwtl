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
    using namespace informave::db;
    using namespace informave::i18n;


    typedef Database<dal::sqlite> DBMS;
    //typedef Database<dal::generic> DBMS;

    

    DBMS::Environment env("sqlite:libsqlite");

    assert(env.getOption("env_library_path").isnull() == true);

    int x = 5;
    env.setOption("env_library_path", x);
    assert(env.getOption("env_library_path").asWideStr() == L"5");

    env.setOption("env_library_path", std::wstring(L"/tmp"));
    assert(env.getOption("env_library_path").asWideStr() == L"/tmp");
                  
    const DBMS::Variant& var = env.getOption("env_library_path");


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
        std::cout << "error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

