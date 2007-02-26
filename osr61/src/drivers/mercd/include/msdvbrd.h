/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdvbrd.h
 * Description                  : board structure definition
 *
 *
 **********************************************************************/

#ifndef _MSDVBRD_
#define _MSDVBRD_

//------------------------------------------------------------------------
// Board specific structures
// Following structures contain information specific to a perticular
// board
//------------------------------------------------------------------------

// Mercury Driver Board Attribute Block (Kaw)
typedef struct _MSD_BOARD_ATTR_BLK {
	merc_uchar_t BusNumber;
	merc_uchar_t SlotNumber;
#ifdef MERCD_PCI
	merc_uchar_t	SavedPciSlotNumber;
	merc_uchar_t SerialNumber[9]; // Length 8 chars + 1 NULL char
#endif /* MERCD_PCI */
	merc_uchar_t BusType;
} MSD_BOARD_ATTR_BLK, *PMSD_BOARD_ATTR_BLK;

// Mercury Driver Virtual Address Map Block
typedef struct _MSD_VIRT_ADDR_MAP_BLOCK{
   merc_uint_t    PhysAddr;
   merc_uint_t    VirtAddr;
   merc_uint_t    ByteCount;
   mercd_dev_acc_handle_T	DevAccHandle;
   merc_uint_t 	KernelSegment;
   merc_uint_t 	PageTableIndex;
} MSD_VIRT_ADDR_MAP_BLOCK, *PMSD_VIRT_ADDR_MAP_BLOCK;

#define MSD_MERC_MEM_PROP_REG	0 	// reg 0 from mercd.conf (KAW)

// Host ram timeout value in millisec. */
#define MSD_HRAM_SEM_TIMEOUT	2000

/* Kernel Virtual mapped addresses */
typedef struct _MERC_BOARD_REG_BLOCK {
	pmerc_uchar_t		HostRamStart;
	pmerc_uchar_t		ClrMsgReadyIntr;
	pmerc_uchar_t		ClrHostRamPendIntr;
	pmerc_uchar_t		ClrErrorIntr;
	pmerc_uchar_t		SetCpIntr;
	pmerc_uchar_t		SetCpNmi;
	pmerc_uchar_t		ClrCpNmi;
	pmerc_uchar_t		SetBoardReset;
	pmerc_uchar_t		ClrBoardReset;
	pmerc_uchar_t		SetHostRamRequest;
	pmerc_uchar_t		ClrHostRamRequest;
	pmerc_uchar_t		SetBootHostRamBit;
	pmerc_uchar_t		ClrBootHostRamBit;
	pmerc_uchar_t		DebugOneAddress;
	pmerc_uchar_t		DebugTwoAddress;
} MERC_BOARD_REG_BLOCK, *PMERC_BOARD_REG_BLOCK;

/* Host interface control block */

typedef struct _MERC_BOARD_HOSTIF_BLOCK {
	merc_uint_t	HostToFwMsgStart; /* addr of host to fw msg cir buffer */
	merc_uint_t	FwToHostMsgStart; /* addr of fw to host cir msg buf */
	merc_uint_t	MsgRingBufferSize; /* size of each msg cir. buf entry */
	merc_uint_t	HostToFwDataStart; /* addr of host to fw data cir buffer */
	merc_uint_t	FwToHostDataStart;/* addr of fw to host cir data buf */
	merc_uint_t	DataRingBufferSize;/* size of each data cir. buf entry */
	pmercury_free_hdr_list_sT  FreeListHeaderStart; /* addr of free data block header */
} MERC_BOARD_HOSTIF_BLOCK, *PMERC_BOARD_HOSTIF_BLOCK;


typedef struct _MERC_BOARD_BLOCK {			// MERC_PAM_ADAPTER_BLOCK
	merc_uint_t		Flags;
	merc_uchar_t		InterruptReason;
	merc_uchar_t		rfu1;
	merc_uchar_t		rfu2;
	merc_uchar_t		rfu3;
	merc_uint_t		SendTimeoutInterval;
	merc_uint_t		TimeoutId;
	MERC_BOARD_REG_BLOCK	RegisterBlock;
	PMERC_DEFERRED_Q_BLOCK BRD_MercDeferredQBlock;
	MERCURY_HOST_IF_CONFIG	ConfigBlock;
	MERCURY_HOST_IF_CONFIG_ACK	ConfigAckBlock;
	MERC_BOARD_HOSTIF_BLOCK	HostIf;
	merc_uint_t		AssociatedOpenBlock;
	merc_uchar_t		PanicDump[MD_PANIC_DUMP_MAX_SIZE];
} MERC_BOARD_BLOCK, *PMERC_BOARD_BLOCK;


/* Merc Board specific flags */

#define MERC_BOARD_FLAG_SEND_TIMEOUT_PEND	0x00000001	
#define MERC_BOARD_FLAG_CONFIGED		0x00000002	
#define MERC_BOARD_FLAG_HOSTIF_CONFIG_SENT	0x00000004
#define MERC_BOARD_FLAG_SEND_MSGS_PEND		0x00000008
#define MERC_BOARD_FLAG_SEND_DATA_PEND		0x00000010
#define MERC_BOARD_FLAG_LAUNCH_INTR		0x00000020
#define MERC_BOARD_FLAG_SRAM_IS_OURS		0x00000040
#define MERC_BOARD_FLAG_ON_DPC_Q		0x00000080
#define MERC_BOARD_FLAG_WAIT_INTR		0x00000100
#define MERC_BOARD_FLAG_DPC_INSERTED		0x00000200

//------------------------------------------------------------------------
// Driver specific structures
// Following structrues contain information which applies to all the MERC
// boards. Or they contain information about all the boards
//------------------------------------------------------------------------

// Mercury Driver Interrupt Block
typedef struct _MSD_INTR_BLOCK {
	merc_uint_t	Level;
	merc_uint_t	IntrVector;
	merc_uint_t	INumber;
	PMSD_FUNCTION	Isr;
	mercd_iblock_cookie_T	IBlockCookie;
} MSD_INTR_BLOCK, *PMSD_INTR_BLOCK;

// Mercury Driver Dpc Block
typedef struct _MSD_DPC_BLOCK {
	mercd_iblock_cookie_T  SoftIntrCookie;
	mercd_softintr_T   SoftIntrId;
	PMSD_FUNCTION  SoftIntrFunc;
	merc_uint_t    ReferenceCount;
	merc_uint_t    SoftIntrFlags;
} MSD_DPC_BLOCK, *PMSD_DPC_BLOCK;

//  Implementation specific state structure. Add any field
//  here and you can use it in the driver. Currently using
//  the dev_info_t field to get the physical registers to be
//  mapped.

typedef struct {
    mercd_dev_info_T        Msd_dip;
} MSD_STATE, *PMSD_STATE;					// KAW_STATE

// Mercury Driver MERC Boards Block
typedef struct _MSD_VBRDS_BLOCK {
	merc_uint_t		_BoardCount;
	merc_uint_t 	        _SramStartOffset[MSD_MAX_BOARD_ID_COUNT];
	MSD_BOARD_ATTR_BLK      _BoardAttrs[MSD_MAX_BOARD_ID_COUNT];
	mercd_dev_info_T  *_dips[MSD_MAX_BOARD_ID_COUNT];
#ifdef MERCD_PCI
	MSD_VIRT_ADDR_MAP_BLOCK _MsdPlxVirtMapBlock[MSD_MAX_BOARD_ID_COUNT];
#endif /* MERCD_PCI */
	MSD_VIRT_ADDR_MAP_BLOCK _MsdVirtMapBlock[MSD_MAX_BOARD_ID_COUNT];
	MSD_INTR_BLOCK          _IntrBlock[MSD_MAX_BOARD_ID_COUNT];
	MSD_DPC_BLOCK           _DpcBlock;
	PSTRM_QUEUE             _QueuePtr[MSD_MAX_OPEN_ALLOWED];
	merc_uchar_t            _ConfigRom[MSD_MAX_BOARD_ID_COUNT][MSD_CONFIG_ROM_MAX_SIZE];
} MSD_VBRDS_BLOCK, *PMSD_VBRDS_BLOCK;

typedef struct _MSD_DPC_DEBUG_BLOCK {
	merc_uint_t DpcIntrAdapterCount;
	merc_uint_t DpcFreeSramCount;
} MSD_DPC_DEBUG_BLOCK, *PMSD_DPC_DEBUG_BLOCK;

#ifdef MERCD_PCI
typedef struct _MSD_PROBE_BOARD_CONFIG {
        merc_ushort_t  BoardType;
        merc_ushort_t  PLXVenId;
        merc_ushort_t  PLXDevId;
        merc_ushort_t  SubId;
        merc_ushort_t  SubVenId;
        merc_uint_t    VendorId;
        merc_uint_t    SecondaryVendorId;
        merc_uint_t    PrimaryBoardId;
        merc_ushort_t  pci_reg_prop_num;
        merc_ushort_t  pci_reg_prop_sram;
        merc_ushort_t  pci_assignedaddr_prop_sram;
        merc_ushort_t  pci_assignedaddr_prop_num;
} MSD_PROBE_BOARD_CONFIG, *PMSD_PROBE_BOARD_CONFIG;
// Some Macros

#define BOARD_TYPE(x)                   msd_probe_brd_cfg[x].BoardType
#define PLX_VEN_ID(x)                   msd_probe_brd_cfg[x].PLXVenId
#define PLX_DEV_ID(x)                   msd_probe_brd_cfg[x].PLXDevId
#define SUBSYSTEM_ID(x)                 msd_probe_brd_cfg[x].SubId
#define SUB_VEN_ID(x)                   msd_probe_brd_cfg[x].SubVenId
#define VENDOR_ID(x)                    msd_probe_brd_cfg[x].VendorId
#define SEC_VENDOR_ID(x)                msd_probe_brd_cfg[x].SecondaryVendorId
#define PRIMARY_BOARD_ID(x)             msd_probe_brd_cfg[x].PrimaryBoardId
#define PCI_REG_PROP_NUM(x)             msd_probe_brd_cfg[x].pci_reg_prop_num
#define PCI_REG_PROP_SRAM(x)            msd_probe_brd_cfg[x].pci_reg_prop_sram
#define PCI_ASSIGNADDR_PROP_NUM(x)      msd_probe_brd_cfg[x].pci_assignedaddr_prop_num
#define PCI_ASSIGNADDR_PROP_SRAM(x)     msd_probe_brd_cfg[x].pci_assignedaddr_prop_sram

//WW support
#define MAX_PROBE_BRD_CFG       8
#define BRD_PCI         1
#define BRD_CPCI_1_5    2
#define BRD_CPCI_1_6    3
#define BRD_CPCI_1_7    4
//WW support
#define BRD_PCI_1_9     5
#define BRD_CPCI_1_9    6
#endif /* MERCD_PCI */
//THIRD_ROCK Support
#define BRD_CPCI_DTI16_21554  7
#define BRD_CPCI_DTI16_80312  8
#define BRD_CPCI_ROZETTA_21554  9
#define BRD_CPCI_DTI16_21555  10
//PCIe
#define BRD_PCIe 11
#endif // _MSDVBRD_
