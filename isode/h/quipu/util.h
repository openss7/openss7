/* util.h - various useful utility definitions */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/util.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: util.h,v $
 * Revision 9.0  1992/06/16  12:23:11  isode
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


#ifndef _DIDUTIL_

#define _DIDUTIL_

#ifndef _H_UTIL			/* PP interwork */

#include <errno.h>
#include <stdio.h>              /* minus the ctype stuff */
#include <ctype.h>
#include <setjmp.h>
#include "manifest.h"
#include "logger.h"

#endif

#ifndef       _GENERAL_
#include "general.h"
#endif

#include "quipu/config.h"

#ifndef _H_UTIL			/* PP interwork */

/* some common logical values */

#ifndef TRUE
#define TRUE    1
#endif
#ifndef FALSE
#define FALSE   0
#endif
#ifndef MAYBE
#define MAYBE   1
#endif

/* stdio extensions */

#ifndef lowtoup
#define lowtoup(chr) (islower(chr)?toupper(chr):chr)
#endif
#ifndef uptolow
#define uptolow(chr) (isupper(chr)?tolower(chr):chr)
#endif
#ifndef MIN
#define MIN(a,b) (( (b) < (a) ) ? (b) : (a) )
#endif
#ifndef MAX
#define MAX(a,b) (( (b) > (a) ) ? (b) : (a) )
#endif
#ifndef	MAXINT
#define MAXINT (~(1 << ((sizeof(int) * 8) - 1)))
#endif

#define isstr(ptr) ((ptr) != 0 && *(ptr) != '\0')
#define isnull(chr) ((chr) == '\0')
#define isnumber(c) ((c) >= '0' && (c) <= '9')

/*
 * provide a timeout facility
 */

extern  jmp_buf _timeobuf;

#define quipu_timeout(val)    (setjmp(_timeobuf) ? 1 : (_timeout(val), 0))

/*
 * some common extensions
 */
#define LINESIZE 1024    /* what we are prepared to consider a line length */
#define FILNSIZE 256    /* max filename length */
#define LOTS    1024    /* a max sort of thing */
#define MAXFILENAMELEN 15	/* size of largest fine name allowed */

# define        MAXFORK 10      /* no. of times to try a fork() */

#define _H_UTIL

#endif 
#endif
