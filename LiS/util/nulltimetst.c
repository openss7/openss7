/*****************************************************************************

 @(#) $RCSfile: nulltimetst.c,v $ $Name:  $($Revision: 1.1.1.1.12.3 $) $Date: 2005/07/18 11:51:27 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2005/07/18 11:51:27 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: nulltimetst.c,v $ $Name:  $($Revision: 1.1.1.1.12.3 $) $Date: 2005/07/18 11:51:27 $"

static char const ident[] =
    "$RCSfile: nulltimetst.c,v $ $Name:  $($Revision: 1.1.1.1.12.3 $) $Date: 2005/07/18 11:51:27 $";

#define	inline			/* make disappear */

#include <sys/stream.h>
#include <sys/stropts.h>
#include <sys/LiS/loop.h>	/* an odd place for this file */

#include <unistd.h>
#include <time.h>
#ifdef LINUX
#include <stdio.h>
#include <fcntl.h>
#include "linuxio.h"
#else
#ifdef SYS_QNX
#include "qnxio.h"
#include <ioctl.h>
#else
#include "usrio.h"
#endif
#endif

/************************************************************************
*                      File Names                                       *
************************************************************************/
#ifdef LINUX
#define NULL_CLONE	"/dev/null_clone"
#define	LOOP_1		"/dev/loop.1"
#else
#ifdef QNX
#define NULL_CLONE	"/dev/gcom/null_clone"
#define	LOOP_1		"/dev/gcom/loop.1"
#else
#define NULL_CLONE	"null_clone"
#define	LOOP_1		"loop.1"
#endif
#endif

/************************************************************************
*                           Storage                                     *
************************************************************************/

char buf[1000];				/* general purpose */
char rdbuf[1000];			/* for reading */
long iter_cnt = 100000;			/* default iteration count */
char ctlbuf[1000];			/* control messages */
char rdctlbuf[1000];			/* control messages */

struct strbuf wr_ctl = { 0, 0, ctlbuf };
struct strbuf wr_dta = { 0, 0, buf };
struct strbuf rd_ctl = { 0, 0, rdctlbuf };
struct strbuf rd_dta = { 0, 0, rdbuf };

extern void make_nodes(void);

/************************************************************************
*                           register_drivers                            *
*************************************************************************
*									*
* Register the drivers that we are going to use in the test with	*
* streams.								*
*									*
************************************************************************/
void
register_drivers(void)
{
#if	!defined(LINUX) && !defined(QNX)
	port_init();		/* stream head init routine */
#endif

}				/* register_drivers */

/************************************************************************
*                           timing_test                                 *
*************************************************************************
*									*
* Time reading and writing messages through streams.			*
*									*
************************************************************************/
void
timing_test(void)
{
	time_t time_on;
	time_t et;
	long i;
	int fd1;
	int rslt;
	int lgth;
	int arg;
	int flags = MSG_ANY;
	int rband = 0;
	struct strioctl ioc;

	printf("\nBegin timing test\n");

	fd1 = user_open(NULL_CLONE, O_RDWR, 0);
	if (fd1 < 0) {
		printf("null_clone: %s\n", strerror(-fd1));
		return;
	}

	strcpy(buf, "Data to send down the file");
	lgth = strlen(buf);

	printf("Time test:  send %d bytes using write to null device: ", lgth);
	fflush(stdout);

	sync();			/* do file sync now rather than in the middle of the test. */
	time_on = time(NULL);
#if 1
	for (i = 0; i < iter_cnt; i++) {
		rslt = user_write(fd1, buf, lgth);

		if (rslt != lgth) {
			if (rslt < 0)
				printf("null.1: write: %s\n", strerror(-rslt));
			else
				printf("null.1:  write returned %d, expected %d\n", rslt, lgth);

			break;
		}
	}
#endif
	et = (time(NULL) - time_on) * 1000000;	/* time in usecs */

	printf("%ld micro-secs\n", et / iter_cnt);

	printf("Time test:  send %d bytes using putmsg to null device: ", lgth);
	fflush(stdout);
	sync();
	time_on = time(NULL);

	rd_ctl.maxlen = sizeof(rdctlbuf);
	rd_dta.maxlen = sizeof(rdbuf);
	wr_ctl.len = -1;
	wr_dta.len = lgth;
	for (i = 0; i < iter_cnt; i++) {
		rslt = user_putpmsg(fd1, &wr_ctl, &wr_dta, 0, MSG_BAND);

		if (rslt < 0) {
			printf("null.1: putmsg: %s\n", strerror(-rslt));
			break;
		}
	}

	et = (time(NULL) - time_on) * 1000000;	/* time in usecs */

	printf("%ld micro-secs\n", et / iter_cnt);

	user_close(fd1);

}				/* timing_test */

/************************************************************************
*                          set_debug_mask                               *
*************************************************************************
*									*
* Use stream ioctl to set the debug mask for streams.			*
*									*
************************************************************************/
void
set_debug_mask(long msk)
{
	int fd;
	int rslt;

	fd = user_open(LOOP_1, 0, 0);
	if (fd < 0) {
		printf("loop.1: %s\n", strerror(-fd));
		return;
	}

	rslt = user_ioctl(fd, I_LIS_SDBGMSK, msk);
	if (rslt < 0) {
		printf("loop.1: I_LIS_SDBGMSK: %s\n", strerror(-rslt));
		return;
	}

	printf("\nSTREAMS debug mask set to 0x%08lx\n", msk);

	user_close(fd);

}				/* set_debug_mask */

/************************************************************************
*                              main                                     *
************************************************************************/
void
main(int argc, char **argv)
{
	if (argc > 1)
		sscanf(argv[1], "%ld", &iter_cnt);

#if	!defined(LINUX) && !defined(QNX)
	register_drivers();
	make_nodes();
#endif

	printf("Timing test version %s\n\n", "1.4 4/25/97");
	printf("Using safe constructs and message tracing\n");
	set_debug_mask(0x30000L);
	/* set_debug_mask(0x0FFFFFFF) ; */
	timing_test();

	printf("\n\nNot using safe constructs or message tracing\n");
	set_debug_mask(0L);
	timing_test();
}
