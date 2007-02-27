/*****************************************************************************

 @(#) $Id: dlgn.h,v 0.9.2.1 2007/02/27 01:31:22 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2007/02/27 01:31:22 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlgn.h,v $
 Revision 0.9.2.1  2007/02/27 01:31:22  brian
 - added manpages and installed headers

 *****************************************************************************/

#ifndef __SYS_DLGN_N__
#define __SYS_DLGN_N__

#ident "@(#) $RCSfile: dlgn.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

extern char *cf_ioccopy(mblk_t *mp);
extern char *Gn_Endp;
extern char *Gn_Freep;
extern int cf_cmpstr(register unsigned char *sp, register unsigned char *dp, unsigned int bufsz);
extern int Gn_Msglvl;
extern int index;
extern int StatisticsMonitoring;
extern long dlgn_getpeak(void);
extern mblk_t *dlgn_copymp(register mblk_t *mp, register unsigned long size);
extern spinlock_t intmod_lock;
extern spinlock_t st_freelock;
extern spinlock_t st_sctlock;
extern spinlock_t st_uselock;
extern struct gn_board *Gn_Boardp;
extern struct gn_header *Gn_Headerp;
extern struct gn_logdev *Gn_Logdevp;
extern struct _GP_CONTROL_BLOCK *GpControlBlock;
extern struct pm_swtbl pmswtbl[];
extern struct _SRAM_DRV_STATISTICS DriverStats;
extern uchar NumOfVoiceBoards;
extern unsigned long Gn_Maxpm;
extern unsigned long Gn_Memsz;
extern unsigned long Gn_Numpms;
extern void cf_iocput(mblk_t *mp, struct gn_debug *gdp, int msgsz);
extern void dlgn_drvstart(void);
extern void dlgn_drvstop(void);
extern void dlgn_msg(int lvl, const char *fmt, ...) __attribute__ ((__format__(__printf__, 2, 3)));
extern void dlgn_wqreply(queue_t *wq, register mblk_t *mp, int rc);
extern void dl_setintr(int (*intr_handler) (void));

#endif				/* __SYS_DLGN_N__ */
