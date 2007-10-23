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

/* control.c - */

#ifndef lint
static char *rcsid =
    "Header: /xtel/isode/isode/quipu/RCS/control.c,v 9.0 1992/06/16 12:34:01 isode Rel";
#endif

/*
 * Header: /xtel/isode/isode/quipu/RCS/control.c,v 9.0 1992/06/16 12:34:01 isode Rel
 *
 *
 * Log: control.c,v
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

/* the routine dsa_control is called when the modifyentry operation
   is performed, with and 'add attribute' request and the attribute type
   is 'control'.  The value decides what to control
   This is strictly non standard, but gives the dua control of the dsa.
*/

#include "quipu/util.h"
#include "quipu/entry.h"
#include "quipu/dsp.h"
#include "quipu/ds_error.h"
#include "tailor.h"

extern LLog *log_dsap;

#ifndef NO_STATS
extern LLog *log_stat;
#endif
extern void dsa_abort();

dsa_control(as, error, dn)
	Attr_Sequence as;
	struct DSError *error;
	DN dn;
{
	char *str;
	DN dn2;
	Entry theentry;
	extern Entry database_root;
	SFD attempt_restart();

	if (!manager(dn)) {
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = DSE_SC_ACCESSRIGHTS;
		return (DS_ERROR_REMOTE);
	}

	str = (char *) as->attr_value->avseq_av.av_struct;

#ifndef NO_STATS
	LLOG(log_stat, LLOG_NOTICE, ("DSA control: %s", str));
#endif

	switch (*str) {
	case 'd':		/* -dump <directory> */
		str = SkipSpace(++str);
/* 
		directory_dump (str, database_root);
*/
		return (DS_OK);
	case 't':		/* -tailor <string> */
		str = SkipSpace(++str);
		if (dsa_tai_string(str) == OK) {
			isodexport(NULLCP);
			return (DS_OK);
		}
		break;
	case 'a':		/* -abort */
		LLOG(log_dsap, LLOG_FATAL, ("*** abort signal ***"));
		dsa_abort(-1);
		exit(0);
	case 'b':		/* -restart */
		LLOG(log_dsap, LLOG_FATAL, ("*** restart signal ***"));
		attempt_restart(NOTOK);
		exit(0);	/* should not be reached */
	case 'r':		/* -refresh <entry> */
		str = SkipSpace(++str);
		if (lexequ(str, "root") == 0)
			dn2 = NULLDN;
		else if ((dn2 = str2dn(str)) == NULLDN)
			break;

		if (refresh_from_disk(dn2) == OK)
			return (DS_OK);
		break;
	case 'f':		/* -resync <entry> */
		str = SkipSpace(++str);
		if (lexequ(str, "root") == 0)
			dn2 = NULLDN;
		else if ((dn2 = str2dn(str)) == NULLDN)
			break;

		if ((theentry = local_find_entry(dn2, FALSE)) != NULLENTRY)
#ifdef TURBO_DISK
		{
			Entry akid = (Entry) avl_getone(theentry->e_children);

			if (turbo_writeall(akid) == OK)
				return (DS_OK);
		}
#else
		{
			Entry akid = (Entry) avl_getone(theentry->e_children);

			if (journal(akid) == OK)
				return (DS_OK);
		}
#endif
		break;
	case 'l':		/* -lock <entry> */
		str = SkipSpace(++str);
		if (lexequ(str, "root") == 0)
			dn2 = NULLDN;
		else if ((dn2 = str2dn(str)) == NULLDN)
			break;

		if ((theentry = local_find_entry(dn2, FALSE)) != NULLENTRY) {
			theentry->e_lock = TRUE;
			return (DS_OK);
		}
		break;
	case 'u':		/* -unlock <entry> */
		str = SkipSpace(++str);
		if (lexequ(str, "root") == 0)
			dn2 = NULLDN;
		else if ((dn2 = str2dn(str)) == NULLDN)
			break;

		if ((theentry = local_find_entry(dn2, FALSE)) != NULLENTRY) {
			theentry->e_lock = FALSE;
			return (DS_OK);
		}
		break;
	case 's':		/* -slave */
		/* 
		 * When we go async return of OK will mean that a getedb
		 * operation has been scheduled, NOT that it has succeeded.
		 */
		str = SkipSpace(++str);
		if (*str == NULL) {
			slave_update();
			return DS_OK;
		}

		if (lexequ(str, "shadow") == 0) {
			shadow_update();
			return DS_OK;
		}

		if (lexequ(str, "root") == 0)
			dn2 = NULLDN;
		else if ((dn2 = str2dn(str)) == NULLDN)
			break;

		if (update_aux(dn2, dn2 == NULLDN) == OK)
			return DS_OK;
		break;
	default:
		break;
	}

	error->dse_type = DSE_SERVICEERROR;
	error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
	return (DS_ERROR_REMOTE);
}

#ifdef QUIPU_CONSOLE

new_dsa_control(as, error, dn)
	Attr_Sequence as;
	struct DSError *error;
	DN dn;
{
	struct dsa_control *item;
	char *tmp_ptr;
	DN dn2;
	Entry theentry;
	extern Entry database_root;
	SFD attempt_restart();

	/* Return some silly error to distinguish it from the other dsa_control */
	if (!manager(dn)) {
		error->dse_type = DSE_SECURITYERROR;
		error->ERR_SECURITY.DSE_sc_problem = DSE_SC_PROTECTIONREQUIRED;
		return (DS_ERROR_REMOTE);
	}

	item = (struct dsa_control *) as->attr_value->avseq_av.av_struct;

	switch (item->dsa_control_option) {
	case (CONTROL_CHANGETAILOR):	/* -tailor <string> */
	{
		tmp_ptr = qb2str(item->un.changeTailor);
		if (dsa_tai_string(tmp_ptr) == OK) {
			isodexport(NULLCP);
			return (DS_OK);
		}
		break;
	}
	case (CONTROL_STOPDSA):	/* -abort */
	{
		LLOG(log_dsap, LLOG_FATAL, ("*** abort signal ***"));
		stop_listeners();
		exit(0);
	}
	case (CONTROL_REFRESH):	/* -refresh <entry> */
	{
		if (item->un.refresh->offset == DN_PRESENT)
			if (refresh_from_disk(item->un.refresh->un.selectedDN) == OK)
				return (DS_OK);
		break;
	}
	case (CONTROL_RESYNCH):	/* -resync <entry> */
	{
		if (item->un.resynch->offset == DN_PRESENT)
			dn2 = item->un.resynch->un.selectedDN;
		else
			break;

		if ((theentry = local_find_entry(dn2, FALSE)) != NULLENTRY)
#ifdef TURBO_DISK
		{
			Entry akid = (Entry) avl_getone(theentry->e_children);

			if (turbo_writeall(akid) == OK)
				return (DS_OK);
		}
#else
		{
			Entry akid = (Entry) avl_getone(theentry->e_children);

			if (journal(akid) == OK)
				return (DS_OK);
		}
#endif
		break;
	}
	case (CONTROL_LOCKDSA):	/* -lock <entry> */
	{
		if (item->un.resynch->offset == DN_PRESENT)
			dn2 = item->un.resynch->un.selectedDN;
		else
			break;

		if ((theentry = local_find_entry(dn2, FALSE)) != NULLENTRY) {
			theentry->e_lock = TRUE;
			return (DS_OK);
		}
		break;
	}
	case (CONTROL_UNLOCK):	/* -unlock <entry> */
	{
		if (item->un.resynch->offset == DN_PRESENT)
			dn2 = item->un.resynch->un.selectedDN;
		else
			break;

		if ((theentry = local_find_entry(dn2, FALSE)) != NULLENTRY) {
			theentry->e_lock = FALSE;
			return (DS_OK);
		}
		break;
	}
	case (CONTROL_SETLOGLEVEL):	/* Set the Logging Level */
	{
		tmp_ptr = qb2str(item->un.setLogLevel);
		/* What kind of stuff is needed here?!! */
		return (DS_OK);
		break;
	}
	case (CONTROL_UPDATESLAVEEDBS):	/* -slave */
	{
		/* 
		 * When we go async return of OK will mean that a getedb
		 * operation has been scheduled, NOT that it has succeeded.
		 */
		tmp_ptr = qb2str(item->un.updateSlaveEDBs);

		if (lexequ(tmp_ptr, "all") == 0) {
			slave_update();
			return DS_OK;
		}

		if (lexequ(tmp_ptr, "shadow") == 0) {
			shadow_update();
			return DS_OK;
		}

		if (lexequ(tmp_ptr, "root") == 0) {
			dn2 = NULLDN;
		} else {
			if ((dn2 = str2dn(tmp_ptr)) == NULLDN)
				break;
		}

		if (update_aux(dn2, dn2 == NULLDN) == OK)
			return DS_OK;
		break;
	}
	default:
		break;
	}

	error->dse_type = DSE_SERVICEERROR;
	error->ERR_SERVICE.DSE_sv_problem = DSE_SV_UNWILLINGTOPERFORM;
	return (DS_ERROR_REMOTE);
}
#endif				/* QUIPU_CONSOLE */
