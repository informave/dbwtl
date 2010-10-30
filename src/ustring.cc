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

#include <cassert>
#include "i18n/wstring_convert.hh"
#include "i18n/codecvt_byname.hh"

#include "dbwtl/ustring.hh"

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
	ssize_t ustring_base::convert(const char *src, ssize_t srclen, const char *from, char *dest, ssize_t len, const char *to)
	{


//	std::cout << "output" << from << to << src << std::endl;

                        UErrorCode errcode = U_ZERO_ERROR;
                        int32_t c = ::ucnv_convert(to,
                                                   from,
                                                   dest,
						   len,
						src,
						srclen,
						&errcode);

	//std::wcout << (wchar_t*)dest << std::endl;

                        if(! U_SUCCESS(errcode))
                        {
                            switch(errcode)
                            {
                            case U_BUFFER_OVERFLOW_ERROR:
                                std::cout << "BIG" << std::endl;
                                break;
                            case U_TRUNCATED_CHAR_FOUND:
                                std::cout << "SEQ" << std::endl;
                                break;

                            case U_ILLEGAL_CHAR_FOUND:
                            case U_INVALID_CHAR_FOUND:
                                std::cout << "INVALID" << std::endl;
                                break;
                            default:
                                std::cout << "ERR" << std::endl;
                                break; /// bug fixme
                            }
                        }

		return c;
	}

}

    namespace i18n
    {

        ///
        ///
        std::wstring conv_from(const std::string& str, const char *encoding)
        {
            if(str.size() == 0)
                return std::wstring();

            typedef codecvt_charset_byname<std::wstring::value_type>
                ConvT;
            std::wstring_convert<ConvT> conv(new ConvT(encoding));
            return conv.from_bytes(str);
        }


        ///
        ///
        std::wstring conv_from(const char *str, const char *encoding)
        {
            //assert(str);
            if(!str || str[0] == 0)
                return std::wstring();
            typedef codecvt_charset_byname<std::wstring::value_type>
                ConvT;
            std::wstring_convert<ConvT> conv(new ConvT(encoding));
            return conv.from_bytes(str);
        }

	
        ///
        ///
        std::string conv_to(std::wstring str, const char *encoding)
        {
            if(str.size() == 0)
                return std::string();
            typedef codecvt_charset_byname<std::wstring::value_type>
                ConvT;
            std::wstring_convert<ConvT> conv(new ConvT(encoding));
            return conv.to_bytes(str);
        }

    }
}
