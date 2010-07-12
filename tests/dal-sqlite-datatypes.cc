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
    using informave::db::dal::dalstate_t;
    //using namespace informave::db::DAL;

    
    //typedef Database<DAL::sqlite> DBMS;

    typedef Database<dal::sqlite> DBMS;


    DBMS::Environment env("sqlite:libsqlite");
    //env.create("sqlite:libsqlite");

    DBMS::Connection dbc(env);
    dalstate_t state = dbc.connect(L"sampledb.sqlitedb");
    if(! dbc.isConnected() )
        throw std::runtime_error("not connected");

    std::cout << conv_to(dbc.dbmsName(), "ISO-8859-1") << std::endl;


    dal::DatatypeList dtlist = dbc.getDatatypes();
    for(dal::DatatypeList::const_iterator i = dtlist.begin();
        i != dtlist.end();
        ++i)
    {
        std::cout << "NAME: " << (*i)->getName() << std::endl;
        std::cout << "Size: " << (*i)->getSize() << std::endl;
    }


    return 0;
}



int main(void)
{
    try
    {
        test();
    }
    catch(informave::db::ex::exception &e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}

