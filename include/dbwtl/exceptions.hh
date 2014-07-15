//
// exceptions.hh - Exceptions
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

///
/// @file
/// @brief Exceptions
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_EXCEPTIONS_HH
#define INFORMAVE_DB_EXCEPTIONS_HH


#include <stdexcept>
#include <memory>

#include "db_fwd.hh"
#include "variant.hh"
//#include "dbwtl/dal/dal_interface.hh"

DB_NAMESPACE_BEGIN


void internal_helper__throw_ConvertException(daltype_t src, daltype_t dest, const String &str);

//..............................................................................
////////////////////////////////////////////////////////////////////// Exception
///
/// @since 0.0.1
/// @brief Node base class
class DBWTL_EXPORT Exception : public std::exception
{
public:
    Exception(const String &str) : std::exception(),
        m_msg(),
        m_msg_narrow()
        {
            this->setMessage(str);
        }

    virtual ~Exception(void) throw()
    {}

    virtual void setMessage(const String &msg);

    virtual const String& getMessage() const;

    /// Reimplement to provide our own message
    virtual const char* what(void) const throw();




protected:
    /// Protected constructor, the user has to construct a derived type
    Exception(void) : std::exception(),
        m_msg(),
        m_msg_narrow()
        {}


    String        m_msg;
    std::string   m_msg_narrow;
};






//..............................................................................
/////////////////////////////////////////////////////////////// ConvertException
///
/// This exception class is thrown if a value can notbe converted
/// to another (requested) type.
/// @since 0.0.1
/// @brief Exception if converting values fails
class DBWTL_EXPORT ConvertException : public Exception
{
public:

    /// @note The name of the variant is extracted from the argument
    //convert_error(const Variant &var);


    ConvertException(const String &what);

    //convert_error(dalstate_t state, const std::wstring &varname = std::wstring());

    /// @note The name of the variant is extracted from the argument
    //convert_error(dalstate_t state, const Variant &var);


    ConvertException(informave::db::daltype_t, informave::db::DatatypeEnumeration, const String &desc = String() /*, const Variant &var */);

    virtual ~ConvertException(void) throw()
    {}

protected:
    String m_varname;
};



//..............................................................................
////////////////////////////////////////////////////////////////// NullException
///
/// This exception class is thrown if a value or a pointer is null
/// @since 0.0.1
/// @brief Exception if a value or pointer is null
class DBWTL_EXPORT NullException : public Exception
{
public:
    NullException(const String &varname = String());

    NullException(const Variant &var);

    //null_value(dalstate_t state, const std::wstring &varname = std::wstring());

    virtual ~NullException(void) throw()
    {}

protected:
    String m_varname;
};



//..............................................................................
//////////////////////////////////////////////////////////////// EngineException
///
/// This exception class is thrown if anything goes wrong in the DAL
/// backends. Exceptions like sql_error are derived from this class.
/// @since 0.0.1
/// @brief Exception if anything goes wrong in the DAL backend
class DBWTL_EXPORT EngineException : public Exception
{
public:
    EngineException(const String &what);

	EngineException(IHandle *handle, const IDiagnosticRec &rec);

    virtual ~EngineException(void) throw()
    {}

protected:
    EngineException(void);
};


class DBWTL_EXPORT FeatureUnsuppException : public EngineException
{
public:
	FeatureUnsuppException(const String &what);

	virtual ~FeatureUnsuppException(void) throw()
	{}
};



// BEFORE DELELTE THIS, IMPLEMENT SQL STRING INFORMATION IN SQLSTATE EXCEPTION!!!
//--------------------------------------------------------------------------
/// This exception class is thrown if a SQL query fails.
/// @brief Exception if a SQL query fails
/*
  class DBWTL_EXPORT sql_error : public engine_error
  {
  public:
  sql_error(const String &sql, const String &what);

  //sql_error(dalstate_t state, const std::wstring &sql, const std::wstring &what);

  virtual ~sql_error(void) throw()
  {}

  /// @brief Returns the SQL that caused the error
  virtual const String& getSQL(void) const;

  /// @brief Returns the error message reported by the backend engine
  virtual const String& getError(void) const;

  protected:
  String m_sql;
  String m_what;
  };
*/


//..............................................................................
////////////////////////////////////////////////////////////// SqlstateException
///
/// This exception class is the base class for all SQLSTATE exceptions.
/// @since 0.0.1
/// @brief Base class for all SQLSTATE exceptions
class DBWTL_EXPORT SqlstateException : public Exception
{
public:
    SqlstateException(const IDiagnostic& diag_to_clone);


    SqlstateException(const SqlstateException& orig);


    virtual ~SqlstateException(void) throw();


    virtual const IDiagnostic& diag(void) const;


protected:
    std::auto_ptr<IDiagnostic> m_diag;
};



//..............................................................................
////////////////////////////////////////////////////////////// NotfoundException
///
/// This exception class is thrown if something was not found.
/// For example:
///  - Columns
///  - Tables
///  - Parameters
///
/// @since 0.0.1
/// @brief Exception if something was not found.
class DBWTL_EXPORT NotFoundException : public Exception
{
public:
    //not_found(dalstate_t state, const std::wstring &what = std::wstring());

    NotFoundException(const String &what);

    virtual ~NotFoundException(void) throw()
    {}
};



//..............................................................................
////////////////////////////////////////////////////////////// CharconvException
///
/// This exception class is thrown if a conversion from one to another
/// charset fails.
///
/// @since 0.0.1
/// @brief Exception if a charset conversion fails
class DBWTL_EXPORT CharconvException : public Exception
{
public:
    CharconvException(String data, String from_charset,
                      String to_charset);

    virtual ~CharconvException(void) throw()
    {}
};


//..............................................................................
////////////////////////////////////////////////////////////// ReadonlyException
///
/// This exception class is thrown if resource is read only.
///
/// @since 0.0.1
/// @brief Exception if a resource is read only
class DBWTL_EXPORT ReadonlyException : public Exception
{
public:
    //read_only(dalstate_t state, const std::wstring &resource_name);


    ReadonlyException(const String &resource_name = String(L"<unnamed>"),
                      const char *triggered_by = "<unknown>");

    virtual ~ReadonlyException(void) throw()
    {}
};



//..............................................................................
/////////////////////////////////////////////////////////// LibFunctionException
///
/// This exception class is thrown if required function is not
/// found in the specified module.
///
/// @since 0.0.1
/// @brief Exception if a function is not found
class DBWTL_EXPORT LibFunctionException : public EngineException
{
public:
    LibFunctionException(const std::string &func_name, const String &module_path = "<unknown module>");

    virtual ~LibFunctionException(void) throw()
    {}

protected:
    std::string m_func_name;
    String m_module_path;
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
