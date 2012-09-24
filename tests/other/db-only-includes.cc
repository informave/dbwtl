#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>


int main(void)
{
    using namespace informave::db;
    typedef Database<dal::sqlite> DBMS;

    DBMS::Environment env("sqlite:libsqlite");

    return 0;
}

