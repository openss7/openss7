/* idistd.c - remote distribution -- responder */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/others/idist/RCS/idistd.c,v 9.0 1992/06/16 14:38:53 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/others/idist/RCS/idistd.c,v 9.0 1992/06/16 14:38:53 isode Rel $
 *
 * Idist daemon - this module handles the remote operations as they
 * are received. It runs as a state machine for file transfer
 * expecting the sequence transfer, data..., terminate for each file
 * transfered. This module has only slight correlation with the
 * original UCB version. It is entirely client driven. This server is
 * not designed to run other than under tsapd. Two reasons for this :-
 * 1) There is a lot of state info kept around in global variables
 * (this is fixable if required)
 * 2) Its first operation is to setuid to the authenticated user. This
 * is a one way operation - thus 1) is not really worth fixing (IMHO).
 *
 * Julian Onions <jpo@cs.nott.ac.uk>
 * Nottingham University Computer Science.
 * 
 *
 * $Log: idistd.c,v $
 * Revision 9.0  1992/06/16  14:38:53  isode
 * Release 8.0
 *
 *
 */

#include <stdio.h>
#include <varargs.h>
#include "Idist-types.h"      /* type definitions */
#include "Idist-ops.h"                /* operation definitions */
#include "ryresponder.h"      /* for generic idempotent responders */
#include "defs.h"

/*    DATA */

static char *myservice = "isode idist";
static char *mycontext = "isode idist";

extern	struct type_Idist_QueryResult *query ();
extern	struct type_Idist_FileList *do_listcdir ();

static int error (), i_strerror (), syserror (), ureject ();

					/* OPERATIONS */
int	op_init (), op_transfer (), op_terminate (), op_listcdir (),
	op_query (), op_special (), op_data (), op_deletefile ();

static struct dispatch dispatches[] = {
    "init", operation_Idist_init, op_init,

    "transfer", operation_Idist_transfer, op_transfer,

    "terminate", operation_Idist_terminate, op_terminate,

    "listcdir",	operation_Idist_listcdir, op_listcdir,

    "deletefile", operation_Idist_deletefile, op_deletefile,

    "query", operation_Idist_query, op_query,

    "special", operation_Idist_special, op_special,

    "data", operation_Idist_data, op_data,
    
    NULL
};

int catname = 0;
char target[BUFSIZ];
extern	char	*tp;
extern	char	*stp[];
FILE	*cfile;		/* the currently open file */
static struct type_Idist_FileSpec *cpy_fs();
struct type_Idist_FileSpec *cfiletype;
int oumask;
char    utmpfile[] = "/tmp/idistXXXXXX";
char    *tmpname = &utmpfile[5];
struct type_Idist_IA5List *ia5list;

char	*host;
int groupid, userid;
char homedir[BUFSIZ];
char user[100];
struct passwd *pw;
struct group *gr;

/*    MAIN */

/* ARGSUSED */

main (argc, argv, envp)
int	argc;
char  **argv,
      **envp;
{
	int initiate ();
	oumask = umask (0);

	host = getlocalhost ();
		
	(void) ryresponder (argc, argv, PLocalHostName (), myservice, mycontext,
			    dispatches, table_Idist_Operations, initiate,
			    NULLIFP);

	exit (0);		/* NOTREACHED */
}

/*    OPERATIONS */

static int  op_init (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	char	*str;
	register struct type_Idist_InitDir *arg =
		(struct type_Idist_InitDir *) in;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
			"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
			sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);

	if (arg -> offset == type_Idist_InitDir_destdir) {
		catname = 1;
		str = qb2str (arg -> un.destdir);
	}
	else {
		catname = 0;
		str = qb2str (arg ->un.nodestdir);
	}
	if (exptilde (target, str) == NULL)
		return error (sd, error_Idist_badfilename, (caddr_t)ia5list,
			      rox, roi);

	tp = target + strlen (target);

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) NULL, ROS_NOPRIO,
			roi) == NOTOK)
		ros_adios (&roi -> roi_preject, "RESULT");
    
	return OK;
}

op_transfer (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	register struct type_Idist_FileSpec *arg =
		(struct type_Idist_FileSpec *) in;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
			"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
			sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);
	
	switch (arg -> filetype -> parm) {

	    case int_Idist_FileType_regular:
		if (do_rfile (arg) == NOTOK)
			return error (sd, error_Idist_badfilename,
				      (caddr_t)ia5list, rox, roi);
		break;

	    case int_Idist_FileType_directory:
		if (do_direct (arg) == NOTOK)
			return error (sd, error_Idist_badfilename,
				      (caddr_t)ia5list, rox, roi);
		break;

	    case int_Idist_FileType_symlink:
		if (do_symlink (arg) == NOTOK)
			return error (sd, error_Idist_badfilename,
				      (caddr_t) ia5list, rox, roi);
		break;

	    case int_Idist_FileType_hardlink:
		if ( do_hardlink (arg) == NOTOK)
			return error (sd, error_Idist_badfilename,
				      (caddr_t) ia5list, rox, roi);
		break;

	    default:
		return error (sd, error_Idist_badfiletype, (caddr_t) NULL,
			      rox, roi);
	}

	if (cfiletype != NULL)
		free_Idist_FileSpec (cfiletype);
	cfiletype = cpy_fs (arg);;

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) ia5list, ROS_NOPRIO, roi)
	    == NOTOK)
		ros_adios (&roi -> roi_preject, "RESULT");

	free_Idist_IA5List (ia5list);
	ia5list = NULL;

	return OK;
}

op_data (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	register struct type_Idist_Data *arg =
		(struct type_Idist_Data *) in;
	register struct qbuf *qb;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
			"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
			sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);
	for (qb = arg -> qb_forw; qb != arg; qb = qb -> qb_forw) {
		if (fwrite (qb -> qb_data, sizeof (char),
			    qb -> qb_len, cfile) != qb -> qb_len)
			return syserror (sd, error_Idist_writeerror,
					 rox, roi);
	}
	if (RyDsResult (sd, rox -> rox_id, (caddr_t) NULL, ROS_NOPRIO, roi)
	    == NOTOK)
		ros_adios (&roi -> roi_preject, "RESULT");

	return OK;
}

op_query (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	register struct type_UNIV_IA5String *arg =
		(struct type_UNIV_IA5String *) in;
	struct type_Idist_QueryResult *qr;
	char	*str;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
			"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
			sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);

	str = qb2str (arg);
	qr = query (str);
	free (str);

	if (qr == NULL)
		return error (sd, error_Idist_congested, (caddr_t)ia5list,
			      rox, roi);

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) qr, ROS_NOPRIO, roi)
	    == NOTOK)
		ros_adios (&roi -> roi_preject, "RESULT");

	free_Idist_QueryResult (qr);

	return OK;
}

op_terminate (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	register struct type_Idist_TermStatus *arg =
		(struct type_Idist_TermStatus *) in;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
			"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
			sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);

	switch (arg -> filetype -> parm) {
	    case int_Idist_FileType_regular:
		if (cfile == NULL)
			return i_strerror (sd, error_Idist_protocol,
					 "File not open", rox, roi);
		(void) fflush (cfile);
		if (ferror (cfile))
			return syserror (sd, error_Idist_writeerror, rox, roi);
		(void) fclose (cfile);
		if ( fixup () < 0)
			return error (sd, error_Idist_fileproblem,
				      (caddr_t) ia5list, rox, roi);
		break;

	    case int_Idist_FileType_directory:
		*tp = '\0';
		if (catname <= 0)
			return i_strerror (sd, error_Idist_protocol,
					 "Too many directory levels popped",
					 rox, roi);
		tp = stp[--catname];
		*tp = '\0';
		break;

	    case int_Idist_FileType_symlink:
	    case int_Idist_FileType_hardlink:
		return i_strerror (sd, error_Idist_protocol,
				 "Bad file type for terminate operation",
				 rox, roi);

	    default:
		return error (sd, error_Idist_badfiletype, (caddr_t)NULL,
			      rox, roi);
	}

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) NULL, ROS_NOPRIO, roi)
	    == NOTOK)
		ros_adios (&roi -> roi_preject, "RESULT");

	return OK;
}

op_special (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	register struct type_UNIV_IA5String *arg =
		(struct type_UNIV_IA5String *) in;
	int	result;
	char	*str;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
			"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
			sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);

	str = qb2str (arg);
	result = doexec (str);
	free (str);

	if (result == NOTOK)
		return syserror (sd, error_Idist_execError, rox, roi);

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) ia5list, ROS_NOPRIO, roi)
	    == NOTOK)
		ros_adios (&roi -> roi_preject, "RESULT");

	free_Idist_IA5List (ia5list);
	ia5list = NULL;

	return OK;
}

op_deletefile (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	register struct type_UNIV_IA5String *arg =
		(struct type_UNIV_IA5String *) in;
	int	result;
	char	*str;
	char	buf[BUFSIZ];

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
			"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
			sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);

	str = qb2str (arg);
	(void) sprintf (buf, "%s/%s", target, str);
	free (str);
	result = i_remove (buf);

	if (result == NOTOK)
		return error (sd, error_Idist_badfilename, (caddr_t) ia5list,
			      rox, roi);

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) ia5list, ROS_NOPRIO, roi)
	    == NOTOK)
		ros_adios (&roi -> roi_preject, "RESULT");

	free_Idist_IA5List (ia5list);
	ia5list = NULL;
	return OK;
}

/* ARGSUSED */
op_listcdir (sd, ryo, rox, in, roi)
int	sd;
struct RyOperation *ryo;
struct RoSAPinvoke *rox;
caddr_t	in;
struct RoSAPindication *roi;
{
	struct type_Idist_FileList *fl;

	if (rox -> rox_nolinked == 0) {
		advise (LLOG_NOTICE, NULLCP,
			"RO-INVOKE.INDICATION/%d: %s, unknown linkage %d",
			sd, ryo -> ryo_name, rox -> rox_linkid);
		return ureject (sd, ROS_IP_LINKED, rox, roi);
	}
	if (debug)
		advise (LLOG_DEBUG, NULLCP, "RO-INVOKE.INDICATION/%d: %s",
			sd, ryo -> ryo_name);

	fl = do_listcdir ();

	if (RyDsResult (sd, rox -> rox_id, (caddr_t) fl, ROS_NOPRIO, roi)
	    == NOTOK)
		ros_adios (&roi -> roi_preject, "RESULT");

	free_Idist_FileList (fl);

	return OK;
}


/*    ERROR */

static int  error (sd, err, param, rox, roi)
int	sd,
	err;
caddr_t	param;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    if (RyDsError (sd, rox -> rox_id, err, param, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "ERROR");

    if (ia5list)
	    free_Idist_IA5List (ia5list);
    ia5list = NULL;

    return OK;
}

static int i_strerror (sd, err, str, rox, roi)
int	sd, err;
char	*str;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
	addtoia5 (str, strlen(str));

	return error (sd, err, (caddr_t)ia5list, rox, roi);
}

static int syserror (sd, err, rox, roi)
int	sd, err;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
	extern	int errno;

	return i_strerror (sd, err, sys_errname (errno), rox, roi);
}
	

/*    U-REJECT */

static int  ureject (sd, reason, rox, roi)
int	sd,
	reason;
struct RoSAPinvoke *rox;
struct RoSAPindication *roi;
{
    if (RyDsUReject (sd, rox -> rox_id, reason, ROS_NOPRIO, roi) == NOTOK)
	ros_adios (&roi -> roi_preject, "U-REJECT");

    return OK;
}

/*  Initialisation stuff */

/* ARGSUSED */
initiate (sd, acs, pe)
int	sd;
struct AcSAPstart *acs;
PE	*pe;
{
	struct type_Idist_Initiate *initial;
	char	*cp;

	*pe  = NULLPE;
	if ( acs -> acs_ninfo != 1)
		return init_lose (ACS_PERMANENT, pe, "No Association data");

	if (decode_Idist_Initiate (acs -> acs_info[0], 1, NULLIP, NULLVP,
				   &initial) == NOTOK)
		return init_lose (ACS_PERMANENT, pe,
				  "Can't parse initial data");

	if (initial -> version != VERSION)
		return init_lose (ACS_PERMANENT, pe, "Version mismatch");

	cp = qb2str (initial -> user);
	(void) strcpy (user, cp);
	free (cp);
	
	if (baduser (NULLCP, user)) {
	    advise (LLOG_EXCEPTIONS, NULLCP, "Bad listed user '%s'", user);
	    return init_lose (ACS_PERMANENT, pe, "Bad user/password");
	}

	if ((pw = getpwnam (user)) == NULL) {
		advise (LLOG_NOTICE, NULLCP, "Unknown user '%s'", user);
		return init_lose (ACS_PERMANENT, pe, "Bad user/password");

	}

	userid = pw -> pw_uid;
	groupid = pw -> pw_gid;
	(void) strcpy (homedir, pw -> pw_dir);

	cp = qb2str (initial -> passwd);

	if (pw -> pw_passwd == NULL
	            || !chkpassword (user, pw -> pw_passwd, cp)) {
		advise (LLOG_NOTICE, NULLCP, "Password mismatch for %s", user);
		return init_lose (ACS_PERMANENT, pe, "Bad user/password");

	}
	bzero (cp, strlen(cp));	/* in case of cores */
	free (cp);

	free_Idist_Initiate (initial);

	if (chdir (homedir) == -1) {
		advise (LLOG_NOTICE, NULLCP, "Can't set home directory to '%s'",
			homedir);
		return init_lose (ACS_PERMANENT, pe, "No home directory");
	}

#ifdef SVR4
	if (setuid (userid) < 0) {
#else
	if (setreuid (userid, userid) < 0) {
#endif
		advise (LLOG_NOTICE, NULLCP, "Cant set userid %d for %s",
			userid, user);
		return init_lose (ACS_PERMANENT, pe, "Can't set user id");
	}
	
	(void) mktemp (utmpfile);

	return ACS_ACCEPT;
}

init_lose (type, pe, str)
int	type;
PE	*pe;
char	*str;
{
	*pe = ia5s2prim (str, strlen(str));
	(*pe) -> pe_context = 3;	/* magic!! - don't ask me why */
	return type;
}


#define SIZEOFQB(qb)  (sizeof (struct qbuf) +  (qb && qb->qb_data ? qb->qb_len \
				: 0))

static struct qbuf *
qb_cpy(qb)
struct qbuf	*qb;
{
    struct qbuf	*qp;
    struct qbuf	*nqb;
    struct qbuf	*nqp;
    struct qbuf	*pred;

    if (qb == (struct qbuf *)0)
	return ((struct qbuf *)0);
    nqb = (struct qbuf *) smalloc(SIZEOFQB(qb));
    nqb->qb_len = qb->qb_len;
    if (qb->qb_data) {
	nqb->qb_data = nqb->qb_base;
	bcopy(qb->qb_data, nqb->qb_data, qb->qb_len);
    } else
	nqb->qb_data = NULLCP;
    nqb->qb_forw = nqb;
    nqb->qb_back = nqb;
    pred = nqb;
    for (qp = qb->qb_forw; qp != qb; qp = qp->qb_forw) {
	nqp = (struct qbuf *) smalloc(SIZEOFQB(qp));
	nqp->qb_len = qp->qb_len;
	if (qp->qb_data) {
	    nqp->qb_data = nqp->qb_base;
	    bcopy(qp->qb_data, nqp->qb_data, qp->qb_len);
	} else
	    nqp->qb_data = NULLCP;
	insque(nqp, pred);
	pred = nqp;
    }

    return (nqb);
}

static struct type_Idist_FileSpec *cpy_fs (ft)
struct type_Idist_FileSpec *ft;
{
	struct type_Idist_FileSpec *new;

	new = (struct type_Idist_FileSpec *)calloc (1, sizeof *new);

	if (ft -> filetype) {
		new -> filetype = (struct type_Idist_FileType *)
			calloc (1, sizeof *new -> filetype);
		new -> filetype -> parm = ft -> filetype -> parm;
	}
	if (ft -> fileopts)
		new -> fileopts = pe_cpy (ft -> fileopts);

	new -> filemode = ft -> filemode;
	new -> filesize = ft -> filesize;
	if (ft -> filemtime) {
		new -> filemtime = (struct type_Idist_FileTime *)
			calloc (1, sizeof *new -> filemtime);
		new -> filemtime -> parm = ft -> filemtime -> parm;
	}

	new -> fileowner = qb_cpy (ft -> fileowner);
	new -> filegroup = qb_cpy (ft -> filegroup);
	new -> filename = qb_cpy (ft -> filename);
	new -> linkname = qb_cpy (ft -> linkname);

	return new;
}
