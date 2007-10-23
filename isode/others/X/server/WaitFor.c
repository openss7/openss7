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

/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/*****************************************************************
 * OS Depedent input routines:
 *
 *  WaitForSomething,  GetEvent
 *
 *****************************************************************/

#include "Xos.h"		/* for strings, fcntl, time */

#include <errno.h>
#include <stdio.h>
#include "X.h"
#include "misc.h"

#include <sys/param.h>
#include <signal.h>
#include "osdep.h"
#include "dixstruct.h"

extern long AllSockets[];
extern long AllClients[];
extern long LastSelectMask[];
extern long WellKnownConnections;
extern long EnabledDevices;
extern long ClientsWithInput[];
extern long ClientsWriteBlocked[];
extern long OutputPending[];

extern long ScreenSaverTime;		/* milliseconds */
extern long ScreenSaverInterval;	/* milliseconds */
extern ClientPtr ConnectionTranslation[];

extern Bool clientsDoomed;
extern Bool NewOutputPending;
extern Bool AnyClientsWriteBlocked;

extern void CheckConnections();
extern void EstablishNewConnections();

extern int errno;

int isItTimeToYield = 1;

#ifdef MULTI_X_HACK
extern int XMulti;
extern int sigwindow_handler();
#endif	/* MULTI_X_HACK */

#ifdef XTESTEXT1
/*
 * defined in xtestext1dd.c
 */
extern int playback_on;
#endif				/* XTESTEXT1 */

/*****************
 * WaitForSomething:
 *     Make the server suspend until there is
 *	1. data from clients or
 *	2. input events available or
 *	3. ddx notices something of interest (graphics
 *	   queue ready, etc.) or
 *	4. clients that have buffered replies/events are ready
 *
 *     If the time between INPUT events is
 *     greater than ScreenSaverTime, the display is turned off (or
 *     saved, depending on the hardware).  So, WaitForSomething()
 *     has to handle this also (that's why the select() has a timeout.
 *     For more info on ClientsWithInput, see ReadRequestFromClient().
 *     pClientsReady is a mask, the bits set are 
 *     indices into the o.s. depedent table of available clients.
 *     (In this case, there is no table -- the index is the socket
 *     file descriptor.)  
 *****************/

#ifdef ISOCONN
/*
 * cannot use select in TSAP stuff, so...see TNetAccept
 * (which calls xselect, which calls select...)
 */

#include <isode/tsap.h>
#endif				/* ISOCONN */

static long timeTilFrob = 0;		/* while screen saving */

#if (mskcnt>4)
/*
 * This is a macro if mskcnt <= 4
 */
ANYSET(src)
	long *src;
{
	int i;

	for (i = 0; i < mskcnt; i++)
		if (src[i])
			return (TRUE);
	return (FALSE);
}
#endif

WaitForSomething(pClientsReady, nready, pNewClients, nnew)
	ClientPtr *pClientsReady;
	int *nready;
	ClientPtr *pNewClients;
	int *nnew;
{
	int i;
	struct timeval waittime, *wt;
	long timeout;
	long clientsReadable[mskcnt];
	long clientsWritable[mskcnt];
	long curclient;
	int selecterr;

#ifdef ISOCONN
	int vecp;
	char *vec[4];
	struct TSAPdisconnect tds;
	struct TSAPdisconnect *td = &tds;
#endif				/* ISOCONN */

#ifdef	hpux
	long ready_inputs;		/* to tell HIL drivers about input */
#endif	/* hpux */

	*nready = 0;
	*nnew = 0;
	CLEARBITS(clientsReadable);
	if (!(ANYSET(ClientsWithInput))) {
		/* We need a while loop here to handle crashed connections and the screen saver
		   timeout */
		while (1) {
			if (ScreenSaverTime) {
				timeout = ScreenSaverTime - TimeSinceLastInputEvent();
				if (timeout <= 0) {	/* may be forced by AutoResetServer() */
					long timeSinceSave;

					if (clientsDoomed) {
						*nnew = *nready = 0;
						break;
					}

					timeSinceSave = -timeout;
					if ((timeSinceSave >= timeTilFrob) && (timeTilFrob >= 0)) {
						SaveScreens(SCREEN_SAVER_ON, ScreenSaverActive);
						if (ScreenSaverInterval)
							/* round up to the next ScreenSaverInterval 
							 */
							timeTilFrob = ScreenSaverInterval *
							    ((timeSinceSave + ScreenSaverInterval) /
							     ScreenSaverInterval);
						else
							timeTilFrob = -1;
					}
					timeout = timeTilFrob - timeSinceSave;
				} else {
					if (timeout > ScreenSaverTime)
						timeout = ScreenSaverTime;
					timeTilFrob = 0;
				}
				if (timeTilFrob >= 0) {
					waittime.tv_sec = timeout / MILLI_PER_SECOND;
					waittime.tv_usec = (timeout % MILLI_PER_SECOND) *
					    (1000000 / MILLI_PER_SECOND);
					wt = &waittime;
				} else {
					wt = NULL;
				}
			} else
				wt = NULL;
#ifdef MULTI_X_HACK
			if (XMulti) {
				ipc_block_handler();
				signal(SIGWINDOW, sigwindow_handler);
			}
#endif	/* MULTI_X_HACK */
			COPYBITS(AllSockets, LastSelectMask);
			BlockHandler(&wt, LastSelectMask);
			if (NewOutputPending)
				FlushAllOutput();
#ifdef XTESTEXT1
			/* XXX how does this interact with new write block handling? */
			if (playback_on) {
				wt = &waittime;
				XTestComputeWaitTime(&waittime);
			}
#endif				/* XTESTEXT1 */

#ifndef ISOCONN
			if (AnyClientsWriteBlocked) {
				COPYBITS(ClientsWriteBlocked, clientsWritable);
				i = select(MAXSOCKS, LastSelectMask, clientsWritable,
					   (int *) NULL, wt);
			} else {
				i = select(MAXSOCKS, LastSelectMask,
					   (int *) NULL, (int *) NULL, wt);
			}
			selecterr = errno;
#else				/* IS ISOCONN */
			if (AnyClientsWriteBlocked) {
				int secs = wt->tv_sec;

				if (secs == 0 && wt->tv_usec != 0)
					secs = 1;
				COPYBITS(ClientsWriteBlocked, clientsWritable);
				i = TNetAccept(&vecp, vec, MAXSOCKS, LastSelectMask,
					       clientsWritable, (int *) NULL, wt->tv_sec, td);
			} else {
				int secs = wt->tv_sec;

				if (secs == 0 && wt->tv_usec != 0)
					secs = 1;
				i = TNetAccept(&vecp, vec, MAXSOCKS, LastSelectMask,
					       (int *) NULL, (int *) NULL, secs, td);
			}
/* map errors */
			if (i < 0) {
				if (!DR_FATAL(td->td_reason))
					errno = EWOULDBLOCK;
			}
			selecterr = errno;
#endif				/* ISOCONN */
			WakeupHandler(i, LastSelectMask);
#ifdef XTESTEXT1
			if (playback_on) {
				i = XTestProcessInputAction(i, &waittime);
			}
#endif				/* XTESTEXT1 */
			if (i <= 0) {	/* An error or timeout occurred */
#ifdef ISODEBUG
				if (isodexbug)
					fprintf(stderr, "WF: TO or ERR %d\n", i);
#endif
				CLEARBITS(clientsWritable);
				if (i < 0)
					if (selecterr == EBADF) {	/* Some client disconnected 
									 */
						CheckConnections();
						if (!ANYSET(AllClients))
							return;
					} else if (selecterr != EINTR)
						ErrorF("WaitForSomething(): select: errno=%d\n",
						       selecterr);
			} else {
				if (AnyClientsWriteBlocked && ANYSET(clientsWritable)) {
#ifdef ISODEBUG
					if (isodexbug)
						fprintf(stderr, "WF: write\n");
#endif
					NewOutputPending = TRUE;
					ORBITS(OutputPending, clientsWritable, OutputPending);
					UNSETBITS(ClientsWriteBlocked, clientsWritable);
					if (!ANYSET(ClientsWriteBlocked))
						AnyClientsWriteBlocked = FALSE;
				}
#ifdef	hpux
				ready_inputs = (LastSelectMask[0] & EnabledDevices);

				if (ready_inputs > 0)
					store_inputs(ready_inputs);
				/* call the HIL driver to gather inputs.  */
#endif	/* hpux */

				MASKANDSETBITS(clientsReadable, LastSelectMask, AllClients);
#ifdef ISOCONN
/*
 * ISODE version says new connections pending accept if vecp > 0 !!
 */
				if (vecp > 0) {
#ifdef ISODEBUG
					if (isodexbug)
						fprintf(stderr, "WF: CX pending\n");
#endif
					EstablishNewConnections(pNewClients, nnew, vecp, vec);
				}
#else				/* ISOCONN */
				if (LastSelectMask[0] & WellKnownConnections)
					EstablishNewConnections(pNewClients, nnew);
#endif				/* ISOCONN */
				if (*nnew || (LastSelectMask[0] & EnabledDevices)
				    || (ANYSET(clientsReadable)))
					break;
			}
		}
	} else {
		COPYBITS(ClientsWithInput, clientsReadable);
	}

	if (ANYSET(clientsReadable)) {
#ifdef ISODEBUG
		if (isodexbug)
			fprintf(stderr, "WF: readable\n");
#endif
		for (i = 0; i < mskcnt; i++) {
			while (clientsReadable[i]) {
				curclient = ffs(clientsReadable[i]) - 1;
				pClientsReady[(*nready)++] =
				    ConnectionTranslation[curclient + (32 * i)];
				clientsReadable[i] &= ~(1 << curclient);
			}
		}
	}
}
