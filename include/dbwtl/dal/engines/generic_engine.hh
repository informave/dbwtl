//
// generic_engine.hh - Generic Engine
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
/// @brief Generic Engine
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_GENERIC_ENGINE_HH
#define INFORMAVE_DB_DAL_GENERIC_ENGINE_HH

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_objects.hh"
#include "dbwtl/util/smartptr.hh"


DAL_NAMESPACE_BEGIN

// forward
struct generic;

/// parse a driver string
std::map<std::string, i18n::UString> parse_driver(const i18n::UString &str);


//------------------------------------------------------------------------------
/// @brief Main generic interface class
struct generic
{
	typedef IDbc              DBC;
	typedef IResult           RESULT;
	typedef IStmt             STMT;
	typedef IEnv              ENV;
	typedef IVariant          VALUE;
	typedef BaseEngineState   STATE;
	typedef ITable            TABLE;
	//typedef SqliteTypeInfo     TYPEINFO;

	static inline const STATE& engine_state(dalstate_t& state)
	{
        const IEngineState* es = state.getImpl();
        return dynamic_cast<const STATE&>(*es);
	}

    ///
    /// @brief Create a new Environment
    ///
    /// Current supported drivers are:
    ///  - libsqlite
    ///
    DBWTL_EXPORT static ENV* createEnv(i18n::UString driver); 
};


DAL_NAMESPACE_END







DB_NAMESPACE_BEGIN


template<typename tag>
struct db_traits<dal::generic, tag>
{
    typedef Environment<dal::generic, tag>      environment_type;
    typedef Connection<dal::generic, tag>       connection_type;
    typedef Statement<dal::generic, tag>        statement_type;
    typedef Result<dal::generic, tag>           resultset_type;
    typedef CachedResult<dal::generic, tag>     cached_resultset_type;
    typedef dal::generic::VALUE                 value_type;

    typedef dal::generic::RESULT                dal_resultset_type;
    typedef dal::generic::STMT                  dal_stmt_type;
    typedef dal::generic::ENV                   dal_env_type;
    typedef dal::generic::DBC                   dal_dbc_type;

    enum { DB_SYSTEM_ID = 0 };

};


DB_NAMESPACE_END


#endif
