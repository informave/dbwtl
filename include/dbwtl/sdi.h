#ifndef INFORMAVE_DB_SDI_H
#define INFORMAVE_DB_SDI_H

#define SDI_API

//SDI SDI Reference
//SDI =============
//SDI Daniel Vogelbacher <daniel@vogelbacher.name>
//SDI

//SDI Indroduction
//SDI ------------
//SDI The SDI - Simple Data Interface - is an interface specification
//SDI for data driver implementations.
//SDI


//SDI Reference
//SDI ---------
//SDI 
//SDI Functions
//SDI ~~~~~~~~~

//SDI SDIAllocEnv()
//SDI ^^^^^^^^^^^^^
//SDI Allocates a new environment handle.
//SDI
//SDI Syntax
//SDI ++++++
//SDI [source,cpp]
//SDI ----------------------------------------------------------------------
//SDI sdi_code SDI_API SDIAllocEnv(SDIENV *env);
//SDI ----------------------------------------------------------------------
//SDI 
//SDI 
//SDI [options="header",cols="<2,<2,<5",frame="topbot"]
//SDI |===============================================================================
//SDI |Parameter     | Type               | Description
//SDI |'env'         | SDIENV*            | Pointer where the allocated handle should be stored.
//SDI |===============================================================================
//SDI
//SDI
//SDI Description
//SDI +++++++++++
//SDI 
//SDI Return value
//SDI ++++++++++++
//SDI 

#include <stdlib.h>


enum sdi_data_types
{
     SDI_TYPE_CHAR        = 1,  /*  signed 8 bit       */
     SDI_TYPE_UCHAR       = 2,  /*  unsigned 8 bit     */
     SDI_TYPE_SMALLINT    = 3,  /*  signed 16 bit      */
     SDI_TYPE_USMALLINT   = 4,  /*  unsigned 16 bit    */
     SDI_TYPE_INT         = 5,  /*  signed 32 bit      */
     SDI_TYPE_UINT        = 6,  /*  unsigned 32 bit    */
     SDI_TYPE_BIGINT      = 7,  /*  signed 64 bit      */
     SDI_TYPE_UBIGINT     = 8,  /*  unsigned 64 bit    */
     SDI_TYPE_NUMERIC     = 9,  /*  ???                */
     SDI_TYPE_BIT         = 10, /*  char               */
     SDI_TYPE_BOOL        = 11, /*  char               */
     SDI_TYPE_STRING      = 12, /*  char*              */
     SDI_TYPE_DATE        = 13, /*  sdadate_t          */
     SDI_TYPE_TIME        = 14, /*  sdatime_t          */
     SDI_TYPE_DATETIME    = 15, /*  sdadatetime_t      */
     SDI_TYPE_TIMESTAMP   = 16, /*  signed 32 bit      */
     SDI_TYPE_FLOAT       = 17, /*  double             */
     SDI_TYPE_REAL       = 17+232, /*  double             */
     SDI_TYPE_MEMO        = 18, /*  char*              */
     SDI_TYPE_BLOB        = 19, /*  void*              */
     SDI_TYPE_ARRAY,
     SDI_TYPE_CUSTOM
};
typedef enum sdi_data_types sditype_t;

#define SDI_NULL_VALUE 0
#define SDI_HAS_VALUE 1

typedef enum 
{
	SDI_SUCCESS,
	SDI_WARN,
	SDI_ERROR,
	SDI_DONE,
	SDI_INVAL_SIZE
} sdi_code;

#define SDI_NTS		-1
#define SDI_IS_INTEGER	-2


typedef void* SDISTMT;
typedef void* SDIDBC;
typedef void* SDIENV;



typedef struct
{
     short year, month, day;
} sdi_date_t;

typedef struct
{
     short hour, minute, second;
} sdi_time_t;

typedef struct
{
     int value;
} sdi_timestamp_t;


typedef struct
{
	char digits[64];
	unsigned short len;
	unsigned short frac;
	char sign;
} sdi_numeric_t;

typedef enum
{
     SDI_HANDLE_ENV,
     SDI_HANDLE_DBC,
     SDI_HANDLE_STMT
} sdi_handle_type;



enum sdi_diag_facility
{
     SDI_DIAG_INFO,
     SDI_DIAG_WARN,
     SDI_DIAG_ERR
};

//typedef void (*sdiWriteDiag_f)(enum sdi_diag_facility fac, const char msg);




const char* SDI_API SDIDiagText(void *handle, sdi_handle_type type);

sdi_code SDI_API SDIAllocEnv(SDIENV *env);

sdi_code SDI_API SDIDeallocEnv(SDIENV *env);

const char* SDI_API SDICharset(SDIENV env);


sdi_code SDI_API SDIAllocDbc(SDIENV env, SDIDBC *dbc);
sdi_code SDI_API SDIDeallocDbc(SDIDBC *dbc);


sdi_code SDI_API SDITraceCallback(SDIDBC dbc, void(*trace)(int level, const char *msg, void *arg), void *arg);


sdi_code SDI_API SDISetDbcOption(SDIDBC dbc, const char *option, const char *value, int sizeOrInd);

sdi_code SDI_API SDIConnect(SDIDBC dbc, const char *string);
sdi_code SDI_API SDIDisconnect(SDIDBC dbc);

// PRAGMA getinfo('sdi_attr')
const char* SDI_API SDIGetInfo(SDIDBC dbc, const char *infoname);
/*
 sdi_data_charset
 sdi_system_name
 sdi_driver_name
 sdi_write_support
 topspeed_foo
 */

sdi_code SDI_API SDIOpen(SDIDBC dbc, SDISTMT *stmt, const char *object, const char *schema);

sdi_code SDI_API SDIObjects(SDIDBC dbc, SDISTMT *stmt);



/*
 Just pass the 3 columns from SDIObjects() to SDIColumns()
 If the driver canot determine the catalog columns, the driver
 should open the requested object and read columns from SDIDescribeCol()
 */
sdi_code SDI_API SDIColumns(SDIDBC dbc, SDISTMT *stmt, const char *database, const char *schema, const char *object);

// result operations

sdi_code SDI_API SDIClose(SDISTMT *stmt);

sdi_code SDI_API SDIFetch(SDISTMT stmt);

sdi_code SDI_API SDIGetData(SDISTMT stmt, int colnum, void *buf, size_t bufsize,
				int *ind);


sdi_code SDI_API SDIGetLargeObject(SDISTMT stmt, int colnum, void **buf, size_t *size, int *ind);


sdi_code SDI_API SDIFree(void **ptr);


int SDI_API SDINumCols(SDISTMT stmt);

sdi_code SDI_API SDIDescribeCol(SDISTMT stmt, int colnum, size_t *size, const char **name,
	sditype_t *type);


#define SDI_CATALOG_COLUMNS 5
#define SDI_COLUMN_COLUMNS 7

// write support?
//

#endif



/*
// Local Variables:
// mode: C
// c-file-style: "k&r"
// indent-tabs-mode: nil
// End:
*/
