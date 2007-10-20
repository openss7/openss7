/* dsa_control.c - write-only attribute for dsa console. SPT */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/dsa_control.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/dsa_control.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: dsa_control.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
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


#include <signal.h>

#include "quipu/util.h"
/* #include "quipu/dsa_control.h"*/
#include "quipu/attr.h"
#include "quipu/attrvalue.h"
#include "psap.h"
#include <errno.h>
#ifdef  BSD42
#include <sys/wait.h>
#endif
#include "quipu/syntaxes.h"


extern struct qbuf        *str2qb() ;
extern struct qbuf        *qb_cpy() ;

static        PE          dsa_control_enc() ; 
static struct dsa_control * dsa_control_decode() ;
	      int	  dsa_control_print() ;
       struct dsa_control * dsa_control_cpy() ;
	      int	  dsa_control_cmp() ;
              void	  dsa_control_free();
       struct dsa_control * str2dsa_control() ;
static struct dsa_control * str2dsa_control_aux() ;

static        PE          quipu_call_enc() ; 
static struct quipu_call *quipu_call_decode() ;
	      int	  quipu_call_print() ;
       struct quipu_call *quipu_call_cpy() ;
	      int	  quipu_call_cmp() ;
              void	  quipu_call_free();
              void        auth_level_free() ;
       struct quipu_call *str2quipu_call() ;
static struct quipu_call *str2quipu_call_aux() ;
static        int         op_list_print() ;
static        int         ops_print() ;
       struct op_list    *op_list_cpy() ;
       struct ops        *ops_cpy() ;
       struct chain_list *chain_list_cpy() ;
       struct sub_ch_list *sub_ch_list_cpy() ;
static        int         chain_list_print() ;
static        int         sub_ch_list_print() ;

dsa_control_syntax()
{
	(void) add_attribute_syntax 
	  ("DSAControl",
	   (IFP)dsa_control_enc,    (IFP)dsa_control_decode,/* Encoder and decoder */
	   (IFP)str2dsa_control,	dsa_control_print,          /* parser, print */
	   (IFP)dsa_control_cpy,    dsa_control_cmp,/* copy and compare */
	   (IFP)dsa_control_free,	NULLCP,             /* structure_free, print - READOUT */
	   NULLIFP,	TRUE );                     /* Approx matching, multiline output */
}

static PE dsa_control_enc (control_option)
struct dsa_control * control_option;
{
PE ret_pe;

        (void) encode_Quipu_DSAControl(&ret_pe,0,0,NULLCP,control_option);
        return (ret_pe);
}

static struct dsa_control * dsa_control_decode (pe)
PE pe;
{
struct dsa_control * dsa_controlptr;

        if (decode_Quipu_DSAControl(pe,1,NULLIP,NULLVP,&dsa_controlptr) == NOTOK) {
                return ((struct dsa_control *) NULL);
        }
        return (dsa_controlptr);
}

dsa_control_print (ps, dsa_controlptr, format)
register PS ps;
register struct dsa_control * dsa_controlptr;
register int format;
{
        if (format == READOUT)
	{
		ps_printf(ps, "Write attribute only - No READ printing. SPT\n") ;
		switch (dsa_controlptr->dsa_control_option)
		{
		  case(CONTROL_LOCKDSA):
		  {
		    if (dsa_controlptr->un.lockdsa->offset == DN_PRESENT)
		    {
		      ps_printf(ps, "Locked the EDB: ") ;
		      dn_print(ps, dsa_controlptr->un.lockdsa->un.selectedDN, READOUT) ;
		    }
		    break ;
		  }
		  case(CONTROL_SETLOGLEVEL):
		  {
		    ps_printf(ps, "Set logging level to %s\n", qb2str(dsa_controlptr->un.setLogLevel)) ;
		    break ;
		  }
		  case(CONTROL_REFRESH):
		  {
		    ps_printf(ps, "Refreshed the EDB ") ;
		    dn_print(ps, dsa_controlptr->un.refresh->un.selectedDN,EDBOUT) ;
		    ps_printf(ps, ".\n") ;
		    break ;
		  }
		  case(CONTROL_STOPDSA):
		  {
		    ps_printf(ps, "Stopped the DSA.\n") ;
		    break ;
		  }
		  case(CONTROL_UNLOCK):
		  {
		    if (dsa_controlptr->un.unlock->offset == DN_PRESENT)
		    {
		      ps_printf(ps, "Unlocked the EDB: ") ;
		      dn_print(ps, dsa_controlptr->un.unlock->un.selectedDN, READOUT) ;
		    }
		    break ;
		  }
		  case(CONTROL_RESYNCH):
		  {
		    ps_printf(ps, "Resync-ed the EDB ") ;
		    dn_print(ps, dsa_controlptr->un.resynch->un.selectedDN,EDBOUT) ;
		    ps_printf(ps, ".\n") ;
		    break ;
		  }
		  case(CONTROL_CHANGETAILOR):
		  {
		    ps_printf(ps, "Changed the tailorong to %s.\n", 
			      qb2str(dsa_controlptr->un.changeTailor)) ;
		    break ;
		  }
		  case(CONTROL_UPDATESLAVEEDBS):
		  {
		    ps_printf(ps, "Updated the EDBs with arg '%s'...\n",
		    	      qb2str(dsa_controlptr->un.updateSlaveEDBs)) ;
		    break ;
		  }
		}
	}
	else 
	{
		ps_printf(ps, "Write attribute only - No EDB printing. SPT\n") ;
	}
}

static struct dsa_control * dsa_control_cpy (dsa_c_ptr)
struct dsa_control * dsa_c_ptr ;
{
	struct dsa_control * new_item = (struct dsa_control *) malloc (sizeof (struct dsa_control)) ;

	new_item->dsa_control_option = dsa_c_ptr->dsa_control_option ;

	switch (dsa_c_ptr->dsa_control_option)
	{
	case (CONTROL_LOCKDSA):
	{
	        struct optional_dn * tmp_opt_dn = (struct optional_dn *) 
		                                  malloc (sizeof (struct optional_dn)) ;
		if (dsa_c_ptr->un.lockdsa->offset == EMPTYDN)
		{
		        tmp_opt_dn->offset = EMPTYDN ;
			tmp_opt_dn->un.no_dn = NULL ;
			new_item->un.lockdsa = tmp_opt_dn ;
		}
		else
		  {
		        tmp_opt_dn->offset = DN_PRESENT ;
			tmp_opt_dn->un.selectedDN = dn_cpy(dsa_c_ptr->un.lockdsa->un.selectedDN) ;
			new_item->un.lockdsa = tmp_opt_dn ;
		}
		break ;
	}
	case (CONTROL_SETLOGLEVEL):
	{
		new_item->un.setLogLevel = qb_cpy(dsa_c_ptr->un.setLogLevel) ;
		break ;
	}
	case (CONTROL_REFRESH):
	{
	        struct optional_dn * tmp_opt_dn = (struct optional_dn *) 
		                                  malloc (sizeof (struct optional_dn)) ;
		if (dsa_c_ptr->un.refresh->offset == EMPTYDN)
		{
		        tmp_opt_dn->offset = EMPTYDN ;
			tmp_opt_dn->un.no_dn = NULL ;
			new_item->un.refresh = tmp_opt_dn ;
		}
		else
		  {
		        tmp_opt_dn->offset = DN_PRESENT ;
			tmp_opt_dn->un.selectedDN = dn_cpy(dsa_c_ptr->un.refresh->un.selectedDN) ;
			new_item->un.refresh = tmp_opt_dn ;
		}
		break ;
	}
	case (CONTROL_STOPDSA):
	{
		new_item->un.stopDSA = dsa_c_ptr->un.stopDSA ;
		break ;
	}
	case (CONTROL_UNLOCK):
	{
	        struct optional_dn * tmp_opt_dn = (struct optional_dn *) 
		                                  malloc (sizeof (struct optional_dn)) ;
		if (dsa_c_ptr->un.unlock->offset == EMPTYDN)
		{
		        tmp_opt_dn->offset = EMPTYDN ;
			tmp_opt_dn->un.no_dn = NULL ;
			new_item->un.unlock = tmp_opt_dn ;
		}
		else
		  {
		        tmp_opt_dn->offset = DN_PRESENT ;
			tmp_opt_dn->un.selectedDN = dn_cpy(dsa_c_ptr->un.unlock->un.selectedDN) ;
			new_item->un.unlock = tmp_opt_dn ;
		}
		break ;
	}
	case (CONTROL_RESYNCH):
	{
	      new_item->un.resynch = (struct optional_dn *) 
		                              malloc (sizeof (struct optional_dn)) ;
		if (dsa_c_ptr->un.resynch->offset == EMPTYDN)
		{
			new_item->un.resynch->offset = EMPTYDN ;
			new_item->un.resynch->un.no_dn = 0 ;
		}
		else
		{
			new_item->un.resynch->offset = DN_PRESENT ;
			new_item->un.resynch->un.selectedDN = 
			  dn_cpy( dsa_c_ptr->un.resynch->un.selectedDN ) ;
		}
		break ;
	}
	case (CONTROL_CHANGETAILOR):
	{
		new_item->un.changeTailor = qb_cpy(dsa_c_ptr->un.changeTailor) ;
		break ;
	}
	case (CONTROL_UPDATESLAVEEDBS):
	{
		new_item->un.updateSlaveEDBs= qb_cpy(dsa_c_ptr->un.updateSlaveEDBs) ;
		break ;
	}
	}
	return(new_item) ;
}

/* ARGSUSED */
int
dsa_control_cmp(a, b)
struct dsa_control *a, *b ;
{
	return (2) ;
}

void 
dsa_control_free(item_to_free)
struct dsa_control * item_to_free ;
{
	switch (item_to_free->dsa_control_option)
	{
	case(CONTROL_SETLOGLEVEL):
	{
		qb_free(item_to_free->un.setLogLevel) ;
		break ;
	}
	case(CONTROL_CHANGETAILOR):
	{
		qb_free(item_to_free->un.changeTailor) ;
		break ;
	}
	case(CONTROL_UPDATESLAVEEDBS):
	{
		qb_free(item_to_free->un.updateSlaveEDBs) ;
		break ;
	}
	case(CONTROL_LOCKDSA):
	{
		optional_dn_free(item_to_free->un.lockdsa) ;
		break ;
	}
	case(CONTROL_UNLOCK):
	{
		optional_dn_free(item_to_free->un.unlock) ;
		break ;
	}
	case(CONTROL_REFRESH):
	{
		optional_dn_free(item_to_free->un.refresh) ;
		break ;
	}
	case(CONTROL_RESYNCH):
	{
		optional_dn_free(item_to_free->un.resynch) ;
		break ;
	}
	}
	free ((char *)item_to_free) ;
}

struct dsa_control * str2dsa_control (str)
char * str;
{
struct dsa_control * the_item;

        the_item = (struct dsa_control *) malloc (sizeof (struct dsa_control)) ;
        if (str2dsa_control_aux(str, the_item) != (struct dsa_control *) 0 )
                return (the_item);
        free ((char *)the_item);
        return ((struct dsa_control *) 0);
}

static struct dsa_control * str2dsa_control_aux (str, item)
char * str ;
struct dsa_control * item ;
{
/*	format: number $ string */

        char * ptr_to_num, *ptr_to_string ;

	ptr_to_num = str ;
	while (!isdigit(*ptr_to_num))
		ptr_to_num++ ;
	ptr_to_string = ptr_to_num ;
	switch ((*ptr_to_num) - '0' )
	{
	  case(CONTROL_LOCKDSA):
	    {
	      struct optional_dn * tmp_opt_dn = (struct optional_dn *) 
		                              calloc (1, sizeof (struct optional_dn)) ;
	      item->dsa_control_option = CONTROL_LOCKDSA ;
	      while ((*ptr_to_string != NULL) && (*ptr_to_string != '$'))
		ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		{
		  tmp_opt_dn->offset = DN_PRESENT ;
		  tmp_opt_dn->un.selectedDN = NULLDN ;
		}
	      else
		{
		  tmp_opt_dn->offset = DN_PRESENT ;
		  if ((tmp_opt_dn->un.selectedDN = str2dn(ptr_to_string)) == NULLDN)
		    return ((struct dsa_control *) 0) ;
		}
	      item->un.lockdsa = tmp_opt_dn ;
	      break ;
	    }
	  case(CONTROL_SETLOGLEVEL):
	    {
	      item->dsa_control_option = CONTROL_SETLOGLEVEL ;
	      ptr_to_string = ptr_to_num ;
	      while ((*ptr_to_string != NULL) && (*ptr_to_string != '$'))
		ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      item->un.setLogLevel = str2qb(ptr_to_string, strlen(ptr_to_string), 1) ;
	      break ;
	    }
	  case(CONTROL_REFRESH):
	    {
	      struct optional_dn * tmp_opt_dn = (struct optional_dn *) 
		                              calloc (1, sizeof (struct optional_dn)) ;

	      item->dsa_control_option = CONTROL_REFRESH ;
	      while ((*ptr_to_string != NULL) && (*ptr_to_string != '$'))
		ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		{
		  tmp_opt_dn->offset = DN_PRESENT ;
		  tmp_opt_dn->un.selectedDN = NULLDN ;
		}
	      else
		{
		  tmp_opt_dn->offset = DN_PRESENT ;
		  if ((tmp_opt_dn->un.selectedDN = str2dn(ptr_to_string)) == NULLDN)
		    return ((struct dsa_control *) 0) ;
		}
	      item->un.refresh = tmp_opt_dn ;
	      break ;
	    }
	  case(CONTROL_STOPDSA):
	    {
	      item->dsa_control_option = CONTROL_STOPDSA ;
	      break ;
	    }
	  case(CONTROL_UNLOCK):
	    {
	      struct optional_dn * tmp_opt_dn = (struct optional_dn *) 
		                              calloc (1, sizeof (struct optional_dn)) ;
	      item->dsa_control_option = CONTROL_UNLOCK ;
	      while ((*ptr_to_string != NULL) && (*ptr_to_string != '$'))
		ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		{
		  tmp_opt_dn->offset = DN_PRESENT ;
		  tmp_opt_dn->un.selectedDN = NULLDN ;
		}
	      else
		{
		  tmp_opt_dn->offset = DN_PRESENT ;
		  if ((tmp_opt_dn->un.selectedDN = str2dn(ptr_to_string)) == NULLDN)
		    return ((struct dsa_control *) 0) ;
		}
	      item->un.unlock = tmp_opt_dn ;
	      break ;
	    }
	  case(CONTROL_RESYNCH):
	    {
	      struct optional_dn * tmp_opt_dn = (struct optional_dn *) malloc (sizeof (struct optional_dn)) ;

	      item->dsa_control_option = CONTROL_RESYNCH ;
	      while (! ((*ptr_to_string == NULL) || (*ptr_to_string == '$')))
		ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		{
		  tmp_opt_dn->offset = DN_PRESENT ;
		  tmp_opt_dn->un.selectedDN = NULLDN ;
		}
	      else
		{
		  tmp_opt_dn->offset = DN_PRESENT ;
		  tmp_opt_dn->un.selectedDN = str2dn(ptr_to_string) ;
		}
	      item->un.resynch = tmp_opt_dn ;
	      break ;
	    }
	  case(CONTROL_CHANGETAILOR):
	    {
	      item->dsa_control_option = CONTROL_CHANGETAILOR ;
	      ptr_to_string = ptr_to_num ;
	      while ((*ptr_to_string != NULL) && (*ptr_to_string != '$'))
		ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      item->un.changeTailor = str2qb(ptr_to_string, strlen(ptr_to_string), 1) ;
	      break ;
	    }
	  case(CONTROL_UPDATESLAVEEDBS):
	    {
	      item->dsa_control_option = CONTROL_UPDATESLAVEEDBS ;
	      ptr_to_string = ptr_to_num ;
	      while ((*ptr_to_string != NULL) && (*ptr_to_string != '$'))
		ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		return ((struct dsa_control *) 0) ;
	      ptr_to_string++ ;
	      if (*ptr_to_string == NULL)
		item->un.updateSlaveEDBs = str2qb("all", strlen("all"), 1) ;
	      else
		item->un.updateSlaveEDBs = str2qb(ptr_to_string, strlen(ptr_to_string), 1) ;
	      break ;
	    }
	}	    
	return (item) ;	
}

optional_dn_free(item_to_free)
struct optional_dn * item_to_free ;
{
  if (item_to_free->offset == DN_PRESENT)
    {
      dn_free(item_to_free->un.selectedDN) ;
    }
  free ((char *)item_to_free) ;
}


quipu_call_syntax()
{
	(void) add_attribute_syntax 
	  ("Call",
	   (IFP)quipu_call_enc,    (IFP)quipu_call_decode,     /* Encoder and decoder */
	   (IFP)str2quipu_call,	  quipu_call_print,      /* parser, print */
	   (IFP)quipu_call_cpy,    quipu_call_cmp,        /* copy and compare */
	   (IFP)quipu_call_free,	  NULLCP,               /* structure_free, print - READOUT */
	   NULLIFP,	          TRUE );               /* Approx matching, multiline output */
}

static PE quipu_call_enc (control_option)
struct quipu_call * control_option;
{
PE ret_pe;

        (void) encode_Quipu_Call(&ret_pe,0,0,NULLCP,control_option);
        return (ret_pe);
}

static struct quipu_call * quipu_call_decode (pe)
PE pe;
{
struct quipu_call * quipu_callptr;

        if (decode_Quipu_Call(pe,1,NULLIP,NULLVP,&quipu_callptr) == NOTOK) {
                return ((struct quipu_call *) 0);
        }
        return (quipu_callptr);
}

struct quipu_call * str2quipu_call(str)
char * str ;
{
  struct quipu_call * the_item ;

  the_item = (struct quipu_call *) malloc (sizeof (struct quipu_call)) ;
  if ((struct quipu_call *) str2quipu_call_aux(str, the_item) != (struct quipu_call *) 0 )
    return (the_item);
  free ((char *)the_item);
  return ((struct quipu_call *) 0);
}

/* ARGSUSED */
static struct quipu_call * str2quipu_call_aux(str, item)
char * str ;
struct quipu_call * item ;
{
  /* SPT: Forget it! The structure is horrible 
   and should be filled in by hand!          
   Read only structure. */

  return((struct quipu_call *) 0) ;
}

int
quipu_call_print(ps, item, format)
PS ps ;
struct quipu_call * item ;
int format ;
{
    /* When the DSA exits, we do not want to print this out */
    /* as it really is irrelevant info, not to be reloaded */
    /* upon start up. */

    if (format == EDBOUT)
    	return ;

    if (item->protocol == PROTOCOL_DAP)
    {
      ps_printf(ps, "\tProtocol = DAP\n") ;
    }
    else
    if (item->protocol == PROTOCOL_DSP)
    {
      ps_printf(ps, "\tProtocol = DSP\n") ;
    }
    else
    if (item->protocol == PROTOCOL_QUIPUDSP)
    {
      ps_printf(ps, "\tProtocol = Quipu DSP\n") ;
    }
    else
    {
      ps_printf(ps, "\tProtocol = Internet DSP\n") ;
    }

    ps_printf(ps, "\tAssociation ID = %d\n", item->assoc_id) ;

    switch (item->authorizationLevel->parm)
    {
      case(AUTHLEVEL_NONE):
      {
	ps_print(ps, "\tAuth Level = NONE\n") ;
	break ;
      }
      case(AUTHLEVEL_IDENTIFIED):
      {
	ps_print(ps, "\tAuth Level = Identified\n") ;
	break ;
      }
      case(AUTHLEVEL_SIMPLE):
      {
	ps_print(ps, "\tAuth Level = Simple\n") ;
	break ;
      }
      case(AUTHLEVEL_PROTECTED_SIMPLE):
      {
	ps_print(ps, "\tAuth Level = Protected Simple\n") ;
	break ;
      }
      case(AUTHLEVEL_STRONG):
      {
	ps_print(ps, "\tAuth Level = Strong\n") ;
	break ;
      }
    }

    if (item->initiated_by_dsa == TRUE)
    {
      ps_print(ps, "\tInitialised by DSA.\n") ;
    }
    else
    {
      ps_print(ps, "\tNot initialised by DSA.\n") ;
    }

    ps_print(ps, "\tUsers DN = ") ;
    dn_print(ps, item->usersDN, format) ;
    ps_print(ps, "\n") ;

    ps_printf(ps, "\tNet Address: %s\n", item->net_address) ;
    ps_print(ps, "\tStart: ") ;
    utcprint(ps, item->start_time, format) ;

    ps_print(ps, "   Finish: ") ;
    if (item->finish_time)
    {
      utcprint(ps, item->finish_time, format) ;
    }
    else
    {
      ps_print(ps, " <unknown>") ;
    }

    ps_print(ps, "\n\tPending Operations:\n") ;
    op_list_print(ps, item->pending_ops, format) ;

    ps_print(ps, "\tInvoked Operations:\n") ;
    op_list_print(ps, item->invoked_ops, format) ;
}

static int
op_list_print(ps, item, format)
PS ps ;
struct op_list * item ;
int format ;
{
  while (item != (struct op_list *) 0)
  {
    ps_print(ps, "\t\tOperation List:\n") ;
    ops_print(ps, item->operation_list, format) ;
    item = item->next ;
  }
}

static int
ops_print(ps, item, format)
PS ps ;
struct ops * item ;
int format ;
{
  ps_printf(ps, "\t\t   Invoke_id %d, ", item->invoke_id) ;
  ps_printf(ps, "Op Id. %d, ", item->operation_id) ;
  dn_print(ps, item->base_object, format) ;
  ps_print(ps, " Start: ") ;
  utcprint(ps, item->start_time, format) ;
  ps_print(ps, " Finish: ") ;
  if (item->finish_time)
  {
    utcprint(ps, item->finish_time, format) ;
  }
  else
  {
    ps_print(ps, " <unknown> ") ;
  }
  ps_print(ps, " Chained Ops: ") ;
  chain_list_print(ps, item->chained_ops, format) ;
  ps_print(ps, "\n") ;
}

static int
chain_list_print(ps, item, format)
PS ps ;
struct chain_list * item ;
int format ;
{
  while (item != (struct chain_list *) 0)
  {
    sub_ch_list_print(ps, item->sub_chained_ops, format) ;
    item = item->next ;
    if (item != (struct chain_list *) 0)
    {
      ps_print(ps, " $ ") ;
    }
  }
}

/* ARGSUSED */
static int
sub_ch_list_print(ps, item, format)
PS ps ;
struct sub_ch_list * item ;
int format ;
{
  ps_printf(ps, "%d, %d", item->assoc_id, item->invok_id) ;
}

struct quipu_call * quipu_call_cpy(item)
struct quipu_call * item ;
{
  struct quipu_call * tmp_item = (struct quipu_call *) 0 ;

  tmp_item = (struct quipu_call *) calloc (1, sizeof (struct quipu_call)) ;

  tmp_item->protocol = item->protocol ;
  tmp_item->assoc_id = item->assoc_id ;
  if (item->authorizationLevel != (struct auth_level *) 0)
  {
    tmp_item->authorizationLevel = (struct auth_level *) malloc (sizeof (struct auth_level)) ;
    tmp_item->authorizationLevel->parm = item->authorizationLevel->parm ;
  }

  tmp_item->initiated_by_dsa = item->initiated_by_dsa ;

  tmp_item->usersDN = dn_cpy(item->usersDN) ;
  tmp_item->net_address = strdup(item->net_address) ;
  tmp_item->start_time = strdup(item->start_time) ;
  if (item->finish_time)
    tmp_item->finish_time = strdup(item->finish_time) ;
  else
    item->finish_time = (char *) 0 ;

/*  tmp_item->net_address = strdup(item->net_address) ; */
  tmp_item->pending_ops = op_list_cpy(item->pending_ops) ;
  tmp_item->invoked_ops = op_list_cpy(item->invoked_ops) ;

  return (tmp_item) ;
}

struct op_list * op_list_cpy(item)
struct op_list * item ;
{
  struct op_list * new_item = (struct op_list *) 0 ;
  struct op_list * tmp_item = (struct op_list *) 0 ;

  while (item != (struct op_list *) 0)
  {
    tmp_item = (struct op_list *) malloc (sizeof (struct op_list)) ;
    tmp_item->operation_list = ops_cpy(item->operation_list) ;
    tmp_item->next = (struct op_list *) 0 ;

    if (new_item == (struct op_list *) 0)
    {
      new_item = tmp_item ;
    }
    else
    {
      struct op_list * tmp = new_item ;

      while (tmp->next != (struct op_list * ) 0)
      {
	tmp = tmp->next ;
      }
      tmp->next = tmp_item ;
    }
    item = item->next ;
  }
  return (new_item) ;
}

struct ops * ops_cpy(item)
struct ops * item ;
{
  struct ops * tmp_item = (struct ops *) malloc (sizeof (struct ops)) ;

  tmp_item->invoke_id = item->invoke_id ;
  tmp_item->operation_id = item->operation_id ;
  tmp_item->base_object = dn_cpy(item->base_object) ;


  if (item->start_time)
  {
    tmp_item->start_time = strdup(item->start_time) ;
  }
  else
  {
    tmp_item->start_time = (char *) 0 ;
  }
  if (item->finish_time)
  {
    tmp_item->finish_time = strdup(item->finish_time) ;
  }
  else
  {
    tmp_item->finish_time = (char *) 0 ;
  }

/*  if (item->start_time)
  {
    tmp_item->start_time = malloc (strlen(item->start_time)+1) ;
    (void) strcpy(tmp_item->start_time, item->start_time) ;
  }
  else
  {
    tmp_item->start_time = (char *) 0 ;
  }
  if (item->finish_time)
  {
    tmp_item->finish_time = malloc (strlen(item->finish_time)+1) ;
    (void) strcpy(tmp_item->finish_time, item->finish_time) ;
  }
  else
  {
    tmp_item->finish_time = (char *) 0 ;
  }
 */
  tmp_item->chained_ops = chain_list_cpy(item->chained_ops) ;
  return (tmp_item) ;
}

struct chain_list *chain_list_cpy(item)
struct chain_list * item ; 
{ 
  struct chain_list * new_item = (struct chain_list *) 0 ; 
  struct chain_list * tmp_item = (struct chain_list *) 0 ;

  while (item != (struct chain_list *) 0)
  {
    tmp_item = (struct chain_list *) malloc (sizeof (struct chain_list)) ;
    tmp_item->sub_chained_ops = sub_ch_list_cpy(item->sub_chained_ops) ;
    tmp_item->next = (struct chain_list *) 0 ;

    if (new_item == (struct chain_list *) 0)
    {
      new_item = tmp_item ;
    }
    else
    {
      struct chain_list * tmp = new_item ;

      while (tmp->next != (struct chain_list * ) 0)
      {
	tmp = tmp->next ;
      }
      tmp->next = tmp_item ;
    }

    item = item->next ;
  }
  return (new_item) ;
}

struct sub_ch_list *sub_ch_list_cpy(item)
struct sub_ch_list *item ;
{
  struct sub_ch_list *new_item = (struct sub_ch_list *) malloc (sizeof (struct sub_ch_list)) ;

  new_item->assoc_id = item->assoc_id ;
  new_item->invok_id = item->invok_id ;
  return (new_item) ;
}

/* ARGSUSED */
int
quipu_call_cmp(a, b)
struct quipu_call *a, *b ;
{
	return (2) ;
}

void
quipu_call_free(item_to_free)
struct quipu_call * item_to_free ;
{
    if (item_to_free->authorizationLevel)
      auth_level_free(item_to_free->authorizationLevel) ;

    if (item_to_free->usersDN)
      dn_free(item_to_free->usersDN) ;

    if (item_to_free->net_address)
      free((char *)item_to_free->net_address) ;

    if (item_to_free->start_time)
      free((char *)item_to_free->start_time) ;

    if (item_to_free->finish_time)
      free((char *)item_to_free->finish_time) ;
/*
    if (item_to_free->net_address)
      free((char *)item_to_free->net_address) ;

    if (item_to_free->start_time)
      free((char *)item_to_free->start_time) ;

    if (item_to_free->finish_time)
      free((char *)item_to_free->finish_time) ;
 */
    if (item_to_free->pending_ops)
      op_list_free(item_to_free->pending_ops) ;

    if (item_to_free->invoked_ops)
      op_list_free(item_to_free->invoked_ops) ;

    free((char *)item_to_free) ;
}

int
op_list_free(elem)
struct op_list * elem ;
{
  struct op_list * tmp_elem ;

  while (elem != (struct op_list *) 0)
  {
    tmp_elem = elem ;
    ops_free (tmp_elem->operation_list) ;

    elem = tmp_elem->next ;
    free ((char *)tmp_elem) ;
  }
}

int
ops_free(elem)
struct ops * elem ;
{
  dn_free(elem->base_object) ;
  if (elem->start_time)
    free((char *)elem->start_time) ;
  if (elem->finish_time)
    free((char *)elem->finish_time) ;

/*  if (elem->start_time)
    free((char *)elem->start_time) ;
  if (elem->finish_time)
    free((char *)elem->finish_time) ;
*/
  if (elem->chained_ops)
    chain_list_free(elem->chained_ops) ;
  free((char *)elem) ;
}

int
chain_list_free(elem)
struct chain_list * elem ;
{
  struct chain_list * tmp_elem ;
  
  while (elem != (struct chain_list *) 0)
  {
    free((char *)elem->sub_chained_ops) ;
    tmp_elem = elem ;
    elem = elem->next ;
    free((char *)tmp_elem) ;
  }
}

void
auth_level_free(item)
struct auth_level * item ;
{
	free((char *)item) ;
}
