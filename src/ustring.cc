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


namespace informave
{
    namespace i18n
    {

        ///
        ///
        UString conv_from(const std::string& str, const char *encoding)
        {
            if(str.size() == 0)
                return UString();

            typedef codecvt_charset_byname<UString::value_type>
                ConvT;
            std::wstring_convert<ConvT> conv(new ConvT(encoding));
            return conv.from_bytes(str);
        }


        ///
        ///
        UString conv_from(const char *str, const char *encoding)
        {
            assert(str);
            if(str[0] == 0)
                return UString();
            typedef codecvt_charset_byname<UString::value_type>
                ConvT;
            std::wstring_convert<ConvT> conv(new ConvT(encoding));
            return conv.from_bytes(str);
        }

	
        ///
        ///
        std::string conv_to(UString str, const char *encoding)
        {
            if(str.size() == 0)
                return std::string();
            typedef codecvt_charset_byname<UString::value_type>
                ConvT;
            std::wstring_convert<ConvT> conv(new ConvT(encoding));
            return conv.to_bytes(str);
        }

    }
}
