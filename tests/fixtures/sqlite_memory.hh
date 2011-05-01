#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>



#include "../cxxc.hh"


using namespace informave::db::dal;
using namespace informave::db::ex;


using namespace informave::db;
typedef Database<dal::sqlite> DBMS;


struct SqliteMemoryFixture
{
    SqliteMemoryFixture(void)
        : env("sqlite:libsqlite"),
          dbc(env)
    {}
    
    
    void onSetUp(void)
    {
        dbc.connect(L":memory:");
    }
    
    DBMS::Environment env;
    DBMS::Connection dbc;
};


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
