/* serror.c - get system error */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/serror.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/compat/RCS/serror.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: serror.c,v $
 * Revision 9.0  1992/06/16  12:07:00  isode
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
#include "general.h"
#include "manifest.h"

/*    DATA */

extern	int sys_nerr;
extern  char *sys_errlist[];

/*  */

char   *sys_errname (i)
int	i;
{
    static char buffer[30];

    if (0 < i && i < sys_nerr)
	return sys_errlist[i];
    (void) sprintf (buffer, "Error %d", i);

    return buffer;
}
