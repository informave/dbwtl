//
// sqlutils.cc - SQL Utils (definition)
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
/// @brief SQL Utils (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "sqlutils.hh"

DAL_NAMESPACE_BEGIN

//
//
SqlTypeParser::SqlTypeParser(void)
    : m_values(),
      m_pattern(),
      m_daltype(DAL_TYPE_UNKNOWN)
{
    this->registerType(DAL_TYPE_INT, L"INT");
    this->registerType(DAL_TYPE_INT, L"INTEGER");
    //this->registerType(DAL_TYPE_INT, L"INTEGER*");
    this->registerType(DAL_TYPE_CHAR, L"CHAR");
    this->registerType(DAL_TYPE_STRING, L"VARCHAR");
    this->registerType(DAL_TYPE_STRING, L"VARCHAR(*");
    //this->registerType(DAL_TYPE_STRING, L"VARCHAR($1)");
    this->registerType(DAL_TYPE_BOOL, L"BOOL");
    this->registerType(DAL_TYPE_SMALLINT, L"SHORTINT");
    this->registerType(DAL_TYPE_BIGINT, L"BIGINT");
    this->registerType(DAL_TYPE_BLOB, L"BLOB");
    this->registerType(DAL_TYPE_NUMERIC, L"NUMERIC");
    this->registerType(DAL_TYPE_NUMERIC, L"NUMERIC(*");
    this->registerType(DAL_TYPE_NUMERIC, L"DECIMAL");
    this->registerType(DAL_TYPE_NUMERIC, L"DECIMAL(*");
    this->registerType(DAL_TYPE_NUMERIC, L"MONEY");
    this->registerType(DAL_TYPE_FLOAT, L"FLOAT");
    this->registerType(DAL_TYPE_FLOAT, L"REAL");
    this->registerType(DAL_TYPE_DOUBLE, L"DOUBLE");
    this->registerType(DAL_TYPE_DATE, L"DATE");
    this->registerType(DAL_TYPE_TIME, L"TIME");  
    this->registerType(DAL_TYPE_TIMESTAMP, L"TIMESTAMP");

    // reset state
    this->reset();
}



//
//
SqlTypeParser::~SqlTypeParser(void)
{
}




//
//
void
SqlTypeParser::reset(void)
{
    this->m_daltype = DAL_TYPE_UNKNOWN;
    this->m_values.clear();
}



//
//
void
SqlTypeParser::registerType(daltype_t type, std::wstring pattern)
{
    this->m_pattern.push_back(pattern_entry_type(type, pattern));
}



//
//
bool
SqlTypeParser::parseSpecial(std::wstring::const_iterator &si,
                            const std::wstring &str,
                            std::wstring::const_iterator &pi,
                            const std::wstring &pattern)
{
    switch(*pi)
    {
        //case '$':
        //case '[':        
    case '*':
        si = str.end();
        pi = pattern.end();
        return true;
    default:
        return false;
    }
}



//
//
bool
SqlTypeParser::parse(const std::wstring &str)
{
    pattern_vector_type::iterator i;

    this->reset();
    for(i = this->m_pattern.begin(); i != this->m_pattern.end(); ++i)
    {
        //std::wcout << "Testing: " << i->second << " against " << str << std::endl;
        std::wstring::const_iterator si, pi;
        std::wstring value;

        si = str.begin();
        pi = i->second.begin();

        while(pi != i->second.end())
        {  
            if(this->parseSpecial(si, str, pi, i->second))
                goto check;
            
            if(si == str.end() || ::toupper(*si) != toupper(*pi))
                break;

            value += *si;
            ++si;
            ++pi;

        check:
            if(si == str.end() && pi == i->second.end())
            {
                this->m_values[1] = value;
                this->m_daltype = i->first;
                return true;
            }
        }
    }
    return false;
}


//
//
daltype_t
SqlTypeParser::getDaltype(void) const
{
    return this->m_daltype;
}


//
//
signed int
SqlTypeParser::getSize(void) const
{
    std::map<int, std::wstring>::const_iterator i = this->m_values.find(2);

    if(i != this->m_values.end())
    {
        std::wstring size = i->second;
        if(size.empty())
            return 0;
        return 50; /// @todo fixme
    }
    return 0;
}

 
//
//
signed int
SqlTypeParser::getPrecision(void) const
{
    return 0;
}




DAL_NAMESPACE_END

