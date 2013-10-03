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

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/ustring.hh"
#include "dbwtl/ustreambuf.hh"
#include "dbwtl/util/smartptr.hh"

#include <memory>
#include <type_traits>
//#include <iostream>
#include <sstream>
#include <locale>


DB_NAMESPACE_BEGIN

class Variant;
template<typename T> struct value_traits;


#define VARIANT_PTR 0x01





struct DBWTL_EXPORT BinaryOperatorVariant
{
    virtual Variant binary_xor           (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_or            (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_and           (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_mod           (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_mul           (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_div           (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_add           (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_concat        (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_sub           (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_less          (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_less_equal    (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_equal         (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_not_equal     (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_greater       (const Variant &v0, const Variant &v1) const = 0;
    virtual Variant binary_greater_equal (const Variant &v0, const Variant &v1) const = 0;


    typedef Variant (BinaryOperatorVariant::*memfun_type) (const Variant &, const Variant &) const;
};




Variant DBWTL_EXPORT apply_binary_method(const Variant &op0, const Variant &op1, BinaryOperatorVariant::memfun_type method);



void DBWTL_EXPORT throw_read_only(void);

void DBWTL_EXPORT throw_convert_error(daltype_t src, daltype_t dest);


//--------------------------------------------------------------------------
/// @brief Get the DAL type for a specific type
template<typename T>
struct DBWTL_EXPORT type_id
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
template<> struct type_id<BlobStream>       { static inline daltype_t value(void) { return DAL_TYPE_BLOB; } };
template<> struct type_id<MemoStream>       { static inline daltype_t value(void) { return DAL_TYPE_MEMO; } };
template<> struct type_id<Blob>             { static inline daltype_t value(void) { return DAL_TYPE_BLOB; } };
template<> struct type_id<Memo>             { static inline daltype_t value(void) { return DAL_TYPE_MEMO; } };
template<> struct type_id<TNumeric>         { static inline daltype_t value(void) { return DAL_TYPE_NUMERIC; } };
template<> struct type_id<float>            { static inline daltype_t value(void) { return DAL_TYPE_FLOAT; } };
template<> struct type_id<double>           { static inline daltype_t value(void) { return DAL_TYPE_DOUBLE; } };
template<> struct type_id<TDate>            { static inline daltype_t value(void) { return DAL_TYPE_DATE; } };
template<> struct type_id<TTime>            { static inline daltype_t value(void) { return DAL_TYPE_TIME; } };
template<> struct type_id<TTimestamp>       { static inline daltype_t value(void) { return DAL_TYPE_TIMESTAMP; } };
template<> struct type_id<TInterval>        { static inline daltype_t value(void) { return DAL_TYPE_INTERVAL; } };




//..............................................................................
////////////////////////////////////////////////////////////////// IVariantValue
///
/// @since 0.0.1
/// @brief Base class for value stores in Variants
/// 
struct DBWTL_EXPORT IVariantValue
{
    virtual ~IVariantValue(void) {}


    virtual int flags(void) const = 0;
    virtual IVariantValue* deepcopy(void) const = 0;
    virtual IVariantValue* clone(void) const = 0;

    virtual const char* get_typename(void) const = 0;
    virtual daltype_t   datatype(void) const = 0;

    virtual void setNull(void) {}
    virtual bool isNull(void) const = 0;


    virtual void release_pointee(void) {}
};


//..............................................................................
//////////////////////////////////////////////////////////////// custom_deepcopy
///
/// @since 0.0.1
/// @brief Deepcopy interface for custom types
/// 
struct DBWTL_EXPORT custom_deepcopy
{
    virtual IVariantValue* do_deepcopy(const IVariantValue*) const = 0;
    virtual ~custom_deepcopy(void) {}
};


//..............................................................................
//////////////////////////////////////////////////////////////// variant_writter
///
/// @since 0.0.1
/// @brief Writter interface for custom types
/// 
struct DBWTL_EXPORT variant_writter
{
    virtual void set_value(const Variant &value) = 0;
    virtual ~variant_writter(void) {}
};


//..............................................................................
/////////////////////////////////////////////////////////////////////// var_info
///
/// @since 0.0.1
/// @brief Provides information about a type
/// 
template<typename T>
struct var_info
{
    static daltype_t type(void) { return DAL_TYPE_UNKNOWN; }
    static const char* name(void) { return "UNKNOWN"; }
    
};


//..............................................................................
/////////////////////////////////////////////////////////////////////// supports
///
/// @since 0.0.1
/// @brief Interface for a specific cast from a value store
/// 
template<typename T>
struct supports
{
    virtual T cast(T*, std::locale) const = 0;
    virtual ~supports(void) {}
};




/// @detail
/// Helper macro to declare a cast method
#define SV_CAST_METHOD(type) virtual type cast(type*, std::locale loc) const



//..............................................................................
//////////////////////////////////////////////////////////////////////// sa_base
///
/// @since 0.0.1
/// @brief Base class for storage accessors
/// 
template<typename T>
struct sa_base : public virtual IVariantValue,
                 public supports<T>
{
    virtual       T& get_value(void)       = 0;
    virtual const T& get_value(void) const = 0;

    virtual const char* get_typename(void) const;
    virtual daltype_t   datatype(void) const;

    virtual T cast(T*, std::locale) const { return this->get_value(); }

    virtual bool valueNullCheck(void) const { return false; }

    virtual void set_value(const Variant &value) { throw_read_only(); }
};


template<typename T> const char* sa_base<T>::get_typename(void) const { return value_traits<T>::info_type::name(); }


template<typename T> daltype_t sa_base<T>::datatype(void) const { return value_traits<T>::info_type::type(); }



//..............................................................................
////////////////////////////////////////////////////////////////// supports_cast
///
/// @since 0.0.1
/// @brief Interface for a supported implicit type cast from a value store
/// 
template<typename U, typename T>
struct supports_cast : public virtual sa_base<U>, /// @todo rename to supports_implicit_cast
                       public supports<T>
{
    virtual T cast(T*, std::locale) const
    {    	
    	return (T)this->get_value();
    }
};


void internal_helper__throw_ConvertException(daltype_t src, daltype_t dest, const String &str);


template<typename U, typename T>
void arithmetic_cast_error(const T &value)
{
    std::stringstream ss;

    ss << "The integral value " << value << " is incompatible to the "
       << "numeric limits of the destination type ("
	      << std::numeric_limits<U>::min() << " - " << std::numeric_limits<U>::max()
	      << ")";

    internal_helper__throw_ConvertException(type_id<T>::value(), type_id<U>::value(), ss.str());
}



//..............................................................................
////////////////////////////////////////////////////////////////// integral_cast
///
/// @since 0.0.1
/// @brief
/// 

template<typename U, typename T>
static inline typename std::enable_if<std::is_signed<U>::value && std::is_signed<T>::value, U>::type integral_cast(const T &value)
{
    if (value < std::numeric_limits<U>::min() || value > std::numeric_limits<U>::max()) 
    {
        arithmetic_cast_error<U, T>(value);
    }
    return static_cast<U>(value);
}

template<typename U, typename T>
static inline typename std::enable_if<std::is_signed<U>::value && std::is_unsigned<T>::value, U>::type integral_cast(const T &value)
{
    if (value > static_cast<typename std::make_unsigned<U>::type>(std::numeric_limits<U>::max()))
    {
        arithmetic_cast_error<U, T>(value);
    }
    return static_cast<U>(value);
}

template<typename U, typename T>
static inline typename std::enable_if<std::is_unsigned<U>::value && std::is_signed<T>::value, U>::type integral_cast(const T &value)
{
    if (value < 0 || static_cast<typename std::make_unsigned<T>::type>(value) > std::numeric_limits<U>::max())
    {
        arithmetic_cast_error<U, T>(value);
        throw std::out_of_range("out of range");
    }
    return static_cast<U>(value);
}

template<typename U, typename T>
static inline typename std::enable_if<std::is_unsigned<U>::value && std::is_unsigned<T>::value, U>::type integral_cast(const T &value) 
{
    if (value > std::numeric_limits<U>::max())
    {
        arithmetic_cast_error<U, T>(value);
    }
    return static_cast<U>(value);
}

//..............................................................................
//////////////////////////////////////////////////////////////// arithmetic_cast
///
/// @since 0.0.1
/// @brief 
/// 
template<typename U, typename T>
static inline typename std::enable_if<std::is_integral<U>::value && std::is_integral<T>::value, U>::type
arithmetic_cast(const T &value)
{
    return integral_cast<U, T>(value);
}


template<typename U, typename T>
static inline typename std::enable_if<std::is_floating_point<U>::value && std::is_integral<T>::value, U>::type
arithmetic_cast(const T &value)
{
    return value;
}


template<typename U, typename T>
static inline typename std::enable_if<std::is_integral<U>::value && std::is_floating_point<T>::value, U>::type
arithmetic_cast(const T &value)
{
    return value;
}


template<typename U, typename T>
static inline typename std::enable_if<std::is_floating_point<U>::value && std::is_floating_point<T>::value, U>::type
arithmetic_cast(const T &value)
{
    return value;
}


//..............................................................................
//////////////////////////////////////////////////// supports_integral_type_cast
///
/// @since 0.0.1
/// @brief
/// 
template<typename U, typename T>
struct supports_integral_type_cast : public virtual sa_base<U>,
                                     public supports<T>
{
    virtual T cast(T*, std::locale) const
    {
        return arithmetic_cast<U, T>(this->get_value());
    }
};




//..............................................................................
//////////////////////////////////////////////////////////////////// sv_accessor
///
/// @since 0.0.1
/// @brief Default accessor
/// 
template<typename T>
struct sv_accessor : public virtual sa_base<T>
{
    virtual bool valueNullCheck(void) const { return false; }
    static_assert(sizeof(T) == -1, "No accessor defined");
};




//..............................................................................
//////////////////////////////////////////////////////////////// deepcopy_helper
///
/// @since 0.0.1
/// @brief Helper class for custom deepcopy
///
struct deepcopy_helper
{
    static IVariantValue* try_copy(const IVariantValue *owner, const custom_deepcopy *x)
    { return x->do_deepcopy(owner); }

    static IVariantValue* try_copy(const IVariantValue *owner, const void*) { return 0; }
};



//..............................................................................
////////////////////////////////////////////////////////////////// variant_value
///
/// @since 0.0.1
/// @brief Storage class for values
/// 
template<typename T>
struct variant_value : public sv_accessor<T>
{
    //variant_value(const variant_value<T> &orig) : sv_accessor<T>(orig), m_value(orig.m_value)
    //{}

    variant_value(const T& value) : m_value(value) {}

    virtual T& get_value(void) { return this->m_value; }
    virtual const T& get_value(void) const { return this->m_value; }

    virtual int flags(void) const { return 0; }

    //virtual IVariantValue* deepcopy(void) const { return new typename value_traits<T>::stored_type(m_value); }

    virtual IVariantValue* deepcopy(void) const
    {
        if(IVariantValue *copy = deepcopy_helper::try_copy(this, &this->m_value))
            return copy;
        else
            return new typename value_traits<T>::stored_type(m_value);
    }



    //virtual IVariantValue* clone(void) const { return new variant_value<T>(m_value); }
    virtual IVariantValue* clone(void) const { return new variant_value<T>(*this); }

    virtual bool isNull(void) const { return this->valueNullCheck(); }

    T m_value;

private:
    variant_value(const variant_value& value) : sv_accessor<T>(value), m_value(value.m_value) {}
    variant_value& operator=(const variant_value&);
};



//..............................................................................
////////////////////////////////////////////////////////////////////// ptr_value
///
/// @since 0.0.1
/// @brief Storage class for non-const pointers
/// 
template<typename T>
struct ptr_value : public sv_accessor<T>, public variant_writter
{
    ptr_value(T* value) : m_value(value) {}

    virtual T& get_value(void) { return *this->m_value; }
    virtual const T& get_value(void) const { return *this->m_value; }

    virtual void set_value(const Variant &value);


    virtual int flags(void) const { return 0 | VARIANT_PTR; }

    virtual IVariantValue* deepcopy(void) const
    {
        if(IVariantValue *copy = deepcopy_helper::try_copy(this, this->m_value))
            return copy;
        else
            return new typename value_traits<T>::stored_type(*m_value);
    }

    virtual IVariantValue* clone(void) const { return new ptr_value<T>(m_value); }

    virtual void release_pointee(void)
    {
        delete m_value;
    }

    virtual bool isNull(void) const { return 0 == this->m_value || this->valueNullCheck(); }

    T *m_value;
private:
    ptr_value(const ptr_value&);
    ptr_value& operator=(const ptr_value&);
};


//..............................................................................
//////////////////////////////////////////////////////////////// const_ptr_value
///
/// @since 0.0.1
/// @brief Storage class for const pointers
/// 
template<typename T>
struct const_ptr_value : public sv_accessor<T>
{
    const_ptr_value(const T* value) : m_value(value) {}
    
    virtual T& get_value(void) { throw_read_only(); throw 0; }
    virtual const T& get_value(void) const { return *this->m_value; }
    
    virtual int flags(void) const { return 0 | VARIANT_PTR; }
    
    virtual IVariantValue* deepcopy(void) const
    {
        if(IVariantValue *copy = deepcopy_helper::try_copy(this, this->m_value))
            return copy;
        else
            return new typename value_traits<T>::stored_type(*m_value);
    }

    virtual IVariantValue* clone(void) const { return new const_ptr_value<T>(m_value); }
    
    virtual bool isNull(void) const { return 0 == this->m_value || this->valueNullCheck(); }

    const T *m_value;


private:
    const_ptr_value(const const_ptr_value&);
    const_ptr_value& operator=(const const_ptr_value&);
};


//..............................................................................
//////////////////////////////////////////////////////////////////// raw_pointer
///
/// @since 0.0.1
/// @brief Storage class for raw pointers
/// @detail
/// While get_value() for ptr_value returns a reference to the value, this
/// storage class returns the stored pointer itself.
template<typename T>
struct raw_pointer : public sv_accessor<T>
{
    raw_pointer(T value) : m_value(value) {}
    
    virtual T& get_value(void) { return this->m_value; }
    virtual const T& get_value(void) const { return this->m_value; }
    
    virtual int flags(void) const { return 0 | VARIANT_PTR; }
    
    virtual IVariantValue* deepcopy(void) const
    {
        if(IVariantValue *copy = deepcopy_helper::try_copy(this, this->m_value))
            return copy;
        else
            return new typename value_traits<T>::stored_type(m_value);
    }

    virtual IVariantValue* clone(void) const { return new raw_pointer<T>(m_value); }
    
    virtual bool isNull(void) const { return 0 == this->m_value || this->valueNullCheck(); }

    T m_value;


private:
    raw_pointer(const raw_pointer&);
    raw_pointer& operator=(const raw_pointer&);
};



///
/// @warning Do not use this from any sv_accessor method!
template<typename T>
inline T use_cast(const IVariantValue *v, std::locale loc = std::locale())
{
    if(const supports<T> *a = dynamic_cast<const supports<T>*>(v))
    {
        return a->cast((T*)0, loc);
    }
    else
        throw std::invalid_argument("use_cast<> was called with a argument which is not "
                                    "convertable to the given type T");
}



//..............................................................................
/////////////////////////////////////////////////////////////////// value_traits
///
/// @since 0.0.1
/// @brief Traits class for values
/// 
template<typename T>
struct value_traits
{
    typedef variant_value<T>    stored_type;
    typedef T                   value_type;
    typedef var_info<T>         info_type;
};


template<typename T>
struct value_traits<T*>
{
    typedef ptr_value<T>        stored_type;
    typedef T                   value_type;
    typedef var_info<T>         info_type;
};

template<typename T>
struct value_traits<const T*>
{
    typedef const_ptr_value<T>  stored_type;
    typedef T                   value_type;
    typedef var_info<T>         info_type;
};



//..............................................................................
////////////////////////////////////////////////////////////////////// Converter
///
/// @since 0.0.1
/// @brief Converter for variant values
/// 
template<typename T>
struct Converter
{
    //static_assert(sizeof(T) == -1, "Converter needs to be specialized.");
    static T convert(const Variant&);

    static bool can_convert(const Variant&);
};



//..............................................................................
//////////////////////////////////////////////////////////////////////// Variant
///
/// @since 0.0.1
/// @brief Variant type
/// 
class DBWTL_EXPORT Variant
{
public:

/////// WORKAROUND
/// @bug fix this, there is no variant for long int

    Variant(const signed long int &value);

    Variant(const unsigned long int &value);

/////// WORKAROUND END

    ///
    template<typename T>
        Variant(const T& value, const char *utf8name = 0)
        : m_storage(),
	 m_name_ptr(utf8name),
        m_type_ifnull(DAL_TYPE_UNKNOWN)
        {
            this->m_storage.reset(new typename value_traits<T>::stored_type(value));
        }

    ///
    Variant(const Variant& value)
        : m_storage(),
        m_name_ptr(value.m_name_ptr),
        m_type_ifnull(value.m_type_ifnull)
        {
            this->assign(value); // assign via deepcopy
        }

    
    ///
    Variant(void) 
        : m_storage(),
        m_name_ptr(0),
        m_type_ifnull(DAL_TYPE_UNKNOWN)
        {}
    
    ///
    Variant(IVariantValue *value) 
        : m_storage(value),
        m_name_ptr(0),
        m_type_ifnull(DAL_TYPE_UNKNOWN)
        {}



    ///
    explicit Variant(daltype_t type, const char *utf8name)
    : m_storage(),
     // m_name(String::fromUTF8(utf8name)),
     m_name_ptr(0),
      m_type_ifnull(type)
    {}

    ///
    /*
    explicit Variant(daltype_t type, const String &name)
        : m_storage(),
        m_name(name),
        m_type_ifnull(type)
        {}
   */

    ///
    virtual ~Variant(void) {}



    ///
    template<typename T> inline bool can_convert(void) const
    {
        if(const supports<T> *a = dynamic_cast<const supports<T>*>(this->get_storage()))
        {
            return true;
        }
        else
            return Converter<T>::can_convert(*this);
    }


    ///
    template<typename T> inline T get(std::locale loc = std::locale()) const
    {
        if(const supports<T> *a = dynamic_cast<const supports<T>*>(this->get_storage()))
        {
            return a->cast((T*)0, loc);
        }
        else
            return Converter<T>::convert(*this);
    }


    ///
    template<typename T> inline operator T(void) const
    {
        return this->get<T>();
    }



    ///
    template<typename T> inline void set(const T& value)
    {

        if((!this->isnull()) && this->get_storage()->flags() & VARIANT_PTR)
        {
            if(variant_writter *vw = dynamic_cast<variant_writter*>(this->get_storage()))
            {
                vw->set_value(Variant(value));
            }
            else
            {
                throw_read_only();
            }
        }
        else
        {
            this->m_storage.reset(new typename value_traits<T>::stored_type(value));
        }
    }


    ///
    inline Variant& operator=(const Variant& value)
        {
            this->assign(value);
            return *this;
        }



    const char* get_typename(void) const;

    void replace_storage(IVariantValue* value);

    virtual IVariantValue* clone(void) const;

    virtual daltype_t datatype(void) const;

    virtual bool isnull(void) const;

    virtual void assign(const Variant& value);

    virtual void setNull(void);

    inline String getName(void) const { return String::fromUTF8(this->m_name_ptr); }


    IVariantValue*       get_storage(void);
    const IVariantValue* get_storage(void) const;


    signed int          asInt(void) const;
    unsigned int        asUInt(void) const;
    signed char         asChar(void) const;
    unsigned char       asUChar(void) const;
    String              asStr(std::locale loc = std::locale()) const;
    bool                asBool(void) const;
    signed short        asSmallint(void) const;
    unsigned short      asUSmallint(void) const;
    signed long long    asBigint(void) const;
    unsigned long long  asUBigint(void) const;
    TNumeric            asNumeric(void) const;
    float               asReal(void) const;
    double              asDouble(void) const;
    TDate               asDate(void) const;
    TTime               asTime(void) const;
    TTimestamp          asTimestamp(void) const;
    TInterval           asInterval(void) const;
    BlobStream                asBlob(void) const;
    MemoStream                asMemo(void) const;


protected:
    std::auto_ptr<IVariantValue> m_storage;
    const char*    m_name_ptr;
    daltype_t m_type_ifnull;
};



DBWTL_EXPORT std::wostream&  operator<<(std::wostream& o, const Variant &var);
DBWTL_EXPORT std::ostream&   operator<<(std::ostream& o,  const Variant &var);





template<typename T>
void ptr_value<T>::set_value(const Variant &value)
{  
    *this->m_value = value.get<T>();
}


template<typename T>
T Converter<T>::convert(const Variant &v)
{
    throw_convert_error(v.datatype(), value_traits<T>::info_type::type());
    throw 0;
}

template<typename T>
bool Converter<T>::can_convert(const Variant &v)
{
    return false;
}




struct binary_operator
{
        virtual Variant add(const Variant &a, const Variant &b) = 0;
};


std::shared_ptr<binary_operator> handle(daltype_t a, daltype_t b);




struct VariantExprHelper
{
    static Variant binaryAdd(const Variant &a, const Variant &b);
    static Variant unaryPositive(const Variant &a);
};







class DBWTL_EXPORT format_error_exception : public std::runtime_error
{
public:
    format_error_exception(const String &fmt, const String &what)
        : std::runtime_error(String("Formatting exception: ") + what + " format() string: " + fmt)
    {}
};







//--------------------------------------------------------------------------
/// @brief Format String Parser
///
/// @details
/// Foo
template<typename Ch, typename Tr = std::char_traits<Ch>, typename Alloc = std::allocator<Ch> >
class DBWTL_EXPORT fmt_parser
{
public:
    /// Formatting settings
    struct Settings
    {
        Settings(void) : mode(T_NONE), alternateForm(false), zeroPadded(false), leftAdjust(false),
                         blank(false), sign(false), width(0), prec(0)
        {}

        enum { T_NONE, T_CHAR, T_SMALL, T_LONG, T_LONGLONG, T_LONGDOUBLE } mode;

        bool alternateForm,
            zeroPadded,
            leftAdjust,
            blank,
            sign;
        int width, prec;
    };


    typedef std::basic_string<Ch, Tr, Alloc>        string_type;
    typedef Ch                                      char_type;
    typedef Tr                                      traits_type;
    typedef std::basic_stringstream<Ch, Tr, Alloc>  stream_type;


    /// Constructor
    fmt_parser(const std::vector<Variant>& args, const string_type &str, std::locale loc)
        : m_fmt(str), m_loc(loc), m_str(), m_pos(), m_char(), 
        m_in(str.begin()), m_end(str.end()), m_args(args)
    {}


    /// Consume next character
    /// Returns false on EOF, otherwise true
    bool consume(void)
    {
        if(this->m_in != m_end)
        {
            this->m_char = *this->m_in++;
            return true;
        }
        this->m_char = traits_type::eof();
        return false;
    }


    /// Consume next character and return it
    inline char_type getnc(void)
    {
        consume();
        return this->m_char;
    }


    /// Get argument
    inline const Variant& get_arg(std::vector<Variant>::size_type idx)
    {
        if(idx > m_args.size()-1)
            throw format_error_exception(this->m_fmt, "Index out of range.");
        return m_args.at(idx);
    }


    /// Do formatting
    void do_fmt(void)
    {
        Settings settings;

        // % err
        enum { ST_FLAG, ST_CMD } state = ST_FLAG;

        char_type c = this->m_char;

        for(c = this->m_char; c != traits_type::eof(); c = getnc())
        {
            switch(c)
            {
            case '#': settings.alternateForm = true; continue;
            case '0': settings.zeroPadded = true; continue;
            case '-': settings.leftAdjust = true; continue;
            case ' ': settings.blank = true; continue;
            case '+': settings.sign = true; continue;
                //case ''':
            default:
                break;
            }
            break;
        }
        
        string_type tmp;
        // check for *
        for(c = this->m_char; c != traits_type::eof() && isdigit(c); c = getnc())
        {
            tmp.push_back(c);
        }
        if(!tmp.empty())
        {
            settings.width = Variant(String(tmp)).asInt();
        }
        else if(c == '*')
        {
            settings.width = this->get_arg(m_pos++).asInt();
            c = getnc();
        }

        if(c == '.')
        {
            tmp.clear();
            consume();
            for(c = this->m_char; c != traits_type::eof() && isdigit(c); c = getnc())
            {
                tmp.push_back(c);
            }
            if(!tmp.empty())
            {
                settings.prec = Variant(String(tmp)).asInt();
            }
            else if(c == '*')
            {
                settings.prec = this->get_arg(m_pos++).asInt();
                c = getnc();
            }
        }
/*
  std::cout << std::boolalpha
  << "alt: " << settings.alternateForm << std::endl
  << "zero: " << settings.zeroPadded << std::endl
  << "left: " << settings.leftAdjust << std::endl
  << "blank: " << settings.blank << std::endl
  << "sign: " << settings.sign << std::endl
  << "width: " << settings.width << std::endl
  << "prec: " << settings.prec << std::endl;

*/

        c = this->m_char;
        switch(c)
        {
        case 'h':
            consume();
            settings.mode = this->m_char == 'h' ? Settings::T_CHAR : Settings::T_SMALL;
            if(this->m_char == 'h') consume();
            break;
        case 'l':
            consume();
            settings.mode = this->m_char == 'l' ? Settings::T_LONGLONG : Settings::T_LONG;
            if(this->m_char == 'l') consume();
            break;
        case 'L':
            consume();
            settings.mode = Settings::T_LONGDOUBLE;
            break;
        default:
            settings.mode = Settings::T_NONE;
        }


        c = this->m_char;
        if(c == traits_type::eof())
            throw format_error_exception(this->m_fmt, "EOL reached");

        tmp.clear();

        switch(c)
        {
        case 's':
            m_str.append(this->get_arg(m_pos++).asStr());
            break;
        case 'd':
        case 'i':
            do_fmt_d(settings);
            break;
            //case 'u':
            //ss << this->get_arg(m_pos++).template get<unsigned int>();
            break;
        case 'f':
        case 'F':
            do_fmt_f(settings);
            break;
        default:
            throw format_error_exception(this->m_fmt, "Invalid format character");
        };
    }



    /// @brief Do 
    void do_fmt_base(Settings &s, stream_type &stream)
    {
        stream.imbue(this->m_loc);

        if(s.sign)
            stream << std::showpos;
            
        stream.width(s.width);
        stream.precision(s.prec);
        stream << std::fixed;
        stream << std::showpoint;
        

        if(s.zeroPadded && ! s.leftAdjust)
            stream.fill('0');
        if(s.leftAdjust)
            stream << std::left;
        else
            stream << std::internal;
    }


    void do_fmt_d(Settings &s)
    {
        stream_type stream;
        do_fmt_base(s, stream);
        switch(s.mode)
        {
        case Settings::T_CHAR:
            stream << this->get_arg(m_pos++).asChar(); break;
        case Settings::T_SMALL:
            stream << this->get_arg(m_pos++).asSmallint(); break;
        case Settings::T_LONG:
            stream << this->get_arg(m_pos++).asInt(); break;
        case Settings::T_LONGLONG:
            stream << this->get_arg(m_pos++).asBigint(); break;
        case Settings::T_LONGDOUBLE:
        case Settings::T_NONE:
            stream << this->get_arg(m_pos++).asInt(); break;
    
        }
        m_str.append(stream.str());
    }

    void do_fmt_f(Settings &s)
    {
        stream_type stream;
        do_fmt_base(s, stream);
        stream << this->get_arg(m_pos++).asReal();
        m_str.append(stream.str());
    }




    string_type parse(void)
    {
        m_pos = 0;
        char_type c = getnc();

        while(c != traits_type::eof())
        {
            if(c == '%')
            {
                if(getnc() == '%')
                    m_str.push_back('%');
                else
                    do_fmt();
            }
            else
                m_str.push_back(c);

            c = getnc();
        }
        
        return this->m_str;;
    }

private:
    const string_type &m_fmt;
    std::locale    m_loc;
    string_type    m_str;
    int            m_pos;
    char_type      m_char;
    typename string_type::const_iterator m_in, m_end;
    const std::vector<Variant>& m_args;

};


template<typename Ch, typename Tr = std::char_traits<Ch>, typename Alloc = std::allocator<Ch> >
class DBWTL_EXPORT basic_format
{
public:
    typedef std::basic_string<Ch, Tr, Alloc>    string_type;
    typedef fmt_parser<Ch, Tr, Alloc>       parser_type;
    typedef std::basic_stringstream<Ch, Tr, Alloc> stream_type;
    typedef Ch  char_type;


    typedef format_error_exception format_error;


    explicit basic_format(const char_type *str, std::locale loc = std::locale()) 
        : m_args(), m_argN(), m_fmt(str), m_loc(loc)
    {
    }

    explicit basic_format(const string_type &str, std::locale loc = std::locale()) 
        : m_args(), m_argN(), m_fmt(str), m_loc(loc)
    {
    }

    
    basic_format& feed(const Variant &var)
    {
        this->m_args.push_back(var);
        return *this;
    }


    basic_format operator%(const char_type *str)
    {
        String s(str);
        return this->feed(s);
    }

    basic_format operator%(const Variant &var)
    {
        return this->feed(var);
    }


    string_type str(void) const
    {
        parser_type p(m_args, m_fmt, m_loc);
        return p.parse();
    }

    operator String(void) const
    {
        return str();
    }


    operator string_type(void) const
    {
        return str();
    }

protected:
    std::vector<Variant> m_args;
    std::size_t m_argN;

    string_type m_fmt;


    std::locale m_loc;

};

typedef basic_format<char> format;
typedef basic_format<wchar_t> wformat;








DB_NAMESPACE_END






#endif



//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
