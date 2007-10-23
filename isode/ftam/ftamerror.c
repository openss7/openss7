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

/* ftamerror.c - return FTAM error code in string form */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/ftam/RCS/ftamerror.c,v 9.0 1992/06/16 12:14:55 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/ftam/RCS/ftamerror.c,v 9.0 1992/06/16 12:14:55 isode Rel
 *
 *
 * Log: ftamerror.c,v
 * Revision 9.0  1992/06/16  12:14:55  isode
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

/* LINTLIBRARY */

#include <stdio.h>
#include "ftam.h"

/*    DATA */

static char *diag_gen[] = {
	"No reason",
	"Responder error (unspecific)",
	"System shutdown",
	"FTAM management problem (unspecific)",
	"FTAM management, bad account",
	"FTAM management, security not passed",
	"Delay may be encountered",
	"Initiator error (unspecific)",
	"Subsequent error",
	"Temporal insufficiency of resources",
	"Access request violates VFS security",
	"Access request violates local security",
	"Indications waiting"
};

static int diag_gen_cnt = sizeof diag_gen / sizeof diag_gen[0];

static char *diag_pro[] = {
	"Conflicting parameter values",
	"Unsupported parameter values",
	"Mandatory parameter not set",
	"Unsupported parameter",
	"Duplicated parameter",
	"Illegal parameter type",
	"Unsupported parameter type",
	"FTAM protocol error (unspecific)",
	"FTAM protocol error, procedure error",
	"FTAM protocol error, functional unit error",
	"FTAM protocol error, corruption error",
	"Lower layer failure",
	"Lower layer addressing error",
	"Timeout",
	"System shutdown",
	"Illegal grouping sequence",
	"Grouping threshold violation",
	"Specific PDU request inconsistent with the current requested access"
};

static int diag_pro_cnt = sizeof diag_pro / sizeof diag_pro[0];

static char *diag_acs[] = {
	"Association with user not allowed",
	"not assigned, #2001",
	"Unsupported service class",
	"Unsupported functional unit",
	"Attribute group error (unspecific)",
	"Attribute group not supported",
	"Attribute group not allowed",
	"Bad account",
	"Association management (unspecific)",
	"Association management - bad address",
	"Association management - bad account",
	"Checkpoint window error - too large",
	"Checkpoint window error - too small",
	"Checkpoint window error - unsupported",
	"Communications QoS not supported",
	"Initiator identity unacceptable",
	"Context management refused",
	"Rollback not available",
	"Contents type list cut by responder",
	"Contents type list cut by Presentation service",
	"Invalid filestore password",
	"Incompatible service classes"
};

static int diag_acs_cnt = sizeof diag_acs / sizeof diag_acs[0];

static char *diag_sel[] = {
	"Filename not found",
	"Selection attributes not matched",
	"Initial attributes not possible",
	"Bad attribute name",
	"Non-existent file",
	"File already exists",
	"File can not be created",
	"File can not be deleted",
	"Concurrency control not available",
	"Concurrency control not supported",
	"Concurrency control not possible",
	"More restrictive lock",
	"File busy",
	"File not available",
	"Access control not available",
	"Access control not supported",
	"Access control inconsistent",
	"Filename truncated",
	"Initial attributes altered",
	"Bad account",
	"Override selected existing file",
	"Override deleted and recreated file with old attributes",
	"Create override deleted and recreated file with new attributes",
	"Create override - not possible",
	"Ambiguous file specification",
	"Invalid create password",
	"Invalid delete password on override",
	"Bad attribute value",
	"Requested access violates permitted actions",
	"Functional unit not available for requested access",
	"File created but not selected"
};

static int diag_sel_cnt = sizeof diag_sel / sizeof diag_sel[0];

static char *diag_mgt[] = {
	"Attribute non-existant",
	"Attribute cannot be read",
	"Attribute cannot be changed",
	"Attribute not supported",
	"Bad attribute name",
	"Bad attribute value",
	"Attribute partially supported",
	"Additional set attribute not distinct"
};

static int diag_mgt_cnt = sizeof diag_mgt / sizeof diag_mgt[0];

static char *diag_acc[] = {
	"Bad FADU (unspecific)",
	"Bad FADU - size error",
	"Bad FADU - type error",
	"Bad FADU - poorly specified",
	"Bad FADU - bad location",
	"FADU does not exist",
	"FADU not available (unspecific)",
	"FADU not available for reading",
	"FADU not available for writing",
	"FADU not available for location",
	"FADU not available for erasure",
	"FADU cannot be inserted",
	"FADU cannot be replaced",
	"FADU cannot be located",
	"Bad data element type",
	"Operation not available",
	"Operation not supported",
	"Operation inconsistent",
	"Concurrency control not available",
	"Concurrency control not supported",
	"Concurrency control inconsistent",
	"Processing mode not available",
	"Processing mode not supported",
	"Processing mode inconsistent",
	"Access context not available",
	"Access context not supported",
	"Bad write (unspecific)",
	"Bad read (unspecific)",
	"Local failure (unspecific)",
	"Local failure - file space exhausted",
	"Local failure - data corrupted",
	"Local failure - device failure",
	"Future file size exceeded",
	"not assigned, #5033",
	"Future file size increased",
	"Functional unit invalid in processing mode",
	"Contents type inconsistent",
	"Contents type simplified",
	"Duplicate FADU name",
	"Damage to select/open regime",
	"FADU locking not available on file",
	"FADU locked by another user"
};

static int diag_acc_cnt = sizeof diag_acc / sizeof diag_acc[0];

static char *diag_rvy[] = {
	"Bad checkpoint (unspecific)",
	"Activity not unique",
	"Checkpoint outside window",
	"Activity no longer exists",
	"Activity not recognized",
	"No docket",
	"Corrupt docket",
	"File waiting restart",
	"Bad recovery point",
	"Non-existent recovery point",
	"Recovery mode not available",
	"Recovery mode inconsistent",
	"Recovery mode reduced",
	"Access control not available",
	"Access control not supported",
	"Access control inconsistent",
	"Contents type inconsistent",
	"Contents type simplified"
};

static int diag_rvy_cnt = sizeof diag_rvy / sizeof diag_rvy[0];

/*  */

char *
FErrString(code)
	register int code;
{
	register int offset;
	char buffer[BUFSIZ];

	offset = FS_CODE2OFF(code);
	switch (code - offset) {
	case FS_GEN_BASE:
		if (offset < diag_gen_cnt)
			return diag_gen[offset];
		break;

	case FS_PRO_BASE:
		if (offset < diag_pro_cnt)
			return diag_pro[offset];
		break;

	case FS_ACS_BASE:
		if (offset < diag_acs_cnt)
			return diag_acs[offset];
		break;

	case FS_SEL_BASE:
		if (offset < diag_sel_cnt)
			return diag_sel[offset];
		break;

	case FS_MGT_BASE:
		if (offset < diag_mgt_cnt)
			return diag_mgt[offset];
		break;

	case FS_ACC_BASE:
		if (offset < diag_acc_cnt)
			return diag_acc[offset];
		break;

	case FS_RVY_BASE:
		if (offset < diag_rvy_cnt)
			return diag_rvy[offset];
		break;
	}

	(void) sprintf(buffer, "unknown error code %d", code);
	return buffer;
}
