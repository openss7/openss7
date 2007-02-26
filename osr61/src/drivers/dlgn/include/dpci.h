/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dpci.h
 * Description                  : generic driver header
 *
 *
 **********************************************************************/

/* Bus Types */

#define BUS_TYPE_ISA 			0x0001
#define BUS_TYPE_EISA			0x0002
#define BUS_TYPE_VME			0x0003
#define BUS_TYPE_PCI			0x0004
#define BUS_TYPE_MICRO_CHANNEL		0x0005
#define BUS_TYPE_PCMCIA			0x0006

#define GP_FLAG_PCI		0x0004
#define GP_MAX_CONFIG_ID_COUNT	32     /*Physical Boards */
#define GP_CONFIG_ROM_MAX_SIZE	0x100  

#define GP_STATE_STOPPED	0
#define GP_STATE_READY		1

#define GP_PCI_PLX_PROP_REG	1	/* reg 1 from reg propertry */
#define GP_PCI_MEM_PROP_REG	2	/* reg 2 from reg propertry */
#define GP_PCI_PYT_PROP_REG	3	/* reg 3 from reg propertry */

#define PCI_PLX_INTR_REG        0x4C
#define PCIe_PLX_INTR_REG       0x68
#define PCI_PLX_FF_REG          0x50
#define PCIe_PLX_FF_REG         0x68
#define PCI_PLX_INTR_ENABLE_BIT 0x41
#define PCIe_PLX_INTR_ENABLE_BIT 0x00010900
#define PCI_PLX_INTR_ACTIVE_BIT 0x04

#define PCI_LOCATE1 0xE4
#define PCI_LOCATE2 0xE0

#define BYTESETFLIPFLOP		0x96
#define BYTERESETFLIPFLOP	0x92
#define PLX_ENABLE		0x143
#define PLX_CLEAR		0x400
#define PLX_DISABLE		0xFFFFFFBE


#define PCI_MAX_BOARDS          16
#define SRAM_MEM                0x100

#define DLG_GENID               0xFFFF
#define DLG_VENDID              0x10B5
#define DLG_DEVID               0x9050
#define NDLG_DEVID              0x0619
#define NDLG_VENDID             0x12C7
#define DLG_BCLASS              0x06
#define DLG_SCLASS              0x80
#define NDLG_BCLASS             0x04
#define NDLG_SCLASS             0x02

#ifndef sun 
#define PCI_CONF_BASE0          0x10  /* Configuration address for DLGC PCI */
#define PCI_CONF_BASE2          0x18  /* SRAM address for DLGC PCI */
#define PCI_CONF_SUBVENID       0x2C  /* Subvendor  ID for DLGC PCI */
#define PCI_CONF_SUBSYSID       0x2E  /* Subsystem ID for DLGC PCI */
#define PCI_CONF_ILINE          0x3C  /* Interrupt Line for DLGC PCI */
#endif

#define PCI_ANT_ID		0x017812C7
#define PCI_MAGNUM_T1_ID	0x052512C7
#define PCI_MAGNUM_E120_ID	0x052612C7
#define PCI_MAGNUM_E1_ID	0x052712C7
#define PCI_PYTHON_ID	        0x052912C7
#define PCI_D120LSI_ID		0x054612C7
#define PCI_ROADRUNNER_ID	0x056112C7
#define PCI_COYOTE_ID		0x056212C7
#define PCI_D80_ID		0x060412C7
#define PCI_D320_ID		0x060512C7
#define PCI_D82U_ID		0x061912C7
#define PCI_SPANPLUS_T1_ID	0x064712C7
#define PCI_SPANPLUS_E120_ID	0x064812C7
#define PCI_SPANPLUS_E1_ID	0x064912C7
#define PCI_MSI_ID		0x065112C7
#define PCI_BRI80_ID            0x067512C7
#define PCI_BRI160_ID		0x067312C7
#define PCI_D41JCT_ID		0x067612C7
#define PCI_D480JCT_T1_ID	0x068512C7
#define PCI_D600JCT_E1_ID	0x068712C7
#define PCI_D600JCT_E120_ID	0x068912C7
#define PCI_D320JCT_ID		0x070712C7
#define PCI_D160JCT_ID		0x070812C7

#define PCI_ANT_DEV_ID          0x0178
#define PCI_MAGNUM_T1_DEV_ID    0x0525
#define PCI_MAGNUM_E120_DEV_ID  0x0526
#define PCI_MAGNUM_E1_DEV_ID    0x0527
#define PCI_PYTHON_DEV_ID       0x0529
#define PCI_D120LSI_DEV_ID      0x0546
#define PCI_ROADRUNNER_DEV_ID   0x0561
#define PCI_COYOTE_DEV_ID       0x0562
#define PCI_D80_DEV_ID          0x0604
#define PCI_D320_DEV_ID         0x0605
#define PCI_D82U_DEV_ID         0x0619
#define PCI_SPANPLUS_T1_DEV_ID  0x0647
#define PCI_SPANPLUS_E120_DEV_ID 0x0648
#define PCI_SPANPLUS_E1_DEV_ID  0x0649
#define PCI_MSI_DEV_ID          0x0651
#define PCI_BRI80_DEV_ID        0x0675
#define PCI_BRI160_DEV_ID       0x0673
#define PCI_D41JCT_DEV_ID       0x0676
#define PCI_D480JCT_T1_DEV_ID   0x0685
#define PCI_D600JCT_E1_DEV_ID   0x0687
#define PCI_D600JCT_E120_DEV_ID 0x0689
#define PCI_D320JCT_DEV_ID	0x0707
#define PCI_D160JCT_DEV_ID	0x0708

#define IS_DIALOGIC_PCI(a) ((a == PCI_MAGNUM_T1_ID)     || \
			    (a == PCI_MAGNUM_E1_ID)     || \
			    (a == PCI_MAGNUM_E120_ID)   || \
			    (a == PCI_MAGNUM_E1_ID)     || \
			    (a == PCI_PYTHON_ID)        || \
			    (a == PCI_D120LSI_ID)       || \
			    (a == PCI_ROADRUNNER_ID)    || \
			    (a == PCI_COYOTE_ID)        || \
			    (a == PCI_D80_ID)           || \
			    (a == PCI_D320_ID)          || \
			    (a == PCI_D82U_ID)          || \
			    (a == PCI_SPANPLUS_T1_ID)   || \
			    (a == PCI_SPANPLUS_E120_ID) || \
			    (a == PCI_SPANPLUS_E1_ID)   || \
			    (a == PCI_MSI_ID)  	        || \
			    (a == PCI_BRI80_ID)         || \
			    (a == PCI_BRI160_ID)        || \
			    (a == PCI_D41JCT_ID)        || \
			    (a == PCI_D160JCT_ID)       || \
			    (a == PCI_D320JCT_ID)       || \
			    (a == PCI_D480JCT_T1_ID)    || \
			    (a == PCI_D600JCT_E1_ID)    || \
			    (a == PCI_D600JCT_E120_ID)) 

#define IS_MAGNUM_TYPE(a) ((a == PCI_MAGNUM_T1_ID)     || \
			   (a == PCI_MAGNUM_E1_ID)     || \
			   (a == PCI_MAGNUM_E120_ID)   || \
			   (a == PCI_MAGNUM_E1_ID)     || \
			   (a == PCI_D80_ID)           || \
			   (a == PCI_D320_ID) )

#define IS_SPANPLUS_TYPE(a) ((a == PCI_D120LSI_ID)       || \
			     (a == PCI_D82U_ID)          || \
			     (a == PCI_SPANPLUS_T1_ID)   || \
			     (a == PCI_SPANPLUS_E120_ID) || \
			     (a == PCI_SPANPLUS_E1_ID)   || \
			     (a == PCI_MSI_ID)  	 || \
			     (a == PCI_BRI80_ID)         || \
			     (a == PCI_BRI160_ID)        || \
			     (a == PCI_D160JCT_ID)       || \
			     (a == PCI_D320JCT_ID)       || \
			     (a == PCI_D480JCT_T1_ID)    || \
			     (a == PCI_D600JCT_E1_ID)    || \
			     (a == PCI_D600JCT_E120_ID)) 

#ifdef DPCI_H
struct { unsigned short vendor, dev_id; }
pci_spring_list[] = {
                {DLG_VENDID,  DLG_DEVID                 },
                {NDLG_VENDID, PCI_ANT_DEV_ID            },
                {NDLG_VENDID, PCI_D120LSI_DEV_ID        },
                {NDLG_VENDID, PCI_ROADRUNNER_DEV_ID     },
	     	{NDLG_VENDID, PCI_COYOTE_DEV_ID         },
                {NDLG_VENDID, PCI_COYOTE_DEV_ID         },
                {NDLG_VENDID, PCI_D80_DEV_ID            },
                {NDLG_VENDID, PCI_D320_DEV_ID           },
                {NDLG_VENDID, PCI_D82U_DEV_ID           },
                {NDLG_VENDID, PCI_SPANPLUS_T1_DEV_ID    },
                {NDLG_VENDID, PCI_SPANPLUS_E120_DEV_ID  },
                {NDLG_VENDID, PCI_SPANPLUS_E1_DEV_ID    },
                {NDLG_VENDID, PCI_MSI_DEV_ID            },
		{NDLG_VENDID, PCI_BRI80_DEV_ID          },
                {NDLG_VENDID, PCI_BRI160_DEV_ID         },
                {NDLG_VENDID, PCI_D41JCT_DEV_ID         },
                {NDLG_VENDID, PCI_D480JCT_T1_DEV_ID     },
                {NDLG_VENDID, PCI_D600JCT_E1_DEV_ID     },
                {NDLG_VENDID, PCI_D600JCT_E120_DEV_ID   },
                {NDLG_VENDID, PCI_D160JCT_DEV_ID	},
                {NDLG_VENDID, PCI_D320JCT_DEV_ID	},
                {0,}

};

#define IS_SUPPORTED_TYPE(a) ((a == PCI_BRI80_DEV_ID)        || \
			      (a == PCI_BRI160_DEV_ID)       || \
			      (a == PCI_COYOTE_DEV_ID)       || \
			      (a == PCI_D41JCT_DEV_ID)       || \
			      (a == PCI_D480JCT_T1_DEV_ID)   || \
			      (a == PCI_D600JCT_E1_DEV_ID)   || \
			      (a == PCI_D600JCT_E120_DEV_ID) || \
			      (a == PCI_D320JCT_DEV_ID)      || \
			      (a == PCI_D160JCT_DEV_ID)      || \
                              (a == PCI_D120LSI_DEV_ID)      || \
			      (a == PCI_D82U_DEV_ID)         || \
			      (a == PCI_SPANPLUS_T1_DEV_ID)  || \
			      (a == PCI_SPANPLUS_E120_DEV_ID)|| \
			      (a == PCI_SPANPLUS_E1_DEV_ID))
#endif

#ifdef LFS
typedef ULONG 	(*PGP_FUNCTION)(int, void *, struct pt_regs *);
#else
typedef ULONG 	(*PGP_FUNCTION)();
#endif
typedef void 	*PVOID;
typedef UCHAR   *PUCHAR;
typedef CHAR    *PCHAR;
typedef INT     *PINT;

typedef struct _GLOBAL_PCI_INFO {
        PUCHAR  LocatorAddress;
        PUCHAR  ReArmAddress;
        PUCHAR  ConfigAddress;
        PUCHAR  InterruptCSR;
        PUCHAR  InterruptEnable;
        USHORT  BoardProperties;
} GLOBAL_PCI_INFO, *PGLOBAL_PCI_INFO;

typedef struct _GP_INTR_BLOCK {
	ULONG		Level;
	ULONG		Intr_arg;
	ULONG		INumber;
	PGP_FUNCTION	Isr;
#ifdef sun
	ddi_iblock_cookie_t	IBlockCookie;
#endif /* sun */
} GP_INTR_BLOCK, *PGP_INTR_BLOCK;

typedef struct _GP_VIRT_ADDR_MAP_BLOCK{
	unsigned long	PhysAddr;
	unsigned long	VirtAddr;
	unsigned int	ByteCount;
	unsigned long 	CfgPhysAddr;
	unsigned long 	CfgVirtAddr;
#ifdef DDI8
	unsigned int 	RmKey;
#endif /* DDI8 */
	unsigned long   PlxAddr;
	unsigned long   PlxSize;	
} GP_VIRT_ADDR_MAP_BLOCK, *PGP_VIRT_ADDR_MAP_BLOCK;

typedef struct _GP_BOARD_ATTR_BLK {
	UCHAR	BusNumber;
	UCHAR	SlotNumber;
 	UCHAR   FuncNumber;
	UCHAR	BusType;
	UCHAR	BoardId;
	ULONG	BoardType;
        USHORT  TypePCIExpress;
        ULONG   PowerGoodReg;
        ULONG   PowerOverrideReg;
        ULONG   ActualPowerProvided;
        ULONG   PowerRequiredByBoard;
} GP_BOARD_ATTR_BLK, *PGP_BOARD_ATTR_BLK;

// PCI Express
#define PCI_CAPABILITY_LIST_REG_OFFSET          0x34            // Start of the PCI capability list
#define PCI_CAPABILITY_TYPE_MASK                0x00FF          // Bits 7:0
#define PCI_CAPABILITY_LIST_OFFSET_MASK         0xFF00          // Bits 15:8
#define PCI_CAPABILITY_LIST_OFFSET              8

#define PCI_EXPRESS_CAPABILITY                  0x10            // Indicates PCI Express board.
#define PCI_EXPRESS_POWER_SCALE_MASK            0x0C000000      // Bits 27:26 of the device capability
#define PCI_EXPRESS_POWER_SCALE_OFFSET          26
#define PCI_EXPRESS_POWER_LIMIT_MASK            0x03FC0000      // Bits 25:18 of the device capability
#define PCI_EXPRESS_POWER_LIMIT_OFFSET          18

// Seaville specific defines for power management
#define SEAVILLE_POWER_GOOD_REG_OFFSET          0x150           // Offset into BAR 0
#define SEAVILLE_POWER_GOOD_MASK                0x20000000      // Bit 29
#define SEAVILLE_POWER_GOOD_OFFSET              29

#define SEAVILLE_USER_OVERRIDE_POWER_REG_OFFSET 0x54            // Offset into BAR 0
#define SEAVILLE_USER_OVERRIDE_POWER_MASK       0x800           // Bit 11
#define SEAVILLE_USER_OVERRIDE_POWER_OFFSET     11

#define SEAVILLE_POWER_BUDGETTING_REG_USED      0x80000000      // MS bit
#define SEAVILLE_POWER_WATTAGE_READ_MASK        0x000000FF      // LS Byte

// The PCIE version of the PCIA board runs in the J mode
// the PoweOverride bit is read off from reg 6C unlike in the I mode
#define SEAVILLE_USER_OVERRIDE_POWER_REG_OFFSET_J_MODE          0x6C
#define SEAVILLE_USER_OVERRIDE_POWER_MASK_J_MODE                0x20000
#define SEAVILLE_USER_OVERRIDE_POWER_OFFSET_J_MODE              17

typedef struct _GP_CONTROL_BLOCK {
	USHORT	GpState;
	USHORT	GpFlags;
	ULONG	IntrRegCount;
	ULONG	MsgLevel;
	UCHAR	BoardCount;
	ULONG	SramStartOffset[GP_MAX_CONFIG_ID_COUNT];
	GP_BOARD_ATTR_BLK	BoardAttrs[GP_MAX_CONFIG_ID_COUNT];
#ifdef sun
	dev_info_t	*dips[GP_MAX_CONFIG_ID_COUNT];
	ddi_acc_handle_t sram_data_handles[GP_MAX_CONFIG_ID_COUNT];
	ddi_acc_handle_t cfg_data_handles[GP_MAX_CONFIG_ID_COUNT];
#endif /* sun */
	GP_INTR_BLOCK	IntrBlock[GP_MAX_CONFIG_ID_COUNT];
	GP_VIRT_ADDR_MAP_BLOCK	GpVirtMapBlock[GP_MAX_CONFIG_ID_COUNT];
} GP_CONTROL_BLOCK, *PGP_CONTROL_BLOCK;

/* Macros to Board Access */
#define GET_BRD_TYPE(index)                                             \
        (GpControlBlock->BoardAttrs[index].BoardType)
 
#define GET_PHY_ADDR(index)                                             \
        ((ULONG)GpControlBlock->GpVirtMapBlock[index].PhysAddr)
 
#define GET_VIRT_ADDR(index)                                            \
        ((ULONG)GpControlBlock->GpVirtMapBlock[index].VirtAddr)
 
#define GET_CFG_VIRT_ADDR(index)                                        \
        ((ULONG)GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr)
 
#define GET_CFG_PHY_ADDR(index)                                         \
        ((ULONG)GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr)
 
#define CHK_PHY_ADDR(physaddr, mask)                                    \
        ((physaddr) & (mask))
 
#define CHK_BRD_PHY_ADDR(physaddr, mask, index)                         \
        ((physaddr & mask) == (GET_PHY_ADDR(index)))

#define SET_PHYS_ADDR(index, address)					\
	(GpControlBlock->GpVirtMapBlock[index].PhysAddr = address)
 
#define SET_VIRT_ADDR(index, address)                                   \
        (GpControlBlock->GpVirtMapBlock[index].VirtAddr = address)
 
#define SET_CFG_VIRT(index, address)                                    \
        (GpControlBlock->GpVirtMapBlock[index].CfgVirtAddr = address)

#define SET_CFG_PHYS(index, address)					\
	(GpControlBlock->GpVirtMapBlock[index].CfgPhysAddr = address)	

#define SET_INTRBLOCK_LEVEL(index, level)				\
	(GpControlBlock->IntrBlock[index].Level = level)

#define SET_INTRBLOCK_INTR_ARG(index, arg)				\
	(GpControlBlock->IntrBlock[index].Intr_arg = arg)
	
#ifdef LINUX 
#ifdef LFS
#define MAP_ADDR(address, sz)                 		                \
        ((UCHAR *)ioremap_nocache(address, sz))
#else
#define MAP_ADDR(address, sz)                 		                \
        ((UCHAR *)lis_ioremap_nocache(address, sz))
#endif
#else
#define MAP_ADDR(address, sz)
#endif

#define PCI_2K(index)                                                   \
        (GET_BRD_TYPE(index) == PCI_MSI_ID)    ||                       \
        (GET_BRD_TYPE(index) == PCI_PYTHON_ID) ||                       \
        (GET_BRD_TYPE(index) == PCI_COYOTE_ID)

/* Macros for Linux PCI kernel calls */
#if LINUX_VERSION_CODE < 0x020400   
#define DLGC_PCI_DEV(pdev) 	LIS_PCI_DEV (pdev->dev_fcn)
#define DLGC_PCI_FCN(pdev)	LIS_PCI_FCN (pdev->dev_fcn)
#define DLGC_PCI_SLOT(pdev)	DLGC_PCI_DEV(pdev)
#define DLGC_PCI_MEM(pdev,val)  pdev->mem_addrs[val]
#elif LINUX_VERSION_CODE < 0x020800
#define DLGC_PCI_DEV(pdev)	PCI_DEVFN(PCI_SLOT(pdev->devfn), pdev->devfn)
#define DLGC_PCI_FCN(pdev) 	PCI_FUNC(pdev->devfn)
#define DLGC_PCI_SLOT(pdev) 	PCI_SLOT(pdev->devfn)
#define DLGC_PCI_MEM(pdev,val)  pci_resource_start(pdev, val)
#else
#define DLGC_PCI_DEV(pdev)
#define DLGC_PCI_FCN(pdev) 
#define DLGC_PCI_SLOT(pdev)
#define DLGC_PCI_MEM(pdev,val)
#endif

#define SET_BUS_SLOT_TYPE_FCN_ID(index, b, s, t, f, id) {		\
	GpControlBlock->BoardAttrs[index].BusNumber = b;		\
	GpControlBlock->BoardAttrs[index].SlotNumber = s;		\
	GpControlBlock->BoardAttrs[index].BusType = t;			\
	GpControlBlock->BoardAttrs[index].FuncNumber = f; 		\
	GpControlBlock->BoardAttrs[index].BoardType = id;		}

#define SET_TYPE_PCIe(index, v) (GpControlBlock->BoardAttrs[index].TypePCIExpress = v)
#define GET_TYPE_PCIe(index) ((USHORT)GpControlBlock->BoardAttrs[index].TypePCIExpress)

#define SET_POWER_GOOD_REG(index, v) (GpControlBlock->BoardAttrs[index].PowerGoodReg = v)
#define SET_POWER_OVERRIDE(index, v) (GpControlBlock->BoardAttrs[index].PowerOverrideReg = v)
#define SET_POWER_PROVIDED(index, v) (GpControlBlock->BoardAttrs[index].ActualPowerProvided = v)
#define SET_POWER_REQUIRED(index, v) (GpControlBlock->BoardAttrs[index].PowerRequiredByBoard = v)

#define GET_POWER_GOOD_REG(index) ((ULONG)GpControlBlock->BoardAttrs[index].PowerGoodReg)
#define GET_POWER_OVERRIDE(index) ((ULONG)GpControlBlock->BoardAttrs[index].PowerOverrideReg)
#define GET_POWER_PROVIDED(index) ((ULONG)GpControlBlock->BoardAttrs[index].ActualPowerProvided)
#define GET_POWER_REQUIRED(index) ((ULONG)GpControlBlock->BoardAttrs[index].PowerRequiredByBoard)


	
