//
// bcd.hh - Numeric class using BCD format
//
// Copyright (C)         informave.org
//   2011,               Daniel Vogelbacher <daniel@vogelbacher.name>
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
/// @brief Numeric class using BCD format
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_UTILS_BCD_HH
#define INFORMAVE_UTILS_BCD_HH

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <iterator>
#include <deque>
#include <cassert>
#include <sstream>
#include <cstdint>
#include <locale>
//#include <cstdlib>



#define INFORMAVE_BCD_NS_BEGIN namespace informave { namespace utils {
#define INFORMAVE_BCD_NS_END }}


INFORMAVE_BCD_NS_BEGIN

// forward declaration
template<typename StorageContainer> class basic_bcd;

INFORMAVE_BCD_NS_END

namespace std
{
    template<typename T>
    informave::utils::basic_bcd<T> abs(const informave::utils::basic_bcd<T> &v);
}


INFORMAVE_BCD_NS_BEGIN


//..............................................................................
////////////////////////////////////////////////////////////////////// basic_bcd
///
/// @since 0.0.1
/// @brief basic_bcd class
template<typename StorageContainer>
class basic_bcd
{
public:
    typedef basic_bcd<StorageContainer>          bcd_type;
    typedef StorageContainer                     storage_type;
    typedef typename storage_type::value_type    value_type;


    template<typename T, typename U> friend basic_bcd<T> operator+(const U &a, const basic_bcd<T> &b);
    template<typename T, typename U> friend basic_bcd<T> operator-(const U &a, const basic_bcd<T> &b);
    template<typename T, typename U> friend basic_bcd<T> operator*(const U &a, const basic_bcd<T> &b);
    template<typename T, typename U> friend basic_bcd<T> operator/(const U &a, const basic_bcd<T> &b);

    template<typename T>             friend basic_bcd<T> std::abs(const basic_bcd<T> &v);


    static bcd_type tolerance;
    static bcd_type inverse_mfactor;




    signed long long asLongLong(void) const
    {
        std::stringstream ss;
        ss << str();
        signed long long data;
        ss >> data;
        return data;
    }

    /// @brief Empty constructor
    basic_bcd(void)
        : m_nibbles(),
          m_scale(0),
          m_sign(true)
    {}


    /// @brief Copy constructor
    basic_bcd(const basic_bcd &orig)
        : m_nibbles(orig.m_nibbles),
          m_scale(orig.m_scale),
          m_sign(orig.m_sign)
    {}

    /// @brief Construct from long long
    basic_bcd(long long v)
        : m_nibbles(),
          m_scale(0),
          m_sign(true)
    {
        set_value(v);
    }


    static basic_bcd from_float(float v)
    {
        basic_bcd bcd;
        bcd.set_value(v);
	return bcd;
    }

    static basic_bcd from_double(double v)
    {
        basic_bcd bcd;
        bcd.set_value(v);
        return bcd;
    }


    /// @brief Construct from long long and specific scale
    basic_bcd(long long num, unsigned short scale)
       : m_nibbles(),
         m_scale(0),
	 m_sign(true)
    {
        set_value(num);
        m_scale = scale;
        if(this->precision() == this->scale())
            this->m_nibbles.push_front(0);
        assert(this->precision() > this->scale());
    }

    /// @brief Construct from string
    basic_bcd(const std::string &v, const std::locale &loc = std::locale())
        : m_nibbles(),
          m_scale(0),
          m_sign(true)
    {
        set_strvalue(v, loc);
    }

    /// @brief Returns the scale
    size_t scale(void) const
    {
        return this->m_scale;
    }

    /// @brief Returns the precision
    size_t precision(void) const
    {
        return this->m_nibbles.size();
    }


    /// @brief Return raw nibbles
    storage_type& nibbles(void)
    {
        return this->m_nibbles;
    }

    /// @brief Return raw nibbles
    const storage_type& nibbles(void) const
    {
        return this->m_nibbles;
    }

    /// @brief Return sign
    bool sign(void) const
    {
        return this->m_sign;
    }

    template<typename T>
    T convert(void) const
    {
 	std::stringstream ss;
	ss << str();
	T x;
	ss >> x;
	return x;
    }


    /// @brief Set value as long
    void set_value(long long v)
    {
        m_nibbles.clear();
        m_sign = v >= 0;
        m_scale = 0;
        v = std::abs(v);
        std::front_insert_iterator<StorageContainer> d = std::front_inserter(m_nibbles);

        do
        {
            *d++ = v % 10;
            v /= 10;
        }
        while(v);
    }

    void set_value(float v)
    {
        std::stringstream ss;
        std::locale loc("C");
        ss.imbue(loc);
        ss << std::fixed << v;
        set_strvalue(ss.str(), loc);
    }


    void set_value(double v)
    {
        std::stringstream ss;
        std::locale loc("C");
        ss.imbue(loc);
        ss << std::fixed << v;
        set_strvalue(ss.str(), loc);
    }


    /// @brief Set value as string
    void set_strvalue(std::string s, const std::locale &loc)
    {
    	std::string orig(s);
        m_nibbles.clear();
        m_sign = true;
        m_scale = 0;

        if(s.size() == 0) return;

        std::back_insert_iterator<StorageContainer> d = std::back_inserter(m_nibbles);

        size_t dot = 0;

        if(s[0] == '+' || s[0] == '-')
        {
            m_sign = s[0] == '+' ? true : false;
            s.erase(s.begin());
        }
        
        if(!s.size()) throw std::invalid_argument(std::string("Invalid numeric value: ").append(orig));

	const char radix_separator = std::use_facet<std::numpunct<char> >(loc).decimal_point();
	const char thousands_separator = std::use_facet<std::numpunct<char> >(loc).thousands_sep();
	size_t th_n = 0;

        for(size_t i = 0; i < s.size(); ++i)
        {
            if(dot == 0 && s[i] == radix_separator)
            {
                dot = i;
                continue;
            }
			if(s[i] == thousands_separator)
			{
				++th_n;
				continue;
			}
            if(s[i] < 0x30 || s[i] > 0x39)
                throw std::invalid_argument(std::string("Invalid numeric value: ").append(orig));
            *d++ = s[i] - 0x30;
        }
        if(dot)
            m_scale = m_nibbles.size() - (dot-th_n);
        else
            m_scale = 0;

        this->normalize();
    }


    /// @brief Convert number to string
    std::string str(const std::locale &loc = std::locale("")) const
    {
        std::string s;
        std::transform(m_nibbles.begin(), m_nibbles.end(),
                       std::back_inserter(s), int2char);
        if(m_scale)
        {
			std::numpunct<std::string::value_type> const &n = std::use_facet<std::numpunct<std::string::value_type> >(loc);

            return std::string(this->sign() ? "" : "-") + s.substr(0, s.size() - scale())
				+ n.decimal_point() + s.substr((s.size() - scale()));
        }
        else
            return std::string(this->sign() ? "" : "-") + s;
    }



    void assign(const basic_bcd &v)
    {
        this->m_scale = v.m_scale;
        this->m_nibbles = v.m_nibbles;
        this->m_sign = v.m_sign;
    }


    bool operator<(const basic_bcd &b) const
    {
        if(this->sign() != b.sign())
        {
            if(b.sign()) return true;
            else return false;
        }
    
        bcd_type valA, valB;
        scale_zero(*this, b, valA, valB);

        if(valA.precision() < valB.precision()) return this->sign();
        else if(valA.precision() > valB.precision()) return ! this->sign();
        else // both equal
        {
            typename storage_type::const_iterator a(valA.nibbles().begin());
            typename storage_type::const_iterator b(valB.nibbles().begin());
            for(; a != valA.nibbles().end(); ++a, ++b)
            {
                if(*a == *b) continue;
                else return this->sign() ? *a < *b : *a > *b;
            }
            return false; // eqal
        }
    }

    bool operator>(const basic_bcd &b) const
    {
        if(this->sign() != b.sign())
        {
            if(this->sign()) return true;
            else return false;
        }
        bcd_type valA, valB;
        scale_zero(*this, b, valA, valB);

        if(valA.precision() > valB.precision()) return this->sign();
        else if(valA.precision() < valB.precision()) return ! this->sign();
        else // both equal
        {
            typename storage_type::const_iterator a(valA.nibbles().begin());
            typename storage_type::const_iterator b(valB.nibbles().begin());
            for(; a != valA.nibbles().end(); ++a, ++b)
            {
                if(*a == *b) continue;
                else return this->sign() ? *a > *b : *a < *b;
            }
            return false; // eqal
        }
    }

    
    bool operator==(const basic_bcd &b) const
    {
        bcd_type valA, valB;
        scale_zero(*this, b, valA, valB);

        if(valA.precision() == valB.precision() && this->sign() == b.sign())
        {
            return std::equal(valB.nibbles().begin(), valB.nibbles().end(), valA.nibbles().begin());
        }
        else
            return false;
    }


    bool operator!=(const basic_bcd &b) const
    {
        return ! this->operator==(b);
    }

    bool operator>=(const basic_bcd &b) const
    {
        return this->operator==(b) || this->operator>(b);
    }

    bool operator<=(const basic_bcd &b) const
    {
        return this->operator==(b) || this->operator<(b);
    }



    basic_bcd& operator+=(const basic_bcd &b)
    {
        if(this->sign() == b.sign())
        {
            this->add(b);
            this->m_sign = b.sign();
        }
        else if(this->sign())
        {
            bool sig = (std::abs(*this) >= std::abs(b));
            this->sub(b);
            this->m_sign = sig;
        }
        else // b.sign()
        {
            bool sig = (std::abs(*this) <= std::abs(b));
            this->sub(b);
            this->m_sign = sig;
        }
        return *this;
    }

    basic_bcd& operator-=(const basic_bcd &b)
    {
        if(this->sign() == b.sign())
        {
            bool sig = (std::abs(*this) >= std::abs(b)) ? this->sign() : false;
            this->sub(b);
            this->m_sign = sig;
        }
       
        else if(this->sign())
        {
            bool sig = (std::abs(*this) >= std::abs(b));
            this->add(b);
            this->m_sign = sig;
        }
        else // b.sign()
        {
            bool sig = (std::abs(*this) <= std::abs(b));
            this->add(b);
            this->m_sign = sig;
        }
        return *this;
    }

    basic_bcd operator+(const basic_bcd &b) const
    {
        basic_bcd result(*this);
        result += b;
        return result;
    }

    basic_bcd operator-(const basic_bcd &b) const
    {
        basic_bcd result(*this);
        result -= b;
        return result;
    }

    basic_bcd operator*(const basic_bcd &b) const
    {
        basic_bcd result(*this);
        result *= b;
        return result;
    }

    bcd_type& operator*=(const bcd_type &b)
    {
        bool sig = this->m_sign == b.sign();
        bcd_type x(std::abs(*this));
        x.mul(std::abs(b));
        x.m_sign = sig;
        this->assign(x);
        return *this;
    }

    basic_bcd operator/(const basic_bcd &b) const
    {
        basic_bcd result(*this);
        result /= b;
        return result;
    }

    bcd_type& operator/=(const bcd_type &b)
    {
        bool sig = this->m_sign == b.sign();
        bcd_type x(std::abs(*this));
        x.div(std::abs(b));
        x.m_sign = sig;
        this->assign(x);
        return *this;
    }

    bcd_type operator%(const bcd_type &b) const
    {
    	if(*this == b) return 0;
	if(b > *this) return *this;

    	//bcd_type a = b;
	bcd_type a = divi(*this, b) - 2;
	a = a * b;
	while(a+b <= *this) a = a+b;
	return (*this)-a;
    }



    /// @brief Addition routine
    /// @details
    void add(const basic_bcd &summand)
    {
        value_type carry = 0;
        bcd_type result, sumA, sumB;
        size_t sc = scale_zero(*this, summand, sumA, sumB);

        std::front_insert_iterator<storage_type> p(std::front_inserter(result.nibbles()));

        typename storage_type::const_reverse_iterator a(sumA.nibbles().rbegin());
        typename storage_type::const_reverse_iterator b(sumB.nibbles().rbegin());
            

        while(a != sumA.nibbles().rend() || b != sumB.nibbles().rend())
        {
            value_type val = carry;
            carry = 0;

            if(a != sumA.nibbles().rend())
                val += *a++;

            if(b != sumB.nibbles().rend())
                val += *b++;

            if(valid_bcd(val))
            {
                *p++ = val;
                carry = 0;
            }
            else
            {
                *p++ = (val + 6) & 0x0F;
                carry = 1;
            }
        }

        if(carry)
            *p++ = 1;

        this->assign(result.shift_right(sc));
        this->normalize();
    }




    /// @brief Substraction routine
    /// @details
    /// Subtraction is done by adding the ten's complement of the subtrahend.
    /// For example:
    /// 778 - 9
    /// a) Calculate the ten's complement of 009 -> 991
    /// b) Add 991 to 778 -> (1)869 (1 = carry)
    /// c) Ignore the carry -> 869
    /// d) If carry = 1 after all digits processed, the result is positive (+)
    /// e) If carry = 0, the result is negative (-). You need to invert the result to get the correct value.
    void sub(const basic_bcd &subtrahend)
    {
        value_type carry = 0;
        bcd_type result, termA, termB;
        size_t sc = scale_zero(*this, subtrahend, termA, termB);

        size_t termB_prec = termB.precision();

        // The complement of termB must have the same length as termA.
        // For example, for 778 - 9, the complement of 9 must be 991 (1000-009)
        if(termA.precision() > termB.precision())
            termB.fill_front(termA.precision(), 0);
        termB = termB.compl10();

        std::front_insert_iterator<storage_type> p(std::front_inserter(result.nibbles()));

        typename storage_type::const_reverse_iterator a(termA.nibbles().rbegin());
        typename storage_type::const_reverse_iterator b(termB.nibbles().rbegin());


        // iterate over all digits
        while(a != termA.nibbles().rend() || b != termB.nibbles().rend())
        {
            value_type val = carry;

            if(a != termA.nibbles().rend())
                val += *a++;

            if(b != termB.nibbles().rend())
                val += *b++;

            if(valid_bcd(val))
            {
                *p++ = val;
                carry = 0;
            }
            else
            {
                *p++ = (val + 6) & 0x0F;
                carry = 1;
            }
        }

        if(carry == 0) // negative result
        {
            result.fill_front(termB_prec, 0);
            result = result.compl10();
            result.m_sign = false;
        }

        this->assign(result.shift_right(sc));
        this->normalize();
    }



    /// @brief Multiplication routine
    /// @details
    void mul(const bcd_type &multiplier)
    {
        bcd_type result(0), factorA, factorB;
        size_t sc = scale_zero(*this, multiplier, factorA, factorB);
        size_t c = 0;

        for(typename storage_type::const_reverse_iterator b(factorB.nibbles().rbegin());
            b != factorB.nibbles().rend(); ++b)
        {
            bcd_type v(0);
            for(int i = 0; i < *b; ++i) v += factorA;
            result += v.shift_left(c++);
        }

        // shift twice because of multiplication
        this->assign(result.shift_right(sc).shift_right(sc));
        this->normalize();
    }




    bcd_type getInverse(void) const
    {
    	bcd_type d = *this;
	int scale = d.m_scale;
	d.m_scale = d.precision();
	d.nibbles().push_front(0);
    	bcd_type x0 = inverse_mfactor - bcd_type(2) * d;
	//bcd_type tolerance("0.0000000000000000000000000001");
	bcd_type diff;
	bcd_type value = x0;
	do
	{
		//x_{n + 1} = x_n * (2 - a * x_n)
		bcd_type prev = value;
		value = value * (bcd_type(2) - d * value);
		value.rescale(50);
		diff = value - prev;
		//std::cout << value.str() << std::endl;
	}
	while(std::abs(diff) >= tolerance);
	value = value.shift_right(this->precision() - this->scale());
	value.normalize();
	value.rescale(tolerance.scale()/2);
	return value;
    }


    /// @brief Division routine
    /// @details
    /// Division is done by Newon-Raphson division method.
    void div(const bcd_type &divisor)
    {
    	if(*this == divisor)
	{
		this->assign(1);
		this->normalize();
		return;
	}
	else
	{
        bcd_type result = *this * divisor.getInverse();
        //result = result.shift_right(1);

        this->assign(result);
        this->normalize();	
	return;
	}
    }



    /// @brief Change sign to +
    inline void make_positive(void)
    {
        this->m_sign = true;
    }

    inline void make_negative(void)
    {
    	this->m_sign = false;
    }

protected:

    /// @brief Shift decimal point
    bcd_type shift_left(size_t c) const
    {
        if(!c)
            return *this;

        bcd_type result(*this);
        std::back_insert_iterator<storage_type> p(std::back_inserter(result.nibbles()));
        if(result.m_scale > 0)
        {
            if(c > result.m_scale)
            {
                c = c - result.m_scale;
                result.m_scale = 0;
                while(c--) *p++ = 0;
            }
            else
                result.m_scale -= c;
        }
        else
            while(c--) *p++ = 0;            
        
        return result;
    }


    /// @brief Shift decimal point
    bcd_type shift_right(size_t c) const
    {
        if(!c)
            return *this;
        bcd_type result(*this);
        assert(result.precision() > result.scale());
        while(c)
        {
            result.m_scale++;
            if(result.scale() >= result.precision())
                result.nibbles().push_front(0);
            c--;
        }
        assert(result.precision() > result.scale());
        return result;
    }


    /// @brief 
    inline bcd_type compl10(void) const
    {
    	bcd_type result;
        std::transform(this->nibbles().begin(), this->nibbles().end(),
                       std::back_inserter(result.nibbles()), _compl9);
        result = result + bcd_type(1);
        return result;
    }



    /// @brief 
    inline void fill_front(size_t c, value_type v)
    {
        while(this->precision() < c) this->nibbles().push_front(v);
    }


public:
    void zeroscale(void)
    {
    	this->assign(this->shift_left(this->scale()));
    }

    /// @brief 
    void rescale(size_t sc)
    {
        typename storage_type::reverse_iterator y = this->nibbles().rbegin();
        value_type v = 0;
            
        while(this->scale() > sc && y != this->nibbles().rend())
        {
            m_scale--;

            v = *y;

            this->nibbles().erase( -- (y.base()) );
            y++;
        }

        if(/* this->scale() >= 0 && */ v >= 5) // round
        {
            bcd_type a = bcd_type(1).shift_right(this->scale());
            this->add(a);
        }
	if(this->scale() < sc && this->precision() >= sc)
	{
		//std::cout << this->precision() << " / " << sc << std::endl;
		this->m_scale = sc;
	}

        assert(! this->nibbles().empty());
        assert(this->precision() != 0);
        assert(this->precision() >= this->scale());   
    }


protected:
    /// @brief 
    void normalize(void)
    {
        assert(this->precision() > this->scale());
        assert(this->precision() > 0);

        typename storage_type::iterator p = this->nibbles().begin();
        typename storage_type::iterator endp = this->nibbles().begin();
            
        if(this->scale() == 0)
            endp = this->nibbles().end() - 1;
        else
            std::advance(endp, (this->precision() - this->scale()) -1);

        while(p != endp && *p == 0) ++p;

        if(p != nibbles().begin())
            nibbles().erase(nibbles().begin(), p);

        typename storage_type::reverse_iterator q = this->nibbles().rbegin();

        while(m_scale && q != this->nibbles().rend() && *q == 0)
        {
            m_scale--;
            this->nibbles().erase( -- (q.base()) );
            q++;
        }
        assert(! this->nibbles().empty());
        assert(this->precision() != 0);
        assert(this->precision() > this->scale());
    }



    /// @brief 
    static inline size_t scale_zero(const bcd_type &s1, const bcd_type &s2, bcd_type &d1, bcd_type &d2)
    {
        assert(s1.precision() > s1.scale());
        assert(s2.precision() > s2.scale()); 
        
        size_t sc = (s1.scale() > s2.scale()) ? s1.scale() : s2.scale();
    
        d1 = sc ? s1.shift_left(sc) : s1;
        d2 = sc ? s2.shift_left(sc) : s2;

        return sc;
    }


    /// @brief Convert integer to ASCII char
    static inline std::string::value_type int2char(int v)
    {
        return v + 0x30;
    }
    

    /// @briec Check if digit is a valid BCD number
    static inline bool valid_bcd(value_type v)
    {
        return v < 10;
    }

    /// @brief 
    static inline value_type _compl10(value_type v)
    {
        return (9-v)+1;
    }

    /// @brief 
    static inline value_type _compl9(value_type v)
    {
        return (9-v);
    }


protected:
    StorageContainer  m_nibbles;
    size_t            m_scale;
    bool              m_sign;
};




template<typename T, typename U> 
basic_bcd<T> operator+(const U &a, const basic_bcd<T> &b) { return basic_bcd<T>(a) + b; }

template<typename T, typename U> 
basic_bcd<T> operator-(const U &a, const basic_bcd<T> &b) { return basic_bcd<T>(a) - b; }

template<typename T, typename U> 
basic_bcd<T> operator*(const U &a, const basic_bcd<T> &b) { return basic_bcd<T>(a) * b; }

template<typename T, typename U> 
basic_bcd<T> operator/(const U &a, const basic_bcd<T> &b) { return basic_bcd<T>(a) / b; }


template<typename T, typename U>
basic_bcd<T> divi(const U &a, const basic_bcd<T> &b) { basic_bcd<T> x = basic_bcd<T>(a) / b; x.rescale(0); return x; }

typedef informave::utils::basic_bcd<std::deque<uint8_t> > bcd;


template<typename StorageContainer>
basic_bcd<StorageContainer> basic_bcd<StorageContainer>::tolerance("0.0000000000000000000000000001", std::locale("C"));

template<typename StorageContainer>
basic_bcd<StorageContainer> basic_bcd<StorageContainer>::inverse_mfactor("2.914", std::locale("C"));


INFORMAVE_BCD_NS_END




template<typename T>
std::ostream& operator<<(std::ostream& o, const informave::utils::basic_bcd<T> &b) { return (o << b.str()); }



namespace std
{
    template<typename T>
    informave::utils::basic_bcd<T> abs(const informave::utils::basic_bcd<T> &v)
    {
        informave::utils::basic_bcd<T> r(v);
        r.make_positive();
        return r;
    }
}



#endif

//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
