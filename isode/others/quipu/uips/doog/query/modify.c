#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/modify.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/modify.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*****************************************************************************

  modify.c - Modify entry attributes.

*****************************************************************************/

#include "types.h"
#include "util.h"
#include "read.h"
#include "modify.h"
#include "request.h"
#include "ds_tasks.h"

#include "quipu/common.h"
#include "quipu/entry.h"
#include "quipu/modify.h"
#include "quipu/name.h"
#include "quipu/util.h"
#include "quipu/dap2.h"
#include "quipu/attrvalue.h"
#include "quipu/photo.h"

#define AS_SYNTAX(attrSeq) attrSeq->attr_type->oa_syntax
#define AS_STROID(attrSeq) attrSeq->attr_type->oa_ot.ot_stroid

extern int dsap_ad;
extern int next_task_id;

extern void get_read_attrs();

#ifndef NO_STATS
extern LLog *log_stat;
#endif

static errorList start_modify();
static struct entrymod * ems_append();

static void get_template_attrs();
static modifyAttr remove_common_attrs(), remove_double_attrs();

extern QE_error_code start_read();

/*
 * 	Start of procedures for entry modification requests.
 *
 *
 */

/*
 * - get_modify_result() -
 *
 *
 */
modifyResult get_modify_result(request_id)
     QCardinal request_id;
{
  requestRec request = _get_request_of_id(request_id);
  modifyRec modrec;
  modifyResult result;

  if (request == NULLReqRec) return NULLModifyResult;

  modrec = request->MODIFY_REC;
  result = modrec->result;

  modrec->result = NULLModifyResult;

  _request_complete(request_id);

  return result;
} /* get_modify_result */

/*
 * - do_modify() -
 *
 *
 */
errorList do_modify(baseobject, id_ptr, mod_attr_list)
     char *baseobject;
     QCardinal *id_ptr;
     modifyAttr mod_attr_list;
{
  modifyRec modify_request;
  requestRec this_request;

  (void) _request_invoked(MODIFY_ENTRY, id_ptr);
  this_request = _get_request_of_id(*id_ptr);

  modify_request = this_request->MODIFY_REC;

  modify_request->request_id = *id_ptr;
  modify_request->base_object = copy_string(baseobject);

  return start_modify(*id_ptr,
		      baseobject,
		      mod_attr_list,
		      &modify_request->task_id);
} /* do_modify */

/*
 * - start_modify() -
 *
 *
 */
static errorList start_modify(request_id,
			      baseobject,
			      mod_attr_list,
			      task_id_ptr)
     QCardinal request_id;
     char *baseobject;
     modifyAttr mod_attr_list;
     int *task_id_ptr;
{
  int task_id;

  struct ds_modifyentry_arg mod_arg;
  struct DAPindication di;
  struct entrymod *curr_mod = (struct entrymod *) NULL,
                  *entrymods = (struct entrymod *) NULL;

  char err_msg[LINESIZE];
  errorList error;

  modifyAttr curr_attr = mod_attr_list;
  modifyValue curr_val;
  AttributeType curr_type = NULLAttrT;
  AttributeValue attr_val = (AttributeValue) NULL;
  AV_Sequence attrVal_seq = (AV_Sequence) NULL;

  /* Quipu procedures */
  int avs_free(), as_free();

  for (curr_attr = mod_attr_list;
       curr_attr != NULLModifyAttr;
       curr_attr = curr_attr->next)
    {
      if (curr_attr->to_modify)
	{
	  QCardinal val_count = 0,    /* Number of existing values */
	            delete_count = 0, /* Number of values to delete */
	            new_count = 0;    /* Number of values that need adding */

	  curr_type = curr_attr->attr_type;

/*
 *	Check the modification status of this attribute.
 *
 */

	  for (curr_val = curr_attr->values;
	       curr_val != NULLModifyValue;
	       curr_val = curr_val->next)
	    {
	      switch (curr_val->status)
		{
		case updated:

		  new_count++;
		  delete_count++;
		  val_count++;

		  break;

		case deleted:
		  
		  delete_count++;
		  val_count++;

		  break;

		case new:
		  
		  new_count++;

		  break;

		default:
		  
		  val_count++;
		}
	    }

	  curr_mod = em_alloc();

	  curr_mod->em_next = NULLMOD;
	  curr_mod->em_what = NULL;
	  curr_mod->em_type = 127;

/*
 * 	If 
 * 	   i) There are NO values to add, and
 *	   ii) There are existing values, and
 *	   iii) All existing values are marked for deletion.
 *	Then
 *         Delete the attribute.
 */
	  if (new_count == 0 && val_count > 0 && val_count == delete_count)
	    {
	      curr_mod->em_type = EM_REMOVEATTRIBUTE;

	      curr_mod->em_what = as_comp_new(curr_attr->attr_type,
					      NULLAV,
					      NULLACL_INFO);
	    }
/*
 *	Else if
 *	   i) There are values to add, and
 *	   ii) There are no existing values.
 *	Then
 *	   Add a new attribute with given values.
 */
	  else if (new_count > 0 && val_count == 0)
	    {
	      curr_mod->em_type = EM_ADDATTRIBUTE;
	      
	      for (curr_val = curr_attr->values;
		   curr_val != NULLModifyValue;
		   curr_val = curr_val->next)
		{
		  attr_val = str2AttrV(curr_val->new_value,
				       curr_type->oa_syntax);

		  if (attr_val != NULLAttrV)
		    {
		      if (attrVal_seq != NULLAV)
			attrVal_seq = avs_merge(attrVal_seq,
						avs_comp_new(attr_val));
		      else
			attrVal_seq = avs_comp_new(attr_val);
		    }
		  else /* Attribute value has an inappropriate syntax! */
		    {
		      if (entrymods) ems_free(entrymods);
		      if (attrVal_seq != NULLAV) avs_free(attrVal_seq);

		      (void) sprintf
			(err_msg,
			 "*** Bad Value `%s' in Attribute `%s' ***",
			 curr_val->new_value,
			 attr2name(curr_attr->attr_type, OIDPART));

		      error = error_alloc();
		      error->next = NULLError;

		      error->baseobject = NULLCP;
		      error->error_type = QERR_bad_value_syntax;
		      error->ds_message = copy_string(err_msg);

		      return error;
		    }
		}

	      /* Add value list to entrymods */
	      curr_mod->em_what = as_comp_new(curr_attr->attr_type,
					      attrVal_seq,
					      NULLACL_INFO);
	    }
/*
 *	Else if
 *	    i) There are existing values to be deleted
 *	Then
 *	    i) Remove marked values
 *	    ii) Check if any new values are given.
 */
	  else if (delete_count > 0)
	    {
	      curr_mod->em_type = EM_REMOVEVALUES;

	      for (curr_val = curr_attr->values;
		   curr_val != NULLModifyValue;
		   curr_val = curr_val->next)
		{
		  if (curr_val->status == updated ||
		      curr_val->status == deleted)
		    {
		      attr_val = str2AttrV(curr_val->value,
					   curr_type->oa_syntax);

		      if (attr_val != NULLAttrV)
			{
			  if (attrVal_seq != NULLAV)
			    attrVal_seq = avs_merge(attrVal_seq,
						    avs_comp_new(attr_val));
			  else
			    attrVal_seq = avs_comp_new(attr_val);
			}
		      else /* Attribute value has an inappropriate syntax! */
			{
			  if (entrymods) ems_free(entrymods);
			  
			  if (attrVal_seq != NULLAV) avs_free(attrVal_seq);
			  
			  (void) sprintf
			    (err_msg,
			     "*** Bad Value `%s' in Attribute `%s' ***",
			     curr_val->new_value,
			     attr2name(curr_attr->attr_type, OIDPART));
			  
			  error = error_alloc();
			  error->next = NULLError;
			  
			  error->baseobject = NULLCP;
			  error->error_type = QERR_bad_value_syntax;
			  error->ds_message = copy_string(err_msg);
			  
			  return error;
			}
		    }
		}

	      curr_mod->em_what = as_comp_new(curr_attr->attr_type,
					      attrVal_seq,
					      NULLACL_INFO);
	      
	      attrVal_seq = NULLAV;

/*
 *	Check if there are any new values to replace the ones just deleted.
 */

	      if (new_count > 0)
		{
		  entrymods = ems_append(entrymods, curr_mod);

		  curr_mod = em_alloc();
		  curr_mod->em_next = NULLMOD;

		  curr_mod->em_type = EM_ADDVALUES;

		  for (curr_val = curr_attr->values;
		       curr_val != NULLModifyValue;
		       curr_val = curr_val->next)
		    {
		      if (curr_val->status == updated ||
			  curr_val->status == new)
			{
			  attr_val = str2AttrV(curr_val->new_value,
					       curr_type->oa_syntax);

			  if (attr_val != NULLAttrV)
			    {
			      if (attrVal_seq != NULLAV)
				attrVal_seq = avs_merge(attrVal_seq,
						       avs_comp_new(attr_val));
			      else
				attrVal_seq = avs_comp_new(attr_val);
			    }
			  else /* Value has an inappropriate syntax! */
			    {
			      if (entrymods) ems_free(entrymods);
			      ems_free(curr_mod);
			      
			      if (attrVal_seq != NULLAV) avs_free(attrVal_seq);
			      
			      (void) sprintf
				(err_msg,
				 "*** Bad Value `%s' in Attribute `%s' ***",
				 curr_val->new_value,
				 attr2name(curr_attr->attr_type, OIDPART));
			      
			      error = error_alloc();
			      error->next = NULLError;
			      
			      error->baseobject = NULLCP;
			      error->error_type = QERR_bad_value_syntax;
			      error->ds_message = copy_string(err_msg);
			      
			      return error;
			    }
			}
		    }

		  if (attrVal_seq == NULLAV)
		    {
		      if (entrymods) ems_free(entrymods);
		      ems_free(curr_mod);

		      error = error_alloc();
		      error->next = NULLError;
		      
		      error->baseobject = NULLCP;
		      error->error_type = QERR_internal;
		      error->ds_message = NULLCP;
		      
		      return error;
		    }
		  else
		    curr_mod->em_what = as_comp_new(curr_attr->attr_type,
						    attrVal_seq,
						    NULLACL_INFO);
		}
	    }
	  else if (new_count > 0)
	    {
	      curr_mod->em_type = EM_ADDVALUES;
	      
	      for (curr_val = curr_attr->values;
		   curr_val != NULLModifyValue;
		   curr_val = curr_val->next)
		{
		  if (curr_val->status == new)
		    {
		      attr_val = str2AttrV(curr_val->new_value,
					   curr_type->oa_syntax);
		      
		      if (attr_val != NULLAttrV)
			{
			  if (attrVal_seq != NULLAV)
			    attrVal_seq = avs_merge(attrVal_seq,
						    avs_comp_new(attr_val));
			  else
			    attrVal_seq = avs_comp_new(attr_val);
			}
		      else /* Value has an inappropriate syntax! */
			{
			  if (entrymods) ems_free(entrymods);
			  ems_free(curr_mod);
			  
			  if (attrVal_seq != NULLAV) avs_free(attrVal_seq);
			  
			  (void) sprintf
			    (err_msg,
			     "*** Bad Value `%s' in Attribute `%s' ***",
			     curr_val->new_value,
			     attr2name(curr_attr->attr_type, OIDPART));
			  
			  error = error_alloc();
			  error->next = NULLError;
			  
			  error->baseobject = NULLCP;
			  error->error_type = QERR_bad_value_syntax;
			  error->ds_message = copy_string(err_msg);
			  
			  return error;
			}
		    }
		}
	      
	      if (attrVal_seq == NULLAV)
		{
		  if (entrymods) ems_free(entrymods);
		  ems_free(curr_mod);
		  
		  error = error_alloc();
		  error->next = NULLError;
		  
		  error->baseobject = NULLCP;
		  error->error_type = QERR_internal;
		  error->ds_message = NULLCP;
		  
		  return error;
		}
	      else
		curr_mod->em_what = as_comp_new(curr_attr->attr_type,
						attrVal_seq,
						NULLACL_INFO);
	    }
	    
	  /* This attributes arguments set */
	  attrVal_seq = NULLAV;
	  
	  if (curr_mod->em_type != 127)
	    {
	      entrymods = ems_append(entrymods, curr_mod);
	      curr_mod = NULLMOD;
	    }
	  else
	    ems_free(curr_mod);
	}
    }


  if (entrymods == NULLMOD)
    {
      error = error_alloc();
      error->next = NULLError;
      
      error->baseobject = NULLCP;
      error->error_type = QERR_no_mods_supplied;
      error->ds_message = NULLCP;
      
      return error;
    }


  (void) get_default_service(&mod_arg.mea_common);

  mod_arg.mea_common.ca_servicecontrol.svc_options =
    mod_arg.mea_common.ca_servicecontrol.svc_options | SVC_OPT_PREFERCHAIN;

  mod_arg.mea_object = str2dn(baseobject);
  mod_arg.mea_changes = entrymods;


  if (_task_invoked(MODIFY_TASK, baseobject, request_id, &task_id) != QERR_ok)
    {
      error = error_alloc();
      error->next = NULLError;
      
      error->baseobject = NULLCP;
      error->error_type = QERR_internal_limit_reached;
      error->ds_message = NULLCP;
      
      return error;
    }


  *task_id_ptr = task_id;

  if (DapModifyEntry(dsap_ad, task_id, &mod_arg, &di, ROS_ASYNC) == NOTOK)
    {
      error = error_alloc();
      error->next = NULLError;
      
      error->baseobject = NULLCP;
      error->error_type = QERR_internal;
      error->ds_message = NULLCP;
      
      return error;
    }


#ifndef NO_STATS
  LLOG (log_stat, LLOG_NOTICE,
        ("MODIFY +%s, task %d",
         baseobject,
         task_id));
#endif


  return NULLError;
} /* start_modify */

/*
 * - process_modify_ds_result() -
 *
 *
 */
/* ARGSUSED */
request_state process_modify_ds_result(request, task_id, ds_result)
     requestRec request;
     int task_id;
     struct DSResult *ds_result;
{
  modifyRec modrec = request->MODIFY_REC;

  /* This shouldn't happen */
  if (_get_task_of_id(task_id) == NULLDsTask)
    return RQ_processing;

  modrec->result = modify_result_alloc();

  modrec->result->was_successful = TRUE;
  modrec->result->errors = NULLError;

  return RQ_results_returned;
} /* process_modify_ds_result */

/*
 * - process_modify_ds_error() -
 *
 *
 */
request_state process_modify_ds_error(request, task_id, ds_error)
     requestRec request;
     int task_id;
     struct DSError *ds_error;
{
  DsTask task_rec;
  modifyRec modrec = request->MODIFY_REC;
  QE_error_code error_type;

  /* This shouldn't happen */
  if ((task_rec = _get_task_of_id(task_id)) == NULLDsTask)
    return RQ_processing;

  /* Save error and log it */
  error_type = get_log_error_type(ds_error, task_id);
  add_error_to_request_rec(request,
			   task_rec->baseobject,
			   error_type,
			   ds_error);
  
  modrec->result = modify_result_alloc();

  modrec->result->was_successful = FALSE;
  modrec->result->errors = request->errors;
  
  request->errors = NULLError;

  /* Remove invocation record for this task */
  _task_complete(task_id);

  return RQ_results_returned;  
} /* process_modify_ds_error */

/*
 *	Start of procedures for making entry templates.
 *
 *
 */

/*
 * - get_modify_result() -
 *
 *
 */
makeTemplateResult get_make_template_result(request_id)
     QCardinal request_id;
{
  requestRec request = _get_request_of_id(request_id);
  makeTemplateRec temprec;
  makeTemplateResult result;

  if (request == NULLReqRec) return NULLTemplateResult;

  temprec = request->TEMPLATE_REC;
  result = temprec->result;

  temprec->result = NULLTemplateResult;

  _request_complete(request_id);

  return result;
} /* get_make_template_result */

/*
 * - do_make_template() -
 *
 *
 */
QE_error_code do_make_template(baseobject, id_ptr, attr_list)
     char *baseobject;
     QCardinal *id_ptr;
     modifyAttr *attr_list;
{
  makeTemplateRec template_rec;
  requestRec this_request;
  stringCell str_attr_list = NULLStrCell;
  modifyAttr curr_attr;
  QE_error_code error_code;

  (void) _request_invoked(ENTRY_TEMPLATE, id_ptr);
  this_request = _get_request_of_id(*id_ptr);

  template_rec = this_request->TEMPLATE_REC;

  template_rec->base_object = copy_string(baseobject);
  template_rec->request_id = *id_ptr;


/*
 * 	Make sure to read objectClass
 */

  curr_attr = modify_attr_alloc();

  curr_attr->next = *attr_list;

  curr_attr->attr_name = copy_string("2.5.4.0");
  curr_attr->attr_type = AttrT_new("2.5.4.0");

  curr_attr->values = NULLModifyValue;
  curr_attr->to_modify = FALSE;

  template_rec->include_attrs = curr_attr;


/*
 *	Perform the read (reading all attributes)
 */  
  
  error_code = start_read(*id_ptr,
			  baseobject,
			  NULLStrCell,
			  &template_rec->task_id);

  free_string_seq(&str_attr_list);

  return error_code;
} /* do_make_template */


/*
 * - process_template_ds_result() -
 *
 *
 */
request_state process_template_ds_result(request, task_id, ds_result)
     requestRec request;
     int task_id;
     struct DSResult *ds_result;
{
  DsTask task_rec;
  makeTemplateRec templaterec = request->TEMPLATE_REC;
  struct ds_read_result *read_result;
  modifyAttr entry_template = templaterec->include_attrs,
  	     oclass_attr = NULLModifyAttr,
	     curr_attr,
	     last_attr,
  	     must_in = NULLModifyAttr,
             may_in = NULLModifyAttr,
             must_out = NULLModifyAttr,
             may_out = NULLModifyAttr,
             tmp;
  modifyValue curr_oclass;
  objectclass *ocp;
  register table_seq optr;

  /* This shouldn't happen */
  if ((task_rec = _get_task_of_id(task_id)) == NULLDsTask)
    return RQ_processing;


  read_result = &ds_result->res_rd;


/*
 *	Log receipt of results
 */

#ifndef NO_STATS
  LLOG (log_stat,
        LLOG_NOTICE,
        ("READ RESULT +%s, task %d", task_rec->baseobject, task_id));
#endif


/*
 *	Put the results into the entry_template linked list.
 */

  get_template_attrs(read_result->rdr_entry.ent_attr, entry_template);


/*
 *	Get the object class field, and check that all other attributes
 *	conform to object class specifications.
 */

  oclass_attr = entry_template;
  entry_template = entry_template->next;

  oclass_attr->next = NULLModifyAttr;

  last_attr = NULLModifyAttr;


/*
 *	Pick out the must attributes
 */

  for (curr_oclass = oclass_attr->values;
       curr_oclass != NULLModifyValue;
       curr_oclass = curr_oclass->next)
    {
      ocp = name2oc(curr_oclass->value);

      for (optr = ocp->oc_must; optr != NULLTABLE_SEQ; optr = optr->ts_next)
	{
	  for (curr_attr = entry_template, last_attr = NULLModifyAttr;
	       curr_attr != NULLModifyAttr;
	       last_attr = curr_attr, curr_attr = curr_attr->next)
	    if (curr_attr->attr_type == optr->ts_oa)
	      break;

	  if (curr_attr == NULLModifyAttr)
	    /*
	     * 	The attr type in `oc_must' wasn't requested so stick it
	     *  in the `must_out' list.
	     */
	    {
	      if (must_out == NULLModifyAttr)
		{
		  must_out = modify_attr_alloc();
		  must_out->next = NULLModifyAttr;
		}
	      else
		{
		  tmp = must_out;

		  must_out = modify_attr_alloc();
		  must_out->next = tmp;
		}
	      
	      must_out->attr_name =copy_string(attr2name(optr->ts_oa,OIDPART));
	      must_out->attr_type = optr->ts_oa;

	      must_out->values = modify_value_alloc();
	      
	      must_out->values->value = must_out->values->new_value = NULLCP;
	      must_out->values->status = unchanged;
	      must_out->values->next = NULLModifyValue;

	      must_out->to_modify = FALSE;
	    }
	  else
	    /*
	     *	The attr type in `oc_must' was requested so put it in the
	     *	`must_in' list.
	     */
	    {
	      if (last_attr == NULLModifyAttr)
		entry_template = curr_attr->next;
	      else
		last_attr->next = curr_attr->next;
	      
	      curr_attr->next = must_in;
	      must_in = curr_attr;
	    }
	}

      /*
       *	Get the `oc_may' attributes
       */

      for (optr = ocp->oc_may; optr != NULLTABLE_SEQ; optr = optr->ts_next)
	{
	  for (curr_attr = entry_template, last_attr = NULLModifyAttr;
	       curr_attr != NULLModifyAttr;
	       last_attr = curr_attr, curr_attr = curr_attr->next)
	    if (curr_attr->attr_type == optr->ts_oa)
	      break;

	  if (curr_attr == NULLModifyAttr)
	    /*
	     * 	The attr type in `oc_may' wasn't requested so stick it
	     *  in the `may_out' list.
	     *
	     */
	    {
	      
	      if (may_out == NULLModifyAttr)
		{
		  may_out = modify_attr_alloc();
		  may_out->next = NULLModifyAttr;
		}
	      else
		{
		  tmp = may_out;

		  may_out = modify_attr_alloc();
		  may_out->next = tmp;
		}
	      
	      may_out->attr_name = copy_string(attr2name(optr->ts_oa,OIDPART));
	      may_out->attr_type = optr->ts_oa;

	      may_out->values = modify_value_alloc();
	      
	      may_out->values->value = may_out->values->new_value = NULLCP;
	      may_out->values->status = unchanged;
	      may_out->values->next = NULLModifyValue;

	      may_out->to_modify = FALSE;
	    }
	  else
	    /*
	     *	The attr type in `oc_may' was requested so put it in the
	     *	`may_in' list.
	     */
	    {
	      if (last_attr == NULLModifyAttr)
		entry_template = curr_attr->next;
	      else
		last_attr->next = curr_attr->next;
	      
	      curr_attr->next = may_in;
	      may_in = curr_attr;
	    }
	}
    }


  /*
   *	Check for attribute overlaps and copies in the attribute lists
   */

  must_in = remove_double_attrs(must_in);
  may_out = remove_double_attrs(may_out);

  may_in = remove_double_attrs(may_in);
  must_out = remove_double_attrs(must_out);

  must_out = remove_common_attrs(must_in, must_out);
  may_in = remove_common_attrs(must_in, may_in);
  may_out = remove_common_attrs(must_in, may_out);

  must_out = remove_common_attrs(may_in, must_out);
  may_out = remove_common_attrs(may_in, may_out);

  may_out = remove_common_attrs(must_out, may_out);


  /*
   *	Put the results structure into the request record
   *	for later retrieval.
   *
   */

  templaterec->result = template_result_alloc();

  templaterec->result->object_class = oclass_attr;

  templaterec->result->must_request = must_in;

  get_template_attrs(read_result->rdr_entry.ent_attr, must_out);
  templaterec->result->must_exc = must_out;

  templaterec->result->may_request = may_in;

  get_template_attrs(read_result->rdr_entry.ent_attr, may_out);
  templaterec->result->may_exc = may_out;

  templaterec->include_attrs = NULLModifyAttr;
  templaterec->result->errors = templaterec->errors = NULLError;

  templaterec->result->base_object = templaterec->base_object;
  templaterec->base_object = NULLCP;

  return RQ_results_returned;
} /* process_template_ds_result */

/*
 * - process_template_ds_error() -
 *
 *
 */
request_state process_template_ds_error(request, task_id, error)
     requestRec request;
     int task_id;
     struct DSError *error;
{
  DsTask task_rec;
  makeTemplateRec temprec = request->TEMPLATE_REC;
  QE_error_code error_type;

  task_rec = _get_task_of_id(task_id);

  /*
   *	This shouldn't happen
   */

  if (task_rec == NULLDsTask) return RQ_processing;


  /*
   *	Save error and log it
   */
  
  error_type = get_log_error_type(error, task_id);
  add_error_to_request_rec(request, task_rec->baseobject, error_type, error);


  /*
   *	Put the results structure into the request record
   *	for later retrieval.
   *
   */

  temprec->result = template_result_alloc();

  temprec->errors = request->errors;
  request->errors = NULLError;

  temprec->result->base_object = temprec->base_object;
  temprec->base_object = NULLCP;

  temprec->result->must_request = NULLModifyAttr;
  temprec->result->must_exc = NULLModifyAttr;

  temprec->result->may_request = NULLModifyAttr;
  temprec->result->must_exc = NULLModifyAttr;


  /*
   *	Remove invocation record for this task
   */

  _task_complete(task_id);

  return RQ_results_returned;
} /* process_template_ds_error */

/*
 * - get_template_attrs() -
 *
 * Copy values in an `Attr_Sequence' linked list into a `modify_attr'
 * linked list.
 *
 * Uses EDBOUT format for the output style as READOUT isn't allowed
 * for modification operations.
 *
 */
static void get_template_attrs(readattrs, template)
     Attr_Sequence readattrs;
     modifyAttr template;
{
  PS valps;
  modifyValue curr_val = NULLModifyValue;
  modifyAttr this_attr;
  AV_Sequence curr_av;
  Attr_Sequence curr_as;
  extern int oidformat;

  for (curr_as = readattrs; curr_as != NULLATTR; curr_as = curr_as->attr_link)
    {
      for (this_attr = template;
	   this_attr != NULLModifyAttr;
	   this_attr = this_attr->next)
	if (curr_as->attr_type == this_attr->attr_type &&
	    /* Don't want to read file attributes at present! */
	    AS_SYNTAX(curr_as) < AV_WRITE_FILE)
	  /* The attribute corresponds to one in the template. */
	  {
	    curr_val = NULLModifyValue;
	    
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
		
		avs_comp_print(valps, curr_av, EDBOUT);
		
		if (valps->ps_ptr != NULLCP)
		  {
		    *(valps->ps_ptr) = NULL;
		    valps->ps_cnt++;
		  }

		if (curr_val == NULLModifyValue)
		  this_attr->values = curr_val = modify_value_alloc();
		else
		  {
		    curr_val->next = modify_value_alloc();
		    curr_val = curr_val->next;
		  }
		
		if (valps->ps_base == NULLCP)
		  curr_val->value = NULLCP;
		else
		  curr_val->value = copy_string(valps->ps_base);

		curr_val->new_value = NULLCP;
		curr_val->status = unchanged;

		curr_val->next = NULLModifyValue;

		ps_free(valps);
	      }
	  }
    }
} /* get_template_attrs */

/*
 *
 * Proc's to free structures.
 *
 */

void free_mod_attr_list(mod_attr_list_ptr)
     modifyAttr *mod_attr_list_ptr;
{
  modifyAttr curr_attr = *mod_attr_list_ptr, last_attr;

  while (curr_attr != NULLModifyAttr)
    {
      free_mod_val_list(&curr_attr->values);

      if (curr_attr->attr_name != NULLCP)
	(void) free(curr_attr->attr_name);

      last_attr = curr_attr;
      curr_attr = curr_attr->next;

      (void) free((char *) last_attr);
    }

  *mod_attr_list_ptr = NULLModifyAttr;
} /* free_mod_attr_list */


void free_mod_val_list(mod_val_list_ptr)
     modifyValue *mod_val_list_ptr;
{
  modifyValue curr_val = *mod_val_list_ptr, last_val;

  while (curr_val != NULLModifyValue)
    {
      if (curr_val->value != NULLCP)
	(void) free(curr_val->value);

      if (curr_val->new_value != NULLCP)
	(void) free(curr_val->new_value);

      last_val = curr_val;
      curr_val = curr_val->next;

      (void) free((char *) last_val);
    }

  *mod_val_list_ptr = NULLModifyValue;
} /* free_mod_val_list */

void free_modify_result(result_ptr)
     modifyResult *result_ptr;
{
  modifyResult result = *result_ptr;

  if (result == NULLModifyResult)
    return;

  if (result->errors != NULLError)
    error_list_free(&result->errors);

  (void) free((char *) result);
  
  *result_ptr = NULLModifyResult;
} /* free_modify_result */


void modify_rec_free(record)
     modifyRec record;
{
  if (record == NULLModifyRec)
    return;
  
  if (record->base_object != NULLCP)
    (void) free(record->base_object);

  if (record->result != NULLModifyResult)
    free_modify_result(&record->result);
} /* free_modify_rec */


void free_make_template_result(result_ptr)
     makeTemplateResult *result_ptr;
{
  makeTemplateResult result = *result_ptr;

  if (result->base_object != NULLCP)
    (void) free(result->base_object);
  
  if (result->must_request != NULLModifyAttr)
    free_mod_attr_list(&result->must_request);

  if (result->must_exc != NULLModifyAttr)
    free_mod_attr_list(&result->must_exc);

  if (result->may_request != NULLModifyAttr)
    free_mod_attr_list(&result->may_request);

  if (result->may_exc != NULLModifyAttr)
    free_mod_attr_list(&result->may_exc);

  if (result->errors != NULLError)
    error_list_free(&result->errors);

  (void) free((char *) result);

  *result_ptr = NULLTemplateResult;
} /* free_make_template_result */

void free_make_template_rec(record)
     makeTemplateRec record;
{
  if (record == NULLTemplateRec) return;

  if (record->base_object != NULLCP)
    (void) free(record->base_object);

  if (record->include_attrs != NULLModifyAttr)
    free_mod_attr_list(&record->include_attrs);

  if (record->errors != NULLError)
    error_list_free(&record->errors);

  (void) free((char *) record);
} /* free_make_template_rec */


/*
 * Miscellaneous modify procedures.
 *
 *
 */

static struct entrymod * ems_append (a,b)
     struct entrymod *a;
     struct entrymod *b;
{
  struct entrymod *ptr;

  if ((ptr = a) == NULLMOD)
    return b;
  
  for ( ; ptr->em_next != NULLMOD; ptr = ptr->em_next)
    ;

  ptr->em_next = b;

  return a;
}

modifyAttr avList2modifyAttr(attr_list)
     attrValList attr_list;
{
  modifyAttr curr_mod_attr, head = NULLModifyAttr;
  modifyValue curr_mod_val;
  stringCell curr_val;
  attrValList curr_attr;

  for (curr_attr = attr_list;
       curr_attr != NULLAVList;
       curr_attr = curr_attr->next)
    {
      if (head == NULLModifyAttr)
	{
	  head = curr_mod_attr = modify_attr_alloc();
	}
      else
	{
	  curr_mod_attr->next = modify_attr_alloc();
	  curr_mod_attr = curr_mod_attr->next;
	}

      curr_mod_attr->next = NULLModifyAttr;

      curr_mod_attr->attr_name = copy_string(curr_attr->attr_name->string);
      curr_mod_attr->attr_type = AttrT_new(curr_attr->attr_name->string);

      curr_mod_attr->values = NULLModifyValue;
      curr_mod_attr->to_modify = FALSE;

      for (curr_val = curr_attr->val_list, curr_mod_val = NULLModifyValue;
	   curr_val != NULLStrCell;
	   curr_val = curr_val->next)
	{
	  if (curr_mod_attr->values == NULLModifyValue)
	    {
	      curr_mod_attr->values = curr_mod_val = modify_value_alloc();
	    }
	  else
	    {
	      curr_mod_val->next = modify_value_alloc();
	      curr_mod_val = curr_mod_val->next;
	    }

	  curr_mod_val->next = NULLModifyValue;
	  
	  curr_mod_val->new_value = NULLCP;
	  curr_mod_val->status = unchanged;

	  curr_mod_val->value = copy_string(curr_val->string);
	}
    }

  return head;
} /* avList2modifyAttr */


/*
 * - remove_common_attrs() -
 *
 *
 */
static modifyAttr remove_common_attrs(primary, secondary)
     modifyAttr primary, secondary;
{
  register modifyAttr curr_sec, last_sec;
  modifyAttr tmp;

  if (primary == NULLModifyAttr || secondary == NULLModifyAttr)
    return secondary;

  while (primary != NULLModifyAttr)
    {
      curr_sec = secondary;
      last_sec = NULLModifyAttr;

      while (curr_sec != NULLModifyAttr)
	{
	  if (primary->attr_type == curr_sec->attr_type)
	    {
	      tmp = curr_sec;

	      if (last_sec != NULLModifyAttr)
		{
		  last_sec->next = curr_sec->next;
		  curr_sec = curr_sec->next;
		}
	      else
		curr_sec = secondary = curr_sec->next;

	      tmp->next = NULLModifyAttr;
	      free_mod_attr_list(&tmp);
	    }
	  else
	    {
	      last_sec = curr_sec;
	      curr_sec = curr_sec->next;
	    }
	}

      primary = primary->next;
    }

  return secondary;
}

/*
 * - remove_double_attrs() -
 *
 *
 */
static modifyAttr remove_double_attrs(primary)
     modifyAttr primary;
{
  register modifyAttr curr_attr, last_attr;
  modifyAttr curr_primary, tmp;

  if (primary == NULLModifyAttr)
    return primary;

  for (curr_primary = primary;
       curr_primary != NULLModifyAttr;
       curr_primary = curr_primary->next)
    {
      curr_attr = curr_primary->next;
      last_attr = NULLModifyAttr;

      while (curr_attr != NULLModifyAttr)
	if (curr_primary->attr_type == curr_attr->attr_type)
	  {
	    tmp = curr_attr;
	    
	    if (last_attr != NULLModifyAttr)
		curr_attr = last_attr->next = curr_attr->next;
	    else
	      {
		curr_attr = curr_primary = curr_attr->next;
		
		if (curr_attr != NULLModifyAttr)
		  curr_attr = curr_attr->next;
		
		if (primary->next == curr_primary)
		  primary = curr_primary;
	      }
	    
	    tmp->next = NULLModifyAttr;
	    free_mod_attr_list(&tmp);
	  }
	else
	  {
	    last_attr = curr_attr;
	    curr_attr = curr_attr->next;
	  }
    }

  return primary;
}
