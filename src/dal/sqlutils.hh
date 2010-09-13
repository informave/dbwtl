//
// sqlutils.hh - SQL Utils
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
/// @brief SQL Utils
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_SQLUTILS_HH
#define INFORMAVE_DB_DAL_SQLUTILS_HH



#include "dbwtl/dal/dal_interface.hh"
//#include "dbwtl/db_objects.hh"
#include "dbwtl/util/smartptr.hh"



DAL_NAMESPACE_BEGIN


//--------------------------------------------------------------------------
/// Parses a SQL type like INTEGER or VARCHAR.
/// 
/// @since 0.0.1
/// @brief SQL Type parser
class DBWTL_EXPORT SqlTypeParser
{
public:
    SqlTypeParser(void);

    virtual ~SqlTypeParser(void);

    
    virtual void registerType(daltype_t type, i18n::UString pattern);

    virtual bool parse(const i18n::UString &str);

    virtual daltype_t getDaltype(void) const;

    virtual signed int getSize(void) const;

    virtual signed int getPrecision(void) const;



protected:
    typedef std::pair<daltype_t, i18n::UString>   pattern_entry_type;
    typedef std::vector<pattern_entry_type>       pattern_vector_type;

    virtual void reset(void);

    virtual bool parseSpecial(i18n::UString::const_iterator &si,
                              const i18n::UString &str,
                              i18n::UString::const_iterator &pi,
                              const i18n::UString &pattern);


    /// 1 - SIZE or SCALE
    /// 2 - PRECISION
    std::map<int, i18n::UString>      m_values;
    pattern_vector_type               m_pattern;
    daltype_t                         m_daltype;
};




DAL_NAMESPACE_END

#endif
