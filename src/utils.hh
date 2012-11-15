//
// utils.hh - Internal utils, DO NOT INCLUDE THIS FILE IN NON-DBWTL CODE
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
/// @brief Internal utils
/// @author Daniel Vogelbacher
/// @since 0.0.1

#ifndef INFORMAVE_DB_UTILS_HH
#define INFORMAVE_DB_UTILS_HH


#include "dbwtl/db_fwd.hh"

#include <vector>


#if DBWTL_INTERNAL_CHARTYPE == 1
#define US(str) str
#elif DBWTL_INTERNAL_CHARTYPE == 2
#define US(str) L##str
#elif DBWTL_INTERNAL_CHARTYPE == 3
#define US(str) u##str
#elif DBWTL_INTERNAL_CHARTYPE == 4
#define US(str) U##str
#endif



#define DBWTL_BUGCHECK2(cond) { if(! (cond) ) utils::raise_bugcheck_exception(#cond, "", __LINE__, __FILE__, __PRETTY_FUNCTION__); }



DB_NAMESPACE_BEGIN

namespace utils
{
    void raise_bugcheck_exception(const char *cond, const char *what, int line, const char *file, const char *function);


    template<typename T>
    inline std::vector<T> split(const T& str, const typename T::value_type del)
    {
        std::vector<T> v;

        typename T::const_iterator i(str.begin());
        typename T::const_iterator end(str.end());

        T tmp;
        while(i != end)
        {
            if(*i != del) tmp.push_back(*i);
            else
            {
                v.push_back(tmp);
                tmp.clear();
            }
            ++i;
        }
        if(!tmp.empty()) v.push_back(tmp);

        return v;
    }



    template<typename T, typename U>
    inline bool between(const T &v, const U &min, const U &max)
    {
        return (v >= min) && (v <= max);
    }


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
