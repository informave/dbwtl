//
// ustring.hh - Unicode string and conversion functions
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
/// @brief Unicode string and conversion functions
/// @author Daniel Vogelbacher
/// @since 0.0.1

#ifndef INFORMAVE_I18N_USTRING_HH
#define INFORMAVE_I18N_USTRING_HH

#include "db_fwd.hh"

#include <string>
#include <stdexcept>
#include <vector>
#include <iosfwd>
#include <cstring>
#include <locale>

#ifdef DBWTL_CXX98_COMPATIBILITY
#include <tr1/type_traits>
#else
#include <type_traits>
#endif


DB_NAMESPACE_BEGIN


template<typename T>
class string_ptr_base;



#ifdef DBWTL_CXX98_COMPATIBILITY
template<typename T, typename U, bool writeable = ! std::tr1::is_const<U>::value >
class string_ptr;
#else
template<typename T, typename U, bool writeable = ! std::is_const<U>::value >
class string_ptr;
#endif




//--------------------------------------------------------------------------
/// ustring base class
///
/// @since 0.0.1
/// @brief Base class for ustring
class DBWTL_EXPORT ustring_base
{
public:
    /// Convert from one charset to another charset
    static size_t convert(const char    *src,
                          size_t         srclen,
                          const char    *from,
                          char          *dest,
                          size_t         len,
                          const char    *to);


    /// Returns the right enconding for the given size
    static inline const char* encoding(size_t size)
    {
        switch(size)
        {
        case 1: return "UTF-8";
        case 2: return "UTF-16-LE";
        case 4: return "UTF-32-LE";
        default:
            throw std::runtime_error("invalid char size");
        }
    }

    virtual ~ustring_base(void)
    {}
};





//--------------------------------------------------------------------------
/// convert function for strings
///
/// @since 0.0.1
/// @brief convert a string to another string
template<typename DestT, typename SrcT>
inline std::vector<DestT> conv_string(const SrcT *str, size_t n, std::string srccs, std::string charset)
{
    std::vector<DestT> dest;
    if(!n)
        return dest;
    dest.resize(n * 4);
    size_t c = ustring_base::convert(reinterpret_cast<const char*>(str),
                                      n * sizeof(SrcT),
                                      srccs.c_str(),
                                      reinterpret_cast<char*>(&dest[0]),
                                      dest.size(),
                                      charset.c_str());
    dest.resize(c / sizeof(DestT));
    return dest;
}

/// Specialization for wchar_t, requires no conversion
template<>
inline std::vector<wchar_t> conv_string(const wchar_t *str, size_t n, std::string srccs, std::string charset)
{
    std::vector<wchar_t> tmp(str, str+n);
    return tmp;
}




//--------------------------------------------------------------------------
/// ustring class for strings
///
/// @since 0.0.1
/// @brief String class
template<class CharT, class TraitsT = std::char_traits<CharT>, class AllocatorT = std::allocator<CharT> >
class DBWTL_EXPORT ustring : public ustring_base
{
public:
    typedef std::basic_string<CharT, TraitsT, AllocatorT>   Internal;
    typedef ustring<CharT, TraitsT, AllocatorT>             string_type;
    typedef CharT                                           char_type;

	virtual ~ustring(void)
	{
		delete this->m_strptr;
	}

    /// Constructor from char
    ustring(const char *s, std::string charset = DBWTL_DEFAULT_CHARSET)
        : m_str(),
          m_narrow(),
          m_strptr(0)
    {
        if(s)
        {
            std::vector<char_type> str(conv_string<char_type, char>(s,
                                                                    strlen(s),
                                                                    charset,
                                                                    ustring_base::encoding(sizeof(char_type))));
            this->m_str = Internal(str.data(), str.size());
        }
    }

    /// Constructor from wchar_t
    ustring(const wchar_t *s) : m_str(),
                                m_narrow(),
                                m_strptr(0)
    {
        if(s)
        {
            std::vector<char_type> str(conv_string<char_type, wchar_t>(s,
                                                                       wcslen(s),
                                                                       ustring_base::encoding(sizeof(wchar_t)),
                                                                       ustring_base::encoding(sizeof(char_type))));
            this->m_str = Internal(str.data(), str.size());
        }
    }

    /// Constructor from internal string type
    ustring(const Internal& str) : m_str(str),
                                   m_narrow(),
                                   m_strptr(0)
    {}


    /// Constructor from std::basic_string
    template<class A, class B, class C>
    ustring(const std::basic_string<A, B, C> &s,
            const std::string &charset = DBWTL_DEFAULT_CHARSET)
	    : m_str(),
          m_narrow(),
          m_strptr(0)
    {
        if(s.length())
        {
            std::vector<char_type> str(conv_string<char_type, A>(s.c_str(),
                                                                 s.length(),
                                                                 charset,
                                                                 ustring_base::encoding(sizeof(char_type))));
            this->m_str = Internal(str.data(), str.size());
        }
    }

    /// Constructor from another ustring
    ustring(const string_type& str) : m_str(str.readStr()),
                                      m_narrow(),
                                      m_strptr(0)
    {}
            
     
    /// Constructor
    ustring(void) : m_str(),
                    m_narrow(),
                    m_strptr(0)
    {}


    /// Assign operator
    ustring& operator=(const string_type& str)
    {
        this->writeStr(str.readStr());
        return *this;
    }

    /// Assign operator from std::basic_string
    template<class A, class B, class C>
    string_type& operator=(const std::basic_string<A, B, C>& s)
    {
        std::vector<char_type> str(conv_string<char_type, A>(s.c_str(), 
                                                             s.length(),
                                                             ustring_base::encoding(sizeof(A)),
                                                             ustring_base::encoding(sizeof(char_type))));
        this->writeStr(Internal(str.data(), str.size()));
        return *this;
    }
            
    /// Append operator from std::basic_string
    template<class A, class B, class C>
    string_type operator+(const std::basic_string<A, B, C>& s)
    {
        std::vector<char_type> str(conv_string<char_type, A>(s.c_str(), 
                                                             s.length(),
                                                             ustring_base::encoding(sizeof(A)),
                                                             ustring_base::encoding(sizeof(char_type))));
        this->append(Internal(str.data(), str.size()));
        return *this;
    }

    /// Append operator for ustring
    string_type operator+(const string_type& str)
    {
        //this->m_str.append(str.m_str);
        this->append(str);
        return *this;
    }

    /// Compare operator for ustring
    bool operator==(const string_type& str) const
    {
        return this->readStr() == str.readStr();
    }
            
    /// Convert method
    const char* to(std::string x = "UTF-8") const
    {
    	Internal tmp = this->readStr();
        std::vector<char> str(conv_string<char, char_type>(tmp.c_str(),
                                                           tmp.length(),
                                                           ustring_base::encoding(sizeof(char_type))
                                                           , x));
        this->m_narrow = std::string(str.data(), str.size());
        return this->m_narrow.c_str();
    }

    
    /// Conversion to system encoding
    const char* toSystemEncoding(void) const
    {
        typedef std::codecvt<typename std::wstring::value_type, char, std::mbstate_t> facet;

        if(this->empty())
            return "";

        std::wstring tmp(*this);
        std::string out;
        std::locale loc;

        const facet &cv = std::use_facet< facet >(loc);
        std::mbstate_t state = std::mbstate_t();

        typename std::wstring::value_type *next;
        out.resize((tmp.size()+1) * cv.max_length());

        const typename std::wstring::value_type *in_next;
        char *out_next;

        typename facet::result res = cv.out(state,
                                            tmp.c_str(), tmp.c_str() + tmp.length()+1, in_next,
                                            &out[0], &out[out.length()-1], out_next);
        switch(res)
        {
        case facet::error:
            throw std::runtime_error("invalid conversion");
        case facet::partial:
            throw std::runtime_error("Partial conversion");
        default:
            break;
        }

        this->m_narrow = out;
        return this->m_narrow.c_str();
    }
    


    const char* utf8(void) const
    {
        return this->to("UTF-8");
    }


    /// Compare operator for ustring
    bool operator<(const string_type& s) const
    {
        return this->readStr() < s.readStr();
    }

    /// Check if empty
    bool empty(void) const
    {
        return this->readStr().empty();
    }
            
    /// Append ustring
    string_type& append(const string_type& s)
    {
    	Internal tmp = this->readStr();
        tmp.append(s.readStr());
        this->writeStr(tmp);
        return *this;
    }


    /// Append ustring
    template<class A, class B, class C>
    string_type& append(const std::basic_string<A, B, C>& s)
    {
    	Internal tmp = this->readStr();
        tmp.append(String(s));
        this->writeStr(tmp);
        return *this;
    }

            
    /// Get length
    size_t length(void) const
    {
        return this->readStr().length();
    }
    
    /// Compare with another ustring
    int compare(const string_type& s) const
    {
        return this->readStr().compare(s.readStr());
    }

    /// Compare with another ustring
    template<class A, class B, class C>
    int compare(const std::basic_string<A, B, C>& s) const
    {
        return this->readStr().compare(String(s));
    }

            
    /// Convert operator to std::basic_string
    template<class A, class B, class C>
    operator std::basic_string<A, B, C>(void) const
    {
    	Internal tmp = this->readStr();
        std::vector<A> str(conv_string<A, char_type>(tmp.c_str(),
                                                     tmp.length(),
                                                     ustring_base::encoding(sizeof(char_type)),
                                                     ustring_base::encoding(sizeof(A)))); 
        return std::basic_string<A, B, C>(str.data(), str.size());
    }

    template<class A, class B, class C>
    void sync_with(std::basic_string<A, B, C> *str)
    {
    	delete m_strptr;
        m_strptr = new string_ptr<string_type, std::basic_string<A, B, C> >(str);
    }

    template<class A, class B, class C>
    void sync_with(const std::basic_string<A, B, C> *str)
    {
    	delete m_strptr;
        m_strptr = new string_ptr<string_type, const std::basic_string<A, B, C> >(str);
    }

private:
    const Internal& readStr(void) const
    {
    	if(m_strptr) this->m_str = m_strptr->get();
        return m_str;
    }

    Internal& readStr(void)
    {
		if(m_strptr) this->m_str = m_strptr->get();
		return m_str;
	}


    void writeStr(const Internal &str)
    {
    	this->m_str = str;
    	if(m_strptr) m_strptr->set(str);
    }

    mutable Internal               m_str;
    mutable std::string            m_narrow;
    string_ptr_base<string_type>  *m_strptr;
};



//--------------------------------------------------------------------------
/// string_ptr base class
///
/// @since 0.0.1
/// @brief Base class for string_ptr
template<typename T>
class string_ptr_base
{
public:
    virtual ~string_ptr_base(void)
    {}

    virtual    typename T::Internal get(void) const = 0;

    virtual    void set(const typename T::Internal &data) = 0;

};


//--------------------------------------------------------------------------
/// string_ptr holds a pointer to a basic_string type and convert the value
///
/// @since 0.0.1
/// @brief string_ptr
template<typename T, typename U, bool writeable>
class string_ptr : public string_ptr_base<T>
{
public:
    string_ptr(U* str) : string_ptr_base<T>(), m_ptr(str)
    {}

    virtual ~string_ptr(void)
    {}

    virtual typename T::Internal get(void) const
    {
    	T str(*this->m_ptr);
        return str; // auto conversion
    }

    virtual void set(const typename T::Internal &data)
    {
    	T src(data);
    	U str(src);
        *this->m_ptr = str;
    }

protected:
	U *m_ptr;

private:
    string_ptr(const string_ptr&);
    string_ptr& operator=(const string_ptr&);
};

// specialization for const strings
template<typename T, typename U>
class string_ptr<T, U, false> : public string_ptr_base<T>
{
public:
    string_ptr(U* str) : string_ptr_base<T>(), m_ptr(str)
    {}

    virtual ~string_ptr(void)
    {}

    virtual typename T::Internal get(void) const
    {
        T str(*this->m_ptr);
        return str; // auto conversion
    }

    virtual void set(const typename T::Internal &data)
    {
    	throw std::runtime_error("BUG: string_ptr: Writing to a const strptr. Please report this bug.");
    }

protected:
    U *m_ptr;

private:
    string_ptr(const string_ptr&);
    string_ptr& operator=(const string_ptr&);
};




///////// IF YOU ARE SEARCHING FOR THE `String` TYPE, HERE IS IT /////////

/// String type used by DBWTL

#if DBWTL_INTERNAL_CHARTYPE == 1
typedef ustring<char>                 String;
#elif DBWTL_INTERNAL_CHARTYPE == 2
typedef ustring<wchar_t>              String;
#elif DBWTL_INTERNAL_CHARTYPE == 3
typedef ustring<u16char_t>            String;
#elif DBWTL_INTERNAL_CHARTYPE == 4
typedef ustring<u32char_t>            String;
#endif



/// 
template<class T, class U>
inline std::basic_ostream<T, U>& operator<<(std::basic_ostream<T, U>& o, const String& str)
{
    o << std::basic_string<T, U>(str);
    return o;
}



template<class T, class U>
inline std::basic_istream<T, U>& operator>>(std::basic_istream<T, U>& o, String& str)
{
    std::basic_string<T, U> tmp;
    o >> tmp;
    str = tmp;
    return o;
}



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
