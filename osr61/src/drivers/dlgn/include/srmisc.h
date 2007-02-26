/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : srmisc.h
 * Description                  : SRAM protocol - srmisc.c header
 *
 *
 **********************************************************************/

#ifndef __SRMISC_H__
#define __SRMISC_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * GET_IOPORT(n) -- Given a GN_BOARD pointer, returns the IO port
 * value from the associated DX_BOARD structure.
 */
#define GET_IOPORT(n)   (((DX_BOARD *)((n)->bd_devdepp))->db_sr.br_ioport)
#ifdef PCI_SPAN
#define GET_PCIBRDID(n)   (((DX_BOARD *)((n)->bd_devdepp))->db_sr.br_pcibrdid)
#endif
#define DMA_MIN_XFER_SIZE	128

/*
 * Function prototypes
 */
#ifdef LFS
extern void sr_loopstop(mblk_t *, GN_LOGDEV *);
extern int  sr_putstop(GN_LOGDEV *, mblk_t *, UCHAR);
extern int  sr_sendreply(GN_LOGDEV *, mblk_t *, int);
extern void sr_reply(GN_LOGDEV *, mblk_t *, int);
extern int  sr_getdata(mblk_t **, UCHAR *, ULONG, ULONG);
extern int  sr_putdata(mblk_t **, UCHAR *, ULONG);
extern void sr_cleanup(GN_LOGDEV *, ULONG);
extern void sr_flush(void);
extern void sr_putsilence(register UCHAR *, register ULONG);
extern int  sr_bdreset(GN_BOARD *);
extern void sr_setqdefaults(register DX_LOGDEV *);
#else
extern void sr_loopstop();
extern int  sr_putstop();
extern int  sr_sendreply();
extern void sr_reply();
extern int  sr_getdata();
extern int  sr_putdata();
extern void sr_cleanup();
extern void sr_flush();
extern void sr_putsilence();
extern int  sr_bdreset();
extern void sr_setqdefaults();
#endif


extern SR_IRQLIST *Sr_Irq[];


#endif

