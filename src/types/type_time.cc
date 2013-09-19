//
// type_time.cc - Type: TIME (definitions)
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
/// @brief Type: TIME (definitions)
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
sv_accessor<TTime>::cast(String*, std::locale loc) const
{
    return this->get_value().str();
}






/// 
TTimestamp
sv_accessor<TTime>::cast(TTimestamp*, std::locale loc) const
{
    return TTimestamp(this->get_value());
}




/// 
TTime::TTime(void)
    : m_hour(0),
      m_minute(0),
      m_second(0)
{}


/// 
TTime::TTime(const TTime &time)
    : m_hour(time.m_hour),
      m_minute(time.m_minute),
      m_second(time.m_second)
{}



/// 
TTime::TTime(short int hour, short int minute, short int second)
    : m_hour(hour),
      m_minute(minute),
      m_second(second)
{}



/// 
TTime::TTime(const TTimestamp &ts)
	: m_hour(ts.hour()),
		m_minute(ts.minute()),
		m_second(ts.second())
		{}




/// 
TTime::TTime(const String &str)
    : m_hour(0),
      m_minute(0),
      m_second(0)
{
    if(TTimestamp::isTimestamp(str))
    {
        TTimestamp ts(str);
        m_hour = ts.hour();
        m_minute = ts.minute();
        m_second = ts.second();
        return;
    }

    std::vector<std::string> parts = utils::split<std::string>(str, ':');
    if(! utils::between(parts.size(), size_t(2), size_t(3)))
        throw ConvertException(format("Invalid time format: %s") % str);

    //m_year = utils::lexical_cast<int>(parts[0]);
    m_hour = Variant(String(parts[0])).asInt();
    m_minute = Variant(String(parts[1])).asInt();
    if(parts.size() == 3)
        m_second = Variant(String(parts[2].substr(0, 2))).asInt();

    if(! (utils::between(m_hour, 0, 23) && utils::between(m_minute, 0, 59) && utils::between(m_second, 0, 61)))
        throw ConvertException(format("Invalid time format: %s") % str);

}


/// 
TTime::TTime(const std::tm &time)
    : m_hour(time.tm_hour),
      m_minute(time.tm_min),
      m_second(time.tm_sec)
{}



/// 
String
TTime::str(void) const
{
    return (format("%02hd:%02hd:%02hd", std::locale("C")) % hour() % minute() % second());
}



/// 
short int 
TTime::hour(void) const
{
    return this->m_hour;
}


/// 
short int 
TTime::minute(void) const
{
    return this->m_minute;
}


/// 
short int
TTime::second(void) const
{
    return this->m_second;
}



/// 
bool
TTime::operator==(const TTime &time) const
{
        return this->hour() == time.hour() &&
                this->minute() == time.minute() &&
                this->second() == time.second();
}


/// 
bool
TTime::operator!=(const TTime &time) const
{
	return ! this->operator==(time);
}



/// 
bool
TTime::operator<(const TTime &time) const
{
    if(this->hour() == time.hour())
    {
        if(this->minute() == time.minute())
            return this->second() < time.second();
        else
            return this->minute() < time.minute();
    }
    else
        return this->hour() < time.hour();
}


/// 
bool
TTime::operator>(const TTime &time) const
{
    return (!this->operator<(time)) && (!this->operator==(time));
}

TTime::operator ::tm(void) const
{
    ::tm data;
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

