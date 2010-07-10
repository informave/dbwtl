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


///
/// @brief Type for DAL state codes
typedef signed short dalstate_code_t;

#define DALSTATE_OK                      0
#define DALSTATE_ERROR                  -1
#define DALSTATE_BAD_CONNECTION         -2
#define DALSTATE_API_ERR                -3
#define DALSTATE_DB_NOTFOUND            -4
#define DALSTATE_AUTH_ERR               -5
#define DALSTATE_SQL_ERROR              -6
#define DALSTATE_PARAM_UNKNOW           -7
#define DALSTATE_EPERM                  -8
#define DALSTATE_LIB_ERROR              -9

#define DAL_MSG_SUCCESS                 L"Success."


#define DAL_DEFAULT_SQL_CODE  "HY000"


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
/// Interface for engine state implemetations
/// 
/// @since 0.0.1
/// @brief Interface for engine state implemetations
class DBWTL_EXPORT IEngineState
{
public:
    IEngineState(void);

    virtual ~IEngineState(void);

    /// @brief Set a state message
    virtual void setMsg(i18n::UString msg) = 0;


    /// @brief Set a state message
    virtual i18n::UString getMsg(void) const = 0;


    /// @brief Set a state description
    virtual void setDescription(i18n::UString desc) = 0;


    /// @brief Set a state source type
    virtual void setSource(std::string file, std::string func) = 0;


    /// @brief Set the DAL state code
    virtual void setDALCode(int code) = 0;


    /// @brief Get the DAL state code
    virtual int getDALCode(void) const = 0;


    /// @brief Add a file or path which is important for the state
    virtual void addUsedFile(std::string file) = 0;


    /// @brief Set the SQLSTATE
    virtual void setSQLState(std::string state) = 0;

    
    /// @brief Dumps all state contents to a string
    virtual i18n::UString dump(void) const = 0;


    /// @brief Clone a State object
    virtual IEngineState* clone(void) const = 0;
};


DBWTL_EXPORT std::wostream&  operator<<(std::wostream& o, const informave::db::dal::IEngineState& state);
DBWTL_EXPORT std::ostream&   operator<<(std::ostream& o,  const informave::db::dal::IEngineState& state);




//--------------------------------------------------------------------------
/// If a specific driver implementation returns a dalstate object,
/// the object is returned by value.
/// The "real" IEngineState object is encapsulated by this general
/// EngineState class.
/// 
/// @since 0.0.1
/// @brief EngineState Proxy
class DBWTL_EXPORT EngineState : public IEngineState
{
public:
    EngineState(void);
    EngineState(int code);
    EngineState(const IEngineState& newstate);
    EngineState(const EngineState& newstate);

    virtual ~EngineState(void);

    virtual EngineState*        clone(void) const;
    virtual const IEngineState* getImpl(void) const;
    virtual IEngineState*       getImpl(void);


    /// @brief Resets the internal engine state and saves the new code
    EngineState& operator=(int code);


    /// @brief Resets the internal engine state by newstate
    EngineState& operator=(const EngineState& newstate);

    bool operator!=(int code);
    bool operator==(int code);


    /// @brief Set the state message
    virtual void setMsg(i18n::UString msg);


    /// @brief Get the state message
    virtual i18n::UString getMsg(void) const;


    /// @brief Set the state message as UTF-8 String 
    virtual std::string getMsgUTF8(void) const;


    /// @brief Set a state description
    virtual void setDescription(i18n::UString desc);


    /// @brief Set a state source type
    virtual void setSource(std::string file, std::string func);


    /// @brief Set the DAL state code
    virtual void setDALCode(int code);

    
    /// @brief Get the DAL state code
    virtual int getDALCode(void) const;


    /// @brief Add a file or path which is important for the state.
    virtual void addUsedFile(std::string file);


    /// @brief Set the SQLSTATE
    virtual void setSQLState(std::string state);
    

    /// @brief Dumps all state contents to a string
    virtual i18n::UString dump(void) const;


protected:
    std::auto_ptr<IEngineState>  m_state;
    int                          m_state_code;
};

typedef EngineState dalstate_t;




//--------------------------------------------------------------------------
/// Base class for all driver-specific implementations of IEngineState
///
/// @since 0.0.1
/// @brief Base class for all driver-specific implementations of IEngineState
class DBWTL_EXPORT BaseEngineState : public IEngineState
{
public:
    BaseEngineState(void);
    virtual ~BaseEngineState(void);

    virtual void           setMsg(i18n::UString);
    virtual i18n::UString  getMsg(void) const;
    virtual void           setDescription(i18n::UString);
    virtual void           setSource(std::string, std::string);
    virtual void           setDALCode(int);
    virtual int            getDALCode(void) const;
    virtual void           addUsedFile(std::string);
    virtual void           setSQLState(std::string);

protected:
    i18n::UString             m_msg;
    i18n::UString             m_desc;
    std::string               m_srcfile;
    std::string               m_srcfunc;
    int                       m_dalcode;
    std::vector<std::string>  m_files; /// @todo use i18n::UString
    std::string               m_sqlstate;
};




//--------------------------------------------------------------------------
/// @brief Base class for all DAL classes
class DBWTL_EXPORT IDALObject
{
public:
    virtual ~IDALObject(void) { }
};




//--------------------------------------------------------------------------
/// @brief DAL Interface for drivers
class DBWTL_EXPORT IDALDriver : public IDALObject
{
public:
    virtual ~IDALDriver(void) { }
};




//--------------------------------------------------------------------------
/// @brief DAL Interface for BLOB data
class DBWTL_EXPORT IBlob : public IDALObject
{
public:
    enum seekdir { DAL_SEEK_SET, DAL_SEEK_CUR, DAL_SEEK_END };

    virtual ~IBlob(void);

    virtual size_t  read(void *buf, size_t count) = 0;
    virtual size_t  write(void *buf, size_t count) = 0;
    virtual size_t  seek(off_t offset, enum seekdir whence) = 0;
    virtual off_t   tell(void) const = 0;
    virtual size_t  size(void) const = 0;


    /// @brief Checks if BLOB field is NULL.
    virtual bool isNull(void) const = 0;


    ///
    /// @brief Clears out the BLOB field (set value to NULL).
    ///
    /// @post size() and tell() returns 0, isNull() returns true.
    virtual void clear(void) = 0;


    ///
    /// @brief Reads data from the stream into the BLOB.
    ///
    /// @post Data has written to the blob, beginning on the
    /// current blob cursor position.
    ///
    /// @note If the BLOB contains data, the stream data replaces
    /// the data of the BLOB or is append, depending on the BLOB cursor position.
    /// If you want to override the BLOB data securely, call clear() before
    /// writing data to the BLOB.
    ///
    /// @param[in] stream Input Stream
    virtual void fromStream(std::ifstream& stream) = 0;


    ///
    /// @brief Writes data from the BLOB into the stream.
    ///
    /// @post Data has written to the stream, beginning on the
    /// current stream position.
    ///
    /// @param[in] stream Output Stream
    virtual void toStream(std::ofstream& stream) = 0;
};





class blob_buf : std::streambuf
{
};

#if 0
class BlobStream : public std::iostream
{
public:
    BlobStream(IBlob& blob) { }

    BlobStream(void) { }

    void open(IBlob& blob) { }
};
#endif


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
    virtual daltype_t      datatype(void) const = 0;
    virtual bool            isnull(void) const = 0;
    virtual void            setNull(void) = 0;
    virtual void            assign(const IVariant& var);

    // getter methods
    virtual signed int      asInt(void) const = 0;
    virtual unsigned int    asUInt(void) const = 0;
    virtual signed char     asChar(void) const = 0;
    virtual unsigned char   asUChar(void) const = 0;

    virtual i18n::UString   asStr(void) const = 0;
    virtual i18n::UString   asStr(std::locale loc) const = 0;
    virtual std::string     asNarrowStr(const char *charset) const = 0;
    virtual std::string     asNarrowStr(const char *charset, std::locale loc) const = 0;

    virtual bool            asBool(void) const = 0;
    virtual signed short    asSmallint(void) const = 0;
    virtual unsigned short  asUSmallint(void) const = 0;
    virtual signed long long    asBigint(void) const = 0;
    virtual unsigned long long  asUBigint(void) const = 0;
    virtual TNumeric        asNumeric(void) const = 0;
    virtual TMoney          asMoney(void) const = 0;
    virtual float           asReal(void) const = 0;
    virtual double          asDouble(void) const = 0;
    virtual TDate           asDate(void) const = 0;
    virtual TTime           asTime(void) const = 0;
    virtual TDatetime       asDatetime(void) const = 0;
    virtual signed int      asTimestamp(void) const = 0;
    //virtual TCustom&        asCustom(void) const = 0;
    virtual TCidr           asCIDR(void) const = 0;
    virtual TInterval       asInterval(void) const = 0;
    virtual TMacaddr        asMacaddr(void) const = 0;
    virtual TInetaddr       asInetaddr(void) const = 0;
    virtual TUuid           asUUID(void) const = 0;
    virtual TXml            asXML(void) const = 0;
    virtual IBlob&          asBlob(void) const = 0;

    // setter methods
    virtual void            setInt(const signed int&) = 0;
    virtual void            setUInt(const unsigned int&) = 0;
    virtual void            setChar(const signed char&) = 0;
    virtual void            setUChar(const unsigned char&) = 0;
    virtual void            setStr(const i18n::UString&) = 0;
    virtual void            setStr(const char* data, std::size_t len, const char* charset) = 0;
    virtual void            setStr(const std::string&, const char* charset) = 0;
    virtual void            setBool(const bool&) = 0;
    virtual void            setSmallint(const signed short&) = 0;
    virtual void            setUSmallint(const unsigned short&) = 0;
    virtual void            setBigint(const signed long long&) = 0;
    virtual void            setUBigint(const unsigned long long&) = 0;
    virtual void            setNumeric(const TNumeric&) = 0;
    virtual void            setMoney(const TMoney&) = 0;
    virtual void            setReal(const float&) = 0;
    virtual void            setDouble(const double&) = 0;
    virtual void            setDate(const TDate&) = 0;
    virtual void            setTime(const TTime&) = 0;
    virtual void            setDatetime(const TDatetime&) = 0;
    virtual void            setTimestamp(const signed int&) = 0;
    //virtual void        asCustom(void) const = 0;
    virtual void            setCIDR(const TCidr&) = 0;
    virtual void            setInterval(const TInterval&) = 0;
    virtual void            setMacaddr(const TMacaddr&) = 0;
    virtual void            setInetaddr(const TInetaddr&) = 0;
    virtual void            setUUID(const TUuid&) = 0;
    virtual void            setXML(const TXml&) = 0;
    virtual void            setBlob(IBlob&) = 0;
  
private:
    IVariant&               operator=(const IVariant& o);
};



DBWTL_EXPORT std::wostream&  operator<<(std::wostream& o, const IVariant &var);
DBWTL_EXPORT std::ostream&   operator<<(std::ostream& o,  const IVariant &var);




//--------------------------------------------------------------------------
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


    virtual bool            asBool(void) const;
    virtual signed short    asSmallint(void) const;
    virtual unsigned short  asUSmallint(void) const;
    virtual signed long long     asBigint(void) const;
    virtual unsigned long long  asUBigint(void) const;
    virtual TNumeric        asNumeric(void) const;
    virtual TMoney          asMoney(void) const;
    virtual float           asReal(void) const;
    virtual double          asDouble(void) const;
    virtual TDate           asDate(void) const;
    virtual TTime           asTime(void) const;
    virtual TDatetime       asDatetime(void) const;
    virtual signed int      asTimestamp(void) const;
    //virtual TCustom&        asCustom(void) const = 0;
    virtual TCidr           asCIDR(void) const;
    virtual TInterval       asInterval(void) const;
    virtual TMacaddr        asMacaddr(void) const;
    virtual TInetaddr       asInetaddr(void) const;
    virtual TUuid           asUUID(void) const;
    virtual TXml            asXML(void) const;
    virtual IBlob&          asBlob(void) const;


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
    virtual void            setBlob(IBlob&);


protected:
    bool m_isnull;
};


//--------------------------------------------------------------------------
/// @brief Internal stored variants needs a clone method
class DBWTL_EXPORT IStoredVariant : public IVariant//BaseVariantImplFor
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





IStoredVariant* new_default_storage(daltype_t type);




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



    virtual i18n::UString   asStr(void) const;
    virtual i18n::UString   asStr(std::locale loc) const;
    virtual std::string     asNarrowStr(const char *charset) const;
    virtual std::string     asNarrowStr(const char *charset, std::locale loc) const;
    virtual bool            asBool(void) const;
    virtual signed short    asSmallint(void) const;
    virtual unsigned short  asUSmallint(void) const;
    virtual signed long long     asBigint(void) const;
    virtual unsigned long long  asUBigint(void) const;
    virtual TNumeric        asNumeric(void) const;
    virtual TMoney          asMoney(void) const;
    virtual float           asReal(void) const;
    virtual double          asDouble(void) const;
    virtual TDate           asDate(void) const;
    virtual TTime           asTime(void) const;
    virtual TDatetime       asDatetime(void) const;
    virtual signed int      asTimestamp(void) const;
    //virtual TCustom&        asCustom(void) const = 0;
    virtual TCidr           asCIDR(void) const;
    virtual TInterval       asInterval(void) const;
    virtual TMacaddr        asMacaddr(void) const;
    virtual TInetaddr       asInetaddr(void) const;
    virtual TUuid           asUUID(void) const;
    virtual TXml            asXML(void) const;
    virtual IBlob&          asBlob(void) const;


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
    virtual void            setBlob(IBlob&);


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

    virtual i18n::UString        name(void) const = 0;
    virtual daltype_t            daltype(void) const = 0;
/*
  virtual int                  maxLength(void) const = 0;
  virtual systype_t            systype(void) const = 0;
  virtual i18n::UString        schema(void) const = 0;
  virtual i18n::UString        catalog(void) const = 0;
*/
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
/// @brief DAL Interface for type information
class DBWTL_EXPORT ITypeInfo : IDALObject
{
public:
    ITypeInfo(void);
    virtual ~ITypeInfo(void);

    virtual i18n::UString    name(void) const;
    virtual i18n::UString    sys_name(void) const = 0;
    virtual dal::daltype_t   daltype(void) const = 0;
    virtual int              radix(void) const = 0;


protected:
    daltype_t m_typeid;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for Environments
class DBWTL_EXPORT IEnv : public IDALObject
{
public:
    typedef std::auto_ptr<IEnv> ptr;

    virtual ~IEnv(void) { }

    virtual IDbc* newConnection(void) = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for connections
class DBWTL_EXPORT IDbc : public IDALObject
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
    virtual dalstate_t     connect(i18n::UString database,
                                   i18n::UString user = i18n::UString(),
                                   i18n::UString password = i18n::UString()) = 0;


    ///
    /// @brief Connect to a database by options
    virtual dalstate_t     connect(IDbc::Options& options) = 0;


    virtual bool           isConnected(void) const = 0;
    virtual dalstate_t     disconnect(void) = 0;
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

    /*
      virtual void   prepare(i18n::UString sql) = 0;
      virtual void   execute(void) = 0;
    */

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

    virtual const value_type&        column(colnum_t num) = 0;
    //virtual value_type&        field(colnum_t num) = 0;
    virtual const value_type&        column(i18n::UString name) = 0;
    //virtual variant_type&        field(i18n::UString name) = 0;

    //virtual IBlob&           getBlob(colnum_t num) = 0;

    // column methods
    virtual size_t           columnCount(void) const = 0;
    virtual colnum_t         columnID(i18n::UString name) const = 0;
    virtual i18n::UString    columnName(colnum_t num) const = 0;
    virtual const ITypeInfo& datatype(colnum_t num) const = 0;

    /// @brief Returns the column descriptor for the given column number
    virtual const IColumnDesc& metadata(colnum_t num) const = 0;

    /// @brief Returns the column descriptor for the given column name
    virtual const IColumnDesc& metadata(i18n::UString name) const = 0;


    virtual IDALDriver* getDriver(void) const = 0;
};





//------------------------------------------------------------------------------
///
/// @brief DAL Interface for statements
class DBWTL_EXPORT IStmt : public IDALObject
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
    virtual void      bind(int num, Variant data) = 0;

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


DAL_NAMESPACE_END



// include the exception declarations
#include "../db_exceptions.hh"



DAL_NAMESPACE_BEGIN




//#define DAL_CODE_POS __FUNCTION__



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
            return L"Implement me";
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
IBlob&          BaseVariantImplFor<Base>::asBlob(void) const
{ throw ex::convert_error(this->datatype(), DAL_TYPE_BLOB); }

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
void            BaseVariantImplFor<Base>::setBlob(IBlob&)
{ throw ex::read_only(); }



DAL_NAMESPACE_END

#endif
