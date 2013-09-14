#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/firebird>
//#include <dbwtl/dal/engines/generic>
#include <dbwtl/ustring>



#include "../cxxc.hh"


using namespace informave::db::ex;


using namespace informave::db;
typedef Database<firebird> DBMS;


struct FirebirdTestbaseFixture
{
    FirebirdTestbaseFixture(void)
        : env("firebird:libfbclient"),
          dbc(env)
    {}
    
    
    void onSetUp(void)
    {
    	 dbc.connect("localhost:/var/lib/firebird/dbwtldb.fdb", "SYSDBA", "masterkey");
    }
    
   virtual ~FirebirdTestbaseFixture(void) {}

    DBMS::Environment env;
    DBMS::Connection dbc;
};


struct FirebirdTestbaseIso88591Fixture
{
    FirebirdTestbaseIso88591Fixture(void)
        : env("firebird:libfbclient"),
          dbc(env)
    {}


    void onSetUp(void)
    {
	IDbc::Options opts;
	opts["database"] = "localhost:/var/lib/firebird/dbwtldb.fdb";
	opts["username"] = "SYSDBA";
	opts["password"] = "masterkey";
         dbc.connect(opts);
    }

   virtual ~FirebirdTestbaseIso88591Fixture(void) {}

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
