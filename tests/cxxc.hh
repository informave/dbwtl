//
// cxxc.hh - C++ Check
//
// Copyright (C)         informave.org
//   2011,               Daniel Vogelbacher <daniel@vogelbacher.name>
//
// You can use and redistribute this file without any restrictions.
//

/// @file
/// @brief C++ Test
/// @author Daniel Vogelbacher
/// @version 0.0.1


#ifndef CXXC_HEADER_HH
#define CXXC_HEADER_HH

#include <iostream>
#include <stdexcept>
#include <list>
#include <algorithm>
#include <sstream>
#include <string>


namespace cxxc
{
    class Test;
    typedef std::list<Test*> TestList;


    /// Selects the type T if V is true, else type U.
    ///
    template<bool V, typename T, typename U> struct select { typedef T result; };

    template<typename T, typename U> struct select<false, T, U> { typedef U result; };




/// Defines a helper template to test for a specific member function.
///
#define DEFINE_HAS_MEM(func)                                            \
    template<typename Type>                                             \
    struct has_mem_##func                                               \
    {                                                                   \
        typedef char yes[1];                                            \
        typedef char no[2];                                             \
                                                                        \
                                                                        \
        template <typename T, T> struct TypeCheck;                      \
                                                                        \
        template <typename T> struct Method                             \
        {                                                               \
            typedef void (T::*fptr)();                                  \
        };                                                              \
                                                                        \
        template <typename T> static yes& test(TypeCheck< typename Method<T>::fptr, &T::func >*); \
        template <typename T> static no&  test(...);                    \
                                                                        \
                                                                        \
        static const int value = (sizeof(test<Type>(0)) == sizeof(yes)); \
    }

    DEFINE_HAS_MEM(onSetUp);
    DEFINE_HAS_MEM(onTearDown);




    /// Functor to run a test.
    ///
    struct TestRunner
    {
        void operator()(Test *test);
    };

    


    /// Test manager.
    ///
    /// This class is implemented as a singleton.
    ///
    class Manager
    {
    public:
        inline TestList& getTestList(void) { return this->m_testlist; }

        int runAll();

        int testCount() const { return m_testlist.size(); }


        static Manager& instance(void)
        {
            static Manager _instance;
            return _instance;
        }

    protected:
        TestList m_testlist;

    private:
        Manager(void) : m_testlist() {}
    };




    /// Base class for tests.
    ///
    class Test
    {
    public:
        Test(const char *name, int line, const char *file);

        virtual void run_impl(void) = 0;

        void run(void) { this->run_impl(); }

        const char* name(void) const { return this->m_name; }

        const char* file(void) const { return this->m_file; }

        int line(void) const { return this->m_line; }

        int testnum(void) const { return this->m_test_num; }

    protected:
        const char  *m_name;
        int          m_line;
        const char  *m_file;
        int          m_test_num;
    };



    class TestDetails
    { 
    public:
        TestDetails(const char *name, const char *file, int line, const char *extra = 0)
            : m_what()
        {
            std::stringstream ss;
            ss << name << " failed at " << file << "(" << line << "). ";
            if(extra)
                ss << extra;
            m_what = ss.str();
               
        }

        std::string what(void) const { return this->m_what; }

        std::string m_what;
    };

    class Failure : public std::exception
    { 
    public:
        Failure(TestDetails tr) :m_what(tr.what()) {}

        virtual ~Failure(void) throw() {}

        virtual const char* what(void) const throw() { return m_what.c_str(); }

    protected:
        std::string m_what;
    };



    // SHORTCUT FUNCTIONS

    int runAll(void) { return Manager::instance().runAll(); }

    int testCount(void) { return Manager::instance().testCount(); }


    // IMPLEMENTATION

    int Manager::runAll()
    { 
        try
        {
            std::for_each(this->m_testlist.begin(), this->m_testlist.end(), TestRunner());
        }
        catch(Failure &e)
        {
            return 1;
        }
        return 0;
    }
    

    Test::Test(const char *name, int line, const char *file)
        : m_name(name),
          m_line(line),
          m_file(file),
          m_test_num(0)
    {
        Manager::instance().getTestList().push_back(this);
        this->m_test_num = Manager::instance().getTestList().size();
    }


    void TestRunner::operator()(Test *test)
    {
        std::cerr << "Running test (" << test->testnum() << "/" << Manager::instance().testCount() << ") " << test->name() << std::endl;
        try
        {
            test->run();
        }
        catch(Failure &e)
        {
            std::cerr << e.what() << std::endl;
            if(true) throw e;
        }
        catch(std::exception &e)
        {
            TestDetails td(test->name(), test->file(), test->line(), std::string("Exception raised, what(): ").append(e.what()).c_str());
            std::cerr << td.what() << std::endl;
            if(true) throw Failure(td);
        }

        catch(...)
        {
            TestDetails td(test->name(), test->file(), test->line(), "Test raised an exception which is not derived from std::exception.");
            std::cerr << td.what() << std::endl;
            if(true) throw Failure(td);
        }

    }



    namespace details
    {
        template<typename T>
        struct EventSetUpMapper : public virtual T
        { 
            void do_SetUp(void) { T::onSetUp(); } 

            virtual ~EventSetUpMapper(void) {}
        };

        template<typename T>
        struct EmptyEventSetUp : public virtual T
        { 
            void do_SetUp(void) { } 
            
            virtual ~EmptyEventSetUp(void) {}
        };


        template<typename T>
        struct EventTearDownMapper : public virtual T
        {
            void do_TearDown(void) { T::onTearDown(); } 

            virtual ~EventTearDownMapper(void) {}
        };

        template<typename T>
        struct EmptyEventTearDown : public virtual T
        {
            void do_TearDown(void) { } 

            virtual ~EmptyEventTearDown(void) {}
        };

    } // namespace details end






// MACROS


/// CHECK(expr) Macro
///
/// Checks if the expr expression evaluates to true.
/// If the result is false, a CheckError exception is raised.
///
#define CXXC_CHECK(expr) if(expr) {} else { throw cxxc::Failure(cxxc::TestDetails("CHECK() test", __FILE__, __LINE__, \
                                                                                  "Failed expression: (" #expr ")")); }

#define CXXC_CMP_DOUBLE(v1, v2, tolerance) if(std::abs(v1 - v2) <= tolerance) {} else { throw cxxc::Failure(cxxc::TestDetails("CHECK() test", __FILE__, __LINE__, \
								"Failed expression: (" #v1 ")")); }


/// EVAL(expr) Macro
///
/// Prints the expr and evaluates it
///
#define CXXC_EVAL(expr) std::cout << "\tEVAL[ " << #expr << " ]: " << (expr) << std::endl

/// ECHO(expr) Macro
///
/// Prints the expr
///
#define CXXC_ECHO(expr) std::cout << "\t" << expr << std::endl



/// TEST(name) Macro
///
/// Defines a new test which is automatically registered in the
/// global test manager.
///
#define DEFINE_TEST_CLASS(name)                                     \
    class Test##name : public cxxc::Test                            \
    {                                                               \
    public:                                                         \
        Test##name(void) : cxxc::Test(#name, __LINE__, __FILE__)    \
        {}                                                          \
                                                                    \
        virtual ~Test##name(void) {}                                \
    private:                                                        \
        virtual void run_impl(void);                                \
    } test##name##Instance;                                         \
                                                                    \
    void Test##name::run_impl(void)


#define CXXC_TEST(name) DEFINE_TEST_CLASS(name)




/// FIXTURE_TEST(name) Macro
/// 
/// Defines a new test which uses a fixture class.
///
#define DEFINE_FIXTURE_TEST_CLASS(fixture, name)                        \
    class FixtureTest##name##Helper :                                   \
        public virtual cxxc::select<cxxc::has_mem_onSetUp<fixture>::value, \
                                    cxxc::details::EventSetUpMapper<fixture>, \
                                    cxxc::details::EmptyEventSetUp<fixture> >::result, \
        public virtual cxxc::select<cxxc::has_mem_onTearDown<fixture>::value, \
                                    cxxc::details::EventTearDownMapper<fixture>, \
                                    cxxc::details::EmptyEventTearDown<fixture> >::result \
                                                                        \
    {                                                                   \
    public:                                                             \
        /*not needed*/                                                  \
        typedef cxxc::select<cxxc::has_mem_onSetUp<fixture>::value,     \
                             cxxc::details::EventSetUpMapper<fixture>,  \
                             cxxc::details::EmptyEventTearDown<fixture> >::result base; \
                                                                        \
        FixtureTest##name##Helper(void)                                 \
        {}                                                              \
                                                                        \
        virtual ~FixtureTest##name##Helper(void) {}                     \
        void run(void) { this->run_impl(); }                            \
                                                                        \
        void triggerSetUp(void) { do_SetUp(); }                         \
        void triggerTearDown(void) { do_TearDown(); }                   \
                                                                        \
    private:                                                            \
        virtual void run_impl(void);                                    \
};                                                                      \
                                                                        \
                                                                        \
    DEFINE_TEST_CLASS(name)                                             \
    {                                                                   \
        FixtureTest##name##Helper x;                                    \
        x.triggerSetUp();                                               \
        x.triggerTearDown();                                            \
        x.run();                                                        \
    }                                                                   \
                                                                        \
    void FixtureTest##name##Helper::run_impl(void)


#define CXXC_FIXTURE_TEST(fixture, name) DEFINE_FIXTURE_TEST_CLASS(fixture, name)    


/// CHECK_THROW(ex, expr) Macro
///
/// Checks if the expr expression throws an exception of type ex.
/// If not, a ExceptionError exception is raised.
#define CXXC_CHECK_THROW(ex, expr)                                      \
    {                                                                   \
        bool __caught = false;                                          \
        try { expr; }                                                   \
        catch(ex) { __caught = true; }                                  \
        if(!__caught)                                                   \
            throw cxxc::Failure(cxxc::TestDetails(                      \
                                    "CHECK_THROW() test", __FILE__, __LINE__, \
                                    "Expression (" #expr ") has not raised the " \
                                    "expected exception: " #ex));       \
    }
    

} // namespace end

#endif


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
