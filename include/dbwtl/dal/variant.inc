
///
/// @cond DEV_DOCS

DAL_NAMESPACE_BEGIN



#define DBWTL_VARIANT_DISPATCHER(method, type)                          \
    template<>                                                          \
    struct variant_dispatch_method<type>                                \
        : public variant_dispatch_storage                               \
    {                                                                   \
        variant_dispatch_method(const dal::IVariant &var)               \
            : variant_dispatch_storage(var)                             \
        {}                                                              \
                                                                        \
        type operator()(void) const                                     \
        {                                                               \
            return this->get().method();                                \
        }                                                               \
    };


DBWTL_VARIANT_DISPATCHER(asInt, signed int)
DBWTL_VARIANT_DISPATCHER(asUInt, unsigned int)
DBWTL_VARIANT_DISPATCHER(asChar, signed char)
DBWTL_VARIANT_DISPATCHER(asUChar, unsigned char)
DBWTL_VARIANT_DISPATCHER(asStr, i18n::UString)
DBWTL_VARIANT_DISPATCHER(asBool, bool)
DBWTL_VARIANT_DISPATCHER(asSmallint, signed short)
DBWTL_VARIANT_DISPATCHER(asUSmallint, unsigned short)
DBWTL_VARIANT_DISPATCHER(asBigint, signed long long)
DBWTL_VARIANT_DISPATCHER(asUBigint, unsigned long long)
DBWTL_VARIANT_DISPATCHER(asNumeric, TNumeric)
DBWTL_VARIANT_DISPATCHER(asReal, float)
DBWTL_VARIANT_DISPATCHER(asDouble, double)
DBWTL_VARIANT_DISPATCHER(asDate, TDate)
DBWTL_VARIANT_DISPATCHER(asTime, TTime)
DBWTL_VARIANT_DISPATCHER(asTimestamp, TTimestamp)

//DBWTL_VARIANT_DISPATCHER(asUUID, TUuid)
//DBWTL_VARIANT_DISPATCHER(asXML, TXml)





template<>
class storage_accessor<std::streambuf*> : public BaseVariantImplFor<sa_base<std::streambuf*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_BLOB; }

/*
    virtual i18n::UString asStr(void) const
        {
            std::stringstream ss;
            //ss << this->getValue();
            ss << "need impl-x";
            return i18n::conv_from(ss.str(), "UTF-8");
        }


    virtual i18n::UString asStr(std::locale loc) const
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
    virtual i18n::UString asStr(void) const
        {
            std::stringstream ss;
            //ss << this->getValue();
            ss << "need impl-x";
            return i18n::conv_from(ss.str(), "UTF-8");
        }


    virtual i18n::UString asStr(std::locale loc) const
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


    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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



    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << *this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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



    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << *this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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

    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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

    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << *this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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


    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << *this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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


    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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


    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << *this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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


    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << *this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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


    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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


    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << *this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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


    virtual i18n::UString asStr(void) const
        {
            std::wstringstream ss;
            ss << *this->getValue();
            return ss.str();
        }


    virtual i18n::UString asStr(std::locale loc) const
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
///
template<>
class storage_accessor<i18n::UString> : public BaseVariantImplFor<sa_base<i18n::UString> >
{
public:
    DAL_VARIANT_ACCESSOR;

    virtual ~storage_accessor(void) { }

    virtual void setStr(const i18n::UString &str)
        {
            this->getValue() = str;
            this->m_isnull = false;
        }


    virtual void setInt(const signed int& value)
        {
            std::wstringstream ss;
            ss << value;
            this->getValue() = ss.str();
            this->m_isnull = false;
        }

    virtual void setUInt(const unsigned int& value)
        {
            std::wstringstream ss;
            ss << value;
            this->getValue() = ss.str();
            this->m_isnull = false;
        }

    virtual void setChar(const signed char& value)
        {
            std::wstringstream ss;
            ss << value;
            this->getValue() = ss.str();
            this->m_isnull = false;
        }

    virtual void setUChar(const unsigned char& value)
        {
            std::wstringstream ss;
            ss << value;
            this->getValue() = ss.str();           
            this->m_isnull = false;
        }



    virtual void setStr(const std::string& value, const char* charset)
        {
            this->setStr(i18n::conv_from(value, charset));
        }

    virtual void setBool(const bool& value)
        {
            std::wstringstream ss;
            ss << value;
            this->getValue() = ss.str();            
            this->m_isnull = false;
        }

    virtual void setSmallint(const signed short& value)
        {
            std::wstringstream ss;
            ss << value;
            this->getValue() = ss.str();
            this->m_isnull = false;
        }

    virtual void setUSmallint(const unsigned short& value)
        {
            std::wstringstream ss;
            ss << value;
            this->getValue() = ss.str();
            this->m_isnull = false;
        }





    virtual i18n::UString asStr() const
        {
            return this->getValue();
        }

    virtual i18n::UString asStr(std::locale loc) const
        {
            return this->getValue(); // no conversion with locale needed
        }

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



    virtual i18n::UString asStr() const
        {
            return this->getValue()->asStr();
        }

    virtual i18n::UString asStr(std::locale loc) const
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



    virtual i18n::UString asStr() const
        {
            return this->getValue()->asStr();
        }

    virtual i18n::UString asStr(std::locale loc) const
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
i18n::UString   BaseVariantImplFor<Base>::asStr(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_VARCHAR); }

template<class Base>
i18n::UString   BaseVariantImplFor<Base>::asStr(std::locale loc) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_VARCHAR); }

template<class Base>
std::string     BaseVariantImplFor<Base>::asNarrowStr(const char *charset) const 
{ return i18n::conv_to(this->asStr(), charset); }

template<class Base>
std::string     BaseVariantImplFor<Base>::asNarrowStr(const char *charset, std::locale loc) const
{ return i18n::conv_to(this->asStr(loc), charset); }

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
void            BaseVariantImplFor<Base>::setStr(const i18n::UString&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setStr(const char* data, std::size_t len, const char* charset) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setStr(const std::string&, const char* charset) 
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



//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
