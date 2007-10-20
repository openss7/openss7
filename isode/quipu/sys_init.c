/* sys_init.c - System tailoring initialisation */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/quipu/RCS/sys_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/quipu/RCS/sys_init.c,v 9.0 1992/06/16 12:34:01 isode Rel $
 *
 *
 * $Log: sys_init.c,v $
 * Revision 9.0  1992/06/16  12:34:01  isode
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


#include <stdio.h>
#include "quipu/oid.h"
#include "quipu/config.h"
#include "tailor.h"
#include "logger.h"
#include "usr.dirent.h"
#include <sys/stat.h>

extern char *dsaoidtable;
extern LLog * log_dsap;
extern time_t cache_timeout;
extern time_t retry_timeout;
extern time_t slave_timeout;
extern char * edbtmp_path;
extern char * treedir;

extern int errno;

static int rmFiles();

dsa_sys_init(acptr,avptr)
int *acptr;
char *** avptr;
{
char *name;
char **ptr;
int cnt;
extern int parse_line;
extern char dsa_mode;

#ifdef  TURBO_DISK
extern char quipu_faststart;

	quipu_faststart = 1;
#endif

	parse_line = 0;		/* stop 'line 1:' being printed in tailor file errors */
	dsa_mode = 1;

	name = **avptr;

	DLOG (log_dsap,LLOG_TRACE,("Initialisation"));

	cnt = *acptr;
	ptr = *avptr;
	dsa_tai_args (acptr,avptr);

	if (dsa_tai_init(name) != OK)
		fatal (-43,"Tailoring failed");

	dsa_tai_args (&cnt,&ptr);    /* second call IS needed !!! */

	DLOG (log_dsap,LLOG_TRACE,("Loading oid table (%s)",dsaoidtable));

	if (load_oid_table (dsaoidtable) == NOTOK)
		fatal (-43, "Can't load oid tables");

	if (retry_timeout == (time_t)0)
		retry_timeout = cache_timeout;

	if (slave_timeout == (time_t)0)
		slave_timeout = cache_timeout;

}


void mk_dsa_tmp_dir()
{
struct stat statbuf;
char edbtmp_buf[BUFSIZ];
char err_buf[BUFSIZ];

	(void) sprintf (edbtmp_buf, "%stmp", treedir);

	(void) strcat (edbtmp_buf, "/");
	edbtmp_path = strdup (edbtmp_buf);
	edbtmp_buf[strlen(edbtmp_path) - 1] = 0;	/* remove "/" */

	if ((stat(edbtmp_buf, &statbuf) == OK) 
	    && ((statbuf.st_mode & S_IFMT) == S_IFDIR)) {
		    /* tmpdir exists - clean it */
		struct dirent **namelist;
		(void) _scandir(edbtmp_buf, &namelist, rmFiles, NULLIFP);
		if (namelist)
		    free((char *) namelist);

	} else if (mkdir (edbtmp_buf,0700) != 0) {
		(void)sprintf (err_buf,"Can't create tmp directory: %s (%d)",
			       edbtmp_path,errno);
		fatal (-43,err_buf);
	}
}

static int rmFiles(entry)
struct dirent *entry;
{
	char cbuf[BUFSIZ];

	if (*entry->d_name == '.' &&
	    (strcmp(entry->d_name,".") == 0)
	    || (strcmp(entry->d_name,"..") == 0))
		return 0;

	(void) strcpy (cbuf, edbtmp_path);
	(void) strcat (cbuf, entry -> d_name);

	if (unlink(cbuf) == NOTOK) {
	        LLOG (log_dsap,LLOG_EXCEPTIONS,
		      ("remove failure \"%s\")",cbuf,errno));
		return 0;
	}

	return 0;
}


