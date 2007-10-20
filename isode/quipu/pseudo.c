/* pseudo.c -- Handle pseudo DSA attributes */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/pseudo.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/quipu/RCS/pseudo.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: pseudo.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include "quipu/util.h"
#include "quipu/commonarg.h"
#include "quipu/entry.h"
#include "quipu/connection.h"
#include "tailor.h"
#include <sys/stat.h>
#include <errno.h>
extern int errno;

#define cacheEDB  "0.9.2342.19200300.99.1.29"

Attr_Sequence dsa_pseudo_attr = NULLATTR;
Attr_Sequence dsa_real_attr = NULLATTR;
extern char * parse_file;
extern RDN parse_rdn;
extern LLog * log_dsap;
extern char * treedir;
struct dn_seq * dn_cached = NULLDNSEQ;
extern DN mydsadn;
#ifdef TURBO_DISK
extern Attr_Sequence fget_attributes ();
#else
extern Attr_Sequence get_attributes ();
#endif
extern Entry local_find_entry_aux();

#ifdef QUIPU_CONSOLE
/* SPT: Some imported defines. */
extern	int	local_master_size ;
extern	int	local_slave_size ;
extern	int	local_cache_size ;
extern	PS	opt ;
extern	AV_Sequence open_call_avs ;

#endif /* QUIPU_CONSOLE */


update_pseudo_attr ()
{
/*
	Called just before dsa_pseudo_attr is referenced.
	Any dynamic changes should be reflected.
*/

#ifdef QUIPU_CONSOLE

        Attr_Sequence   as ;
	AttributeValue  av ;
	AttributeType   att ;
	AV_Sequence     avs, tmp_avs ;
	char	    buffer[10] ;
	int         num_of_ops = 0 ;

/* SPT: Update the Master, slave and cache EDBs in case */
/* someone has just added an entry or two. */

/* SPT Adding Management bit for a test. */

/* Insert the open_call attribute here. */
/* Should remove the old open_calls, as it will be easier to maintain them */
/* in the open_call_avs rather than messing about in the pseudo attrs. */

#ifdef SPT_DEBUG
	fprintf(stderr, "Entering update_pseudo_attr\n") ;
#endif

	att = AttrT_new("masterEntries") ;
	if ((as = as_find_type(dsa_pseudo_attr, att)) == NULLATTR)
	{
	  av = AttrV_alloc() ;
	  sprintf(buffer, "%d", local_master_size) ;
	  if ((AttributeValue)(av = str2AttrV (buffer, att->oa_syntax)) == NULLAttrV)
	  {
	    ps_print(opt, "Darn Null attribute value.\n") ;
	  }
	  else
	  {
	    avs = avs_comp_new(av) ;
	    if (dsa_pseudo_attr == NULLATTR)
	      dsa_pseudo_attr = as_comp_new(att,avs,NULLACL_INFO) ;
	    else
	      dsa_pseudo_attr = as_merge (dsa_pseudo_attr, as_comp_new (att,avs,NULLACL_INFO)) ;
	  }
	}
	else
	{
	  *((int *)as->attr_value->avseq_av.av_struct) = local_master_size ;
	}
	AttrT_free(att) ; 


	att = AttrT_new("slaveEntries") ;
	if ((as = as_find_type(dsa_pseudo_attr, att)) == NULLATTR)
	{
	  av = AttrV_alloc() ;
	  sprintf(buffer, "%d", local_slave_size) ;
	  if ((AttributeValue)(av = str2AttrV (buffer, att->oa_syntax)) == NULLAttrV)
	  {
	    ps_print(opt, "Darn Null attribute value.\n") ;
	  }
	  else
	  {
	    avs = avs_comp_new(av) ;
	    dsa_pseudo_attr = as_merge (dsa_pseudo_attr, as_comp_new (att,avs,NULLACL_INFO)) ;
	  }
	}
	else
	{
	  *((int *)as->attr_value->avseq_av.av_struct) = local_slave_size ;
	}
	AttrT_free(att) ; 


	att = AttrT_new("cacheEntries") ;
	if ((as = as_find_type(dsa_pseudo_attr, att)) == NULLATTR)
	{
  
	  av = AttrV_alloc() ;
	  sprintf(buffer, "%d", local_cache_size) ;
	  if ((AttributeValue)(av = str2AttrV (buffer, att->oa_syntax)) == NULLAttrV)
	  {
	    ps_print(opt, "Darn Null attribute value.\n") ;
	  }
	  else
	  {
	    avs = avs_comp_new(av) ;
	    dsa_pseudo_attr = as_merge (dsa_pseudo_attr, as_comp_new (att,avs,NULLACL_INFO)) ;
	  }
	}
	else
	{
	  *((int *)as->attr_value->avseq_av.av_struct) = local_cache_size ;
	}
	AttrT_free(att) ; 

	att = AttrT_new("usageRate") ;
	tmp_avs = open_call_avs ;
	while (tmp_avs != NULLAV)
	{
	  struct op_list * tmp_op_list ;
	  time_t timenow ;

	  (void) time (&timenow) ;

	  tmp_op_list = ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->pending_ops ;
	  while(tmp_op_list != (struct op_list *) 0)
	  {
	    if ((tmp_op_list->operation_list->start_time != (char *) 0) &&
		(timenow < gtime(ut2tm(str2utct(tmp_op_list->operation_list->start_time, 
						strlen(tmp_op_list->operation_list->start_time))))+300))
	    {
	      num_of_ops++ ;
	    }
	    tmp_op_list = tmp_op_list->next ;
	  }

	  tmp_op_list = ((struct quipu_call *)tmp_avs->avseq_av.av_struct)->invoked_ops ;
	  while(tmp_op_list != (struct op_list *) 0)
	  {
	    if ((tmp_op_list->operation_list->finish_time != (char *) 0) &&
		(timenow < gtime(ut2tm(str2utct(tmp_op_list->operation_list->finish_time, 
						strlen(tmp_op_list->operation_list->finish_time))))+300))
	    {
	      num_of_ops++ ;
	    }
	    tmp_op_list = tmp_op_list->next ;
	  }

	  tmp_avs = tmp_avs->avseq_next ;
	}
	if ((as = as_find_type(dsa_pseudo_attr, att)) == NULLATTR)
	{
	  av = AttrV_alloc() ;
	  sprintf(buffer, "%d", (num_of_ops/5) ) ;
	  if ((AttributeValue)(av = str2AttrV (buffer, att->oa_syntax)) == NULLAttrV)
	  {
	    ps_print(opt, "Darn Null attribute value.\n") ;
	  }
	  else
	  {
	    avs = avs_comp_new(av) ;
	    dsa_pseudo_attr = as_merge (dsa_pseudo_attr, as_comp_new (att,avs,NULLACL_INFO)) ;
	  }
	}
	else
	{
	  *((int *)as->attr_value->avseq_av.av_struct) = (num_of_ops / 5) ;
	}
	AttrT_free(att) ; 


	att = AttrT_new("openCall") ;
	as = as_find_type(dsa_pseudo_attr, att) ;
	if (open_call_avs != (AV_Sequence) 0) 
	{
/* Strangely enough, this only ever needs to e done once. */
/* Any additions to the open_call_avs are automatically */
/* incorporated into the dsa_pseudo_attr. Very Handy! (Deliberate?) */
	  if (as == NULLATTR)
	  {
	    dsa_pseudo_attr = as_merge (dsa_pseudo_attr, 
					as_comp_new (att, open_call_avs, NULLACL_INFO)) ;
	  }
	}
	else
	{
	  /* Remove the open_call_avs if it is null */
	  /* Need to be careful here. */
#ifdef SPT_DEBUG
fprintf(stderr, "***** SPT: pseudo.c: Hacked out empty open_call_avs *******\n") ;
#endif
          if (as != NULLATTR)      /* If this attribute is in the entry then remove it */
	  {                        /* Otherwise do nothing. No value & no type so nowt to do */
	    if (as == dsa_pseudo_attr)
	    {
	      dsa_pseudo_attr = as->attr_link ;
	      AttrT_free(as->attr_type) ;
	      free((char *) as) ;
	    }
	    else                /* However, it may be questionable how this piece of code gets activated */
	    {                   /* as one must read a pseudo attr and therefore have a connection. But.. */
	      Attr_Sequence trail ;
	      for  (as=dsa_pseudo_attr; as != NULLATTR; as=as->attr_link)
	      {
		  if ((AttrT_cmp (as->attr_type, att)) == 0)
		    break;
		  trail = as;
	      }
	      trail->attr_link = as->attr_link ;
	      as->attr_link = NULLATTR ;
	      AttrT_free(as->attr_type) ;
	      free((char *) as) ;
	    }
	  }
        }
	AttrT_free(att) ;

/* Testing to see that the attributes have been incorporated */
/* properly and passed across correctly.
	as_print(opt, dsa_pseudo_attr, EDBOUT) ;
 */
#endif /* QUIPU_CONSOLE */
}

new_cacheEDB (dn)
DN dn;
{
AttributeType at;
AttributeValue av;
AV_Sequence avs;
Entry e;
DN trail;

	at = AttrT_new (cacheEDB);

	av = AttrV_alloc ();
	av -> av_syntax = str2syntax ("DN");
	av -> av_struct = (caddr_t) dn_cpy (dn);

	/* remove last component of DN */
	for (dn = (DN)av->av_struct; dn->dn_parent != NULLDN; dn = dn->dn_parent)
		trail = dn;

	dn_comp_free (dn);
	trail->dn_parent = NULLDN;
	
	avs = avs_comp_new (av);

	if (as_find_type (dsa_pseudo_attr,at) == NULLATTR)
		dsa_pseudo_attr = as_comp_new (at,avs,NULLACL_INFO);
	else
		dsa_pseudo_attr = as_merge (dsa_pseudo_attr,
			as_comp_new (at,avs,NULLACL_INFO));

	if ((e = local_find_entry_aux (mydsadn,TRUE)) != NULLENTRY)
		write_dsa_entry (e);

}

Attr_Sequence get_cacheEDB ()
{
AttributeType at;

	at = AttrT_new (cacheEDB);

	return (as_find_type (dsa_pseudo_attr,at));
}

write_dsa_entry(eptr)
Entry eptr;
{
int um;
FILE * fptr;
char filename[LINESIZE];
PS ps;
	/* write e_attributes, and preserved attributes to DSA file */

	update_pseudo_attr ();

	if (dsa_pseudo_attr) {
	       (void) sprintf (filename,"%sDSA.pseudo",isodefile(treedir,0));

	       um = umask (0177);
	       if ((fptr = fopen (filename,"w")) == (FILE *) NULL) {
		       LLOG (log_dsap,LLOG_EXCEPTIONS,("can't write DSA pseudo entry: \"%s\" (%d)",filename,errno));
	       }
	       (void) umask (um);

	       if ((ps = ps_alloc (std_open)) == NULLPS) {
		       LLOG (log_dsap,LLOG_EXCEPTIONS,("ps_alloc failed"));
		       (void) fclose (fptr);
		       return;
	       }
	       if (std_setup (ps,fptr) == NOTOK) {
		       LLOG (log_dsap,LLOG_EXCEPTIONS,("std_setup failed"));
		       (void) fclose (fptr);
		       return;
	       }

	       parse_file = filename;
	       parse_rdn = eptr->e_name;

	       as_print (ps,dsa_pseudo_attr,EDBOUT);

	       if (ps->ps_errno != PS_ERR_NONE) {
		       LLOG (log_dsap,LLOG_EXCEPTIONS,("write DSA ps error: %s",ps_error(ps->ps_errno)));
		       (void) fclose (fptr);
		       return;
	       }
	       ps_free (ps);

	       if (fflush (fptr) != 0) {
		       LLOG (log_dsap,LLOG_EXCEPTIONS,("write DSA flush error: %d",errno));
		       (void) fclose (fptr);
		       return;
	       }
#if	defined(SYS5) && !defined(SVR4)
	       sync ();
#else
	       if (fsync (fileno(fptr)) != 0) {
		       LLOG (log_dsap,LLOG_EXCEPTIONS,("write DSA fsync error: %d",errno));
		       (void) fclose (fptr);
		       return;
	       }
#endif
	       if (fclose (fptr) != 0) {
		       LLOG (log_dsap,LLOG_EXCEPTIONS,("write DSA EDB close error: %d",errno));
		       return;
	       }

	       LLOG (log_dsap,LLOG_NOTICE,("Written %s",filename));
        }

	if (eptr->e_data == E_DATA_MASTER) 
		return;

	(void) sprintf (filename,"%sDSA.real",isodefile(treedir,0));

	um = umask (0177);
	if ((fptr = fopen (filename,"w")) == (FILE *) NULL) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("can't write DSA pseudo entry: \"%s\" (%d)",filename,errno));
	}
	(void) umask (um);
	
	if ((ps = ps_alloc (std_open)) == NULLPS) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("ps_alloc failed"));
		(void) fclose (fptr);
		return;
	}
	if (std_setup (ps,fptr) == NOTOK) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("std_setup failed"));
		(void) fclose (fptr);
		return;
	}

	parse_file = filename;
	parse_rdn = eptr->e_name;

	if (dsa_real_attr)
		as_free (dsa_real_attr);
	dsa_real_attr = as_cpy (eptr->e_attributes);

	as_print (ps,dsa_real_attr,EDBOUT);

	if (ps->ps_errno != PS_ERR_NONE) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("write DSA ps error: %s",ps_error(ps->ps_errno)));
		(void) fclose (fptr);
		return;
	}
	ps_free (ps);

	if (fflush (fptr) != 0) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("write DSA flush error: %d",errno));
		(void) fclose (fptr);
		return;
	}
#if     defined(SYS5) && !defined(SVR4)
        sync ();
#else
	if (fsync (fileno(fptr)) != 0) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("write DSA fsync error: %d",errno));
		(void) fclose (fptr);
		return;
	}
#endif
	if (fclose (fptr) != 0) {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("write DSA EDB close error: %d",errno));
		return;
	}

	LLOG (log_dsap,LLOG_NOTICE,("Written %s",filename));

	
}

load_pseudo_attrs (data_type)
char data_type;
{
FILE * fptr;
char filename[LINESIZE];
DN dn;

	/* write e_attributes, and preserved attributes to DSA file */

	(void) sprintf (filename,"%sDSA.pseudo",isodefile(treedir,0));

	/* What if DSA at top level with same name as us !?! */
	parse_file = filename;
	if (mydsadn) 
		for (dn=mydsadn; dn != NULLDN; dn = dn->dn_parent)
			parse_rdn = dn->dn_rdn;

	if ((fptr = fopen (filename,"r")) == (FILE *) NULL) 
		LLOG (log_dsap,LLOG_TRACE,("No DSA pseudo entry: \"%s\" (%d)",filename,errno));
	else {

#ifdef TURBO_DISK
		if ((dsa_pseudo_attr = fget_attributes (fptr)) == NULLATTR)
#else
		if ((dsa_pseudo_attr = get_attributes (fptr)) == NULLATTR)
#endif
			LLOG (log_dsap,LLOG_TRACE,("Error in DSA pseudo entry: \"%s\" (%d)",filename,errno));

		(void) fclose (fptr);

	}

	if (data_type == E_DATA_MASTER)
		return;

	(void) sprintf (filename,"%sDSA.real",isodefile(treedir,0));

	if ((fptr = fopen (filename,"r")) == (FILE *) NULL) 
		LLOG (log_dsap,LLOG_TRACE,("No DSA real entry: \"%s\" (%d)",filename,errno)); 
	else {

#ifdef TURBO_DISK
		if ((dsa_real_attr = fget_attributes (fptr)) == NULLATTR)
#else
		if ((dsa_real_attr = get_attributes (fptr)) == NULLATTR)
#endif
			LLOG (log_dsap,LLOG_TRACE,("Error in DSA real entry: \"%s\" (%d)",filename,errno));

		(void) fclose (fptr);
	}

}
