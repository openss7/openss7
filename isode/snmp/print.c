/* print.c - MIB realization of the print group */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/snmp/RCS/print.c,v 9.0 1992/06/16 12:38:11 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/print.c,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 *
 * $Log: print.c,v $
 * Revision 9.0  1992/06/16  12:38:11  isode
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


/*    PRINT */

#include <errno.h>
#include <stdio.h>
#include <sys/signal.h>
#include "smux.h"
#include "objects.h"
#include "logger.h"


#define	generr(offset)	((offset) == type_SNMP_SMUX__PDUs_get__next__request \
				    ? NOTOK : int_SNMP_error__status_genErr)


extern	int	quantum;

void	advise ();

/*  */

#include "pathnames.h"
#include "sys.file.h"
#include "usr.dirent.h"
#include <sys/stat.h>


struct pq {
#define	PQ_SIZE	20
    unsigned int  pq_instance[PQ_SIZE];
    int		  pq_insize;

    char	 *pq_name;
    int		  pq_status;
#define	PQ_STQUEUEING	0x01
#define	PQ_STPRINTING	0x02
#define	PQ_STDAEMON	0x04
    char	  pq_display[BUFSIZ];
    int		  pq_entries;
    int		  pq_action;
#define	PQ_OTHER	1
#define	PQ_ABORT	2
#define	PQ_CLEAN	3
#define	PQ_ENABLE	4
#define	PQ_DISABLE	5
#define	PQ_RESTART	6
#define	PQ_START	7
#define	PQ_STOP		8

    char	  pq_buffer[BUFSIZ];
    char	 *pq_bp;
    char	  pq_pbuf[BUFSIZ];

    char	 *pq_SD;
    char	 *pq_LO;
    char	 *pq_ST;

    int		  pq_lastq;
    int		  pq_touched;

    struct stat	  pq_st;
};

static	struct pq *pq_head = NULL;
static	struct stat pq_st;


struct pj {
#define	PJ_SIZE	(PQ_SIZE + 1)
    unsigned int  pj_instance[PJ_SIZE];
    int		  pj_insize;

    struct pq    *pj_pq;
    char	  pj_owner[BUFSIZ];
    char	  pj_description[BUFSIZ];
    int		  pj_size;
    int		  pj_action;
#define	PJ_OTHER	1
#define	PJ_TOPQ		2
#define	PJ_REMOVE	3

    int		  pj_touched;

    char	  pj_file[MAXNAMLEN + 1];
    struct stat   pj_st;
};

static	struct pj *pj_head = NULL;


char   *pgetstr ();

/*  */

static int  pq_compar (a, b)
register struct pq *a,
		   *b;
{
    return elem_cmp (a -> pq_instance, a -> pq_insize,
		     b -> pq_instance, b -> pq_insize);
}


/* ARGSUSED */

static int  get_pq (offset)
int	offset;
{
    register int    i;
    char   *bp,
	    buffer[BUFSIZ],
	    pbuf[BUFSIZ];
    register struct pq *pq;
    struct stat st;
    static int lastq = -1;

    if (quantum == lastq)
	return OK;
    if (pq_head
	    && stat (_PATH_PRINTCAP, &st) != NOTOK
	    && pq_st.st_mtime == st.st_mtime) {
	lastq = quantum;
	return OK;
    }

    free_pq ();
    if (stat (_PATH_PRINTCAP, &st) == NOTOK) {
	advise (LLOG_EXCEPTIONS, _PATH_PRINTCAP, "unable to fstat");
	return NOTOK;
    }

    i = 1;
    bp = pbuf;
    while (getprent (buffer) > 0)
	if (!pgetstr ("rm", &bp))
	    i++;
    endprent ();

    if ((pq_head = (struct pq *) calloc ((unsigned) i, sizeof *pq_head))
	    == NULL) {
	advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	return NOTOK;
    }

    pq_st = st;	/* struct copy */
    lastq = quantum;

    for (pq = pq_head; i > 1; i--) {
	register int	j;
	register unsigned int *ip;
	register char  *cp;
	register struct pq *qp;

	if (getprent (pq -> pq_buffer) <= 0)
	    break;
	pq -> pq_bp = pq -> pq_pbuf;
	if (pgetstr ("rm", &pq -> pq_bp))
	    continue;
	if (!(pq -> pq_SD = pgetstr ("sd", &pq -> pq_bp)))
	    pq -> pq_SD = _PATH_DEFSPOOL;
	if (!(pq -> pq_LO = pgetstr ("lo", &pq -> pq_bp)))
	    pq -> pq_LO = "lock";
	if (!(pq -> pq_ST = pgetstr ("st", &pq -> pq_bp)))
	    pq -> pq_ST = "status";

	for (cp = pq -> pq_buffer; *cp; cp++)
	    if (*cp == '|' || *cp == ':') {
		*cp = NULL;
		break;
	    }
	if (cp - pq -> pq_buffer > PQ_SIZE) {
	    advise (LLOG_EXCEPTIONS, NULLCP,
		    "name of print queue (%s) too long, %d octets max",
		    pq -> pq_buffer, PQ_SIZE);
	    continue;
	}
	pq -> pq_name = pq -> pq_buffer;

	for (cp = pq -> pq_name, ip = pq -> pq_instance, j = 0; *cp; j++)
	    *ip++ = *cp++ & 0xff;
	pq -> pq_insize = j;

	pq -> pq_action = PQ_OTHER;
	pq -> pq_lastq = -1;

	for (qp = pq_head; qp < pq; qp++)
	    if (elem_cmp (pq -> pq_instance, pq -> pq_insize,
			  qp -> pq_instance, qp -> pq_insize) == 0)
		break;
	if (qp < pq) {
	    advise (LLOG_EXCEPTIONS, NULLCP, "duplicate print queue: %s",
		    qp -> pq_name);
	    pq -> pq_name = NULL;
	}
	else
	    pq++;
    }
    endprent ();

    if (pq - pq_head > 1)
	qsort ((char *) pq_head, pq - pq_head, sizeof *pq_head, pq_compar);

    return OK;
}


static	free_pq ()
{
    if (pq_head)
	free ((char *) pq_head), pq_head = NULL;
    free_pj ();
}

/*  */

static struct pq *get_pqent (ip, len, isnext)
register unsigned int *ip;
int	len;
int	isnext;
{
    register struct pq *pq;

    for (pq = pq_head; pq -> pq_name; pq++)
	switch (elem_cmp (pq -> pq_instance, pq -> pq_insize, ip, len)) {
	    case 0:
	        if (!isnext)
		    return pq;
		if ((++pq) -> pq_name == NULL)
		    return NULL;
		/* else fall... */

	    case 1:
		return (isnext ? pq : NULL);
	}

    return NULL;
}

/*  */

static int  pj_compar (a, b)
register struct pj *a,
		   *b;
{
    int	    i;

    if (i = elem_cmp (a -> pj_instance, a -> pj_insize,
		      b -> pj_instance, b -> pj_insize))
	return i;
    return (a -> pj_st.st_mtime - b -> pj_st.st_mtime);
}


static int  get_pj (offset)
int	offset;
{
    register int    i;
    register struct pj *pj;
    register struct pq *pq;
    struct stat st;
    static int lastq = -1;

    if (quantum == lastq)
	return OK;
    if (get_pq (offset) == NOTOK)
	return NOTOK;
    if (pj_head) {
	for (pq = pq_head; pq -> pq_name; pq++)
	    if (stat (pq -> pq_SD, &st) == NOTOK
	            || pq -> pq_st.st_mtime != st.st_mtime)
		break;
	if (!pq -> pq_name) {
	    lastq = quantum;
	    return OK;
	}
    }

    free_pj ();
    i = 1;
    for (pq = pq_head; pq -> pq_name; pq++) {
	struct dirent *dd;
	DIR   *dp;

	if (!(dp = opendir (pq -> pq_SD)))
	    continue;
	while (dd = readdir (dp))
	    if (dd -> d_name[0] == 'c' && dd -> d_name[1] == 'f')
		i++;
	(void) closedir (dp);
    }

    if ((pj_head = (struct pj *) calloc ((unsigned) i, sizeof *pj_head))
	    == NULL) {
	advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	return NOTOK;
    }

    lastq = quantum;

    pj = pj_head;
    for (pq = pq_head; pq -> pq_name; pq++) {
	struct dirent *dd;
	DIR   *dp;

	if (chdir (pq -> pq_SD) == NOTOK || !(dp = opendir (".")))
	    continue;
	(void) fstat (dp -> dd_fd, &pq -> pq_st);
	while (dd = readdir (dp)) {
	    register int    j;
	    register unsigned int *ip;
	    register char  *cp;

	    if (dd -> d_name[0] != 'c' || dd -> d_name[1] != 'f')
		continue;

	    if (stat (dd -> d_name, &pj -> pj_st) == NOTOK)
		continue;
	    if (i <= 1)
		break;
	    (void) strcpy (pj -> pj_file, dd -> d_name);
	    pj -> pj_pq = pq;

	    for (cp = pq -> pq_name, ip = pj -> pj_instance, j = 0; *cp; j++)
		*ip++ = *cp++ & 0xff;
	    pj -> pj_insize = j;

	    pj -> pj_action = PJ_OTHER;

	    pj++, i--;
	}
	(void) closedir (dp);
	if (i <= 1)
	    break;
    }

    if (pj - pj_head > 1)
	qsort ((char *) pj_head, pj - pj_head, sizeof *pj_head, pj_compar);
    pq = NULL;
    for (pj = pj_head; pj -> pj_pq; pj++) {
	if (pj -> pj_pq != pq) {
	    char    buffer[BUFSIZ];

	    pq = pj -> pj_pq;
	    i = 1;

	    if (chdir (pq -> pq_SD) != NOTOK
		    && findaemon (pq, buffer) > OK
		    && strcmp (pj -> pj_file, buffer) == 0)
		i = 0;
	}

	pj -> pj_instance[pj -> pj_insize++] = i++;
    }

    return OK;
}


static	free_pj ()
{
    if (pj_head)
	free ((char *) pj_head), pj_head = NULL;
}

/*  */

static struct pj *get_pjent (ip, len, isnext)
register unsigned int *ip;
int	len;
int	isnext;
{
    register struct pj *pj;

    for (pj = pj_head; pj -> pj_pq; pj++)
	switch (elem_cmp (pj -> pj_instance, pj -> pj_insize, ip, len)) {
	    case 0:
	        if (!isnext)
		    return pj;
		if ((++pj) -> pj_pq == NULL)
		    return NULL;
		/* else fall... */

	    case 1:
		return (isnext ? pj : NULL);
	}

    return NULL;
}

/*  */

#define	printQName	0
#define	printQStatus	1
#define	printQDisplay	2
#define	printQEntries	3
#define	printQAction	4


static int  o_pq (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i;
    register unsigned int *ip,
			  *jp;
    register struct pq *pq;
    register OID    oid = oi -> oi_name;
    OID	    new;
    register OT	    ot = oi -> oi_type;

    if (get_pq (offset) == NOTOK)
	return generr (offset);

    ifvar = (int) ot -> ot_info;
try_again: ;
    switch (offset) {
	case type_SNMP_SMUX__PDUs_get__request:
	    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
		return int_SNMP_error__status_noSuchName;
	    if ((pq = get_pqent (oid -> oid_elements
				     + ot -> ot_name -> oid_nelem,
				 oid -> oid_nelem
				     - ot -> ot_name -> oid_nelem, 0)) == NULL)
		return int_SNMP_error__status_noSuchName;
	    break;

	case type_SNMP_SMUX__PDUs_get__next__request:
	    if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
		if ((pq = pq_head) == NULL || pq -> pq_name == NULL)
		    return NOTOK;

		if ((new = oid_extend (oid, pq -> pq_insize)) == NULLOID)
		    return NOTOK;
		ip = new -> oid_elements + new -> oid_nelem - pq -> pq_insize;
		jp = pq -> pq_instance;
		for (i = pq -> pq_insize; i > 0; i--)
		    *ip++ = *jp++;
		
		if (v -> name)
		    free_SNMP_ObjectName (v -> name);
		v -> name = new;

		oid = new;	/* for try_again... */
	    }
	    else {
		int	j;

		if ((pq = get_pqent (oid -> oid_elements
				         + ot -> ot_name -> oid_nelem,
				     j = oid -> oid_nelem
				             - ot -> ot_name -> oid_nelem, 1))
		        == NULL)
		    return NOTOK;

		if ((i = j - pq -> pq_insize) < 0) {
		    if ((new = oid_extend (oid, -i)) == NULLOID)
			return NOTOK;
		    if (v -> name)
			free_SNMP_ObjectName (v -> name);
		    v -> name = new;

		    oid = new;
		}
		else
		    if (i > 0)
			oid -> oid_nelem -= i;

		ip = oid -> oid_elements + ot -> ot_name -> oid_nelem;
		jp = pq -> pq_instance;
		for (i = pq -> pq_insize; i > 0; i--)
		    *ip++ = *jp++;
	    }
	    break;

	default:
	    return int_SNMP_error__status_genErr;
    }

    switch (ifvar) {
	case printQName:
	case printQAction:
	    break;

	default:
	    if (quantum == pq -> pq_lastq)
		break;
	    pq -> pq_lastq = quantum;

	    {
		char   *cp;
		struct stat st;
		DIR    *dp;

		if (chdir (pq -> pq_SD) == NOTOK) {
		    advise (LLOG_EXCEPTIONS, pq -> pq_SD,
			    "unable to change directory to");
		    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
			goto try_again;
		    return int_SNMP_error__status_genErr;
		}

		pq -> pq_status = 0, pq -> pq_display[0] = NULL;
		if (stat (pq -> pq_LO, &st) != NOTOK) {
		    FILE   *fp;

		    if (!(st.st_mode & 010))
			pq -> pq_status |= PQ_STQUEUEING;
		    if (!(st.st_mode & 0100))
			pq -> pq_status |= PQ_STPRINTING;

		    if (findaemon (pq, NULLCP) > OK)
			pq -> pq_status |= PQ_STDAEMON;

		    if (fp = fopen (pq -> pq_ST, "r")) {
			char    buffer[sizeof pq -> pq_display];

			if (!fgets (buffer, sizeof buffer - 1, fp))
			    buffer[0] = NULL;
			if (cp = index (buffer, '\n'))
			    *cp = NULL;
			if (strncmp (buffer, "job: ", sizeof "job: " - 1) == 0
			        && (cp = index (buffer + sizeof "job: " - 1,
						';'))
			        && *++cp == ' '
				&& *++cp != NULL)
			    (void) strcpy (pq -> pq_display,
					   (pq -> pq_status & PQ_STDAEMON)
						    ? cp : "");
			else
			    (void) strcpy (pq -> pq_display, buffer);

			(void) fclose (fp);
		    }
		}
		else
		    pq -> pq_status |= PQ_STQUEUEING | PQ_STPRINTING;

		pq -> pq_entries = 0;
		if (dp = opendir (".")) {
		    register struct dirent *dd;
		    
		    while (dd = readdir (dp))
			if (dd -> d_name[0] == 'c' && dd -> d_name[1] == 'f')
			    pq -> pq_entries++;
		    (void) closedir (dp);
		}
	    }
	    break;
    }

    switch (ifvar) {
	case printQName:
	    return o_string (oi, v, pq -> pq_name,  strlen (pq -> pq_name));

	case printQStatus:
	    return o_integer (oi, v, pq -> pq_status);

	case printQDisplay:
	    if (pq -> pq_display[0])
		return o_string (oi, v, pq -> pq_display,
				 strlen (pq -> pq_display));
	    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
		goto try_again;
	    return NOTOK;

	case printQEntries:
	    return o_integer (oi, v, pq -> pq_entries);

	case printQAction:
	    return o_integer (oi, v, pq -> pq_action);

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

static int  s_pq (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i;
    register struct pq *pq;
    register OID    oid = oi -> oi_name;
    register OT	    ot = oi -> oi_type;
    register OS	    os = ot -> ot_syntax;
    caddr_t	value;

    if (get_pq (offset) == NOTOK)
	return generr (offset);

    ifvar = (int) ot -> ot_info;
    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
	return int_SNMP_error__status_noSuchName;
    if ((pq = get_pqent (oid -> oid_elements + ot -> ot_name -> oid_nelem,
			 oid -> oid_nelem - ot -> ot_name -> oid_nelem, 0))
	    == NULL)
	return int_SNMP_error__status_noSuchName;

    if (os == NULLOS) {
	advise (LLOG_EXCEPTIONS, NULLCP,
		"no syntax defined for object \"%s\"", ot -> ot_text);
	return int_SNMP_error__status_genErr;
    }

    switch (ifvar) {
	case printQAction:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		return int_SNMP_error__status_badValue;
	    i = *((integer *) value);
	    (*os -> os_free) (value);
	    switch (i) {
		case PQ_OTHER:
		case PQ_ABORT:
		case PQ_CLEAN:
		case PQ_ENABLE:
		case PQ_DISABLE:
		case PQ_RESTART:
		case PQ_START:
		case PQ_STOP:
			break;

		default:
		    return int_SNMP_error__status_badValue;
	    }
	    pq -> pq_touched = i;
	    return int_SNMP_error__status_noError;

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

#define	printJRank	0
#define	printJName	1
#define	printJOwner	2
#define	printJDescription 3
#define	printJSize	4
#define	printJAction 5


static int  o_pj (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i;
    register unsigned int *ip,
			  *jp;
    register struct pj *pj;
    register struct pq *pq;
    register OID    oid = oi -> oi_name;
    OID	    new;
    register OT	    ot = oi -> oi_type;

    if (get_pj (offset) == NOTOK)
	return generr (offset);

    ifvar = (int) ot -> ot_info;
try_again: ;
    switch (offset) {
	case type_SNMP_SMUX__PDUs_get__request:
	    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
		return int_SNMP_error__status_noSuchName;
	    if ((pj = get_pjent (oid -> oid_elements
				     + ot -> ot_name -> oid_nelem,
				 oid -> oid_nelem
				     - ot -> ot_name -> oid_nelem, 0)) == NULL)
		return int_SNMP_error__status_noSuchName;
	    break;

	case type_SNMP_SMUX__PDUs_get__next__request:
	    if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
		if ((pj = pj_head) == NULL || pj -> pj_pq == NULL)
		    return NOTOK;

		if ((new = oid_extend (oid, pj -> pj_insize)) == NULLOID)
		    return NOTOK;
		ip = new -> oid_elements + new -> oid_nelem - pj -> pj_insize;
		jp = pj -> pj_instance;
		for (i = pj -> pj_insize; i > 0; i--)
		    *ip++ = *jp++;
		
		if (v -> name)
		    free_SNMP_ObjectName (v -> name);
		v -> name = new;

		oid = new;	/* for try_again... */
	    }
	    else {
		int	j;

		if ((pj = get_pjent (oid -> oid_elements
				         + ot -> ot_name -> oid_nelem,
				     j = oid -> oid_nelem
				             - ot -> ot_name -> oid_nelem, 1))
		        == NULL)
		    return NOTOK;

		if ((i = j - pj -> pj_insize) < 0) {
		    if ((new = oid_extend (oid, -i)) == NULLOID)
			return NOTOK;
		    if (v -> name)
			free_SNMP_ObjectName (v -> name);
		    v -> name = new;

		    oid = new;
		}
		else
		    if (i > 0)
			oid -> oid_nelem -= i;

		ip = oid -> oid_elements + ot -> ot_name -> oid_nelem;
		jp = pj -> pj_instance;
		for (i = pj -> pj_insize; i > 0; i--)
		    *ip++ = *jp++;
	    }
	    break;

	default:
	    return int_SNMP_error__status_genErr;
    }

    pq = pj -> pj_pq;
    switch (ifvar) {
	case printJRank:
	case printJName:
	case printJAction:
	    break;

	default:
	    if (!pj -> pj_owner[0]) {
		char    buffer[BUFSIZ],
			host[BUFSIZ];
		FILE   *fp;

		if (chdir (pq -> pq_SD) == NOTOK) {
		    advise (LLOG_EXCEPTIONS, pq -> pq_SD,
			    "unable to change directory to");
		    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
			goto try_again;
		    return int_SNMP_error__status_genErr;
		}

		if (!(fp = fopen (pj -> pj_file, "r"))) {
		    advise (LLOG_EXCEPTIONS, pj -> pj_file, "unable to read");
		    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
			goto try_again;
		    return int_SNMP_error__status_genErr;
		}

		pj -> pj_size = 0, host[0] = NULL;

		while (fgets (buffer, sizeof buffer - 1, fp)) {
		    char   *cp;
		    struct stat st;

		    if (cp = index (buffer, '\n'))
			*cp = NULL;
		    switch (buffer[0]) {
			case 'P':
			    if (!pj -> pj_owner[0])
				(void) strcpy (pj -> pj_owner, buffer + 1);
			    break;

			case 'H':
			    if (!host[0])
				(void) strcpy (host, buffer + 1);
			    break;

			case 'J':
			    if (!pj -> pj_description[0])
				(void) strcpy (pj -> pj_description,
					       buffer + 1);
			    break;

			default:
			    if (buffer[0] >= 'a'
				    && buffer[0] <= 'z'
				    && stat (buffer + 1, &st) != NOTOK)
				pj -> pj_size += st.st_size;
			    break;
		    }
		}
		if (host[0])
		    (void) sprintf (pj -> pj_owner + strlen (pj -> pj_owner),
				    "@%s", host);

		(void) fclose (fp);		
	    }
	    break;
    }

    switch (ifvar) {
	case printJRank:
	    return o_integer (oi, v, pj -> pj_instance[pj -> pj_insize - 1]);

	case printJName:
	    return o_string (oi, v, pj -> pj_file, strlen (pj -> pj_file));

	case printJOwner:
	    if (pj -> pj_owner[0])
		return o_string (oi, v, pj -> pj_owner,
				 strlen (pj -> pj_owner));
	    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
		goto try_again;
	    return NOTOK;

	case printJDescription:
	    if (pj -> pj_description[0])
		return o_string (oi, v, pj -> pj_description,
				 strlen (pj -> pj_description));
	    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
		goto try_again;
	    return NOTOK;

	case printJSize:
	    if (pj -> pj_size > 0)
		return o_integer (oi, v, pj -> pj_size);

	case printJAction:
	    return o_integer (oi, v, pj -> pj_action);

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

static int  s_pj (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i;
    register struct pj *pj;
    register OID    oid = oi -> oi_name;
    register OT	    ot = oi -> oi_type;
    register OS	    os = ot -> ot_syntax;
    caddr_t	value;

    if (get_pj (offset) == NOTOK)
	return generr (offset);

    ifvar = (int) ot -> ot_info;
    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
	return int_SNMP_error__status_noSuchName;
    if ((pj = get_pjent (oid -> oid_elements + ot -> ot_name -> oid_nelem,
			 oid -> oid_nelem - ot -> ot_name -> oid_nelem, 0))
	    == NULL)
	return int_SNMP_error__status_noSuchName;

    if (os == NULLOS) {
	advise (LLOG_EXCEPTIONS, NULLCP,
		"no syntax defined for object \"%s\"", ot -> ot_text);
	return int_SNMP_error__status_genErr;
    }

    switch (ifvar) {
	case printJAction:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		return int_SNMP_error__status_badValue;
	    i = *((integer *) value);
	    (*os -> os_free) (value);
	    switch (i) {
		case PJ_OTHER:
		case PJ_TOPQ:
		case PJ_REMOVE:
			break;

		default:
		    return int_SNMP_error__status_badValue;
	    }
	    pj -> pj_touched = i;
	    return int_SNMP_error__status_noError;

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

int	init_print () {
    register OT	    ot;

    if (ot = text2obj ("printQName"))
	ot -> ot_getfnx = o_pq,
	ot -> ot_info = (caddr_t) printQName;
    if (ot = text2obj ("printQStatus"))
	ot -> ot_getfnx = o_pq,
	ot -> ot_info = (caddr_t) printQStatus;
    if (ot = text2obj ("printQDisplay"))
	ot -> ot_getfnx = o_pq,
	ot -> ot_info = (caddr_t) printQDisplay;
    if (ot = text2obj ("printQEntries"))
	ot -> ot_getfnx = o_pq,
	ot -> ot_info = (caddr_t) printQEntries;
    if (ot = text2obj ("printQAction"))
	ot -> ot_getfnx = o_pq,
	ot -> ot_setfnx = s_pq,
	ot -> ot_info = (caddr_t) printQAction;

    if (ot = text2obj ("printJRank"))
	ot -> ot_getfnx = o_pj,
	ot -> ot_info = (caddr_t) printJRank;
    if (ot = text2obj ("printJName"))
	ot -> ot_getfnx = o_pj,
	ot -> ot_info = (caddr_t) printJName;
    if (ot = text2obj ("printJOwner"))
	ot -> ot_getfnx = o_pj,
	ot -> ot_info = (caddr_t) printJOwner;
    if (ot = text2obj ("printJDescription"))
	ot -> ot_getfnx = o_pj,
	ot -> ot_info = (caddr_t) printJDescription;
    if (ot = text2obj ("printJSize"))
	ot -> ot_getfnx = o_pj,
	ot -> ot_info = (caddr_t) printJSize;
    if (ot = text2obj ("printJAction"))
	ot -> ot_getfnx = o_pj,
	ot -> ot_setfnx = s_pj,
	ot -> ot_info = (caddr_t) printJAction;

    (void) umask (0);
}

/*  */

/* the following code is from the BSD lpd sources, and is subject to these
   restrictions:

/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


static int  select (dd)
struct dirent *dd;
{
    char    c = dd -> d_name[0];

    return ((c == 't' || c == 'c' || c == 'd') && dd -> d_name[1] == 'f');
}


static int  sortq (d1, d2)
struct dirent **d1,
	      **d2;
{
    char    c1,
	    c2;

    if (c1 = strcmp ((*d1) -> d_name + 3, (*d2) -> d_name + 3))
	return c1;
    if ((c1 = (*d1) -> d_name[0]) == (c2 = (*d2) -> d_name[0]))
	return ((*d1) -> d_name[2] - (*d2) ->d_name[2]);

    if (c1 == 'c')
	return (-1);
    if (c1 == 'd' || c2 == 'c')
	return 1;

    return 0;
}


#define	chmode(file,m) \
    	   (chmod ((file), (int) (m)) == NOTOK) \
    		advise (LLOG_EXCEPTIONS, (file), \
			"unable to set mode 0%o for", (int) (m)); \
	else

int	sync_print (cor)
integer	cor;
{
    int	    invalid,
	    pid;
    char    buffer[BUFSIZ];
    register struct pq *pq;
    struct pq *qp;
    register struct pj *pj;
    FILE   *fp;

    switch (cor) {
	case int_SNMP_SOutPDU_commit:
	    for (pq = pq_head; pq -> pq_name; pq++) {
		register int    i;
		int	status;
		char   *cp;
		struct dirent **queue;
		struct stat st;

		if (!pq -> pq_touched)
		    continue;

		if (chdir (pq -> pq_SD) == NOTOK) {
		    advise (LLOG_EXCEPTIONS, pq -> pq_SD,
			    "unable to change directory to");
		    continue;
		}

		status = stat (pq -> pq_LO, &st);

		switch (pq -> pq_touched) {
		    case PQ_OTHER:
		        break;

		    case PQ_ABORT:
		    case PQ_STOP:
			if (status != NOTOK) {
			    if chmode (pq -> pq_LO, (st.st_mode & 0777) | 0100)
				upstat (pq, "printing disabled\n");
			}
			else
			    if (errno == ENOENT) {
				int	fd;

				if ((fd = open (pq -> pq_LO,
						O_WRONLY | O_CREAT, 0760))
				        == NOTOK)
				    advise (LLOG_EXCEPTIONS, pq -> pq_LO,
					    "unable to create");
				else {
				    (void) close (fd);
				    upstat (pq, "printing disabled\n");
				}

				break;
			    }
			    else {
				advise (LLOG_EXCEPTIONS, pq -> pq_LO,
					"unable to stat");

				break;
			    }
			if (pq -> pq_touched == PQ_STOP)
			    break;
			/* else fall... */

		    case PQ_RESTART:
			if ((pid = findaemon (pq, NULLCP)) <= OK) {
			    if (pq -> pq_touched == PQ_RESTART)
				startdaemon (pq);
			    break;
			}
			if (kill (pid, SIGTERM) == NOTOK)
			    advise (LLOG_EXCEPTIONS, "",
				    "unable to terminate pid %d", pid);
			else {
			    for (i = 5; i-- > 0; sleep (1))
				if (kill (pid, 0) == NOTOK)
				    break;
			    if (i <= 0)
				(void) kill (pid, SIGKILL);
			}
			if (pq -> pq_touched == PQ_RESTART)
			    startdaemon (pq);
		        break;

		    case PQ_CLEAN:
			if ((status = scandir (".", &queue, select, sortq))
			        == NOTOK) {
			    advise (LLOG_EXCEPTIONS, NULLCP,
				    "unable to examine %s", pq -> pq_SD);
			    break;
			}
			for (i = 0; i < status; i++) {
			    int	    n;

			    switch (*(cp = queue[i] -> d_name)) {
				case 'c':
				    n = 0;
				    while (i + 1 < status) {
					char   *dp = queue[i + 1] -> d_name;

					if (*dp != 'd'
					        || strcmp (cp + 3, dp + 3))
					    break;
					i++, n++;
				    }
				    if (n == 0 && unlink (cp) == NOTOK)
					advise (LLOG_EXCEPTIONS, cp,
						"unable to remove");
				    break;

				case 'd':
				    if (unlink (cp) == NOTOK)
					advise (LLOG_EXCEPTIONS, cp,
						"unable to remove");
				    break;

				default:
				    break;
			    }
			}
			for (i = 0; i < status; i++)
			    free ((char *) queue[i]);
			free ((char *) queue);
		        break;

		    case PQ_ENABLE:
			if (status != NOTOK) {
			    if chmode (pq -> pq_LO, st.st_mode & 0767);
			}
		        break;

		    case PQ_DISABLE:
			if (status != NOTOK) {
			    if chmode (pq -> pq_LO,(st.st_mode & 0777) | 0010);
			}
			else
			    if (errno == ENOENT) {
				int	fd;

				if ((fd = open (pq -> pq_LO,
						O_WRONLY | O_CREAT, 0670))
				        == NOTOK)
				    advise (LLOG_EXCEPTIONS, pq -> pq_LO,
					    "unable to create");
				else
				    (void) close (fd);
			    }
			    else
				advise (LLOG_EXCEPTIONS, pq -> pq_LO,
					"unable to stat");
		        break;

		    case PQ_START:
			if (status != NOTOK) {
			    if chmode (pq -> pq_LO, st.st_mode & 0677);
			}
			startdaemon (pq);
		        break;

		    default:
			advise (LLOG_EXCEPTIONS, NULLCP,
				"unknown action %d for print queue %s",
				pq -> pq_touched, pq -> pq_name);
			pq -> pq_touched = PQ_OTHER;
			break;
		}
		pq -> pq_action = pq -> pq_touched, pq -> pq_touched = 0;
	    }

	    invalid = 0;
	    qp = NULL;
	    for (pj = pj_head; pj -> pj_pq; pj++) {
		if (!pj -> pj_touched)
		    continue;

		if ((pq = pj -> pj_pq) != qp)
		    if (chdir (pq -> pq_SD) == NOTOK) {
			advise (LLOG_EXCEPTIONS, pq -> pq_SD,
				"unable to change directory to");
			continue;
		    }
		    else
			qp = pq;

		switch (pj -> pj_touched) {
		    case PJ_OTHER:
		        break;

		    case PJ_TOPQ:
			{
			    register struct pj *qj;
			    struct timeval tv[2];

			    for (qj = pj_head; qj < pj; qj++)
				if (qj -> pj_pq == pq)
				    break;
			    if (qj >= pj)
				continue;
			    tv[0].tv_sec = tv[1].tv_sec =
						   --qj -> pj_st.st_mtime;
			    tv[0].tv_usec = tv[1].tv_usec = 0;
			    if (utimes (pj -> pj_file, tv) == NOTOK)
				advise (LLOG_EXCEPTIONS, pj -> pj_file,
					"unable to touch");
			    invalid = 1;
			}
			break;

		    case PJ_REMOVE:
			if (fp = fopen (pj -> pj_file, "r")) {
			    if ((pid = findaemon (pq, buffer)) > OK
				    && strcmp (pj -> pj_file, buffer) == 0) {
				register int	i;

				if (kill (pid, SIGINT) == NOTOK) {
				    advise (LLOG_EXCEPTIONS, "",
					    "unable to interrupt pid %d", pid);
				    pid = NOTOK;
				}
				else {
				    for (i = 5; i-- > 0; sleep (1))
					if (kill (pid, 0) == NOTOK)
					    break;
				    if (i <= 0)
					(void) kill (pid, SIGKILL);
				}
			    }
			    else
				pid = NOTOK;

			    while (fgets (buffer, sizeof buffer - 1, fp)) {
				char   *cp;

				if (buffer[0] != 'U')
				    continue;
				if (cp = index (buffer, '\n'))
				    *cp = NULL;
				if (unlink (buffer + 1) == NOTOK) {
				    advise (LLOG_EXCEPTIONS, buffer + 1,
					    "unable to remove");
				    invalid = 1;
				}
			    }
			    (void) fclose (fp);

			    if (unlink (pj -> pj_file) == NOTOK) {
				advise (LLOG_EXCEPTIONS, pj -> pj_file,
					"unable to remove");
				invalid = 1;
			    }

			    if (pid > OK)
				startdaemon (pq);
			}
			else {
			    advise (LLOG_EXCEPTIONS, pj -> pj_file,
				    "unable to read");
			    invalid = 1;
			}
			break;

		    default:
			advise (LLOG_EXCEPTIONS, NULLCP,
				"unknown action %d for print job %s/%s",
				pq -> pq_touched, pq -> pq_name,
				pj -> pj_file);
			pj -> pj_touched = PJ_OTHER;
			break;
		}
		pj -> pj_action = pj -> pj_touched, pj -> pj_touched = 0;
	    }
	    if (invalid)
		free_pj ();
	    break;

	case int_SNMP_SOutPDU_rollback:
	    for (pq = pq_head; pq -> pq_name; pq++)
		if (pq -> pq_touched)
		    pq -> pq_touched = 0;

	    for (pj = pj_head; pj -> pj_pq; pj++)
		if (pj -> pj_touched)
		    pj -> pj_touched = 0;
	    break;
    }
}

/*  */

static	upstat (pq, msg)
register struct pq *pq;
char   *msg;
{
    int	    fd;

    if ((fd = open (pq -> pq_ST, O_WRONLY | O_CREAT, 0664)) == NOTOK) {
	advise (LLOG_EXCEPTIONS, pq -> pq_ST, "unable to create");
	return;
    }

    if (flock (fd, LOCK_EX) < 0) {
	advise (LLOG_EXCEPTIONS, pq -> pq_ST, "unable to flock");
	(void) close (fd);
	return;
    }

    (void) ftruncate (fd, (off_t) 0);
    (void) write (fd, msg, strlen (msg));

    (void) close (fd);
}

/*  */

#include <sys/socket.h>
#include <sys/un.h>


static	startdaemon (pq)
register struct pq *pq;
{
    register int    n,
		    sd;
    char buffer[BUFSIZ];
    struct sockaddr_un sunix;

    if ((sd = socket (AF_UNIX, SOCK_STREAM, 0)) == NOTOK) {
	advise (LLOG_EXCEPTIONS, "unix socket", "unable to start");
	return;
    }

    bzero ((char *) &sunix, sizeof sunix);
    sunix.sun_family = AF_UNIX;
    (void) strcpy (sunix.sun_path, _PATH_SOCKETNAME);
    if (connect(sd, (struct sockaddr *) &sunix, strlen (sunix.sun_path) + 2)
	    == NOTOK) {
	advise (LLOG_EXCEPTIONS, "unix socket", "unable to connect");
	(void) close (sd);
	return;
    }

    (void) sprintf (buffer, "\1%s\n", pq -> pq_name);
    n = strlen (buffer);
    if (write (sd, buffer, n) != n) {
	advise (LLOG_EXCEPTIONS, "unix socket", "error writing to");
	(void) close (sd);
	return;
    }
    if (read (sd, buffer, 1) == 1)
	if (buffer[0] == NULL) {
	    (void) close (sd);
	    return;
	}
    advise (LLOG_EXCEPTIONS, NULLCP, "lpd refuses to start print queue %s",
	    pq -> pq_name);
    while ((n = read (sd, buffer, sizeof (buffer))) > 0)
	continue;
    (void) close (sd);

    return;
}

/*  */

static int  findaemon (pq, current)
register struct pq *pq;
char   *current;
{
    int	    pid;
    char    buffer[BUFSIZ];
    FILE   *fp;

    if (!(fp = fopen (pq -> pq_LO, "r")))
	return NOTOK;

    pid = OK;
    if (fgets (buffer, sizeof buffer - 1, fp)) {
	char   *cp;

	if (cp = index (buffer, '\n'))
	    *cp = NULL;
	if ((pid = atoi (buffer)) > OK && kill (pid, 0) == NOTOK)
	    pid = OK;

	if (current) {
	    if (pid > OK
	            && fgets (buffer, sizeof buffer - 1, fp)) {
		if (cp = index (buffer, '\n'))
		    *cp = NULL;
		(void) strcpy (current, buffer);
	    }
	    else
		current[0] = NULL;
	}
    }

    (void) fclose (fp);

    return pid;
}
