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
//    using namespace informave::i18n;
    //using namespace informave::db::DAL;

    
    //typedef Database<DAL::sqlite> DBMS;

    typedef Database<dal::sqlite> DBMS;


    DBMS::Environment env("sqlite:libsqlite");
    //env.create("sqlite:libsqlite");
    env.specialFunc();

    DBMS::Connection dbc(env);
    dbc.connect(L"sampledb.sqlitedb");


    std::cout << dbc.dbmsName().to("ISO-8859-1") << std::endl;



    dbc.directCmd(L"create temp table foo(id int);");

    try
    {
        dal::TableList list = dbc.getTables();

        for(dal::TableList::iterator i = list.begin(); i != list.end(); ++i)
        {
            std::cout
                << (*i)->getName().asStr().to("iso-8859-1") << "|"
                << (*i)->getCatalog().asStr().to("iso-8859-1") << "|"
                << (*i)->getSchema().asStr().to("iso-8859-1") << "|"
                << (*i)->getDDL().asStr().to("iso-8859-1") << "|"
                << std::endl;
        }
    }
    catch(...)
    {
    }
    //abort();



    DBMS::Statement stmt(dbc);
    stmt.execDirect(L"SELECT *,* from customers");

    DBMS::Resultset res;
    res.attach(stmt);


    for(res.first(); ! res.eof(); res.next())
    {
        for(size_t i = 1; i <= res.columnCount(); ++i)
        {
            if(res.column(i).isnull())
                std::cout << "<NULL>" << " | ";
            else
            {
                //DBMS::Value x = res.column(i);
                const DBMS::Value& v = res.column(i);
                std::cout << res.column(i).asStr().to("UTF-8") << " ";
                //std::cout << res.field(i).asWideStr("ISO-8859-15") << " | ";
            }
        }
        std::cout << std::endl;
    }

/*
  DBMS::Resultset::iterator a = res.begin();
  DBMS::Resultset::const_iterator b;
*/
    DBMS::Value* var;

    int abc = 1;
    DBMS::Variant va(&abc);
    

    //DBMS::Variant var; FIX: this is SqliteVariant, but I want PodVariant

    //int id = db_traits<dal::sqlite>::DB_SYSTEM_ID;

    return 0;

/*
// auto_ptr for scope deletion
SqliteEnv::ptr env(Factory::create<Sqlite>(L"libsqlite"));
// db connection
SqliteDbc::ptr dbc(env->newConnection());


// now connect
dalstate_t state = dbc->connect(L"testdb2.db");
if(state != DALSTATE_OK)
{
std::wstring s = state.dump();
//throw std::runtime_error(state.getMsgUTF8());
throw std::runtime_error(i18n::conv_to(s, "UTF-8"));
}
*/
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

