#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ufsearch.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ufsearch.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*****************************************************************************
  
  ufsearch.c -

*****************************************************************************/

#include "types.h"
#include "util.h"
#include "ufsearch.h"
#include "ditmodel.h"
#include "ds_tasks.h"
#include "request.h"
#include "ds_util.h"
#include "read.h"

#include "quipu/ds_search.h"
#include "quipu/dap2.h"
#include "quipu/name.h"

extern int dsap_ad;
extern int next_task_id;

extern ufsPath ufs_paths;

#ifndef NO_STATS
extern LLog    *log_stat;
#endif

static QE_error_code process_ufsearch();
static QE_error_code do_directory_search();
static QE_error_code follow_up();

char *TidyString();

/*
 * - do_ufsearch() -
 * Start a user friendly search. The 'search_data' list is taken (not copied)
 * by the proc.
 *
 */
QE_error_code do_ufsearch(target_data, search_data, baselist, id_ptr)
     searchPair target_data;
     searchPair search_data;
     entryList baselist;
     QCardinal *id_ptr;
{
  requestRec request;
  ufsearchRec ufsrec;
  searchPair curr_srch_data;
  QBool have_co = FALSE,
        have_org = FALSE,
        have_loc = FALSE;
  AttributeType co_type, org_type, ou_type, loc_type, curr_type, per_type;

  /* Record request invocation */
  (void) _request_invoked(UFSEARCH, id_ptr);

  request = _get_request_of_id(*id_ptr);
  ufsrec = request->UFSEARCH_REC;

  ufsrec->target_data = target_data;
  ufsrec->search_data = search_data;

  /* ### Ignore given base path for the moment ### */

  co_type = AttrT_new("2.5.6.2");
  org_type = AttrT_new("2.5.6.4");
  ou_type = AttrT_new("2.5.6.5");
  loc_type = AttrT_new("2.5.6.3");
  per_type = AttrT_new("2.5.6.6");

  /* Check for country attribute */
  for (curr_srch_data = search_data;
       curr_srch_data != NULLSearchPair;
       curr_srch_data = curr_srch_data->next)
    {
      curr_type = curr_srch_data->type;
      
      if (curr_type == co_type)
	{
	  have_co = TRUE;
	  continue;
	}

      if (curr_type == loc_type)
	{
	  have_loc = TRUE;
	  continue;
	}
  
      if (curr_type == org_type)
	{
	  have_org = TRUE;
	  continue;
	}

      if (curr_type == ou_type)
	continue;
    }

  if (target_data->type == per_type || target_data->type == ou_type)
    {
      if (have_co == FALSE)
	{
	  if (have_org == FALSE && have_loc == FALSE)
	    {
	      dn_list_copy(get_default_path(org_type), &ufsrec->path);
	    }
	  else
	    {
	      dn_list_copy(get_default_path(co_type), &ufsrec->path);
	    }
	  
	  if (ufsrec->path != NULLEntryList)
	    {
	      ufsrec->to_follow = ufsrec->path;
	      ufsrec->path = ufsrec->path->next;
	      
	      ufsrec->to_follow->next = NULLEntryList;
	    }
	  else
	    ufsrec->to_follow = baselist;
	}
      else
	{
	  if (have_org == FALSE && have_loc == FALSE)
	    return QERR_request_failed;
	  
	  ufsrec->to_follow = baselist;
	}
    }
  else if (target_data->type == org_type)
    {
      if (have_co == FALSE)
	dn_list_copy(get_default_path(co_type), &ufsrec->path);
      
      if (ufsrec->path != NULLEntryList)
	{
	  ufsrec->to_follow = ufsrec->path;
	  ufsrec->path = ufsrec->path->next;
	  
	  ufsrec->to_follow->next = NULLEntryList;
	}
      else
	ufsrec->to_follow = baselist;
    }

  return process_ufsearch(ufsrec);
} /* do_ufsearch */

/*
 * - process_ufsearch() -
 *
 *
 */
static QE_error_code process_ufsearch(ufsrec)
     ufsearchRec ufsrec;
{
  entryList to_follow;
  register objectTypeList curr_type, child_types;
  QCardinal succeeded = 0;

  /* If ds searches outstanding, then do nothing. */
  if (ufsrec->exact_task_count > 0 || ufsrec->approx_task_count > 0)
    return QERR_ok;

  if (ufsrec->trying_target_search == TRUE)
    {
      ufsrec->tried_target_search = TRUE;
      ufsrec->trying_target_search = FALSE;
      
      ufsrec->to_follow = ufsrec->followed;
      ufsrec->followed = NULLEntryList;
    }

  /* Check if attempt to search for target is valid */
  if (ufsrec->tried_target_search == FALSE)
    {
      for (to_follow = ufsrec->to_follow;
	   to_follow != NULLEntryList;
	   to_follow = to_follow->next)
	{
	  child_types = get_child_list(to_follow->object_class);

	  for (curr_type = child_types;
	       curr_type != NULLObjectType;
	       curr_type = curr_type->next)
	    if (qy_in_hierarchy
		(name2oc(ufsrec->target_data->type->oa_ot.ot_stroid),
		 name2oc(curr_type->object_type->oa_ot.ot_stroid)))
	      {
		ufsrec->trying_target_search = TRUE;
		if (follow_up(to_follow, ufsrec) == QERR_ok) succeeded++;
		
		break;
	      }
	}
      
      if (ufsrec->trying_target_search == TRUE)
	{
	  if (succeeded == 0)
	    return QERR_bad_value_syntax;
	  else
	    {
	      ufsrec->followed = ufsrec->to_follow;
	      ufsrec->to_follow = NULLEntryList;
	    }
	  
	  return QERR_ok;
	}
    }
  else
    ufsrec->tried_target_search = FALSE;

  /* Doing an intermediate search */
  /* For each that needs to be followed up */
  for (to_follow = ufsrec->to_follow;
       to_follow != NULLEntryList;
       to_follow = to_follow->next)
    if (follow_up(to_follow, ufsrec) == QERR_ok) succeeded++;

  /* If trying a search for the target then don't remove list in case
     search fails to hit and further intermediate searches are required */
  dn_list_free(&ufsrec->to_follow);

  if (succeeded == 0) 
    return QERR_bad_value_syntax;

  ufsrec->followed = ufsrec->to_follow = NULLEntryList;
  return QERR_ok;
} /* process_ufsearch */

/*
 * - follow_up() -
 *
 *
 */
static QE_error_code follow_up(baseinfo, ufsrec)
     entryList baseinfo;
     ufsearchRec ufsrec;
{
  AttributeType base_type;
  objectTypeList search_types, child_types, curr_type;
  searchPair search_vals = NULLSearchPair,
                 curr_val = NULLSearchPair,
                 search_on = NULLSearchPair;
  stringCell curr_str_val;
  QCardinal search_val_count = 0;
  QE_error_code return_status;
  char *objectname;

  objectname = baseinfo->string_dn;

  for (search_vals = ufsrec->search_data;
       search_vals != NULLSearchPair;
       search_vals = search_vals->next)
    {
      search_vals->checked = FALSE;
      search_val_count++;
    }

  if (ufsrec->trying_target_search == TRUE)
    { /* Searching for target */
      for (search_types = get_search_attrs(ufsrec->target_data->type);
	   search_types != NULLObjectType;
	   search_types = search_types->next)
	{
	  search_on = search_pair_alloc();
	  search_on->type = search_types->object_type;

	  search_on->values = NULLStrCell;
	  search_on->next = NULLSearchPair;
	  
	  for (curr_str_val = ufsrec->target_data->values;
	       curr_str_val != NULLStrCell;
	       curr_str_val = curr_str_val->next)
	    add_string_to_seq(curr_str_val->string, &search_on->values);
	}

      return_status = do_directory_search(objectname, search_on, ufsrec);
    }
  else /* Searching for intermediate entry */
    {
      base_type = baseinfo->object_class;
      child_types = get_child_list(base_type);
      
      for (curr_type = child_types;
	   curr_type != NULLObjectType && search_val_count > 0;
	   curr_type = curr_type->next)
	{
	  for (search_vals = ufsrec->search_data;
	       search_vals != NULLSearchPair && search_val_count > 0;
	       search_vals = search_vals->next)
	    if (search_vals->checked == FALSE &&
		search_vals->type == curr_type->object_type)
	      {
		search_vals->checked = TRUE;
		search_val_count--;
	      }
	}
      
      for (search_vals = ufsrec->search_data, search_on = NULLSearchPair;
	   search_vals != NULLSearchPair;
	   search_vals = search_vals->next)
	if (search_vals->checked == TRUE)
	  {
	    for (search_types = get_search_attrs(search_vals->type);
		 search_types != NULLObjectType;
		 search_types = search_types->next)
	      {
		curr_val = search_pair_alloc();
		curr_val->next = search_on;
		search_on = curr_val;
		
		search_on->type = search_types->object_type;
		search_on->values = NULLStrCell;
		
		for (curr_str_val = search_vals->values;
		     curr_str_val != NULLStrCell;
		     curr_str_val = curr_str_val->next)
		  add_string_to_seq(curr_str_val->string, &curr_val->values);
		
		search_on->use_subtree = TRUE;
	      }
	  }
      
      if (search_on != NULLSearchPair)
	return_status = do_directory_search(objectname, search_on, ufsrec);
      else 
	return_status = QERR_request_failed;
    }
  
  while (search_on != NULLSearchPair)
    {
      curr_val = search_on->next;
      free_string_seq(&search_on->values);
      free((char *) search_on);
      search_on = curr_val;
    }

  return return_status;
} /* follow_up */

/*
 * - do_directory_search() -
 *
 *
 */
static QE_error_code do_directory_search(baseobject, search_vals, ufsrec)
     char *baseobject;
     searchPair search_vals;
     ufsearchRec ufsrec;
{
  struct ds_search_arg search_arg;
  struct DAPindication di;
  DN base_dn;
  char *start, *end;
  QCardinal level_count;
  Filter exact_filter, *exfilt_ptr, tmp_ex_filt,
         approx_filter, *apfilt_ptr, tmp_ap_filt;
  Attr_Sequence get_attrs = NULLATTR;
  searchPair curr_val_list;
  stringCell curr_val;
  int task_id, count;
  char valbuf[1024];

  valbuf[0] = '\0';

  /* Make a DN struct from given string object name.
     If not valid then return an error. */
  if (baseobject == NULLCP || isnull(*baseobject)) base_dn = NULLDN;
  else if ((base_dn = str2dn(baseobject)) == NULLDN) return QERR_bad_name;

  /* `Analyse' the string dn. */
  start = baseobject;
  level_count = 0;

  if (base_dn != NULLDN)
    while ((end = index(start, '=')) != NULLCP)
      {
	level_count++;
	start = ++end;
      }

  (void) get_default_service(&search_arg.sra_common);

  search_arg.sra_common.ca_servicecontrol.svc_options =
    search_arg.sra_common.ca_servicecontrol.svc_options | SVC_OPT_PREFERCHAIN;

  search_arg.sra_common.ca_servicecontrol.svc_timelimit = SVC_NOTIMELIMIT;
  search_arg.sra_common.ca_servicecontrol.svc_sizelimit = 10;
  search_arg.sra_baseobject = base_dn;

  /* Don't want any attributes back. */
  search_arg.sra_eis.eis_allattributes = FALSE;
  /* Approx search will ask for some attributes back */
  search_arg.sra_eis.eis_infotypes = EIS_ATTRIBUTESANDVALUES;

  search_arg.sra_searchaliases = TRUE;

  /* Want objectClasss back */
  get_attrs = as_comp_new(AttrT_new("2.5.4.0"), NULLAV, NULLACL_INFO);

  exact_filter = filter_alloc();
  approx_filter = filter_alloc();

  if (search_vals->next != NULLSearchPair ||
      search_vals->values->next != NULLStrCell)
    {
      approx_filter->flt_next = exact_filter->flt_next = NULLFILTER;
      exact_filter->flt_type = approx_filter->flt_type = FILTER_OR;
      
      for (curr_val_list = search_vals,
	   apfilt_ptr = &approx_filter->FUFILT,
	   exfilt_ptr = &exact_filter->FUFILT;
	   curr_val_list != NULLSearchPair;
	   curr_val_list = curr_val_list->next)
	{
	  if (curr_val_list != search_vals)
	    {
	      tmp_ap_filt = *apfilt_ptr;
	      apfilt_ptr = &tmp_ap_filt->flt_next;
	      
	      tmp_ex_filt = *exfilt_ptr;
	      exfilt_ptr = &tmp_ex_filt->flt_next;
	    }
	  
	  for (curr_val = curr_val_list->values;
	       curr_val != NULLStrCell;
	       curr_val = curr_val->next)
	    {
	      if (curr_val != curr_val_list->values)
		{
		  tmp_ap_filt = *apfilt_ptr;
		  apfilt_ptr = &tmp_ap_filt->flt_next;
		  
		  tmp_ex_filt = *exfilt_ptr;
		  exfilt_ptr = &tmp_ex_filt->flt_next;
		}
	      
	      if (make_filter_items(curr_val_list->type,
				    curr_val->string,
				    exfilt_ptr,
				    apfilt_ptr) != QERR_ok)
		{
		  filter_free(exact_filter);
		  filter_free(approx_filter);
		  
		  if (get_attrs != NULLATTR)
		    as_free(get_attrs);

		  return QERR_bad_value_syntax;
		}
	      
	      (void) strcat(valbuf, ", ");
	      (void) strcat(valbuf, curr_val->string);
	    }
	  
	  get_attrs = as_merge(get_attrs,
			       as_comp_new(curr_val_list->type,
					   NULLAV,
					   NULLACL_INFO));
	}
    }
  else
    {
      if (make_filter_items(search_vals->type,
			    search_vals->values->string,
			    &exact_filter,
			    &approx_filter) != QERR_ok)
	{
	  filter_free(exact_filter);
	  filter_free(approx_filter);
	  
	  if (get_attrs != NULLATTR) as_free(get_attrs);
	  return QERR_bad_value_syntax;
	}
      
      (void) strcat(valbuf, ", ");
      (void) strcat(valbuf, search_vals->values->string);
      
      get_attrs = as_merge(get_attrs,
			   as_comp_new(search_vals->type,
				       NULLAV,
				       NULLACL_INFO));
    }
  
  if (ufsrec->trying_target_search == TRUE && level_count >= 2)
    search_arg.sra_subset = SRA_WHOLESUBTREE;
  else
    search_arg.sra_subset = SRA_ONELEVEL;
    
  /* First invoke exact match, then invoke approx match. */
  /* Record task invocation. */
  /* Record task invocation for this particular request. */
  for (count = 0;
       ufsrec->exact_task_ids[count] != NO_TASK && count < MaxUfsearchTasks;
       count++)
    ;

  if (count == MaxUfsearchTasks)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);

      if (get_attrs != NULLATTR)
	as_free(get_attrs);

      return QERR_internal_limit_reached;
    }
  
  if (_task_invoked(SEARCH_TASK, baseobject, ufsrec->request_id, &task_id)
      != QERR_ok)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      if (get_attrs != NULLATTR)
	as_free(get_attrs);

      return QERR_internal_limit_reached;
    }
  
  /* Invoke search using exact match. */
  search_arg.sra_filter = exact_filter;
  search_arg.sra_eis.eis_select = get_attrs;

  if (DapSearch(dsap_ad, task_id, &search_arg, &di, ROS_ASYNC) == NOTOK)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      _task_complete(task_id);

      if (get_attrs != NULLATTR)
	as_free(get_attrs);

      return QERR_request_failed;
    }
  else
    {
      ufsrec->exact_task_count++;
      ufsrec->exact_task_ids[count] = task_id;
      ufsrec->tasks_sent++;
    }
  
#ifndef NO_STATS
  LLOG (log_stat, LLOG_NOTICE,
        ("EXACT MATCH +%s, task %d, extent %d%s",
         baseobject, task_id, search_arg.sra_subset, valbuf));
#endif

  if (count == MaxUfsearchTasks)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      if (get_attrs != NULLATTR)
	as_free(get_attrs);

      return QERR_internal_limit_reached;
    }
  
  /* Invoke approx match. */
  /* Record task invocation. */
  /* Record task invocation for this particular request. */
  for (count = 0;
       ufsrec->approx_task_ids[count] != NO_TASK && count < MaxUfsearchTasks;
       count++)
    ;

  if (count == MaxUfsearchTasks)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      if (get_attrs != NULLATTR)
	as_free(get_attrs);

      return QERR_internal_limit_reached;
    }
  
  if (_task_invoked(SEARCH_TASK, baseobject, ufsrec->request_id, &task_id)
      != QERR_ok)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      if (get_attrs != NULLATTR)
	as_free(get_attrs);

      return QERR_internal_limit_reached;
    }
  
  search_arg.sra_filter = approx_filter;

  if (DapSearch(dsap_ad, task_id, &search_arg, &di, ROS_ASYNC) == NOTOK)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      _task_complete(task_id);
      if (get_attrs != NULLATTR) as_free(get_attrs);
      return QERR_request_failed;
    }
  else
    {
      ufsrec->approx_task_count++;
      ufsrec->approx_task_ids[count] = task_id;
      ufsrec->tasks_sent++;
    }

#ifndef NO_STATS
  LLOG (log_stat, LLOG_NOTICE,
        ("APPROX MATCH +%s, task %d, extent %d, vals%s",
         baseobject, task_id, search_arg.sra_subset, valbuf));
#endif  
	
  if (get_attrs != NULLATTR)
    as_free(get_attrs);

  filter_free(exact_filter);
  filter_free(approx_filter);

  return QERR_ok;
} /* do_directory_search */

/*
 * - process_ufs_ds_result() -
 *
 *
 */
request_state process_ufs_ds_result(request, task_id, ds_result)
     requestRec request;
     int task_id;
     struct DSResult *ds_result;
{
  DsTask task_rec;
  ufsearchRec ufsrec = request->UFSEARCH_REC;
  int *ufs_task_array;
  QBool is_exact_match_result;
  QCardinal aindex, hit_count;
  struct ds_search_result *search_result;

  task_rec = _get_task_of_id(task_id);

  /* This shouldn't happen */
  if (task_rec == NULLDsTask) return RQ_processing;

  /* Check if result comes from a search using exact or approx match. */
  for (aindex = 0, ufs_task_array = ufsrec->exact_task_ids;
       ufs_task_array[aindex] != task_id && aindex < MaxUfsearchTasks;
       aindex++)
    ;

  /* Not an exact search, so must have been an approx search. */
  if (aindex >= MAX_TASKS_PER_REQ)
    {
      for (aindex = 0, ufs_task_array = ufsrec->approx_task_ids;
	   ufs_task_array[aindex] != task_id && aindex < MaxUfsearchTasks;
	   aindex++)
	;
      
      if (aindex < MaxUfsearchTasks)
	is_exact_match_result = FALSE;
    }
  else
    is_exact_match_result = TRUE;

  /* If not an approx search, then something's wrong! */
  if (aindex >= MaxUfsearchTasks)
    {
      _task_complete(task_id);
      return RQ_processing;
    }
  
  /* Now get the search results. */
  search_result = &ds_result->res_sr;
  correlate_search_results(search_result);

  search_result->srr_next = NULLSRR;

  hit_count = 0;
  if (search_result->CSR_entries != NULLENTRYINFO)
    {
      EntryInfo *entry_ptr;
      register stringCell curr_attr;
      attrValList got_attrs = NULLAVList, curr_av;
      searchPair search_vals, curr_val_list;
      stringCell curr_val;
      char *curr_name;
      void get_read_attrs();
      QBool good_match;

      search_vals = ufsrec->trying_target_search == TRUE?
	ufsrec->target_data:
	ufsrec->search_data;
      
      /* Decode and add found entry names */
      for (entry_ptr = search_result->CSR_entries;
	   entry_ptr != NULLENTRYINFO;
	   entry_ptr = entry_ptr->ent_next)
	{
	  curr_name = qy_dn2str(entry_ptr->ent_dn);
	  good_match = FALSE;
	  
	  get_read_attrs(entry_ptr->ent_attr, &got_attrs, READOUT);
	  
	  if (!is_exact_match_result)
	    {
	      if (got_attrs != NULLAVList)
		for (curr_av = got_attrs, good_match = FALSE;
		     curr_av != NULLAVList && good_match == FALSE;
		     curr_av = curr_av->next)
		  for (curr_attr = curr_av->val_list;
		       curr_attr != NULLStrCell && good_match == FALSE;
		       curr_attr = curr_attr->next)
		    for (curr_val_list = search_vals;
			 curr_val_list != NULLSearchPair &&
			 good_match == FALSE;
			 curr_val_list = curr_val_list->next)
		      for (curr_val = curr_val_list->values;
			   curr_val != NULLStrCell;
			   curr_val = curr_val->next)
			if (is_good_match(curr_val->string, curr_attr->string))
			  good_match = TRUE;
	    }
	  else
	    good_match = TRUE;
      
	  /* It is a good_match. If it's objectClass can be found in the
	     `ditmodel' or if it is a match against target data kep it */
	  if (good_match == TRUE)
	    {
	      AttributeType val_type, oclass_type, target_type;
	      objectclass *target_class, *val_class;
	      ditRelation dit_objects;
	      extern ditRelation ditmodel;
	      objectclass *name2oc();
	      
	      oclass_type = AttrT_new("2.5.4.0");
	      
	      for (curr_av = got_attrs;
		   curr_av != NULLAVList &&
		   AttrT_new(curr_av->attr_name->string) != oclass_type;
		   curr_av = curr_av->next)
		;
	      
	      if (ufsrec->trying_target_search == FALSE)
		{
		  QBool is_dit_type = FALSE;
		  
		  if (AttrT_new(curr_av->attr_name->string) == oclass_type)
		    {
		      char *start, *end, save;
		      
		      for (curr_val = curr_av->val_list;
			   curr_val != NULLStrCell && is_dit_type == FALSE;
			   curr_val = curr_val->next)
			{
			  end = curr_val->string;
			  
			  while (*end != '\0' &&
				 *end != '\n' &&
				 is_dit_type == FALSE)
			    {
			      start = end;
			      
			      while (isspace(*start))
				start++;

			      end = start;
			      
			      while (!isspace(*end))
				end++;
			      save = *end;
			      
			      *end = '\0';
			      
			      val_class = name2oc(start);
			      val_type = AttrT_new(val_class->oc_ot.ot_stroid);

			      for (dit_objects = ditmodel;
				   dit_objects != NULLDitRelation &&
				   is_dit_type == FALSE;
				   dit_objects = dit_objects->next)
				if (val_type == dit_objects->parent_type ||
				    (dit_objects->parent_type != NULLAttrT &&
				     qy_in_hierarchy
				     (name2oc(dit_objects->parent_type->oa_ot.
					      ot_stroid),
				      val_class)))
				  {
				    /* In case it's a subclass of
				     * the dit type
				     */
				    val_type = dit_objects->parent_type;
				    
				    is_dit_type = TRUE;
				  }
			      
			      *end = save;
			      while (is_dit_type == FALSE &&
				     *end != '\0' &&
				     *end != '\n' &&
				     !isalnum(*end))
				end++;
			    }
			}
		    }
		  
		  if (is_dit_type == TRUE)
		    (void) dn_list_add(curr_name,
				       &ufsrec->to_follow, val_type);
		}
	      else
		{
		  QBool is_target_type = FALSE;
		  target_type = ufsrec->target_data->type;
		  target_class = name2oc(target_type->oa_ot.ot_name);
		  
		  if (AttrT_new(curr_av->attr_name->string) == oclass_type)
		    {
		      char *start, *end, save;
		      
		      for (curr_val = curr_av->val_list;
			   curr_val != NULLStrCell && is_target_type == FALSE;
			   curr_val = curr_val->next)
			{
			  end = curr_val->string;
			  
			  while (*end != '\0' &&
				 *end != '\n' &&
				 is_target_type == FALSE)
			    {
			      start = end;
			      
			      while (isspace(*start))
				start++;

			      end = start;
			      
			      while (!isspace(*end))
				end++;

			      save = *end;
			      
			      *end = '\0';

			      val_class = name2oc(start);

			      if (val_class == target_class ||
				  qy_in_hierarchy(target_class, val_class))
				is_target_type = TRUE;
			      
			      *end = save;
			      
			      while (is_target_type == FALSE &&
				     *end != '\0' &&
				     *end != '\n' &&
				     !isalnum(*end))
				end++;
			    }
			}
		    }
		  
		  if (is_target_type)
		    (void) dn_list_add(curr_name,
				       &ufsrec->to_follow, NULLAttrT);
		}
	    }
	  
	  if (got_attrs != NULLAVList)
	    {
	      free_string_seq(&(got_attrs->attr_name));
	      free_string_seq(&(got_attrs->val_list));

	      (void) free((char *) got_attrs);
	    }
	  
	  got_attrs = NULLAVList;
	  hit_count++;

	  if (curr_name != NULLCP)
	    (void) free(curr_name);
	}
    }
  
  if (search_result->CSR_limitproblem != LSR_NOLIMITPROBLEM)
    {
      QE_error_code limitproblem;
      
      switch (search_result->CSR_limitproblem)
	{
	case LSR_TIMELIMITEXCEEDED:
	  limitproblem = QERR_time_limit_reached;
	  break;
	  
	case LSR_SIZELIMITEXCEEDED:
	  limitproblem = QERR_size_limit_reached;
	  break;
	  
	case LSR_ADMINSIZEEXCEEDED:
	  limitproblem = QERR_admin_limit_reached;
	  break;
	}
      
      add_error_to_request_rec(request,
			       task_rec->baseobject,
			       limitproblem,
			       (struct DSError *) NULL);
    }
  
#ifndef NO_STATS
  LLOG (log_stat, LLOG_NOTICE,
	("SEARCH RESULT from +%s, task %d, hits %d",
	 task_rec->baseobject,
	 task_id,
	 hit_count));
#endif

  /* Remove record of this task. */
  ufs_task_array[aindex] = NO_TASK;

  if (is_exact_match_result == TRUE)
    ufsrec->exact_task_count--;
  else
    ufsrec->approx_task_count--;
  
  _task_complete(task_id);
  
  /* Check the status of this request. */
  /* First check if this stage of matching has been completed */
  if (ufsrec->exact_task_count == 0 && ufsrec->approx_task_count == 0)
    {
      ufsearchResult result;
      
      /* If have made good matches for target */
      if (ufsrec->trying_target_search)
	{
	  result = ufsearch_res_alloc();
	  
	  if (ufsrec->to_follow != NULLEntryList)
	    {
	      result->tasks_sent = ufsrec->tasks_sent;
	      result->tasks_failed = ufsrec->tasks_failed;
	      
	      result->hits = ufsrec->to_follow;
	      ufsrec->to_follow = NULLEntryList;
	      result->hit_num = 0;
	      
	      result->errors = request->errors;
	      request->errors = NULLError;
	      
	      result->search_status = UFS_Succeeded;
	      
	      ufsrec->result = result;
	      
	      return RQ_results_returned;
	    }
	  else /* Failed to find anything */
	    {
	      if (process_ufsearch(ufsrec) == QERR_ok)
		return RQ_processing;
	      else
		{
		  if (ufsrec->path != NULLEntryList)
		    {
		      ufsrec->to_follow = ufsrec->path;
		      ufsrec->path = ufsrec->path->next;
	      
		      ufsrec->to_follow->next = NULLEntryList;
		      
		      if (process_ufsearch(ufsrec) == QERR_ok)
			return RQ_processing;
		    }
		  
		  result->tasks_sent = ufsrec->tasks_sent;
		  result->tasks_failed = ufsrec->tasks_failed;
		  
		  result->hits = NULLEntryList;
		  result->hit_num = 0;
		  
		  result->errors = request->errors;
		  request->errors = NULLError;
		  
		  result->search_status = UFS_Failed;
		  
		  ufsrec->result = result;
		  
		  return RQ_results_returned;
		}
	    }
	}
      else /* Results from an intermediate search */
	{
	  if (ufsrec->to_follow == NULLEntryList)
	    {
	      if (ufsrec->path != NULLEntryList)
		{
		  ufsrec->to_follow = ufsrec->path;
		  ufsrec->path = ufsrec->path->next;
		  
		  ufsrec->to_follow->next = NULLEntryList;
		  
		  if (process_ufsearch(ufsrec) == QERR_ok)
		    return RQ_processing;
		}
	      
	      result = ufsearch_res_alloc();
	      
	      result->tasks_sent = ufsrec->tasks_sent;
	      result->tasks_failed = ufsrec->tasks_failed;
	      
	      result->hits = NULLEntryList;
	      result->hit_num = 0;
	      
	      result->errors = request->errors;
	      request->errors = NULLError;
	      
	      result->search_status = UFS_Failed;
	      
	      ufsrec->result = result;
	      
	      return RQ_results_returned;
	    }
	  
	  if (process_ufsearch(ufsrec) != QERR_ok)
	    return RQ_results_returned;
	  else
	    return RQ_processing;
	}
    }
  
  return RQ_processing;
} /* process_ufs_ds_result */

/*
 * - process_ufs_ds_error() -
 *
 *
 */
request_state process_ufs_ds_error(request, task_id, error)
 requestRec request;
     int task_id;
     struct DSError *error;
{
  DsTask task_rec;
  ufsearchRec ufsrec = request->UFSEARCH_REC;
  int *ufs_task_array;
  QBool is_exact_match_error;
  QCardinal aindex;
  QE_error_code error_type;

  task_rec = _get_task_of_id(task_id);

  /*
   *	This shouldn't happen
   */
  
  if (task_rec == NULLDsTask) return RQ_processing;


  /*
   *	Check if result comes from a search using exact or approx match.
   */

  for (aindex = 0, ufs_task_array = ufsrec->exact_task_ids;
       ufs_task_array[aindex] != task_id && aindex < MaxUfsearchTasks;
       aindex++)
    ;


  /*
   *	Not an exact search, so must have been an approx search.
   */

  if (aindex >= MAX_TASKS_PER_REQ)
    {
      for (aindex = 0, ufs_task_array = ufsrec->approx_task_ids;
	   ufs_task_array[aindex] != task_id && aindex < MaxUfsearchTasks;
	   aindex++)
	;
      
      if (aindex < MaxUfsearchTasks) is_exact_match_error = FALSE;
    }
  else
    is_exact_match_error = TRUE;


  /*
   *	  If not an approx search, then something's wrong!
   */

  if (aindex >= MaxUfsearchTasks)
    {
      _task_complete(task_id);
      return RQ_processing;
    }


  /*
   *	Save error and log it
   */

  error_type = get_log_error_type(error, task_id);
  add_error_to_request_rec(request, task_rec->baseobject, error_type, error);

  ufsrec->tasks_failed++;


  /*
   *  Remove invocation record for this task
   */

  _task_complete(task_id);
  ufs_task_array[aindex] = NO_TASK;

  if (is_exact_match_error)
    ufsrec->exact_task_count--;
  else
    ufsrec->approx_task_count--;


  /*
   *	Check if this stage of matching has been completed
   */

  if (ufsrec->exact_task_count == 0 && ufsrec->approx_task_count == 0)
    {
      ufsearchResult result;
      
      /*
       *	If have made good matches for target
       */
      if (ufsrec->trying_target_search)
	{
	  result = ufsearch_res_alloc();
	  
	  if (ufsrec->to_follow != NULLEntryList)
	    {
	      result->tasks_sent = ufsrec->tasks_sent;
	      result->tasks_failed = ufsrec->tasks_failed;
	      
	      result->hits = ufsrec->to_follow;
	      ufsrec->to_follow = NULLEntryList;

	      result->hit_num = 0;
	      
	      result->errors = request->errors;
	      request->errors = NULLError;
	      
	      result->search_status = UFS_Succeeded;
	      
	      ufsrec->result = result;
	      
	      return RQ_results_returned;
	    }
	  else /* Failed to find anything */
	    {
	      if (process_ufsearch(ufsrec) == QERR_ok)
		return RQ_processing;
	      else
		{
		  if (ufsrec->path != NULLEntryList)
		    {
		      ufsrec->to_follow = ufsrec->path;
		      ufsrec->path = ufsrec->path->next;
		      
		      ufsrec->to_follow->next = NULLEntryList;
		      
		      if (process_ufsearch(ufsrec) == QERR_ok)
			return RQ_processing;
		    }
		  
		  result->tasks_sent = ufsrec->tasks_sent;
		  result->tasks_failed = ufsrec->tasks_failed;
		  
		  result->hits = NULLEntryList;
		  result->hit_num = 0;
		  
		  result->errors = request->errors;
		  request->errors = NULLError;
		  
		  result->search_status = UFS_Failed;
		  
		  ufsrec->result = result;
		  
		  return RQ_results_returned;
		}
	    }
	}
      else /* Results from an intermediate search */
	{
	  if (ufsrec->to_follow == NULLEntryList)
	    {
	      if (ufsrec->path != NULLEntryList)
		{
		  ufsrec->to_follow = ufsrec->path;
		  ufsrec->path = ufsrec->path->next;
		  
		  ufsrec->to_follow->next = NULLEntryList;
		  
		  if (process_ufsearch(ufsrec) == QERR_ok)
		    return RQ_processing;
		}
	      
	      result = ufsearch_res_alloc();
	      
	      result->tasks_sent = ufsrec->tasks_sent;
	      result->tasks_failed = ufsrec->tasks_failed;
	      
	      result->hits = NULLEntryList;
	      result->hit_num = 0;
	      
	      result->errors = request->errors;
	      request->errors = NULLError;
	      
	      result->search_status = UFS_Failed;
      
	      ufsrec->result = result;
	
	      return RQ_results_returned;
	    }
	  
	  return process_ufsearch(ufsrec) == QERR_ok ?
	    RQ_processing : RQ_results_returned;
	}
    }
  
  return RQ_processing;
} /* process_ufs_ds_result */


/*
 * - ufsearch_result_free() -
 *
 *
 */
void ufsearch_result_free(result_ptr)
     ufsearchResult *result_ptr;
{
  ufsearchResult ufs_result = *result_ptr;

  if (ufs_result == NULLUfsearchResult) return;

  *result_ptr = NULLUfsearchResult;

  dn_list_free(&ufs_result->hits);

  free((char *) ufs_result);
}

/*
 * - get_ufs_results() -
 * Copy and return results for the identified ufn request, then delete all
 * records for that request.
 *
 */
ufsearchResult get_ufsearch_result(id)
     QCardinal id;
{
  requestRec ufs_request = _get_request_of_id(id);
  ufsearchRec ufsrec;
  ufsearchResult result;

  ufsrec = ufs_request->UFSEARCH_REC;
  result = ufsrec->result;

  result->tasks_sent = ufsrec->tasks_sent;
  result->tasks_failed = ufsrec->tasks_failed;

  search_pair_list_free(&ufsrec->search_data);
  search_pair_list_free(&ufsrec->target_data);

  ufsrec->result = NULLUfsearchResult;

  _request_complete(id);

  return result;
} /* get_ufn_results */


/*
 * - ufsearch_rec_free() -
 *
 *
 */
void ufsearch_rec_free(record)
     ufsearchRec record;
{
  if (record == NULLUfsearchRec) return;

  if (record->search_data != NULLSearchPair)
    search_pair_list_free(&record->search_data);

  if (record->target_data != NULLSearchPair)
    search_pair_list_free(&record->target_data);

  free((char *) record);
} /* ufsearch_rec_free */


void search_pair_list_free(list)
     searchPair *list;
{
  searchPair curr, next;

  for (curr = *list; curr != NULLSearchPair; curr = next)
    {
      next = curr->next;
      free_string_seq(&curr->values);
      free((char *) curr);
    }

  *list = NULLSearchPair;
}

