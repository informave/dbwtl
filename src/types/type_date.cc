//
// type_date.cc - Type: DATE (definitions)
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
/// @brief Type: DATE (definitions)
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
read_accessor<TDate>::asStr(std::locale loc) const
{
    return this->getValue().str();
}


/// 
TDate
read_accessor<TDate>::asDate(void) const
{
    return this->getValue();
}



/// 
TTime
read_accessor<TDate>::asTime(void) const
{
    return TTime(this->getValue());
}



/// 
TTimestamp
read_accessor<TDate>::asTimestamp(void) const
{
    return TTimestamp(this->getValue());
}





/// 
TDate::TDate(void)
    : m_year(1970),
      m_month(1),
      m_day(1)
{}


/// 
TDate::TDate(const TDate &date)
    : m_year(date.m_year),
      m_month(date.m_month),
      m_day(date.m_day)
{}



/// 
TDate::TDate(const TTimestamp &ts)
    : m_year(ts.year()),
      m_month(ts.month()),
      m_day(ts.day())
{}



/// 
TDate::TDate(const String &str)
    : m_year(0),
      m_month(0),
      m_day(0)
{
    if(str == "now")
    {
        std::time_t t = std::time(0);
        std::tm *m = std::localtime(&t);
        this->m_year = 1900 + m->tm_year;
        this->m_month = m->tm_mon + 1;
        this->m_day = m->tm_mday;
        return;
    }

    if(TTimestamp::isTimestamp(str))
    {
        TTimestamp ts(str);
        m_year = ts.year();
        m_month = ts.month();
        m_day = ts.day();
        return;
    }

    /// @bug use try catch(conversion_error) to handle invalid numbers

    std::vector<std::string> parts = utils::split<std::string>(str, '-');
    if(parts.size() != 3)
        throw ex::convert_error(format("Invalid date format: %s") % str);

    //m_year = utils::lexical_cast<int>(parts[0]);
    m_year = Variant(String(parts[0])).asInt();
    m_month = Variant(String(parts[1])).asInt();
    m_day = Variant(String(parts[2])).asInt();

    if(m_month < 1 || m_month > 12 || m_day < 1 || m_day > 31)
        throw ex::convert_error(format("Invalid date format: %s") % str);
}



/// 
TDate::TDate(short int year, short int month, short int day)
    : m_year(year),
      m_month(month),
      m_day(day)
{}


/// 
TDate::TDate(const std::tm &time)
    : m_year(1900 + time.tm_year),
      m_month(1 + time.tm_mon),
      m_day(time.tm_mday)
{}



/// 
short int 
TDate::year(void) const
{
    return this->m_year;
}


/// 
short int 
TDate::month(void) const
{
    return this->m_month;
}


/// 
short int 
TDate::day(void) const
{
    return this->m_day;
}


/// 
String
TDate::str(void) const
{
    return (format("%04hd-%02hd-%02hd", std::locale("C")) % year() % month() % day());
}



/// 
bool
TDate::operator==(const TDate &date) const
{

    std::cout << this->year() << this->month() << this->day() << std::endl;
    std::cout << date.year() << date.month() << date.day() << std::endl;

    return this->year() == date.year() &&
        this->month() == date.month() &&
        this->day() == date.day();
}


/// 
bool
TDate::operator!=(const TDate &date) const
{
    return this->year() != date.year() ||
        this->month() != date.month() ||
        this->day() != date.day();

}



/// 
bool
TDate::operator<(const TDate &date) const
{
    if(this->year() == date.year())
    {
        if(this->month() == date.month())
            return this->day() < date.day();
        else
            return this->month() < date.month();
    }
    else
        return this->year() < date.year();
}



/// 
bool
TDate::operator>(const TDate &date) const
{
    return (!this->operator<(date)) && (!this->operator==(date));
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

