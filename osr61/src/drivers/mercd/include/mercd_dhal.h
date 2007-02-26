/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_dhal.h
 * Description                  : driver hardware abstractoin layer
 *
 *
 **********************************************************************/


#define MERCD_DHAL_DRVR_IDENTIFY	0x0001
#define MERCD_DHAL_DRVR_SPECIFIC	0x0002
#define MERCD_DHAL_DRVR_VERIFY		0x0003
#define MERCD_DHAL_DRVR_PROBE		0x0004
#define MERCD_DHAL_MEM_COPY		0x0005
#define MERCD_DHAL_MEM_PEEK		0x0006
#define MERCD_DHAL_BRD_PROBE		0x0007
#define MERCD_DHAL_BRD_GETPROP		0x0008
#define MERCD_DHAL_BRD_MAP_ATTR 	0x0009
#define MERCD_DHAL_BRD_UNMAP_ATTR	0x000a
#define MERCD_DHAL_BRD_FILL_ATTR	0x000b
#define MERCD_DHAL_BRD_SAVE_ATTR	0x000c
#define MERCD_DHAL_BRD_CLEAN_ATTR	0x000d
#define MERCD_DHAL_BRD_GETINUMBER	0x000e
#define MERCD_DHAL_BRD_SCAN		0x000f
#define MERCD_DHAL_INTR_ENABLE		0x0010
#define MERCD_DHAL_INTR_DISABLE		0x0011
#define MERCD_DHAL_INTR_CHECK		0x0012
#define MERCD_DHAL_DRVR_FREE            0x0013

//////SRAMDMA BEGIN////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
#define MERCD_DHAL_DMA_FREE_HANDLE      0x0015
#define MERCD_DHAL_DMA_ALLOC_HANDLE     0x0016
#define MERCD_DHAL_PREPARE_DMA          0x0017
#endif
//////SRAMDMA END////////////



//WW support
#define MERCD_DHAL_WW_MF_ALLOC                          0x0018
#define MERCD_DHAL_WW_MF_DEALLOC                        0x0019
#define MERCD_DHAL_WW_RCV_COPY_FROM_DMAABLE_BUFFERS     0x001a
#define MERCD_DHAL_WW_BRD_MAP_PLXATTR                   0x001b
#define MERCD_DHAL_WW_INTR_CHECK                        0x001c
#define MERCD_DHAL_WW_GET_INTR_REQUEST                  0x001d
#define MERCD_DHAL_WW_CLEAR_INTR_REQUEST                0x001e
#define MERCD_DHAL_WW_RCV_BIGMSG_DIRCOPY_FROM_KERNEL    0x001f

//For deallocating both Rcv, Snd Big Msg Descriptor Tables
#define MERCD_DHAL_WW_DEALLOC_DESC_TABLE     0x0020 /* for both Kernel
                                                                  and DMAble buffers */
#define MERCD_DHAL_WW_REPLENISH_RCV_DESC         0x0021 /* relenish rcv descri */
#define MERCD_DHAL_WW_SWITCH_PLXMAP                     0x0022
#define MERCD_DHAL_WW_BRD_UNMAP_ATTR                    0x0023
#define MERCD_DHAL_WW_SND_COPY_FROM_DMAABLE_BUFFERS     0x0024
#define MERCD_DHAL_WW_SND_DIRCOPY_FROM_KERNEL           0x0025
#define MERCD_DHAL_WW_DEALLOC_RCV_BIGMSG_DESC           0x0026
#define MERCD_DHAL_WW_DEALLOC_SND_BIGMSG_DESC           0x0027
#define MERCD_DHAL_WW_RCV_STREAM_DIRCOPY_FROM_KERNEL    0x0028



typedef struct _mercd_dhal_drvr_identify{
	md_status_t ret;
} mercd_dhal_drvr_identify_sT, *pmercd_dhal_drvr_identify_sT;

typedef struct _mercd_dhal_drvr_specific {
	merc_int_t ConfigId;
	md_status_t ret;
}mercd_dhal_drvr_specific_sT, *pmercd_dhal_drvr_specific_sT;

typedef struct _mercd_dhal_mem_copy {
	pmerc_char_t src;
	pmerc_char_t dest;
	merc_uint_t size;
	merc_uint_t datatype;
	merc_uint_t dir;
}mercd_dhal_mem_copy_sT, *pmercd_dhal_mem_copy_sT;
	
typedef struct _mercd_dhal_brd_probe {
	merc_int_t board_type;
	mercd_dev_info_T *dip;
	md_status_t ret;
}mercd_dhal_brd_probe_sT, *pmercd_dhal_brd_probe_sT;

typedef struct _mercd_dhal_mem_peek {
	pmerc_char_t Address;
	merc_char_t Char;
	merc_int_t ConfigId;
	md_status_t ret;
}mercd_dhal_mem_peek_sT, *pmercd_dhal_mem_peek_sT;

typedef struct _mercd_dhal_mem_hotswap {
	merc_int_t ConfigId;
	mercd_dev_info_T *dip;
	pmercd_hw_info_sT phw_info;
	merc_int_t	hotswap;
	md_status_t ret;
}mercd_dhal_mem_hotswap_sT, *pmercd_dhal_mem_hotswap_sT;


typedef struct _mercd_dhal_brd_getprop {
	merc_int_t ConfigId;
	mercd_dev_info_T *dip;
	pmercd_hw_info_sT phw_info;
	md_status_t ret;
}mercd_dhal_brd_getprop_sT, *pmercd_dhal_brd_getprop_sT;


typedef struct _mercd_dhal_intr_enable {
	merc_int_t ConfigId;
	pmercd_hw_info_sT phw_info;
	md_status_t ret;
}mercd_dhal_intr_enable_sT, *pmercd_dhal_intr_enable_sT;

typedef struct _mercd_dhal_intr_disable {
	merc_int_t ConfigId;
	md_status_t ret;
}mercd_dhal_intr_disable_sT, *pmercd_dhal_intr_disable_sT;

	
typedef struct _mercd_dhal_brd_map_attr {
	pmercd_hw_info_sT phw_info;
	mercd_dev_info_T  *dip;
	md_status_t       ret;
}mercd_dhal_brd_map_attr_sT, *pmercd_dhal_brd_map_attr_sT;
	
typedef struct _mercd_dhal_brd_unmap_attr {
	merc_int_t        ConfigId;
	pmercd_hw_info_sT phw_info;
	mercd_dev_info_T  *dip;
	md_status_t       ret;
} mercd_dhal_brd_unmap_attr_sT, *pmercd_dhal_brd_unmap_attr_sT;
	
#define MERCD_DHAL_BRD_ATTRIB_DEV_ACC	1
#define MERCD_DHAL_BRD_ATTR_DEV_INFO    2

typedef struct _mercd_dhal_brd_fill_attr {
	merc_uint_t  attrib_type;
	mercd_acc_attr_T *pacc_handle;
	mercd_dev_info_T *dip;
} mercd_dhal_brd_fill_attr_sT, *pmercd_dhal_brd_fill_attr_sT;


typedef struct _mercd_dhal_brd_save_attr {
	merc_uint_t BusNumber;
	merc_uint_t SlotNumber;
	volatile pmerc_uchar_t ConfigStart; /* Was -> pmerc_uchar_t ConfigStart; */
	pmercd_hw_info_sT phw_info;	
	pmercd_host_info_sT phost_info;
	merc_uint_t PhysAddr;
	merc_int_t ConfigId;
	md_status_t ret;
}mercd_dhal_brd_save_attr_sT, *pmercd_dhal_brd_save_attr_sT;

typedef struct _mercd_dhal_brd_clean_attr {
	merc_short_t ConfigId;
}mercd_dhal_brd_clean_attr_sT, *pmercd_dhal_brd_clean_attr_sT;
	
typedef struct _mercd_dhal_brd_getinumber {
	merc_short_t ConfigId;
	merc_uint_t Level;
	merc_uint_t Vector;
	merc_uint_t  INumber;
	md_status_t ret;
}mercd_dhal_brd_getinumber_sT, *pmercd_dhal_brd_getinumber_sT;
	
typedef struct _mercd_dhal_brd_scan {
	merc_uint_t Slot;
	md_status_t ret;
}mercd_dhal_brd_scan_sT, *pmercd_dhal_brd_scan_sT;
	
typedef struct _mercd_dhal_drvr_verify {
	merc_int_t ConfigId;
	volatile pmerc_uchar_t ConfigStart; /* Was -> pmerc_uchar_t ConfigStart; */
	md_status_t ret;
}mercd_dhal_drvr_verify_sT, *pmercd_dhal_drvr_verify_sT;


typedef struct _mercd_dhal_drvr_probe {
	mercd_dev_info_T *pdevi;
	md_status_t ret;
}mercd_dhal_drvr_probe_sT, *pmercd_dhal_drvr_probe_sT;

typedef struct _mercd_dhal_drvr_free_sT {
	mercd_dev_info_T *pdevi;
	md_status_t ret;
}mercd_dhal_drvr_free_sT, *pmercd_dhal_drvr_free_sT;
	
typedef struct _mercd_dhal_intr_check {
	merc_uint_t AdapterNumber;
	merc_int_t ConfigId;
	merc_int_t InterruptVector;
	md_status_t ret;
}mercd_dhal_intr_check_sT, *pmercd_dhal_intr_check_sT;	

//WW support
typedef struct _mercd_dhal_ww_intr_check {
        pmercd_adapter_block_sT   padapter;
        md_status_t                    ret;
        merc_uint_t              intrReason;
}mercd_dhal_ww_intr_check_sT, *pmercd_dhal_ww_intr_check_sT;

typedef struct _mercd_dhal_ww_msgmf_alloc {
        pmercd_ww_dev_info_sT pwwDevi;
        void *resource; /*MFs and MSG Blk */
        md_status_t ret;
}mercd_dhal_ww_msgmf_alloc_sT, *pmercd_dhal_ww_msgmf_alloc_sT;

typedef struct _mercd_dhal_ww_rcv_copybigmsg_kmem {
        pmercd_ww_dev_info_sT pwwDevi;
        void *resource; /*MFs and MSG Blk */
        md_status_t ret;
        merc_uint_t purpose;
        merc_uint_t index;
}mercd_dhal_ww_rcv_copybigmsg_kmem_sT, *pmercd_dhal_ww_rcv_copybigmsg_kmem_sT;

typedef struct _mercd_dhal_ww_rcv_copybigmsg_str {
        pmercd_ww_dev_info_sT pwwDevi;
        merc_uint_t purpose;
        merc_uint_t index; /*This matters only for one descriptor alloc */
        md_status_t ret;
} mercd_dhal_ww_rcv_copybigmsg_str_sT, *pmercd_dhal_ww_rcv_copybigmsg_str_sT;

typedef struct _mercd_dhal_ww_rcv_rep_bigmsgdes {
        pmerc_void_t pwwDevi;
        merc_uint_t index;
        md_status_t ret;
        merc_uint_t purpose;
} mercd_dhal_ww_rcv_rep_bigmsgdes_sT, *pmercd_dhal_ww_rcv_rep_bigmsgdes_sT;

typedef struct _mercd_dhal_ww_dealloc_dmadescr {
        pmercd_ww_dev_info_sT pwwDevi;
        pmercd_ww_dma_descr_sT *resource;
        merc_uint_t           purpose;
        pmerc_void_t          extradata;
}mercd_dhal_ww_dealloc_dmadescr_sT, *pmercd_dhal_ww_dealloc_dmadescr_sT;

typedef struct _mercd_dhal_ww_snd_stream_mblk{
        pmercd_ww_dma_descr_sT  pdmaDescr;
        pmercd_ww_dev_info_sT pwwDevi;
        pmerc_void_t *mb;
        md_status_t ret;
        merc_uint_t purpose;
} mercd_dhal_ww_snd_stream_mblk_sT, *pmercd_dhal_ww_snd_stream_mblk_sT;


typedef struct _mercd_dhal_ww_plxmap {
        merc_ww_dev_info_sT *pwwDevi;
        md_status_t ret;
}mercd_dhal_ww_plxmap_sT, *pmercd_dhal_ww_plxmap_sT;

typedef struct _mercd_dhal_ww_switch_plxmap {
        merc_ww_dev_info_sT *pwwDevi;
        merc_int_t ConfigId;
        md_status_t ret;
}mercd_dhal_ww_switch_plxmap_sT, *pmercd_dhal_ww_switch_plxmap_sT;

typedef struct _mercd_dhal_ww_free_desc {
        merc_ww_dev_info_sT *pwwDevi;
        merc_ushort_t          index;
        md_status_t              ret;
        pmerc_void_t      StreamBlock; //WW Streaming
        merc_uint_t       purpose; //WW Streaming
}mercd_dhal_ww_free_desc_sT, *pmercd_dhal_ww_free_desc_sT;

//////SRAMDMA BEGIN////////////
#ifdef SRAM_DRVR_DMA_SUPPORT
typedef struct _mercd_dhal_dma_sT {
    mercd_dev_info_T    *pdevi;
    mercd_dev_acc_handle_T acc_handle;
    pmercd_dma_info_sT pdma;
    uint32_t   sramAddr;
    void *  padapter;
    void *  mb;
    merc_uint_t size;
    merc_int_t   dir;
    md_status_t  ret;
} mercd_dhal_dma_sT, *pmercd_dhal_dma_sT;

typedef struct _mercd_dhal_dma_init_sT {
        merc_int_t   ConfigId;
        md_status_t  ret;
}  mercd_dhal_dma_init_sT, *pmercd_dhal_dma_init_sT;

typedef struct _mercd_dhal_dma_free_sT {
        merc_int_t   ConfigId;
        md_status_t  ret;
} mercd_dhal_dma_free_sT, *pmercd_dhal_dma_free_sT;
#endif /* SRAM_DRVR_DMA_SUPPORT */
//////SRAMDMA END////////////
