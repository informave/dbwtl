//
// ustreambuf.hh - Unicode buffer and conversion functions
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
/// @brief Unicode buffer and conversion functions
/// @author Daniel Vogelbacher
/// @since 0.0.1

#ifndef INFORMAVE_I18N_USTREAMBUF_HH
#define INFORMAVE_I18N_USTREAMBUF_HH

#include "db_fwd.hh"


#include <stdexcept>
#include <locale>
#include <iosfwd>

#ifdef DBWTL_CXX98_COMPATIBILITY
#include <tr1/type_traits>
#else
#include <type_traits>
#endif


DB_NAMESPACE_BEGIN


typedef std::wstreambuf        UnicodeStreamBuf;
typedef std::streambuf         ByteStreamBuf;


template<class Elem,
         class Tr = std::char_traits<Elem> >
class streambuf_convert : public std::basic_streambuf<wchar_t>
{
public:
    typedef std::basic_streambuf<Elem, Tr> stream_type;

    typedef typename Tr::state_type state_type;

    streambuf_convert(stream_type *buf = 0,
                      state_type state = state_type())
        : m_bufptr(buf),
          m_state(state)
    {}

    stream_type* rdbuf() const
    {
        return this->m_bufptr;
    }

    stream_type* rdbuf(stream_type *buf)
    {
        stream_type *tmp = this->m_bufptr;
        this->m_bufptr = buf;
        return tmp;
    }


    state_type state() const
    {
        return this->m_state;
    }

private:
    stream_type *m_bufptr;
    //Codecvt *cvtptr;
    state_type m_state;
};



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
