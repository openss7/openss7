/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 *			F I N D F I L E . C 
 *
 * $Revision: 1.5 $
 *
 * Log:	findfile.c,v
 * Revision 1.5  86/10/01  18:17:35  dorab
 * made changes to use gf rather than pxl
 * ifdeffed with USEPXL
 * made corrections (pointed out by <trinkle@purdue.edu>) to make
 * a null directory name be treated as the current directory by
 * all the functions consistently.
 * 
 * Revision 1.4  86/09/04  10:29:14  dorab
 * merged the gf stuff
 * 
 * Revision 1.3  86/04/29  23:00:24  dorab
 * first general release
 * 
 * Revision 1.2  86/04/29  12:53:54  dorab
 * Added distinctive RCS header
 * 
 */
#ifndef lint
static char RCSid[] = "@(#)Header: findfile.c,v 1.5 86/10/01 18:17:35 dorab Exp (UCLA)";
#endif

/* findfile.c
 * Copyright 1985 Massachusetts Institute of Technology
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/param.h>		/* for MAXPATHLEN */

int
findfileindir(area, name, mag, s, nname, nmag)
	char *area, *name, *s, *nname;
	int mag, *nmag;
{
	FILE *f;
	char buf[BUFSIZ];
	int found = 0;
	FILE *fopen();

	area = (*area ? area : ".");	/* a null directory is . */
	sprintf(s, "%s/SUBDIR", area);
#ifdef USEPXL
	if (!access(s, 0))
		sprintf(s, "%s/%s/%s.%dpxl", area, name, name, mag);
	else
		sprintf(s, "%s/%s.%dpxl", area, name, mag);
#else	/* ~USEPXL */
	if (!access(s, 0))
		sprintf(s, "%s/%s/%s.%dgf", area, name, name, mag);
	else
		sprintf(s, "%s/%s.%dgf", area, name, mag);
#endif	/* ~USEPXL */
	if (!access(s, 4)) {
		strcpy(nname, name);
		*nmag = mag;
		return (-1);
	} else {
		sprintf(buf, "%s/NEEDED", area);
		if (!access(buf, 2)) {
#ifdef USEPXL
			sprintf(s, "%s.%dpxl\n", name, mag);
#else	/* ~USEPXL */
			sprintf(s, "%s.%dgf\n", name, mag);
#endif	/* ~USEPXL */
			f = fopen(buf, "r+");
			while (fgets(buf, sizeof(buf), f))
				if (!strcmp(buf, s))
					found++;
			if (!found)
				fputs(s, f);
			fclose(f);
		}
		return (0);
	}
}

/* true if it found a file, false otherwise; name is in s */
int
findfile(dirvec, dirveclen, area, name, mag, s, nname, nmag)
	char *dirvec[], *area, *name, *s, *nname;
	int dirveclen, mag, *nmag;
{
	int i, point;
	char family[128];

	strcpy(nname, name);
	*nmag = mag;
	point = -1;
	(void) sscanf(name, "%[^0123456789.]%d", family, &point);

	/* First check dir area given in DVI file */
	if (*area && findfileindir(area, name, mag, s, nname, nmag))
		return (-1);

	/* Then check along dirvec */
	for (i = 0; i < dirveclen; i++)
		if (findfileindir(dirvec[i], name, mag, s, nname, nmag))
			return (-1);

	/* next check for closest magnification along dirvec */
	return (findanyfile(dirvec, dirveclen, family, point, mag, name, s, nname, nmag));
}

int
strdiff(s1, s2)
	char *s1, *s2;
{
	register int diff = 0;

	while (*s1 && *s2)
		diff += abs(*s1++ - *s2++);
	while (*s1)
		diff += *s1++;
	while (*s2)
		diff += *s2++;
	return (diff);
}

/* find closest match for font in directory */
scanpdir(dir, name,
	 family, point, mag, bestfamily, bestname, bestpoint, bestmag, min_df, min_dp, min_dm)
	char *dir, *name, *family, *bestfamily, *bestname;
	int point, mag, *bestpoint, *bestmag, *min_df, *min_dp, *min_dm;
{
	DIR *dirstream;
	struct direct *dirrecord;
	char qfamily[128];
	int qpoint, qmag, df, dp, dm;

	/* dir = "" means current directory */
	if (dirstream = opendir(dir)) {
		while (dirrecord = readdir(dirstream)) {
#ifdef USEPXL
			if (!strcmp(dirrecord->d_name + dirrecord->d_namlen - 3, "pxl")) {
#else	/* ~USEPXL */
			if (!strcmp(dirrecord->d_name + dirrecord->d_namlen - 2, "gf")) {
#endif	/* ~USEPXL */
				qpoint = -1;
				qmag = -1;
				(void) sscanf(dirrecord->d_name, "%[^0123456789.]%d.%d",
					      qfamily, &qpoint, &qmag);
				df = strdiff(family, qfamily);
				dp = abs(point - qpoint);
				dm = abs(mag - qmag);
				if ((df < *min_df)
				    || (df == *min_df && dp < *min_dp)
				    || (df == *min_df && dp == *min_dp && dm < *min_dm)) {
					sprintf(bestname, "%s/%s", dir, dirrecord->d_name);
					strcpy(bestfamily, qfamily);
					*bestpoint = qpoint;
					*bestmag = qmag;
					*min_df = df;
					*min_dp = dp;
					*min_dm = dm;
				}
			}
		}
		closedir(dirstream);
	}
}

/* look for closest font in its own subdir */
scanfdir(dir, name,
	 family, point, mag, bestfamily, bestname, bestpoint, bestmag, min_df, min_dp, min_dm)
	char *dir, *name, *family, *bestfamily, *bestname;
	int point, mag, *bestpoint, *bestmag, *min_df, *min_dp, *min_dm;
{
	DIR *dirstream;
	struct direct *dirrecord;
	int df;
	char pdir[MAXPATHLEN];

	/* dir = "" means current dir */
	if (dirstream = opendir(dir)) {
		while (dirrecord = readdir(dirstream)) {
			if (dirrecord->d_name[0] != '.') {
				df = strdiff(name, dirrecord->d_name);
				if (df <= *min_df) {
					sprintf(pdir, "%s/%s", dir, dirrecord->d_name);
					scanpdir(pdir, name,
						 family, point, mag,
						 bestfamily, bestname, bestpoint, bestmag,
						 min_df, min_dp, min_dm);
				}
			}
		}
		closedir(dirstream);
	}
}

/* finds the best match to the desired font */
int
findanyfile(dirvec, dirveclen, family, point, mag, name, s, nname, nmag)
	char *dirvec[], *family, *name, *s, *nname;
	int dirveclen, point, mag, *nmag;
{
	char foo[MAXPATHLEN], bestname[MAXPATHLEN], bestfamily[128];
	int min_df, min_dp, min_dm, df, dp, dm, i, bestpoint, bestmag;
	char *realDir;

	bestname[0] = '\0';
	min_df = min_dp = min_dm = 9999999;
	for (i = 0; i < dirveclen; i++) {
		realDir = (*dirvec[i] ? dirvec[i] : ".");	/* a null dir is . */
		sprintf(foo, "%s/SUBDIR", realDir);
		if (!access(foo, 0))
			scanfdir(realDir, name,
				 family, point, mag,
				 bestfamily, bestname, &bestpoint, &bestmag,
				 &min_df, &min_dp, &min_dm);
		else
			scanpdir(realDir, name,
				 family, point, mag,
				 bestfamily, bestname, &bestpoint, &bestmag,
				 &min_df, &min_dp, &min_dm);
	}
	if (bestname[0]) {
		if (bestpoint > 0)
			sprintf(nname, "%s%d", bestfamily, bestpoint);
		else
			strcpy(nname, bestfamily);
		*nmag = bestmag;
		strcpy(s, bestname);
		if ((strcmp(bestfamily, family)
		     || bestpoint != point || abs(bestmag - mag) > 2))
			fprintf(stderr, "Substituted font %s at mag %d for %s at mag %d.\n", nname,
#ifdef USEPXL
				(bestmag * 4 + 3) / 6,
#else	/* ~USEPXL */
				(bestmag * 20 + 3) / 6,
#endif	/* ~USEPXL */
				name,
#ifdef USEPXL
				(mag * 4 + 3) / 6);
#else	/* ~USEPXL */
				(mag * 20 + 3) / 6);
#endif	/* ~USEPXL */
		return (-1);
	}
	return (0);
}
