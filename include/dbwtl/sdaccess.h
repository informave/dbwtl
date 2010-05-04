/*
//TIT Simple Data Access API
//TIT ======================
//TIT Daniel Vogelbacher <daniel@vogelbacher.name>
//TIT 
*/

#include <stdlib.h>
#include <stdint.h>

typedef void* SDAHDBC;
typedef void* SDAHSTMT;
typedef void* SDAHANDLE;
typedef void* sdablob_t;


#define SDA_MAX_INFO_SIZE          1024
#define SDA_MAX_COLUMN_LEN         1024

#define SDA_NULL_VALUE             0
#define SDA_VALID_VALUE            1

enum sda_handle_types
{
     SDA_HANDLE_DBC,
     SDA_HANDLE_STMT
};
typedef enum sda_handle_types sdahandle_t;

enum sda_info_types
{
     SDA_INFO_VERSION           = 1, /* int       */
     SDA_INFO_CHARSET           = 23,
     SDA_INFO_DRV_NAME          = 2, /* char*     */
     SDA_INFO_API_VERSION       = 3, /* int       */
     SDA_INFO_CMD_SUPPORT,
     SDA_INFO_SQL_SUPPORT,
     SDA_INFO_READONLY_SOURCE
};
typedef enum sda_info_types sdainfo_t;

enum sda_data_types
{
     SDA_TYPE_CHAR        = 1,  /*  signed 8 bit       */
     SDA_TYPE_UCHAR       = 2,  /*  unsigned 8 bit     */
     SDA_TYPE_SMALLINT    = 3,  /*  signed 16 bit      */
     SDA_TYPE_USMALLINT   = 4,  /*  unsigned 16 bit    */
     SDA_TYPE_INT         = 5,  /*  signed 32 bit      */
     SDA_TYPE_UINT        = 6,  /*  unsigned 32 bit    */
     SDA_TYPE_BIGINT      = 7,  /*  signed 64 bit      */
     SDA_TYPE_UBIGINT     = 8,  /*  unsigned 64 bit    */
     SDA_TYPE_NUMERIC     = 9,  /*  ???                */
     SDA_TYPE_BIT         = 10, /*  char               */
     SDA_TYPE_BOOL        = 11, /*  char               */
     SDA_TYPE_STRING      = 12, /*  char*              */
     SDA_TYPE_DATE        = 13, /*  sdadate_t          */
     SDA_TYPE_TIME        = 14, /*  sdatime_t          */
     SDA_TYPE_DATETIME    = 15, /*  sdadatetime_t      */
     SDA_TYPE_TIMESTAMP   = 16, /*  signed 32 bit      */
     SDA_TYPE_FLOAT       = 17, /*  double             */
     SDA_TYPE_MEMO        = 18, /*  char*              */
     SDA_TYPE_BLOB        = 19, /*  void*              */
     SDA_TYPE_ARRAY,
     SDA_TYPE_UNKNOWN
};
typedef enum sda_data_types sdatype_t;

#define SDA_SUCCESS       0
#define SDA_SUCCESS_INFO  1
#define SDA_NO_DATA       2
#define SDA_ERROR        -1



enum sda_option_types
{
     SDA_OPTION_CHARSET
};
typedef enum sda_option_types sdaoption_t;


enum sda_error_types
{
     SDA_ERROR_TRUNC             =  1, /* */
     SDA_ERROR_NOT_CONNECTED     =  2, /* */
     SDA_ERROR_AUTH              =  3, /* */
     SDA_ERROR_READONLY          =  4, /* */
     SDA_ERROR_COLUMN_NOT_FOUND,
     SDA_EEROR_OBJECT_NOT_FOUND,
     SDA_ERROR_INVALID_DATATYPE,
     SDA_ERROR_NULL_PTR,
     SDA_ERROR_FUNCTION_SEQ,
     SDA_ERROR_INVALID_INFOID,
     SDA_ERROR_INVALID_CURSOR,
     SDA_ERROR_INVALID,
     SDA_ERROR_CONN
};
typedef enum sda_error_types sdaerror_t;


enum sda_action_types
{
     SDA_ACTION_ADD,
     SDA_ACTION_UPDATE,
     SDA_ACTION_DELETE
};
typedef enum sda_action_types sdaaction_t;


struct sda_column_type
{
     char name[1024];
     size_t length;
     // precision for numeric?
     sdatype_t type;
};
typedef struct sda_column_type sdacolumn_t;

struct sda_date_type
{
     short int year;
     short int month;
     short int day;
};
typedef struct sda_date_type sdadate_t;


struct sda_time_type
{
     short hour;
     short min;
     short sec;
     short frac;
};
typedef struct sda_time_type sdatime_t;


struct sda_decimal_type
{
     char value[32+1];

     uint16_t decimal_len;
     uint16_t fractional_len;
};
typedef struct sda_decimal_type sdadecimal_t;




/*
//FUNC.01 API Functions
//FUNC.01 -------------
//FUNC.01 
*/

/*
//INFO.01 
//INFO.01 SDAInfo() - Get information about the driver
//INFO.01 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//INFO.01 The SDAInfo() function returns information about the driver, such as
//INFO.01 name, version, features etc.
//INFO.01 
//INFO.01 The function returns SDA_SUCCESS or SDA_SUCCESS_INFO if the
//INFO.01 info value is valid. If the buffer is not large enough,
//INFO.01 the data is right-truncated and SDA_SUCCESS_INFO is returned.
//INFO.01 
//INFO.01 If the value id is invalid/unknown, SDA_ERROR is returned.
//INFO.01 
//INFO.01 Usage
//INFO.01 ^^^^^
//INFO.01 The user must allocate space for the value buffer and supply the
//INFO.01 buffer and buffersize to the function.
//INFO.01 The info identifier can be one of the following values:
//INFO.01  SDA_INFO_VERSION
//INFO.01  SDA_INFO_DRV_NAME
//INFO.01  SDA_INFO_API_VERSION
//INFO.01  *
*/
int SDAInfo(sdainfo_t    info,    /* info value identifier */
            void        *value,   /* OUT */
            size_t       size     /* size of the value buffer */
     );


int SDAInit(SDAHDBC *dbc);

int SDAFreeHandle(SDAHDBC dbc);

int SDABrowseConnect(SDAHDBC       dbc,   /* IN:  DBC Handle */
                     const char  **key,   /* OUT: Key name */
                     const char  **desc,  /* OUT: Description of the key */
                     const char  **def,   /* OUT: Default value */
                     int          *type   /* OUT: Datatype of the key */
     );

int SDAConnect(SDAHDBC dbc, 
               const char *dbcstr);

int SDADisconnect(SDAHDBC dbc);

int SDAGetOption(SDAHDBC dbc, 
                 sdaoption_t option, 
                 void *buf, 
                 size_t size); 

int SDASetOption(); // 

int SDAGetDiag(sdahandle_t handle_type,
               SDAHANDLE handle,
               char *message,
               size_t size,
               

int *errcode);

int SDAOpen(SDAHDBC dbc,
            SDAHSTMT *stmt,
            const char *catalog,
            const char *schema,
            const char *name); 


int SDAClose(SDAHSTMT stmt);

int SDAFetch(SDAHSTMT stmt);

int SDAGetData(SDAHSTMT stmt, 
               int column, 
               void *value, 
               size_t size, 
               int *len_ind); // blob handles?


int SDAReadBinary(SDAHSTMT stmt, 
                  sdablob_t handle, 
                  void *buffer, 
                  size_t size, 
                  int *len_ind);

int SDACloseBlob(sdablob_t handle);

int SDAObjects(SDAHDBC dbc, 
               SDAHSTMT *stmt); 

int SDAColumns(SDAHDBC dbc, 
               SDAHSTMT *stmt, 
               const char *catalog, 
               const char *schema, 
               const char *objname);

int SDADescribeColumn(SDAHSTMT    stmt,     /* IN:  Statement handle */
                      int         column,   /* IN:  Column number */
                      char       *name,     /* OUT: Buffer for column name */
                      size_t      bufsize,  /* IN:  Size of the buffer */
                      char      **charset,  /* OUT: Charset of the text column */
                      sdatype_t  *type      /* OUT: SDA Type of the column */
     );


int SDANumCols(SDAHSTMT stmt, 
               int *columns);

//int SDAIsNull(SDAHSTMT stmt, int *ind);

int SDACommand(SDAHDBC dbc, 
               const char *cmd); // OPTION_COMMAND_ENCODING!!!


/*
//PUTDATA.01 
//PUTDATA.01 If the driver has write support, data can be changed, deleted or added
//PUTDATA.01 with SDAPutData() and SDADone().
*/
int SDAPutData(SDAHSTMT stmt, 
               int column, 
               sdatype_t type, 
               void *data);

int SDADone(SDAHSTMT stmt, 
            sdaaction_t action);

int SDACreateObject(SDAHDBC dbc, 
                    const char *name, 
                    sdacolumn_t **definition, 
                    size_t array_size);



int SDADropObject(SDAHDBC dbc, 
                  const char *name);

int SDAGetTypes(SDAHDBC dbc, 
                SDAHSTMT *stmt);

/*
// Local Variables:
// mode: C
// c-file-style: "k&r"
// indent-tabs-mode: nil
// End:
*/

