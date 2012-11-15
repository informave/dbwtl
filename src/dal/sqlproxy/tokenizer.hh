//
// tokenizer.hh - Read a stream and produce tokens
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
/// @brief Tokenizer
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_SQLPROXY_TOKENIZER_HH
#define INFORMAVE_DB_SQLPROXY_TOKENIZER_HH


#include "sqlproxy_fwd.hh"
#include "token.hh"

#include <iterator>
#include <vector>
#include <cctype>
#include <locale>
#include <cassert>
#include <algorithm>
#include <map>
#include <iostream>


SQLPROXY_NAMESPACE_BEGIN

template<class T>
bool isnumber(const std::basic_string<T> &s)
{
    for(typename std::basic_string<T>::const_iterator i = s.begin();
        i != s.end();
        ++i)
    {
        if(! (isdigit(*i) || *i == '.'))
            return false;
    }
    return true;
}


template <typename CharT, typename TraitsT, size_t N>
std::basic_string<CharT, TraitsT>
inline _str(const char (&src)[N])
{
    return std::basic_string<CharT, TraitsT>(src, src+N-1);
}



template <typename CharT, typename TraitsT>
std::basic_string<CharT, TraitsT>
inline upper(const std::basic_string<CharT, TraitsT> &str)
{
    std::basic_string<CharT, TraitsT> out;
    out.resize(str.length());
    std::locale loc;
    const std::ctype<char>& ct = std::use_facet<std::ctype<char> >(loc);
    std::transform(str.begin(), str.end(), out.begin(), std::bind1st(std::mem_fun(&std::ctype<char>::toupper), &ct));
    return out;
}



//..............................................................................
////////////////////////////////////////////////////////////////////// Tokenizer
///
/// @since 0.0.1
/// @brief Tokenizer
template<typename CharT, typename TraitsT = std::char_traits<CharT> >
class Tokenizer
{
public:
    typedef std::istreambuf_iterator<CharT, TraitsT>             streambuf_iterator;
    typedef std::map< std::basic_string<CharT, TraitsT>, int >   map_type;
    typedef CharT                                                char_type;
    typedef TraitsT                                              traits_type;


    /// Create new Tokenizer from input stream iterator
    Tokenizer(streambuf_iterator in)
        : m_in(in),
          m_char(),
          m_keywords(),
          m_templates(),
          m_line(1),
          m_charpos(0),
          m_srcname("<input>")
    {
        /// We need to consume the first character for initial state.
        consume();


        /// Add your keywords here
        this->m_keywords[ _str<CharT, TraitsT>("SELECT") ] = DBWTL_TOK_SELECT;
        this->m_keywords[ _str<CharT, TraitsT>("FROM")       ] = DBWTL_TOK_FROM;
        this->m_keywords[ _str<CharT, TraitsT>("WHERE")       ] = DBWTL_TOK_WHERE;
        this->m_keywords[ _str<CharT, TraitsT>("AND")       ] = DBWTL_TOK_AND;
        this->m_keywords[ _str<CharT, TraitsT>("OR")       ] = DBWTL_TOK_OR;
        this->m_keywords[ _str<CharT, TraitsT>("XOR")       ] = DBWTL_TOK_XOR;
        this->m_keywords[ _str<CharT, TraitsT>("IS")       ] = DBWTL_TOK_IS;
        this->m_keywords[ _str<CharT, TraitsT>("AS")       ] = DBWTL_TOK_AS;
        this->m_keywords[ _str<CharT, TraitsT>("NOT")       ] = DBWTL_TOK_NOT;
        this->m_keywords[ _str<CharT, TraitsT>("NULL")     ] = DBWTL_TOK_NULL;
        this->m_keywords[ _str<CharT, TraitsT>("MOD")     ] = DBWTL_TOK_MOD;
    }



    /// Consume next character
    /// Returns false on EOF, otherwise true
    bool consume(void)
    {
        if(this->m_in != streambuf_iterator())
        {
            this->m_charpos++;
            this->m_char = *this->m_in++;
            return true;
        }
        this->m_char = traits_type::eof();
        return false;
    }



    /// Consume next character and return it
    inline char_type getnc(void)
    {
        consume();
        return this->m_char;
    }



    /// Skip line breaks and returns the next character
    /// or EOF.
    char_type skiplb(void)
    {
        /// Handle multiple linebreaks
        while(this->m_char == 0x0A || this->m_char == 0x0D)
        {
            char_type prev = this->m_char;
            this->m_line++;
            if(! consume())
                return traits_type::eof();
             
            /// Skip 2-Byte linebreaks on Windows and MAC
            if((m_char == 0x0A && prev == 0x0D) || (m_char == 0x0D && prev == 0x0A))
                if(! consume())
                    return traits_type::eof();
        }
        return this->m_char;
    }


    
    /// Read input and return the next Token
    Token next(void)
    {
        char_type c;

        skiplb();

        // skip whitespace and linebreaks
        for(c = this->m_char;
            (traits_type::to_int_type(c) != traits_type::eof() && (c == ' ' || c == '\t'));
            consume(), c = skiplb()); 

            
        /// Start position of current char
        std::streamsize start = this->m_charpos;
        size_t line = this->m_line;
        size_t len = 0;
                  
        SourceInfo si(m_srcname, start, len, line);

        if(traits_type::to_int_type(c) == traits_type::eof())
            return Token(0);
            
        switch(c)
        {
        case '"':
            return this->readQuotedId(start, len, line);
        case '\'':
            return this->readLiteral(start, len, line);
        case ';':
            consume();
            return Token(DBWTL_TOK_SEP, si, ";");
        case '(':
            consume();
            return Token(DBWTL_TOK_LP, si, "(");
        case ')':
            consume();
            return Token(DBWTL_TOK_RP, si, ")");

        case '[':
            return this->readQuotedId2(start, len, line);
        case '=':
            consume();
            return Token(DBWTL_TOK_EQUAL, si, "=");
	case '!':
	    consume();
	    if(m_char == '=')
	    {
	    	consume();
                return Token(DBWTL_TOK_NOTEQUAL, si, "!=");
	    }
	    else
	    {
	    	throw std::runtime_error("invalid character");
		break; /// @bug fixme
	    }
        case '|':
            consume();
            assert(m_char == '|'); /// @bug mixme
            consume();
            return Token(DBWTL_TOK_CONCAT, si, "||");
        case '?':
            consume();
            return Token(DBWTL_TOK_QMARK, si, "?");
        case ',':
            consume();
            return Token(DBWTL_TOK_COMMA, si, ",");
        case '+':
            consume();
            return Token(DBWTL_TOK_PLUS, si, "+");
        case '-':
            consume();
            return Token(DBWTL_TOK_MINUS, si, "-");
        case '*':
            consume();
            return Token(DBWTL_TOK_MUL, si, "*");
        case '.':
            consume();
            return Token(DBWTL_TOK_DOT, si, ".");
        case '<':
            consume();
            if(m_char == '=')
            {
                consume();
                return Token(DBWTL_TOK_LOWEREQUAL, si, "<=");
            }
            else return Token(DBWTL_TOK_LOWER, si, "<");
            
        case '>':
            consume();
            if(m_char == '=')
            {
                consume();
                return Token(DBWTL_TOK_GREATEREQUAL, si, ">=");
            }
            else return Token(DBWTL_TOK_GREATER, si, ">");
        case '/':
            consume();
            if(m_char == '/' || m_char == '*')
            {
                this->skipComment();
                return this->next();
            }
            else
            {
                //consume();
                return Token(DBWTL_TOK_DIV, si, "/");                
            }

        default:
            return this->readMulti(start, len, line);
        };
	throw std::runtime_error("invalid character2"); /// @bug fixme
    }



    /// Returns true if EOF is reached
    bool eof(void) const
    {
        return !(this->m_in != streambuf_iterator());
    }



    /// Skip comment
    ///
    /// @todo
    /// Not skip, create a new token which can skipped by the parser
    void skipComment(void)
    {
        char_type c;

        // check second char
        assert(m_char == '/' || m_char == '*');

        if(m_char == '/')
        {
              
            for(c = m_char;
                traits_type::to_int_type(c) != traits_type::eof();
                c = getnc())
            {
                
                if(c == 0x0A || c == 0x0D) // read until line end
                {
                    skiplb();
                    break;
                }
            };
        }
        else if(m_char == '*')
        {
            char_type back;
            for(c = m_char, back = m_char;
                traits_type::to_int_type(c) != traits_type::eof();
                c = getnc())
            {
                if(back == '*' && c == '/')
                {
                    consume(); // skip
                    break;
                }
                else
                {
                    // skip linebreaks and return next "valid" character
                    back = skiplb();
                    //back = c;
                }
            };                
        }
    }


    /// Read literal
    ///
    /// @todo
    /// Support for escaping
    Token readLiteral(std::streamsize start, size_t len, size_t line)
    {
        std::vector<char_type> v;
        char_type c;

        for(c = getnc();
            traits_type::to_int_type(c) != traits_type::eof();
            c = getnc())
        {
            if(c != '\'')
                v.push_back(c);
            else
                break;
        };
        if(c == '\'')
            consume(); // skip '
        std::basic_string<char_type> s(v.begin(), v.end());
        Token tok(DBWTL_TOK_LITERAL, SourceInfo(m_srcname, start, len, line));
        tok.setData(String(s));
        return tok;
    }




    /// Read literal
    ///
    /// @todo
    /// Support for escaping
    Token readQuotedId(std::streamsize start, size_t len, size_t line)
    {
        std::vector<char_type> v;
        char_type c;

        for(c = getnc();
            traits_type::to_int_type(c) != traits_type::eof();
            c = getnc())
        {
            if(c != '"')
                v.push_back(c);
            else
                break;
        };
        if(c == '"')
            consume(); // skip "
        std::basic_string<char_type> s(v.begin(), v.end());
        Token tok(DBWTL_TOK_ID, SourceInfo(m_srcname, start, len, line));
        tok.setData(String(s));
        return tok;
    }


    Token readQuotedId2(std::streamsize start, size_t len, size_t line)
    {
        std::vector<char_type> v;
        char_type c;

        for(c = getnc();
            traits_type::to_int_type(c) != traits_type::eof();
            c = getnc())
        {
            if(c != ']')
                v.push_back(c);
            else
                break;
        };
        if(c == ']')
            consume(); // skip "
        std::basic_string<char_type> s(v.begin(), v.end());
        Token tok(DBWTL_TOK_ID, SourceInfo(m_srcname, start, len, line));
        tok.setData(String(s));
        return tok;
    }




    Token readMulti(std::streamsize start, size_t len, size_t line)
    {
        std::vector<char_type> v;
        v.push_back(this->m_char);
            
        char_type c;

        for(c = getnc();
            traits_type::to_int_type(c) != traits_type::eof();
            c = getnc())
        {
            //if(::isalnum(c) || c == '.' || c == '_')
            if(::isalnum(c) /* || c == '.' */ || c == '_' || (std::isdigit(v[0]) && c == '.'))
                v.push_back(c);
            else
                break;
        };
        std::basic_string<char_type> s(v.begin(), v.end());

        std::basic_string<char_type> up(upper(s));

        {
            typename map_type::iterator i = this->m_keywords.find(up);
            if(i != this->m_keywords.end())
            {
                Token tok(i->second, SourceInfo(m_srcname, start, len, line));
                tok.setData(up);
                return tok;
            }
        }

        {
            typename map_type::iterator i = this->m_templates.find(up);
            if(i != this->m_templates.end())
            {
                Token tok(i->second, SourceInfo(m_srcname, start, len, line));
                tok.setData(up);
                return tok;
            }
        }

        if(isnumber(s))
        {
            Token tok(DBWTL_TOK_NUMBER, SourceInfo(m_srcname, start, len, line), s);
            return tok;
        }
            
        Token tok(DBWTL_TOK_ID, SourceInfo(m_srcname, start, len, line));
        tok.setData(s);
        return tok;
    }


    void setSourceName(const String &str)
    {
        this->m_srcname = str;
    }

protected:
    streambuf_iterator  m_in;
    char_type           m_char;
    map_type            m_keywords;
    map_type            m_templates;
    size_t              m_line;
    std::streamsize     m_charpos;
    String      m_srcname;
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
