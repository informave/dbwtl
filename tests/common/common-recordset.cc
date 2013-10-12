#include <dbwtl/dbobjects>
#include <dbwtl/dal/engines/generic>
#include"../cxxc.hh"


using namespace informave::db;

bool sortme(const ShrRecord& a, const ShrRecord& b)
{
	return a[1].get<int>() < b[1].get<int>();
}

#ifdef _UNIX
CXXC_TEST(RecordClear)
{
	RecordSet foo;
	foo.open();
	foo.insert(ShrRecord({Variant(3), Variant(2)}));
	foo.insert(ShrRecord({Variant(2), Variant(2)}));
	foo.insert(ShrRecord({Variant(1), Variant(2)}));

/*
  std::sort(foo.begin(), foo.end(), [](const Record &a, const Record &b)
  {
  return a.get(1).get<int>() < b.get(1).get<int>();
  });
*/

	std::sort(foo.begin(), foo.end(), ColumnSortByNumber<int>(1));


    CXXC_CHECK((*foo.begin())[0].get<int>() == 1);

    RecordSet::iterator i = foo.begin();

    (*i)[1].get<int>();



    ShrRecord r1;
    r1.allocate(5);
    CXXC_CHECK(r1.size() == 5);
    r1.clear();
    CXXC_CHECK(r1.size() == 0);
}


CXXC_TEST(RecordsetCloseCheck)
{
	RecordSet foo;
	foo.open();
	foo.insert(ShrRecord({Variant(3), Variant(2)}));
	foo.close();

	foo.open();
	foo.first();
	CXXC_CHECK( !foo.eof() );
	CXXC_CHECK( foo.column(1).get<int>() == 3 );
	CXXC_CHECK( foo.column(2).get<int>() == 2 );

	foo.clear();
	CXXC_CHECK( !foo.isOpen() );
	CXXC_CHECK( foo.describeColumn(1).getName().get<String>() == std::string("column1") );

	foo.open();
	foo.insert(ShrRecord({Variant(String("hello")), Variant(2)}));
 	foo.first();
	CXXC_CHECK( foo.column(1).get<String>() == std::string("hello") );
	foo.close();
	foo.reset();
	CXXC_CHECK( foo.columnCount() == 0 );
}


CXXC_TEST(RecordsetDesc)
{
	RecordSet foo;
	foo.open();
	//foo.setColumnCount(2);
	// # NOTES: we ne open() before insert(), but insert() may work if not positioned.
	foo.insert(ShrRecord({Variant(3), Variant(2)}));
	CXXC_CHECK( foo.columnCount() == 2 );
	for(colnum_t i = 1; i <= foo.columnCount(); ++i)
	{
		std::cout << foo.describeColumn(i).getName() << std::endl;
	}
	foo.first();
	for(colnum_t i = 1; i <= foo.columnCount(); ++i)
	{
		std::cout << foo.column(i).get<int>() << std::endl;
	}
}


CXXC_TEST(RecordsetScroll)
{
	RecordSet foo;
	foo.open();
	foo.insert(ShrRecord({Variant(3), Variant(2)}));
	CXXC_CHECK( foo.columnCount() == 2 );
	foo.insert(ShrRecord({Variant(4), Variant(5)}));

	foo.first();
	CXXC_CHECK( foo.column(1).get<int>() == 3 );
    foo.next();
	CXXC_CHECK( foo.column(1).get<int>() == 4 );
    foo.prev();
	CXXC_CHECK( foo.column(1).get<int>() == 3 );
    foo.setpos(2);
	CXXC_CHECK( foo.column(1).get<int>() == 4 );
    foo.setpos(1);
	CXXC_CHECK( foo.column(1).get<int>() == 3 );
	
}
#endif


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

