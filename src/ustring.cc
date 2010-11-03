//
// ustring.cc - Unicode string and conversion functions
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
/// @brief Unicode string and conversion functions (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1

#include "dbwtl/ustring.hh"
#include "dbwtl/dal/dal_interface.hh"
#include <cassert>

#ifdef DBWTL_USE_ICONV
#include <iconv.h>
#endif

#ifdef DBWTL_USE_ICU
#include <unicode/ucnv.h>
#include <unicode/ucnv_err.h>
#include <unicode/uenum.h>
#include <unicode/localpointer.h>
#endif

#include <iostream>

namespace informave
{

    namespace db
    {
        size_t ustring_base::convert(const char *src, size_t srclen, const char *from,
                                     char *dest, size_t len, const char *to)
        {
            UErrorCode errcode = U_ZERO_ERROR;
            int32_t c = ::ucnv_convert(to,
                                       from,
                                       dest,
                                       len,
                                       src,
                                       srclen,
                                       &errcode);

            if(! U_SUCCESS(errcode))
            {
                switch(errcode)
                {
                case U_BUFFER_OVERFLOW_ERROR:
                    throw ex::charset_error("Buffer overflow", String(from), String(to));

                case U_TRUNCATED_CHAR_FOUND:
                    throw ex::charset_error("Truncated character sequence", String(from), String(to));

                case U_ILLEGAL_CHAR_FOUND:
                case U_INVALID_CHAR_FOUND:
                    throw ex::charset_error("Illegal character found", String(from), String(to));

                default:
                    throw ex::charset_error("Unknown ICU conversion error", String(from), String(to));
                }
            }
            return c;
        }

    }


}
