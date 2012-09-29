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

#include "dbwtl/db_fwd.hh"
#include "dbwtl/dal/dal_fwd.hh"

#include "dbwtl/ustring.hh"
#include "dbwtl/ustreambuf.hh"
#include "dbwtl/util/smartptr.hh"
#include "dbwtl/util/bcd.hh"
#include "dbwtl/variant.hh"

#include <ctime>


DB_NAMESPACE_BEGIN


/// @brief Maps a daltype ID to a string name
DBWTL_EXPORT String daltype2string(daltype_t type);

DBWTL_EXPORT String daltype2sqlname(daltype_t type);



//..............................................................................
///////////////////////////////////////////////////////////////////// BlobStream
///
/// @since 0.0.1
/// @brief BlobStream Datatype
class DBWTL_EXPORT BlobStream : public std::iostream
{
public:
    BlobStream(ByteStreamBuf *buf);
    BlobStream(const Variant &variant);
    virtual ~BlobStream();

    BlobStream(const BlobStream&);
    BlobStream& operator=(const BlobStream&);


///@bug use std::istream instead? and override rdbuf()
protected:
    ByteStreamBuf* m_buf;
private:

};



//..............................................................................
///////////////////////////////////////////////////////////////////// MemoStream
///
/// @since 0.0.1
/// @brief MemoStream Datatype
class DBWTL_EXPORT MemoStream : public std::wiostream
{
public:
    MemoStream(UnicodeStreamBuf *buf);
    virtual ~MemoStream(void);

    MemoStream(const MemoStream&);
    MemoStream& operator=(const MemoStream&);


    std::string   narrow_str(const char *charset) const;
    std::wstring  str() const;

protected:
    UnicodeStreamBuf* m_buf;
private:

};



//..............................................................................
/////////////////////////////////////////////////////////////////////////// Blob
///
/// @since 0.0.1
/// @brief BLOB Datatype
class Blob
{
public:
	Blob(void);
	Blob(ByteStreamBuf *buf);

	virtual ~Blob(void);

	std::streambuf* rdbuf(void) const;


    Blob(const Blob&);
    Blob& operator=(const Blob&);

protected:
    std::stringstream m_data;
};



//..............................................................................
/////////////////////////////////////////////////////////////////////////// Memo
///
/// @since 0.0.1
/// @brief MEMO Datatype
class Memo
{
public:
	Memo(void);
	Memo(UnicodeStreamBuf *buf);

	virtual ~Memo(void);

	std::wstreambuf* rdbuf(void) const;

    Memo(const Memo&);
    Memo(const Variant&);
    Memo& operator=(const Memo&);

    String str(void) const;

protected:
	std::wstringstream m_data;
};



//..............................................................................
///////////////////////////////////////////////////////////////////// TVarbinary
///
/// @since 0.0.1
/// @brief VARBINARY Datatype
class DBWTL_EXPORT TVarbinary
{
public:
	TVarbinary(void);
    TVarbinary(int); /// @bug

    TVarbinary(const void* buf, size_t n);

    String str(void) const;

    size_t size(void) const;

    size_t write(void *buf, size_t size) const;

    inline uint8_t operator[](size_t i) const
    {
    	return m_data[i];
    }

/*
	template<typename T>
	Varbinary(T beginIterator, T endIterator);
*/
	

protected:
	std::vector<uint8_t> m_data;
};


//..............................................................................
////////////////////////////////////////////////////////////////////////// TDate
///
/// @since 0.0.1
/// @brief DATE Datatype
class DBWTL_EXPORT TDate
{
public:
    /// @brief Creates a DATE value set to 1970-01-01
    TDate(void);

    /// @brief Copy constructor
    TDate(const TDate& date);

    /// @brief Creates a DATE value from TIMESTAMP
    TDate(const TTimestamp &ts);

    /// @brief Creates a DATE value from an ISO-8601 date string (YYYY-MM-DD)
    TDate(const String &str);

    TDate(short int year, short int month, short int day);

    TDate(const std::tm &time);

    bool operator==(const TDate &date) const;
    bool operator!=(const TDate &date) const;
    bool operator<(const TDate &date) const;
    bool operator>(const TDate &date) const;

    operator ::tm(void) const;

    virtual String str(void) const;

    short int year(void) const;
    short int month(void) const;
    short int day(void) const;
    

protected:
    short int m_year, m_month, m_day;
};



//..............................................................................
////////////////////////////////////////////////////////////////////////// TTime
///
/// @since 0.0.1
/// @brief TIME Datatype
class DBWTL_EXPORT TTime
{
public:
    /// @brief Creates a TIME value set to 00:00:00
    TTime(void);

    /// @brief Copy constructor
    TTime(const TTime &time);

    /// @brief Creates a TIME value from TIMESTAMP
    TTime(const TTimestamp &ts);

    TTime(short int hour, short int minute, short int second);

    /// @brief Creates a TIME value from an ISO-8601 time string (HH:MM:SS)
    TTime(const String &str);

    TTime(const std::tm &time);

    virtual String str(void) const;

    short int hour(void) const;
    short int minute(void) const;
    short int second(void) const;

    bool operator==(const TTime &time) const;
    bool operator!=(const TTime &time) const;
    bool operator<(const TTime &time) const;
    bool operator>(const TTime &time) const;

    operator ::tm(void) const;


protected:
    short int m_hour, m_minute, m_second;
};



//..............................................................................
///////////////////////////////////////////////////////////////////// TTimestamp
///
/// @since 0.0.1
/// @brief TIMESTAMP Datatype
class DBWTL_EXPORT TTimestamp
{
public:
    /// @brief Creates a TIMESTAMP value set to 1970-01-01 00:00:00.000
	TTimestamp(void);

    /// @brief Creates a TIMESTAMP value from TDate
    TTimestamp(const TDate &date);
    
    /// @brief Creates a TIMESTAMP value from TTime
    TTimestamp(const TTime &time);

    /// @brief Creates a TIMESTAMP from a UNIX Timestamp
    TTimestamp(signed long long ts);
    
    TTimestamp(const std::tm &time);

    /// @brief Creates a TIMESTAMP from an ISO-8601 string (YYYY-MM-DD HH:MM:SS.000)
    TTimestamp(const String &str);

    TTimestamp(short int year, short int month, short int day,
               short int hour, short int minute, short int second, int fraction = 0);

    
    static bool isTimestamp(const String &str);

    virtual String str(void) const;

    short int year(void) const;
    short int month(void) const;
    short int day(void) const;
    short int hour(void) const;
    short int minute(void) const;
    short int second(void) const;
    int fraction(void) const;

    bool operator==(const TTimestamp &ts) const;
    bool operator!=(const TTimestamp &ts) const;
    bool operator<(const TTimestamp &ts) const;
    bool operator>(const TTimestamp &ts) const;

    operator ::tm(void) const;
    
protected:
    short int m_year, m_month, m_day, m_hour, m_minute, m_second;
    int m_fraction;

};









//------------------------------------------------------------------------------
///
/// @brief DAL Interface for special types

class DBWTL_EXPORT TType
{
public:
    TType(bool null = true) : m_isnull(null)
    {}


    virtual bool isNull(void) const;

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









class DBWTL_EXPORT TGuid
{
public:
    virtual daltype_t datatype(void) const;
    virtual String asStr(void) const;


    TGuid(void);

    TGuid(const TGuid &guid);

    TGuid(const String &str);

protected:
    uint32_t data1;
    uint16_t data2, data3;
    uint8_t data4[8];
};






class DBWTL_EXPORT TInterval : public TType
{
public:
    virtual String         asStr(std::locale loc = std::locale()) const;

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
















template<>
struct var_info<bool>
{
    static daltype_t type(void) { return DAL_TYPE_BOOL; }
    static const char* name(void) { return "DAL_TYPE_BOOL"; }
    
};


template<>
struct var_info<signed char>
{
    static daltype_t type(void) { return DAL_TYPE_CHAR; }
    static const char* name(void) { return "DAL_TYPE_CHAR"; }
    
};


template<>
struct var_info<unsigned char>
{
    static daltype_t type(void) { return DAL_TYPE_UCHAR; }
    static const char* name(void) { return "DAL_TYPE_UCHAR"; }
    
};



template<>
struct var_info<signed short>
{
    static daltype_t type(void) { return DAL_TYPE_SMALLINT; }
    static const char* name(void) { return "DAL_TYPE_SMALLINT"; }
    
};


template<>
struct var_info<unsigned short>
{
    static daltype_t type(void) { return DAL_TYPE_USMALLINT; }
    static const char* name(void) { return "DAL_TYPE_USMALLINT"; }
    
};



template<>
struct var_info<signed int>
{
    static daltype_t type(void) { return DAL_TYPE_INT; }
    static const char* name(void) { return "DAL_TYPE_INT"; }
    
};


template<>
struct var_info<unsigned int>
{
    static daltype_t type(void) { return DAL_TYPE_UINT; }
    static const char* name(void) { return "DAL_TYPE_UINT"; }
    
};



template<>
struct var_info<signed long long>
{
    static daltype_t type(void) { return DAL_TYPE_BIGINT; }
    static const char* name(void) { return "DAL_TYPE_BIGINT"; }
    
};


template<>
struct var_info<unsigned long long>
{
    static daltype_t type(void) { return DAL_TYPE_UBIGINT; }
    static const char* name(void) { return "DAL_TYPE_UBIGINT"; }
    
};



template<>
struct var_info<float>
{
    static daltype_t type(void) { return DAL_TYPE_FLOAT; }
    static const char* name(void) { return "DAL_TYPE_FLOAT"; }
    
};


template<>
struct var_info<double>
{
    static daltype_t type(void) { return DAL_TYPE_DOUBLE; }
    static const char* name(void) { return "DAL_TYPE_DOUBLE"; }
    
};


template<>
struct var_info<TDate>
{
    static daltype_t type(void) { return DAL_TYPE_DATE; }
    static const char* name(void) { return "DAL_TYPE_DATE"; }
    
};


template<>
struct var_info<TTime>
{
    static daltype_t type(void) { return DAL_TYPE_TIME; }
    static const char* name(void) { return "DAL_TYPE_TIME"; }
    
};



template<>
struct var_info<TTimestamp>
{
    static daltype_t type(void) { return DAL_TYPE_TIMESTAMP; }
    static const char* name(void) { return "DAL_TYPE_TIMESTAMP"; }
    
};



template<>
struct var_info<TInterval>
{
    static daltype_t type(void) { return DAL_TYPE_INTERVAL; }
    static const char* name(void) { return "DAL_TYPE_INTERVAL"; }
    
};



template<>
struct var_info<String>
{
    static daltype_t type(void) { return DAL_TYPE_STRING; }
    static const char* name(void) { return "DAL_TYPE_STRING"; }
    
};


template<>
struct var_info<TNumeric>
{
    static daltype_t type(void) { return DAL_TYPE_NUMERIC; }
    static const char* name(void) { return "DAL_TYPE_NUMERIC"; }
    
};


template<>
struct var_info<Blob>
{
    static daltype_t type(void) { return DAL_TYPE_BLOB; }
    static const char* name(void) { return "DAL_TYPE_BLOB"; }
    
};


template<>
struct var_info<Memo>
{
    static daltype_t type(void) { return DAL_TYPE_MEMO; }
    static const char* name(void) { return "DAL_TYPE_MEMO"; }
    
};


template<>
struct var_info<BlobStream>
{
    static daltype_t type(void) { return DAL_TYPE_BLOB; }
    static const char* name(void) { return "DAL_TYPE_BLOB"; }

};


template<>
struct var_info<MemoStream>
{
    static daltype_t type(void) { return DAL_TYPE_MEMO; }
    static const char* name(void) { return "DAL_TYPE_MEMO"; }

};



template<>
struct var_info<TVarbinary>
{
    static daltype_t type(void) { return DAL_TYPE_VARBINARY; }
    static const char* name(void) { return "DAL_TYPE_VARBINARY"; }

};




/// @bug fixme
#define SV_CAST_METHOD(type) virtual type cast(type*, std::locale loc) const


template<>
struct sv_accessor<Blob> : public virtual sa_base<Blob>,
                           public supports<BlobStream>
{
	SV_CAST_METHOD(BlobStream);
};

template<>
struct sv_accessor<Memo> : public virtual sa_base<Memo>,
                           public supports<String>,
                           public supports<MemoStream>
{
    SV_CAST_METHOD(String);
	SV_CAST_METHOD(MemoStream);
};


template<>
struct sv_accessor<BlobStream> : public virtual sa_base<BlobStream>
{};

template<>
struct sv_accessor<MemoStream> : public virtual sa_base<MemoStream>,
                                 public supports<String>
{
    SV_CAST_METHOD(String);
};



template<>
struct sv_accessor<TVarbinary> : public virtual sa_base<TVarbinary>,
                                 public supports<String>
{
    SV_CAST_METHOD(String);
};



template<>
struct sv_accessor<signed int> : public virtual sa_base<signed int>,
                                 public supports_cast<signed int, bool>,
                                 public supports<signed char>,
                                 public supports<signed short>,
//public supports_cast<signed int, signed int>,
                                 public supports_cast<signed int, signed long long>,
                                 public supports<unsigned char>,
                                 public supports<unsigned short>,
                                 public supports<unsigned int>,
                                 public supports<unsigned long long>,
                                 public supports_cast<signed int, float>,
                                 public supports_cast<signed int, double>,
                                 public supports_cast<signed int, TNumeric>,
                                 public supports<TTimestamp>,
                                 public supports<String>
{
	SV_CAST_METHOD(signed char);
	SV_CAST_METHOD(signed short);
	SV_CAST_METHOD(unsigned char);
	SV_CAST_METHOD(unsigned short);
	SV_CAST_METHOD(unsigned int);
	SV_CAST_METHOD(unsigned long long);
	SV_CAST_METHOD(String);
	SV_CAST_METHOD(TTimestamp);
};



template<>
struct sv_accessor<unsigned int> : public virtual sa_base<unsigned int>
                                   // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{};

template<>
struct sv_accessor<signed char> : public virtual sa_base<signed char>
                                   // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{};


template<>
struct sv_accessor<unsigned char> : public virtual sa_base<unsigned char>
                                   // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{};

template<>
struct sv_accessor<bool> : public virtual sa_base<bool>,
                           public supports<String>

                                   // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual String cast(String*, std::locale loc) const;
};

template<>
struct sv_accessor<signed short> : public virtual sa_base<signed short>,
                                   public supports<String>,
                                  public supports_cast<signed short, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   public supports_cast<signed short, signed int>,
                                   public supports_cast<signed short, signed long long>
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual String cast(String*, std::locale loc) const;
};

template<>
struct sv_accessor<unsigned short> : public virtual sa_base<unsigned short>,
                                     public supports<String>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual String cast(String*, std::locale loc) const;
};



template<>
struct sv_accessor<signed long long> : public virtual sa_base<signed long long>,
                                       public supports<String>,
                                       public supports_cast<signed long long, signed int>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual String cast(String*, std::locale loc) const;
};

template<>
struct sv_accessor<unsigned long long> : public virtual sa_base<unsigned long long>,
                                       public supports<String>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual String cast(String*, std::locale loc) const;
};


template<>
struct sv_accessor<TNumeric> : public virtual sa_base<TNumeric>,
                               public supports<String>,
                               public supports<bool>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual String cast(String*, std::locale loc) const;
    virtual bool cast(bool*, std::locale loc) const;
};


template<>
struct sv_accessor<double> : public virtual sa_base<double>,
                             public supports_cast<double, float>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
};

template<>
struct sv_accessor<float> : public virtual sa_base<float>,
                            public supports_cast<float, double>
{
};


template<>
struct sv_accessor<TDate> : public virtual sa_base<TDate>,
                            public supports<TTimestamp>,
                            public supports<String>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual TTimestamp cast(TTimestamp*, std::locale loc) const;
    virtual String cast(String*, std::locale loc) const;
};


template<>
struct sv_accessor<TTime> : public virtual sa_base<TTime>,
                            public supports<TTimestamp>,
                            public supports<String>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual TTimestamp cast(TTimestamp*, std::locale loc) const;
    virtual String cast(String*, std::locale loc) const;
};


template<>
struct sv_accessor<TTimestamp> : public virtual sa_base<TTimestamp>,
                                 public supports<TDate>,
                                 public supports<TTime>,
                                 public supports<String>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual TDate cast(TDate*, std::locale loc) const;
    virtual TTime cast(TTime*, std::locale loc) const;
    virtual String cast(String*, std::locale loc) const;
};


template<>
struct sv_accessor<TInterval> : public virtual sa_base<TInterval>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
};

template<>
struct sv_accessor<String> : public virtual sa_base<String>,
                             public supports<signed int>,
                             public supports<unsigned int>,
                             public supports<bool>,
                             public supports<signed short>,
                             public supports<unsigned short>,
                             public supports<signed long long>,
                             public supports<unsigned long long>,
                             public supports<float>,
                             public supports<double>,
                             public supports<MemoStream>

                                  // public supports_cast<signed int, bool>,
                                   // public supports_cast<signed int, signed char>,
                                   // public supports_cast<signed int, signed short>,
                                   // public supports_cast<signed int, signed int>,
                                   // public supports_cast<signed int, signed long long>,
                                   // public supports_cast<signed int, unsigned char>,
                                   // public supports_cast<signed int, unsigned short>,
                                   // public supports_cast<signed int, unsigned int>,
                                   // public supports_cast<signed int, unsigned long long>,
                                   // public supports_cast<signed int, float>,
                                   // public supports_cast<signed int, double>,
                                   // public supports_cast<signed int, TNumeric>,
                                   // public supports<TTimestamp>,
                                   // public supports<Blob>,
                                   // public supports<Memo>,
                                   // public supports<String>
{
    virtual signed int cast(signed int*, std::locale loc) const;
    virtual unsigned int cast(unsigned int*, std::locale loc) const;
    virtual bool cast(bool*, std::locale loc) const;
    virtual signed short cast(signed short*, std::locale loc) const;
    virtual unsigned short cast(unsigned short*, std::locale loc) const;
    virtual signed long long cast(signed long long*, std::locale loc) const;
    virtual unsigned long long cast(unsigned long long*, std::locale loc) const;
    virtual float cast(float*, std::locale loc) const;
    virtual double cast(double*, std::locale loc) const;
    virtual MemoStream cast(MemoStream*, std::locale loc) const;
};





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
