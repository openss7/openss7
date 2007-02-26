/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : antares.h
 * Description                  : antares driver header
 *
 *
 **********************************************************************/

#ifndef __ANTARES_H__
#define __ANTARES_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define DIALOGIC  1      /* To confirm file inclusion */
#define DRVRID    0x10	 /* driver id code */

#ifndef LINUX
typedef void VOID;             
#endif
#ifdef LFS
typedef void VOID;             
#endif
typedef char CHAR;
typedef unsigned char BYTE;
#ifdef LINUX
#undef ADDR
#endif
typedef unsigned char ADDR;
typedef          short int  SHORT;
typedef unsigned short int  WORD;
typedef unsigned short int  UINT16;
typedef          long  int  LONG;
typedef unsigned long  int  ULONG;
typedef unsigned long       UINT32;

/*
 * Antares Functions defines
 */
#define F_EXNACT     0  /* No active function (idle)       */
#define F_EXSTART    1  /* Start the Example system        */
#define F_ANOPEN     2	/* Opens resource calss unit       */
#define F_ANCLOSE    3	/* Closes resource class unit      */
#define F_ANREGDPI   4	/* registers dpi structure         */
#define F_ANSENDMSG  5	/* send msg host -> dsp            */
#define F_ANRECVMSG  6	/* send msg dsp -> host            */    
#define F_ANRAWOPEN  7	/* opens antares raw board device  */
#define F_ANRAWCLOSE 8	/* closes an open antares raw device */
#define F_ANRAWCMD   9	/* issues a debug command to the antares board */
#define F_EXSTOP     10 /* Stop the Example system         */
#define F_ANBDOPEN   11 /* opens antares bulk data transfer stream */
#define F_ANBDCLOSE  12 /* Closes antares bulk data transfer stream */
#define F_ANBDFSEND  13 /* Sends file through bulk data protocol */
#define F_ANBDFRECV  14 /* Sends file through bulk data protocol */
#define F_ANBDSTOP   15 /* Stops antares bulk data transfer protocol */
#define F_ANGETINFO  16 /* Gets system information from driver */
#define F_ANPUTQUEUE 17	/* Puts message into system queues */
#define F_ANROUTE    18	/* Assign time slots to an RCU */
#define F_ANROUTERXTS 19 /* Assign time slots to an RCU (different values) */
#define	F_ANROUTELIST 20 /* Assigns a list of time slots to an RCU */
#define F_ANMAPSLOT   21 /* Map external SCBUS time slots to internal */
#define F_ANXPLAYF    22 /* Send multiple files/buffers to bulk data stream */
#define F_ANXRECF     23 /* Gets multiple files/buffers from bulk data stream */
#define F_ANGETVER    24 /* Gets driver version             */
#define F_ANDONGLE    25 /* Get Dongle info */
#define F_ANEEPROM    26 /* Get EEPROM info */
#define F_ANGETXMITSLOT 27 /* Gets SCBUS timeslot connected t ospecific RCU. */
#define F_ANLISTEN   28  /* Assign time slot to an RCU. */
#define F_ANUNLISTEN 29  /* UnAssign time slot to an RCU. */
#define F_ANASSIGNXMITSLOT 30    /* Connect timeslot transmit to SCBUS. */
#define F_ANUNASSIGNXMITSLOT 31  /* DisConnect timeslot transmit from SCBUS. */
#define F_ANSETXMITSLOT 32  /* Connect timeslot transmit to External PEB. */
#define F_ANUNSETXMITSLOT 33/* DisConnect timeslot transmit from External PEB */
#define F_ANBDSETUIO 34  /* Bulk data stream user I/O functions */
#define F_ANBDGETCHAN 35  /* Get bulk data channel from driver */
#define F_ANSETUIO 36  /* User I/O functions for specific DPI identifier */ 

#define  EXMAXFCN     36          /* Maximum ANTARES function number */

/*
 * Equates for error (return) codes
 */
#define  E_SUCC      0         /* Function was successful          */

/*
 * EVT_BLK to used in an_wait_evt()
 *
 * Event Block Structure - if 8 bytes or less of event-specific data, it
 * is stored in ev_data and ev_datap is NULL. Otherwise ev_datap points to
 * malloc'd memory and ev_data is unused. In either case, ev_len specifies
 * the number of bytes of event-specific data present.
 */

typedef struct evt_blk {
   LONG   ev_dev;	  /* Device on which event occurred     */
   ULONG  ev_event;	  /* Event that occured                 */
   LONG   ev_len;	  /* # of bytes of event-specific data  */
   BYTE   ev_data[8];     /* Event-specific data                */
   VOID   *ev_datap;	  /* Event-specific data pointer        */
} EVT_BLK;

#define AN_ANYEVT 0xFFFFFFFF

/*************************/
/* RCU structure         */
/*************************/
typedef struct rcu
{
    BYTE  unit; 	/* unit number */
    BYTE  rclass; 	/* board id number */
    BYTE  dsp;  	/* Resource Class */
    BYTE  board; 	/* subdevice on board (DSP) */
} RCU;
#define RCUSIZE (sizeof(RCU))

/*************************/
/* AN_RAW_CMD structures */
/*************************/
typedef struct ant_data {
    ULONG value;
    ULONG *buf;
} ANT_DATA;

/* RAW commands structure */
typedef struct ant_db_cmd {	 
    ULONG type;
    ULONG address;
    ULONG count;
    ANT_DATA data;
} ANT_DB_CMD;
#define DBCMDSIZE (sizeof(ANT_DB_CMD))

/* AN_RAW_CMD available instructions */
#define ANGETADDRESS   0  /* Read content of antares address port */
#define ANSETADDRESS   1  /* Set content of antares address port */
#define ANREADCONTROL  2  /* Read content of antares control port */
#define ANWRITECONTROL 3  /* Write content of antares control port */
#define ANREAD16BITS   4  /* Read content of antares data port */
#define ANWRITE16BITS  5  /* write content of antares data port */
#define ANREADWORD     6  /* Read 32 bit content of antares data port */
#define ANWRITEWORD    7  /* Write 32 bit content of antares data port */
#define ANREADDATA     8  /* reads data to buffer */
#define ANWRITEDATA    9  /* write data from buffer */

#define MAXDSP	4	  /* Maximum DSPs number for ANTARES board */
#define MAXFILENAME 64	  /* Maximum length of COFF file name */

/*****************************/
/* AN_READ_DONGLE structures */
/*****************************/
typedef struct dongleinfo {
	LONG msgid;	 	/* for ANTARES driver internal use */
	LONG Pass1[2];	 	/* Password 1 */
	LONG Pass2[2];	 	/* Password 2 */
	LONG Pass3[2];	 	/* Password 3 */
	LONG Pass4[2];	 	/* Password 4 */
	LONG PartData1[12];	/* Data returned 1 */
	LONG PartData2[12];	/* Data returned 2 */
	LONG PartData3[12];	/* Data returned 3 */
	LONG ScretchData[16];   /* Data returned 4 */
} DONGLE_INFO;
#define DONGLEINFOSIZE (sizeof(DONGLE_INFO))

/*****************************/
/* AN_READ_EEPROM structures */
/*****************************/
typedef struct eeprominfo {
	LONG msgid;			  /* Kind of info needed */
	LONG EepromPass[2];
	LONG EepromBuffer[8];
} EEPROM_INFO;
#define EEPROMINFOSIZE (sizeof(EEPROM_INFO))

#define EEPROMVAR    0x30303030
#define EEPROMDATA   0x33333333 
#define DEVELOPERVAR 0x31313131  /* EEPROM request id code */
#define DIALOGICVAR  0x32323232

/******************************/
/* SCBUS functions structures */
/******************************/

/**************************/
/* AN_GET_INFO structures */
/**************************/
typedef struct config_info {
    SHORT maxrcu;        /* number of maximum rcu's */
    SHORT maxbd;         /* maximum of bulk data channels */
    SHORT MasterBoard;   /* Location of master board in boards array */
    SHORT hwintr;        /* Hardware interrupt level */
    SHORT nboards;       /* number of available antares boards */
    SHORT m_message;     /* maximum size of message */
    SHORT anq_buffer;    /* size of antares queue data buffer */
    SHORT anq_entrysize; /* size of antares queue single entry */
    SHORT anq_items;     /* maximum of items available */
    LONG  bd_iobuf;      /* size of bulk data buffer */
    LONG  bd_single;     /* size of half bulk data buffer */
    SHORT maxopenedrcu;  /* number of maximum opened (pseudu) rcu's */
    SHORT intrdwrt;      /* number of I/O operations while interrupt disabled */
    SHORT maxslots;      /* Maximum number of slots avaliable in list */
    SHORT dspcall;       /* number of calls to dsp if no answer */
    SHORT asktoken;      /* number of calls to get token if no answer */
    BYTE  autoinc;       /* Auto_Increament option (TRUE/FALSE) */
    WORD  woption;	
    BYTE *scraddr;   /* debugging screen address */
} CFG_INFO;
#define CFGINFOSIZE (sizeof(CFG_INFO))

/* boards data structure */
#ifdef _SCO_
typedef struct board_data {
    WORD  board_id;         /* board identification code */
    WORD  port;             /* board access port */
    BYTE ndsp;              /* number of dsp's available */
    BYTE nbdstream;         /* number of bulk data stream available */
    ULONG pbdcb;            /* address in board global memory of first BDCB */
    ULONG phdcb;            /* address in board global memory of first HDCB */
    ULONG pdhcb;            /* address in board global memory of first DHCB */
    BYTE ready;             /* Is DSP application is ready on board */
    SHORT error_code;       /* Error code if Error / E_SUCC. */
    ULONG DSPerror[MAXDSP]; /* Includes DSP error codes */
} BOARDDATA;
#else
typedef struct board_data {
    WORD  board_id;         /* board identification code */
    WORD  port;             /* board access port */
    ULONG pbdcb;            /* address in board global memory of first BDCB */
    ULONG phdcb;            /* address in board global memory of first HDCB */
    ULONG pdhcb;            /* address in board global memory of first DHCB */
    ULONG DSPerror[MAXDSP]; /* Includes DSP error codes */
    SHORT error_code;       /* Error code if Error / E_SUCC. */
#ifdef VME_ANT
    WORD slot_id;
    char *kva;
    ULONG brd_addr; 
    ULONG mem_size;
#endif
    BYTE ndsp;              /* number of dsp's available */
    BYTE nbdstream;         /* number of bulk data stream available */
    BYTE ready;	            /* Is DSP application is ready on board */
} BOARDDATA;
#endif /* _SCO_ */
#define BOARDDATASIZE (sizeof(BOARDDATA))

typedef struct system_info {
    WORD      int_level;    /* Software interrupt vector level of IDDS */
    BYTE      an_state;     /* Is device active or not */
    BYTE      EmsOption;    /* Is EMS option available */
    WORD      tmr55;
    BYTE      crcu;         /* number of opened rcu's */
    BYTE      availrcu;     /* number of rcu's available */
    SHORT     cdpi;         /* number of registerd dpi */
    SHORT     maxdpi;       /* Max of dpi request (function an_reg_dpi) */
    BYTE      cbdata;       /* number of bulk data available channels */
    BYTE      raw_ind;      /* number of raws opened */
    WORD      maxhookinfo;  /* number of maximum opened (pseudu) rcu's */
    BOARDDATA single_board;
    CHAR fwlfile[MAXDSP][MAXFILENAME]; /* COFF file name for each DSP loaded */
} SYS_INFO;
#define SYSINFOSIZE (sizeof(SYS_INFO))

/*************************/
/* Inner DSP information */
/*************************/
typedef struct dsp_info {
	BYTE LineConfig;     /* Number of multiplexed channels (T1 or E1) */
	BYTE PcmConfig;      /* SCSA/PEB bus mode (SCSA,MVIP,PEB, or PEBSW) */
	BYTE Encoding;	     /* Compending algorithm Mu_Law  or A_Law */
	BYTE Sync;	     /* SYNC/ASYNC */
	WORD  MaxScsa;	     /* SCSA time slots bendwidth */ 
	ULONG rxDSPPools[4]; /* DSP receive time slot allocation pools */
	ULONG txDSPPools[4]; /* DSP transmit time slot allocation pools */
} DSP_INFO;
#define DSPINFOSIZE (sizeof(DSP_INFO))

#define T1 0     /* LineConfig options */
#define E1 1

#define SCSA  0  /* PcmConfig options */
#define MVIP  1
#define PEB   2
#define PEBSW 3

#define MULAW 0  /* Encoding options */
#define ALAW  1  

#define SYNC  0  /* Sync options */
#define ASYNC 1

/************************************************************/
/* process identification structure (proccess id + dpi id ) */
/************************************************************/
typedef struct process {  
	ULONG proc_id;		 /* process identification code */
	ULONG dpi_id;		 /* DPI function code */
} PRC;
#define PRCSIZE (sizeof(PRC))

/**************************/
/* AN_RECV_MSG structure  */
/**************************/
typedef struct recv_msg {
	SHORT dev;   /* Target RCU device handler or bulk data device handler */
	VOID *pmessage; /* Message buffer */
	SHORT msgsize;	    /* Length of message accepted */
	SHORT msgtype;	    /* Type of message accepted */
	RCU src_rcu;	    /* RCU which sent the message */
	ULONG msgid;	    /* ID code of message accepted */
	PRC proc;           /* Process structure */
	SHORT rtcode;  /* Return code which contains end of recv_msg reason */
} RECV_MSG;

/**************************/
/* AN_SEND_MSG structures */
/**************************/
typedef struct send_msg {
	ULONG msgid;	  /* ID code of message accepted */
	SHORT msgsize;	  /* Length of message accepted */
	BYTE *pmessage;	  /* Message buffer */
} SEND_MSG;
#define SENDMSGSIZE (sizeof(SEND_MSG))

typedef struct sync_reply {
	SHORT smsgsize;	  /* Length of message accepted */
	BYTE *smessage;	  /* Message buffer */
} SYNC_REPLY;
#define SYNCREPLYSIZE (sizeof(SYNC_REPLY))

/**************************/
/* AN_REG_DPI structures  */
/**************************/
/* Hook functions prototypes decleration */
#if defined(__cplusplus) || defined(__STDC__)
typedef	SHORT (*SendStartHook)(SHORT,SEND_MSG *,SYNC_REPLY *,PRC *,SHORT *);
typedef	SHORT (*SendEndHook)(SHORT,SEND_MSG *,SYNC_REPLY *,PRC *,SHORT *);
typedef  SHORT (*RecvStartHook)(BYTE,RECV_MSG *);
typedef  SHORT (*RecvEndHook)(BYTE,RECV_MSG *);
#else
typedef	SHORT (*SendStartHook)();
typedef	SHORT (*SendEndHook)();
typedef SHORT (*RecvStartHook)();
typedef SHORT (*RecvEndHook)();
#endif

/*
  Return code for hook function:
	NOEVENT if event should not be processed 
	EVENT EXIST if o.k
	> 0 if error message  
*/

#define NOEVENT 0
#define EVENTEXIST -1

/* Who is responsible for end of an_recv_msg() */
#define	ENDHOOK_1 0
#define ENDHOOK_2 1
#define ENDDRIVER 2

typedef struct rdpi {
    ULONG dpi_id;                /* dpi identifier */
    WORD  old_ds;
    SendStartHook send_hook;     
    SendEndHook   endsend_hook;  /* Pointers for hook functions */
    RecvStartHook recv_hook; 
    RecvEndHook   endrecv_hook;
} RDPI;
#define RDPISIZE (sizeof(RDPI))  /* size of rdpi structure */

/*************************************/		  
/* AN_MAPLIST structures & constants */
/*************************************/
typedef struct map_slot {
	ULONG TX_slot;	  /* Internal transmit time slot number */
	ULONG RX_slot;	  /* Internal receive  time slot number */
	ULONG GL_slot;	  /* External time slot */
	ULONG direction;  /* direction of connection */
	ULONG operation;  /* Operation method */
	ULONG rfu;	  /* For later use */
} MAP_SLOT;
#define MAPSLOTSIZE (sizeof(MAP_SLOT)) 
#define MAPSLOT_SIZE 6

/* MAPSLOT direction (bit mask) */
#define MAP_RX_TX  1
#define MAP_RX_GRX 2
#define MAP_RX_GTX 3
#define MAP_TX_GTX 16
#define MAP_TX_GRX 32

/* Mapping for unused slots */
#define MAP_NOSLOT 0xFEFEFEFE

/* MAPSLOT operation (bit mask) */
#define MAP_ASSIGN 1
#define MAP_RELEASE 2
#define MAP_EOL 0xFFFFFFFF

/***********************************/		  
/* AN_ROUTE structures & constants */
/***********************************/
/* AN_ROUTELIST() structure */
typedef struct ts_info {
	LONG listlength;	/* Number of items to time slots list */
	LONG *slots;	/* Far pointer to time slots list */
} TS_INFO;
#define TSINFOSIZE (sizeof(TS_INFO))

#define TS_SIZE 2

#define MAXSLOTS 64
#define TS_DEASSIGN 0xFFFFFFFF
#define TS_REVERSE 0x10000

/*
 * Type definition for SCBus Timeslots information structure.
 */
#ifndef __SC_TSINFO__
#define __SC_TSINFO__
typedef struct sc_tsinfo {
   unsigned long  sc_numts;
   long          *sc_tsarrayp;
} SC_TSINFO;
#endif

/***********************************/
/* AN_XPLAYF & AN_XRECF structures */
/***********************************/
typedef struct p_file {
	BYTE option;
	int handler;	        /* File handler */
	CHAR *buffer;           /* I/O buffer */
	ULONG fstart;		/* Start point	to play */
	ULONG offset;  		/* End point to play   */
	WORD rfu[4];		/* For later use       */
	struct p_file *io_next;	/* Pointer to next DX_IOTT if IO_LINK is set */
	struct p_file *io_prev;	/* Pointer to prev DX_IOTT if IO_LINK is set */
} P_FILE;
#define PFILESIZE (sizeof(P_FILE))

/* Xplay/Xrecord file options */
#define XPLAY_F 0x1		  /* Xplay from/to file option */
#define XPLAY_B 0x2		  /* Xplay from/to board option */
#define XPLAY_CONT 0x00		  /* Continue to next structure option */
#define XPLAY_LINK 0x10		  /* Continue to linked structure option */
#define XPLAY_END  0x20		  /* End of xplay list */

#define XPLAY_ALL 0xFFFFFFFF	  /* Play file to end of file */


#define AN_ANY 255		/* any kind of data */

/*******************************/
/* bulk data  mode constants   */
/*******************************/

#define ANT_BDWRITE 0x0000  /* sned to board direction */
#define ANT_BDREAD 0x0001   /* get from board direction */
#define ANT_BDCLICK 0x0002  /* Click when error  */
#define ANT_BDSTOP  0x0004  /* Stops when error  */
#define ANT_BDASYNC 0x0008  /* Sending async message at end of process */
#define ANT_BD_ID  0x0010  /* Sending async message at end of process */
#define ANT_BDSWAP 0x0020  /* Sending BD message in each driver buffer swapping */

/*******************************/
/* Message Delivery Specifiers */
/*******************************/
#define NULL_MESSAGE 0
#define ANT_MSG 1	  /* Antares regular message */
#define ANT_UNS 2	  /* Antares unsolicited event message */
#define ANT_MSG_ERROR 3	  /* Antares message truncated */
#define ANT_UNS_ERROR 4	  /* Antares unsolicited event message - truncated */
#define ANT_ASYNC_CMPLT 5  /* Antares bulk_data_complete message */
#define ANT_ASYNC_ERROR	6  /* Antares bulk_data_complete error_message */
#define ANT_ASYNC_READEREOD 7  /* Antares bulk_data_complete by user */
#define ANT_DSP_ERROR  8    /* Antares DSP error */
#define ANT_ASYNC_BDSWAP 9 /* Antares bulk_data buffer swapping message type */

/*****************/
/* AN_OPEN FLAGS */
/*****************/
#define UNITSORT 0x8    /* RCU request according to UNIT */
#define DSPSORT  0      /* RCU request according to BOARD/DSP/UNIT */

#define REPSYNC  1  	  /* Reply from DSP expected immediately */
#define REPASYNC 0	  /* Reply from DSP is not expected immediately */

/*******************************/
/* Bulk Data complete messages */
/*******************************/
#define HOSTEOD 1
#define HOSTSTOP 2

#define SYSTEMDPI_ID 0xFFFF		/* system DPI identification code */
#define ERRORDPI_ID 0xFFFE		/* Error DPI identification code */
#define GETDPI_ID 0xFFFD		/* Get DPI identifier number */

/*******************************/
/* Board available positions   */
/*******************************/
#define BOARD_NOTREADY  0 /* Board not found */
#define BOARD_EXIST  1 	  /* Board exist but DSP application not initiated */
#define BOARD_ADDRESSED 2 /* Board exist but available RCU'S not found */
#define BOARD_LOADED    3 /* Board ready for work */

/*******************************/
/* Message Types               */
/*******************************/
#define STD_MESSAGE     1
#define UNS_EVENT       2	  /* Unsolicited event */
#define CONTROL_MESSAGE 3         /* Used for an_route, an_route_list */
#define BDMESSAGE       4	  /* for internal use */
#define DSPMESSAGE      8	  /* for internal use */
#define NO_RCU		0xFE	  /* for internal use */
#define NO_BOARD   0xFFFF  /* for internal use */

/*****************************************************/
/* Write in memory debug options - used in WRITE_SCR */   
/* and debug options of antares.cfg                  */
/*****************************************************/
#define DE_NONE	  0x0000	/* No debugging on screen */
#define DE_SCREEN 0x0001	/* Debugging on screen */

/***********************************/
/* System default initiation sizes */
/***********************************/
#define MAXBD   8 		 /* max bulk data channels  */
#define HWINTR  5 		 /* default hardware interrupt */

#define NBOARDS 16		 /* maximum available antares boars */
#define MAXERRORLENGTH 100  /* maximum error description length */

/*
 * User Defined I/O Functions for read()/write()/lseek()
 */
#if defined(__cplusplus) || defined(__STDC__)
typedef	int (*U_READ) (int,ADDR*,WORD);
typedef	int (*U_WRITE) (int,ADDR*,WORD);
typedef int (*U_SEEK) (int,LONG,WORD);
#else
typedef	int (*U_READ) ();
typedef	int (*U_WRITE) ();
typedef int (*U_SEEK) ();
#endif

/*
 * User Defined I/O Functions for read()/write()/lseek()
 */
typedef struct an_uio {
   U_READ u_read;       /* User Defined replacement for read() */
   U_WRITE u_write;      /* User Defined replacement for write() */
   U_SEEK u_seek;       /* User Defined replacement for lseek() */
} AN_UIO;
#define ANUIOSIZE (sizeof(AN_UIO))

/* terminate the extern "C" for c plus plus */
#if defined(__cplusplus)
}
#endif

#endif /* __ANTARES_H__ */
