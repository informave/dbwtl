#ifndef INFORMAVE_DBWTL_DAL_ODBC30DRIVER_HH
#define INFORMAVE_DBWTL_DAL_ODBC30DRIVER_HH

//#include "Sqlite3Exceptions.hh"
//#include "../LibrarySupport.hh"


#include "../library_support.hh"
#include "dbwtl/dal/dal_interface.hh"


//#include "../DALDriverFactory.hh"
//#include "dal/DALInterface.hh"

#include <memory>

#include <sqlite3.h>


#define ODBCVER 0x0300

#include <sql.h>
#include <sqlext.h>
#include <sqltypes.h>
#include <odbcinst.h>

#if defined(DBWTL_ON_WIN32)
#define DBWTL_ODBC_DEFAULT_LIB "odbc32.dll"
#elif defined(DBWTL_ON_UNIX)
#define DBWTL_ODBC_DEFAULT_LIB "libodbc.so"
#else
#error "Unsupported plattform"
#endif



DAL_NAMESPACE_BEGIN

/*
 * @brief SQLite 3 Driver
 */
class ODBC30Drv : public DynamicLibrary
{

    typedef SQLRETURN (SQL_API *odbcapi__SQLAllocHandle)(SQLSMALLINT HandleType,
                                                 SQLHANDLE InputHandle, SQLHANDLE *OutputHandle);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLBindCol)(SQLHSTMT StatementHandle, 
                                             SQLUSMALLINT ColumnNumber,
                                             SQLSMALLINT TargetType, 
                                             SQLPOINTER TargetValue,
                                             SQLLEN BufferLength, 
                                             SQLLEN *StrLen_or_Ind);

// Core[1]
    typedef SQLRETURN (SQL_API *odbcapi__SQLBindParameter)(SQLHSTMT hstmt,
                                                   SQLUSMALLINT      ipar,
                                                   SQLSMALLINT        fParamType,
                                                   SQLSMALLINT        fCType,
                                                   SQLSMALLINT        fSqlType,
                                                   SQLULEN            cbColDef,
                                                   SQLSMALLINT        ibScale,
                                                   SQLPOINTER         rgbValue,
                                                   SQLLEN             cbValueMax,
                                                   SQLLEN     	   *pcbValue);


// Level 1
/*
  SQLRETURN SQL_API SQLBrowseConnect(SQLHDBC     ConnectionHandle,
  SQLCHAR *     InConnectionString,
  SQLSMALLINT     StringLength1,
  SQLCHAR *     OutConnectionString,
  SQLSMALLINT     BufferLength,
  SQLSMALLINT *     StringLength2Ptr)
  {
  ShowMessage(__PRETTY_FUNCTION__);
  return SQL_SUCCESS;
  }
*/


// Level 1
/*
  SQLRETURN SQL_API SQLBulkOperations(SQLHSTMT     StatementHandle,
  SQLUSMALLINT     Operation)
  {
  ShowMessage(__PRETTY_FUNCTION__);
  return MPAPI_SQLBulkOperations(StatementHandle, Operation);
  }
*/

// Core[1]
    typedef SQLRETURN (SQL_API *odbcapi__SQLCancel)(SQLHSTMT StatementHandle);


// Core

    typedef SQLRETURN (SQL_API *odbcapi__SQLCloseCursor)(SQLHSTMT StatementHandle);


//extern "C"
//SQLRETURN  SQL_API SQLFoobar(SQLHSTMT StatementHandle)
//{
//	 ShowMessage(__PRETTY_FUNCTION__);
//	 return MPAPI_SQLCloseCursor(StatementHandle);
//}


// Core[1]
    typedef SQLRETURN (SQL_API *odbcapi__SQLColAttributeA)(
        SQLHSTMT     StatementHandle,
        SQLUSMALLINT     ColumnNumber,
        SQLUSMALLINT     FieldIdentifier,
        SQLPOINTER     CharacterAttributePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *     StringLengthPtr,
        SQLPOINTER     NumericAttributePtr);

    typedef SQLRETURN (SQL_API *odbcapi__SQLColAttributeW)(
        SQLHSTMT     StatementHandle,
        SQLUSMALLINT     ColumnNumber,
        SQLUSMALLINT     FieldIdentifier,
        SQLPOINTER     CharacterAttributePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *     StringLengthPtr,
        SQLPOINTER     NumericAttributePtr);

// Level 2
/*
  SQLRETURN SQL_API SQLColumnPrivileges(SQLHSTMT     StatementHandle,
  SQLCHAR *     CatalogName,
  SQLSMALLINT     NameLength1,
  SQLCHAR *     SchemaName,
  SQLSMALLINT     NameLength2,
  SQLCHAR *     TableName,
  SQLSMALLINT     NameLength3,
  SQLCHAR *     ColumnName,
  SQLSMALLINT     NameLength4)
  {
  ShowMessag(__PRETTY_FUNCTION__);
  return MPAPI_SQLColumnPrivileges(StatementHandle, CatalogName, SchemaName,
  TableName, ColumnName);
  }
*/


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLColumnsA)(SQLHSTMT     StatementHandle,
                                              SQLCHAR *     CatalogName,
                                              SQLSMALLINT     NameLength1,
                                              SQLCHAR *     SchemaName,
                                              SQLSMALLINT     NameLength2,
                                              SQLCHAR *     TableName,
                                              SQLSMALLINT     NameLength3,
                                              SQLCHAR *     ColumnName,
                                              SQLSMALLINT     NameLength4);

    typedef SQLRETURN (SQL_API *odbcapi__SQLColumnsW)(SQLHSTMT     StatementHandle,
                                              SQLWCHAR *     CatalogName,
                                              SQLSMALLINT     NameLength1,
                                              SQLWCHAR *     SchemaName,
                                              SQLSMALLINT     NameLength2,
                                              SQLWCHAR *     TableName,
                                              SQLSMALLINT     NameLength3,
                                              SQLWCHAR *     ColumnName,
                                              SQLSMALLINT     NameLength4);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLConnectA)(SQLHDBC     ConnectionHandle,
                                              SQLCHAR *     ServerName,
                                              SQLSMALLINT     NameLength1,
                                              SQLCHAR *     UserName,
                                              SQLSMALLINT     NameLength2,
                                              SQLCHAR *     Authentication,
                                              SQLSMALLINT     NameLength3);


    typedef SQLRETURN (SQL_API *odbcapi__SQLConnectW)(SQLHDBC     ConnectionHandle,
                                              SQLWCHAR *     ServerName,
                                              SQLSMALLINT     NameLength1,
                                              SQLWCHAR *     UserName,
                                              SQLSMALLINT     NameLength2,
                                              SQLWCHAR *     Authentication,
                                              SQLSMALLINT     NameLength3);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLCopyDesc)(SQLHDESC     SourceDescHandle,
                                              SQLHDESC     TargetDescHandle);

// Core[1]
    typedef SQLRETURN (SQL_API *odbcapi__SQLDescribeColA)(SQLHSTMT StatementHandle,
                                                  SQLUSMALLINT ColumnNumber,
                                                  SQLCHAR *ColumnName,
                                                  SQLSMALLINT BufferLength,
                                                  SQLSMALLINT *NameLength,
                                                  SQLSMALLINT *DataType,
                                                  SQLULEN *ColumnSize,
                                                  SQLSMALLINT *DecimalDigits,
                                                  SQLSMALLINT *Nullable);


    typedef SQLRETURN (SQL_API *odbcapi__SQLDescribeColW)(SQLHSTMT StatementHandle,
                                                  SQLUSMALLINT ColumnNumber,
                                                  SQLWCHAR *ColumnName,
                                                  SQLSMALLINT BufferLength,
                                                  SQLSMALLINT *NameLength,
                                                  SQLSMALLINT *DataType,
                                                  SQLULEN *ColumnSize,
                                                  SQLSMALLINT *DecimalDigits,
                                                  SQLSMALLINT *Nullable);


// Level 2

    typedef  SQLRETURN (SQL_API *odbcapi__SQLDescribeParam)(SQLHSTMT           hstmt,
                                                    SQLUSMALLINT       ipar,
                                                    SQLSMALLINT 	  *pfSqlType,
                                                    SQLULEN      	  *pcbParamDef,
                                                    SQLSMALLINT 	  *pibScale,
                                                    SQLSMALLINT 	  *pfNullable);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLDisconnect)(SQLHDBC ConnectionHandle);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLDriverConnectA)(SQLHDBC     ConnectionHandle,
                                                    SQLHWND     WindowHandle,
                                                    SQLCHAR *     InConnectionString,
                                                    SQLSMALLINT     StringLength1,
                                                    SQLCHAR *     OutConnectionString,
                                                    SQLSMALLINT     BufferLength,
                                                    SQLSMALLINT *     StringLength2Ptr,
                                                    SQLUSMALLINT     DriverCompletion);


    typedef SQLRETURN (SQL_API *odbcapi__SQLDriverConnectW)(SQLHDBC     ConnectionHandle,
                                                    SQLHWND     WindowHandle,
                                                    SQLWCHAR *     InConnectionString,
                                                    SQLSMALLINT     StringLength1,
                                                    SQLWCHAR *     OutConnectionString,
                                                    SQLSMALLINT     BufferLength,
                                                    SQLSMALLINT *     StringLength2Ptr,
                                                    SQLUSMALLINT     DriverCompletion);


// Core[1]
    typedef SQLRETURN (SQL_API *odbcapi__SQLEndTran)(SQLSMALLINT HandleType,
                                             SQLHANDLE Handle,
                                             SQLSMALLINT CompletionType);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLExecDirectA)(SQLHSTMT     StatementHandle,
                                                 SQLCHAR *     StatementText,
                                                 SQLINTEGER     TextLength);

    typedef SQLRETURN (SQL_API *odbcapi__SQLExecDirectW)(SQLHSTMT     StatementHandle,
                                                 SQLWCHAR *     StatementText,
                                                 SQLINTEGER     TextLength);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLExecute)(SQLHSTMT StatementHandle);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLFetch)(SQLHSTMT StatementHandle);

// Core[1]
    typedef SQLRETURN (SQL_API *odbcapi__SQLFetchScroll)(SQLHSTMT StatementHandle,
                                                 SQLSMALLINT FetchOrientation,
                                                 SQLLEN FetchOffset);
// Level 2
/*
  SQLRETURN SQL_API SQLForeignKeys(SQLHSTMT     StatementHandle,
  SQLCHAR *     PKCatalogName,
  SQLSMALLINT     NameLength1,
  SQLCHAR *     PKSchemaName,
  SQLSMALLINT     NameLength2,
  SQLCHAR *     PKTableName,
  SQLSMALLINT     NameLength3,
  SQLCHAR *     FKCatalogName,
  SQLSMALLINT     NameLength4,
  SQLCHAR *     FKSchemaName,
  SQLSMALLINT     NameLength5,
  SQLCHAR *     FKTableName,
  SQLSMALLINT     NameLength6)
  {
  ShowMessag(__PRETTY_FUNCTION__);
  return SQL_SUCCESS;
  }
*/


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLFreeHandle)(SQLSMALLINT HandleType,
                                                SQLHANDLE Handle);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLFreeStmt)(SQLHSTMT     StatementHandle,
                                              SQLUSMALLINT     Option);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetConnectAttrA)(SQLHDBC            hdbc,
                                                     SQLINTEGER         fAttribute,
                                                     SQLPOINTER         rgbValue,
                                                     SQLINTEGER         cbValueMax,
                                                     SQLINTEGER     *pcbValue);


    typedef SQLRETURN (SQL_API *odbcapi__SQLGetConnectAttrW)(SQLHDBC            hdbc,
                                                     SQLINTEGER         fAttribute,
                                                     SQLPOINTER         rgbValue,
                                                     SQLINTEGER         cbValueMax,
                                                     SQLINTEGER     *pcbValue);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetCursorNameA)(SQLHSTMT     StatementHandle,
                                                    SQLCHAR *     CursorName,
                                                    SQLSMALLINT     BufferLength,
                                                    SQLSMALLINT *     NameLengthPtr);


    typedef SQLRETURN (SQL_API *odbcapi__SQLGetCursorNameW)(SQLHSTMT     StatementHandle,
                                                    SQLWCHAR *     CursorName,
                                                    SQLSMALLINT     BufferLength,
                                                    SQLSMALLINT *     NameLengthPtr);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetData)(SQLHSTMT StatementHandle,
                                             SQLUSMALLINT ColumnNumber,
                                             SQLSMALLINT TargetType,
                                             SQLPOINTER TargetValue,
                                             SQLLEN BufferLength,
                                             SQLLEN *StrLen_or_Ind);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetDescFieldA)(SQLHDESC           hdesc,
                                                   SQLSMALLINT        iRecord,
                                                   SQLSMALLINT        iField,
                                                   SQLPOINTER         rgbValue,
                                                   SQLINTEGER		   cbValueMax,
                                                   SQLINTEGER     *pcbValue);

    typedef SQLRETURN (SQL_API *odbcapi__SQLGetDescFieldW)(SQLHDESC           hdesc,
                                                   SQLSMALLINT        iRecord,
                                                   SQLSMALLINT        iField,
                                                   SQLPOINTER         rgbValue,
                                                   SQLINTEGER		   cbValueMax,
                                                   SQLINTEGER     *pcbValue);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetDescRecA)(SQLHDESC     DescriptorHandle,
                                                 SQLSMALLINT     RecNumber,
                                                 SQLCHAR *     Name,
                                                 SQLSMALLINT     BufferLength,
                                                 SQLSMALLINT *     StringLengthPtr,
                                                 SQLSMALLINT *     TypePtr,
                                                 SQLSMALLINT *     SubTypePtr,
                                                 SQLINTEGER *     LengthPtr,
                                                 SQLSMALLINT *     PrecisionPtr,
                                                 SQLSMALLINT *     ScalePtr,
                                                 SQLSMALLINT *     NullablePtr);

    typedef SQLRETURN (SQL_API *odbcapi__SQLGetDescRecW)(SQLHDESC     DescriptorHandle,
                                                 SQLSMALLINT     RecNumber,
                                                 SQLWCHAR *     Name,
                                                 SQLSMALLINT     BufferLength,
                                                 SQLSMALLINT *     StringLengthPtr,
                                                 SQLSMALLINT *     TypePtr,
                                                 SQLSMALLINT *     SubTypePtr,
                                                 SQLINTEGER *     LengthPtr,
                                                 SQLSMALLINT *     PrecisionPtr,
                                                 SQLSMALLINT *     ScalePtr,
                                                 SQLSMALLINT *     NullablePtr);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetDiagFieldA)(SQLSMALLINT        fHandleType,
                                                   SQLHANDLE          handle,
                                                   SQLSMALLINT        iRecord,
                                                   SQLSMALLINT        fDiagField,
                                                   SQLPOINTER         rgbDiagInfo,
                                                   SQLSMALLINT        cbDiagInfoMax,
                                                   SQLSMALLINT    *pcbDiagInfo);

    typedef SQLRETURN (SQL_API *odbcapi__SQLGetDiagFieldW)(SQLSMALLINT        fHandleType,
                                                   SQLHANDLE          handle,
                                                   SQLSMALLINT        iRecord,
                                                   SQLSMALLINT        fDiagField,
                                                   SQLPOINTER         rgbDiagInfo,
                                                   SQLSMALLINT        cbDiagInfoMax,
                                                   SQLSMALLINT    *pcbDiagInfo);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetDiagRecA)(SQLSMALLINT     HandleType,
                                                 SQLHANDLE     handle_,
                                                 SQLSMALLINT     RecNumber,
                                                 SQLCHAR *     SQLState,
                                                 SQLINTEGER *     NativeErrorPtr,
                                                 SQLCHAR *     MessageText,
                                                 SQLSMALLINT     BufferLength,
                                                 SQLSMALLINT *     TextLengthPtr);

    typedef SQLRETURN (SQL_API *odbcapi__SQLGetDiagRecW)(SQLSMALLINT     HandleType,
                                                 SQLHANDLE     handle_,
                                                 SQLSMALLINT     RecNumber,
                                                 SQLWCHAR *     SQLState,
                                                 SQLINTEGER *     NativeErrorPtr,
                                                 SQLWCHAR *     MessageText,
                                                 SQLSMALLINT     BufferLength,
                                                 SQLSMALLINT *     TextLengthPtr);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetEnvAttr)(SQLHENV EnvironmentHandle,
                                                SQLINTEGER Attribute, SQLPOINTER Value,
                                                SQLINTEGER BufferLength, SQLINTEGER *StringLength);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetFunctions)(SQLHDBC ConnectionHandle,
                                                  SQLUSMALLINT FunctionId, SQLUSMALLINT *Supported);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetInfoA)(SQLHDBC            hdbc,
                                              SQLUSMALLINT       fInfoType,
                                              SQLPOINTER         rgbInfoValue,
                                              SQLSMALLINT        cbInfoValueMax,
                                              SQLSMALLINT    *pcbInfoValue);

    typedef SQLRETURN (SQL_API *odbcapi__SQLGetInfoW)(SQLHDBC            hdbc,
                                              SQLUSMALLINT       fInfoType,
                                              SQLPOINTER         rgbInfoValue,
                                              SQLSMALLINT        cbInfoValueMax,
                                              SQLSMALLINT    *pcbInfoValue);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetStmtAttrA)(SQLHSTMT           hstmt,
                                                  SQLINTEGER         fAttribute,
                                                  SQLPOINTER         rgbValue,
                                                  SQLINTEGER         cbValueMax,
                                                  SQLINTEGER     *pcbValue);


    typedef SQLRETURN (SQL_API *odbcapi__SQLGetStmtAttrW)(SQLHSTMT           hstmt,
                                                  SQLINTEGER         fAttribute,
                                                  SQLPOINTER         rgbValue,
                                                  SQLINTEGER         cbValueMax,
                                                  SQLINTEGER     *pcbValue);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLGetTypeInfo)(SQLHSTMT			StatementHandle,
                                                 SQLSMALLINT			DataType);


// Level 1
    typedef SQLRETURN (SQL_API *odbcapi__SQLMoreResults)(SQLHSTMT           hstmt);


// Core
/*
  SQLRETURN SQL_API SQLNativeSql(SQLHDBC     ConnectionHandle,
  SQLCHAR *     InStatementText,
  SQLINTEGER     TextLength1,
  SQLCHAR *     OutStatementText,
  SQLINTEGER     BufferLength,
  SQLINTEGER *     TextLength2Ptr)
  {
  ShowMessage(__PRETTY_FUNCTION__);
  return MPAPI_SQLNativeSql(ConnectionHandle, InStatementText,
  TextLength1, OutStatementText, BufferLength,
  TextLength2Ptr);
  }
*/


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLNumParams)(SQLHSTMT           hstmt,
                                               SQLSMALLINT 	  *pcpar);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLNumResultCols)(SQLHSTMT StatementHandle,
                                                   SQLSMALLINT *ColumnCount);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLParamData)(SQLHSTMT StatementHandle,
                                               SQLPOINTER *Value);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLPrepareA)(SQLHSTMT     StatementHandle,
                                              SQLCHAR *     StatementText,
                                              SQLINTEGER     TextLength);

    typedef SQLRETURN (SQL_API *odbcapi__SQLPrepareW)(SQLHSTMT     StatementHandle,
                                              SQLWCHAR *     StatementText,
                                              SQLINTEGER     TextLength);


// Level 1

    typedef SQLRETURN (SQL_API *odbcapi__SQLPrimaryKeysA)(SQLHSTMT     StatementHandle,
                                                  SQLCHAR *     CatalogName,
                                                  SQLSMALLINT     NameLength1,
                                                  SQLCHAR *     SchemaName,
                                                  SQLSMALLINT     NameLength2,
                                                  SQLCHAR *     TableName,
                                                  SQLSMALLINT     NameLength3);

    typedef SQLRETURN (SQL_API *odbcapi__SQLPrimaryKeysW)(SQLHSTMT     StatementHandle,
                                                  SQLWCHAR *     CatalogName,
                                                  SQLSMALLINT     NameLength1,
                                                  SQLWCHAR *     SchemaName,
                                                  SQLSMALLINT     NameLength2,
                                                  SQLWCHAR *     TableName,
                                                  SQLSMALLINT     NameLength3);



// Level 1
    typedef SQLRETURN (SQL_API *odbcapi__SQLProcedureColumnsA)(SQLHSTMT     StatementHandle,
                                                       SQLCHAR *     CatalogName,
                                                       SQLSMALLINT     NameLength1,
                                                       SQLCHAR *     SchemaName,
                                                       SQLSMALLINT     NameLength2,
                                                       SQLCHAR *     ProcName,
                                                       SQLSMALLINT     NameLength3,
                                                       SQLCHAR *     ColumnName,
                                                       SQLSMALLINT     NameLength4);

    typedef SQLRETURN (SQL_API *odbcapi__SQLProcedureColumnsW)(SQLHSTMT     StatementHandle,
                                                       SQLWCHAR *     CatalogName,
                                                       SQLSMALLINT     NameLength1,
                                                       SQLWCHAR *     SchemaName,
                                                       SQLSMALLINT     NameLength2,
                                                       SQLWCHAR *     ProcName,
                                                       SQLSMALLINT     NameLength3,
                                                       SQLWCHAR *     ColumnName,
                                                       SQLSMALLINT     NameLength4);


// Level 1
    typedef SQLRETURN (SQL_API *odbcapi__SQLProceduresA)(SQLHSTMT     StatementHandle,
                                                 SQLCHAR *     CatalogName,
                                                 SQLSMALLINT     NameLength1,
                                                 SQLCHAR *     SchemaName,
                                                 SQLSMALLINT     NameLength2,
                                                 SQLCHAR *     ProcName,
                                                 SQLSMALLINT     NameLength3);

    typedef SQLRETURN (SQL_API *odbcapi__SQLProceduresW)(SQLHSTMT     StatementHandle,
                                                 SQLWCHAR *     CatalogName,
                                                 SQLSMALLINT     NameLength1,
                                                 SQLWCHAR *     SchemaName,
                                                 SQLSMALLINT     NameLength2,
                                                 SQLWCHAR *     ProcName,
                                                 SQLSMALLINT     NameLength3);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLPutData)(SQLHSTMT StatementHandle,
                                             SQLPOINTER Data,
                                             SQLLEN StrLen_or_Ind);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLRowCount)(SQLHSTMT StatementHandle, 
                                              SQLLEN* RowCount);


// Core[2]
    typedef SQLRETURN (SQL_API *odbcapi__SQLSetConnectAttrA)(SQLHDBC            hdbc,
                                                     SQLINTEGER         fAttribute,
                                                     SQLPOINTER         rgbValue,
                                                     SQLINTEGER         cbValue);

    typedef SQLRETURN (SQL_API *odbcapi__SQLSetConnectAttrW)(SQLHDBC            hdbc,
                                                     SQLINTEGER         fAttribute,
                                                     SQLPOINTER         rgbValue,
                                                     SQLINTEGER         cbValue);

// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLSetCursorNameA)(SQLHSTMT      StatementHandle,
                                                    SQLCHAR *     CursorName,
                                                    SQLSMALLINT     NameLength);

    typedef SQLRETURN (SQL_API *odbcapi__SQLSetCursorNameW)(SQLHSTMT      StatementHandle,
                                                    SQLWCHAR *     CursorName,
                                                    SQLSMALLINT     NameLength);

// Core[1]
    typedef SQLRETURN (SQL_API *odbcapi__SQLSetDescFieldA)(SQLHDESC DescriptorHandle,
                                                   SQLSMALLINT RecNumber, 
                                                   SQLSMALLINT FieldIdentifier,
                                                   SQLPOINTER Value, 
                                                   SQLINTEGER BufferLength);

    typedef SQLRETURN (SQL_API *odbcapi__SQLSetDescFieldW)(SQLHDESC DescriptorHandle,
                                                   SQLSMALLINT RecNumber, 
                                                   SQLSMALLINT FieldIdentifier,
                                                   SQLPOINTER Value, 
                                                   SQLINTEGER BufferLength);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLSetDescRec)(SQLHDESC DescriptorHandle,
                                                SQLSMALLINT RecNumber,
                                                SQLSMALLINT Type,
                                                SQLSMALLINT SubType,
                                                SQLLEN Length,
                                                SQLSMALLINT Precision,
                                                SQLSMALLINT Scale,
                                                SQLPOINTER Data,
                                                SQLLEN *StringLength,
                                                SQLLEN *Indicator);

// Core[2]
    typedef SQLRETURN (SQL_API *odbcapi__SQLSetEnvAttr)(SQLHENV EnvironmentHandle,
                                                SQLINTEGER Attribute, SQLPOINTER Value,
                                                SQLINTEGER StringLength);


// Level 1[1]
/*
  SQLRETURN SQL_API SQLSetPos(SQLHSTMT           hstmt,
  SQLSETPOSIROW      irow,
  SQLUSMALLINT       fOption,
  SQLUSMALLINT       fLock)
  {
  return SQL_SUCCESS;
  }
*/


// Core[2]
    typedef SQLRETURN (SQL_API *odbcapi__SQLSetStmtAttrA)(SQLHSTMT           hstmt,
                                                  SQLINTEGER         fAttribute,
                                                  SQLPOINTER         rgbValue,
                                                  SQLINTEGER         cbValueMax);


    typedef SQLRETURN (SQL_API *odbcapi__SQLSetStmtAttrW)(SQLHSTMT           hstmt,
                                                  SQLINTEGER         fAttribute,
                                                  SQLPOINTER         rgbValue,
                                                  SQLINTEGER         cbValueMax);


// Core[1]
    typedef SQLRETURN (SQL_API *odbcapi__SQLSpecialColumnsA)(SQLHSTMT StatementHandle,
                                                     SQLUSMALLINT IdentifierType, SQLCHAR *CatalogName,
                                                     SQLSMALLINT NameLength1, SQLCHAR *SchemaName,
                                                     SQLSMALLINT NameLength2, SQLCHAR *TableName,
                                                     SQLSMALLINT NameLength3, SQLUSMALLINT Scope,
                                                     SQLUSMALLINT Nullable);

    typedef SQLRETURN (SQL_API *odbcapi__SQLSpecialColumnsW)(SQLHSTMT StatementHandle,
                                                     SQLUSMALLINT IdentifierType, SQLWCHAR *CatalogName,
                                                     SQLSMALLINT NameLength1, SQLWCHAR *SchemaName,
                                                     SQLSMALLINT NameLength2, SQLWCHAR *TableName,
                                                     SQLSMALLINT NameLength3, SQLUSMALLINT Scope,
                                                     SQLUSMALLINT Nullable);


// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLStatisticsA)(SQLHSTMT     StatementHandle,
                                                 SQLCHAR *     CatalogName,
                                                 SQLSMALLINT     NameLength1,
                                                 SQLCHAR *     SchemaName,
                                                 SQLSMALLINT     NameLength2,
                                                 SQLCHAR *     TableName,
                                                 SQLSMALLINT     NameLength3,
                                                 SQLUSMALLINT     Unique,
                                                 SQLUSMALLINT     Reserved);

    typedef SQLRETURN (SQL_API *odbcapi__SQLStatisticsW)(SQLHSTMT     StatementHandle,
                                                 SQLWCHAR *     CatalogName,
                                                 SQLSMALLINT     NameLength1,
                                                 SQLWCHAR *     SchemaName,
                                                 SQLSMALLINT     NameLength2,
                                                 SQLWCHAR *     TableName,
                                                 SQLSMALLINT     NameLength3,
                                                 SQLUSMALLINT     Unique,
                                                 SQLUSMALLINT     Reserved);



// Level 2
/*
  SQLRETURN SQL_API SQLTablePrivileges(SQLHSTMT     StatementHandle,
  SQLCHAR *     CatalogName,ä
  SQLSMALLINT     NameLength1,
  SQLCHAR *     SchemaName,
  SQLSMALLINT     NameLength2,
  SQLCHAR *     TableName,
  SQLSMALLINT     NameLength3)
  {
  ShowMessage(__PRETTY_FUNCTION__);
  return MPAPI_SQLTablePrivileges(StatementHandle, CatalogName, NameLength1,
  SchemaName, NameLength2, TableName, NameLength3);
  }
*/



// Core
    typedef SQLRETURN (SQL_API *odbcapi__SQLTablesA)(SQLHSTMT     StatementHandle,
                                             SQLCHAR *     CatalogName,
                                             SQLSMALLINT     NameLength1,
                                             SQLCHAR *     SchemaName,
                                             SQLSMALLINT     NameLength2,
                                             SQLCHAR *     TableName,
                                             SQLSMALLINT     NameLength3,
                                             SQLCHAR *     TableType,
                                             SQLSMALLINT     NameLength4);


    typedef SQLRETURN (SQL_API *odbcapi__SQLTablesW)(SQLHSTMT     StatementHandle,
                                             SQLWCHAR *     CatalogName,
                                             SQLSMALLINT     NameLength1,
                                             SQLWCHAR *     SchemaName,
                                             SQLSMALLINT     NameLength2,
                                             SQLWCHAR *     TableName,
                                             SQLSMALLINT     NameLength3,
                                             SQLWCHAR *     TableType,
                                             SQLSMALLINT     NameLength4);


protected:

    odbcapi__SQLAllocHandle 	  m_func_SQLAllocHandle;
    odbcapi__SQLBindCol 		  m_func_SQLBindCol;
    odbcapi__SQLBindParameter 	  m_func_SQLBindParameter;
    odbcapi__SQLCancel 		  m_func_SQLCancel;
    odbcapi__SQLCloseCursor 	  m_func_SQLCloseCursor;
    odbcapi__SQLColAttributeA 	  m_func_SQLColAttributeA;
    odbcapi__SQLColAttributeW 	  m_func_SQLColAttributeW;
    odbcapi__SQLColumnsA 		  m_func_SQLColumnsA;
    odbcapi__SQLColumnsW 		  m_func_SQLColumnsW;
    odbcapi__SQLConnectA 		  m_func_SQLConnectA;
    odbcapi__SQLConnectW 		  m_func_SQLConnectW;
    odbcapi__SQLCopyDesc 		  m_func_SQLCopyDescA;
    odbcapi__SQLDescribeColA 	  m_func_SQLDescribeColA;
    odbcapi__SQLDescribeColW 	  m_func_SQLDescribeColW;
    odbcapi__SQLDescribeParam	m_func_SQLDescribeParam;
    odbcapi__SQLDisconnect 		  m_func_SQLDisconnect;
    odbcapi__SQLDriverConnectA 	  m_func_SQLDriverConnectA;
    odbcapi__SQLDriverConnectW 	  m_func_SQLDriverConnectW;
    odbcapi__SQLEndTran 		  m_func_SQLEndTran;
    odbcapi__SQLExecDirectA 		  m_func_SQLExecDirectA;
    odbcapi__SQLExecDirectW 		  m_func_SQLExecDirectW;
    odbcapi__SQLExecute 		  m_func_SQLExecute;
    odbcapi__SQLFetch 		  m_func_SQLFetch;
    odbcapi__SQLFetchScroll 	  m_func_SQLFetchScroll;
    odbcapi__SQLFreeHandle 		  m_func_SQLFreeHandle;
    odbcapi__SQLFreeStmt 		  m_func_SQLFreeStmt;
    odbcapi__SQLGetConnectAttrA 	  m_func_SQLGetConnectAttrA;
    odbcapi__SQLGetConnectAttrW 	  m_func_SQLGetConnectAttrW;
    odbcapi__SQLGetCursorNameA 	  m_func_SQLGetCursorNameA;
    odbcapi__SQLGetCursorNameW 	  m_func_SQLGetCursorNameW;
    odbcapi__SQLGetData 		  m_func_SQLGetData;
    odbcapi__SQLPutData                   m_func_SQLPutData;
    odbcapi__SQLGetFunctions      m_func_SQLGetFunctions;
    odbcapi__SQLGetDescFieldA 	  m_func_SQLGetDescFieldA;
    odbcapi__SQLGetDescFieldW 	  m_func_SQLGetDescFieldW;
    odbcapi__SQLGetDescRecA 		  m_func_SQLGetDescRecA;
    odbcapi__SQLGetDescRecW 		  m_func_SQLGetDescRecW;
    odbcapi__SQLGetDiagFieldA 	  m_func_SQLGetDiagFieldA;
    odbcapi__SQLGetDiagFieldW 	  m_func_SQLGetDiagFieldW;
    odbcapi__SQLGetDiagRecA 		  m_func_SQLGetDiagRecA;
    odbcapi__SQLGetDiagRecW 		  m_func_SQLGetDiagRecW;
    odbcapi__SQLGetEnvAttr 		  m_func_SQLGetEnvAttr;
    odbcapi__SQLGetInfoA 		  m_func_SQLGetInfoA;
    odbcapi__SQLGetInfoW 		  m_func_SQLGetInfoW;
    odbcapi__SQLGetStmtAttrA 	  m_func_SQLGetStmtAttrA;
    odbcapi__SQLGetStmtAttrW 	  m_func_SQLGetStmtAttrW;
    odbcapi__SQLGetTypeInfo 	  m_func_SQLGetTypeInfo;
    odbcapi__SQLMoreResults 	  m_func_SQLMoreResults;
    odbcapi__SQLNumParams 		  m_func_SQLNumParams;
    odbcapi__SQLNumResultCols 	  m_func_SQLNumResultCols;
    odbcapi__SQLParamData 		  m_func_SQLParamData;
    odbcapi__SQLPrepareA 		  m_func_SQLPrepareA;
    odbcapi__SQLPrepareW 		  m_func_SQLPrepareW;
    odbcapi__SQLPrimaryKeysA 	  m_func_SQLPrimaryKeysA;
    odbcapi__SQLPrimaryKeysW 	  m_func_SQLPrimaryKeysW;
    odbcapi__SQLProcedureColumnsA 	  m_func_SQLProcedureColumnsA;
    odbcapi__SQLProcedureColumnsW 	  m_func_SQLProcedureColumnsW;
    odbcapi__SQLProceduresA 		  m_func_SQLProceduresA;
    odbcapi__SQLProceduresW 		  m_func_SQLProceduresW;
    odbcapi__SQLRowCount 		  m_func_SQLRowCount;
    odbcapi__SQLSetConnectAttrA 	  m_func_SQLSetConnectAttrA;
    odbcapi__SQLSetConnectAttrW 	  m_func_SQLSetConnectAttrW;
    odbcapi__SQLSetCursorNameA 	  m_func_SQLSetCursorNameA;
    odbcapi__SQLSetCursorNameW 	  m_func_SQLSetCursorNameW;
    odbcapi__SQLSetDescFieldA 	  m_func_SQLSetDescFieldA;
    odbcapi__SQLSetDescFieldW 	  m_func_SQLSetDescFieldW;
    odbcapi__SQLSetDescRec 		  m_func_SQLSetDescRec;
    odbcapi__SQLSetEnvAttr		  m_func_SQLSetEnvAttr;
    odbcapi__SQLSetStmtAttrA 	  m_func_SQLSetStmtAttrA;
    odbcapi__SQLSetStmtAttrW 	  m_func_SQLSetStmtAttrW;
    odbcapi__SQLSpecialColumnsA 	  m_func_SQLSpecialColumnsA;
    odbcapi__SQLSpecialColumnsW 	  m_func_SQLSpecialColumnsW;
    odbcapi__SQLStatisticsA 		  m_func_SQLStatisticsA;
    odbcapi__SQLStatisticsW 		  m_func_SQLStatisticsW;
    odbcapi__SQLTablesA 		  m_func_SQLTablesA;
    odbcapi__SQLTablesW 		  m_func_SQLTablesW;



public:
    ODBC30Drv(String path) : DynamicLibrary(path.length() ? path.toSystemEncoding() : DBWTL_ODBC_DEFAULT_LIB),
                             m_func_SQLAllocHandle(0),
                             m_func_SQLBindCol(0),
                             m_func_SQLBindParameter(0),
                             m_func_SQLCancel(0),
                             m_func_SQLCloseCursor(0),
                             m_func_SQLColAttributeA(0),
                             m_func_SQLColAttributeW(0),
        m_func_SQLColumnsA(0),
        m_func_SQLColumnsW(0),
        m_func_SQLConnectA(0),
        m_func_SQLConnectW(0),
        m_func_SQLCopyDescA(0),
        m_func_SQLDescribeColA(0),
        m_func_SQLDescribeColW(0),
        m_func_SQLDescribeParam(0),
        m_func_SQLDisconnect(0),
        m_func_SQLDriverConnectA(0),
        m_func_SQLDriverConnectW(0),
        m_func_SQLEndTran(0),
        m_func_SQLExecDirectA(0),
        m_func_SQLExecDirectW(0),
        m_func_SQLExecute(0),
        m_func_SQLFetch(0),
        m_func_SQLFetchScroll(0),
        m_func_SQLFreeHandle(0),
        m_func_SQLFreeStmt(0),
        m_func_SQLGetConnectAttrA(0),
        m_func_SQLGetConnectAttrW(0),
        m_func_SQLGetCursorNameA(0),
        m_func_SQLGetCursorNameW(0),
        m_func_SQLGetData(0),
        m_func_SQLPutData(0),
        m_func_SQLGetFunctions(0),
        m_func_SQLGetDescFieldA(0),
        m_func_SQLGetDescFieldW(0),
        m_func_SQLGetDescRecA(0),
        m_func_SQLGetDescRecW(0),
        m_func_SQLGetDiagFieldA(0),
        m_func_SQLGetDiagFieldW(0),
        m_func_SQLGetDiagRecA(0),
        m_func_SQLGetDiagRecW(0),
        m_func_SQLGetEnvAttr(0),
        m_func_SQLGetInfoA(0),
        m_func_SQLGetInfoW(0),
        m_func_SQLGetStmtAttrA(0),
        m_func_SQLGetStmtAttrW(0),
        m_func_SQLGetTypeInfo(0),
        m_func_SQLMoreResults(0),
        m_func_SQLNumParams(0),
        m_func_SQLNumResultCols(0),
        m_func_SQLParamData(0),
        m_func_SQLPrepareA(0),
        m_func_SQLPrepareW(0),
        m_func_SQLPrimaryKeysA(0),
        m_func_SQLPrimaryKeysW(0),
        m_func_SQLProcedureColumnsA(0),
        m_func_SQLProcedureColumnsW(0),
        m_func_SQLProceduresA(0),
        m_func_SQLProceduresW(0),
        m_func_SQLRowCount(0),
        m_func_SQLSetConnectAttrA(0),
        m_func_SQLSetConnectAttrW(0),
        m_func_SQLSetCursorNameA(0),
        m_func_SQLSetCursorNameW(0),
        m_func_SQLSetDescFieldA(0),
        m_func_SQLSetDescFieldW(0),
        m_func_SQLSetDescRec(0),
        m_func_SQLSetEnvAttr(0),
        m_func_SQLSetStmtAttrA(0),
        m_func_SQLSetStmtAttrW(0),
        m_func_SQLSpecialColumnsA(0),
        m_func_SQLSpecialColumnsW(0),
        m_func_SQLStatisticsA(0),
        m_func_SQLStatisticsW(0),
        m_func_SQLTablesA(0),
        m_func_SQLTablesW(0)
        {
            this->getproc(this->m_func_SQLAllocHandle, "SQLAllocHandle");
            this->getproc(this->m_func_SQLFreeHandle, "SQLFreeHandle");
            this->getproc(this->m_func_SQLConnectA, "SQLConnectA");
            this->getproc(this->m_func_SQLConnectW, "SQLConnectW");
            this->getproc(this->m_func_SQLDisconnect, "SQLDisconnect");

            this->getproc(this->m_func_SQLExecDirectA, "SQLExecDirectA");
            this->getproc(this->m_func_SQLExecDirectW, "SQLExecDirectW");

            this->getproc(this->m_func_SQLSetConnectAttrA, "SQLSetConnectAttrA");
            this->getproc(this->m_func_SQLSetConnectAttrW, "SQLSetConnectAttrW");

            this->getproc(this->m_func_SQLSetStmtAttrA, "SQLSetStmtAttrA");
            this->getproc(this->m_func_SQLSetStmtAttrW, "SQLSetStmtAttrW");

            this->getproc(this->m_func_SQLSetEnvAttr, "SQLSetEnvAttr");

            this->getproc(this->m_func_SQLGetDiagRecA, "SQLGetDiagRecA");
            this->getproc(this->m_func_SQLGetDiagRecW, "SQLGetDiagRecW");

            this->getproc(this->m_func_SQLPrepareA, "SQLPrepareA");
            this->getproc(this->m_func_SQLPrepareW, "SQLPrepareW");

            this->getproc(this->m_func_SQLExecute, "SQLExecute");

            this->getproc(this->m_func_SQLNumResultCols, "SQLNumResultCols");
	
            this->getproc(this->m_func_SQLGetData, "SQLGetData");

            this->getproc(this->m_func_SQLFetch, "SQLFetch");
            this->getproc(this->m_func_SQLDescribeColA, "SQLDescribeColA");
            this->getproc(this->m_func_SQLDescribeColW, "SQLDescribeColW");
            this->getproc(this->m_func_SQLBindCol, "SQLBindCol");

            this->getproc(this->m_func_SQLBindParameter, "SQLBindParameter");
            this->getproc(this->m_func_SQLNumParams, "SQLNumParams");

            this->getproc(this->m_func_SQLFreeStmt, "SQLFreeStmt");
            this->getproc(this->m_func_SQLGetInfoA, "SQLGetInfoA");
            this->getproc(this->m_func_SQLGetInfoW, "SQLGetInfoW");

            this->getproc(this->m_func_SQLRowCount, "SQLRowCount");

            this->getproc(this->m_func_SQLParamData, "SQLParamData");
            this->getproc(this->m_func_SQLPutData, "SQLPutData");

            this->getproc(this->m_func_SQLColAttributeA, "SQLColAttributeA");
            this->getproc(this->m_func_SQLColAttributeW, "SQLColAttributeW");

            this->getproc(this->m_func_SQLGetStmtAttrA, "SQLGetStmtAttrA");
            this->getproc(this->m_func_SQLGetStmtAttrW, "SQLGetStmtAttrW");

            this->getproc(this->m_func_SQLGetDescFieldA, "SQLGetDescFieldA");
            this->getproc(this->m_func_SQLGetDescFieldW, "SQLGetDescFieldW");

            this->getproc(this->m_func_SQLSetDescFieldA, "SQLSetDescFieldA");
            this->getproc(this->m_func_SQLSetDescFieldW, "SQLSetDescFieldW");
	
            this->getproc(this->m_func_SQLDescribeParam, "SQLDescribeParam");

            this->getproc(this->m_func_SQLGetFunctions, "SQLGetFunctions");

            this->getproc(this->m_func_SQLGetDescRecA, "SQLGetDescRecA");
            this->getproc(this->m_func_SQLGetDescRecW, "SQLGetDescRecW");

	this->getproc(this->m_func_SQLEndTran, "SQLEndTran");

	this->getproc(this->m_func_SQLTablesA, "SQLTablesA");
	this->getproc(this->m_func_SQLTablesW, "SQLTablesW");

	this->getproc(this->m_func_SQLColumnsA, "SQLColumnsA");
	this->getproc(this->m_func_SQLColumnsW, "SQLColumnsW");

	this->getproc(this->m_func_SQLDriverConnectA, "SQLDriverConnectA");
	this->getproc(this->m_func_SQLDriverConnectW, "SQLDriverConnectW");

/*
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
*/
        }


    virtual ~ODBC30Drv(void) { }


    inline SQLRETURN SQLAllocHandle(SQLSMALLINT HandleType,
                                    SQLHANDLE InputHandle, SQLHANDLE *OutputHandle)
        {
	


            if(this->m_func_SQLAllocHandle)
                return this->m_func_SQLAllocHandle(HandleType, InputHandle, OutputHandle);
            else
                throw LibFunctionException(__FUNCTION__);
        }


// Core
    inline SQLRETURN SQLFreeHandle(SQLSMALLINT HandleType,
                                   SQLHANDLE Handle)
        {
            if(this->m_func_SQLConnectA)
                return this->m_func_SQLFreeHandle(HandleType, Handle);
            else
                throw LibFunctionException(__FUNCTION__);
        }



// Core
    inline SQLRETURN SQLConnectA(SQLHDBC     ConnectionHandle,
                                 SQLCHAR *     ServerName,
                                 SQLSMALLINT     NameLength1,
                                 SQLCHAR *     UserName,
                                 SQLSMALLINT     NameLength2,
                                 SQLCHAR *     Authentication,
                                 SQLSMALLINT     NameLength3)
        {
            if(this->m_func_SQLConnectA)
                return this->m_func_SQLConnectA(ConnectionHandle, ServerName, NameLength1,
                                                UserName, NameLength2, Authentication, NameLength3);
            else
                throw LibFunctionException(__FUNCTION__);
        }


    inline SQLRETURN SQLConnectW(SQLHDBC     ConnectionHandle,
                                 SQLWCHAR *     ServerName,
                                 SQLSMALLINT     NameLength1,
                                 SQLWCHAR *     UserName,
                                 SQLSMALLINT     NameLength2,
                                 SQLWCHAR *     Authentication,
                                 SQLSMALLINT     NameLength3)
        {
            if(this->m_func_SQLConnectW)
                return this->m_func_SQLConnectW(ConnectionHandle, ServerName, NameLength1,
                                                UserName, NameLength2, Authentication, NameLength3);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLDriverConnectA(SQLHDBC     ConnectionHandle,
                                                    SQLHWND     WindowHandle,
                                                    SQLCHAR *     InConnectionString,
                                                    SQLSMALLINT     StringLength1,
                                                    SQLCHAR *     OutConnectionString,
                                                    SQLSMALLINT     BufferLength,
                                                    SQLSMALLINT *     StringLength2Ptr,
                                                    SQLUSMALLINT     DriverCompletion)
{
                    if(this->m_func_SQLDriverConnectA)
                return this->m_func_SQLDriverConnectA(ConnectionHandle, WindowHandle,
                                InConnectionString, StringLength1, OutConnectionString,
                                                BufferLength, StringLength2Ptr, DriverCompletion);
            else
                throw LibFunctionException(__FUNCTION__);
}


    inline SQLRETURN SQLDriverConnectW(SQLHDBC     ConnectionHandle,
                                                    SQLHWND     WindowHandle,
                                                    SQLWCHAR *     InConnectionString,
                                                    SQLSMALLINT     StringLength1,
                                                    SQLWCHAR *     OutConnectionString,
                                                    SQLSMALLINT     BufferLength,
                                                    SQLSMALLINT *     StringLength2Ptr,
                                                    SQLUSMALLINT     DriverCompletion)
{
	            if(this->m_func_SQLDriverConnectW)
                return this->m_func_SQLDriverConnectW(ConnectionHandle, WindowHandle,
				InConnectionString, StringLength1, OutConnectionString,
						BufferLength, StringLength2Ptr, DriverCompletion);
            else
                throw LibFunctionException(__FUNCTION__);

}



    inline SQLRETURN SQLDisconnect(SQLHDBC ConnectionHandle)
        {
            if(this->m_func_SQLDisconnect)
                return this->m_func_SQLDisconnect(ConnectionHandle);
            else
                throw LibFunctionException(__FUNCTION__);

        }

    inline SQLRETURN SQLSetConnectAttrA(SQLHDBC            hdbc,
                                        SQLINTEGER         fAttribute,
                                        SQLPOINTER         rgbValue,
                                        SQLINTEGER         cbValue)
        {
            if(this->m_func_SQLSetConnectAttrA)
                return this->m_func_SQLSetConnectAttrA(hdbc, fAttribute, rgbValue, cbValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLSetConnectAttrW(SQLHDBC            hdbc,
                                        SQLINTEGER         fAttribute,
                                        SQLPOINTER         rgbValue,
                                        SQLINTEGER         cbValue)
        {
            if(this->m_func_SQLSetConnectAttrW)
                return this->m_func_SQLSetConnectAttrW(hdbc, fAttribute, rgbValue, cbValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }


    inline SQLRETURN SQLExecDirectA(SQLHSTMT     StatementHandle,
                                    SQLCHAR *     StatementText,
                                    SQLINTEGER     TextLength)
        {
            if(this->m_func_SQLExecDirectA)
                return this->m_func_SQLExecDirectA(StatementHandle, StatementText, TextLength);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLExecDirectW(SQLHSTMT     StatementHandle,
                                    SQLWCHAR *     StatementText,
                                    SQLINTEGER     TextLength)
        {
            if(this->m_func_SQLExecDirectW)
                return this->m_func_SQLExecDirectW(StatementHandle, StatementText, TextLength);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLSetEnvAttr(SQLHENV EnvironmentHandle,
                                   SQLINTEGER Attribute, SQLPOINTER Value,
                                   SQLINTEGER StringLength)
        {
            if(this->m_func_SQLSetEnvAttr)
                return this->m_func_SQLSetEnvAttr(EnvironmentHandle, Attribute, Value, StringLength);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLSetStmtAttrA(SQLHSTMT           hstmt,
                                     SQLINTEGER         fAttribute,
                                     SQLPOINTER         rgbValue,
                                     SQLINTEGER         cbValueMax)
        {
            if(this->m_func_SQLSetStmtAttrA)
                return this->m_func_SQLSetStmtAttrA(hstmt, fAttribute, rgbValue, cbValueMax);
            else
                throw LibFunctionException(__FUNCTION__);
        }



    inline SQLRETURN SQLSetStmtAttrW(SQLHSTMT           hstmt,
                                     SQLINTEGER         fAttribute,
                                     SQLPOINTER         rgbValue,
                                     SQLINTEGER         cbValueMax)
        {
            if(this->m_func_SQLSetStmtAttrW)
                return this->m_func_SQLSetStmtAttrW(hstmt, fAttribute, rgbValue, cbValueMax);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLGetDiagRecA(SQLSMALLINT     HandleType,
                                    SQLHANDLE     handle_,
                                    SQLSMALLINT     RecNumber,
                                    SQLCHAR *     SQLState,
                                    SQLINTEGER *     NativeErrorPtr,
                                    SQLCHAR *     MessageText,
                                    SQLSMALLINT     BufferLength,
                                    SQLSMALLINT *     TextLengthPtr)
        {
            if(this->m_func_SQLGetDiagRecA)
                return this->m_func_SQLGetDiagRecA(HandleType, handle_, RecNumber, SQLState, NativeErrorPtr, MessageText, BufferLength, TextLengthPtr);
            else
                throw LibFunctionException(__FUNCTION__);

        }

    inline SQLRETURN SQLGetDiagRecW(SQLSMALLINT     HandleType,
                                    SQLHANDLE     handle_,
                                    SQLSMALLINT     RecNumber,
                                    SQLWCHAR *     SQLState,
                                    SQLINTEGER *     NativeErrorPtr,
                                    SQLWCHAR *     MessageText,
                                    SQLSMALLINT     BufferLength,
                                    SQLSMALLINT *     TextLengthPtr)
        {
            if(this->m_func_SQLGetDiagRecW)
                return this->m_func_SQLGetDiagRecW(HandleType, handle_, RecNumber, SQLState, NativeErrorPtr, MessageText, BufferLength, TextLengthPtr);
            else
                throw LibFunctionException(__FUNCTION__);

        }


// Core
    inline SQLRETURN SQLPrepareA(SQLHSTMT     StatementHandle,
                                 SQLCHAR *     StatementText,
                                 SQLINTEGER     TextLength)
        {
            if(this->m_func_SQLPrepareA)
                return this->m_func_SQLPrepareA(StatementHandle, StatementText, TextLength);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLPrepareW(SQLHSTMT     StatementHandle,
                                 SQLWCHAR *     StatementText,
                                 SQLINTEGER     TextLength)
        {
            if(this->m_func_SQLPrepareW)
                return this->m_func_SQLPrepareW(StatementHandle, StatementText, TextLength);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLExecute(SQLHSTMT StatementHandle)
        {
            if(this->m_func_SQLExecute)
                return this->m_func_SQLExecute(StatementHandle);
            else
                throw LibFunctionException(__FUNCTION__);

        }


// Core
    inline SQLRETURN SQLNumResultCols(SQLHSTMT StatementHandle,
                                      SQLSMALLINT *ColumnCount)
        {
            if(this->m_func_SQLNumResultCols)
                return this->m_func_SQLNumResultCols(StatementHandle, ColumnCount);
            else
                throw LibFunctionException(__FUNCTION__);

        }
        


// Core
    inline SQLRETURN SQLGetData(SQLHSTMT StatementHandle,
                                SQLUSMALLINT ColumnNumber,
                                SQLSMALLINT TargetType,
                                SQLPOINTER TargetValue,
                                SQLLEN BufferLength,
                                SQLLEN *StrLen_or_Ind)
        {
            if(this->m_func_SQLNumResultCols)
                return this->m_func_SQLGetData(StatementHandle, ColumnNumber, TargetType, TargetValue,
                                               BufferLength, StrLen_or_Ind);
            else
                throw LibFunctionException(__FUNCTION__);
        }
                            
// Core
    inline SQLRETURN SQLFetch(SQLHSTMT StatementHandle)
        {
            if(this->m_func_SQLFetch)
                return this->m_func_SQLFetch(StatementHandle);
            else
                throw LibFunctionException(__FUNCTION__);
        }

// Core
    inline SQLRETURN SQLBindCol(SQLHSTMT StatementHandle,
                                SQLUSMALLINT ColumnNumber,
                                SQLSMALLINT TargetType,
                                SQLPOINTER TargetValue,
                                SQLLEN BufferLength,
                                SQLLEN *StrLen_or_Ind)
        {
            if(this->m_func_SQLBindCol)
                return this->m_func_SQLBindCol(StatementHandle, ColumnNumber, TargetType, TargetValue,
                                               BufferLength, StrLen_or_Ind);
            else
                throw LibFunctionException(__FUNCTION__);

        }

// Core[1]
    inline SQLRETURN SQLDescribeColA(SQLHSTMT StatementHandle,
                                     SQLUSMALLINT ColumnNumber,
                                     SQLCHAR *ColumnName,
                                     SQLSMALLINT BufferLength,
                                     SQLSMALLINT *NameLength,
                                     SQLSMALLINT *DataType,
                                     SQLULEN *ColumnSize,
                                     SQLSMALLINT *DecimalDigits,
                                     SQLSMALLINT *Nullable)
        {
            if(this->m_func_SQLDescribeColA)
                return this->m_func_SQLDescribeColA(StatementHandle, ColumnNumber,
                                                    ColumnName, BufferLength, NameLength, DataType, ColumnSize, DecimalDigits, Nullable);
            else
                throw LibFunctionException(__FUNCTION__);

        }


    inline SQLRETURN SQLDescribeColW(SQLHSTMT StatementHandle,
                                     SQLUSMALLINT ColumnNumber,
                                     SQLWCHAR *ColumnName,
                                     SQLSMALLINT BufferLength,
                                     SQLSMALLINT *NameLength,
                                     SQLSMALLINT *DataType,
                                     SQLULEN *ColumnSize,
                                     SQLSMALLINT *DecimalDigits,
                                     SQLSMALLINT *Nullable)
        {
            if(this->m_func_SQLDescribeColW)
                return this->m_func_SQLDescribeColW(StatementHandle, ColumnNumber,
                                                    ColumnName, BufferLength, NameLength, DataType, ColumnSize, DecimalDigits, Nullable);
            else
                throw LibFunctionException(__FUNCTION__);

        }


    inline SQLRETURN SQLNumParams(SQLHSTMT           hstmt,
                                  SQLSMALLINT 	  *pcpar)
        {
            if(this->m_func_SQLNumParams)
                return this->m_func_SQLNumParams(hstmt, pcpar);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLBindParameter(SQLHSTMT hstmt,
                                      SQLUSMALLINT      ipar,
                                      SQLSMALLINT        fParamType,
                                      SQLSMALLINT        fCType,
                                      SQLSMALLINT        fSqlType,
                                      SQLULEN            cbColDef,
                                      SQLSMALLINT        ibScale,
                                      SQLPOINTER         rgbValue,
                                      SQLLEN             cbValueMax,
                                      SQLLEN     	   *pcbValue)
        {
            if(this->m_func_SQLBindParameter)
                return this->m_func_SQLBindParameter(hstmt, ipar, fParamType, fCType,
                                                     fSqlType, cbColDef, ibScale,
                                                     rgbValue, cbValueMax, pcbValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLFreeStmt(SQLHSTMT     StatementHandle,
                                 SQLUSMALLINT     Option)
        {
            if(this->m_func_SQLFreeStmt)
                return this->m_func_SQLFreeStmt(StatementHandle, Option);
            else
                throw LibFunctionException(__FUNCTION__);
        }


    inline SQLRETURN SQLGetInfoA(SQLHDBC            hdbc,
                                 SQLUSMALLINT       fInfoType,
                                 SQLPOINTER         rgbInfoValue,
                                 SQLSMALLINT        cbInfoValueMax,
                                 SQLSMALLINT    *pcbInfoValue)
        {
            if(this->m_func_SQLGetInfoA)
                return this->m_func_SQLGetInfoA(hdbc, fInfoType, rgbInfoValue, cbInfoValueMax,
                                                pcbInfoValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLGetInfoW(SQLHDBC            hdbc,
                                 SQLUSMALLINT       fInfoType,
                                 SQLPOINTER         rgbInfoValue,
                                 SQLSMALLINT        cbInfoValueMax,
                                 SQLSMALLINT    *pcbInfoValue)
        {
            if(this->m_func_SQLGetInfoW)
                return this->m_func_SQLGetInfoW(hdbc, fInfoType, rgbInfoValue, cbInfoValueMax,
                                                pcbInfoValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLRowCount(SQLHSTMT StatementHandle, 
                                 SQLLEN* RowCount)
        {
            if(this->m_func_SQLRowCount)
                return this->m_func_SQLRowCount(StatementHandle, RowCount);
            else
                throw LibFunctionException(__FUNCTION__);
        }



// Core
    inline SQLRETURN SQLParamData(SQLHSTMT StatementHandle,
                                  SQLPOINTER *Value)

        {
            if(this->m_func_SQLParamData)
                return this->m_func_SQLParamData(StatementHandle, Value);
            else
                throw LibFunctionException(__FUNCTION__);
        }


// Core
    inline SQLRETURN SQLPutData(SQLHSTMT StatementHandle,
                                SQLPOINTER Data,
                                SQLLEN StrLen_or_Ind)
        {
            if(this->m_func_SQLPutData)
                return this->m_func_SQLPutData(StatementHandle, Data, StrLen_or_Ind);
            else
                throw LibFunctionException(__FUNCTION__);

        }



    inline SQLRETURN SQLColAttributeA(
        SQLHSTMT     StatementHandle,
        SQLUSMALLINT     ColumnNumber,
        SQLUSMALLINT     FieldIdentifier,
        SQLPOINTER     CharacterAttributePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *     StringLengthPtr,
        SQLPOINTER     NumericAttributePtr)
        {
            if(this->m_func_SQLColAttributeA)
                return this->m_func_SQLColAttributeA(StatementHandle, ColumnNumber, FieldIdentifier,
                                                     CharacterAttributePtr, BufferLength, StringLengthPtr, NumericAttributePtr);
            
            else
                throw LibFunctionException(__FUNCTION__);

        }

    inline SQLRETURN SQLColAttributeW(
        SQLHSTMT     StatementHandle,
        SQLUSMALLINT     ColumnNumber,
        SQLUSMALLINT     FieldIdentifier,
        SQLPOINTER     CharacterAttributePtr,
        SQLSMALLINT     BufferLength,
        SQLSMALLINT *     StringLengthPtr,
        SQLPOINTER     NumericAttributePtr)
        {
            if(this->m_func_SQLColAttributeW)
                return this->m_func_SQLColAttributeW(StatementHandle, ColumnNumber, FieldIdentifier,
                                                     CharacterAttributePtr, BufferLength, StringLengthPtr, NumericAttributePtr);
            else
                throw LibFunctionException(__FUNCTION__);
        }



    inline SQLRETURN SQLGetStmtAttrA(SQLHSTMT           hstmt,
                                     SQLINTEGER         fAttribute,
                                     SQLPOINTER         rgbValue,
                                     SQLINTEGER         cbValueMax,
                                     SQLINTEGER     *pcbValue)
        {
            if(this->m_func_SQLGetStmtAttrA)
                return this->m_func_SQLGetStmtAttrA(hstmt, fAttribute, rgbValue, cbValueMax, pcbValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }


    inline SQLRETURN SQLGetStmtAttrW(SQLHSTMT           hstmt,
                                     SQLINTEGER         fAttribute,
                                     SQLPOINTER         rgbValue,
                                     SQLINTEGER         cbValueMax,
                                     SQLINTEGER     *pcbValue)
        {
            if(this->m_func_SQLGetStmtAttrW)
                return this->m_func_SQLGetStmtAttrW(hstmt, fAttribute, rgbValue, cbValueMax, pcbValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLGetDescFieldA(SQLHDESC           hdesc,
                                      SQLSMALLINT        iRecord,
                                      SQLSMALLINT        iField,
                                      SQLPOINTER         rgbValue,
                                      SQLINTEGER		   cbValueMax,
                                      SQLINTEGER     *pcbValue)
        {
            if(this->m_func_SQLGetDescFieldA)
                return this->m_func_SQLGetDescFieldA(hdesc, iRecord, iField, rgbValue, cbValueMax, pcbValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLGetDescFieldW(SQLHDESC           hdesc,
                                      SQLSMALLINT        iRecord,
                                      SQLSMALLINT        iField,
                                      SQLPOINTER         rgbValue,
                                      SQLINTEGER		   cbValueMax,
                                      SQLINTEGER     *pcbValue)
        {
            if(this->m_func_SQLGetDescFieldW)
                return this->m_func_SQLGetDescFieldW(hdesc, iRecord, iField, rgbValue, cbValueMax, pcbValue);
            else
                throw LibFunctionException(__FUNCTION__);
        }


    inline SQLRETURN SQLSetDescFieldA(SQLHDESC DescriptorHandle,
                                      SQLSMALLINT RecNumber, 
                                      SQLSMALLINT FieldIdentifier,
                                      SQLPOINTER Value, 
                                      SQLINTEGER BufferLength)
        {
            if(this->m_func_SQLSetDescFieldA)
                return this->m_func_SQLSetDescFieldA(DescriptorHandle, RecNumber, FieldIdentifier, Value, BufferLength);
            else
                throw LibFunctionException(__FUNCTION__);
        }   

    inline SQLRETURN SQLSetDescFieldW(SQLHDESC DescriptorHandle,
                                      SQLSMALLINT RecNumber, 
                                      SQLSMALLINT FieldIdentifier,
                                      SQLPOINTER Value, 
                                      SQLINTEGER BufferLength)
        {
            if(this->m_func_SQLSetDescFieldW)
                return this->m_func_SQLSetDescFieldW(DescriptorHandle, RecNumber, FieldIdentifier, Value, BufferLength);
            else
                throw LibFunctionException(__FUNCTION__);
        }   

    inline SQLRETURN SQLDescribeParam(SQLHSTMT           hstmt,
                                      SQLUSMALLINT       ipar,
                                      SQLSMALLINT     *pfSqlType,
                                      SQLULEN         *pcbParamDef,
                                      SQLSMALLINT     *pibScale,
                                      SQLSMALLINT     *pfNullable)
        {
            if(this->m_func_SQLDescribeParam)
                return this->m_func_SQLDescribeParam(hstmt, ipar, pfSqlType, pcbParamDef, pibScale, pfNullable);
            else
                throw LibFunctionException(__FUNCTION__);

        }

    inline SQLRETURN SQLGetFunctions(SQLHDBC ConnectionHandle,
                                     SQLUSMALLINT FunctionId, SQLUSMALLINT *Supported)
        {
            if(this->m_func_SQLGetFunctions)
                return this->m_func_SQLGetFunctions(ConnectionHandle, FunctionId, Supported);
            else
                throw LibFunctionException(__FUNCTION__);
        }


    inline SQLRETURN SQLGetDescRecA(SQLHDESC     DescriptorHandle,
                                    SQLSMALLINT     RecNumber,
                                    SQLCHAR *     Name,
                                    SQLSMALLINT     BufferLength,
                                    SQLSMALLINT *     StringLengthPtr,
                                    SQLSMALLINT *     TypePtr,
                                    SQLSMALLINT *     SubTypePtr,
                                    SQLINTEGER *     LengthPtr,
                                    SQLSMALLINT *     PrecisionPtr,
                                    SQLSMALLINT *     ScalePtr,
                                    SQLSMALLINT *     NullablePtr)
        {
            if(this->m_func_SQLGetDescRecA)
                return this->m_func_SQLGetDescRecA(DescriptorHandle, RecNumber, Name,
                                                   BufferLength, StringLengthPtr,
                                                   TypePtr, SubTypePtr, LengthPtr,
                                                   PrecisionPtr, ScalePtr, NullablePtr);
            else
                throw LibFunctionException(__FUNCTION__);
        }

    inline SQLRETURN SQLGetDescRecW(SQLHDESC     DescriptorHandle,
                                    SQLSMALLINT     RecNumber,
                                    SQLWCHAR *     Name,
                                    SQLSMALLINT     BufferLength,
                                    SQLSMALLINT *     StringLengthPtr,
                                    SQLSMALLINT *     TypePtr,
                                    SQLSMALLINT *     SubTypePtr,
                                    SQLINTEGER *     LengthPtr,
                                    SQLSMALLINT *     PrecisionPtr,
                                    SQLSMALLINT *     ScalePtr,
                                    SQLSMALLINT *     NullablePtr)
        {
            if(this->m_func_SQLGetDescRecW)
                return this->m_func_SQLGetDescRecW(DescriptorHandle, RecNumber, Name,
                                                   BufferLength, StringLengthPtr,
                                                   TypePtr, SubTypePtr, LengthPtr,
                                                   PrecisionPtr, ScalePtr, NullablePtr);
            else
                throw LibFunctionException(__FUNCTION__);
        }

inline SQLRETURN SQLEndTran(SQLSMALLINT HandleType,
                                             SQLHANDLE Handle,
                                             SQLSMALLINT CompletionType)
{
            if(this->m_func_SQLEndTran)
                return this->m_func_SQLEndTran(HandleType, Handle, CompletionType);
            else
                throw LibFunctionException(__FUNCTION__);
}



    inline SQLRETURN SQLTablesA(SQLHSTMT     StatementHandle,
                                             SQLCHAR *     CatalogName,
                                             SQLSMALLINT     NameLength1,
                                             SQLCHAR *     SchemaName,
                                             SQLSMALLINT     NameLength2,
                                             SQLCHAR *     TableName,
                                             SQLSMALLINT     NameLength3,
                                             SQLCHAR *     TableType,
                                             SQLSMALLINT     NameLength4)
        {
            if(this->m_func_SQLTablesA)
                return this->m_func_SQLTablesA(StatementHandle, CatalogName, NameLength1,
                                               SchemaName, NameLength2, TableName,
                                               NameLength3, TableType, NameLength4);
            else
                throw LibFunctionException(__FUNCTION__);
        }


    inline SQLRETURN SQLTablesW(SQLHSTMT     StatementHandle,
                                             SQLWCHAR *     CatalogName,
                                             SQLSMALLINT     NameLength1,
                                             SQLWCHAR *     SchemaName,
                                             SQLSMALLINT     NameLength2,
                                             SQLWCHAR *     TableName,
                                             SQLSMALLINT     NameLength3,
                                             SQLWCHAR *     TableType,
                                             SQLSMALLINT     NameLength4)
        {
            if(this->m_func_SQLTablesW)
                return this->m_func_SQLTablesW(StatementHandle, CatalogName, NameLength1,
                                               SchemaName, NameLength2, TableName,
                                               NameLength3, TableType, NameLength4);
            else
                throw LibFunctionException(__FUNCTION__);
        }


    inline SQLRETURN SQLColumnsA(SQLHSTMT     StatementHandle,
                                              SQLCHAR *     CatalogName,
                                              SQLSMALLINT     NameLength1,
                                              SQLCHAR *     SchemaName,
                                              SQLSMALLINT     NameLength2,
                                              SQLCHAR *     TableName,
                                              SQLSMALLINT     NameLength3,
                                              SQLCHAR *     ColumnName,
                                              SQLSMALLINT     NameLength4)
    {
            if(this->m_func_SQLColumnsA)
                return this->m_func_SQLColumnsA(StatementHandle, CatalogName, NameLength1,
                                               SchemaName, NameLength2, TableName,
                                               NameLength3, ColumnName, NameLength4);
            else
                throw LibFunctionException(__FUNCTION__);
    }

    inline SQLRETURN SQLColumnsW(SQLHSTMT     StatementHandle,
                                              SQLWCHAR *     CatalogName,
                                              SQLSMALLINT     NameLength1,
                                              SQLWCHAR *     SchemaName,
                                              SQLSMALLINT     NameLength2,
                                              SQLWCHAR *     TableName,
                                              SQLSMALLINT     NameLength3,
                                              SQLWCHAR *     ColumnName,
                                              SQLSMALLINT     NameLength4)
    {
            if(this->m_func_SQLColumnsW)
                return this->m_func_SQLColumnsW(StatementHandle, CatalogName, NameLength1,
                                               SchemaName, NameLength2, TableName,
                                               NameLength3, ColumnName, NameLength4);
            else
                throw LibFunctionException(__FUNCTION__);
    }


/*
  inline int sqlite3_open_v2(const char *filename, sqlite3 **ppDb, int flags, const char *zVfs)
  {
  if(this->m_func_sqlite3_open_v2)
  return this->m_func_sqlite3_open_v2(filename, ppDb, flags, zVfs);
  else
  throw DALELibFuncNullPtr();
  }



  inline int sqlite3_step(sqlite3_stmt* stmt)
  {
  if(this->m_func_sqlite3_step)
  return this->m_func_sqlite3_step(stmt);
  else
  throw DALELibFuncNullPtr();
  }

  inline const char* sqlite3_libversion(void)
  {
  if(this->m_func_sqlite3_libversion)
  return this->m_func_sqlite3_libversion();
  else
  throw DALELibFuncNullPtr();
  }


  inline int sqlite3_close(::sqlite3* db)
  {
  if(this->m_func_sqlite3_close)
  return this->m_func_sqlite3_close(db);
  else
  throw DALELibFuncNullPtr();
  }


  inline int sqlite3_finalize(sqlite3_stmt *pStmt)
  {
  if(this->m_func_sqlite3_finalize)
  return this->m_func_sqlite3_finalize(pStmt);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte,       
  sqlite3_stmt **ppStmt, const char **pzTail)
  {
  if(this->m_func_sqlite3_prepare_v2)
  return this->m_func_sqlite3_prepare_v2(db, zSql, nByte, ppStmt, pzTail);
  else
  throw DALELibFuncNullPtr();
  }

  inline void *sqlite3_aggregate_context(sqlite3_context* context, int nBytes)
  {
  if(this->m_func_sqlite3_aggregate_context)
  return this->m_func_sqlite3_aggregate_context(context, nBytes);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_auto_extension(void (*xEntryPoint)(void))
  {
  if(this->m_func_sqlite3_auto_extension)
  return this->m_func_sqlite3_auto_extension(xEntryPoint);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_blob(sqlite3_stmt* stmt, int col, const void* value, int n, void(*dtor)(void*))
  {
  if(this->m_func_sqlite3_bind_blob)
  return this->m_func_sqlite3_bind_blob(stmt, col, value, n, dtor);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_double(sqlite3_stmt* stmt, int col, double value)
  {
  if(this->m_func_sqlite3_bind_double)
  return this->m_func_sqlite3_bind_double(stmt, col, value);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_int(sqlite3_stmt* stmt, int col, int value)
  {
  if(this->m_func_sqlite3_bind_int)
  return this->m_func_sqlite3_bind_int(stmt, col, value);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_int64(sqlite3_stmt* stmt, int col, sqlite3_int64 value)
  {
  if(this->m_func_sqlite3_bind_int64)
  return this->m_func_sqlite3_bind_int64(stmt, col, value);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_null(sqlite3_stmt* stmt, int col)
  {
  if(this->m_func_sqlite3_bind_null)
  return this->m_func_sqlite3_bind_null(stmt, col);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_text(sqlite3_stmt* stmt, int col, const char* value, int n, void(*dtor)(void*))
  {
  if(this->m_func_sqlite3_bind_text)
  return this->m_func_sqlite3_bind_text(stmt, col, value, n, dtor);
  else
  throw DALELibFuncNullPtr();
  }


  inline int sqlite3_bind_value(sqlite3_stmt* stmt, int col, const sqlite3_value* value)
  {
  if(this->m_func_sqlite3_bind_value)
  return this->m_func_sqlite3_bind_value(stmt, col, value);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_zeroblob(sqlite3_stmt* stmt, int col, int n)
  {
  if(this->m_func_sqlite3_bind_zeroblob)
  return this->m_func_sqlite3_bind_zeroblob(stmt, col, n);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_parameter_count(sqlite3_stmt* stmt)
  {
  if(this->m_func_sqlite3_bind_parameter_count)
  return this->m_func_sqlite3_bind_parameter_count(stmt);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_bind_parameter_index(sqlite3_stmt* stmt, const char *zName)
  {
  if(this->m_func_sqlite3_bind_parameter_index)
  return this->m_func_sqlite3_bind_parameter_index(stmt, zName);
  else
  throw DALELibFuncNullPtr();
  }

  inline const char *sqlite3_bind_parameter_name(sqlite3_stmt* stmt, int col)
  {
  if(this->m_func_sqlite3_bind_parameter_name)
  return this->m_func_sqlite3_bind_parameter_name(stmt, col);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_blob_bytes(sqlite3_blob *blob)
  {
  if(this->m_func_sqlite3_blob_bytes)
  return this->m_func_sqlite3_blob_bytes(blob);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_blob_close(sqlite3_blob *blob)
  {
  if(this->m_func_sqlite3_blob_close)
  return this->m_func_sqlite3_blob_close(blob);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_blob_open(sqlite3* db, const char *zDb, const char *zTable, const char *zColumn,
  sqlite3_int64 iRow, int flags, sqlite3_blob **ppBlob)
  {
  if(this->m_func_sqlite3_blob_open)
  return this->m_func_sqlite3_blob_open(db, zDb, zTable, zColumn, iRow, flags, ppBlob);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_blob_read(sqlite3_blob *blob, void *Z, int N, int iOffset)
  {
  if(this->m_func_sqlite3_blob_read)
  return this->m_func_sqlite3_blob_read(blob, Z, N, iOffset);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_blob_write(sqlite3_blob *blob, const void *z, int n, int iOffset)
  {
  if(this->m_func_sqlite3_blob_write)
  return this->m_func_sqlite3_blob_write(blob, z, n, iOffset);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_busy_handler(sqlite3 *db, int(*handler)(void*,int), void *arg)
  {
  if(this->m_func_sqlite3_busy_handler)
  return this->m_func_sqlite3_busy_handler(db, handler, arg);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_busy_timeout(sqlite3* db, int ms)
  {
  if(this->m_func_sqlite3_busy_timeout)
  return this->m_func_sqlite3_busy_timeout(db, ms);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_changes(sqlite3* db)
  {
  if(this->m_func_sqlite3_changes)
  return this->m_func_sqlite3_changes(db);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_clear_bindings(sqlite3_stmt* stmt)
  {
  if(this->m_func_sqlite3_clear_bindings)
  return this->m_func_sqlite3_clear_bindings(stmt);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_collation_needed(sqlite3* db, void* a, 
  void(*b)(void*,sqlite3*,int eTextRep,const char*))
  {
  if(this->m_func_sqlite3_collation_needed)
  return this->m_func_sqlite3_collation_needed(db, a, b);
  else
  throw DALELibFuncNullPtr();
  }


  inline const void *sqlite3_column_blob(sqlite3_stmt* stmt, int iCol)
  {
  if(this->m_func_sqlite3_column_blob)
  return this->m_func_sqlite3_column_blob(stmt, iCol);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_column_bytes(sqlite3_stmt* stmt, int iCol)
  {
  if(this->m_func_sqlite3_column_bytes)
  return this->m_func_sqlite3_column_bytes(stmt, iCol);
  else
  throw DALELibFuncNullPtr();
  }


  inline double sqlite3_column_double(sqlite3_stmt* stmt, int iCol)
  {
  if(this->m_func_sqlite3_column_double)
  return this->m_func_sqlite3_column_double(stmt, iCol);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_column_int(sqlite3_stmt* stmt, int iCol)
  {
  if(this->m_func_sqlite3_column_int)
  return this->m_func_sqlite3_column_int(stmt, iCol);
  else
  throw DALELibFuncNullPtr();
  }

  inline sqlite3_int64 sqlite3_column_int64(sqlite3_stmt* stmt, int iCol)
  {
  if(this->m_func_sqlite3_column_int64)
  return this->m_func_sqlite3_column_int64(stmt, iCol);
  else
  throw DALELibFuncNullPtr();
  }

  inline const unsigned char *sqlite3_column_text(sqlite3_stmt* stmt, int iCol)
  {
  if(this->m_func_sqlite3_column_text)
  return this->m_func_sqlite3_column_text(stmt, iCol);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_column_type(sqlite3_stmt *stmt, int iCol)
  {
  if(this->m_func_sqlite3_column_type)
  return this->m_func_sqlite3_column_type(stmt, iCol);
  else
  throw DALELibFuncNullPtr();
  }

  inline sqlite3_value *sqlite3_column_value(sqlite3_stmt *stmt, int iCol)
  {
  if(this->m_func_sqlite3_column_value)
  return this->m_func_sqlite3_column_value(stmt, iCol);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_column_count(sqlite3_stmt *pStmt)
  {
  if(this->m_func_sqlite3_column_count)
  return this->m_func_sqlite3_column_count(pStmt);
  else
  throw DALELibFuncNullPtr();
  }

  inline const char *sqlite3_column_name(sqlite3_stmt *stmt, int N)
  {
  if(this->m_func_sqlite3_column_name)
  return this->m_func_sqlite3_column_name(stmt, N);
  else
  throw DALELibFuncNullPtr();
  }
*/
/*
  inline const char *sqlite3_column_database_name(sqlite3_stmt *stmt,int col)
  {
  if(this->m_func_sqlite3_database_name)
  return this->m_func_sqlite3_column_database_name(stmt, col);
  else
  throw DALELibFuncNullPtr();
  }
*/
/*
  inline const char *sqlite3_column_table_name(sqlite3_stmt *stmt,int col)
  {
  if(this->m_func_sqlite3_column_table_name)
  return this->m_func_sqlite3_column_table_name(stmt, col);
  else
  throw DALELibFuncNullPtr();
  }

  inline const char *sqlite3_column_origin_name(sqlite3_stmt* stmt,int col)
  {
  if(this->m_func_sqlite3_column_origin_name)
  return this->m_func_sqlite3_column_origin_name(stmt, col);
  else
  throw DALELibFuncNullPtr();
  }

  inline const char *sqlite3_column_decltype(sqlite3_stmt *stmt, int col)
  {
  if(this->m_func_sqlite3_column_decltype)
  return this->m_func_sqlite3_column_decltype(stmt, col);
  else
  throw DALELibFuncNullPtr();
  }

  inline void *sqlite3_commit_hook(sqlite3*db, int(*hook)(void*), void*arg)
  {
  if(this->m_func_sqlite3_commit_hook)
  return this->m_func_sqlite3_commit_hook(db, hook, arg);
  else
  throw DALELibFuncNullPtr();
  }

  inline void *sqlite3_rollback_hook(sqlite3*db, void(*hook)(void *), void*arg)
  {
  if(this->m_func_sqlite3_rollback_hook)
  return this->m_func_sqlite3_rollback_hook(db, hook, arg);
  else
  throw DALELibFuncNullPtr();
  }


  inline int sqlite3_complete(const char *sql)
  {
  if(this->m_func_sqlite3_complete)
  return this->m_func_sqlite3_complete(sql);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_create_collation( sqlite3*db, const char *zName, int eTextRep, 
  void* a, int(*xCompare)(void*,int,const void*,int,const void*))
  {
  if(this->m_func_sqlite3_create_collation)
  return this->m_func_sqlite3_create_collation(db, zName, eTextRep, a, xCompare);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_create_collation_v2(sqlite3*db, const char *zName, int eTextRep, void* a,
  int(*xCompare)(void*,int,const void*,int,const void*),
  void(*xDestroy)(void*))
  {
  if(this->m_func_sqlite3_create_collation_v2)
  return this->m_func_sqlite3_create_collation_v2(db, zName, eTextRep, a, xCompare, xDestroy);
  else
  throw DALELibFuncNullPtr();
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
  throw DALELibFuncNullPtr();
  }


  inline int sqlite3_data_count(sqlite3_stmt *pStmt)
  {
  if(this->m_func_sqlite3_data_count)
  return this->m_func_sqlite3_data_count(pStmt);
  else
  throw DALELibFuncNullPtr();
  }

  inline sqlite3 *sqlite3_db_handle(sqlite3_stmt* stmt)
  {
  if(this->m_func_sqlite3_db_handle)
  return this->m_func_sqlite3_db_handle(stmt);
  else
  throw DALELibFuncNullPtr();
  }


  inline int sqlite3_errcode(sqlite3 *db)
  {
  if(this->m_func_sqlite3_errcode)
  return this->m_func_sqlite3_errcode(db);
  else
  throw DALELibFuncNullPtr();
  }

  inline int sqlite3_extended_errcode(sqlite3 *db)
  {
  if(this->m_func_sqlite3_extended_errcode)
  return this->m_func_sqlite3_extended_errcode(db);
  else
  throw DALELibFuncNullPtr();
  }

  inline const char *sqlite3_errmsg(sqlite3* db)
  {
  if(this->m_func_sqlite3_errmsg)
  return this->m_func_sqlite3_errmsg(db);
  else
  throw DALELibFuncNullPtr();
  }


  inline int sqlite3_extended_result_codes(sqlite3*db, int onoff)
  {
  if(this->m_func_sqlite3_extended_result_code)
  return this->m_func_sqlite3_extended_result_code(db, onoff);
  else
  throw DALELibFuncNullPtr();
  }
*/
    /*
      inline int sqlite3_get_autocommit(sqlite3* db)
      {
      if(this->m_func_sqlite3_get_autocommit)
      return this->m_func_sqlite3_get_autocommit(db);
      else
      throw DALELibFuncNullPtr();
      }


      inline void sqlite3_interrupt(sqlite3*db)
      {
      if(this->m_func_sqlite3_interrupt)
      return this->m_func_sqlite3_interrupt(db);
      else
      throw DALELibFuncNullPtr();
      }

      inline sqlite3_int64 sqlite3_last_insert_rowid(sqlite3*db)
      {
      if(this->m_func_sqlite3_last_insert_rowid)
      return this->m_func_sqlite3_last_insert_rowid(db);
      else
      throw DALELibFuncNullPtr();
      }

      inline int sqlite3_limit(sqlite3*db, int id, int newVal)
      {
      if(this->m_func_sqlite3_limit)
      return this->m_func_sqlite3_limit(db, id, newVal);
      else
      throw DALELibFuncNullPtr();
      }

      inline sqlite3_stmt *sqlite3_next_stmt(sqlite3 *pDb, sqlite3_stmt *pStmt)
      {
      if(this->m_func_sqlite3_next_stmt)
      return this->m_func_sqlite3_next_stmt(pDb, pStmt);
      else
      throw DALELibFuncNullPtr();
      }

      inline void sqlite3_progress_handler(sqlite3* db, int a, int(*handler)(void*), void*arg)
      {
      if(this->m_func_sqlite3_progress_handler)
      return this->m_func_sqlite3_progress_handler(db, a, handler, arg);
      else
      throw DALELibFuncNullPtr();
      }

      inline int sqlite3_reset(sqlite3_stmt *pStmt)
      {
      if(this->m_func_sqlite3_reset)
      return this->m_func_sqlite3_reset(pStmt);
      else
      throw DALELibFuncNullPtr();
      }

      inline const char *sqlite3_sql(sqlite3_stmt *pStmt)
      {
      if(this->m_func_sqlite3_sql)
      return this->m_func_sqlite3_sql(pStmt);
      else
      throw DALELibFuncNullPtr();
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
      throw DALELibFuncNullPtr();
      }

      inline int sqlite3_threadsafe(void)
      {
      if(this->m_func_sqlite3_threadsafe)
      return this->m_func_sqlite3_threadsafe();
      else
      throw DALELibFuncNullPtr();
      }

      inline int sqlite3_total_changes(sqlite3*db)
      {
      if(this->m_func_sqlite3_total_changes)
      return this->m_func_sqlite3_total_changes(db);
      else
      throw DALELibFuncNullPtr();
      }

      inline void *sqlite3_update_hook(sqlite3*db, 
      void(*a)(void *,int ,char const *,char const *,sqlite3_int64),
      void*arg)
      {
      if(this->m_func_sqlite3_update_hook)
      return this->m_func_sqlite3_update_hook(db, a, arg);
      else
      throw DALELibFuncNullPtr();
      }
    */                
};





DAL_NAMESPACE_END

#endif


