/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : anterror.h
 * Description                  : antares error header
 *
 *
 **********************************************************************/

#ifndef __ANTERROR_H__
#define __ANTERROR_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define E_SUCC 	0

#define E_D40NORES -20	 /* Voice driver is not resident */
#define E_NOANDRV  -21	 /* Antares driver is not resident */
#define E_NOSYS	   -22	 /* Error starting driver system */

/*
 * Antares Return codes
 */
#define  E_EXSUCC    0          /* Function successful             */
#define  E_EXNOFW    -23        /* No firmware on Example device   */
#define  E_EXSNACT   -24        /* Antares system not active       */
#define  E_EXSACT    -25        /* Antares system already active   */
#define  E_EXMTACT   -26        /* Multitasking function active    */
#define  E_EXBADCH   -27        /* Invalid Antares channel number  */
#define  E_EXBADDEV  -28        /* Invalid Antares device number   */
#define  E_EXBADPRM  -29        /* Invalid Antares parameter       */
#define  E_EXD4NACT  -30        /* D/4x system not active          */

/*
 *  UNIX special system error return codes 
 */
#define  E_STROPEN     -50  /* Error open the STREAM for current process */ 
#define  E_STRCLOSE    -51  /* Error close the STREAM for current process */ 
#define  E_SRLSTRREG   -52  /* Errro register the STREAM with the SRL library */
#define  E_SRLSTRDEREG -53  /* Errro unregister the STREAM with the SRL lib */
#define  E_GENBIND     -54  /* Error binding process with the generic driver */
#define  E_SRLREGDEV   -55  /* Error register rcu with SRL library */
#define  E_SRLDEREGDEV -56  /* Error unregister the rcu from SRL library */
#define  E_NOHANDLE    -57  /* Invalid SRL device descriptor */ 
#define  E_SENDCMD     -58  /* Error send command to board */
#define  E_SYSWAITEVT  -59  /* System error in wait for SRL event */
#define  E_TIMEOUTEVT  -60  /* Timeout wait for SRL event */
#define  E_WRONGEVT    -61  /* Wrong SRL event is arrived */
#define  E_CHGSRLMODE  -62  /* Error changing SRL to working in poll mode  */
#define  E_GETSRLMODE  -63  /* Error Getting SRL mode  */
#define  E_SRLPUTEVT   -64  /* Error put event to SRL queue */
#define  E_COPYDATA    -65  /* Error in copy data */
#define  E_ALLOCMEM    -66  /* Error in allocate memory */

/* F_START return error return codes */
#define  E_NOBOARD    -140	  /* Board not found in port address */
#define  E_BADDSPNUM  -141	  /* Illegal dsp numbers found in board */
#define  E_NOTSTART   -142	  /* Board state != AN_START */
#define  E_ALLDSPS    -143 	  /* All DSP's not working in board */
#ifdef VME_ANT
#define  E_SETA32     -144  /* Error set the board to work in A32 mode */
#define  E_GETMEMSIZE -145  /* Error get board global memory size */
#define  E_GETKVA     -146  /* Error get kernel virtual address for board */
#endif
#define  E_RCUSNUMBER   -147	  /* Number of available RCUs more  */
#define  E_NOLEGALBOARD -148	  /* No legal available board was found */
#define  E_BOARDNUMBER  -149	  /* RCU.board != board_number */

/* F_ANOPEN error return codes */
#define E_RCUNOTAVAIL -150      /* No more RCUs available */ 
#define	E_DPINOPEN    -151	/* Not registered with an_reg_dpi */
#define	E_RCUNOTFOUND -152	/* Specific RCU not found   */ 
#define E_DOUBLEDRCU  -153	/* Two diffrent RCUs with same details */
#define E_ATTRFLAG    -154   /* RCU with requested attributes not found */
#define E_CAPFLAG     -155   /* RCU with requested capacity not found */
#define E_RCUFLAGS    -156   /* RCU with requested flags not found (AN_ANY) */
#define E_RCUFIELDS   -157   /* RCU with requested fields not found (AN_ANY) */

/* F_ANCLOSE error return codes */
#define  E_CLOSESYSERROR -161	/* RCU is not open (internal system error) */

/* F_ANREGDPI error return codes */
#define  E_FULLDPI  -165    /* Dpi array is full */

/* F_ANSENDMSG error return codes */
#define  E_ILLEGALRCU -170	  /* Illegal RCU handler */
#define  E_NOSIZE     -171	  /* Not enough size in dsp memory */
#define  E_DSPNORESPONSE -172     /* No response from dsp */
#define  E_DSPNOTOKEN -173	  /* Request token failed */
#define  E_IMMEDIATEACK -174	  /* Request immediate acknowledge */
#define  E_NODELIVER -175	  /* Could not deliver data */
#define  E_USERBREAK -176	  /* Stopped by user (through hook function) */
#define  E_MSGTRUNC  -178 	  /* Recieved message truncated by user */
#define  E_ERRORFOUND -179	  /* Error found in DSP work */
#define  E_DSPSIZEERROR -182 /* Not enough place allocated by DSP for message */
#define	E_DSPNOCONFIRM -183       /* No confirmation from DSP */
#define  E_MAX_HOOK    -184       /* Infinite loop of hook_functions */

/* open + close + cmd _raw option */
#define  E_MAXRAW   -185	  /* Max opened board handlers exceeds */
#define  E_ILLEGALRAW  -186	  /* Illegal board handler request */
#define  E_CLOSEDRAW   -187	  /* Board handler already closed */
#define  E_COMMILLEGAL -188	  /* Illegal command */

/* add board errors */
#define  E_BOARDNOTLOADED -192  /* Board exist,addressed but not loaded */
#define  E_BOARDNOTEXIST -193	/* Board not exist / or error */

/* Receive ISR errors */
#define	E_RCV_TOO_LONG	  -201	/* DSP message longer than message buffer */
#define  E_LOCALQUEUE	  -203	/* Error in local queue */
#define  E_LOCALQUEUEFULL -209	/* Local queue is full */
#define  E_REQSQUEUE	-204	/* Error sending request to request queue */

#define	 E_CLASSNUM  -205    /* Illegal RCU.rclass in message header */
#define  E_BOARDNUM  -206    /* Illegal RCU.board in message header */
#define  E_DSPNUM    -207    /* Illegal RCU.DSP message header */
#define  E_UNITNUM   -208    /* Illegal RCU.unit in message header */


/* Bulk Data Protocol errors */
#define  E_BDREMEMBER  	 -300	/* DSP not ready for BD protocol */
#define  E_FULLBD        -301   /* Bulk data channel array is full */  
#define  E_ILLEGALBD	 -302	/* Illegal bulk data request (send_file) */
#define  E_CLOSEDBD      -303	/* Bulk data stream is closed */
#define  E_BDDIRECTION   -304 	/* Bulk data direction request is wrong */
#define  E_EOD		 -307	/* BDCB EOD Ack set */
#define	 E_BDNOFREE      -308  /* Bulk data channel not free for open request */
#define  E_BDOPENED	 -309	/* Bulk data channel already opened */
#define  E_ILLEGALOPEN	 -310	/* Stream opened by dsp and direction requested wrong */
#define  E_BDNOTOPEN     -311 	/* Bd_close error (try to close closed bd) */
#define  E_BDILLEGAL	 -312	/* Illeagal bulk data stream request */
#define  E_BDHOSTNOTOPEN -314	/* Hostopen = 0, after open procedure */
#define  E_BDFLUSHBUF    -316   /* Error flushing file */
#define	 E_BDWRITE       -317   /* Error in bdwrite protocol */
#define  E_BDREADFILE    -318   /* Error reading file from disk */
#define  E_BDWRITEFILE   -319   /* Error writing file to   disk */
#define  E_OVERRUN  	 -320   /* Overrun occured through record operation */
#define  E_MAINQUEUE     -321	/* Error sending messages to main queue */
#define  E_BDISACTIVE	 -322	/* Try to close bulk data channel while process is active */
#define  E_BADSTATEMEMORY -323  /* Bad state_memory accepted */
#define  E_UNDERRUN 	  -324  /* Underrun occured through play operation */
#define  E_BDEMSERROR     -325	/* Error during EMS operation */

/* Xplayf errors */
#define  E_INVALIDHANDLER -331	/* Invalid file handler */
#define  E_ERRORSEEK      -332	/* Error seeking position in file */
#define  E_INVALID_START  -333	/* Invalid start point */
#define  E_INVALID_OFFSET -334 	/* Invalid offset amount */
#define  E_EMPTYLIST      -335	/* Empty input list */
#define  E_XPLAY_ILLEGAL  -336	/* Illegal xplay option */ 
#define  E_XPLAY_LINK     -337	/* Null list pointer at XPLAY_LINK option */


/* AN_ROUTE errors */
#define  E_ILLEGALTSLOT  -401	/* Illegal time slot number */
#define  E_ILLEGALMODE	 -402	/* Illegal mode of operation */
#define  E_ROUTELIST     -403	/* Endless route_list or list too long */
#define  E_MAPSLOTLIST   -404	/* Endless mapslot_list or list too long */
#define  E_DSPERROR      -405	/* An_route() or An_mapslot() DSP errors */

#ifdef RTDL_ANT
/* AN_START errors */
#define E_RCUDATABASE	-501	/* error in rcus internal database */
#define E_OPENED_RCUS	-502	/* opened rcus in downloaded board (hot-download) */
#define E_HDCLOSERCU    -503    /* un-sucsessfull internal call to an_close() */
#define E_HDCLOSEBD	-504 /* un-sucsessfull internal call to an_bd_close() */
#endif

#define  E_BDLOCALERROR  0xFE /* Local process error  */
#define  E_GLOBALERROR   0xFF /* Error - communication with DSP but
		                 driver behaves correctly; Means system probably
			         may continue */

/**********************************************************************
 * AN_RECV_MSG() errors; because of function return code their values *
 *				 should be > 0 	                      *
 **********************************************************************/
#define  E_FATALDSPERROR 0xEE /* DSP error found in interrupt message handler */
#define	E_MSGDPINOPEN    0xE1 /* Not registered through an_reg_dpi() function */
#define  E_EMPTYMESSAGE  0xE2 /* Empty message accepted from DSP */
#define	E_BADECODE       0xE3  /* Bad event code found in queue */
#define  E_NOQUEUE	 0xE4	 /* No queue result */
#define  E_BDMSG_TRUNCATE 0xE5 /* Bulk data message truncated */
#define  E_MSGMISMATCH 	 0xE6  /* Message in local queue is different 
				  from its description in global queue */
#define  E_NOANTARES     0xE7	 /* Non ANTARES event */
#define  E_FATALERROR  0xEF /* Fatal error in driver queues (Antares queue,
			     D40DRV queue or scheduler queue). System should be
			     checked and reorganized */                

/***********************/
/* DSP internal errors */
/***********************/
#define FATAL 0x0000
#define ERHOST (FATAL + 0x300)
#define EMISC (FATAL + 0x800)

 /* no receive timeslots allocated */
#define ERR_HOSTIO_NO_RXSLOTS ERHOST+0xD

/* no tx timeslots allocated */
#define ERR_HOSTIO_NO_TXSLOTS ERHOST+0xE

/* excess timeslots listed */
#define ERR_HOSTIO_MAXRXLIST ERHOST+0xF

/* excess tx timeslots listed */
#define ERR_HOSTIO_MAXTXLIST ERHOST+0x10

/* excess rx_timeslot number */
#define ERR_HOSTIO_MAXRXSLOT ERHOST+0x13

/* Illegal rx_timeslot number */
#define ERR_HOSTIO_RXSLOT 	  ERHOST+0x1B

/* Illegal tx_timeslot number */
#define ERR_HOSTIO_TXSLOT 	  ERHOST+0x1C

/* Illegal SCBUS transmit password */
#define ERR_HOSTIO_PASSWORD  ERHOST+0x1D

/* excess tx_timeslot number */
#define ERR_HOSTIO_MAXTXSLOT ERHOST+0x14

/* excess mapping of timeslots -see MAP_MAXSLOTS in hostlink.h */
#define ERR_HOSTIO_MAXMAPSLOTS ERHOST+0x15

/* Error in routine callback function */
#define ERR_HOSTIO_TS_CALLBACK  ERHOST+0x16

/* Unsupported function */
#define ERR_HOSTIO_TS_ASSIGNED  ERHOST+0x17

/* Illegal mapslot timeslot value (value = 0) */ 
#define ERR_HOSTIO_BADMAPSLOT  ERHOST+0xC

/* unsupported function + prototype */
#define ERR_HOSTIO_UNSUPPORTED ERHOST+0xFF

/* RCU not registered */
#define ERR_HOSTIO_NORCU ERHOST+8

/* resource management request type does not exist */
#define ERR_HOSTIO_RMTYPE ERHOST+0xB

/* Error allocating query response */
#define ERR_QUERY_REAPONSE ERHOST + 0x30

/* Error in time slot orientation */
#define ERR_TS_ORIENTATION ERHOST + 0x40

/* PCM configuration must be SCSA */
#define ERR_NOT_IN_SCSA	ERHOST + 0x50

/* PCM configuration must be PEB */
#define ERR_NOT_IN_PEB	ERHOST + 0x60

/* Timeslot request failed */
#define ERR_TS_BUSY  	EMISC+6

/* Mapslot request failed  SC2000 busy */
#define ERR_SC2000_BUSY	EMISC+7

/* Mapslot direction error */
#define ERR_MAPSLOT_DIRECTION EMISC+8

/* EEPROM password failed */
#define	ERR_EEPROM_1	EMISC+0xC

#define	ERR_EEPROM_2	EMISC+0xD

/* Key module password failed */
#define	ERR_KEY_PASSWORD_1	 EMISC+0xE1 
#define	ERR_KEY_PASSWORD_2	 EMISC+0xE2
#define	ERR_KEY_PASSWORD_12	 EMISC+0xE3
#define	ERR_KEY_PASSWORD_3	 EMISC+0xE4
#define	ERR_KEY_PASSWORD_13	 EMISC+0xE5
#define	ERR_KEY_PASSWORD_23	 EMISC+0xE6
#define	ERR_KEY_PASSWORD_123	 EMISC+0xE7
#define	ERR_KEY_PASSWORD_S	 EMISC+0xE8
#define	ERR_KEY_PASSWORD_1S	 EMISC+0xE9
#define	ERR_KEY_PASSWORD_2S	 EMISC+0xEA
#define	ERR_KEY_PASSWORD_12S	 EMISC+0xEB
#define	ERR_KEY_PASSWORD_3S	 EMISC+0xEC
#define	ERR_KEY_PASSWORD_13S	 EMISC+0xED
#define	ERR_KEY_PASSWORD_23S	 EMISC+0xEE
#define	ERR_KEY_PASSWORD_123S EMISC+0xEF

/* Function not available for DSP */
#define	ERR_DSP_ACCESS			EMISC+0xF

/* Timeslot not available */
#define	ERR_TS_NOT_AVAIL		EMISC+0x10

/* Function not available */
#define	ERR_NOT_AVAIL			EMISC+0xFF 

/* terminate the extern "C" for c plus plus */
#if defined(__cplusplus)
}
#endif

#endif /* __ANTARES_H__ */
