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


    typedef Database<dal::sqlite> DBMS;
    //typedef Database<dal::generic> DBMS;

    

    DBMS::Environment env("sqlite:libsqlite");

    DBMS::Connection dbc(env);

    try
    {
        dbc.connect(L"/tmp/foo/bar/sampledb.sqlitedb");
    }
    catch(DBMS::SQLSTATE_08000 state) // ups, no reference?
    {
    	//std::cout << conv_to(dbc.fetchDiag().str(), "UTF-8") << std::endl;
        std::cout << state.diag().str().to("UTF-8") << std::endl;
        return 0;
    }
/*
    catch(DBMS::SQLSTATE_XY000 state)
    {
    }
*/

    env.diagAvail();

    //const DBMS::Diag &diag = env.fetchDiag();

    //DBMS::Diag &diag = env.fetchDiag(1);

    /*
      DBMS::Environment::diag_iterator i1;

      env.diagClear();
      DBMS::diag_iterator i2 = env.diagBegin();; // is const
      DBMS::diag_iterator i3 = env.diagEnd();; // is const
      env.diagEmpty();

      env.diag().begin();
    


      //IDiagnostic::iterator 

      abc.diag().getQuery();

    */

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

