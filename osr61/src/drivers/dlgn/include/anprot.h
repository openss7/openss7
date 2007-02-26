/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : anprot.h
 * Description                  : antares protocol definition
 *
 *
 **********************************************************************/

#ifdef LFS
#undef far
#define far
#undef huge
#define huge
#endif

#if defined(_SCO_) || defined(LINUX)
VOID init_interface(CFG_INFO far *);
CHAR far *ant_forceaddr(CHAR huge *);
#else
VOID init_interface(CFG_INFO *);
CHAR *ant_forceaddr(CHAR huge *);
#endif /* _SCO_ */

VOID ant_id_click(VOID);

VOID ant_memmov(ADDR *,ADDR *,WORD);

SHORT send_delay_request(SHORT,BYTE,BYTE,BYTE,LONG,ADDR*); 
#if defined(_SCO_) || defined(LINUX)
SHORT  xant_fileread(int,ADDR *,WORD far *,ADDR *);
SHORT  xant_filewrite(int,ADDR *,WORD far *,ADDR *);
SHORT  xant_fileseek(SHORT,int,WORD,ULONG,LONG far *,ADDR *);

SHORT far xsend_bulkdata_msg(BYTE,SHORT,SHORT,SHORT,SHORT,SHORT,SHORT,ULONG,ADDR *);
SHORT far xsend_error_msg(SHORT,SHORT,SHORT,SHORT,SHORT,ADDR *);
SHORT far xsend_dsp_msg(BYTE,SHORT,WORD,WORD,WORD,ADDR*,SHORT,SHORT,RCU,ADDR*);

ADDR *xx_an_open(SHORT,RCU far *,SHORT, PRC far *,LONG far *);
ADDR *xx_bd_open(SHORT, SHORT, SHORT, SHORT far *, PRC far *);
#else
SHORT xsend_bulkdata_msg(BYTE,SHORT,SHORT,SHORT,SHORT,SHORT,SHORT,ULONG,ADDR *);
SHORT xsend_error_msg(SHORT,SHORT,SHORT,SHORT,SHORT,ADDR *);
SHORT xsend_dsp_msg(BYTE,SHORT,WORD,WORD,WORD,ADDR*,SHORT,SHORT,RCU,ADDR*);

ADDR *xx_an_open(SHORT,RCU *,SHORT, PRC *,LONG *);
ADDR *xx_bd_open(SHORT, SHORT, SHORT, SHORT *, PRC *);
#endif /* _SCO_ */
SHORT xx_an_close(SHORT);
SHORT xx_bd_close(SHORT); 
