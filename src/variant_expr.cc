//
// variant_expr.cc - Variant expressions
//
// Copyright (C)         informave.org
//   2012,               Daniel Vogelbacher <daniel@vogelbacher.name>
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
/// @brief Variant expressions
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/variant.hh"

#include <memory>


DB_NAMESPACE_BEGIN


template<typename T, typename U>
struct binary_operator_impl : public binary_operator
{
    virtual Variant add(const Variant &a, const Variant &b)
        {
            return a.get<T>() + b.get<U>();
        }
};



typedef std::shared_ptr<binary_operator> (*binary_function)(void);


template<typename T, typename U>
std::shared_ptr<binary_operator> binary_expr(void)
{
    return std::shared_ptr<binary_operator>(new binary_operator_impl<T, U>());
}




enum { DBWTL_TYPE_COUNT = 2};

binary_function mapping[DBWTL_TYPE_COUNT][2] =
{
    {
        binary_expr<int, int>,
        binary_expr<int, int>
    },
    {
        binary_expr<int, int>,
        binary_expr<int, int>
    }

};



std::shared_ptr<binary_operator> handle(daltype_t a, daltype_t b)
{
    //daltype_t ta = a.datatype();
    //daltype_t tb = b.datatype();

    return mapping[0][0]();
}


Variant
VariantExprHelper::binaryAdd(const Variant &a, const Variant &b)
{
    return handle(a.datatype(), b.datatype())->add(a, b);   
}

Variant
VariantExprHelper::unaryPositive(const Variant &a)
{
    return + a.get<int>();
}



DB_NAMESPACE_END


//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
