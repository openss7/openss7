/*
 * RFA - Remote File Access
 *
 * Access and Management for a partial file system tree that exists
 * at two sites either as master files or slave files
 *
 * printerr.c - print error message for protocol defined errors
 *
 * Contributed by Oliver Wenzel, GMD Berlin, 1990
 *
 * $Header: /xtel/isode/isode/others/rfa/RCS/printerr.c,v 9.0 1992/06/16 12:47:25 isode Rel $
 *
 * $Log: printerr.c,v $
 * Revision 9.0  1992/06/16  12:47:25  isode
 * Release 8.0
 *
 */

#ifndef       lint
static char *rcsid = "$Header: /xtel/isode/isode/others/rfa/RCS/printerr.c,v 9.0 1992/06/16 12:47:25 isode Rel $";
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
#include <sys/types.h>
#include "RFA-ops.h"        /* operation definitions */
#include "RFA-types.h"  /* type definitions */
#include "rfa.h"

extern FILE *err;
extern char *shortTime();


/*--------------------------------------------------------------*/
/*  Print Error							*/
/*--------------------------------------------------------------*/
printError(error, param, rc)
    int error;
    caddr_t param;
    int *rc;
{
    time_t t;
    struct type_RFA_StatusErrorParm *se = 
			(struct type_RFA_StatusErrorParm *)param;

    switch(error) {
	case error_RFA_miscError:
		printf("*** remote error : %s ***\n", 
			qb2str((struct qbuf *)param));
		*rc = 3;
		break;
	case error_RFA_fileAccessError:
		printf("*** remote file access error : %s ***\n", 
			qb2str((struct qbuf *)param));
		*rc = 4;
		break;
	case error_RFA_statusError:
		switch (se->reason) {
		    case int_RFA_reason_notMaster:
			fprintf(err,"*** status error : remote site is not master ***\n");
			*rc = 10;
			break;
		    case int_RFA_reason_locked:
		       t = se->since;
		       fprintf(err,"*** status error : locked at remote site by %s since %s ***\n",	qb2str(se->user), shortTime(&t));
			*rc = 11;
		       break;
		    case int_RFA_reason_notRegistered:
			fprintf(err, "*** status error : file not registered at remote site ***\n");
			*rc = 12;
		        break;
		    case int_RFA_reason_notWritable:
			fprintf(err,"*** status error : file not writable at remote site ***\n");
			*rc = 13;
			break;
		    case int_RFA_reason_wrongVersion:
			fprintf(err, "*** status error : wrong version ***\n");
			*rc = 14;
		    	break;
		    case int_RFA_reason_notRegularFile:
			fprintf(err,"*** status error : not a regular file ***\n");
			*rc = 16;
		    	break;
		    case int_RFA_reason_slaveNewer:
			fprintf(err,"*** status error : local slave version is newer than remote master ***\n");
			*rc = 17;
		    	break;
		    default:
			fprintf(err,"*** status error : unknown error (%d) ***\n",
				se->reason);
			*rc = 19; 
		}
		break;
	default:
	    fprintf(err, "*** unknown error (%d) ***\n", error);
	    *rc = 2;
    }
}

