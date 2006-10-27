/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

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

 Last Modified $Date$ by $Author$

 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/************************************************************************
*                        Version File                                   *
*************************************************************************
*									*
* This file is to document changes to STREAMS.  It contains the version	*
* number for STREAMS.  When you make a new version, bump the version	*
* number and add comments in here for what is in the new version.	*
*									*
* We declare a char array in this module that others can use to		*
* refer to the version number in ASCII string form.			*
*									*
* Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>		*
*									*
************************************************************************/

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

char lis_version[] = PACKAGE_VERSION "." PACKAGE_RELEASE;
char lis_date[] = PACKAGE_DATE;

/*

1.2	Changes made as follows:

	Timeouts mapped correctly into local ticks
	Makefile fix for Red Hat 4.0
	getmsg with several blocks did not work correctly
	Needed prototype for flushband
	Needed to make lis_qdetach extern
	Fix problems with I_FIND, I_PEEK
	Code for I_FDINSERT from Eric Levenez
	Fix DEFAULT_RC_SEL in npi.h
	Read did not return when it ran out of queued data
	putmsg/write did not unblock when downstream flow control opened up
	Remove DMA attribute from memory allocation
	Put in stub for allocb_physreq()

1.3	Changes made as follows:

	Put magic number into queue structure so as to verify validity
	Check both read and write queues for sched list at close time
	Print out version number at boot time

1.4	Changes made as follows:
	Add initialization function for STREAMS drivers.  Declared
	in STREAMS config file.
	More modularity for packages.  Each package has its own config
	file.  All configs are concatenated together.

1.5	Changes made as follows:
	STREAMS subsystem made into loadable module.
	Fix bug in select() on STREAMS files not waking up properly.
	
1.6	Changes made as follows:
	lis_freeb had problem if dblk had ref cnt > 1.  It would sometimes
	free the mblk that contained the dblk that other mblks pointed
	to for benefit of the reference count.
	pullupmsg needs to fail if the mblk has use count > 1 since it
	is going to reassign the base/lim pointers in the dblk.

1.8	Add putnextctl and putnextctl1 functions

1.9	Add GNU Library license notice to all .c and .h files

1.10	For esballoc, keep the frtn structure in the dblk so the user does
        not have to allocate those structures.  NON-SVR4 feature.
	Fix race between strread, strputmmsg versus strrput if strrput were
	to get called at interrupt time.

1.11	Lower mux queues need arming at I_LINK time.
	ioctl with INFTIM was timing out.

1.12	Timeout callback functions changed to type void rather than int

1.13	Changed linux sys include files to linux include files to be able
	to compile on RedHat  5.0

1.14	Had to fix a problem in the 1.13 above
1.15	Had to fix a problem in the 1.14 above

1.16
1.17	putpmsg was misusing canput in such a way as to render it
	ineffective.

1.18	stream head write service routine did not perform correctly to
	wake up a blocked writer when canput blockage relieved.

1.19	open routine not calling driver open routine correctly for 2nd open
	when driver open failed, open freed an in-use queue structure
	bring driver MODOPEN, CLONEOPEN bits into conformance with SVR4
	MUST RECOMPILE DRIVERS w/NEW mod.h TO USE THIS VERSION OF LiS

1.20	Bug in poll using select for non-STREAMS files.
	Clear datap in new mblk in case cannot allocate dblk.
	Move stats printing out of stats.c and into streams.c utility.
	Remove syscall definitions from .h files.  Replace with prototypes.
	Introduce libc routines for getpmsg, putpmsg, poll. libLiS.a
	Use S_ZERO_WR flag for 2.0.36.
	Eliminate #ifdef LINUX in some .h files.
	Removed ()s from macros in stream.h.
	Now have a non-null tihdr.h.  Updates to dlpi.h.
	Delete some extraneous includes leading to dependency anomolies.
	Installation and build procedure now outside of kernel tree.

1.21
1.22
1.23	Fix problem with printk not working when installed as module.
	Eliminate SEM_INIT (2.1 kernel problems).
	Some changes for 2.1 compatibility (not ready for 2.1 yet, though).
	Finalize Configure script.

1.24	More changes for 2.1 compatibility.
	Prevent running of queues interrupting putnext operation.
	Ioctl default timeout now implemented (15 secs).

1.25	Candidate final changes for kernel version 2.2.x compatibility.
	Fixed bug in which stream head putpmsg did not send M_PCPROTO
	downstream if flow control blocked; it returned EAGAIN instead.
	Queues run either as bottom half or as kernel thread.

2.0
2.1
2.2	Much better support for loadable modules and kerneld.
	Support for 2.2.x kernels.
	The ldl driver has been used and tested more.
	Implementation of transparent ioctls.
	make sure getpmsg returns correct more bits not obscured by err codes
	ioctl to print queues, streams -q triggers it
	stream head flow ctl vs M_PCPROTO
	M_SETOPTS incorrect for hi/lo water and min/max psize
	run queues either as bottom half or kernel thread process (real time)
	fix problem  with noenable function
	fix a few minor problems with poll
	Installation improvements
	Documentation in html

2.3	Fix a few installation problems with 2.2.
	Get user mode working.
	Change prefix used by osif to "lis_" instead of "osif_".

2.4	Fix a few more installation problems.
	Ole has a start on a SAD driver.
	Updated ldl driver.

2.5	Fix locking for multiple writers to same stream.
	Update html documentation.

2.6	Make ip_strm_mod work properly and for kernel version 2.2.x.
	Improve installation vis a vis SMP.
	Additions to osif.[ch].
	Add raw HDLC interface type to ldl.
	Improve kernel patch for drivers/char/mem.c -- used for
		linking LiS directly into kernel.
	Clone open now allowed to re-use exiting minor dev.

2.7	qprocson/qprocsoff dummies for SVR4 MP compatibility.
	More Linux kernel interface functions in osif.c.
	Compatibility with 2.3.x kernels.
	Compatibility with new glibc stropts.h and poll.h
	Ability to auto-attach IRQs for STREAMS drivers.
	Callout to a driver terminate routine when unloading driver.
	Make module name size compatible with SVR4 (smaller).
	Integrate changes for SPARC version.
	Read of M_PROTO should requeue msg for SVR4 compatibility.
	Portable poll code was not including POLLERR, POLLHUP in events.
	Include POLLOUT with POLLWRNORM if band zero is writeable.
	Implement I_PUNLINK(MUXID_ALL) in SVR4 compatible way.
	Change iocblk structure to conform with SVR4.
	ldl driver: make sure queued messages stay FIFO.
	ldl driver: kernel 2.3.x compatibility.
	ip_strm_mod: better flow control coupling.
	ip_strm_mod: print msg with SIOCSIFNAME succeeds.
	ip_strm_mod: kernel 2.3.x compatibility.
	strtst: tests for bug fixes and new features.
	strconf: "interrupt" and "terminate" declarations.
	libLiS: dummies for fattach, fdetach, isastream.

2.7.1	Fix a bug in read vs M_PCPROTO requeueing mecanism.
	ip_strm_mod: new flow control technique for net drivers.
	ip_strm_mod: ASSERT interfered with IRDA
	More SPARC compatibility.

2.8	A few more tweaks for SPARC.
	Testing for I_LINKed streams in read/write/getmsg/putmsg.
	ldl compiles on 2.3.x kernels (not tested).
	Fix a few warnings.

2.9	Mainly a beta release on the way to 2.10

2.10	fattach/fdetach implemented
	STREAMS pipes and fifos
	SMP compatibility (defensive)
	LiS insulation for drivers (mem, pci, locks, semaphores)
	Updated documentation
	Compatibility with glibc 2.2 (stropts.h)
	Improved installation scripts

2.11	Compatibility with 2.4.0 kernel

2.12	Aggressive SMP in 2.4.0 kernel
	More abstractions to protect drivers from kernel
	Numerous other changes noted in htdocs

2.15	Fix holding spin locks while sleeping (big change)
    	fattach works again
	pipes/fifos work again
	Compatibility with more kernel versions

2.16	Eliminate use of Linux system calls
	Eliminate all kernel patches from distribution
	Some compatibility changes for newer kernels

*/
