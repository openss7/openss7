/*
 * RFA - Remote File Access
 /*
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * ls.c : prepare file info in a "ls" style
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * $Header: /xtel/isode/isode/others/rfa/RCS/ls.c,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: ls.c,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid = "$Header: /xtel/isode/isode/others/rfa/RCS/ls.c,v 9.0 1992/06/16 12:47:25 isode Rel $";
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

#include <ctype.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "general.h"
#include "RFA-types.h" 
#include "rfa.h"
#include "rfainfo.h"

struct pair {
	u_short	val;
	char	*s;
};

static struct pair ftype[] = { {S_IFIFO, "?"}, { S_IFCHR, "c" }, {S_IFDIR, "d"},
			 {S_IFBLK, "b"}, { S_IFREG, "-" }, {S_IFLNK, "l"},
			 {S_IFSOCK, "s"}, { 0, NULL }}; 

static struct pair facc[] = { { 01, "--x" }, { 02, "-w-" }, { 03, "-wx" },
			      { 04, "r--" }, { 05, "r-x" }, { 06, "rw-" },
			      { 07, "rwx" }, { 00, "---" } };

static void mode2str(m, mstr)
    u_short m;
    char *mstr;
{
    u_short v;
    struct pair *pp;

    v = m & S_IFMT;
    for (pp = ftype; pp->s; pp++) 
	if (v == pp->val) {
	    strcat(mstr, pp->s);
	    break;
	}
    v = (u_short)(m & 0700) >> 6;
    for (pp = facc; pp->s; pp++) 
	if (v == pp->val) {
	    strcat(mstr, pp->s);
	    break;
	}
    if (m & S_ISUID)
	*(mstr + strlen(mstr) - 1) = 's';
    v = (u_short)(m & 070) >> 3;
    for (pp = facc; pp->s; pp++) 
	if (v == pp->val) {
	    strcat(mstr, pp->s);
	    break;
	}
    if (m & S_ISGID)
	*(mstr + strlen(mstr) - 1) = 's';
    v = m & 07;
    for (pp = facc; pp->s; pp++) 
	if (v == pp->val) {
	    strcat(mstr, pp->s);
	    break;
	}
}

char *shortTime(t)
    long *t;
{
    char *s;

    s = ctime(t);
     
    *(rindex(s, ':')) = '\0';

    s+=4;
    return s;
}


char *rfa2ls(rfa)
    struct RfaInfo *rfa;
{
    static char buf[512], *bp;

    *buf = '\0';
    bp = buf;

    mode2str(rfa->ri_mode, bp);
    bp += strlen(bp);

    sprintf(bp, " %3.3s", status2str(rfa->ri_status));
    bp += strlen(bp);

    if(IS_LOCKED(rfa->ri_status)) {
    	sprintf(bp, " lockby %-10s %8d", rfa->ri_lckname, rfa->ri_size);
    } else 
	sprintf(bp, " %-8s %-8s %8d", rfa->ri_owner, rfa->ri_group,
		rfa->ri_size);
    bp += strlen(bp);

    sprintf(bp, " %12s", shortTime(&(rfa->ri_modTime)));
    bp += strlen(bp);
    sprintf(bp, " %s", rfa->ri_filename);
    if ((rfa->ri_mode & S_IFMT) == S_IFLNK) {
	bp += strlen(bp);
	sprintf(bp, " -> %s", rfa->ri_lnkName);
    }

    return buf;
}

