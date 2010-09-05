#ifndef INFORMAVE_DB_UTIL_DEVUTILS_HH
#define INFORMAVE_DB_UTIL_DEVUTILS_HH

#include <iostream>
#include <cassert>
#include <string>


inline std::wstring to_widen(std::string s)
{ return std::wstring(s.begin(), s.end()); }

#define DAL_SPOS to_widen(std::string(__FUNCTION__))



#if defined(__GNUC__)
 #define DBWTL_FUNC_NAME __FUNCTION__
//#define DBWTL_FUNC_NAME __PRETTY_FUNCTION__
#elif defined(_MSC_VER)
 #define DBWTL_FUNC_NAME __FUNCTION__
#else
 #define DBWTL_FUNC_NAME "<func-macro-not-supported>"
#endif


///
/// @brief Adds a new option to Env, Dbc or Stmt
///
/// Should be only used in a cunstructor of one of these
/// three classes.
#define DAL_ADD_OPTION(name, type)                                      \
    this->m_options.insert(options_type::value_type(name,               \
                                                    Variant(type, L##name))) 




/// @brief Convert a number to string
#define DBWTL_STRINGIFY(x) #x
#define DBWTL_TOSTRING(x) DBWTL_STRINGIFY(x)



/// @brief Current code position as string
#define DBWTL_CODE_POS                          \
    __FILE__ ":"                                \
    DBWTL_TOSTRING(__LINE__)

#define DBWTL_STRINGIFY(x) #x
#define DBWTL_TOSTRING(x) DBWTL_STRINGIFY(x)



#define DAL_NEEDS_CHECK(msg)                                            \
    {                                                                   \
        std::cerr << "[*** NEED CHECK ***] " msg << " # " << __FILE__   \
                  << ": " << __FUNCTION__                               \
                  << std::endl;                                         \
    }

#define DAL_NOT_IMPL() { assert(!"not implemented"); }

 

//--------------------------------------------------------------------------
///
/// @cond DEV_DOCS
/// Deletes a object
struct delete_object
{
    template <typename T>
    void operator()(T ptr){ delete ptr;}

    template<typename U, typename V>
    void operator()(std::pair<U, V> val){ delete val.second;}
};
/// @endcond



#endif
