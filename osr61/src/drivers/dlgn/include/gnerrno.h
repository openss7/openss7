/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gnerrno.h
 * Description                  : generic error
 *
 *
 **********************************************************************/

#ifndef __GNERRNO_H__
#define __GNERRNO_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * Error codes for Config Driver
 */
#define EGN_ERRBASE              0x1000

#define EGN_BADBDCNT             (EGN_ERRBASE | 0x0001)
#define EGN_BADIRQ               (EGN_ERRBASE | 0x0002)
#define EGN_BADLDCNT             (EGN_ERRBASE | 0x0003)
#define EGN_BADLDCNT2            (EGN_ERRBASE | 0x0004)
#define EGN_BADMSGSIZE           (EGN_ERRBASE | 0x0005)
#define EGN_BADNAME              (EGN_ERRBASE | 0x0006)
#define EGN_BADPMCNT             (EGN_ERRBASE | 0x0007)
#define EGN_BADPMID              (EGN_ERRBASE | 0x0008)
#define EGN_BADSTATE0            (EGN_ERRBASE | 0x0009)
#define EGN_BADSTATE1            (EGN_ERRBASE | 0x000A)
#define EGN_BDBUSY               (EGN_ERRBASE | 0x000B)
#define EGN_BDSTARTED            (EGN_ERRBASE | 0x000C)
#define EGN_BDSTARTFAIL          (EGN_ERRBASE | 0x000D)
#define EGN_BDSTOPFAIL           (EGN_ERRBASE | 0x000E)
#define EGN_DRVRSTARTED          (EGN_ERRBASE | 0x000F)
#define EGN_DRVRSTOPPED          (EGN_ERRBASE | 0x0010)
#define EGN_DUPNAME              (EGN_ERRBASE | 0x0011)
#define EGN_INITERR              (EGN_ERRBASE | 0x0012)
#define EGN_NODBSPACE            (EGN_ERRBASE | 0x0013)
#define EGN_NODLSPACE            (EGN_ERRBASE | 0x0014)
#define EGN_NOGBSPACE            (EGN_ERRBASE | 0x0015)
#define EGN_NOGLSPACE            (EGN_ERRBASE | 0x0016)
#define EGN_NOMEM                (EGN_ERRBASE | 0x0017)
#define EGN_OPENFAIL             (EGN_ERRBASE | 0x0018)
#define EGN_PMBUSY               (EGN_ERRBASE | 0x0019)
#define EGN_PMSTARTED            (EGN_ERRBASE | 0x001A)
#define EGN_PMSTARTFAIL          (EGN_ERRBASE | 0x001B)
#define EGN_PMSTOPFAIL           (EGN_ERRBASE | 0x001C)
#define EGN_PMSTOPPED            (EGN_ERRBASE | 0x001D)
#define EGN_BADSTATE             (EGN_ERRBASE | 0x001E)
#define EGN_BADCMD               (EGN_ERRBASE | 0x001F)


#endif

