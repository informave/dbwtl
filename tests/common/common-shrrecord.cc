#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/generic>
#include "../cxxc.hh"

using namespace informave::db;

/*
CXXC_TEST(RecordInitCpp0x)
{
   //Record r3 = { Variant(1), Variant (2) };
}
*/


CXXC_TEST(RecordAccess)
{
    ShrRecord r1;
}


CXXC_TEST(RecordCopy)
{
	ShrRecord r1; // size is 0;
	ShrRecord r2;
	ShrRecord r3(5); // explicit 5 columns, operator= must fail!
	r1.allocate(5);
	CXXC_CHECK(r1.size() == 5);

	CXXC_CHECK_THROW(EngineException, r1 = r2);
	r2.allocate(r1.size());
    r1 = r2;

	r2[1] = 5;
	r1 = r2;
	CXXC_CHECK(r1[1].get<int>() == r2[1].get<int>());
}

CXXC_TEST(RecordClear)
{
    ShrRecord r1;
    r1.allocate(5);
    CXXC_CHECK(r1.size() == 5);
    r1.clear();
    CXXC_CHECK(r1.size() == 0);
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

