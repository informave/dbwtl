//
// odbc_engine.hh - SQLite Engine
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
/// @brief SQLite Engine
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DB_DAL_ODBC_ENGINE_HH
#define INFORMAVE_DB_DAL_ODBC_ENGINE_HH

#include "dbwtl/dal/dal_fwd.hh"
#include "dbwtl/dal/dal_interface.hh"
#include "dbwtl/db_objects.hh"
#include "dbwtl/util/smartptr.hh"

#if !defined(DBWTL_WITH_ODBC)
#error "DBWTL was compiled without SQLite support!"
#endif


DAL_NAMESPACE_BEGIN



class OdbcBlob;
class OdbcMemo;
class OdbcData;
class OdbcResult;
class OdbcStmt;
class OdbcDbc;
class OdbcEnv;
class OdbcVariant;
class OdbcEngineState;
class OdbcTable;
class OdbcDiag;
class OdbcDatatype;
class OdbcColumnDesc;
class OdbcMetadata;

struct odbc;
struct odbc_sqlstates;



//------------------------------------------------------------------------------
///
/// @brief SQLite SQLSTATEs
struct odbc_sqlstates : public basic_states
{
    typedef enum 
    {
        DAL_ODBC_SQLSTATE_07000 = DAL_SQLSTATE_07000, // Dynamic SQL error
        DAL_ODBC_SQLSTATE_07001 = DAL_SQLSTATE_07001, // Wrong number of parameters
        DAL_ODBC_SQLSTATE_07002 = DAL_SQLSTATE_07002, // COUNT field incorrect
        DAL_ODBC_SQLSTATE_07005 = DAL_SQLSTATE_07005, // Prepared statement not a cursor-specification
        DAL_ODBC_SQLSTATE_07006 = DAL_SQLSTATE_07006, // Restricted data type attribute violation
        DAL_ODBC_SQLSTATE_07009 = DAL_SQLSTATE_07009, // Invalid descriptor index
        DAL_ODBC_SQLSTATE_08000 = DAL_SQLSTATE_08000, // connection exception
        DAL_ODBC_SQLSTATE_08001 = DAL_SQLSTATE_08001, // Client unable to establish connection
        DAL_ODBC_SQLSTATE_08002 = DAL_SQLSTATE_08002, // Connection name in use
        DAL_ODBC_SQLSTATE_08003 = DAL_SQLSTATE_08003, // Connection not open
        DAL_ODBC_SQLSTATE_08004 = DAL_SQLSTATE_08004, // Server rejected the connection
        DAL_ODBC_SQLSTATE_08007 = DAL_SQLSTATE_08007, // Connection failure during transaction
        DAL_ODBC_SQLSTATE_21000 = DAL_SQLSTATE_21000, // cardinality violation
        DAL_ODBC_SQLSTATE_22000 = DAL_SQLSTATE_22000, // data exception
        DAL_ODBC_SQLSTATE_22001 = DAL_SQLSTATE_22001, // String data, right-truncated
        DAL_ODBC_SQLSTATE_22002 = DAL_SQLSTATE_22002, // Indicator variable required but not supplied
        DAL_ODBC_SQLSTATE_22003 = DAL_SQLSTATE_22003, // Numeric value out of range
        DAL_ODBC_SQLSTATE_22007 = DAL_SQLSTATE_22007, // Invalid datetime format
        DAL_ODBC_SQLSTATE_22008 = DAL_SQLSTATE_22008, // Datetime field overflow
        DAL_ODBC_SQLSTATE_22012 = DAL_SQLSTATE_22012, // Division by zero
        DAL_ODBC_SQLSTATE_22015 = DAL_SQLSTATE_22015, // Interval field overflow
        DAL_ODBC_SQLSTATE_22018 = DAL_SQLSTATE_22018, // Invalid character value for cast specification
        DAL_ODBC_SQLSTATE_22019 = DAL_SQLSTATE_22019, // Invalid escape character
        DAL_ODBC_SQLSTATE_22025 = DAL_SQLSTATE_22025, // Invalid escape sequence
        DAL_ODBC_SQLSTATE_22026 = DAL_SQLSTATE_22026, // String data, length mismatch
        DAL_ODBC_SQLSTATE_23000 = DAL_SQLSTATE_23000, // Integrity constraint violation
        DAL_ODBC_SQLSTATE_24000 = DAL_SQLSTATE_24000, // Invalid cursor state
        DAL_ODBC_SQLSTATE_25000 = DAL_SQLSTATE_25000, // Invalid transaction state
        DAL_ODBC_SQLSTATE_28000 = DAL_SQLSTATE_28000, // Invalid authorization specification
        DAL_ODBC_SQLSTATE_34000 = DAL_SQLSTATE_34000, // Invalid cursor name
        DAL_ODBC_SQLSTATE_3C000 = DAL_SQLSTATE_3C000, // Duplicate cursor name
        DAL_ODBC_SQLSTATE_3D000 = DAL_SQLSTATE_3D000, // Invalid catalog name
        DAL_ODBC_SQLSTATE_3F000 = DAL_SQLSTATE_3F000, // Invalid schema name
        DAL_ODBC_SQLSTATE_42000 = DAL_SQLSTATE_42000, // Syntax error or access violation
        DAL_ODBC_SQLSTATE_44000 = DAL_SQLSTATE_44000, // WITH CHECK OPTION violation
        DAL_ODBC_SQLSTATE_HY000 = DAL_SQLSTATE_HY000, // General error

        DAL_ODBC_SQLSTATE_07S01 = DAL_SQLSTATE_LAST + 1, // Invalid use of default parameter

        DAL_ODBC_SQLSTATE_08S01, // Communication link failure
        DAL_ODBC_SQLSTATE_21S01, // Insert value list does not match column list
        DAL_ODBC_SQLSTATE_21S02, // Degree of derived table does not match column list
        DAL_ODBC_SQLSTATE_25S01, // Transaction state
        DAL_ODBC_SQLSTATE_25S02, // Transaction is still active
        DAL_ODBC_SQLSTATE_25S03, // Transaction is rolled back
        DAL_ODBC_SQLSTATE_42S01, // Base table or view already exists
        DAL_ODBC_SQLSTATE_42S02, // Base table or view not found
        DAL_ODBC_SQLSTATE_42S11, // Index already exists
        DAL_ODBC_SQLSTATE_42S12, // Index not found
        DAL_ODBC_SQLSTATE_42S21, // Column already exists
        DAL_ODBC_SQLSTATE_42S22, // Column not found
	DAL_ODBC_SQLSTATE_40000, // General transaction error
        DAL_ODBC_SQLSTATE_40001, // Serialization failure
        DAL_ODBC_SQLSTATE_40002, // Integrity constraint violation
        DAL_ODBC_SQLSTATE_40003, // Statement completion unknown
        DAL_ODBC_SQLSTATE_HY001, // Memory allocation error
        DAL_ODBC_SQLSTATE_HY003, // Invalid application buffer type
        DAL_ODBC_SQLSTATE_HY004, // Invalid SQL data type
        DAL_ODBC_SQLSTATE_HY007, // Associated statement is not prepared
        DAL_ODBC_SQLSTATE_HY008, // Operation canceled
        DAL_ODBC_SQLSTATE_HY009, // Invalid use of null pointer
        DAL_ODBC_SQLSTATE_HY010, // Function sequence error
        DAL_ODBC_SQLSTATE_HY011, // Attribute cannot be set now
        DAL_ODBC_SQLSTATE_HY012, // Invalid transaction operation code
        DAL_ODBC_SQLSTATE_HY013, // Memory management error
        DAL_ODBC_SQLSTATE_HY014, // Limit on the number of handles exceeded
        DAL_ODBC_SQLSTATE_HY015, // No cursor name available
        DAL_ODBC_SQLSTATE_HY016, // Cannot modify an implementation row descriptor
        DAL_ODBC_SQLSTATE_HY017, // Invalid use of an automatically allocated descriptor handle
        DAL_ODBC_SQLSTATE_HY018, // Server declined cancel request
        DAL_ODBC_SQLSTATE_HY019, // Non-character and non-binary data sent in pieces
        DAL_ODBC_SQLSTATE_HY020, // Attempt to concatenate a null value
        DAL_ODBC_SQLSTATE_HY021, // Inconsistent descriptor information
        DAL_ODBC_SQLSTATE_HY024, // Invalid attribute value
        DAL_ODBC_SQLSTATE_HY090, // Invalid string or buffer length
        DAL_ODBC_SQLSTATE_HY091, // Invalid descriptor field identifier
        DAL_ODBC_SQLSTATE_HY092, // Invalid attribute/option identifier
        DAL_ODBC_SQLSTATE_HY095, // Function type out of range
        DAL_ODBC_SQLSTATE_HY096, // Invalid information type
        DAL_ODBC_SQLSTATE_HY097, // Column type out of range
        DAL_ODBC_SQLSTATE_HY098, // Scope type out of range
        DAL_ODBC_SQLSTATE_HY099, // Nullable type out of range
        DAL_ODBC_SQLSTATE_HY100, // Uniqueness option type out of range
        DAL_ODBC_SQLSTATE_HY101, // Accuracy option type out of range
        DAL_ODBC_SQLSTATE_HY103, // Invalid retrieval code
        DAL_ODBC_SQLSTATE_HY104, // Invalid precision or scale value
        DAL_ODBC_SQLSTATE_HY105, // Invalid parameter type
        DAL_ODBC_SQLSTATE_HY106, // Fetch type out of range
        DAL_ODBC_SQLSTATE_HY107, // Row value out of range
        DAL_ODBC_SQLSTATE_HY109, // Invalid cursor position
        DAL_ODBC_SQLSTATE_HY110, // Invalid driver completion
        DAL_ODBC_SQLSTATE_HY111, // Invalid bookmark value
        DAL_ODBC_SQLSTATE_HYC00, // Optional feature not implemented
        DAL_ODBC_SQLSTATE_HYT00, // Timeout expired
        DAL_ODBC_SQLSTATE_HYT01, // Connection timeout expired
        DAL_ODBC_SQLSTATE_IM000, // ???
        DAL_ODBC_SQLSTATE_IM001, // Driver does not support this function
        DAL_ODBC_SQLSTATE_IM002, // Data source name not found and no default driver specified
        DAL_ODBC_SQLSTATE_IM003, // Specified driver could not be loaded
        DAL_ODBC_SQLSTATE_IM004, // Driver's SQLAllocHandle on SQL_HANDLE_ENV failed
        DAL_ODBC_SQLSTATE_IM005, // Driver's SQLAllocHandle on SQL_HANDLE_DBC failed
        DAL_ODBC_SQLSTATE_IM006, // Driver's SQLSetConnectAttr failed
        DAL_ODBC_SQLSTATE_IM007, // No data source or driver specified; dialog prohibited
        DAL_ODBC_SQLSTATE_IM008, // Dialog failed
        DAL_ODBC_SQLSTATE_IM009, // Unable to load translation DLL
        DAL_ODBC_SQLSTATE_IM010, // Data source name too long
        DAL_ODBC_SQLSTATE_IM011, // Driver name too long
        DAL_ODBC_SQLSTATE_IM012, // DRIVER keyword syntax error
        DAL_ODBC_SQLSTATE_IM013, // Trace file error
        DAL_ODBC_SQLSTATE_IM014, // Invalid name of File DSN
        DAL_ODBC_SQLSTATE_IM015  // Corrupt file data source

/*
  DAL_ODBC_SQLSTATE_08000 = DAL_SQLSTATE_08000, // connection exception
  DAL_ODBC_SQLSTATE_08001 = DAL_SQLSTATE_08001, // SQL-client unable to establish SQL-connection
  DAL_ODBC_SQLSTATE_0A000 = DAL_SQLSTATE_0A000, // feature not supported
  DAL_ODBC_SQLSTATE_22000 = DAL_SQLSTATE_22000, // data exception
  DAL_ODBC_SQLSTATE_23000 = DAL_SQLSTATE_23000, // integrity constraint violation
  DAL_ODBC_SQLSTATE_25000 = DAL_SQLSTATE_25000, // invalid transaction state
  DAL_ODBC_SQLSTATE_25001 = DAL_SQLSTATE_25001, // active SQL-transaction
  DAL_ODBC_SQLSTATE_25006 = DAL_SQLSTATE_25006, // read-only SQL-transaction
  DAL_ODBC_SQLSTATE_28000 = DAL_SQLSTATE_28000, // invalid authorization specification
  DAL_ODBC_SQLSTATE_42000 = DAL_SQLSTATE_42000  // syntax error or access rule violation
*/
        //DAL_ODBC_SQLSTATE_XY000 = DAL_SQLSTATE_LAST + 1
    } engine_states_t;
    


// Dynamic SQL error
    typedef sqlstate<DAL_ODBC_SQLSTATE_07000>                         SQLSTATE_07000;

// Wrong number of parameters
    typedef sqlstate<DAL_ODBC_SQLSTATE_07001, SQLSTATE_07000>         SQLSTATE_07001;

// COUNT field incorrect
    typedef sqlstate<DAL_ODBC_SQLSTATE_07002, SQLSTATE_07000>         SQLSTATE_07002;

// Prepared statement not a cursor-specification
    typedef sqlstate<DAL_ODBC_SQLSTATE_07005, SQLSTATE_07000>         SQLSTATE_07005;

// Restricted data type attribute violation
    typedef sqlstate<DAL_ODBC_SQLSTATE_07006, SQLSTATE_07000>         SQLSTATE_07006;

// Invalid descriptor index
    typedef sqlstate<DAL_ODBC_SQLSTATE_07009, SQLSTATE_07000>         SQLSTATE_07009;

// Invalid use of default parameter
    typedef sqlstate<DAL_ODBC_SQLSTATE_07S01, SQLSTATE_07000>         SQLSTATE_07S01;


// Connection exception
    typedef sqlstate<DAL_ODBC_SQLSTATE_08000>                         SQLSTATE_08000;


// Client unable to establish connection
    typedef sqlstate<DAL_ODBC_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_08001;

// Connection name in use
    typedef sqlstate<DAL_ODBC_SQLSTATE_08002, SQLSTATE_08000>         SQLSTATE_08002;

// Connection not open
    typedef sqlstate<DAL_ODBC_SQLSTATE_08003, SQLSTATE_08000>         SQLSTATE_08003;

// Server rejected the connection
    typedef sqlstate<DAL_ODBC_SQLSTATE_08004, SQLSTATE_08000>         SQLSTATE_08004;

// Connection failure during transaction
    typedef sqlstate<DAL_ODBC_SQLSTATE_08007, SQLSTATE_08000>         SQLSTATE_08007;

// Communication link failure
    typedef sqlstate<DAL_ODBC_SQLSTATE_08S01, SQLSTATE_08000>         SQLSTATE_08S01;


// cardinality violation
    typedef sqlstate<DAL_ODBC_SQLSTATE_21000>                         SQLSTATE_21000;

// Insert value list does not match column list
    typedef sqlstate<DAL_ODBC_SQLSTATE_21S01, SQLSTATE_21000>         SQLSTATE_21S01;

// Degree of derived table does not match column list
    typedef sqlstate<DAL_ODBC_SQLSTATE_21S02, SQLSTATE_21000>         SQLSTATE_21S02;


// Data exception
    typedef sqlstate<DAL_ODBC_SQLSTATE_22000>                         SQLSTATE_22000;

// String data, right-truncated
    typedef sqlstate<DAL_ODBC_SQLSTATE_22001, SQLSTATE_22000>         SQLSTATE_22001;

// Indicator variable required but not supplied
    typedef sqlstate<DAL_ODBC_SQLSTATE_22002, SQLSTATE_22000>         SQLSTATE_22002;

// Numeric value out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_22003, SQLSTATE_22000>         SQLSTATE_22003;

// Invalid datetime format
    typedef sqlstate<DAL_ODBC_SQLSTATE_22007, SQLSTATE_22000>         SQLSTATE_22007;

// Datetime field overflow
    typedef sqlstate<DAL_ODBC_SQLSTATE_22008, SQLSTATE_22000>         SQLSTATE_22008;

// Division by zero
    typedef sqlstate<DAL_ODBC_SQLSTATE_22012, SQLSTATE_22000>         SQLSTATE_22012;

// Interval field overflow
    typedef sqlstate<DAL_ODBC_SQLSTATE_22015, SQLSTATE_22000>         SQLSTATE_22015;

// Invalid character value for cast specification
    typedef sqlstate<DAL_ODBC_SQLSTATE_22018, SQLSTATE_22000>         SQLSTATE_22018;

// Invalid escape character
    typedef sqlstate<DAL_ODBC_SQLSTATE_22019, SQLSTATE_22000>         SQLSTATE_22019;

// Invalid escape sequence
    typedef sqlstate<DAL_ODBC_SQLSTATE_22025, SQLSTATE_22000>         SQLSTATE_22025;

// String data, length mismatch
    typedef sqlstate<DAL_ODBC_SQLSTATE_22026, SQLSTATE_22000>         SQLSTATE_22026;

// Integrity constraint violation
    typedef sqlstate<DAL_ODBC_SQLSTATE_23000>                         SQLSTATE_23000;

// Invalid cursor state
    typedef sqlstate<DAL_ODBC_SQLSTATE_24000>                         SQLSTATE_24000;

// Invalid transaction state
    typedef sqlstate<DAL_ODBC_SQLSTATE_25000>                         SQLSTATE_25000;

// Transaction state
    typedef sqlstate<DAL_ODBC_SQLSTATE_25S01, SQLSTATE_25000>         SQLSTATE_25S01;

// Transaction is still active
    typedef sqlstate<DAL_ODBC_SQLSTATE_25S02, SQLSTATE_22000>         SQLSTATE_25S02;

// Transaction is rolled back
    typedef sqlstate<DAL_ODBC_SQLSTATE_25S03, SQLSTATE_22000>         SQLSTATE_25S03;

// Invalid authorization specification
    typedef sqlstate<DAL_ODBC_SQLSTATE_28000>                         SQLSTATE_28000;

// Invalid cursor name
    typedef sqlstate<DAL_ODBC_SQLSTATE_34000>                         SQLSTATE_34000;

// Duplicate cursor name
    typedef sqlstate<DAL_ODBC_SQLSTATE_3C000>                         SQLSTATE_3C000;

// Invalid catalog name
    typedef sqlstate<DAL_ODBC_SQLSTATE_3D000>                         SQLSTATE_3D000;

// Invalid schema name
    typedef sqlstate<DAL_ODBC_SQLSTATE_3F000>                         SQLSTATE_3F000;

// General transaction error
    typedef sqlstate<DAL_ODBC_SQLSTATE_40000>                         SQLSTATE_40000;

// Serialization failure
    typedef sqlstate<DAL_ODBC_SQLSTATE_40001, SQLSTATE_40000>         SQLSTATE_40001;

// Integrity constraint violation
    typedef sqlstate<DAL_ODBC_SQLSTATE_40002, SQLSTATE_40000>         SQLSTATE_40002;

// Statement completion unknown
    typedef sqlstate<DAL_ODBC_SQLSTATE_40003, SQLSTATE_40000>         SQLSTATE_40003;

// Syntax error or access violation
    typedef sqlstate<DAL_ODBC_SQLSTATE_42000>                         SQLSTATE_42000;

// Base table or view already exists
    typedef sqlstate<DAL_ODBC_SQLSTATE_42S01, SQLSTATE_42000>         SQLSTATE_42S01;

// Base table or view not found
    typedef sqlstate<DAL_ODBC_SQLSTATE_42S02, SQLSTATE_42000>         SQLSTATE_42S02;

// Index already exists
    typedef sqlstate<DAL_ODBC_SQLSTATE_42S11, SQLSTATE_42000>         SQLSTATE_42S11;

// Index not found
    typedef sqlstate<DAL_ODBC_SQLSTATE_42S12, SQLSTATE_42000>         SQLSTATE_42S12;

// Column already exists
    typedef sqlstate<DAL_ODBC_SQLSTATE_42S21, SQLSTATE_42000>         SQLSTATE_42S21;

// Column not found
    typedef sqlstate<DAL_ODBC_SQLSTATE_42S22, SQLSTATE_42000>         SQLSTATE_42S22;

// WITH CHECK OPTION violation
    typedef sqlstate<DAL_ODBC_SQLSTATE_44000>                         SQLSTATE_44000;

// General error
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY000>                         SQLSTATE_HY000;

// Memory allocation error
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY001, SQLSTATE_HY000>         SQLSTATE_HY001;

// Invalid application buffer type
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY003, SQLSTATE_HY000>         SQLSTATE_HY003;

// Invalid SQL data type
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY004, SQLSTATE_HY000>         SQLSTATE_HY004;

// Associated statement is not prepared
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY007, SQLSTATE_HY000>         SQLSTATE_HY007;

// Operation canceled
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY008, SQLSTATE_HY000>         SQLSTATE_HY008;

// Invalid use of null pointer
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY009, SQLSTATE_HY000>         SQLSTATE_HY009;

// Function sequence error
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY010, SQLSTATE_HY000>         SQLSTATE_HY010;

// Attribute cannot be set now
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY011, SQLSTATE_HY000>         SQLSTATE_HY011;

// Invalid transaction operation code
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY012, SQLSTATE_HY000>         SQLSTATE_HY012;

// Memory management error
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY013, SQLSTATE_HY000>         SQLSTATE_HY013;

// Limit on the number of handles exceeded
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY014, SQLSTATE_HY000>         SQLSTATE_HY014;

// No cursor name available
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY015, SQLSTATE_HY000>         SQLSTATE_HY015;

// Cannot modify an implementation row descriptor
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY016, SQLSTATE_HY000>         SQLSTATE_HY016;

// Invalid use of an automatically allocated descriptor handle
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY017, SQLSTATE_HY000>         SQLSTATE_HY017;

// Server declined cancel request
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY018, SQLSTATE_HY000>         SQLSTATE_HY018;

// Non-character and non-binary data sent in pieces
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY019, SQLSTATE_HY000>         SQLSTATE_HY019;

// Attempt to concatenate a null value
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY020, SQLSTATE_HY000>         SQLSTATE_HY020;

// Inconsistent descriptor information
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY021, SQLSTATE_HY000>         SQLSTATE_HY021;

// Invalid attribute value
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY024, SQLSTATE_HY000>         SQLSTATE_HY024;

// Invalid string or buffer length
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY090, SQLSTATE_HY000>         SQLSTATE_HY090;

// Invalid descriptor field identifier
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY091, SQLSTATE_HY000>         SQLSTATE_HY091;

// Invalid attribute/option identifier
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY092, SQLSTATE_HY000>         SQLSTATE_HY092;

// Function type out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY095, SQLSTATE_HY000>         SQLSTATE_HY095;

// Invalid information type
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY096, SQLSTATE_HY000>         SQLSTATE_HY096;

// Column type out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY097, SQLSTATE_HY000>         SQLSTATE_HY097;

// Scope type out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY098, SQLSTATE_HY000>         SQLSTATE_HY098;

// Nullable type out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY099, SQLSTATE_HY000>         SQLSTATE_HY099;

// Uniqueness option type out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY100, SQLSTATE_HY000>         SQLSTATE_HY100;

// Accuracy option type out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY101, SQLSTATE_HY000>         SQLSTATE_HY101;

// Invalid retrieval code
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY103, SQLSTATE_HY000>         SQLSTATE_HY103;

// Invalid precision or scale value
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY104, SQLSTATE_HY000>         SQLSTATE_HY104;

// Invalid parameter type
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY105, SQLSTATE_HY000>         SQLSTATE_HY105;

// Fetch type out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY106, SQLSTATE_HY000>         SQLSTATE_HY106;

// Row value out of range
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY107, SQLSTATE_HY000>         SQLSTATE_HY107;

// Invalid cursor position
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY109, SQLSTATE_HY000>         SQLSTATE_HY109;

// Invalid driver completion
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY110, SQLSTATE_HY000>         SQLSTATE_HY110;

// Invalid bookmark value
    typedef sqlstate<DAL_ODBC_SQLSTATE_HY111, SQLSTATE_HY000>         SQLSTATE_HY111;

// Optional feature not implemented
    typedef sqlstate<DAL_ODBC_SQLSTATE_HYC00, SQLSTATE_HY000>         SQLSTATE_HYC00;

// Timeout expired
    typedef sqlstate<DAL_ODBC_SQLSTATE_HYT00, SQLSTATE_HY000>         SQLSTATE_HYT00;

// Connection timeout expired
    typedef sqlstate<DAL_ODBC_SQLSTATE_HYT01, SQLSTATE_HY000>         SQLSTATE_HYT01;


// ????
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM000>                         SQLSTATE_IM000;

// Driver does not support this function
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM001, SQLSTATE_IM000>         SQLSTATE_IM001;

// Data source name not found and no default driver specified
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM002, SQLSTATE_IM000>         SQLSTATE_IM002;

// Specified driver could not be loaded
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM003, SQLSTATE_IM000>         SQLSTATE_IM003;

// Driver's SQLAllocHandle on SQL_HANDLE_ENV failed
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM004, SQLSTATE_IM000>         SQLSTATE_IM004;

// Driver's SQLAllocHandle on SQL_HANDLE_DBC failed
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM005, SQLSTATE_IM000>         SQLSTATE_IM005;

// Driver's SQLSetConnectAttr failed
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM006, SQLSTATE_IM000>         SQLSTATE_IM006;

// No data source or driver specified; dialog prohibited
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM007, SQLSTATE_IM000>         SQLSTATE_IM007;

// Dialog failed
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM008, SQLSTATE_IM000>         SQLSTATE_IM008;

// Unable to load translation DLL
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM009, SQLSTATE_IM000>         SQLSTATE_IM009;

// Data source name too long
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM010, SQLSTATE_IM000>         SQLSTATE_IM010;

// Driver name too long
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM011, SQLSTATE_IM000>         SQLSTATE_IM011;

// DRIVER keyword syntax error
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM012, SQLSTATE_IM000>         SQLSTATE_IM012;

// Trace file error
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM013, SQLSTATE_IM000>         SQLSTATE_IM013;

// Invalid name of File DSN
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM014, SQLSTATE_IM000>         SQLSTATE_IM014;

// Corrupt file data source
    typedef sqlstate<DAL_ODBC_SQLSTATE_IM015, SQLSTATE_IM000>         SQLSTATE_IM015;

/*

// connection exception
typedef sqlstate<DAL_ODBC_SQLSTATE_08000>                         SQLSTATE_08000;
typedef sqlstate<DAL_ODBC_SQLSTATE_08000>                         SQLSTATE_connection_exception;

// SQL-client unable to establish SQL-connection
typedef sqlstate<DAL_ODBC_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_08001;
typedef sqlstate<DAL_ODBC_SQLSTATE_08001, SQLSTATE_08000>         SQLSTATE_sql_client_unable_to_establish_sql_connection;

// feature not supported
typedef sqlstate<DAL_ODBC_SQLSTATE_0A000>                         SQLSTATE_0A000;
typedef sqlstate<DAL_ODBC_SQLSTATE_0A000>                         SQLSTATE_feature_not_supported;

// data exception
typedef sqlstate<DAL_ODBC_SQLSTATE_22000>                         SQLSTATE_22000;
typedef sqlstate<DAL_ODBC_SQLSTATE_22000>                         SQLSTATE_data_exception;

// integrity constraint violation
typedef sqlstate<DAL_ODBC_SQLSTATE_23000>                         SQLSTATE_23000;
typedef sqlstate<DAL_ODBC_SQLSTATE_23000>                         SQLSTATE_integrity_constraint_violation;

// invalid transaction state
typedef sqlstate<DAL_ODBC_SQLSTATE_25000>                         SQLSTATE_25000;
typedef sqlstate<DAL_ODBC_SQLSTATE_25000>                         SQLSTATE_invalid_transaction_state;

// active SQL-transaction
typedef sqlstate<DAL_ODBC_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_25001;
typedef sqlstate<DAL_ODBC_SQLSTATE_25001, SQLSTATE_25000>         SQLSTATE_active_sql_transaction;

// read-only SQL-transaction
typedef sqlstate<DAL_ODBC_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_25006;
typedef sqlstate<DAL_ODBC_SQLSTATE_25006, SQLSTATE_25000>         SQLSTATE_read_only_sql_transaction;

// invalid authorization specification
typedef sqlstate<DAL_ODBC_SQLSTATE_28000>                         SQLSTATE_28000;
typedef sqlstate<DAL_ODBC_SQLSTATE_28000>                         SQLSTATE_invalid_authorization_specification;

// syntax error or access rule violation
typedef sqlstate<DAL_ODBC_SQLSTATE_42000>                         SQLSTATE_42000;
typedef sqlstate<DAL_ODBC_SQLSTATE_42000>                         SQLSTATE_syntax_error_or_access_rule_violation;


//     typedef sqlstate<DAL_ODBC_SQLSTATE_XY000>                         SQLSTATE_XY000;
*/
};




//------------------------------------------------------------------------------
///
/// @brief SQLite Diagnostic
class DBWTL_EXPORT OdbcDiag : public DiagBase
{
public:
    virtual ~OdbcDiag(void)
    {}


    virtual OdbcDiag* clone(void) const = 0;
    virtual void raiseException(void) const;


protected:
    OdbcDiag(dalstate_t state,
             CodePosInfo pos,
             Variant what,
             String description,
             const std::string &sqlstate,
             int nativecode);
    
    OdbcDiag(const OdbcDiag& ref);

    odbc_sqlstates::engine_states_t m_sqlstate_id;
    std::string m_sqlstate;
    int m_nativecode;
};



typedef DiagController<OdbcDiag> OdbcDiagController;



//------------------------------------------------------------------------------
///
/// @brief SQLite BLOB
class DBWTL_EXPORT OdbcBlob : public IBlobBuffer
{
    // no odbc specific extensions
};



//------------------------------------------------------------------------------
///
/// @brief SQLite BLOB
class DBWTL_EXPORT OdbcMemo : public IMemoBuffer
{
    // no odbc specific extensions
};





//------------------------------------------------------------------------------
///
/// @internal
/// @details 
/// Internal representation of the SQLite data(types).
///
/// @todo How to deal with UTF-8 strings?
class OdbcData : public custom_deepcopy
{
public:
    OdbcData(void);
    virtual ~OdbcData(void);

    virtual OdbcBlob*  getBlobStream(void) const = 0;
    virtual UnicodeStreamBuf*  getMemoStream(void) const = 0;

    virtual String getString(void) const = 0;
    virtual signed short int getSShort(void) const = 0;
    virtual unsigned short int getUShort(void) const = 0;
    virtual signed long int getSLong(void) const = 0;
    virtual unsigned long int getULong(void) const = 0;
    virtual float getFloat(void) const = 0;
    virtual double getDouble(void) const = 0;
    virtual unsigned char getBit(void) const = 0;
    virtual signed char getSTinyint(void) const = 0;
    virtual unsigned char getUTinyint(void) const = 0;
    virtual signed long long getSBigint(void) const = 0;
    virtual unsigned long long getUBigint(void) const = 0;
    virtual TVarbinary getBinary(void) const = 0;
    virtual unsigned long int getBookmark(void) const = 0;
    virtual const unsigned char* getVarbookmark(void) const = 0;
    virtual TDate getDate(void) const = 0;
    virtual TTime getTime(void) const = 0;
    virtual TTimestamp getTimestamp(void) const = 0;
    virtual TNumeric getNumeric(void) const = 0;
//TGuid getGUID(void) const = 0;



 
    virtual rowid_t getCurrentRowID(void) const = 0;


    virtual bool isnull(void) const = 0;

    virtual void refresh(void) = 0;

    virtual daltype_t daltype(void) const = 0;

    virtual IVariantValue* do_deepcopy(const IVariantValue* owner) const;

    virtual void fetchParts(void) = 0;
};



namespace odbc_ext
{

    class DBWTL_EXPORT Text : public TCustomType
    {
    public:
        Text(void) { }
        Text(const OdbcVariant&) { }
        Text& operator=(const OdbcVariant&) { return *this; }


        virtual daltype_t  datatype(void) const { return DAL_TYPE_CUSTOM; }
        virtual String asStr(std::locale loc) const { return L"foo"; }
        //virtual std::wstring asWideStr() const { return L"foo"; }
    };

}






//------------------------------------------------------------------------------
///
/// @brief SQLite Variant
class DBWTL_EXPORT OdbcVariant : public EngineVariant
{
public:
    OdbcVariant(OdbcData* data);

    virtual ~OdbcVariant(void);

    virtual void refresh(void);

    virtual odbc_ext::Text asText(void) const
    { return odbc_ext::Text(); }

    virtual void fetchParts(void);

private:
    OdbcData *m_data;

    OdbcVariant(const OdbcVariant& ref);
    OdbcVariant(void);
    OdbcVariant& operator=(const OdbcVariant&);
};





//------------------------------------------------------------------------------
///
/// @brief SQLite Table object
class DBWTL_EXPORT OdbcTable : public ITable
{
public:
    typedef utils::SmartPtr<OdbcTable,
                            utils::RefCounted,
                            utils::AllowConversion> ptr;
    
    OdbcTable(String dbname, OdbcResult& src);
    
    virtual ~OdbcTable(void);


    virtual const ITable::value_type& getName(void) const;
    virtual const ITable::value_type& getCatalog(void) const;
    virtual const ITable::value_type& getSchema(void) const;
    virtual const ITable::value_type& getComment(void) const;
    virtual const ITable::value_type& getDDL(void) const;
    virtual const ITable::value_type& isSystemObject(void) const;


protected:
    Variant  m_name;
    Variant  m_catalog;
    Variant  m_schema;
    Variant  m_comment;
    Variant  m_ddl;
    Variant  m_systemobject;
};


//------------------------------------------------------------------------------
///
/// @brief SQLite Index object
class DBWTL_EXPORT OdbcIndex : public IIndex
{
public:
    typedef utils::SmartPtr<OdbcIndex,
                            utils::RefCounted,
                            utils::AllowConversion> ptr;
    
    OdbcIndex(String dbname, OdbcResult& src);
    
    virtual ~OdbcIndex(void);


    virtual const ITable::value_type& getName(void) const;
    virtual const ITable::value_type& getTable(void) const;
    virtual const ITable::value_type& getCatalog(void) const;
    virtual const ITable::value_type& getSchema(void) const;
    virtual const ITable::value_type& getComment(void) const;
    virtual const ITable::value_type& getDDL(void) const;
    virtual const ITable::value_type& isSystemObject(void) const;


protected:
    Variant  m_name;
    Variant  m_table;
    Variant  m_catalog;
    Variant  m_schema;
    Variant  m_comment;
    Variant  m_ddl;
    Variant  m_systemobject;
};



//------------------------------------------------------------------------------
///
/// @brief SQLite View object
class DBWTL_EXPORT OdbcView : public IView
{
public:
    typedef utils::SmartPtr<OdbcView,
                            utils::RefCounted,
                            utils::AllowConversion> ptr;
    
    OdbcView(String dbname, OdbcResult& src);
    
    virtual ~OdbcView(void);


    virtual const IView::value_type& getName(void) const;
    virtual const IView::value_type& getCatalog(void) const;
    virtual const IView::value_type& getSchema(void) const;
    virtual const IView::value_type& getComment(void) const;
    virtual const IView::value_type& getDDL(void) const;
    virtual const IView::value_type& isSystemObject(void) const;


protected:
    Variant  m_name;
    Variant  m_catalog;
    Variant  m_schema;
    Variant  m_comment;
    Variant  m_ddl;
    Variant  m_systemobject;
};



//------------------------------------------------------------------------------
///
/// @brief SQLite Catalog object
class DBWTL_EXPORT OdbcCatalog : public ICatalog
{
public:
    typedef utils::SmartPtr<OdbcCatalog,
                            utils::RefCounted,
                            utils::AllowConversion> ptr;
    
    OdbcCatalog(String dbname);
    
    virtual ~OdbcCatalog(void);


    virtual const ICatalog::value_type& getName(void) const;
    virtual const ICatalog::value_type& getComment(void) const;
    virtual const ICatalog::value_type& getDDL(void) const;
    virtual const ICatalog::value_type& isSystemObject(void) const;


protected:
    Variant  m_name;
    Variant  m_comment;
    Variant  m_ddl;
    Variant  m_systemobject;
};




//------------------------------------------------------------------------------
///
/// @brief SQLite Column Descriptor
class DBWTL_EXPORT OdbcColumnDesc : public ColumnDescBase
{
public:
    friend class OdbcResult;

    OdbcColumnDesc(void) : ColumnDescBase()
    {}

    virtual ~OdbcColumnDesc(void)
    {}

};


class DBWTL_EXPORT OdbcMetadata : public IMetadata
{
public:
	typedef utils::SmartPtr<OdbcMetadata,
	                        utils::RefCounted,
				utils::AllowConversion> ptr;

	OdbcMetadata(OdbcDbc &dbc) : IMetadata(), m_dbc(dbc)
	{}


        virtual RecordSet getCatalogs(const Variant &catalog = Variant(),
                const IMetadata::ObjectClass system = META_OBJECT_CLASS_USER,
                const DatasetFilter &filter = NoFilter());
        virtual RecordSet getSchemas(const Variant &catalog = Variant(),
                const Variant &schema = Variant(),
                const ObjectClass system = META_OBJECT_CLASS_USER,
                const DatasetFilter &filter = NoFilter());
        virtual RecordSet getTables(const Variant &schema = Variant(),
                const Variant &catalog = Variant(),
                const Variant &table = Variant(),
                const IMetadata::ObjectClass system = META_OBJECT_CLASS_USER,
                const DatasetFilter &filter = NoFilter());
        virtual RecordSet getColumns(const Variant &table = Variant(),
                const Variant &schema = Variant(),
                const Variant &catalog = Variant(),
                const Variant &column = Variant(),
                const IMetadata::ObjectClass system = META_OBJECT_CLASS_USER,
                const DatasetFilter &filter = NoFilter());


	virtual ~OdbcMetadata(void)
	{}

	OdbcDbc &m_dbc;
};


//------------------------------------------------------------------------------
///
/// @brief SQLite Datatype
class DBWTL_EXPORT OdbcDatatype : public DatatypeBase
{
public:
    friend class OdbcDbc;

    typedef utils::SmartPtr<OdbcDatatype,
                            utils::RefCounted,
                            utils::AllowConversion> ptr;

    OdbcDatatype(void);

    virtual ~OdbcDatatype(void);
};






//------------------------------------------------------------------------------
///
/// @brief SQLite Resultset
class DBWTL_EXPORT OdbcResult : public ResultBase
{
public:
    typedef std::auto_ptr<OdbcResult> ptr;

    typedef size_t                      bookmark_type;
    typedef OdbcVariant               value_type;

    OdbcResult(OdbcDiagController &dc) : ResultBase(),
        m_diag(dc)
        {}

    virtual const value_type&     column(colnum_t num) = 0;
    //virtual OdbcVariant&     field(colnum_t num) = 0;
    virtual const value_type&     column(String name) = 0;
    //virtual OdbcVariant&     field(std::wstring name) = 0;


    virtual const OdbcColumnDesc& describeColumn(colnum_t num) const = 0;

    virtual const OdbcColumnDesc& describeColumn(String name) const = 0;
  
protected:
    OdbcDiagController &m_diag;
};





//------------------------------------------------------------------------------
///
///  @brief SQLite Statement
class DBWTL_EXPORT OdbcStmt : public StmtBase
{
public:
    typedef std::auto_ptr<OdbcStmt> ptr;

    OdbcStmt(void)
        : StmtBase(),
        m_diag()
        {}

    virtual OdbcResult&        resultset(void) = 0;
    virtual const OdbcResult&  resultset(void) const = 0;

    virtual bool                 diagAvail(void) const;
    virtual const OdbcDiag&   fetchDiag(void);

protected:
    OdbcDiagController m_diag;
};





//------------------------------------------------------------------------------
///
/// @brief SQLite Connection
class DBWTL_EXPORT OdbcDbc : public DbcBase
{
public:
    typedef std::auto_ptr<OdbcDbc> ptr;

    OdbcDbc(void)
        : DbcBase(),
        m_diag()
        {}

    virtual OdbcStmt*    newStatement(void) = 0;

    virtual OdbcMetadata* newMetadata(void);

    virtual OdbcStmt*      getOdbcCatalogs(void) = 0;
    virtual OdbcStmt*      getOdbcSchemas(const Variant &catalog) = 0;
    virtual OdbcStmt*      getOdbcTables(const Variant &catalog, const Variant &schema, const Variant &type) = 0;
    virtual OdbcStmt*      getOdbcColumns(const Variant &catalog, const Variant &schema, const Variant &table) = 0;

    virtual Variant        getCurrentCatalog(void) = 0;

    virtual TableList      getTables(const ITableFilter& = EmptyTableFilter());
    virtual ViewList       getViews(const IViewFilter& = EmptyViewFilter());
    virtual DatatypeList   getDatatypes(const IDatatypeFilter& = EmptyDatatypeFilter());
    virtual SchemaList     getSchemas(const ISchemaFilter& = EmptySchemaFilter());
    virtual CatalogList    getCatalogs(const ICatalogFilter& = EmptyCatalogFilter());
    virtual ProcedureList  getProcedures(const IProcedureFilter& = EmptyProcedureFilter());
    virtual IndexList      getIndices(const IIndexFilter& = EmptyIndexFilter());

    virtual void           directCmd(String cmd);

    virtual void           commit(Transaction trx);
    virtual void           rollback(Transaction trx);

    virtual void           commit(void) = 0;
    virtual void           rollback(String name = String()) = 0;

    virtual bool                 diagAvail(void) const;
    virtual const OdbcDiag&    fetchDiag(void);


protected:
    OdbcDiagController m_diag;
};





//------------------------------------------------------------------------------
///
/// @brief SQLite Environment
class DBWTL_EXPORT OdbcEnv : public EnvBase
{
public:
    typedef std::auto_ptr<OdbcEnv> ptr;

    OdbcEnv(void);
    virtual ~OdbcEnv(void);
    
    virtual OdbcDbc* newConnection(void) = 0;


    virtual bool                 diagAvail(void) const;
    virtual const OdbcDiag&    fetchDiag(void);

    virtual dal_engine getEngineType(void) const { return DAL_ENGINE_ODBC; }
        

protected:
    OdbcDiagController m_diag;
};







//------------------------------------------------------------------------------
///
/// @brief Main SQLite interface class 
struct odbc
{
    typedef OdbcDbc          DBC;
    typedef OdbcResult       RESULT;
    typedef OdbcStmt         STMT;
    typedef OdbcEnv          ENV;
    typedef OdbcVariant      VALUE;
    typedef OdbcDiag         DIAG;
    typedef odbc_sqlstates   STATES;
    typedef OdbcTable        TABLE;
    typedef OdbcColumnDesc   COLUMNDESC;
    typedef OdbcMetadata     METADATA;



    //typedef STATES::engine_states_t 

    DBWTL_EXPORT static const char* sqlstate2string(STATES::engine_states_t id);


    ///
    /// @brief Create a new Environment
    ///
    /// Current supported drivers are:
    ///  - libodbc
    ///
    DBWTL_EXPORT static ENV* createEnv(String driver);
};


DAL_NAMESPACE_END



DB_NAMESPACE_BEGIN




template<>
struct value_traits<OdbcData*>
{
    typedef raw_pointer<OdbcData*>  stored_type;
    typedef OdbcData*    value_type;
    typedef var_info<OdbcData*>         info_type;
};



template<>
struct value_traits<const OdbcData*>
{
/*
  typedef raw_pointer<const OdbcData*>  stored_type;
  typedef const OdbcData*    value_type;
  typedef var_info<const OdbcData*>         info_type;
*/
};



//------------------------------------------------------------------------------
///
/// @brief Variant storage accessor for OdbcData 
template<>
struct sv_accessor<OdbcData*> : public virtual sa_base<OdbcData*>,
                                         public supports<signed char>,
                                         public supports<signed int>,
                                         public supports<signed short>,
                                         public supports<signed long long>,
                                         public supports<unsigned char>,
                                         public supports<unsigned int>,
                                         public supports<unsigned short>,
                                         public supports<unsigned long long>,
                                         public supports<float>,
                                         public supports<double>,
                                public supports<bool>,
                                public supports<BlobStream>,
                                         public supports<MemoStream>,
					 public supports<Blob>,
					 public supports<Memo>,
                                public supports<TDate>,
                                public supports<TTime>,
                                public supports<TTimestamp>,
                                public supports<TNumeric>,
                                         public supports<TVarbinary>,
                                public supports<String>
{
    virtual signed char cast(signed char*, std::locale loc) const;
    virtual signed int cast(signed int*, std::locale loc) const;
    virtual signed short cast(signed short*, std::locale loc) const;
    virtual signed long long cast(signed long long*, std::locale loc) const;
    virtual unsigned char cast(unsigned char*, std::locale loc) const;
    virtual unsigned int cast(unsigned int*, std::locale loc) const;
    virtual unsigned short cast(unsigned short*, std::locale loc) const;
    virtual unsigned long long cast(unsigned long long*, std::locale loc) const;

    virtual float cast(float*, std::locale loc) const;
    virtual double cast(double*, std::locale loc) const;

    virtual bool cast(bool*, std::locale loc) const;
    virtual BlobStream cast(BlobStream*, std::locale loc) const;
    virtual MemoStream cast(MemoStream*, std::locale loc) const;
    virtual Blob cast(Blob*, std::locale loc) const;
    virtual Memo cast(Memo*, std::locale loc) const;
    virtual TDate cast(TDate*, std::locale loc) const;
    virtual TTime cast(TTime*, std::locale loc) const;
    virtual TTimestamp cast(TTimestamp*, std::locale loc) const;
    virtual TNumeric cast(TNumeric*, std::locale loc) const;
    virtual TVarbinary cast(TVarbinary*, std::locale loc) const;
    virtual String cast(String*, std::locale loc) const;


    virtual bool valueNullCheck(void) const;
    //virtual bool isNull() const;

    virtual daltype_t datatype() const;

protected:
   sv_accessor(void) : m_blob_buffer(), m_memo_buffer() {}

   sv_accessor(const sv_accessor &orig)
   : m_blob_buffer(orig.m_blob_buffer),
     m_memo_buffer(orig.m_memo_buffer)
   {}

   mutable std::shared_ptr<std::stringstream> m_blob_buffer;
   mutable std::shared_ptr<std::wstringstream> m_memo_buffer;
};


/*
  Template<>
  class read_accessor<odbc_ext::Text> : public default_accessor<odbc_ext::Text>
  {
  DAL_VARIANT_ACCESSOR;

  virtual ~read_accessor(void) { }
  virtual daltype_t datatype() const { return DAL_TYPE_CUSTOM; }    
  };
*/


/*
  template<>
  struct assign_value<OdbcData*>
  {
  void operator()(OdbcData *value, const Variant &src)
  {
  // *value = src;
  //value->assign(src);
  }
  };
*/


/*
  template<>
  struct DeepCopyPtr<OdbcData*>
  {
  Variant operator()(const OdbcData* ptr)
  {
  }
  };
*/




struct odbc_datatypes : public basic_datatypes
{
/*
  virtual ~odbc_datatypes(void)
  {}
*/
	typedef odbc_ext::Text Text;
};


template<typename tag>
class OdbcEnvironment : public Environment<odbc, tag>
{
public:

    OdbcEnvironment(String str)
        : Environment<odbc, tag>(str)
    {}

    virtual ~OdbcEnvironment(void)
    {}

    virtual void specialFunc(void)
    {
    	this->m_env;
    }

};


//------------------------------------------------------------------------------
///
/// 
template<typename tag>
struct db_traits<odbc, tag>
{
    //typedef Environment<odbc, tag>      environment_type;
    typedef OdbcEnvironment<tag>             environment_type; /// @todo for tests only
    typedef Connection<odbc, tag>       connection_type;
    typedef Statement<odbc, tag>        statement_type;
    typedef Result<odbc, tag>           resultset_type;
    typedef CachedResult<odbc, tag>     cached_resultset_type;
    typedef Metadata<odbc, tag>		metadata_type;
    typedef odbc::VALUE                 value_type;

    typedef odbc::RESULT                dal_resultset_type;
    typedef odbc::STMT                  dal_stmt_type;
    typedef odbc::ENV                   dal_env_type;
    typedef odbc::DBC                   dal_dbc_type;
    typedef odbc::DIAG                  dal_diag_type;    
    typedef odbc::COLUMNDESC            dal_columndesc_type;
    typedef odbc::METADATA		dal_metadata_type;
    typedef odbc::STATES                sqlstate_types;

    typedef odbc_datatypes                   datatype_types;


    typedef Variant                            dal_variant_type;

    enum { DB_SYSTEM_ID = DAL_ENGINE_ODBC };
};

struct odbc_v4 { };



class OdbcConnection;
/*
  template<>
  struct db_traits<DAL::Odbc, odbc_v4>
  {
  typedef Environment<DAL::Odbc, odbc_v4>      environment_type;
  typedef OdbcConnection                         connection_type;
  typedef int                                      statement_type;
  typedef Result<DAL::Odbc, odbc_v4>           resultset_type;
  typedef CachedResult<DAL::Odbc, odbc_v4>     cached_resultset_type;
  typedef int                                      variant_type;
  typedef DAL::Odbc::RESULT                      dal_resultset_type;

  enum { DB_SYSTEM_ID = 0 };
  };


  class OdbcConnection : public Connection<DAL::Odbc, odbc_v4, DAL::OdbcDbc>
  { };
*/


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
