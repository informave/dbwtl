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

//#ifdef DBWTL_USE_ICU
#include <unicode/ucnv.h>
#include <unicode/ucnv_err.h>
#include <unicode/uenum.h>
#include <unicode/localpointer.h>
//#endif

#include <iostream>
#include <map>

namespace informave
{

    namespace db
    {




typedef std::map<std::string, UConverter*> converter_map_type;

#ifdef _WIN32
__declspec( thread ) converter_map_type *converterMap = 0;
#else
 __thread converter_map_type *converterMap = 0;
#endif



size_t
ustring_icu::from_wide(char *cdest, size_t cdestbuf,
                       const UCharT *wsrc, size_t wlen,
                       const std::string &charset) const
{
    if(!converterMap)
        converterMap = new converter_map_type();

    converter_map_type::iterator p = converterMap->find(std::string(charset));
    if(p == converterMap->end())
    {
        UErrorCode errcode = U_ZERO_ERROR;
        UConverter *conv = ::ucnv_open(charset.c_str(), &errcode);
		if(! U_SUCCESS(errcode))
		{
			throw ConversionError();
		}
		(*converterMap)[charset] = conv;
		p = converterMap->find(charset);
    }

    UConverter *conv = p->second;
    UErrorCode errcode = U_ZERO_ERROR;

    int32_t c = ::ucnv_fromUChars(conv,
                                  cdest, cdestbuf,
                                  (const UChar*)wsrc, wlen,
                                  &errcode);
    if(! U_SUCCESS(errcode))
    {
        throw ConversionError();
    }
    return c;

}

size_t
ustring_icu::to_wide(UCharT *wdest, size_t wbuflen,
                     const char *csrc, size_t clen,
                     const std::string &charset) const
{
    if(!converterMap)
        converterMap = new converter_map_type();

    converter_map_type::iterator p = converterMap->find(charset);
    if(p == converterMap->end())
    {
        UErrorCode errcode = U_ZERO_ERROR;
        UConverter *conv = ::ucnv_open(charset.c_str(), &errcode);
		if(! U_SUCCESS(errcode))
		{
			throw ConversionError();
		}
		(*converterMap)[std::string(charset)] = conv;
		p = converterMap->find(charset);
    }

    UConverter *conv = p->second;
    UErrorCode errcode = U_ZERO_ERROR;

    int32_t c = ::ucnv_toUChars(conv,
                                (UChar*)wdest, wbuflen,
                                csrc, clen,
                                &errcode);


    if(! U_SUCCESS(errcode))
    {
        throw ConversionError();
    }
    return c;
}




size_t
ustring_icu::utf16_to_utf8(char *cdest, int32_t cbuflen,
				const UCharT *wsrc, size_t wlen) const
{
	int32_t len = 0;
	UErrorCode errcode = U_ZERO_ERROR;
	::u_strToUTF8(cdest, cbuflen, &len, (const UChar*)wsrc, wlen, &errcode);
	    if(! U_SUCCESS(errcode))
	        {
		        throw ConversionError();
	}
	return len;
}

size_t
ustring_icu::utf8_to_utf16(UCharT *wdest, int32_t wbuflen,
				const char *csrc, size_t clen) const
{
        int32_t len = 0;
        UErrorCode errcode = U_ZERO_ERROR;
        u_strFromUTF8((UChar*)wdest, wbuflen, &len, csrc, clen, &errcode);
            if(! U_SUCCESS(errcode))
                {
                        throw ConversionError();
        }
        return len;
}






size_t
ustring_icu::utf32_to_utf16(UCharT *wdest, size_t wbuflen,
                            const char32_t *wsrc, size_t wlen) const
{
	static_assert(sizeof(UCharT) == sizeof(UChar), "Invalid UCharT size");
	int32_t len = 0;
	UErrorCode errcode = U_ZERO_ERROR;
	u_strFromUTF32((UChar*)wdest, wbuflen, &len, (const UChar32*)wsrc, wlen, &errcode);
    if(! U_SUCCESS(errcode))
    {
        throw ConversionError();
    }

	return len;
/*
    size_t c = 0;
    while(*wsrc && wlen && wbuflen)
    {
        *wdest++ = *wsrc++;
        --wlen;
        --wbuflen;
        ++c;
    }
    return c;
*/
}

size_t
ustring_icu::utf16_to_utf32(char32_t *wdest, size_t wbuflen,
                            const UCharT *wsrc, size_t wlen) const
{
	static_assert(sizeof(char32_t) == sizeof(UChar32), "Invalid char32_t size");
	int32_t len = 0;
	UErrorCode errcode = U_ZERO_ERROR;
	u_strToUTF32((UChar32*)wdest, wbuflen, &len, (UChar*)wsrc, wlen, &errcode);
    if(! U_SUCCESS(errcode))
    {
        throw ConversionError();
    }
	return len;

/*
    size_t c = 0;
    while(*wsrc && wlen && wbuflen)
    {
        *wdest++ = *wsrc++;
        --wlen;
        --wbuflen;
        ++c;
    }
    return c;
*/
}







        int32_t ustring_base::convert(const char *src, size_t srclen, const char *from,
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
                    throw CharconvException("Buffer overflow", String(from), String(to));

                case U_TRUNCATED_CHAR_FOUND:
                    throw CharconvException("Truncated character sequence", String(from), String(to));

                case U_ILLEGAL_CHAR_FOUND:
                case U_INVALID_CHAR_FOUND:
                    throw CharconvException("Illegal character found", String(from), String(to));

                default:
                    throw CharconvException("Unknown ICU conversion error", String(from), String(to));
                }
            }
            return c;
        }

    }


}
