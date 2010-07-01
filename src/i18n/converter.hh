//
// converter.hh - Charset converter
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
/// @brief Charset converter
/// @author Daniel Vogelbacher
/// @since 0.0.1
/// @bug perform correct checks for sizeof(wchar_t)

#ifndef INFORMAVE_I18N_CONVERTER_HH
#define INFORMAVE_I18N_CONVERTER_HH

#include "dbwtl/dbwtl_config.hh"

#include <locale>
#include <iostream>
#include <cassert>
#include <cerrno>
#include <cstdlib>

#ifdef DBWTL_USE_ICONV
#include <iconv.h>
#endif

#ifdef DBWTL_USE_ICU
#include <unicode/ucnv.h>
#include <unicode/ucnv_err.h>
#include <unicode/uenum.h>
#include <unicode/localpointer.h>
#endif



namespace informave
{
    namespace i18n
    {
        namespace details
        {

            //--------------------------------------------------------------------------
            /// This template class represent a converter state
            /// @brief converter state
            template<class T>
            struct _SM
            {
                typedef char _stateType;
                
                static _stateType& cast(T& state)
                    {
                        return reinterpret_cast<_stateType&>(state);
                    }
                
                static void bomWritten(T& state)
                    {
                        _stateType& s = cast(state);
                        s = s | 0xf0;
                    }
                
                static bool hasBom(T& state)
                    {
                        return cast(state) & 0xf0;
                    }
            };


            //--------------------------------------------------------------------------
            /// This is the base class for all implemented converters.
            /// @brief Base class for converters
            class Converter
            {
            public:
                typedef std::codecvt_base::result    result;
                typedef std::mbstate_t               state_type;
                
                virtual ~Converter(void) {}
                
                virtual result convert_in(state_type &state,
                                          const char *from,
                                          const char *from_end,
                                          const char *&from_next,
                                          wchar_t *to,
                                          wchar_t *to_end,
                                          wchar_t *&to_next) const = 0;       
                
                virtual result convert_out(state_type &state,
                                           const wchar_t *from,
                                           const wchar_t *from_end,
                                           const wchar_t *&from_next,
                                           char *to,
                                           char *to_end,
                                           char *&to_next) const = 0;                    
                
                
                virtual std::string getError() const
                    {
                        return "<convert error>"; /// @bug fixme
                    }
            };



            //--------------------------------------------------------------------------
            /// ICU Library converter
            /// @brief ICU Library converter
            class ICUConverterImpl : public Converter
            {
            public:
            protected:
                const char *m_enc;
                std::size_t m_bcout;
                
                ICUConverterImpl(const ICUConverterImpl&);
                ICUConverterImpl& operator=(const ICUConverterImpl&);
                
            public:
                ICUConverterImpl(const char *charset, std::codecvt_mode mode, std::size_t size)
                    : m_enc(charset), m_bcout(size)
                    {
                        if(std::string(charset) == "UTF-16")
                        {
                            if(mode & std::little_endian)
                                this->m_enc = "UTF-16LE";
                            else
                                this->m_enc = "UTF-16BE";
                        }
                        if(std::string(charset) == "UTF-32")
                        {
                            if(mode & std::little_endian)
                                this->m_enc = "UTF-32LE";
                            else
                                this->m_enc = "UTF-32BE";
                        }
                    }
            

                virtual ~ICUConverterImpl(void) {}
                

                ///
                ///
                virtual result convert_in(state_type &state,
                                          const char *from,
                                          const char *from_end,
                                          const char *&from_next,
                                          wchar_t *to,
                                          wchar_t *to_end,
                                          wchar_t *&to_next) const
                    {
                        SHOW();
                        result ret = std::codecvt_base::ok;

                        UErrorCode errcode = U_ZERO_ERROR;
                        int32_t c = ::ucnv_convert(sizeof(wchar_t) == 4 ? "UTF-32LE" : "UTF-16LE",
                                                   this->m_enc,
                                                   reinterpret_cast<char*>(to),
                                                   (to_end - to) * sizeof(wchar_t),
                                                   from,
                                                   (from_end - from) * sizeof(char),
                                                   &errcode);
                        if(! U_SUCCESS(errcode))
                        {
                            switch(errcode)
                            {
                            case U_BUFFER_OVERFLOW_ERROR: 
                                std::cout << "BIG" << std::endl; 
                                ret = std::codecvt_base::partial;
                                break;
                            case U_TRUNCATED_CHAR_FOUND:
                                std::cout << "SEQ" << std::endl;
                                ret = std::codecvt_base::error;
                                break;
                                
                            case U_ILLEGAL_CHAR_FOUND: 
                            case U_INVALID_CHAR_FOUND:
                                std::cout << "INVALID" << std::endl; 
                                ret = std::codecvt_base::error;
                                break;
                            default:
                                std::cout << "ERR" << std::endl; 
                                ret = std::codecvt_base::error;
                                break; /// @bug fixme
                            }
                        }

                        from_next = from_end + 1;
                        to_next = reinterpret_cast<wchar_t*>(reinterpret_cast<char*>(to) + c);

                        return ret;
                    }


                ///
                ///
                virtual result convert_out(state_type &state,
                                           const wchar_t *from,
                                           const wchar_t *from_end,
                                           const wchar_t *&from_next,
                                           char *to,
                                           char *to_end,
                                           char *&to_next) const
                    { 
                        SHOW();
                        result ret = std::codecvt_base::ok;

                        UErrorCode errcode = U_ZERO_ERROR;
                        int32_t c = ::ucnv_convert(this->m_enc, sizeof(wchar_t) == 4 ? "UTF-32LE" : "UTF-16LE",
                                                   to,
                                                   (to_end - to) * sizeof(char),
                                                   reinterpret_cast<const char*>(from),
                                                   (from_end - from) * sizeof(wchar_t),
                                                   &errcode);
                        
                        if(! U_SUCCESS(errcode))
                        {
                            switch(errcode)
                            {
                            case U_BUFFER_OVERFLOW_ERROR: 
                                std::cout << "BIG" << std::endl; 
                                ret = std::codecvt_base::partial;
                                break;
                            case U_TRUNCATED_CHAR_FOUND:
                                std::cout << "SEQ" << std::endl;
                                ret = std::codecvt_base::error;
                                break;
                                
                            case U_ILLEGAL_CHAR_FOUND: 
                            case U_INVALID_CHAR_FOUND:
                                std::cout << "INVALID" << std::endl; 
                                ret = std::codecvt_base::error;
                                break;
                            default:
                                std::cout << "ERR" << std::endl; 
                                ret = std::codecvt_base::error;
                                break; /// @bug fixme
                            }
                        }

                        to_next = to + c;
                        from_next = from_end + 1;

                        return ret;
                    }  
            };


#ifdef DBWTL_USE_ICONV

            //--------------------------------------------------------------------------
            /// ICONV Library converter
            /// @brief ICONV Library converter
            class IconvConverterImpl : public Converter
            {
            protected:
                const char *m_enc;
                std::size_t m_bcout;
                
                IconvConverterImpl(const IconvConverterImpl&);
                IconvConverterImpl& operator=(const IconvConverterImpl&);
                
            public:
                IconvConverterImpl(const char *charset, std::codecvt_mode mode, std::size_t size)
                    : m_enc(charset), m_bcout(size)
                    {
                        if(std::string(charset) == "UTF-16")
                        {
                            if(mode & std::little_endian)
                                this->m_enc = "UTF-16LE";
                            else
                                this->m_enc = "UTF-16BE";
                        }
                        if(std::string(charset) == "UTF-32")
                        {
                            if(mode & std::little_endian)
                                this->m_enc = "UTF-32LE";
                            else
                                this->m_enc = "UTF-32BE";
                        }
                    }
            

                virtual ~IconvConverterImpl(void) {}
                

                ///
                ///
                virtual result convert_in(state_type &state,
                                          const char *from,
                                          const char *from_end,
                                          const char *&from_next,
                                          wchar_t *to,
                                          wchar_t *to_end,
                                          wchar_t *&to_next) const
                    {
                        SHOW();
                        result ret = std::codecvt_base::ok;

                        iconv_t cd = iconv_open("UCS-4LE", this->m_enc);
                        if(cd == reinterpret_cast<iconv_t>(-1))
                            abort();
                        char* src = const_cast<char*>(from);
                        char* dest = reinterpret_cast<char*>(to);
                        size_t c = (from_end - from); //* sizeof(char);
                        size_t out = (to_end - to) * sizeof(wchar_t); //elem count * size, because iconv needs byte count
                        //std::cerr << c << " - " << out << " - " << std::endl;
                        size_t copy = iconv(cd, &src, &c, &dest,  &out);
                        //std::cerr << c << " - " << out << " - " << copy << std::endl;
                        if(copy == static_cast<size_t>(-1))
                        {
                            switch(errno)
                            {
                            case E2BIG: 
                                std::cout << "BIG" << std::endl; 
                                ret = std::codecvt_base::partial;
                                break;
                            case EILSEQ:
                                std::cout << "SEQ" << std::endl;
                                ret = std::codecvt_base::error;
                                break;
                                
                            case EINVAL: 
                                std::cout << "INVALID" << std::endl; 
                                ret = std::codecvt_base::partial;
                                break;
                            default:
                                break; /// @bug fixme
                            }
                        }
                        iconv_close(cd);
                        
                        from_next = src;
                        to_next = reinterpret_cast<wchar_t*>(dest);
                        return ret;
                    }


                ///
                ///
                virtual result convert_out(state_type &state,
                                           const wchar_t *from,
                                           const wchar_t *from_end,
                                           const wchar_t *&from_next,
                                           char *to,
                                           char *to_end,
                                           char *&to_next) const
                    { 
                        SHOW();
                        result ret = std::codecvt_base::ok;
                        
                        iconv_t cd = iconv_open(this->m_enc, "UCS-4LE");
                        if(cd == reinterpret_cast<iconv_t>(-1))
                            abort();
                        char* src = reinterpret_cast<char*>(const_cast<wchar_t*>(from));
                        char* dest = to;
                        size_t c = (from_end - from) * sizeof(wchar_t);
                        size_t out = (to_end - to);
                        //std::cerr << c << " - " << out << " - " << std::endl;
                        size_t copy = iconv(cd, &src, &c, &dest,  &out);
                        //std::cerr << c << " - " << out << " - " << copy << std::endl;
                        if(copy == static_cast<size_t>(-1))
                        {
                            switch(errno)
                            {
                            case E2BIG: 
                                std::cout << "BIG" << std::endl; 
                                ret = std::codecvt_base::partial;
                                break;
                            case EILSEQ:
                                std::cout << "SEQ" << std::endl;
                            ret = std::codecvt_base::error;
                            break;
                            
                            case EINVAL: 
                                std::cout << "INVALID" << std::endl; 
                                ret = std::codecvt_base::partial;
                                break;
                            default:
                                break; /// @bug fixme
                            }
                        }
                        iconv_close(cd);
                        
                        from_next = reinterpret_cast<wchar_t*>(src);
                        to_next = dest;
                        return ret;
                    }  
            };

#endif

#ifdef DBWTL_USE_ICONV
	typedef details::IconvConverterImpl default_converter_type;
#endif

#ifdef DBWTL_USE_ICU
	typedef details::ICUConverterImpl default_converter_type;
#endif
		}
    }
}

#endif
