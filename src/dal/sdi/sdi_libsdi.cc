//
// sdi_libsdi.cc - SDI Backend libsdi (definition)
//
// Copyright (C)         informave.org
//   2012,               Daniel Vogelbacher <daniel@vogelbacher.name>
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
/// @brief SDI Backend libsdi (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "sdi_libsdi.hh"
#include "dbwtl/variant.hh"
#include "dbwtl/stdext/functional"
#include "../dal_debug.hh"
#include "../sqlutils.hh"
#include "../../utils.hh"

#include <cassert>
#include <cstring>
#include <algorithm>
#include <cstdlib>
#include <sstream>



#define DAL_SDI_LIBSDI_DIAG_ERROR(handle, msg, desc, code)      \
    {                                                           \
        sdi::DIAG *__dal__diag =                                \
            new SDIDiag_libsdi(DAL_STATE_ERROR,                 \
                               DBWTL_SPOS(),                    \
                               String(msg), desc, code, -1);    \
        handle->m_diag.push_back(__dal__diag);                  \
        __dal__diag->raiseException();                          \
    } 



DAL_NAMESPACE_BEGIN


/*
struct delete_object
{
    template <typename T>
    void operator()(T ptr){ delete ptr;}

    template<typename U, typename V>
    void operator()(std::pair<U, V> val){ delete val.second;}
};
*/


static void delete_resultset(SDIDataProvider_libsdi* rs)
{
    delete rs;
}



daltype_t sdi2daltype(enum sdi_data_types type)
{
    switch(type)
    {
    case SDI_TYPE_CHAR: return DAL_TYPE_CHAR;
    case SDI_TYPE_UCHAR: return DAL_TYPE_UCHAR;
    case SDI_TYPE_SMALLINT: return DAL_TYPE_SMALLINT;
    case SDI_TYPE_USMALLINT: return DAL_TYPE_USMALLINT;
    case SDI_TYPE_INT: return DAL_TYPE_INT;
    case SDI_TYPE_UINT: return DAL_TYPE_UINT;
    case SDI_TYPE_BIGINT: return DAL_TYPE_BIGINT;
    case SDI_TYPE_UBIGINT: return DAL_TYPE_UBIGINT;
    case SDI_TYPE_NUMERIC: return DAL_TYPE_NUMERIC;
    case SDI_TYPE_BIT: return DAL_TYPE_BOOL;
    case SDI_TYPE_BOOL: return DAL_TYPE_BOOL;
    case SDI_TYPE_STRING: return DAL_TYPE_STRING;
    case SDI_TYPE_DATE: return DAL_TYPE_DATE;
    case SDI_TYPE_TIME: return DAL_TYPE_TIME;
    case SDI_TYPE_DATETIME: return DAL_TYPE_TIMESTAMP;
    case SDI_TYPE_TIMESTAMP: return DAL_TYPE_TIMESTAMP;
    case SDI_TYPE_FLOAT: return DAL_TYPE_FLOAT;
    case SDI_TYPE_REAL: return DAL_TYPE_DOUBLE;
    case SDI_TYPE_MEMO: return DAL_TYPE_MEMO;
    case SDI_TYPE_BLOB: return DAL_TYPE_BLOB;
    case SDI_TYPE_ARRAY:
    case SDI_TYPE_CUSTOM:
    default: assert(!"foo");
    }
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::


//
//
SDIBlob_libsdi::SDIBlob_libsdi(const SDIData_libsdi& data)
    : SDIBlob(),
      m_data(data),
      m_buf(0),
      m_buf_end(0),
      m_cur(0)
{}


//
//
SDIBlob_libsdi::~SDIBlob_libsdi(void)
{}


//
//
SDIBlob_libsdi::int_type
SDIBlob_libsdi::underflow()
{
    if(this->m_cur == this->m_buf_end)
        return traits_type::eof();
    else
        return *this->m_cur;
}


//
//
SDIBlob_libsdi::pos_type
SDIBlob_libsdi::seekpos(SDIBlob_libsdi::pos_type p, std::ios_base::openmode m)
{
    std::cout << "seek" << std::endl;
    if(m & std::ios_base::in)
    {
        this->m_cur = this->m_buf + p > this->m_buf_end ? this->m_buf_end : this->m_buf + p;
        return this->m_cur - this->m_buf;
    }
    return traits_type::eof();
}


//
//
SDIBlob_libsdi::pos_type
SDIBlob_libsdi::seekoff(off_type off, std::ios_base::seekdir way, std::ios_base::openmode m)
{
    if(m & std::ios_base::in)
    {
        switch(way)
        {
        case std::ios_base::beg:
            this->m_cur = this->m_buf + off > this->m_buf_end ? this->m_buf_end : this->m_buf + off;
            return this->m_cur - this->m_buf;

        case std::ios_base::cur:
            this->m_cur = this->m_cur + off > this->m_buf_end ? this->m_buf_end : this->m_cur + off;
            return this->m_cur - this->m_buf;

        case std::ios_base::end:
            this->m_cur = this->m_buf_end + off > this->m_buf_end ? this->m_buf_end : this->m_buf_end + off;
            return this->m_cur - this->m_buf;
        default:
            break;
            // catchall for GCC warnings (gcc defines more than the dree ways)
        }
    }
    return traits_type::eof();    
}


//
//
SDIBlob_libsdi::int_type
SDIBlob_libsdi::uflow()
{
    if(this->m_cur == this->m_buf_end)
        return traits_type::eof();
    else
        return *this->m_cur++;
}


//
//
SDIBlob_libsdi::int_type
SDIBlob_libsdi::pbackfail(int_type ch)
{
    if(this->m_cur == this->m_buf || (ch != traits_type::eof() && ch != this->m_cur[-1]))
        return traits_type::eof();
    else
        return *--this->m_cur;
}


//
//
std::streamsize
SDIBlob_libsdi::showmanyc(void)
{
    return this->m_buf_end - this->m_cur;
}


//
//
std::streamsize
SDIBlob_libsdi::xsgetn(char_type *ch, std::streamsize n)
{
    std::streamsize cp = n > showmanyc() ? showmanyc() : n;
    std::memcpy(ch, this->m_cur, cp * sizeof(traits_type::char_type));
    this->m_cur += cp;
    return cp;
}


///
///
void
SDIBlob_libsdi::setBufPtr(const unsigned char *buf, size_t size)
{
    this->m_buf = buf;
    this->m_buf_end = buf + size;
    this->m_cur = buf;
}


//
//
bool
SDIBlob_libsdi::isNull(void) const
{
    return (this->m_buf == this->m_buf_end);
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

///
SDIData_libsdi::SDIData_libsdi(SDIDataProvider_libsdi& result, colnum_t colnum, bool locked)
    : SDIData(),
      m_resultset(result),
      m_colnum(colnum),
      m_blobbuf(),
      m_memo_stream(),
      m_buf(0)
{ }



//
SDIData_libsdi::~SDIData_libsdi(void)
{
    if(this->m_blobbuf.get())
    {
        this->m_blobbuf.reset(0);
    }

    if(this->m_buf)
    {
        this->m_resultset.drv()->SDIFree(&this->m_buf);
    }
}



//
SDIBlob_libsdi*
SDIData_libsdi::getBlob(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);

    size_t size = 0;

    if(this->isnull())
        throw ex::null_value(String("SDIData_libsdi result column"));


    if(! this->m_blobbuf.get())
    {
        this->m_blobbuf.reset(new SDIBlob_libsdi(*this));
    }

    if(this->m_buf)
    {
        this->m_resultset.drv()->SDIFree(&this->m_buf);
    }


    // the streambuffer is set to null on each fetch() (through refresh())
    if(this->m_blobbuf->isNull())
    {
        int ind = SDI_NULL_VALUE;
        this->m_resultset.drv()->SDIGetLargeObject(this->m_resultset.getHandle(), this->m_colnum, &m_buf, &size, &ind);
        assert(ind != SDI_NULL_VALUE);

        this->m_blobbuf->setBufPtr(static_cast<const unsigned char*>(m_buf), size);
    }
    return this->m_blobbuf.get();
}



UnicodeStreamBuf*
SDIData_libsdi::getMemo(void) const
{
    DALTRACE("VISIT");
    assert(this->m_colnum > 0);

    size_t size = 0;

    if(this->isnull())
        throw ex::null_value(String("SDIData_libsdi result column"));

   
    std::stringstream ss;
    ss << this->getBlob();

    /// @bug use SDI encoding
    m_memo_stream << String(ss.str(), "ISO-8859-1");

    return this->m_memo_stream.rdbuf();
}






template<typename T>
T get_data(const SDIData_libsdi &data, SDIDataProvider_libsdi &result)
{
    if(data.isnull())
        throw ex::null_value(String("SDIData_libsdi result column"));
    else
    {
        T value;
        int ind;
        sdi_code e = result.drv()->SDIGetData(result.getHandle(), data.colnum(), &value, sizeof(value), &ind);
        assert(ind != SDI_NULL_VALUE);
        return value;
    }
}


colnum_t
SDIData_libsdi::colnum(void) const
{
    return this->m_colnum;
}


signed char 
SDIData_libsdi::getChar(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_CHAR);
    assert(this->m_colnum > 0);
    return get_data<signed char>(*this, this->m_resultset);
    
}

unsigned char 
SDIData_libsdi::getUChar(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_UCHAR);
    assert(this->m_colnum > 0);
    return get_data<unsigned char>(*this, this->m_resultset);
}
    
signed short 
SDIData_libsdi::getSmallint(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_SMALLINT);
    assert(this->m_colnum > 0);
    return get_data<signed short>(*this, this->m_resultset);
}

unsigned short 
SDIData_libsdi::getUSmallint(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_USMALLINT);
    assert(this->m_colnum > 0);
    return get_data<unsigned short>(*this, this->m_resultset);
}
    
signed int 
SDIData_libsdi::getInt(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_INT);
    assert(this->m_colnum > 0);
    return get_data<signed int>(*this, this->m_resultset);
}

unsigned int 
SDIData_libsdi::getUInt(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_UINT);
    assert(this->m_colnum > 0);
    return get_data<unsigned int>(*this, this->m_resultset);
}
    
signed long long 
SDIData_libsdi::getBigint(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_BIGINT);
    assert(this->m_colnum > 0);
    return get_data<signed long long>(*this, this->m_resultset);
}
unsigned long long 
SDIData_libsdi::getUBigint(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_UBIGINT);
    assert(this->m_colnum > 0);
    return get_data<unsigned long long>(*this, this->m_resultset);
}




std::string conv_numeric(sdi_numeric_t num)
{
    char buf[64];
    int i;
    for(i = 0; i < num.len; ++i) buf[i] = num.digits[i] + 48;

    char tmp[64+1+1];
    int c = snprintf(tmp, 64+1+1, "%s%.*s.%.*s", num.sign > 0 ? "+" : "-", num.len - num.frac, buf, num.frac,buf+(num.len-num.frac));

    return tmp;
}




TNumeric 
SDIData_libsdi::getNumeric(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_NUMERIC);
    assert(this->m_colnum > 0);

    sdi_numeric_t dec = get_data<sdi_numeric_t>(*this, this->m_resultset);


    return TNumeric(conv_numeric(dec));

    //sdi_date_t date = get_data<sdi_date_t>(*this, this->m_resultset);
}
bool 
SDIData_libsdi::getBool(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_BOOL || this->sdi_type() == SDI_TYPE_BIT);
    assert(this->m_colnum > 0);
    return get_data<unsigned char>(*this, this->m_resultset);    
}
String 
SDIData_libsdi::getString(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_STRING);
    assert(this->m_colnum > 0);
   
    if(this->isnull())
        throw ex::null_value(String("SDIData_libsdi result column"));
    else
    {
        int ind;
        sdi_code e = m_resultset.drv()->SDIGetData(m_resultset.getHandle(), colnum(), 0, 0, &ind);
        assert(ind != SDI_NULL_VALUE);
        if(ind)
        {
            std::string tmp;
            tmp.resize(ind+1);
            sdi_code e = m_resultset.drv()->SDIGetData(m_resultset.getHandle(), colnum(), &tmp[0], ind+1, &ind);
            tmp.resize(ind);
            return String(tmp, "ISO-8859-1"); /// @bug check encoding
        }
        else
            return "";
    }
}


TDate 
SDIData_libsdi::getDate(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_DATE);
    assert(this->m_colnum > 0);
    sdi_date_t date = get_data<sdi_date_t>(*this, this->m_resultset);
    return TDate(date.year, date.month, date.day);
}
TTime 
SDIData_libsdi::getTime(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_TIME);
    assert(this->m_colnum > 0);
    sdi_time_t time = get_data<sdi_time_t>(*this, this->m_resultset);
    return TTime(time.hour, time.minute, time.second);
}
TTimestamp 
SDIData_libsdi::getTimestamp(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_TIMESTAMP || this->sdi_type() == SDI_TYPE_DATETIME);
    assert(this->m_colnum > 0);
    sdi_timestamp_t ts = get_data<sdi_timestamp_t>(*this, this->m_resultset);
    return TTimestamp(ts.value);
}
float 
SDIData_libsdi::getFloat(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_FLOAT);
    assert(this->m_colnum > 0);
    return get_data<float>(*this, this->m_resultset);
}
double 
SDIData_libsdi::getReal(void) const
{
    DALTRACE_VISIT;
    DBWTL_BUGCHECK(this->sdi_type() == SDI_TYPE_REAL);
    assert(this->m_colnum > 0);
    return get_data<double>(*this, this->m_resultset);
}





sditype_t
SDIData_libsdi::sdi_type(void) const
{
    size_t size;
    const char *name = 0;
    sditype_t type;
    sdi_code e = this->m_resultset.drv()->SDIDescribeCol(this->m_resultset.getHandle(), this->m_colnum, &size, &name, &type);
    if(e != SDI_SUCCESS)
   	DBWTL_BUG(); 
    return type;
}




//
bool 
SDIData_libsdi::isnull(void) const
{
    DALTRACE("VISIT");

    assert(this->m_colnum > 0);

    int ind = SDI_NULL_VALUE;
    sdi_code e = this->m_resultset.drv()->SDIGetData(this->m_resultset.getHandle(), this->m_colnum, 0, 0, &ind);
    return ind == SDI_NULL_VALUE;
}



//
rowid_t 
SDIData_libsdi::getCurrentRowID(void) const
{
    DALTRACE("VISIT");
    return this->m_resultset.getCurrentRowID();
}


//
void
SDIData_libsdi::refresh(void)
{
    if(this->m_blobbuf.get())
    {
        this->m_blobbuf->setBufPtr(0, 0); // reset the buffer
    }
    if(this->m_buf)
    {
        this->m_resultset.drv()->SDIFree(&this->m_buf);
    }
}



//
daltype_t
SDIData_libsdi::daltype(void) const
{
    return this->m_resultset.describeColumn(this->m_colnum).getDatatype();
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
SDIDataProvider_libsdi::SDIDataProvider_libsdi(SDIStmt_libsdi& stmt)
    : SDIDataProvider(stmt.m_diag),
      m_stmt(stmt),
      m_handle(NULL),
      m_current_tuple(DAL_TYPE_ROWID_NPOS),
      m_last_row_status(0),
      m_isprepared(false),
      m_isopen(false),
      m_isbad(false),
      m_column_desc(),
      m_column_accessors(),
      m_allocated_accessors()
{ }



//
SDIDataProvider_libsdi::~SDIDataProvider_libsdi(void)
{
    this->m_column_accessors.clear();
    this->m_allocated_accessors.clear();
    this->close();
}





//
void 
SDIDataProvider_libsdi::first(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    if(this->m_current_tuple != 1)
    {
        throw ex::engine_error("can't scroll to first record");
    }
}



//
bool
SDIDataProvider_libsdi::next(void)
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    this->m_last_row_status = this->drv()->SDIFetch(this->getHandle());

    switch(this->m_last_row_status)
    {   
    case SDI_SUCCESS:
        // refresh all column accessors
        std::for_each(this->m_column_accessors.begin(),
                      this->m_column_accessors.end(),
                      [](VariantListT::value_type& p)
                      {
                          p.second->refresh();
                      });
        ++this->m_current_tuple;
        break;
    case SDI_DONE:
        break;
    default:
        /*
          const char *msg = this->drv()->sdi3_errmsg(this->m_stmt.getDbc().getHandle());
          String u_msg(msg, "UTF_8");
          DAL_SDI_LIBSDI_DIAG_ERROR(this,
          "Can not prepare query",
          u_msg,
          this->drv()->sdi3_errcode(this->m_stmt.getDbc().getHandle()),
          this->drv()->sdi3_extended_errcode(this->m_stmt.getDbc().getHandle()));
        */
        break;
    };

    return !this->eof();
}



//
bool  
SDIDataProvider_libsdi::eof(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    return this->m_last_row_status != SDI_SUCCESS;
}



//
void   
SDIDataProvider_libsdi::close(void)
{
/*
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");
*/

    if(this->m_handle)
    {
        sdi_code err = this->drv()->SDIClose(&this->m_handle);
    }
}



//
rowcount_t
SDIDataProvider_libsdi::rowCount(void) const
{
    // SDI can't tell you the row count
    return -1;
}



rowcount_t
SDIDataProvider_libsdi::affectedRows(void) const
{
    return -1;
/*
  if(this->isBad())
  throw ex::engine_error("Resultset is in bad state.");

  if(! this->isPrepared())
  throw ex::engine_error("Resultset is not prepared.");

  int count = this->drv()->sdi3_changes(this->m_stmt.getDbc().getHandle());
  return count;
*/
}


//
Variant
SDIDataProvider_libsdi::lastInsertRowId(void)
{
    int id = -1;
    return Variant(id);
/*
  if(this->isBad())
  throw ex::engine_error("Resultset is in bad state.");

  if(! this->isPrepared())
  throw ex::engine_error("Resultset is not prepared.");

  long long id = this->drv()->sdi3_last_insert_rowid(this->m_stmt.getDbc().getHandle());
    
  return Variant(id);
*/
}




//
const SDIVariant&
SDIDataProvider_libsdi::column(String name)
{
    colnum_t num = this->columnID(name);
    return this->column(num);
}



//
const SDIVariant&
SDIDataProvider_libsdi::column(colnum_t num)
{

    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    if(num > this->columnCount())
        throw ex::not_found("column number out of range");


    VariantListT::iterator p = this->m_column_accessors.find(num);
    if(this->m_column_accessors.end() == p)
    {
        std::pair<VariantListT::iterator,bool> r;
        SDIVariant* v = new SDIVariant(new SDIData_libsdi(*this, num, false));
        this->m_allocated_accessors.push_back(v); // smart ptr
        r = this->m_column_accessors.insert(VariantListT::value_type(num, v));
        p = r.first;
    }
    DALTRACE_LEAVE;
    return *(p->second);
}



//
rowid_t
SDIDataProvider_libsdi::getCurrentRowID(void) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    return this->m_current_tuple;
}



//
size_t    
SDIDataProvider_libsdi::columnCount(void) const
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    int c = this->drv()->SDINumCols(this->getHandle());
    DAL_DEBUG("Query column count: " << c);
    DALTRACE_LEAVE;
    return c;
}



//
colnum_t
SDIDataProvider_libsdi::columnID(String name) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    for(colnum_t i = 1; i <= this->columnCount(); ++i)
    {
        if(name == this->columnName(i))
            return i;
    }
    throw ex::not_found(US("Column '") + String::Internal(name) + US("' not found."));
}



//
String
SDIDataProvider_libsdi::columnName(colnum_t num) const
{
    DALTRACE("VISIT");

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");

    return this->describeColumn(num).getName().asStr();
}



//
SDIDrv*
SDIDataProvider_libsdi::drv(void) const
{
    return this->getDbc().drv();
}



//
SDIDbc_libsdi& 
SDIDataProvider_libsdi::getDbc(void) const
{
    return this->m_stmt.getDbc();
}




//
//
SDIColumnDesc_libsdi::SDIColumnDesc_libsdi(colnum_t i, SDIDataProvider_libsdi &result)
    : SDIColumnDesc()
{
    if(i == 0)
    {
        this->m_daltype = DAL_TYPE_UNKNOWN;
        this->m_name.set(String("__DBWTL_BOOKMARK__"));
    }
    else
    {
        size_t size;
        const char *name = 0;
        sditype_t type;
        result.drv()->SDIDescribeCol(result.getHandle(), i, &size, &name, &type);
        //printf("Name: %s\tSize: %d\n", name, size);

        // set name
        this->m_name.set(String(name ? name : "", "UTF-8"));

        this->m_type_name.set(daltype2sqlname(sdi2daltype(type)));
        this->m_daltype = sdi2daltype(type);
        this->m_size.set<signed int>(size);
        this->m_precision.set<unsigned short>(0);
        this->m_scale.set<unsigned short>(0);
    }
}


//
const SDIColumnDesc&
SDIDataProvider_libsdi::describeColumn(colnum_t num) const
{
    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->isOpen())
        throw ex::engine_error("Resultset is not open.");


    std::map<colnum_t, SDIColumnDesc_libsdi>::const_iterator i =
        this->m_column_desc.find(num);

    if(i == this->m_column_desc.end())
    {
        throw ex::not_found(US("Column '") + String::Internal(Variant(int(num)).asStr()) + US("' not found."));
    }
    else
        return i->second;
}


//
const SDIColumnDesc&
SDIDataProvider_libsdi::describeColumn(String name) const
{
    colnum_t num = this->columnID(name);
    return this->describeColumn(num);
}


void
SDIDataProvider_libsdi::refreshMetadata(void)
{
    DALTRACE_ENTER;
    this->m_column_desc.clear();
    if(! this->getHandle())
        return;
    
    size_t colcount = this->columnCount();
    SDIColumnDesc desc;
    
    for(size_t i = 0; i <= colcount; ++i)
    {
        SDIColumnDesc_libsdi x(i, *this);
        this->m_column_desc.insert(std::pair<colnum_t, SDIColumnDesc_libsdi>(i, x));
    }
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
SDIEnv_libsdi::SDIEnv_libsdi(String lib)
    : SDIEnv(),
      m_env(0),
      m_lib()
{
    this->m_lib.reset(new SDIDrv(lib));
    drv()->SDIAllocEnv(&this->m_env);
}



//
SDIEnv_libsdi::~SDIEnv_libsdi(void)
{ 
    if(this->m_env)
    {
        this->drv()->SDIDeallocEnv(&this->m_env);
    }
}



//
SDIDbc_libsdi*
SDIEnv_libsdi::newConnection(void)
{
    return new SDIDbc_libsdi(*this);
}



//
SDIDrv* 
SDIEnv_libsdi::drv() const
{
    return this->m_lib.get();
}





//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



//
SDIDbc_libsdi::SDIDbc_libsdi(SDIEnv_libsdi& env)
    : SDIDbc(),
      m_env(env),
      m_lib(env.drv()),
      m_dbh(0)
{ }



//
SDIDbc_libsdi::~SDIDbc_libsdi(void)
{
    this->disconnect();
}



//
SDIStmt_libsdi*       
SDIDbc_libsdi::newStatement(void)
{
    return new SDIStmt_libsdi(*this);
}



//
void
SDIDbc_libsdi::connect(String database,
                       String user,
                       String password)
{
    IDbc::Options options;
    
    options[ "database" ] = database;
    return this->connect(options);
}




static sdi_sqlstates::engine_states_t sdi3error_to_sqlstate(int code)
{
    return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
#if 0
    switch(code)
    {
    case SDI_ERROR:         /* SQL error or missing database */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_INTERNAL:       /* Internal logic error in SDI */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_PERM:           /* Access permission denied */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_28000;
    case SDI_ABORT:          /* Callback routine requested an abort */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_BUSY:           /* The database file is locked */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_25001;
    case SDI_LOCKED:         /* A table in the database is locked */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_NOMEM:          /* A malloc() failed */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_READONLY:       /* Attempt to write a readonly database */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_25006;
    case SDI_INTERRUPT:      /* Operation terminated by sdi3_interrupt()*/
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_IOERR:          /* Some kind of disk I/O error occurred */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_CORRUPT:        /* The database disk image is malformed */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_NOTFOUND:       /* NOT USED. Table or record not found */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_FULL:           /* Insertion failed because database is full */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_CANTOPEN:       /* Unable to open the database file */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_08001;
    case SDI_PROTOCOL:       /* Database lock protocol error */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_EMPTY:          /* Database is empty */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_SCHEMA:         /* The database schema changed */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_TOOBIG:         /* String or BLOB exceeds size limit */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_22000;
    case SDI_CONSTRAINT:     /* Abort due to constraint violation */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_23000;
    case SDI_MISMATCH:       /* Data type mismatch */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_MISUSE:         /* Library used incorrectly */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_NOLFS:          /* Uses OS features not supported on host */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_0A000;
    case SDI_AUTH:           /* Authorization denied */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_28000;
    case SDI_FORMAT:         /* Auxiliary database format error */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_42000;
    case SDI_RANGE:          /* 2nd parameter to sdi3_bind out of range */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_22000;
    case SDI_NOTADB:         /* File opened that is not a database file */
        return sdi_sqlstates::DAL_SDI_SQLSTATE_08001;
    default:
        return sdi_sqlstates::DAL_SDI_SQLSTATE_08000;
        //throw sdi::STATES::SQLSTATE_08006(*this);
    }
#endif
}


//
SDIDiag_libsdi::SDIDiag_libsdi(dalstate_t state,
                               CodePosInfo pos,
                               Variant what,
                               String description,
                               int sdi_code,
                               int sdi_excode)
    : SDIDiag(state, pos, what, description),
      m_sdi_code(DAL_TYPE_STRING, "SDIDiag::sdi_code"),
      m_sdi_excode(DAL_TYPE_STRING, "SDIDiag::sdi_excode")
{
    m_sdi_code = sdi_code;
    m_sdi_excode = sdi_excode;

    m_sqlstate_id = sdi3error_to_sqlstate(sdi_code);
    m_sqlstate.set(String(sdi::sqlstate2string(m_sqlstate_id), "UTF-8"));
}


//
SDIDiag_libsdi*
SDIDiag_libsdi::clone(void) const
{
    return new SDIDiag_libsdi(*this);
}


//
SDIDiag_libsdi::SDIDiag_libsdi(const SDIDiag_libsdi& ref)
    : SDIDiag(ref),
      m_sdi_code(ref.m_sdi_code),
      m_sdi_excode(ref.m_sdi_excode)
{}



//
SDIDiag_libsdi::~SDIDiag_libsdi(void)

{}



//
String
SDIDiag_libsdi::str(void) const
{
    std::wstringstream ss;

    ss << L"[SQLSTATE:" << ifnull<String>(this->m_sqlstate, L"fooo") << L"] "
       << ifnull<String>(this->m_message, L"No message") << std::endl
       << ifnull<String>(this->m_description, L"No description") << std::endl
       << L"SDI errcode: " << ifnull<String>(this->m_sdi_code, L"NULL")
       << L" (" << ifnull<String>(this->m_sdi_excode, L"NULL") << L")" << std::endl
       << L"Raised at: " 
       << L" [" << this->m_codepos.str() << "]";

    return ss.str();
}



//
void
SDIDbc_libsdi::connect(IDbc::Options& options)
{
    DALTRACE_ENTER;

    std::string dbc_db(options[ "database" ].to("UTF-8"));
    assert(m_dbh == 0);

    sdi_code err = drv()->SDIAllocDbc(this->m_env.getHandle(), &this->m_dbh);
    if(err != SDI_SUCCESS)
    {
        DAL_SDI_LIBSDI_DIAG_ERROR(this, "SDIAllocDbc error", "Allocation error", err);
    }

    err = drv()->SDIConnect(this->m_dbh, dbc_db.c_str());
    if(err == SDI_SUCCESS)
    {
        this->m_isConnected = true;
        this->setDbcEncoding("UTF-8");
    }
    else
    {
        DAL_SDI_LIBSDI_DIAG_ERROR(this, "Can't connect to database", 
                                  drv()->SDIDiagText(m_dbh, SDI_HANDLE_DBC), err);
    }

    DALTRACE_LEAVE;
}



//
void
SDIDbc_libsdi::disconnect(void)
{
    DALTRACE_ENTER;
    
    if(this->m_dbh)
    {
        DALTRACE("is connected, disconnecting...");
        sdi_code err = this->drv()->SDIDisconnect(this->m_dbh);

        if(err == SDI_SUCCESS)
        {
            this->m_isConnected = false;
            this->drv()->SDIDeallocDbc(&this->m_dbh);
        }
        else
        {
            DAL_SDI_LIBSDI_DIAG_ERROR(this, "Can't disconnect from database", 
                                      drv()->SDIDiagText(m_dbh, SDI_HANDLE_DBC), err);
        }
    }

    DALTRACE_LEAVE;
}



//
SDIDBC
SDIDbc_libsdi::getHandle(void) const
{
    assert(this->m_dbh);
    return this->m_dbh;
}

SDIENV
SDIEnv_libsdi::getHandle(void)
{
    assert(this->m_env);
    return this->m_env;
}



//
String 
SDIDbc_libsdi::driverName(void) const
{
    return "SDI (libsdi)";
}



//
String 
SDIDbc_libsdi::dbmsName(void) const
{
    const char *s = this->drv()->SDIGetInfo(this->getHandle(), "sdi_driver_name");
    return String("SDI Driver: ") + String(s);
}



//
SDIDrv*
SDIDbc_libsdi::drv(void) const
{
    assert(this->m_lib);
    return this->m_lib;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::



bool
SDIDataProvider_libsdi::isBad(void) const
{
    return this->m_isbad;
}

bool
SDIDataProvider_libsdi::isOpen(void) const
{
    return this->m_isopen;
}


bool
SDIDataProvider_libsdi::isPositioned(void) const
{
    return (this->m_current_tuple != DAL_TYPE_ROWID_NPOS);
}



void
SDIDataProvider_libsdi::openObjects(void)
{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->m_stmt.getDbc().isConnected())
        throw ex::engine_error("not connected");

    // if anything is currently open, we need to close.
    // This removes all binded vars, too.
    if(this->isOpen())
        this->close();


    sdi_code err = this->drv()->SDIObjects(this->m_stmt.getDbc().getHandle(), &this->m_handle);

    switch(err)
    {
    case SDI_SUCCESS:
        this->m_isprepared = true;
        this->m_isopen = true;
        break;
    default:
        const char *msg = "";//this->drv()->sdi3_errmsg(this->m_stmt.getDbc().getHandle());
        String u_msg(msg, "UTF-8");
        DAL_SDI_LIBSDI_DIAG_ERROR(this,
                                  "Can not prepare query",
                                  u_msg,
                                  err);
        break;
    };

    std::map<int, std::string> tmp_strings;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");


    this->m_last_row_status = this->drv()->SDIFetch(this->m_handle);


    this->m_current_tuple = 1;
    this->m_isopen = true;
    this->refreshMetadata();


    DALTRACE_LEAVE;
}



void
SDIDataProvider_libsdi::open(open_mode mode,
                         const String &table,
                         const String &schema,
			 const String &database)

{
    DALTRACE_ENTER;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");

    if(! this->m_stmt.getDbc().isConnected())
        throw ex::engine_error("not connected");

    // if anything is currently open, we need to close.
    // This removes all binded vars, too.
    if(this->isOpen())
        this->close();

    std::string table_enc = table.to("UTF-8");
    std::string schema_enc = schema.to("UTF-8");

/*
    std::cout << "TABLE: " << table_enc << std::endl;
    std::cout << "SCHEMA: " << schema_enc << std::endl;
*/

    sdi_code err = this->drv()->SDIOpen(this->m_stmt.getDbc().getHandle(), &this->m_handle, table_enc.c_str(), schema_enc.c_str());

    switch(err)
    {
    case SDI_SUCCESS:
        this->m_isprepared = true;
        this->m_isopen = true;
        break;
    default:
        const char *msg = this->drv()->SDIDiagText(this->m_stmt.getDbc().getHandle(), SDI_HANDLE_DBC);
        String u_msg(msg, "UTF-8");
        DAL_SDI_LIBSDI_DIAG_ERROR(this, "Can not prepare query", u_msg,  err);
        throw 1;
        break;
    };

    std::map<int, std::string> tmp_strings;

    if(this->isBad())
        throw ex::engine_error("Resultset is in bad state.");


    this->m_last_row_status = this->drv()->SDIFetch(this->m_handle);


    this->m_current_tuple = 1;
    this->m_isopen = true;
    this->refreshMetadata();


    DALTRACE_LEAVE;
}


//
SDIStmt_libsdi::SDIStmt_libsdi(SDIDbc_libsdi& conn)
    : m_conn(conn),
      m_resultsets(),
      m_providers(),
      m_currentResultset(0),
      m_isPrepared(false)
{ }



//
SDIStmt_libsdi::~SDIStmt_libsdi(void)
{
    this->close();
}




SDIDataProvider_libsdi*
SDIStmt_libsdi::newProvider(void)
{
    this->m_providers.push_back(new SDIDataProvider_libsdi(*this));
    return this->m_providers.at(this->m_providers.size()-1);
}



//
SDIResult& 
SDIStmt_libsdi::resultset(void)
{
    return *this->m_resultsets.at(this->m_currentResultset);
}



//
const SDIResult& 
SDIStmt_libsdi::resultset(void) const
{
//  sqlproxy::Sqlproxy<SDIDataProviderExternal, SDIDataProviderInternal, SDIStmt> proxy(*this);
//  return proxy;

    return *this->m_resultsets.at(this->m_currentResultset);
}



/// @todo For now, prepare() only supports a single statement.
/// It is planned to support multiple statements.
void  
SDIStmt_libsdi::prepare(String sql)
{
    /// For SDI, it is save to create a new resultset
    /// and execute the query inside of the resultset, because
    /// a query can never return more then one resultset.
    //

    SDIResult_libsdi* rs = this->newResultset();
    rs->prepare(sql);

    assert(rs->isPrepared());

    this->m_isPrepared = true;
}


SDIDataProvider*
SDIStmt_libsdi::openObjectList(void)
{
    SDIDataProvider_libsdi* rs = this->newProvider();
    rs->openObjects();
    assert(rs->isOpen());
    //this->m_isPrepared = true;
    //this->m_isOpen = true;
    return rs;
}




//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::

//
TableList
SDIDbc_libsdi::getTables(const ITableFilter&)
{
    TableList list;

    SDIStmt_libsdi::ptr objlist(this->newStatement());

    SDIDataProvider *prov = objlist->openObjectList();


    for(prov->first(); ! prov->eof(); prov->next())
    {
        list.push_back(new SDITable("", *prov));
    }
    
    return list;
}




bool   
SDIStmt_libsdi::isPrepared(void) const
{
    return this->m_isPrepared;
}



//
void   
SDIStmt_libsdi::execute(void)
{
    DALTRACE_ENTER;

    for(ResultsetVectorT::iterator i = this->m_resultsets.begin();
        i != this->m_resultsets.end();
        ++i)
    {             
        (*i)->execute(this->m_params);

        assert((*i)->isOpen());
    }

    this->m_currentResultset = 0;

    DALTRACE_LEAVE;
}



//
void  
SDIStmt_libsdi::execDirect(String sql)
{ 
    this->prepare(sql);
    this->execute();
}



//
void   
SDIStmt_libsdi::close(void)
{
    DALTRACE_ENTER;
    using std::for_each;
    using std::mem_fun;

    // clear resultsets from statement
    for_each(this->m_resultsets.begin(),
             this->m_resultsets.end(),
             delete_object());

    this->m_resultsets.clear();


    for_each(this->m_providers.begin(),
             this->m_providers.end(),
             delete_object());

    this->m_providers.clear();


    /// Params and accessors are cleaned up by the base class.
    SDIStmt::close();

    DALTRACE_LEAVE;
}



//
bool 
SDIStmt_libsdi::nextResultset(void)
{
    return false;
}



//
bool  
SDIStmt_libsdi::moreResultsets(void) const
{
    return false;
}



//
size_t  
SDIStmt_libsdi::paramCount(void) const
{
    return this->resultset().paramCount();
}



//
rowid_t 
SDIStmt_libsdi::affectedRows(void) const
{
    return this->resultset().affectedRows();
}


//
Variant
SDIStmt_libsdi::lastInsertRowId(void)
{
    return this->resultset().lastInsertRowId();
}



//
SDIDrv* 
SDIStmt_libsdi::drv(void) const
{
    return this->getDbc().drv();
}



//
SDIDbc_libsdi& 
SDIStmt_libsdi::getDbc(void) const
{
    return this->m_conn;
}



//
SDIResult_libsdi*
SDIStmt_libsdi::newResultset(void)
{
    this->m_resultsets.push_back(new SDIResult_libsdi(*this));
    return this->m_resultsets.at(this->m_resultsets.size()-1);
}


//
/*
  SDIDataProvider_libsdi*
  SDIStmt_libsdi::getProvider(const ObjectName &name)
  {
  this->m_resultsets.push_back(new SDIDataProvider_libsdi(*this));
  return this->m_resultsets.at(this->m_resultsets.size()-1);
  }
*/


//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
//::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::




std::string  
SDIDbc_libsdi::getDbcEncoding(void) const
{
    return "UTF-8";
}

void    
SDIDbc_libsdi::setDbcEncoding(std::string encoding)
{
    // nothing to do, SDI only can handle UTF-8 and UTF-16
}




DAL_NAMESPACE_END
