//
// driver_libsdi.hh - SDI wrapper
//
// Copyright (C)         informave.org
//   2012,               Daniel Vogelbacher <daniel@vogelbacher.name>
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
/// @brief SDI wrapper
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DBWTL_DAL_SDIDRIVER_HH
#define INFORMAVE_DBWTL_DAL_SDIDRIVER_HH

#include "../library_support.hh"
#include "dbwtl/dal/dal_interface.hh"

#include <memory>
#include "../../../include/dbwtl/sdi.h"


DAL_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
///
/// @brief SQLite 3 Driver
class SDIDrv : public DynamicLibrary
{

    typedef    const char* (*sdiapi_SDIDiagText)(void *handle, sdi_handle_type type);

    typedef    sdi_code (*sdiapi_SDIAllocEnv)(SDIENV *env);

    typedef     sdi_code (*sdiapi_SDIDeallocEnv)(SDIENV *env);

    typedef     const char* (*sdiapi_SDICharset)(SDIENV env);

typedef sdi_code (*sdiapi_SDIAllocDbc)(SDIENV env, SDIDBC *dbc);

    typedef sdi_code (*sdiapi_SDIDeallocDbc)(SDIDBC *dbc);

typedef sdi_code (*sdiapi_SDIConnect)(SDIDBC dbc, const char *string);
    
typedef sdi_code (*sdiapi_SDIDisconnect)(SDIDBC dbc);

typedef const char* (*sdiapi_SDIGetInfo)(SDIDBC dbc, const char *infoname);

typedef sdi_code (*sdiapi_SDIOpen)(SDIDBC dbc, SDISTMT *stmt, const char *object, const char *schema);

typedef sdi_code (*sdiapi_SDIObjects)(SDIDBC dbc, SDISTMT *stmt);

typedef sdi_code (*sdiapi_SDIColumns)(SDIDBC dbc, SDISTMT *stmt, const char *database, const char *schema, const char *object);

typedef sdi_code (*sdiapi_SDIClose)(SDISTMT *stmt);

typedef sdi_code (*sdiapi_SDIFetch)(SDISTMT stmt);

typedef sdi_code (*sdiapi_SDIGetData)(SDISTMT stmt, int colnum, void *buf, size_t bufsize, int *ind);

typedef sdi_code (*sdiapi_SDIGetLargeObject)(SDISTMT stmt, int colnum, void **buf, size_t *size, int *ind);

    typedef sdi_code (*sdiapi_SDIFree)(void **ptr);

typedef int (*sdiapi_SDINumCols)(SDISTMT stmt);

typedef sdi_code (*sdiapi_SDIDescribeCol)(SDISTMT stmt, int colnum, size_t *size, const char **name, sditype_t *type);



protected:
    sdiapi_SDIDiagText                      m_func_SDIDiagText;
    sdiapi_SDIAllocEnv                      m_func_SDIAllocEnv;
    sdiapi_SDIDeallocEnv                    m_func_SDIDeallocEnv;
    sdiapi_SDICharset                       m_func_SDICharset;
    sdiapi_SDIAllocDbc                      m_func_SDIAllocDbc;
    sdiapi_SDIDeallocDbc                     m_func_SDIDeallocDbc;
    sdiapi_SDIConnect                       m_func_SDIConnect;
    sdiapi_SDIDisconnect                    m_func_SDIDisconnect;
    sdiapi_SDIGetInfo                       m_func_SDIGetInfo;
    sdiapi_SDIOpen                  m_func_SDIOpen;
    sdiapi_SDIObjects                       m_func_SDIObjects;
    sdiapi_SDIColumns                       m_func_SDIColumns;
    sdiapi_SDIClose                 m_func_SDIClose;
    sdiapi_SDIFetch                 m_func_SDIFetch;
    sdiapi_SDIGetData                       m_func_SDIGetData;
    sdiapi_SDIGetLargeObject			m_func_SDIGetLargeObject;
    sdiapi_SDIFree                      m_func_SDIFree;
    sdiapi_SDINumCols                       m_func_SDINumCols;
    sdiapi_SDIDescribeCol                   m_func_SDIDescribeCol;


public:
    SDIDrv(String path)
        : DynamicLibrary(path.length() ? path.toSystemEncoding() : "/home/cytrinox/informave/topspeed/build/libsdi_tps.so"),
          m_func_SDIDiagText(0),
          m_func_SDIAllocEnv(0),
          m_func_SDIDeallocEnv(0),
          m_func_SDICharset(0),
          m_func_SDIAllocDbc(0),
          m_func_SDIDeallocDbc(0),
          m_func_SDIConnect(0),
          m_func_SDIDisconnect(0),
          m_func_SDIGetInfo(0),
          m_func_SDIOpen(0),
          m_func_SDIObjects(0),
          m_func_SDIColumns(0),
          m_func_SDIClose(0),
          m_func_SDIFetch(0),
          m_func_SDIGetData(0),
	  m_func_SDIGetLargeObject(0),
          m_func_SDIFree(0),
          m_func_SDINumCols(0),
          m_func_SDIDescribeCol(0)
    {
        this->getproc(this->m_func_SDIDiagText, "SDIDiagText");
        this->getproc(this->m_func_SDIAllocEnv, "SDIAllocEnv");
        this->getproc(this->m_func_SDIDeallocEnv, "SDIDeallocEnv");
        this->getproc(this->m_func_SDICharset, "SDICharset");
        this->getproc(this->m_func_SDIAllocDbc, "SDIAllocDbc");
        this->getproc(this->m_func_SDIDeallocDbc, "SDIDeallocDbc");
        this->getproc(this->m_func_SDIConnect, "SDIConnect");
        this->getproc(this->m_func_SDIDisconnect, "SDIDisconnect");
        this->getproc(this->m_func_SDIGetInfo, "SDIGetInfo");
        this->getproc(this->m_func_SDIOpen, "SDIOpen");
        this->getproc(this->m_func_SDIObjects, "SDIObjects");
        this->getproc(this->m_func_SDIColumns, "SDIColumns");
        this->getproc(this->m_func_SDIClose, "SDIClose");
        this->getproc(this->m_func_SDIFetch, "SDIFetch");
        this->getproc(this->m_func_SDIGetData, "SDIGetData");
	this->getproc(this->m_func_SDIGetLargeObject, "SDIGetLargeObject");
    this->getproc(this->m_func_SDIFree, "SDIFree");
        this->getproc(this->m_func_SDINumCols, "SDINumCols");
        this->getproc(this->m_func_SDIDescribeCol, "SDIDescribeCol");
    }


    virtual ~SDIDrv(void) { }



  inline   const char* SDIDiagText(void *handle, sdi_handle_type type)
    {
        if(this->m_func_SDIDiagText)
            return this->m_func_SDIDiagText(handle, type);
        else
            throw ex::missing_function(__FUNCTION__);
    }

  inline    sdi_code SDIAllocEnv(SDIENV *env)
    {
        if(this->m_func_SDIAllocEnv)
            return this->m_func_SDIAllocEnv(env);
        else
            throw ex::missing_function(__FUNCTION__);
    }

 inline     sdi_code SDIDeallocEnv(SDIENV *env)
    {
        if(this->m_func_SDIDeallocEnv)
            return this->m_func_SDIDeallocEnv(env);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline     const char* SDICharset(SDIENV env)
    {
        if(this->m_func_SDICharset)
            return this->m_func_SDICharset(env);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIAllocDbc(SDIENV env, SDIDBC *dbc)
    {
        if(this->m_func_SDIAllocDbc)
            return this->m_func_SDIAllocDbc(env, dbc);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIDeallocDbc(SDIDBC *dbc)
    {
        if(this->m_func_SDIDeallocDbc)
            return this->m_func_SDIDeallocDbc(dbc);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIConnect(SDIDBC dbc, const char *string)
    {
        if(this->m_func_SDIConnect)
            return this->m_func_SDIConnect(dbc, string);
        else
            throw ex::missing_function(__FUNCTION__);
    }
    
inline sdi_code SDIDisconnect(SDIDBC dbc)
    {
        if(this->m_func_SDIDisconnect)
            return this->m_func_SDIDisconnect(dbc);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline const char* SDIGetInfo(SDIDBC dbc, const char *infoname)
    {
        if(this->m_func_SDIGetInfo)
            return this->m_func_SDIGetInfo(dbc, infoname);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIOpen(SDIDBC dbc, SDISTMT *stmt, const char *object, const char *schema)
    {
        if(this->m_func_SDIOpen)
            return this->m_func_SDIOpen(dbc, stmt, object, schema);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIObjects(SDIDBC dbc, SDISTMT *stmt)
    {
        if(this->m_func_SDIObjects)
            return this->m_func_SDIObjects(dbc, stmt);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIColumns(SDIDBC dbc, SDISTMT *stmt, const char *database, const char *schema, const char *object)
    {
        if(this->m_func_SDIColumns)
            return this->m_func_SDIColumns(dbc, stmt, database, schema, object);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIClose(SDISTMT *stmt)
    {
        if(this->m_func_SDIClose)
            return this->m_func_SDIClose(stmt);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIFetch(SDISTMT stmt)
    {
        if(this->m_func_SDIFetch)
            return this->m_func_SDIFetch(stmt);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIGetData(SDISTMT stmt, int colnum, void *buf, size_t bufsize, int *ind)
    {
        if(this->m_func_SDIGetData)
            return this->m_func_SDIGetData(stmt, colnum, buf, bufsize, ind);
        else
            throw ex::missing_function(__FUNCTION__);
    }

inline sdi_code SDIGetLargeObject(SDISTMT stmt, int colnum, void **buf, size_t *size, int *ind)
{
        if(this->m_func_SDIGetLargeObject)
            return this->m_func_SDIGetLargeObject(stmt, colnum, buf, size, ind);
        else
            throw ex::missing_function(__FUNCTION__);
}


inline sdi_code SDIFree(void **ptr)
    {
        if(this->m_func_SDIFree)
            return this->m_func_SDIFree(ptr);
        else
            throw ex::missing_function(__FUNCTION__);
    }



inline int SDINumCols(SDISTMT stmt)
    {
        if(this->m_func_SDINumCols)
            return this->m_func_SDINumCols(stmt);
        else
            throw ex::missing_function(__FUNCTION__);
    }

    inline sdi_code SDIDescribeCol(SDISTMT stmt, int colnum, size_t *size, const char **name, sditype_t *type)
    {
        if(this->m_func_SDIDescribeCol)
            return this->m_func_SDIDescribeCol(stmt, colnum, size, name, type);
        else
            throw ex::missing_function(__FUNCTION__);
    }

                
};





DAL_NAMESPACE_END

#endif



//
// Local Variables:
// mode: C++
// c-file-style: "bsd"
// c-basic-offset: 4
// indent-tabs-mode: nil
// End:
//
