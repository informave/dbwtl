//
// generic_engine.cc - Generic Engine (definition)
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
/// @brief Generic Engine (definition)
/// @author Daniel Vogelbacher
/// @since 0.0.1


#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/dal/engines/generic_engine.hh"
#include "dbwtl/dal/active_engines.hh"
#include "../dal_debug.hh"
#include "../../utils.hh"

#include <sstream>



DAL_NAMESPACE_BEGIN


std::map<std::string, String::Internal> parse_driver(const String::Internal &str)
{
    std::map<std::string, String::Internal> data;
    std::wstring::size_type i = 0, j = 0;

    j = str.find(US(":"), i);
    if(j != String::Internal::npos)
    {
        data["engine"] = str.substr(i, j++);
        i = j;
    }

    j = str.find(US(":"), i);
    if(j != String::Internal::npos)
    {
        data["driver"] = str.substr(i, j++ - i);
        i = j;
        data["lib"] = str.substr(i);
    }
    else
    {
        data["driver"] = str.substr(i);
    }

    return data;
}




generic::ENV*
generic::createEnv(String driver)
{
    String engine = parse_driver(driver)["engine"];

    if(! engine.length())
        goto err;

#if defined(DBWTL_WITH_SQLITE)
    else if(engine.compare(String("sqlite")) == 0)
        return sqlite::createEnv(driver);
#endif

#if defined(DBWTL_WITH_FIREBIRD)
    else if(engine.compare(String("firebird")) == 0)
        return firebird::createEnv(driver);
#endif

    else if(engine.compare(String("sdi")) == 0)
        return sdi::createEnv(driver);

/*
    else if(engine.compare(L"postgres") == 0)
        return Postgres::createEnv(driver);
*/

err:
    throw ex::engine_error(format("Env: Unknown driver: %s") % driver);
}


DAL_NAMESPACE_END
