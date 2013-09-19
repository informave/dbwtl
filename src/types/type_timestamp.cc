//
// type_timestamp.cc - Type: TIMESTAMP (definitions)
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
/// @brief Type: TIMESTAMP (definitions)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "dbwtl/db_fwd.hh"
#include "dbwtl/variant.hh"
#include "dbwtl/types.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "../dal/dal_debug.hh"
#include "../utils.hh"

#include <ctime>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <typeinfo>

DB_NAMESPACE_BEGIN



///
String
sv_accessor<TTimestamp>::cast(String*, std::locale loc) const
{
    return this->get_value().str();
}


/// 
TDate
sv_accessor<TTimestamp>::cast(TDate*, std::locale loc) const
{
    return TDate(this->get_value());
}



/// 
TTime
sv_accessor<TTimestamp>::cast(TTime*, std::locale loc) const
{
    return TTime(this->get_value());
}





///
TTimestamp::TTimestamp(void)
    : m_year(0), m_month(0), m_day(0),
      m_hour(0), m_minute(0), m_second(0),
      m_fraction(0)
{}



///
TTimestamp::TTimestamp(short int year, short int month, short int day,
                       short int hour, short int minute, short int second, int fraction)
    : m_year(year), m_month(month), m_day(day),
      m_hour(hour), m_minute(minute), m_second(second),
      m_fraction(fraction)
{}



///
TTimestamp::TTimestamp(const TDate &date)
    : m_year(date.year()), m_month(date.month()), m_day(date.day()),
      m_hour(0), m_minute(0), m_second(0),
      m_fraction(0)
{}



///
TTimestamp::TTimestamp(const TTime &time)
    : m_year(0), m_month(0), m_day(0),
      m_hour(time.hour()), m_minute(time.minute()), m_second(time.second()),
      m_fraction(0)
{}



///
TTimestamp::TTimestamp(signed long long ts)
    : m_year(0), m_month(0), m_day(0),
      m_hour(0), m_minute(0), m_second(0),
      m_fraction(0)
{}


/// 
TTimestamp::TTimestamp(const std::tm &time)
    : m_year(1900 + time.tm_year), m_month(1 + time.tm_mon), m_day(time.tm_mday),
      m_hour(time.tm_hour), m_minute(time.tm_min), m_second(time.tm_sec),
      m_fraction(0)
{}


///
static inline int toInt(std::string::const_iterator beg, std::string::const_iterator end)
{
    return Variant(String(std::string(beg, end))).asInt();
}



///
TTimestamp::TTimestamp(const String &str)
    : m_year(0), m_month(0), m_day(0),
      m_hour(0), m_minute(0), m_second(0),
      m_fraction(0)

{
    if(! TTimestamp::isTimestamp(str))
        throw ConvertException(format("Invalid timestamp format: %s") % str);

    const std::string s = str;
    std::string v;
    std::string::const_iterator i = s.begin();

    m_year = toInt(i, i+4);
    i += 4+1;
    m_month = toInt(i, i+2);
    i += 2+1;
    m_day = toInt(i, i+2);
    i += 2+1;
    m_hour = toInt(i, i+2);
    i += 2+1;
    m_minute = toInt(i, i+2);
    i += 2+1;
    m_second = toInt(i, i+2);
    i += 2+1;
    if(i < s.end())
    {
        std::string::const_iterator j = i;
        while(j != s.end() && std::isdigit(*j, std::locale())) ++j;
        m_fraction = toInt(i, j);
        i = j;
    }
    if(i != s.end())
    {
        /// @todo Implement timezone specification
    }
}



static const char _fmt1[] = "YYYY-MM-DDTHH:MM:SS";
static const char _fmt2[] = "YYYY-MM-DDTHH:MM:SS.SSS+00:00";

#define FMT_LEN(var) ((sizeof(var) / sizeof(var[0]))-1)



///
bool
TTimestamp::isTimestamp(const String &str)
{
    std::string s = str;

    if((s.length() < FMT_LEN(_fmt1)) || s.length() > FMT_LEN(_fmt2)) return false;

    if(s[4] == '-' && s[7] == '-' && (s[10] == 'T' || s[10] == ' ') && s[13] == ':'
       && s[16] == ':')
        return true;
    else
        return false;
}


///
short int     
TTimestamp::year(void) const
{
    return this->m_year;
}


///
short int     
TTimestamp::month(void) const
{
    return this->m_month;
}


///
short int     
TTimestamp::day(void) const
{
    return this->m_day;
}


///
short int     
TTimestamp::hour(void) const
{
    return this->m_hour;
}


///
short int
TTimestamp::minute(void) const
{
    return this->m_minute;
}


///
short int
TTimestamp::second(void) const
{
    return this->m_second;
}


/// 
int
TTimestamp::fraction(void) const
{
    return this->m_fraction;
}



/// 
bool
TTimestamp::operator==(const TTimestamp &ts) const
{
        return this->year() == ts.year() &&
		this->month() == ts.month() &&
		this->day() == ts.day() &&
		this->hour() == ts.hour() &&
                this->minute() == ts.minute() &&
                this->second() == ts.second() &&
		this->fraction() == ts.fraction();
}



/// 
bool
TTimestamp::operator!=(const TTimestamp &ts) const
{
        return ! this->operator==(ts);
}



/// 
bool
TTimestamp::operator<(const TTimestamp &ts) const
{
    if(this->year() == ts.year())
    {
        if(this->month() == ts.month())
        {
            if(this->day() == ts.day())
            {
                if(this->hour() == ts.hour())
                {
                    if(this->minute() == ts.minute())
                        return this->second() < ts.second();
                    else
                        return this->minute() < ts.minute();
                }
                else
                    return this->hour() < ts.hour();
            }
            else
                return this->day() < ts.day();
        }
        else
            return this->month() < ts.month();
    }
    else
        return this->year() < ts.year();
}


/// 
bool
TTimestamp::operator>(const TTimestamp &ts) const
{
    return (!this->operator<(ts)) && (!this->operator==(ts));
}


/// 
String
TTimestamp::str(void) const
{
    return (format("%04hd-%02hd-%02hd %02hd:%02hd:%02hd.%03ld", std::locale("C")) 
    	 % year() % month() % day() % hour() % minute() % second() % fraction());
}

TTimestamp::operator ::tm(void) const
{
    ::tm data;
    data.tm_year = m_year - 1900;
    data.tm_mon = m_month - 1;
    data.tm_mday = m_day;
    data.tm_hour = m_hour;
    data.tm_min = m_minute;
    data.tm_sec = m_second;
    return data;
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
