/* users.c - MIB realization of the users group */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/snmp/RCS/users.c,v 9.0 1992/06/16 12:38:11 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/snmp/RCS/users.c,v 9.0 1992/06/16 12:38:11 isode Rel $
 *
 *
 * $Log: users.c,v $
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


/*    USERS */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include "smux.h"
#include "objects.h"
#include "logger.h"


#define	generr(offset)	((offset) == type_SNMP_SMUX__PDUs_get__next__request \
				    ? NOTOK : int_SNMP_error__status_genErr)


extern	int	quantum;

void	advise ();

/*  */

#include <pwd.h>
#include <grp.h>
#include "sys.file.h"
#include <sys/stat.h>


struct pw {
#define	PW_SIZE	8
    unsigned int  pw_instance[PW_SIZE];
    int		  pw_insize;

    struct passwd pw_pw;
    int		  pw_malloc;
    int		  pw_new;

    int		  pw_status;
#define	PW_OTHER	1
#define	PW_INVALID	2
};

static	struct pw *pw_head = NULL;
static	char   *pw_data = NULL;
static	int	pw_touched = 0;
static	int	pw_fd = NOTOK;
static	struct stat pw_st;


struct gr {
#define	GR_SIZE	8
    unsigned int  gr_instance[GR_SIZE];
    int		  gr_insize;

    int		  gr_serial;
    struct group  gr_gr;
    int		  gr_malloc;

    int		  gr_status;
#define	GR_OTHER	1
#define	GR_INVALID	2
};

static	struct gr *gr_head = NULL;
static	char   *gr_data = NULL;
static	int	gr_touched = 0;
static	int	gr_fd = NOTOK;
static	struct stat gr_st;


struct gu {
#define	GU_SIZE	(PW_SIZE + 1 + GR_SIZE)
    unsigned int  gu_instance[GU_SIZE];
    int		  gu_insize;

    int		  gu_serial;
    char	 *gu_user;
    int		  gu_malloc;

    int		  gu_status;
#define	GU_OTHER	1
#define	GU_INVALID	2
};

static	struct gu *gu_head = NULL;


char   *mycrypt ();

/*  */

static int  pw_compar (a, b)
register struct pw *a,
		   *b;
{
    return elem_cmp (a -> pw_instance, a -> pw_insize,
		     b -> pw_instance, b -> pw_insize);
}


static int  get_pw (offset)
int	offset;
{
    register int    i,
		    j;
    int	    cc;
    register char  *dp;
    char   *ep;
    register struct pw *pw;
    struct stat st;
    static int lastq = -1;

    if (quantum == lastq)
	return OK;
    if (pw_head
	    && (offset == type_SNMP_SMUX__PDUs_set__request
		    ? pw_fd != NOTOK
 		    : (stat ("/etc/passwd", &st) != NOTOK
		           && pw_st.st_mtime == st.st_mtime))) {
	lastq = quantum;
	return OK;
    }

    free_pw ();
    if ((pw_fd = open ("/etc/passwd", O_RDONLY)) == NOTOK) {
	advise (LLOG_EXCEPTIONS, "/etc/passwd", "unable to read");
	return NOTOK;
    }
    if (flock (pw_fd, (offset == type_SNMP_SMUX__PDUs_set__request ? LOCK_EX 
								   : LOCK_SH)
			    | LOCK_NB) == NOTOK) {
	advise (LLOG_EXCEPTIONS, "/etc/passwd", "unable to flock");
ohoh: ;
	(void) close (pw_fd), pw_fd = NOTOK;
	return NOTOK;
    }
    if (fstat (pw_fd, &st) == NOTOK) {
	advise (LLOG_EXCEPTIONS, "/etc/passwd", "unable to fstat");
oops: ;
	(void) flock (pw_fd, LOCK_UN);
	goto ohoh;
    }
    if ((cc = st.st_size) == 0) {
	advise (LLOG_EXCEPTIONS, NULLCP, "/etc/passwd: empty file");
	goto oops;
    }

    if ((pw_data = malloc ((unsigned) (cc + 1))) == NULL) {
	advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	goto oops;
    }
    for (dp = pw_data, j = cc; j > 0; dp += i, j -= i)
	switch (i = read (pw_fd, dp, j)) {
	    case NOTOK:
	        advise (LLOG_EXCEPTIONS, "/etc/passwd", "error reading");
losing: ;
		free (pw_data), pw_data = NULL;
		goto oops;

	    case OK:
	        advise (LLOG_EXCEPTIONS, NULLCP,
			"premature eof on /etc/passwd");
		goto losing;

	    default:
		break;
	}
    *dp = NULL;
    pw_st = st;	/* struct copy */
    lastq = quantum;

    i = 1;
    for (ep = (dp = pw_data) + cc; dp < ep; )
	if (*dp++ == '\n')
	    i++;

    if ((pw_head = (struct pw *) calloc ((unsigned) i, sizeof *pw_head))
	    == NULL) {
	advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	goto losing;
    }
    pw_touched = 0;
    if (offset != type_SNMP_SMUX__PDUs_set__request) {
	(void) flock (pw_fd, LOCK_UN);
	(void) close (pw_fd), pw_fd = NOTOK;
    }

    for (pw = pw_head, dp = pw_data; dp < ep; pw++) {
again: ;
	pw -> pw_pw.pw_name = dp;

	for (;;) {
	    if (*dp == ':') {
		register unsigned int *ip;
		register char   *cp;
		register struct pw *qw;

		*dp++ = NULL;
		while (++dp < ep)
		    if (*dp == '\n') {
			dp++;
			break;
		    }

		for (cp = pw -> pw_pw.pw_name, ip = pw -> pw_instance, i = 0;
		         *cp;
		         i++)
		    *ip++ = *cp++ & 0xff;
		pw -> pw_insize = i;

		for (qw = pw_head; qw < pw; qw++)
		    if (elem_cmp (pw -> pw_instance, pw -> pw_insize,
				  qw -> pw_instance, qw -> pw_insize) == 0) {
			advise (LLOG_EXCEPTIONS, NULLCP,
				"duplicate user: %s", qw -> pw_pw.pw_name);
			pw -> pw_pw.pw_name = NULL;
			if (dp < ep)
			    goto again;
			goto out;
		    }

		pw -> pw_status = PW_OTHER;
		break;
	    }

	    if (*dp++ == '\n' || dp >= ep)
		break;
	}
    }
out: ;

    if (pw - pw_head > 1)
	qsort ((char *) pw_head, pw - pw_head, sizeof *pw_head, pw_compar);

    return OK;
}


static	fill_pw (pwp)
register struct passwd *pwp;
{
    register char  *cp,
		   *dp;

    if (dp = index (cp = pwp -> pw_name + strlen (pwp -> pw_name) + 1, '\n'))
	*dp = NULL;

#ifdef PW_QUOTA
    pwp -> pw_quota = -1;
#endif
#ifndef BSD44
    pwp -> pw_comment = 
#endif
    pwp -> pw_gecos = pwp -> pw_dir = pwp -> pw_shell = "";
    if (!(dp = index (pwp -> pw_passwd = cp, ':')))
	return;
    *dp++ = NULL;
    if (!(cp = index (dp, ':')))
	return;
    *cp++ = NULL;
    pwp -> pw_uid = atoi (dp);
    if (!(dp = index (cp, ':')))
	return;
    *dp++ = NULL;
    pwp -> pw_gid = atoi (cp);
    if (!(cp = index (pwp -> pw_gecos = dp, ':')))
	return;
    *cp++ = NULL;
    if (!(dp = index (pwp -> pw_dir = cp, ':')))
	return;
    *dp++ = NULL;
    pwp -> pw_shell = dp;
}


static	free_pw ()
{
    register struct pw *pw;

    if (pw_head) {
	for (pw = pw_head; pw -> pw_pw.pw_name; pw++) {
	    register struct passwd *pwp = &pw -> pw_pw;

	    if (pw -> pw_malloc) {
		if (pwp -> pw_name)
		    free (pwp -> pw_name);
		if (pwp -> pw_passwd)
		    free (pwp -> pw_passwd);
#ifndef BSD44
		if (pwp -> pw_comment)
		    free (pwp -> pw_comment);
#endif
		if (pwp -> pw_gecos)
		    free (pwp -> pw_gecos);
		if (pwp -> pw_dir)
		    free (pwp -> pw_dir);
		if (pwp -> pw_shell)
		    free (pwp -> pw_shell);
	    }
	}

	free ((char *) pw_head), pw_head = NULL;
    }
    if (pw_data)
	free (pw_data), pw_data = NULL;

    if (pw_fd != NOTOK) {
	(void) flock (pw_fd, LOCK_UN);
	(void) close (pw_fd), pw_fd = NOTOK;
    }
}

/*  */

static struct pw *get_pwent (ip, len, isnext)
register unsigned int *ip;
int	len;
int	isnext;
{
    register struct pw *pw;

    for (pw = pw_head; pw -> pw_pw.pw_name; pw++)
	switch (elem_cmp (pw -> pw_instance, pw -> pw_insize, ip, len)) {
	    case 0:
	        if (!isnext)
		    return pw;
		if ((++pw) -> pw_pw.pw_name == NULL)
		    return NULL;
		/* else fall... */

	    case 1:
		return (isnext ? pw : NULL);
	}

    return NULL;
}

/*  */

static int  gr_compar (a, b)
register struct gr *a,
		   *b;
{
    return elem_cmp (a -> gr_instance, a -> gr_insize,
		     b -> gr_instance, b -> gr_insize);
}


static int  get_gr (offset)
int	offset;
{
    register int    i,
		    j;
    int	    cc;
    register char  *dp;
    char   *ep;
    register struct gr *gr;
    struct stat st;
    static int lastq = -1;

    if (quantum == lastq)
	return OK;
    if (gr_head
	    && (offset == type_SNMP_SMUX__PDUs_set__request
		    ? gr_fd != NOTOK
 		    : (stat ("/etc/group", &st) != NOTOK
		           && gr_st.st_mtime == st.st_mtime))) {
	lastq = quantum;
	return OK;
    }

    free_gr ();
    if ((gr_fd = open ("/etc/group", O_RDONLY)) == NOTOK) {
	advise (LLOG_EXCEPTIONS, "/etc/group", "unable to read");
	return NOTOK;
    }
    if (flock (gr_fd, (offset == type_SNMP_SMUX__PDUs_set__request ? LOCK_EX 
								   : LOCK_SH)
			    | LOCK_NB) == NOTOK) {
	advise (LLOG_EXCEPTIONS, "/etc/group", "unable to flock");
ohoh: ;
	(void) close (gr_fd), gr_fd = NOTOK;
	return NOTOK;
    }
    if (fstat (gr_fd, &st) == NOTOK) {
	advise (LLOG_EXCEPTIONS, "/etc/group", "unable to fstat");
oops: ;
	(void) flock (gr_fd, LOCK_UN);
	goto ohoh;
    }
    if ((cc = st.st_size) == 0) {
	advise (LLOG_EXCEPTIONS, NULLCP, "/etc/group: empty file");
	goto oops;
    }

    if ((gr_data = malloc ((unsigned) (cc + 1))) == NULL) {
	advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	goto oops;
    }
    for (dp = gr_data, j = cc; j > 0; dp += i, j -= i)
	switch (i = read (gr_fd, dp, j)) {
	    case NOTOK:
	        advise (LLOG_EXCEPTIONS, "/etc/group", "error reading");
losing: ;
		free (gr_data), gr_data = NULL;
		goto oops;

	    case OK:
	        advise (LLOG_EXCEPTIONS, NULLCP,
			"premature eof on /etc/group");
		goto losing;

	    default:
		break;
	}
    *dp = NULL;
    gr_st = st;	/* struct copy */
    lastq = quantum;

    i = 1;
    for (ep = (dp = gr_data) + cc; dp < ep; )
	if (*dp++ == '\n')
	    i++;

    if ((gr_head = (struct gr *) calloc ((unsigned) i, sizeof *gr_head))
	    == NULL) {
	advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	goto losing;
    }
    gr_touched = 0;
    if (offset != type_SNMP_SMUX__PDUs_set__request) {
	(void) flock (gr_fd, LOCK_UN);
	(void) close (gr_fd), gr_fd = NOTOK;
    }

    for (gr = gr_head, dp = gr_data; dp < ep; gr++) {
again: ;
	gr -> gr_serial = gr - gr_head;
	gr -> gr_gr.gr_name = dp;

	for (;;) {
	    if (*dp == ':') {
		register unsigned int *ip;
		register char   *cp;
		register struct gr *qw;

		*dp++ = NULL;
		while (++dp < ep)
		    if (*dp == '\n') {
			dp++;
			break;
		    }

		for (cp = gr -> gr_gr.gr_name, ip = gr -> gr_instance, i = 0;
		         *cp;
		         i++)
		    *ip++ = *cp++ & 0xff;
		gr -> gr_insize = i;

		for (qw = gr_head; qw < gr; qw++)
		    if (elem_cmp (gr -> gr_instance, gr -> gr_insize,
				  qw -> gr_instance, qw -> gr_insize) == 0) {
			advise (LLOG_EXCEPTIONS, NULLCP,
				"duplicate group: %s", qw -> gr_gr.gr_name);
			gr -> gr_gr.gr_name = NULL;
			if (dp < ep)
			    goto again;
			goto out;
		    }

		gr -> gr_status = GR_OTHER;
		break;
	    }

	    if (*dp++ == '\n' || dp >= ep)
		break;
	}
    }
out: ;

    if (gr - gr_head > 1)
	qsort ((char *) gr_head, gr - gr_head, sizeof *gr_head, gr_compar);

    return OK;
}


static	fill_gr (grp)
register struct group *grp;
{
    register char *cp,
		  *dp;

    if (dp = index (cp = grp -> gr_name + strlen (grp -> gr_name) + 1, '\n'))
	*dp = NULL;

    if (!(dp = index (grp -> gr_passwd = cp, ':')))
	return;
    *dp++ = NULL;
    grp -> gr_gid = atoi (dp);
}


static	free_gr ()
{
    register struct gr *gr;

    if (gr_head) {
	register struct gu *gu;

	for (gr = gr_head; gr -> gr_gr.gr_name; gr++) {
	    register struct group *grp = &gr -> gr_gr;

	    if (gr -> gr_malloc) {
		if (grp -> gr_name)
		    free (grp -> gr_name);
		if (grp -> gr_passwd)
		    free (grp -> gr_passwd);
	    }
	    if (grp -> gr_mem)
		free ((char *)  grp -> gr_mem);
	}

	free ((char *) gr_head), gr_head = NULL;

	if (gu_head) {
	    for (gu = gu_head; gu -> gu_user; gu++)
		if (gu -> gu_malloc) {
		    if (gu -> gu_user)
			free (gu -> gu_user);
		}
	    
	    free ((char *) gu_head), gu_head = NULL;
	}
    }
    if (gr_data)
	free (gr_data), gr_data = NULL;

    if (gr_fd != NOTOK) {
	(void) flock (gr_fd, LOCK_UN);
	(void) close (gr_fd), gr_fd = NOTOK;
    }
}

/*  */

static struct gr *get_grent (ip, len, isnext)
register unsigned int *ip;
int	len;
int	isnext;
{
    register struct gr *gr;

    for (gr = gr_head; gr -> gr_gr.gr_name; gr++)
	switch (elem_cmp (gr -> gr_instance, gr -> gr_insize, ip, len)) {
	    case 0:
	        if (!isnext)
		    return gr;
		if ((++gr) -> gr_gr.gr_name == NULL)
		    return NULL;
		/* else fall... */

	    case 1:
		return (isnext ? gr : NULL);
	}

    return NULL;
}

/*  */

static int  gu_compar (a, b)
register struct gu *a,
		   *b;
{
    return elem_cmp (a -> gu_instance, a -> gu_insize,
		     b -> gu_instance, b -> gu_insize);
}


static int  gm_compar (a, b)
char  **a,
      **b;
{
    return strcmp (*a, *b);
}


static int  get_gu (offset)
int	offset;
{
    register int     i;
    register char   *cp,
		   **ap;
    register struct gr *gr;
    register struct gu *gu;

    if (get_gr (offset) == NOTOK)
	return NOTOK;
    if (gu_head)
	return OK;

    for (gr = gr_head; gr -> gr_gr.gr_name; gr++)
	if (!gr -> gr_gr.gr_mem) {
	    register char  *dp;
	    register struct group *grp = &gr -> gr_gr;

	    if (!grp -> gr_passwd)
		fill_gr (grp);
	    dp = grp -> gr_passwd + strlen (grp -> gr_passwd) + 1;

	    i = 0;
	    if (cp = index (dp, ':')) {
		for (dp = ++cp; dp = index (dp, ','); dp++)
		    i++;
		if (*cp)
		    i++;
	    }

	    if (!(grp -> gr_mem = (char **) calloc ((unsigned) (i + 1),
						    sizeof *grp -> gr_mem))) {
		advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
		return NOTOK;
	    }
	    if (!i)
		continue;

	    dp = cp;
	    for (ap = grp -> gr_mem;; ap++) {
		register char **bp;

again: ;
		if (i-- <= 0)
		    break;
		if (dp = index (*ap = dp, ','))
		    *dp++ = NULL;
		for (bp = grp -> gr_mem; bp < ap; bp++)
		    if (strcmp (*bp, *ap) == 0) {
			advise (LLOG_EXCEPTIONS, NULLCP,
				"duplicate member: %d for %s", grp -> gr_gid,
				*bp);
			*ap = NULL;
			goto again;
		    }
	    }

	    if (ap -  grp -> gr_mem > 1)
		qsort ((char *) grp -> gr_mem, ap -  grp -> gr_mem,
		       sizeof *grp -> gr_mem, gm_compar);
	}

    i = 1;
    for (gr = gr_head; gr -> gr_gr.gr_name; gr++)
	for (ap = gr -> gr_gr.gr_mem; *ap; ap++)
	    i++;

    if ((gu_head = (struct gu *) calloc ((unsigned) i, sizeof *gu_head))
	    == NULL) {
	advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	return NOTOK;
    }

    gu = gu_head;
    for (gr = gr_head; gr -> gr_gr.gr_name; gr++)
	for (ap = gr -> gr_gr.gr_mem; *ap; ap++) {
	    register unsigned int *ip;

	    ip = gu -> gu_instance, i = 0;
	    for (cp = gr -> gr_gr.gr_name; *cp; i++)
		*ip++ = *cp++ & 0xff;
	    *ip++ = 0, i++;
	    for (cp = *ap; *cp; i++)
		*ip++ = *cp++ & 0xff;
	    gu -> gu_insize = i;

	    gu -> gu_serial = gr -> gr_serial;
	    gu -> gu_user = *ap;
	    gu -> gu_status = GU_OTHER;
	    gu++;
	}

    if (gu - gu_head > 1)
	qsort ((char *) gu_head, gu -gu_head, sizeof *gu_head, gu_compar);

    return OK;
}

/*  */

static struct gu *get_guent (ip, len, isnext)
register unsigned int *ip;
int	len;
int	isnext;
{
    register struct gu *gu;

    if (!gu_head)
	return NULL;
    for (gu = gu_head; gu -> gu_user; gu++)
	switch (elem_cmp (gu -> gu_instance, gu -> gu_insize, ip, len)) {
	    case 0:
	        if (!isnext)
		    return gu;
		if ((++gu) -> gu_user == NULL)
		    return NULL;
		/* else fall... */

	    case 1:
		return (isnext ? gu : NULL);
	}

    return NULL;
}

/*  */

#define	userName	0
#define	userPasswd	1
#define	userID		2
#define	userGroup	3
#define	userQuota	4
#define	userComment	5
#define	userFullName	6
#define	userHome	7
#define	userShell	8
#define	userStatus	9


static int  o_user (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i;
    register unsigned int *ip,
			  *jp;
    register struct pw *pw;
    register struct passwd *pwp;
    register OID    oid = oi -> oi_name;
    OID	    new;
    register OT	    ot = oi -> oi_type;

    if (get_pw (offset) == NOTOK) {
losing: ;
	return generr (offset);
    }

    if ((ifvar = (int) ot -> ot_info) == userGroup
	    && get_gr (offset) == NOTOK)
	goto losing;
try_again: ;
    switch (offset) {
	case type_SNMP_SMUX__PDUs_get__request:
	    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
		return int_SNMP_error__status_noSuchName;
	    if ((pw = get_pwent (oid -> oid_elements
				     + ot -> ot_name -> oid_nelem,
				 oid -> oid_nelem
				     - ot -> ot_name -> oid_nelem, 0)) == NULL)
		return int_SNMP_error__status_noSuchName;
	    break;

	case type_SNMP_SMUX__PDUs_get__next__request:
	    if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
		if ((pw = pw_head) == NULL || pw -> pw_pw.pw_name == NULL)
		    return NOTOK;

		if ((new = oid_extend (oid, pw -> pw_insize)) == NULLOID)
		    return NOTOK;
		ip = new -> oid_elements + new -> oid_nelem - pw -> pw_insize;
		jp = pw -> pw_instance;
		for (i = pw -> pw_insize; i > 0; i--)
		    *ip++ = *jp++;
		
		if (v -> name)
		    free_SNMP_ObjectName (v -> name);
		v -> name = new;

		oid = new;	/* for try_again... */
	    }
	    else {
		int	j;

		if ((pw = get_pwent (oid -> oid_elements
				         + ot -> ot_name -> oid_nelem,
				     j = oid -> oid_nelem
				             - ot -> ot_name -> oid_nelem, 1))
		        == NULL)
		    return NOTOK;

		if ((i = j - pw -> pw_insize) < 0) {
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
		jp = pw -> pw_instance;
		for (i = pw -> pw_insize; i > 0; i--)
		    *ip++ = *jp++;
	    }
	    break;

	default:
	    return int_SNMP_error__status_genErr;
    }

    pwp = &pw -> pw_pw;
    switch (ifvar) {
	case userName:
	case userStatus:
	    break;

	default:
	    if (!pwp -> pw_passwd)
		fill_pw (pwp);
	    break;
    }

    switch (ifvar) {
	case userName:
	    return o_string (oi, v, pwp -> pw_name,  strlen (pwp -> pw_name));

	case userPasswd:
	    if (strcmp (pwp -> pw_passwd, "*") == 0)
		return o_string (oi, v, pwp -> pw_passwd,
				 strlen (pwp -> pw_passwd));
	    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
		goto try_again;
	    return NOTOK;

	case userID:
	    return o_integer (oi, v, pwp -> pw_uid);

	case userGroup:
	    {
		char    buffer[BUFSIZ];
		register struct gr *gr;

		for (gr = gr_head; gr -> gr_gr.gr_name; gr++) {
		    register struct group *grp = &gr -> gr_gr;

		    if (!grp -> gr_passwd)
			fill_gr (grp);
		    if (pwp -> pw_gid == grp -> gr_gid)
			return o_string (oi, v, grp -> gr_name,
					 strlen (grp -> gr_name));
		}

		(void) sprintf (buffer, "%d", pwp -> pw_gid);
		return o_string (oi, v, buffer, strlen (buffer));
	    }

	case userQuota:
#ifdef PW_QUOTA
	    if (pwp -> pw_quota != -1)
		return o_integer (oi, v, pwp -> pw_quota);
#endif
	    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
		goto try_again;
	    return NOTOK;

	case userComment:
#ifndef BSD44
	    if (*pwp -> pw_comment)
		return o_string (oi, v, pwp -> pw_comment,
				 strlen (pwp -> pw_comment));
#endif
	    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
		goto try_again;
	    return NOTOK;

	case userFullName:
	    return o_string (oi, v, pwp -> pw_gecos, strlen (pwp -> pw_gecos));

	case userHome:
	    return o_string (oi, v, pwp -> pw_dir, strlen (pwp -> pw_dir));

	case userShell:
	    return o_string (oi, v, pwp -> pw_shell, strlen (pwp -> pw_shell));

	case userStatus:
	    return o_integer (oi, v, pw -> pw_status);

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

static int  s_user (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar,
	    result;
    register int    i;
    register unsigned int *ip;
    char   *cp,
	  **ap;
    register struct pw *pw;
    register struct passwd *pwp;
    register OID    oid = oi -> oi_name;
    register OT	    ot = oi -> oi_type;
    register OS	    os = ot -> ot_syntax;
    caddr_t	value;

    if (get_pw (offset) == NOTOK)
	return int_SNMP_error__status_genErr;

    if ((ifvar = (int) ot -> ot_info) == userGroup
	    && get_gr (offset) == NOTOK)
	return int_SNMP_error__status_genErr;
    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
	return int_SNMP_error__status_noSuchName;
    if ((pw = get_pwent (ip = oid -> oid_elements + ot -> ot_name -> oid_nelem,
			 i = oid -> oid_nelem - ot -> ot_name -> oid_nelem, 0))
	    == NULL) {
	register unsigned int *jp;
	char    name[PW_SIZE + 1];

	if (i > PW_SIZE)
	    return int_SNMP_error__status_noSuchName;
	for (cp = name; i-- > 0; ip++, cp++)
	    if (*ip > 0xff || !isascii ((u_char) (*cp = *ip & 0xff)))
		return int_SNMP_error__status_noSuchName;
	*cp = NULL;

	i = 0;
	for (pw = pw_head; pw -> pw_pw.pw_name; pw++)
	    i++;
	pwp = NULL;
	if ((pw = (struct pw *) realloc ((char *) pw_head,
					(unsigned) (i + 2) * sizeof *pw_head))
	        == NULL) {
no_mem: ;
	    advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	    if (pwp) {
		if (pwp -> pw_name)
		    free (pwp -> pw_name), pwp -> pw_name = NULL;
		if (pwp -> pw_passwd)
		    free (pwp -> pw_passwd);
#ifndef BSD44
		if (pwp -> pw_comment)
		    free (pwp -> pw_comment);
#endif
		if (pwp -> pw_gecos)
		    free (pwp -> pw_gecos);
		if (pwp -> pw_dir)
		    free (pwp -> pw_dir);
		if (pwp -> pw_shell)
		    free (pwp -> pw_shell);
	    }

	    return int_SNMP_error__status_genErr;
	}

	bzero ((char *)	(pw = (pw_head = pw) + i), sizeof *pw);
	pw -> pw_insize = oid -> oid_nelem - ot -> ot_name -> oid_nelem;
	for (ip = oid -> oid_elements + ot -> ot_name -> oid_nelem,
		    jp = pw -> pw_instance, i = pw -> pw_insize;
	         i > 0;
	         i--)
	    *jp++ = *ip++;
	pwp = &pw -> pw_pw;
	if ((pwp -> pw_name = strdup (name)) == NULL
		|| (pwp -> pw_passwd = strdup ("*")) == NULL
#ifndef BSD44
		|| (pwp -> pw_comment = strdup ("")) == NULL
#endif
		|| (pwp -> pw_gecos = strdup ("")) == NULL
		|| (pwp -> pw_dir = strdup ("/")) == NULL
		|| (pwp -> pw_shell = strdup ("")) == NULL)
	    goto no_mem;
	pwp -> pw_uid = pwp -> pw_gid = 1;
#ifdef PW_QUOTA
	pwp -> pw_quota = -1;
#endif
	pw -> pw_malloc = pw -> pw_new = 1;
	pw -> pw_status = PW_OTHER;

	pw++;
	bzero ((char *) pw, sizeof *pw);
	if (pw - pw_head > 1)
	    qsort ((char *) pw_head, pw - pw_head, sizeof *pw_head, pw_compar);
	if (!(pw = get_pwent (oid -> oid_elements
			          + ot -> ot_name -> oid_nelem,
			      oid -> oid_nelem
			          - ot -> ot_name -> oid_nelem, 0))) {
	    advise (LLOG_EXCEPTIONS, NULLCP, "lost newly created user");
	    free_pw ();
	    return int_SNMP_error__status_noSuchName;	    
	}
	pwp = &pw -> pw_pw;
    }
    else
	if (!(pwp = &pw -> pw_pw) -> pw_passwd)
	    fill_pw (pwp);

    if (!pwp -> pw_uid)
	goto bad_value;
    if (os == NULLOS) {
	advise (LLOG_EXCEPTIONS, NULLCP,
		"no syntax defined for object \"%s\"", ot -> ot_text);
	return int_SNMP_error__status_genErr;
    }

    switch (ifvar) {
	case userPasswd:
	case userComment:
	case userFullName:
	case userHome:
	case userShell:
	    if (!pw -> pw_malloc) {
		if (!(pwp -> pw_name = strdup (pwp -> pw_name))
			|| !(pwp -> pw_passwd = strdup (pwp -> pw_passwd))
#ifndef BSD44
			|| !(pwp -> pw_comment = strdup (pwp -> pw_comment))
#endif
			|| !(pwp -> pw_gecos = strdup (pwp -> pw_gecos))
			|| !(pwp -> pw_dir = strdup (pwp -> pw_dir))
			|| !(pwp -> pw_shell = strdup (pwp -> pw_shell))) {
losing: ;
		    advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
		    result = int_SNMP_error__status_genErr;

you_lose: ;
		    free_pw ();

		    return result;
		}

		pw -> pw_malloc = 1;
	    }
	    break;

	default:
	    break;
    }
    
    pw_touched++;
    switch (ifvar) {
	case userPasswd:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK) {
bad_value: ;
		result = int_SNMP_error__status_badValue;
		goto you_lose;
	    }
	    cp = qb2str ((struct qbuf *) value);
	    (*os -> os_free) (value);
	    if (*cp == NULL) {
		free (cp);
		goto bad_value;
	    }
	    free (pwp -> pw_passwd);
	    if (strcmp (cp, "*")) {
		pwp -> pw_passwd = strdup (mycrypt (cp));
		free (cp);
		if (!pwp -> pw_passwd)
		    goto losing;
	    }
	    else
		pwp -> pw_passwd = cp;
	    return int_SNMP_error__status_noError;	    

	case userID:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		goto bad_value;
	    pwp -> pw_uid = *((integer *) value);
	    (*os -> os_free) (value);
	    if (!pwp -> pw_uid)
		goto bad_value;
	    return int_SNMP_error__status_noError;

	case userGroup:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		goto bad_value;
	    cp = qb2str ((struct qbuf *) value);
	    (*os -> os_free) (value);
	    {
		int	gid;
		register struct gr *gr;

		for (gr = gr_head; gr -> gr_gr.gr_name; gr++)
		    if (strcmp (gr -> gr_gr.gr_name, cp) == 0) {
			register struct group *grp = &gr -> gr_gr;

			if (!grp -> gr_passwd)
			    fill_gr (grp);

			pwp -> pw_gid = grp -> gr_gid;
done_gid: ;
			free (cp);
			return int_SNMP_error__status_noError;	    
		    }

		if (sscanf (cp, "%d", &gid) == 1) {
		    pwp -> pw_gid = gid;
		    goto done_gid;
		}
		free (cp);
		goto bad_value;		
	    }

	case userQuota:
#ifdef PW_QUOTA
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		goto bad_value;
	    pwp -> pw_quota = *((integer *) value);
	    (*os -> os_free) (value);
	    return int_SNMP_error__status_noError;
#else
	    goto bad_value;
#endif

	case userComment:
#ifndef BSD44
	    ap = &pwp -> pw_comment;
	    goto do_string;
#else
	    goto bad_value;
#endif

	case userHome:
	    ap = &pwp -> pw_dir;
do_string: ;
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		goto bad_value;
	    cp = qb2str ((struct qbuf *) value);
	    (*os -> os_free) (value);
	    if (*cp == NULL) {
		free (cp);
		goto bad_value;
	    }
	    if (ifvar == userHome) {
		register char *dp;

		if (*cp != '/') {
		    free (cp);
		    goto bad_value;
		}
		if ((dp = rindex (cp + 1, '/')) && !*++dp)
		    *--dp = NULL;
	    }
	    free (*ap);
	    *ap = cp;
	    return int_SNMP_error__status_noError;	    

	case userFullName:
	    ap = &pwp -> pw_gecos;
	    goto do_string;

	case userShell:
	    ap = &pwp -> pw_shell;
	    goto do_string;

	case userStatus:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		goto bad_value;
	    i = *((integer *) value);
	    (*os -> os_free) (value);
	    switch (i) {
		case PW_OTHER:
    		case PW_INVALID:
		    pw -> pw_status = i;
		    break;

		default:
		    goto bad_value;
	    }
	    return int_SNMP_error__status_noError;

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

#define	groupName	0
#define	groupPasswd	1
#define	groupID		2
#define	groupStatus	3


static int  o_group (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i;
    register unsigned int *ip,
			  *jp;
    register struct gr *gr;
    register struct group *grp;
    register OID    oid = oi -> oi_name;
    OID	    new;
    register OT	    ot = oi -> oi_type;

    if (get_gr (offset) == NOTOK)
	return generr (offset);

    ifvar = (int) ot -> ot_info;
try_again: ;
    switch (offset) {
	case type_SNMP_SMUX__PDUs_get__request:
	    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
		return int_SNMP_error__status_noSuchName;
	    if ((gr = get_grent (oid -> oid_elements
				     + ot -> ot_name -> oid_nelem,
				 oid -> oid_nelem
				     - ot -> ot_name -> oid_nelem, 0)) == NULL)
		return int_SNMP_error__status_noSuchName;
	    break;

	case type_SNMP_SMUX__PDUs_get__next__request:
	    if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
		if ((gr = gr_head) == NULL || gr -> gr_gr.gr_name == NULL)
		    return NOTOK;

		if ((new = oid_extend (oid, gr -> gr_insize)) == NULLOID)
		    return NOTOK;
		ip = new -> oid_elements + new -> oid_nelem - gr -> gr_insize;
		jp = gr -> gr_instance;
		for (i = gr -> gr_insize; i > 0; i--)
		    *ip++ = *jp++;
		
		if (v -> name)
		    free_SNMP_ObjectName (v -> name);
		v -> name = new;

		oid = new;	/* for try_again... */
	    }
	    else {
		int	j;

		if ((gr = get_grent (oid -> oid_elements
				         + ot -> ot_name -> oid_nelem,
				     j = oid -> oid_nelem
				             - ot -> ot_name -> oid_nelem, 1))
		        == NULL)
		    return NOTOK;

		if ((i = j - gr -> gr_insize) < 0) {
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
		jp = gr -> gr_instance;
		for (i = gr -> gr_insize; i > 0; i--)
		    *ip++ = *jp++;
	    }
	    break;

	default:
	    return int_SNMP_error__status_genErr;
    }

    grp = &gr -> gr_gr;
    switch (ifvar) {
	case groupName:
	case groupStatus:
	    break;

	default:
	    if (!grp -> gr_passwd)
		fill_gr (grp);
	    break;
    }

    switch (ifvar) {
	case groupName:
	    return o_string (oi, v, grp -> gr_name,  strlen (grp -> gr_name));

	case groupPasswd:
	    if (strcmp (grp -> gr_passwd, "*") == 0)
		return o_string (oi, v, grp -> gr_passwd,
				 strlen (grp -> gr_passwd));
	    if (offset == type_SNMP_SMUX__PDUs_get__next__request)
		goto try_again;
	    return NOTOK;

	case groupID:
	    return o_integer (oi, v, grp -> gr_gid);

	case groupStatus:
	    return o_integer (oi, v, gr -> gr_status);

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

static int  s_group (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar,
	    result;
    register int    i;
    register unsigned int *ip;
    char   *cp;
    register struct gr *gr;
    register struct group *grp;
    register OID    oid = oi -> oi_name;
    register OT	    ot = oi -> oi_type;
    register OS	    os = ot -> ot_syntax;
    caddr_t	value;

    if (get_gr (offset) == NOTOK || get_gu (offset) == NOTOK)
	return int_SNMP_error__status_genErr;

    ifvar = (int) ot -> ot_info;
    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
	return int_SNMP_error__status_noSuchName;
    if ((gr = get_grent (ip = oid -> oid_elements + ot -> ot_name -> oid_nelem,
			 i = oid -> oid_nelem - ot -> ot_name -> oid_nelem, 0))
	    == NULL) {
	register unsigned int *jp;
	char    name[GR_SIZE + 1];

	if (i > GR_SIZE)
	    return int_SNMP_error__status_noSuchName;
	for (cp = name; i-- > 0; ip++, cp++)
	    if (*ip > 0xff || !isascii ((u_char) (*cp = *ip & 0xff)))
		return int_SNMP_error__status_noSuchName;
	*cp = NULL;

	i = 0;
	for (gr = gr_head; gr -> gr_gr.gr_name; gr++)
	    i++;
	grp = NULL;
	if ((gr = (struct gr *) realloc ((char *) gr_head,
					(unsigned) (i + 2) * sizeof *gr_head))
	        == NULL) {
no_mem: ;
	    advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	    if (grp) {
		if (grp -> gr_name)
		    free (grp -> gr_name), grp -> gr_name = NULL;
		if (grp -> gr_passwd)
		    free (grp -> gr_passwd);
	    }

	    return int_SNMP_error__status_genErr;
	}

	bzero ((char *)	(gr = (gr_head = gr) + i), sizeof *gr);
	gr -> gr_insize = oid -> oid_nelem - ot -> ot_name -> oid_nelem;
	for (ip = oid -> oid_elements + ot -> ot_name -> oid_nelem,
		    jp = gr -> gr_instance, i = gr -> gr_insize;
	         i > 0;
	         i--)
	    *jp++ = *ip++;
	gr -> gr_serial = gr - gr_head;
	grp = &gr -> gr_gr;
	if ((grp -> gr_name = strdup (name)) == NULL
		|| (grp -> gr_passwd = strdup ("*")) == NULL)
	    goto no_mem;
	grp -> gr_gid = -1;
	if ((grp -> gr_mem = (char **) calloc (1, sizeof *grp ->gr_mem))
	        == NULL)
	    goto no_mem;
	gr -> gr_malloc = 1;
	gr -> gr_status = GR_OTHER;

	gr++;
	bzero ((char *) gr, sizeof *gr);
	if (gr - gr_head > 1)
	    qsort ((char *) gr_head, gr - gr_head, sizeof *gr_head, gr_compar);
	if (!(gr = get_grent (oid -> oid_elements
			          + ot -> ot_name -> oid_nelem,
			      oid -> oid_nelem
			          - ot -> ot_name -> oid_nelem, 0))) {
	    advise (LLOG_EXCEPTIONS, NULLCP, "lost newly created group");
	    free_gr ();
	    return int_SNMP_error__status_noSuchName;	    
	}
	grp = &gr -> gr_gr;
    }
    else
	if (!(grp = &gr -> gr_gr) -> gr_passwd)
	    fill_gr (grp);

    if (os == NULLOS) {
	advise (LLOG_EXCEPTIONS, NULLCP,
		"no syntax defined for object \"%s\"", ot -> ot_text);
	return int_SNMP_error__status_genErr;
    }

    switch (ifvar) {
	case groupPasswd:
	    if (!gr -> gr_malloc) {
		if (!(grp -> gr_name = strdup (grp -> gr_name))
		    	|| !(grp -> gr_passwd = strdup (grp -> gr_passwd))) {
losing: ;
		    advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
		    result = int_SNMP_error__status_genErr;

you_lose: ;
		    free_gr ();

		    return result;
		}

		gr -> gr_malloc = 1;
	    }
	    break;

	default:
	    break;
    }
    
    gr_touched++;
    switch (ifvar) {
	case groupPasswd:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK) {
bad_value: ;
		result = int_SNMP_error__status_badValue;
		goto you_lose;
	    }
	    cp = qb2str ((struct qbuf *) value);
	    (*os -> os_free) (value);
	    if (*cp == NULL) {
		free (cp);
		goto bad_value;
	    }
	    free (grp -> gr_passwd);
	    if (strcmp (cp, "*")) {
		grp -> gr_passwd = strdup (mycrypt (cp));
		free (cp);
		if (!grp -> gr_passwd)
		    goto losing;
	    }
	    else
		grp -> gr_passwd = cp;
	    return int_SNMP_error__status_noError;	    

	case groupID:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		goto bad_value;
	    grp -> gr_gid = *((integer *) value);
	    (*os -> os_free) (value);
	    return int_SNMP_error__status_noError;

	case groupStatus:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK)
		goto bad_value;
	    i = *((integer *) value);
	    (*os -> os_free) (value);
	    switch (i) {
		case GR_OTHER:
    		case GR_INVALID:
		    gr -> gr_status = i;
		    break;

		default:
		    goto bad_value;
	    }
	    return int_SNMP_error__status_noError;

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

#define	grUserStatus	0


static int  o_gruser (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i;
    register unsigned int *ip,
			  *jp;
    register struct gu *gu;
    register OID    oid = oi -> oi_name;
    OID	    new;
    register OT	    ot = oi -> oi_type;

    if (get_gu (offset) == NOTOK)
	return generr (offset);

    ifvar = (int) ot -> ot_info;
    switch (offset) {
	case type_SNMP_SMUX__PDUs_get__request:
	    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
		return int_SNMP_error__status_noSuchName;
	    if ((gu = get_guent (oid -> oid_elements
				     + ot -> ot_name -> oid_nelem,
				 oid -> oid_nelem
				     - ot -> ot_name -> oid_nelem, 0)) == NULL)
		return int_SNMP_error__status_noSuchName;
	    break;

	case type_SNMP_SMUX__PDUs_get__next__request:
	    if (oid -> oid_nelem == ot -> ot_name -> oid_nelem) {
		if ((gu = gu_head) == NULL || gu -> gu_user == NULL)
		    return NOTOK;

		if ((new = oid_extend (oid, gu -> gu_insize)) == NULLOID)
		    return NOTOK;
		ip = new -> oid_elements + new -> oid_nelem - gu -> gu_insize;
		jp = gu -> gu_instance;
		for (i = gu -> gu_insize; i > 0; i--)
		    *ip++ = *jp++;
		
		if (v -> name)
		    free_SNMP_ObjectName (v -> name);
		v -> name = new;
	    }
	    else {
		int	j;

		if ((gu = get_guent (oid -> oid_elements
				         + ot -> ot_name -> oid_nelem,
				     j = oid -> oid_nelem
				             - ot -> ot_name -> oid_nelem, 1))
		        == NULL)
		    return NOTOK;

		if ((i = j - gu -> gu_insize) < 0) {
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
		jp = gu -> gu_instance;
		for (i = gu -> gu_insize; i > 0; i--)
		    *ip++ = *jp++;
	    }
	    break;

	default:
	    return int_SNMP_error__status_genErr;
    }

    switch (ifvar) {
	case grUserStatus:
	    return o_integer (oi, v, gu -> gu_status);

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

static int  s_gruser (oi, v, offset)
OI	oi;
register struct type_SNMP_VarBind *v;
int	offset;
{
    int	    ifvar;
    register int    i;
    register unsigned int *ip;
    char   *cp;
    register struct gu *gu;
    register OID    oid = oi -> oi_name;
    register OT	    ot = oi -> oi_type;
    register OS	    os = ot -> ot_syntax;
    caddr_t	value;

    if (get_gu (offset) == NOTOK)
	return int_SNMP_error__status_genErr;

    ifvar = (int) ot -> ot_info;
    if (oid -> oid_nelem <= ot -> ot_name -> oid_nelem)
	return int_SNMP_error__status_noSuchName;
    if ((gu = get_guent (ip = oid -> oid_elements + ot -> ot_name -> oid_nelem,
			 i = oid -> oid_nelem - ot -> ot_name -> oid_nelem, 0))
	    == NULL) {
	register unsigned int *jp;
	char    group[GU_SIZE + 1],
		user[PW_SIZE + 1];
	register struct gr *gr;

	if (i > GU_SIZE)
	    return int_SNMP_error__status_noSuchName;
	for (cp = group; i-- > 0; ip++, cp++) {
	    if (*ip == 0)
		break;
	    else
		if (*ip > 0xff || !isascii ((u_char) (*cp = *ip & 0xff)))
		    return int_SNMP_error__status_noSuchName;
	}
	*cp = NULL, ip++;
	for (gr = gr_head; gr -> gr_gr.gr_name; gr++)
	    if (strcmp (gr -> gr_gr.gr_name, group) == 0)
		break;
	if (!gr -> gr_gr.gr_name)
	    return int_SNMP_error__status_noSuchName;
	if (i > PW_SIZE)
	    return int_SNMP_error__status_noSuchName;
	for (cp = user; i-- > 0; ip++, cp++)
	    if (*ip > 0xff || !isascii ((u_char) (*cp = *ip & 0xff)))
		return int_SNMP_error__status_noSuchName;
	*cp = NULL;
	if (cp == user)
	    return int_SNMP_error__status_noSuchName;

	i = 0;
	if (gu_head)
	    for (gu = gu_head; gu -> gu_user; gu++)
		i++;
	if ((gu = (struct gu *) realloc ((char *) gu_head,
					(unsigned) (i + 2) * sizeof *gu_head))
	        == NULL) {
no_mem: ;
	    advise (LLOG_EXCEPTIONS, NULLCP, "out of memory");
	    return int_SNMP_error__status_genErr;
	}

	bzero ((char *)	(gu = (gu_head = gu) + i), sizeof *gu);
	gu -> gu_insize = oid -> oid_nelem - ot -> ot_name -> oid_nelem;
	for (ip = oid -> oid_elements + ot -> ot_name -> oid_nelem,
		    jp = gu -> gu_instance, i = gu -> gu_insize;
	         i > 0;
	         i--)
	    *jp++ = *ip++;
	gu -> gu_serial = gr -> gr_serial;
	if ((gu -> gu_user = strdup (user)) == NULL)
	    goto no_mem;
	gu -> gu_malloc = 1;
	gu -> gu_status = GU_OTHER;

	gu++;
	bzero ((char *) gu, sizeof *gu);
	if (gu - gu_head > 1)
	    qsort ((char *) gu_head, gu - gu_head, sizeof *gu_head, gu_compar);
	if (!(gu = get_guent (oid -> oid_elements
			          + ot -> ot_name -> oid_nelem,
			      oid -> oid_nelem
			          - ot -> ot_name -> oid_nelem, 0))) {
	    advise (LLOG_EXCEPTIONS, NULLCP,
		    "lost newly created group member");
	    free_gr ();
	    return int_SNMP_error__status_noSuchName;
	}
    }

    if (os == NULLOS) {
	advise (LLOG_EXCEPTIONS, NULLCP,
		"no syntax defined for object \"%s\"", ot -> ot_text);
	return int_SNMP_error__status_genErr;
    }

    gr_touched++;
    switch (ifvar) {
	case grUserStatus:
	    if ((*os -> os_decode) (&value, v -> value) == NOTOK) {
bad_value: ;
		free_gr ();

		return int_SNMP_error__status_badValue;
	    }
	    i = *((integer *) value);
	    (*os -> os_free) (value);
	    switch (i) {
		case GU_OTHER:
    		case GU_INVALID:
		    gu -> gu_status = i;
		    break;

		default:
		    goto bad_value;
	    }
	    return int_SNMP_error__status_noError;

	default:
	    return int_SNMP_error__status_noSuchName;
    }
}

/*  */

static char itoa64[] =
	"./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

long	random ();
char   *crypt ();


static char *mycrypt (s)
char   *s;
{
    long    v;
    char    salt[3];

    (void) srandom ((int) time ((long *) 0));
    salt[0] = itoa64[(v = random ()) & 0x3f];
    salt[1] = itoa64[(v >> 6) & 0x3f];
    salt[2] = NULL;

    return crypt (s, salt);
}

/*  */

int	init_users () {
    register OT	    ot;

    if (ot = text2obj ("userName"))
	ot -> ot_getfnx = o_user,
	ot -> ot_info = (caddr_t) userName;
    if (ot = text2obj ("userPasswd"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userPasswd;
    if (ot = text2obj ("userID"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userID;
    if (ot = text2obj ("userGroup"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userGroup;
    if (ot = text2obj ("userQuota"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userQuota;
    if (ot = text2obj ("userComment"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userComment;
    if (ot = text2obj ("userFullName"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userFullName;
    if (ot = text2obj ("userHome"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userHome;
    if (ot = text2obj ("userShell"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userShell;
    if (ot = text2obj ("userStatus"))
	ot -> ot_getfnx = o_user,
	ot -> ot_setfnx = s_user,
	ot -> ot_info = (caddr_t) userStatus;

    if (ot = text2obj ("groupName"))
	ot -> ot_getfnx = o_group,
	ot -> ot_info = (caddr_t) groupName;
    if (ot = text2obj ("groupPasswd"))
	ot -> ot_getfnx = o_group,
	ot -> ot_setfnx = s_group,
	ot -> ot_info = (caddr_t) groupPasswd;
    if (ot = text2obj ("groupID"))
	ot -> ot_getfnx = o_group,
	ot -> ot_setfnx = s_group,
	ot -> ot_info = (caddr_t) groupID;
    if (ot = text2obj ("groupStatus"))
	ot -> ot_getfnx = o_group,
	ot -> ot_setfnx = s_group,
	ot -> ot_info = (caddr_t) groupStatus;

    if (ot = text2obj ("grUserStatus"))
	ot -> ot_getfnx = o_gruser,
	ot -> ot_setfnx = s_gruser,
	ot -> ot_info = (caddr_t) grUserStatus;
}

/*  */

static int  pw_sort (a, b)
register struct pw *a,
		   *b;
{
    int	    i;

    if (a -> pw_pw.pw_uid == 0 || b -> pw_pw.pw_uid == 0) {
	if (strcmp (a -> pw_pw.pw_name, "root") == 0)
	    return (-1);
	if (strcmp (b -> pw_pw.pw_name, "root") == 0)
	    return (1);
    }

    if (i = a -> pw_pw.pw_uid - b -> pw_pw.pw_uid)
	return i;
    return strcmp (a -> pw_pw.pw_name, b -> pw_pw.pw_name);
}


static int  gr_sort (a, b)
register struct gr *a,
		   *b;
{
    int	    i;

    if (a -> gr_gr.gr_gid == 0 || b -> gr_gr.gr_gid == 0) {
	if (strcmp (a -> gr_gr.gr_name, "wheel") == 0)
	    return (-1);
	if (strcmp (b -> gr_gr.gr_name, "wheel") == 0)
	    return (1);
    }

    if (i = a -> gr_gr.gr_gid - b -> gr_gr.gr_gid)
	return i;
    return strcmp (a -> gr_gr.gr_name, b -> gr_gr.gr_name);
}


int	sync_users (cor)
integer	cor;
{
    int	    invalid,
	    iserr,
    	    ngr,
	    npw;
    char    tmpfil[BUFSIZ];
    register struct pw *pw;
    register struct gr *gr;
    register struct gu *gu;
    FILE   *fp;

    switch (cor) {
	case int_SNMP_SOutPDU_commit:
	    if (!pw_touched)
		goto check_gr;
	    invalid = 0;
	    (void) strcpy (tmpfil, "/etc/ptmpXXXXXX");
	    (void) unlink (mktemp (tmpfil));
	    if (!(fp = fopen (tmpfil, "w"))) {
		advise (LLOG_FATAL, tmpfil, "unable to write");
		goto flush_pw;
	    }
	    if (fchown (fileno (fp), pw_st.st_uid, pw_st.st_gid) == NOTOK)
		advise (LLOG_EXCEPTIONS, "",
			"unable to change owner of %s to %d:%d",
			tmpfil, pw_st.st_uid, pw_st.st_gid);
	    if (fchmod (fileno (fp), (int) (pw_st.st_mode & 0777)) == NOTOK)
		advise (LLOG_EXCEPTIONS, "",
			"unable to change mode of %s to 0%o",
			tmpfil, pw_st.st_mode & 0777);

	    for (pw = pw_head; pw -> pw_pw.pw_name; pw++)
		if (!pw -> pw_pw.pw_passwd)
		    fill_pw (&pw -> pw_pw);
	    if ((npw = pw - pw_head) > 1)
		qsort ((char *) pw_head, npw, sizeof *pw_head, pw_sort);

	    for (pw = pw_head; pw -> pw_pw.pw_name; pw++) {
		register struct passwd *pwp = &pw -> pw_pw;

		if (pw -> pw_status == PW_INVALID) {
		    invalid = 1;
		    continue;
		}
		fprintf (fp, "%s:%s:%d:%d:%s:%s:%s\n", pwp -> pw_name,
			 pwp -> pw_passwd, pwp -> pw_uid, pwp -> pw_gid,
			 pwp -> pw_gecos, pwp -> pw_dir, pwp -> pw_shell);
		if (pw -> pw_new
		        && mkdir (pwp -> pw_dir, 0777) != NOTOK
		        && chown (pwp -> pw_dir, pwp -> pw_uid, pwp -> pw_gid)
		    		== NOTOK)
		    advise (LLOG_EXCEPTIONS, pwp -> pw_dir,
			    "unable to set permissions for %s", pwp -> pw_dir);
	    }

	    (void) fflush (fp);
	    if (iserr = ferror (fp))
		advise (LLOG_FATAL, tmpfil, "error writing");
	    (void) fclose (fp);

	    if (iserr)
		(void) unlink (tmpfil);
	    else {
		if (access ("/etc/passwd.orig", F_OK) == NOTOK
		        && errno == ENOENT
		        && rename ("/etc/passwd.old", "/etc/passwd.orig")
		    		== NOTOK
			&& errno != ENOENT)
		    advise (LLOG_FATAL, "/etc/passwd.orig",
			    "unable to rename %s to", "/etc/passwd.old");
		if (rename ("/etc/passwd", "/etc/passwd.old") == NOTOK)
		    advise (LLOG_FATAL, "/etc/passwd.old",
			    "unable to rename %s to", "/etc/passwd");
		if (rename (tmpfil, "/etc/passwd") == NOTOK)
		    advise (LLOG_FATAL, "/etc/passwd",
			    "unable to rename %s to", tmpfil);
	    }
	    if (npw > 1)
		qsort ((char *) pw_head, npw, sizeof *pw_head, pw_compar);

flush_pw: ;
	    if (invalid)
		free_pw ();

check_gr: ;
	    if (!gr_touched)
		break;
	    invalid = 0;
	    (void) strcpy (tmpfil, "/etc/gtmpXXXXXX");
	    (void) unlink (mktemp (tmpfil));
	    if (!(fp = fopen (tmpfil, "w"))) {
		advise (LLOG_FATAL, tmpfil, "unable to write");
		goto flush_gr;
	    }
	    if (fchown (fileno (fp), gr_st.st_uid, gr_st.st_gid) == NOTOK)
		advise (LLOG_EXCEPTIONS, "",
			"unable to change owner of %s to %d:%d",
			tmpfil, gr_st.st_uid, gr_st.st_gid);
	    if (fchmod (fileno (fp), (int) (gr_st.st_mode & 0777)) == NOTOK)
		advise (LLOG_EXCEPTIONS, "",
			"unable to change mode of %s to 0%o",
			tmpfil, gr_st.st_mode & 0777);

	    for (gr = gr_head; gr -> gr_gr.gr_name; gr++) {
#ifdef	not_needed	/* call to get_gu (offset) from s_group does this... */
		if (!gr -> gr_gr.gr_passwd)
		    fill_gr (&gr -> gr_gr);
#endif
		continue;
	    }
	    if ((ngr = gr - gr_head) > 1)
		qsort ((char *) gr_head, ngr, sizeof *gr_head, gr_sort);

	    for (gr = gr_head; gr -> gr_gr.gr_name; gr++) {
		char   *cp = "";
		register struct group *grp = &gr -> gr_gr;

		if (gr -> gr_status == GR_INVALID) {
		    invalid = 1;
		    continue;
		}
		fprintf (fp, "%s:%s:%d:", grp -> gr_name, grp -> gr_passwd,
			 grp -> gr_gid);
		if (gu_head)
		    for (gu = gu_head; gu -> gu_user; gu++)
			if (gu -> gu_serial == gr -> gr_serial
			        && gu -> gu_status == GU_OTHER) {
			    fprintf (fp, "%s%s", cp, gu -> gu_user);
			    cp = ":";
			}
		fprintf (fp, "\n");
	    }

	    (void) fflush (fp);
	    if (iserr = ferror (fp))
		advise (LLOG_FATAL, tmpfil, "error writing");
	    (void) fclose (fp);
	    if (iserr)
		(void) unlink (tmpfil);
	    else {
		if (access ("/etc/group.orig", F_OK) == NOTOK
		        && errno == ENOENT
		        && rename ("/etc/group.old", "/etc/group.orig")
		    		== NOTOK
			&& errno != ENOENT)
		    advise (LLOG_FATAL, "/etc/group.orig",
			    "unable to rename %s to", "/etc/group.old");
		if (rename ("/etc/group", "/etc/group.old") == NOTOK)
		    advise (LLOG_FATAL, "/etc/group.old",
			    "unable to rename %s to", "/etc/group");
		if (rename (tmpfil, "/etc/group") == NOTOK)
		    advise (LLOG_FATAL, "/etc/group", "unable to rename %s to",
			    tmpfil);
	    }
	    if (ngr)
		qsort ((char *) gr_head, ngr, sizeof *gr_head, gr_compar);

flush_gr: ;
	    if (!invalid && gu_head)
		for (gu = gu_head; gu -> gu_user; gu++)
		    if (gu -> gu_status == GU_INVALID) {
			invalid = 1;
			break;
		}
	    if (invalid)
		free_gr ();
	    break;

	case int_SNMP_SOutPDU_rollback:
	    if (pw_touched)
		free_pw ();
	    if (gr_touched)
		free_gr ();
	    break;
    }

    pw_touched = 0;
    if (pw_fd != NOTOK) {
	(void) fstat (pw_fd, &pw_st);
	(void) flock (pw_fd, LOCK_UN);
	(void) close (pw_fd), pw_fd = NOTOK;
    }

    gr_touched = 0;
    if (gr_fd != NOTOK) {
	(void) fstat (gr_fd, &gr_st);
	(void) flock (gr_fd, LOCK_UN);
	(void) close (gr_fd), gr_fd = NOTOK;
    }
}
