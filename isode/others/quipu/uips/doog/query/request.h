/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/request.h,v 9.0 1992/06/16 12:45:27 isode Rel $ */

#ifndef _query_request_h_
#define _query_request_h_

#include "types.h"
#include "error.h"
#include "ufname.h"
#include "ufsearch.h"
#include "modify.h"
#include "read.h"

#define MAXREQUESTS 64

/*
 *	Request type identifier
 */
typedef enum
{
  DS_READ,
  READ_DN_ATTR,
  DS_SEARCH,
  UFNAME,
  UFSEARCH,
  ENTRY_TEMPLATE,
  MODIFY_ENTRY,
  MODIFY_RDN,
  REMOVE_ENTRY,
  ADD_ENTRY
} request_type;

/*
 *	Generic request record
 */
typedef struct _request_rec
{
  request_state status;
  errorList errors;
  request_type type;
  QCardinal request_id;

  struct _request_rec *next;
  struct _request_rec *prev;
  
  union
    {
      ufnameRec ufname_request;
      ufsearchRec ufsearch_request;
      readRec read_request;
      readDnAttrRec read_dn_attr_request;
      modifyRec modify_request;
      makeTemplateRec template_request;
    } req_type_rec;

#define UFNAME_REC req_type_rec.ufname_request
#define UFSEARCH_REC req_type_rec.ufsearch_request
#define READ_REC req_type_rec.read_request
#define READ_DN_ATTR_REC req_type_rec.read_dn_attr_request
#define MODIFY_REC req_type_rec.modify_request
#define TEMPLATE_REC req_type_rec.template_request

} request_rec, *requestRec;

#define NULLReqRec (requestRec) NULL
#define request_rec_alloc() (requestRec) smalloc(sizeof(request_rec))

/*
 *	Procedures
 */

QE_error_code _request_invoked();
void _request_complete();

QCardinal directory_wait();
void abort_request();
requestRec _get_request_of_id();

#endif _query_request_h_
