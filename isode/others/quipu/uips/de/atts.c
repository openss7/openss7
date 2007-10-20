/* template.c - your comments here */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/quipu/uips/de/RCS/atts.c,v 9.0 1992/06/16 12:45:59 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/quipu/uips/de/RCS/atts.c,v 9.0 1992/06/16 12:45:59 isode Rel $
 *
 *
 * $Log: atts.c,v $
 * Revision 9.0  1992/06/16  12:45:59  isode
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
#include "quipu/attrvalue.h"
#include "quipu/syntaxes.h"
#include "quipu/ds_search.h"
#include "demanifest.h"
#include "destrings.h"
#include "namelist.h"
#include "mapatts.h"

#define PADCHARS 22

extern char * TidyString();
extern char * TidyString2();
extern int soundex_match();
extern int sfree();
extern int dn_cmp();
extern int dn_free();
extern PE dn_enc();
extern DN dn_dec();
extern DN str2dnX();
extern char * octparse ();
extern char * prtparse ();
extern char * mapPhone ();
extern int ch_set;
extern int telcmp();



int greyBook = FALSE;
char padding[20];


static PS ps = NULLPS;

char *
val2str (av)
AttributeValue  av;
{
char       *cp;

  if (ps == NULL
         && ((ps = ps_alloc (str_open)) == NULLPS)
		  || str_setup (ps, NULLCP, BUFSIZ, 0) == NOTOK) {
      if (ps)
          ps_free (ps), ps = NULLPS;

      return NULLCP;
  }
  AttrV_print(ps, av, READOUT);
  ps_print (ps, " ");
  *--ps -> ps_ptr = NULL, ps -> ps_cnt++;

  cp = ps -> ps_base;

  ps -> ps_base = NULL, ps -> ps_cnt = 0;
  ps -> ps_ptr = NULL, ps -> ps_bufsiz = 0;

  return cp;
}


printDetails(objectType, lp)
int objectType;
struct namelist * lp;
{
Attr_Sequence at;
AV_Sequence av;
char * cp;

  if (lp->ats == NULLATTR)
  {
    switch (objectType)
    {
      case PERSON:
        (void) strcpy(padding, "        ");
	break;
      case ORGUNIT:
        (void) strcpy(padding, "      ");
	break;
      case ORG:
        (void) strcpy(padding, "    ");
	break;
      case COUNTRY:
        (void) strcpy(padding, "  ");
	break;
    }
    pageprint("%s(Sorry - no details available)\n", padding);
    return;
  }
  
  linewrapOn();
  for (at = lp->ats; at != NULLATTR; at = at->attr_link)
  {
    switch (objectType)
    {
      case PERSON:
        (void) strcpy(padding, "        ");
	break;
      case ORGUNIT:
        (void) strcpy(padding, "      ");
	break;
      case ORG:
        (void) strcpy(padding, "    ");
	break;
      case COUNTRY:
        (void) strcpy(padding, "  ");
	break;
    }
    pageprint("%s%-*s", padding, PADCHARS, 
    		mapAttName(attr2name(at->attr_type, OIDPART)));
    for (av = at->attr_value; av != NULLAV; av = av->avseq_next)
    {
      if (av != at->attr_value)
        pageprint("%s%*s", padding, PADCHARS, "");
      cp = val2str(&(av->avseq_av));
      pageprint("%s\n", cp);
      free(cp);
    }
  }
  linewrapOff();
}

printPersonOneLiner(lp, number)
struct namelist * lp;
int number;
{
Attr_Sequence at;
char * cp;
char mail[LINESIZE], phone[LINESIZE];

  mail[0] = phone[0] = '\0';
  pageprint("       %3d ", number);
  cp = lastComponent(lp->name, PERSON);
  pageprint(" %-19s ", cp);
  free(cp);
  for (at = lp->ats; at != NULLATTR; at = at->attr_link)
  {
    if (strcmp(attr2name(at->attr_type, OIDPART), DE_MAILBOX) == 0)
    {
      cp = val2str(&(at->attr_value->avseq_av));
      (void) strcpy(mail, cp);
      free(cp);
      continue;
    }
    if (strcmp(attr2name(at->attr_type, OIDPART), DE_TELEPHONE) == 0)
    {
      cp = val2str(&(at->attr_value->avseq_av));
      (void) strcpy(phone, cp);
      free(cp);
      continue;
    }
  }
  pageprint("%-20s  %s\n", phone, mail);
}

/* special functions for handling attributes with PostalAddress syntax */
/* actually we only want a special handler for the printing, but static
   declarations in dsap/common/post.c force us to do all this */

static de_addrfree (addr)
struct postaddr * addr;
{
	struct postaddr * next;
	for (; addr != (struct postaddr *) NULL; addr = next) {
	        next = addr->pa_next;
		free (addr->addrcomp);
		free ( (char *)addr);
	}
}

static de_addrcmp (a,b)
struct postaddr * a, *b;
{
int res;
        for (; (a != (struct postaddr *) NULL) && (b != (struct postaddr *) NULL) ;
			a = a->pa_next, b=b->pa_next) 
		if ((res = lexequ (a->addrcomp, b->addrcomp)) != 0)
			return (res);

	if ( a != b)
		return ( a > b ? 1 : -1 );
	else
		return (0);
	
}

static struct postaddr * de_addrcpy (a)
struct postaddr * a;
{
struct postaddr * b, *c, *result = (struct postaddr *) NULL;

	c = result; /* to keep lint quiet ! */

        for (; a != (struct postaddr *) NULL; a = a->pa_next) {
	        b = (struct postaddr *) smalloc (sizeof (struct postaddr));
		b -> addrtype = a->addrtype;
		b -> addrcomp = strdup (a->addrcomp);
		
		if (result == (struct postaddr *) NULL) 
			result = b;
		else 
			c->pa_next = b;
		c = b;
	}

	b->pa_next = (struct postaddr *) NULL;
	return (result);
}

static struct postaddr* de_addrparse (str)
char * str;
{
struct postaddr * result = (struct postaddr *) NULL;
struct postaddr * a, *b;
char * ptr;
char * mark = NULLCP;
char t61_str = FALSE;
extern char t61_flag;
int i;

   b = result; /* to keep lint quiet */

   if (t61_flag) {
	t61_str = TRUE;
	t61_flag = FALSE;  /* indicate recognition */
   }

   str = SkipSpace(str);

   for (i=0; i < UB_POSTAL_LINE; i++) {
	mark = NULLCP;
	a = (struct postaddr *) smalloc (sizeof (struct postaddr));

	if ( (ptr=index (str,'$')) != NULLCP) {
		*ptr-- = 0;
		if (isspace (*ptr)) {
			*ptr = 0;
			mark = ptr;
		}
		ptr++;
	}

	if (t61_str) {
		a -> addrtype = 1;
		if ((a -> addrcomp = octparse (str)) == NULLCP)
			return ((struct postaddr *)NULL);
		if ((int)strlen (a->addrcomp) > UB_POSTAL_STRING) {
			parse_error ("address string too long",NULLCP);
			return ((struct postaddr *)NULL);
		}
	} else {
		a -> addrtype = 2;
		if ((a -> addrcomp = prtparse (str)) == NULLCP)
			return ((struct postaddr *)NULL);
		if ((int)strlen (a->addrcomp) > UB_POSTAL_STRING) {
			parse_error ("address string too long",NULLCP);
			return ((struct postaddr *)NULL);
		}
	}


	if (result == (struct postaddr *) NULL) 
		result = a;
	else 
		b->pa_next = a;
	b = a;

	t61_str = FALSE;

	if (ptr != NULLCP) {
		*ptr++ = '$';
		if (mark != NULLCP)
			*mark = ' ';
		str = (SkipSpace(ptr));	
		ptr = str;

		if (*ptr++ == '{') {
			if (( str = index (ptr,'}')) == 0) {
	                        parse_error ("close bracket missing '%s'",--ptr);
        	                return ((struct postaddr *) NULL);
	                }
			*str = 0;
			if (lexequ ("T.61",ptr) != 0) {
				*str = '}';
                                parse_error ("{T.61} expected '%s'",--ptr);
                                return ((struct postaddr *) NULL);
                        }
			*str++ = '}';
			str = (SkipSpace(str));
			t61_str = TRUE;
		}
	} else
		break;
   }

   if (ptr != NULLCP) {
	parse_error ("Too many address components",NULLCP);
	return ((struct postaddr *) NULL);
   }
   a -> pa_next = (struct postaddr *) NULL ;

   return (result);
}

static PE de_addrenc (m)
struct postaddr * m;
{
PE ret_pe;

        (void) encode_SA_PostalAddress (&ret_pe,0,0,NULLCP,m);

	return (ret_pe);
}

static struct postaddr * de_addrdec (pe)
PE pe;
{
struct postaddr * m;

	if (decode_SA_PostalAddress (pe,1,NULLIP,NULLVP,&m) == NOTOK)
		return ((struct postaddr *) NULL);
	return (m);
}

extern int postal_indent;

static de_addrprint (xps,addr,format)
PS xps;
struct postaddr * addr;
int format;
{
char * prefix = NULLCP;
char prefbuff[100];

	for (; addr != (struct postaddr *) NULL; addr = addr->pa_next) 
	{
                if (prefix != NULLCP)
                        if (format != READOUT || postal_indent < 0)
                            ps_print (xps,prefix);
                        else 
			{
				ps_print (xps, "\n");
				if (postal_indent > 0)
				    ps_printf (xps, "%*s", postal_indent, "");
			}
		if (addr->addrtype == 1) 
		{
			if (format != READOUT)
			    ps_print (xps,"{T.61}");
			octprint (xps,addr->addrcomp,format);
		}
		else
			ps_print (xps,addr->addrcomp);
		if (format == READOUT) 
		{
			if (postal_indent == 0)
				postal_indent = 2;
			(void) sprintf(prefbuff, "\n%s%*s", padding, PADCHARS, "");
			prefix = prefbuff;
		}
	}
}

/* special functions for handling email addresses */

static PE de_ia5enc (x)
char *x;
{
        return (ia5s2prim(x,strlen(x)));
}


static char * de_ia5sdec (pe)
PE pe;
{
int z;

        if (test_prim_pe (pe,PE_CLASS_UNIV,PE_DEFN_IA5S))
                return (TidyString2(prim2str(pe,&z)));
        else
                return (NULLCP);
}

de_mailprint (xps,str)
PS xps;
char * str;
{
char	buf[BUFSIZ], workbuf[BUFSIZ];
char	*ptr = buf;
char	* cp, * cp2;

  if ((greyBook == TRUE) && (index(str, '@') != NULLCP))
  {
    (void) strcpy(workbuf, str);
    for (cp = workbuf; *cp != '@'; *ptr++ = *cp++) {};
    *ptr++ = *cp++;
    *ptr = '\0';
    while (TRUE)
    {
      cp2 = rindex(cp, '.');
      if (cp2 != NULLCP)
      {
        (void) strcat(buf, cp2 + 1);
	(void) strcat(buf, ".");
	*cp2 = '\0';
      }
      else
      {
        (void) strcat(buf, cp);
	break;
      }
    }
    ps_print(xps, buf);
  }
  else
    ps_print(xps, str);
}

/* modified versioin of dn_print to give appropriately formatted DNs
   the code should only alter the print format for READOUT */
   
de_dn_print (xps,dn,format)
PS   xps;
DN  dn;
int  format;
{
register DN eptr;
int pad;
char padstr[100];

	if (dn == NULLDN) {
		return ;
	}

	if (format == UFNOUT) {
	    ufn_dn_print (xps, dn, 1);
	    return;
	}

	if (format == READOUT)
	{
		pad = PADCHARS + strlen(padding) + 2;
		AttrV_print(xps, &(dn->dn_rdn->rdn_av), format);
		for (eptr = dn->dn_parent; eptr != NULLDN; eptr = eptr->dn_parent)
		{
			(void) sprintf(padstr, ",\n%*s", pad, "");
			ps_print(xps, padstr);
			AttrV_print(xps, &(eptr->dn_rdn->rdn_av), format);
			pad += 2;
		}
	}
	else
	{
		dn_comp_print (xps,dn,format);
		for (eptr = dn->dn_parent; eptr != NULLDN; eptr = eptr->dn_parent) {
			switch (format) {
				case DIROUT:  ps_print (xps,"/"); break;
				case FILEOUT:
				case RDNOUT:
				case EDBOUT:  ps_print (xps,"@"); break;
				case READOUT: ps_print (xps,"\n\t\t\t"); break;
			}
			dn_comp_print (xps,eptr,format);
		}
	}
}


de_phone_print (xps,str,format)
PS xps;
char * str;
int format;
{
  if (*str == T61_MARK) 
  {
    if (format != READOUT) 
    {
      ps_print (xps,"{T.61}");
      octprint (xps,mapPhone(++str),format);
    } 
    else 
      if (ch_set == 1)
        iso8859print(xps,mapPhone(++str));
      else
        octprint(xps,mapPhone(++str),format);
  } 
  else 
  {
    if ((*str == ' ') && (*(str+1) == 0))
      if (format == READOUT)
        ps_print (xps," (single space)");
      else
        ps_print (xps,"\\20");
    else
        ps_print (xps,mapPhone(str));
  }
}


static PE de_strenc (x)
char *x;
{
	if (*x == T61_MARK) {
		x++;
		return (t61s2prim(x,strlen(x)));
	} else
		return (prts2prim(x,strlen(x)));
}

static char * de_prtsdec (pe)
PE pe;
{
int z;
char * p, *ptr, val;

	if (test_prim_pe (pe,PE_CLASS_UNIV,PE_DEFN_PRTS)) {
		ptr = prim2str(pe,&z);
		val = *ptr;
		if (((p = TidyString2(ptr)) == NULLCP) || (*p == 0))
		    if (val == ' ') {
			    free (p);	
			    return strdup (" ");
		    } else
			    return NULLCP;

		p = TidyString2(prim2str(pe,&z));
		if (check_print_string(p))
			return (p);
		free (p);
	}
	return (NULLCP);
}


struct pair {
    char   *p_name;
    int     p_value;
};


static struct pair pairs[] = {
    "twoDimensional", 8,
    "fineResolution", 9,
    "unlimitedLength", 20,
    "b4Length", 21,
    "a3Width", 22,
    "b4Width", 23,
    "uncompressed", 30,
    NULL
};


static	de_fax_free (f)
register struct fax *f;
{
    free (f -> number);

    if (f -> bits)
	pe_free (f -> bits);

    free ((char *) f);
}

static struct fax *de_fax_cpy (a)
register struct fax *a;
{
    register struct fax *f;

    f = (struct fax *) smalloc (sizeof *f);

    f -> number = strdup (a -> number);
    f -> bits = a -> bits ? pe_cpy (a -> bits) : NULLPE;

    return f;
}

static int  de_fax_cmp (a, b)
register struct fax *a;
register struct fax *b;
{
    int	    i;

    if (a == (struct fax *) NULL)
	return (b ? -1 : 0);
    else
	if  (b == (struct fax *) NULL)
	    return 1;

    if (i = telcmp (a -> number, b -> number))
	return i;

    return pe_cmp (a -> bits, b -> bits);
}

static	de_fax_print (xps, f, format)
register PS xps;
register struct fax *f;
int	format;
{
    register int   i;
    register struct pair *p;
    register PE    pe;

    if (format == READOUT) {
	ps_printf (xps, "%s", mapPhone(f -> number));

	if ((pe = f -> bits) && (i = pe -> pe_nbits) > 0) {
	    char    *cp = " {";

	    while (i-- >= 0)
		if (bit_test (pe, i) > OK) {
		    for (p = pairs; p -> p_name; p++)
			if (p -> p_value == i)
			    break;
		    if (p -> p_name)
			ps_printf (xps, "%s %s", cp, p -> p_name);
		    else
			ps_printf (xps, "%s %d", cp, i);
		    cp = ",";
		}

	    if (*cp == ',')
		ps_print (xps, " }");
	}
    }
    else {
	ps_printf (xps, "%s", mapPhone(f -> number));

	if ((pe = f -> bits) && (i = pe -> pe_nbits) > 0) {
	    char    *cp = " $";

	    while (i-- >= 0)
		if (bit_test (pe, i) > OK) {
		    for (p = pairs; p -> p_name; p++)
			if (p -> p_value == i)
			    break;
		    if (p -> p_name)
			ps_printf (xps, "%s %s", cp, p -> p_name);
		    else
			ps_printf (xps, "%s %d", cp, i);
		    cp = "";
		}
	}
    }
}

static struct fax *de_str2fax (str)
register char  *str;
{
    int	    value;
    register char  *ptr,
		  **ap;
    char   *vec[NVEC + 1];
    register struct fax *f;
    register struct pair *p;

    f = (struct fax *) smalloc (sizeof *f);

    if (ptr = index (str, '$'))
	*ptr = NULL;
    if ((int)strlen (str) > UB_TELEPHONE_NUMBER) {
	parse_error ("fax phone number too big",NULLCP);
	free ((char *) f);
        return ((struct fax *) NULL);
    }
    f -> number = TidyString (strdup (str));
    f -> bits = NULLPE;

    if (!ptr)
	return f;

    *ptr++ = '$';
    ptr = strdup (ptr);

    bzero ((char *) vec, sizeof vec);
    (void) str2vec (ptr, vec);

    for (ap = vec; *ap; ap++) {
	if (sscanf (*ap, "%d", &value) == 1 && value >= 0)
	    goto got_value;

	for (p = pairs; p -> p_name; p++)
	    if (lexequ (p -> p_name, *ap) == 0)
		break;
	if (! p -> p_name) {
	    parse_error ("unknown G3fax non-basic parameter: '%s'", *ap);

you_lose: ;
	    free (ptr);
	    free (f -> number);
	    if (f -> bits)
		pe_free (f -> bits);
	    free ((char *) f);

	    return ((struct fax *) NULL);
	}
	value = p -> p_value;

got_value: ;
	if ((f -> bits == NULLPE
		    && (f -> bits = prim2bit (pe_alloc (PE_CLASS_UNIV,
							PE_FORM_PRIM,
							PE_PRIM_BITS)))
			    == NULLPE)
		|| bit_on (f -> bits, value) == NOTOK) {
no_allocate: ;
	    parse_error ("unable to allocate G3fax non-basic parameter",NULLCP);
	    goto you_lose;
	}
    }

    if (bit2prim (f -> bits) == NULLPE)
	goto no_allocate;

    free (ptr);

    return f;
}

static PE  de_fax_enc (f)
struct fax *f;
{
    PE	pe = NULLPE;

    f -> fax_bits = bitstr2strb (f -> bits, & f -> fax_len);

    (void) encode_SA_FacsimileTelephoneNumber (&pe, 0, 0, NULLCP, f);

    if (f -> fax_bits)
	    free (f -> fax_bits);

    return pe;
}

static struct fax *de_fax_dec (pe)
PE	pe;
{
    struct fax *f;

    if (decode_SA_FacsimileTelephoneNumber (pe, 1, NULLIP, NULLVP, &f)
	    == NOTOK) {
	return ((struct fax *) NULL);
    }

    if ( f -> fax_bits ) {
	    f -> bits = strb2bitstr ( f -> fax_bits, f -> fax_len, 
			      PE_CLASS_UNIV, PE_PRIM_BITS);

	    free ( f -> fax_bits );
    }

    return f;
}


specialSyntaxHandlers()
{
AttributeType at;

  set_attribute_syntax(str2syntax("PostalAddress"), (IFP) de_addrenc, (IFP)de_addrdec, 
                       (IFP) de_addrparse, (IFP) de_addrprint, (IFP) de_addrcpy, 
		       (IFP) de_addrcmp, (IFP) de_addrfree, NULLCP, NULLIFP, TRUE);
  set_attribute_syntax(str2syntax("DN"), (IFP) dn_enc, (IFP) dn_dec, (IFP) str2dnX,
                           de_dn_print, (IFP) dn_cpy, dn_cmp, dn_free, 
			   NULLCP, NULLIFP, TRUE );
  set_attribute_syntax(str2syntax("TelephoneNumber"), (IFP) de_strenc, 
                       (IFP) de_prtsdec, (IFP) prtparse, de_phone_print,
		       (IFP) strdup, telcmp, sfree, NULLCP, soundex_match,
  		       TRUE);
 set_attribute_syntax(str2syntax("FacsimileTelephoneNumber"), (IFP) de_fax_enc, 
                       (IFP) de_fax_dec, (IFP) de_str2fax, de_fax_print, 
		       (IFP) de_fax_cpy, de_fax_cmp, de_fax_free, NULLCP, 
		       NULLIFP, TRUE);

  /* modify syntax handler for rfc822 mail attribute 
     to allow for JNT ordering */
  at = AttrT_new (DE_MAILBOX);
  at->oa_syntax = add_attribute_syntax("elecMail", (IFP) de_ia5enc, (IFP) de_ia5sdec,
                       (IFP) octparse, de_mailprint, (IFP) strdup, lexequ,
		       sfree, NULLCP, soundex_match, TRUE);
}
