/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : interface.h
 * Description                  : driver.c export functions
 *
 *
 **********************************************************************/

#ifdef _SCO_
SHORT drvrcmnd(WORD,WORD,ADDR *,mblk_t *);
#else
SHORT drvrcmnd(WORD,WORD,ADDR *,BYTE *);
#endif /* _SCO_ */

#define LOCALCFGFILE "antares.cfg"/* Configuration file default name */
#define CFGFILE "/usr/dialogic/config/antares.cfg" /* Configuration file default name */
#define DEFCFGFILE "/usr/dialogic/cfg/antares.cfg" /* default Configuration file default name */

#define MESSAGEAREA 200 /* default message commands area in bytes */
#define MASTERBOARD 0   /* Default master board id */ 

#define SCBUSID      0x1002

/* SCBUS functions options, for ANTARES internal use */
#define GETXMITSLOT   0x35353535 
#define LISTEN        0x36363636
#define UNLISTEN      0x37373737
#define ASSIGNXMIT    0x38383838
#define UNASSIGNXMIT  0x39393939
#define SETXMITSLOT   0x3A3A3A3A
#define UNSETXMITSLOT 0x3B3B3B3B
