/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sbdlib.h
 * Description                  : libsbd.a linkable library
 *
 *
 **********************************************************************/

#ifndef __SBDLIB_H__
#define __SBDLIB_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif

#include "sbdmsg.h"


/*
 *  SRL events - produced when libsbd.a is used asynchronously
 *
 *  NOTE:  Be sure to check SB_MESSAGE for completion result, after SBEVT_FWL.
 */
enum { 
	SBEVT_GENBOOT	= 0x5601, 	/* SRL event for Gen boot loaded    */
	SBEVT_FWL	= 0x5602 	/* SRL event for FWL load completed */
};


/*
 * Defines
 */

#define DV_NAMELEN      80

#define SB_DIAGBUFSZ	0x0050   /* Additional diagnostic results buffer size */

#define SBPC_ACKNAK     0x0000   /* Acknowledge/Not Acknowledge		      */
#define SDT_TESTALL     0x2001   /* Perform all tests (Diagnostics Firmware)  */


/*
 * Diagnostics Firmware Error return codes
 */
#define SDE_NOERR       0x0000   /* No error returned by firmware on download */
#define SDE_TSTFAIL     0x2001   /* The test has failed			      */
#define SDE_NIDLE       0x2002   /* No error returned by firmware on download */


/*
 * Ack/Nak Error return codes (dxxx.fwl)
 */
#define SBERR_NOERR      0   /* No error returned by firmware on download */
#define SBERR_CHKSUM     1   /* Bad Checksum on download		*/
#define SBERR_VERIFY     2   /* Verification of download failed		*/
#define SBERR_UNKNOWN    3   /* Message sent to the firmware is unknown */
#define SBERR_BADHEX     4   /* Bad hex record				*/
#define SBERR_BADSTRT    5   /* Bad start message(not at the end of module) */
#define SBERR_BADCRLF    6   /* Expecting cr/lf at the end of line	*/
#define SBERR_BADMSG     7   /* Expecting first message to be dnld	*/
#define SBERR_INTWRGMEM  8   /* DSP internal code wrong memory type	*/
#define SBERR_INTBADADR  9   /* DSP internal code bad address		*/
#define SBERR_FILLEVER  10   /* savednld < new address			*/
#define SBERR_ADDRTST   11   /* DRAM address test failed		*/
#define SBERR_WLKBIT1   12   /* DRAM walk bit 1 test failed		*/
#define SBERR_WLKBIT0   13   /* DRAM walk bit 0 test failed 		*/
#define SBERR_DSPADRTST 14   /* DSP SRAM address test failed 		*/
#define SBERR_DSPWLKB1  15   /* DSP SRAM walk bit 1 test failed 	*/
#define SBERR_DSPWLKB0  16   /* DSP SRAM walk bit 0 test failed 	*/
#define SBERR_DSPPEB    17   /* Missing PEB connection 			*/
#define SBERR_DSPSPEED  18   /* Unknown DSP speed 			*/
#define SBERR_DSPMISS   19   /* FWL is missing required DSP code 	*/
#define SBERR_DSPUNKERR 20   /* Unexpected DSP error 			*/
#define SBERR_BADFWL    21   /* Invalid FWL file header 		*/
#define	SBERR_EOF	22   /* Premature end of FWL input 		*/
#define	SBERR_OLDFWL	23   /* FWL file is old format 			*/
#define	SBERR_RECLEN	24   /* FWL record length too long (>255) 	*/
#define	SBERR_HEXTYPE	25   /* Invalid HEX module record type 		*/
#define	SBERR_MODHDR	26   /* Expected Module Header 			*/
#define	SBERR_BOOTCMD	27   /* Invalid BOOTCMD parameter 		*/
#define	SBERR_MODTYPE	31   /* Invalid Module Type 			*/
#define	SBERR_UNSPROD	32   /* Unsupported Product ID 			*/
#define	SBERR_NEWFWL	33   /* FWL header version is too new 		*/


/* Defines for cf_setecho field - parameters for genboot.bin */
#define FE_DEFAULT  0  /* use default value: 1 for SBD, 2 for D/4XD	*/
#define FE_DIGITAL  1  /* digital front-end, use error correction	*/
#define FE_ANALOG   2  /* analog front-end, use echo cancellation	*/


/*
 *   F U N C T I O N    P R O T O T Y P E S . . .
 */
#ifdef __cplusplus
extern "C" {
#endif


#if (defined(__STDC__) || defined(__cplusplus))

extern int sb_open(  char *devnamp, int oflags );
extern int sb_close( int BdDev );
extern int sb_dnldboot( int BdDev,  int bootBinFd,  int bootFwlFd,  int mode );
extern int sb_dnldfwl(  int BdDev, int fwlfd, SB_MESSAGE *sb_msgp, 
			unsigned short *sb_diagbufp, int mode );

#else

extern int sb_open();
extern int sb_close();
extern int sb_dnldboot();
extern int sb_dnldfwl();

#endif


#ifdef __cplusplus
}
#endif


/*+************************************************************************\
 ***			End of File:	sbdlib.h			***
\*-************************************************************************/
#endif
