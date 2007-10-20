/* cmd_srch.c - search a lookup table: return numeric value */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/cmd_srch.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/cmd_srch.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: cmd_srch.c,v $
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

/* map a string onto a value */

cmd_srch(str, cmd)
register char   *str;
register CMD_TABLE *cmd;
{
	extern char chrcnv[];

	for(;cmd->cmd_key != NULLCP; cmd++)
		if(chrcnv[*str] == chrcnv[*cmd->cmd_key] &&
		   lexequ(str, cmd->cmd_key) == 0)
			return(cmd->cmd_value);
	return(cmd->cmd_value);
}
