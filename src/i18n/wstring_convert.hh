//
// wstring_convert.hh - wstring_convert from c++0x
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
/// @brief wstring_convert from c++0x
/// @author Daniel Vogelbacher
/// @since 0.0.1

#ifndef INFORMAVE_I18N_WSTRING_CONVERT_HH
#define INFORMAVE_I18N_WSTRING_CONVERT_HH


#include <string>
#include <vector>
#include <cstring>
#include <locale>
#include <stdexcept>

//#define SHOW() std::cerr << __PRETTY_FUNCTION__ << std::endl
#define SHOW()

namespace std
{
    
    //--------------------------------------------------------------------------
    /// This template class implements wstring_convert
    /// @brief wstring_convert implementation
    template<class Codecvt, class Elem = wchar_t,
             class Wide_alloc = std::allocator<Elem>,
             class Byte_alloc = std::allocator<char> > 
    class wstring_convert 
    {
    public:
        typedef std::basic_string<char, char_traits<char>, Byte_alloc>   byte_string;
        typedef std::basic_string<Elem, char_traits<Elem>, Wide_alloc>   wide_string;
        typedef typename Codecvt::state_type                             state_type;
        typedef typename wide_string::traits_type::int_type              int_type;

        wstring_convert(Codecvt *pcvt = new Codecvt) : m_cvt(pcvt), m_werr(), m_berr()
            {}

        wstring_convert(Codecvt *pcvt, state_type state) : m_cvt(pcvt), m_werr(), m_berr()
            {}

        wstring_convert(const byte_string& byte_err,
                        const wide_string& wide_err = wide_string()) : m_cvt(), m_werr(), m_berr()
            {}
        
        ~wstring_convert()
            {
                delete this->m_cvt;
            }

        /// bug needs implementation
        wide_string from_bytes(char byte)
            {
                return "";
                /*
                const char bytes[1] = {byte}, *bnext;

                codecvt_base::result re = cvt->in..

                if(re == codecvt_base::error){
                    if(werr.empty())
                        __ntl_throw(range_error("conversion from bytes failed"));
                */
                    
            }

        ///
        ///
        wide_string from_bytes(const char *ptr)
            {
                using namespace std;
                std::vector<Elem> tmp;
                state_type    state;
                std::size_t   from_len  = ::strlen(ptr);
                const char   *from_next = 0;
                Elem         *to_next   = 0;

                tmp.resize(from_len);
                std::codecvt_base::result res = this->m_cvt->in(state,
                                                                ptr, ptr + from_len, from_next,
                                                                &tmp[0], (&tmp[0] + from_len), to_next);
                if(res == codecvt_base::error)
                {
                    if(this->m_werr.empty())
                        throw range_error("conversion from bytes failed");
                    return this->m_werr;
                }
                return wide_string(&tmp[0], to_next - &tmp[0]);
            }


        ///
        wide_string from_bytes(const byte_string& str)
            {
                return this->from_bytes(str.data(), str.data() ? (str.data() + str.length()) : 0);
            }


        ///
        wide_string from_bytes(const char *first, const char *last)
            {
                std::vector<Elem> tmp;
                state_type    state;
                std::size_t   from_len  = last - first;
                const char   *from_next = 0;
                Elem         *to_next   = 0;

                tmp.resize(from_len);
                std::codecvt_base::result res = this->m_cvt->in(state,
                                                                first, last, from_next,
                                                                &tmp[0], (&tmp[0] + from_len), to_next);
                
                if(res == std::codecvt_base::error)
                {
                    if(this->m_werr.empty())
                        throw range_error("conversion from bytes failed");
                    return this->m_werr;
                }

                return wide_string(&tmp[0], to_next - &tmp[0]);
            }


        /// bug needs implementation
        byte_string to_bytes(Elem wchar)
            { }

        /// bug needs implementation
        byte_string to_bytes(const Elem *wptr)
            { }

        ///
        byte_string to_bytes(const wide_string& wstr)
            {
                std::vector<char> tmp;
                state_type    state;
                std::size_t   from_len  = wstr.size();
                char         *to_next   = 0;
                const Elem   *from_next = 0;
                std::size_t   enc_size  = 0;
                /// bug needs fixes for stateful encodings
                enc_size = this->m_cvt->encoding() > 0 ? this->m_cvt->encoding() : 4;
                tmp.resize(from_len * enc_size);

                std::codecvt_base::result res = this->m_cvt->out(state,
                                                                 wstr.c_str(), wstr.c_str() + wstr.size(), from_next,
                                                                 &tmp[0], (&tmp[0] + tmp.size()), to_next);
                if(res == std::codecvt_base::error)
                {
                    if(this->m_berr.empty())
                        throw range_error("conversion to bytes failed");
                    return this->m_berr;
                }

                return byte_string(&tmp[0], to_next - &tmp[0]);
            }


        byte_string to_bytes(const Elem *first, const Elem *last)
            { 
                return ""; /// bug fixme
            }

        size_t converted() const
            { 
                return 0;  /// bug fixme
            }

        state_type state() const
            { 
                return state_type();  /// bug fixme
            }


    protected:
        Codecvt *m_cvt;
        std::wstring m_werr;
        std::string m_berr;

        wstring_convert(const wstring_convert&);
        wstring_convert& operator=(const wstring_convert&);
    };


} /// namespace std


#endif
