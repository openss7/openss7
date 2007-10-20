/* udptest.c - test out -ltsap for unit data service over UDP */

#ifndef	lint
static char *rcsid = "$Header: /f/iso/tsap/RCS/tsaptest.c,v 5.0 88/07/21 15:00:04 mrose Rel $";
#endif


/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#include <stdio.h>
#include <ctype.h>
#include "tpkt.h"
#include "tsap.h"
#include "tusap.h"
#include "isoservent.h"
#include "isoaddrs.h"
#include "internet.h"

/*  */

main ()

{

	TUnitDataListen();	/* Listen on a datagram socket */
	TUnitDataBind(); 	/* Bind socket to a remote address */
	TUnitDataUnbind(); 	/* UnBind socket to a remote address */
	TUnitDataRequest();	/* Unit Data write on unbound socket */
 	TUnitDataWrite();	/* Write unit data on a bound socket */
 	TUnitDataRead();	/* Read unit data on a bound socket */
	TUnitDataWakeUp();	/* Sync wakeup routine on kill */

}


