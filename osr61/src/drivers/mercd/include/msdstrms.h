/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdstrms.h
 * Description                  : streams structure definitions
 *
 *
 **********************************************************************/

#ifndef _MSDSTRMS_
#define _MSDSTRMS_
sadasdsa
typedef struct _STREAM_Q_PARAMETERS {
	merc_uint_t		SendQueueLimit;
	merc_uint_t		SendQueueLow;
	merc_uint_t		CurrentSendCount;
} STREAM_Q_PARAMETERS, *PSTREAM_Q_PARAMETERS;

typedef struct _STREAM_STATISTICS{
	merc_uint_t		BlocksSent;
	merc_uint_t		BlocksReceived;
} STREAM_STATISTICS, *PSTREAM_STATISTICS;

typedef struct _MSD_STREAM_CONN_BLOCK_ {
	PMSD_STREAM_CONN_BLOCK	Next;
	merc_uint_t		StreamState;
	merc_uint_t		StreamFlags;
	PMSD_BIND_BLOCK		BindBlock;
	merc_uint_t		StreamId;
	PSTRM_MSG		SendStreamQueue;
	merc_uint_t		TransactionId;
	merc_uint_t		StreamCanTake;
	PMSD_ADAPTER_BLOCK	AdapterBlock;
	STREAM_Q_PARAMETERS	QParameters;
	STREAM_STATISTICS	StatsBlock;
	MSD_HANDLE		StreamHandle;
	merc_ushort_t		BlockSequence;
	merc_uint_t		StreamType;
	merc_uint_t		AccumulatedUserAck;
} MSD_STREAM_CONN_BLOCK, *PMSD_STREAM_CONN_BLOCK_;

/* Stream entry flags */
#define MSD_STREAM_FLAG_WRITE_ONLY		0x00000001
#define MSD_STREAM_FLAG_READ_ONLY		0x00000002
#define MSD_STREAM_FLAG_SEND_FLOW_OFF		0x00000004
#define MSD_STREAM_FLAG_RECEIVE_FLOW_OFF	0x00000008
#define MSD_STREAM_FLAG_GSTREAM			0x00000010


/* Stream entry states */
#define MSD_STREAM_STATE_NOT_OPENED		0x00000000
#define MSD_STREAM_STATE_OPEN_PEND		0x00000001
#define MSD_STREAM_STATE_CONNECTED		0x00000002
#define MSD_STREAM_STATE_BROKEN			0x00000004
#define MSD_STREAM_STATE_CLOSE_PEND		0x00000008
#define MSD_STREAM_STATE_INTERNAL_CLOSE_PEND	0x00000010
#define MSD_STREAM_STATE_CLOSED			MSD_STREAM_STATE_NOT_OPENED

typedef struct _MSD_STREAM_MAP_ENTRY {
	PMSD_STREAM_CONN_BLOCK		StreamBlock;
} MSD_STREAM_MAP_ENTRY, *PCD_STREAM_MAP_ENTRY;

/* Internal message types */

typedef struct _STRAM_CLOSE_INTERNAL {
	PMSD_STREAM_CONN_BLOCK	StreamBlock;
} STREAM_CLOSE_INTERNAL, *PSTREAM_CLOSE_INTERNAL;

#define MSD_STREAM_SEND_LIMIT		4*4032
#define MSD_STREAM_SEND_LOW		4032

#define ALLOW_COUNT		5
			/* used by MsdAdvanceGatherSendBlocks(). */
#define DUMMY_DATA_BLOCK_SIZE	16
			/* used when we alloc for a missing data block. */
#endif // _MSDSTRMS_
