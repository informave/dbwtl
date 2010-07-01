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
    using namespace informave;
    using namespace informave::db::dal;


    // auto_ptr for scope deletion
    SqliteEnv::ptr env(Factory::create<sqlite>(L"libsqlite"));
    // db connection
    SqliteDbc::ptr dbc(env->newConnection());


    // now connect
    dalstate_t state = dbc->connect(L"sampledb.sqlitedb");
    if(state != DALSTATE_OK)
    {
        i18n::UString s = state.dump();
        //throw std::runtime_error(state.getMsgUTF8());
        throw std::runtime_error(i18n::conv_to(s, "UTF-8"));
    }

    // print current state
    //std::cout << Sqlite::engine_state(state);
    
    std::cout << "Driver name: " << i18n::conv_to(dbc->driverName(), "ISO-8859-1") << std::endl;
    std::cout << "DBMS name: " << i18n::conv_to(dbc->dbmsName(), "ISO-8859-1") << std::endl;


    

    SqliteStmt::ptr stmt(dbc->newStatement());

    dbc->directCmd(L"CREATE TEMP TABLE test(id INTEGER, name TEXT);");

    stmt->prepare(L"INSERT INTO test(id, name) VALUES(?, 'foo');");
    //std::string v = "mynew";
    int v = 104;
    stmt->bind(1, &v);
    /*
      const SqliteVariant* abc = &stmt->resultset().column(1);
      stmt->bind(2, abc);
    */

    stmt->execute();
    v = 105;
    stmt->execute();
    v = 106;
    stmt->execute();

    stmt->close();


    stmt->prepare(L"SELECT oid,* from test;");
    stmt->execute();

    SqliteResult& res = stmt->resultset();    

    {
        const IVariant& var = res.column(3);

        //IBlob& blob = var.asBlob();

/*

  bind(IBlob& );
  bind(blob);

*/

    }





    // print header
    for(size_t i = 1; i <= res.columnCount(); ++i)
    {
        std::cout << i18n::conv_to(res.columnName(i), "UTF-8") << " (" 
                  << i18n::conv_to(res.datatype(i).name(), "UTF-8") << ") ";
    }



    std::cout << std::endl;


    // print rows

    for(res.first(); ! res.eof(); res.next())
    {
        for(size_t i = 1; i <= res.columnCount(); ++i)
        {
            if(res.column(i).isnull())
                std::cout << "<NULL>" << " | ";
            else
            {
                std::cout << res.column(i).asNarrowStr("UTF-8") << " = ";
                //std::cout << res.field(i).asStr("ISO-8859-15") << " | ";
                if(res.datatype(i).daltype() == DAL_TYPE_BLOB)
                {
                    SqliteBlob& myblob = res.column(i).asBlob();
                    
                    char *buf = new char[myblob.size() + 1];
                    myblob.read(buf, myblob.size());
                    buf[myblob.size()] = '\0';
                    std::cout << "BLOB: " << buf;
                    delete[] buf;

                    /*
                      BlobStream bs(myblob);
                      std::string x;
                      bs >> x;
                    */

                    /*
                      SqliteBlob& myblob = res.column(i).asBlob();
                      SqliteMemo& mymemo = res.column(i).asMemo();
                      oblobstream bs;
                      bs.open(myblob);

                      MemoStream ms;
                      BlobStream bs;

                      IBlob& blob = res.column(i).asBlob();
                      bs.open(blob); // basic_istream<char>

                      IMemo& memo = res.column(i).asMemo();
                      ms.open(memo);
                    */
                }
            }
        }
        std::cout << std::endl;
    }


    stmt->close();

    state = dbc->disconnect();
    //std::cout << Sqlite::engine_state(state);

    return state == DALSTATE_OK ? 0 : -1;
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

