#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ds_util.c,v 9.0 1992/06/16 12:45:27 isode Rel $";
#endif

/* $Header: /xtel/isode/isode/others/quipu/uips/doog/query/RCS/ds_util.c,v 9.0 1992/06/16 12:45:27 isode Rel $ */

/*
 * $Log: ds_util.c,v $
 * Revision 9.0  1992/06/16  12:45:27  isode
 * Release 8.0
 *
 */

/*****************************************************************************

  ds_util.c

*****************************************************************************/

#include "ds_util.h"

#include "quipu/ds_search.h"
#include "quipu/dap2.h"
#include "quipu/name.h"

static QBool char_compare();

QCardinal match_word_limit = 3;

/*
 * - qy_dn2str(dn) -
 *
 *
 */
char *qy_dn2str(dn)
     DN dn;
{
  char *cp;
  PS ps;
  
  if (((ps = ps_alloc (str_open)) == NULLPS)
      || str_setup (ps, NULLCP, BUFSIZ, 0) == NOTOK)
    return NULLCP;

  qy_dn_print(ps, dn, READOUT);
  ps_print (ps, " ");
  *--ps->ps_ptr = NULL;
  ps -> ps_cnt++;

  cp = ps->ps_base;

  ps->ps_base = NULL;
  ps->ps_cnt = 0;

  ps_free(ps);

  return cp;
}

/*
 * - qy_dn_print() -
 *
 *
 */
void qy_dn_print(ps, dn, format)
     DN dn;
     PS ps;
     int format;
{
  register DN eptr;

  if (dn == NULLDN) {
    if (format == READOUT)
      ps_print (ps,"NULL DN");

    return ;
  }

  dn_comp_print(ps, dn, format);
  for (eptr = dn->dn_parent; eptr != NULLDN; eptr = eptr->dn_parent) {
    switch (format) {
    case DIROUT:  ps_print (ps,"/"); break;
    case FILEOUT:
    case RDNOUT:
    case EDBOUT:  ps_print (ps,"@"); break;
    case READOUT: ps_print (ps,"@"); break;
    }
    dn_comp_print (ps, eptr, format);
  }
}

/*
 * - is_good_match -
 * Find out if match_str is a good match of the rdn of dn_str.
 *
 */
QBool is_good_match(match_str, dn_str)
     char *match_str, *dn_str;
{
  char *match_from;
  register char *str1, *str2;
  QCardinal match_char_num = 0, max_char_matches = 0, words_matched = 0;

  for (str1 = dn_str; isspace(*str1); str1++)
    ;
  
  match_from = str1;

  for (str2 = match_str; isspace(*str2) && !isnull(*str2); str2++)
    ;
  
  match_str = str2;

  if (isnull(*str2))
    return FALSE;

  for (;;)
    {
      str1 = match_from;
      str2 = match_str;

      while (!char_compare(*str1, *str2))
	{
	  for (++str1; !isspace(*str1) && !isnull(*str1); str1++)
	    ;

	  for (; isspace(*str1); str1++)
	    ;

	  if (isnull(*str1))
	    return FALSE;
	}
      
      for (; !isnull(*str1) && !isnull(*str2); str1++, str2++)
	{
	  if (isspace(*str2))
	    {
	      for (++str2; isspace(*str2); str2++)
		;

	      while (!char_compare(*str1, *str2))
		{
		  for (; !isspace(*str1) && !isnull(*str1); str1++)
		    ;

		  if (isnull(*str1))
		    break;

		  for (++str1; isspace(*str1); str1++)
		    ;

		  if (isnull(*str2))
		    {
		      words_matched++;

		      if (words_matched > 1 ||
			  max_char_matches >= match_word_limit)
			return TRUE;
		      else
			return FALSE;
		    }
		  else
		    words_matched++;
		}

	      if (isnull(*str1))
		break;
	    }
	  else if (!char_compare(*str1, *str2))
	    break;
	  else
	    match_char_num++;
	}

      if (match_char_num > max_char_matches)
	max_char_matches = match_char_num;

      if (isnull(*str2))
	{
	  if (words_matched > 1 || max_char_matches >= match_word_limit)
	    return TRUE;
	  else
	    return FALSE;
	}
    
      while (!isspace(*match_from) && !isnull(*match_from))
	match_from++;

      if (isnull(*match_from))
	return FALSE;

      for (++match_from; isspace(*match_from); match_from++)
	;

      if (isnull(*match_from))
	return FALSE;
    }
} /* is_good_match */


static QBool char_compare(a, b)
     char a, b;
{
  int chr1 = a, chr2 = b;

  if (isalpha(chr1) && islower(chr1)) chr1 = toupper(chr1);
  if (isalpha(chr2) && islower(chr2)) chr2 = toupper(chr2);

  return chr1 == chr2;
}

/*
 * - make_typed_filter_items() -
 *  Make basic filters (approx and equal) for given string encoded filter item.
 *
 */
QE_error_code make_typed_filter_items(filter_str,
					     ex_filter_ptr, ap_filter_ptr)
     char *filter_str;
     Filter *ex_filter_ptr;
     Filter *ap_filter_ptr;
{
  register char *start, *end;
  char *str_attr_type, *str_attr_value;
  char save;
  AttributeType attr_type;
  QE_error_code error;
  char *TidyString();

  start = filter_str;
  while (isspace(*start)) start++;
  
  end = start;
  while(!isspace(*end) && *end != '=') end++;
  
  save = *end;
  *end = '\0';
  str_attr_type = copy_string(start);
  *end = save;

  start = end + 1;
  
  while(!isalnum(*start)) start++;
  
  str_attr_value = copy_string(start);
  str_attr_value = TidyString(str_attr_value);

  if ((attr_type = AttrT_new(str_attr_type)) == NULLAttrT) {
    free(str_attr_type);
    free(str_attr_value);
    return QERR_bad_attr_syntax;
  }

  error = make_filter_items(attr_type, str_attr_value,
			    ex_filter_ptr, ap_filter_ptr);
  free(str_attr_type);
  free(str_attr_value);

  return error;
} /* make_typed_filter_items */


/*
 * - make_filter_items() -
 * Make basic filters (approx and equal) for given attribute type and
 * string search value.
 *
 */
QE_error_code make_filter_items(attr_type, search_value,
				ex_filter_ptr, ap_filter_ptr)
     AttributeType attr_type;
     char *search_value;
     Filter *ex_filter_ptr;
     Filter *ap_filter_ptr;
{
  Filter exact_filter, approx_filter;
  AttributeValue exact_value, approx_value;

  exact_filter = filter_alloc();
  approx_filter = filter_alloc();
  
  approx_filter->flt_next = exact_filter->flt_next = NULLFILTER;
  approx_filter->flt_type = exact_filter->flt_type = FILTER_ITEM;
  
  exact_filter->FUITEM.fi_type = FILTERITEM_EQUALITY;
  approx_filter->FUITEM.fi_type = FILTERITEM_APPROX;
  
  approx_filter->FUITEM.UNAVA.ava_type =
    exact_filter->FUITEM.UNAVA.ava_type =
      attr_type;
  
  *ex_filter_ptr = exact_filter;
  *ap_filter_ptr = approx_filter;

  approx_filter->FUITEM.UNAVA.ava_value =
    exact_filter->FUITEM.UNAVA.ava_value = NULLAttrV;

  if ((approx_value = str2AttrV(search_value, attr_type->oa_syntax))
      == NULLAttrV)
    return QERR_bad_value_syntax;

  if ((exact_value = str2AttrV(search_value, attr_type->oa_syntax))
      == NULLAttrV)
    return QERR_bad_value_syntax;

  approx_filter->FUITEM.UNAVA.ava_value = approx_value;
  exact_filter->FUITEM.UNAVA.ava_value = exact_value;

  return QERR_ok;
} /* make_filter_items */

/*
 * - get_entry_type_name() - 
 * Return the attribute name of the RDN of the given entry name.
 *
 */
char *get_entry_type_name(entry_name)
  register char *entry_name;
{
  register char *start, *end;
  char *type_name;
  char save;

  end = entry_name;
  while (!isnull(*end)) end++;

  while (*end-- != '=') 
    ;

  while (isspace(*end)) end--;
  end++;

  save = *end;
  *end = '\0';

  start = end;
  while (*start != '@' && start >= entry_name) start--;
  start++;

  while (isspace(*start)) start++;

  type_name = copy_string(start);
  *end = save;

  return type_name;
} /* get_entry_type_name */

/*
 * - qy_in_hierarchy() -
 * Return TRUE if b is a subclass of a.
 *
 */
QBool qy_in_hierarchy(a, b)
     objectclass *a, *b;
{
  struct oc_seq *oidseq;

  if (a == (objectclass *) NULL || b == (objectclass *) NULL) return FALSE;

  if (a == b) return TRUE;

  for (oidseq = b->oc_hierachy; oidseq != NULLOCSEQ; oidseq = oidseq->os_next)
    if (a == oidseq->os_oc)
      return TRUE;
    else
      if (qy_in_hierarchy(a, oidseq->os_oc))
	return TRUE;

  return FALSE;
} /* qy_in_hierarchy */
