//
// driver_libfbclient.hh - fbclient wrapper
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
/// @brief fbclient wrapper
/// @author Daniel Vogelbacher
/// @since 0.0.1


#ifndef INFORMAVE_DBWTL_DAL_FBCLIENTDRIVER_HH
#define INFORMAVE_DBWTL_DAL_FBCLIENTDRIVER_HH

#include "../library_support.hh"
#include "dbwtl/dal/dal_interface.hh"

#include <memory>
#include <ibase.h>

#if defined(DBWTL_ON_WIN32)
#define DBWTL_FBCLIENT_DEFAULT_LIB "fbclient.dll"
#elif defined(DBWTL_ON_UNIX)
#define DBWTL_FBCLIENT_DEFAULT_LIB "libfbclient.so"
#else
#error "Unsupported plattform"
#endif


DAL_NAMESPACE_BEGIN

//------------------------------------------------------------------------------
///
/// @brief SQLite 3 Driver
class FBClientDrv : public DynamicLibrary
{

    typedef ISC_STATUS (ISC_EXPORT *api__isc_attach_database)(ISC_STATUS*,
                                                   short,
                                                   const ISC_SCHAR*,
                                                   isc_db_handle*,
                                                   short,
                                                   const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_array_gen_sdl)(ISC_STATUS*,
                                                 const ISC_ARRAY_DESC*,
                                                 ISC_SHORT*,
                                                 ISC_UCHAR*,
                                                 ISC_SHORT*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_array_get_slice)(ISC_STATUS*,
                                                   isc_db_handle*,
                                                   isc_tr_handle*,
                                                   ISC_QUAD*,
                                                   const ISC_ARRAY_DESC*,
                                                   void*,
                                                   ISC_LONG*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_array_lookup_bounds)(ISC_STATUS*,
                                                       isc_db_handle*,
                                                       isc_tr_handle*,
                                                       const ISC_SCHAR*,
                                                       const ISC_SCHAR*,
                                                       ISC_ARRAY_DESC*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_array_lookup_desc)(ISC_STATUS*,
                                                     isc_db_handle*,
                                                     isc_tr_handle*,
                                                     const ISC_SCHAR*,
                                                     const ISC_SCHAR*,
                                                     ISC_ARRAY_DESC*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_array_set_desc)(ISC_STATUS*,
                                                  const ISC_SCHAR*,
                                                  const ISC_SCHAR*,
                                                  const short*,
                                                  const short*,
                                                  const short*,
                                                  ISC_ARRAY_DESC*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_array_put_slice)(ISC_STATUS*,
                                                   isc_db_handle*,
                                                   isc_tr_handle*,
                                                   ISC_QUAD*,
                                                   const ISC_ARRAY_DESC*,
                                                   void*,
                                                   ISC_LONG *);

    typedef void (ISC_EXPORT *api__isc_blob_default_desc)(ISC_BLOB_DESC*,
                                               const ISC_UCHAR*,
                                               const ISC_UCHAR*);


    typedef ISC_STATUS (ISC_EXPORT *api__isc_blob_gen_bpb)(ISC_STATUS*,
                                                const ISC_BLOB_DESC*,
                                                const ISC_BLOB_DESC*,
                                                unsigned short,
                                                ISC_UCHAR*,
                                                unsigned short*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_blob_info)(ISC_STATUS*,
                                             isc_blob_handle*,
                                             short,
                                             const ISC_SCHAR*,
                                             short,
                                             ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_blob_lookup_desc)(ISC_STATUS*,
                                                    isc_db_handle*,
                                                    isc_tr_handle*,
                                                    const ISC_UCHAR*,
                                                    const ISC_UCHAR*,
                                                    ISC_BLOB_DESC*,
                                                    ISC_UCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_blob_set_desc)(ISC_STATUS*,
                                                 const ISC_UCHAR*,
                                                 const ISC_UCHAR*,
                                                 short,
                                                 short,
                                                 short,
                                                 ISC_BLOB_DESC*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_cancel_blob)(ISC_STATUS *,
                                               isc_blob_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_cancel_events)(ISC_STATUS *,
                                                 isc_db_handle *,
                                                 ISC_LONG *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_close_blob)(ISC_STATUS *,
                                              isc_blob_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_commit_retaining)(ISC_STATUS *,
                                                    isc_tr_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_commit_transaction)(ISC_STATUS *,
                                                      isc_tr_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_create_blob)(ISC_STATUS*,
                                               isc_db_handle*,
                                               isc_tr_handle*,
                                               isc_blob_handle*,
                                               ISC_QUAD*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_create_blob2)(ISC_STATUS*,
                                                isc_db_handle*,
                                                isc_tr_handle*,
                                                isc_blob_handle*,
                                                ISC_QUAD*,
                                                short,
                                                const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_create_database)(ISC_STATUS*,
                                                   short,
                                                   const ISC_SCHAR*,
                                                   isc_db_handle*,
                                                   short,
                                                   const ISC_SCHAR*,
                                                   short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_database_info)(ISC_STATUS*,
                                                 isc_db_handle*,
                                                 short,
                                                 const ISC_SCHAR*,
                                                 short,
                                                 ISC_SCHAR*);

    typedef void (ISC_EXPORT *api__isc_decode_date)(const ISC_QUAD*,
                                         void*);

    typedef void (ISC_EXPORT *api__isc_decode_sql_date)(const ISC_DATE*,
                                             void*);

    typedef void (ISC_EXPORT *api__isc_decode_sql_time)(const ISC_TIME*,
                                             void*);

    typedef void (ISC_EXPORT *api__isc_decode_timestamp)(const ISC_TIMESTAMP*,
                                              void*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_detach_database)(ISC_STATUS *,
                                                   isc_db_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_drop_database)(ISC_STATUS *,
                                                 isc_db_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_allocate_statement)(ISC_STATUS *,
                                                           isc_db_handle *,
                                                           isc_stmt_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_alloc_statement2)(ISC_STATUS *,
                                                         isc_db_handle *,
                                                         isc_stmt_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_describe)(ISC_STATUS *,
                                                 isc_stmt_handle *,
                                                 unsigned short,
                                                 XSQLDA *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_describe_bind)(ISC_STATUS *,
                                                      isc_stmt_handle *,
                                                      unsigned short,
                                                      XSQLDA *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_exec_immed2)(ISC_STATUS*,
                                                    isc_db_handle*,
                                                    isc_tr_handle*,
                                                    unsigned short,
                                                    const ISC_SCHAR*,
                                                    unsigned short,
                                                    XSQLDA*,
                                                    XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_execute)(ISC_STATUS*,
                                                isc_tr_handle*,
                                                isc_stmt_handle*,
                                                unsigned short,
                                                XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_execute2)(ISC_STATUS*,
                                                 isc_tr_handle*,
                                                 isc_stmt_handle*,
                                                 unsigned short,
                                                 XSQLDA*,
                                                 XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_execute_immediate)(ISC_STATUS*,
                                                          isc_db_handle*,
                                                          isc_tr_handle*,
                                                          unsigned short,
                                                          const ISC_SCHAR*,
                                                          unsigned short,
                                                          XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_fetch)(ISC_STATUS *,
                                              isc_stmt_handle *,
                                              unsigned short,
                                              XSQLDA *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_finish)(isc_db_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_free_statement)(ISC_STATUS *,
                                                       isc_stmt_handle *,
                                                       unsigned short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_insert)(ISC_STATUS*,
                                               isc_stmt_handle*,
                                               unsigned short,
                                               XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_prepare)(ISC_STATUS*,
                                                isc_tr_handle*,
                                                isc_stmt_handle*,
                                                unsigned short,
                                                const ISC_SCHAR*,
                                                unsigned short,
                                                XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_set_cursor_name)(ISC_STATUS*,
                                                        isc_stmt_handle*,
                                                        const ISC_SCHAR*,
                                                        unsigned short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_sql_info)(ISC_STATUS*,
                                                 isc_stmt_handle*,
                                                 short,
                                                 const ISC_SCHAR*,
                                                 short,
                                                 ISC_SCHAR*);

    typedef void (ISC_EXPORT *api__isc_encode_date)(const void*,
                                         ISC_QUAD*);

    typedef void (ISC_EXPORT *api__isc_encode_sql_date)(const void*,
                                             ISC_DATE*);

    typedef void (ISC_EXPORT *api__isc_encode_sql_time)(const void*,
                                             ISC_TIME*);

    typedef void (ISC_EXPORT *api__isc_encode_timestamp)(const void*,
                                              ISC_TIMESTAMP*);

    typedef ISC_LONG  (ISC_EXPORT *api__isc_event_block)(ISC_UCHAR**,
                                                               ISC_UCHAR**,
                                                               unsigned short, ...);

    typedef void (ISC_EXPORT *api__isc_event_counts)(ISC_ULONG*,
                                          short,
                                          ISC_UCHAR*,
                                          const ISC_UCHAR *);



    typedef int (ISC_EXPORT *api__isc_modify_dpb)(ISC_SCHAR**,
                                       short*,
                                       unsigned short,
                                       const ISC_SCHAR*,
                                       short);

    typedef ISC_LONG (ISC_EXPORT *api__isc_free)(ISC_SCHAR *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_get_segment)(ISC_STATUS *,
                                               isc_blob_handle *,
                                               unsigned short *,
                                               unsigned short,
                                               ISC_SCHAR *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_get_slice)(ISC_STATUS*,
                                             isc_db_handle*,
                                             isc_tr_handle*,
                                             ISC_QUAD*,
                                             short,
                                             const ISC_SCHAR*,
                                             short,
                                             const ISC_LONG*,
                                             ISC_LONG,
                                             void*,
                                             ISC_LONG*);


/* CVC: This non-const signature is needed for compatibility, see gds.cpp. */
    typedef ISC_LONG (ISC_EXPORT *api__isc_interprete)(ISC_SCHAR*,
                                            ISC_STATUS**);


/* This const params version used in the engine and other places. */
    typedef ISC_LONG (ISC_EXPORT *api__fb_interpret)(ISC_SCHAR*,
                                          unsigned int,
                                          const ISC_STATUS**);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_open_blob)(ISC_STATUS*,
                                             isc_db_handle*,
                                             isc_tr_handle*,
                                             isc_blob_handle*,
                                             ISC_QUAD*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_open_blob2)(ISC_STATUS*,
                                              isc_db_handle*,
                                              isc_tr_handle*,
                                              isc_blob_handle*,
                                              ISC_QUAD*,
                                              ISC_USHORT,
                                              const ISC_UCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_prepare_transaction2)(ISC_STATUS*,
                                                        isc_tr_handle*,
                                                        ISC_USHORT,
                                                        const ISC_UCHAR*);

    typedef void (ISC_EXPORT *api__isc_print_sqlerror)(ISC_SHORT,
                                            const ISC_STATUS*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_print_status)(const ISC_STATUS*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_put_segment)(ISC_STATUS*,
                                               isc_blob_handle*,
                                               unsigned short,
                                               const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_put_slice)(ISC_STATUS*,
                                             isc_db_handle*,
                                             isc_tr_handle*,
                                             ISC_QUAD*,
                                             short,
                                             const ISC_SCHAR*,
                                             short,
                                             const ISC_LONG*,
                                             ISC_LONG,
                                             void*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_que_events)(ISC_STATUS*,
                                              isc_db_handle*,
                                              ISC_LONG*,
                                              short,
                                              const ISC_UCHAR*,
                                              ISC_EVENT_CALLBACK,
                                              void*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_rollback_retaining)(ISC_STATUS *,
                                                      isc_tr_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_rollback_transaction)(ISC_STATUS *,
                                                        isc_tr_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_start_multiple)(ISC_STATUS *,
                                                  isc_tr_handle *,
                                                  short,
                                                  void *);

    typedef ISC_STATUS  (ISC_EXPORT *api__isc_start_transaction)(ISC_STATUS *,
                                                                       isc_tr_handle *,
                                                                       short, ...);

    typedef ISC_LONG (ISC_EXPORT *api__isc_sqlcode)(const ISC_STATUS*);

    typedef void (ISC_EXPORT *api__isc_sql_interprete)(short,
                                            ISC_SCHAR *,
                                            short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_transaction_info)(ISC_STATUS*,
                                                    isc_tr_handle*,
                                                    short,
                                                    const ISC_SCHAR*,
                                                    short,
                                                    ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_transact_request)(ISC_STATUS*,
                                                    isc_db_handle*,
                                                    isc_tr_handle*,
                                                    unsigned short,
                                                    const ISC_SCHAR*,
                                                    unsigned short,
                                                    ISC_SCHAR*,
                                                    unsigned short,
                                                    ISC_SCHAR*);

    typedef ISC_LONG (ISC_EXPORT *api__isc_vax_integer)(const ISC_SCHAR*,
                                             short);

    typedef ISC_INT64 (ISC_EXPORT *api__isc_portable_integer)(const ISC_UCHAR*,
                                                   short);


    typedef ISC_STATUS (ISC_EXPORT *api__isc_add_user)(ISC_STATUS*, const USER_SEC_DATA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_delete_user)(ISC_STATUS*, const USER_SEC_DATA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_modify_user)(ISC_STATUS*, const USER_SEC_DATA*);



    typedef ISC_STATUS (ISC_EXPORT *api__isc_compile_request)(ISC_STATUS*,
                                                   isc_db_handle*,
                                                   isc_req_handle*,
                                                   short,
                                                   const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_compile_request2)(ISC_STATUS*,
                                                    isc_db_handle*,
                                                    isc_req_handle*,
                                                    short,
                                                    const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_ddl)(ISC_STATUS*,
                                       isc_db_handle*,
                                       isc_tr_handle*,
                                       short,
                                       const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_prepare_transaction)(ISC_STATUS*,
                                                       isc_tr_handle*);


    typedef ISC_STATUS (ISC_EXPORT *api__isc_receive)(ISC_STATUS*,
                                           isc_req_handle*,
                                           short,
                                           short,
                                           void*,
                                           short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_reconnect_transaction)(ISC_STATUS*,
                                                         isc_db_handle*,
                                                         isc_tr_handle*,
                                                         short,
                                                         const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_release_request)(ISC_STATUS*,
                                                   isc_req_handle*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_request_info)(ISC_STATUS*,
                                                isc_req_handle*,
                                                short,
                                                short,
                                                const ISC_SCHAR*,
                                                short,
                                                ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_seek_blob)(ISC_STATUS*,
                                             isc_blob_handle*,
                                             short,
                                             ISC_LONG,
                                             ISC_LONG*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_send)(ISC_STATUS*,
                                        isc_req_handle*,
                                        short,
                                        short,
                                        const void*,
                                        short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_start_and_send)(ISC_STATUS*,
                                                  isc_req_handle*,
                                                  isc_tr_handle*,
                                                  short,
                                                  short,
                                                  const void*,
                                                  short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_start_request)(ISC_STATUS *,
                                                 isc_req_handle *,
                                                 isc_tr_handle *,
                                                 short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_unwind_request)(ISC_STATUS *,
                                                  isc_tr_handle *,
                                                  short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_wait_for_event)(ISC_STATUS*,
                                                  isc_db_handle*,
                                                  short,
                                                  const ISC_UCHAR*,
                                                  ISC_UCHAR*);



    typedef ISC_STATUS (ISC_EXPORT *api__isc_close)(ISC_STATUS*,
                                         const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_declare)(ISC_STATUS*,
                                           const ISC_SCHAR*,
                                           const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_describe)(ISC_STATUS*,
                                            const ISC_SCHAR*,
                                            XSQLDA *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_describe_bind)(ISC_STATUS*,
                                                 const ISC_SCHAR*,
                                                 XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_execute)(ISC_STATUS*,
                                           isc_tr_handle*,
                                           const ISC_SCHAR*,
                                           XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_execute_immediate)(ISC_STATUS*,
                                                     isc_db_handle*,
                                                     isc_tr_handle*,
                                                     short*,
                                                     const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_fetch)(ISC_STATUS*,
                                         const ISC_SCHAR*,
                                         XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_open)(ISC_STATUS*,
                                        isc_tr_handle*,
                                        const ISC_SCHAR*,
                                        XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_prepare)(ISC_STATUS*,
                                           isc_db_handle*,
                                           isc_tr_handle*,
                                           const ISC_SCHAR*,
                                           short*,
                                           const ISC_SCHAR*,
                                           XSQLDA*);



    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_execute_m)(ISC_STATUS*,
                                                  isc_tr_handle*,
                                                  isc_stmt_handle*,
                                                  unsigned short,
                                                  const ISC_SCHAR*,
                                                  unsigned short,
                                                  unsigned short,
                                                  ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_execute2_m)(ISC_STATUS*,
                                                   isc_tr_handle*,
                                                   isc_stmt_handle*,
                                                   unsigned short,
                                                   const ISC_SCHAR*,
                                                   unsigned short,
                                                   unsigned short,
                                                   const ISC_SCHAR*,
                                                   unsigned short,
                                                   ISC_SCHAR*,
                                                   unsigned short,
                                                   unsigned short,
                                                   ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_execute_immediate_m)(ISC_STATUS*,
                                                            isc_db_handle*,
                                                            isc_tr_handle*,
                                                            unsigned short,
                                                            const ISC_SCHAR*,
                                                            unsigned short,
                                                            unsigned short,
                                                            const ISC_SCHAR*,
                                                            unsigned short,
                                                            unsigned short,
                                                            ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_exec_immed3_m)(ISC_STATUS*,
                                                      isc_db_handle*,
                                                      isc_tr_handle*,
                                                      unsigned short,
                                                      const ISC_SCHAR*,
                                                      unsigned short,
                                                      unsigned short,
                                                      const ISC_SCHAR*,
                                                      unsigned short,
                                                      unsigned short,
                                                      ISC_SCHAR*,
                                                      unsigned short,
                                                      ISC_SCHAR*,
                                                      unsigned short,
                                                      unsigned short,
                                                      ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_fetch_m)(ISC_STATUS*,
                                                isc_stmt_handle*,
                                                unsigned short,
                                                const ISC_SCHAR*,
                                                unsigned short,
                                                unsigned short,
                                                ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_insert_m)(ISC_STATUS*,
                                                 isc_stmt_handle*,
                                                 unsigned short,
                                                 const ISC_SCHAR*,
                                                 unsigned short,
                                                 unsigned short,
                                                 const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_prepare_m)(ISC_STATUS*,
                                                  isc_tr_handle*,
                                                  isc_stmt_handle*,
                                                  unsigned short,
                                                  const ISC_SCHAR*,
                                                  unsigned short,
                                                  unsigned short,
                                                  const ISC_SCHAR*,
                                                  unsigned short,
                                                  ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_dsql_release)(ISC_STATUS*,
                                                const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_close)(ISC_STATUS*,
                                                    const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_declare)(ISC_STATUS*,
                                                      const ISC_SCHAR*,
                                                      const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_describe)(ISC_STATUS*,
                                                       const ISC_SCHAR*,
                                                       unsigned short,
                                                       XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_describe_bind)(ISC_STATUS*,
                                                            const ISC_SCHAR*,
                                                            unsigned short,
                                                            XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_execute)(ISC_STATUS*,
                                                      isc_tr_handle*,
                                                      const ISC_SCHAR*,
                                                      unsigned short,
                                                      XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_execute2)(ISC_STATUS*,
                                                       isc_tr_handle*,
                                                       const ISC_SCHAR*,
                                                       unsigned short,
                                                       XSQLDA*,
                                                       XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_execute_immed)(ISC_STATUS*,
                                                            isc_db_handle*,
                                                            isc_tr_handle*,
                                                            unsigned short,
                                                            const ISC_SCHAR*,
                                                            unsigned short,
                                                            XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_fetch)(ISC_STATUS*,
                                                    const ISC_SCHAR*,
                                                    unsigned short,
                                                    XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_open)(ISC_STATUS*,
                                                   isc_tr_handle*,
                                                   const ISC_SCHAR*,
                                                   unsigned short,
                                                   XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_open2)(ISC_STATUS*,
                                                    isc_tr_handle*,
                                                    const ISC_SCHAR*,
                                                    unsigned short,
                                                    XSQLDA*,
                                                    XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_insert)(ISC_STATUS*,
                                                     const ISC_SCHAR*,
                                                     unsigned short,
                                                     XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_prepare)(ISC_STATUS*,
                                                      isc_db_handle*,
                                                      isc_tr_handle*,
                                                      const ISC_SCHAR*,
                                                      unsigned short,
                                                      const ISC_SCHAR*,
                                                      unsigned short,
                                                      XSQLDA*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_embed_dsql_release)(ISC_STATUS*,
                                                      const ISC_SCHAR*);


    typedef BSTREAM* (ISC_EXPORT *api__BLOB_open)(isc_blob_handle,
                                       ISC_SCHAR*,
                                       int);

    typedef int (ISC_EXPORT *api__BLOB_put)(ISC_SCHAR,
                                 BSTREAM*);

    typedef int (ISC_EXPORT *api__BLOB_close)(BSTREAM*);

    typedef int (ISC_EXPORT *api__BLOB_get)(BSTREAM*);

    typedef int (ISC_EXPORT *api__BLOB_display)(ISC_QUAD*,
                                     isc_db_handle,
                                     isc_tr_handle,
                                     const ISC_SCHAR*);

    typedef int (ISC_EXPORT *api__BLOB_dump)(ISC_QUAD*,
                                  isc_db_handle,
                                  isc_tr_handle,
                                  const ISC_SCHAR*);

    typedef int (ISC_EXPORT *api__BLOB_edit)(ISC_QUAD*,
                                  isc_db_handle,
                                  isc_tr_handle,
                                  const ISC_SCHAR*);

    typedef int (ISC_EXPORT *api__BLOB_load)(ISC_QUAD*,
                                  isc_db_handle,
                                  isc_tr_handle,
                                  const ISC_SCHAR*);

    typedef int (ISC_EXPORT *api__BLOB_text_dump)(ISC_QUAD*,
                                       isc_db_handle,
                                       isc_tr_handle,
                                       const ISC_SCHAR*);

    typedef int (ISC_EXPORT *api__BLOB_text_load)(ISC_QUAD*,
                                       isc_db_handle,
                                       isc_tr_handle,
                                       const ISC_SCHAR*);

    typedef BSTREAM* (ISC_EXPORT *api__Bopen)(ISC_QUAD*,
                                   isc_db_handle,
                                   isc_tr_handle,
                                   const ISC_SCHAR*);



    typedef ISC_LONG (ISC_EXPORT *api__isc_ftof)(const ISC_SCHAR*,
                                      const unsigned short,
                                      ISC_SCHAR*,
                                      const unsigned short);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_print_blr)(const ISC_SCHAR*,
                                             ISC_PRINT_CALLBACK,
                                             void*,
                                             short);

    typedef void (ISC_EXPORT *api__isc_set_debug)(int);

    typedef void (ISC_EXPORT *api__isc_qtoq)(const ISC_QUAD*,
                                  ISC_QUAD*);

    typedef void (ISC_EXPORT *api__isc_vtof)(const ISC_SCHAR*,
                                  ISC_SCHAR*,
                                  unsigned short);

    typedef void (ISC_EXPORT *api__isc_vtov)(const ISC_SCHAR*,
                                  ISC_SCHAR*,
                                  short);

    typedef int (ISC_EXPORT *api__isc_version)(isc_db_handle*,
                                    ISC_VERSION_CALLBACK,
                                    void*);

    typedef ISC_LONG (ISC_EXPORT *api__isc_reset_fpe)(ISC_USHORT);





    typedef ISC_STATUS (ISC_EXPORT *api__isc_service_attach)(ISC_STATUS*,
                                                  unsigned short,
                                                  const ISC_SCHAR*,
                                                  isc_svc_handle*,
                                                  unsigned short,
                                                  const ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_service_detach)(ISC_STATUS *,
                                                  isc_svc_handle *);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_service_query)(ISC_STATUS*,
                                                 isc_svc_handle*,
                                                 isc_resv_handle*,
                                                 unsigned short,
                                                 const ISC_SCHAR*,
                                                 unsigned short,
                                                 const ISC_SCHAR*,
                                                 unsigned short,
                                                 ISC_SCHAR*);

    typedef ISC_STATUS (ISC_EXPORT *api__isc_service_start)(ISC_STATUS*,
                                                 isc_svc_handle*,
                                                 isc_resv_handle*,
                                                 unsigned short,
                                                 const ISC_SCHAR*);



    typedef void (ISC_EXPORT *api__isc_get_client_version)( ISC_SCHAR  *);
    typedef int (ISC_EXPORT *api__isc_get_client_major_version)();
    typedef int (ISC_EXPORT *api__isc_get_client_minor_version)();




protected:

        api__isc_attach_database     m_func_isc_attach_database;
    api__isc_array_gen_sdl     m_func_isc_array_gen_sdl;
    api__isc_array_get_slice     m_func_isc_array_get_slice;
    api__isc_array_lookup_bounds     m_func_isc_array_lookup_bounds;
    api__isc_array_lookup_desc     m_func_isc_array_lookup_desc;
    api__isc_array_set_desc     m_func_isc_array_set_desc;
    api__isc_array_put_slice     m_func_isc_array_put_slice;
    api__isc_blob_default_desc     m_func_isc_blob_default_desc;
    api__isc_blob_gen_bpb     m_func_isc_blob_gen_bpb;
    api__isc_blob_info     m_func_isc_blob_info;
    api__isc_blob_lookup_desc     m_func_isc_blob_lookup_desc;
    api__isc_blob_set_desc     m_func_isc_blob_set_desc;
    api__isc_cancel_blob     m_func_isc_cancel_blob;
    api__isc_cancel_events     m_func_isc_cancel_events;
    api__isc_close_blob     m_func_isc_close_blob;
    api__isc_commit_retaining     m_func_isc_commit_retaining;
    api__isc_commit_transaction     m_func_isc_commit_transaction;
    api__isc_create_blob     m_func_isc_create_blob;
    api__isc_create_blob2     m_func_isc_create_blob2;
    api__isc_create_database     m_func_isc_create_database;
    api__isc_database_info     m_func_isc_database_info;
    api__isc_decode_date     m_func_isc_decode_date;
    api__isc_decode_sql_date     m_func_isc_decode_sql_date;
    api__isc_decode_sql_time     m_func_isc_decode_sql_time;
    api__isc_decode_timestamp     m_func_isc_decode_timestamp;
    api__isc_detach_database     m_func_isc_detach_database;
    api__isc_drop_database     m_func_isc_drop_database;
    api__isc_dsql_allocate_statement     m_func_isc_dsql_allocate_statement;
    api__isc_dsql_alloc_statement2     m_func_isc_dsql_alloc_statement2;
    api__isc_dsql_describe     m_func_isc_dsql_describe;
    api__isc_dsql_describe_bind     m_func_isc_dsql_describe_bind;
    api__isc_dsql_exec_immed2     m_func_isc_dsql_exec_immed2;
    api__isc_dsql_execute     m_func_isc_dsql_execute;
    api__isc_dsql_execute2     m_func_isc_dsql_execute2;
    api__isc_dsql_execute_immediate     m_func_isc_dsql_execute_immediate;
    api__isc_dsql_fetch     m_func_isc_dsql_fetch;
    api__isc_dsql_finish     m_func_isc_dsql_finish;
    api__isc_dsql_free_statement     m_func_isc_dsql_free_statement;
    api__isc_dsql_insert     m_func_isc_dsql_insert;
    api__isc_dsql_prepare     m_func_isc_dsql_prepare;
    api__isc_dsql_set_cursor_name     m_func_isc_dsql_set_cursor_name;
    api__isc_dsql_sql_info     m_func_isc_dsql_sql_info;
    api__isc_encode_date     m_func_isc_encode_date;
    api__isc_encode_sql_date     m_func_isc_encode_sql_date;
    api__isc_encode_sql_time     m_func_isc_encode_sql_time;
    api__isc_encode_timestamp     m_func_isc_encode_timestamp;
    api__isc_event_block     m_func_isc_event_block;
    api__isc_event_counts     m_func_isc_event_counts;
    //api__isc_expand_dpb     m_func_isc_expand_dpb;
    api__isc_modify_dpb     m_func_isc_modify_dpb;
    api__isc_free     m_func_isc_free;
    api__isc_get_segment     m_func_isc_get_segment;
    api__isc_get_slice     m_func_isc_get_slice;
    api__isc_interprete     m_func_isc_interprete;
    api__fb_interpret     m_func_fb_interpret;
    api__isc_open_blob     m_func_isc_open_blob;
    api__isc_open_blob2     m_func_isc_open_blob2;
    api__isc_prepare_transaction2     m_func_isc_prepare_transaction2;
    api__isc_print_sqlerror     m_func_isc_print_sqlerror;
    api__isc_print_status     m_func_isc_print_status;
    api__isc_put_segment     m_func_isc_put_segment;
    api__isc_put_slice     m_func_isc_put_slice;
    api__isc_que_events     m_func_isc_que_events;
    api__isc_rollback_retaining     m_func_isc_rollback_retaining;
    api__isc_rollback_transaction     m_func_isc_rollback_transaction;
    api__isc_start_multiple     m_func_isc_start_multiple;
    api__isc_start_transaction     m_func_isc_start_transaction;
    api__isc_sqlcode     m_func_isc_sqlcode;
    api__isc_sql_interprete     m_func_isc_sql_interprete;
    api__isc_transaction_info     m_func_isc_transaction_info;
    api__isc_transact_request     m_func_isc_transact_request;
    api__isc_vax_integer     m_func_isc_vax_integer;
    api__isc_portable_integer     m_func_isc_portable_integer;
    api__isc_add_user     m_func_isc_add_user;
    api__isc_delete_user     m_func_isc_delete_user;
    api__isc_modify_user     m_func_isc_modify_user;
    api__isc_compile_request     m_func_isc_compile_request;
    api__isc_compile_request2     m_func_isc_compile_request2;
    api__isc_ddl     m_func_isc_ddl;
    api__isc_prepare_transaction     m_func_isc_prepare_transaction;
    api__isc_receive     m_func_isc_receive;
    api__isc_reconnect_transaction     m_func_isc_reconnect_transaction;
    api__isc_release_request     m_func_isc_release_request;
    api__isc_request_info     m_func_isc_request_info;
    api__isc_seek_blob     m_func_isc_seek_blob;
    api__isc_send     m_func_isc_send;
    api__isc_start_and_send     m_func_isc_start_and_send;
    api__isc_start_request     m_func_isc_start_request;
    api__isc_unwind_request     m_func_isc_unwind_request;
    api__isc_wait_for_event     m_func_isc_wait_for_event;
    api__isc_close     m_func_isc_close;
    api__isc_declare     m_func_isc_declare;
    api__isc_describe     m_func_isc_describe;
    api__isc_describe_bind     m_func_isc_describe_bind;
    api__isc_execute     m_func_isc_execute;
    api__isc_execute_immediate     m_func_isc_execute_immediate;
    api__isc_fetch     m_func_isc_fetch;
    api__isc_open     m_func_isc_open;
    api__isc_prepare     m_func_isc_prepare;
    api__isc_dsql_execute_m     m_func_isc_dsql_execute_m;
    api__isc_dsql_execute2_m     m_func_isc_dsql_execute2_m;
    api__isc_dsql_execute_immediate_m     m_func_isc_dsql_execute_immediate_m;
    api__isc_dsql_exec_immed3_m     m_func_isc_dsql_exec_immed3_m;
    api__isc_dsql_fetch_m     m_func_isc_dsql_fetch_m;
    api__isc_dsql_insert_m     m_func_isc_dsql_insert_m;
    api__isc_dsql_prepare_m     m_func_isc_dsql_prepare_m;
    api__isc_dsql_release     m_func_isc_dsql_release;
    api__isc_embed_dsql_close     m_func_isc_embed_dsql_close;
    api__isc_embed_dsql_declare     m_func_isc_embed_dsql_declare;
    api__isc_embed_dsql_describe     m_func_isc_embed_dsql_describe;
    api__isc_embed_dsql_describe_bind     m_func_isc_embed_dsql_describe_bind;
    api__isc_embed_dsql_execute     m_func_isc_embed_dsql_execute;
    api__isc_embed_dsql_execute2     m_func_isc_embed_dsql_execute2;
    api__isc_embed_dsql_execute_immed     m_func_isc_embed_dsql_execute_immed;
    api__isc_embed_dsql_fetch     m_func_isc_embed_dsql_fetch;
    api__isc_embed_dsql_open     m_func_isc_embed_dsql_open;
    api__isc_embed_dsql_open2     m_func_isc_embed_dsql_open2;
    api__isc_embed_dsql_insert     m_func_isc_embed_dsql_insert;
    api__isc_embed_dsql_prepare     m_func_isc_embed_dsql_prepare;
    api__isc_embed_dsql_release     m_func_isc_embed_dsql_release;
    api__BLOB_open     m_func_BLOB_open;
    api__BLOB_put     m_func_BLOB_put;
    api__BLOB_close     m_func_BLOB_close;
    api__BLOB_get     m_func_BLOB_get;
    api__BLOB_display     m_func_BLOB_display;
    api__BLOB_dump     m_func_BLOB_dump;
    api__BLOB_edit     m_func_BLOB_edit;
    api__BLOB_load     m_func_BLOB_load;
    api__BLOB_text_dump     m_func_BLOB_text_dump;
    api__BLOB_text_load     m_func_BLOB_text_load;
    api__Bopen     m_func_Bopen;
    api__isc_ftof     m_func_isc_ftof;
    api__isc_print_blr     m_func_isc_print_blr;
    api__isc_set_debug     m_func_isc_set_debug;
    api__isc_qtoq     m_func_isc_qtoq;
    api__isc_vtof     m_func_isc_vtof;
    api__isc_vtov     m_func_isc_vtov;
    api__isc_version     m_func_isc_version;
    api__isc_reset_fpe     m_func_isc_reset_fpe;
    api__isc_service_attach     m_func_isc_service_attach;
    api__isc_service_detach     m_func_isc_service_detach;
    api__isc_service_query     m_func_isc_service_query;
    api__isc_service_start     m_func_isc_service_start;
    api__isc_get_client_version     m_func_isc_get_client_version;
    api__isc_get_client_major_version     m_func_isc_get_client_major_version;
    api__isc_get_client_minor_version     m_func_isc_get_client_minor_version;



public:
    FBClientDrv(String path)
    : DynamicLibrary(path.length() ? path.toSystemEncoding() : DBWTL_FBCLIENT_DEFAULT_LIB),

      m_func_isc_attach_database(0),
      m_func_isc_array_gen_sdl(0),
      m_func_isc_array_get_slice(0),
      m_func_isc_array_lookup_bounds(0),
      m_func_isc_array_lookup_desc(0),
      m_func_isc_array_set_desc(0),
      m_func_isc_array_put_slice(0),
      m_func_isc_blob_default_desc(0),
      m_func_isc_blob_gen_bpb(0),
      m_func_isc_blob_info(0),
      m_func_isc_blob_lookup_desc(0),
      m_func_isc_blob_set_desc(0),
      m_func_isc_cancel_blob(0),
      m_func_isc_cancel_events(0),
      m_func_isc_close_blob(0),
      m_func_isc_commit_retaining(0),
      m_func_isc_commit_transaction(0),
      m_func_isc_create_blob(0),
      m_func_isc_create_blob2(0),
      m_func_isc_create_database(0),
      m_func_isc_database_info(0),
      m_func_isc_decode_date(0),
      m_func_isc_decode_sql_date(0),
      m_func_isc_decode_sql_time(0),
      m_func_isc_decode_timestamp(0),
      m_func_isc_detach_database(0),
      m_func_isc_drop_database(0),
      m_func_isc_dsql_allocate_statement(0),
      m_func_isc_dsql_alloc_statement2(0),
      m_func_isc_dsql_describe(0),
      m_func_isc_dsql_describe_bind(0),
      m_func_isc_dsql_exec_immed2(0),
      m_func_isc_dsql_execute(0),
      m_func_isc_dsql_execute2(0),
      m_func_isc_dsql_execute_immediate(0),
      m_func_isc_dsql_fetch(0),
      m_func_isc_dsql_finish(0),
      m_func_isc_dsql_free_statement(0),
      m_func_isc_dsql_insert(0),
      m_func_isc_dsql_prepare(0),
      m_func_isc_dsql_set_cursor_name(0),
      m_func_isc_dsql_sql_info(0),
      m_func_isc_encode_date(0),
      m_func_isc_encode_sql_date(0),
      m_func_isc_encode_sql_time(0),
      m_func_isc_encode_timestamp(0),
      m_func_isc_event_block(0),
      m_func_isc_event_counts(0),
      //m_func_isc_expand_dpb(0),
      m_func_isc_modify_dpb(0),
      m_func_isc_free(0),
      m_func_isc_get_segment(0),
      m_func_isc_get_slice(0),
      m_func_isc_interprete(0),
      m_func_fb_interpret(0),
      m_func_isc_open_blob(0),
      m_func_isc_open_blob2(0),
      m_func_isc_prepare_transaction2(0),
      m_func_isc_print_sqlerror(0),
      m_func_isc_print_status(0),
      m_func_isc_put_segment(0),
      m_func_isc_put_slice(0),
      m_func_isc_que_events(0),
      m_func_isc_rollback_retaining(0),
      m_func_isc_rollback_transaction(0),
      m_func_isc_start_multiple(0),
      m_func_isc_start_transaction(0),
      m_func_isc_sqlcode(0),
      m_func_isc_sql_interprete(0),
      m_func_isc_transaction_info(0),
      m_func_isc_transact_request(0),
      m_func_isc_vax_integer(0),
      m_func_isc_portable_integer(0),
      m_func_isc_add_user(0),
      m_func_isc_delete_user(0),
      m_func_isc_modify_user(0),
      m_func_isc_compile_request(0),
      m_func_isc_compile_request2(0),
      m_func_isc_ddl(0),
      m_func_isc_prepare_transaction(0),
      m_func_isc_receive(0),
      m_func_isc_reconnect_transaction(0),
      m_func_isc_release_request(0),
      m_func_isc_request_info(0),
      m_func_isc_seek_blob(0),
      m_func_isc_send(0),
      m_func_isc_start_and_send(0),
      m_func_isc_start_request(0),
      m_func_isc_unwind_request(0),
      m_func_isc_wait_for_event(0),
      m_func_isc_close(0),
      m_func_isc_declare(0),
      m_func_isc_describe(0),
      m_func_isc_describe_bind(0),
      m_func_isc_execute(0),
      m_func_isc_execute_immediate(0),
      m_func_isc_fetch(0),
      m_func_isc_open(0),
      m_func_isc_prepare(0),
      m_func_isc_dsql_execute_m(0),
      m_func_isc_dsql_execute2_m(0),
      m_func_isc_dsql_execute_immediate_m(0),
      m_func_isc_dsql_exec_immed3_m(0),
      m_func_isc_dsql_fetch_m(0),
      m_func_isc_dsql_insert_m(0),
      m_func_isc_dsql_prepare_m(0),
      m_func_isc_dsql_release(0),
      m_func_isc_embed_dsql_close(0),
      m_func_isc_embed_dsql_declare(0),
      m_func_isc_embed_dsql_describe(0),
      m_func_isc_embed_dsql_describe_bind(0),
      m_func_isc_embed_dsql_execute(0),
      m_func_isc_embed_dsql_execute2(0),
      m_func_isc_embed_dsql_execute_immed(0),
      m_func_isc_embed_dsql_fetch(0),
      m_func_isc_embed_dsql_open(0),
      m_func_isc_embed_dsql_open2(0),
      m_func_isc_embed_dsql_insert(0),
      m_func_isc_embed_dsql_prepare(0),
      m_func_isc_embed_dsql_release(0),
      m_func_BLOB_open(0),
      m_func_BLOB_put(0),
      m_func_BLOB_close(0),
      m_func_BLOB_get(0),
      m_func_BLOB_display(0),
      m_func_BLOB_dump(0),
      m_func_BLOB_edit(0),
      m_func_BLOB_load(0),
      m_func_BLOB_text_dump(0),
      m_func_BLOB_text_load(0),
      m_func_Bopen(0),
      m_func_isc_ftof(0),
      m_func_isc_print_blr(0),
      m_func_isc_set_debug(0),
      m_func_isc_qtoq(0),
      m_func_isc_vtof(0),
      m_func_isc_vtov(0),
      m_func_isc_version(0),
      m_func_isc_reset_fpe(0),
      m_func_isc_service_attach(0),
      m_func_isc_service_detach(0),
      m_func_isc_service_query(0),
      m_func_isc_service_start(0),
      m_func_isc_get_client_version(0),
      m_func_isc_get_client_major_version(0),
      m_func_isc_get_client_minor_version(0)


    {

        this->getproc(this->m_func_isc_attach_database, "isc_attach_database");
        this->getproc(this->m_func_isc_array_gen_sdl, "isc_array_gen_sdl");
        this->getproc(this->m_func_isc_array_get_slice, "isc_array_get_slice");
        this->getproc(this->m_func_isc_array_lookup_bounds, "isc_array_lookup_bounds");
        this->getproc(this->m_func_isc_array_lookup_desc, "isc_array_lookup_desc");
        this->getproc(this->m_func_isc_array_set_desc, "isc_array_set_desc");
        this->getproc(this->m_func_isc_array_put_slice, "isc_array_put_slice");
        this->getproc(this->m_func_isc_blob_default_desc, "isc_blob_default_desc");
        this->getproc(this->m_func_isc_blob_gen_bpb, "isc_blob_gen_bpb");
        this->getproc(this->m_func_isc_blob_info, "isc_blob_info");
        this->getproc(this->m_func_isc_blob_lookup_desc, "isc_blob_lookup_desc");
        this->getproc(this->m_func_isc_blob_set_desc, "isc_blob_set_desc");
        this->getproc(this->m_func_isc_cancel_blob, "isc_cancel_blob");
        this->getproc(this->m_func_isc_cancel_events, "isc_cancel_events");
        this->getproc(this->m_func_isc_close_blob, "isc_close_blob");
        this->getproc(this->m_func_isc_commit_retaining, "isc_commit_retaining");
        this->getproc(this->m_func_isc_commit_transaction, "isc_commit_transaction");
        this->getproc(this->m_func_isc_create_blob, "isc_create_blob");
        this->getproc(this->m_func_isc_create_blob2, "isc_create_blob2");
        this->getproc(this->m_func_isc_create_database, "isc_create_database");
        this->getproc(this->m_func_isc_database_info, "isc_database_info");
        this->getproc(this->m_func_isc_decode_date, "isc_decode_date");
        this->getproc(this->m_func_isc_decode_sql_date, "isc_decode_sql_date");
        this->getproc(this->m_func_isc_decode_sql_time, "isc_decode_sql_time");
        this->getproc(this->m_func_isc_decode_timestamp, "isc_decode_timestamp");
        this->getproc(this->m_func_isc_detach_database, "isc_detach_database");
        this->getproc(this->m_func_isc_drop_database, "isc_drop_database");
        this->getproc(this->m_func_isc_dsql_allocate_statement, "isc_dsql_allocate_statement");
        this->getproc(this->m_func_isc_dsql_alloc_statement2, "isc_dsql_alloc_statement2");
        this->getproc(this->m_func_isc_dsql_describe, "isc_dsql_describe");
        this->getproc(this->m_func_isc_dsql_describe_bind, "isc_dsql_describe_bind");
        this->getproc(this->m_func_isc_dsql_exec_immed2, "isc_dsql_exec_immed2");
        this->getproc(this->m_func_isc_dsql_execute, "isc_dsql_execute");
        this->getproc(this->m_func_isc_dsql_execute2, "isc_dsql_execute2");
        this->getproc(this->m_func_isc_dsql_execute_immediate, "isc_dsql_execute_immediate");
        this->getproc(this->m_func_isc_dsql_fetch, "isc_dsql_fetch");
        this->getproc(this->m_func_isc_dsql_finish, "isc_dsql_finish");
        this->getproc(this->m_func_isc_dsql_free_statement, "isc_dsql_free_statement");
        this->getproc(this->m_func_isc_dsql_insert, "isc_dsql_insert");
        this->getproc(this->m_func_isc_dsql_prepare, "isc_dsql_prepare");
        this->getproc(this->m_func_isc_dsql_set_cursor_name, "isc_dsql_set_cursor_name");
        this->getproc(this->m_func_isc_dsql_sql_info, "isc_dsql_sql_info");
        this->getproc(this->m_func_isc_encode_date, "isc_encode_date");
        this->getproc(this->m_func_isc_encode_sql_date, "isc_encode_sql_date");
        this->getproc(this->m_func_isc_encode_sql_time, "isc_encode_sql_time");
        this->getproc(this->m_func_isc_encode_timestamp, "isc_encode_timestamp");
        this->getproc(this->m_func_isc_event_block, "isc_event_block");
        this->getproc(this->m_func_isc_event_counts, "isc_event_counts");
        //this->getproc(this->m_func_isc_expand_dpb, "isc_expand_dpb");
        this->getproc(this->m_func_isc_modify_dpb, "isc_modify_dpb");
        this->getproc(this->m_func_isc_free, "isc_free");
        this->getproc(this->m_func_isc_get_segment, "isc_get_segment");
        this->getproc(this->m_func_isc_get_slice, "isc_get_slice");
        this->getproc(this->m_func_isc_interprete, "isc_interprete");
        this->getproc(this->m_func_fb_interpret, "fb_interpret");
        this->getproc(this->m_func_isc_open_blob, "isc_open_blob");
        this->getproc(this->m_func_isc_open_blob2, "isc_open_blob2");
        this->getproc(this->m_func_isc_prepare_transaction2, "isc_prepare_transaction2");
        this->getproc(this->m_func_isc_print_sqlerror, "isc_print_sqlerror");
        this->getproc(this->m_func_isc_print_status, "isc_print_status");
        this->getproc(this->m_func_isc_put_segment, "isc_put_segment");
        this->getproc(this->m_func_isc_put_slice, "isc_put_slice");
        this->getproc(this->m_func_isc_que_events, "isc_que_events");
        this->getproc(this->m_func_isc_rollback_retaining, "isc_rollback_retaining");
        this->getproc(this->m_func_isc_rollback_transaction, "isc_rollback_transaction");
        this->getproc(this->m_func_isc_start_multiple, "isc_start_multiple");
        this->getproc(this->m_func_isc_start_transaction, "isc_start_transaction");
        this->getproc(this->m_func_isc_sqlcode, "isc_sqlcode");
        this->getproc(this->m_func_isc_sql_interprete, "isc_sql_interprete");
        this->getproc(this->m_func_isc_transaction_info, "isc_transaction_info");
        this->getproc(this->m_func_isc_transact_request, "isc_transact_request");
        this->getproc(this->m_func_isc_vax_integer, "isc_vax_integer");
        this->getproc(this->m_func_isc_portable_integer, "isc_portable_integer");
        this->getproc(this->m_func_isc_add_user, "isc_add_user");
        this->getproc(this->m_func_isc_delete_user, "isc_delete_user");
        this->getproc(this->m_func_isc_modify_user, "isc_modify_user");
        this->getproc(this->m_func_isc_compile_request, "isc_compile_request");
        this->getproc(this->m_func_isc_compile_request2, "isc_compile_request2");
        this->getproc(this->m_func_isc_ddl, "isc_ddl");
        this->getproc(this->m_func_isc_prepare_transaction, "isc_prepare_transaction");
        this->getproc(this->m_func_isc_receive, "isc_receive");
        this->getproc(this->m_func_isc_reconnect_transaction, "isc_reconnect_transaction");
        this->getproc(this->m_func_isc_release_request, "isc_release_request");
        this->getproc(this->m_func_isc_request_info, "isc_request_info");
        this->getproc(this->m_func_isc_seek_blob, "isc_seek_blob");
        this->getproc(this->m_func_isc_send, "isc_send");
        this->getproc(this->m_func_isc_start_and_send, "isc_start_and_send");
        this->getproc(this->m_func_isc_start_request, "isc_start_request");
        this->getproc(this->m_func_isc_unwind_request, "isc_unwind_request");
        this->getproc(this->m_func_isc_wait_for_event, "isc_wait_for_event");
        this->getproc(this->m_func_isc_close, "isc_close");
        this->getproc(this->m_func_isc_declare, "isc_declare");
        this->getproc(this->m_func_isc_describe, "isc_describe");
        this->getproc(this->m_func_isc_describe_bind, "isc_describe_bind");
        this->getproc(this->m_func_isc_execute, "isc_execute");
        this->getproc(this->m_func_isc_execute_immediate, "isc_execute_immediate");
        this->getproc(this->m_func_isc_fetch, "isc_fetch");
        this->getproc(this->m_func_isc_open, "isc_open");
        this->getproc(this->m_func_isc_prepare, "isc_prepare");
        this->getproc(this->m_func_isc_dsql_execute_m, "isc_dsql_execute_m");
        this->getproc(this->m_func_isc_dsql_execute2_m, "isc_dsql_execute2_m");
        this->getproc(this->m_func_isc_dsql_execute_immediate_m, "isc_dsql_execute_immediate_m");
        this->getproc(this->m_func_isc_dsql_exec_immed3_m, "isc_dsql_exec_immed3_m");
        this->getproc(this->m_func_isc_dsql_fetch_m, "isc_dsql_fetch_m");
        this->getproc(this->m_func_isc_dsql_insert_m, "isc_dsql_insert_m");
        this->getproc(this->m_func_isc_dsql_prepare_m, "isc_dsql_prepare_m");
        this->getproc(this->m_func_isc_dsql_release, "isc_dsql_release");
        this->getproc(this->m_func_isc_embed_dsql_close, "isc_embed_dsql_close");
        this->getproc(this->m_func_isc_embed_dsql_declare, "isc_embed_dsql_declare");
        this->getproc(this->m_func_isc_embed_dsql_describe, "isc_embed_dsql_describe");
        this->getproc(this->m_func_isc_embed_dsql_describe_bind, "isc_embed_dsql_describe_bind");
        this->getproc(this->m_func_isc_embed_dsql_execute, "isc_embed_dsql_execute");
        this->getproc(this->m_func_isc_embed_dsql_execute2, "isc_embed_dsql_execute2");
        this->getproc(this->m_func_isc_embed_dsql_execute_immed, "isc_embed_dsql_execute_immed");
        this->getproc(this->m_func_isc_embed_dsql_fetch, "isc_embed_dsql_fetch");
        this->getproc(this->m_func_isc_embed_dsql_open, "isc_embed_dsql_open");
        this->getproc(this->m_func_isc_embed_dsql_open2, "isc_embed_dsql_open2");
        this->getproc(this->m_func_isc_embed_dsql_insert, "isc_embed_dsql_insert");
        this->getproc(this->m_func_isc_embed_dsql_prepare, "isc_embed_dsql_prepare");
        this->getproc(this->m_func_isc_embed_dsql_release, "isc_embed_dsql_release");
        this->getproc(this->m_func_BLOB_open, "BLOB_open");
        this->getproc(this->m_func_BLOB_put, "BLOB_put");
        this->getproc(this->m_func_BLOB_close, "BLOB_close");
        this->getproc(this->m_func_BLOB_get, "BLOB_get");
        this->getproc(this->m_func_BLOB_display, "BLOB_display");
        this->getproc(this->m_func_BLOB_dump, "BLOB_dump");
        this->getproc(this->m_func_BLOB_edit, "BLOB_edit");
        this->getproc(this->m_func_BLOB_load, "BLOB_load");
        this->getproc(this->m_func_BLOB_text_dump, "BLOB_text_dump");
        this->getproc(this->m_func_BLOB_text_load, "BLOB_text_load");
        this->getproc(this->m_func_Bopen, "Bopen");
        this->getproc(this->m_func_isc_ftof, "isc_ftof");
        this->getproc(this->m_func_isc_print_blr, "isc_print_blr");
        this->getproc(this->m_func_isc_set_debug, "isc_set_debug");
        this->getproc(this->m_func_isc_qtoq, "isc_qtoq");
        this->getproc(this->m_func_isc_vtof, "isc_vtof");
        this->getproc(this->m_func_isc_vtov, "isc_vtov");
        this->getproc(this->m_func_isc_version, "isc_version");
        this->getproc(this->m_func_isc_reset_fpe, "isc_reset_fpe");
        this->getproc(this->m_func_isc_service_attach, "isc_service_attach");
        this->getproc(this->m_func_isc_service_detach, "isc_service_detach");
        this->getproc(this->m_func_isc_service_query, "isc_service_query");
        this->getproc(this->m_func_isc_service_start, "isc_service_start");
        this->getproc(this->m_func_isc_get_client_version, "isc_get_client_version");
        this->getproc(this->m_func_isc_get_client_major_version, "isc_get_client_major_version");
        this->getproc(this->m_func_isc_get_client_minor_version, "isc_get_client_minor_version");
    }


    virtual ~FBClientDrv(void) { }





    inline ISC_STATUS isc_attach_database(ISC_STATUS* a,
                                          short b,
                                          const ISC_SCHAR* c,
                                          isc_db_handle* d,
                                          short e,
                                          const ISC_SCHAR* f)
    {
        if(this->m_func_isc_attach_database)
            return this->m_func_isc_attach_database(a, b, c, d, e, f);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_array_gen_sdl(ISC_STATUS*,
                                        const ISC_ARRAY_DESC*,
                                        ISC_SHORT*,
                                        ISC_UCHAR*,
                                        ISC_SHORT*);

    inline ISC_STATUS isc_array_get_slice(ISC_STATUS*,
                                          isc_db_handle*,
                                          isc_tr_handle*,
                                          ISC_QUAD*,
                                          const ISC_ARRAY_DESC*,
                                          void*,
                                          ISC_LONG*);

    inline ISC_STATUS isc_array_lookup_bounds(ISC_STATUS*,
                                              isc_db_handle*,
                                              isc_tr_handle*,
                                              const ISC_SCHAR*,
                                              const ISC_SCHAR*,
                                              ISC_ARRAY_DESC*);

    inline ISC_STATUS isc_array_lookup_desc(ISC_STATUS*,
                                            isc_db_handle*,
                                            isc_tr_handle*,
                                            const ISC_SCHAR*,
                                            const ISC_SCHAR*,
                                            ISC_ARRAY_DESC*);

    inline ISC_STATUS isc_array_set_desc(ISC_STATUS*,
                                         const ISC_SCHAR*,
                                         const ISC_SCHAR*,
                                         const short*,
                                         const short*,
                                         const short*,
                                         ISC_ARRAY_DESC*);

    inline ISC_STATUS isc_array_put_slice(ISC_STATUS*,
                                          isc_db_handle*,
                                          isc_tr_handle*,
                                          ISC_QUAD*,
                                          const ISC_ARRAY_DESC*,
                                          void*,
                                          ISC_LONG *);

    inline void isc_blob_default_desc(ISC_BLOB_DESC* a,
                                      const ISC_UCHAR* b,
                                      const ISC_UCHAR* c)
    {
        if(this->m_func_isc_blob_default_desc)
            return this->m_func_isc_blob_default_desc(a, b, c);
        else
            throw LibFunctionException(__FUNCTION__);                
    }

    inline ISC_STATUS isc_blob_gen_bpb(ISC_STATUS* a,
                                       const ISC_BLOB_DESC* b,
                                       const ISC_BLOB_DESC* c,
                                       unsigned short d,
                                       ISC_UCHAR* e,
                                       unsigned short* f)
    {
        if(this->m_func_isc_blob_gen_bpb)
            return this->m_func_isc_blob_gen_bpb(a, b, c, d, e, f);
        else
            throw LibFunctionException(__FUNCTION__);                
    }

    inline ISC_STATUS isc_blob_info(ISC_STATUS*,
                                    isc_blob_handle*,
                                    short,
                                    const ISC_SCHAR*,
                                    short,
                                    ISC_SCHAR*);

    inline ISC_STATUS isc_blob_lookup_desc(ISC_STATUS* a,
                                           isc_db_handle* b,
                                           isc_tr_handle* c,
                                           const ISC_UCHAR* d,
                                           const ISC_UCHAR* e,
                                           ISC_BLOB_DESC* f,
                                           ISC_UCHAR* g)
    {
        if(this->m_func_isc_blob_lookup_desc)
            return this->m_func_isc_blob_lookup_desc(a, b, c, d, e, f, g);
        else
            throw LibFunctionException(__FUNCTION__);                
    }

    inline ISC_STATUS isc_blob_set_desc(ISC_STATUS*,
                                        const ISC_UCHAR*,
                                        const ISC_UCHAR*,
                                        short,
                                        short,
                                        short,
                                        ISC_BLOB_DESC*);

    inline ISC_STATUS isc_cancel_blob(ISC_STATUS *,
                                      isc_blob_handle *);

    inline ISC_STATUS isc_cancel_events(ISC_STATUS *,
                                        isc_db_handle *,
                                        ISC_LONG *);

    inline ISC_STATUS isc_close_blob(ISC_STATUS * a,
                                     isc_blob_handle * b)
    {
        if(this->m_func_isc_close_blob)
            return this->m_func_isc_close_blob(a, b);
        else
            throw LibFunctionException(__FUNCTION__);                
    }

    inline ISC_STATUS isc_commit_retaining(ISC_STATUS *,
                                           isc_tr_handle *);

    inline ISC_STATUS isc_commit_transaction(ISC_STATUS * a,
                                             isc_tr_handle * b)
    {
        if(this->m_func_isc_commit_transaction)
            return this->m_func_isc_commit_transaction(a, b);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_create_blob(ISC_STATUS*,
                                      isc_db_handle*,
                                      isc_tr_handle*,
                                      isc_blob_handle*,
                                      ISC_QUAD*);

    inline ISC_STATUS isc_create_blob2(ISC_STATUS* a,
                                       isc_db_handle* b,
                                       isc_tr_handle* c,
                                       isc_blob_handle* d,
                                       ISC_QUAD* e,
                                       short f,
                                       const ISC_SCHAR* g)
    {
        if(this->m_func_isc_create_blob2)
            return this->m_func_isc_create_blob2(a, b, c, d, e, f, g);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_create_database(ISC_STATUS*,
                                          short,
                                          const ISC_SCHAR*,
                                          isc_db_handle*,
                                          short,
                                          const ISC_SCHAR*,
                                          short);

    inline ISC_STATUS isc_database_info(ISC_STATUS*,
                                        isc_db_handle*,
                                        short,
                                        const ISC_SCHAR*,
                                        short,
                                        ISC_SCHAR*);

    inline void isc_decode_date(const ISC_QUAD*,
                                void*);


    inline void isc_decode_sql_date(const ISC_DATE* a,
                                    void* b)
    {
        if(this->m_func_isc_decode_sql_date)
            return this->m_func_isc_decode_sql_date(a, b);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline void isc_decode_sql_time(const ISC_TIME* a,
                                    void* b)
    {
        if(this->m_func_isc_decode_sql_time)
            return this->m_func_isc_decode_sql_time(a, b);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline void isc_decode_timestamp(const ISC_TIMESTAMP* a,
                                     void* b)
    {
        if(this->m_func_isc_decode_timestamp)
            return this->m_func_isc_decode_timestamp(a, b);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_detach_database(ISC_STATUS *a,
                                          isc_db_handle *b)
    {
        if(this->m_func_isc_detach_database)
            return this->m_func_isc_detach_database(a, b);
        else
            throw LibFunctionException(__FUNCTION__);
    }


    inline ISC_STATUS isc_drop_database(ISC_STATUS *,
                                        isc_db_handle *);

    inline ISC_STATUS isc_dsql_allocate_statement(ISC_STATUS * a,
                                                  isc_db_handle * b,
                                                  isc_stmt_handle * c)
    {
        if(this->m_func_isc_dsql_allocate_statement)
            return this->m_func_isc_dsql_allocate_statement(a, b, c);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_dsql_alloc_statement2(ISC_STATUS *,
                                                isc_db_handle *,
                                                isc_stmt_handle *);

    inline ISC_STATUS isc_dsql_describe(ISC_STATUS * a,
                                        isc_stmt_handle * b,
                                        unsigned short c,
                                        XSQLDA * d)
{
        if(this->m_func_isc_dsql_describe)
            return this->m_func_isc_dsql_describe(a, b, c, d);
        else
            throw LibFunctionException(__FUNCTION__);

}

    inline ISC_STATUS isc_dsql_describe_bind(ISC_STATUS * a,
                                             isc_stmt_handle * b,
                                             unsigned short c,
                                             XSQLDA * d)
    {
        if(this->m_func_isc_dsql_describe_bind)
            return this->m_func_isc_dsql_describe_bind(a, b, c, d);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_dsql_exec_immed2(ISC_STATUS*,
                                           isc_db_handle*,
                                           isc_tr_handle*,
                                           unsigned short,
                                           const ISC_SCHAR*,
                                           unsigned short,
                                           XSQLDA*,
                                           XSQLDA*);

    inline ISC_STATUS isc_dsql_execute(ISC_STATUS* a,
                                       isc_tr_handle* b,
                                       isc_stmt_handle* c,
                                       unsigned short d,
                                       XSQLDA* e)
    {
        if(this->m_func_isc_dsql_execute)
            return this->m_func_isc_dsql_execute(a, b, c, d, e);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_dsql_execute2(ISC_STATUS* a,
                                        isc_tr_handle* b,
                                        isc_stmt_handle* c,
                                        unsigned short d,
                                        XSQLDA* e,
                                        XSQLDA* f)
    {
        if(this->m_func_isc_dsql_execute2)
            return this->m_func_isc_dsql_execute2(a, b, c, d, e, f);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_dsql_execute_immediate(ISC_STATUS* a,
                                                 isc_db_handle* b,
                                                 isc_tr_handle* c,
                                                 unsigned short d,
                                                 const ISC_SCHAR* e,
                                                 unsigned short f,
                                                 XSQLDA* g)
    {
        if(this->m_func_isc_dsql_execute_immediate)
            return this->m_func_isc_dsql_execute_immediate(a, b, c, d, e, f, g);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_dsql_fetch(ISC_STATUS * a,
                                     isc_stmt_handle * b,
                                     unsigned short c,
                                     XSQLDA * d)
{
        if(this->m_func_isc_dsql_fetch)
            return this->m_func_isc_dsql_fetch(a, b, c, d);
        else
            throw LibFunctionException(__FUNCTION__);

}

    inline ISC_STATUS isc_dsql_finish(isc_db_handle *);

    inline ISC_STATUS isc_dsql_free_statement(ISC_STATUS * a,
                                              isc_stmt_handle * b,
                                              unsigned short c)
    {
        if(this->m_func_isc_dsql_free_statement)
            return this->m_func_isc_dsql_free_statement(a, b, c);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

    inline ISC_STATUS isc_dsql_insert(ISC_STATUS*,
                                      isc_stmt_handle*,
                                      unsigned short,
                                      XSQLDA*);

    inline ISC_STATUS isc_dsql_prepare(ISC_STATUS* a,
                                       isc_tr_handle* b,
                                       isc_stmt_handle* c,
                                       unsigned short d,
                                       const ISC_SCHAR* e,
                                       unsigned short f,
                                       XSQLDA* g)
    {
        if(this->m_func_isc_dsql_prepare)
            return this->m_func_isc_dsql_prepare(a, b, c, d, e, f, g);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_dsql_set_cursor_name(ISC_STATUS*,
                                               isc_stmt_handle*,
                                               const ISC_SCHAR*,
                                               unsigned short);

    inline ISC_STATUS isc_dsql_sql_info(ISC_STATUS* a,
                                        isc_stmt_handle* b,
                                        short c,
                                        const ISC_SCHAR* d,
                                        short e,
                                        ISC_SCHAR* f)
    {
        if(this->m_func_isc_dsql_sql_info)
            return this->m_func_isc_dsql_sql_info(a, b, c, d, e, f);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

    inline void isc_encode_date(const void* a,
                                ISC_QUAD* b)
    {
        if(this->m_func_isc_encode_date)
            this->m_func_isc_encode_date(a, b);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

    inline void isc_encode_sql_date(const void* a,
                                    ISC_DATE* b)
    {
        if(this->m_func_isc_encode_sql_date)
            this->m_func_isc_encode_sql_date(a, b);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

    inline void isc_encode_sql_time(const void* a,
                                    ISC_TIME* b)
    {
        if(this->m_func_isc_encode_sql_time)
            this->m_func_isc_encode_sql_time(a, b);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

    inline void isc_encode_timestamp(const void* a,
                                     ISC_TIMESTAMP* b)
    {
        if(this->m_func_isc_encode_timestamp)
            this->m_func_isc_encode_timestamp(a, b);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

    inline ISC_LONG ISC_EXPORT_VARARG isc_event_block(ISC_UCHAR**,
                                                      ISC_UCHAR**,
                                                      unsigned short, ...);

    inline void isc_event_counts(ISC_ULONG*,
                                 short,
                                 ISC_UCHAR*,
                                 const ISC_UCHAR *);

/* 17 May 2001 - isc_expand_dpb is DEPRECATED */
    inline void FB_API_DEPRECATED ISC_EXPORT_VARARG isc_expand_dpb(ISC_SCHAR**,
                                                                   short*, ...);

    inline int isc_modify_dpb(ISC_SCHAR**,
                              short*,
                              unsigned short,
                              const ISC_SCHAR*,
                              short);

    inline ISC_LONG isc_free(ISC_SCHAR *);

    inline ISC_STATUS isc_get_segment(ISC_STATUS * a,
                                      isc_blob_handle * b,
                                      unsigned short * c,
                                      unsigned short d,
                                      ISC_SCHAR * e)
    {
        if(this->m_func_isc_get_segment)
            return this->m_func_isc_get_segment(a, b, c, d, e);
        else
            throw LibFunctionException(__FUNCTION__);                
    }

    inline ISC_STATUS isc_get_slice(ISC_STATUS*,
                                    isc_db_handle*,
                                    isc_tr_handle*,
                                    ISC_QUAD*,
                                    short,
                                    const ISC_SCHAR*,
                                    short,
                                    const ISC_LONG*,
                                    ISC_LONG,
                                    void*,
                                    ISC_LONG*);

/* CVC: This non-const signature is needed for compatibility, see gds.cpp. */
    inline ISC_LONG /* FB_API_DEPRECATED */ isc_interprete(ISC_SCHAR* a,
                                                     ISC_STATUS** b)
    {
        if(this->m_func_isc_interprete)
            return this->m_func_isc_interprete(a, b);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

/* This const params version used in the engine and other places. */
    inline ISC_LONG fb_interpret(ISC_SCHAR*,
                                 unsigned int,
                                 const ISC_STATUS**);

    inline ISC_STATUS isc_open_blob(ISC_STATUS*,
                                    isc_db_handle*,
                                    isc_tr_handle*,
                                    isc_blob_handle*,
                                    ISC_QUAD*);

    inline ISC_STATUS isc_open_blob2(ISC_STATUS*a ,
                                     isc_db_handle* b,
                                     isc_tr_handle* c,
                                     isc_blob_handle* d,
                                     ISC_QUAD* e,
                                     ISC_USHORT f,
                                     const ISC_UCHAR* g)
    {
        if(this->m_func_isc_open_blob2)
            return this->m_func_isc_open_blob2(a, b, c, d, e, f, g);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

    inline ISC_STATUS isc_prepare_transaction2(ISC_STATUS*,
                                               isc_tr_handle*,
                                               ISC_USHORT,
                                               const ISC_UCHAR*);

    inline void isc_print_sqlerror(ISC_SHORT,
                                   const ISC_STATUS*);

    inline ISC_STATUS isc_print_status(const ISC_STATUS*);

    inline ISC_STATUS isc_put_segment(ISC_STATUS* a,
                                      isc_blob_handle* b,
                                      unsigned short c,
                                      const ISC_SCHAR* d)
    {
        if(this->m_func_isc_put_segment)
            return this->m_func_isc_put_segment(a, b, c, d);
        else
            throw LibFunctionException(__FUNCTION__);        
    }

    inline ISC_STATUS isc_put_slice(ISC_STATUS*,
                                    isc_db_handle*,
                                    isc_tr_handle*,
                                    ISC_QUAD*,
                                    short,
                                    const ISC_SCHAR*,
                                    short,
                                    const ISC_LONG*,
                                    ISC_LONG,
                                    void*);

    inline ISC_STATUS isc_que_events(ISC_STATUS*,
                                     isc_db_handle*,
                                     ISC_LONG*,
                                     short,
                                     const ISC_UCHAR*,
                                     ISC_EVENT_CALLBACK,
                                     void*);

    inline ISC_STATUS isc_rollback_retaining(ISC_STATUS *,
                                             isc_tr_handle *);

    inline ISC_STATUS isc_rollback_transaction(ISC_STATUS * a,
                                               isc_tr_handle * b)
    {
        if(this->m_func_isc_rollback_transaction)
            return this->m_func_isc_rollback_transaction(a, b);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_STATUS isc_start_multiple(ISC_STATUS *,
                                         isc_tr_handle *,
                                         short,
                                         void *);

    inline ISC_STATUS ISC_EXPORT_VARARG isc_start_transaction(ISC_STATUS * a,
                                                              isc_tr_handle * b,
                                                              short c,
                                                              isc_db_handle* d,
                                                              unsigned short e,
                                                              char *f)
    {
        if(this->m_func_isc_start_transaction)
            return this->m_func_isc_start_transaction(a, b, c, d, e, f);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_LONG isc_sqlcode(const ISC_STATUS*);

    inline void isc_sql_interprete(short,
                                   ISC_SCHAR *,
                                   short);

    inline ISC_STATUS isc_transaction_info(ISC_STATUS*,
                                           isc_tr_handle*,
                                           short,
                                           const ISC_SCHAR*,
                                           short,
                                           ISC_SCHAR*);

    inline ISC_STATUS isc_transact_request(ISC_STATUS*,
                                           isc_db_handle*,
                                           isc_tr_handle*,
                                           unsigned short,
                                           const ISC_SCHAR*,
                                           unsigned short,
                                           ISC_SCHAR*,
                                           unsigned short,
                                           ISC_SCHAR*);

    inline ISC_LONG isc_vax_integer(const ISC_SCHAR* a,
                                    short b)
    {
        if(this->m_func_isc_vax_integer)
            return this->m_func_isc_vax_integer(a, b);
        else
            throw LibFunctionException(__FUNCTION__);
    }

    inline ISC_INT64 isc_portable_integer(const ISC_UCHAR*,
                                          short);


    inline ISC_STATUS isc_add_user(ISC_STATUS*, const USER_SEC_DATA*);

    inline ISC_STATUS isc_delete_user(ISC_STATUS*, const USER_SEC_DATA*);

    inline ISC_STATUS isc_modify_user(ISC_STATUS*, const USER_SEC_DATA*);



    inline ISC_STATUS isc_compile_request(ISC_STATUS*,
                                          isc_db_handle*,
                                          isc_req_handle*,
                                          short,
                                          const ISC_SCHAR*);

    inline ISC_STATUS isc_compile_request2(ISC_STATUS*,
                                           isc_db_handle*,
                                           isc_req_handle*,
                                           short,
                                           const ISC_SCHAR*);

    inline ISC_STATUS isc_ddl(ISC_STATUS*,
                              isc_db_handle*,
                              isc_tr_handle*,
                              short,
                              const ISC_SCHAR*);

    inline ISC_STATUS isc_prepare_transaction(ISC_STATUS*,
                                              isc_tr_handle*);


    inline ISC_STATUS isc_receive(ISC_STATUS*,
                                  isc_req_handle*,
                                  short,
                                  short,
                                  void*,
                                  short);

    inline ISC_STATUS isc_reconnect_transaction(ISC_STATUS*,
                                                isc_db_handle*,
                                                isc_tr_handle*,
                                                short,
                                                const ISC_SCHAR*);

    inline ISC_STATUS isc_release_request(ISC_STATUS*,
                                          isc_req_handle*);

    inline ISC_STATUS isc_request_info(ISC_STATUS*,
                                       isc_req_handle*,
                                       short,
                                       short,
                                       const ISC_SCHAR*,
                                       short,
                                       ISC_SCHAR*);

    inline ISC_STATUS isc_seek_blob(ISC_STATUS*,
                                    isc_blob_handle*,
                                    short,
                                    ISC_LONG,
                                    ISC_LONG*);

    inline ISC_STATUS isc_send(ISC_STATUS*,
                               isc_req_handle*,
                               short,
                               short,
                               const void*,
                               short);

    inline ISC_STATUS isc_start_and_send(ISC_STATUS*,
                                         isc_req_handle*,
                                         isc_tr_handle*,
                                         short,
                                         short,
                                         const void*,
                                         short);

    inline ISC_STATUS isc_start_request(ISC_STATUS *,
                                        isc_req_handle *,
                                        isc_tr_handle *,
                                        short);

    inline ISC_STATUS isc_unwind_request(ISC_STATUS *,
                                         isc_tr_handle *,
                                         short);

    inline ISC_STATUS isc_wait_for_event(ISC_STATUS*,
                                         isc_db_handle*,
                                         short,
                                         const ISC_UCHAR*,
                                         ISC_UCHAR*);



    inline ISC_STATUS isc_close(ISC_STATUS*,
                                const ISC_SCHAR*);

    inline ISC_STATUS isc_declare(ISC_STATUS*,
                                  const ISC_SCHAR*,
                                  const ISC_SCHAR*);

    inline ISC_STATUS isc_describe(ISC_STATUS*,
                                   const ISC_SCHAR*,
                                   XSQLDA *);

    inline ISC_STATUS isc_describe_bind(ISC_STATUS*,
                                        const ISC_SCHAR*,
                                        XSQLDA*);

    inline ISC_STATUS isc_execute(ISC_STATUS*,
                                  isc_tr_handle*,
                                  const ISC_SCHAR*,
                                  XSQLDA*);

    inline ISC_STATUS isc_execute_immediate(ISC_STATUS*,
                                            isc_db_handle*,
                                            isc_tr_handle*,
                                            short*,
                                            const ISC_SCHAR*);

    inline ISC_STATUS isc_fetch(ISC_STATUS*,
                                const ISC_SCHAR*,
                                XSQLDA*);

    inline ISC_STATUS isc_open(ISC_STATUS*,
                               isc_tr_handle*,
                               const ISC_SCHAR*,
                               XSQLDA*);

    inline ISC_STATUS isc_prepare(ISC_STATUS*,
                                  isc_db_handle*,
                                  isc_tr_handle*,
                                  const ISC_SCHAR*,
                                  short*,
                                  const ISC_SCHAR*,
                                  XSQLDA*);



    inline ISC_STATUS isc_dsql_execute_m(ISC_STATUS*,
                                         isc_tr_handle*,
                                         isc_stmt_handle*,
                                         unsigned short,
                                         const ISC_SCHAR*,
                                         unsigned short,
                                         unsigned short,
                                         ISC_SCHAR*);

    inline ISC_STATUS isc_dsql_execute2_m(ISC_STATUS*,
                                          isc_tr_handle*,
                                          isc_stmt_handle*,
                                          unsigned short,
                                          const ISC_SCHAR*,
                                          unsigned short,
                                          unsigned short,
                                          const ISC_SCHAR*,
                                          unsigned short,
                                          ISC_SCHAR*,
                                          unsigned short,
                                          unsigned short,
                                          ISC_SCHAR*);

    inline ISC_STATUS isc_dsql_execute_immediate_m(ISC_STATUS*,
                                                   isc_db_handle*,
                                                   isc_tr_handle*,
                                                   unsigned short,
                                                   const ISC_SCHAR*,
                                                   unsigned short,
                                                   unsigned short,
                                                   const ISC_SCHAR*,
                                                   unsigned short,
                                                   unsigned short,
                                                   ISC_SCHAR*);

    inline ISC_STATUS isc_dsql_exec_immed3_m(ISC_STATUS*,
                                             isc_db_handle*,
                                             isc_tr_handle*,
                                             unsigned short,
                                             const ISC_SCHAR*,
                                             unsigned short,
                                             unsigned short,
                                             const ISC_SCHAR*,
                                             unsigned short,
                                             unsigned short,
                                             ISC_SCHAR*,
                                             unsigned short,
                                             ISC_SCHAR*,
                                             unsigned short,
                                             unsigned short,
                                             ISC_SCHAR*);

    inline ISC_STATUS isc_dsql_fetch_m(ISC_STATUS*,
                                       isc_stmt_handle*,
                                       unsigned short,
                                       const ISC_SCHAR*,
                                       unsigned short,
                                       unsigned short,
                                       ISC_SCHAR*);

    inline ISC_STATUS isc_dsql_insert_m(ISC_STATUS*,
                                        isc_stmt_handle*,
                                        unsigned short,
                                        const ISC_SCHAR*,
                                        unsigned short,
                                        unsigned short,
                                        const ISC_SCHAR*);

    inline ISC_STATUS isc_dsql_prepare_m(ISC_STATUS*,
                                         isc_tr_handle*,
                                         isc_stmt_handle*,
                                         unsigned short,
                                         const ISC_SCHAR*,
                                         unsigned short,
                                         unsigned short,
                                         const ISC_SCHAR*,
                                         unsigned short,
                                         ISC_SCHAR*);

    inline ISC_STATUS isc_dsql_release(ISC_STATUS*,
                                       const ISC_SCHAR*);

    inline ISC_STATUS isc_embed_dsql_close(ISC_STATUS*,
                                           const ISC_SCHAR*);

    inline ISC_STATUS isc_embed_dsql_declare(ISC_STATUS*,
                                             const ISC_SCHAR*,
                                             const ISC_SCHAR*);

    inline ISC_STATUS isc_embed_dsql_describe(ISC_STATUS*,
                                              const ISC_SCHAR*,
                                              unsigned short,
                                              XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_describe_bind(ISC_STATUS*,
                                                   const ISC_SCHAR*,
                                                   unsigned short,
                                                   XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_execute(ISC_STATUS*,
                                             isc_tr_handle*,
                                             const ISC_SCHAR*,
                                             unsigned short,
                                             XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_execute2(ISC_STATUS*,
                                              isc_tr_handle*,
                                              const ISC_SCHAR*,
                                              unsigned short,
                                              XSQLDA*,
                                              XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_execute_immed(ISC_STATUS*,
                                                   isc_db_handle*,
                                                   isc_tr_handle*,
                                                   unsigned short,
                                                   const ISC_SCHAR*,
                                                   unsigned short,
                                                   XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_fetch(ISC_STATUS*,
                                           const ISC_SCHAR*,
                                           unsigned short,
                                           XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_open(ISC_STATUS*,
                                          isc_tr_handle*,
                                          const ISC_SCHAR*,
                                          unsigned short,
                                          XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_open2(ISC_STATUS*,
                                           isc_tr_handle*,
                                           const ISC_SCHAR*,
                                           unsigned short,
                                           XSQLDA*,
                                           XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_insert(ISC_STATUS*,
                                            const ISC_SCHAR*,
                                            unsigned short,
                                            XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_prepare(ISC_STATUS*,
                                             isc_db_handle*,
                                             isc_tr_handle*,
                                             const ISC_SCHAR*,
                                             unsigned short,
                                             const ISC_SCHAR*,
                                             unsigned short,
                                             XSQLDA*);

    inline ISC_STATUS isc_embed_dsql_release(ISC_STATUS*,
                                             const ISC_SCHAR*);


    inline BSTREAM* BLOB_open(isc_blob_handle,
                              ISC_SCHAR*,
                              int);

    inline int BLOB_put(ISC_SCHAR,
                        BSTREAM*);

    inline int BLOB_close(BSTREAM*);

    inline int BLOB_get(BSTREAM*);

    inline int BLOB_display(ISC_QUAD*,
                            isc_db_handle,
                            isc_tr_handle,
                            const ISC_SCHAR*);

    inline int BLOB_dump(ISC_QUAD*,
                         isc_db_handle,
                         isc_tr_handle,
                         const ISC_SCHAR*);

    inline int BLOB_edit(ISC_QUAD*,
                         isc_db_handle,
                         isc_tr_handle,
                         const ISC_SCHAR*);

    inline int BLOB_load(ISC_QUAD*,
                         isc_db_handle,
                         isc_tr_handle,
                         const ISC_SCHAR*);

    inline int BLOB_text_dump(ISC_QUAD*,
                              isc_db_handle,
                              isc_tr_handle,
                              const ISC_SCHAR*);

    inline int BLOB_text_load(ISC_QUAD*,
                              isc_db_handle,
                              isc_tr_handle,
                              const ISC_SCHAR*);

    inline BSTREAM* Bopen(ISC_QUAD*,
                          isc_db_handle,
                          isc_tr_handle,
                          const ISC_SCHAR*);



    inline ISC_LONG isc_ftof(const ISC_SCHAR*,
                             const unsigned short,
                             ISC_SCHAR*,
                             const unsigned short);

    inline ISC_STATUS isc_print_blr(const ISC_SCHAR*,
                                    ISC_PRINT_CALLBACK,
                                    void*,
                                    short);

    inline void isc_set_debug(int);

    inline void isc_qtoq(const ISC_QUAD*,
                         ISC_QUAD*);

    inline void isc_vtof(const ISC_SCHAR*,
                         ISC_SCHAR*,
                         unsigned short);

    inline void isc_vtov(const ISC_SCHAR*,
                         ISC_SCHAR*,
                         short);

    inline int isc_version(isc_db_handle*,
                           ISC_VERSION_CALLBACK,
                           void*);

    inline ISC_LONG isc_reset_fpe(ISC_USHORT);





    inline ISC_STATUS isc_service_attach(ISC_STATUS*,
                                         unsigned short,
                                         const ISC_SCHAR*,
                                         isc_svc_handle*,
                                         unsigned short,
                                         const ISC_SCHAR*);

    inline ISC_STATUS isc_service_detach(ISC_STATUS *,
                                         isc_svc_handle *);

    inline ISC_STATUS isc_service_query(ISC_STATUS*,
                                        isc_svc_handle*,
                                        isc_resv_handle*,
                                        unsigned short,
                                        const ISC_SCHAR*,
                                        unsigned short,
                                        const ISC_SCHAR*,
                                        unsigned short,
                                        ISC_SCHAR*);

    inline ISC_STATUS isc_service_start(ISC_STATUS*,
                                        isc_svc_handle*,
                                        isc_resv_handle*,
                                        unsigned short,
                                        const ISC_SCHAR*);



    inline void isc_get_client_version( ISC_SCHAR  *);
    inline int isc_get_client_major_version();
    inline int isc_get_client_minor_version();

                
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
