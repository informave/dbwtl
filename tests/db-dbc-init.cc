#include <dbwtl/dal/dalinterface>
//#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>


int main(void)
{
    //using namespace informave;
    using namespace informave::db;
    using namespace informave::i18n;
    using informave::db::dal::dalstate_t;
    //using namespace informave::db::DAL;

    
    //typedef Database<DAL::sqlite> DBMS;

    typedef Database<dal::generic> DBMS;


    DBMS::Environment env("sqlite:libsqlite");
    //env.create("sqlite:libsqlite");

    DBMS::Connection dbc(env);
    dalstate_t state = dbc.connect(L"sampledb.sqlitedb");
    if(! dbc.isConnected() )
        throw std::runtime_error("not connected");

    std::cout << conv_to(dbc.dbmsName(), "ISO-8859-1") << std::endl;

    dbc.directCmd(L"create temp table foo(id int);");

try
{
    dal::TableList list = dbc.getTables();

    for(dal::TableList::iterator i = list.begin(); i != list.end(); ++i)
    {
	   std::cout
	   	<< conv_to((*i)->getName().asStr(), "iso-8859-1") << "|"
		<< conv_to((*i)->getCatalog().asStr(), "iso-8859-1") << "|"
		<< conv_to((*i)->getSchema().asStr(), "iso-8859-1") << "|"
		<< conv_to((*i)->getDDL().asStr(), "iso-8859-1") << "|"
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
