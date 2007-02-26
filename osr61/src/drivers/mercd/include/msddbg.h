/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msddbg.h
 * Description                  : driver debug structure definitions
 *
 *
 **********************************************************************/

#ifndef _MSDDBG_
#define _MSDDBG_

// Mercury Driver Statistics Block (Driver Level)
typedef struct _MSD_STATISTICS_BLOCK {		// CD_STATS_BLOCK
	merc_uint_t TotalBlocksSent;
	merc_uint_t TotalBlocksReceived;
	merc_uint_t TotalBlocksDiscarded;
	merc_uint_t TotalMercuryMsgsSent;
	merc_uint_t TotalMercuryMsgsReceived;
} MSD_STATISTICS_BLOCK, *PMSD_STATISTICS_BLOCK;

// Mercury Driver Statistics Block (Driver Level)
typedef struct _MSD_DEBUG_BLOCK {
	merc_uint_t		MsdDebugAreaFlag;		// Specifies area
	merc_uint_t		MsdDebugPrintLevel;		// MsgLevel (Kaw) specifies level for prn
	merc_uint_t		MsdDebugTraceLevel;		// Specifies trace buff info level
} MSD_DEBUG_BLOCK, *PMSD_DEBUG_BLOCK;

// Defines for MdDebugArea
#define MSD_NO_DEBUG			0x00000000
#define MSD_STREAM_ENTIRE_AREA		0x00000001
#define MSD_STREAM_CONNECT_AREA		0x00000002
#define MSD_STREAM_CANTAKE_AREA		0x00000003
#define MSD_MESSAGE_ENTIRE_AREA		0x00000004
#define MSD_MESSAGE_BIGMSG_AREA		0x00000005

// Defines for MdDebugPrintLevel
#define MSD_NO_PRINT			-1
#define MSD_ALWAYS_PRINT		0x00000000
#define MSD_WARNING_PRINT		0x00004000
#define MSD_ERROR_PRINT			0x00008000
#define MSD_PANIC_PRINT			0x10000000
#define MSD_PRINT_LEVEL1		0x00000001
#define MSD_PRINT_LEVEL2		0x00000002
#define MSD_PRINT_LEVEL3		0x00000003

// Defines for MdDebugTareceLevel
#define MSD_NO_TRACE			0x00000000
#define MSD_TRACE_LEVEL1		0x00000001
#define MSD_TRACE_LEVEL2		0x00000002
#define MSD_TRACE_LEVEL3		0x00000003

#define NO_PARAMETERS			0
#define ONE_PARAMETER			1
#define TWO_PARAMETERS			2
#define THREE_PARAMETERS		3
#define FOUR_PARAMETERS			4
#define FIVE_PARAMETERS			5
#define SIX_PARAMETERS			6

#ifdef DEBUG_ENABLED
#define MSD_FUNCTION_TRACE      i_printmsg	
#define MSD_LEVEL1_TRACE 	                   //dbg_level1_trace
#define MSD_LEVEL2_TRACE                           //dbg_level2_trace 
#define MSD_LEVEL3_TRACE	                   //dbg_level3_trace

// Print defines
#define MSD_PRINT		                    //dbg_always_print
#define MSD_LEVEL1_DBGPRINT	                    //dbg_level1_print
#define MSD_LEVEL2_DBGPRINT     i_printmsg	
#define MSD_LEVEL3_DBGPRINT	                    //dbg_level3_print
#define MSD_WARN_DBGPRINT	                    //dbg_warn_print
#define MSD_ERR_DBGPRINT	                    //dbg_error_print
#define MSD_PANIC_DBGPRINT	                    //dbg_panic_print

#else
#ifdef LFS
#define MSD_FUNCTION_TRACE(fmt, args...)
#else
#define MSD_FUNCTION_TRACE 	
#endif
#define MSD_LEVEL1_TRACE 	
#define MSD_LEVEL2_TRACE
#define MSD_LEVEL3_TRACE

// Print defines
#ifdef LFS
#define MSD_PRINT(fmt, args...)
#define MSD_LEVEL1_DBGPRINT(fmt, args...)
#define MSD_LEVEL2_DBGPRINT(fmt, args...)
#define MSD_LEVEL3_DBGPRINT(fmt, args...)
#define MSD_WARN_DBGPRINT(fmt, args...)
#define MSD_ERR_DBGPRINT(fmt, args...)
#define MSD_PANIC_DBGPRINT(fmt, args...)
#else
#define MSD_PRINT		
#define MSD_LEVEL1_DBGPRINT
#define MSD_LEVEL2_DBGPRINT	
#define MSD_LEVEL3_DBGPRINT
#define MSD_WARN_DBGPRINT	
#define MSD_ERR_DBGPRINT	
#define MSD_PANIC_DBGPRINT		
#endif

#endif

#ifndef assert
#define assert(assert)                                   \
do {                                                     \
   if (!(assert)) {                                      \
       printk (KERN_EMERG                                \
         "Assertion failure in %s() at %s:%d: \"%s\"\n", \
         __FUNCTION__, __FILE__, __LINE__, # assert);    \
   }                                                     \
} while (0)
#endif

#define MSD_ASSERT(x)           assert(x)
#define ASSERT(x)               assert(x)

#endif // _MSDDBG_
