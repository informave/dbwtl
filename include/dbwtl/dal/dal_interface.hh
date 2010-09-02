//
// dal_interface.hh - Public DAL types and classes
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
/// @brief Public DAL types and classes
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_INTERFACES_HH
#define INFORMAVE_DB_DAL_INTERFACES_HH


#define DAL_DEV_INCLUDE_DEVUTILS

#include "dbwtl/dal/dal_fwd.hh"
//#include "db_exceptions.hh"
//#include "dbwtl/i18n/ustring.hh"
#include "dbwtl/util/smartptr.hh"

#ifdef DAL_DEV_INCLUDE_DEVUTILS
#include "dbwtl/util/devutils.hh"
#endif

#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <memory>
#include <fstream>
#include <deque>
#include <algorithm>
#include <sstream>



DAL_NAMESPACE_BEGIN



///
/// @brief Type for row IDs
typedef signed long        rowid_t;

#define DAL_TYPE_ROWID_FIRST_ROW   1      /// Row IDs starts at 1
#define DAL_TYPE_ROWID_NPOS        0      /// not positioned

typedef signed long        rowcount_t;

///
/// @brief Type for column numbers
typedef size_t             colnum_t;

#define DAL_TYPE_COLNUM_FIRST_COLUMN  1  /// Columns start at 1
#define DAL_TYPE_COLNUM_BOOKMARK      0  /// Bookmark columns


///
/// @brief DBMS dependend TYPE ID
typedef signed int systype_t;



#define DAL_MSG_SUCCESS                 L"Success."



typedef enum
{
	DAL_STATE_OK,
	DAL_STATE_INFO,
	DAL_STATE_ERROR
} dalstate_t;


///
/// @brief Get the name of the code as string
const char* dal_state_msg(int code);

#define DAL_CODE(codename) dal_state_msg(codename)





#define DAL_VARIANT_ACCESSOR          typedef void __accessor_is_defined
#define DAL_VARIANT_ACCESSOR_CHECK    typedef typename storage_accessor<T>::__accessor_is_defined __compiler_check




//--------------------------------------------------------------------------
/// All types defined by the DAL
/// 
/// @since 0.0.1
/// @brief DAL datatypes
enum DatatypeEnumeration
{
    DAL_TYPE_CUSTOM = 1,
    DAL_TYPE_UNKNOWN,
    DAL_TYPE_INT,
    DAL_TYPE_UINT,
    DAL_TYPE_CHAR,
    DAL_TYPE_UCHAR,
    DAL_TYPE_VARCHAR,
    DAL_TYPE_NVARCHAR,
    DAL_TYPE_NCHAR,
    DAL_TYPE_BOOL,
    DAL_TYPE_SMALLINT,
    DAL_TYPE_USMALLINT,
    DAL_TYPE_BIGINT,
    DAL_TYPE_UBIGINT,
    //DAL_TYPE_BIT,
    //DAL_TYPE_VARBIT,
    DAL_TYPE_BLOB,
    DAL_TYPE_MEMO,
    DAL_TYPE_NUMERIC,
    DAL_TYPE_DECIMAL,
    DAL_TYPE_MONEY,
    DAL_TYPE_FLOAT,
    DAL_TYPE_DOUBLE,
    DAL_TYPE_DATE,
    DAL_TYPE_TIME,
    DAL_TYPE_DATETIME,
    DAL_TYPE_TIMESTAMP,
    DAL_TYPE_INTERVAL,
    DAL_TYPE_CIDR,
    DAL_TYPE_MACADDR,
    DAL_TYPE_INETADDR,
    //DAL_TYPE_POINT,
    //DAL_TYPE_POLYGON,
    //DAL_TYPE_LINE,
    //DAL_TYPE_CIRCLE,
    //DAL_TYPE_LINESEG,
    //DAL_TYPE_PATH,
    //DAL_TYPE_BOX,
    DAL_TYPE_UUID,
    DAL_TYPE_XML
};

typedef enum DatatypeEnumeration daltype_t;


/// @brief Maps a daltype ID to a string name
i18n::UString daltype2string(daltype_t type);

i18n::UString daltype2sqlname(daltype_t type);




//--------------------------------------------------------------------------
/// Controller for diagnostic records
///
/// @since 0.0.1
/// @brief Controller for diagnostic records
template<class T>
class DiagController
{
public:
    DiagController(void)
        : m_list(),
          m_cur()
    {
        m_cur = m_list.begin();
    }
    

    /// Free all records
    ~DiagController(void)
    {
        std::for_each(this->m_list.begin(), this->m_list.end(), delete_object());
    }


    /// Push back a diagnostic record. The livetime is now controlled by
    /// this controller class.
    ///
    /// @todo Use an option variable for the max. size of the deque.
    void push_back(T *diag)
    {
        this->m_list.push_back(diag);
    	if(m_cur == m_list.end())
            m_cur = m_list.begin();

        // remove older records
        if(m_list.size() > 10)
        {
            typename std::deque<T*>::iterator
                from = this->m_list.begin(),
                to = this->m_list.begin();

            std::advance(to, m_list.size() - 10);
            if(this->m_cur <= to)
            {
                this->m_cur = to;
                this->m_cur++;
            }
            std::for_each(from, to, delete_object());
            this->m_list.erase(from, to);
        }
    }
    
    
    /// Checks if there are any records available
    bool diagAvail(void) const
    {
        return this->m_cur != this->m_list.end();
    }

    
    /// The reference keeps valid until no other method is called.
    const T& fetchDiag(void)
    {
        if(this->m_cur != this->m_list.end())
        {
            return **this->m_cur++;
        }
        else
            throw std::range_error("No diagnostic records available");
    }

protected:
    std::deque<T*>                     m_list;
    typename std::deque<T*>::iterator  m_cur;
};




//--------------------------------------------------------------------------
/// Interface for diagnostic records
///
/// @since 0.0.1
/// @brief Interface for diagnostic records
class DBWTL_EXPORT IDiagnostic
{
public:
    virtual ~IDiagnostic(void)
    {}

    virtual dalstate_t         getState(void) const = 0;
//     virtual const Variant&     getQuery(void) const = 0;
//     virtual const Variant&     getNativeErrorCode(void) const = 0;
    virtual const Variant&     getMsg(void) const = 0;
    virtual const Variant&     getDescription(void) const = 0;
    virtual const Variant&     getCodepos(void) const = 0;
    virtual const Variant&     getSqlstate(void) const = 0;

//     virtual const Variant&     getRowNumber(void) const = 0;
//     virtual const Variant&     getServerName(void) const = 0;
//     virtual const Variant&     getColumnNumber(void) const = 0;
//     virtual const Variant&     getUsedFiles(void) const = 0;


    /// SQLSTATE exceptions stores an own copy of a diagnostic record.
    /// It must be save to clone a complete record even if the handle that
    /// has initially created the record is destroyed.
    virtual IDiagnostic* clone(void) const = 0;

    /// Dumps the information as as string
    virtual i18n::UString str(void) const = 0;

    /// Checks the stored information and error codes and raises a
    /// appropriate SQLSTATE exception.
    virtual void raiseException(void) const = 0;
};




//--------------------------------------------------------------------------
/// @brief Base class for all DAL classes
class DBWTL_EXPORT IDALObject
{
public:
    virtual ~IDALObject(void) { }
};




//--------------------------------------------------------------------------
/// @brief Base class for Handles
class DBWTL_EXPORT IHandle : public IDALObject
{
public:
    IHandle(void);
    virtual ~IHandle(void) {}

    virtual bool                 diagAvail(void) const = 0;

    /// The reference keeps valid until no other method is called.
    virtual const IDiagnostic&   fetchDiag(void) = 0;
};



//--------------------------------------------------------------------------
/// @brief DAL Interface for drivers
class DBWTL_EXPORT IDALDriver : public IDALObject
{
public:
    virtual ~IDALDriver(void) { }
};




//--------------------------------------------------------------------------
/// @brief DAL Interface for BLOB buffers
class IBlobBuffer : public std::streambuf
{
public:
protected:
};



//--------------------------------------------------------------------------
/// @brief DAL Interface for MEMO buffers
class IMemoBuffer : public std::wstringbuf
{
public:
protected:
};



//--------------------------------------------------------------------------
/// @brief BLOB stream
class Blob : public std::istream
{
public:
	Blob(std::streambuf *buf);
	virtual ~Blob();

protected:
    std::streambuf *m_buf;
private:
	Blob(const Blob&);
	Blob& operator=(const Blob&);
};



//--------------------------------------------------------------------------
/// @brief MEMO stream
class Memo : public std::wistream
{
public:
	Memo(std::wstreambuf *buf);
	virtual ~Memo(void);

	std::string   narrow_str(const char *charset) const;
	std::wstring  str() const;

protected:
	std::wstreambuf *m_buf;
private:
	Memo(const Memo&);
	Memo& operator=(const Memo&);
};






/// @todo just a workaround for now
#if defined(DBWTL_ON_WIN32)
typedef long long int64_t;
#endif



//--------------------------------------------------------------------------
/// @brief DAL Interface for column descriptors
class DBWTL_EXPORT IColumnDesc : public IDALObject
{
public:
    typedef IVariant value_type;
    //typedef util::SmartPtr<ITable, util::RefCounted, util::AllowConversion> ptr;

    /// Empty virtual destructor
    virtual ~IColumnDesc(void) { }

    /// @brief Returns the name of the column
    virtual const value_type& getName(void) const = 0;
    virtual const value_type& getCatalogName(void) const = 0;
    virtual const value_type& getSchemaName(void) const = 0;
    virtual const value_type& getBaseColumnName(void) const = 0;
    virtual const value_type& getTypeName(void) const = 0;
    virtual const value_type& getBaseTableName(void) const = 0;
    virtual const value_type& getSize(void) const = 0;
    virtual const value_type& getComment(void) const = 0;
    virtual const value_type& getIsNullable(void) const = 0;
    virtual const value_type& getPrecision(void) const = 0;
    virtual const value_type& getScale(void) const = 0;
    virtual const value_type& getIsSearchable(void) const = 0;

    virtual daltype_t getDatatype(void) const = 0;
};



//--------------------------------------------------------------------------
/// @brief DAL Interface for variant types
class DBWTL_EXPORT IVariant : public IDALObject
{
public:
    virtual ~IVariant(void) { }

    //  basic functions
    virtual daltype_t   datatype(void) const = 0;
    virtual bool        isnull(void) const = 0;
    virtual void        setNull(void) = 0;
    virtual void        assign(const IVariant& var);

    // getter methods
    virtual signed int           asInt(void) const = 0;
    virtual unsigned int         asUInt(void) const = 0;
    virtual signed char          asChar(void) const = 0;
    virtual unsigned char        asUChar(void) const = 0;

    virtual i18n::UString        asStr(void) const = 0;
    virtual i18n::UString        asStr(std::locale loc) const = 0;
    virtual std::string          asNarrowStr(const char *charset) const = 0;
    virtual std::string          asNarrowStr(const char *charset, std::locale loc) const = 0;

    virtual bool                 asBool(void) const = 0;
    virtual signed short         asSmallint(void) const = 0;
    virtual unsigned short       asUSmallint(void) const = 0;
    virtual signed long long     asBigint(void) const = 0;
    virtual unsigned long long   asUBigint(void) const = 0;
    virtual TNumeric             asNumeric(void) const = 0;
    virtual TMoney               asMoney(void) const = 0;
    virtual float                asReal(void) const = 0;
    virtual double               asDouble(void) const = 0;
    virtual TDate                asDate(void) const = 0;
    virtual TTime                asTime(void) const = 0;
    virtual TDatetime            asDatetime(void) const = 0;
    virtual signed int           asTimestamp(void) const = 0;
    //virtual TCustom&        asCustom(void) const = 0;
    virtual TCidr                asCIDR(void) const = 0;
    virtual TInterval            asInterval(void) const = 0;
    virtual TMacaddr             asMacaddr(void) const = 0;
    virtual TInetaddr            asInetaddr(void) const = 0;
    virtual TUuid                asUUID(void) const = 0;
    virtual TXml                 asXML(void) const = 0;
    virtual std::streambuf*      asBlob(void) const = 0;
    virtual std::wstreambuf*     asMemo(void) const = 0;

    // setter methods
    virtual void                 setInt(const signed int&) = 0;
    virtual void                 setUInt(const unsigned int&) = 0;
    virtual void                 setChar(const signed char&) = 0;
    virtual void                 setUChar(const unsigned char&) = 0;
    virtual void                 setStr(const i18n::UString&) = 0;
    virtual void                 setStr(const char* data, std::size_t len, const char* charset) = 0;
    virtual void                 setStr(const std::string&, const char* charset) = 0;
    virtual void                 setBool(const bool&) = 0;
    virtual void                 setSmallint(const signed short&) = 0;
    virtual void                 setUSmallint(const unsigned short&) = 0;
    virtual void                 setBigint(const signed long long&) = 0;
    virtual void                 setUBigint(const unsigned long long&) = 0;
    virtual void                 setNumeric(const TNumeric&) = 0;
    virtual void                 setMoney(const TMoney&) = 0;
    virtual void                 setReal(const float&) = 0;
    virtual void                 setDouble(const double&) = 0;
    virtual void                 setDate(const TDate&) = 0;
    virtual void                 setTime(const TTime&) = 0;
    virtual void                 setDatetime(const TDatetime&) = 0;
    virtual void                 setTimestamp(const signed int&) = 0;
    //virtual void           asCustom(void) const = 0;
    virtual void                 setCIDR(const TCidr&) = 0;
    virtual void                 setInterval(const TInterval&) = 0;
    virtual void                 setMacaddr(const TMacaddr&) = 0;
    virtual void                 setInetaddr(const TInetaddr&) = 0;
    virtual void                 setUUID(const TUuid&) = 0;
    virtual void                 setXML(const TXml&) = 0;
    virtual void                 setBlob(std::streambuf*) = 0;
    virtual void                 setMemo(std::wstreambuf*) = 0;
  
private:
    IVariant&               operator=(const IVariant& o);
};



DBWTL_EXPORT std::wostream&  operator<<(std::wostream& o, const IVariant &var);
DBWTL_EXPORT std::ostream&   operator<<(std::ostream& o,  const IVariant &var);




//--------------------------------------------------------------------------
///
/// @cond DEV_DOCS
/// @brief Base implementation for Variant
template<class Base>
class DBWTL_EXPORT BaseVariantImplFor : public Base
{
public:
    BaseVariantImplFor(void) : m_isnull(true)
    {
    }

    virtual bool            isnull(void) const;
    virtual void            setNull(void);

    virtual signed int      asInt(void) const;
    virtual unsigned int    asUInt(void) const;
    virtual signed char     asChar(void) const;
    virtual unsigned char   asUChar(void) const;


    virtual i18n::UString   asStr(void) const;
    virtual i18n::UString   asStr(std::locale loc) const;
    virtual std::string     asNarrowStr(const char *charset) const;
    virtual std::string     asNarrowStr(const char *charset, std::locale loc) const;


    virtual bool                  asBool(void) const;
    virtual signed short          asSmallint(void) const;
    virtual unsigned short        asUSmallint(void) const;
    virtual signed long long      asBigint(void) const;
    virtual unsigned long long    asUBigint(void) const;
    virtual TNumeric              asNumeric(void) const;
    virtual TMoney                asMoney(void) const;
    virtual float                 asReal(void) const;
    virtual double                asDouble(void) const;
    virtual TDate                 asDate(void) const;
    virtual TTime                 asTime(void) const;
    virtual TDatetime             asDatetime(void) const;
    virtual signed int            asTimestamp(void) const;
    //virtual TCustom&        asCustom(void) const = 0;
    virtual TCidr                 asCIDR(void) const;
    virtual TInterval             asInterval(void) const;
    virtual TMacaddr              asMacaddr(void) const;
    virtual TInetaddr             asInetaddr(void) const;
    virtual TUuid                 asUUID(void) const;
    virtual TXml                  asXML(void) const;
    virtual std::streambuf*       asBlob(void) const;
    virtual std::wstreambuf*      asMemo(void) const;


    virtual void            setInt(const signed int&);
    virtual void            setUInt(const unsigned int&);
    virtual void            setChar(const signed char&);
    virtual void            setUChar(const unsigned char&);
    virtual void            setStr(const i18n::UString&);
    virtual void            setStr(const char* data, std::size_t len, const char* charset);
    virtual void            setStr(const std::string&, const char* charset);
    virtual void            setBool(const bool&);
    virtual void            setSmallint(const signed short&);
    virtual void            setUSmallint(const unsigned short&);
    virtual void            setBigint(const signed long long&);
    virtual void            setUBigint(const unsigned long long&);
    virtual void            setNumeric(const TNumeric&);
    virtual void            setMoney(const TMoney&);
    virtual void            setReal(const float&);
    virtual void            setDouble(const double&);
    virtual void            setDate(const TDate&);
    virtual void            setTime(const TTime&);
    virtual void            setDatetime(const TDatetime&);
    virtual void            setTimestamp(const signed int&);
    //virtual void        asCustom(void) const = 0;
    virtual void            setCIDR(const TCidr&);
    virtual void            setInterval(const TInterval&);
    virtual void            setMacaddr(const TMacaddr&);
    virtual void            setInetaddr(const TInetaddr&);
    virtual void            setUUID(const TUuid&);
    virtual void            setXML(const TXml&);
    virtual void            setBlob(std::streambuf*);
    virtual void            setMemo(std::wstreambuf*);


protected:
    bool m_isnull;
};

//--------------------------------------------------------------------------
///
/// @brief Internal stored variants needs a clone method
class DBWTL_EXPORT IStoredVariant : public IVariant
{
public:
    virtual IStoredVariant* clone(void) const = 0;

    virtual void releasePointee(void) = 0;
};



//--------------------------------------------------------------------------
/// Defines the interface for all accessors which can be used
/// to get the stored variable. 
///
/// @brief Storage Accessor Base
template<class T>
class sa_base : public IStoredVariant
{
public:
    virtual ~sa_base(void) { }

    virtual const T& getValue() const = 0;
    virtual T&       getValue() = 0;
};
/// @endcond


//--------------------------------------------------------------------------
/// Defines an empty class. For every type which should
/// be stored, a specialization must be defined.
///
/// @brief Storage Accessor, empty template
template<class T>
class storage_accessor : public sa_base<T>
{
/*
  This are the minimum of members which are required in specializations.

  public:

  DAL_VARIANT_ACCESSOR;

  virtual ~storage_accessor(void) { }
  
  daltype_t datatype(void) const
  {
  return DAL_TYPE_CUSTOM;
  }
*/
};




//--------------------------------------------------------------------------
/// @brief Deletes a Pointee (if T is a pointer)
template<class T>
struct pointee_deleter
{ explicit pointee_deleter(const T* pointer) { } };

template<class T>
struct pointee_deleter<T*>
{ explicit pointee_deleter(T** pointer) { delete *pointer; *pointer = 0; } };




//--------------------------------------------------------------------------
/// Implements the interface for the accessor methods so they
/// can get the saved value.
///
/// @brief Variant Storage
template<class T>
class var_storage : public storage_accessor<T>
{
    /// check if there is a storage_accessor for T defined
    DAL_VARIANT_ACCESSOR_CHECK;

    typedef var_storage<T> self_t;

public:
    var_storage(void) : m_var()
        {
            this->m_isnull = true;
        }

    explicit var_storage(T v) : m_var(v)
        { 
            this->m_isnull = false;
        }

    var_storage(const self_t& orig) : m_var(orig.m_var)
        {
            this->m_isnull = orig.m_isnull;
        }

    virtual ~var_storage(void)
        { }

    self_t& operator=(const self_t& o)
        {
            this->m_var = o.m_var;
            return *this;
        }

    virtual self_t* clone() const
        {
            return new self_t(*this);
        }

    virtual const T& getValue() const
        {
            return this->m_var;
        }

    virtual T& getValue()
        {
            return this->m_var;
        }

    virtual void releasePointee(void)
        {
            pointee_deleter<T> deleter(&this->m_var);
        }


protected:
    T m_var;
};



/// @brief create a new default storage object
IStoredVariant* new_default_storage(daltype_t type);



//------------------------------------------------------------------------------
/// Dispatches the template argument type to a variant getter method
/// Required specialization for each type which should be dispatched.
///
/// @since 0.0.1
/// @brief Dispatches the template argument type to a variant getter method
template<typename T>
struct variant_dispatch_method
{};




//------------------------------------------------------------------------------
///
/// @brief Base class for variant_dispatch_method
struct variant_dispatch_storage
{
protected:
    explicit variant_dispatch_storage(const IVariant &var) : m_var(var)
    {}

    inline const IVariant& get(void) const
    { return this->m_var; }

private:
    const IVariant &m_var;
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
    /// A helper class to the Variant() constructor if you want to create
    /// a empty Variant object (set to null).
    //struct NullValue { };


    /// This constructor creates a Variant object from the given value
    /// of type T and initializes the storage accessor of type
    /// storage_accessor<T> with the given value.
    /// The optional name can be used to identify the value (e.g. exeptions).
    /// @brief Creates a Variant object from the given value/type
    template<class T>
        Variant(T value, const i18n::UString &name = i18n::UString(L"<unnamed>"))
        : IVariant(),
        m_storage(new var_storage<T>(value)),
        m_name(name),
        m_type(DAL_TYPE_UNKNOWN)
        {}
    

    /// This constructor creates a Variant object from another Variant
    /// object.
    /// @brief Copy constructor for Variant objects
    /// @bug should we really use clone()?
    Variant(const Variant& v)
        : IVariant(),
        m_storage(),
        m_name(v.m_name),
        m_type(v.m_type)
        {
            if(! v.isnull())
                m_storage.reset(v.m_storage->clone());
        }
    

    /// Assign a new value to the variant.
    /// The name, storage etc. are preserved, we only set a
    /// new value for the internal variant_storage.
    Variant& operator=(const Variant &v)
        {
            this->assign(v);
            return *this;
        }

    Variant& operator=(const IVariant &v)
        {
            this->assign(v);
            return *this;
        }

    
    /// This constructor can be used to create a empty Variant object.
    /// The storage accessor is initialized if any of the set-methods is
    /// called.
    /// @brief Creates an empty Variant object
    explicit Variant(daltype_t type = DAL_TYPE_UNKNOWN,
                     const i18n::UString &name = i18n::UString(L"<unnamed>"))
        : IVariant(),
        m_storage(),
        m_name(name),
        m_type(type)
        {
            this->m_storage.reset(new_default_storage(type));
        }


    virtual const i18n::UString& getName(void) const;

    typedef util::SmartPtr<IStoredVariant,
        util::RefCounted,
        util::AllowConversion> storage_type;


    
    /// @brief Check the value object for NULL
    virtual bool isnull(void) const;


    /// @brief Set the value to NULL
    virtual void setNull(void);


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



    virtual i18n::UString       asStr(void) const;
    virtual i18n::UString       asStr(std::locale loc) const;
    virtual std::string         asNarrowStr(const char *charset) const;
    virtual std::string         asNarrowStr(const char *charset, std::locale loc) const;
    virtual bool                asBool(void) const;
    virtual signed short        asSmallint(void) const;
    virtual unsigned short      asUSmallint(void) const;
    virtual signed long long    asBigint(void) const;
    virtual unsigned long long  asUBigint(void) const;
    virtual TNumeric            asNumeric(void) const;
    virtual TMoney              asMoney(void) const;
    virtual float               asReal(void) const;
    virtual double              asDouble(void) const;
    virtual TDate               asDate(void) const;
    virtual TTime               asTime(void) const;
    virtual TDatetime           asDatetime(void) const;
    virtual signed int          asTimestamp(void) const;
    //virtual TCustom&        asCustom(void) const = 0;
    virtual TCidr               asCIDR(void) const;
    virtual TInterval           asInterval(void) const;
    virtual TMacaddr            asMacaddr(void) const;
    virtual TInetaddr           asInetaddr(void) const;
    virtual TUuid               asUUID(void) const;
    virtual TXml                asXML(void) const;
    virtual std::streambuf*     asBlob(void) const;
    virtual std::wstreambuf*    asMemo(void) const;


    virtual void            setInt(const signed int&);
    virtual void            setUInt(const unsigned int&);
    virtual void            setChar(const signed char&);
    virtual void            setUChar(const unsigned char&);
    virtual void            setStr(const i18n::UString&);
    virtual void            setStr(const char* data, std::size_t len, const char* charset);
    virtual void            setStr(const std::string&, const char* charset);
    virtual void            setBool(const bool&);
    virtual void            setSmallint(const signed short&);
    virtual void            setUSmallint(const unsigned short&);
    virtual void            setBigint(const signed long long&);
    virtual void            setUBigint(const unsigned long long&);
    virtual void            setNumeric(const TNumeric&);
    virtual void            setMoney(const TMoney&);
    virtual void            setReal(const float&);
    virtual void            setDouble(const double&);
    virtual void            setDate(const TDate&);
    virtual void            setTime(const TTime&);
    virtual void            setDatetime(const TDatetime&);
    virtual void            setTimestamp(const signed int&);
    //virtual void        asCustom(void) const = 0;
    virtual void            setCIDR(const TCidr&);
    virtual void            setInterval(const TInterval&);
    virtual void            setMacaddr(const TMacaddr&);
    virtual void            setInetaddr(const TInetaddr&);
    virtual void            setUUID(const TUuid&);
    virtual void            setXML(const TXml&);
    virtual void            setBlob(std::streambuf*);
    virtual void            setMemo(std::wstreambuf*);


protected:
    IStoredVariant*       getStorageImpl(void);
    const IStoredVariant* getStorageImpl(void) const;

    /// @brief Internal storage of the value
    storage_type      m_storage;
    
    /// @brief Name of the variant
    i18n::UString     m_name;

    /// This type is only used if m_storage is not initialized.
    daltype_t         m_type;
};




//------------------------------------------------------------------------------
/// The ITable interface represents a single table.
/// All values returned a variant types because some of them may be null.
///
/// @since 0.0.1
/// @brief DAL Interface for a single table
class DBWTL_EXPORT ITable : IDALObject
{
public:
    typedef IVariant value_type;
    typedef util::SmartPtr<ITable, util::RefCounted, util::AllowConversion> ptr;
    
    /// Empty virtual destructor
    virtual ~ITable(void) { }
    
    /// @brief Returns the name of the table
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the name of the catalog
    virtual const value_type& getCatalog(void) const = 0;

    /// @brief Returns the name of the schema
    virtual const value_type& getSchema(void) const = 0;

    /// @brief Returns the comment for the table
    virtual const value_type& getComment(void) const = 0;

    /// @brief Returns the DDL statement for the table
    virtual const value_type& getDDL(void) const = 0;

    /// @brief Returns true/false if the table is a system object
    /// @return a null value if is not determinable
    virtual const value_type& isSystemObject(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for types
class DBWTL_EXPORT IDatatype : IDALObject
{
public:
    typedef util::SmartPtr<IDatatype,
        util::RefCounted,
        util::AllowConversion> ptr;

    typedef IVariant value_type;

    /// @brief Returns the name of the table
    virtual const value_type& getName(void) const = 0;

    /// @brief Returns the maximum size for variable length types
    /// or the size for fixed length types (like INT)
    virtual const value_type& getSize(void) const = 0;

    virtual const value_type& getLiteralPrefix(void) const = 0;

    virtual const value_type& getLiteralSuffix(void) const = 0;

    virtual const value_type& getIsUnsigned(void) const = 0;

    virtual daltype_t         daltype(void) const = 0;
};



//------------------------------------------------------------------------------
///
/// @brief DAL Interface for views
class DBWTL_EXPORT IView : IDALObject
{
public:
    typedef util::SmartPtr<IView,
        util::RefCounted,
        util::AllowConversion> ptr;
    
    virtual ~IView(void) { }
};


typedef std::vector<ITable::ptr>      TableList;
typedef std::vector<IDatatype::ptr>   DatatypeList;
typedef std::vector<IView::ptr>       ViewList;





//------------------------------------------------------------------------------
///
/// @brief Interface for Table Filters
class DBWTL_EXPORT ITableFilter : public IDALObject
{
public:
    virtual ~ITableFilter(void) { }

    virtual i18n::UString simpleNameFilter(void) const = 0;
    virtual i18n::UString simpleCatalogFilter(void) const = 0;
    virtual i18n::UString simpleSchemaFilter(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief Empty Table Filter implementation
class DBWTL_EXPORT EmptyTableFilter : public ITableFilter
{
public:
    virtual ~EmptyTableFilter(void) { }

    virtual i18n::UString simpleNameFilter(void) const;
    virtual i18n::UString simpleCatalogFilter(void) const;
    virtual i18n::UString simpleSchemaFilter(void) const;
};




//------------------------------------------------------------------------------
///
/// @brief Interface for Datatype Filters
class DBWTL_EXPORT IDatatypeFilter : public IDALObject
{
public:
    virtual ~IDatatypeFilter(void) { }

    virtual i18n::UString simpleNameFilter(void) const = 0;
    virtual i18n::UString simpleCatalogFilter(void) const = 0;
    virtual i18n::UString simpleSchemaFilter(void) const = 0;
};



//------------------------------------------------------------------------------
///
/// @brief Empty Datatype Filter implementation
class DBWTL_EXPORT EmptyDatatypeFilter : public IDatatypeFilter
{
public:
    virtual ~EmptyDatatypeFilter(void) { }

    virtual i18n::UString simpleNameFilter(void) const;
    virtual i18n::UString simpleCatalogFilter(void) const;
    virtual i18n::UString simpleSchemaFilter(void) const;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for Environments
class DBWTL_EXPORT IEnv : public IHandle
{
public:
    typedef std::auto_ptr<IEnv> ptr;

    virtual ~IEnv(void) { }

    virtual IDbc* newConnection(void) = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for connections
class DBWTL_EXPORT IDbc : public IHandle
{
public:
    typedef std::auto_ptr<IDbc>                      ptr;
    typedef std::map<i18n::UString, i18n::UString>   Options;

    /// @brief Connection transaction modes
    enum trx_mode
    {
        trx_read_uncommitted,   /// SQL:2003 TRX UNCOMMITTED
        trx_read_committed,     /// SQL:2003 TRX COMMITTED
        trx_repeatable_read,    /// SQL:2003 TRX REPEATABLE READ
        trx_serializable        /// SQL:2003 TRX SERIALIZABLE
    };

    
    /// @brief Transaction access mode
    enum access_mode
    {
        trx_readwrite,
        trx_readonly,
        trx_default        /// default is the default of the database engine
    };


    virtual ~IDbc(void);

    ///
    /// @brief Connect to a database by name, user and password
    ///
    /// The value of database depends on the selected driver.
    /// Some drivers (filebased) requires a path to a directory or file
    /// and other drivers needs the name of the database.
    virtual void     connect(i18n::UString database,
                             i18n::UString user = i18n::UString(),
                             i18n::UString password = i18n::UString()) = 0;


    ///
    /// @brief Connect to a database by options
    virtual void     connect(IDbc::Options& options) = 0;


    virtual bool           isConnected(void) const = 0;
    virtual void     disconnect(void) = 0;
    virtual i18n::UString  driverName(void) const = 0;
    virtual i18n::UString  dbmsName(void) const = 0;
    virtual IStmt*         newStatement(void) = 0;
    virtual IDALDriver*    getDriver(void) const = 0;
    virtual void           beginTrans(IDbc::trx_mode mode,
                                      IDbc::access_mode access = IDbc::trx_default,
                                      i18n::UString name = i18n::UString()) { };
    virtual void           commit(void) { };
    virtual void           savepoint(i18n::UString name) { }
    virtual void           rollback(i18n::UString name = i18n::UString()) { }
    virtual void           directCmd(i18n::UString cmd) { }
    virtual std::string    getDbcEncoding(void) const = 0;


    /// @bug can these be const methods?
    virtual TableList      getTables(const ITableFilter& = EmptyTableFilter()) = 0;
    virtual DatatypeList   getDatatypes(const IDatatypeFilter& = EmptyDatatypeFilter())
    { 
        throw std::runtime_error("not impl");
    }


    /*
      virtual ColumnList     getTables(const IColumnFilter& = EmptyColumnFilter()) = 0;
      virtual SchemaList     getSchemas(const ISchemaFilter& = EmptySchemaFilter()) = 0;
      virtual CatalogList    getCatalogs(const ICatalogFilter& = EmptyCatalogFilter()) = 0;
      virtual ViewList       getViews(const IViewFilter& = EmptyViewFilter()) = 0;
      virtual ProcedureList  getProcedures(const IProcedureFilter& = EmptyProcedureFilter()) = 0;
      virtual ProcColumnList getProcColumns(const IProcColumnFilter& = EmptyProcColumnFilter()) = 0;
    */


protected:
    virtual void           setDbcEncoding(std::string encoding) = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for resultsets
class DBWTL_EXPORT IResult : public IDALObject
{
public:
    typedef std::auto_ptr<IResult> ptr;
    typedef size_t                      bookmark_type;
    typedef IVariant                    value_type;
    typedef std::vector<value_type*>    row_type; // NOT NEEDED ???????????????????????????????????????


    virtual ~IResult(void);


    virtual bool   isPrepared(void) const = 0;
    virtual bool   isBad(void) const = 0;

    virtual void   first(void) = 0;
    virtual void   next(void) = 0;
    virtual bool   eof(void) const = 0;
    virtual bool   isOpen(void) const = 0;
    virtual void   close(void) = 0;

    // row methods
    virtual rowcount_t       rowCount(void) const = 0;
    virtual size_t           paramCount(void) const = 0;
    virtual rowcount_t       affectedRows(void) const = 0;

    virtual const value_type&      column(colnum_t num) = 0;
    virtual const value_type&      column(i18n::UString name) = 0;
    //virtual value_type&          field(colnum_t num) = 0;
    //virtual variant_type&        field(i18n::UString name) = 0;

    // column methods
    virtual size_t           columnCount(void) const = 0;
    virtual colnum_t         columnID(i18n::UString name) const = 0;
    virtual i18n::UString    columnName(colnum_t num) const = 0;
    //virtual const ITypeInfo& datatype(colnum_t num) const = 0;

    /// @brief Returns the column descriptor for the given column number
    virtual const IColumnDesc& describeColumn(colnum_t num) const = 0;

    /// @brief Returns the column descriptor for the given column name
    virtual const IColumnDesc& describeColumn(i18n::UString name) const = 0;


    virtual IDALDriver* getDriver(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for statements
class DBWTL_EXPORT IStmt : public IHandle
{
public:
    typedef std::map<int, IVariant*> ParamMapT;
    typedef std::auto_ptr<IStmt> ptr;

    virtual ~IStmt(void);

    virtual bool      isBad(void) const = 0;
    virtual void      prepare(i18n::UString sql) = 0;
    virtual bool      isPrepared(void) const = 0;
    virtual void      execute(void) = 0;
    virtual void      execDirect(i18n::UString sql) = 0;
    /*
      virtual void      execBatch(std::istream src, const char *charset) = 0;
      virtual void      execBatch(std::wistream src) = 0;
    */
    virtual void      close(void) = 0;

    virtual IResult&  resultset(void) = 0;
    virtual const IResult&  resultset(void) const = 0;

    //virtual void      setResultsetMode(ResultsetMode mode) = 0;

    virtual bool      nextResultset(void) = 0;
    virtual bool      moreResultsets(void) const = 0;

    virtual size_t    paramCount(void) const = 0;
    virtual void      bind(int num, IVariant* data) = 0;
    virtual void      bind(int num, const IVariant* data) = 0;
    virtual void      bind(int num, Variant data) = 0; /// @bug are we using clone() here? is this ok?
    virtual void      bind(int num, std::streambuf *data) = 0;
    virtual void      bind(int num, std::wstreambuf *data) = 0;

    virtual rowcount_t  affectedRows(void) const = 0;


    virtual IDALDriver* getDriver(void) const = 0;
    //virtual IDbc& getConnection(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Factory
class DBWTL_EXPORT Factory
{
public:
    template<class T> DBWTL_EXPORT static typename T::ENV* create(i18n::UString driver)
    {
        typename T::ENV* env = T::createEnv(driver);
        return env;
    }

    //static IEnv* createByName(i18n::UString name, i18n::UString driver); not needed
};






//------------------------------------------------------------------------------
///
/// @brief DAL Interface for special types
class DBWTL_EXPORT TType : public BaseVariantImplFor<IVariant>
{
public:
    TType(void) : BaseVariantImplFor<IVariant>()
    { this->m_isnull = true; }
};


class DBWTL_EXPORT TCidr : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};
class DBWTL_EXPORT TDate : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};
class DBWTL_EXPORT TTime : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};
class DBWTL_EXPORT TInterval : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};
class DBWTL_EXPORT TMacaddr : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale

public:
    // converts XX:XX:XX:XX:XX:XX or raises an exception
    static TMacaddr convert(i18n::UString value)
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
class DBWTL_EXPORT TNumeric : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};
class DBWTL_EXPORT TMoney : public TNumeric
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};
class DBWTL_EXPORT TDecimal : public TNumeric
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};
class DBWTL_EXPORT TInetaddr : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};

class DBWTL_EXPORT TUuid : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};


class DBWTL_EXPORT TTimestamp : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};



class DBWTL_EXPORT TDatetime : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};



class DBWTL_EXPORT TXml : public TType
{
public:
    virtual daltype_t  datatype(void) const;
    virtual i18n::UString asStr(std::locale loc) const;
    virtual i18n::UString asStr() const; // using objects locale
};








//--------------------------------------------------------------------------
/// Base implementation for IDiagnostic
///
/// @since 0.0.1
/// @brief Base implementation for IDiagnostic
class DBWTL_EXPORT DiagBase : public IDiagnostic
{
public:
    virtual ~DiagBase(void)
    {}

    DiagBase(dalstate_t state,
             const char *codepos,
             const char *func,
             i18n::UString message,
             i18n::UString description);
    
    DiagBase(const DiagBase& ref);



    virtual dalstate_t         getState(void) const;
//     virtual const Variant&     getQuery(void) const;
//     virtual const Variant&     getNativeErrorCode(void) const;
    virtual const Variant&     getMsg(void) const;
    virtual const Variant&     getDescription(void) const;
    virtual const Variant&     getCodepos(void) const;
    virtual const Variant&     getSqlstate(void) const;
//     virtual const Variant&     getRowNumber(void) const;
//     virtual const Variant&     getServerName(void) const;
//     virtual const Variant&     getColumnNumber(void) const;
//     virtual const Variant&     getUsedFiles(void) const = 0;
//     virtual const Variant&     getSourcePos(void) const = 0;

protected:
    dalstate_t m_state;
    Variant m_sqlstate;
    Variant m_codepos;
    Variant m_func;
    Variant m_message;
    Variant m_description;
};




//------------------------------------------------------------------------------
///
/// @brief Base implementation for IStmt
class DBWTL_EXPORT StmtBase : public IStmt
{
public:
    typedef std::map<int, IVariant*>   ParamMap;
    typedef ParamMap::iterator         ParamMapIterator;


public:
    virtual void      bind(int num, IVariant* data);
    virtual void      bind(int num, const IVariant* data);
    virtual void      bind(int num, Variant data);
    virtual void      bind(int num, std::streambuf *data);
    virtual void      bind(int num, std::wstreambuf *data);

    virtual bool      isPrepared(void) const;

    virtual bool      isBad(void) const;

    //virtual void      setResultsetMode(ResultsetMode mode);

    virtual void      close(void);

    //virtual void      bind(i18n::UString name, IVariant* data);
    //virtual void      bind(i18n::UString name, PodVariant data);
    //virtual int       getParamNumberByName(i18n::UString name) const = 0;


protected:
    StmtBase(void) : m_params(),
        m_temp_params(),
        m_isPrepared(false),
        m_isBad(false)
        { }


    /** @brief Variants* passed by user */
    ParamMap                  m_params;
    /** @brief Temp. POD Variants, destroyed by StmtBase */
    std::vector<Variant*>    m_temp_params;
    bool                      m_isPrepared;
    bool                      m_isBad;
    //ResultsetMode             m_mode;
};





//------------------------------------------------------------------------------
///
/// @brief Base implementation for IResult
class DBWTL_EXPORT ResultBase : public IResult
{
public:
    virtual bool      isPrepared(void) const;
    virtual bool      isBad(void) const;
    virtual bool      isOpen(void) const;



protected:

    ResultBase(void) : m_isPrepared(false),
        m_isOpen(false),
        m_isBad(false)
        { }

    bool    m_isPrepared;
    bool    m_isOpen;
    bool    m_isBad;
};




//------------------------------------------------------------------------------
///
/// 
class DBWTL_EXPORT DbcBase : public IDbc
{
public:
    virtual bool      isConnected(void) const;
    virtual bool      isBad(void) const;


protected:

    DbcBase(void) : m_isConnected(false),
        m_isBad(false)
        { }

    bool    m_isConnected;
    bool    m_isBad;
};



//------------------------------------------------------------------------------
///
///
class DBWTL_EXPORT ColumnDescBase : public IColumnDesc
{
public:
    virtual const IColumnDesc::value_type& getName(void) const;
    virtual const value_type& getCatalogName(void) const;
    virtual const value_type& getSchemaName(void) const;
    virtual const value_type& getBaseColumnName(void) const;
    virtual const value_type& getTypeName(void) const;
    virtual const value_type& getBaseTableName(void) const;
    virtual const value_type& getSize(void) const;
    virtual const value_type& getComment(void) const;
    virtual const value_type& getIsNullable(void) const;
    virtual const value_type& getPrecision(void) const;
    virtual const value_type& getScale(void) const;
    virtual const value_type& getIsSearchable(void) const;


    virtual daltype_t getDatatype(void) const;



protected:
    ColumnDescBase(void);

    virtual ~ColumnDescBase(void);

    Variant m_name;
    Variant m_catalog_name;
    Variant m_schema_name;
    Variant m_base_column_name;
    Variant m_type_name;
    Variant m_base_table_name;
    Variant m_size;
    Variant m_comment;
    Variant m_is_nullable;
    Variant m_precision;
    Variant m_scale;
    Variant m_is_searchable;

    daltype_t m_daltype;
};




//------------------------------------------------------------------------------
///
///
class DBWTL_EXPORT DatatypeBase : public IDatatype
{
public:
    virtual const value_type& getName(void) const;
    virtual const value_type& getSize(void) const;
    virtual const value_type& getLiteralPrefix(void) const;
    virtual const value_type& getLiteralSuffix(void) const;
    virtual const value_type& getIsUnsigned(void) const;
    virtual daltype_t         daltype(void) const;

protected:
    DatatypeBase(void);

    virtual ~DatatypeBase(void);

    Variant m_name;
    Variant m_size;
    Variant m_literal_prefix;
    Variant m_literal_suffix;
    Variant m_is_unsigned;

    daltype_t m_daltype;
};




DAL_NAMESPACE_END



// include the exception declarations
#include "../db_exceptions.hh"













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
DBWTL_VARIANT_DISPATCHER(asMoney, TMoney)
DBWTL_VARIANT_DISPATCHER(asReal, float)
DBWTL_VARIANT_DISPATCHER(asDouble, double)
DBWTL_VARIANT_DISPATCHER(asDate, TDate)
DBWTL_VARIANT_DISPATCHER(asTime, TTime)
DBWTL_VARIANT_DISPATCHER(asDatetime, TDatetime)
//DBWTL_VARIANT_DISPATCHER(asTimestamp, signed int) /// @bug sure? TTimestamp
DBWTL_VARIANT_DISPATCHER(asCIDR, TCidr)
DBWTL_VARIANT_DISPATCHER(asInterval, TInterval)
DBWTL_VARIANT_DISPATCHER(asMacaddr, TMacaddr)
DBWTL_VARIANT_DISPATCHER(asInetaddr, TInetaddr)
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
class storage_accessor<TDecimal> : public BaseVariantImplFor<sa_base<TDecimal> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DECIMAL; }
};


template<>
class storage_accessor<TDecimal*> : public BaseVariantImplFor<sa_base<TDecimal*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DECIMAL; }
};


template<>
class storage_accessor<const TDecimal*> : public BaseVariantImplFor<sa_base<const TDecimal*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_DECIMAL; }
};




//------------------------------------------------------------------------------
///
///
template<>
class storage_accessor<TMoney> : public BaseVariantImplFor<sa_base<TMoney> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_MONEY; }
};


template<>
class storage_accessor<TMoney*> : public BaseVariantImplFor<sa_base<TMoney*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_MONEY; }
};


template<>
class storage_accessor<const TMoney*> : public BaseVariantImplFor<sa_base<const TMoney*> >
{
public:
    DAL_VARIANT_ACCESSOR;
    virtual ~storage_accessor(void) { }
    virtual daltype_t datatype() const { return DAL_TYPE_MONEY; }
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
TMoney          BaseVariantImplFor<Base>::asMoney(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_MONEY); }

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
TDatetime       BaseVariantImplFor<Base>::asDatetime(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_DATETIME); }

template<class Base>
signed int      BaseVariantImplFor<Base>::asTimestamp(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_TIMESTAMP); }


//virtual TCustom&        asCustom(void) const = 0;


template<class Base>
TCidr           BaseVariantImplFor<Base>::asCIDR(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_CIDR); }

template<class Base>
TInterval       BaseVariantImplFor<Base>::asInterval(void) const 
{ throw ex::convert_error(this->datatype(), DAL_TYPE_INTERVAL); }

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
void            BaseVariantImplFor<Base>::setMoney(const TMoney&) 
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
void            BaseVariantImplFor<Base>::setDatetime(const TDatetime&) 
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setTimestamp(const signed int&)
{ throw ex::read_only(); }


//virtual void        asCustom(void) const = 0;

template<class Base>
void            BaseVariantImplFor<Base>::setCIDR(const TCidr&)
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setInterval(const TInterval&)
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

template<class Base>
void            BaseVariantImplFor<Base>::setBlob(std::streambuf*)
{ throw ex::read_only(); }

template<class Base>
void            BaseVariantImplFor<Base>::setMemo(std::wstreambuf*)
{ throw ex::read_only(); }



DAL_NAMESPACE_END

/// @endcond

#endif



//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
