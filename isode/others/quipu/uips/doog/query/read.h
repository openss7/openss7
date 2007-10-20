/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/read.h,v 9.0 1992/06/16 12:45:27 isode Rel $ */

#ifndef _query_read_h_
#define _query_read_h_

#include "types.h"
#include "error.h"
#include "sequence.h"

typedef struct _photo_data
{
  int width;
  int height;

  unsigned char *bytes;
} *photoData;

typedef struct _attr_val_list
{
  stringCell attr_name;
  
  QBool is_photo;

  stringCell val_list;
  photoData photo;

  struct _attr_val_list *next;
} attr_val_list, *attrValList;

#define NULLAVList (attrValList) NULL
#define av_list_alloc() (attrValList) smalloc(sizeof(attr_val_list))


typedef struct _read_results
{
  char *base_object;
  attrValList entry;
  errorList errors;
} read_results, *readResults;

#define NULLReadResults (readResults) NULL
#define read_res_alloc() (readResults) smalloc(sizeof(read_results))


typedef struct _read_rec
{
  QCardinal request_id;

  char *base_object;

  int task_id;

  readResults results;
} read_rec, *readRec;

#define NULLReadRec (readRec) NULL
#define read_rec_alloc() (readRec) smalloc(sizeof(read_rec))


typedef struct _read_dn_attr_rec
{
  QCardinal request_id;

  char *base_object;
  char *read_object;

  int task_id;

  stringCell read_attrs;
  stringCell dn_attr;

  readResults results;
} read_dn_attr_rec, *readDnAttrRec;

#define NULLReadDnAttrRec (readDnAttrRec) NULL
#define read_dn_attr_rec_alloc() \
        (readDnAttrRec) smalloc(sizeof(read_dn_attr_rec))

/*
 *	Procs
 */
QE_error_code do_read();
QE_error_code do_dn_attr_read();

request_state process_read_ds_result(),
	      process_read_ds_error();

request_state process_read_dn_attr_result(),
  	      process_read_dn_attr_error();

readResults get_read_results(), get_read_dn_attr_results();

void read_rec_free(), read_dn_attr_rec_free(), read_result_free();

int photo2xbm();

#endif
