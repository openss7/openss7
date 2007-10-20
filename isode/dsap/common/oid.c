/* oid.c - Object Identifier routines */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/oid.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/dsap/common/RCS/oid.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: oid.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include "quipu/util.h"
#include "quipu/entry.h"
#include "cmd_srch.h"
#include "tailor.h"

extern char chrcnv [];

extern LLog * log_dsap;

FILE * f_table;

static char * get_entry ();
static add_entry ();
char * get_oid();
static char * name2gen ();
int add_entry_aux ();

oid_table       OIDTable [TABLESIZE];
oid_table_attr  attrOIDTable [TABLESIZE];
objectclass     ocOIDTable [TABLESIZE];


int NumEntries          = 0;
int attrNumEntries      = 0;
int ocNumEntries        = 0;

unsigned attr_index = 0;

#define GEN  1
#define ATTR 2
#define OC   3


#define	PBUCKETS	128
#define	PHASH(nm) \
    (((nm)[1]) ? (((chrcnv[((nm)[0])] - chrcnv[((nm)[1])]) & 0x1f) + ((chrcnv[(nm)[2]]) & 0x5f)) \
	       : (chrcnv[(nm)[0]]) & 0x7f)

struct pair {
    char   *p_name;
    caddr_t p_value;
    int     p_type;
    struct pair *p_chain;
};

struct aliases {
	char * a_full;
	char * a_alias;
};

static struct aliases Palias[LOTS];
static int Palias_next = 0;
static struct pair *Pbuckets[PBUCKETS];

static char allow_single_oid = FALSE;

IFP oc_load = NULLIFP;
IFP oc_macro_add = NULLIFP;

int load_oid_table (table)
char * table;
{
register char * name;
char filename [FILNSIZE];
register char * extension;
char *isodetable;

        if (NumEntries != 0)
		return OK;    /* already loaded */

	isodetable = isodefile(table, 0);

	(void) strcpy (filename,isodetable);
	extension = &filename[strlen(isodetable)];

	(void) strcpy (extension,".gen");
	if (( f_table = fopen (filename,"r")) == (FILE *) NULL) {
	    bad_file: ;
		LLOG (log_dsap, LLOG_EXCEPTIONS, ("file %s",filename));
		return NOTOK;
	}

	while (1) {     /* break out */
		if ( (name = get_entry ()) == NULLCP)
			break;
		allow_single_oid = TRUE;
		if (! add_entry (name,GEN)) {
		    bad_entry:;
			LLOG (log_dsap,LLOG_EXCEPTIONS,
			      ("Can't add entry to oid table"));
			return NOTOK;
	        }
		allow_single_oid = FALSE;
	}
	(void) fclose (f_table);

	(void) strcpy (extension,".at");
	if (( f_table = fopen (filename,"r")) == (FILE *) NULL)
		goto bad_file;

	while (1) {     /* break out */
		if ( (name = get_entry ()) == NULLCP)
			break;
		if (!add_entry (name,ATTR))
			goto bad_entry;
	}
	(void) fclose (f_table);

	(void) strcpy (extension,".oc");
	if (( f_table = fopen (filename,"r")) == (FILE *) NULL)
	    goto bad_file;

	while (1) {     /* break out */
		if ( (name = get_entry ()) == NULLCP)
			break;
		if (!add_entry (name,OC))
			goto bad_entry;
	}
	(void) fclose (f_table);
	return OK;
}

add_entry_aux (a,b,c,d)
char * a;
caddr_t b;
int c;
char * d;
{
    int	    i;
    register struct pair *p;

    for (p = Pbuckets[PHASH (a)];
	p && lexequ (p -> p_name, a);
	p = p -> p_chain) 
	    ; /* NO-OP */

    if ( p ) {
	LLOG (log_dsap,LLOG_FATAL,("duplicate OID '%s'",a));
	return FALSE;
    }

    if ((p = (struct pair *) calloc (1, sizeof *p)) == NULL) {
	SLOG (log_dsap, LLOG_EXCEPTIONS, NULLCP,
	      ("calloc of oid hash structure failed"));
	return FALSE;
    }

    p -> p_name = a;
    p -> p_value = b;
    p -> p_type = c;
    p -> p_chain = Pbuckets[i = PHASH (p -> p_name)];
    Pbuckets[i] = p;

    if (d != NULLCP) {
	for (p = Pbuckets[PHASH (d)];
	     p && lexequ (p -> p_name, d);
	     p = p -> p_chain) 
		; /* NO-OP */

	if ( p ) {
		LLOG (log_dsap,LLOG_FATAL,("duplicate alias OID '%s'",d));
		return FALSE;
	}

	if ((p = (struct pair *) calloc (1, sizeof *p)) == NULL) {
		SLOG (log_dsap, LLOG_EXCEPTIONS, NULLCP,
	      	   	   ("calloc of alias hash structure failed"));
		return FALSE;
    	}

	Palias[Palias_next].a_full = a;
	Palias[Palias_next++].a_alias = d;

    	p -> p_name = d;
    	p -> p_value = b;
    	p -> p_type = c;
    	p -> p_chain = Pbuckets[i = PHASH (p -> p_name)];
    	Pbuckets[i] = p;
    }

    return TRUE;
}


static add_entry (newname,towho)
char * newname;
int towho;
{
register char *nptr, *ptr, *sep;
OID oid;
oid_table * Current;
char * alias = NULLCP;

	if ((nptr = index (newname,SEPERATOR)) == 0) {
		LLOG (log_dsap,LLOG_FATAL,("oid missing in %s",newname));
		return FALSE;
	}
	*nptr = 0;
	if ((sep = index (newname,COMMA)) != 0) {
		*sep++ = 0;
		alias = strdup(newname);
		newname = sep;
	} 

	switch (towho) {
		case GEN:
			Current = &OIDTable[NumEntries];
			Current->ot_name = strdup(newname);
			if (!add_entry_aux (Current->ot_name,(caddr_t)&OIDTable[NumEntries],GEN,alias))
				return FALSE;
			break;
		case ATTR:
			Current = &attrOIDTable[attrNumEntries].oa_ot;
			Current->ot_name = strdup(newname);
			if (!add_entry_aux (Current->ot_name,(caddr_t)&attrOIDTable[attrNumEntries],ATTR,alias))
				return FALSE;
			break;
		case OC:
			Current = &ocOIDTable[ocNumEntries].oc_ot;
			Current->ot_name = strdup(newname);
			if (!add_entry_aux (Current->ot_name,(caddr_t)&ocOIDTable[ocNumEntries],OC,alias))
				return FALSE;
			break;
		}
	*nptr = SEPERATOR;
	nptr++;

	if ((sep = index (nptr,SEPERATOR)) != 0)
		*sep++ = 0;

	if ((ptr = index (nptr,COMMA)) != 0) {
		char * p;
		*ptr++ = 0;
		if (( p = get_oid (ptr)) == NULLCP) {
			LLOG (log_dsap,LLOG_FATAL,("invalid alias oid '%s'",ptr));
			return FALSE;
		}
		if ((oid = str2oid (p)) == NULLOID)
			Current->ot_aliasoid = NULLOID;
		else
			Current->ot_aliasoid = oid_cpy (oid);
	} 
	ptr = get_oid (nptr);
	if (ptr == NULLCP) {
		LLOG (log_dsap,LLOG_FATAL,("invalid oid '%s'",nptr));
		return FALSE;
	}
	Current->ot_stroid = strdup(ptr);

	oid = str2oid (Current->ot_stroid);
	if (oid == NULLOID)
		Current->ot_oid = NULLOID;
		/* only reason for failure is generic oid of length 1 */
	else
		Current->ot_oid = oid_cpy (oid);

	/* now do special work for at and oc types */
	switch (towho) {
		case GEN:
			NumEntries++;   /* nothing else to do */
			break;
		case ATTR:
			if (sep == 0) {
				LLOG (log_dsap,LLOG_FATAL,("syntax missing in %s",newname));
				return FALSE;
			} else {
				if (( ptr = index (sep,SEPERATOR)) != NULLCP) {
					*ptr++ = 0;
					if (lexequ (ptr,"FILE") != 0) {
						LLOG (log_dsap,LLOG_FATAL,("FILE syntax expected, '%s' found",ptr));
						return FALSE;
					} else {
						attrOIDTable[attrNumEntries].oa_syntax = str2syntax(sep);
						attrOIDTable[attrNumEntries].oa_syntax += AV_WRITE_FILE;
						attrNumEntries++;
					}
				} else {
					attrOIDTable[attrNumEntries].oa_syntax = str2syntax(sep);
					attrNumEntries++;
				}
			}
			break;
		case OC:
			if (oc_load != NULLIFP) {
				if ((*oc_load)(sep,ptr) == OK)
					ocNumEntries++;
			}
			else
				ocNumEntries++;
	}
	return TRUE;
}

char * get_oid (str)
char * str;
{
static char * buffer = NULLCP;
register char * ptr;
register char * dotptr;
register char * soid;
char deref = FALSE;
char got_dot = FALSE;

      if (buffer == NULLCP)
	      buffer = smalloc (LINESIZE);
  
      ptr = buffer;

      if ( ! isdigit(*str))
	      if ((dotptr = index (str,DOT)) == 0)
		      return (name2gen(str));

	while (*str != 0) {
	        if (*str == DOT)
		    got_dot = TRUE;

		if ( (! isdigit (*str)) && (*str != DOT) ) {
			if ((dotptr = index (str,DOT)) == 0) {
				*--ptr = 0;
				return (buffer);
			} else
				got_dot = TRUE;

			*dotptr = 0;
			*ptr = 0;
			if ((soid = name2gen(str)) == NULLCP) {
				*dotptr = DOT;
				return (NULLCP);
			}
			*dotptr = DOT;
			str = dotptr;
			if (deref) {
				if ((dotptr = rindex (soid,DOT)) == 0)
					return (NULLCP); /* invalid */
				if ((strncmp (soid,buffer,strlen(buffer))) != 0)
					return (NULLCP);  /* inconsistent */
			}
			deref = TRUE;
			(void) strcpy (buffer,soid);
			ptr = buffer + strlen (soid);
			}
		else 
			*ptr++ = *str++;
	}

	if ( !got_dot && !allow_single_oid )
		return NULLCP;

	*ptr = 0;
	return (buffer);
}

static char * name2gen (nodename)
char * nodename;
{
#ifdef NOT_ANY_MORE
register int i;
register oid_table * tblptr = &OIDTable[0];

	for (i=0;i<NumEntries;i++,tblptr++) {
		if (lexequ (tblptr->ot_name, nodename) == 0)
			return (tblptr->ot_stroid);
	}

	return (NULLCP);

#endif
struct pair *p;
oid_table * ot;

	for (p = Pbuckets[PHASH (nodename)];
		p && lexequ (p -> p_name, nodename);
     		p = p -> p_chain) 
			; /* NO-OP */

	if (p != NULL)
		switch (p -> p_type) {
		    case GEN: 
			ot = (oid_table *) p->p_value;
			return (ot->ot_stroid);
		    default: 
			return (NULLCP);
		    }

	return NULLCP;
}

static char * soid2gen (soid)
char * soid;
{
register int i;
register oid_table * tblptr = &OIDTable[0];
	for (i=0;i<NumEntries;i++,tblptr++) {
		if (strcmp (tblptr->ot_stroid, soid) == 0)
			return (tblptr->ot_name);
	}

	return (NULLCP);
}

static char * get_line ()
{
static char * buffer;
static int buflen;
register char * buf;
register char * ptr;
register int done;
register int left;
register int size;

        if (buffer == NULLCP) {
	      buffer = smalloc (LINESIZE);
	      buflen = LINESIZE;
	}

        buf = buffer, ptr = buffer;
	left = buflen;

	/* read line, ignore comments, join lines in '/' found */

	do {
		done = TRUE;
		if (fgets (buf, left, f_table) == NULLCP)
			return (NULLCP);

		StripSpace (buf);
		if (*buf != 0) {
			size = strlen(buf);
			ptr += size - 1;
			left -= size;
			if (*ptr == '\\') {
				buf = ptr;
				done = FALSE;

				if (left <= 20) {
				    buflen += LINESIZE;
				    if ((buffer = realloc(buffer, 
						  (unsigned)buflen)) == NULL)
					return (NULL);
				    size = strlen(buffer);
				    buf = buffer + size - 1;
				    left = buflen - size;
				}
			}
		} else
			done = FALSE;
	} while (done == FALSE);

	return (buffer);
}

static char * get_entry ()
{
register char * buf;
register char * ptr;

	/* read next line - test for macro defs */
	while (1) { /* return out */
		if ((buf = get_line ()) == NULLCP)
			return (NULLCP);

		if ((ptr = index (buf,'=')) != 0) {
			*ptr++ = 0;
			if (oc_macro_add != NULLIFP) {
				(*oc_macro_add) (buf,ptr);
			}
		} else
			return (buf);

	}


}

oid_table_attr *name2attr(nodename)
char * nodename;
{
register int i;
register char * ptr;
char * str;
register oid_table_attr * atrptr = &attrOIDTable[0];

	attr_index = 0;

	if ((ptr = rindex (nodename,DOT)) == 0) {
		struct pair *p;
		for (p = Pbuckets[PHASH (nodename)];
			p && lexequ (p -> p_name, nodename);
	     		p = p -> p_chain) 
				; /* NO-OP */

		if ((p != NULL) && (p->p_type == ATTR)) {
			attr_index = ((oid_table_attr *) p->p_value) - atrptr;
			return ( (oid_table_attr *) p->p_value);
		} else 
			return (NULLTABLE_ATTR);

	} else {
		if ((str = get_oid (nodename)) == NULLCP) {
			LLOG (log_dsap,LLOG_FATAL,("invalid oid '%s'",nodename));
			return (NULLTABLE_ATTR);
		}
		if (isdigit (*++ptr)) {
			for (i=0;i<attrNumEntries;i++,atrptr++)
				if (lexequ (atrptr->oa_ot.ot_stroid, str) == 0) {
					attr_index = atrptr - &attrOIDTable[0];
					return (atrptr);
				}
			return (NULLTABLE_ATTR);
		} else {
			for (i=0;i<attrNumEntries;i++,atrptr++)
				if (lexequ (atrptr->oa_ot.ot_name, ptr) == 0)
					if (strncmp (str,atrptr->oa_ot.ot_stroid,strlen(str)) == 0) {
						attr_index = atrptr - &attrOIDTable[0];
						return (atrptr);
					} else
						return (NULLTABLE_ATTR);
		}
	}
	return (NULLTABLE_ATTR);
	
}

set_heap (x)
AttributeType x;
{
	if (x == NULLTABLE_ATTR)
		return 0;

	return (attr_index = x - &attrOIDTable[0]);
		
}

oid_table_attr *oid2attr(oid)
OID oid;
{
register int i;
register oid_table_attr * ptr = &attrOIDTable[0];

	for (i=0;i<attrNumEntries;i++,ptr++) {
		if (oid_cmp (ptr->oa_ot.ot_oid, oid) == 0)
			return (ptr);
		else if (oid_cmp (ptr->oa_ot.ot_aliasoid, oid) == 0)
			return (ptr);
		
	}

	return (NULLTABLE_ATTR);
}

objectclass *name2oc(nodename)
register char * nodename;
{
register int i;
register char * ptr;
char * str;
register objectclass * oc = & ocOIDTable[0];

	if ((ptr = rindex (nodename,DOT)) == 0) {
		struct pair *p;
		for (p = Pbuckets[PHASH (nodename)];
			p && lexequ (p -> p_name, nodename);
	     		p = p -> p_chain) 
				; /* NO-OP */

		if ((p != NULL) && (p->p_type == OC))
			return ( (objectclass *) p->p_value);
		else 
			return (NULLOBJECTCLASS);
	} else {
		str = get_oid (nodename);
		if (isdigit (*++ptr)) {
			for (i=0;i<ocNumEntries;i++,oc++)
				if (lexequ (oc->oc_ot.ot_stroid, str) == 0)
				return (oc);
			return (NULLOBJECTCLASS);
		} else {
			for (i=0;i<ocNumEntries;i++,oc++)
				if (lexequ (oc->oc_ot.ot_name, ptr) == 0)
					if (strncmp (str,oc->oc_ot.ot_stroid,strlen(str)) == 0)
						return (oc);
					else
						return (NULLOBJECTCLASS);

		}
	}

	return (NULLOBJECTCLASS);
}


objectclass *oid2oc(oid)
OID oid;
{
register int i;
register objectclass * oc = &ocOIDTable[0];

	for (i=0;i<ocNumEntries;i++,oc++) {
		if (oid_cmp (oc->oc_ot.ot_oid, oid) == 0)
			return (oc);
		else if (oid_cmp (oc->oc_ot.ot_aliasoid, oid) == 0)
			return (oc);
	}

	return (NULLOBJECTCLASS);
}


static char * full_gen (ot)
oid_table * ot;
{
static char * buffer;
register char * ptr;
register char * soid;
register char * str;

        if (buffer == NULLCP)
	      buffer = smalloc (LINESIZE);

	ptr = buffer;

	buffer [0] = '\0';
	str = sprintoid (ot->ot_oid);

	soid   = str;

	while (*str != '\0')
		if ( *str == DOT)  {
			*str = '\0';
			ptr = soid2gen (soid);
			if (ptr == NULLCP) {
				*str++ = DOT;
				continue;
			}
			(void) strcat (buffer,".");
			(void) strcat (buffer,ptr);
			*str++ = DOT;
		} else
			str++;
	
	(void) strcat (buffer,".");
	(void) strcat (buffer,ot->ot_name);
	return (&buffer[1]);
}

static char * part_gen (ot)
oid_table * ot;
{
static char * buffer = NULLCP;
register char * ptr;
register char * soid;
register char * str;
char * last;

        if (buffer == NULLCP)
	      buffer = smalloc (LINESIZE);

	ptr = buffer;

	if (index (ot->ot_name,DOT) == NULLCP)
		return (ot->ot_name);

	str = sprintoid (ot->ot_oid);

	soid = str;
	last = str;

	while (*str != '\0')
		if ( *str == DOT)  {
			*str = '\0';
			ptr = soid2gen (soid);
			if (ptr == NULLCP) {
				*str++ = DOT;
				break;
			} else
				(void) strcpy (buffer,ptr);
			last = str;
			*str++ = DOT;
			
		} else
			str++;

	(void) strcat (buffer,last);
	return (buffer);
}

#ifndef attr2name_aux
char *attr2name_aux (oa)
register oid_table_attr *oa;
{
	if ( oa != NULLTABLE_ATTR)
		return (oa->oa_ot.ot_name);
	else {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("NULL table entry"));
		return (NULLCP);
	}
}
#endif

char *attr2name(oa,format)
register oid_table_attr *oa;
int format;
{
int x;

	if ( oa != NULLTABLE_ATTR)
		switch (format) {
		case OIDFULL:
			return (full_gen (&oa->oa_ot));
		case OIDNUM:
			return (oa->oa_ot.ot_stroid);
		default:
			/* look for long name */
			for (x=0; x<Palias_next; x++)
				if (lexequ (Palias[x].a_full,oa->oa_ot.ot_name) == 0)
					return (Palias[x].a_alias);
			return (part_gen (&oa->oa_ot));
		}
	else {
		LLOG (log_dsap,LLOG_EXCEPTIONS,("NULL table entry"));
		return (NULLCP);
	}
}


char *oc2name(oc,format)
register objectclass *oc;
int format;
{
	if ( oc != NULLOBJECTCLASS)
		switch (format) {
		case OIDFULL:
			return (full_gen (&oc->oc_ot));
		case OIDNUM:
			return (oc->oc_ot.ot_stroid);
		default:
			return (part_gen (&oc->oc_ot));
		}
	else
		return (NULLCP);
}

char *oid2name(oid,format)
OID oid;
int format;
{
oid_table_attr * at;
objectclass * oc;
register int i;
register oid_table* ptr = &OIDTable[0];
char * sptr;

	/* try attribute first */
	if (( at = oid2attr (oid)) != NULLTABLE_ATTR)
		return (attr2name(at,format)) ;

	/* try objectclass */
	if (( oc = oid2oc (oid)) != NULLOBJECTCLASS)
		return (oc2name(oc,format)) ;

	/* try gen tables */
	for (i=0; i<NumEntries;i++,ptr++) {
           if ((oid_cmp (ptr->ot_oid, oid) == 0) ||
       		oid_cmp (ptr->ot_aliasoid, oid) == 0) {
		switch (format) {
		case OIDFULL:
			return (full_gen (ptr));
		case OIDNUM:
			return (ptr->ot_stroid);
		default:
			return (part_gen (ptr));
		}
	   }
	}

	if ((sptr = oid2ode_aux (oid,0)) == NULLCP)
		parse_error ("Bad OID '%s'",sprintoid (oid));

	return (sptr);

}

OID name2oid (str)
register char * str;
{
register struct pair *p;
OID ptr;

	if (*str == 0)
		return NULLOID;

	for (p = Pbuckets[PHASH (str)];
		p && lexequ (p -> p_name, str);
     		p = p -> p_chain) 
			; /* NO-OP */

	if (p != NULL)
		switch (p -> p_type) {
		    case GEN: {
			oid_table * ot;
			ot = (oid_table *) p->p_value;
			return (oid_cpy(ot->ot_oid));
		    }
		    case ATTR: {
			oid_table_attr * at;
			at = (oid_table_attr *) p->p_value;
			return (oid_cpy(at->oa_ot.ot_oid)) ;
		    }
		    case OC: {
			objectclass * oc;
			oc = (objectclass *) p->p_value;
			return (oid_cpy(oc->oc_ot.ot_oid)) ;
		    }
		    default:
			if ( (ptr=ode2oid (str)) == NULLOID)
				return (oid_cpy(str2oid(str)));
			return (oid_cpy(ptr));
		}
	else {
 		/* try general oid lookup */
 		char * x;
 		if ((x=get_oid(str)) != NULLCP)
 			return (oid_cpy(str2oid(x)));
 		
		/* try isobjects */
		if ( (ptr=ode2oid (str)) == NULLOID) {
			if ((ptr=str2oid(str)) == NULLOID) {
				parse_error ("unknown OID '%s'",str);
				return NULLOID;
			}
			return (oid_cpy(ptr));
		}
		return (oid_cpy(ptr));
	}

}

PE oid2pe (o)
OID o;
{ 
/* needed cos of a macro */
	return (oid2prim (o));		
}

oidprint (ps,o,format)
PS ps;
OID o;
int format;
{
extern int oidformat;

	if (format == READOUT)
		ps_printf (ps,"%s",oid2name(o,oidformat));
	else
		ps_printf (ps,"%s",oid2name(o,OIDPART));
}

OID dup_prim2oid (pe)
PE pe;
{ 	
OID oid;

        if (! test_prim_pe (pe,PE_CLASS_UNIV,PE_PRIM_OID))
		return (NULLOID);

	if (( oid = prim2oid(pe)) == NULLOID)
		return (NULLOID);
	
	return (oid_cpy(oid));
}

void	free_oid_buckets()
{
int i;
struct pair *p, *np;
 
	for (i=0;i<PBUCKETS;i++) 
		for (p = Pbuckets[i]; p != (struct pair *)NULL ; p = np) {
			np = p -> p_chain;
			free (p->p_name);
			free ((char *)p);
		}
  
}


oid_syntax ()
{
	(void) add_attribute_syntax ("oid",
		(IFP) oid2pe,	(IFP) dup_prim2oid,
		(IFP) name2oid,	oidprint,
		(IFP) oid_cpy,	oid_cmp,
		oid_free,	NULLCP,
		NULLIFP,	FALSE );
}

