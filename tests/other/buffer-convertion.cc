#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>

#include <sstream>
#include <iostream>
#include <locale>
#include <string>

int main(void)
{
    using namespace informave::db;
    typedef Database<dal::sqlite> DBMS;

    std::locale::global(std::locale(""));
    std::wcout.imbue(std::locale());
    
    std::stringstream ss;
    ss << "Hello World" << std::endl;

    std::basic_streambuf<char> *buf = ss.rdbuf();

    std::wstreambuf *wb = new streambuf_convert<char>(buf);

    std::wcout << wb;
    
    delete wb;

    return 0;
}

