/* policy.c - SecurityPolicy abstract syntax  */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/policy.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/policy.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: policy.c,v $
 * Revision 9.0  1992/06/16  12:12:39  isode
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


#include "cmd_srch.h"
#include "quipu/policy.h"

/*
 * This syntax is not yet implemented. However, the following routines for
 * handling security policies are used by some applications.
 */

static CMD_TABLE permtab[] = {
	"detect", POLICY_ACCESS_DETECT,
	"read", POLICY_ACCESS_READ,
	"add", POLICY_ACCESS_ADD,
	"write", POLICY_ACCESS_WRITE,
	"all", POLICY_ACCESS_ALL,
	"none", 0,
	NULLCP, 0
};

unsigned str2permission(str)
char *str;
{
char *cp;
unsigned result = 0;

  while (str && (*str != '\0'))
  {
    cp = index(str, '$');
    if (cp != NULLCP)
      *cp++ = '\0';
    result |= cmd_srch(str, permtab);
    str = cp;
  }
  return (result);
}

