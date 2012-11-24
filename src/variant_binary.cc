//
// variant_binary.cc - Variant binary operators
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
/// @brief Variant binary operators
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/variant.hh"

#include <memory>
#include <iostream>
#include <memory>
#include <cstdlib>



DB_NAMESPACE_BEGIN



struct UnsupportedException {};

template<typename T>
struct binary_op_unsupported : public BinaryOperatorVariant
{
    virtual Variant binary_xor           (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_or            (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_and           (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_mod           (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_mul           (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_div           (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_add           (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_concat        (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_sub           (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_less          (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_less_equal    (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_equal         (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_not_equal     (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_greater       (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
    virtual Variant binary_greater_equal (const Variant &v0, const Variant &v1) const { throw UnsupportedException(); };
};


template<typename T>
struct binary_op : public BinaryOperatorVariant
{
    virtual Variant binary_xor           (const Variant &v0, const Variant &v1) const { return v0.get<bool>() ^ v1.get<bool>(); }
    virtual Variant binary_or            (const Variant &v0, const Variant &v1) const { return v0.get<bool>() || v1.get<bool>(); }
    virtual Variant binary_and           (const Variant &v0, const Variant &v1) const { return v0.get<bool>() && v1.get<bool>(); }
    virtual Variant binary_mod           (const Variant &v0, const Variant &v1) const { return v0.get<T>() % v1.get<T>(); }
    virtual Variant binary_mul           (const Variant &v0, const Variant &v1) const { return v0.get<T>() * v1.get<T>(); }
    virtual Variant binary_div           (const Variant &v0, const Variant &v1) const { return v0.get<T>() / v1.get<T>(); }
    virtual Variant binary_add           (const Variant &v0, const Variant &v1) const { return v0.get<T>() + v1.get<T>(); }
    virtual Variant binary_concat        (const Variant &v0, const Variant &v1) const { return v0.get<String>() + v1.get<String>(); }
    virtual Variant binary_sub           (const Variant &v0, const Variant &v1) const { return v0.get<T>() - v1.get<T>(); }
    virtual Variant binary_less          (const Variant &v0, const Variant &v1) const { return v0.get<T>() < v1.get<T>(); }
    virtual Variant binary_less_equal    (const Variant &v0, const Variant &v1) const { return v0.get<T>() <= v1.get<T>(); }
    virtual Variant binary_equal         (const Variant &v0, const Variant &v1) const { return v0.get<T>() == v1.get<T>(); }
    virtual Variant binary_not_equal     (const Variant &v0, const Variant &v1) const { return v0.get<T>() != v1.get<T>(); }
    virtual Variant binary_greater       (const Variant &v0, const Variant &v1) const { return v0.get<T>() > v1.get<T>(); }
    virtual Variant binary_greater_equal (const Variant &v0, const Variant &v1) const { return v0.get<T>() >= v1.get<T>(); }

};


template<typename T>
struct binary_op_impl : public binary_op_unsupported<T>
{
};


template<> struct binary_op_impl<signed int> : public binary_op<signed int> { };
template<> struct binary_op_impl<signed short> : public binary_op<signed short> { };
template<> struct binary_op_impl<signed long long> : public binary_op<signed long long> { };
template<> struct binary_op_impl<signed char> : public binary_op<signed char> { };

template<> struct binary_op_impl<unsigned int> : public binary_op<unsigned int> { };
template<> struct binary_op_impl<unsigned short> : public binary_op<unsigned short> { };
template<> struct binary_op_impl<unsigned long long> : public binary_op<unsigned long long> { };
template<> struct binary_op_impl<unsigned char> : public binary_op<unsigned char> { };

template<> struct binary_op_impl<bool> : public binary_op<bool> { };
//template<> struct binary_op_impl<TNumeric> : public binary_op<TNumeric> { };

template<> struct binary_op_impl<String> : public binary_op_unsupported<String>
{
    virtual Variant binary_concat(const Variant &v0, const Variant &v1) const { return v0.get<String>() + v1.get<String>(); }
    virtual Variant binary_equal(const Variant &v0, const Variant &v1) const { return v0.get<String>() == v1.get<String>(); }
    virtual Variant binary_not_equal(const Variant &v0, const Variant &v1) const { return ! (v0.get<String>() == v1.get<String>()); }
};



static daltype_t resolve_higher(const Variant &op0, const Variant &op1)
{
    switch(op0.datatype())
    {
    case DAL_TYPE_CUSTOM:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_UNKNOWN:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_INT:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_INT;
        case DAL_TYPE_UINT:          return DAL_TYPE_BIGINT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_INT;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_INT;
        case DAL_TYPE_STRING:        return DAL_TYPE_STRING;
        case DAL_TYPE_BOOL:          return DAL_TYPE_INT;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_INT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_INT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_BIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UBIGINT;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_UINT:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_BIGINT;
        case DAL_TYPE_UINT:          return DAL_TYPE_UINT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UINT;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UINT;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UINT;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_BIGINT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UINT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UBIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UBIGINT;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_CHAR:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_INT;
        case DAL_TYPE_UINT:          return DAL_TYPE_INT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_CHAR;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_INT;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_CHAR;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_SMALLINT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_INT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_BIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UBIGINT;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_UCHAR:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_INT;
        case DAL_TYPE_UINT:          return DAL_TYPE_UINT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_INT;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UCHAR;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_BOOL;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_INT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_INT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_BIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UBIGINT;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_STRING:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_STRING;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_BOOL:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_INT;
        case DAL_TYPE_UINT:          return DAL_TYPE_UINT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_CHAR;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UCHAR;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_BOOL;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_SMALLINT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_USMALLINT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_BIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UBIGINT;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_SMALLINT:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_INT;
        case DAL_TYPE_UINT:          return DAL_TYPE_BIGINT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_CHAR;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_SMALLINT;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_SMALLINT;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_SMALLINT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_INT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_BIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_USMALLINT:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_INT;
        case DAL_TYPE_UINT:          return DAL_TYPE_UINT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_INT;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_USMALLINT;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_INT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_USMALLINT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_BIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UBIGINT;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_BIGINT:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_BIGINT;
        case DAL_TYPE_UINT:          return DAL_TYPE_UBIGINT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_BIGINT;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_BIGINT;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_BIGINT;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_BIGINT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_BIGINT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_BIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_UBIGINT:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UBIGINT;
        case DAL_TYPE_UINT:          return DAL_TYPE_UBIGINT;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UBIGINT;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UBIGINT;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UBIGINT;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UBIGINT;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UBIGINT;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UBIGINT;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UBIGINT;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_BLOB:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_BLOB;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_MEMO:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_MEMO;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_NUMERIC:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_NUMERIC;
        case DAL_TYPE_UINT:          return DAL_TYPE_NUMERIC;
        case DAL_TYPE_CHAR:          return DAL_TYPE_NUMERIC;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_NUMERIC;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_NUMERIC;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_NUMERIC;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_NUMERIC;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_NUMERIC;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_DECIMAL:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_DECIMAL;
        case DAL_TYPE_UINT:          return DAL_TYPE_DECIMAL;
        case DAL_TYPE_CHAR:          return DAL_TYPE_DECIMAL;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_DECIMAL;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_DECIMAL;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_DECIMAL;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_DECIMAL;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_DECIMAL;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_DECIMAL;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DECIMAL;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_VARBINARY:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_VARBINARY;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_FLOAT:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_FLOAT;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_DOUBLE:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_NUMERIC;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_DECIMAL;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_DOUBLE;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_DATE:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_DATE;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_TIMESTAMP;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_TIME:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_TIME;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_TIMESTAMP;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_TIMESTAMP:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_TIMESTAMP;
        case DAL_TYPE_TIME:          return DAL_TYPE_TIMESTAMP;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_TIMESTAMP;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    case DAL_TYPE_INTERVAL:
        switch(op1.datatype())
        {
        case DAL_TYPE_CUSTOM:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UNKNOWN:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INT:           return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UINT:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_CHAR:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UCHAR:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_STRING:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BOOL:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_SMALLINT:      return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_USMALLINT:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BIGINT:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_UBIGINT:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_BLOB:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_MEMO:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_NUMERIC:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DECIMAL:       return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_VARBINARY:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_FLOAT:         return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DOUBLE:        return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_DATE:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIME:          return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_TIMESTAMP:     return DAL_TYPE_UNKNOWN;
        case DAL_TYPE_INTERVAL:      return DAL_TYPE_UNKNOWN;
        };
    };

    throw std::runtime_error("Binary operator not implemented");
}


Variant apply_binary_method(const Variant &op0, const Variant &op1, BinaryOperatorVariant::memfun_type method)
{
    if(op0.isnull() || op1.isnull())
        throw NullException();

    daltype_t type = resolve_higher(op0, op1);

    switch(type)
    {
    case DAL_TYPE_CUSTOM:        return (binary_op_impl<int                 >().*method)(op0, op1);
    case DAL_TYPE_UNKNOWN:       return (binary_op_impl<int                 >().*method)(op0, op1);
    case DAL_TYPE_INT:           return (binary_op_impl<signed int          >().*method)(op0, op1);
    case DAL_TYPE_UINT:          return (binary_op_impl<unsigned int        >().*method)(op0, op1);
    case DAL_TYPE_CHAR:          return (binary_op_impl<signed char         >().*method)(op0, op1);
    case DAL_TYPE_UCHAR:         return (binary_op_impl<unsigned char       >().*method)(op0, op1);
    case DAL_TYPE_STRING:        return (binary_op_impl<String              >().*method)(op0, op1);
    case DAL_TYPE_BOOL:          return (binary_op_impl<bool                >().*method)(op0, op1);
    case DAL_TYPE_SMALLINT:      return (binary_op_impl<signed short        >().*method)(op0, op1);
    case DAL_TYPE_USMALLINT:     return (binary_op_impl<unsigned short      >().*method)(op0, op1);
    case DAL_TYPE_BIGINT:        return (binary_op_impl<signed long long    >().*method)(op0, op1);
    case DAL_TYPE_UBIGINT:       return (binary_op_impl<unsigned long long  >().*method)(op0, op1);
    case DAL_TYPE_BLOB:          return (binary_op_impl<Blob                >().*method)(op0, op1);
    case DAL_TYPE_MEMO:          return (binary_op_impl<Memo                >().*method)(op0, op1);
    case DAL_TYPE_NUMERIC:       return (binary_op_impl<TNumeric            >().*method)(op0, op1);
        //case DAL_TYPE_DECIMAL:       return (binary_op_impl<TDecimal>().*method)(op0, op1);
    case DAL_TYPE_VARBINARY:     return (binary_op_impl<TVarbinary          >().*method)(op0, op1);
    case DAL_TYPE_FLOAT:         return (binary_op_impl<float               >().*method)(op0, op1);
    case DAL_TYPE_DOUBLE:        return (binary_op_impl<double              >().*method)(op0, op1);
    case DAL_TYPE_DATE:          return (binary_op_impl<TDate               >().*method)(op0, op1);
    case DAL_TYPE_TIME:          return (binary_op_impl<TTime               >().*method)(op0, op1);
    case DAL_TYPE_TIMESTAMP:     return (binary_op_impl<TTimestamp          >().*method)(op0, op1);
    case DAL_TYPE_INTERVAL:      return (binary_op_impl<TInterval           >().*method)(op0, op1);
    };

    throw std::runtime_error("Binary operator not implemented");
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
