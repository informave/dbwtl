
#include <fstream>
#include <sstream>

#include <dbwtl/dal/engines/generic>
#include "../cxxc.hh"

using namespace informave::db;




CXXC_TEST(CreateVariantX)
{
	std::stringstream ss;
	ss << std::string("TEST");
	
	std::stringstream s1;
	s1 << ss.rdbuf();

	ss.seekg(0);
	std::stringstream s2;
	s2 << ss.rdbuf();

	CXXC_CHECK( s1.str() == s2.str() );
}



CXXC_TEST(BlobStream2String)
{
	char data[] = {0x45, 0x47, 0x02, 0x00, 0x3F };
	std::stringstream ss;
	ss << std::string(&data[0], (&data[0])+sizeof(data));
	Variant x(BlobStream(ss.rdbuf()));

	CXXC_CHECK( ss.rdbuf() == x.get<BlobStream>().rdbuf() );

	//CXXC_ECHO( x.get<String>() );
	CXXC_CHECK( x.get<String>() == String("454702003F") );
	CXXC_CHECK( x.get<String>() == String("454702003F") );

	CXXC_CHECK( ss.rdbuf() != x.get<BlobStream>().rdbuf() );

	CXXC_CHECK( Variant(x.get<BlobStream>()).get<String>() == String("454702003F") );
}


CXXC_TEST(MemoStream2String)
{
	std::wstringstream ss;
        ss << L"HELLO WORLD";
        Variant x(MemoStream(ss.rdbuf()));

	CXXC_CHECK( ss.rdbuf() == x.get<MemoStream>().rdbuf() );

        CXXC_CHECK( x.get<String>() == String("HELLO WORLD") );
	CXXC_CHECK( x.get<String>() == String("HELLO WORLD") );

	CXXC_CHECK( ss.rdbuf() != x.get<MemoStream>().rdbuf() );

	CXXC_CHECK( Variant(x.get<MemoStream>()).get<String>() == String("HELLO WORLD") );

}


CXXC_TEST(BlobStreamDeepcopy)
{
        char data[] = {0x45, 0x47, 0x02, 0x00, 0x3F };
        std::stringstream ss;
        ss << std::string(&data[0], (&data[0])+sizeof(data));
        Variant x(BlobStream(ss.rdbuf()));

        CXXC_CHECK( ss.rdbuf() == x.get<BlobStream>().rdbuf() );

        Variant copy(x);
	CXXC_CHECK( ss.rdbuf() != x.get<BlobStream>().rdbuf() );

        CXXC_CHECK( copy.get<String>() == String("454702003F") );
        CXXC_CHECK( x.get<String>() == String("454702003F") );

        CXXC_CHECK( ss.rdbuf() != x.get<BlobStream>().rdbuf() );

        CXXC_CHECK( Variant(x.get<BlobStream>()).get<String>() == String("454702003F") );
}


CXXC_TEST(MemoStreamDeepcopy)
{
        std::wstringstream ss;
        ss << L"HELLO WORLD";
        Variant x(MemoStream(ss.rdbuf()));

        CXXC_CHECK( ss.rdbuf() == x.get<MemoStream>().rdbuf() );

	Variant copy(x);
	CXXC_CHECK( ss.rdbuf() != x.get<MemoStream>().rdbuf() );

	CXXC_CHECK( copy.get<String>() == String("HELLO WORLD") );
	CXXC_CHECK( x.get<String>() == String("HELLO WORLD") );

	CXXC_CHECK( ss.rdbuf() != x.get<MemoStream>().rdbuf() );

	CXXC_CHECK( Variant(x.get<MemoStream>()).get<String>() == String("HELLO WORLD") );
}



int main(void)
{
    std::locale::global(std::locale(""));
    std::cout.imbue(std::locale());
    std::cerr.imbue(std::locale());
    std::clog.imbue(std::locale());
    std::wcout.imbue(std::locale());
    std::wcerr.imbue(std::locale());
    std::wclog.imbue(std::locale());

    return cxxc::runAll();
}

