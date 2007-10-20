/* rcmd_srch.c: search a lookup table: return string value */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/rcmd_srch.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/rcmd_srch.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: rcmd_srch.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
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


/* LINTLIBRARY */

#include "manifest.h"
#include "cmd_srch.h"

/*  */

char   *rcmd_srch(val, cmd)
register int   val;
register CMD_TABLE *cmd;
{
	for(;cmd->cmd_key != NULLCP; cmd++)
		if(val == cmd->cmd_value)
			return(cmd->cmd_key);
	return(NULLCP);
}
