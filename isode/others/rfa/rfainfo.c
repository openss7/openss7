/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * rfainfo.c : functions to manipulate fileinfo structure and files
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * $Header: /xtel/isode/isode/others/rfa/RCS/rfainfo.c,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: rfainfo.c,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid = "$Header: /xtel/isode/isode/others/rfa/RCS/rfainfo.c,v 9.0 1992/06/16 12:47:25 isode Rel $";
#endif

/*
 *                              NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "sys.file.h"
#include <sys/time.h>
#include <ctype.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <unistd.h>
#include <signal.h>
#include "psap.h"
#include "logger.h"
#include "rfainfo.h"
#include "rfa.h"

#define MAXSTAT 4

struct LockEntry {
	FILE			*le_filep;
	char			 le_fn[512];
	struct LockEntry 	*le_next;
} static *lockList = NULL;

static int timedout = 0;

extern char *sys_errname();
char rfaErrStr[512];

/*------------------------------------------------------
 * str2status - convert status string to integer code
 *------------------------------------------------------*/
int str2status (stat)
    char *stat;
{
    int i = 0;

    switch(*(stat++)) {
	case '-':
	    SET_STATUS(i, RI_UNREGISTERED); 
	    break;
	case 'M':
	case 'm':
	    SET_STATUS(i, RI_MASTER); 
	    break;
	case 'S':
	case 's':
	    SET_STATUS(i, RI_SLAVE); 
	    break;
	default:
	    return NOTOK;
    }
    switch(*(stat++)) {
	case '-':
	    SET_LOCKINFO(i, RI_UNLOCKED); 
	    break;
	case 'L':
	case 'l':
	    SET_LOCKINFO(i, RI_LOCKED); 
	    break;
	default:
	    return NOTOK;
    }
    switch(*(stat++)) {
	case '-':
	    SET_TRANSFER(i, RI_TR_REQ); 
	    break;
	case 'A':
	case 'a':
	    SET_TRANSFER(i, RI_TR_AUTO); 
	    break;
	default:
	    return NOTOK;
    }
    return i;
}

/*------------------------------------------------------
 * status2str - convert status to string
 *------------------------------------------------------*/
char *status2str(stat)
    int stat;
{
    static char sbuf[5];
    register char *s = sbuf;

    switch(RI_STATUS(stat)) {
	case RI_UNREGISTERED: 
		*s = '-';
		break;
	case RI_MASTER: 
		*s = 'M';
		break;
	case RI_SLAVE: 
		*s = 'S';
		break;
	default:
		sbuf[0] = '?';
    }
    s++;
    switch(RI_LOCKINFO(stat)) {
	case RI_LOCKED: 
		*s = 'L';
		break;
	case RI_UNLOCKED: 
		*s = '-';
		break;
	default:
		*s = '?';
    }
    s++;
    switch(RI_TRANSFER(stat)) {
	case RI_TR_AUTO: 
		*s = 'A';
		break;
	case RI_TR_REQ: 
		*s = '-';
		break;
	default:
		*s = '?';
    }
    *(++s) = '\0';
    return sbuf;
}

/*------------------------------------------------------
 * mallocRfaInfo - malloc list elements
 *------------------------------------------------------*/
struct RfaInfo *mallocRfaInfo (fn)
    char *fn;
{
    register struct RfaInfo *rfa;

    if ((rfa = (struct RfaInfo *) malloc(sizeof(struct RfaInfo))) == NULL) {
	sprintf(rfaErrStr, "out of memory");
	advise(LLOG_EXCEPTIONS,NULLCP,rfaErrStr);
	return NULL;
    }
    bzero((char *)rfa, sizeof(struct RfaInfo));
    rfa->ri_filename = fn;
    return rfa;
}

	
/*------------------------------------------------------
 * freeRfaInfoList - free list elements
 *------------------------------------------------------*/
void freeRfaInfoList (rfa)
   struct RfaInfo *rfa;
{
   struct RfaInfo *r;

   for (; rfa; rfa = r) {
	if (rfa->ri_filename)
	    free(rfa->ri_filename);
	if (rfa->ri_lckname)
	    free(rfa->ri_lckname);
	if (rfa->ri_owner)
	    free(rfa->ri_owner);
	if (rfa->ri_group)
	    free(rfa->ri_group);
	r = rfa->ri_next;
	free((char *)rfa);
    }
}


/*------------------------------------------------------
 * lock_timeout - handler for lock timeout
 *------------------------------------------------------*/
/* ARGSUSED */
SFD lock_timeout(sig) 
int sig;
{
    timedout++;
}


/*------------------------------------------------------
 * lckRfainfo 
 *------------------------------------------------------*/
static struct LockEntry *lockRfainfo(fn, fp)
    char *fn;
    FILE *fp;
{
    struct LockEntry *le;
    struct itimerval itv;

    if ((le = (struct LockEntry *)malloc(sizeof(struct LockEntry))) == NULL) {
	sprintf(rfaErrStr, "can't lock (no memory)"); 
	advise (LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
	return NULL;
    }
    strcpy (le->le_fn, fn);
    le->le_filep = fp;

    /*-- set timer --*/
    itv.it_interval.tv_sec = 0L;
    itv.it_interval.tv_usec = 0L;
    itv.it_value.tv_sec = LOCK_TIMEOUT;
    itv.it_value.tv_usec = 0L;
    if (setitimer(ITIMER_REAL, &itv, (struct itimerval *)NULL) != -1) {
	signal(SIGALRM, lock_timeout);
    }

    timedout = 0;
    if (lockf(fileno(fp), F_LOCK, 0L) == -1) {
	if (timedout)	
	    sprintf(rfaErrStr, "lock timed out");
	else
	    sprintf(rfaErrStr, "can't lock (%s)", sys_errname(errno));
	advise (LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
	free((char *)le);
	itv.it_value.tv_sec = 0L;
	setitimer(ITIMER_REAL, &itv, (struct itimerval *)NULL);
	signal(SIGALRM, SIG_DFL);
	return NULL;
    }
    itv.it_value.tv_sec = 0L;
    setitimer(ITIMER_REAL, &itv, (struct itimerval *)NULL);
    signal(SIGALRM, SIG_DFL);

    le->le_next = lockList;
    lockList = le;
    return le;
}

/*------------------------------------------------------
 * closeAndUnlockRfainfo
 *------------------------------------------------------*/
static int closeAndUnlockRfainfo(fn)
    char *fn;
{
    struct LockEntry *le, **lep;

    for (lep = &lockList; *lep; lep = &((*lep)->le_next))
	if (strcmp((*lep)->le_fn, fn) == 0) {
	    le = *lep;
	    *lep = le->le_next;
	    if (lockf(fileno(le->le_filep), F_ULOCK, 0L) == -1)  {
		sprintf(rfaErrStr, "can't unlock (%s)",sys_errname(errno));
		advise (LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
		if (le->le_filep)
		    fclose(le->le_filep);
		free((char *)le);
		return NOTOK;
	    }
	    if (le->le_filep)
		fclose(le->le_filep);
	    free((char *)le);
	    return OK;
	}
    sprintf(rfaErrStr, "can't unlock (not locked)");
    advise (LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
    return NOTOK;
}


/*------------------------------------------------------
 * releaseRfaInfoList - unlock list and free it
 *------------------------------------------------------*/
void releaseRfaInfoList(fn, rfa)
    char *fn;
    struct RfaInfo *rfa;
{
    closeAndUnlockRfainfo(fn);
    freeRfaInfoList(rfa);
}

/*------------------------------------------------------
 * statFile - return RfaInfo with results of stat(2) for "fn"
 *------------------------------------------------------*/
static int statFile(fn, rfa)
    char *fn;
    struct RfaInfo *rfa;
{
    struct stat st;
    struct group *gr, *getgrgid();
    struct passwd *pw, *getpwuid();
    char   lnkbuf[BUFSIZ];
    char   buf[100];
    int    rc;
    static char lastuname[100], lastgname[100];
    static int lastuid = -1, lastgid = -1;

    /*advise (LLOG_DEBUG, NULLCP, "statFile:  '%s'", fn);*/

    if (lstat(fn,&st) == -1) {
	advise (LLOG_EXCEPTIONS, NULLCP, "cant stat '%s':%s", fn,
		sys_errname(errno));
	sprintf(rfaErrStr, "%s (%s)", fn, sys_errname(errno));
	return NOTOK;
    }
    rfa->ri_size = st.st_size;
    rfa->ri_accTime = st.st_atime;
    rfa->ri_modTime = st.st_mtime;
    rfa->ri_mode = st.st_mode;

    /*--- get user and group of owner ---*/
    if(st.st_uid == lastuid)
	rfa->ri_owner = strdup(lastuname);
    else 
	if ((pw = getpwuid (lastuid = st.st_uid)) == NULL) {
	    advise (LLOG_EXCEPTIONS, NULLCP, "Unknown user-id '%d'", lastuid);
	    sprintf(lastuname, "uid %d", lastuid);
	    rfa->ri_owner = strdup(lastuname);
	} else {
	    rfa->ri_owner = strdup(pw->pw_name);
	    strcpy(lastuname, pw->pw_name);
	}
    if(st.st_gid == lastgid)
	rfa->ri_group = strdup(lastgname);
    else 
	if ((gr = getgrgid (lastgid = st.st_gid)) == NULL) {
	    advise (LLOG_EXCEPTIONS, NULLCP, "Unknown group-id '%d'", lastgid);
	    sprintf(buf, "group-id %d", st.st_gid);
	    rfa->ri_group = strdup(buf);
	} else {
	    rfa->ri_group = strdup(gr->gr_name);
	    strcpy(lastgname, gr->gr_name);
	}

    /*--- read symbolic links ---*/
    if ((rfa->ri_mode & S_IFMT) == S_IFLNK) {
	if ((rc = readlink(fn, lnkbuf, sizeof lnkbuf)) == -1) 
	    sprintf(lnkbuf, "(error reading link)");
	else
	    lnkbuf[rc] = '\0';
	rfa->ri_lnkName = strdup(lnkbuf);
    }
    
    return OK;
}


/*------------------------------------------------------
 * getRfaInfo - read RFA file info from ".rfainfo"
 *------------------------------------------------------*/
static int getRfaInfo(dirname, f, rfap)
    char *dirname;
    FILE *f;
    struct RfaInfo **rfap;
{
    register char *s, *d;
    char buf[BUFSIZ];
    char line[BUFSIZ];
    int rc = 1;
    struct RfaInfo *rfa;

    advise(LLOG_DEBUG,NULLCP,"getRfaInfo: %s",dirname);

    if (f == NULL)
	return OK;

    while (fgets(line, sizeof(line), f)) {
	s = line;

	while (isspace(*s))
	    s++;
	for(d = buf; ! isspace(*s); s++, d++)
	    *d = *s;
	*d = '\0';
	if ((rfa = findRfaInfo(buf, *rfap)) == NULL) 
	    continue;

	while (isspace(*s))
	    s++;
	for(d = buf; ! isspace(*s); s++, d++)
	    *d = *s;
	*d = '\0';
	if ((rc = str2status(buf)) == NOTOK) {
	    sprintf(rfaErrStr, "invalid status in '%s/.rfainfo'",dirname);
	    advise(LLOG_EXCEPTIONS,NULLCP,rfaErrStr);
	    continue;
	}
	rfa->ri_status = rc;

	while (isspace(*s))
	    s++;
	for(d = buf; ! isspace(*s); s++, d++)
	    *d = *s;
	*d = '\0';
	rfa->ri_lastChange = (time_t)atol(buf);

	while (isspace(*s))
	    s++;
	for(d = buf; ! isspace(*s); s++, d++)
	    *d = *s;
	*d = '\0';
	if (strcmp(buf, "NONE"))
	    rfa->ri_lckname = strdup(buf);
	else
	    rfa->ri_lckname = NULL;
		

	while (isspace(*s))
	    s++;
	for(d = buf; ! isspace(*s); s++, d++)
	    *d = *s;
	*d = '\0';
	rfa->ri_lcksince = (time_t)atol(buf);

    }  /*-- while fgets --*/
    return OK;
}

/*------------------------------------------------------
 * getRfaInfoList - get file info list for "dir"
 *------------------------------------------------------*/
int getRfaInfoList(dir, rfaHeadp, target, locked)
    char *dir;
    struct RfaInfo **rfaHeadp;
    char *target;
    int locked;
{
    struct RfaInfo *rfalist = NULL, **rfap = & rfalist;
    DIR *dirp;
    struct dirent *dp;
    struct stat st;
    int rc;
    char *rfa_fn;
    FILE *rfa_fp;
    struct LockEntry *le;

    advise (LLOG_DEBUG, NULLCP, "getRfaInfoList: '%s', target '%s'", 
		dir, target);

    /*--- find out if directory  --*/
    if (stat(makeFN(dir),&st) == -1) {
	advise (LLOG_EXCEPTIONS, NULLCP, "cant stat '%s':%s", dir, 
		sys_errname(errno));
	sprintf(rfaErrStr, "%s - %s", dir, sys_errname(errno));
	return NOTOK;
    }

    /*-- get rfainfo --*/
    if (st.st_mode & S_IFDIR) 
	rfa_fn = dir;
    else
	rfa_fn = dirname(dir);

    if (rfa_fp = fopen(makeFN2(rfa_fn,".rfainfo"),locked ? "a+":"r")) 
	rewind(rfa_fp);
    else 
	if (errno != ENOENT)  {
	    sprintf(rfaErrStr, "%s/.rfainfo (%s)", rfa_fn, sys_errname(errno));
	    advise(LLOG_EXCEPTIONS, NULLCP, "cant open '%s/.rfainfo' - %s", 	
		rfa_fn, sys_errname(errno));
	    return NOTOK_SYS;
	}

    if (locked)
	if ((le = lockRfainfo(rfa_fn, rfa_fp)) == NULL) {
	    fclose(rfa_fp);
	    return NOTOK;
	}

    if (st.st_mode & S_IFDIR) {
	/*--- dir is a directory name, so open dir ---*/
	if ((dirp = opendir(makeFN(dir))) ==  NULL) {
	    sprintf(rfaErrStr, "%s - %s", dir, sys_errname(errno));
	    advise(LLOG_EXCEPTIONS, NULLCP, "can't open dir '%s' - %s", 	
		dir, sys_errname(errno));
	    rc = NOTOK;
	    goto err_cleanup;
	}

	for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)) {
	    if (((*rfap) = mallocRfaInfo(strdup(dp->d_name))) == NULL) {
		freeRfaInfoList(rfalist);
		sprintf(rfaErrStr, "out of memory");
		advise(LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
		rc = NOTOK;
		goto err_cleanup;
	    }
	    SET_STATUS((*rfap)->ri_status, RI_UNREGISTERED);
	    SET_LOCKINFO((*rfap)->ri_status, RI_UNLOCKED);
	    SET_TRANSFER((*rfap)->ri_status, RI_TR_REQ);

	    if ((target == NULL ) || (strcmp(target, dp->d_name) == 0)) {
		if ((rc = statFile(makeFN2(dir, dp->d_name), *rfap)) != OK) {
		    freeRfaInfoList(rfalist);
		    goto err_cleanup;
		}
	    }
	    rfap = &((*rfap)->ri_next);
	}
	closedir(dirp);
	if ((rc = getRfaInfo(dir, rfa_fp, &rfalist)) != OK) {
	    freeRfaInfoList(rfalist);
	    goto err_cleanup;
	}
    } else { 
	/*--- dir is a single file ---*/

	if (((*rfap) = mallocRfaInfo(strdup(basename(dir)))) == NULL) {
	    freeRfaInfoList(rfalist);
	    sprintf(rfaErrStr, "out of memory");
	    advise(LLOG_EXCEPTIONS, NULLCP, rfaErrStr);
	    rc = NOTOK;
	    goto err_cleanup;
	}
	SET_STATUS((*rfap)->ri_status, RI_UNREGISTERED);
	SET_LOCKINFO((*rfap)->ri_status, RI_UNLOCKED);
	SET_TRANSFER((*rfap)->ri_status, RI_TR_REQ);

	if ((rc = statFile(makeFN(dir), *rfap)) != OK) {
	    freeRfaInfoList(rfalist);
	    goto err_cleanup;
	}

	/*--- get locking info ---*/
	if ((rc = getRfaInfo(dirname(dir), rfa_fp, &rfalist)) != OK) {
	    freeRfaInfoList(rfalist);
	    goto err_cleanup;
	}
    }
    *rfaHeadp = rfalist;
    if (!locked && rfa_fp)
	fclose(rfa_fp);

    return OK;

err_cleanup:;
    if (locked) 
	closeAndUnlockRfainfo(rfa_fn);
    else
	if (rfa_fp)
	    fclose(rfa_fp);
    return rc;
}

/*------------------------------------------------------
 * putRfaInfoList - write RFA file info list to ".rfainfo"
 *------------------------------------------------------*/
int putRfaInfoList(dirname, rfa)
    char *dirname;
    struct RfaInfo *rfa;
{
    FILE *backup_f;
    char buf[BUFSIZ];
    struct LockEntry *le;
    int num;

    advise(LLOG_DEBUG,NULLCP,"putRfaInfo %s", makeFN(dirname));

    for (le = lockList; le; le = le->le_next)
	if (strcmp(le->le_fn, dirname) == 0)
	    break;
    if (le == NULL) {
	sprintf(rfaErrStr, "can't write %s/.rfainfo (not locked)", dirname);
	advise(LLOG_EXCEPTIONS,NULLCP,rfaErrStr);
	return NOTOK;
    }

    if (backup_f = fopen(makeFN2(dirname, ".rfainfo.bak"), "w")) {
	rewind(le->le_filep);
	while (num = fread(buf, 1, BUFSIZ, le->le_filep)) 
	    fwrite(buf, 1, num, backup_f);
	fclose(backup_f);
    }
    rewind(le->le_filep);
    for (; rfa; rfa = rfa->ri_next) {
	if (rfa->ri_mode && (rfa->ri_mode & S_IFMT & (S_IFREG | S_IFDIR)) == 0)
	    continue;
	
	if (fprintf(le->le_filep, "%s %s %ld %s %ld\n", rfa->ri_filename,
		status2str(rfa->ri_status),  rfa->ri_lastChange, 
		rfa->ri_lckname ? rfa->ri_lckname : "NONE", 
		rfa->ri_lcksince) == EOF) 
	{
	    sprintf(rfaErrStr, "can't write %s/.rfainfo (%s)", dirname, 
		    sys_errname(errno));
	    (void)advise(LLOG_EXCEPTIONS,NULLCP,rfaErrStr);
	    unlink(makeFN2(dirname, ".rfainfo"));
	    strcpy(buf, makeFN2(dirname,".rfainfo.bak"));
	    rename (buf, makeFN2(dirname,".rfainfo"));
	    return NOTOK;
	}
    }
    fflush(le->le_filep);
    return OK;
}


/*------------------------------------------------------
 * extractRfaInfo - extract RFA file info by filename
 *------------------------------------------------------*/
struct RfaInfo *extractRfaInfo(fn, rfap)
    char *fn;
    struct RfaInfo **rfap;
{
    struct RfaInfo *h;

    for (; *rfap; rfap = &((*rfap)->ri_next))
	if (strcmp(fn, (*rfap)->ri_filename) == 0) {
	    h = *rfap;
	    *rfap = h->ri_next;
	    h->ri_next = NULL;
	    return h;
	}
    return NULL;
}

/*------------------------------------------------------
 * remRfaInfo - remove RFA file info from list
 *------------------------------------------------------*/
void remRfaInfo (fn, rfap)
    char *fn;
    struct RfaInfo **rfap;
{
    struct RfaInfo *h;

    if ((h = extractRfaInfo(fn, rfap))) {
	h->ri_next = NULL;
	freeRfaInfoList(h);
    }
}
    
/*------------------------------------------------------
 * findRfaInfo - find RFA file info by filename
 *------------------------------------------------------*/
struct RfaInfo *findRfaInfo(fn, rfa)
    char *fn;
    register struct RfaInfo *rfa;
{
    for (; rfa; rfa = rfa->ri_next)
	if (strcmp(fn, rfa->ri_filename) == 0)
		return rfa;
    return NULL;
}

/*------------------------------------------------------
 * sortRfaInfoList - sort RFA list
 *------------------------------------------------------*/
void sortRfaInfoList(rfap)
    struct RfaInfo **rfap;
{
    struct RfaInfo *tnext, *tosort, *sorted, **spp;

    sorted = NULL;
    for (tosort = *rfap; tosort; tosort = tnext) {
	tnext = tosort->ri_next;
	for (spp = &sorted; *spp ; spp = &((*spp)->ri_next)) 
	    if (strcmp((*spp)->ri_filename, tosort->ri_filename) > 0) 
		break;
	tosort->ri_next = *spp;
	*spp = tosort;
    }
    *rfap = sorted;
}
		

	
