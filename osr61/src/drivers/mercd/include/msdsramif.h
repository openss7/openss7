/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdsramif.h
 * Description                  : SRAM interface definitions
 *
 *
 **********************************************************************/

#ifndef _MERCIF_
#define _MERCIF_

/*
 *	The following equates are used for the SRAM configuration Error status
 */

#define GENERAL_ENTRIES 32

/* Host IF config ack struct */
/* This struct must be identical to the one in sramdefs.h
	as provided by fw group
*/
typedef struct _MERCURY_HOST_IF_CONFIG_ACK {
	merc_uint_t	protocolRev;
	merc_uint_t	checkSum;
	merc_uint_t	replyCode;
	merc_uint_t	pHostToFwMsgs;
	merc_uint_t	pFwToHostMsgs;
	merc_uint_t	pFreeNodes;
	merc_uint_t	blockCount;
	merc_uint_t	pHostDataXfer;
	merc_uint_t	pFwDataXfer;
	merc_uint_t	general[GENERAL_ENTRIES];
	merc_uchar_t	Pad[12];
} MERCURY_HOST_IF_CONFIG_ACK, *PMERCURY_HOST_IF_CONFIG_ACK;

/* Config Ack Reply Codes */
#define MERC_HOSTIF_CFG_OK	0
#define MERC_HOSTIF_CFG_BAD_REVISION	1
#define MERC_HOSTIF_CFG_BAD_ALIGNMENT	2

/* Host IF config struct */
/* This struct must be identical to the one in sramdefs.h
	as provided by fw group
*/
typedef struct _MERCURY_HOST_IF_CONFIG {
	merc_uint_t	protocolRev;
	merc_uint_t	checkSum;
	merc_uint_t	hostConfigSize;	/* the total size of the structure in bytes */
	merc_uint_t	maxHostMsgs;
	merc_uint_t	maxFwMsgs;
/*	merc_uint_t	blockDataSize; */
	merc_uint_t	dataAlignment;	/* power of 2 starts at 2**1 */
/*	merc_uint_t	pageSize; */
	merc_uint_t	maxHostDataXfer;
	merc_uint_t	maxFwDataXfer;
	merc_uint_t	hostConfigMask;
	merc_uint_t	hwIntInterval;
	merc_uint_t	dataBlockLimit;	/* Must be < 100 */
	merc_uint_t	extBlockLimit;	/* This + dataBlockLimit must be < 100 */
	merc_uint_t	boardNumber;
	merc_uint_t	NodeNumber;
	merc_uint_t	vmeIntLevel;
	merc_uint_t	vmeIntVector;
} MERCURY_HOST_IF_CONFIG, *PMERCURY_HOST_IF_CONFIG;

#define MERCURY_HOST_IF_BLK_SIZE 4032 
#define MERCURY_HOST_IF_HCS      2

/* Free list header struct for bulk data block control */
typedef struct _MERC_FREE_LIST_HDR{
	merc_uint_t	HeadNodeOffset;
	merc_uint_t	TailNodeOffset;
	merc_uint_t	FreeBlockCount;
	merc_uint_t	CpFreeCounter;
} MERC_FREE_LIST_HDR, *PMERC_FREE_LIST_HDR;

/* Data Node struct.  One per data block */
typedef struct _MERC_DATA_BLK_HDR {
	merc_uint_t	DataBlockOffset;
	merc_uint_t	NextNodeOffset;
	merc_uint_t	BufFlags;
	merc_uint_t	Encoding;
	merc_uint_t	DataCount;
	merc_uint_t	Sequence;
	merc_uchar_t	Pad[8];
} MERC_DATA_BLK_HDR, *PMERC_DATA_BLK_HDR;

/* DataCount flags */
/* Must follow sramdef.h */

#define HIF_STREAM_F_EON	0x00000000
#define HIF_STREAM_F_EOD	0x00000001
#define HIF_STREAM_F_EOT	0x00000002
#define HIF_STREAM_F_EOS	0x00000004

/* data circular buffer entry struct */
typedef struct _MERC_DATA_CIR_BUF_ENTRY {
	merc_uint_t	DataHeaderOffset;
	merc_uint_t	StreamId;
	merc_uchar_t	Pad[8];
} MERC_DATA_CIR_BUF_ENTRY, *PMERC_DATA_CIR_BUF_ENTRY;


/* circular buffer control struct */
typedef struct _MERC_CIR_BUFFER {
	merc_uint_t	Write;
	merc_uint_t	Read;
	merc_uint_t	DebugChecksum;
	merc_uchar_t	Pad[4];
} MERC_CIR_BUFFER, *PMERC_CIR_BUFFER;


#define HIF_MESSAGE_DRIVER	0x00000000
#define HIF_MESSAGE_RESERVED	0x40000000
#define HIF_MESSAGE_SYSTEM	0x80000000
#define HIF_MESSAGE_RESOURCE	0xC0000000


/* Message circular buffer entry struct 
	Same as mercury message 
*/
typedef struct _MERC_HOSTIF_MSG {
	merc_uint_t	FlagTransactionId;
	merc_uint_t	MessageType;
	merc_uint_t	Source;
	merc_uint_t	Destination;
	merc_uint_t	SourceDestComponent;
	merc_uint_t	BodySize;
} MERC_HOSTIF_MSG, *PMERC_HOSTIF_MSG;

#define MD_OLD_BODY_SIZE	24

/* Mercury message is same as host interface message */
typedef MERC_HOSTIF_MSG MERCURY_MESSAGE, *PMERCURY_MESSAGE;

#define MERCURY_BODY_START(x) ((PMERC_HOSTIF_MSG)x+1)
#define MERCURY_MESSAGE_SIZE	(sizeof(MERC_HOSTIF_MSG))


/* mercury flag encoding */
#define MERCURY_FLAG_ENCODING		0x80
#define MERCURY_FLAG_32_ALIGNMENT	0x00
#define MERCURY_FLAG_24_ALIGNMENT	0x40
#define MERCURY_FLAG_NULL_BUFFER	0x00 
#define MERCURY_FLAG_PARAMETER		0x10	
#define MERCURY_FLAG_LOCAL_BUF		0x20
#define MERCURY_FLAG_PRIORITY_LOW	0x00
#define MERCURY_FLAG_PRIORITY_HIGH	0x08


/* Mercury message Macros */
/* They allow the caller to fetch and set individual fields in
a mercury message.  If field position changes, just modify
the corresponding macros. 
*/


#define MERCURY_FLAG_MASK	0x000000FF
#define MERCURY_FLAG_BIT_POS   0
#define MERCURY_FLAG_MAX	0x000000ff
#define MERCURY_GET_MESSAGE_FLAG(m) \
  ((merc_uchar_t)(((m)->FlagTransactionId& MERCURY_FLAG_MASK) >> MERCURY_FLAG_BIT_POS))

#define MERCURY_SET_MESSAGE_FLAG(m,flag) { \
	(m)->FlagTransactionId&= ~MERCURY_FLAG_MASK; \
	(m)->FlagTransactionId|= (((merc_uint_t)(flag) & MERCURY_FLAG_MAX)<<MERCURY_FLAG_BIT_POS); \
	}

#define MERCURY_TRANSACTIONID_MAX	0x00FFFFFF
#define MERCURY_TRANSID_MASK	0xFFFFFF00
#define MERCURY_TRANSID_BIT_POS   8
#define MERCURY_GET_TRANSACTION_ID(m) \
	((m)->FlagTransactionId& MERCURY_TRANSID_MASK) >> MERCURY_TRANSID_BIT_POS

#define MERCURY_SET_TRANSACTION_ID(m,id)  {\
		(m)->FlagTransactionId &= ~MERCURY_TRANSID_MASK;\
		(m)->FlagTransactionId |=  (((merc_uint_t)id&MERCURY_TRANSACTIONID_MAX) <<MERCURY_TRANSID_BIT_POS); \
		}


#define MERCURY_MESSAGE_CLASS_MAX	0x00000003
#define MERCURY_CLASS_MASK	0x00C00000
#define MERCURY_CLASS_BIT_POS   22
#define MERCURY_GET_MESSAGE_CLASS(m) \
	((m)->MessageType & MERCURY_CLASS_MASK) >> MERCURY_CLASS_BIT_POS

 #ifdef MERCD_PCI
     #define MERCURY_MSGTYPE_DATA_MASK	0x00400000
     #define MERCURY_GET_MESSAGE_DATA_TYPE(m) \
     	((m)->MessageType & MERCURY_MSGTYPE_DATA_MASK) >> MERCURY_CLASS_BIT_POS
 #endif /* MERCD_PCI */


#define MERCURY_SET_MESSAGE_CLASS(m,class) { \
	(m)->MessageType &= ~MERCURY_CLASS_MASK; \
	(m)->MessageType |= ((class&MERCURY_MESSAGE_CLASS_MAX)<<MERCURY_CLASS_BIT_POS); \
	}

#define MERCURY_MESSAGE_TYPE_MAX	0x003FFFFF
#define MERCURY_TYPE_MASK	0x003FFFFF
#define MERCURY_TYPE_BIT_POS   0
#define MERCURY_GET_MESSAGE_TYPE(m) \
	((m)->MessageType & MERCURY_TYPE_MASK) >> MERCURY_TYPE_BIT_POS

#define MERCURY_SET_MESSAGE_TYPE(m,type) { \
	(m)->MessageType &= ~MERCURY_TYPE_MASK; \
	(m)->MessageType |= ((type&MERCURY_MESSAGE_TYPE_MAX) << MERCURY_TYPE_BIT_POS); \
	}

#ifdef _8_BIT_INSTANCE
#define MERCURY_SOURCE_NODE_MAX		0x0000FFFF
#define MERCURY_SOURCE_NODE_MASK	0x0000FFFF
#define MERCURY_SOURCE_NODE_BIT_POS   0
#define MERCURY_GET_SOURCE_NODE(m) \
	((m)->Source& MERCURY_SOURCE_NODE_MASK) >> MERCURY_SOURCE_NODE_BIT_POS

#define MERCURY_SET_SOURCE_NODE(m,id)  {\
		(m)->Source &= ~MERCURY_SOURCE_NODE_MASK;\
		(m)->Source |=  (((merc_uint_t)id&MERCURY_SOURCE_NODE_MAX) <<MERCURY_SOURCE_NODE_BIT_POS); \
		}
#else
#define MERCURY_SOURCE_NODE_MAX         0x000000FF
#define MERCURY_SOURCE_NODE_MASK        0x000000FF
#define MERCURY_SOURCE_NODE_BIT_POS   0
#define MERCURY_GET_SOURCE_NODE(m) \
        ((m)->SourceDestComponent & MERCURY_SOURCE_NODE_MASK) >> MERCURY_SOURCE_NODE_BIT_POS
#define MERCURY_SET_SOURCE_NODE(m,id)  {\
   (m)->SourceDestComponent &= ~MERCURY_SOURCE_NODE_MASK;\
   (m)->SourceDestComponent |=  (((merc_uint_t)id&MERCURY_SOURCE_NODE_MAX) <<MERCURY_SOURCE_NODE_BIT_POS); \
   }
#endif

#define MERCURY_SOURCE_BOARD_MAX		0x000000FF
#define MERCURY_SOURCE_BOARD_MASK	0x00FF0000
#define MERCURY_SOURCE_BOARD_BIT_POS   16
#define MERCURY_GET_SOURCE_BOARD(m) \
	((m)->Source& MERCURY_SOURCE_BOARD_MASK) >> MERCURY_SOURCE_BOARD_BIT_POS

#define MERCURY_SET_SOURCE_BOARD(m,id)  {\
		(m)->Source &= ~MERCURY_SOURCE_BOARD_MASK;\
		(m)->Source |=  (((merc_uint_t)id&MERCURY_SOURCE_BOARD_MAX) <<MERCURY_SOURCE_BOARD_BIT_POS); \
		}

#define MERCURY_SOURCE_PROC_MAX		0x000000FF
#define MERCURY_SOURCE_PROC_MASK	0xFF000000
#define MERCURY_SOURCE_PROC_BIT_POS   24
#define MERCURY_GET_SOURCE_PROC(m) \
	(m->Source& MERCURY_SOURCE_PROC_MASK) >> MERCURY_SOURCE_PROC_BIT_POS

#define MERCURY_SET_SOURCE_PROC(m,value) { \
	m->Source&= ~MERCURY_SOURCE_PROC_MASK; \
	m->Source|= (((merc_uint_t)value & MERCURY_SOURCE_PROC_MAX)<<MERCURY_SOURCE_PROC_BIT_POS); \
	}

#ifdef _8_BIT_INSTANCE
#define MERCURY_SOURCE_INST_MAX		0x000000FF
#define MERCURY_SOURCE_INST_MASK	0x000000FF
#define MERCURY_SOURCE_INST_BIT_POS   0
#define MERCURY_GET_SOURCE_INST(m) \
	(m->SourceDestComponent& MERCURY_SOURCE_INST_MASK) >> MERCURY_SOURCE_INST_BIT_POS

#define MERCURY_SET_SOURCE_INST(m,value) { \
	m->SourceDestComponent&= ~MERCURY_SOURCE_INST_MASK; \
	m->SourceDestComponent|= (((merc_uint_t)value & MERCURY_SOURCE_INST_MAX)<<MERCURY_SOURCE_INST_BIT_POS); \
	}
#else
#define MERCURY_SOURCE_INST_MAX         0x0000FFFF
#define MERCURY_SOURCE_INST_MASK        0x0000FFFF
#define MERCURY_SOURCE_INST_BIT_POS   0
#define MERCURY_GET_SOURCE_INST(m) \
        (m->Source & MERCURY_SOURCE_INST_MASK) >> MERCURY_SOURCE_INST_BIT_POS

#define MERCURY_SET_SOURCE_INST(m,value) { \
        m->Source&= ~MERCURY_SOURCE_INST_MASK; \
        m->Source|= (((merc_uint_t)value & MERCURY_SOURCE_INST_MAX)<<MERCURY_SOURCE_INST_BIT_POS); \
        }
#endif


#define MERCURY_SOURCE_COMP_MAX		0x000000FF
#define MERCURY_SOURCE_COMP_MASK	0x0000FF00
#define MERCURY_SOURCE_COMP_BIT_POS   8
#define MERCURY_GET_SOURCE_COMP(m) \
	(m->SourceDestComponent& MERCURY_SOURCE_COMP_MASK) >> MERCURY_SOURCE_COMP_BIT_POS

#define MERCURY_SET_SOURCE_COMP(m,value) { \
	m->SourceDestComponent&= ~MERCURY_SOURCE_COMP_MASK; \
	m->SourceDestComponent|= (((merc_uint_t)value & MERCURY_SOURCE_COMP_MAX)<<MERCURY_SOURCE_COMP_BIT_POS); \
	}

#ifdef _8_BIT_INSTANCE
#define MERCURY_SOURCE_COMPINST_MAX	0x0000FFFF
#define MERCURY_SOURCE_COMPINST_MASK	0x0000FFFF
#define MERCURY_SOURCE_COMPINST_BIT_POS   0
#define MERCURY_GET_SOURCE_COMPINST(m) \
	((m)->SourceDestComponent& MERCURY_SOURCE_COMPINST_MASK) >> MERCURY_SOURCE_COMPINST_BIT_POS

#define MERCURY_SET_SOURCE_COMPINST(m,value) { \
	(m)->SourceDestComponent&= ~MERCURY_SOURCE_COMPINST_MASK; \
	(m)->SourceDestComponent|= (((merc_uint_t)value & MERCURY_SOURCE_COMPINST_MAX)<<MERCURY_SOURCE_COMPINST_BIT_POS); \
	}

#define MERCURY_DEST_NODE_MAX		0x0000FFFF
#define MERCURY_DEST_NODE_MASK	0x0000FFFF
#define MERCURY_DEST_NODE_BIT_POS  0
#define MERCURY_GET_DEST_NODE(m) \
	((m)->Destination& MERCURY_DEST_NODE_MASK) >> MERCURY_DEST_NODE_BIT_POS

#define MERCURY_SET_DEST_NODE(m,id)  {\
		(m)->Destination &= ~MERCURY_DEST_NODE_MASK;\
		(m)->Destination |=  (((merc_uint_t)id&MERCURY_DEST_NODE_MAX) <<MERCURY_DEST_NODE_BIT_POS); \
		}
#else
#define MERCURY_SOURCE_COMPINST_BIT_POS   8
#define MERCURY_SOURCE_COMPINST_MASK    0x00FF0000   /* For componenet - 8 bit */
#define MERCURY_SOURCE_COMPINST_MASK_8BITMODE    0x0000FFFF   /* For componenet - 8 bit */

#define MERCURY_GET_SOURCE_COMPINST(m) \
        (((m)->SourceDestComponent& MERCURY_SOURCE_COMP_MASK) << MERCURY_SOURCE_COMPINST_BIT_POS) | \
        (((m)->Source &  MERCURY_SOURCE_INST_MASK))

#define MERCURY_GET_SOURCE_COMPINST_8BITMODE(m) \
        ((m)->SourceDestComponent& MERCURY_SOURCE_COMPINST_MASK_8BITMODE)

#define MERCURY_SET_SOURCE_COMPINST(m,value) { \
        (m)->SourceDestComponent&= ~MERCURY_SOURCE_COMP_MASK; \
        (m)->SourceDestComponent |= (((merc_uint_t)value & MERCURY_SOURCE_COMPINST_MASK) >> MERCURY_SOURCE_COMPINST_BIT_POS); \
        (m)->Source &= ~MERCURY_SOURCE_INST_MASK; \
        (m)->Source |= ((merc_uint_t)value & MERCURY_SOURCE_INST_MASK); \
        }

#define MERCURY_DEST_NODE_MAX           0x000000FF
#define MERCURY_DEST_NODE_MASK  0x00FF0000
#define MERCURY_DEST_NODE_BIT_POS  16
#define MERCURY_GET_DEST_NODE(m) \
        ((m)->SourceDestComponent & MERCURY_DEST_NODE_MASK) >> MERCURY_DEST_NODE_BIT_POS

#define MERCURY_SET_DEST_NODE(m,id)  {\
                (m)->SourceDestComponent  &= ~MERCURY_DEST_NODE_MASK;\
                (m)->SourceDestComponent  |=  (((merc_uint_t)id&MERCURY_DEST_NODE_MAX) <<MERCURY_DEST_NODE_BIT_POS); \
                }
#endif

/*
 * We need to convert BindBlocks to qCompDescs.  This is done by Cheetah when a component is
 * registered by qCompUse.  See mercdrv.c BIND2COMP macro.
 * When unregistering a component via exit notification, the qCompDesc used *must* be exactly the
 * same as the qCompDesc used to register via qCompUse originally.
 * Hence, we need an equivalent macro to BIND2COMP.
 *
 * The SourceDestComponent contains the "Component" filed of the qCompDescriptor
 * The component field is 8 bits, located in the least significatnt bits of
 * SourceDestComponent.
 *
 * Need to take upper 8 bits of BindBlock and map into the 8 bits of the component.
 * The least significant 8 bits of the BindBlock is mapped into the instance.
 *
 * In other words:
 *   component = BindHandle & 0xFF00 >> 8
 *   instance  = BindHandle & 0x00FF
 */
#define MERCURY_SOURCEDESTCOMPONENT_COMPONENT_BIT_POS 8
#define MERCURY_SOURCEDESTCOMPONENT_COMPONENT_MASK 0x0000FF00
#define MERCURY_SOURCE_BIND2COMP_INSTANCE_MASK 0xFF

#define MERCURY_SOURCE_BIND2COMP(__m__,__value__) { \
        (__m__)->SourceDestComponent&= ~MERCURY_SOURCE_COMP_MASK; /* clear the Component Bits */ \
        (__m__)->Source &= ~MERCURY_SOURCE_INST_MASK;             /* clear the Instance Bits  */ \
        (__m__)->SourceDestComponent |= (((merc_uint_t)__value__ & MERCURY_SOURCEDESTCOMPONENT_COMPONENT_MASK) \
               >> MERCURY_SOURCEDESTCOMPONENT_COMPONENT_BIT_POS); /* set the Component 8 bits */ \
        (__m__)->Source |= ((merc_uint_t)__value__ & MERCURY_SOURCE_BIND2COMP_INSTANCE_MASK); /* set the Instance  8 bits */ \
        }

#define MERCURY_DEST_BOARD_MAX		0x000000FF
#define MERCURY_DEST_BOARD_MASK	0x00FF0000
#define MERCURY_DEST_BOARD_BIT_POS   16
#define MERCURY_GET_DEST_BOARD(m) \
	((m)->Destination& MERCURY_DEST_BOARD_MASK) >> MERCURY_DEST_BOARD_BIT_POS

#define MERCURY_SET_DEST_BOARD(m,id)  {\
		(m)->Destination &= ~MERCURY_DEST_BOARD_MASK;\
		(m)->Destination |=  (((merc_uint_t)id&MERCURY_DEST_BOARD_MAX) <<MERCURY_DEST_BOARD_BIT_POS); \
		}


#define MERCURY_DEST_PROC_MAX	0x000000FF
#define MERCURY_DEST_PROC_MASK	0xFF000000
#define MERCURY_DEST_PROC_BIT_POS   24
#define MERCURY_GET_DEST_PROC(m) \
	(m->Destination& MERCURY_DEST_PROC_MASK) >> MERCURY_DEST_PROC_BIT_POS

#define MERCURY_SET_DEST_PROC(m,value) { \
	m->Destination&= ~MERCURY_DEST_PROC_MASK; \
	m->Destination|= (((merc_uint_t)value & MERCURY_DEST_PROC_MAX)<<MERCURY_DEST_PROC_BIT_POS); \
	}

#ifdef _8_BIT_INSTANCE
#define MERCURY_DEST_INST_MAX	0x000000FF
#define MERCURY_DEST_INST_MASK	0x00FF0000
#define MERCURY_DEST_INST_BIT_POS  16 
#define MERCURY_GET_DEST_INST(m) \
	(m->SourceDestComponent& MERCURY_DEST_INST_MASK) >> MERCURY_DEST_INST_BIT_POS

#define MERCURY_SET_DEST_INST(m,value) { \
	m->SourceDestComponent&= ~MERCURY_DEST_INST_MASK; \
	m->SourceDestComponent|= (((merc_uint_t)value & MERCURY_DEST_INST_MAX)<<MERCURY_DEST_INST_BIT_POS); \
	}
#else
#define MERCURY_DEST_INST_MAX   0x0000FFFF
#define MERCURY_DEST_INST_MASK  0x0000FFFF
#define MERCURY_DEST_INST_BIT_POS  0
#define MERCURY_GET_DEST_INST(m) \
        (m->Destination & MERCURY_DEST_INST_MASK) >> MERCURY_DEST_INST_BIT_POS

#define MERCURY_SET_DEST_INST(m,value) { \
        m->Destination &= ~MERCURY_DEST_INST_MASK; \
        m->Destination |= (((merc_uint_t)value & MERCURY_DEST_INST_MAX)<<MERCURY_DEST_INST_BIT_POS); \
        }

#endif

#define MERCURY_DEST_COMP_MAX	0x000000FF
#define MERCURY_DEST_COMP_MASK	0xFF000000
#define MERCURY_DEST_COMP_BIT_POS   24
#define MERCURY_GET_DEST_COMP(m) \
	(m->SourceDestComponent& MERCURY_DEST_COMP_MASK) >> MERCURY_DEST_COMP_BIT_POS

#define MERCURY_SET_DEST_COMP(m,value) { \
	m->SourceDestComponent&= ~MERCURY_DEST_COMP_MASK; \
	m->SourceDestComponent|= (((merc_uint_t)value & MERCURY_DEST_COMP_MAX)<<MERCURY_DEST_COMP_BIT_POS); \
	}

#ifdef _8_BIT_INSTANCE
#define MERCURY_DEST_COMPINST_MAX	0x0000FFFF
#define MERCURY_DEST_COMPINST_MASK	0xFFFF0000
#define MERCURY_DEST_COMPINST_BIT_POS   16
#define MERCURY_GET_DEST_COMPINST(m) \
	((m)->SourceDestComponent& MERCURY_DEST_COMPINST_MASK) >> MERCURY_DEST_COMPINST_BIT_POS

#define MERCURY_SET_DEST_COMPINST(m,value) { \
	(m)->SourceDestComponent&= ~MERCURY_DEST_COMPINST_MASK; \
	(m)->SourceDestComponent|= (((merc_uint_t)value & MERCURY_DEST_COMPINST_MAX)<<MERCURY_DEST_COMPINST_BIT_POS); \
	}

#else
#define MERCURY_DEST_COMPINST_BIT_POS   8
#define MERCURY_DEST_COMPINST_MASK      0x00FF0000 /* Only for COmponent - 8 bit */
#define MERCURY_DEST_COMPINST_MASK_8BITMODE      0xFFFF0000 /* Only for COmponent - 8 bit */
#define MERCURY_DEST_COMPINST_BIT_POS_8BITMODE   16

#define MERCURY_GET_DEST_COMPINST(m) \
        (((m)->SourceDestComponent& MERCURY_DEST_COMP_MASK) >> MERCURY_DEST_COMPINST_BIT_POS) | \
        (((m)->Destination & MERCURY_DEST_INST_MASK))

#define MERCURY_GET_DEST_COMPINST_8BITMODE(m) \
        ((m)->SourceDestComponent& MERCURY_DEST_COMPINST_MASK_8BITMODE) >> MERCURY_DEST_COMPINST_BIT_POS_8BITMODE

#define MERCURY_SET_DEST_COMPINST(m,value) { \
        (m)->SourceDestComponent&= ~MERCURY_DEST_COMP_MASK; \
        (m)->SourceDestComponent|= (((merc_uint_t)value & MERCURY_DEST_COMPINST_MASK) << MERCURY_DEST_COMPINST_BIT_POS); \
        (m)->Destination &= ~MERCURY_DEST_INST_MASK; \
        (m)->Destination |= ((merc_uint_t)value & MERCURY_DEST_INST_MASK); \
        }
#endif

#define MERCURY_MIN_BODY_SIZE	0x4
#define MERCURY_MAX_BODY_SIZE	0x800

#define MERCURY_BODY_SIZE_MAX	0x00FFFFFF
#define MERCURY_BODY_SIZE_MASK	0x00FFFFFF
#define MERCURY_BODY_SIZE_BIT_POS   0
#define MERCURY_GET_BODY_SIZE(m) \
	((m->BodySize & MERCURY_BODY_SIZE_MASK) >> MERCURY_BODY_SIZE_BIT_POS)

#ifdef LFS
#define MERCURY_SET_BODY_SIZE(m,buf) { \
	m->BodySize &= ~MERCURY_BODY_SIZE_MASK; \
	m->BodySize |= (((buf)&MERCURY_BODY_SIZE_MAX)<< MERCURY_BODY_SIZE_BIT_POS); \
	}
#else
#define MERCURY_SET_BODY_SIZE(m,buf) { \
	m->BodySize &= ~MERCURY_BODY_SIZE_MASK; \
	m->BodySize |= ((buf&MERCURY_BODY_SIZE_MAX)<< MERCURY_BODY_SIZE_BIT_POS); \
	}
#endif


/* Mercury Host Interface Driver class messages */

typedef struct _HIF_CAN_TAKE_ENTRY {
	merc_uint_t	StreamId;
	merc_uint_t	CanTake;
} HIF_CAN_TAKE_ENTRY, *PHIF_CAN_TAKE_ENTRY;

typedef struct _HIF_CAN_TAKE {
	merc_uint_t	NoCanTakes;
	HIF_CAN_TAKE_ENTRY	Entry;
} HIF_CAN_TAKE, *PHIF_CAN_TAKE;

typedef struct _HIF_OPEN_STREAM {
	merc_uint_t	StreamId;
	merc_uint_t	Mode;
	merc_uint_t	RequestSize;
	merc_uint_t	CanTakeLimit;
} HIF_OPEN_STREAM, *PHIF_OPEN_STREAM;

/* Mode flags */
#define OPEN_STR_READ_ONLY	0x00
#define OPEN_STR_WRITE_ONLY	0x01
#define OPEN_STR_DO_WAIT	0x02
#define OPEN_STR_FLUSH		0x04
#define OPEN_STR_PERSISTENT	0x08      

typedef struct _HIF_OPEN_STREAM_ACK {
	merc_uint_t	StreamId;
	merc_uint_t	ActualSize;
	merc_uint_t	InitialCanTake;
} HIF_OPEN_STREAM_ACK, *PHIF_OPEN_STREAM_ACK;

typedef struct _HIF_CLOSE_STREAM{
	merc_uint_t	StreamId;
} HIF_CLOSE_STREAM, *PHIF_CLOSE_STREAM;

typedef struct _HIF_SESS_CLOSED{
	merc_uint_t	StreamId;
} HIF_SESS_CLOSED, *PHIF_SESS_CLOSED;

typedef struct _HIF_SUCCESS{
	merc_uint_t	MessageNumber;
	merc_uint_t	StreamId;
} HIF_SUCCESS , *PHIF_SUCCESS;

typedef struct _HIF_FAILED{
	merc_uint_t	Type;
	merc_uint_t	ErrorCode;
	merc_uint_t	StreamId;
} HIF_FAILED, *PHIF_FAILED;

// The field names are preserved same as MERC_HOSTIF_MSG so that the
// macros work fine. Especially the second field which does not have
// valid Destinations fields.
typedef struct _HIF_PROC_EXIT_ENTRY {
   merc_uint_t Source;				// Processor(8) + Board(8) + Node(16)
   merc_uint_t SourceDestComponent;		// Lower 16 bits(Source)
} HIF_PROC_EXIT_ENTRY, *PHIF_PROC_EXIT_ENTRY;

// Exit notification message
typedef struct	_HIF_PROC_EXIT_NOTIFY {
	merc_uint_t	Count;
	HIF_PROC_EXIT_ENTRY	Entry;
} HIF_PROC_EXIT_NOTIFY, *PHIF_PROC_EXIT_NOTIFY;

// HCS Support - init message
typedef struct _HIF_INIT_HCS {
   merc_uint_t startStreamId;
   merc_uint_t numOfStreams;
   merc_uint_t streamSize;
} HIF_INIT_HCS, *PHIF_INIT_HCS;

#define MERCURY_CLASS_RESOURCE		0x0
#define MERCURY_CLASS_RESOURCE_INT	0x1	/* PTR1489 */
#define MERCURY_CLASS_SYSTEM		0x2
#define MERCURY_CLASS_DRIVER		0x3

#define MERCURY_HOST_IF_HCS_INIT_21554  0xB0
#define MERCURY_HOST_IF_HCS_INIT_ON_PLX 0x48

// MUST DEFINE QCNTRL_ functions to match FW 

#define	QCNTRL_INITIALIZE	1   // The Host sends this to the FW once 
#define	QCNTRL_INIT_ACK		2
#define	QCNTRL_CAN_TAKE		3  
#define	QCNTRL_OPEN_STREAM	4
#define	QCNTRL_OPEN_STR_ACK	5
#define	QCNTRL_CLOSE_STREAM	6
#define	QCNTRL_FAILED		7
#define	QCNTRL_SUCCEEDED	8
#define	QCNTRL_SESS_CLOSED	9
#define	QCNTRL_PROC_DEATH	10 
#define	QCNTRL_PROC_DTH_ACK 	11 
#define QCNTRL_TERMINATE 	12
#define QCNTRL_INIT_HCS         0x16
#define QCNTRL_SET_STREAM_PROPERTY   0x17
#define QCNTRL_RESET_STREAM_PROPERTY 0x18

/*
 * The following are the error codes that are returned with the 
 * QCNTRL_FAILED message.
 */
#define	QFAILED_EXTENDED	1	/* Ext. message pool not found */
#define	QFAILED_DESTINATION	2	/* Invalid Destination address specified */
#define	QFAILED_BAD_STREAM_ID	3
#define	QFAILED_ALREADY_CLOSED	4	/* Stream already closed */


/* address conversion macros */

/* must fix TO_KVADDRESS */
#define TO_KV_ADDRESS(padapter,x) \
	((size_t)padapter->phost_info->reg_block.HostRamStart + ((size_t) x))
	/* Was -> ((ULONG)padapter->phost_info->reg_block.HostRamStart +((ULONG) x)) */

#define TO_HIF_OFFSET(padapter,x) \
	((size_t)x - (size_t)padapter->phost_info->reg_block.HostRamStart) 
	/* Was -> ((ULONG)x - (ULONG)padapter->phost_info->reg_block.HostRamStart)  */

/*
 *	The following equates are used for the SRAM configuration Error status
 */
typedef enum {
	SRAMCFG_NO_ERROR=0,
	SRAMCFG_PROTOCOL_REV=1,	/* FW cannot support host protocol revision */
	SRAMCFG_BAD_ALIGN_VALUE=2 /* Bad data alignment specified by host */
} SramCfgCode;


/* must verify default dest_node,,, */
#define DEFAULT_DEST_NODE	0
#define DEFAULT_DEST_PROC	1
#define DEFAULT_DEST_BOARD	0
#define DEFAULT_DEST_COMPINST	0x0002

#define PROC_DEATH_DEST_PROC   		0x0001
#define PROC_DEATH_DEST_COMPINST	0x0005

#define DEFAULT_SOURCE_PROC	0

typedef struct _MERC_EXTENDED_BODY_HDR{
	merc_uint_t	BodySize;
	merc_uint_t	NextLink;
	merc_uint_t	Pad[2];
} MERC_EXTENDED_BODY_HDR, *PMERC_EXTENDED_BODY_HDR;

#endif
