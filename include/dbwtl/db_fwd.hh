//
// db_fwd.hh - DB forward declarations
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
/// @brief DB forward declarations
/// @author Daniel Vogelbacher
/// @since 0.0.1

#ifndef INFORMAVE_DB_DBFWD_HH
#define INFORMAVE_DB_DBFWD_HH

#include "dbwtl/dbwtl_config.hh"

#include "dbwtl/util/bcd.hh"

#define DB_NAMESPACE_BEGIN namespace informave { namespace db {
#define DB_NAMESPACE_END }}


DB_NAMESPACE_BEGIN

class RecordSet;

//class NotFoundException;

class Variant;

class BlobStream;
class MemoStream;

class Blob;
class Memo;

class TType;
class TDate;
class TTime;
class TInterval;
class TVarbinary;
//class TNumeric;
typedef informave::utils::bcd TNumeric;
//typedef informave::utils::bcd TDecimal;

class TTimestamp;
class TCustomType;
//class TDatetime;

class DatasetFilter;
class MetadataFilter;
class NoFilter;

class IHandle;
class IDiagnosticRec;


//--------------------------------------------------------------------------
/// All types defined by the DAL
/// 
/// @since 0.0.1
/// @brief DAL datatypes
enum DatatypeEnumeration
{
    DAL_TYPE_CUSTOM = 0,
    DAL_TYPE_UNKNOWN,
    DAL_TYPE_INT,
    DAL_TYPE_UINT,
    DAL_TYPE_CHAR,
    DAL_TYPE_UCHAR,
    DAL_TYPE_STRING,
    DAL_TYPE_BOOL,
    DAL_TYPE_SMALLINT,
    DAL_TYPE_USMALLINT,
    DAL_TYPE_BIGINT,
    DAL_TYPE_UBIGINT,
    DAL_TYPE_BLOB,
    DAL_TYPE_BLOBSTREAM,
    DAL_TYPE_MEMO,
    DAL_TYPE_MEMOSTREAM,
    DAL_TYPE_NUMERIC,
//    DAL_TYPE_DECIMAL,
    DAL_TYPE_VARBINARY,
    DAL_TYPE_FLOAT,
    DAL_TYPE_DOUBLE,
    DAL_TYPE_DATE,
    DAL_TYPE_TIME,
    DAL_TYPE_TIMESTAMP,
    DAL_TYPE_INTERVAL
    //DAL_TYPE_BIT,g
    //DAL_TYPE_VARBIT,
};

typedef enum DatatypeEnumeration daltype_t;




DB_NAMESPACE_END

#endif
