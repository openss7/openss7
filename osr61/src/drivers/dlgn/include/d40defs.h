/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/***********************************************************************
 *        FILE: d40defs.h
 * DESCRIPTION: 
 *
 **********************************************************************/

#ifndef __D40DEFS_H__
#define __D40DEFS_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * Ensure all structures are packed.
 */
#if defined(sparc)
#else
#pragma pack(1)
#endif



#define  MAXBRD                  16       /* Maximum number of boards */

#ifndef VME_SPAN

#define  SH_RAM_SIZE             0x2000   /* Size of the D41 shared RAM */
#define  MAXCHAN                 64       /* Maximum number of channels */

#else /* VME_SPAN */

#define  SH_RAM_SIZE             0x10000  /* Size of the D41 shared RAM */
#define  MAXCHAN                 300      /* Maximum number of channels */
#define  PC_REQZERO_INTMASK      0x1      /* Bit 0 */
#define  DL_REQ_INTMASK          0x2      /* Bit 1 */

#endif /* VME_SPAN */

#define  DL_NORM                 0x1F     /* Normal board signature */



/*
 * D4x command transmission packet definition
 */
typedef struct dx_cmnd {
   unsigned char  pc_cmd;              /* command to send to D4x */
#ifdef BIG_ENDIAN
   unsigned char  pc_data2;            /* 2nd associated datum */
   unsigned short pc_data;             /* 1st associated datum */
#else
   unsigned short pc_data;             /* 1st associated datum */
   unsigned char  pc_data2;            /* 2nd associated datum */
#endif
   unsigned short pc_data3;            /* 3rd associated datum */
   unsigned char  pc_sub;              /* sub-command to send to D4x */
   unsigned char  pc_data4;            /* 4th associated datum */
} DX_CMND;

/*
 * D4x reply packet definition
 */
typedef struct dx_reply {
   unsigned char  dl_cmd;              /* reply returned by D4x       */
   unsigned char  dl_lstat;            /* raw line status             */
   unsigned short dl_data;             /* 1st associated datum        */
   unsigned short dl_bcnt;             /* buffer count field          */
   union {
       unsigned short du_data2;        /* 2nd associated datum        */
       struct {
       /*
       ** NOTE: Endian swapping is done at the tip of the funnel
       ** as a matter of speech.  That is, the endian_swap() function
       ** is called before the message is passed to or after the read
       ** from the kernel.  Endian swapping is not done at different 
       ** places for individual fields of the structure.  This way,
       ** the same code can be used.
       **
       ** In light of the scheme used to swap endians, redefining the 
       ** order of the elements is needed so that the same information 
       ** is conveyed.  For example, since du_data2 will always be 
       ** swapped, ds_sub and ds_data3 will not hold the same information
       ** if the order is not re-defined.  To re-order the elements,
       ** just do this in the needed C files:
       **
       **	#define ENDIAN_SWAPPED_AT_FUNNEL_TIP
       */
#ifdef ENDIAN_SWAPPED_AT_FUNNEL_TIP
           unsigned char  ds_data3;    /* 3rd associated datum        */
           unsigned char  ds_sub;      /* sub-reply returned by D4x   */
#else
           unsigned char  ds_sub;      /* sub-reply returned by D4x   */
           unsigned char  ds_data3;    /* 3rd associated datum        */
#endif /* ENDIAN_SWAPPED_AT_FUNNEL_TIP */
       } du_s;
   } dl_u;
} DX_REPLY;

/*
 * Macros for easy access to the union parts
 */
#define     dl_data2       dl_u.du_data2
#define     dl_sub         dl_u.du_s.ds_sub
#define     dl_data3       dl_u.du_s.ds_data3

/*
 * D4x shared memory format
 */
typedef struct d4xsram {
   unsigned char    dl_ram0[0xE0];
   unsigned char    dl_locator;      /* 0xE0 = D4XE locator */
   unsigned char    dl_ram1;         /* 0xE1 */
   unsigned char    dl_rearm;        /* 0xE2 = D4XE IRQ rearm */
   unsigned char    dl_ram2;         /* 0xE3 */
   unsigned char    dl_locator2;     /* 0xE4 = D4XE locator #2 */
   unsigned char    dl_ram3[0x0A];   /* 0xE5 */
   unsigned char    dl_reqcopy;      /* 0xEF */
   unsigned char    dl_msgrdy;       /* 0xF0 */
   unsigned char    pc_msgrdy;       /* 0xF1 */
   unsigned char    dl_fill0[2];     /* 0xF2 */
   unsigned short   dl_cherr;        /* 0xF4 */
   unsigned short   dl_chnum;        /* 0xF6 */
   unsigned char    dl_unlock1;      /* 0xF8 */
#ifdef VME_SPAN
   unsigned char    dl_fill1[1];     /* 0xF9 */
   unsigned char    pc_dltype;       /* 0xFA */
   unsigned char    pc_intreq;       /* 0xFB */
   unsigned char    pc_reqzero;      /* 0xFC */
   unsigned char    pc_rztype;       /* 0xFD */
#else
   unsigned char    dl_fill1[5];     /* 0xF9 */
#endif /* VME_SPAN */
   unsigned char    clr_reset;       /* 0xFE */
   unsigned char    set_reset;       /* 0xFF */
   unsigned char    dl_req;          /* 0x100 */
   unsigned char    pc_req;          /* 0x101 */
   unsigned char    dl_flash;        /* 0x102 */
   unsigned short   dl_ftime;        /* 0x103 */
   unsigned short   dl_ptime;        /* 0x105 */
   unsigned short   dl_drate;        /* 0x107 */
   unsigned char    dl_fill2[2];     /* 0x109 */
   unsigned short   dl_pmk;          /* 0x10B */
   unsigned char    dl_btype;        /* 0x10D */
   unsigned char    dl_fill3[4];     /* 0x10E */
   unsigned short   dl_bufsize;      /* 0x112 */
   unsigned char    dl_nchans;       /* 0x114 */
   unsigned short   dl_bfaddr[4];    /* 0x115 */
   unsigned char    dl_results;      /* 0x11D */
   unsigned char    dl_vers[4];      /* 0x11E */
   unsigned char    dl_fill4[0x10];  /* 0x122 */
   unsigned char    dl_rfu[16];      /* 0x132 */
   unsigned char    dl_fill5[0xa2];  /* 0x142 */
   unsigned char    dl_syscfg;       /* 0x1E4 */
   unsigned char    dl_product;      /* 0x1E5 */
   unsigned char    dl_mintion;      /* 0x1E6 */
   unsigned char    dl_mintioff;     /* 0x1E7 */
   unsigned char    dl_bdsign;       /* 0x1E8 */
   unsigned char    dl_fill6[1];     /* 0x1E9 */
   unsigned short   dl_devarea;      /* 0x1EA */
} D4XSRAM;

/*
 * End of initialized shared memory.
 * Needed because we must not clear SBA Configuration area until SBA FW is
 * changed to move it to private RAM.
 */
#define   ENDRAM      0x1E00

/*
 * The D4CHBUFF maps the D4X channel buffer layout. The count and
 * silence arrays are indexed by buffer no. (0 or 1), and for the
 * silence array, 0 for silence start, 1 for silence end.
 *
 * CAUTION: The pc_cmnd fields are only good for D4X/C. The D4X/B fields
 * take up the same amount of space (so you can use the struct for
 * other fields) but are laid out differently - they are not word
 * aligned, so we have to use explicit offsets and cast.
 */

#ifndef VME_SPAN
#define BF_BUFSIZ   512
#else
#define BF_BUFSIZ   7680	/* 7.5K bytes */
#endif /* VME_SPAN */

typedef struct chnbuff {
   DX_CMND         bf_cmnd;               /* cmnd to be sent to d4x channel */
   DX_REPLY        bf_reply;              /* reply from the d4x channel */
   unsigned short  bf_cnt[2];             /* number of bytes in buff 0/1 */
   unsigned short  bf_silence[2][2];      /* buf0:sil on/off, buf1:sil on/off */
   unsigned short  bf_info[2];            /* # of bytes returned in buff 0/1 */
   unsigned char   bf_buff[2][BF_BUFSIZ]; /* buff 0/1 - 512 byte chan buffers */
} CHNBUFF;

#define DV_BUFSIZ   0x30

/*
 * ISDN Boards use an extended buffer of 272 bytes in the device area.
 */
#define DV_ISDNBUFSIZ 0x110
#define ISDN_CMD      0x05

typedef struct devbuff {
   DX_CMND          dv_cmnd;              /* device cmnd to be sent */
   DX_REPLY         dv_reply;             /* device reply from the channel */
   unsigned char    dv_cmdbf[DV_BUFSIZ];  /* outgoing command buffer */
   unsigned char    dv_repbf[DV_BUFSIZ];  /* response reply buffer */
   unsigned char    dv_isdncmdbf[DV_ISDNBUFSIZ];  /* outgoing command buffer */
   unsigned char    dv_isdnrepbf[DV_ISDNBUFSIZ];  /* response reply buffer */
} DEVBUFF;

#define DV_DCBBUFSIZ  0xC0
#define DCB_AT        0xB1
#define BBDSP1        0x01
#define BBDSP2        0x02
#define DBDSP1        BBDSP1
#define DCBBRD        0x0
#define HIDSP1        0x100
#define HIDSP2        0x200

typedef struct devbuff1 {
   DX_CMND          dv_cmnd;              /* device cmnd to be sent */
   DX_REPLY         dv_reply;             /* device reply from the channel */
   unsigned char    dv_cmdbf[DV_BUFSIZ];  /* outgoing command buffer */
   unsigned char    dv_repbf[DV_BUFSIZ];  /* response reply buffer */
   unsigned char    dv_rfu1[16];          /* Not used */
   unsigned char    dsp_info[8];          /* ATI BITS for 2 DSPs */
   unsigned char    dv_rfu2[120];         /* Not used */
   unsigned char    dv_dcbrepbf1[DV_DCBBUFSIZ];  /* response reply buffer1 */
   unsigned char    dv_dcbrepbf2[DV_DCBBUFSIZ];  /* response reply buffer2 */
} DEVBUFF1;


/*
 * Command Termination
 */

/*
 * DL_TBLCK
 *
 * Terminator block.
 */
typedef struct dl_tblck {
   unsigned short tr_cmd;               /* Terminator number */
   unsigned short tr_len;               /* Terminator value */
   unsigned short tr_type;              /* Type bits, control usage */
   unsigned short tr_rfu;               /* Currently unused */
} DL_TBLCK;

/*
 * Values for terminator ID's (tr_cmd).
 */
#define     TI_RECVD    1              /* Received N digits */
#define     TI_SIL      2              /* N 100ms. silence */
#define     TI_NOSIL    3              /* N 100ms. non-silence */
#define     TI_LCOFF    4              /* N 100ms. loop current off */
#define     TI_IDD      5              /* Max. inter-digit delay */
#define     TI_FTIME    6              /* Max funtion time */
#define     TI_DTERM    7              /* Terminate on digit mask */
#define     TI_PMOFF    8              /* Pattern match silence off */
#define     TI_PMON     9              /* Pattern match silence on */

#define     N_TERM      20        /* Number of termination types */

/*
 * Bitmasks for terminator value usage (tr_type).
 */

/* The following bit can be set for: TI_RECVD, SIL, NOSIL, LCOFF, DTERM */
#define  TC_SENSE       0x0001      /* Level triggered if set, edge if not */
#define  TC_LEVEL       TC_SENSE    /* Level triggering requested */
#define  TC_EDGE        0x0000      /* Edge triggering requested */

/* The following bits apply to: TI_RECVD, SIL, NOSIL, LCOFF */
#define  TC_CLREND      0x0002      /* Clear history at cmd termination */
#define  TC_CLRBEG      0x0004      /* Clear history at cmd beginning */
#define  TC_USE         0x0008      /* Use block as terminator if set; */
                                    /* Else only clear history */
/* The following bit applies to: TI_IDD */
#define  TC_FIRST       TC_CLREND   /* Start delay after 1st digit if set */

/* The following bit applies to: TI_SIL */
#define  TC_INI         0x0010      /* EDGE: Use LENINIT time for 1st sil. */

/* The following bit applies to: TI_SIL, NOSIL, LCOFF, IDD, FTIME, PMOFF/PMON */
#define  TC_10MSTIM     0x0020      /* Specified time is in 10ms. units */

/*
 * Termination reason masks (from dl_data2 of completion replies).
 */
#define  TM_NORMAL      0x0000         /* Normal termination */
#define  TM_RECVDTMF    0x0001         /* Received N DTMF digits */
#define  TM_CONTSIL     0x0002         /* Continuous silence */
#define  TM_CONTNONSIL  0x0004         /* Continuous non-silence */
#define  TM_LCDROP      0x0008         /* Loop current dropped */
#define  TM_IDD         0x0010         /* Inter digit delay exceeded */
#define  TM_TIMEOUT     0x0020         /* Time out - max function time*/
#define  TM_DIGTERM     0x0040         /* Recv'd DTMF digit terminator*/
#define  TM_PATMATCH    0x0080         /* Pattern match detected */
#define  TM_STOPPED     0x0100         /* Stopped by command */
#define  TM_CNT         0x0200         /* Count reached */
#define  TM_OSTOP       0x0400         /* Old user stopped message */
#define  TM_ODIG        0x0800         /* Old dtmf termination */

/*
 * Structure for parameter passing
 */
typedef struct dl_pblock {
   unsigned short pr_number;           /* Parameter number */
   unsigned short pr_value;            /* Value */
} DL_PBLOCK;

/*
 * Mask for clearing histories at function call time
 */
#define  CL_LC          0x0001         /* Loop current drop history */
#define  CL_SILON       0x0002         /* silence on */
#define  CL_SILOFF      0x0004         /* silence off */
#define  CL_DTMF        0x0008         /* DTMF */

/*
 * Record/play option bits (pc_data)
 */
#define  OP_COMPRESS    0x0001         /* Compress silence (record only) */
#define  OP_DIGINIT     0x0002         /* Initiate upon digit reception */
#define  OP_DIGTERM     0x0004         /* Terminate upon digit reception */
#define  OP_TONE        0x0008         /* Precede operation with tone */
#define  OP_SELINIT     0x0010         /* Init. upon selected dig. reception */
#define  OP_USERTONE    0x0020         /* Precede operation with USER tone */
#define  OP_ALAW        0x4000         /* Use ALAW PCM for play and record */

/*
 * Firmware Structures for Call Analysis
 */

/*
 * DL_FWCAP
 *
 * Call Analysis parameters
 */
typedef struct dl_fwcap {

#ifndef VME_SPAN

   unsigned char  ca_nbrdna;     /* # of rings before no answer. */
   unsigned short ca_stdely;     /* Delay after dialing before analysis. */
   unsigned short ca_cnosig;     /* Duration of no signal time out delay. */
   unsigned short ca_lcdly;      /* Delay after dial before lc drop connect*/
   unsigned short ca_lcdly1;     /* Delay after lc drop con. before msg. */
   unsigned char  ca_hedge;      /* Edge of answer to send connect message.*/
   unsigned short ca_cnosil;     /* Initial continuous noise timeout delay.*/
   unsigned char  ca_lo1tola;    /* % acceptable pos. dev of short low sig.*/
   unsigned char  ca_lo1tolb;    /* % acceptable neg. dev of short low sig.*/
   unsigned char  ca_lo2tola;    /* % acceptable pos. dev of long low sig. */
   unsigned char  ca_lo2tolb;    /* % acceptable neg. dev of long low sig. */
   unsigned char  ca_hi1tola;    /* % acceptable pos. dev of high signal. */
   unsigned char  ca_hi1tolb;    /* % acceptable neg. dev of high signal. */
   unsigned short ca_lo1bmax;    /* Maximum interval for shrt low for busy.*/
   unsigned short ca_lo2bmax;    /* Maximum interval for long low for busy.*/
   unsigned short ca_hi1bmax;    /* Maximum interval for 1st high for busy */
   unsigned char  ca_nsbusy;     /* Num. of highs after nbrdna busy check. */
   unsigned short ca_logltch;    /* Silence deglitch duration. */
   unsigned short ca_higltch;    /* Non-silence deglitch duration. */
   unsigned short ca_lo1rmax;    /* Max. short low  dur. of double ring. */
   unsigned short ca_lo2rmin;    /* Min. long low  dur. of double ring. */
   unsigned char  ca_intflg;     /* Operator intercept mode. */
   unsigned char  ca_intfltr;    /* Minimum signal to qualify freq. detect.*/
   unsigned short ca_frqmin;     /* Lower freq. detect limit */
   unsigned short ca_frqmax;     /* Upper freq. detect limit */
   unsigned short ca_devmax;     /* Upper freq. limit deviation */
   unsigned short ca_smpsize;    /* No.of samples used in freq. detection */
   unsigned short ca_hisiz;      /* Used to determine which lowmax to use. */
   unsigned short ca_alowmax;    /* Max. low before con. if high >hisize. */
   unsigned short ca_blowmax;    /* Max. low before con. if high <hisize. */
   unsigned char  ca_nbrbeg;     /* Number of rings before analysis begins.*/
   unsigned short ca_hi1ceil;    /* Maximum 2nd high dur. for a retrain. */
   unsigned short ca_lo1ceil;    /* Maximum 1st low dur. for a retrain. */
   unsigned short ca_lowerfrq;   /* Lower allowable frequency in hz. */
   unsigned short ca_upperfrq;   /* Upper allowable frequency in hz. */
   unsigned char  ca_timefrq;    /* Total duration of good signal required*/
   unsigned char  ca_rejctfrq;   /* Allowable % of bad signal. */
   unsigned short ca_maxansr;    /* Maximum duration of answer. */
   unsigned short ca_ansrdgl;    /* Silence deglitching value for answer. */
   unsigned char  ca_pvdmxper;   /* Percent of maximum sum */
   unsigned short ca_pvdszwnd;   /* Length of window in 10 msec */
   unsigned short ca_pvddly;     /* Delay time */ 
   unsigned short ca_mxtimefrq;  /* max time for 1st freq to remain in bounds */
   unsigned short ca_lower2frq;  /* lower bound for second frequency */
   unsigned short ca_upper2frq;  /* upper bound for second frequency */
   unsigned short ca_time2frq;   /* min time for 2nd freq to remains in bounds */
   unsigned short ca_mxtime2frq; /* max time for 2nd freq to remain in bounds */
   unsigned short ca_lower3frq;  /* lower bound for third frequency */
   unsigned short ca_upper3frq;  /* upper bound for third frequency */
   unsigned short ca_time3frq;   /* min time for 3rd freq to remains in bounds */
   unsigned short ca_mxtime3frq; /* max time for 3rd freq to remain in bounds */
   unsigned short ca_dtn_pres;   /* Length of a valid dial tone (default=1sec) */
   unsigned short ca_dtn_npres;  /* Max time to wait for dial tone (default=3sec)*/
   unsigned short ca_dtn_deboff; /* The dialtone off debouncer (default=100ms) */
   unsigned short ca_pamd_failtime; /* Wait for AMD/PVD after cadence break(default=4sec)*/
   unsigned short ca_pamd_minring;  /* min allowable ring duration (default=1.9sec)*/
   unsigned char ca_pamd_spdval; /* Set to 2 selects quick decision only (default=1) */
   unsigned char ca_pamd_qtemp;  /* The Qualification template to use for PAMD */
   unsigned short ca_noanswer;   /* time before no answer after first ring (default=30sec) */
   unsigned short ca_maxintering;   /* Max inter ring delay before connect (8 sec) */

#else

   unsigned short ca_stdely;     /* Delay after dialing before analysis. */
   unsigned short ca_cnosig;     /* Duration of no signal time out delay. */
   unsigned short ca_lcdly;      /* Delay after dial before lc drop connect*/
   unsigned short ca_lcdly1;     /* Delay after lc drop con. before msg. */
   unsigned short ca_cnosil;     /* Initial continuous noise timeout delay.*/
   unsigned short ca_lo1bmax;    /* Maximum interval for shrt low for busy.*/
   unsigned short ca_lo2bmax;    /* Maximum interval for long low for busy.*/
   unsigned short ca_hi1bmax;    /* Maximum interval for 1st high for busy */
   unsigned short ca_logltch;    /* Silence deglitch duration. */
   unsigned short ca_higltch;    /* Non-silence deglitch duration. */
   unsigned short ca_lo1rmax;    /* Max. short low  dur. of double ring. */
   unsigned short ca_lo2rmin;    /* Min. long low  dur. of double ring. */
   unsigned short ca_frqmin;     /* Lower freq. detect limit */
   unsigned short ca_frqmax;     /* Upper freq. detect limit */
   unsigned short ca_devmax;     /* Upper freq. limit deviation */
   unsigned short ca_smpsize;    /* No.of samples used in freq. detection */
   unsigned short ca_hisiz;      /* Used to determine which lowmax to use. */
   unsigned short ca_alowmax;    /* Max. low before con. if high >hisize. */
   unsigned short ca_blowmax;    /* Max. low before con. if high <hisize. */
   unsigned short ca_hi1ceil;    /* Maximum 2nd high dur. for a retrain. */
   unsigned short ca_lo1ceil;    /* Maximum 1st low dur. for a retrain. */
   unsigned short ca_lowerfrq;   /* Lower allowable frequency in hz. */
   unsigned short ca_upperfrq;   /* Upper allowable frequency in hz. */
   unsigned short ca_maxansr;    /* Maximum duration of answer. */
   unsigned short ca_ansrdgl;    /* Silence deglitching value for answer. */
   unsigned short ca_pvdszwnd;   /* Length of window in 10 msec */
   unsigned short ca_pvddly;     /* Delay time */ 
   unsigned short ca_mxtimefrq;  /* max time for 1st freq to remain in bounds */
   unsigned short ca_lower2frq;  /* lower bound for second frequency */
   unsigned short ca_upper2frq;  /* upper bound for second frequency */
   unsigned short ca_time2frq;   /* min time for 2nd freq to remains in bounds */
   unsigned short ca_mxtime2frq; /* max time for 2nd freq to remain in bounds */
   unsigned short ca_lower3frq;  /* lower bound for third frequency */
   unsigned short ca_upper3frq;  /* upper bound for third frequency */
   unsigned short ca_time3frq;   /* min time for 3rd freq to remains in bounds */
   unsigned short ca_mxtime3frq; /* max time for 3rd freq to remain in bounds */
   unsigned short ca_dtn_pres;   /* Length of a valid dial tone (default=1sec) */
   unsigned short ca_dtn_npres;  /* Max time to wait for dial tone (default=3sec)*/
   unsigned short ca_dtn_deboff; /* The dialtone off debouncer (default=100ms) */
   unsigned short ca_pamd_failtime; /* Wait for AMD/PVD after cadence 
break(default=4sec)*/
   unsigned short ca_pamd_minring;  /* min allowable ring duration (default=1.9sec)*/
   unsigned short ca_noanswer;   /* time before no answer after first ring (default=30sec) 
*/
   unsigned short ca_maxintering;   /* Max inter ring delay before connect (8 sec) */
   unsigned char  ca_nbrdna;     /* # of rings before no answer. */
   unsigned char  ca_hedge;      /* Edge of answer to send connect message.*/
   unsigned char  ca_lo1tola;    /* % acceptable pos. dev of short low sig.*/
   unsigned char  ca_lo1tolb;    /* % acceptable neg. dev of short low sig.*/
   unsigned char  ca_lo2tola;    /* % acceptable pos. dev of long low sig. */
   unsigned char  ca_lo2tolb;    /* % acceptable neg. dev of long low sig. */
   unsigned char  ca_hi1tola;    /* % acceptable pos. dev of high signal. */
   unsigned char  ca_hi1tolb;    /* % acceptable neg. dev of high signal. */
   unsigned char  ca_nsbusy;     /* Num. of highs after nbrdna busy check. */
   unsigned char  ca_intflg;     /* Operator intercept mode. */
   unsigned char  ca_intfltr;    /* Minimum signal to qualify freq. detect.*/
   unsigned char  ca_nbrbeg;     /* Number of rings before analysis begins.*/
   unsigned char  ca_timefrq;    /* Total duration of good signal required*/
   unsigned char  ca_rejctfrq;   /* Allowable % of bad signal. */
   unsigned char  ca_pvdmxper;   /* Percent of maximum sum */
   unsigned char ca_pamd_spdval; /* Set to 2 selects quick decision only (default=1) */
   unsigned char ca_pamd_qtemp;  /* The Qualification template to use for PAMD */
#endif /* VME_SPAN */

} DL_FWCAP;

/*
 * DL_FWCAR
 *
 * Call Analysis results structure.
 */
typedef struct dl_fwcar {
   unsigned char  cr_termtype;     /* Termination reason */
   unsigned char  cr_frqout;       /* % of freq. out of bounds */
   unsigned short cr_frqhz;        /* Freq. detect in Hz. */
   unsigned short cr_sizehi;       /* Duration of non-sil (10mS units) */
   unsigned short cr_shortlow;     /* Duration of shorter silence */
   unsigned short cr_longlow;      /* Duration of longer silence */
   unsigned short cr_ansrsize;     /* Duration of answer (10mS units) */
   unsigned char  cr_conntype;     /* Connection type */
   unsigned char  cr_dtnfail;      /* Character that failed tone detection */
   unsigned short cr_freqdur;      /* */
   unsigned short cr_frqherz2;     /* */
   unsigned short cr_freqdur2;     /* */
   unsigned short cr_frqherz3;     /* */
   unsigned short cr_freqdur3;     /* */
   unsigned short cr_stoptype;     /* */
   unsigned short cr_tn_id;        /* The tone ID that caused termtype */
   unsigned char  cr_rfu[6];       /* Reserved for future use */
} DL_FWCAR;

/*
 * D/4x Daughterboard Hardware Signature structure
 */
typedef struct hwsign {
   unsigned char     hs_id;      /* Daughterboard ID */
   unsigned char     hs_ver;     /* Daughterboard version number */
   unsigned short    hs_seg;     /* First memory segment number */
   unsigned char     hs_mem;     /* Memory size (8k byte units) */
   unsigned char     hs_rfu;     /* RFU */
   unsigned char     hs_cksm;    /* Checksum of preceding bytes */
   unsigned char     hs_rfu1;    /* RFU1 */
} HWSIGN;


/*
 * Remove structure packing
 */
#pragma pack()

#endif /* __D40DEFS_H__ */

