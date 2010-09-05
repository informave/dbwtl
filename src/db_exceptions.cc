//
// db_exceptions.cc - Exceptions thrown by the DBWTL
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
/// @brief Declaration of all exceptions  (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1


//#include "db_exceptions.hh"
#include "dbwtl/dal/dal_interface.hh"

DB_NAMESPACE_BEGIN


namespace ex
{
    //--------------------------------------------------------------------------
    //
    //
    const char*
    exception::what(void) const throw()
    {
        return this->m_msg_narrow.c_str();
    }


    //
    //
    void
    exception::setMessage(const i18n::UString &msg)
    {
        this->m_msg = msg;
        this->m_msg_narrow = i18n::conv_to(msg, "UTF-8");
    }


    //
    //
    const i18n::UString&
    exception::getMessage(void) const
    {
        return this->m_msg;
    }





    //--------------------------------------------------------------------------
    //
    //
    convert_error::convert_error(const i18n::UString &varname)
        : exception(),
          m_varname(varname)
    {
        i18n::UString msg;
        msg = L"Conversion error: Can't convert the variant (identified by \"" +
            varname + 
            L"\") to the requested type.";

        this->setMessage(msg);
    }



    //
    //
    convert_error::convert_error(const dal::Variant &var)
        : exception(),
          m_varname()
    {
        i18n::UString msg;
        msg = L"Conversion error: Can't convert the variant (identified by \"" +
            var.getName() + 
            L"\") to the requested type.";

        this->setMessage(msg);
    }



    //
    //
//     convert_error::convert_error(dal::dalstate_t state, const i18n::UString &varname)
//         : exception(),
//           m_varname()
//     {
//         i18n::UString msg;
//         msg = L"Conversion error: Can't convert the variant (identified by \"" +
//             varname + 
//             L"\") to the requested type.";

//         this->setMessage(msg);
//     }



    //
    //
//     convert_error::convert_error(dal::dalstate_t state, const dal::Variant &var)
//         : exception(),
//           m_varname()
//     {
//         i18n::UString msg;
//         msg = L"Conversion error: Can't convert the variant (identified by \"" +
//             var.getName() + 
//             L"\") to the requested type.";

//         this->setMessage(msg);
//     }


    convert_error::convert_error(informave::db::dal::daltype_t src, informave::db::dal::DatatypeEnumeration dest)
        : exception(),
          m_varname()
    {
    	i18n::UString msg;
	msg = L"Conversion error: Can't convert type " + daltype2string(src) + L" to " + daltype2string(dest) + L".";
	this->setMessage(msg);
    }



    //--------------------------------------------------------------------------
    //
    //
    null_value::null_value(const i18n::UString &varname)
        : exception(),
          m_varname(varname)
    {
        i18n::UString msg;
        msg = L"NULL exception: The variant (identified by \"" +
            varname + 
            L"\") is null.";

        this->setMessage(msg);
    }



    //
    //
    null_value::null_value(const dal::Variant &var)
        : exception(),
          m_varname()
    {
        i18n::UString msg;
        msg = L"NULL exception: The variant (identified by \"" +
            var.getName() + 
            L"\") is null.";

        this->setMessage(msg);
    }



    //
    //
//     null_value::null_value(dal::dalstate_t state, const i18n::UString &varname)
//         : exception(),
//           m_varname()
//     {
//         i18n::UString msg;
//         msg = L"NULL exception: The variant (identified by \"" +
//             varname + 
//             L"\") is null.";

//         this->setMessage(msg);
//     }





    //--------------------------------------------------------------------------
    //
    //
//     engine_error::engine_error(dal::dalstate_t state)
//         : exception()
//     {
//     	this->setMessage(L"remove dalstate here");
//     }


    //
    //
    engine_error::engine_error(const i18n::UString &what)
        : exception()
    {
    	this->setMessage(what);
    }



    //
    //
    engine_error::engine_error(void)
        : exception()
    {
    	this->setMessage(L"No engine error description was given.");
    }





    //--------------------------------------------------------------------------
    //
    //
    sql_error::sql_error(const i18n::UString &sql, const i18n::UString &what)
        : engine_error(),
          m_sql(sql),
          m_what(what)
    {
        this->setMessage(what);
    }



    //
    //
//     sql_error::sql_error(dal::dalstate_t state, const i18n::UString &sql, const i18n::UString &what)
//         : engine_error(),
//           m_sql(sql),
//           m_what(what)
//     {
//         this->setMessage(what);
//     }



    //
    //
    const i18n::UString&
    sql_error::getSQL(void) const
    {
        return this->m_sql;
    }



    //
    //
    const i18n::UString&
    sql_error::getError(void) const
    {
        return this->m_what;
    }





    //--------------------------------------------------------------------------
    //
    //
//     not_found::not_found(dal::dalstate_t state, const i18n::UString &what)
//         : exception()
//     {
//         this->setMessage(what);
//     }


    //
    //
    not_found::not_found(const i18n::UString &what)
    {
        this->setMessage(what);
    }





    //--------------------------------------------------------------------------
    //
    //
    charset_error::charset_error(std::string data, i18n::UString from_charset,
                                 i18n::UString to_charset)
        : exception()
    {
        this->setMessage(L"Conversion error from " + from_charset + L" to " + to_charset + L".");
    }



    //
    //
    charset_error::charset_error(i18n::UString data, i18n::UString from_charset,
                                 i18n::UString to_charset)
        : exception()
    {
        this->setMessage(L"Conversion error from " + from_charset + L" to " + to_charset + L".");
    }





    //--------------------------------------------------------------------------
    //
    //
//     read_only::read_only(dal::dalstate_t state, const i18n::UString &resource_name)
//         : exception()
//     {
//         this->setMessage(L"The resource \"" + resource_name + L"\" is read only.");
//     }



    //
    //
    read_only::read_only(const i18n::UString &resource_name,
                         const char *triggered_by)
        : exception()
    {
        this->setMessage(L"The resource \"" + resource_name + L"\" is read only or a variant storage setter " +
                         L"for the given type is not implemented." +
                         L" (triggered by: " + i18n::conv_from(triggered_by, "UTF-8") + L").");
    }





    //--------------------------------------------------------------------------
    //
    //
    missing_function::missing_function(const std::string &func_name, const i18n::UString &module_path)
        : engine_error(),
          m_func_name(func_name),
          m_module_path(module_path)
    {
        i18n::UString msg = 
            L"The function \"" + i18n::conv_from(func_name, "ASCII") + L"\" was not found "
            L"in the module \"" + module_path + L"\".";

        this->setMessage(msg);
    }





    //--------------------------------------------------------------------------
    //
    //
    logic_error::logic_error(const i18n::UString &what)
    {
        this->setMessage(what);
    }



    //
    //
//     logic_error::logic_error(dal::dalstate_t state)
//     {
//         this->setMessage(L"Needs dalstate impl");
//     }





    //--------------------------------------------------------------------------
    //
    //
    busy_error::busy_error(const i18n::UString &what)
        : engine_error()
    {
        this->setMessage(what);
    }



    //
    //
//     busy_error::busy_error(dal::dalstate_t state, const i18n::UString &what)
//     {
//         this->setMessage(what);
//     }

}


DB_NAMESPACE_END
