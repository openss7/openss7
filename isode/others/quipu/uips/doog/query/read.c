#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/read.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/read.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*
 * $Log: read.c,v $
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************


*****************************************************************************/

#include "types.h"
#include "util.h"
#include "read.h"
#include "request.h"
#include "ds_tasks.h"

#include "quipu/common.h"
#include "quipu/util.h"
#include "quipu/dap2.h"
#include "quipu/attrvalue.h"
#include "quipu/photo.h"

extern int dsap_ad;
extern int next_task_id;

#ifndef NO_STATS
extern LLog    *log_stat;
#endif

/* Not static as are used by other parts of query engine */
QE_error_code start_read();
void get_read_attrs();

int photo_width, photo_height, tmp_width;
PS photo_ps;
int bit_count, byte_count;
unsigned char photo_byte;
unsigned char *photo_bytes, *photo_ptr;

QBool photo_pass_two;

/*
 * - get_read_results() -
 *
 *
 */
readResults get_read_results(request_id)
     QCardinal request_id;
{
  requestRec request = _get_request_of_id(request_id);
  readRec readrec;
  readResults results;

  if (request == NULLReqRec) return NULLReadResults;

  readrec = request->READ_REC;
  results = readrec->results;

  readrec->results = NULLReadResults;

  _request_complete(request_id);

  return results;
} /* get_read_results */

/*
 * - do_read() -
 *
 *
 */
QE_error_code do_read(baseobject, id_ptr, attr_list)
     char *baseobject;
     QCardinal *id_ptr;
     stringCell attr_list;
{
  readRec new_request;
  requestRec this_request;
  
  (void) _request_invoked(DS_READ, id_ptr);
  this_request = _get_request_of_id(*id_ptr);

  new_request = this_request->READ_REC;

  new_request->base_object = copy_string(baseobject);
  new_request->request_id = *id_ptr;

  return start_read(*id_ptr, baseobject, attr_list, &new_request->task_id);
} /* do_read */

/*
 * - process_read() -
 *
 *
 */
QE_error_code start_read(request_id, baseobject, attr_list, task_id_ptr)
     QCardinal request_id;
     char *baseobject;
     stringCell attr_list;
     int *task_id_ptr;
{
  int task_id;
  struct ds_read_arg read_arg;
  struct DAPindication di;
  Attr_Sequence read_types = NULLATTR, merge_types;
  AttributeType attr_type;

  if (baseobject == NULLCP || isnull(*baseobject)) return QERR_bad_name;

  while (attr_list != NULLStrCell)
    {
      if ((attr_type = AttrT_new(attr_list->string)) == NULLAttrT)
	continue;

      if (!read_types) 
	read_types = as_comp_new(attr_type, NULLAV, NULLACL_INFO);
      else
	{
	  merge_types = as_comp_new(attr_type, NULLAV, NULLACL_INFO);
	  read_types = as_merge(read_types, merge_types);
	}

      attr_list = attr_list->next;
    }

  (void) get_default_service(&read_arg.rda_common);

  read_arg.rda_common.ca_servicecontrol.svc_options =
    read_arg.rda_common.ca_servicecontrol.svc_options | SVC_OPT_PREFERCHAIN;
  
  read_arg.rda_common.ca_servicecontrol.svc_timelimit = -1;

  read_arg.rda_eis.eis_infotypes = EIS_ATTRIBUTESANDVALUES;
  if (read_types == NULLATTR)
    {
      read_arg.rda_eis.eis_allattributes = TRUE;
      read_arg.rda_eis.eis_select = NULLATTR;
    }
  else
    {
      read_arg.rda_eis.eis_allattributes = FALSE;
      read_arg.rda_eis.eis_select = read_types;
    }

  read_arg.rda_object = str2dn(baseobject);

  if (_task_invoked(READ_TASK, baseobject, request_id, &task_id) 
      != QERR_ok)
    {
      return QERR_internal_limit_reached;
    }

  *task_id_ptr = task_id;

  if (DapRead(dsap_ad, task_id, &read_arg, &di, ROS_ASYNC) == NOTOK)
    {
      return QERR_request_failed;
    }

#ifndef NO_STATS
  LLOG (log_stat, LLOG_NOTICE,
	("READ +%s, task %d",
	 baseobject,
	 task_id));
#endif

  return QERR_ok;
} /* start_read */


/*
 * - process_read_ds_result() -
 *
 *
 */
request_state process_read_ds_result(request, task_id, ds_result)
     requestRec request;
     int task_id;
     struct DSResult *ds_result;
{
  DsTask task_rec;
  readRec readrec = request->READ_REC;
  struct ds_read_result *read_result;

  /* This shouldn't happen */
  if ((task_rec = _get_task_of_id(task_id)) == NULLDsTask)
    return RQ_processing;

  read_result = &ds_result->res_rd;

  readrec->results = read_res_alloc();

  readrec->results->entry = NULLAVList; 
  readrec->results->base_object = readrec->base_object;
  readrec->base_object = NULLCP;

  get_read_attrs(read_result->rdr_entry.ent_attr,
		 &readrec->results->entry,
		 READOUT);

  readrec->results->errors = request->errors;
  request->errors = NULLError;

  _task_complete(task_id);

#ifndef NO_STATS
  LLOG (log_stat, LLOG_NOTICE,
	("READ RESULT +%s, task %d", 
	 task_rec->baseobject,
	 task_id));
#endif

  return RQ_results_returned;
} /* process_read_ds_result */


/*
 * - process_read_ds_error() -
 *
 *
 */
request_state process_read_ds_error(request, task_id, error)
     requestRec request;
     int task_id;
     struct DSError *error;
{
  DsTask task_rec;
  readRec readrec = request->READ_REC;
  QE_error_code error_type;

  task_rec = _get_task_of_id(task_id);

  /* This shouldn't happen */
  if (task_rec == NULLDsTask) return RQ_processing;
  
  /* Save error and log it */
  error_type = get_log_error_type(error, task_id);
  add_error_to_request_rec(request, task_rec->baseobject, error_type, error);

  readrec->results = read_res_alloc();
  readrec->results->errors = request->errors;
  readrec->results->entry = NULLAVList;

  readrec->results->base_object = readrec->base_object;
  readrec->base_object = NULLCP;
  request->errors = NULLError;

  /* Remove invocation record for this task */
  _task_complete(task_id);

  return RQ_results_returned;
} /* process_read_ds_error */
  
/*
 * - read_result_free() -
 *
 *
 */
void read_result_free(result_ptr)
     readResults *result_ptr;
{
  attrValList av_list, av_last;

  readResults result = *result_ptr;
  if (result == NULLReadResults) return;

  for (av_list = result->entry, av_last = NULLAVList;
       av_list != NULLAVList;
       av_last = av_list, av_list = av_list->next, free((char *) av_last)) {
    free_string_seq(&av_list->attr_name);
    free_string_seq(&av_list->val_list);
  }
  
  error_list_free(&result->errors);

  if (result->base_object != NULLCP) free(result->base_object);

  free((char *) result);
  *result_ptr = NULLReadResults;
} /* read_result_free */

/*
 * - read_rec_free() -
 * Assumes that results and errors have already been freed.
 *
 */
void read_rec_free(record)
     readRec record;
{
  if (record == NULLReadRec) return;

  if (record->base_object != NULLCP) free(record->base_object);
  free((char *) record);
} /* read_rec_free */

/*
 * - read_dn_attr_rec_free() -
 * Assumes that results and errors have already been freed.
 *
 */
void read_dn_attr_rec_free(record)
     readDnAttrRec record;
{
  if (record == NULLReadDnAttrRec) return;

  if (record->base_object != NULLCP) free(record->base_object);
  if (record->read_object != NULLCP) free(record->read_object);

/*   free_string_seq(&record->read_attrs); */
  free_string_seq(&record->dn_attr);

  free((char *) record);
} /* read_rec_free */

/*
 * - get_read_attrs() -
 *
 *
 */
void get_read_attrs(readattrs, entryattrs, format)
     Attr_Sequence readattrs;
     attrValList *entryattrs;
     int format;
{
  PS valps;
  photoData photo_data = (photoData) NULL;
  QBool is_photo;
  stringCell curr_type = NULLStrCell, curr_val = NULLStrCell;
  attrValList curr_attr;
  Attr_Sequence curr_as;
  AV_Sequence curr_av;
  extern int oidformat;

  if (entryattrs == (attrValList *) NULL) return;

  for (curr_as = readattrs, curr_attr = NULLAVList;
       curr_as != NULLATTR;
       curr_as = curr_as->attr_link)
    {
      is_photo = FALSE;
      
      if (curr_attr == NULLAVList)
	{
	  *entryattrs = curr_attr = av_list_alloc();
	}
      else
	{
	  curr_attr->next = av_list_alloc(); 
	  curr_attr = curr_attr->next;
	}
      
      curr_attr->next = NULLAVList;
      curr_type = curr_attr->attr_name = NULLStrCell;
      curr_val = curr_attr->val_list = NULLStrCell;
      
      if (format == READOUT)
	{
	  add_string_to_seq(attr2name(curr_as->attr_type, OIDPART),
			    &curr_type);
	}
      else
	{
	  add_string_to_seq(attr2name_aux(curr_as->attr_type), &curr_type);
	}

      if (curr_as->attr_type == AttrT_new("photo"))
	{
	  is_photo = TRUE;
	  if ((valps = ps_alloc(str_open)) == NULLPS) return;
	  
	  if (str_setup(valps, NULLCP, 0, 0) == NOTOK)
	    {
	      ps_free(valps);
	      return;
	    }
	  
	  avs_print(valps, curr_as->attr_value, format);
	  
	  photo_data = (photoData) smalloc(sizeof(struct _photo_data));
	  
	  photo_data->width = photo_width;
	  photo_data->height = photo_height;

	  photo_data->bytes = photo_bytes;

	  *(valps->ps_base) = NULL;
	  *(valps->ps_ptr) = NULL;

	  ps_free(valps);
	}
      else if (split_attr(curr_as)) /* If a multiline value */
	{
	  if (curr_as->attr_value == NULLAV)
	    {
	      if (format != READOUT)
		add_string_to_seq("", &curr_val);
	      else
		add_string_to_seq("NO VALUE", &curr_val);
	    }
	  else
	    {
	      for (curr_av = curr_as->attr_value;
		   curr_av != NULLAV;
		   curr_av = curr_av->avseq_next)
		{
		  if ((valps = ps_alloc(str_open)) == NULLPS) return;
		  
		  if (str_setup(valps, NULLCP, 0, 0) == NOTOK)
		    {
		      ps_free(valps);
		      return;
		    }
		  
		  avs_comp_print(valps, curr_av, format);
		  
		  *(valps->ps_ptr) = NULL;
		  valps->ps_cnt++;
		  
		  add_string_to_seq(valps->ps_base, &curr_val);
		  
		  ps_free(valps);
		}
	    }
	}
      else
	{ /* If not a multi line value */
	  if ((valps = ps_alloc(str_open)) == NULLPS) return;
	  
	  if (str_setup(valps, NULLCP, 0, 0) == NOTOK)
	    {
	      ps_free(valps);
	      return;
	    }
	  
	  avs_print(valps, curr_as->attr_value, format);
	  *(valps->ps_ptr) = NULL;
	  valps->ps_cnt++;
	  
	  add_string_to_seq(valps->ps_base, &curr_val);
	  
	  ps_free(valps);
	}
      
      curr_attr->attr_name = curr_type;
      
      if (is_photo)
	{
	  curr_attr->photo = photo_data;
	  curr_attr->is_photo = TRUE;

	  curr_attr->val_list = NULLStrCell;
	}
      else
	{
	  curr_attr->val_list = curr_val;
	  curr_attr->is_photo = FALSE;
	}
    }
} /* get_read_attrs */


/*
 * - photo2xbm() -
 *
 *
 */
/* ARGSUSED */
int photo2xbm(ps, picture, format)
     PS ps;
     PE picture;
     int format;
{
  PS sps;

  if ((sps = ps_alloc(str_open)) == NULLPS)
    return 0;

  if (str_setup (sps, NULLCP, LINESIZE, 0) == NOTOK)
    {
      ps_free(sps);
      return 0;
    }

  photo_ps = ps;

  (void) pe2ps(sps, picture);

  photo_pass_two = FALSE;
  (void)decode_t4(sps->ps_base, "photo", 0);

  photo_pass_two = TRUE;
  (void)decode_t4(sps->ps_base, "photo", 0);

  ps_free(sps);

  return 0;
} /* photo2xbm */

/* ARGSUSED */
int photo_start(name)
     char *name;
{
  if (photo_pass_two == FALSE)
    {
      tmp_width = photo_width = photo_height = 0;
      return 0;
    }

  bit_count = 0;
  photo_byte = 0;

  return 0;
}

/* ARGSUSED */
int photo_end(name)
     char *name;
{
  register int byte_width;
  
  if (photo_pass_two == FALSE)
    {
      byte_width = (photo_width / 8) + 1;
      photo_height ++;

      photo_ptr = photo_bytes =
	(unsigned char *) smalloc(byte_width * photo_height);

      byte_count = byte_width * photo_height;
    }

  return 0;
}

int photo_black(line_length)
     int line_length;
{
  register unsigned char tmp_byte;
  register int count;

  if (photo_pass_two == FALSE)
    {
      tmp_width += line_length;
      return 0;
    }

  tmp_byte = photo_byte;

  for (count = 0; count < line_length; count++)
    {
      switch (bit_count)
	{
	case 0:
	  tmp_byte = tmp_byte | 0x01;
	  break;
	  
	case 1:
	  tmp_byte = tmp_byte | 0x02;
	  break;
	  
	case 2:
	  tmp_byte = tmp_byte | 0x04;
	  break;
	  
	case 3:
	  tmp_byte = tmp_byte | 0x08;
	  break;
	  
	case 4:
	  tmp_byte = tmp_byte | 0x10;
	  break;
	  
	case 5:
	  tmp_byte = tmp_byte | 0x20;
	  break;
	  
	case 6:
	  tmp_byte = tmp_byte | 0x40;
	  break;
	  
	case 7:
	  tmp_byte = tmp_byte | 0x80;

	  break;
	}

      if (++bit_count == 8)
	{
	  if (byte_count > (photo_ptr - photo_bytes))
	      *photo_ptr++ = tmp_byte;

	  tmp_byte = 0;
	  bit_count = 0;
	}
    }

  photo_byte = tmp_byte;

  return 0;
}

int photo_white(line_length)
     int line_length;
{
  register int count;

  if (photo_pass_two == FALSE)
    {
      tmp_width += line_length;
      return 0;
    }

  for (count = 0; count < line_length; count++)
    if (++bit_count == 8)
      {
	if (byte_count > (photo_ptr - photo_bytes))
	  {
	    *photo_ptr = photo_byte;
	    photo_ptr++;
	  }

	photo_byte = 0;
	bit_count = 0;
      }

  return 0;
}

/* ARGSUSED */
int photo_line_end(line)
     bit_string *line;
{
  /* the end of a line has been reached */
  /* A bit string is stored in line dbuf_top */

  if (photo_pass_two == FALSE)
    {
      photo_height++;

      if (photo_width == 0)
	photo_width = tmp_width;

      return 0;
    }

  if (bit_count != 0)
    {
      if (byte_count > (photo_ptr - photo_bytes))
	{
	  *photo_ptr = photo_byte;
	  photo_ptr++;
	}

      photo_byte = 0;
      bit_count = 0;
    }

  return 0;
}

/*
 * - do_dn_attr_read() -
 *
 *
 */
QE_error_code do_dn_attr_read(baseobject, id_ptr, dn_attr)
     char *baseobject;
     QCardinal *id_ptr;
     stringCell dn_attr;
{
  readDnAttrRec new_request;
  requestRec this_request;
  
  (void) _request_invoked(READ_DN_ATTR, id_ptr);
  this_request = _get_request_of_id(*id_ptr);

  new_request = this_request->READ_DN_ATTR_REC;

  new_request->base_object = copy_string(baseobject);
  new_request->read_object = NULLCP;

  new_request->request_id = *id_ptr;

  new_request->task_id = NO_TASK;

  new_request->dn_attr = copy_string_seq(dn_attr);

  return start_read(*id_ptr, baseobject, dn_attr, &new_request->task_id);
} /* do_dn_attr_read */

/*
 * - process_dn_attr_read_result() -
 *
 *
 */
request_state process_read_dn_attr_result(request, task_id, ds_result)
     requestRec request;
     int task_id;
     struct DSResult *ds_result;
{
  DsTask task_rec;
  readDnAttrRec readrec = request->READ_DN_ATTR_REC;
  struct ds_read_result *read_result;
  attrValList dn_attr = NULLAVList;

  /* This shouldn't happen */
  if ((task_rec = _get_task_of_id(task_id)) == NULLDsTask)
    return RQ_processing;

  read_result = &ds_result->res_rd;

#ifndef NO_STATS
  LLOG (log_stat,
	LLOG_NOTICE,
	("READ RESULT +%s, task %d", task_rec->baseobject, task_id));
#endif

  get_read_attrs(read_result->rdr_entry.ent_attr, &dn_attr, EDBOUT);
  
  _task_complete(task_id);
  
  readrec->results = read_res_alloc();

  readrec->results->errors = NULLError;

  readrec->results->base_object = readrec->base_object;
  readrec->base_object = NULLCP;

  readrec->results->entry = dn_attr;
  
  return RQ_results_returned;
} /* process_dn_attr_read_result */

/*
 * - process_read_ds_error() -
 *
 *
 */
request_state process_read_dn_attr_error(request, task_id, error)
     requestRec request;
     int task_id;
     struct DSError *error;
{
  DsTask task_rec;
  readDnAttrRec readrec = request->READ_DN_ATTR_REC;
  QE_error_code error_type;

  task_rec = _get_task_of_id(task_id);

  /* This shouldn't happen */
  if (task_rec == NULLDsTask) return RQ_processing;
  
  /* Save error and log it */
  error_type = get_log_error_type(error, task_id);
  add_error_to_request_rec(request, task_rec->baseobject, error_type, error);

  readrec->results = read_res_alloc();

  readrec->results->errors = request->errors;
  readrec->results->entry = NULLAVList;

  readrec->results->base_object = readrec->read_object;
  readrec->base_object = NULLCP;

  request->errors = NULLError;

  /* Remove invocation record for this task */
  _task_complete(task_id);

  return RQ_results_returned;
} /* process_read_dn_attr_error */

/*
 * - get_read_results() -
 *
 *
 */
readResults get_read_dn_attr_results(request_id)
     QCardinal request_id;
{
  requestRec request = _get_request_of_id(request_id);
  readDnAttrRec readrec;
  readResults results;

  if (request == NULLReqRec) return NULLReadResults;

  readrec = request->READ_DN_ATTR_REC;
  results = readrec->results;

  readrec->results = NULLReadResults;

  _request_complete(request_id);

  return results;
} /* get_read_dn_attr_results */

