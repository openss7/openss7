/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : protocol.h
 * Description                  : protocol header
 *
 *
 **********************************************************************/

/*
 * Define the ANTARES memory map
 */
#define A4_LOCAL_ADDR      0x0         /* Start of Antares local memory     */
#define A4_LOCAL_TOP       0x7FFFFFL   /* Last legal local address         */
#define A4_PERIPH_ADDR     0x808000L   /* Start of C31 peripheral memory   */
#define A4_INTERNAL_ADDR   0x809800L   /* Start of C31 internal memory     */
#define A4_INTERNAL_TOP    0x809FFFL   /* Last legal on-chip address       */
#define A4_IO_ADDR         0xC00000L   /* Start of Antares global I/O        */
#ifdef VME_ANT
#define A4_IO_TOP          0xC001FFL   /* Last legal I/O address           */
#endif
#define A4_GLOBAL_ADDR     0xEC0000L   /* Start of Antares std. global mem   */
#define A4_GLOBAL_TOP      0xFFFFFFL   /* Last legal address in global mem */
#define A4_BOOT_ADDRESS    0xFFF000L   /* Address of C31 boot load block   */
#define A4_CONFIG_ADDR     0xFFFF00L   /* Antares configuration block        */
#define A4_GDATA_ADDR      0xFC0000L   /* Antares start of initialized data  */
#define A4_HMEM_ADDR       0xF00000L   /* Antares start of high 1/2 global mem*/
#define A4_LMEM_ADDR       0xE00000L   /* Antares start of low 1/2 global mem*/

#define A4_PAGE_SIZE       0x040000L   /* size of a Antares memory page */
#define A4_FILL_VAL        0x10770008L /* fill value for unused memory */

#define GIO_DSPCount          (A4_IO_ADDR + 0x0c0)	 /* number of dsp in board */
#define GIO_SerialNo          (A4_IO_ADDR + 0x160)	 /* Dsp Serial number      */
#define A4_CFGParm(elem)      (A4_CONFIG_ADDR + elem)

#ifdef VME_ANT
#define A4_LBASE_ADDR      0x1000000L  /* Base VME slot offset in 4MB DRAM */
#define A4_HBASE_ADDR      0x1400000L  /* Base VME slot offset in 8MB DRAM */
#endif

/* Offset of elements inside configuration driver */
#define LINECONFIG  1
#define BDSTREAM    6
#define STARTSTATE  7
#define ERRORMASK   13
#define ERRORCODE   14
#define MESSAGEMASK 18
#define BDBUFMASK   19
#define BDOPENMASK  21
#define BDCBADDR    23
#define HDCBADDR    24
#define DHCBADDR    25
#define RCUADDR	    27
#define PCMCONFIG   37
#define ENCODING    38
#define SYNCH       70
#define MAXSCSA	    89
#define RXPOOLS	    90
#define TXPOOLS	    94

/****************************************************************
 *	               					        *
 *	  Inner dsp structures                                  *           
 *                                                              *
 ****************************************************************/

/**********************************/
/* Default Board Id for Host RCUs */
/**********************************/
#define HOST_ID 0xff

/*************************/
/* Message Header Format */
/*************************/
typedef struct
{
    ULONG messageType;  /* delivery specifier (see above) */
    RCU source;         /* sender RCU identifier */
    RCU destination;    /* recipient RCU identifier */
    ULONG messageId;    /* application specific identifier */
    ULONG dataLength;   /* length in octets of message data */
    ULONG reserved[2];  /* Antares internal use */
} MHdr;

/* NIR */
#if 0
#define MHDRSIZE 7
#endif
#define MHDRSIZE (sizeof(MHdr)/sizeof(ULONG))


/***************************************/
/* Hardware Attribute descriptor block */
/***************************************/
typedef struct
{
    ULONG DSPCount;	 /* Number of DSPs on board */
    ULONG ToBeDefined;   /* Attributes to be defined */
} AttrB;

/********************************************/
/* DSP to Host Message Control Block (DHCB) */
/********************************************/
typedef struct
{
#ifdef BIG_ANT
    ULONG        :31;    /* align word */
    ULONG TF     :1;     /* Transfer Flag (buffer ready for transfer) */
#else
    ULONG TF     :1;     /* Transfer Flag (buffer ready for transfer) */
    ULONG        :31;    /* align word */
#endif
    ULONG pHeader;     /* pointer to message header buffer */
    ULONG pMessage;    /* pointer to message data buffer */
#ifdef BIG_ANT
    ULONG        :30;    /* align word */
    ULONG Syncres :1;
    ULONG ack    :1;     /* message is to be acknowledged (not used by host) */
#else
    ULONG ack    :1;     /* message is to be acknowledged (not used by host) */
    ULONG Syncres :1;
    ULONG        :30;    /* align word */
#endif
    ULONG dataLength;   /* length in octets of message data (set by DSP) */         
    ULONG Error;        /* Host Error code */
	 ULONG ResponseP;	  /* Buffer address for DSP replay */
	 ULONG RepLength;	  /* Length of DSP replay */
} DHCB;

#define DHCBSIZE 8

/********************************************/
/* Host to DSP Message Control Block (HDCB) */
/********************************************/
typedef struct
{
#ifdef BIG_ANT
    ULONG        :27;  /* align word */
    ULONG Syncres :1;
    ULONG EF     :1;   /* DSP Error Flag */
    ULONG NB     :1;   /* No buffer available flag */
    ULONG SF     :1;   /* Size Needed Flag (Host needs bigger buffer) */
    ULONG TF     :1;   /* Transfer Flag (buffer ready for transfer) */
#else
    ULONG TF     :1;   /* Transfer Flag (buffer ready for transfer) */
    ULONG SF     :1;   /* Size Needed Flag (Host needs bigger buffer) */
    ULONG NB     :1;   /* No buffer available flag */
    ULONG EF     :1;   /* DSP Error Flag */
    ULONG Syncres :1;
    ULONG        :27;  /* align word */
#endif
    ULONG Error;       /* DSP Error code */
    ULONG pHeader;     /* pointer to message header buffer */
    ULONG pMessage;    /* pointer to message data buffer */
    ULONG messageSize; /* size in octets of message data buffer */
    ULONG dataLength;  /* length in octets of message data (set by host) */            
    ULONG ResponseP;   /* Buffer address for DSP replay */
    ULONG RepLength;   /* Length of DSP replay */
} HDCB;

#define HDCBSIZE 8

/*******************************************/
/* Bulk Data Transfer Control Block (BDCB) */
/*******************************************/
typedef struct
{
#ifdef BIG_ANT
    ULONG            :31; /* align word */
    ULONG SL         :1;  /* BDCB is locked for modification */

    ULONG            :9;  /* align word */
    ULONG completion :16; /* Completion code */
    ULONG HostEOD    :2;  /* Host end of data reason */
    ULONG readerEOD  :1;  /* Reader generated EOD */
    ULONG EODAck     :1;  /* Previous EOD acknowledged */
    ULONG EOD        :1;  /* End of Data for transfer group */
    ULONG IPF        :1;  /* In process flag (transfer in progress) */
    ULONG TF         :1;  /* Transfer Flag (buffer ready for transfer) */

    ULONG            :25; /* align word */
    ULONG DSPNumber  :4;  /* DSP number of DSP RCU of open */
    ULONG direction  :1;  /* transfer direction 0=Host->DSP */
    ULONG hostOpen   :1;  /* Host side open flag */
    ULONG DSPOpen    :1;  /* DSP side open flag */
#else
    ULONG SL         :1;  /* BDCB is locked for modification */
    ULONG            :31; /* align word */

    ULONG TF         :1;  /* Transfer Flag (buffer ready for transfer) */
    ULONG IPF        :1;  /* In process flag (transfer in progress) */
    ULONG EOD        :1;  /* End of Data for transfer group */
    ULONG EODAck     :1;  /* Previous EOD acknowledged */
    ULONG readerEOD  :1;  /* Reader generated EOD */
    ULONG HostEOD    :2;  /* Host end of data reason */
    ULONG completion :16; /* Completion code */
    ULONG            :9;  /* align word */

    ULONG DSPOpen    :1;  /* DSP side open flag */
    ULONG hostOpen   :1;  /* Host side open flag */
    ULONG direction  :1;  /* transfer direction 0=Host->DSP */
    ULONG DSPNumber  :4;  /* DSP number of DSP RCU of open */
    ULONG            :25; /* align word */
#endif
    ULONG DSPId;      /* stream open on user specified Id (not used by host) */
    RCU hostId;       /* stream open on Host RCU Id (not used by DSP) */
    ULONG pCallback;  /* pointer to callback function (not used by host) */
    ULONG callbackId; /* callback function argument (not used by host) */
    ULONG pBuffer;    /* Antares global memory address of data buffer */
    ULONG bufferSize; /* size in octets of data buffer */
    ULONG dataLength; /* length in octets of actual data transferred */
} BDCB;

#define BDCBSIZE (sizeof(BDCB)/sizeof(ULONG))

/*************************/
/* Run State Definitions */
/*************************/
#define AN_NULL     0
#define AN_LOADED   1
#define AN_START    2
#define AN_STOP     3
#define AN_INIT     4
#define AN_IWAIT    5
#define AN_ISTART   6
#define AN_PCMINIT  7

#define AN_FATAL    0xFFFFFFFF

typedef struct rcustruc
{
	 RCU    grcu;		 /* Rcu in details */
	 UINT32 rxSlots;	 /* receive time slots number for RCU */
	 UINT32 txSlots;	 /* transmit time slots number for RCU */
	 UINT32 AttrA;
	 UINT32 AttrB;
	 UINT32 rfu[2];		 /* for later use */
} RCUSTRUC;
#define RCUsize 7


typedef struct rcinfo {
      UINT32 RCUCount;   /* number of Resource Class Units */
      UINT32 pRCU;       /* pointer to array of RCUs structure  */
 } RCInfo;


/* structure of read/write buffer in driver memory */
struct rwbuf {
	BYTE *buf;	/* pointer to buffer */
	BYTE eob;	/* is end of data occured during last read/write */
	ULONG size;	/* size of information in current buffer */
#ifdef AIX_ANT
        mblk_t *ump;
#endif
};

/* Bulk data basic structure */
typedef struct bd_channel {
	struct rwbuf rcu_buf[2];    /* read buffer structure */
	BYTE direction;	  /* data direction */
	BYTE click;	  /* click or not click if error in protocol */
	BYTE enderror;	  /* Reader Eod or not if error in protocol */
	BYTE async;       /* Send or not ASYNC_MESSAGE_CMPLT at successful */
			  /* end of protocol */              
        BYTE bufswap;
	BYTE bdstop;	  /* Let user stop protocol by command */
	BYTE state_memory;  /* May use interrupt or not */
	BYTE HostStop;		  /* Process stop reason */
	WORD Completion;	  /* BD completion code */

	BYTE onprocess;	  /* Is process running */
	BYTE bnumber;	  /* board number */
	BYTE snumber;	  /* stream number */
	BYTE fillbuf;	  /* remember fillbuf byte */
	BYTE overrun;	  /* Overrun had already notified - byte */
	BYTE rcu_readbuf;  /* IO flag: buffer to read/write to/from rcu */
	ULONG  offset;	  /* pointer to current read buffer */
	                  /* xplayf function variables */
	ULONG pfstart;	  /* current start point for current file */
	ULONG poffset;	  /* current offset for current file */
        ULONG datalen;      /* Counter for actual data recorded or play */
	P_FILE *pfile;	  /* pointer to list of Xplayf structures */ 
        AN_UIO bduio;
  	ADDR *BTransAddr;
} BD_CHANNEL;
#define BDSIZE (sizeof(BD_CHANNEL))  /* size in bytes of struct prcu */

#define BD_IOBUF 0x2000              /* default size of bulk data size */      

#define MAXBDPERBOARD 64	/* Max bulk data channels per board */
#define BDLIMIT (NBOARDS * MAXBDPERBOARD) /* Maximum bulk data channels in any system */

/* structure for physical single rcu */
struct prcu {
  SHORT  board;		  /* Board_id */
  UINT32 code;		   
  UINT32 ucode;		   
  SHORT opened; /* number of open on this rcu - initialized to 0 */
  SHORT next;	 /* parameter for indexing array */
  SHORT unext;	 /* parameter for indexing array */
  RCUSTRUC l_rcu;        /* RCU structure */
};
typedef struct prcu PRCU;
#define PRCUSIZE (16 + (4*RCUsize))

#define MAXRCU	 16	 /* default number of  RCUs available in system */

/* structure for pseudo single rcu (opened rcu) */
struct o_prcu	{
  SHORT index; /* index to matching physical rcu */
  BYTE  solo;  /* solicited or not */

  BYTE Class;
  BYTE Board;	/* send message parameters */
  BYTE Dsp;
  BYTE Unit;
  ADDR *RTransAddr;
};
typedef struct o_prcu O_PRCU;
#define OPRCUSIZE (sizeof(O_PRCU))	/* size in bytes of struct rcu */

#define MAXOPENEDRCU 12  /* default number of maximum opened (pseudu) RCUs */


#define MAXDPI 4     /* default number of different dpi structures available */     

#ifdef PCI_ANT
#define MAXOPENRAW 32  /* default maximum requests of an_raw_open */
#else
#define MAXOPENRAW 16  /* default maximum requests of an_raw_open */
#endif

#if 0
#define DSPCALL 100  /* BUSY_WAIT CONSTANT = number of checks */
#define ASKTOKEN 50
#else
#define DSPCALL 2  /* BUSY_WAIT CONSTANT = number of 55Msec doubles */
#define ASKTOKEN 2
#endif



#define INTRDWRT 200 /* Number of I/O operations in disable_interrupt_period */
#define AUTOINC  1	/* Auto_Increament option */


/* AN_ROUTE CONSTANTS */

#define BTOWORD(nbytes) ( ((nbytes & (WORD)0x0003) == 0) ? (nbytes >> 2) : ((nbytes >> 2) + (WORD)1)   )


/* bulk data  direction constants */

#define BDCLOSE 0xFF
#define BD_WAITEOD  0xFE 
#define BD_NOTREADY 0xFF /* not ready for bulk data, fill buffers not activated */


#define COMMAND_CLASS 0xF0	 /* RCU.class for route & mapslot commands */

/* Get info parameters for reading COFF file names from DSP memory */
#define COFFNAMEADDRESS 0xFFFC10
#define NULL32 "                               "

#define EMSPAGESIZE (16 * 1024)
#define EMSPSIZE (EMSPAGESIZE >> 1)

/****************************************************/
/* Synchronise functions constants                  */
/****************************************************/

#define SPECMESS 0xFA
#define PSEUDOVALUE 0xFF

#define DONGLE_REQUEST 36	 /* Dongle request message size */
#define DONGLE_ANSWER  208	 /* Dongle DSP answer length */
#define DONGLEVAR    0x34343434	/* Dongle request id code */
#define DONGLEID 0x1000

#define EEPROM_REQUEST 12	 /* Dongle request message size */
#define EEPROM_ANSWER  32	 /* Dongle DSP answer length */
#define EEPROMID 0x1001


#ifdef PCI_ANT
#define MAXSYNCANSWER 460	 /* Maximum send_message async answer */
#else
#define MAXSYNCANSWER 230	 /* Maximum send_message async answer */
#endif

#define RSTART	0x41414141
#define ROUTEID 0x1003

#define MSTART 0x4d4d4d4d
#define MAPID  0x1004

#define REND  0L

/****************************************************/
/*        PROTOCOL FUNCTIONNS PROTOTYPES            */   
/*  (used only by protocol functions)               */
/****************************************************/

VOID init_msg_address(VOID);
VOID init_bd_address(VOID);

SHORT send_message(SHORT,SHORT,RCU,SEND_MSG *,SYNC_REPLY *,ULONG,BYTE); 
SHORT ask_token(WORD);
SHORT get_message(BYTE,BYTE);
SHORT put_queue(WORD,WORD,WORD,WORD,ADDR *);
UINT32 raw_cmd(WORD,UINT32,UINT32,UINT32,UINT32,UINT32 *);


SHORT bd_open(BYTE,SHORT,SHORT *,SHORT);
SHORT bd_close(BYTE,BYTE);
#if defined(_SCO_) || defined(LINUX)
SHORT check_pfile(SHORT);
SHORT check_xrec(SHORT,P_FILE*);
#endif /* _SCO_ */
SHORT fillxplaybufs(WORD); 
VOID bd_protocol(BYTE,BYTE,UINT32); 

#if defined(_SCO_) || defined(LINUX)
/* I/O functions */
SHORT  ant_fileread(SHORT,int,ADDR *,WORD *);
SHORT  ant_filewrite(SHORT,int,ADDR *,WORD *);
SHORT  ant_fileseek(SHORT,int,WORD,LONG,LONG *);
#endif /* _SCO_ */

/* Queue functions */
SHORT send_bulkdata_msg(BYTE,SHORT,SHORT,SHORT,SHORT,SHORT,SHORT,ULONG);
SHORT send_error_msg(SHORT,SHORT,SHORT,SHORT,SHORT);
SHORT send_dsp_msg(BYTE,SHORT,WORD,WORD,WORD,ADDR *,SHORT,SHORT,RCU);

