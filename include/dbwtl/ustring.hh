//
// ustring.hh - Unicode string and conversion functions
//
// Copyright (C)         informave.org
//   2013,               Daniel Vogelbacher <daniel@vogelbacher.name>
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

#ifdef __GNUC__
#define LIKELY(x)      __builtin_expect(!!(x), 1)
#define UNLIKELY(x)    __builtin_expect(!!(x), 0)
#else
#define LIKELY(x)      (x)
#define UNLIKELY(x)    (x)
#endif

typedef uint16_t UCharT;

#define DEFAULT_CHARSET "UTF-8"

#ifdef _WIN32
#define USTRING_WCHAR_SIZE 2
#else
#define USTRING_WCHAR_SIZE 4
#endif

#ifndef _WIN32
#define USTRING_WCHAR_NEED_CONV
#endif


class DBWTL_EXPORT ustring_base
{
public:

    static int32_t convert(const char *src, size_t srclen, const char *from,
                           char *dest, size_t len, const char *to);

    virtual ~ustring_base(void)
    {}
};


class DBWTL_EXPORT ustring_icu : public ustring_base
{
public:
    virtual ~ustring_icu(void)
    {}


protected:

    size_t from_wide(char *cdest, size_t cdestbuf,
                     const UCharT *wsrc, size_t wlen,
                     const std::string &charset) const;

    size_t to_wide(UCharT *wdest, size_t wbuflen,
                   const char *csrc, size_t clen,
                   const std::string &charset) const;


    size_t utf32_to_utf16(UCharT *wdest, size_t wbuflen,
                          const char32_t *wsrc, size_t wlen) const;

    size_t utf16_to_utf32(char32_t *wdest, size_t wbuflen,
                          const UCharT *wsrc, size_t wlen) const;

    size_t utf16_to_utf8(char *cdest, int32_t cbuflen,
                         const UCharT *wsrc, size_t wlen) const;

    size_t utf8_to_utf16(UCharT *wdest, int32_t wbuflen,
                         const char *csrc, size_t clen) const;

};

struct DBWTL_EXPORT ConversionError : std::exception
{


    virtual ~ConversionError(void) throw()
    {}

    const char* what(void) const throw()
    {
        return "convert error";
    }
};



class DBWTL_EXPORT ustring_error : public std::exception
{
public:
        ustring_error(const std::string &what)
        {}
};



template<typename T>
class string_ptr_base;



#ifdef DBWTL_CXX98_COMPATIBILITY
template<typename T, typename U, bool writeable = ! std::tr1::is_const<U>::value >
class string_ptr;
#else
template<typename T, typename U, bool writeable = ! std::is_const<U>::value >
class string_ptr;
#endif



template<typename allocator = std::allocator<UCharT> >
class ustring : public ustring_icu
{
public:

    typedef std::u32string UTF32Str;

    typedef std::basic_string<UCharT, std::char_traits<UCharT>, allocator> StrT;

    typedef std::wstring Internal;

    typedef ustring<allocator> string_type;

    virtual ~ustring(void)
    {}


    /// @final
    inline static string_type fromUTF8(const char *s)
    {
        string_type tmp;
        tmp.loadUTF8(s);
        return tmp;
    }


    /// @final
    inline void loadUTF8(const char *s)
    {
        this->m_data.clear();
        if(s)
        {
            size_t len = ::strlen(s);
            ref().resize(len*2); // 4 correct? no ->2
            size_t c = this->utf8_to_utf16(cptr(), ref().size(),
                                           s, len);
            ref().resize(c);
        }
        this->writeStr(this->m_data);
    }


    /// @final
    ustring(const char *s)
        : m_data(),
          m_narrow(),
          m_strptr(0)

    {
        if(!s) return;
        this->loadUTF8(s);
    }


    /// @final
    ustring(const char *s, const std::string &charset)
        : m_data(),
          m_narrow(),
          m_strptr(0)
    {
        if(!s) return;
        size_t clen = ::strlen(s);
        this->ref().resize(clen*2);

        size_t wlen = this->to_wide(cptr(), ref().size(),
                                    s, clen,
                                    charset);
        this->ref().resize(wlen);
    }


    /// @final
    ustring(const wchar_t *s)
        : m_data(),
          m_narrow(),
          m_strptr(0)
    {
        if(LIKELY(s))
        {
#if defined(USTRING_WCHAR_NEED_CONV)
            static_assert(sizeof(wchar_t) == 4, "wchar_t must of size 4");
            size_t wlen = ::wcslen(s);
            ref().resize(wlen*2);
            size_t c = this->utf32_to_utf16(cptr(), ref().size(),
                                            (const char32_t*)s, wlen);
            ref().resize(c);
#else
            ref().insert(ref().begin(), s, s+::wcslen(s));
#endif
        }
    }


    /// @final
    template<class B, class C>
    ustring(const std::basic_string<wchar_t, B, C> &s)
        : m_data(),
          m_narrow(),
          m_strptr(0)
    {
        if(s.empty()) return;

#if defined(USTRING_WCHAR_NEED_CONV)
        static_assert(sizeof(wchar_t) == 4, "wchar_t must of size 4");
        ref().resize(s.size()*2);
        size_t c = this->utf32_to_utf16(cptr(), ref().size(),
                                        (const char32_t*)s.c_str(), s.size());
        ref().resize(c);
#else
        ref().insert(ref().begin(), s.begin(), s.end());
#endif
    }


    /// @final
    template<class B, class C>
    ustring(const std::basic_string<char, B, C> &s,
            const std::string &charset = DEFAULT_CHARSET)
        : m_data(),
          m_narrow(),
          m_strptr(0)
    {
        static_assert(sizeof(char) == 1, "Invalid char size");

        if(!s.length()) return;

        ref().resize(s.size()*2); // 4 correct? no -> 2
        size_t wlen = this->to_wide(cptr(), ref().size(),
                                    s.c_str(), s.size(),
                                    charset);
        this->ref().resize(wlen);
    }


    /// @final
    ustring(const string_type &s)
        : m_data(s.readStr()),
          m_narrow(),
          m_strptr(0)
    {
    }


    /// @final
    ustring(void)
        : m_data(),
          m_narrow(),
          m_strptr(0)
    {
    }



    /// @final
    inline bool hasExternStorage(void) const
    {
        return this->m_strptr != 0;
    }



    /// @final
    bool empty(void) const
    {
        if(hasExternStorage())
            return this->readStr().empty();
        else
            return ref().empty();
    }


    /// @final
    string_type upper(void) const
    {
        /// use icu u_strToUpper()
        if(LIKELY(!hasExternStorage()))
        {
            std::wstring s = *this;
            std::transform(s.begin(), s.end(),
                           s.begin(),
                           ::towupper);
            return string_type(s);
        }
        else
        {
            string_type tmp;
            tmp.m_data = this->readStr();
            std::wstring s(tmp);
            std::transform(s.begin(), s.end(),
                           s.begin(),
                           ::towupper);
            return string_type(s);
        }
    }


    /// @final
    string_type& operator=(const string_type& str)
    {
        if(LIKELY(!hasExternStorage()))
            m_data = str.hasExternStorage() ? str.readStr() : str.m_data;
        else
            this->writeStr(str.readStr());
        return *this;
    }


    /// @final
    template<class A, class B, class C>
    string_type& operator=(const std::basic_string<A, B, C>& s)
    {
        string_type tmp(s);
        if(LIKELY(!hasExternStorage()))
            this->m_data = tmp.m_data;
        else
            this->writeStr(tmp.m_data);
        return *this;
    }



    /// @final
    string_type operator+(const string_type& str) const
    {
        if(LIKELY(!hasExternStorage()))
        {
            string_type tmp;
            tmp.m_data = this->m_data;
            tmp.m_data += str.readStr();
            return tmp;
        }
        else
        {
            string_type tmp(*this);
            tmp.m_data += str.readStr();
            return tmp;
        }
    }


    /// @final
    template<class A, class B, class C>
    string_type operator+(const std::basic_string<A, B, C>& s) const
    {
        string_type tmp1(*this);
        string_type tmp2(s);
        return tmp1+tmp2;
    }


    /// @final
    inline bool operator==(const string_type& str) const
    {
        return this->readStr() == str.readStr();
    }


    /// @final
    inline bool operator<(const string_type& s) const
    {
        return this->readStr() < s.readStr();
    }


    /// @final
    const char* utf8(void) const
    {
        if(LIKELY(!hasExternStorage()))
        {
            this->m_narrow.resize(ref().size()*4);
            size_t clen = this->utf16_to_utf8(&this->m_narrow[0], this->m_narrow.size(),
                                              ref().c_str(), ref().size());
            this->m_narrow.resize(clen);
            return this->m_narrow.c_str();
        }
        else
        {
            StrT tmp = this->readStr();
            this->m_narrow.resize(tmp.size()*4);
            size_t clen = this->utf16_to_utf8(&this->m_narrow[0], this->m_narrow.size(),
                                              tmp.c_str(), tmp.size());
            this->m_narrow.resize(clen);
            return this->m_narrow.c_str();
        }
    }


    /// @final
    const char* to(const std::string &charset = DEFAULT_CHARSET) const
    {
        if(LIKELY(!hasExternStorage()))
        {
            this->m_narrow.resize(ref().size()*4);
            size_t clen = this->from_wide(&this->m_narrow[0], this->m_narrow.size(),
                                          ref().c_str(), ref().size(), charset);
            this->m_narrow.resize(clen);
            return this->m_narrow.c_str();
        }
        else
        {
            StrT tmp = this->readStr();
            this->m_narrow.resize(tmp.size()*4);
            size_t clen = this->from_wide(&this->m_narrow[0], this->m_narrow.size(),
                                          tmp.c_str(), tmp.size(), charset);
            this->m_narrow.resize(clen);
            return this->m_narrow.c_str();
        }
    }


    /// @final
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

        //typename std::wstring::value_type *next;
        out.resize((tmp.size()+1) * cv.max_length());

        const typename std::wstring::value_type *in_next;
        char *out_next;

        typename facet::result res = cv.out(state,
                                            tmp.c_str(), tmp.c_str() + tmp.length()+1, in_next,
                                            &out[0], &out[out.length()-1], out_next);
        switch(res)
        {
        case facet::error:
            throw ustring_error("ustring error: conversion error");
        case facet::partial:
            throw ustring_error("ustring error: partial conversion");
        default:
            break;
        }

        this->m_narrow = out;
        return this->m_narrow.c_str();
    }


    /// @final
    string_type& operator+=(const string_type& s)
    {
        if(LIKELY(!hasExternStorage()))
            this->m_data += s.m_data;
        else
            this->m_data += s.readStr();

        this->writeStr(this->m_data);
        return *this;
    }


    /// @final
    string_type& append(const string_type& s)
    {
        (*this) += s;
        return *this;
    }


    /// @final
    template<class A, class B, class C>
    string_type& append(const std::basic_string<A, B, C>& s)
    {
        return append(string_type(s));
    }



    /// final
    inline size_t length(void) const
    {
        if(LIKELY(!hasExternStorage()))
            return this->m_data.length();
        else
            return this->readStr().length();
    }


    /// @final
    size_t capacity(void) const
    {
        return this->m_data.capacity();
    }


    /// @final
    void reserve(size_t res_arg = 0)
    {
        this->m_data.reserve(res_arg);
    }


    /// @final
    int compare(const string_type& s) const
    {
        return this->readStr().compare(s.readStr());
    }


    /// @final
    template<class A, class B, class C>
    int compare(const std::basic_string<A, B, C>& s) const
    {
        return this->readStr().compare(string_type(s));
    }


    /// @final
    template<typename B, typename C>
    operator std::basic_string<wchar_t, B, C>(void) const
    {
#if defined(USTRING_WCHAR_NEED_CONV)
        std::basic_string<wchar_t, B, C> s;
        s.resize(ref().size()*2);
        size_t c = this->utf16_to_utf32((char32_t*)&s[0], s.size(),
                                        ref().c_str(), ref().size());
        s.resize(c);
        return s;
#else
        if(LIKELY(!hasExternStorage()))
            return std::wstring(this->m_data.begin(), this->m_data.end());
        else
        {
            StrT tmp = this->readStr();
            return std::basic_string<wchar_t, B, C>(tmp.begin(), tmp.end());
        }
#endif
    }


    /// @final
    template<typename B, typename C>
    inline operator std::basic_string<char, B, C>(void) const
    {
        return std::basic_string<char, B, C>(utf8());
    }



    /// @finalized
    inline StrT readStr(void) const
    {
        if(UNLIKELY(this->m_strptr))
        {
            return string_type(this->m_strptr->get()).m_data;
        }
        else
            return ref();
    }

    /// @finalized
    inline void writeStr(const StrT &s)
    {
        if(this->m_strptr)
        {
            string_type tmp;
            tmp.m_data = s;
            Internal data(tmp);;
            this->m_strptr->set(data);
        }
    }



    inline StrT& ref(void)
    {
        return this->m_data;
    }

    inline const StrT& ref(void) const
    {
        return this->m_data;
    }

    inline UCharT* cptr(void)
    {
        return &this->m_data[0];
    }

    inline const UCharT* cptr(void) const
    {
        return &this->m_data[0];
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
    StrT m_data;
    mutable std::string            m_narrow;
    string_ptr_base<string_type>  *m_strptr;
};


template<typename T>
class string_ptr_base
{
public:
    virtual ~string_ptr_base(void)
    {}

    virtual    typename T::Internal get(void) const = 0;

    virtual    void set(const typename T::Internal &data) = 0;

};



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
        //throw ustring_error("string_ptr: Unable to update a const string pointer.");
        throw ConversionError();
    }

protected:
    U *m_ptr;

private:
    string_ptr(const string_ptr&);
    string_ptr& operator=(const string_ptr&);
};



typedef ustring<> String;


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
