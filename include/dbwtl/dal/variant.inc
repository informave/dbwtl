
///
/// @cond DEV_DOCS

DAL_NAMESPACE_BEGIN



#define DBWTL_VARIANT_DISPATCHER(method, type)              \
    template<>                                              \
    struct variant_dispatch_method<type>                    \
        : public variant_dispatch_storage                   \
    {                                                       \
        variant_dispatch_method(const dal::IVariant &var)   \
            : variant_dispatch_storage(var)                 \
        {}                                                  \
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
DBWTL_VARIANT_DISPATCHER(asWideStr,    std::wstring);
DBWTL_VARIANT_DISPATCHER(asStr,        std::string);
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





template<typename T, typename U>
bool inline convertValue(const T& src, U& dest)
{
    std::wstringstream ss;
    ss << src;
    return ! (!(ss >> dest));
}

template<typename T, typename U>
void inline setValue(const T& src, U& dest)
{
    std::wstringstream ss;
    ss << src;
    dest = ss.str();
}



#define DAL_THROW_INVALID_CAST() throw std::runtime_error(__FUNCTION__)

#define DAL_SET_STORAGE_TYPE(type)              \
    virtual daltype_t datatype() const          \
    {                                           \
        return type;                            \
    }

#define DAL_VARIANT_DEFINE_ACCESSOR(type)       \
    DAL_VARIANT_ACCESSOR;                       \
    DAL_SET_STORAGE_TYPE(type)                  \
    virtual ~storage_accessor(void) { }





template<>
class storage_accessor<std::streambuf*> : public BaseVariantImplFor<sa_base<std::streambuf*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_BLOB; }

/*
  virtual std::wstring asWideStr(void) const
  {
  std::stringstream ss;
  //ss << this->getValue();
  ss << "need impl-x";
  return i18n::conv_from(ss.str(), "UTF-8");
  }


  virtual std::wstring asWideStr(std::locale loc = std::locale()) const
  {
  std::stringstream ss;
  ss.imbue(loc);
  //ss << this->getValue();
  return i18n::conv_from(ss.str(), "UTF-8");
  }
*/
    virtual std::streambuf* asBlob(void) const
    {
        return this->getValue();
    }
};

template<>
class storage_accessor<std::wstreambuf*> : public BaseVariantImplFor<sa_base<std::wstreambuf*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_MEMO; }

/*
  virtual std::wstring asWideStr(void) const
  {
  std::stringstream ss;
  //ss << this->getValue();
  ss << "need impl-x";
  return i18n::conv_from(ss.str(), "UTF-8");
  }


  virtual std::wstring asWideStr(std::locale loc = std::locale()) const
  {
  std::stringstream ss;
  ss.imbue(loc);
  //ss << this->getValue();
  return i18n::conv_from(ss.str(), "UTF-8");
  }
*/
    virtual std::wstreambuf* asMemo(void) const
    {
        return this->getValue();
    }
};




//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<signed int> : public BaseVariantImplFor<sa_base<signed int> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_INT; }

    virtual int asInt() const
    { return this->getValue(); }


    virtual void setInt(const signed int &value)
    {
        this->getValue() = value;
        this->m_isnull = false;
    }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << this->getValue();
        return ss.str();
    }
};



template<>
class storage_accessor<signed int*> : public BaseVariantImplFor<sa_base<signed int*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_INT; }


    virtual int asInt() const
    { return *this->getValue(); }


    virtual void setInt(const signed int &value)
    {
        *this->getValue() = value;
        this->m_isnull = false;
    }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << *this->getValue();
        return ss.str();
    }
};



template<>
class storage_accessor<const signed int*> : public BaseVariantImplFor<sa_base<const signed int*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_INT; }


    virtual int asInt() const
    { return *this->getValue(); }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << *this->getValue();
        return ss.str();
    }
};





//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<unsigned int> : public BaseVariantImplFor<sa_base<unsigned int> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UINT; }

    virtual void setUInt(const unsigned int &value)
    {
        this->getValue() = value;
        this->m_isnull = false;
    }

};


template<>
class storage_accessor<unsigned int*> : public BaseVariantImplFor<sa_base<unsigned int*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UINT; }
};


template<>
class storage_accessor<const unsigned int*> : public BaseVariantImplFor<sa_base<const unsigned int*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UINT; }
};







//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<signed char> : public BaseVariantImplFor<sa_base<signed char> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_CHAR; }
};


template<>
class storage_accessor<signed char*> : public BaseVariantImplFor<sa_base<signed char*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_CHAR; }
};


template<>
class storage_accessor<const signed char*> : public BaseVariantImplFor<sa_base<const signed char*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_CHAR; }
};






//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<unsigned char> : public BaseVariantImplFor<sa_base<unsigned char> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UCHAR; }
};


template<>
class storage_accessor<unsigned char*> : public BaseVariantImplFor<sa_base<unsigned char*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UCHAR; }
};


template<>
class storage_accessor<const unsigned char*> : public BaseVariantImplFor<sa_base<const unsigned char*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UCHAR; }
};








// varchars







//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<bool> : public BaseVariantImplFor<sa_base<bool> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_BOOL; }


    virtual void setBool(const bool &value)
    {
        this->getValue() = value;
        this->m_isnull = false;
    }

    virtual bool asBool(void) const
    {
        return this->getValue();
    }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << this->getValue();
        return ss.str();
    }

};


template<>
class storage_accessor<bool*> : public BaseVariantImplFor<sa_base<bool*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_BOOL; }

    virtual void setBool(const bool &value)
    {
        *this->getValue() = value;
        this->m_isnull = false;
    }


    virtual bool asBool(void) const
    {
        return *this->getValue();
    }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << *this->getValue();
        return ss.str();
    }

};


template<>
class storage_accessor<const bool*> : public BaseVariantImplFor<sa_base<const bool*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_BOOL; }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << *this->getValue();
        return ss.str();
    }

};






//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<signed short int> : public BaseVariantImplFor<sa_base<signed short int> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_SMALLINT; }


    virtual void setSmallint(const signed short int &value)
    {
        this->getValue() = value;
        this->m_isnull = false;
    }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << this->getValue();
        return ss.str();
    }
};


template<>
class storage_accessor<signed short int*> : public BaseVariantImplFor<sa_base<signed short int*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_SMALLINT; }


    virtual void setSmallint(const signed short int &value)
    {
        *this->getValue() = value;
        this->m_isnull = false;
    }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << *this->getValue();
        return ss.str();
    }
};


template<>
class storage_accessor<const signed short int*> : public BaseVariantImplFor<sa_base<const signed short int*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_SMALLINT; }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << *this->getValue();
        return ss.str();
    }
};






//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<unsigned short int> : public BaseVariantImplFor<sa_base<unsigned short int> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_USMALLINT; }


    virtual void setUSmallint(const unsigned short int &value)
    {
        this->getValue() = value;
        this->m_isnull = false;
    }

    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << this->getValue();
        return ss.str();
    }
};


template<>
class storage_accessor<unsigned short int*> : public BaseVariantImplFor<sa_base<unsigned short int*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_USMALLINT; }

    virtual void setUSmallint(const unsigned short int &value)
    {
        *this->getValue() = value;
        this->m_isnull = false;
    }

    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << *this->getValue();
        return ss.str();
    }
};


template<>
class storage_accessor<const unsigned short int*> : public BaseVariantImplFor<sa_base<const unsigned short int*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_USMALLINT; }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        std::wstringstream ss;
        ss.imbue(loc);
        ss << *this->getValue();
        return ss.str();
    }
};





//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<signed long long> : public BaseVariantImplFor<sa_base<signed long long> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_BIGINT; }
};


template<>
class storage_accessor<signed long long*> : public BaseVariantImplFor<sa_base<signed long long*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_BIGINT; }
};


template<>
class storage_accessor<const signed long long*> : public BaseVariantImplFor<sa_base<const signed long long*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_BIGINT; }
};









//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<unsigned long long> : public BaseVariantImplFor<sa_base<unsigned long long> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UBIGINT; }
};


template<>
class storage_accessor<unsigned long long*> : public BaseVariantImplFor<sa_base<unsigned long long*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UBIGINT; }
};


template<>
class storage_accessor<const unsigned long long*> : public BaseVariantImplFor<sa_base<const unsigned long long*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_UBIGINT; }
};








// Blob




//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TNumeric> : public BaseVariantImplFor<sa_base<TNumeric> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_NUMERIC; }
};


template<>
class storage_accessor<TNumeric*> : public BaseVariantImplFor<sa_base<TNumeric*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_NUMERIC; }
};


template<>
class storage_accessor<const TNumeric*> : public BaseVariantImplFor<sa_base<const TNumeric*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_NUMERIC; }
};







//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<float> : public BaseVariantImplFor<sa_base<float> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_FLOAT; }
};


template<>
class storage_accessor<float*> : public BaseVariantImplFor<sa_base<float*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_FLOAT; }
};


template<>
class storage_accessor<const float*> : public BaseVariantImplFor<sa_base<const float*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_FLOAT; }
};




//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<double> : public BaseVariantImplFor<sa_base<double> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DOUBLE; }
};


template<>
class storage_accessor<double*> : public BaseVariantImplFor<sa_base<double*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DOUBLE; }
};


template<>
class storage_accessor<const double*> : public BaseVariantImplFor<sa_base<const double*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DOUBLE; }
};





//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TDate> : public BaseVariantImplFor<sa_base<TDate> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DATE; }
};


template<>
class storage_accessor<TDate*> : public BaseVariantImplFor<sa_base<TDate*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DATE; }
};


template<>
class storage_accessor<const TDate*> : public BaseVariantImplFor<sa_base<const TDate*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DATE; }
};




//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TTime> : public BaseVariantImplFor<sa_base<TTime> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_TIME; }
};


template<>
class storage_accessor<TTime*> : public BaseVariantImplFor<sa_base<TTime*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_TIME; }
};


template<>
class storage_accessor<const TTime*> : public BaseVariantImplFor<sa_base<const TTime*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_TIME; }
};



//------------------------------------------------------------------------------
///
///
/*
  template<>
  class storage_accessor<TDatetime> : public BaseVariantImplFor<sa_base<TDatetime> >
  {
  public:
  DAL_VARIANT_ACCESSOR;
  virtual ~storage_accessor(void) { }
  virtual daltype_t datatype() const { return DAL_TYPE_DATETIME; }
  };


  template<>
  class storage_accessor<TDatetime*> : public BaseVariantImplFor<sa_base<TDatetime*> >
  {
  public:
  DAL_VARIANT_ACCESSOR;
  virtual ~storage_accessor(void) { }
  virtual daltype_t datatype() const { return DAL_TYPE_DATETIME; }
  };


  template<>
  class storage_accessor<const TDatetime*> : public BaseVariantImplFor<sa_base<const TDatetime*> >
  {
  public:
  DAL_VARIANT_ACCESSOR;
  virtual ~storage_accessor(void) { }
  virtual daltype_t datatype() const { return DAL_TYPE_DATETIME; }
  };
*/


//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TTimestamp> : public BaseVariantImplFor<sa_base<TTimestamp> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_TIMESTAMP; }
};


template<>
class storage_accessor<TTimestamp*> : public BaseVariantImplFor<sa_base<TTimestamp*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_TIMESTAMP; }
};


template<>
class storage_accessor<const TTimestamp*> : public BaseVariantImplFor<sa_base<const TTimestamp*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_TIMESTAMP; }
};




//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TInterval> : public BaseVariantImplFor<sa_base<TInterval> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_INTERVAL; }
};


template<>
class storage_accessor<TInterval*> : public BaseVariantImplFor<sa_base<TInterval*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_INTERVAL; }
};


template<>
class storage_accessor<const TInterval*> : public BaseVariantImplFor<sa_base<const TInterval*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_INTERVAL; }
};



/*
//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TCidr> : public BaseVariantImplFor<sa_base<TCidr> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_CIDR; }
};


template<>
class storage_accessor<TCidr*> : public BaseVariantImplFor<sa_base<TCidr*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_CIDR; }
};


template<>
class storage_accessor<const TCidr*> : public BaseVariantImplFor<sa_base<const TCidr*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_CIDR; }
};





//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TMacaddr> : public BaseVariantImplFor<sa_base<TMacaddr> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_MACADDR; }
};


template<>
class storage_accessor<TMacaddr*> : public BaseVariantImplFor<sa_base<TMacaddr*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_MACADDR; }
};


template<>
class storage_accessor<const TMacaddr*> : public BaseVariantImplFor<sa_base<const TMacaddr*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_MACADDR; }
};






//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TInetaddr> : public BaseVariantImplFor<sa_base<TInetaddr> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_INETADDR; }
};


template<>
class storage_accessor<TInetaddr*> : public BaseVariantImplFor<sa_base<TInetaddr*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_INETADDR; }
};


template<>
class storage_accessor<const TInetaddr*> : public BaseVariantImplFor<sa_base<const TInetaddr*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_INETADDR; }
};






//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TUuid> : public BaseVariantImplFor<sa_base<TUuid> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_UUID; }
};


template<>
class storage_accessor<TUuid*> : public BaseVariantImplFor<sa_base<TUuid*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_UUID; }
};


template<>
class storage_accessor<const TUuid*> : public BaseVariantImplFor<sa_base<const TUuid*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_UUID; }
};




//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TXml> : public BaseVariantImplFor<sa_base<TXml> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_XML; }
};


template<>
class storage_accessor<TXml*> : public BaseVariantImplFor<sa_base<TXml*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_XML; }
};


template<>
class storage_accessor<const TXml*> : public BaseVariantImplFor<sa_base<const TXml*> >
{
public:
DAL_VARIANT_ACCESSOR;
virtual ~storage_accessor(void) { }
virtual daltype_t datatype() const { return DAL_TYPE_XML; }
};


*/












//------------------------------------------------------------------------------
/// 
/// @brief Storage accessor for std::wstring
///
template<>
struct storage_accessor<std::wstring> : public BaseVariantImplFor<sa_base<std::wstring> >
{
    DAL_VARIANT_DEFINE_ACCESSOR(DAL_TYPE_VARCHAR)

    ////// GETTER METHODS

    virtual signed int asInt(void) const
    {
        signed int v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

    virtual unsigned int asUInt(void) const
    {
        unsigned int v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

//     virtual signed char asChar(void) const
//     {
//     }

//     virtual unsigned char asUChar(void) const
//     {
//     }

    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        return this->getValue();
    }

    virtual bool asBool(void) const
    {
        bool v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

    virtual signed short asSmallint(void) const
    {
        signed short v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

    virtual unsigned short asUSmallint(void) const
    {
        unsigned int v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

    virtual signed long long asBigint(void) const
    {
        signed long long v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

    virtual unsigned long long asUBigint(void) const
    {
        unsigned long long v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

//     virtual TNumeric asNumeric(void) const
//     {
//     }

    virtual float asReal(void) const
    {
        float v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

    virtual double asDouble(void) const
    {
        double v;
        if(!convertValue(this->getValue(), v)) DAL_THROW_INVALID_CAST();
        else return v;
    }

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

//     virtual std::streambuf* asBlob(void) const
//     {
//     }

    virtual std::wstreambuf* asMemo(void) const
    {
        std::wstringbuf *buf = new std::wstringbuf();
        buf->str(this->getValue());
        return buf;
    }

    ////// SETTER METHODS

    virtual void setInt(const signed int &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

    virtual void setUInt(const unsigned int &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

//     virtual void setChar(const signed char &val)
//     {
//     }

//     virtual void setUChar(const unsigned char &val)
//     {
//     }

    virtual void setWideStr(const std::wstring  &val, std::locale loc = std::locale())
    {
        this->getValue() = val;
        this->m_isnull = false;
    }

    virtual void setBool(const bool &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

    virtual void setSmallint(const signed short &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

    virtual void setUSmallint(const unsigned short &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

    virtual void setBigint(const signed long long &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

    virtual void setUBigint(const unsigned long long &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

    virtual void setNumeric(const TNumeric &val)
    {
        this->getValue() = val.asWideStr();
        this->m_isnull = false;
    }

    virtual void setReal(const float &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

    virtual void setDouble(const double &val)
    {
        setValue(val, this->getValue());
        this->m_isnull = false;
    }

    virtual void setDate(const TDate &val)
    {
        this->getValue() = val.asWideStr();
        this->m_isnull = false;
    }

    virtual void setTime(const TTime &val)
    {
        this->getValue() = val.asWideStr();
        this->m_isnull = false;
    }

    virtual void setTimestamp(const TTimestamp &val)
    {
        this->getValue() = val.asWideStr();
        this->m_isnull = false;
    }

    virtual void setInterval(const TInterval &val)
    {
        this->getValue() = val.asWideStr();
        this->m_isnull = false;
    }

//     virtual void setBlob(std::streambuf *val)
//     {
//     }

    virtual void setMemo(std::wstreambuf *val)
    {
        Memo m(val);
        this->getValue() = m.str();
        this->m_isnull = false;
    }
};








template<>
class storage_accessor<std::wstring*> : public BaseVariantImplFor<sa_base<std::wstring*> >
{
public:
    DAL_VARIANT_ACCESSOR;

    virtual ~storage_accessor(void) { }

    virtual daltype_t datatype() const
    {
        return DAL_TYPE_VARCHAR;
    }

};

template<>
class storage_accessor<const std::wstring*> : public BaseVariantImplFor<sa_base<const std::wstring*> >
{
public:
    DAL_VARIANT_ACCESSOR;

    virtual ~storage_accessor(void) { }

    virtual daltype_t datatype() const
    {
        return DAL_TYPE_VARCHAR;
    }
};



//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<IVariant*> : public BaseVariantImplFor<sa_base<IVariant*> >
{
public:
    DAL_VARIANT_ACCESSOR;

    virtual ~storage_accessor(void) { }

    virtual int asInt() const
    { return this->getValue()->asInt(); }


    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        return L"Implement me";
    }

    virtual daltype_t datatype() const
    {
        return this->getValue()->datatype();
    }
};


template<>
class storage_accessor<const IVariant*> : public BaseVariantImplFor<sa_base<const IVariant*> >
{
public:
    DAL_VARIANT_ACCESSOR;

    virtual ~storage_accessor(void) { }

    virtual int asInt() const
    { return this->getValue()->asInt(); }



    virtual std::wstring asWideStr(std::locale loc = std::locale()) const
    {
        return L"Implement me";
    }


    virtual daltype_t datatype() const
    {
        return this->getValue()->datatype();
    }
};












//------------------------------------------------------------------------------
///
///
template<class Base>
bool BaseVariantImplFor<Base>::isnull(void) const
{
    return this->m_isnull;
}

template<class Base>
void BaseVariantImplFor<Base>::setNull(void)
{
    this->m_isnull = true;
}



template<class Base>
signed int      BaseVariantImplFor<Base>::asInt(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_INT); }

template<class Base>
unsigned int    BaseVariantImplFor<Base>::asUInt(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_UINT); }

template<class Base>
signed char     BaseVariantImplFor<Base>::asChar(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_CHAR); }

template<class Base>
unsigned char   BaseVariantImplFor<Base>::asUChar(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_UCHAR); }

template<class Base>
std::wstring   BaseVariantImplFor<Base>::asWideStr(std::locale loc) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_VARCHAR); }


template<class Base>
bool            BaseVariantImplFor<Base>::asBool(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_BOOL); }

template<class Base>
signed short    BaseVariantImplFor<Base>::asSmallint(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_SMALLINT); }

template<class Base>
unsigned short  BaseVariantImplFor<Base>::asUSmallint(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_USMALLINT); }

template<class Base>
signed long long    BaseVariantImplFor<Base>::asBigint(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_BIGINT); }

template<class Base>
unsigned long long  BaseVariantImplFor<Base>::asUBigint(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_UBIGINT); }



template<class Base>
TNumeric        BaseVariantImplFor<Base>::asNumeric(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_NUMERIC); }

template<class Base>
float           BaseVariantImplFor<Base>::asReal(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_FLOAT); }

template<class Base>
double          BaseVariantImplFor<Base>::asDouble(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_DOUBLE); }

template<class Base>
TDate           BaseVariantImplFor<Base>::asDate(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_DATE); }

template<class Base>
TTime           BaseVariantImplFor<Base>::asTime(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_TIME); }


template<class Base>
TTimestamp      BaseVariantImplFor<Base>::asTimestamp(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_TIMESTAMP); }

template<class Base>
TInterval       BaseVariantImplFor<Base>::asInterval(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_INTERVAL); }

//virtual TCustom&        asCustom(void) const = 0;

/*
  template<class Base>
  TCidr           BaseVariantImplFor<Base>::asCIDR(void) const 
  { throw ex::convert_error(this->datatype(), DAL_TYPE_CIDR); }

  template<class Base>
  TMacaddr        BaseVariantImplFor<Base>::asMacaddr(void) const 
  { throw ex::convert_error(this->datatype(), DAL_TYPE_MACADDR); }

  template<class Base>
  TInetaddr       BaseVariantImplFor<Base>::asInetaddr(void) const 
  { throw ex::convert_error(this->datatype(), DAL_TYPE_INETADDR); }


  template<class Base>
  TUuid           BaseVariantImplFor<Base>::asUUID(void) const 
  { throw ex::convert_error(this->datatype(), DAL_TYPE_UUID); }

  template<class Base>
  TXml            BaseVariantImplFor<Base>::asXML(void) const
  { throw ex::convert_error(this->datatype(), DAL_TYPE_XML); }
*/

template<class Base>
std::streambuf*         BaseVariantImplFor<Base>::asBlob(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_BLOB); }

template<class Base>
std::wstreambuf*         BaseVariantImplFor<Base>::asMemo(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_MEMO); }


template<class Base>
void            BaseVariantImplFor<Base>::setInt(const signed int&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setUInt(const unsigned int&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setChar(const signed char&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setUChar(const unsigned char&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setWideStr(const std::wstring&, std::locale loc) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setBool(const bool&)
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setSmallint(const signed short&)
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setUSmallint(const unsigned short&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setBigint(const signed long long&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setUBigint(const unsigned long long&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setNumeric(const TNumeric&) 
{ throw ex::read_only(); }


template<class Base>
void            BaseVariantImplFor<Base>::setReal(const float&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setDouble(const double&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setDate(const TDate&)
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setTime(const TTime&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setTimestamp(const TTimestamp&)
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setInterval(const TInterval&)
{ throw ex::read_only(); }

//virtual void        asCustom(void) const = 0;

/*
  template<class Base>
  void            BaseVariantImplFor<Base>::setCIDR(const TCidr&)
  { throw ex::read_only(); }

  template<class Base>
  void            BaseVariantImplFor<Base>::setMacaddr(const TMacaddr&)
  { throw ex::read_only(); }

  template<class Base>
  void            BaseVariantImplFor<Base>::setInetaddr(const TInetaddr&) 
  { throw ex::read_only(); }

  template<class Base>
  void            BaseVariantImplFor<Base>::setUUID(const TUuid&)
  { throw ex::read_only(); }

  template<class Base>
  void            BaseVariantImplFor<Base>::setXML(const TXml&) 
  { throw ex::read_only(); }
*/

template<class Base>
void            BaseVariantImplFor<Base>::setBlob(std::streambuf*)
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setMemo(std::wstreambuf*)
{ throw ex::read_only(); }



DAL_NAMESPACE_END

/// @endcond





// template<>
// struct storage_accessor<std::wstring> : public BaseVariantImplFor<sa_base<std::wstring> >
// {
//     DAL_VARIANT_DEFINE_ACCESSOR(<your-type>)

//     ////// GETTER METHODS

//     virtual signed int asInt(void) const
//     {
//     }

//     virtual unsigned int asUInt(void) const
//     {
//     }

//     virtual signed char asChar(void) const
//     {
//     }

//     virtual unsigned char asUChar(void) const
//     {
//     }

//     virtual std::wstring asWideStr(std::locale loc = std::locale()) const
//     {
//     }

//     virtual bool asBool(void) const
//     {
//     }

//     virtual signed short asSmallint(void) const
//     {
//     }

//     virtual unsigned short asUSmallint(void) const
//     {
//     }

//     virtual signed long long asBigint(void) const
//     {
//     }

//     virtual unsigned long long asUBigint(void) const
//     {
//     }

//     virtual TNumeric asNumeric(void) const
//     {
//     }

//     virtual float asReal(void) const
//     {
//     }

//     virtual double asDouble(void) const
//     {
//     }

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

//     virtual std::streambuf* asBlob(void) const
//     {
//     }

//     virtual std::wstreambuf* asMemo(void) const
//     {
//     }

//     ////// SETTER METHODS

//     virtual void setInt(const signed int &val)
//     {
//     }

//     virtual void setUInt(const unsigned int &val)
//     {
//     }

//     virtual void setChar(const signed char &val)
//     {
//     }

//     virtual void setUChar(const unsigned char &val)
//     {
//     }

//     virtual void setWideStr(const std::wstring  &val, std::locale loc = std::locale())
//     {
//     }

//     virtual void setBool(const bool &val)
//     {
//     }

//     virtual void setSmallint(const signed short &val)
//     {
//     }

//     virtual void setUSmallint(const unsigned short &val)
//     {
//     }

//     virtual void setBigint(const signed long long &val)
//     {
//     }

//     virtual void setUBigint(const unsigned long long &val)
//     {
//     }

//     virtual void setNumeric(const TNumeric &val)
//     {
//     }

//     virtual void setReal(const float &val)
//     {
//     }

//     virtual void setDouble(const double &val)
//     {
//     }

//     virtual void setDate(const TDate &val)
//     {
//     }

//     virtual void setTime(const TTime &val)
//     {
//     }

//     virtual void setTimestamp(const TTimestamp &val)
//     {
//     }

//     virtual void setInterval(const TInterval &val)
//     {
//     }

//     virtual void setBlob(std::streambuf *val)
//     {
//     }

//     virtual void setMemo(std::wstreambuf *val)
//     {
//     }
// }





//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
