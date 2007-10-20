/* chrcnv.c - character conversion table */

#ifndef lint
static char *rcsid = "$Header: /xtel/isode/isode/compat/RCS/chrcnv.c,v 9.0 1992/06/16 12:07:00 isode Rel $";
#endif

/*
 * $Header: /xtel/isode/isode/compat/RCS/chrcnv.c,v 9.0 1992/06/16 12:07:00 isode Rel $
 *
 *
 * $Log: chrcnv.c,v $
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

#include <stdio.h>
#include "general.h"

/*  */

char                                   /* character conversion table   */
	chrcnv[] =                     /*   lower to upper case letters */
{
    '\0', '\1', '\2', '\3', '\4', '\5', '\6', '\7',
    '\10', '\t', '\n', '\13', '\14', '\r', '\16', '\17',
    '\20', '\21', '\22', '\23', '\24', '\25', '\26', '\27',
    '\30', '\31', '\32', '\33', '\34', '\35', '\36', '\37',
    ' ', '!', '"', '#', '$', '%', '&', '\47',
    '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '{', '|', '}', '~', '\177',
    '\0', '\1', '\2', '\3', '\4', '\5', '\6', '\7',
    '\10', '\t', '\n', '\13', '\14', '\r', '\16', '\17',
    '\20', '\21', '\22', '\23', '\24', '\25', '\26', '\27',
    '\30', '\31', '\32', '\33', '\34', '\35', '\36', '\37',
    ' ', '!', '"', '#', '$', '%', '&', '\47',
    '(', ')', '*', '+', ',', '-', '.', '/',
    '0', '1', '2', '3', '4', '5', '6', '7',
    '8', '9', ':', ';', '<', '=', '>', '?',
    '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '[', '\\', ']', '^', '_',
    '`', 'A', 'B', 'C', 'D', 'E', 'F', 'G',
    'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
    'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W',
    'X', 'Y', 'Z', '{', '|', '}', '~', '\177'
};

