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
    //using namespace informave::i18n;
    //using namespace informave::db::DAL;

    
    //typedef Database<DAL::sqlite> DBMS;

    typedef Database<sqlite> DBMS;


    DBMS::Environment env("sqlite:libsqlite");
    //env.create("sqlite:libsqlite");

    DBMS::Connection dbc(env);
    dbc.connect(L"sampledb.sqlitedb");

    std::cout << dbc.dbmsName().to("ISO-8859-1") << std::endl;



    dbc.directCmd(L"create temp table foo(id integer primary key, "
                  L"name VARCHAR(50), bday DATE, married BOOL);");



    TableList list = dbc.getTables();
    
    for(TableList::const_iterator i = list.begin();
        i != list.end();
        ++i)
    {
        std::cout << std::boolalpha;
        std::cout << "DB  : " << (*i)->getCatalog() << std::endl;
        std::cout << "NAME: " << (*i)->getName() << std::endl;
        std::cout << "SYS : " << (*i)->isSystemObject().asBool() << std::endl;
       // String ssss = (*i)->getDDL().get<String>();
        std::cout << "DLL : " << ifnull<String>((*i)->getDDL(),
                                                     String("NULL"))			    
                  << std::endl;

    }

    return 0;
}



int main(void)
{
    try
    {
        test();
    }
    catch(informave::db::Exception &e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}

