
#include <dbwtl/dbobjects>
#include <iostream>
#include "../cxxc.hh"


using informave::utils::bcd;






CXXC_TEST(Addition)
{
    CXXC_CHECK( bcd(159) + bcd(41) == bcd(200) );

    CXXC_CHECK( bcd(159) + bcd(41) != bcd(100) );

    CXXC_CHECK( bcd(9999) + bcd(1) == bcd(10000) );

    CXXC_CHECK( bcd("1.0001").scale() == 4 );

    CXXC_CHECK( bcd(9999) + bcd("1.001") == bcd("10000.001") );

    CXXC_CHECK( bcd(33) + bcd(2) == bcd("35") );

    CXXC_CHECK( bcd(33) + bcd(-35) == bcd("-2") );

    CXXC_CHECK( bcd(-33) + bcd(-2) == bcd("-35") );
    CXXC_CHECK( bcd(33) + bcd(-2) == bcd("31") );
    CXXC_CHECK( bcd(-33) + bcd(2) == bcd("-31") );
}




CXXC_TEST(Subtraction)
{
	CXXC_CHECK( bcd(101) - bcd(4) == bcd(97) );

	CXXC_CHECK( bcd(10) - bcd(10) == bcd(0) );

	CXXC_CHECK( bcd(9999) - bcd(1111) == bcd(8888) );

	CXXC_CHECK( bcd("1.345") - bcd("0.045") == bcd("1.300") );

	CXXC_CHECK( bcd(2) - bcd("0.10000000000000000001") == bcd("1.89999999999999999999") );

	CXXC_CHECK( bcd(873) - bcd(218) == bcd(655) );

	CXXC_CHECK( bcd(5) - bcd(8) == bcd("-3") );

    CXXC_CHECK( bcd("4016783.28276") - bcd("9267.1825267392671935261936711782")
                == bcd("4007516.1002332607328064738063288218") );


    CXXC_CHECK( bcd(33) - bcd(2) == bcd("31") );
    CXXC_CHECK( bcd(-33) - bcd(-2) == bcd("-31") );
    CXXC_CHECK( bcd(33) - bcd(-2) == bcd("35") );
    CXXC_CHECK( bcd(-33) - bcd(2) == bcd("-35") );

}



CXXC_TEST(Multiplication)
{
    CXXC_CHECK( bcd(100) * bcd("2.5") == bcd(250) );

    CXXC_CHECK( bcd(1) * bcd("0") == bcd(0) );

    CXXC_CHECK( bcd(1) * bcd("1") == bcd(1) );

    CXXC_CHECK( bcd(100) * bcd("1.1") == bcd(110) );

    CXXC_CHECK( bcd(100) * bcd("1.1") == bcd(110) );

    CXXC_CHECK( bcd("12398.00028736") * bcd("0.9172860267") == bcd("11372.512422617912632512") );
   
    CXXC_CHECK( bcd(9999) * bcd("1.000") == bcd(9999) );

    CXXC_CHECK( bcd("0.000000000000000000000000000000001") * bcd(3)
                == bcd("0.000000000000000000000000000000003") );

    CXXC_CHECK( bcd(33) * bcd(2) == bcd("66") );
    CXXC_CHECK( bcd(-33) * bcd(-2) == bcd("66") );
    CXXC_CHECK( bcd(33) * bcd(-2) == bcd("-66") );
    CXXC_CHECK( bcd(-33) * bcd(2) == bcd("-66") );
}


CXXC_TEST(Division)
{
    CXXC_CHECK( bcd(10) / bcd(2) == bcd(5) );

    CXXC_CHECK( bcd(10) / bcd(2) != bcd(50) );

    CXXC_CHECK( bcd(5) != bcd(50) );

    CXXC_CHECK( bcd(98) / bcd("2.5") == bcd("39.2"));

    CXXC_CHECK( 998 - bcd(3) == 995 );

    CXXC_CHECK( bcd(3) - 998 == -995 );

    CXXC_CHECK( bcd(33) - bcd(1) == 32 );

    CXXC_CHECK( bcd(33) / bcd(2) == bcd("16.5") );
    CXXC_CHECK( bcd(-33) / bcd(-2) == bcd("16.5") );
    CXXC_CHECK( bcd(33) / bcd(-2) == bcd("-16.5") );
    CXXC_CHECK( bcd(-33) / bcd(2) == bcd("-16.5") );



    //CXXC_CHECK( bcd("0.182736728") / bcd("0.18273600001") == bcd("1.000003983834602706427053087162") );
}


CXXC_TEST(Important)
{

	CXXC_CHECK( bcd("1.3000") == bcd("1.3") );

    CXXC_CHECK( bcd("-512") == bcd(-512) );
}



CXXC_TEST(Compare)
{
	CXXC_CHECK( bcd(5) > bcd(4) );

	CXXC_CHECK( bcd(4) == bcd(4) );

	CXXC_CHECK( bcd(4) < bcd(5) );

	CXXC_CHECK( bcd("0.0004") < bcd("0.003") );

	CXXC_CHECK( bcd("-5") < bcd("-4") );

	CXXC_CHECK( bcd(39) > bcd("-0.39") );
}



int main(void)
{
    return cxxc::runAll();
}


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
