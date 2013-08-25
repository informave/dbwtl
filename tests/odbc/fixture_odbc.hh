#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/firebird>
#include <dbwtl/dal/engines/odbc>
//#include <dbwtl/dal/engines/generic>
#include <dbwtl/ustring>



#include "../cxxc.hh"


using namespace informave::db::ex;


using namespace informave::db;
typedef Database<odbc> DBMS;


struct OdbcTestbaseFixture
{
    OdbcTestbaseFixture(void)
        : env("odbc:libodbc"),
          dbc(env)
    {}
    
    
    void onSetUp(void)
    {
        IDbc::Options opts;
        opts["datasource"] = "foo";
        opts["username"] = "sa";
        opts["password"] = "test";
        opts["charset"] = "iso8859_1";
	opts["unicode"] = "yes";

    	dbc.connect(opts);
    	 //dbc.connect("mypgsql", "postgres", "test");
    }
    
   virtual ~OdbcTestbaseFixture(void) {}

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
