//
// codecvt_impl.hh - codecvt facet implementation
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

///
/// @cond DEV_DOCS
/// @file
/// @brief codecvt facet implementation
/// @author Daniel Vogelbacher
/// @since 0.0.1

#ifndef INFORMAVE_I18N_CODECVT_IMPL_HH
#define INFORMAVE_I18N_CODECVT_IMPL_HH

#include "types.hh"
#include "converter.hh"

#include <memory>
#include <cassert>
#include <iostream>


//#define TRACE_CODECVT() { std::cout << "codecvt trace: " << __PRETTY_FUNCTION__ << std::endl; }
#define TRACE_CODECVT() 


namespace informave
{
    namespace i18n
    {
        namespace details
        {
            //--------------------------------------------------------------------------
            /// This template class implements a codecvt facet.
            /// @brief codecvt implementation
            /// @internal
            template< 
                class              ElemT, 
                unsigned long      Maxcode = 0x10ffff,
                std::codecvt_mode  Mode = static_cast<std::codecvt_mode>(0),
                typename           ConverterT = details::default_converter_type
                >
                class codecvt_charset_impl : public std::codecvt<ElemT, char, typename std::char_traits<ElemT>::state_type> 
                {
                public:
                    typedef char                                           extern_type;
                    typedef ElemT                                          intern_type;
                    typedef std::codecvt_base::result                      result;
                    typedef typename std::char_traits<ElemT>::state_type   state_type;
                    
                    /// Construct a new codecvt for the given charset
                    explicit codecvt_charset_impl(const char *charset, std::size_t refs = 0) 
                        : std::codecvt<ElemT, char, typename std::char_traits<ElemT>::state_type>(refs),
                          m_impl(new ConverterT(charset, Mode, sizeof(ElemT)))
                        {}

                    /// Get implementation
                    ConverterT& getImpl(void)
                        { return *this->m_impl; }

                    /// Get implementation
                    const ConverterT& getImpl(void) const
                        { return *this->m_impl; }
                    
                    

                protected:

                    /// Always do convertion
                    virtual bool do_always_noconv() const throw ()
                        { 
                            return false; 
                        }


                    ///
                    /// @brief provides information about the type of conversion
                    ///
                    /// @returns
                    ///  -1 if the external representation uses stateful encoding
                    ///   0 if the external representation uses a variable size encoding
                    ///  a constant number representing the maximum width in a fixed-width encoding
                    virtual int do_encoding() const throw ()
                        { 
                            TRACE_CODECVT();
                            return 1; // default, redefined by derived codecvt's
                        }



                    /// Do convertion
                    virtual result do_in(state_type &state,
                                         const extern_type *from,
                                         const extern_type *from_end,
                                         const extern_type *&from_next,
                                         intern_type *to,
                                         intern_type *to_end,
                                         intern_type *&to_next) const
                        { 
                            TRACE_CODECVT();

                            assert(from);
                            assert(from_end);
                            assert(to);
                            assert(to_end);
                            
                            return this->getImpl().convert_in(state, from, from_end, from_next, to, to_end, to_next); 
                        }
        

                    /// Determines and returns n, where n is the number of elements of extern_type
                    /// in the source range [from,end) that can be converted to max or fewer characters
                    /// of intern_type, as if by a call to in(state, from, from_end, from_next, to, to_end, to_next)
                    /// where (to_end==to + max).
                    virtual int do_length(state_type &state,
                                          const extern_type *from,
                                          const extern_type *end,
                                          size_t max) const 
                        { 
                            TRACE_CODECVT();
                            return 0; /// @bug implement me
                        }
        
            

                    /// Returns the maximum value that do_length() can return for any valid combination
                    /// of its first three arguments, with the fourth argument (max) set to 1.
                    virtual int do_max_length() const throw () 
                        { 
                            TRACE_CODECVT();
                            return 2;
                        }



                    /// Do convertion
                    virtual result do_out(state_type &state,
                                          const intern_type *from,
                                          const intern_type *from_end,
                                          const intern_type *&from_next,
                                          extern_type *to,
                                          extern_type *to_end,
                                          extern_type *&to_next) const
                        { 
                            TRACE_CODECVT();

                            assert(from);
                            assert(from_end);
                            assert(to);
                            assert(to_end);
                            
                            if((Mode & std::generate_header) && ! details::_SM<state_type>::hasBom(state))
                            {
                                *to++ = static_cast<char>(0xff);
                                *to++ = static_cast<char>(0xfe);
                                details::_SM<state_type>::bomWritten(state);
                            }
                            return this->getImpl().convert_out(state, from, from_end, from_next, to, to_end, to_next); 
                        }
        


                    /// Do unshift
                    virtual result do_unshift(state_type &state,
                                              extern_type *to,
                                              extern_type *to_end,
                                              extern_type *&to_next) const
                        { 
                            TRACE_CODECVT();
                            to_next=to;
                            return std::codecvt_base::ok;
                        }
                    

                    /// Pointer to the implementation
                    std::auto_ptr<ConverterT>   m_impl;
                };
            
        }
    }
}

/// @endcond

#endif
