#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ufname.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ufname.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*
 * $Log: ufname.c,v $
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  ufname.c -

*****************************************************************************/

#include <ctype.h>

#include "types.h"
#include "ufname.h"
#include "request.h"
#include "error.h"
#include "util.h"
#include "ds_tasks.h"
#include "ds_util.h"

#include "quipu/ds_search.h"
#include "quipu/dap2.h"
#include "quipu/name.h"

extern int dsap_ad;
extern int next_task_id;

#ifndef NO_STATS
extern LLog    *log_stat;
#endif

static QE_error_code process_ufn_search();
static QE_error_code directory_search();
static QBool follow_path();

searchPath ufnpaths = NULLSearchPath;

/*
 * - get_ufn_status() -
 *
 *
 */
ufnStatus get_ufn_status(request_id)
     QCardinal request_id;
{
  (void) _get_request_of_id(request_id);

  return NULLUfnStatus;
  
} /* get_ufn_status */


/*
 * - get_ufn_results() -
 * Copy and return results for the identified ufn request, then delete all
 * records for that request.
 *
 */
ufnResults get_ufn_results(id)
     QCardinal id;
{
  requestRec ufn_request = _get_request_of_id(id);
  ufnameRec ufnrec;
  ufnResults results;

  ufnrec = ufn_request->UFNAME_REC;
  results = ufnrec->results;

  results->tasks_sent = ufnrec->tasks_sent;
  results->tasks_failed = ufnrec->tasks_failed;

  ufnrec->results = NULLUfnResults;

  return results;
} /* get_ufn_results */

/*
 * - do_ufn_resolve() -
 * Initiate a UFN search.
 *
 */
QE_error_code do_ufn_resolve(baseobjects, ufname, is_leaf, id_ptr)
     entryList baseobjects;
     namePart ufname;
     known is_leaf;
     QCardinal *id_ptr;
{
  ufnameRec new_request;
  namePart root_part, tmp_part;
  entryList path;
  requestRec request;
  int name_part_count;

  /* Record request invocation */
  (void) _request_invoked(UFNAME, id_ptr);

  request = _get_request_of_id(*id_ptr);
  new_request = request->UFNAME_REC;

  new_request->name_parts = ufname;
  new_request->tasks_sent = new_request->tasks_failed = 0;

  for (tmp_part = ufname, name_part_count = 1;
       tmp_part->next != NULLNamePart;
       tmp_part = tmp_part->next, name_part_count++)
    ;
  
  tmp_part->is_bottom_level = is_leaf;
    
  if (baseobjects != NULLEntryList)
    {
      QCardinal base_object_num;

      root_part = name_part_alloc();
      root_part->next = ufname;
      
      root_part->good_matches = root_part->poor_matches = NULLEntryList;
      root_part->good_match_num = root_part->exact_match_num = 0;

      root_part->part_name = NULLCP;
      root_part->is_resolved = TRUE;

      root_part->is_bottom_level = no;
      root_part->exact_matches = baseobjects;

      for (base_object_num = 0;
	   baseobjects != NULLEntryList;
	   baseobjects = baseobjects->next, base_object_num++)
	;
      
      root_part->exact_match_num = base_object_num;
      new_request->name_parts = root_part;
    }
  else
    {
      path = new_request->path = get_ufn_path(name_part_count);

      root_part = name_part_alloc();
      root_part->next = ufname;
      
      root_part->good_matches =
	root_part->poor_matches =
	  root_part->exact_matches = NULLEntryList;

      root_part->good_match_num = 0;
      root_part->part_name = NULLCP;

      root_part->is_resolved = TRUE;
      root_part->is_bottom_level = no;
      
      (void) dn_list_add(path == NULLEntryList? "" : path->string_dn,
			 &root_part->exact_matches,
			 NULLAttrT);
      new_request->path = new_request->path->next;

      root_part->exact_match_num = 1;
      new_request->name_parts = root_part;
    }

  new_request->request_id = *id_ptr;
  new_request->results = NULLUfnResults;

  return process_ufn_search(new_request);
} /* do_ufn_resolve */

/*
 * - process_ufn_search() -
 * Do/continue a UFN search. Uses requestRec to determine the stage that
 * the search has reached, and then proceeds accordingly.
 *		
 * If search not yet started do an appropriate ds_search against the given base
 * object.
 *
 * If the given name is only partially resolved, search against the list
 * of partial names resolved thus far.
 *
 */
static QE_error_code process_ufn_search(ufnrec)
     ufnameRec ufnrec;
{
  namePart name_comp, prev_comp;
  entryList resolved_names;
  QE_error_code search_status;
  requestRec request;

  /* If ds searches outstanding, then do nothing. */
  if (ufnrec->exact_task_count > 0 || ufnrec->approx_task_count > 0)
    return QERR_ok;

  /* Find out how far the search has gone. */
  for (name_comp = ufnrec->name_parts, prev_comp = NULLNamePart; 
       name_comp != NULLNamePart && name_comp->is_resolved == TRUE;
       prev_comp = name_comp, name_comp = name_comp->next)
    ;

  request = _get_request_of_id(ufnrec->request_id);
  if (prev_comp == NULLNamePart)
    {
      /* If no name part_names yet resolved search against root object. */
      if ((search_status = directory_search(NULLCP, name_comp, ufnrec))
	  != QERR_ok)
	{
	  add_error_to_request_rec(request,
				   NULLCP,
				   search_status,
				   (struct DSError *) NULL);
	  return search_status;
	}
    }
  else
    {
      /* Else search against all resolved names. If no exact matches, use
	 good matches. If no good matches use poor matches.
	 Return partial matches if match number exceeds limit */
      
      if (prev_comp->exact_match_num > 0)
	resolved_names = prev_comp->exact_matches;
      else if (prev_comp->good_match_num > 0)
	resolved_names = prev_comp->good_matches;
      else if (prev_comp->poor_match_num > 0)
	resolved_names = prev_comp->poor_matches;
      else
	return QERR_nothing_found;
      
      while (resolved_names != NULLEntryList)
	{
	  if ((search_status =
	       directory_search(resolved_names->string_dn, name_comp, ufnrec))
	      != QERR_ok)
	    return search_status;

	  resolved_names = resolved_names->next;
	}
    }
  
  return QERR_ok;
} /* process_ufn_search */


/*
 *
 *
 *
 */
request_state continue_ufn_search(good_matches, request_id)
     entryList good_matches;
     QCardinal request_id;
{
  namePart last_part, first_real_part;
  ufnameRec ufnrec;
  requestRec request;
  ufnResults results;
  QCardinal good_match_num;
  entryList curr_match;

  request = _get_request_of_id(request_id);
  ufnrec = request->UFNAME_REC;

  for (last_part = ufnrec->name_parts->next;
       last_part->poor_matches != NULLEntryList &&
       last_part->good_matches == NULLEntryList &&
       last_part->exact_matches == NULLEntryList &&
       last_part != NULLNamePart;
       last_part = last_part->next)
    ;

  if (last_part == NULLNamePart)
    return RQ_error_returned;
  else
    {
      for (good_match_num = 0, curr_match = good_matches;
	   curr_match != NULLEntryList;
	   curr_match = curr_match->next, good_match_num++)
	;

      if (good_match_num == 0)
	{
	  first_real_part = ufnrec->name_parts->next;
	  
	  if (first_real_part->is_resolved != TRUE)
	    {
	      if (ufnrec->path != NULLEntryList)
		if (follow_path(ufnrec) == TRUE)
		  {
		    ufnrec->results = NULLUfnResults;
		    return RQ_processing;
		  }

	      results = ufnrec->results = ufn_res_alloc();
	      
	      results->match_status = Failed;
	      results->tried_intermediate = TRUE;
	      
	      results->resolved_part = results->unresolved_part = NULLCP;

	      results->tasks_sent = ufnrec->tasks_sent;
	      results->tasks_failed = ufnrec->tasks_failed;

	      results->match_num = 0;
	      results->matches = NULLEntryList;

	      results->errors = request->errors;
	      request->errors = NULLError;
	    }
	  else
	    {
	      ufnrec->results = NULLUfnResults;
	      return RQ_processing;
	    }
	}
      
      last_part->good_match_num = good_match_num;
      last_part->good_matches = good_matches;

      last_part->is_resolved = TRUE;

      if (process_ufn_search(ufnrec) != QERR_ok)
	return RQ_error_returned;
      else
	return RQ_processing;
    }
} /* continue_ufn_search */

/*
 * - directory_search() -
 * Compose and invoke a X500 search.
 *
 */
static QE_error_code directory_search(base_name, purp_name_comp, ufnrec)
     char *base_name;
     namePart purp_name_comp;
     ufnameRec ufnrec;
{
  struct ds_search_arg search_arg;
  struct DAPindication di;
  DN base_dn;
  AttributeType attr_type;
  QBool is_attr_cmp = FALSE;
  char *str_oid,
       *start,
       *end,
       *search_value;
  QCardinal level_count;
  QBool has_org = FALSE, has_loc = FALSE, has_org_unit = FALSE;
  Filter exact_filter, approx_filter;
  Attr_Sequence get_attrs = (Attr_Sequence) NULL;
  int task_id, count;

  char *TidyString();

  if (purp_name_comp->next == NULLNamePart &&
      purp_name_comp->is_bottom_level == no)
    return QERR_nothing_found;

  if (purp_name_comp->is_bottom_level == might_not)
    purp_name_comp->is_bottom_level = no;

  /* Make a DN struct from given string object name.
     If not valid then return an error. */
  if (base_name == NULLCP || isnull(*base_name)) base_dn = NULLDN;
  else if ((base_dn = str2dn(base_name)) == NULLDN) return QERR_bad_name;

  /* `Analyse' the string dn. */
  start = end = base_name;
  level_count = 0;
  if (base_dn != NULLDN)
    while (1)
      {
	end = index(start, '=');
	
	*end = '\0';
	if ((attr_type = AttrT_new(start)) == NULLAttrT) return QERR_bad_name;
	*end = '=';
	
	/* Need the string oid of the base type. */
	str_oid = attr_type->oa_ot.ot_stroid;
	
	if (lexequ(str_oid, "2.5.4.7") == 0) has_loc = TRUE;
	else if (lexequ(str_oid, "2.5.4.11") == 0) has_org_unit = TRUE;
	else if (lexequ(str_oid, "2.5.4.10") == 0) has_org = TRUE;
	
	level_count++;
	
	if ((start = index(end, '@')) == NULLCP) break;
	end = ++start;
      }
  
  /* Implace search parameters. */
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
  search_arg.sra_eis.eis_select = NULL;

  /* Search dereferences alias entries. */
  search_arg.sra_searchaliases = TRUE;

  if (level_count >= 2 && has_org == TRUE)
    search_arg.sra_subset = SRA_WHOLESUBTREE;
  else
    search_arg.sra_subset = SRA_ONELEVEL;

  /* Make search filters for exact and approx matches. */
  search_value = TidyString(purp_name_comp->part_name);

  /* If a type is given, make appropriate filters. */
  if (index(search_value, '=') != NULLCP)
    {
      is_attr_cmp = TRUE;
      if (make_typed_filter_items(search_value, &exact_filter, &approx_filter)
	  != QERR_ok)
	  return QERR_bad_value_syntax;
    }
  /*
   * If base object == root, can only search for a country.
   */
  else if (base_dn == NULLDN)
    {
      if ((int)strlen(search_value) > 2)
	{
	  attr_type = AttrT_new("0.9.2342.19200300.99.1.8");
	  get_attrs = as_comp_new(attr_type, NULLAV, NULLACL_INFO);

	  if (make_filter_items(attr_type,
				search_value,
				&exact_filter,
				&approx_filter) != QERR_ok)
	    return QERR_bad_value_syntax;
	}
      else
	{
	  exact_filter = filter_alloc();
	  approx_filter = filter_alloc();
	  
	  approx_filter->flt_next = exact_filter->flt_next = NULLFILTER;
	  exact_filter->flt_type = approx_filter->flt_type = FILTER_OR;
	  
	  attr_type = AttrT_new("2.5.4.6");
	  get_attrs = as_comp_new(attr_type, NULLAV, NULLACL_INFO);

	  if (make_filter_items(attr_type,
				search_value,
				&exact_filter->FUFILT,
				&approx_filter->FUFILT) != QERR_ok)
	    {
	      filter_free(exact_filter);
	      filter_free(approx_filter);

	      attr_type = AttrT_new("0.9.2342.19200300.99.1.8");
	      get_attrs = as_merge(get_attrs,
				   as_comp_new(attr_type,
					       NULLAV,
					       NULLACL_INFO));

	      if (make_filter_items(attr_type,
				    search_value,
				    &exact_filter,
				    &approx_filter) != QERR_ok)
		{
		  return QERR_bad_value_syntax;
		}
	    }
	  else
	    {
	      attr_type = AttrT_new("0.9.2342.19200300.99.1.8");
	      get_attrs = as_merge(get_attrs,
				   as_comp_new(attr_type,
					       NULLAV,
					       NULLACL_INFO));


	      if (make_filter_items(attr_type,
				    search_value,
				    &exact_filter->FUFILT->flt_next,
				    &approx_filter->FUFILT->flt_next)
		  != QERR_ok)
		{
		  filter_free(exact_filter);
		  filter_free(approx_filter);
		  
		  return QERR_bad_value_syntax;
		}
	    }

	}
    }
  /*
   * Check for `bottom level' case first so search 
   * against `cn', `sn' and `uid'. Get the `cn' and `uid' attrs back.
   */
  else if (purp_name_comp->is_bottom_level == unknown &&
	   level_count >= 2 &&
	   has_org == TRUE)
    {
      search_arg.sra_common.ca_servicecontrol.svc_sizelimit = 20;
      
      exact_filter = filter_alloc();
      approx_filter = filter_alloc();
      
      approx_filter->flt_next = exact_filter->flt_next = NULLFILTER;
      exact_filter->flt_type = approx_filter->flt_type = FILTER_OR;
      
      /* `cn' filter item. */
      attr_type = AttrT_new("2.5.4.3");
      get_attrs = as_comp_new(attr_type, NULLAV, NULLACL_INFO);
   
      if (make_filter_items(attr_type,
			    search_value,
			    &exact_filter->FUFILT,
			    &approx_filter->FUFILT) != QERR_ok)
	{
	  filter_free(exact_filter);
	  filter_free(approx_filter);
	  
	  return QERR_bad_value_syntax;
	}
      
      /* `sn' filter item. */
      attr_type = AttrT_new("2.5.4.4");

      if (make_filter_items(attr_type,
			    search_value,
			    &exact_filter->FUFILT->flt_next, 
			    &approx_filter->FUFILT->flt_next) != QERR_ok)
	{
	  filter_free(exact_filter);
	  filter_free(approx_filter);
	  
	  return QERR_bad_value_syntax;
	}

      exact_filter->FUFILT->flt_next->flt_next = NULLFILTER;
      approx_filter->FUFILT->flt_next->flt_next = NULLFILTER;
    }
  /*
   * Otherwise do an intermediate search.
   */
  else
    {
      /*
       * If `organizationalUnit' in base object name filter on `ou'.
       */
      search_arg.sra_subset = SRA_ONELEVEL;
      
      if (has_org_unit)
	{
	  attr_type = AttrT_new("2.5.4.11");

	  /* Get back the 'ou' attrs */
	  get_attrs = as_comp_new(attr_type, NULLAV, NULLACL_INFO);

	  if (make_filter_items(attr_type,
				search_value,
				&exact_filter,
				&approx_filter) != QERR_ok)
	    {
	      return QERR_bad_value_syntax;
	    }
	}
      /*
       * If no `locality' or `organization' in base object name search
       * filter on `locality' or `organization'.
       */
      else if (!has_loc && !has_org)
	{
	  exact_filter = filter_alloc();
	  approx_filter = filter_alloc();
	  
	  approx_filter->flt_next = exact_filter->flt_next = NULLFILTER;
	  exact_filter->flt_type = approx_filter->flt_type = FILTER_OR;
	  
	  exact_filter->FUFILT = filter_alloc();
	  approx_filter->FUFILT = filter_alloc();
	  
	  exact_filter->FUFILT->flt_next = filter_alloc();
	  approx_filter->FUFILT->flt_next = filter_alloc();
	  
	  approx_filter->FUFILT->flt_next->flt_next =
	    exact_filter->FUFILT->flt_next->flt_next = 
	      approx_filter->flt_next =
		exact_filter->flt_next = NULLFILTER;
	  
	  exact_filter->FUFILT->flt_type =
	    approx_filter->FUFILT->flt_type =
	      exact_filter->FUFILT->flt_next->flt_type =
		approx_filter->FUFILT->flt_next->flt_type = FILTER_AND;
	  
	  /* `localityName' filter. */
	  attr_type = AttrT_new("2.5.4.7");
	  get_attrs = as_comp_new(attr_type, NULLAV, NULLACL_INFO);

	  if (make_filter_items(attr_type,
				search_value,
				&exact_filter->FUFILT->FUFILT,
				&approx_filter->FUFILT->FUFILT) != QERR_ok)
	    {
	      filter_free(exact_filter);
	      filter_free(approx_filter);
	      
	      return QERR_bad_value_syntax;
	    }
	  
	  /* Make sure it's a locality object type */
	  attr_type = AttrT_new("2.5.4.0");

	  if (make_filter_items(attr_type,
				"2.5.6.3",
				&exact_filter->FUFILT->FUFILT->flt_next,
				&approx_filter->FUFILT->FUFILT->flt_next)
	      != QERR_ok)
	    {
	      filter_free(exact_filter);
	      filter_free(approx_filter);
	      
	      return QERR_bad_value_syntax;
	    }

	  exact_filter->FUFILT->FUFILT->flt_next->flt_next =
	    approx_filter->FUFILT->FUFILT->flt_next->flt_next = NULLFILTER;
	  
	  /* `organizationName' filter item. */
	  attr_type = AttrT_new("2.5.4.10");
	  get_attrs = as_merge(get_attrs,
			       as_comp_new(attr_type, NULLAV, NULLACL_INFO));

	  if (make_filter_items(attr_type,
				search_value,
				&exact_filter->FUFILT->flt_next->FUFILT,
				&approx_filter->FUFILT->flt_next->FUFILT)
	      != QERR_ok)
	    {
	      filter_free(exact_filter);
	      filter_free(approx_filter);
	      
	      return QERR_bad_value_syntax;
	    }

	  /* Make sure it's an org object type */
	  attr_type = AttrT_new("2.5.4.0");

	  if (make_filter_items(attr_type,
				"2.5.6.4",
			    &exact_filter->FUFILT->flt_next->FUFILT->flt_next,
			    &approx_filter->FUFILT->flt_next->FUFILT->flt_next)
	      != QERR_ok)
	    {
	      filter_free(exact_filter);
	      filter_free(approx_filter);
	      
	      return QERR_bad_value_syntax;
	    }
	  
	  exact_filter->FUFILT->flt_next->FUFILT->flt_next->flt_next =
	    approx_filter->FUFILT->flt_next->FUFILT->flt_next->flt_next =
	      NULLFILTER;
	}
      /*
       * If `organization' in base object name filter against 
       * `ou' and `localityName'.
       */
      else if (has_org)
	{
	  exact_filter = filter_alloc();
	  approx_filter = filter_alloc();
	  
	  approx_filter->flt_next = exact_filter->flt_next = NULLFILTER;
	  exact_filter->flt_type = approx_filter->flt_type = FILTER_OR;
	  
	  /* `ou' filter item. */
	  attr_type = AttrT_new("2.5.4.11");
	  get_attrs = as_comp_new(attr_type, NULLAV, NULLACL_INFO);

	  if (make_filter_items(attr_type,
				search_value,
				&exact_filter->FUFILT,
				&approx_filter->FUFILT) != QERR_ok)
	    {
	      filter_free(exact_filter);
	      filter_free(approx_filter);
	
	      return QERR_bad_value_syntax;
	    }
	  
	  /* `localityName' filter item. */
	  attr_type = AttrT_new("2.5.4.7");
	  get_attrs = as_merge(get_attrs,
			       as_comp_new(attr_type, NULLAV, NULLACL_INFO));

	  if (make_filter_items(attr_type,
				search_value,
				&exact_filter->FUFILT->flt_next,
				&approx_filter->FUFILT->flt_next) != QERR_ok)
	    {
	      filter_free(exact_filter);
	      filter_free(approx_filter);
	      
	      return QERR_bad_value_syntax;
	    }
	  
	  exact_filter->FUFILT->flt_next->flt_next =
	    approx_filter->FUFILT->flt_next->flt_next = NULLFILTER;
	}
      /*
       * If none of above and base object name has `locality' filter
       * on `organizationName'.
       */
      else if (has_loc)
	{
	  attr_type = AttrT_new("2.5.4.10");
	  get_attrs = as_comp_new(attr_type, NULLAV, NULLACL_INFO);

	  if (make_filter_items(attr_type,
				search_value,
				&exact_filter,
				&approx_filter) != QERR_ok)
	    {
	      return QERR_bad_value_syntax;
	    }
	}
    }
  
  /*
   * First invoke exact match, then invoke approx match.
   * Record task invocation for this particular request.
   *
   */
  for (count = 0;
       ufnrec->dap_exact_task_ids[count] != NO_TASK 
       && count < MAX_TASKS_PER_REQ;
       count++)
    ;

  if (count == MAX_TASKS_PER_REQ)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      return QERR_internal_limit_reached;
    }    
  
  if (_task_invoked(SEARCH_TASK, base_name, ufnrec->request_id, &task_id) 
      != QERR_ok)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      return QERR_internal_limit_reached;
    }
  
  /* Invoke search using exact match. */
  search_arg.sra_filter = exact_filter;
  if (DapSearch(dsap_ad, task_id, &search_arg, &di, ROS_ASYNC) == NOTOK)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      _task_complete(task_id);
      return QERR_request_failed;
    }
  else
    {
      ufnrec->exact_task_count++;
      ufnrec->dap_exact_task_ids[count] = task_id;
      ufnrec->tasks_sent++;
    }
  
#ifndef NO_STATS
  LLOG (log_stat, LLOG_NOTICE,
	("EXACT MATCH +%s, task %d, extent %d, val %s",
	 base_name, task_id, 
	 search_arg.sra_subset,
	 search_value));
#endif

  if (count == MAX_TASKS_PER_REQ)
    {
      filter_free(exact_filter);
      filter_free(approx_filter);
      
      return QERR_internal_limit_reached;
    }
  
  /*
   * Invoke approx match.
   * Record task invocation for this particular request.
   */
  
  if (is_attr_cmp == FALSE)
    {
      for (count = 0;
	   ufnrec->dap_approx_task_ids[count] != NO_TASK 
	   && count < MAX_TASKS_PER_REQ;
	   count++)
	;
      
      if (count == MAX_TASKS_PER_REQ)
	{
	  filter_free(exact_filter);
	  filter_free(approx_filter);
	  
	  return QERR_internal_limit_reached;
	}
      
      if (_task_invoked(SEARCH_TASK, base_name, ufnrec->request_id, &task_id) 
	  != QERR_ok)
	{
	  filter_free(exact_filter);
	  filter_free(approx_filter);
	  
	  return QERR_internal_limit_reached;
	}
      
      search_arg.sra_filter = approx_filter;
      search_arg.sra_eis.eis_select = get_attrs;
      
      if (DapSearch(dsap_ad, task_id, &search_arg, &di, ROS_ASYNC) == NOTOK)
	{
	  filter_free(exact_filter);
	  filter_free(approx_filter);
	  
	  _task_complete(task_id);
	  return QERR_request_failed;
	}
      else
	{
	  ufnrec->approx_task_count++;
	  ufnrec->dap_approx_task_ids[count] = task_id;
	  ufnrec->tasks_sent++;
	}
      
#ifndef NO_STATS
      LLOG (log_stat, LLOG_NOTICE,
	    ("APPROX MATCH +%s, task %d, extent %d, val %s",
	     base_name,
	     task_id,
	     search_arg.sra_subset,
	     search_value));
#endif
    }

  if (get_attrs != (Attr_Sequence) NULL)
    as_free(get_attrs);
  
  filter_free(exact_filter);
  filter_free(approx_filter);

  return QERR_ok;
} /* directory_search */

/*
 * - ufname_rec_free() -
 * Free a ufname_rec structure.
 *
 */
void ufname_rec_free(record)
     ufnameRec record;
{
  if (record == NULLUfnameRec) return;
  
  name_part_free(&record->name_parts);
  free((char *) record);
} /* ufname_rec_free */

/*
 * - ufname_result_free() -
 *
 *
 */
void ufname_result_free(ufn_result)
     ufnResults *ufn_result;
{
  ufnResults result = *ufn_result;
  if (result == NULLUfnResults) return;

  error_list_free(&result->errors);
  dn_list_free(&result->matches);

  if (result->unresolved_part != NULLCP) free(result->unresolved_part);
  if (result->resolved_part != NULLCP) free(result->resolved_part);

  free((char *) result);
  *ufn_result = NULLUfnResults;
} /* ufname_result_free */

/*
 * - name_part_free() -
 * Free a name part list.
 *
 */
void name_part_free(name)
     namePart *name;
{ 
  register namePart part = *name;
  namePart next_part;

  while (part)
    {
      free(part->part_name);
      
      if (part->exact_matches != NULLEntryList)
	dn_list_free(&part->exact_matches);
      
      if (part->good_matches != NULLEntryList)
	dn_list_free(&part->good_matches);
      
      if (part->poor_matches != NULLEntryList)
	dn_list_free(&part->poor_matches);
      
      next_part = part->next;
      free((char *) part);
      part = next_part;
    }

  *name = NULLNamePart;
} /* name_part_free */


/*
 * - str2ufname() -
 * Convert a string encode UFN into a list of name part_names.
 *
 */
namePart str2ufname(str_ufn)
     char *str_ufn;
{
  namePart name_comp = NULLNamePart, new_comp;
  register char *start, *end;
  char save;

  start = str_ufn;
  while (!isnull(*start))
    {
      while (isspace(*start) && !isnull(*start)) start++;
      
      end = start;
      if (*end++ == '\"')
	{
	  while (*end != '\"' && !isnull(*end)) end++;
	  
	  if (isnull(*end))
	    {
	      if (name_comp != NULLNamePart) name_part_free(&name_comp);
	      return NULLNamePart;
	    }
	}

      while (*end != ',' && !isnull(*end)) end++;
      end--;
      
      while (isspace(*end)) end--;
      end++;
      
      save = *end;
      *end = '\0';
      
      if (name_comp == NULLNamePart)
	{
	  name_comp = name_part_alloc();
	  name_comp->is_bottom_level = unknown;
	  name_comp->next = NULLNamePart;
	}
      else
	{
	  new_comp = name_part_alloc();
	  new_comp->next = name_comp;
	  name_comp = new_comp;
	  name_comp->is_bottom_level = no;
	}
      
      name_comp->part_name = copy_string(start);
      name_comp->is_resolved = FALSE;

      name_comp->exact_matches =
	name_comp->good_matches =
	  name_comp->poor_matches = NULLEntryList;
      
      name_comp->exact_match_num =
	name_comp->good_match_num =
	  name_comp->poor_match_num = 0;
      
      *end = save;

      while (*end != ',' && !isnull(*end)) end++;
      if (!isnull(*end)) end++;
      while (isspace(*end)) end++;
      
      start = end;
    }

  return name_comp;
} /* str2ufname */

/*
 * - process_ufn_ds_result() -
 * A ds result bound for a ufn request has been received. Process the
 * results and update the request record accordingly. If request has
 * completed indicate this.
 *
 */
request_state process_ufn_ds_result(request, task_id, ds_result)
     requestRec request;
     int task_id;
     struct DSResult *ds_result;
{
  DsTask task_rec;
  ufnameRec ufnrec = request->UFNAME_REC;
  namePart name_comp, curr_part, part;
  int *ufn_task_array;
  QBool is_exact_match_result;
  QCardinal aindex, hit_count;
  struct ds_search_result *search_result;

  task_rec = _get_task_of_id(task_id);

  /* This shouldn't happen */
  if (task_rec == NULLDsTask) return RQ_processing;

  for (name_comp = ufnrec->name_parts;
       name_comp->is_resolved == TRUE && name_comp != NULLNamePart;
       name_comp = name_comp->next) 
    ;
  
  /* This shouldn't happen! */
  if (name_comp == NULLNamePart)
    {
      _task_complete(task_id);
      return RQ_processing;
    }

  /* Check if result comes from a search using exact or approx match. */
  for (aindex = 0, ufn_task_array = ufnrec->dap_exact_task_ids;
       ufn_task_array[aindex] != task_id && aindex < MAX_TASKS_PER_REQ;
       aindex++)
    ;
  
  /* Not an exact search, so must have been an approx search. */
  if (aindex >= MAX_TASKS_PER_REQ)
    {
      for (aindex = 0, ufn_task_array = ufnrec->dap_approx_task_ids;
	   ufn_task_array[aindex] != task_id && aindex < MAX_TASKS_PER_REQ;
	   aindex++)
	;
      
      if (aindex < MAX_TASKS_PER_REQ) is_exact_match_result = FALSE;
    }
  else
    is_exact_match_result = TRUE;

  /* If not an approx search, then something's wrong! */
  if (aindex >= MAX_TASKS_PER_REQ)
    {
      _task_complete(task_id);
      return RQ_processing;
    }

  /* Now get the search results. */
  search_result = &ds_result->res_sr;
  correlate_search_results(search_result);
  
  /* ######### Have to d this could because of a dsap bug(?) ######### */
  search_result->srr_next = NULLSRR;

  hit_count = 0;
  if (search_result->CSR_entries != NULLENTRYINFO)
    {
      EntryInfo *entry_ptr;
      register stringCell curr_attr;
      attrValList got_attrs, curr_av;
      char *curr_name, *match_str;
      QBool good_match;
      char *dn2str();
      void get_read_attrs();
      
      /* Decode and add found entry names */
      for (entry_ptr = search_result->CSR_entries;
	   entry_ptr != NULLENTRYINFO;
	   entry_ptr = entry_ptr->ent_next)
	{
	  curr_name = qy_dn2str(entry_ptr->ent_dn);
	  if (!is_exact_match_result)
	    {
	      match_str = name_comp->part_name;
	      
	      if (index(match_str, '=') == NULLCP)
		{
		  get_read_attrs(entry_ptr->ent_attr, &got_attrs, READOUT);
		  if (got_attrs != NULLAVList)
		    for (curr_av = got_attrs, good_match = FALSE;
			 curr_av != NULLAVList && good_match == FALSE;
			 curr_av = curr_av->next)
		      for (curr_attr = curr_av->val_list;
			   curr_attr != NULLStrCell && good_match == FALSE;
			   curr_attr = curr_attr->next)
			{
			  if (is_good_match(match_str, curr_attr->string))
			    good_match = TRUE;
			}
		  else
		    good_match = FALSE;
		}
	      else
		good_match = TRUE;

	      if (good_match == TRUE)
		{
		  if (dn_list_add(curr_name,
				  &name_comp->good_matches,
				  NULLAttrT))
		    name_comp->good_match_num++;
		}
	      else
		{
		  if ((int)strlen(match_str) > 2
		      &&
		      dn_list_add(curr_name,
				  &name_comp->poor_matches,
				  NULLAttrT))
		    name_comp->poor_match_num++;
		}
	      
	      if (got_attrs != NULLAVList)
		{
		  free_string_seq(&(got_attrs->attr_name));
		  free_string_seq(&(got_attrs->val_list));
		  free((char *) got_attrs);
	      
		  got_attrs = NULLAVList;
		}
	    }
	  else
	    if (dn_list_add(curr_name, &name_comp->good_matches, NULLAttrT))
	      name_comp->good_match_num++;

	  hit_count++;
	  if (curr_name != NULLCP)
	    free(curr_name);
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
    }
  
#ifndef NO_STATS
    LLOG (log_stat, LLOG_NOTICE,
	  ("SEARCH RESULT from +%s, task %d, hits %d",
	   task_rec->baseobject,
	   task_id,
	   hit_count));
#endif
    
  /*
   *	Remove record of this task
   */

  ufn_task_array[aindex] = NO_TASK;

  if (is_exact_match_result == TRUE)
    ufnrec->exact_task_count--;
  else
    ufnrec->approx_task_count--;

  _task_complete(task_id);

  /* Check the status of this request. */
  /* First check if this stage of matching has been completed */
  if (ufnrec->exact_task_count == 0 && ufnrec->approx_task_count == 0)
    {
      ufnResults results = ufn_res_alloc();

      results->tried_intermediate = FALSE;
      results->match_status = Failed;

      results->match_num = 0;
      results->matches = NULLEntryList;

      results->unresolved_part = results->resolved_part = NULLCP;
      results->errors = NULLError;

      results->tasks_sent = ufnrec->tasks_sent;
      results->tasks_failed = ufnrec->tasks_failed;

      if (name_comp->exact_match_num > 0 || name_comp->good_match_num > 0)
	name_comp->is_resolved = TRUE;

      /*
       * If no matches found and haven't checked for intermediate level
       * entries, do so, else continue.
       *
       */
      
      if (name_comp->exact_match_num == 0 &&
	  name_comp->good_match_num == 0 &&
	  name_comp->poor_match_num == 0)
	{
	  if (name_comp->is_bottom_level == unknown)
	    name_comp->is_bottom_level = might_not;

	  if (name_comp->is_bottom_level == no ||
	      process_ufn_search(ufnrec) == QERR_nothing_found)
	    {
	      part = ufnrec->name_parts->next;
	      
	      if (ufnrec->path != NULLEntryList && part->is_resolved != TRUE)
		if (follow_path(ufnrec) == TRUE)
		  {
		    (void) free((char *) results);
		    return RQ_processing;
		  }
	      
	      ufnrec->results = results;
	      
	      results->match_num = 0;
	      results->matches = NULLEntryList;

	      results->errors = request->errors;
	      request->errors = NULLError;

	      return RQ_results_returned;
	    }
	  else
	    {
	      (void) free((char *) results);
	      return RQ_processing;
	    }
	}

      /*
       * If final part_name of name has been matched,
       * then return appropriate list of entry names.
       *
       */
      if (name_comp->next == NULLNamePart)
	{
	  ufnrec->results = results;

	  results->tried_intermediate = TRUE;
	  results->unresolved_part = results->resolved_part = NULLCP;

	  results->errors = request->errors;
	  request->errors = NULLError;

	  if (name_comp->exact_match_num > 0)
	    {
	      name_comp->is_resolved = TRUE;

	      results->match_num = name_comp->exact_match_num;
	      results->matches = name_comp->exact_matches;

	      results->match_status = GoodMatches;
	      
	      name_comp->exact_matches = NULLEntryList;
	      name_comp->exact_match_num = 0;
	    }
	  else if (name_comp->good_match_num > 0)
	    {
	      name_comp->is_resolved = TRUE;
    
	      results->match_num = name_comp->good_match_num;
	      results->matches = name_comp->good_matches;

	      results->match_status = GoodMatches;
	      
	      name_comp->good_matches = NULLEntryList;
	      name_comp->good_match_num = 0;
	    }
	  else if (name_comp->poor_match_num > 0)
	    {
	      char resolved_part[LINESIZE], buf[LINESIZE];
		  
	      part = ufnrec->name_parts->next;

	      resolved_part[0] = '\0';
	      buf[0] = '\0';
	      for (curr_part = ufnrec->name_parts;
		   curr_part != NULLNamePart;
		   curr_part = curr_part->next)
		{
		  if (curr_part->part_name != NULLCP)
		    {
		      if (resolved_part[0] != '\0')
			{
			  (void) strcpy(buf, resolved_part);
			  (void) strcpy(resolved_part, curr_part->part_name);
			  (void) strcat(resolved_part, ", ");
			  (void) strcat(resolved_part, buf);
			}
		      else
			(void) strcpy(resolved_part, curr_part->part_name);
		    }
		}
	      
	      results->tried_intermediate =
		(name_comp->is_bottom_level == unknown)? FALSE: TRUE;
	      
	      results->match_num = name_comp->poor_match_num;
	      results->matches = name_comp->poor_matches;

	      results->resolved_part = copy_string(resolved_part);
	      results->unresolved_part = NULLCP;

	      results->match_status = PoorComplete;
	      
	      name_comp->poor_matches = NULLEntryList;
	      name_comp->poor_match_num = 0;
	    }
	  else
	    {
	      part = ufnrec->name_parts->next;
	      
	      if (ufnrec->path != NULLEntryList && part->is_resolved != TRUE)
		if (follow_path(ufnrec) == TRUE)
		  {
		    ufnrec->results = NULLUfnResults;
		    
		    request->errors = results->errors;
		    results->errors = NULLError;

		    free((char *) results);
		    return RQ_processing;
		  }

	      results->match_status = Failed;
	      results->match_num = 0;

	      results->matches = NULLEntryList;
	      results->resolved_part = results->unresolved_part = NULLCP;
	    }
	  
	  return RQ_results_returned;
	}
      else
	{
	  if (name_comp->good_match_num == 0 &&
	      name_comp->exact_match_num == 0)
	    {
	      if (name_comp->poor_match_num > 0)
		{
		  char resolved_part[LINESIZE],
		       unresolved_part[LINESIZE],
		       buf[LINESIZE];
		  
		  part = ufnrec->name_parts->next;

		  resolved_part[0] = '\0';
		  buf[0] = '\0';

		  for (curr_part = ufnrec->name_parts;
		       curr_part != name_comp->next;
		       curr_part = curr_part->next)
		    if (curr_part->part_name != NULLCP)
		      {
			if (resolved_part[0] != '\0')
			  {
			    (void) strcpy(buf, resolved_part);
			    (void) strcpy(resolved_part, curr_part->part_name);
			    (void) strcat(resolved_part, ", ");
			    (void) strcat(resolved_part, buf);
			  }
			else
			  (void) strcpy(resolved_part, curr_part->part_name);
		      }
		  
		  unresolved_part[0] = '\0';
		  buf[0] = '\0';

		  for (; curr_part != NULLNamePart;
		       curr_part = curr_part->next)
		    if (curr_part->part_name != NULLCP)
		      {
			if (unresolved_part[0] != '\0')
			  {
			    (void) strcpy(buf, unresolved_part);
			    (void) strcpy(unresolved_part,
					  curr_part->part_name);
			    (void) strcat(unresolved_part, ", ");
			    (void) strcat(unresolved_part, buf);
			  }
			else
			  (void) strcpy(unresolved_part, curr_part->part_name);
		      }
		  
		  ufnrec->results = results;

		  results->tried_intermediate = TRUE;
		  results->match_status = PoorPartial;

		  results->match_num = name_comp->poor_match_num;
		  results->matches = name_comp->poor_matches;

		  name_comp->poor_matches = NULLEntryList;
		  name_comp->poor_match_num = 0;

		  results->errors = request->errors;
		  request->errors = NULLError;

		  results->resolved_part = copy_string(resolved_part);
		  results->unresolved_part = copy_string(unresolved_part);
	  
		  return RQ_results_returned;
		}
	      else
		{
		  part = ufnrec->name_parts->next;

		  if (ufnrec->path != NULLEntryList && part->is_resolved!=TRUE)
		    if (follow_path(ufnrec) == TRUE)
		      {
			free((char *) results);
			return RQ_processing;
		      }

		  ufnrec->results = results;

		  results->tried_intermediate = TRUE;
		  results->match_status = Failed;

		  results->match_num = 0;
		  results->matches = NULLEntryList;

		  results->unresolved_part = results->resolved_part = NULLCP;

		  results->errors = request->errors;
		  request->errors = NULLError;
		  
		  return RQ_results_returned;
		}
	    }
	  
	  name_comp->is_resolved = TRUE;
	  if (process_ufn_search(ufnrec) == QERR_nothing_found)
	    {
	      ufnrec->results = results;

	      results->tried_intermediate = TRUE;
	      results->match_status = Failed;

	      results->match_num = 0;
	      results->matches = NULLEntryList;

	      results->unresolved_part = results->resolved_part = NULLCP;

	      results->errors = request->errors;
	      request->errors = NULLError;
	      
	      return RQ_results_returned;
	    }
	  else
	    {
	      (void) free((char *) results);
	      return RQ_processing;
	    }
	}
    }

  return RQ_processing;
} /* process_ufn_ds_result */
  

/*
 * - process_ufn_ds_error() -
 *
 *
 */
request_state process_ufn_ds_error(request, task_id, error)
     requestRec request;
     int task_id;
     struct DSError *error;
{
  DsTask task_rec;
  ufnameRec ufnrec = request->UFNAME_REC;
  namePart name_comp, part;
  int *ufn_task_array;
  QBool is_exact_match_error;
  QCardinal aindex;
  QE_error_code error_type;

  task_rec = _get_task_of_id(task_id);

  /* This shouldn't happen */
  if (task_rec == NULLDsTask) return RQ_processing;

  for (name_comp = ufnrec->name_parts;
       name_comp->is_resolved == TRUE && name_comp != NULLNamePart;
       name_comp = name_comp->next) 
    ;
  
  /* This shouldn't happen! */
  if (name_comp == NULLNamePart)
    {
      _task_complete(task_id);
      return RQ_processing;
    }

  /* Check if error comes from a search using exact or approx match. */
  for (aindex = 0, ufn_task_array = ufnrec->dap_exact_task_ids;
       ufn_task_array[aindex] != task_id && aindex < MAX_TASKS_PER_REQ;
       aindex++)
    ;
  
  /* Not an exact search, so must have been an approx search. */
  if (aindex >= MAX_TASKS_PER_REQ)
    {
      for (aindex = 0, ufn_task_array = ufnrec->dap_approx_task_ids;
	   ufn_task_array[aindex] != task_id && aindex < MAX_TASKS_PER_REQ;
	   aindex++)
	;
      
      if (aindex < MAX_TASKS_PER_REQ) is_exact_match_error = FALSE;
    }
  else
    is_exact_match_error = TRUE;

  if (aindex >= MAX_TASKS_PER_REQ)
    {
      _task_complete(task_id);
      return RQ_processing;
    }


  /* Save error and log it */

  error_type = get_log_error_type(error, task_id);
  add_error_to_request_rec(request, task_rec->baseobject, error_type, error);

  ufnrec->tasks_failed++;
  

  /* Remove invocation record for this task */

  _task_complete(task_id);
  ufn_task_array[aindex] = NO_TASK;


  if (is_exact_match_error)
    ufnrec->exact_task_count--;
  else
    ufnrec->approx_task_count--;


  if (ufnrec->exact_task_count == 0 && ufnrec->approx_task_count == 0)
    {
      ufnResults results = ufn_res_alloc();

      results->tried_intermediate = FALSE;
      results->match_status = Failed;
      results->match_num = 0;
      results->matches = NULLEntryList;
      results->unresolved_part = results->resolved_part = NULLCP;
      results->errors = NULLError;
	      

      if (name_comp->exact_match_num > 0 ||
	  name_comp->good_match_num > 0)
	name_comp->is_resolved = TRUE;
      
      if (name_comp->exact_match_num == 0 &&
	  name_comp->good_match_num == 0 &&
	  name_comp->poor_match_num == 0)
	{
	  if (name_comp->is_bottom_level == unknown)
	    name_comp->is_bottom_level = might_not;

	  if (name_comp->is_bottom_level == no ||
	      process_ufn_search(ufnrec) == QERR_nothing_found)
	    {
	      part = ufnrec->name_parts->next;

	      if (ufnrec->path != NULLEntryList && part->is_resolved != TRUE)
		if (follow_path(ufnrec) == TRUE)
		  {
		    free((char *) results);
		    return RQ_processing;
		  }

	      ufnrec->results = results;

	      results->errors = request->errors;
	      request->errors = NULLError;

	      return RQ_results_returned;
	    }
	  else
	    {
	      free((char *) results);
	      return RQ_processing;
	    }
	}
      

      /*
       * If final component of name has been matched, then return
       * appropriate list of entry names.
       *
       */

      if (name_comp->next == NULLNamePart)
	{
	  ufnrec->results = results;

	  results->tried_intermediate = TRUE;
	  results->unresolved_part = results->resolved_part = NULLCP;

	  results->errors = request->errors;
	  request->errors = NULLError;

	  if (name_comp->exact_match_num > 0)
	    {
	      name_comp->is_resolved = TRUE;
      
	      results->match_num = name_comp->exact_match_num;
	      results->matches = name_comp->exact_matches;

	      name_comp->exact_matches = NULLEntryList;
	      name_comp->exact_match_num = 0;

	      results->match_status = GoodMatches;
	    }
	  else if (name_comp->good_match_num > 0)
	    {
	      name_comp->is_resolved = TRUE;
      
	      results->match_num = name_comp->good_match_num;
	      results->matches = name_comp->good_matches;

	      name_comp->good_matches = NULLEntryList;
	      name_comp->good_match_num = 0;

	      results->match_status = GoodMatches;
	    }
	  else if (name_comp->poor_match_num > 0)
	    {
	      part = ufnrec->name_parts->next;

	      results->tried_intermediate =
		(name_comp->is_bottom_level == unknown)? FALSE : TRUE;

	      results->match_num = name_comp->poor_match_num;
	      results->matches = name_comp->poor_matches;

	      name_comp->poor_matches = NULLEntryList;
	      name_comp->poor_match_num = 0;

	      results->match_status = PoorComplete;
	    }
	  else
	    {
	      part = ufnrec->name_parts->next;

	      if (ufnrec->path != NULLEntryList && part->is_resolved != TRUE)
		if (follow_path(ufnrec) == TRUE)
		  {
		    ufnrec->results = NULLUfnResults;

		    request->errors = results->errors;
		    results->errors = NULLError;

		    free((char *) results);
		    return RQ_processing;
		  }

	      results->match_num = 0;
	      results->matches = NULLEntryList;

	      results->match_status = Failed;
	    }
	  
	  return RQ_results_returned;
	}
      else
	{
	  name_comp->is_resolved = TRUE;

	  if (process_ufn_search(ufnrec) == QERR_nothing_found)
	    {
	      part = ufnrec->name_parts->next;

	      ufnrec->results = results;

	      results->tried_intermediate = TRUE;
	      results->match_status = Failed;

	      results->match_num = 0;
	      results->matches = NULLEntryList;

	      results->unresolved_part = results->resolved_part = NULLCP;
	      results->errors = request->errors;

	      request->errors = NULLError;
	      
	      return RQ_results_returned;
	    }
	  else
	    {
	      (void) free((char *) results);
	      return RQ_processing;
	    }
	}
    }

  return RQ_processing;
} /* process_ufn_ds_error */

  
/*
 *
 *
 *
 */
static QBool follow_path(ufnrec)
     ufnameRec ufnrec;
{
  namePart part;

  for (part = ufnrec->name_parts;
       part != NULLNamePart;
       part = part->next)
    {
      part->exact_match_num =
	part->good_match_num =
	  part->poor_match_num = 0;
      
      if (part->exact_matches != NULLEntryList)
	  dn_list_free(&(part->exact_matches));
      
      if (part->good_matches != NULLEntryList)
	  dn_list_free(&(part->good_matches));
      
      if (part->poor_matches != NULLEntryList)
	  dn_list_free(&(part->poor_matches));

      if (part->next != NULLNamePart)
	part->is_bottom_level = no;
      else
	part->is_bottom_level = unknown;

      part->is_resolved = FALSE;
    }
  
  ufnrec->name_parts->exact_match_num = 1;
  ufnrec->name_parts->is_resolved = TRUE;

  (void) dn_list_add(ufnrec->path->string_dn,
		     &(ufnrec->name_parts->exact_matches),
		     NULLAttrT);
  
  ufnrec->path = ufnrec->path->next;
  
  if (process_ufn_search(ufnrec) != QERR_ok)
    return FALSE;
  else
    return TRUE;
}



/*
 *
 *	Procedures for UFN search path configuration
 *
 */

void add_ufn_path_element(lower, upper, path)
     int lower, upper;
     entryList path;
{
  searchPath curr_path;

  curr_path = search_path_alloc();
  curr_path->next = ufnpaths;

  ufnpaths = curr_path;

  curr_path->upper_bound = upper;
  curr_path->lower_bound = lower;

  curr_path->path = path;
} /* add_ufn_path_element */


entryList get_ufn_path(comp_num)
     int comp_num;
{
  searchPath curr_path;
  int lower, upper;
  
  for (curr_path = ufnpaths;
       curr_path != NULLSearchPath;
       curr_path = curr_path->next)
    {
      lower = curr_path->lower_bound;
      upper = curr_path->upper_bound;

      if (comp_num > lower && (upper == UfnPlus || comp_num <= upper))
	break;

      if (comp_num == lower)
	break;
    }

  if (curr_path != NULLSearchPath)
    return curr_path->path;
  else
    return NULLEntryList;
} /* get_ufn_path */
