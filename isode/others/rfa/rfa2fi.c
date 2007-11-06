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
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * rfa2fi.c : convert between RfaInfo and type_RFA_FileInfoList and vice versa
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * Header: /xtel/isode/isode/others/rfa/RCS/rfa2fi.c,v 9.0 1992/06/16 12:47:25 isode Rel
 *
 * Log: rfa2fi.c,v
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid =
    "Header: /xtel/isode/isode/others/rfa/RCS/rfa2fi.c,v 9.0 1992/06/16 12:47:25 isode Rel";
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
#include "rfa.h"
#include "RFA-types.h"
#include "rfainfo.h"

extern char *getRelativeFN();
extern char *fsBase;

/*--------------------------------------------------------------------
 * rfa2fi  - RfaInfo to type_RFA_FileInfo
 *--------------------------------------------------------------------*/
struct type_RFA_FileInfo *
rfa2fi(dir, rfa)
	char *dir;
	struct RfaInfo *rfa;
{
	struct type_RFA_FileInfo *fi;
	char *l;

	if ((fi = (struct type_RFA_FileInfo *)
	     malloc(sizeof(struct type_RFA_FileInfo))) == NULL) {
		sprintf(rfaErrStr, "out of memory");
		return NULL;
	}
	fi->name = str2qb(rfa->ri_filename, strlen(rfa->ri_filename), 1);
	fi->mode = rfa->ri_mode;
	fi->user = str2qb(rfa->ri_owner, strlen(rfa->ri_owner), 1);
	fi->group = str2qb(rfa->ri_group, strlen(rfa->ri_group), 1);
	fi->size = rfa->ri_size;
	fi->accTime = rfa->ri_accTime;
	fi->modTime = rfa->ri_modTime;
	if (l = rfa->ri_lnkName) {
		if (*l != '/') {
			if (getRelativeFN(realPath3(fsBase, dir, l)) == NULL)
				l = "(link outside RFA context)";
		} else if ((l = getRelativeFN(rfa->ri_lnkName)) == NULL)
			l = "(link outside RFA context)";
		fi->lnkName = str2qb(l, strlen(l), 1);
	} else
		fi->lnkName = NULL;
	fi->status = rfa->ri_status;
	if (IS_LOCKED(rfa->ri_status)) {
		fi->lockedBy = str2qb(rfa->ri_lckname, strlen(rfa->ri_lckname), 1);
		fi->lockedSince = rfa->ri_lcksince;
	} else {
		fi->lockedBy = NULL;
		fi->lockedSince = NULL;
	}
	return fi;
}

/*--------------------------------------------------------------------
 * rfa2fil  - RfaInfo to type_RFA_FileInfoList
 *--------------------------------------------------------------------*/
struct type_RFA_FileInfoList *
rfa2fil(dir, rfa)
	char *dir;
	struct RfaInfo *rfa;
{
	struct type_RFA_FileInfoList *fil, **filp;

	filp = &fil;
	for (; rfa; rfa = rfa->ri_next) {
		if (((*filp) = (struct type_RFA_FileInfoList *)
		     malloc(sizeof(struct type_RFA_FileInfoList))) == NULL) {
			free_RFA_FileInfoList(fil);
			sprintf(rfaErrStr, "out of memory");
			return NULL;
		}
		(*filp)->next = NULL;
		if (((*filp)->FileInfo = rfa2fi(dir, rfa)) == NULL) {
			free_RFA_FileInfoList(fil);
			return NULL;
		}
		filp = &((*filp)->next);
	}
	return fil;
}

/*--------------------------------------------------------------------
 * fi2rfa  - type_RFA_FileInfoList to RfaInfo 
 *--------------------------------------------------------------------*/
struct RfaInfo *
fi2rfa(fil)
	struct type_RFA_FileInfoList *fil;
{
	struct type_RFA_FileInfo *fi;
	struct RfaInfo *rfa, *rfaNew;
	char *l;

	rfa = NULL;
	for (; fil; fil = fil->next) {
		fi = fil->FileInfo;
		if ((rfaNew = mallocRfaInfo(qb2str(fi->name))) == NULL) {
			freeRfaInfoList(rfa);
			return NULL;
		}
		rfaNew->ri_next = rfa;
		rfa = rfaNew;

		rfa->ri_mode = fi->mode;
		rfa->ri_owner = qb2str(fi->user);
		rfa->ri_group = qb2str(fi->group);
		rfa->ri_size = fi->size;
		rfa->ri_accTime = fi->accTime;
		rfa->ri_modTime = fi->modTime;
		if (fi->lnkName) {
			l = qb2str(fi->lnkName);
			if (*l == '/')
				l = makeFN(l);
			rfa->ri_lnkName = strdup(l);
		} else
			rfa->ri_lnkName = NULL;
		rfa->ri_status = fi->status;
		if (IS_LOCKED(rfa->ri_status)) {
			rfa->ri_lckname = qb2str(fi->lockedBy);
			rfa->ri_lcksince = fi->lockedSince;
		} else {
			rfa->ri_lckname = NULL;
			rfa->ri_lcksince = NULL;
		}
	}
	return rfa;
}
