//
// variant.hh - Variant class
//
// Copyright (C)         informave.org
//   2010,               Daniel Vogelbacher <daniel@vogelbacher.name>
//
// BSD License
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
// Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the
// distribution.
//
// Neither the name of the copyright holders nor the names of its
// contributors may be used to endorse or promote products derived
// from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

/// @file
/// @brief DAL Variant class
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_VARIANT_HH
#define INFORMAVE_DB_DAL_VARIANT_HH

#include "dal_fwd.hh"
#include "dbwtl/ustring.hh"
#include "dbwtl/ustreambuf.hh"
#include "dbwtl/util/smartptr.hh"

#include <memory>
#include <type_traits>


DAL_NAMESPACE_BEGIN

template<typename T> struct variant_traits;
template<typename T> struct variant_assign;
template<typename T> struct variant_deepcopy;


///
/// @brief SFINAE based method to check if a type is defined
#define DAL_DECLARE_HAS_TYPE(type)                                      \
    template<typename T>                                                \
    struct has_##type                                                   \
    {                                                                   \
        typedef char yes[1];                                            \
        typedef char no[2];                                             \
                                                                        \
        template<typename U>                                            \
            static yes& test(typename U::type*);                        \
                                                                        \
        template<typename>                                              \
            static no& test(...);                                       \
                                                                        \
        static const bool value = sizeof(test<T>(0)) == sizeof(yes);    \
                                                                        \
    }                                                                  




#define DAL_VARIANT_ACCESSOR          typedef void __accessor_is_defined

DAL_DECLARE_HAS_TYPE(__accessor_is_defined);


#ifndef DBWTL_CXX98_COMPATIBILITY
// Using the SFINAE based method to check if an accessor is defined.
#define DAL_VARIANT_ACCESSOR_CHECK                                      \
    static_assert(has___accessor_is_defined<typename variant_traits<T>::accessor_type>::value, \
                  "No accessor defined for this type.")
#else
// Fallback for C++98
#define DAL_VARIANT_ACCESSOR_CHECK                                      \
    typedef typename variant_traits<T>::accessor_type::__accessor_is_defined __compiler_check
#endif






//--------------------------------------------------------------------------
/// @brief Get the DAL type for a specific type
template<typename T>
struct type_id
{
    static inline daltype_t value(void) { return DAL_TYPE_UNKNOWN; }
};

template<> struct type_id<signed int>       { static inline daltype_t value(void) { return DAL_TYPE_INT; } };
template<> struct type_id<unsigned int>     { static inline daltype_t value(void) { return DAL_TYPE_UINT; } };
template<> struct type_id<signed char>      { static inline daltype_t value(void) { return DAL_TYPE_CHAR; } };
template<> struct type_id<unsigned char>    { static inline daltype_t value(void) { return DAL_TYPE_UCHAR; } };
template<> struct type_id<String>           { static inline daltype_t value(void) { return DAL_TYPE_STRING; } };
template<> struct type_id<bool>             { static inline daltype_t value(void) { return DAL_TYPE_BOOL; } };
template<> struct type_id<signed short>     { static inline daltype_t value(void) { return DAL_TYPE_SMALLINT; } };
template<> struct type_id<unsigned short>   { static inline daltype_t value(void) { return DAL_TYPE_USMALLINT; } };
template<> struct type_id<signed long long> { static inline daltype_t value(void) { return DAL_TYPE_BIGINT; } };
template<> struct type_id<unsigned long>    { static inline daltype_t value(void) { return DAL_TYPE_UBIGINT; } };
template<> struct type_id<ByteStreamBuf>    { static inline daltype_t value(void) { return DAL_TYPE_BLOB; } };
template<> struct type_id<UnicodeStreamBuf> { static inline daltype_t value(void) { return DAL_TYPE_MEMO; } };
template<> struct type_id<TNumeric>         { static inline daltype_t value(void) { return DAL_TYPE_NUMERIC; } };
template<> struct type_id<float>            { static inline daltype_t value(void) { return DAL_TYPE_FLOAT; } };
template<> struct type_id<double>           { static inline daltype_t value(void) { return DAL_TYPE_DOUBLE; } };
template<> struct type_id<TDate>            { static inline daltype_t value(void) { return DAL_TYPE_DATE; } };
template<> struct type_id<TTime>            { static inline daltype_t value(void) { return DAL_TYPE_TIME; } };
template<> struct type_id<TTimestamp>       { static inline daltype_t value(void) { return DAL_TYPE_TIMESTAMP; } };
template<> struct type_id<TInterval>        { static inline daltype_t value(void) { return DAL_TYPE_INTERVAL; } };





//--------------------------------------------------------------------------
/// @brief DAL Interface for variant types
class DBWTL_EXPORT IVariant : public IDALObject
{
public:
    virtual ~IVariant(void) { }

    //  basic functions
    virtual daltype_t   datatype(void) const = 0;
    virtual bool        isnull(void) const = 0;
    virtual void        setNull(bool mode = true) = 0;

    virtual void assign(const Variant& v) = 0;

    operator bool                  (void) const;
    operator signed int            (void) const;
    operator unsigned int          (void) const;
    operator signed char           (void) const;
    operator unsigned char         (void) const;
    operator signed short          (void) const;
    operator unsigned short        (void) const;
    operator signed long long      (void) const;
    operator unsigned long long    (void) const;
    operator float                 (void) const;
    operator double                (void) const;
    operator String                (void) const;
    operator std::string           (void) const;
    operator ByteStreamBuf*        (void) const;
    operator UnicodeStreamBuf*     (void) const;


    // This classes should provide a constructor for IVariant&
    // operator TNumeric              (void) const;
    // operator TDate                 (void) const;
    // operator TTime                 (void) const;
    // operator TTimestamp            (void) const;
    // operator TInterval             (void) const;


    // getter methods
    virtual signed int           asInt(void) const = 0;
    virtual unsigned int         asUInt(void) const = 0;
    virtual signed char          asChar(void) const = 0;
    virtual unsigned char        asUChar(void) const = 0;
    virtual String               asStr(std::locale loc = std::locale()) const = 0;
    virtual bool                 asBool(void) const = 0;
    virtual signed short         asSmallint(void) const = 0;
    virtual unsigned short       asUSmallint(void) const = 0;
    virtual signed long long     asBigint(void) const = 0;
    virtual unsigned long long   asUBigint(void) const = 0;
    virtual TNumeric             asNumeric(void) const = 0;
    virtual float                asReal(void) const = 0;
    virtual double               asDouble(void) const = 0;
    virtual TDate                asDate(void) const = 0;
    virtual TTime                asTime(void) const = 0;
    virtual TTimestamp           asTimestamp(void) const = 0;
    virtual TInterval            asInterval(void) const = 0;
    virtual ByteStreamBuf*       asBlob(void) const = 0;
    virtual UnicodeStreamBuf*    asMemo(void) const = 0;
//     virtual const TCustomType&   asCustom(void) const = 0;

  
private:
    IVariant&               operator=(const IVariant& o);
};



DBWTL_EXPORT std::wostream&  operator<<(std::wostream& o, const IVariant &var);
DBWTL_EXPORT std::ostream&   operator<<(std::ostream& o,  const IVariant &var);







//--------------------------------------------------------------------------
///
/// @brief Internal stored variants needs a clone method
class DBWTL_EXPORT IStoredVariant : public IVariant
{
public:
    virtual IStoredVariant* clone(void) const = 0;

    virtual bool has_local_storage(void) const = 0;

    virtual void releasePointee(void) = 0;

    virtual IStoredVariant* new_deepcopy(void) const = 0;
};


//--------------------------------------------------------------------------
///
/// @cond DEV_DOCS
///
/// @brief Default implementation for variants
///
/// Each method throws an exception.
class DBWTL_EXPORT DefaultVariant : public IStoredVariant
{
public:

    DefaultVariant(void) : m_isnull(true)
    {}

    virtual ~DefaultVariant(void)
    {}

    virtual bool            isnull(void) const;
    virtual void            setNull(bool mode = true);

    virtual signed int            asInt(void) const;
    virtual unsigned int          asUInt(void) const;
    virtual signed char           asChar(void) const;
    virtual unsigned char         asUChar(void) const;
    virtual String                asStr(std::locale loc = std::locale()) const;
    virtual bool                  asBool(void) const;
    virtual signed short          asSmallint(void) const;
    virtual unsigned short        asUSmallint(void) const;
    virtual signed long long      asBigint(void) const;
    virtual unsigned long long    asUBigint(void) const;
    virtual TNumeric              asNumeric(void) const;
    virtual float                 asReal(void) const;
    virtual double                asDouble(void) const;
    virtual TDate                 asDate(void) const;
    virtual TTime                 asTime(void) const;
    virtual TTimestamp            asTimestamp(void) const;
    virtual TInterval             asInterval(void) const;
    virtual ByteStreamBuf*        asBlob(void) const;
    virtual UnicodeStreamBuf*     asMemo(void) const;
    //virtual TCustom&        asCustom(void) const = 0;


protected:
    /// This method is provided for derived variants for simple NULL handling.
    /// It should be called after a new value is set with set*() methods.
    virtual void hasValue(void)
    {
        this->m_isnull = false;
    }

    bool m_isnull;
};



//--------------------------------------------------------------------------
/// Defines the interface for all accessors which can be used
/// to get the stored variable. 
///
/// @brief Storage Accessor Base
template<typename T>
struct sa_base : public variant_traits<T>::base_interface
{
    virtual const T& getValue(void) const = 0;
};



//--------------------------------------------------------------------------
///
/// @brief Default accessor
template<typename T>
struct default_accessor : public sa_base<T>
{
};

//--------------------------------------------------------------------------
///
/// @brief Read accessor
template<typename T>
struct read_accessor : public default_accessor<T>
{
};



//--------------------------------------------------------------------------
///
/// @brief Variant traits
template<typename T>
struct variant_traits
{
    typedef typename std::remove_const<typename std::remove_pointer<T>::type>::type  object_type;
    typedef typename std::remove_const<typename std::remove_pointer<T>::type>::type&  object_ref_type;
    typedef typename std::remove_const<typename std::remove_pointer<T>::type>::type*  object_ptr_type;

    typedef const typename std::remove_pointer<T>::type&  const_object_ref_type;
    typedef const typename std::remove_pointer<T>::type*  const_object_ptr_type;

    typedef read_accessor<object_type> accessor_type;

    typedef DefaultVariant base_interface;
};


//--------------------------------------------------------------------------
///
/// @brief Variant storage
template<typename T>
struct variant_storage
{
    enum { local_storage = true };

    typename variant_traits<T>::const_object_ref_type get_object_ref(const T &val) const
    {
        return val;
    }
    
    void release_pointee(T &ref)
    {}
};

//--------------------------------------------------------------------------
///
/// @brief Variant storage specialized for pointers
template<typename T>
struct variant_storage<T*>
{
    enum { local_storage = false };

    typedef T* __ptr_type;

    typename variant_traits<T>::const_object_ref_type get_object_ref(const __ptr_type &val) const
    {
        return *val;
    }

    void release_pointee(__ptr_type &ref)
    {
        delete ref;
    }
};



//--------------------------------------------------------------------------
///
/// @brief Variant value
template<typename T,
         template<typename> class StoragePolicy  = variant_storage,
         template<typename> class AssignPolicy   = variant_assign,
         template<typename> class DeepcopyPolicy = variant_deepcopy>
class variant_value : public variant_traits<T>::accessor_type,
                      protected StoragePolicy<T>,
                      protected AssignPolicy<T>,
                      protected DeepcopyPolicy<T>
{
public:
    DAL_VARIANT_ACCESSOR_CHECK;

    typedef typename variant_traits<T>::object_type           object_type;
    typedef typename variant_traits<T>::object_ref_type       object_ref_type;
    typedef typename variant_traits<T>::const_object_ref_type const_object_ref_type;


    explicit variant_value(const T &v) : m_value(v)
    {
        this->hasValue();
    }

    explicit variant_value(const variant_value<T> &v) : m_value(v.m_value)
    {
        this->setNull(v.isnull());
    }


    virtual bool has_local_storage(void) const { return StoragePolicy<T>::local_storage; }


    // only called if variant_value* cant replaced with the new variant_value (e.g. ptr)
    virtual void assign(const Variant& v)
    {
        AssignPolicy<T>::set_new_value(m_value, v);
    }

    virtual IStoredVariant* clone(void) const
    {
        return new variant_value<T, StoragePolicy, AssignPolicy, DeepcopyPolicy>(*this);
    }


    // creates a new stored variant with has its own value, even if source was a pointer
    virtual IStoredVariant* new_deepcopy(void) const
    {
        return DeepcopyPolicy<T>::create_deepcopy(m_value, this);
    }


    virtual const_object_ref_type getValue(void) const
    { 
        return StoragePolicy<T>::get_object_ref(this->m_value);
    }

    virtual T& getRawValue(void)
    { 
        return this->m_value;
    }


    virtual const T& getRawValue(void) const
    { 
        return this->m_value;
    }


    virtual void releasePointee(void)
    {
        StoragePolicy<T>::release_pointee(m_value);
    }


private:
    T m_value;
};




//--------------------------------------------------------------------------
///
/// @brief Deepcopy for T
template<typename T>
struct variant_deepcopy
{
    IStoredVariant* create_deepcopy(const T& ref, const IStoredVariant *var) const
    {
        return new variant_value<typename variant_traits<T>::object_type>(ref);
    }
};



//--------------------------------------------------------------------------
///
/// @brief Deepcopy for T*
template<typename T>
struct variant_deepcopy<T*>
{
    typedef T* ptr;
    IStoredVariant* create_deepcopy(const ptr & ref, const IStoredVariant *var) const
    {
        return new variant_value<typename variant_traits<T>::object_type>(*ref);
    }
};




//--------------------------------------------------------------------------
///
/// @brief Deepcopy for UnicodeStreamBuf*
template<>
struct variant_deepcopy<UnicodeStreamBuf*>
{
    typedef UnicodeStreamBuf* ptr;
    IStoredVariant* create_deepcopy(const ptr & ref, const IStoredVariant *var) const
    {
        throw std::runtime_error("unicodestreambuf does not deep copy");
        //return new variant_value<typename variant_traits<T>::object_type>(*ref);
    }
};



//--------------------------------------------------------------------------
///
/// @brief Deepcopy for ByteStreamBuf*
template<>
struct variant_deepcopy<ByteStreamBuf*>
{
    typedef ByteStreamBuf* ptr;
    IStoredVariant* create_deepcopy(const ptr & ref, const IStoredVariant *var) const
    {
        throw std::runtime_error("bytetreambuf does not deep copy");
        //return new variant_value<typename variant_traits<T>::object_type>(*ref);
    }
};



//--------------------------------------------------------------------------
///
/// @brief Deepcopy for IVariant*
template<>
struct variant_deepcopy<IVariant*>
{
    typedef IVariant* ptr;
    IStoredVariant* create_deepcopy(const ptr & ref, const IStoredVariant *var) const
    {
        try
        {
            return dynamic_cast<const IStoredVariant&>(*ref).new_deepcopy();
        }
        catch(std::bad_cast&)
        {
            throw;
        }
        //return new variant_value<typename variant_traits<T>::object_type>(*ref);
    }
};



//--------------------------------------------------------------------------
///
/// @brief Deepcopy for const IVariant*
template<>
struct variant_deepcopy<const IVariant*>
{
    typedef const IVariant* ptr;
    IStoredVariant* create_deepcopy(const ptr & ref, const IStoredVariant *var) const
    {
        try
        {
            return dynamic_cast<const IStoredVariant&>(*ref).new_deepcopy();
        }
        catch(std::bad_cast&)
        {
            throw;
        }
        //return new variant_value<typename variant_traits<T>::object_type>(*ref);
    }
};
/// @endcond



//--------------------------------------------------------------------------
///
/// @brief Converter template
template<typename T>
struct Converter
{
    static_assert(sizeof(T) == -1, "There is no Converter defined for this type.");

    T operator()(const IStoredVariant &sv); // not defined
};




//------------------------------------------------------------------------------
/// A class to represent a variant value and converting the value to another
/// type. The conversion is done by the storage_accessor<> template.
///
/// @note For each type that Variant should store, a specialization for the
/// storage_accessor<> template must be defined.
///
/// @since 0.0.1
/// @brief Represent a variant value
class DBWTL_EXPORT  Variant : public IVariant
{
public:

    Variant(void) : m_storage()
    {}


    Variant(const Variant &var)
        : m_storage(),
        m_name(var.m_name)
        {
            if(! var.isnull())
                m_storage.reset(var.deepcopy());
        }

    template<typename T>
        Variant(const T& v, const String &name) : m_storage(new variant_value<T>(v))
    {}


    Variant(const EngineVariant& v);


    template<typename T>
        Variant(const T& v, typename std::enable_if<!std::is_base_of<Variant, T>::value, void*>::type = 0) 
        : m_storage(new variant_value<T>(v))
    {}



    explicit Variant(daltype_t type,
                     const String &name = String(L"<unnamed>"))
        : IVariant(),
        m_storage(),
        m_name(name)
        {}



    // used for Variant x(colval.deepcopy()); or x(colval.clone());
    explicit Variant(IStoredVariant *var) : m_storage(var)
    {}
    

    virtual const String& getName(void) const;
    

    IStoredVariant* deepcopy(void) const
    {
        if(this->isnull())
            return 0;
        else
            return this->getStorageImpl()->new_deepcopy();
    }


    IStoredVariant* clone(void) const
    {
        if(this->isnull())
            return 0;
        else
            return this->getStorageImpl()->clone();
    }


    template<typename T>
        operator T(void) const
    {
        return this->get<T>();
    }


    virtual ~Variant(void)
    {}


    Variant& operator=(const Variant& var)
        {
            this->assign(var);
            return *this;
        }
    

    virtual void assign(const Variant& var)
    {
        if(var.isnull())
        {
            this->m_storage.reset(0);
        }
        else
        {
            if(this->m_storage.get() && ! this->getStorageImpl()->has_local_storage())
            {
                this->getStoredVar()->assign(var);
            }
            else
            {
                m_storage.reset(var.getStorageImpl()->new_deepcopy()); 
                }                
        }
    }


    template<typename T>
        inline void set(const T &v)
    {
        if(this->m_storage.get() && ! this->getStorageImpl()->has_local_storage())
        {
            Variant va(v);
            this->getStoredVar()->assign(va);
        }
        else
        {
            this->m_storage.reset(new variant_value<T>(v));
        }
    }


    template<typename T>
        typename Converter<T>::type get(void) const
    {
        return Converter<T>()(*this->getStoredVar());
    }



    /// @brief Check the value object for NULL
    virtual bool isnull(void) const;


    /// @brief Set the value to NULL
    virtual void        setNull(bool mode = true);


    /// If the value is set to NULL, the returned type
    /// may be DAL_TYPE_UNKNOW.
    /// @brief Returns the datatype of the value.
    virtual daltype_t datatype(void) const;


    /// Returns the value as signed int
    /// @exception convert_error if the value can't converted
    /// @exception null_value if the value is null
    virtual signed int asInt(void) const;


    /// Returns the value as unsigned int
    /// @exception convert_error if the value can't converted
    /// @exception null_value if the value is null
    virtual unsigned int asUInt(void) const;


    /// Returns the value as signed char
    /// @exception convert_error if the value can't converted
    /// @exception null_value if the value is null
    virtual signed char asChar(void) const;


    /// Returns the value as unsigned char
    /// @exception convert_error if the value can't converted
    /// @exception null_value if the value is null
    virtual unsigned char  asUChar(void) const;


    virtual String              asStr(std::locale loc = std::locale()) const;
    virtual bool                asBool(void) const;
    virtual signed short        asSmallint(void) const;
    virtual unsigned short      asUSmallint(void) const;
    virtual signed long long    asBigint(void) const;
    virtual unsigned long long  asUBigint(void) const;
    virtual TNumeric            asNumeric(void) const;
    virtual float               asReal(void) const;
    virtual double              asDouble(void) const;
    virtual TDate               asDate(void) const;
    virtual TTime               asTime(void) const;
    virtual TTimestamp          asTimestamp(void) const;
    virtual TInterval           asInterval(void) const;
    virtual ByteStreamBuf*      asBlob(void) const;
    virtual UnicodeStreamBuf*   asMemo(void) const;
    //virtual TCustom&        asCustom(void) const = 0;


    IStoredVariant*       getStorageImpl(void);
    const IStoredVariant* getStorageImpl(void) const;

    IStoredVariant*       getStoredVar(void);
    const IStoredVariant* getStoredVar(void) const;


protected:
    std::auto_ptr<IStoredVariant> m_storage;

    /// @brief Name of the variant
    String     m_name;
};



template<typename T>
struct variant_assign
{

    void set_new_value(T& dest, const Variant &src)
    {
        dest = src.get<typename variant_traits<T>::object_type>();
    }

};


template<typename T>
struct variant_assign<T*>
{
    void set_new_value(T*& dest, const Variant &src)
    {
        *dest = src.get<T>();
    }

};


void throw_read_only(void);

template<typename T>
struct variant_assign<const T*>
{
    void set_new_value(const T*& dest, const Variant &src)
    {
        throw_read_only();
    }

};


template<>
struct variant_assign<IVariant*>
{

    /// @todo Implement a testcase
    void set_new_value(IVariant*& dest, const Variant &src)
    {
        dest->assign(src);
    }
};




DAL_NAMESPACE_END

#endif

//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//