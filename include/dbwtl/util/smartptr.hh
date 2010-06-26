#ifndef INFORMAVE_HELPER_SMARTPTR_HH
#define INFORMAVE_HELPER_SMARTPTR_HH


#include <cassert>
#include <functional>
#include <stdexcept>


#define NS_BEGIN namespace informave { namespace util {


NS_BEGIN

template <bool flag, typename T, typename U>
struct Select
{
    typedef T Result;
};
template <typename T, typename U>
struct Select<false, T, U>
{
    typedef U Result;
};





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template<class T>
class DefaultStorage
{
public:
    typedef T* StoredType;
    typedef T* PointerType;
    typedef T& ReferenceType;

    DefaultStorage(void) : m_pointee(Default())
        {}

    DefaultStorage(const DefaultStorage&) : m_pointee(0)
        {}

    template<class U>
    DefaultStorage(const DefaultStorage<U>&) : m_pointee(0)
        {}

    DefaultStorage(const StoredType& p) : m_pointee(p)
        {}

    PointerType operator->(void) const
        { return this->m_pointee; }

    ReferenceType operator*(void) const
        { return *this->m_pointee; }

	void reset(StoredType& value)
	{
		this->m_pointee = value;
	}

    void swap(DefaultStorage& orig)
        { std::swap(this->m_pointee, orig.m_pointee); }

    friend inline PointerType getImpl(const DefaultStorage& sp)
        { return sp.m_pointee; }

    friend inline const StoredType& getImplRef(const DefaultStorage& sp)
        { return sp.m_pointee; }

    friend inline StoredType& getImplRef(DefaultStorage& sp)
        { return sp.m_pointee; }

protected:
    void destroy(void)
        {
            //std::cerr << "DESTROY" << std::endl;
            if(this->m_pointee)
                delete this->m_pointee;
        }
        
    static StoredType Default(void)
        { return 0; }

    virtual ~DefaultStorage(void)
        { }

private:
    StoredType m_pointee;

    DefaultStorage& operator=(const DefaultStorage&);
};





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template<class T>
class RefCounted
{
public:
    RefCounted(void) : m_count(new unsigned int(1))
        {
            assert(this->m_count);
        }

    RefCounted(const RefCounted& orig) : m_count(orig.m_count)
        {}

    /*
    // MWCW lacks template friends, hence the following kludge
    template <typename P1>
    RefCounted(const RefCounted<P1>& rhs) 
    : pCount_(reinterpret_cast<const RefCounted&>(rhs).pCount_)
    {}
    */

    T clone(const T& v)
        {
            ++*this->m_count;
            return v;
        }

    bool release(const T&)
        {
            if(! --*this->m_count)
            {
                delete this->m_count;
                return true;
            }
            return false;
        }

    void swap(RefCounted& orig)
        { std::swap(this->m_count, orig.m_count); }


    enum { destructiveCopy = false };

protected:
    virtual ~RefCounted(void)
        { }

private:
    unsigned int *m_count;

    RefCounted& operator=(const RefCounted&);

};


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template<class T>
class NoCopy
{
public:
    NoCopy(void)
        {}

    template<class U>
    NoCopy(const NoCopy<U>&)
        { }

    static T clone(const T&)
        {
            throw std::runtime_error("NoCopy Smartptr");
        }
    
    static bool release(const T&)
        { return true; }


    static void swap(NoCopy&)
        { }

    enum { destructiveCopy = false };

protected:
    virtual ~NoCopy(void)
        { }
};



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class AllowConversion
{
public:
    enum { allow = true };

    void swap(AllowConversion&)
        {}

protected:
    virtual ~AllowConversion(void)
        { }
};


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
class DisallowConversion
{
public:
    DisallowConversion(void)
        {}

    DisallowConversion(const AllowConversion&)
        {}

    enum { allow = false };

    void swap(DisallowConversion&)
        {}

protected:
    virtual ~DisallowConversion(void)
        { }
};


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template<class T>
struct NoCheck
{
    NoCheck()
        {}
    
    template<class U>
    NoCheck(const NoCheck<U>&)
        {}
    
    static void onDefault(const T&)
        {}
    
    static void onInit(const T&)
        {}
    
    static void onDereference(const T&)
        {}
    
    static void swap(NoCheck&)
        {}

protected:
    virtual ~NoCheck(void)
        { }
};



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template<class T>
struct AssertCheck
{
    AssertCheck()
        {}
    
    template<class U>
    AssertCheck(const AssertCheck<U>&)
        {}
    
    template<class U>
    AssertCheck(const NoCheck<U>&)
        {}

    static void onDefault(const T&)
        {}
    
    static void onInit(const T&)
        {}
    
    static void onDereference(const T& v)
        { 
            assert(v);
            (void)v;
        }
    
    static void swap(AssertCheck&)
        {}

protected:
    virtual ~AssertCheck(void)
        { }
};



//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template <class T>
class ByRef
{
public:
    ByRef(T& v) : value_(v) {}
    operator T&() { return value_; }
    // gcc doesn't like this:
    // operator const T&() const { return value_; }
private:
    ByRef& operator=(const ByRef &);
    T& value_;
};






//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
/**
 * @
 */
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
template<typename T,
         template<class> class OwnershipPolicy = RefCounted,
         class ConversionPolicy = DisallowConversion,
         template<class> class CheckingPolicy = AssertCheck,
         template<class> class StoragePolicy = DefaultStorage
         >
class SmartPtr : public StoragePolicy<T>,
                 public OwnershipPolicy<typename StoragePolicy<T>::PointerType>,
                 public CheckingPolicy<typename StoragePolicy<T>::StoredType>,
                 public ConversionPolicy
{
    typedef StoragePolicy<T> SP;
    typedef OwnershipPolicy<typename StoragePolicy<T>::PointerType> OP;
    typedef CheckingPolicy<typename StoragePolicy<T>::StoredType> KP;
    typedef ConversionPolicy CP;

    
public:
    typedef typename SP::PointerType PointerType;
    typedef typename SP::StoredType StoredType;
    typedef typename SP::ReferenceType ReferenceType;

    typedef typename Select<OP::destructiveCopy, SmartPtr, const SmartPtr>::Result CopyArg;
    

private:
    struct NeverMatched;
  
    typedef typename Select< CP::allow, const StoredType&, NeverMatched>::Result ImplicitArg;
    typedef typename Select<!CP::allow, const StoredType&, NeverMatched>::Result ExplicitArg;
    

public:
    SmartPtr(void)
        { KP::onDefault(getImpl(*this)); }

    explicit SmartPtr(ExplicitArg p) : SP(p)
        { KP::onDefault(getImpl(*this)); }

    SmartPtr(ImplicitArg p) : SP(p)
        { KP::onDefault(getImpl(*this)); }

    SmartPtr(CopyArg& orig)
        : SP(orig), OP(orig), KP(orig), CP(orig)
        { getImplRef(*this) = OP::clone(getImplRef(orig)); }


    // const version
    template<typename T1,
             template<class> class OP1,
             class CP1,
             template<class> class KP1,
             template<class> class SP1
             >
    SmartPtr(const SmartPtr<T1, OP1, CP1, KP1, SP1>& orig)
        : SP(orig), OP(orig), KP(orig), CP(orig)
             { getImplRef(*this) = OP::clone(getImplRef(orig)); }
    
// non-const version
template<typename T1,
         template<class> class OP1,
         class CP1,
         template<class> class KP1,
         template<class> class SP1
         >
SmartPtr(SmartPtr<T1, OP1, CP1, KP1, SP1>& orig)
    : SP(orig), OP(orig), KP(orig), CP(orig)
        { getImplRef(*this) = OP::clone(getImplRef(orig)); }
    
    
    SmartPtr(ByRef<SmartPtr> orig)
        : SP(orig), OP(orig), KP(orig), CP(orig)
        {}
    
    operator ByRef<SmartPtr>(void)
        { return ByRef<SmartPtr>(*this); }
    
    SmartPtr& operator=(CopyArg& orig)
        {
            SmartPtr tmp(orig);
            tmp.swap(*this);
            return *this;
        }
    
    // const version
    template<typename T1,
             template<class> class OP1,
             class CP1,
             template<class> class KP1,
             template<class> class SP1
             >
    SmartPtr& operator=(const SmartPtr<T1, OP1, CP1, KP1, SP1>& orig)
             {
                 SmartPtr tmp(orig);
                 tmp.swap(*this);
                 return *this;            
             }

// non-const version
template<typename T1,
         template<class> class OP1,
         class CP1,
         template<class> class KP1,
         template<class> class SP1
         >
SmartPtr& operator=(SmartPtr<T1, OP1, CP1, KP1, SP1>& orig)
        {
            SmartPtr tmp(orig);
            tmp.swap(*this);
            return *this;            
        }
  

    void swap(SmartPtr& orig)
        {
            OP::swap(orig);
            CP::swap(orig);
            KP::swap(orig);
            SP::swap(orig);
        }

    virtual ~SmartPtr(void)
        {
            if( OP::release(getImpl(*static_cast<SP*>(this))) )
                SP::destroy();
        }


    friend inline void release(SmartPtr& sp, typename SP::StoredType& p)
        {
            p = getImplRef(sp);
            getImplRef(sp) = SP::Default();
        }

    friend inline void reset(SmartPtr& sp, typename SP::StoredType p)
        { SmartPtr(p).swap(sp); }

    PointerType operator->(void)
        {
            KP::onDereference(getImplRef(*this));
            return SP::operator->();
        }

    PointerType operator->(void) const
        {
            KP::onDereference(getImplRef(*this));
            return SP::operator->();
        }

    ReferenceType operator*(void)
        {
            KP::onDereference(getImplRef(*this));
            return SP::operator*();
        }

    ReferenceType operator*(void) const
        {
            KP::onDereference(getImplRef(*this));
            return SP::operator*();
        }

    bool operator!(void) const
        { return getImpl(*this) == 0; }


    const PointerType get(void) const
        {
            return SP::operator->();
        }

    PointerType get(void)
        {
            return SP::operator->();
        }

	void reset(typename SP::StoredType value)
	{
		SP::reset(value);
	}


    // Ambiguity buster
    template<typename T1,
             template <class> class OP1,
             class CP1,
             template <class> class KP1,
             template <class> class SP1
             >
    bool operator==(const SmartPtr<T1, OP1, CP1, KP1, SP1>& orig) const
             { return getImpl(*this) == getImpl(orig); }
    
// Ambiguity buster
    template<typename T1,
             template <class> class OP1,
             class CP1,
             template <class> class KP1,
             template <class> class SP1
             >
    bool operator!=(const SmartPtr<T1, OP1, CP1, KP1, SP1>& orig) const
             { return !(*this == orig); }

// Ambiguity buster
template<typename T1,
         template <class> class OP1,
         class CP1,
         template <class> class KP1,
         template <class> class SP1
         >
bool operator<(const SmartPtr<T1, OP1, CP1, KP1, SP1>& orig) const
        { return getImpl(*this) < getImpl(orig); }

};









template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator==(const SmartPtr<T, OP, CP, KP, SP>& lhs,
                       U* rhs)
{ return getImpl(lhs) == rhs; }
    

template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator==(U* lhs,
                       const SmartPtr<T, OP, CP, KP, SP>& rhs)
{ return rhs == lhs; }


template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator!=(const SmartPtr<T, OP, CP, KP, SP>& lhs,
                       U* rhs)
{ return !(lhs == rhs); }
    

template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator!=(U* lhs,
                       const SmartPtr<T, OP, CP, KP, SP>& rhs)
{ return rhs != lhs; }


template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator<(const SmartPtr<T, OP, CP, KP, SP>& lhs,
                      U* rhs);
    
template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator<(U* lhs,
                      const SmartPtr<T, OP, CP, KP, SP>& rhs);


template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator>(const SmartPtr<T, OP, CP, KP, SP>& lhs,
                      U* rhs)
{ return rhs < lhs; }

template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator>(U* lhs,
                      const SmartPtr<T, OP, CP, KP, SP>& rhs)
{ return rhs < lhs; }


template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator<=(const SmartPtr<T, OP, CP, KP, SP>& lhs,
                       U* rhs)
{ return !(rhs < lhs); }


template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator<=(U* lhs,
                       const SmartPtr<T, OP, CP, KP, SP>& rhs)
{ return !(rhs < lhs); }
    

template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator>=(const SmartPtr<T, OP, CP, KP, SP>& lhs,
                       U* rhs)
{ return !(lhs < rhs); }


template<typename T,
         template <class> class OP,
         class CP,
         template <class> class KP,
         template <class> class SP,
         typename U
         >
inline bool operator>=(U* lhs,
                       const SmartPtr<T, OP, CP, KP, SP>& rhs)
{ return !(lhs < rhs); }

}} // namespace end
       


namespace std
{
    template<typename T,
             template <class> class OP,
             class CP,
             template <class> class KP,
             template <class> class SP
             >
    struct less<informave::util::SmartPtr<T, OP, CP, KP, SP> >
        : public binary_function<informave::util::SmartPtr<T, OP, CP, KP, SP>,
                                 informave::util::SmartPtr<T, OP, CP, KP, SP>, bool>
    {
        bool operator()(const informave::util::SmartPtr<T, OP, CP, KP, SP>& lhs,
                        const informave::util::SmartPtr<T, OP, CP, KP, SP>& rhs) const
            { return less<T*>()(getImpl(lhs), getImpl(rhs)); }
    };
}


#endif
