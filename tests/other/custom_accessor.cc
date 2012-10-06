#include <dbwtl/dal/dalinterface>
#include <dbwtl/dal/engines/sqlite>
#include <dbwtl/dal/engines/generic>
#include <dbwtl/dbobjects>
#include <dbwtl/ustring>


#include <iostream>
#include <memory>
#include <cstdlib>

#define PRINT(x) std::cout << x << std::endl;



class Foo
{
public:
    int x;
};


DB_NAMESPACE_BEGIN

template<>
class sv_accessor<Foo> : public virtual sa_base<Foo>,
	public supports<signed int>
{
public:
    //READ_ACCESSOR(DAL_TYPE_CUSTOM);

    virtual signed int cast(signed int*, std::locale) const
        {
            return this->get_value().x;
        }
};



DB_NAMESPACE_END


int test(void)
{
    using namespace informave::db;
//    using namespace informave::i18n;
    //using namespace informave::db::DAL;



    Foo f;
    f.x = 23;


    Variant v(f);

    assert(v.asInt() == 23);

    
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
        std::cout << e.what() << std::endl;
        return -1;
    }
    return 0;
}

