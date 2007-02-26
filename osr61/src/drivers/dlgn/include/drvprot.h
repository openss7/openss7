/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : drvprot.h
 * Description                  : driver protocol
 *
 *
 **********************************************************************/

#if defined(_SCO_) || defined(LINUX)
SHORT p_an_start(SHORT far *,SHORT far *);
SHORT p_an_open(RCU far *,SHORT, PRC far *,LONG far *);
SHORT p_an_close(SHORT);
SHORT p_send_msg(SHORT,SEND_MSG far *,SYNC_REPLY far *,ULONG,SHORT);
SHORT p_bd_open(SHORT,SHORT,SHORT far *, PRC far *);
SHORT p_bd_close(SHORT);
SHORT p_bd_send_file(SHORT far *,SHORT);
SHORT p_bd_setuio(SHORT, AN_UIO *);
SHORT p_bd_getchan(SHORT, P_FILE *);
SHORT p_bd_xplayf(SHORT, P_FILE far *);
SHORT p_bd_xrecf(SHORT, P_FILE far *);
SHORT p_bd_recv_file(SHORT far *,SHORT);
SHORT p_bd_stop(SHORT);
SHORT p_put_queue(WORD,WORD,WORD,WORD,ADDR *);
SHORT p_raw_open(SHORT);
SHORT p_raw_close(SHORT);
SHORT p_raw_cmd(SHORT,ANT_DB_CMD far *);
SHORT p_get_info(WORD,CFG_INFO far *,SYS_INFO far *,DSP_INFO far *);
SHORT p_get_dongle(WORD, DONGLE_INFO far *);
SHORT p_get_eeprom(WORD, EEPROM_INFO far *);
SHORT p_get_version(ULONG far *,ULONG far *);
SHORT an_all_scbus(SHORT,SC_TSINFO far *,LONG,ULONG,BYTE);
SHORT an_get_scbus(SHORT,SC_TSINFO far *,LONG);
SHORT an_all_route(SHORT,TS_INFO far *,TS_INFO far *);
SHORT p_mapslot(SHORT,MAP_SLOT far *);
#else
SHORT p_an_start(SHORT *,SHORT *);
SHORT p_an_open(RCU *,SHORT, PRC *,LONG *);
SHORT p_an_close(SHORT);
SHORT p_send_msg(SHORT,SEND_MSG *,SYNC_REPLY *,ULONG,SHORT);
SHORT p_bd_open(SHORT,SHORT,SHORT *, PRC *);
SHORT p_bd_close(SHORT);
SHORT p_bd_send_file(SHORT *,SHORT);
SHORT p_bd_setuio(SHORT, AN_UIO *);
SHORT p_bd_getchan(SHORT, P_FILE *);
SHORT p_bd_xplayf(SHORT, P_FILE *);
SHORT p_bd_xrecf(SHORT, P_FILE *);
SHORT p_bd_recv_file(SHORT *,SHORT);
SHORT p_bd_stop(SHORT);
SHORT p_put_queue(WORD,WORD,WORD,WORD,ADDR *);
SHORT p_raw_open(SHORT);
SHORT p_raw_close(SHORT);
SHORT p_raw_cmd(SHORT,ANT_DB_CMD *);
SHORT p_get_info(WORD,CFG_INFO *,SYS_INFO *,DSP_INFO *);
SHORT p_get_dongle(WORD, DONGLE_INFO *);
SHORT p_get_eeprom(WORD, EEPROM_INFO *);
SHORT p_get_version(ULONG *,ULONG *);
SHORT an_all_scbus(SHORT,SC_TSINFO *,LONG,ULONG,BYTE);
SHORT an_get_scbus(SHORT,SC_TSINFO *,LONG);
SHORT an_all_route(SHORT,TS_INFO *,TS_INFO *);
SHORT p_mapslot(SHORT,MAP_SLOT *);
#endif /* _SCO_ */

#ifdef RTDL_ANT 
SHORT p_stop(SHORT *,SHORT *);
#else
SHORT p_stop(VOID);
#endif

/* for antares.h */
SHORT InitTimeValues(VOID);
VOID call_interrupt(VOID);

#if defined(_SCO_) || defined(LINUX)
SHORT init_protocol(VOID far * far *,VOID far * far *,VOID far * far *);
VOID drvrintr(VOID (*imsgp)(VOID));
VOID cmsghdlr(VOID);
#else
SHORT init_protocol(VOID  **,VOID **,VOID **);
void p_an_byteswap(char *,int,int);
#endif  /* _SCO_ */

#ifdef RTDL_ANT 
#define START_BOARDID 0xa1b2  /* key to an_start. 
                                 cause it to init only one board */
#endif
