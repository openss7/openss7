/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

 /***********************************************************
 * Equates and structures for the DIALOG/4x firmware Ver 60 *
 ************************************************************
 *                                                          *
 *   DDDDDD     4444    00000   LL        OOOOO   W     W   *
 *   DD   DD   44 44   00   00  LL       OO   OO  W     W   *
 *   DD   DD  44  44   00   00  LL       OO   OO  W     W   *
 *   DD   DD  44  44   00   00  LL       OO   OO  W  W  W   *
 *   DD   DD  4444444  00   00  LL       OO   OO  W WWW W   *
 *   DD   DD      44   00   00  LL       OO   OO  WWW WWW   *
 *   DDDDDD       44    00000   LLLLLL    OOOOO    W   W    *
 *                                                          *
 ************************************************************
 ***********************************************************/
#define D40LOW     /* we are included */

#ifndef __D40LOW_H__
#define __D40LOW_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * Misc. values
 */
#define  NULLSPTR      0x00FFFF  /* null silence pointer */

/*
 * reset complete values
 */
#define  DLR_SUCC      0x000     /* board was successfully reset */
#define  DLR_MEMFAIL   0x001     /* board failed memory test */
#define  DLR_REGFAIL   0x002     /* board failed register test */
#define  DLR_FLAGFAIL  0x003     /* board failed flags test */
#define  DLR_CHKSUM    0x004     /* board failed checksum test */
#define  DLR_PRODID    0x005     /* invalid product ID code */
#define  DLR_CHNUM     0x006     /* invalid number of channels */

/*
 * Equates for Dialog/4x to PC command codes.
 * The following messages are enabled by the set_IEN command.
 */
#define  DL_B0FULL     0x001     /* buffer 0 full */
#define  DL_B1FULL     0x002     /* buffer 1 full */
#define  DL_B0EMPTY    0x003     /* buffer 0 empty */
#define  DL_B1EMPTY    0x004     /* buffer 1 empty */

/*
 * Equates for Dialog/4x to PC command codes.
 * The following message is enabled by the 'old' param. of the PC_INITBUF cmd.
 */
#define  DL_DIGITRDY   0x000     /* digit collected */

/*
 * Equates for Dialog/4x to PC command codes.
 * The following messages are always enabled.
 */
#define  DL_RECORD     0x008     /* record complete */
#define  DL_RECCMPLT   DL_RECORD /* duplicate name for driver */
#define  DL_PLAYBACK   0x007     /* playback complete */
#define  DL_PLAYCMPLT  DL_PLAYBACK /* duplicate name for driver */
#define  DL_DIAL       0x005     /* dial complete */
#define  DL_BEEP       0x00A     /* beep complete */

#define  DL_COMTEST    0x00B     /* communication test */
#define  DL_ACK        0x00C     /* communication test acknowledgement */

#define  DL_CST        0x006     /* call status transition */

#define  DL_ERROR      0x009     /* PC to DIALOG/4X command error */

#define  DL_MTST       0x00D     /* memory test complete */

/*
 * New equates for Dialog/4x to PC command codes supported by firmware ver.60.
 * The following messages are always enabled.
 */
#define  DL_RPTBUF     0x15      /* digit buffer count */
#define  DL_GETDIGITS  0x10      /* get digits complete */
#define  DL_UPDPARM    0x16      /* update of parameters complete */
#define  DL_GETPARM    0x13      /* current parameters */
#define  DL_WINK       0x17      /* wink complete */
#define  DL_TIMER      0x11      /* timer reached zero */
#define  DL_WATCHDOG1  0x12      /* 1st watchdog timeout */
#define  DL_WATCHDOG2  0x14      /* 2nd watchdog timeout */

#define  MAXDLCMD      0x17      /* maximum number of DIALOG/4X commands */

/*
 * Special equates for new Dialog/4x to PC command codes to replace certain
 * CST event flavors.
 */
#define  DL_OFFHOOKC   0x30      /* offhook complete */
#define  DL_ONHOOKC    0x31      /* onhook complete */
#define  DL_CALL       0x32      /* call analysis complete */

/*
 * Special equates for simulated Dialog/4x to PC command codes to support
 * driver handling of play and record.
 */
#define  DL_BUFEMPTY   0x40      /* virtual buffer empty */
#define  DL_BUFFULL    0x41      /* virtual buffer full */

/*
 * Equates for PC to Dialog/4x command codes.
 * The following commands do not cause the board to send a DL reply msg.
 */
#define  PC_SETIEN     0x007     /* set the interrupt enable mask */
#define  PC_SETRINGS   0x005     /* set number of rings for cst event */
#define  PC_PULSE      0x00B     /* enable pulse digit collection */

/*
 * Equates for PC to Dialog/4x command codes.
 * The following commands cause the board to send a DL reply msg.
 */
#define  PC_RECORD     0x001     /* place channel in record mode */
#define  PC_PLAYBACK   0x002     /* place channel in playback mode */
#define  PC_DIAL       0x006     /* dial ASCIIZ string in buf0 */
#define  PC_BEEP       0x008     /* start beeping */

#define  PC_COMTEST    0x009     /* communication test */
#define  PC_ACK        0x00A     /* communication test response */

#define  PC_OFFHOOK    0x004     /* place channel offhook */
#define  PC_ONHOOK     0x003     /* place channel onhook */

#define  PC_STOP       0x000     /* stop operation in progress */

/*
 * New equates for PC to Dialog/4x command codes supported by firmware ver.60.
 * The following commands do not cause the board to send a DL reply msg.
 */
#define  PC_SETTERMS   0x14      /* set terminating conditions */
#define  PC_CLRHIST    0x19      /* clear termination history */
#define  PC_INITBUF    0x10      /* initialize digit buffer */
#define  PC_CLRBUF     0x11      /* clear digit buffer */

/*
 * New equates for PC to Dialog/4x command codes supported by firmware ver.60.
 * The following commands cause the board to send a DL reply msg.
 */
#define  PC_RPTBUF     0x12      /* report digit buffer count */
#define  PC_GETDIGITS  0x13      /* get digits */
#define  PC_UPDPARM    0x17      /* update parameters */
#define  PC_GETPARM    0x18      /* get current parameters */
#define  PC_WINK       0x1a      /* initiate a wink command */
#define  PC_TIMER      0x15      /* start timer */
#define  PC_WATCHDOG   0x16      /* start watchdog */

#define  PC_UPDNOBUFF  0x1F      /* update parms using control buffer */
#define  PC_GETNOBUFF  0x20      /* get current parms using control buffer */
#define  PC_VADREARM   0x21      /* voice activated detection re-arm */

#define  MAXPCCMD      0x1a      /* maximum number of PC commands */

/*
 * Equates for PC to Dialog/4x at boot up time.
 */
#define  REQ_RESET     0x00FF    /* clear, reset and go */

#define  REQ_MTST      0x00FE    /* do the memory test and stop */

/*
 * Call status transition data equates.
 * The following CST's are enabled by the set_IEN command.
 */
#define  CST_OFFHOOK    0x001    /* offhook transition complete */
#define  CST_HOOK_DONE  CST_OFFHOOK /* duplicate name for driver */
#define  CST_ONHOOK     0x006    /* onhook transition complete */
#define  CST_ONH_DONE   CST_ONHOOK /* duplicate name for driver */
#define  CST_RINGS      0x002    /* rings received */
#define  CST_WINKRCVD   0x00E    /* wink received */
#define  CST_RNGOFF     0x00F    /* ring off messages */

#define  CST_LCOFF      0x003    /* loop current off */
#define  CST_LCON       0x00D    /* loop current on */
#define  CST_LCREV      0x020    /* loop current reversal */
#define  CST_SILENCEOFF 0x004    /* silence off */
#define  CST_SILENCEON  0x005    /* silence on */
#define  CST_VAD        0x013    /* voice activity detected */

/*
 * Call status transition data equates.
 * The following CST's are always enabled.
 */
#define CST_BUSY        0x007    /* line busy  (call analysis) */
#define CST_NOANS       0x008    /* no answer (call analysis) */
#define CST_NORB        0x009    /* no ringback (call analysis) */
#define CST_CNCT        0x00A    /* call connected (call analysis) */
#define CST_CEPT        0x00B    /* operator intercept (call analysis) */
#define CST_STOPD       0x00C    /* call analysis stopped (call analysis) */
#define CST_NO_DIALTONE 0x011
#define CST_FAXTONE     0x012
#define CST_DISCONNECT  0x022	 /* Calling party disconnect */
#define CST_HKFLSHRCVD  0x021    /* D/160SC-LS Hook Flash detected */

/*
 * Interrupt enable(IEN) bits, if bit is 1(on) then int is enabled, 0 is
 * disabled for set_ien message.
 * The following bits enable DL reply messages.
 */
#define  IEN_BUFEMPTY   0x00001  /* enable buffer empty messages */
#define  IEN_BUFFULL    0x00002  /* enable buffer full messages */

/*
 * Interrupt enable(IEN) bits, if bit is 1(on) then int is enabled, 0 is
 * disabled for set_ien message.
 * The following bits enable flavors of the DL_CST message.
 */
#define  IEN_OFFHOOK    0x00020  /* enable offhook complete messages */
#define  IEN_ONHOOK     0x00100  /* enable onhook complete messages */
#define  IEN_RINGS      0x00010  /* enable rings received messages */
#define  IEN_WINKRCVD   0x00800  /* enable wink received messages */
#define  IEN_RNGOFF     0x01000  /* enable ring off messages */

#define  IEN_LCOFF      0x00008  /* enable loop current drop messages */
#define  IEN_LCON       0x00400  /* enable loop current on messages */
#define  IEN_LCREVON    0x02000  /* enable loop current reversal messages */
#define  IEN_LCREVOFF   0x04000  /* enable loop current reversal messages */
#define  IEN_LCREV      0x06000  /* enable loop current reversal messages */
#define  IEN_SILENCEOFF 0x00080  /* enable silence off messages */
#define  IEN_SILENCEON  0x00040  /* enable silence on messages */
#define  IEN_VADEVTS    0x00200  /* enable voice activity detected messages */

/*
 * Interrupt enable(IEN) bits, if bit is 1(on) then int is enabled, 0 is
 * disabled for set_ien message.
 * The following bit has no effect on DL reply messages.
 */
#define  IEN_DTMF       0x00004  /* enable DTMF digit collection */

/*
 * pdd reception type - data value for PC_PULSE message
 */
#define  PDD_OFF       0x000     /* turn pulse digit detect off */
#define  PDD_ON        0x001     /* turn pulse digit detect on */


/*
 * Set digit type in the DL_DIGITRDY (digit ready) message.
 */
#define DG_DTMF         0x0000   /* DTMF Digit */
#define DG_LPD          0x0001   /* Loop Pulse Digit  */
#define DG_APD          0x0002   /* Audio Pulse Digit */
#define DG_DPD          0x0002   /* Dial Pulse Digit */
#define DG_MF           0x0003   /* MF Digit */

#define DM_DTMF         (1 << DG_DTMF)
#define DM_LPD          (1 << DG_LPD)
#define DM_APD          (1 << DG_APD)
#define DM_MF           (1 << DG_MF)
#define DM_DPD          0x10      /* Enable DPD detection */
#define DM_DPDZ         0x20      /* Enable Zero-Trained DPD Detection */


/*
 * Record complete reason codes
 */
#define  RCR_STOP      0x002     /* record stopped */
#define  RCR_DTMF      0x003     /* record terminated by DTMF */

/*
 * Play complete reason codes
 */
#define  PCR_MAXBYTE   0x001     /* playback reached maximum bytes */
#define  PCR_STOP      0x002     /* playbacked stopped */
#define  PCR_DTMF      0x003     /* playback terminated by DTMF */

/*
 * Beep complete reason codes
 */
#define  BCR_MAXCNT    0x001     /* beep reached maximum repetions */
#define  BCR_STOP      0x002     /* beep stopped */
#define  BCR_DTMF      0x003     /* beep terminated by DTMF */

/*
 * Record type codes - this field is bitmapped
 */
#define  RT_SIMARK     0x001     /* mark silence start/stop events */
#define  RT_TTINIT     0x002     /* initiate record with DTMF */
#define  RT_TTTERM     0x004     /* terminate record with DTMF */
#define  RT_TDTONE     0x008     /* play tone before record initiation */
#define  RT_INITSET    0x110     /* touch tone initiation set */

/*
 * Playback type codes - this field is bitmapped
 */
#define  PT_TTINIT     0x002     /* initiate playback with DTMF */
#define  PT_TTTERM     0x004     /* terminate playback with DTMF */
#define  PT_TDTONE     0x008     /* play tone before playback initiation */
#define  PT_INITSET    0x110     /* touch tone initiation set */

/*
 * Beep type codes - this field is bitmapped
 */
#define  BT_TTINIT     0x002     /* initiate beep with DTMF */
#define  BT_TTTERM     0x004     /* terminate beep with DTMF */

/*
 * DTMF flags (see dl_dflags) - the edge on which to send DTMF ready message
 */
#define  DF_MKBK1      0x001     /* for channel 1 */
#define  DF_MKBK2      0x002     /* for channel 2 */
#define  DF_MKBK3      0x004     /* for channel 3 */
#define  DF_MKBK4      0x008     /* for channel 4 */

/*
 * Flag for operator intercept part of call progress
 */
#define  OPTEN         0x001     /* enable operator intercept w/connect */
#define  OPTDIS        0x002     /* disable operator intercept */
#define  OPTCON        0x003     /* enable operator intercept w/o connect */

#define  H_REDGE       0x001     /* rising edge detection of "hello" */
#define  H_FEDGE       0x002     /* falling edge detection of "hello" */

#define  ANSRDIS       0x00FFFF  /* disable answer deglitching */

#define  DLPRODA       00        /* an A series D/4x */
#define  DLPRODB       11        /* a B series D/4x */

/*
 * Success or failure indicators of update parameters function
 */
#define  UPD_OK        00        /* parameters updated successfully */
#define  UPD_FAIL      11        /* parameters not updated successfully */

/*
 * ROM type definition bit masked
 */
#define  ROM_CCITT     11        /* CCITT dial table used for DTMF dialing */

#ifdef VME_LIVE_INSERTION_B
 /* Equates for messages TO Network firmware via shared ram interface. NTH_ */
#define NTH_COMTST        0x10            /* Comtest to DTI/xxx */

 /* ndt_flags definitions */
#define NTH_DEVNUM 0x0002
#define NTH_TERM   0x00FF

 /* Equates for messages FROM Network firmware via shared ram interface. NTT_ */
#define NTT_COMRSP        0x10            /* Reply to host comtest */

 /* Network firmware message processing error codes. NTE_ */
#define NET_OK             0x00           /* Successful */

#endif /* VME_LIVE_INSERTION */

#endif /* __D40LOW_H__ */

