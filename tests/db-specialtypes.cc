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
    //using namespace informave::db::DAL;

    
    //typedef Database<DAL::sqlite> DBMS;

    typedef Database<dal::sqlite> DBMS;


    DBMS::Environment env("sqlite:libsqlite");
    //env.create("sqlite:libsqlite");

    DBMS::Connection dbc(env);
    dbc.connect(L"sampledb.sqlitedb");


    std::cout << conv_to(dbc.dbmsName(), "ISO-8859-1") << std::endl;



    dbc.directCmd(L"create temp table foo(id int);");


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
                
                DBMS::Integer myi(res.column(i));
                DBMS::Text  mfi = res.column(i).asText();
                DBMS::Text ffa = res.column(i);
                DBMS::Memo ffm(res.column(i));
		/*
                stmt.bind(1, myi);
                stmt.bind(2, mfi);
                stmt.bind(2, &mfi);
		*/


                std::cout << res.column(i).asNarrowStr("UTF-8") << " ";
                //std::cout << res.field(i).asStr("ISO-8859-15") << " | ";
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
i18n::UString s = state.dump();
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

