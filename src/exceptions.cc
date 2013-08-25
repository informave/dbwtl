//
// exceptions.cc - Exceptions thrown by the DBWTL
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


#include "dbwtl/exceptions.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "utils.hh"

DB_NAMESPACE_BEGIN

void internal_helper__throw_ConvertException(daltype_t src, daltype_t dest, const String &str)
{
	throw ConvertException(src, dest, str);
}


SqlstateException::SqlstateException(const IDiagnostic& diag_to_clone)
    : Exception(diag_to_clone.str()),
      m_diag()

{
    this->m_diag.reset(diag_to_clone.clone());
}


SqlstateException::SqlstateException(const SqlstateException& orig)
    : Exception(orig),
      m_diag()
{
    this->m_diag.reset(orig.m_diag->clone());
}


SqlstateException::~SqlstateException(void) throw()
{}

const IDiagnostic&
SqlstateException::diag(void) const
{
    return *this->m_diag.get();
}





//--------------------------------------------------------------------------
//
//
const char*
Exception::what(void) const throw()
{
    return this->m_msg_narrow.c_str();
}


//
//
void
Exception::setMessage(const String &msg)
{
    this->m_msg = msg;
    this->m_msg_narrow = msg.to("UTF-8");
}


//
//
const String&
Exception::getMessage(void) const
{
    return this->m_msg;
}





//--------------------------------------------------------------------------
//
//
/*
  ConvertException::ConvertException(const String &varname)
  : exception(),
  m_varname(varname)
  {
  String msg;
  msg = US("Conversion error: Can't convert the variant (identified by '") +
  String::Internal(varname) +
  US("') to the requested type.");

  this->setMessage(msg);
  }
*/


//
//
/*
  ConvertException::ConvertException(const Variant &var)
  : exception(),
  m_varname()
  {
  String msg;
  msg = US("Conversion error: Can't convert the variant (identified by '") +
  String::Internal(var.getName()) +
  US("') to the requested type.");

  this->setMessage(msg);
  }
*/


ConvertException::ConvertException(const String &what)
    : Exception(),
      m_varname()
{
    this->setMessage(what);
}






//
//
//     ConvertException::ConvertException(dalstate_t state, const std::wstring &varname)
//         : exception(),
//           m_varname()
//     {
//         std::wstring msg;
//         msg = L"Conversion error: Can't convert the variant (identified by \"" +
//             varname +
//             L"\") to the requested type.";

//         this->setMessage(msg);
//     }



//
//
//     ConvertException::ConvertException(dalstate_t state, const Variant &var)
//         : exception(),
//           m_varname()
//     {
//         std::wstring msg;
//         msg = L"Conversion error: Can't convert the variant (identified by \"" +
//             var.getName() +
//             L"\") to the requested type.";

//         this->setMessage(msg);
//     }


ConvertException::ConvertException(informave::db::daltype_t src, informave::db::DatatypeEnumeration dest, const String &desc)
    : Exception(),
      m_varname()
{
    String msg;
    msg = US("Conversion error: Can't convert type ") + String::Internal(daltype2string(src)) + US(" to ")
        + String::Internal(daltype2string(dest)) + US(". ");
    if(!desc.empty()) msg += desc;
    this->setMessage(msg);
}



//--------------------------------------------------------------------------
//
//
NullException::NullException(const String &varname)
    : Exception(),
      m_varname(varname)
{
    String msg;
    msg = US("NULL exception: The variant (identified by '") +
        String::Internal(varname) +
        US("') is null.");

    this->setMessage(msg);
}



//
//
NullException::NullException(const Variant &var)
    : Exception(),
      m_varname()
{
    String msg;
    msg = US("NULL exception: The variant (identified by '") +
        String::Internal(var.getName()) +
        US("') is null.");

    this->setMessage(msg);
}



//
//
//     NullException::NullException(dalstate_t state, const std::wstring &varname)
//         : exception(),
//           m_varname()
//     {
//         std::wstring msg;
//         msg = L"NULL exception: The variant (identified by \"" +
//             varname +
//             L"\") is null.";

//         this->setMessage(msg);
//     }





//--------------------------------------------------------------------------
//
//
//     EngineException::EngineException(dalstate_t state)
//         : exception()
//     {y
//      this->setMessage(L"remove dalstate here");
//     }


//
//
EngineException::EngineException(const String &what)
    : Exception()
{
    this->setMessage(what);
}



//
//
EngineException::EngineException(void)
    : Exception()
{
    this->setMessage("No engine error description was given.");
}

FeatureUnsuppException::FeatureUnsuppException(const String &what)
	: EngineException()
{
	this->setMessage(String("Feature unsupported: ") + what);
}





/*


//--------------------------------------------------------------------------
//
//
sql_error::sql_error(const String &sql, const String &what)
: EngineException(),
m_sql(sql),
m_what(what)
{
this->setMessage(what);
}



//
//
//     sql_error::sql_error(dalstate_t state, const std::wstring &sql, const std::wstring &what)
//         : EngineException(),
//           m_sql(sql),
//           m_what(what)
//     {
//         this->setMessage(what);
//     }



//
//
const String&
sql_error::getSQL(void) const
{
return this->m_sql;
}



//
//
const String&
sql_error::getError(void) const
{
return this->m_what;
}


*/


//--------------------------------------------------------------------------
//
//
//     NotfoundException::NotfoundException(dalstate_t state, const std::wstring &what)
//         : exception()
//     {
//         this->setMessage(what);
//     }


//
//
NotfoundException::NotfoundException(const String &what)
{
    this->setMessage(what);
}





//--------------------------------------------------------------------------
//
//
CharconvException::CharconvException(String data, String from_charset,
                                     String to_charset)
    : Exception()
{
    this->setMessage(US("Conversion error from ") + String::Internal(from_charset) + US(" to ")
                     + String::Internal(to_charset) + US(".") + US(" Data: ") + String::Internal(data));
}





//--------------------------------------------------------------------------
//
//
//     ReadonlyException::ReadonlyException(dalstate_t state, const std::wstring &resource_name)
//         : exception()
//     {
//         this->setMessage(L"The resource \"" + resource_name + L"\" is read only.");
//     }



//
//
ReadonlyException::ReadonlyException(const String &resource_name,
                                     const char *triggered_by)
    : Exception()
{

    this->setMessage(US("The resource '") + String::Internal(resource_name) +
                     US("' is read only or a variant storage setter ") +
                     US("for the given type is not implemented.") +
                     US(" (triggered by: ") + String::Internal(String(triggered_by, "UTF-8")) + US(")."));
}





//--------------------------------------------------------------------------
//
//
FunctionException::FunctionException(const std::string &func_name, const String &module_path)
    : EngineException(),
      m_func_name(func_name),
      m_module_path(module_path)
{
    String msg =
        US("The function '") + String::Internal(String(func_name, "ASCII")) + US("' was not found ")
        US("in the module '") + String::Internal(module_path) + US("'.");

    this->setMessage(msg);
}




/*
//--------------------------------------------------------------------------
//
//
logic_error::logic_error(const String &what)
{
this->setMessage(what);
}



//
//
//     logic_error::logic_error(dalstate_t state)
//     {
//         this->setMessage(L"Needs dalstate impl");
//     }





//--------------------------------------------------------------------------
//
//
busy_error::busy_error(const String &what)
: EngineException()
{
this->setMessage(what);
}

*/

//
//
//     busy_error::busy_error(dalstate_t state, const std::wstring &what)
//     {
//         this->setMessage(what);
//     }



DB_NAMESPACE_END


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
