
#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/filt.c,v 9.0 1992/06/16 12:44:54 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/others/quipu/uips/pod/RCS/filt.c,v 9.0 1992/06/16 12:44:54 isode Rel $
 *
 *
 * $Log: filt.c,v $
 * Revision 9.0  1992/06/16  12:44:54  isode
 * Release 8.0
 *
 */

#include "quipu/util.h"
#include "quipu/common.h"
#include "quipu/entry.h"

#include <ctype.h>

#include "filt.h"
#include "y.tab.h"
#include "defs.h"

extern unsigned int curr_filt;
extern unsigned int filt_num;
extern unsigned int typeindx;
extern filt_struct *filt_arr[];
extern char *filtvalue[];
extern char *filttype[];
extern char *default_arr[];
char dir_error_message[STRINGLEN];

extern char mvalue[];

make_type(name_val, filt)
     char * name_val;
     filt_struct * filt;
{
  filttype[curr_filt] = (char *) malloc((unsigned int) (strlen(name_val) + 1));
  (void) strcpy(filttype[curr_filt], name_val);
  
  filt_arr[curr_filt] = filt;
}

filt_struct *make_item_filter(oid, match, value)
     char *oid;
     int match;
     char *value;
{
  register filt_struct * filt = (filt_struct *) malloc(sizeof(filt_struct));
  
  filt->flt_type = ITEM;
  filt->next = 0;
  
  filt->fu_cont.item.fi_type = match;
  filt->fu_cont.item.stroid = 
    (char *) malloc((unsigned int) (strlen(oid) + 1));
  (void) strcpy(filt->fu_cont.item.stroid, oid);
  
  if (*value == '*') filt->fu_cont.item.name = (char *) 0;
  else {
    filt->fu_cont.item.name = 
      (char *) malloc((unsigned int) (strlen(value) + 1));
    (void) strcpy(filt->fu_cont.item.name, value);
  }
  return filt;
}

filt_struct *link_filters(filt1, filt2)
     filt_struct *filt1, *filt2;
{
  filt1->next = filt2;
  return filt1;
}

filt_struct *make_parent_filter(filt_type, filt1, filt2, filt3)
     int filt_type;
     filt_struct *filt1, *filt2, *filt3;
{
  filt_struct * parent = (filt_struct *) malloc(sizeof(filt_struct));

  switch (filt_type) {

  case NOT:
    parent->flt_type = NOT;
    parent->fu_cont.sub_filt = filt1;
    parent->next = 0;
    break;

  case AND:
    parent->flt_type = AND;
    parent->fu_cont.sub_filt = filt1;
    filt1->next = filt2;
    filt2->next = filt3;
    parent->next = 0;
    break;

  default:
    parent->flt_type = OR;
    parent->fu_cont.sub_filt = filt1;
    filt1->next = filt2;
    filt2->next = filt3;
    parent->next = 0;
    break;
  }

  return parent;
}

free_filt(filt)
     filt_struct *filt;
{
  if (filt) {
    free_filt(filt->next);

    if (filt->flt_type = ITEM) {
      free(filt->fu_cont.item.stroid);
      if (filt->fu_cont.item.name) free(filt->fu_cont.item.name);
    } else
      free_filt(filt->fu_cont.sub_filt);

    free((char *) filt);
  } else
    return;
}

Filter make_attr_filter()
{
  int match_type;
  char attr_name[STRINGLEN], 
       attr_val[STRINGLEN], sub_val_initial[STRINGLEN], 
       sub_val_final[STRINGLEN], sub_val_any[STRINGLEN];
  register char *end, *start, *next;
  char save;
  short type_syntax;
  Filter sfilt = filter_alloc();

  sfilt->flt_type = FILTER_ITEM;
  sfilt->flt_next = NULLFILTER;
  sfilt->FUITEM.fi_type = FILTERITEM_EQUALITY;
  sfilt->FUITEM.UNAVA.ava_value = NULLAttrV;
  sfilt->FUITEM.UNAVA.ava_type = NULLAttrT;

  if (end = index(mvalue, '~')) match_type = APPROX;
  else if (end = index(mvalue, '*')) match_type = SUBSTRING;
  else match_type = EQUAL;

  start = mvalue;
  while (isspace(*start) && *start != '\0') start++;
  end = start;
  while (!isspace(*end) && *end != '=' && *end != '~' 
	 && *end != '*' && *end != '\0') end++;
  save = *end;
  *end = '\0';
  (void) strcpy(attr_name, start);
  *end = save;

  if (attr_name [0] == '\0') {
    (void) sprintf (dir_error_message,
	     "Error: Cannot search, invalid syntax on filter '%s'.\n",
	     mvalue);
    filter_free(sfilt);
    return NULLFILTER;
  }

  start = end + 1;
  if (match_type != SUBSTRING) {
    while (!isalnum(*start) && *start != '\0') start++;
    
    end = start;
    
    while (*end != '\0') end++;
    while (!isalnum(*end) && end > start) end--;
    if (*end != '\0') end++;
    
    save = *end;
    *end = '\0';
    (void) strcpy(attr_val, start);
    *end = save;

    if (attr_val[0] == '\0') {
      (void) sprintf (dir_error_message,
	       "Error: Cannot search, invalid syntax on filter '%s'.\n",
	       mvalue);
      filter_free(sfilt);
      return NULLFILTER;
    }
  } else {
    while (!isalnum(*start) && *start != '*' && *start != '\0') start++;
    
    if (*start == '\0') {
      (void) sprintf (dir_error_message,
	       "Error: Cannot search, invalid syntax on filter '%s'.\n",
	       mvalue);
      filter_free(sfilt);
      return NULLFILTER;
    }

    if (*start == '*') {
      sub_val_initial[0] = '*';
      ++start;
      while (isspace(*start) && *start != '\0') start++;
      
      if (*start == '\0' || !isalnum(*start)) {
	(void) sprintf (dir_error_message,
		 "Error: Cannot search, invalid syntax on filter '%s'.\n",
		 mvalue);
	filter_free(sfilt);
	return NULLFILTER;
      }
      
      end = start;
      while (isalnum(*end) && isspace(*end) && *end != '\0') end++;
      
      if (*end == '\0') {
	(void) strcpy (sub_val_final, start);
	sub_val_any[0] = '*';
      } else {
	next = end;
	
	while (*next != '*' && *next != '\0') next++;
	
	if (*next == '*') {
	  sub_val_final[0] = '*';
	  
	  save = *end;
	  *end = '\0';
	  (void) strcpy(sub_val_any, start);
	  *end = save;
	} else {
	  sub_val_any[0] = '*';
	  
	  save = *end;
	  *end = '\0';
	  (void) strcpy(sub_val_final, start);
	  *end = save;
	}
      }
    } else if (isalnum(*start)) {
      end = start;
      while (*end != '*' && *end != '\0') end++;

      end--;
      while(isspace(*end)) end--;
      end++;
      
      if (*end == '\0') {
	(void) sprintf (dir_error_message,
		 "Error: Cannot search, invalid syntax on filter '%s'.\n",
		 mvalue);
	filter_free(sfilt);
	return NULLFILTER;
      }
      
      if (index ((char *) (end + 1), '*') == NULLCP) {
	(void) sprintf (dir_error_message,
		 "Error: Cannot search, invalid syntax on filter '%s'.\n",
		 mvalue);
	filter_free(sfilt);
	return NULLFILTER;
      } else {
	save = *end;
	*end = '\0';

	(void) strcpy(sub_val_initial, start);
	sub_val_any[0] = sub_val_final[0] = '*';

	*end = save;
      }
    }
  }

  switch (match_type) {
  case APPROX:
  case EQUAL:
    sfilt->FUITEM.UNAVA.ava_type = AttrT_new(attr_name);

    if (match_type == EQUAL)
      sfilt->FUITEM.fi_type = FILTERITEM_EQUALITY;
    else
      sfilt->FUITEM.fi_type = FILTERITEM_APPROX;

    if (!sfilt->FUITEM.UNAVA.ava_type ||
	sfilt->FUITEM.UNAVA.ava_type->oa_syntax == 0) {
      (void) sprintf (dir_error_message,
	       "Error: Cannot search, invalid attribute type '%s'.\n",
	       attr_name);
      sfilt->flt_next = NULLFILTER;
      filter_free(sfilt);
      return NULLFILTER;
    }

    if ((sfilt->FUITEM.UNAVA.ava_value =
	 str2AttrV(attr_val, sfilt->FUITEM.UNAVA.ava_type->oa_syntax))
	== NULL) {

      (void) sprintf (dir_error_message,
	 "Error: Cannot search, invalid value '%s' for attribute type '%s'.\n",
	  attr_val, attr_name);

      filter_free(sfilt);
      return NULLFILTER;
    }
    return sfilt;

  case SUBSTRING:
    sfilt->FUITEM.fi_type = FILTERITEM_SUBSTRINGS;

    sfilt->FUITEM.UNSUB.fi_sub_initial = NULLAV;
    sfilt->FUITEM.UNSUB.fi_sub_final = NULLAV;
    sfilt->FUITEM.UNSUB.fi_sub_any = NULLAV;

    sfilt->FUITEM.UNSUB.fi_sub_type = AttrT_new(attr_name);

    type_syntax = sfilt->FUITEM.UNSUB.fi_sub_type->oa_syntax;

    if (type_syntax == 0) {
      (void) sprintf (dir_error_message,
	       "Error: Cannot search, invalid attribute type '%s'.\n", 
	       attr_name);
      filter_free(sfilt);
      return NULLFILTER;
    }

    if (!sub_string(type_syntax)) {
      (void) sprintf (dir_error_message,
    "Error: Cannot search, substring search unavailable for attribute '%s'.\n",
    attr_name);
      filter_free(sfilt);
      return NULLFILTER;
    }
    
    if (sub_val_initial[0] != '*')
      if ((sfilt->FUITEM.UNSUB.fi_sub_initial =
	   avs_comp_new(str2AttrV(sub_val_initial, 
				  sfilt->FUITEM.UNSUB.fi_sub_type->oa_syntax)))
	  == NULLAV) {

	(void) sprintf (dir_error_message,
	 "Error: Cannot search, invalid value '%s' for attribute type '%s'.\n",
	  sub_val_initial, attr_name);

	filter_free(sfilt);
	return NULLFILTER;
      }

    if (sub_val_any[0] != '*')
      if ((sfilt->FUITEM.UNSUB.fi_sub_any =
	   avs_comp_new(str2AttrV(sub_val_any, 
				  sfilt->FUITEM.UNSUB.fi_sub_type->oa_syntax)))
	  == NULLAV) {
	(void) sprintf (dir_error_message,
	 "Error: Cannot search, invalid value '%s' for attribute type '%s'.\n",
		 sub_val_any, attr_name);

	filter_free(sfilt);
	return NULLFILTER;
      }

    if (sub_val_final[0] != '*')
      if ((sfilt->FUITEM.UNSUB.fi_sub_final =
	   avs_comp_new(str2AttrV(sub_val_final, 
				  sfilt->FUITEM.UNSUB.fi_sub_type->oa_syntax)))
	  == NULLAV) {

	(void) sprintf (dir_error_message,
	 "Error: Cannot search, invalid value '%s' for attribute type '%s'.\n",
 	  sub_val_final, attr_name);

	filter_free(sfilt);
	return NULLFILTER;
      }
    return sfilt;
  default:
    (void) sprintf(dir_error_message, "Internal Error! Sorry.\n");
    return NULLFILTER;
  }
}

Filter make_filter(filt)
     filt_struct *filt;
{
  int type;
  char svalue[STRINGLEN];
  Filter rfilt, sfilt;
  short type_syntax;
  
  if (!filt) return NULLFILTER;

  if (index(mvalue, '=')) return make_attr_filter();

  sfilt = filter_alloc();

  switch(filt->flt_type) {
  case ITEM:
    sfilt->flt_type = FILTER_ITEM;
    sfilt->flt_next = make_filter(filt->next);

    (void) strcpy(svalue, (filt->fu_cont.item.name? 
			   filt->fu_cont.item.name:
			   mvalue));

    type = filt->fu_cont.item.fi_type;

    switch(type) {
    case APPROX:
    case EQUAL:
      sfilt->FUITEM.UNAVA.ava_type = AttrT_new(filt->fu_cont.item.stroid);

      if (!sfilt->FUITEM.UNAVA.ava_type ||
	  sfilt->FUITEM.UNAVA.ava_type->oa_syntax == 0) {

	(void) sprintf(dir_error_message, 
		       "Search Error! Invalid attribute type '%s'.\n",
		        filt->fu_cont.item.stroid);

        rfilt = sfilt->flt_next;
	sfilt->FUITEM.UNAVA.ava_value = NULLAttrV;
        sfilt->flt_next = NULLFILTER;

        filter_free(sfilt);
        return rfilt;
      }

      if ((sfilt->FUITEM.UNAVA.ava_value =
	   str2AttrV(svalue, sfilt->FUITEM.UNAVA.ava_type->oa_syntax))
	  == NULLAttrV) {

	(void) sprintf(dir_error_message, 
	   "Search Error! Invalid value '%s' for attribute type '%s'.\n",
	   svalue, filt->fu_cont.item.stroid);

	rfilt = sfilt->flt_next;
	sfilt->flt_next = NULLFILTER;

	filter_free(sfilt);
	return rfilt;
      }

      if (type == EQUAL)
	sfilt->FUITEM.fi_type = FILTERITEM_EQUALITY;
      else
	sfilt->FUITEM.fi_type = FILTERITEM_APPROX;

      break;

    case SUBSTRING:
      sfilt->FUITEM.fi_type = FILTERITEM_SUBSTRINGS;

      sfilt->FUITEM.UNSUB.fi_sub_initial = NULLAV;
      sfilt->FUITEM.UNSUB.fi_sub_final = NULLAV;
      sfilt->FUITEM.UNSUB.fi_sub_type = 
	AttrT_new(filt->fu_cont.item.stroid);
      
      type_syntax =sfilt->FUITEM.UNSUB.fi_sub_type->oa_syntax;

      if (type_syntax == 0) {
	(void) sprintf(dir_error_message, 
		       "Search Error! Invalid attribute type '%s'.\n",
		       filt->fu_cont.item.stroid);

        rfilt = sfilt->flt_next; 
	sfilt->flt_un.flt_un_item.UNSUB.fi_sub_any = NULLAV;
	sfilt->flt_next = NULLFILTER;

        filter_free(sfilt);
        return rfilt;
      }

      if (!sub_string(type_syntax)) {

	(void) sprintf(dir_error_message, 
	    "Search Error: Substring search unavailable for attribute '%s'.\n",
             filt->fu_cont.item.stroid);

        rfilt = sfilt->flt_next; 
	sfilt->FUITEM.UNSUB.fi_sub_any = NULLAV;
	sfilt->flt_next = NULLFILTER;

        filter_free(sfilt);
        return rfilt;
      }

      sfilt->FUITEM.UNSUB.fi_sub_any =
	avs_comp_new(str2AttrV(svalue,
			       sfilt->FUITEM.UNSUB.fi_sub_type->oa_syntax));

      if (sfilt->FUITEM.UNSUB.fi_sub_any == NULLAV) {

	(void) sprintf(dir_error_message, 
	   "Search Error! Invalid value '%s' for attribute type '%s'.\n",
	    svalue, filt->fu_cont.item.stroid);	

	rfilt = sfilt->flt_next;
        sfilt->flt_next = NULLFILTER;

        filter_free(sfilt);
        return rfilt;
      }

      break;
      
    default:
      sfilt->FUITEM.fi_type = FILTERITEM_APPROX;
      break;
    }
    return sfilt;
    
  case AND:
    sfilt->flt_type = FILTER_AND;
    sfilt->FUFILT = make_filter(filt->fu_cont.sub_filt);
    sfilt->flt_next = make_filter(filt->next);
    return sfilt;
    
  case OR:
    sfilt->flt_type = FILTER_OR;
    sfilt->FUFILT = make_filter(filt->fu_cont.sub_filt);
    sfilt->flt_next = make_filter(filt->next);
    return sfilt;

  case NOT:
    sfilt->flt_type = FILTER_NOT;
    sfilt->flt_next = make_filter(filt->next);
    sfilt->FUFILT = make_filter(filt->fu_cont.sub_filt);
    return sfilt;

  default:
    return NULLFILTER;
  }
}














