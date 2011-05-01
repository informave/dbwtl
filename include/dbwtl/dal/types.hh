//
// types.hh - DAL Types
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
/// @brief DAL Types
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_TYPES_HH
#define INFORMAVE_DB_DAL_TYPES_HH

#include "dal_fwd.hh"

#include "dbwtl/ustring.hh"
#include "dbwtl/ustreambuf.hh"
#include "dbwtl/util/smartptr.hh"
#include "dbwtl/dal/variant.hh"


DAL_NAMESPACE_BEGIN




/// @brief Maps a daltype ID to a string name
DBWTL_EXPORT String daltype2string(daltype_t type);

DBWTL_EXPORT String daltype2sqlname(daltype_t type);






//------------------------------------------------------------------------------
///
/// @brief DAL Interface for special types
class DBWTL_EXPORT TType
{
public:
    TType(void)
    { this->m_isnull = true; }

    bool m_isnull;
};


class DBWTL_EXPORT TCustomType : public TType
{
public:
    TCustomType(void) : TType()
    {}

    virtual ~TCustomType(void)
    {}
};




class DBWTL_EXPORT TDate : public TType
{
public:
    virtual daltype_t  datatype(void) const;

    virtual String         asStr(std::locale loc = std::locale()) const;
};


class DBWTL_EXPORT TTime : public TType
{
public:
    virtual daltype_t  datatype(void) const;

    virtual String         asStr(std::locale loc = std::locale()) const;
};


class DBWTL_EXPORT TInterval : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual String         asStr(std::locale loc = std::locale()) const;

};


class DBWTL_EXPORT TNumeric : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual String         asStr(std::locale loc = std::locale()) const;


    TNumeric(void);

    TNumeric(long long num, long long frac = 0);

    TNumeric(std::string value);
    
    //std::string m_value;

    long long m_value;
    long long m_frac;

    bool operator==(const TNumeric &num) const
    {
        return m_value == num.m_value && m_frac == num.m_frac;
    }

};


class DBWTL_EXPORT TTimestamp : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual String         asStr(std::locale loc = std::locale()) const;

	TTimestamp(void) : m_value(0) {}

	TTimestamp(signed long long ts) : m_value(ts) {}

	bool operator==(const TTimestamp &ts) const
	{
		return m_value == ts.m_value;
	}

protected:

	signed long long m_value;
};







/*
  class DBWTL_EXPORT TCidr : public TType
  {
  public:
  virtual daltype_t  datatype(void) const;
  virtual std::wstring asWideStr(std::locale loc) const;
  virtual std::wstring asWideStr() const; // using objects locale
  };
  class DBWTL_EXPORT TMacaddr : public TType
  {
  public:
  virtual daltype_t  datatype(void) const;
  virtual std::wstring asWideStr(std::locale loc) const;
  virtual std::wstring asWideStr() const; // using objects locale

  public:
  // converts XX:XX:XX:XX:XX:XX or raises an exception
  static TMacaddr convert(std::wstring value)
  {
  TMacaddr v;
  v.blocks[0] = 0xAA;
  v.blocks[1] = 0xBB;
  v.blocks[2] = 0xCC;
  v.blocks[3] = 0xDD;
  v.blocks[4] = 0xEE;
  v.blocks[5] = 0xFF;
  return v;
  }

  unsigned char blocks[6];
  };

  class DBWTL_EXPORT TInetaddr : public TType
  {
  public:
  virtual daltype_t  datatype(void) const;
  virtual std::wstring asWideStr(std::locale loc) const;
  virtual std::wstring asWideStr() const; // using objects locale
  };

  class DBWTL_EXPORT TUuid : public TType
  {
  public:
  virtual daltype_t  datatype(void) const;
  virtual std::wstring asWideStr(std::locale loc) const;
  virtual std::wstring asWideStr() const; // using objects locale
  };



  class DBWTL_EXPORT TXml : public TType
  {
  public:
  virtual daltype_t  datatype(void) const;
  virtual std::wstring asWideStr(std::locale loc) const;
  virtual std::wstring asWideStr() const; // using objects locale
  };
*/





























/*
#define DBWTL_VARIANT_DISPATCHER(method, type)              \
    template<>                                              \
    struct variant_dispatch_method<type>                    \
        : public variant_dispatch_storage                   \
    {                                                       \
        variant_dispatch_method(const dal::IVariant &var)   \
            : variant_dispatch_storage(var)                 \
        {}                                                  \
                                                            \
        virtual ~variant_dispatch_method(void) {}           \
                                                            \
        type operator()(void) const                         \
        {                                                   \
            return this->get().method();                    \
        }                                                   \
    }


DBWTL_VARIANT_DISPATCHER(asInt,        signed int);
DBWTL_VARIANT_DISPATCHER(asUInt,       unsigned int);
DBWTL_VARIANT_DISPATCHER(asChar,       signed char);
DBWTL_VARIANT_DISPATCHER(asUChar,      unsigned char);
DBWTL_VARIANT_DISPATCHER(asStr,        String);
DBWTL_VARIANT_DISPATCHER(asBool,       bool);
DBWTL_VARIANT_DISPATCHER(asSmallint,   signed short);
DBWTL_VARIANT_DISPATCHER(asUSmallint,  unsigned short);
DBWTL_VARIANT_DISPATCHER(asBigint,     signed long long);
DBWTL_VARIANT_DISPATCHER(asUBigint,    unsigned long long);
DBWTL_VARIANT_DISPATCHER(asNumeric,    TNumeric);
DBWTL_VARIANT_DISPATCHER(asReal,       float);
DBWTL_VARIANT_DISPATCHER(asDouble,     double);
DBWTL_VARIANT_DISPATCHER(asDate,       TDate);
DBWTL_VARIANT_DISPATCHER(asTime,       TTime);
DBWTL_VARIANT_DISPATCHER(asTimestamp,  TTimestamp);

*/


#define DAL_SET_STORAGE_TYPE(type)              \
    virtual daltype_t datatype() const          \
    {                                           \
        return type;                            \
    }

#define DAL_VARIANT_DEFINE_ACCESSOR(type)       \
    DAL_VARIANT_ACCESSOR;                       \
    DAL_SET_STORAGE_TYPE(type)                  \
    virtual ~storage_accessor(void) { }


#define READ_ACCESSOR(typeval)                              \
    DAL_VARIANT_ACCESSOR;                                   \
    virtual ~read_accessor(void) { }                        \
    virtual daltype_t datatype() const { return typeval; }


#define READWRITE_ACCESSOR                      \
    DAL_VARIANT_ACCESSOR;                       \
    virtual ~readwrite_accessor(void) { }                     



///
template<>
class read_accessor<ByteStreamBuf> : public default_accessor<ByteStreamBuf>
{
public:
    READ_ACCESSOR(DAL_TYPE_BLOB);

    virtual ByteStreamBuf* asBlob(void) const;
};


///
template<>
class read_accessor<UnicodeStreamBuf> : public default_accessor<UnicodeStreamBuf>
{
public:
    READ_ACCESSOR(DAL_TYPE_MEMO);

    virtual UnicodeStreamBuf* asMemo(void) const;
};

///
template<>
class read_accessor<signed int> : public default_accessor<signed int>
{
public:
    READ_ACCESSOR(DAL_TYPE_INT);


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
//    virtual TDate                 asDate(void) const;
//    virtual TTime                 asTime(void) const;
    virtual TTimestamp            asTimestamp(void) const;
//    virtual TInterval             asInterval(void) const;
    virtual ByteStreamBuf*        asBlob(void) const;
    virtual UnicodeStreamBuf*     asMemo(void) const;
    //virtual TCustom&        asCustom(void) const = 0;
    
    

//    virtual int asInt() const;
//    virtual String asStr(std::locale loc = std::locale()) const;
};


///
template<>
class read_accessor<unsigned int> : public default_accessor<unsigned int>
{
public:
    READ_ACCESSOR(DAL_TYPE_UINT);

};

///

///
template<>
class read_accessor<signed char> : public default_accessor<signed char>
{
public:
    READ_ACCESSOR(DAL_TYPE_CHAR);
};

///
template<>
class read_accessor<unsigned char> : public default_accessor<unsigned char>
{
public:
    READ_ACCESSOR(DAL_TYPE_UCHAR);
};

///
template<>
class read_accessor<bool> : public default_accessor<bool>
{
public:
    READ_ACCESSOR(DAL_TYPE_BOOL);

    virtual bool asBool(void) const;
    virtual String asStr(std::locale loc = std::locale()) const;
};



///
template<>
class read_accessor<signed short> : public default_accessor<signed short>
{
public:
    READ_ACCESSOR(DAL_TYPE_SMALLINT);

    virtual String asStr(std::locale loc = std::locale()) const;
};



///
template<>
class read_accessor<unsigned short> : public default_accessor<unsigned short>
{
public:
    READ_ACCESSOR(DAL_TYPE_USMALLINT);

    virtual String asStr(std::locale loc = std::locale()) const;
};



///
template<>
class read_accessor<signed long long> : public default_accessor<signed long long>
{
public:
    READ_ACCESSOR(DAL_TYPE_BIGINT);

    virtual int asInt() const;
    virtual long long asBigint() const;
    virtual String asStr(std::locale loc = std::locale()) const;
};


///
template<>
class read_accessor<unsigned long long> : public default_accessor<unsigned long long>
{
public:
    READ_ACCESSOR(DAL_TYPE_UBIGINT);
};

///
template<>
class read_accessor<TNumeric> : public default_accessor<TNumeric>
{
public:
    READ_ACCESSOR(DAL_TYPE_NUMERIC);
};

///
template<>
class read_accessor<double> : public default_accessor<double>
{
public:
    READ_ACCESSOR(DAL_TYPE_DOUBLE);
};

///
template<>
class read_accessor<float> : public default_accessor<float>
{
public:
    READ_ACCESSOR(DAL_TYPE_FLOAT);
};

///
template<>
class read_accessor<TDate> : public default_accessor<TDate>
{
public:
    READ_ACCESSOR(DAL_TYPE_DATE);
};

///
template<>
class read_accessor<TTime> : public default_accessor<TTime>
{
public:
    READ_ACCESSOR(DAL_TYPE_TIME);
};

///
template<>
class read_accessor<TTimestamp> : public default_accessor<TTimestamp>
{
public:
    READ_ACCESSOR(DAL_TYPE_TIMESTAMP);
};

///
template<>
class read_accessor<TInterval> : public default_accessor<TInterval>
{
public:
    READ_ACCESSOR(DAL_TYPE_INTERVAL);
};

///
template<>
struct read_accessor<String> : public default_accessor<String>
{
    READ_ACCESSOR(DAL_TYPE_STRING);

    virtual signed int asInt(void) const;
    virtual unsigned int asUInt(void) const;

//     virtual signed char asChar(void) const
//     {
//     }

//     virtual unsigned char asUChar(void) const
//     {
//     }

    virtual String asStr(std::locale loc = std::locale()) const;
    virtual bool asBool(void) const;
    virtual signed short asSmallint(void) const;
    virtual unsigned short asUSmallint(void) const;
    virtual signed long long asBigint(void) const;
    virtual unsigned long long asUBigint(void) const;

//     virtual TNumeric asNumeric(void) const
//     {
//     }

    virtual float asReal(void) const;
    virtual double asDouble(void) const;

//     virtual TDate asDate(void) const
//     {
//     }

//     virtual TTime asTime(void) const
//     {
//     }

//     virtual TTimestamp asTimestamp(void) const
//     {
//     }

//     virtual TInterval asInterval(void) const
//     {
//     }

//     virtual ByteStreamBuf* asBlob(void) const
//     {
//     }

    virtual UnicodeStreamBuf* asMemo(void) const;
};



///
template<>
class read_accessor<IVariant> : public default_accessor<IVariant>
{
public:
    DAL_VARIANT_ACCESSOR;

    virtual ~read_accessor(void) { }

    virtual int asInt() const;

    virtual String asStr(std::locale loc = std::locale()) const;

    virtual daltype_t datatype() const;
};





template<> struct Converter<signed char>
{
    typedef signed char type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asChar();
    }
};


template<> struct Converter<unsigned char>
{
    typedef unsigned char type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asUChar();
    }
};


template<> struct Converter<float>
{
    typedef float type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asReal();
    }
};


template<> struct Converter<double>
{
    typedef double type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asDouble();
    }
};


template<> struct Converter<TNumeric>
{
    typedef TNumeric type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asNumeric();
    }
};


template<> struct Converter<TTime>
{
    typedef TTime type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asTime();
    }
};


template<> struct Converter<TDate>
{
    typedef TDate type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asDate();
    }
};


template<> struct Converter<TTimestamp>
{
    typedef TTimestamp type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asTimestamp();
    }
};







template<> struct Converter<signed int>
{
    typedef signed int type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asInt();
    }
};


template<> struct Converter<TInterval>
{
    typedef TInterval type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asInterval();
    }
};




template<> struct Converter<unsigned int>
{
    typedef unsigned int type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asUInt();
    }
};



template<> struct Converter<signed short>
{
    typedef signed short type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asSmallint();
    }
};


template<> struct Converter<unsigned short>
{
    typedef unsigned short type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asUSmallint();
    }
};



template<> struct Converter<signed long long>
{
    typedef signed long long type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asBigint();
    }
};


template<> struct Converter<unsigned long long>
{
    typedef unsigned long long type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asUBigint();
    }
};



template<> struct Converter<bool>
{
    typedef bool type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asBool();
    }
};


template<> struct Converter<String>
{
    typedef String type;
    inline type operator()(const IStoredVariant &sv)
    {
        return sv.asStr();
    }
};

template<> struct Converter<IVariant>
{
    typedef IVariant* type;
    inline type operator()(const IStoredVariant &sv)
    {
        throw std::runtime_error("err");
    }
};









template<>
struct variant_assign<UnicodeStreamBuf*>
{
    void set_new_value(UnicodeStreamBuf*& dest, const Variant &src)
    {
        throw std::runtime_error("read only-todo");
        // *dest = src.get<T>();
    }
};


template<>
struct variant_assign<ByteStreamBuf*>
{
    void set_new_value(ByteStreamBuf*& dest, const Variant &src)
        {
        throw std::runtime_error("read only-todo");
        // *dest = src.get<T>();
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
