//
// driver_libsqlite.hh - libsqlite wrapper
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
/// @brief libsqlite wrapper
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DBWTL_DAL_SQLITE3DRIVER_HH
#define INFORMAVE_DBWTL_DAL_SQLITE3DRIVER_HH

#include "../library_support.hh"
#include "dbwtl/dal/dal_interface.hh"

#include <memory>

#include <sqlite3.h>


DAL_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
///
/// @brief SQLite 3 Driver
class SQLite3Drv : public DynamicLibrary
{

    // Function: sqlite3_step
    typedef int (*sqlite3api__sqlite3_step) 
    (sqlite3_stmt*);

    // Function: sqlite3_close
    typedef int (*sqlite3api__sqlite3_close) 
    (sqlite3 *);

    // Function: sqlite3_prepare_v2
    typedef int (*sqlite3api__sqlite3_prepare_v2)  
    (sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);

    // Function: sqlite3_finalize
    typedef int (*sqlite3api__sqlite3_finalize)
    (sqlite3_stmt *pStmt);

    // Function: sqlite3_open_v2
    typedef int (*sqlite3api__sqlite3_open_v2)
    (const char *filename, sqlite3 **ppDb, int flags, const char *zVfs);

    // Function: sqlite3_libversion
    typedef const char* (*sqlite3api__sqlite3_libversion)
        (void);

    // Function: sqlite3_aggregate_context
    typedef void* (*sqlite3api__sqlite3_aggregate_context) 
        (sqlite3_context*, int nBytes);

    // Function: sqlite3_auto_extensions
    typedef int (*sqlite3api__sqlite3_auto_extension)
    (void (*xEntryPoint)(void));

    // Function sqlite3_bind_blob
    typedef int (*sqlite3api__sqlite3_bind_blob) 
    (sqlite3_stmt*, int, const void*, int n, void(*)(void*));

    // Function: sqlite3_bind_double
    typedef int (*sqlite3api__sqlite3_bind_double)
    (sqlite3_stmt*, int, double);

    // Function: sqlite3_bind_int
    typedef int (*sqlite3api__sqlite3_bind_int)
    (sqlite3_stmt*, int, int);

    // Function: sqlite3_bind_int64
    typedef int (*sqlite3api__sqlite3_bind_int64)
    (sqlite3_stmt*, int, sqlite3_int64);

    // Function: sqlite3_bind_null
    typedef int (*sqlite3api__sqlite3_bind_null)
    (sqlite3_stmt*, int);

    // Function: sqlite3_bind_text
    typedef int (*sqlite3api__sqlite3_bind_text)
    (sqlite3_stmt*, int, const char*, int n, void(*)(void*));

    // Function: sqlite3_bind_value
    typedef int (*sqlite3api__sqlite3_bind_value)
    (sqlite3_stmt*, int, const sqlite3_value*);

    // Function: sqlite3_bind_zeroblob
    typedef int (*sqlite3api__sqlite3_bind_zeroblob)
    (sqlite3_stmt*, int, int n);

    // Function: sqlite3_bind_parameter_count
    typedef int (*sqlite3api__sqlite3_bind_parameter_count)
    (sqlite3_stmt*);

    // Function: sqlite3_bind_parameter_index
    typedef int (*sqlite3api__sqlite3_bind_parameter_index)
    (sqlite3_stmt*, const char *zName);

    // Function: sqlite3_bind_parameter_name
    typedef const char* (*sqlite3api__sqlite3_bind_parameter_name)
        (sqlite3_stmt*, int);

    // Function: sqlite3_blob_bytes
    typedef int (*sqlite3api__sqlite3_blob_bytes)
    (sqlite3_blob *);

    // Function: sqlite3_blob_close
    typedef int (*sqlite3api__sqlite3_blob_close)
    (sqlite3_blob *);

    // Function: sqlite3_blob_open
    typedef int (*sqlite3api__sqlite3_blob_open)       
    (sqlite3*, const char *zDb, const char *zTable, const char *zColumn, sqlite3_int64 iRow,
     int flags, sqlite3_blob **ppBlob);

    // Function: sqlite3_blob_read
    typedef int (*sqlite3api__sqlite3_blob_read) 
    (sqlite3_blob *, void *Z, int N, int iOffset);

    // Function: sqlite3_blob_write
    typedef int (*sqlite3api__sqlite3_blob_write)       
    (sqlite3_blob *, const void *z, int n, int iOffset);

    // Function: sqlite3_busy_handler
    typedef int (*sqlite3api__sqlite3_busy_handler)     
    (sqlite3*, int(*)(void*,int), void*);

    // Function: sqlite3_busy_timeout
    typedef int (*sqlite3api__sqlite3_busy_timeout)
    (sqlite3*, int ms);

    // Function: sqlite3_changes
    typedef int (*sqlite3api__sqlite3_changes)
    (sqlite3*);

    // Function: sqlite3_clear_bindings
    typedef int (*sqlite3api__sqlite3_clear_bindings)
    (sqlite3_stmt*);

    // Function: sqlite3_collation_needed
    typedef int (*sqlite3api__sqlite3_collation_needed)
    (sqlite3*, void*, void(*)(void*,sqlite3*,int eTextRep,const char*));

    // Function: sqlite3_column_blob
    typedef const void* (*sqlite3api__sqlite3_column_blob)
        (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_bytes
    typedef int (*sqlite3api__sqlite3_column_bytes)
    (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_double
    typedef double (*sqlite3api__sqlite3_column_double)
    (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_int
    typedef int (*sqlite3api__sqlite3_column_int)
    (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_int64
    typedef sqlite3_int64 (*sqlite3api__sqlite3_column_int64)
    (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_text
    typedef const unsigned char* (*sqlite3api__sqlite3_column_text) 
        (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_text
    typedef const void* (*sqlite3api__sqlite3_column_text16) 
        (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_type
    typedef int (*sqlite3api__sqlite3_column_type)
    (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_value
    typedef sqlite3_value* (*sqlite3api__sqlite3_column_value)
        (sqlite3_stmt*, int iCol);

    // Function: sqlite3_column_count
    typedef int (*sqlite3api__sqlite3_column_count) 
    (sqlite3_stmt *pStmt);

    // Function: sqlite3_column_name
    typedef const char* (*sqlite3api__sqlite3_column_name)
        (sqlite3_stmt *pStmt, int iCol);

    // Function: sqlite3_column_database_name
    typedef const char* (*sqlite3api__sqlite3_column_database_name)   
        (sqlite3_stmt*,int);

    // Function: sqlite3_column_table_name
    typedef const char* (*sqlite3api__sqlite3_column_table_name)       
        (sqlite3_stmt*,int);

    // Function: sqlite3_column_origin_name
    typedef const char* (*sqlite3api__sqlite3_column_origin_name)
        (sqlite3_stmt*,int);

    // Function: sqlite3_column_decltype
    typedef const char* (*sqlite3api__sqlite3_column_decltype) 
        (sqlite3_stmt*,int);

    // Function: sqlite3_commit_hook
    typedef void* (*sqlite3api__sqlite3_commit_hook) 
        (sqlite3*, int(*)(void*), void*);

    // Function: sqlite3_rollback_hook
    typedef void* (*sqlite3api__sqlite3_rollback_hook)
        (sqlite3*, void(*)(void *), void*);

    // Function: sqlite3_complete
    typedef int (*sqlite3api__sqlite3_complete)
    (const char *sql);

    // Function: sqlite3_create_collation
    typedef int (*sqlite3api__sqlite3_create_collation) 
    (sqlite3*, const char *zName, int eTextRep, void*,
     int(*xCompare)(void*,int,const void*,int,const void*));

    // Function: sqlite3_create_collation_v2
    typedef int (*sqlite3api__sqlite3_create_collation_v2)
    (sqlite3*, const char *zName, int eTextRep, void*,
     int(*xCompare)(void*,int,const void*,int,const void*),
     void(*xDestroy)(void*));
                
    // Function: sqlite3_create_function
    typedef int (*sqlite3api__sqlite3_create_function)
    (sqlite3 *db, const char *zFunctionName, int nArg, int eTextRep, void *pApp,
     void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
     void (*xStep)(sqlite3_context*,int,sqlite3_value**),
     void (*xFinal)(sqlite3_context*));

    // Function: sqlite3_data_count
    typedef int (*sqlite3api__sqlite3_data_count)
    (sqlite3_stmt *pStmt);

    // Function: sqlite3_db_handle
    typedef sqlite3* (*sqlite3api__sqlite3_db_handle)
        (sqlite3_stmt*);

    // Function: sqlite3_errcode
    typedef int (*sqlite3api__sqlite3_errcode)
    (sqlite3 *db);

    // Function: sqlite3_extended_errcode
    typedef int (*sqlite3api__sqlite3_extended_errcode)
    (sqlite3 *db);

    // Function: sqlite3_errmsg
    typedef const char* (*sqlite3api__sqlite3_errmsg)
        (sqlite3*);

    // Function: sqlite3_extended_result_codes
    typedef int (*sqlite3api__sqlite3_extended_result_codes)
    (sqlite3*, int onoff);

    // Function: sqlite3_get_autocommit
    typedef int (*sqlite3api__sqlite3_get_autocommit) 
    (sqlite3*);

    // Function: sqlite3_interrupt
    typedef void (*sqlite3api__sqlite3_interrupt)
    (sqlite3*);

    // Function: sqlite3_last_insert_rowid
    typedef sqlite3_int64 (*sqlite3api__sqlite3_last_insert_rowid)
    (sqlite3*);

    // Function: sqlite3_limit
    typedef int (*sqlite3api__sqlite3_limit)
    (sqlite3*, int id, int newVal);

    // Function: sqlite3_next_stmt
    typedef sqlite3_stmt* (*sqlite3api__sqlite3_next_stmt) 
        (sqlite3 *pDb, sqlite3_stmt *pStmt);

    // Function: sqlite3_progress_handler
    typedef void (*sqlite3api__sqlite3_progress_handler)
    (sqlite3*, int, int(*)(void*), void*);

    // Function: sqlite3_reset
    typedef int (*sqlite3api__sqlite3_reset)
    (sqlite3_stmt *pStmt);

    // Function: sqlite3_sql
    typedef const char* (*sqlite3api__sqlite3_sql)
        (sqlite3_stmt *pStmt);

    // Function: sqlite3_table_column_metadata
    typedef int (*sqlite3api__sqlite3_table_column_metadata)  
    (sqlite3 *db, const char *zDbName, const char *zTableName, const char *zColumnName, 
     char const **pzDataType, char const **pzCollSeq, int *pNotNull,           
     int *pPrimaryKey, int *pAutoinc);

    // Function: sqlite3_threadsafe
    typedef int (*sqlite3api__sqlite3_threadsafe)
    (void);

    // Function: sqlite3_total_changes
    typedef int (*sqlite3api__sqlite3_total_changes)
    (sqlite3*);

    // Function: sqlite3_update_hook
    typedef void* (*sqlite3api__sqlite3_update_hook) 
        (sqlite3*, void(*)(void *,int ,char const *,char const *,sqlite3_int64), void*);



protected:
    sqlite3api__sqlite3_open_v2                  m_func_sqlite3_open_v2;
    sqlite3api__sqlite3_libversion               m_func_sqlite3_libversion;
    sqlite3api__sqlite3_step                     m_func_sqlite3_step;
    sqlite3api__sqlite3_close                    m_func_sqlite3_close;
    sqlite3api__sqlite3_prepare_v2               m_func_sqlite3_prepare_v2;
    sqlite3api__sqlite3_finalize                 m_func_sqlite3_finalize;
    sqlite3api__sqlite3_aggregate_context        m_func_sqlite3_aggregate_context;
    sqlite3api__sqlite3_auto_extension           m_func_sqlite3_auto_extension;
    sqlite3api__sqlite3_bind_blob                m_func_sqlite3_bind_blob;
    sqlite3api__sqlite3_bind_double              m_func_sqlite3_bind_double;
    sqlite3api__sqlite3_bind_int                 m_func_sqlite3_bind_int;
    sqlite3api__sqlite3_bind_int64               m_func_sqlite3_bind_int64;
    sqlite3api__sqlite3_bind_null                m_func_sqlite3_bind_null;
    sqlite3api__sqlite3_bind_text                m_func_sqlite3_bind_text;
    sqlite3api__sqlite3_bind_value               m_func_sqlite3_bind_value;
    sqlite3api__sqlite3_bind_zeroblob            m_func_sqlite3_bind_zeroblob;
    sqlite3api__sqlite3_bind_parameter_count     m_func_sqlite3_bind_parameter_count;
    sqlite3api__sqlite3_bind_parameter_index     m_func_sqlite3_bind_parameter_index;
    sqlite3api__sqlite3_bind_parameter_name      m_func_sqlite3_bind_parameter_name;
    sqlite3api__sqlite3_blob_bytes               m_func_sqlite3_blob_bytes;
    sqlite3api__sqlite3_blob_close               m_func_sqlite3_blob_close;
    sqlite3api__sqlite3_blob_open                m_func_sqlite3_blob_open;
    sqlite3api__sqlite3_blob_read                m_func_sqlite3_blob_read;
    sqlite3api__sqlite3_blob_write               m_func_sqlite3_blob_write;
    sqlite3api__sqlite3_busy_handler             m_func_sqlite3_busy_handler;
    sqlite3api__sqlite3_busy_timeout             m_func_sqlite3_busy_timeout;
    sqlite3api__sqlite3_changes                  m_func_sqlite3_changes;
    sqlite3api__sqlite3_clear_bindings           m_func_sqlite3_clear_bindings;
    sqlite3api__sqlite3_collation_needed         m_func_sqlite3_collation_needed;
    sqlite3api__sqlite3_column_blob              m_func_sqlite3_column_blob;
    sqlite3api__sqlite3_column_bytes             m_func_sqlite3_column_bytes;
    sqlite3api__sqlite3_column_double            m_func_sqlite3_column_double;
    sqlite3api__sqlite3_column_int               m_func_sqlite3_column_int;
    sqlite3api__sqlite3_column_int64             m_func_sqlite3_column_int64;
    sqlite3api__sqlite3_column_text              m_func_sqlite3_column_text;
    sqlite3api__sqlite3_column_text16            m_func_sqlite3_column_text16;
    sqlite3api__sqlite3_column_type              m_func_sqlite3_column_type;
    sqlite3api__sqlite3_column_value             m_func_sqlite3_column_value;
    sqlite3api__sqlite3_column_count             m_func_sqlite3_column_count;
    sqlite3api__sqlite3_column_name              m_func_sqlite3_column_name;
    sqlite3api__sqlite3_column_database_name     m_func_sqlite3_column_database_name;
    sqlite3api__sqlite3_column_table_name        m_func_sqlite3_column_table_name;
    sqlite3api__sqlite3_column_origin_name       m_func_sqlite3_column_origin_name;
    sqlite3api__sqlite3_column_decltype          m_func_sqlite3_column_decltype;
    sqlite3api__sqlite3_commit_hook              m_func_sqlite3_commit_hook;
    sqlite3api__sqlite3_rollback_hook            m_func_sqlite3_rollback_hook;
    sqlite3api__sqlite3_complete                 m_func_sqlite3_complete;
    sqlite3api__sqlite3_create_collation         m_func_sqlite3_create_collation;
    sqlite3api__sqlite3_create_collation_v2      m_func_sqlite3_create_collation_v2;
    sqlite3api__sqlite3_create_function          m_func_sqlite3_create_function;
    sqlite3api__sqlite3_data_count               m_func_sqlite3_data_count;
    sqlite3api__sqlite3_db_handle                m_func_sqlite3_db_handle;
    sqlite3api__sqlite3_errcode                  m_func_sqlite3_errcode;
    sqlite3api__sqlite3_extended_errcode         m_func_sqlite3_extended_errcode;
    sqlite3api__sqlite3_errmsg                   m_func_sqlite3_errmsg;
    sqlite3api__sqlite3_extended_result_codes    m_func_sqlite3_extended_result_codes;
    sqlite3api__sqlite3_get_autocommit           m_func_sqlite3_get_autocommit;
    sqlite3api__sqlite3_interrupt                m_func_sqlite3_interrupt;
    sqlite3api__sqlite3_last_insert_rowid        m_func_sqlite3_last_insert_rowid;
    sqlite3api__sqlite3_limit                    m_func_sqlite3_limit;
    sqlite3api__sqlite3_next_stmt                m_func_sqlite3_next_stmt;
    sqlite3api__sqlite3_progress_handler         m_func_sqlite3_progress_handler;
    sqlite3api__sqlite3_reset                    m_func_sqlite3_reset;
    sqlite3api__sqlite3_sql                      m_func_sqlite3_sql;
    sqlite3api__sqlite3_table_column_metadata    m_func_sqlite3_table_column_metadata;
    sqlite3api__sqlite3_threadsafe               m_func_sqlite3_threadsafe;
    sqlite3api__sqlite3_total_changes            m_func_sqlite3_total_changes;
    sqlite3api__sqlite3_update_hook              m_func_sqlite3_update_hook;
                


public:
    SQLite3Drv(void) : DynamicLibrary("/usr/lib/libsqlite3.so"),
                       m_func_sqlite3_open_v2(0),
                       m_func_sqlite3_libversion(0),
                       m_func_sqlite3_step(0),
                       m_func_sqlite3_close(0),
                       m_func_sqlite3_prepare_v2(0),
                       m_func_sqlite3_finalize(0),
                       m_func_sqlite3_aggregate_context(0),
                       m_func_sqlite3_auto_extension(0),
                       m_func_sqlite3_bind_blob(0),
                       m_func_sqlite3_bind_double(0),
                       m_func_sqlite3_bind_int(0),
                       m_func_sqlite3_bind_int64(0),
                       m_func_sqlite3_bind_null(0),
                       m_func_sqlite3_bind_text(0),
                       m_func_sqlite3_bind_value(0),
                       m_func_sqlite3_bind_zeroblob(0),
                       m_func_sqlite3_bind_parameter_count(0),
                       m_func_sqlite3_bind_parameter_index(0),
                       m_func_sqlite3_bind_parameter_name(0),
                       m_func_sqlite3_blob_bytes(0),
                       m_func_sqlite3_blob_close(0),
                       m_func_sqlite3_blob_open(0),
                       m_func_sqlite3_blob_read(0),
                       m_func_sqlite3_blob_write(0),
                       m_func_sqlite3_busy_handler(0),
                       m_func_sqlite3_busy_timeout(0),
                       m_func_sqlite3_changes(0),
                       m_func_sqlite3_clear_bindings(0),
                       m_func_sqlite3_collation_needed(0),
                       m_func_sqlite3_column_blob(0),
                       m_func_sqlite3_column_bytes(0),
                       m_func_sqlite3_column_double(0),
                       m_func_sqlite3_column_int(0),
                       m_func_sqlite3_column_int64(0),
                       m_func_sqlite3_column_text(0),
                       m_func_sqlite3_column_text16(0),
                       m_func_sqlite3_column_type(0),
                       m_func_sqlite3_column_value(0),
                       m_func_sqlite3_column_count(0),
                       m_func_sqlite3_column_name(0),
                       m_func_sqlite3_column_database_name(0),
                       m_func_sqlite3_column_table_name(0),
                       m_func_sqlite3_column_origin_name(0),
                       m_func_sqlite3_column_decltype(0),
                       m_func_sqlite3_commit_hook(0),
                       m_func_sqlite3_rollback_hook(0),
                       m_func_sqlite3_complete(0),
                       m_func_sqlite3_create_collation(0),
                       m_func_sqlite3_create_collation_v2(0),
                       m_func_sqlite3_create_function(0),
                       m_func_sqlite3_data_count(0),
                       m_func_sqlite3_db_handle(0),
                       m_func_sqlite3_errcode(0),
                       m_func_sqlite3_extended_errcode(0),
                       m_func_sqlite3_errmsg(0),
                       m_func_sqlite3_extended_result_codes(0),
                       m_func_sqlite3_get_autocommit(0),
                       m_func_sqlite3_interrupt(0),
                       m_func_sqlite3_last_insert_rowid(0),
                       m_func_sqlite3_limit(0),
                       m_func_sqlite3_next_stmt(0),
                       m_func_sqlite3_progress_handler(0),
                       m_func_sqlite3_reset(0),
                       m_func_sqlite3_sql(0),
                       m_func_sqlite3_table_column_metadata(0),
                       m_func_sqlite3_threadsafe(0),
                       m_func_sqlite3_total_changes(0),
                       m_func_sqlite3_update_hook(0)
        {
            this->getproc(this->m_func_sqlite3_step, "sqlite3_step");
            this->getproc(this->m_func_sqlite3_libversion, "sqlite3_libversion");
            this->getproc(this->m_func_sqlite3_open_v2, "sqlite3_open_v2");
            this->getproc(this->m_func_sqlite3_close, "sqlite3_close");
            this->getproc(this->m_func_sqlite3_prepare_v2, "sqlite3_prepare_v2");
            this->getproc(this->m_func_sqlite3_finalize, "sqlite3_finalize");
            this->getproc(this->m_func_sqlite3_aggregate_context, "sqlite3_aggregate_context");
            this->getproc(this->m_func_sqlite3_auto_extension, "sqlite3_auto_extension");
            this->getproc(this->m_func_sqlite3_bind_blob, "sqlite3_bind_blob");
            this->getproc(this->m_func_sqlite3_bind_double, "sqlite3_bind_double");
            this->getproc(this->m_func_sqlite3_bind_int, "sqlite3_bind_int");
            this->getproc(this->m_func_sqlite3_bind_int64, "sqlite3_bind_int64");
            this->getproc(this->m_func_sqlite3_bind_null, "sqlite3_bind_null");
            this->getproc(this->m_func_sqlite3_bind_text, "sqlite3_bind_text");
            this->getproc(this->m_func_sqlite3_bind_value, "sqlite3_bind_value");
            this->getproc(this->m_func_sqlite3_bind_zeroblob, "sqlite3_bind_zeroblob");
            this->getproc(this->m_func_sqlite3_bind_parameter_count, "sqlite3_bind_parameter_count");
            this->getproc(this->m_func_sqlite3_bind_parameter_index, "sqlite3_bind_parameter_index");
            this->getproc(this->m_func_sqlite3_bind_parameter_name, "sqlite3_bind_parameter_name");
            this->getproc(this->m_func_sqlite3_blob_bytes, "sqlite3_blob_bytes");
            this->getproc(this->m_func_sqlite3_blob_close, "sqlite3_blob_close");
            this->getproc(this->m_func_sqlite3_blob_open, "sqlite3_blob_open");
            this->getproc(this->m_func_sqlite3_blob_read, "sqlite3_blob_read");
            this->getproc(this->m_func_sqlite3_blob_write, "sqlite3_blob_write");
            this->getproc(this->m_func_sqlite3_busy_handler, "sqlite3_busy_handler");
            this->getproc(this->m_func_sqlite3_busy_timeout, "sqlite3_busy_timeout");
            this->getproc(this->m_func_sqlite3_changes, "sqlite3_changes");
            this->getproc(this->m_func_sqlite3_clear_bindings, "sqlite3_clear_bindings");
            this->getproc(this->m_func_sqlite3_collation_needed, "sqlite3_collation_needed");
            this->getproc(this->m_func_sqlite3_column_blob, "sqlite3_column_blob");
            this->getproc(this->m_func_sqlite3_column_bytes, "sqlite3_column_bytes");
            this->getproc(this->m_func_sqlite3_column_double, "sqlite3_column_double");
            this->getproc(this->m_func_sqlite3_column_int, "sqlite3_column_int");
            this->getproc(this->m_func_sqlite3_column_int64, "sqlite3_column_int64");
            this->getproc(this->m_func_sqlite3_column_text, "sqlite3_column_text");
            this->getproc(this->m_func_sqlite3_column_text16, "sqlite3_column_text16");
            this->getproc(this->m_func_sqlite3_column_type, "sqlite3_column_type");
            this->getproc(this->m_func_sqlite3_column_value, "sqlite3_column_value");
            this->getproc(this->m_func_sqlite3_column_count, "sqlite3_column_count");
            this->getproc(this->m_func_sqlite3_column_name, "sqlite3_column_name");
            this->getproc(this->m_func_sqlite3_column_database_name, "sqlite3_column_database_name");
            this->getproc(this->m_func_sqlite3_column_table_name, "sqlite3_column_table_name");
            this->getproc(this->m_func_sqlite3_column_origin_name, "sqlite3_column_origin_name");
            this->getproc(this->m_func_sqlite3_column_decltype, "sqlite3_column_decltype");
            this->getproc(this->m_func_sqlite3_commit_hook, "sqlite3_commit_hook");
            this->getproc(this->m_func_sqlite3_rollback_hook, "sqlite3_rollback_hook");
            this->getproc(this->m_func_sqlite3_complete, "sqlite3_complete");
            this->getproc(this->m_func_sqlite3_create_collation, "sqlite3_create_collation");
            this->getproc(this->m_func_sqlite3_create_collation_v2, "sqlite3_create_collation_v2");
            this->getproc(this->m_func_sqlite3_create_function, "sqlite3_create_function");
            this->getproc(this->m_func_sqlite3_data_count, "sqlite3_data_count");
            this->getproc(this->m_func_sqlite3_db_handle, "sqlite3_db_handle");
            this->getproc(this->m_func_sqlite3_errcode, "sqlite3_errcode");
            this->getproc(this->m_func_sqlite3_extended_errcode, "sqlite3_extended_errcode");
            this->getproc(this->m_func_sqlite3_errmsg, "sqlite3_errmsg");
            this->getproc(this->m_func_sqlite3_extended_result_codes, "sqlite3_extended_result_codes");
            this->getproc(this->m_func_sqlite3_get_autocommit, "sqlite3_get_autocommit");
            this->getproc(this->m_func_sqlite3_interrupt, "sqlite3_interrupt");
            this->getproc(this->m_func_sqlite3_last_insert_rowid, "sqlite3_last_insert_rowid");
            this->getproc(this->m_func_sqlite3_limit, "sqlite3_limit");
            this->getproc(this->m_func_sqlite3_next_stmt, "sqlite3_next_stmt");
            this->getproc(this->m_func_sqlite3_progress_handler, "sqlite3_progress_handler");
            this->getproc(this->m_func_sqlite3_reset, "sqlite3_reset");
            this->getproc(this->m_func_sqlite3_sql, "sqlite3_sql");
            this->getproc(this->m_func_sqlite3_table_column_metadata, "sqlite3_table_column_metadata");
            this->getproc(this->m_func_sqlite3_threadsafe, "sqlite3_threadsafe");
            this->getproc(this->m_func_sqlite3_total_changes, "sqlite3_total_changes");
            this->getproc(this->m_func_sqlite3_update_hook, "sqlite3_update_hook");

        }


    virtual ~SQLite3Drv(void) { }

    inline int sqlite3_open_v2(const char *filename, sqlite3 **ppDb, int flags, const char *zVfs)
        {
            if(this->m_func_sqlite3_open_v2)
                return this->m_func_sqlite3_open_v2(filename, ppDb, flags, zVfs);
            else
                throw ex::missing_function(__FUNCTION__);
        }



    inline int sqlite3_step(sqlite3_stmt* stmt)
        {
            if(this->m_func_sqlite3_step)
                return this->m_func_sqlite3_step(stmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline const char* sqlite3_libversion(void)
        {
            if(this->m_func_sqlite3_libversion)
                return this->m_func_sqlite3_libversion();
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline int sqlite3_close(::sqlite3* db)
        {
            if(this->m_func_sqlite3_close)
                return this->m_func_sqlite3_close(db);
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline int sqlite3_finalize(sqlite3_stmt *pStmt)
        {
            if(this->m_func_sqlite3_finalize)
                return this->m_func_sqlite3_finalize(pStmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte,       
                                  sqlite3_stmt **ppStmt, const char **pzTail)
        {
            if(this->m_func_sqlite3_prepare_v2)
                return this->m_func_sqlite3_prepare_v2(db, zSql, nByte, ppStmt, pzTail);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline void *sqlite3_aggregate_context(sqlite3_context* context, int nBytes)
        {
            if(this->m_func_sqlite3_aggregate_context)
                return this->m_func_sqlite3_aggregate_context(context, nBytes);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_auto_extension(void (*xEntryPoint)(void))
        {
            if(this->m_func_sqlite3_auto_extension)
                return this->m_func_sqlite3_auto_extension(xEntryPoint);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_blob(sqlite3_stmt* stmt, int col, const void* value, int n, void(*dtor)(void*))
        {
            if(this->m_func_sqlite3_bind_blob)
                return this->m_func_sqlite3_bind_blob(stmt, col, value, n, dtor);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_double(sqlite3_stmt* stmt, int col, double value)
        {
            if(this->m_func_sqlite3_bind_double)
                return this->m_func_sqlite3_bind_double(stmt, col, value);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_int(sqlite3_stmt* stmt, int col, int value)
        {
            if(this->m_func_sqlite3_bind_int)
                return this->m_func_sqlite3_bind_int(stmt, col, value);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_int64(sqlite3_stmt* stmt, int col, sqlite3_int64 value)
        {
            if(this->m_func_sqlite3_bind_int64)
                return this->m_func_sqlite3_bind_int64(stmt, col, value);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_null(sqlite3_stmt* stmt, int col)
        {
            if(this->m_func_sqlite3_bind_null)
                return this->m_func_sqlite3_bind_null(stmt, col);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_text(sqlite3_stmt* stmt, int col, const char* value, int n, void(*dtor)(void*))
        {
            if(this->m_func_sqlite3_bind_text)
                return this->m_func_sqlite3_bind_text(stmt, col, value, n, dtor);
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline int sqlite3_bind_value(sqlite3_stmt* stmt, int col, const sqlite3_value* value)
        {
            if(this->m_func_sqlite3_bind_value)
                return this->m_func_sqlite3_bind_value(stmt, col, value);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_zeroblob(sqlite3_stmt* stmt, int col, int n)
        {
            if(this->m_func_sqlite3_bind_zeroblob)
                return this->m_func_sqlite3_bind_zeroblob(stmt, col, n);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_parameter_count(sqlite3_stmt* stmt)
        {
            if(this->m_func_sqlite3_bind_parameter_count)
                return this->m_func_sqlite3_bind_parameter_count(stmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_bind_parameter_index(sqlite3_stmt* stmt, const char *zName)
        {
            if(this->m_func_sqlite3_bind_parameter_index)
                return this->m_func_sqlite3_bind_parameter_index(stmt, zName);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline const char *sqlite3_bind_parameter_name(sqlite3_stmt* stmt, int col)
        {
            if(this->m_func_sqlite3_bind_parameter_name)
                return this->m_func_sqlite3_bind_parameter_name(stmt, col);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_blob_bytes(sqlite3_blob *blob)
        {
            if(this->m_func_sqlite3_blob_bytes)
                return this->m_func_sqlite3_blob_bytes(blob);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_blob_close(sqlite3_blob *blob)
        {
            if(this->m_func_sqlite3_blob_close)
                return this->m_func_sqlite3_blob_close(blob);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_blob_open(sqlite3* db, const char *zDb, const char *zTable, const char *zColumn,
                                 sqlite3_int64 iRow, int flags, sqlite3_blob **ppBlob)
        {
            if(this->m_func_sqlite3_blob_open)
                return this->m_func_sqlite3_blob_open(db, zDb, zTable, zColumn, iRow, flags, ppBlob);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_blob_read(sqlite3_blob *blob, void *Z, int N, int iOffset)
        {
            if(this->m_func_sqlite3_blob_read)
                return this->m_func_sqlite3_blob_read(blob, Z, N, iOffset);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_blob_write(sqlite3_blob *blob, const void *z, int n, int iOffset)
        {
            if(this->m_func_sqlite3_blob_write)
                return this->m_func_sqlite3_blob_write(blob, z, n, iOffset);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_busy_handler(sqlite3 *db, int(*handler)(void*,int), void *arg)
        {
            if(this->m_func_sqlite3_busy_handler)
                return this->m_func_sqlite3_busy_handler(db, handler, arg);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_busy_timeout(sqlite3* db, int ms)
        {
            if(this->m_func_sqlite3_busy_timeout)
                return this->m_func_sqlite3_busy_timeout(db, ms);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_changes(sqlite3* db)
        {
            if(this->m_func_sqlite3_changes)
                return this->m_func_sqlite3_changes(db);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_clear_bindings(sqlite3_stmt* stmt)
        {
            if(this->m_func_sqlite3_clear_bindings)
                return this->m_func_sqlite3_clear_bindings(stmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_collation_needed(sqlite3* db, void* a, 
                                        void(*b)(void*,sqlite3*,int eTextRep,const char*))
        {
            if(this->m_func_sqlite3_collation_needed)
                return this->m_func_sqlite3_collation_needed(db, a, b);
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline const void *sqlite3_column_blob(sqlite3_stmt* stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_blob)
                return this->m_func_sqlite3_column_blob(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_column_bytes(sqlite3_stmt* stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_bytes)
                return this->m_func_sqlite3_column_bytes(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline double sqlite3_column_double(sqlite3_stmt* stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_double)
                return this->m_func_sqlite3_column_double(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_column_int(sqlite3_stmt* stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_int)
                return this->m_func_sqlite3_column_int(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline sqlite3_int64 sqlite3_column_int64(sqlite3_stmt* stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_int64)
                return this->m_func_sqlite3_column_int64(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline const unsigned char *sqlite3_column_text(sqlite3_stmt* stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_text)
                return this->m_func_sqlite3_column_text(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline const void *sqlite3_column_text16(sqlite3_stmt* stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_text16)
                return this->m_func_sqlite3_column_text16(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_column_type(sqlite3_stmt *stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_type)
                return this->m_func_sqlite3_column_type(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline sqlite3_value *sqlite3_column_value(sqlite3_stmt *stmt, int iCol)
        {
            if(this->m_func_sqlite3_column_value)
                return this->m_func_sqlite3_column_value(stmt, iCol);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_column_count(sqlite3_stmt *pStmt)
        {
            if(this->m_func_sqlite3_column_count)
                return this->m_func_sqlite3_column_count(pStmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline const char *sqlite3_column_name(sqlite3_stmt *stmt, int N)
        {
            if(this->m_func_sqlite3_column_name)
                return this->m_func_sqlite3_column_name(stmt, N);
            else
                throw ex::missing_function(__FUNCTION__);
        }

/*
  inline const char *sqlite3_column_database_name(sqlite3_stmt *stmt,int col)
  {
  if(this->m_func_sqlite3_database_name)
  return this->m_func_sqlite3_column_database_name(stmt, col);
  else
  throw ex::missing_function(__FUNCTION__);
  }
*/

    inline const char *sqlite3_column_table_name(sqlite3_stmt *stmt,int col)
        {
            if(this->m_func_sqlite3_column_table_name)
                return this->m_func_sqlite3_column_table_name(stmt, col);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline const char *sqlite3_column_origin_name(sqlite3_stmt* stmt,int col)
        {
            if(this->m_func_sqlite3_column_origin_name)
                return this->m_func_sqlite3_column_origin_name(stmt, col);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline const char *sqlite3_column_decltype(sqlite3_stmt *stmt, int col)
        {
            if(this->m_func_sqlite3_column_decltype)
                return this->m_func_sqlite3_column_decltype(stmt, col);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline void *sqlite3_commit_hook(sqlite3*db, int(*hook)(void*), void*arg)
        {
            if(this->m_func_sqlite3_commit_hook)
                return this->m_func_sqlite3_commit_hook(db, hook, arg);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline void *sqlite3_rollback_hook(sqlite3*db, void(*hook)(void *), void*arg)
        {
            if(this->m_func_sqlite3_rollback_hook)
                return this->m_func_sqlite3_rollback_hook(db, hook, arg);
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline int sqlite3_complete(const char *sql)
        {
            if(this->m_func_sqlite3_complete)
                return this->m_func_sqlite3_complete(sql);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_create_collation( sqlite3*db, const char *zName, int eTextRep, 
                                         void* a, int(*xCompare)(void*,int,const void*,int,const void*))
        {
            if(this->m_func_sqlite3_create_collation)
                return this->m_func_sqlite3_create_collation(db, zName, eTextRep, a, xCompare);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_create_collation_v2(sqlite3*db, const char *zName, int eTextRep, void* a,
                                           int(*xCompare)(void*,int,const void*,int,const void*),
                                           void(*xDestroy)(void*))
        {
            if(this->m_func_sqlite3_create_collation_v2)
                return this->m_func_sqlite3_create_collation_v2(db, zName, eTextRep, a, xCompare, xDestroy);
            else
                throw ex::missing_function(__FUNCTION__);
        }

                
    inline int sqlite3_create_function(sqlite3 *db, const char *zFunctionName, int nArg,
                                       int eTextRep, void *pApp,
                                       void (*xFunc)(sqlite3_context*,int,sqlite3_value**),
                                       void (*xStep)(sqlite3_context*,int,sqlite3_value**),
                                       void (*xFinal)(sqlite3_context*))
        {
            if(this->m_func_sqlite3_create_function)
                return this->m_func_sqlite3_create_function(db, zFunctionName, nArg, eTextRep, pApp,
                                                            xFunc, xStep, xFinal);
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline int sqlite3_data_count(sqlite3_stmt *pStmt)
        {
            if(this->m_func_sqlite3_data_count)
                return this->m_func_sqlite3_data_count(pStmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline sqlite3 *sqlite3_db_handle(sqlite3_stmt* stmt)
        {
            if(this->m_func_sqlite3_db_handle)
                return this->m_func_sqlite3_db_handle(stmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline int sqlite3_errcode(sqlite3 *db)
        {
            if(this->m_func_sqlite3_errcode)
                return this->m_func_sqlite3_errcode(db);
            else
                throw ex::missing_function(__FUNCTION__);
        }

/*
  inline int sqlite3_extended_errcode(sqlite3 *db)
  {
  if(this->m_func_sqlite3_extended_errcode)
  return this->m_func_sqlite3_extended_errcode(db);
  else
  throw ex::missing_function(__FUNCTION__);
  }
*/

    inline const char *sqlite3_errmsg(sqlite3* db)
        {
            if(this->m_func_sqlite3_errmsg)
                return this->m_func_sqlite3_errmsg(db);
            else
                throw ex::missing_function(__FUNCTION__);
        }

/*
  inline int sqlite3_extended_result_codes(sqlite3*db, int onoff)
  {
  if(this->m_func_sqlite3_extended_result_code)
  return this->m_func_sqlite3_extended_result_code(db, onoff);
  else
  throw ex::missing_function(__FUNCTION__);
  }
*/

    inline int sqlite3_get_autocommit(sqlite3* db)
        {
            if(this->m_func_sqlite3_get_autocommit)
                return this->m_func_sqlite3_get_autocommit(db);
            else
                throw ex::missing_function(__FUNCTION__);
        }


    inline void sqlite3_interrupt(sqlite3*db)
        {
            if(this->m_func_sqlite3_interrupt)
                return this->m_func_sqlite3_interrupt(db);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline sqlite3_int64 sqlite3_last_insert_rowid(sqlite3*db)
        {
            if(this->m_func_sqlite3_last_insert_rowid)
                return this->m_func_sqlite3_last_insert_rowid(db);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_limit(sqlite3*db, int id, int newVal)
        {
            if(this->m_func_sqlite3_limit)
                return this->m_func_sqlite3_limit(db, id, newVal);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline sqlite3_stmt *sqlite3_next_stmt(sqlite3 *pDb, sqlite3_stmt *pStmt)
        {
            if(this->m_func_sqlite3_next_stmt)
                return this->m_func_sqlite3_next_stmt(pDb, pStmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline void sqlite3_progress_handler(sqlite3* db, int a, int(*handler)(void*), void*arg)
        {
            if(this->m_func_sqlite3_progress_handler)
                return this->m_func_sqlite3_progress_handler(db, a, handler, arg);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_reset(sqlite3_stmt *pStmt)
        {
            if(this->m_func_sqlite3_reset)
                return this->m_func_sqlite3_reset(pStmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline const char *sqlite3_sql(sqlite3_stmt *pStmt)
        {
            if(this->m_func_sqlite3_sql)
                return this->m_func_sqlite3_sql(pStmt);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_table_column_metadata(sqlite3 *db, const char *zDbName, const char *zTableName,   
                                             const char *zColumnName, char const **pzDataType,  
                                             char const **pzCollSeq, int *pNotNull,            
                                             int *pPrimaryKey, int *pAutoinc)
        {
            if(this->m_func_sqlite3_table_column_metadata)
                return this->m_func_sqlite3_table_column_metadata(db, zDbName, zTableName, zColumnName, pzDataType,
                                                                  pzCollSeq, pNotNull, pPrimaryKey, pAutoinc);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_threadsafe(void)
        {
            if(this->m_func_sqlite3_threadsafe)
                return this->m_func_sqlite3_threadsafe();
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline int sqlite3_total_changes(sqlite3*db)
        {
            if(this->m_func_sqlite3_total_changes)
                return this->m_func_sqlite3_total_changes(db);
            else
                throw ex::missing_function(__FUNCTION__);
        }

    inline void *sqlite3_update_hook(sqlite3*db, 
                                     void(*a)(void *,int ,char const *,char const *,sqlite3_int64),
                                     void*arg)
        {
            if(this->m_func_sqlite3_update_hook)
                return this->m_func_sqlite3_update_hook(db, a, arg);
            else
                throw ex::missing_function(__FUNCTION__);
        }
                
};





DAL_NAMESPACE_END

#endif


