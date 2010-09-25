//
// codecvt_byname.hh - codecvt_byname facet implementation
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
/// @brief codecvt_byname facet implementation
/// @author Daniel Vogelbacher
/// @since 0.0.1

#ifndef INFORMAVE_I18N_CODECVT_BYNAME_HH
#define INFORMAVE_I18N_CODECVT_BYNAME_HH

#include "types.hh"
#include "codecvt_impl.hh"

namespace informave
{
    namespace i18n
    {
        //--------------------------------------------------------------------------
        /// This template class implements a codecvt_byname facet.
        ///
        /// @brief codecvt_byname implementation
        /// @internal
        template<class              Elem,
                 unsigned long      Maxcode = 0x10ffff,
                 std::codecvt_mode  Mode = static_cast<std::codecvt_mode>(0)>
        class codecvt_charset_byname : public details::codecvt_charset_impl<Elem, Maxcode, Mode>
        {
        public:
            std::string m_encoding;
            
            codecvt_charset_byname(const char *name) : details::codecvt_charset_impl<Elem, Maxcode, Mode>(name),
                                                       m_encoding(name)
                {}
            
            virtual int do_encoding() const throw ()
                { 
                    if(this->m_encoding == "UTF-32")
                        return 4;
                    else if(this->m_encoding == "UTF-16")
                        return 2;
                    else if(this->m_encoding == "UTF-8")
                        return 0; /// bug needs fixes for different charsets
                    else
                        return 1;
                }
        };
    

        //--------------------------------------------------------------------------
        /// @brief Code Conversion from UCS to UTF-8
        /// @internal
        template<class Elem,
                 unsigned long Maxcode = 0x10ffff,
                 std::codecvt_mode  Mode = static_cast<std::codecvt_mode>(0)>
        class codecvt_utf8 : public details::codecvt_charset_impl<Elem, Maxcode, Mode>
        {
        public:
            explicit codecvt_utf8(std::size_t refs = 0) 
                : details::codecvt_charset_impl<Elem, Maxcode, Mode>("UTF-8", refs)
                { }
        
        protected:

            virtual int do_encoding() const throw ()
                { 
                    return 0; // multibyte charset
                }
        };


        //--------------------------------------------------------------------------
        /// @brief Code Conversion from UCS to UTF-16
        /// @internal
        template<class Elem,
                 unsigned long Maxcode = 0x10ffff,
                 std::codecvt_mode  Mode = static_cast<std::codecvt_mode>(0)>
        class codecvt_utf16 : public details::codecvt_charset_impl<Elem, Maxcode, Mode>
        {
        public:
            explicit codecvt_utf16(std::size_t refs = 0) 
                : details::codecvt_charset_impl<Elem, Maxcode, Mode>("UTF-16", refs)
                { }

            typedef char                                          extern_type;
            typedef Elem                                          intern_type;
            typedef std::codecvt_base::result                     result;
            typedef typename std::char_traits<Elem>::state_type   state_type;
        
        
        protected:

            virtual int do_encoding() const throw ()
                { 
                    return 0; // multibyte charset
                }

            virtual result do_out(state_type &state,
                                  const intern_type *from,
                                  const intern_type *from_end,
                                  const intern_type *&from_next,
                                  extern_type *to,
                                  extern_type *to_end,
                                  extern_type *&to_next) const
                { 
                    if((Mode & std::generate_header) && ! details::_SM<state_type>::hasBom(state))
                    {
                        if(Mode & std::little_endian)
                        {
                            *to++ = 0xff;
                            *to++ = 0xfe;
                        }
                        else
                        {
                            *to++ = 0xfe;
                            *to++ = 0xff;
                        }
                        details::_SM<state_type>::bomWritten(state);
                    }
                
                    return details::codecvt_charset_impl<Elem, Maxcode, Mode>::do_out(
                        state, from, from_end, from_next, to, to_end, to_next);
                }  
        };



        //--------------------------------------------------------------------------
        /// @brief Code Conversion from UCS to UTF-32
        /// @internal
        template<class Elem,
                 unsigned long Maxcode = 0x10ffff,
                 std::codecvt_mode  Mode = static_cast<std::codecvt_mode>(0)>
        class codecvt_utf32 : public details::codecvt_charset_impl<Elem, Maxcode, Mode>
        {
        public:
            explicit codecvt_utf32(std::size_t refs = 0) 
                : details::codecvt_charset_impl<Elem, Maxcode, Mode>("UTF-32", refs)
                {}

            typedef char                                           extern_type;
            typedef Elem                                           intern_type;
            typedef std::codecvt_base::result                      result;
            typedef typename std::char_traits<Elem>::state_type    state_type;
        
        
        protected:

            virtual int do_encoding() const throw ()
                { 
                    return 4;
                }

            virtual result do_out(state_type &state,
                                  const intern_type *from,
                                  const intern_type *from_end,
                                  const intern_type *&from_next,
                                  extern_type *to,
                                  extern_type *to_end,
                                  extern_type *&to_next) const
                { 
                    if((Mode & std::generate_header) && ! details::_SM<state_type>::hasBom(state))
                    {
                        if(Mode & std::little_endian)
                        {
                            *to++ = 0xff;
                            *to++ = 0xfe;
                            *to++ = 0x00;
                            *to++ = 0x00;
                        }
                        else
                        {
                            *to++ = 0x00;
                            *to++ = 0x00;
                            *to++ = 0xfe;
                            *to++ = 0xff;
                        }
                        details::_SM<state_type>::bomWritten(state);
                    }
                
                    return details::codecvt_charset_impl<Elem, Maxcode, Mode>::do_out(
                        state, from, from_end, from_next, to, to_end, to_next);
                }  
        };


        //--------------------------------------------------------------------------
        /// @brief Code Conversion from UCS to ISO-8859-1
        /// @internal
        template<class Elem,
                 unsigned long Maxcode = 0x10ffff>
        class codecvt_iso8859_1 : public details::codecvt_charset_impl<Elem, Maxcode>
        {
        public:
            explicit codecvt_iso8859_1(std::size_t refs = 0) 
                : details::codecvt_charset_impl<Elem, Maxcode>("ISO-8859-1", refs)
                {}
        };
    

        //--------------------------------------------------------------------------
        /// @brief Code Conversion from UCS to ISO-8859-15    
        /// @internal
        template<class Elem,
                 unsigned long Maxcode = 0x10ffff>
        class codecvt_iso8859_15 : public details::codecvt_charset_impl<Elem, Maxcode>
        {
        public:
            explicit codecvt_iso8859_15(std::size_t refs = 0) 
                : details::codecvt_charset_impl<Elem, Maxcode>("ISO-8859-15", refs)
                {}
        };

    }
}

#endif
