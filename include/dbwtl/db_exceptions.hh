//
// db_exceptions.hh - Exceptions thrown by the DBWTL
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
/// @brief Declaration of all exceptions
/// @author Daniel Vogelbacher
/// @since 0.0.1

#ifndef INFORMAVE_DB_DAL_INTERFACES_HH
#error "do not include this file directly"
#endif

#ifndef INFORMAVE_DB_DBEXCEPTIONS_HH
#define INFORMAVE_DB_DBEXCEPTIONS_HH


#include <stdexcept>

#include "db_fwd.hh"

DB_NAMESPACE_BEGIN

/// Because a list of all possible exceptions is useful, all exceptions
/// are defined within this namespace.
/// @brief Namespace for all exceptions thrown by the DBWTL
namespace ex
{
    //--------------------------------------------------------------------------
    /// This class provides a set of basic exception-class methods.
    /// 
    /// @since 0.0.1
    /// @brief Base class for all exceptions
    class exception : public std::exception
    {
    public:
        /// Empty virtual destructor
        virtual ~exception(void) throw()
            {}

        virtual void setMessage(const i18n::UString &msg);

        virtual const i18n::UString& getMessage() const;

        /// Reimplement to provide our own message
        virtual const char* what(void) const throw();


        /// Returns the dalstate_t object
        /// @todo needs implementation
        //virtual const dal::dalstate_t& getState(void) const;

    protected:
        /// Protected constructor, the user has to construct a derived type
        exception(void) : std::exception(),
                          m_msg(),
                          m_msg_narrow(),
                          m_state()
            {}


        i18n::UString        m_msg;
        std::string          m_msg_narrow;
        dal::dalstate_t      m_state;
    };





    //--------------------------------------------------------------------------
    /// This exception class is thrown if a value can notbe converted
    /// to another (requested) type.
    /// @brief Exception if converting values fails
    class convert_error : public exception
    {
    public:
        convert_error(const i18n::UString &varname = i18n::UString());

        /// @note The name of the variant is extracted from the argument
        convert_error(const dal::Variant &var);

        convert_error(dal::dalstate_t state, const i18n::UString &varname = i18n::UString());

        /// @note The name of the variant is extracted from the argument
        convert_error(dal::dalstate_t state, const dal::Variant &var);


        convert_error(informave::db::dal::daltype_t, informave::db::dal::DatatypeEnumeration);

        virtual ~convert_error(void) throw()
            {}

    protected:
        i18n::UString m_varname;
    };





    //--------------------------------------------------------------------------
    /// This exception class is thrown if a value or a pointer is null
    /// @brief Exception if a value or pointer is null
    class null_value : public exception
    {
    public:
        null_value(const i18n::UString &varname = i18n::UString());

        null_value(const dal::Variant &var);

        null_value(dal::dalstate_t state, const i18n::UString &varname = i18n::UString());

        virtual ~null_value(void) throw()
            {}

    protected:
        i18n::UString m_varname;
    };



    //--------------------------------------------------------------------------
    /// This exception class is thrown if anything goes wrong in the DAL
    /// backends. Exceptions like sql_error are derived from this class.
    /// @brief Exception if anything goes wrong in the DAL backend
    class engine_error : public exception
    {
    public:
        engine_error(dal::dalstate_t state);

        engine_error(const i18n::UString &what);

        virtual ~engine_error(void) throw()
            {}

    protected:
        engine_error(void);
    };



    //--------------------------------------------------------------------------
    /// This exception class is thrown if a SQL query fails.
    /// @brief Exception if a SQL query fails
    class sql_error : public engine_error
    {
    public:
        sql_error(const i18n::UString &sql, const i18n::UString &what);

        sql_error(dal::dalstate_t state, const i18n::UString &sql, const i18n::UString &what);

        virtual ~sql_error(void) throw()
            {}

        /// @brief Returns the SQL that caused the error
        virtual const i18n::UString& getSQL(void) const;
        
        /// @brief Returns the error message reported by the backend engine
        virtual const i18n::UString& getError(void) const;
        
    protected:
        i18n::UString m_sql;
        i18n::UString m_what;
    };





    //--------------------------------------------------------------------------
    /// This exception class is thrown if something was not found.
    /// For example:
    ///  - Columns
    ///  - Tables
    ///  - Parameters
    ///
    /// @brief Exception if something was not found.
    class not_found : public exception
    {
    public:
        not_found(dal::dalstate_t state, const i18n::UString &what = i18n::UString());

        not_found(const i18n::UString &what);

        virtual ~not_found(void) throw()
            {}
    };





    //--------------------------------------------------------------------------
    /// This exception class is thrown if a conversion from one to another
    /// charset fails.
    /// @brief Exception if a charset conversion fails
    class charset_error : public exception
    {
    public:
        charset_error(dal::dalstate_t state, std::string data, i18n::UString from_charset,
                      i18n::UString to_charset);

        charset_error(dal::dalstate_t state, i18n::UString data, i18n::UString from_charset,
                      i18n::UString to_charset);

        virtual ~charset_error(void) throw()
            {}
    };





    //--------------------------------------------------------------------------
    /// This exception class is thrown if resource is read only.
    /// @brief Exception if a resource is read only
    class read_only : public exception
    {
    public:
        read_only(dal::dalstate_t state, const i18n::UString &resource_name);

        read_only(const i18n::UString &resource_name = i18n::UString());

        virtual ~read_only(void) throw()
            {}
    };





    //--------------------------------------------------------------------------
    /// This exception class is thrown if required function is not
    /// found in the specified module.
    /// @brief Exception if a function is not found
    class missing_function : public engine_error
    {
    public:
        missing_function(const std::string &func_name, const i18n::UString &module_path = L"<unknown module>");

        virtual ~missing_function(void) throw()
            {}

    protected:
        std::string m_func_name;
        i18n::UString m_module_path;
    };





    //--------------------------------------------------------------------------
    /// This exception class is thrown if a logic error occours.
    /// @brief Exception if a logic error occours.
    class logic_error : public exception
    {
    public:
        logic_error(const i18n::UString &what);

        logic_error(dal::dalstate_t state);

        virtual ~logic_error(void) throw()
            {}
    };





    //--------------------------------------------------------------------------
    /// This exception class is thrown if the backend is busy.
    /// @brief Exception if the backend is busy
    class busy_error : public engine_error
    {
    public:
        busy_error(const i18n::UString &what);

        busy_error(dal::dalstate_t state, const i18n::UString &what);

        virtual ~busy_error(void) throw()
            {}
    };



}


DB_NAMESPACE_END

#endif

