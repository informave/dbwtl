//
// token.hh - Token class
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
/// @cond DEV_DOCS
/// @file
/// @brief Token class
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_SQLPROXY_TOKEN_HH
#define INFORMAVE_DB_SQLPROXY_TOKEN_HH


#include "sqlproxy_fwd.hh"

#include <sstream>

SQLPROXY_NAMESPACE_BEGIN

#define SQLPARSER_DEFAULT_SOURCE_NAME "<input>"

//..............................................................................
///////////////////////////////////////////////////////////////////// SourceInfo
///
/// @since 0.0.1
/// @brief Provides information about token source
class SourceInfo
{
public:
    SourceInfo(String file = "",
               std::streamsize chpos = 0, size_t len = 0, size_t line = 0);

    /// @brief Update/expand the current info
    void expand(const SourceInfo &info);

    inline std::streamsize offset(void) const
    {
        return this->m_offset;
    }

    inline size_t length(void) const
    {
        return this->m_len;
    }

    inline size_t linenum(void) const
    {
        return this->m_line;
    }

    inline String sourceName(void) const
    {
        return this->m_file;
    }

    inline String str(void) const
    {
    	std::stringstream ss;
    	ss << sourceName() << ":" << linenum();
	return String(ss.str());
    }



protected:
    String            m_file;
    std::streamsize   m_offset;
    size_t            m_len;
    size_t            m_line;

};


/// for debugging
inline std::ostream& operator<<(std::ostream &o, const SourceInfo &info)
{
    o << "SRCINFO: " << info.sourceName() << ":" << info.linenum() << " " << info.offset() << ":" << info.length();
    return o;
}


//..............................................................................
/////////////////////////////////////////////////////////////////////////  Token
///
/// @since 0.0.1
/// @brief Token class
class Token
{
public:
    /// @brief Constructor for empty Tokens
    Token(void);

    /// @brief Creates a new token with the given ID
    Token(int tid, SourceInfo si = SourceInfo(), String data = String());

    /// @brief Copy constructor
    Token(const Token& t);

    /// @brief Creates a EOF token
    static Token eof(void);

    /// @brief Set the raw data
    void setData(const String &str);

    /// @brief Get the raw data
    String data(void) const;

    /// @brief Get the token ID used by the parser
    int id(void) const;

    /// @brief Get a string representation for the token (used for debugging)
    std::string str(void) const;

    /// @brief returns the source information object
    const SourceInfo& getSourceInfo(void) const;

    /// @brief compare two tokens
    bool operator !=(const Token &tok) const;

protected:
    int         m_id;
    String      m_data;
    //db::Variant m_variant;
    SourceInfo  m_info;
};


SQLPROXY_NAMESPACE_END

#endif

//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
