/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdextern.h
 * Description                  : externe functions
 *
 *
 **********************************************************************/

#ifndef _MSDEXTERN_
#define _MSDEXTERN_

#ifdef _MERCD_C

#if defined LiS || defined LIS || defined LFS
#ifdef LFS
streamscall int mercd_open( queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp);
streamscall int mercd_close( queue_t *q, int flag, cred_t *crp);
streamscall int mercd_writeput( queue_t *q, mblk_t *mp);
streamscall int mercd_rsrv (queue_t * wq);
#else
int mercd_open( queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp);
int mercd_close( queue_t *q);
int mercd_writeput( queue_t *q, mblk_t *mp);
int mercd_rsrv (queue_t * wq);
#endif
#else
int mercd_open(struct inode *inode, struct file *file);
int mercd_close(struct inode *inode, struct file *file);
ssize_t mercd_read(struct file *, char *, size_t, loff_t *);
ssize_t mercd_write(struct file *, const char *, size_t, loff_t *);
unsigned int mercd_poll(struct file *filp, struct poll_table_struct *wait);
int mercd_mmap(struct file *file, struct vm_area_struct *vma);
int mercd_ioctl(struct inode *inode, struct file *file, unsigned int functionId, unsigned long arg);
#endif /* LiS */

#else

#if defined LiS || defined LIS || defined LFS
#ifdef LFS
extern streamscall int mercd_open( queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp);
extern streamscall int mercd_close( queue_t *q, int flag, cred_t *crp);
extern streamscall int mercd_writeput( queue_t *q, mblk_t *mp);
extern streamscall int mercd_rsrv (queue_t * wq);
#else
extern int mercd_open( queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp);
extern int mercd_close( queue_t *q);
extern int mercd_writeput( queue_t *q, mblk_t *mp);
extern int mercd_rsrv (queue_t * wq);
#endif
#else
extern int mercd_open(struct inode *inode, struct file *file);
extern int mercd_close(struct inode *inode, struct file *file);
extern ssize_t mercd_read(struct file *, char *, size_t, loff_t *);
extern ssize_t mercd_write(struct file *, const char *, size_t, loff_t *);
extern unsigned int mercd_poll(struct file *filp, struct poll_table_struct *wait);
extern int mercd_mmap(struct file *file, struct vm_area_struct *vma);
extern int mercd_ioctl(struct inode *inode, struct file *file, unsigned int functionId, unsigned long arg);
#endif /* LiS */

#endif


#ifdef MERCD_SOLARIS

#ifdef _MERCD_SOLARIS_OSAL_C
int initialize_solaris_osal();
#else
extern int initialize_solaris_osal();
#endif

#ifdef MERCD_VME
#ifdef _MERCD_SOLARIS_VME_DHAL_C
int initialize_solaris_vme_dhal();
void mercd_return_success(pmerc_void_t rcvPtr);
#else
extern int initialize_solaris_vme_dhal();
extern void mercd_return_success(pmerc_void_t rcvPtr);
#endif
#endif

#ifdef MERCD_PCI

#ifdef _MERCD_SOLARIS_PCI_DHAL_C
int initialize_solaris_pci_dhal();
void mercd_return_success(pmerc_void_t rcvPtr);
#else
extern int initialize_solaris_pci_dhal();
extern void mercd_return_success(pmerc_void_t rcvPtr);
#endif

#endif

#ifdef MERCD_OSSDL_SOLARIS_C

void solaris_phys2virt(pmerc_void_t rcvPtr);
void solaris_free_virt_addr(pmerc_void_t rcvPtr);
void solaris_timeout(pmerc_void_t rcvPtr);
void solaris_untimeout(pmerc_void_t rcvPtr );
void solaris_intr_register_intr_srvc(pmerc_void_t rcvPtr);
void solaris_intr_deregister_intr_srvc(pmerc_void_t rcvPtr);
merc_uint_t solaris_intr_srvc(merc_uint_t AdapterNumber);
void solaris_register_dpc(pmerc_void_t rcvPtr);
void solaris_deregister_dpc(pmerc_void_t rcvPtr);
merc_uint_t solaris_intr_srvc(merc_uint_t AdapterNumber);

#else

extern void solaris_phys2virt(pmerc_void_t rcvPtr);
extern void solaris_free_virt_addr(pmerc_void_t rcvPtr);
extern void solaris_timeout(pmerc_void_t rcvPtr);
extern void solaris_untimeout(pmerc_void_t rcvPtr );
extern void solaris_intr_register_intr_srvc(pmerc_void_t rcvPtr);
extern void solaris_intr_deregister_intr_srvc(pmerc_void_t rcvPtr);
extern merc_uint_t solaris_intr_srvc(merc_uint_t AdapterNumber);
extern void solaris_register_dpc(pmerc_void_t rcvPtr);
extern void solaris_deregister_dpc(pmerc_void_t rcvPtr);
extern merc_uint_t solaris_intr_srvc(merc_uint_t AdapterNumber);

#endif

#ifdef MERCD_PCI

#ifdef MERCD_OSSHL_SOLARIS_PCI_C
void solaris_pci_drvr_identify(pmerc_void_t rcvPtr);
void solaris_pci_device_copy(pmerc_void_t rcvPtr);
void solaris_pci_probe_this_board(pmerc_void_t rcvPtr);
void solaris_pci_peek_char(pmerc_void_t rcvPtr);
void solaris_pci_get_prop(pmerc_void_t rcvPtr);
void solaris_pci_intr_enable(pmerc_void_t rcvPtr);
void solaris_pci_intr_disable(pmerc_void_t rcvPtr);
void solaris_pci_map_adapter_parm(pmerc_void_t rcvPtr);
void solaris_pci_unmap_adapter_parm(pmerc_void_t rcvPtr);
void solaris_pci_verify_merc_adapter(pmerc_void_t rcvPtr);
void solaris_pci_fill_attr(pmerc_void_t rcvPtr);
void solaris_pci_save_attr(pmerc_void_t rcvPtr);
void solaris_pci_getinumber(pmerc_void_t rcvPtr);
void solaris_pci_drvr_probe(pmerc_void_t rcvPtr);
void solaris_pci_drvr_free(pmerc_void_t rcvPtr);
void solaris_pci_intr_check(pmerc_void_t rcvPtr);
void solaris_pci_peek_char(pmerc_void_t rcvPtr);
void solaris_pci_go_hotswap(pmerc_void_t rcvPtr);
void solaris_pci_dma_alloc_handle(void *rcvPtr);
void solaris_pci_dma_free_handle(void *rcvPtr);
void solaris_pci_prepare_dma(void *rcvPtr);


#else

extern void solaris_pci_drvr_identify(pmerc_void_t rcvPtr);
extern void solaris_pci_device_copy(pmerc_void_t rcvPtr);
extern void solaris_pci_probe_this_board(pmerc_void_t rcvPtr);
extern void solaris_pci_peek_char(pmerc_void_t rcvPtr);
extern void solaris_pci_get_prop(pmerc_void_t rcvPtr);
extern void solaris_pci_intr_enable(pmerc_void_t rcvPtr);
extern void solaris_pci_intr_disable(pmerc_void_t rcvPtr);
extern void solaris_pci_map_adapter_parm(pmerc_void_t rcvPtr);
extern void solaris_pci_unmap_adapter_parm(pmerc_void_t rcvPtr);
extern void solaris_pci_verify_merc_adapter(pmerc_void_t rcvPtr);
extern void solaris_pci_fill_attr(pmerc_void_t rcvPtr);
extern void solaris_pci_save_attr(pmerc_void_t rcvPtr);
extern void solaris_pci_getinumber(pmerc_void_t rcvPtr);
extern void solaris_pci_drvr_probe(pmerc_void_t rcvPtr);
extern void solaris_pci_drvr_free(pmerc_void_t rcvPtr);
extern void solaris_pci_intr_check(pmerc_void_t rcvPtr);
extern void solaris_pci_peek_char(pmerc_void_t rcvPtr);
extern void solaris_pci_go_hotswap(pmerc_void_t rcvPtr);
extern void solaris_pci_dma_alloc_handle(void *rcvPtr);
extern void solaris_pci_dma_free_handle(void *rcvPtr);
extern void solaris_pci_prepare_dma(void *rcvPtr);


#endif

#endif /* MERCD_PCI */

#ifdef MERCD_VME

#ifdef MERCD_OSSHL_SOLARIS_VME_C
void solaris_vme_drvr_identify(pmerc_void_t rcvPtr);
void solaris_vme_scan_brd(pmerc_void_t rcvPtr);
void solaris_vme_device_copy(pmerc_void_t rcvPtr);
void solaris_vme_probe_this_board(pmerc_void_t rcvPtr);
void solaris_vme_peek_char(pmerc_void_t rcvPtr);
void solaris_vme_get_prop(pmerc_void_t rcvPtr);
void solaris_vme_map_adapter_parm(pmerc_void_t rcvPtr);
void solaris_vme_unmap_adapter_parm(pmerc_void_t rcvPtr);
void solaris_vme_verify_merc_adapter(pmerc_void_t rcvPtr);
void solaris_vme_specific(pmerc_void_t rcvPtr);
void solaris_vme_fill_attr(pmerc_void_t rcvPtr);
void solaris_vme_save_attr(pmerc_void_t rcvPtr);
void solaris_vme_getinumber(pmerc_void_t rcvPtr);
void solaris_vme_drvr_probe(pmerc_void_t rcvPtr);
void solaris_vme_brd_clean_attr(pmerc_void_t rcvPtr);

#else

extern void solaris_vme_drvr_identify(pmerc_void_t rcvPtr);
extern void solaris_vme_scan_brd(pmerc_void_t rcvPtr);
extern void solaris_vme_device_copy(pmerc_void_t rcvPtr);
extern void solaris_vme_probe_this_board(pmerc_void_t rcvPtr);
extern void solaris_vme_peek_char(pmerc_void_t rcvPtr);
extern void solaris_vme_get_prop(pmerc_void_t rcvPtr);
extern void solaris_vme_map_adapter_parm(pmerc_void_t rcvPtr);
extern void solaris_vme_unmap_adapter_parm(pmerc_void_t rcvPtr);
extern void solaris_vme_verify_merc_adapter(pmerc_void_t rcvPtr);
extern void solaris_vme_specific(pmerc_void_t rcvPtr);
extern void solaris_vme_fill_attr(pmerc_void_t rcvPtr);
extern void solaris_vme_save_attr(pmerc_void_t rcvPtr);
extern void solaris_vme_getinumber(pmerc_void_t rcvPtr);
extern void solaris_vme_drvr_probe(pmerc_void_t rcvPtr);
extern void solaris_vme_brd_clean_attr(pmerc_void_t rcvPtr);

#endif

#endif /* MERCD_VME */

#endif /* MERCD_SOLARIS */

#ifdef MERCD_UNIXWARE

#ifdef _MERCD_UNIXWARE_OSAL_C
int initialize_unixware_osal();
#else
extern int initialize_unixware_osal();
#endif

#ifdef MERCD_PCI

#ifdef _MERCD_UNIXWARE_PCI_DHAL_C
int initialize_unixware_pci_dhal();
void mercd_return_success(pmerc_void_t rcvPtr);
#else
extern int initialize_unixware_pci_dhal();
extern void mercd_return_success(pmerc_void_t rcvPtr);
#endif

#endif

#ifdef MERCD_OSSDL_UNIXWARE_C

void unixware_phys2virt(pmerc_void_t rcvPtr);
void unixware_free_virt_addr(pmerc_void_t rcvPtr);
void unixware_timeout(pmerc_void_t rcvPtr);
void unixware_untimeout(pmerc_void_t rcvPtr );
void unixware_intr_register_intr_srvc(pmerc_void_t rcvPtr);
void unixware_intr_deregister_intr_srvc(pmerc_void_t rcvPtr);
void mercdintr(int InterruptVector);

#else

extern void unixware_phys2virt(pmerc_void_t rcvPtr);
extern void unixware_free_virt_addr(pmerc_void_t rcvPtr);
extern void unixware_timeout(pmerc_void_t rcvPtr);
extern void unixware_untimeout(pmerc_void_t rcvPtr );
extern void unixware_intr_register_intr_srvc(pmerc_void_t rcvPtr);
extern void unixware_intr_deregister_intr_srvc(pmerc_void_t rcvPtr);
extern void mercdintr(int InterruptVector);

#endif

#ifdef MERCD_PCI

#ifdef MERCD_OSSHL_UNIXWARE_PCI_C
void unixware_pci_drvr_identify(pmerc_void_t rcvPtr);
void unixware_pci_device_copy(pmerc_void_t rcvPtr);
void unixware_pci_probe_this_board(pmerc_void_t rcvPtr);
void unixware_pci_get_prop(pmerc_void_t rcvPtr);
void unixware_pci_intr_enable(pmerc_void_t rcvPtr);
void unixware_pci_intr_disable(pmerc_void_t rcvPtr);
void unixware_pci_map_adapter_parm(pmerc_void_t rcvPtr);
void unixware_pci_unmap_adapter_parm(pmerc_void_t rcvPtr);
void unixware_pci_verify_merc_adapter(pmerc_void_t rcvPtr);
void unixware_pci_save_attr(pmerc_void_t rcvPtr);
void unixware_pci_intr_check(pmerc_void_t rcvPtr);

#else

extern void unixware_pci_drvr_identify(pmerc_void_t rcvPtr);
extern void unixware_pci_device_copy(pmerc_void_t rcvPtr);
extern void unixware_pci_probe_this_board(pmerc_void_t rcvPtr);
extern void unixware_pci_get_prop(pmerc_void_t rcvPtr);
extern void unixware_pci_intr_enable(pmerc_void_t rcvPtr);
extern void unixware_pci_intr_disable(pmerc_void_t rcvPtr);
extern void unixware_pci_map_adapter_parm(pmerc_void_t rcvPtr);
extern void unixware_pci_unmap_adapter_parm(pmerc_void_t rcvPtr);
extern void unixware_pci_verify_merc_adapter(pmerc_void_t rcvPtr);
extern void unixware_pci_save_attr(pmerc_void_t rcvPtr);
extern void unixware_pci_intr_check(pmerc_void_t rcvPtr);

#endif

#endif /* MERCD_PCI */

#endif /* MERCD_UNIXWARE */


#ifdef MERCD_LINUX

#ifdef MERCD_PCI

#ifdef _MERCD_LINUX_OSAL_C
int initialize_linux_osal();
void mid_brdmgr_pam_brd_start(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
#else
extern int initialize_linux_osal(void);
extern void mid_brdmgr_pam_brd_start(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
#endif


#ifdef _MERCD_LINUX_PCI_DHAL_C
int initialize_linux_pci_dhal();
void mercd_return_success(pmerc_void_t rcvPtr);
#else
extern int initialize_linux_pci_dhal(void);
extern void mercd_return_success(pmerc_void_t rcvPtr);
#endif


#ifdef MERCD_OSSDL_LINUX_C

void linux_phys2virt(pmerc_void_t rcvPtr);
void linux_free_virt_addr(pmerc_void_t rcvPtr);
void linux_timeout(pmerc_void_t rcvPtr);
void linux_untimeout(pmerc_void_t rcvPtr );
void linux_intr_register_intr_srvc(pmerc_void_t rcvPtr);
void linux_intr_deregister_intr_srvc(pmerc_void_t rcvPtr);
irqreturn_t linux_intr_srvc(int intrVec, void *dev_id, struct pt_regs *regs);
void linux_ww_generate_soft_reset(pmercd_adapter_block_sT padapter);

#else

extern void linux_phys2virt(pmerc_void_t rcvPtr);
extern void linux_free_virt_addr(pmerc_void_t rcvPtr);
extern void linux_timeout(pmerc_void_t rcvPtr);
extern void linux_untimeout(pmerc_void_t rcvPtr );
extern void linux_intr_register_intr_srvc(pmerc_void_t rcvPtr);
extern void linux_intr_deregister_intr_srvc(pmerc_void_t rcvPtr);
extern irqreturn_t linux_intr_srvc(int intrVec, void *dev_id, struct pt_regs *regs);
extern void linux_ww_generate_soft_reset(pmercd_adapter_block_sT padapter);

#endif


#ifdef MERCD_OSSHL_LINUX_PCI_C


void linux_pci_drvr_identify(pmerc_void_t rcvPtr);
void linux_pci_device_copy(pmerc_void_t rcvPtr);
void linux_pci_probe_this_board(pmerc_void_t rcvPtr);
void linux_pci_brd_probe(void *rcvPtr);
void linux_pci_peek_char(pmerc_void_t rcvPtr);
void linux_pci_get_prop(pmerc_void_t rcvPtr);
void linux_pci_intr_enable(pmerc_void_t rcvPtr);
void linux_pci_intr_disable(pmerc_void_t rcvPtr);
void linux_pci_map_adapter_parm(pmerc_void_t rcvPtr);
void linux_pci_unmap_adapter_parm(pmerc_void_t rcvPtr);
void linux_pci_verify_merc_adapter(pmerc_void_t rcvPtr);
void linux_pci_fill_attr(pmerc_void_t rcvPtr);
void linux_pci_save_attr(pmerc_void_t rcvPtr);
void linux_pci_getinumber(pmerc_void_t rcvPtr);
void linux_pci_drvr_probe(pmerc_void_t rcvPtr);
void linux_pci_drvr_free(pmerc_void_t rcvPtr);
void linux_pci_peek_char(pmerc_void_t rcvPtr);
void linux_pci_go_hotswap(pmerc_void_t rcvPtr);
void linux_pci_intr_check(pmerc_void_t rcvPtr);
void linux_pci_dma_alloc_handle(void *rcvPtr);
void linux_pci_dma_free_handle(void *rcvPtr);
merc_ushort_t linux_pci_get_board_family(merc_ushort_t id);
//WW Support
void linux_pci_ww_mf_mem_alloc(pmerc_void_t rcvPtr);
void linux_pci_ww_mf_mem_dealloc(pmerc_void_t rcvPtr);
void linux_pci_ww_dealloc_descriptor_table(pmerc_void_t rcvPtr);
void linux_pci_ww_dealloc_rcv_bigmsg_descriptors(pmerc_void_t rcvPtr);
void linux_pci_ww_dealloc_snd_bigmsg_descriptors(pmerc_void_t rcvPtr);
void linux_pci_ww_recv_descriptor_table_from_dmaable_buffers(pmerc_void_t rcvPtr);
void linux_pci_ww_map_adapter_plxparm(pmerc_void_t rcvPtr);
void linux_pci_ww_intr_check(pmerc_void_t rcvPtr);
void linux_pci_ww_get_intr_request(void *rcvPtr);
void linux_pci_ww_clear_intr_request(void *rcvPtr);
void linux_pci_ww_recv_bigmsg_descriptor_table_from_strm_buffers(pmerc_void_t rcvPtr);
void linux_pci_ww_switch_adapter_mappings(pmerc_void_t rcvPtr);
void linux_pci_ww_unmap_adapter_parm(pmerc_void_t rcvPtr);
void linux_pci_ww_send_descriptor_table_from_dmaable_buffers(pmerc_void_t rcvPtr);
void linux_pci_ww_send_descriptor_table_from_strm_buffers(pmerc_void_t rcvPtr);
void linux_pci_ww_recv_replenish_descriptor_buffers(pmerc_void_t rcvPtr);
void linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers(pmerc_void_t rcvPtr);


#else

extern void linux_pci_drvr_identify(pmerc_void_t rcvPtr);
extern void linux_pci_device_copy(pmerc_void_t rcvPtr);
extern void linux_pci_probe_this_board(pmerc_void_t rcvPtr);
extern void linux_pci_brd_probe(void *rcvPtr);
extern void linux_pci_peek_char(pmerc_void_t rcvPtr);
extern void linux_pci_get_prop(pmerc_void_t rcvPtr);
extern void linux_pci_intr_enable(pmerc_void_t rcvPtr);
extern void linux_pci_intr_disable(pmerc_void_t rcvPtr);
extern void linux_pci_map_adapter_parm(pmerc_void_t rcvPtr);
extern void linux_pci_unmap_adapter_parm(pmerc_void_t rcvPtr);
extern void linux_pci_verify_merc_adapter(pmerc_void_t rcvPtr);
extern void linux_pci_fill_attr(pmerc_void_t rcvPtr);
extern void linux_pci_save_attr(pmerc_void_t rcvPtr);
extern void linux_pci_getinumber(pmerc_void_t rcvPtr);
extern void linux_pci_drvr_probe(pmerc_void_t rcvPtr);
extern void linux_pci_drvr_free(pmerc_void_t rcvPtr);
extern void linux_pci_peek_char(pmerc_void_t rcvPtr);
extern void linux_pci_go_hotswap(pmerc_void_t rcvPtr);

extern void linux_pci_intr_check(pmerc_void_t rcvPtr);
extern void linux_pci_dma_alloc_handle(void *rcvPtr);
extern void linux_pci_dma_free_handle(void *rcvPtr);
extern merc_ushort_t linux_pci_get_board_family(merc_ushort_t id);
//WW Support
extern void linux_pci_ww_mf_mem_alloc(pmerc_void_t rcvPtr);
extern void linux_pci_ww_mf_mem_dealloc(pmerc_void_t rcvPtr);
extern void linux_pci_ww_dealloc_descriptor_table(pmerc_void_t rcvPtr);
extern void linux_pci_ww_dealloc_rcv_bigmsg_descriptors(pmerc_void_t rcvPtr);
extern void linux_pci_ww_dealloc_snd_bigmsg_descriptors(pmerc_void_t rcvPtr);
extern void linux_pci_ww_recv_descriptor_table_from_dmaable_buffers(pmerc_void_t rcvPtr);
extern void linux_pci_ww_map_adapter_plxparm(pmerc_void_t rcvPtr);
extern void linux_pci_ww_intr_check(pmerc_void_t rcvPtr);
extern void linux_pci_ww_get_intr_request(void *rcvPtr);
extern void linux_pci_ww_clear_intr_request(void *rcvPtr);
extern void linux_pci_ww_recv_bigmsg_descriptor_table_from_strm_buffers(pmerc_void_t rcvPtr);
extern void linux_pci_ww_switch_adapter_mappings(pmerc_void_t rcvPtr);
extern void linux_pci_ww_unmap_adapter_parm(pmerc_void_t rcvPtr);
extern void linux_pci_ww_send_descriptor_table_from_dmaable_buffers(pmerc_void_t rcvPtr);
extern void linux_pci_ww_send_descriptor_table_from_strm_buffers(pmerc_void_t rcvPtr);
extern void linux_pci_ww_recv_replenish_descriptor_buffers(pmerc_void_t rcvPtr);
extern void linux_pci_ww_recv_stream_descriptor_table_from_strm_buffers(pmerc_void_t rcvPtr);

#endif

#endif /* MERCD_PCI */


#endif /* MERCD_LINUX */


#ifdef _MSDBNDMGR_C
void mid_bndmgr_bind(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_bndmgr_unbind(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_bndmgr_unbind_2(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_bndmgr_bind_exit_markup(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);

#else 
extern void mid_bndmgr_bind(pmercd_open_block_sT  mercd_openBlock,PSTRM_MSG Msg);
extern void mid_bndmgr_unbind(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_bndmgr_unbind_2(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_bndmgr_bind_exit_markup(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
#endif 

#ifdef _MSDBNDUTL_C
md_status_t bnd_dispatch_close_strm(PMSD_BIND_BLOCK BindBlock);
md_status_t bnd_snd_death_msg2brd(merc_uint_t Flags, pmerc_void_t BlockPtr);

#else
extern md_status_t bnd_dispatch_close_strm(PMSD_BIND_BLOCK BindBlock);
extern md_status_t bnd_snd_death_msg2brd(merc_uint_t Flags, pmerc_void_t BlockPtr);
#endif

#ifdef _MSDBRDMGR_C
void mid_brdmgr_cfg_scan_brd(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_get_phys_brd_attrib(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_get_brd_state(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_get_brd_diag_state(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_set_brd_state_dnld(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_map_brd(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_brd_config(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_pam_brd_start(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_brd_shutdown(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_get_config_rom(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_get_param(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_get_brd_config(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_save_brd_params(pmercd_adapter_block_sT padapter);
void mid_brdmgr_get_post_state(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);
void mid_brdmgr_set_post_state(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);
void mid_brdmgr_pam_brd_to_cfg(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);
void mid_brdmgr_cfg_get_sram(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);
void mid_brdmgr_get_power_status(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);

#else

extern void mid_brdmgr_cfg_scan_brd(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_get_phys_brd_attrib(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_get_brd_state(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_get_brd_diag_state(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_set_brd_state_dnld(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_map_brd(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_brd_config(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_pam_brd_start(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_brd_shutdown(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_get_config_rom(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_get_param(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_get_brd_config(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_save_brd_params(pmercd_adapter_block_sT padapter);
extern void mid_brdmgr_get_post_state(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_set_post_state(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_pam_brd_to_cfg(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_cfg_get_sram(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);
extern void mid_brdmgr_get_power_status(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg);

#endif

#ifdef _MSDBRDUTL_C
md_status_t check_brd_exist(pmercd_adapter_block_sT AdapterBlock, PMERC_BOARD_BLOCK  MercBoardAdapter);
md_status_t start_brd(PMSD_OPEN_BLOCK MsdOpenBlock, pmercd_adapter_block_sT AdapterBlock, PSTRM_MSG Msg);
void reset_adapter(PMERC_BOARD_BLOCK  MercBoardAdapter);
md_status_t shutdown_adapter(PSTRM_MSG Msg, pmercd_adapter_block_sT AdapterBlock);
void fill_attr(PCFG_GET_PHYS_BOARD_ATTR_ACK AckPtr);
md_status_t mid_sram_check_kernel_running( pmercd_adapter_block_sT padapter );

#else

extern md_status_t check_brd_exist(pmercd_adapter_block_sT AdapterBlock);
extern md_status_t start_brd(PMSD_OPEN_BLOCK MsdOpenBlock,pmercd_adapter_block_sT AdapterBlock,PSTRM_MSG Msg);
extern void reset_adapter(pmercd_adapter_block_sT  padapter);
extern md_status_t shutdown_adapter(PSTRM_MSG Msg, pmercd_adapter_block_sT AdapterBlock);
extern void fill_attr(PCFG_GET_PHYS_BOARD_ATTR_ACK AckPtr);
extern md_status_t mid_sram_check_kernel_running( pmercd_adapter_block_sT padapter );

#endif

#ifdef _MSDDBG_C_

void dbg_trace(size_t printLevel,
                      pmerc_char_t fmt,
                      size_t arg0,
                      size_t arg1,
                      size_t arg2,
                      size_t arg3,
                      size_t arg4,
                      size_t arg5,
                      size_t arg6,
                      size_t arg7,
                      size_t arg8,
                      size_t arg9);

void dbg_print(size_t printLevel,
                      pmerc_char_t fmt,
                      size_t arg0,
                      size_t arg1,
                      size_t arg2,
                      size_t arg3,
                      size_t arg4,
                      size_t arg5,
                      size_t arg6,
                      size_t arg7,
                      size_t arg8,
                      size_t arg9);


void dbg_error_print(pmerc_char_t fmt,
                            size_t arg0,
                            size_t arg1,
                            size_t arg2,
                            size_t arg3,
                            size_t arg4,
                            size_t arg5,
                            size_t arg6,
                            size_t arg7,
                            size_t arg8,
                            size_t arg9);

void dbg_panic_print(size_t brd, 
                            pmerc_char_t fmt,
                            size_t arg0,
                            size_t arg1,
                            size_t arg2 );

void dbg_warn_print(pmerc_char_t fmt,
                           size_t arg0,
                           size_t arg1,
                           size_t arg2, 
                           size_t arg3,
                           size_t arg4,
                           size_t arg5,
                           size_t arg6,
                           size_t arg7,
                           size_t arg8,
                           size_t arg9);


void dbg_level1_print(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

void dbg_level2_print(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

void dbg_level3_print(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

void dbg_always_print(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);


void dbg_level1_trace(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2,
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

void dbg_level2_trace(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2,
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

void dbg_level3_trace(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

void dbg_func_trace( pmerc_char_t funName,
                            size_t parameterCnt,
                            size_t arg0,
                            size_t arg1,
                            size_t arg2,
                            size_t arg3,
                            size_t arg4,
                            size_t arg5,
                            size_t arg6,
                            size_t arg7,
                            size_t arg8,
                            size_t arg9
                            );

/* New MSDDBG additions */
int is_endstring(char *str);
char * i_sprintf(char *s, char *fmt, ... );
int i_bprintf( unsigned char *abuf, unsigned int size);
char * i_vsprintf(char *s, char *fmt, va_list arg );
int convert_arg(char **src,char **dst, va_list arg);
void i_printmsg(char *fmt, ...);

#else

extern void dbg_trace(size_t printLevel, 
                      pmerc_char_t fmt,
                      size_t arg0,
                      size_t arg1,
                      size_t arg2,
                      size_t arg3,
                      size_t arg4,
                      size_t arg5,
                      size_t arg6,
                      size_t arg7,
                      size_t arg8,
                      size_t arg9);

extern void dbg_print(size_t printLevel,
                      pmerc_char_t fmt,
                      size_t arg0,
                      size_t arg1,
                      size_t arg2,
                      size_t arg3,
                      size_t arg4,
                      size_t arg5,
                      size_t arg6,
                      size_t arg7,
                      size_t arg8,
                      size_t arg9);


extern void dbg_error_print(pmerc_char_t fmt,
                            size_t arg0,
                            size_t arg1,
                            size_t arg2,
                            size_t arg3,
                            size_t arg4,
                            size_t arg5,
                            size_t arg6,
                            size_t arg7,
                            size_t arg8,
                            size_t arg9);

extern void dbg_panic_print(size_t brd, 
                            pmerc_char_t fmt,
                            size_t arg0,
                            size_t arg1,
                            size_t arg2 );

extern void dbg_warn_print(pmerc_char_t fmt,
                           size_t arg0,
                           size_t arg1,
                           size_t arg2, 
                           size_t arg3,
                           size_t arg4,
                           size_t arg5,
                           size_t arg6,
                           size_t arg7,
                           size_t arg8,
                           size_t arg9);


extern void dbg_level1_print(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

extern void dbg_level2_print(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

extern void dbg_level3_print(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

extern void dbg_always_print(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);


extern void dbg_level1_trace(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2,
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

extern void dbg_level2_trace(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2,
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

extern void dbg_level3_trace(pmerc_char_t fmt,
                             size_t arg0,
                             size_t arg1,
                             size_t arg2, 
                             size_t arg3,
                             size_t arg4,
                             size_t arg5,
                             size_t arg6,
                             size_t arg7,
                             size_t arg8,
                             size_t arg9);

extern void dbg_func_trace( pmerc_char_t funName,
                            size_t parameterCnt,
                            size_t arg0,
                            size_t arg1,
                            size_t arg2,
                            size_t arg3,
                            size_t arg4,
                            size_t arg5,
                            size_t arg6,
                            size_t arg7,
                            size_t arg8,
                            size_t arg9
                            );
/* New MSDDBG additions */
extern int is_endstring(char *str);
extern char * i_sprintf(char *s, char *fmt, ... );
#ifdef LFS
extern int i_bprintf( unsigned char * abuf, unsigned int size);
#else
extern i_bprintf( unsigned char * abuf, unsigned int size);
#endif
extern char * i_vsprintf(char *s, char *fmt, va_list arg );
extern int convert_arg(char **src,char **dst, va_list arg);
extern void i_printmsg(char *fmt, ...);


#endif // _MSDDBG_C_


#ifdef _MSDDPC_C
int mercd_generic_intr_processing(merc_ulong_t Context1);
void mercd_generic_timeout_processing(pmercd_adapter_block_sT AdapterBlock);
//WW support
void mercd_ww_generic_timeout_processing(pmercd_adapter_block_sT AdapterBlock);
#else
extern int mercd_generic_intr_processing(merc_ulong_t Context1);
extern void mercd_generic_timeout_processing(pmercd_adapter_block_sT AdapterBlock);
//WW support
extern void mercd_ww_generic_timeout_processing(pmercd_adapter_block_sT AdapterBlock);
#endif

#ifdef _MSDDRVRMGR_C
void mid_drvrmgr_cfg_drvr_start(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_drvrmgr_cfg_pam_start(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_drvrmgr_cfg_drvr_shutdown(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_drvrmgr_cfg_pam_shutdown(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_drvrmgr_pam_get_panic_dump(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_drvrmgr_enable_drv_trace(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_drvrmgr_disable_drv_trace(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_drvr_get_drv_trace(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_getset_boot_host_ram_bit(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_drvrmgr_get_statistics(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg);

void fillTraceFilters(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr);
merc_int_t isMessageToBeTracedOnMsgType(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction);
void fillTraceStreamIds(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr);
merc_int_t isMessageToBeTracedOnStreamId(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction);
void fillTraceCompDescs(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr);
merc_int_t isMessageToBeTracedOnStreamId(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction);
merc_uint_t GetStreamId(PMERC_HOSTIF_MSG hMsg);
merc_int_t isMessageToBeTracedOnCompDesc(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction);
void getNewCurTime(struct timeval *tv);

#else

extern void mid_drvrmgr_cfg_drvr_start(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_drvrmgr_cfg_pam_start(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_drvrmgr_cfg_drvr_shutdown(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_drvrmgr_cfg_pam_shutdown(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_drvrmgr_pam_get_panic_dump(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_drvrmgr_enable_drv_trace(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_drvrmgr_disable_drv_trace(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_drvr_get_drv_trace(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_getset_boot_host_ram_bit(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);

extern void mid_drvrmgr_get_statistics(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg);

extern void fillTraceFilters(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr);
extern merc_int_t isMessageToBeTracedOnMsgType(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction);
extern void fillTraceStreamIds(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr);
extern merc_int_t isMessageToBeTracedOnStreamId(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction);
extern void fillTraceCompDescs(pmercd_trace_info_sT trace, PCFG_ENABLE_DRV_TRACE ptr);
extern merc_int_t isMessageToBeTracedOnStreamId(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction);
extern merc_uint_t GetStreamId(PMERC_HOSTIF_MSG hMsg);
extern merc_int_t isMessageToBeTracedOnCompDesc(pmercd_adapter_block_sT padapter, PMERC_HOSTIF_MSG hMsg, merc_uint_t direction);
extern void getNewCurTime(struct timeval *tv);

#endif

#ifdef _MSDFWMGR_C

md_status_t fwmgr_reply_mgr(pmercd_adapter_block_sT AdapterBlock,PSTRM_MSG Msg);
md_status_t fwmgr_msg_reply_mgr(PSTRM_MSG Msg, pmercd_adapter_block_sT AdapterBlock);

#else

extern md_status_t fwmgr_reply_mgr(pmercd_adapter_block_sT AdapterBlock,PSTRM_MSG Msg);
extern md_status_t fwmgr_msg_reply_mgr(PSTRM_MSG Msg, pmercd_adapter_block_sT AdapterBlock);

#endif

#ifdef _MSDMSGMGR_C
void mid_msgmgr_pam_send_mercury(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
md_status_t strm_ww_Q_snd_msg(PSTRM_MSG Msg, pmercd_adapter_block_sT padapter);
merc_uint_t msgutl_ww_build_and_send_cancel_stream_mf(pmercd_bind_block_sT BindBlock);
int msgutl_ww_build_and_send_multi_eos_mf(pmercd_adapter_block_sT padapter);
merc_uint_t msgutl_ww_build_and_send_streams_flush_mf(pmercd_ww_flush_streamMF pflushMf);
merc_uint_t msgutl_ww_build_init_mf(pmercd_ww_build_initMF_sT pinitMfSt);
merc_uint_t msgutl_ww_build_and_send_cancel_bigmsg_mf(pmercd_ww_dev_info_sT pwwDev, pmercd_bind_block_sT BindBlock);
merc_uint_t msgutl_ww_build_and_send_recv_stream_datablk_mf(pmercd_ww_dev_info_sT pwwDev, pmercd_stream_connection_sT StreamBlock);
int msgutl_ww_msgblk_mf_full(merc_ulong_t mfsize, merc_ulong_t numMsgBlksPosted);
int msgutl_ww_cancel_msgblk_mf_full(merc_ulong_t mfsize, merc_ulong_t numMsgBlksPosted);
merc_uint_t msgutl_ww_determine_message_class(pmercd_ww_get_mfClass_sT pClassSt);
merc_uint_t msgutl_ww_build_smallmsg_mf(pmercd_ww_build_smallMF_sT psmallSt);
merc_uint_t msgutl_ww_build_bigmsg_mf(pmercd_ww_build_bigMF_sT pbigSt);
merc_uint_t msgutl_ww_build_write_stream_mf(pmercd_ww_send_streamData pStreamDataSt);
int msgutl_ww_build_and_send_eos_mf(pmercd_ww_build_eos_sT peosMfSt);
void strm_ww_put_eos_Q(pmercd_ww_dev_info_sT pwwDev, PSTRM_MSG Msg);
md_status_t strm_ww_get_mblk_train_info(pmercd_ww_mblk_train_info_sT pmblkTrain);

#else

extern void mid_msgmgr_pam_send_mercury(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern md_status_t strm_ww_Q_snd_msg(PSTRM_MSG Msg, pmercd_adapter_block_sT padapter);
extern merc_uint_t msgutl_ww_build_and_send_cancel_stream_mf(pmercd_bind_block_sT BindBlock);
extern int msgutl_ww_build_and_send_multi_eos_mf(pmercd_adapter_block_sT padapter);
extern merc_uint_t msgutl_ww_build_and_send_streams_flush_mf(pmercd_ww_flush_streamMF pflushMf);
extern merc_uint_t msgutl_ww_build_init_mf(pmercd_ww_build_initMF_sT pinitMfSt);
extern merc_uint_t msgutl_ww_build_and_send_cancel_bigmsg_mf(pmercd_ww_dev_info_sT pwwDev, pmercd_bind_block_sT BindBlock);
extern merc_uint_t msgutl_ww_build_and_send_recv_stream_datablk_mf(pmercd_ww_dev_info_sT pwwDev, pmercd_stream_connection_sT StreamBlock);
extern int msgutl_ww_msgblk_mf_full(merc_ulong_t mfsize, merc_ulong_t numMsgBlksPosted);
extern int msgutl_ww_cancel_msgblk_mf_full(merc_ulong_t mfsize, merc_ulong_t numMsgBlksPosted);
extern merc_uint_t msgutl_ww_determine_message_class(pmercd_ww_get_mfClass_sT pClassSt);
extern merc_uint_t msgutl_ww_build_smallmsg_mf(pmercd_ww_build_smallMF_sT psmallSt);
extern merc_uint_t msgutl_ww_build_bigmsg_mf(pmercd_ww_build_bigMF_sT pbigSt);
extern merc_uint_t msgutl_ww_build_write_stream_mf(pmercd_ww_send_streamData pStreamDataSt);
extern int msgutl_ww_build_and_send_eos_mf(pmercd_ww_build_eos_sT peosMfSt);
extern md_status_t strm_ww_get_mblk_train_info(pmercd_ww_mblk_train_info_sT pmblkTrain);
extern void strm_ww_put_eos_Q(pmercd_ww_dev_info_sT pwwDev, PSTRM_MSG Msg);

#endif


#ifdef _MSDQUEUE_C

void queue_put_Q(PMSD_QUEUE Queue, PMSD_GEN_MSG MsgPtr);
void queue_put_sec_Q(PMSD_QUEUE Queue, PMSD_GEN_TWIN_LINK_MSG MsgPtr);
md_status_t queue_remove_from_Q(PMSD_QUEUE Queue, PMSD_GEN_MSG MsgPtr);
PMSD_GEN_TWIN_LINK_MSG queue_get_sec_Q(PMSD_QUEUE Queue);
void queue_put_msg_Q(PMSD_QUEUE Queue, PSTRM_MSG MsgPtr);
PSTRM_MSG queue_get_msg_Q(PMSD_QUEUE Queue);
void queue_put_bk_msg_Q(PMSD_QUEUE Queue, PSTRM_MSG MsgPtr);
void queue_put_msg_Array(PMSD_ARRAY Array, PSTRM_MSG MsgPtr, int caller);
PSTRM_MSG queue_get_msg_Array(PMSD_ARRAY Array, int caller);
void queue_commit_msg_Array(PMSD_ARRAY Array, int caller);
void queue_put_bk_msg_Array(PMSD_ARRAY Array, PSTRM_MSG MsgPtr, int caller);

#else

extern void queue_put_Q(PMSD_QUEUE Queue, PMSD_GEN_MSG MsgPtr);
extern void queue_put_sec_Q(PMSD_QUEUE Queue, PMSD_GEN_TWIN_LINK_MSG MsgPtr);
extern md_status_t queue_remove_from_Q(PMSD_QUEUE Queue, PMSD_GEN_MSG MsgPtr);
extern PMSD_GEN_TWIN_LINK_MSG queue_get_sec_Q(PMSD_QUEUE Queue);
extern void queue_put_msg_Q(PMSD_QUEUE Queue, PSTRM_MSG MsgPtr);
extern PSTRM_MSG queue_get_msg_Q(PMSD_QUEUE Queue);
extern void queue_put_bk_msg_Q(PMSD_QUEUE Queue, PSTRM_MSG MsgPtr);
extern void queue_put_msg_Array(PMSD_ARRAY Array, PSTRM_MSG MsgPtr, int caller);
extern PSTRM_MSG queue_get_msg_Array(PMSD_ARRAY Array, int caller);
extern void queue_commit_msg_Array(PMSD_ARRAY Array, int caller);
extern void queue_put_bk_msg_Array(PMSD_ARRAY Array, PSTRM_MSG MsgPtr, int caller);

#endif

#ifdef _MSDRCV_C
int rcv_msgs_from_adapter(pmercd_adapter_block_sT AdapterBlock);
int rcv_data_from_adapter(pmercd_adapter_block_sT AdapterBlock, int i );
int postprocessDMA(pmercd_adapter_block_sT AdapterBlock);
md_status_t rcv_rceive_msg(pmercd_adapter_block_sT AdapterBlock,size_t MsgAddress);
int rcv_receive_data(pmercd_adapter_block_sT AdapterBlock,size_t MsgAddress);
int pci_rcv_process_dma(pmercd_adapter_block_sT AdapterBlock,size_t MsgAddress);
void msd_sram_sanity_dump(pmercd_adapter_block_sT padapter);

#else

extern int rcv_msgs_from_adapter(pmercd_adapter_block_sT AdapterBlock);
extern int rcv_data_from_adapter(pmercd_adapter_block_sT AdapterBlock, int i);
extern int postprocessDMA(pmercd_adapter_block_sT AdapterBlock);
extern md_status_t rcv_rceive_msg(pmercd_adapter_block_sT AdapterBlock,size_t MsgAddress);
extern int rcv_receive_data(pmercd_adapter_block_sT AdapterBlock,size_t MsgAddress);
extern int pci_rcv_process_dma(pmercd_adapter_block_sT AdapterBlock,size_t MsgAddress);
extern void msd_sram_sanity_dump(pmercd_adapter_block_sT padapter);

#endif

#ifdef _MSDSHRDATA_C_

#else

extern pmercd_control_block_sT MsdControlBlock;
extern MSD_QUEUE   MsdAdapterList;
extern mercd_mutex_T mercd_open_list_mutex;
extern mercd_mutex_T config_map_table_mutex;
extern MSD_QUEUE   MsdOpenList;
extern PMERC_DEFERRED_Q_BLOCK MercDeferredQBlock[MSD_MAX_ADAPTER_COUNT+1];
extern MSD_BIND_MAP_ENTRY      MsdBindMapTable[MSD_ABSOLUTE_MAX_BIND];
extern MSD_STREAM_MAP_ENTRY    MsdStreamMapTable[MSD_ABSOLUTE_MAX_STREAMS];
extern merc_uint_t             mercd_adapter_map[MSD_MAX_ADAPTER_COUNT];
extern merc_uint_t             mercd_adapter_log_to_phy_map_table[MSD_MAX_ADAPTER_COUNT];
extern merc_uint_t             MsdDbgBuf[2048];
extern merc_uint_t             dbg_traceBuf[2048];
extern MSD_DPC_DEBUG_BLOCK     MsdDpcDebug;
extern pmerc_void_t            MsdStatePtr;

extern PMSD_MUTEX_BLOCK        MsdMutexBlock;

extern merc_char_t             MsdDriverVersion[100];
extern int                     MsdProbePrint;
extern time_t                  MsdProfilerArray[];
extern merc_uint_t             DM3_OS_IDENTIFIED;
extern merc_uint_t             DM3_PLATFORM_IDENTIFIED;
extern MERCD_INTERFACE_FUNC    mercd_osal_mid_func[MAX_INTERFACE_FUNCTION];
extern MERCD_ABSTRACT_FUNC     mercd_osal_func[MAX_OSAL_FUNC];
extern MERCD_ABSTRACT_FUNC     mercd_dhal_func[MAX_OSAL_FUNC];
extern PMSD_MUTEX_STATS        pmutex_stats;

#endif // _MSDSHRDATA_C_

#ifdef _MSDSND_C
int snd_msgs2adapter(pmercd_adapter_block_sT AdapterBlock);
int snd_data2adapter(pmercd_adapter_block_sT AdapterBlock);
PMERC_DATA_BLK_HDR snd_alloc_Hif_data_blk(pmercd_adapter_block_sT AdapterBlock);
merc_void_t snd_release_Hif_data_blk( pmercd_adapter_block_sT padapter,
                                       PMERC_DATA_BLK_HDR DataBlockAddr);
//WW support
extern int snd_ww_msgs2adapter(pmercd_adapter_block_sT AdapterBlock, merc_uint_t sndlimit);

#else

extern int snd_msgs2adapter(pmercd_adapter_block_sT AdapterBlock);
extern int snd_data2adapter(pmercd_adapter_block_sT AdapterBlock);
extern PMERC_DATA_BLK_HDR snd_alloc_Hif_data_blk(pmercd_adapter_block_sT AdapterBlock);
extern merc_void_t snd_release_Hif_data_blk( pmercd_adapter_block_sT  AdapterBlock,
                                             PMERC_DATA_BLK_HDR DataBlockAddr);
//WW support
extern int snd_ww_msgs2adapter(pmercd_adapter_block_sT AdapterBlock, merc_uint_t sndlimit);
#endif

#ifdef _MSDSTRMMGR_C
void mid_strmmgr_send(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_strmmgr_mblk_send(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
int mid_strmmgr_mblk_split_send(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg, char *buffer, size_t size);
void mid_strmmgr_strm_open(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_strmmgr_strm_close(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_strmmgr_strm_flush(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_strmmgr_strm_close_internal(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_strmmgr_get_param(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_strmmgr_set_param(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
void mid_strmmgr_strm_terminate(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg);
unsigned int mid_strmmgr_strm_open_hcs(pmercd_adapter_block_sT padapter,
                               pmercd_stream_connection_sT StreamBlock, PSTRM_MSG NewMsg);
unsigned int mid_strmmgr_strm_close_hcs(pmercd_adapter_block_sT padapter,
                               pmercd_stream_connection_sT StreamBlock, PSTRM_MSG SendMsg);

#else

extern void mid_strmmgr_send(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_strmmgr_mblk_send(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern int mid_strmmgr_mblk_split_send(pmercd_open_block_sT MsdOpenBlock,PSTRM_MSG Msg, char *buffer, size_t size);
extern void mid_strmmgr_strm_open(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_strmmgr_strm_close(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_strmmgr_strm_flush(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_strmmgr_strm_close_internal(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_strmmgr_get_param(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_strmmgr_set_param(pmercd_open_block_sT mercd_openBlock,PSTRM_MSG Msg);
extern void mid_strmmgr_strm_terminate(pmercd_open_block_sT MsdOpenBlock, PSTRM_MSG Msg);
extern unsigned int mid_strmmgr_strm_open_hcs(pmercd_adapter_block_sT padapter,
                               pmercd_stream_connection_sT StreamBlock, PSTRM_MSG NewMsg);
extern unsigned int mid_strmmgr_strm_close_hcs(pmercd_adapter_block_sT padapter,
                               pmercd_stream_connection_sT StreamBlock, PSTRM_MSG SendMsg);

#endif

#ifdef _MSDSTRMUTL_C
MSD_HANDLE strm_fnd_free_connect_entry(pmercd_adapter_block_sT);
pmercd_stream_connection_sT strm_fnd_entry_from_adapter(pmercd_adapter_block_sT AdapterBlock,
			                                merc_uint_t TransactionId);

pmercd_stream_connection_sT search_streamblock( pmercd_open_block_sT MsdOpenBlock, 
                                                MSD_HANDLE StreamHandle);

pmercd_stream_connection_sT search_streamblock_2( pmercd_adapter_block_sT padapter, 
                                                  MSD_HANDLE StreamHandle);

void strm_snd_can_takes(pmercd_adapter_block_sT padapter);
md_status_t strm_sndbrokenstream2usr(pmercd_stream_connection_sT StreamBlock);
void push_trace_msg(pmercd_adapter_block_sT padapter, pmercd_bind_block_sT BindBlock, merc_uint_t MsgType);

#ifdef _8_BIT_INSTANCE
pmercd_bind_block_sT search_bindblock( pmercd_open_block_sT MsdOpenBlock, 
                                       MSD_HANDLE BindHandle);
#else
pmercd_bind_block_sT search_bindblock( pmercd_open_block_sT MsdOpenBlock,
                                       MBD_HANDLE BindHandle);
#endif

void strm_gather_snd_blks(pmercd_adapter_block_sT AdapterBlock);
void strm_adv_gather_snd_blks(pmercd_adapter_block_sT AdapterBlock,
			pmercd_stream_connection_sT StreamBlock);
md_status_t strm_ack2usr(pmercd_stream_connection_sT StreamBlock,
			merc_uint_t BytesSent);
merc_uint_t strm_cal_snd_cnt(pmercd_stream_connection_sT StreamBlockQueue);
void strm_put_data_strm_Q(pmercd_stream_connection_sT StreamBlock,PSTRM_MSG Msg);
void strm_free_only_data_msgs(pmercd_stream_connection_sT StreamBlock);
void strm_free_srm_msgs(pmercd_adapter_block_sT AdapterBlock,
			pmercd_stream_connection_sT StreamBlock);
md_status_t strm_Q_snd_msg(PSTRM_MSG Msg,pmercd_adapter_block_sT AdapterBlock);
PSTRM_MSG strm_fnd_msg_lst_tail(PSTRM_MSG List);
void strm_snd_can_takes(pmercd_adapter_block_sT padapter);
void  stream_add_cantake_queue (pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock);

#else

extern MSD_HANDLE strm_fnd_free_connect_entry(pmercd_adapter_block_sT);
extern pmercd_stream_connection_sT strm_fnd_entry_from_adapter(
                        pmercd_adapter_block_sT AdapterBlock,
                        merc_uint_t  TransactionId);
pmercd_stream_connection_sT search_streamblock(pmercd_open_block_sT , MSD_HANDLE  );


extern void strm_snd_can_takes(pmercd_adapter_block_sT padapter);
extern md_status_t strm_sndbrokenstream2usr(pmercd_stream_connection_sT StreamBlock);
extern void push_trace_msg(pmercd_adapter_block_sT padapter, pmercd_bind_block_sT BindBlock, merc_uint_t MsgType);

extern pmercd_stream_connection_sT search_streamblock_2(pmercd_adapter_block_sT , MSD_HANDLE  );
#ifdef _8_BIT_INSTANCE
extern pmercd_bind_block_sT search_bindblock(pmercd_open_block_sT , MSD_HANDLE );
#else
extern pmercd_bind_block_sT search_bindblock(pmercd_open_block_sT , MBD_HANDLE );
#endif

extern void strm_gather_snd_blks(pmercd_adapter_block_sT AdapterBlock);
extern void strm_adv_gather_snd_blks(pmercd_adapter_block_sT AdapterBlock,
			pmercd_stream_connection_sT StreamBlock);
extern md_status_t strm_ack2usr( pmercd_stream_connection_sT StreamBlock,
			       merc_uint_t BytesSent);
extern merc_uint_t strm_cal_snd_cnt(pmercd_stream_connection_sT StreamBlockQueue);
extern void strm_put_data_strm_Q(pmercd_stream_connection_sT StreamBlock,PSTRM_MSG Msg);
extern void strm_free_only_data_msgs(pmercd_stream_connection_sT StreamBlock);
extern void strm_free_srm_msgs(pmercd_adapter_block_sT AdapterBlock,
			pmercd_stream_connection_sT StreamBlock);
extern md_status_t strm_Q_snd_msg(PSTRM_MSG Msg,pmercd_adapter_block_sT AdapterBlock);
extern PSTRM_MSG strm_fnd_msg_lst_tail(PSTRM_MSG List);
extern void strm_snd_can_tanes(pmercd_adapter_block_sT padapter);
extern void  stream_add_cantake_queue (pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock);
#endif

#ifdef _MSDSUPP_C

PSTRM_MSG supp_alloc_buf(size_t Size, int type);
md_status_t supp_wait4Buf(merc_uint_t Size, merc_int_t (*Func)(pmerc_void_t), size_t Arg);
merc_uint_t supp_rnd2nxt(merc_uint_t Size, merc_uint_t N);
md_status_t supp_dispatch_up_Q(pmercd_adapter_block_sT AdapterBlock,PSTRM_MSG Msg);
void supp_cfg_ack(pmercd_adapter_block_sT AdapterBlock);
md_status_t supp_snd_cfg_rply(pmercd_adapter_block_sT AdapterBlock, merc_uint_t ReplyCode);
merc_int_t supp_cfg_map_number(merc_uint_t BoardNumber);
PSTRM_MSG
supp_drvr_mk_ack( PSTRM_MSG Msg, 
                  merc_uint_t MsgId, 
                  merc_uint_t ErrorCode, 
                  merc_uint_t ReturnValue);

md_status_t supp_do_unbind(PMSD_BIND_BLOCK BindBlock);
merc_uint_t supp_sram_start_offset(merc_int_t ConfigId);

#ifdef _8_BIT_INSTANCE
MSD_HANDLE supp_fnd_free_bnd_entry();
#else
#ifdef LFS
MBD_HANDLE supp_fnd_free_bnd_entry(void);
#else
MBD_HANDLE supp_fnd_free_bnd_entry();
#endif
#endif

md_status_t supp_read_brd_panic_buf( pmercd_adapter_block_sT AdapterBlock,
                                   pmerc_uchar_t PanicBuffer);
MD_PRIVATE void supp_flush_stream_Q(queue_t *q, mblk_t *mp);
void supp_process_receive(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void supp_process_sync_receive(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
int supp_ver2string (char *pInString, int option, char *pOutString);
void supp_push_trace_msg( pmercd_adapter_block_sT AdapterBlock, 
                          merc_uint_t Flag, 
                          PMERC_HOSTIF_MSG fromPtr);

PSTRM_MSG  MakeTraceMsg(pmercd_adapter_block_sT padapter, merc_uint_t thresh);
void  SendTraceToApp(pmercd_adapter_block_sT padapter, merc_uint_t thresh);


#ifdef LFS
void streamscall supp_esb_free(caddr_t pesbuffer);
#else
void supp_esb_free(pmerc_uchar_t pesbuffer);
#endif
#else

extern PSTRM_MSG supp_alloc_buf(size_t Size, int type);
extern md_status_t supp_wait4Buf( merc_uint_t Size, merc_int_t (*Func)(pmerc_void_t), size_t Arg);
extern merc_uint_t supp_rnd2nxt(merc_uint_t Size, merc_uint_t N);
extern md_status_t supp_dispatch_up_Q(pmercd_adapter_block_sT AdapterBlock,PSTRM_MSG Msg);
extern void supp_cfg_ack(pmercd_adapter_block_sT AdapterBlock);
extern md_status_t supp_snd_cfg_rply(pmercd_adapter_block_sT AdapterBlock, merc_uint_t ReplyCode);
extern merc_int_t supp_cfg_map_number(merc_uint_t BoardNumber);
extern PSTRM_MSG supp_drvr_mk_ack( PSTRM_MSG Msg,
                                   merc_uint_t MsgId, 
                                   merc_uint_t ErrorCode,
                                   merc_uint_t ReturnValue);

extern md_status_t supp_do_unbind(PMSD_BIND_BLOCK BindBlock);
extern merc_uint_t supp_sram_start_offset(merc_int_t ConfigId);

#ifdef _8_BIT_INSTANCE
extern MSD_HANDLE  supp_fnd_free_bnd_entry(void);
#else
extern MBD_HANDLE  supp_fnd_free_bnd_entry(void);
#endif

extern md_status_t supp_read_brd_panic_buf( pmercd_adapter_block_sT AdapterBlock,
                                          pmerc_uchar_t PanicBuffer);
extern MD_PRIVATE void supp_flush_stream_Q(queue_t *q, mblk_t *mp);
extern void supp_process_receive(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void supp_process_sync_receive(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern int supp_ver2string(char *pInString, int option, char *pOutString);
extern void supp_push_trace_msg( pmercd_adapter_block_sT AdapterBlock, 
                                 merc_uint_t Flag, 
                                 PMERC_HOSTIF_MSG fromPtr);
extern PSTRM_MSG  MakeTraceMsg(pmercd_adapter_block_sT padapter, merc_uint_t thresh);
extern void  SendTraceToApp(pmercd_adapter_block_sT padapter, merc_uint_t thresh);


#ifdef LFS
extern void streamscall supp_esb_free(caddr_t pesbuffer);
#else
extern void supp_esb_free(pmerc_uchar_t pesbuffer);
#endif
#endif

#ifdef _MSDSUPPNATIVE_C
int mercd_readQueue(pmercd_open_block_sT MsdOpenBlock, size_t size, char *buffer, int dontBlock);
int mercd_copyfromBuffers(PMSD_QUEUE Queue, char *buffer,size_t size);
#else
extern int mercd_readQueue(pmercd_open_block_sT MsdOpenBlock, size_t size, char *buffer, int dontBlock);
extern int mercd_copyfromBuffers(PMSD_QUEUE Queue, char *buffer, size_t size);
#endif

#ifdef _MSDNATIVEALLOC_
#ifdef LFS
void    *native_malloc(int nbytes, int class, char *file_name, int line_nr);
void    native_free(void *ptr, char *file_name, int line_nr);
void    *native_alloc_atomic_asi(int nbytes, char *file, int line);
void    *native_alloc_kernel_asi(int nbytes, char *file, int line);
void    *native_alloc_dma_asi(int nbytes, char *file, int line);
void    *native_free_mem_asi(void *mem_area, char *file, int line);
mblk_t  *native_allocb(int size, int type);
int    native_freeb(mblk_t *mp);
int    native_freemsg(mblk_t *mp);
mblk_t *native_alloc_msg_desr(int size, char *Buffer);
void    native_linkb(mblk_t *mp1, mblk_t *mp2);
mblk_t *native_unlinkb(mblk_t *mp);
mblk_t *native_dupb(mblk_t *mp);
void    native_udelay(long micro_secs);
int qDrvMsgPoolAllocate(int count);
int qDrvMsgPoolFree(void);
void qDrvPrintMsgPoolStatus(void);
mblk_t *qMsgPoolAllocate(void);
int qMsgPoolFree(mblk_t *mp1);
void native_cmn_err(int err_lvl, char *fmt, ...);
#else
void    *native_malloc(int nbytes, int class, char *file_name, int line_nr);
void    native_free(void *ptr, char *file_name, int line_nr);
void    *native_alloc_atomic_asi(int nbytes, char *file, int line);
void    *native_alloc_kernel_asi(int nbytes, char *file, int line);
void    *native_alloc_dma_asi(int nbytes, char *file, int line);
void    *native_free_mem_asi(void *mem_area, char *file, int line);
msgb_t  *native_allocb(int size, int type);
int    native_freeb(mblk_t *mp);
int    native_freemsg(mblk_t *mp);
msgb_t *native_alloc_msg_desr(int size, char *Buffer);
void    native_linkb(msgb_t *mp1, msgb_t *mp2);
msgb_t *native_unlinkb(msgb_t *mp);
msgb_t *native_dupb(msgb_t *mp);
void    native_udelay(long micro_secs);
int qDrvMsgPoolAllocate(int count);
int qDrvMsgPoolFree(void);
void qDrvPrintMsgPoolStatus();
msgb_t *qMsgPoolAllocate();
int qMsgPoolFree(msgb_t *mp1);
void native_cmn_err(int err_lvl, char *fmt, ...);
#endif
#else
#ifdef LFS
extern void    *native_malloc(int nbytes, int class, char *file_name, int line_nr);
extern void    native_free(void *ptr, char *file_name, int line_nr);
extern void    *native_alloc_atomic_asi(int nbytes, char *file, int line);
extern void    *native_alloc_kernel_asi(int nbytes, char *file, int line);
extern void    *native_alloc_dma_asi(int nbytes, char *file, int line);
extern void    *native_free_mem_asi(void *mem_area, char *file, int line);
extern mblk_t  *native_allocb(int size, int type);
extern int    native_freeb(mblk_t *mp);
extern int    native_freemsg(mblk_t *mp);
extern mblk_t *native_alloc_msg_desr(int size, char *Buffer);
extern void    native_linkb(mblk_t *mp1, mblk_t *mp2);
extern mblk_t *native_unlinkb(mblk_t *mp);
extern mblk_t *native_dupb(mblk_t *mp);
extern void    native_udelay(long micro_secs);
extern int qDrvMsgPoolAllocate(int count);
extern int qDrvMsgPoolFree(void);
extern void qDrvPrintMsgPoolStatus(void);
extern mblk_t *qMsgPoolAllocate(void);
extern int qMsgPoolFree(mblk_t *mp1);
extern void native_cmn_err(int err_lvl, char *fmt, ...);
#else
extern void    *native_malloc(int nbytes, int class, char *file_name, int line_nr);
extern void    native_free(void *ptr, char *file_name, int line_nr);
extern void    *native_alloc_atomic_asi(int nbytes, char *file, int line);
extern void    *native_alloc_kernel_asi(int nbytes, char *file, int line);
extern void    *native_alloc_dma_asi(int nbytes, char *file, int line);
extern void    *native_free_mem_asi(void *mem_area, char *file, int line);
extern msgb_t  *native_allocb(int size, int type);
extern int    native_freeb(mblk_t *mp);
extern int    native_freemsg(mblk_t *mp);
extern msgb_t *native_alloc_msg_desr(int size, char *Buffer);
extern void    native_linkb(msgb_t *mp1, msgb_t *mp2);
extern msgb_t *native_unlinkb(msgb_t *mp);
extern msgb_t *native_dupb(msgb_t *mp);
extern void    native_udelay(long micro_secs);
extern int qDrvMsgPoolAllocate(int count);
extern int qDrvMsgPoolFree(void);
extern void qDrvPrintMsgPoolStatus();
extern msgb_t *qMsgPoolAllocate();
extern int qMsgPoolFree(msgb_t *mp1);
extern void native_cmn_err(int err_lvl, char *fmt, ...);
#endif
#endif

#ifdef _MSDTIME_C

md_status_t time_set_host_ram_sem_timer(pmercd_adapter_block_sT AdapterBlock);
void time_host_ram_timeout(pmercd_adapter_block_sT AdapterBlock);
md_status_t time_chk_snd_timer(pmercd_adapter_block_sT AdapterBlock);
md_status_t time_ww_chk_snd_timer(pmercd_adapter_block_sT padapter);

#else

extern md_status_t time_set_host_ram_sem_timer(pmercd_adapter_block_sT AdapterBlock);
extern void time_host_ram_timeout(pmercd_adapter_block_sT AdapterBlock);
extern md_status_t time_chk_snd_timer(pmercd_adapter_block_sT AdapterBlock);
extern md_status_t time_ww_chk_snd_timer(pmercd_adapter_block_sT padapter);

#endif

#ifdef _MERCD_LINUX_DMA_PCI_C
void linux_pci_prepare_dma(void *rcvPtr);
#else
extern void linux_pci_prepare_dma(void *rcvPtr);
#endif

//WW support
#ifdef _MSDWWMGR_C
void mid_wwmgr_set_param(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_wwmgr_get_param(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_wwmgr_set_mode(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_wwmgr_set_mode_sram_family(pmercd_open_block_sT mercd_openBlock, pmercd_adapter_block_sT AdapterBlock, PSTRM_MSG Msg);
void mid_wwmgr_set_mode_3rdrock_family(pmercd_open_block_sT mercd_openBlock, pmercd_adapter_block_sT AdapterBlock, PSTRM_MSG Msg);
void mid_wwmgr_get_mode_operational(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_wwmgr_get_mode(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_wwmgr_start_streams(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
void mid_wwmgr_debug_streams(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
int mid_wwmgr_process_error_interrupt_from_board(pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_allocate_mem_for_rcvdatablks_index_table(pmercd_stream_connection_sT StreamBlock);
int mid_wwmgr_allocate_mem_for_rcvdatablks(pmercd_stream_connection_sT StreamBlock, pmercd_adapter_block_sT padapter);
#else
extern void mid_wwmgr_set_param(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_wwmgr_get_param(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_wwmgr_set_mode(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_wwmgr_set_mode_sram_family(pmercd_open_block_sT mercd_openBlock, pmercd_adapter_block_sT AdapterBlock, PSTRM_MSG Msg);
extern void mid_wwmgr_set_mode_3rdrock_family(pmercd_open_block_sT mercd_openBlock, pmercd_adapter_block_sT AdapterBlock, PSTRM_MSG Msg);
extern void mid_wwmgr_get_mode_operational(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_wwmgr_get_mode(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_wwmgr_start_streams(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern void mid_wwmgr_debug_streams(pmercd_open_block_sT mercd_openBlock, PSTRM_MSG Msg);
extern int  mid_wwmgr_process_error_interrupt_from_board(pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_allocate_mem_for_rcvdatablks_index_table(pmercd_stream_connection_sT StreamBlock);
extern int mid_wwmgr_allocate_mem_for_rcvdatablks(pmercd_stream_connection_sT StreamBlock, pmercd_adapter_block_sT padapter);
#endif

#ifdef MERCD_WW_OSSDL_LINUX_C
irqreturn_t linux_ww_intr_srvc(int InterruptVector, void *dev_id, struct pt_regs *regs);
#else
extern irqreturn_t linux_ww_intr_srvc(int InterruptVector, void *dev_id, struct pt_regs *regs);
#endif

#ifdef _MSWWDDPC_C
int mercd_ww_generic_intr_processing(merc_ulong_t Context1);
#else
extern int mercd_ww_generic_intr_processing(merc_ulong_t Context1);
#endif

#ifdef _MSDWWMSGUTL_C
merc_uint_t msgutl_ww_build_and_send_msgblk_mf(pmercd_ww_dev_info_sT  pwwDev);
#else
extern merc_uint_t msgutl_ww_build_and_send_msgblk_mf(pmercd_ww_dev_info_sT  pwwDev);
#endif

#ifdef _MSDWWSND_C_
int snd_ww_msgs2adapter(pmercd_adapter_block_sT padapter, merc_uint_t sndlimit);
int snd_ww_small_msgs2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg);
int snd_ww_big_msgs2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg);
int snd_ww_data2adapter(pmercd_adapter_block_sT padapter);
int snd_ww_strm_data2adapter(pmercd_ww_send_streamData pStreamDataSt);
int snd_ww_eos2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg, merc_uchar_t wwqueued);
#else
extern int snd_ww_msgs2adapter(pmercd_adapter_block_sT padapter, merc_uint_t sndlimit);
extern int snd_ww_small_msgs2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg);
extern int snd_ww_big_msgs2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg);
extern int snd_ww_strm_data2adapter(pmercd_ww_send_streamData pStreamDataSt);
extern int snd_ww_eos2adapter(pmercd_adapter_block_sT padapter, PSTRM_MSG Msg, merc_uchar_t wwqueued);
extern int snd_ww_data2adapter(pmercd_adapter_block_sT padapter);
#endif // _MSDWWSND_C_

#ifdef _MSDWWRCV_C_
int rcv_ww_process_stream_session_close(pmercd_ww_get_mfAddress_sT pmfAddrSt);
int rcv_ww_prepare_mblk_train_to_sendup(pmercd_ww_prep_mblk_train_sT pmbTrainSt);
int rcv_ww_prepare_mblk_strm_train_to_sendup(pmercd_ww_prep_mblk_train_sT pmbTrainSt);
int rcv_ww_process_eos_read_stream_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_prepare_mf_chain_to_sendup(pmercd_ww_prep_mf_train_sT pmfTrainSt);
unsigned int rcv_ww_dispatch_mblk_train_up(pmercd_ww_dispatch_rcvd_msg_sT  pdispMsg);
unsigned int rcv_ww_process_small_msgs_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_big_msg_read_ack_msg_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_big_xmsg_read_ack_msg_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_big_msgs_from_adapter (pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_big_msg_in_mfs(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_write_request_complete_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_big_msg_cancel_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_stream_reqeust_cancel_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_stream_flush_request_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_data_buffer_request_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int rcv_ww_process_read_request_complete_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
unsigned int msdwwrcv_ww_msgready_intr_processing(pmercd_ww_dev_info_sT pwwDev);
#else
extern int rcv_ww_process_stream_session_close(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern int rcv_ww_prepare_mblk_train_to_sendup(pmercd_ww_prep_mblk_train_sT pmbTrainSt);
extern int rcv_ww_prepare_mblk_strm_train_to_sendup(pmercd_ww_prep_mblk_train_sT pmbTrainSt);
extern int rcv_ww_process_eos_read_stream_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_prepare_mf_chain_to_sendup(pmercd_ww_prep_mf_train_sT pmfTrainSt);
extern unsigned int rcv_ww_dispatch_mblk_train_up(pmercd_ww_dispatch_rcvd_msg_sT  pdispMsg);
extern unsigned int rcv_ww_process_small_msgs_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_big_msg_read_ack_msg_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_big_xmsg_read_ack_msg_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_big_msgs_from_adapter (pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_big_msg_in_mfs(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_write_request_complete_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_big_msg_cancel_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_stream_reqeust_cancel_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_stream_flush_request_ack_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_data_buffer_request_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int rcv_ww_process_read_request_complete_from_adapter(pmercd_ww_get_mfAddress_sT pmfAddrSt);
extern unsigned int msdwwrcv_ww_msgready_intr_processing(pmercd_ww_dev_info_sT pwwDev);
#endif // _MSDWWRCV_C_

#ifdef _MSDWWUTL_C_
int msdwwutl_ww_read_free_inboundQ_for_mf_address(pmercd_ww_get_mfAddress_sT pmfAddressST);
int msdwwutl_ww_read_posted_outboundQ_for_mf_address(pmercd_ww_get_mfAddress_sT pmfAddressST);
merc_uint_t msgutl_ww_get_nextfree_dmadescr(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
merc_uint_t msgutl_ww_get_numofbits_in_current_mask(merc_uint_t bitpos, merc_uint_t bmapsz);
merc_uint_t msgutl_ww_mark_dmadescr_free(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
merc_uint_t msgutl_ww_mark_dmadescr_used(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
merc_uint_t msgutl_ww_check_if_dmadescr_used(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
int mid_wwmgr_check_3rdrock_running(pmercd_adapter_block_sT padapter, merc_uint_t KernelType);
int mid_wwmgr_process_cancel_request_intr_ack_from_board(pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_shutdown_adapter(pmercd_ww_dev_info_sT pwwDev, merc_uint_t initiator);
int mid_wwmgr_read_ww_postlocation(pmercd_adapter_block_sT padapter);
int mid_wwmgr_clear_3rdrock_postlocation(pmercd_adapter_block_sT padapter);
void mid_wwmgr_configure_brd_to_ww(PMSD_OPEN_BLOCK MsdOpenBlock, pmercd_adapter_block_sT padapter, PSTRM_MSG Msg);
int mid_wwmgr_close_streams (pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock, merc_uint_t Normal);
merc_uint_t msgutl_ww_mark_dmadescr_used(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
int mid_wwmgr_process_abnormal_termination ( pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_send_pending_cancel_bigmsg_mfs( pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_send_pending_cancel_stream_mfs( pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_send_pending_flush_stream_mfs( pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_send_pending_recv_streams_databuf_mfs( pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_post_init_intr_processing(pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_post_init_msg_ready_intr_processing(pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_process_reset_interrupt_from_board(pmercd_ww_dev_info_sT pwwDev);
void mid_wwmgr_dump_streamblock_info(pmercd_stream_connection_sT StreamBlock);
int mid_wwmgr_alloc_streams_descriptors ( pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock);
int mid_wwmgr_send_rcvstrm_datablocks_to_board(pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock);
int mid_wwmgr_send_setwwmode_ack_to_admin(pmercd_ww_dev_info_sT pwwDev, merc_uint_t ErrorCode);
int mid_wwmgr_allocate_mem_for_mf(pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_allocate_mem_for_bigmsgblks(pmercd_ww_dev_info_sT pwwDev);
int mid_wwmgr_generate_doorbell_to_board(pmercd_ww_gen_doorbell_sT doorbell);
int mid_wwmgr_read_3rdrock_postlocation(pmercd_adapter_block_sT padapter, merc_uint_t KernelType);
int mid_wwmgr_send_init_mf_to_board(pmercd_ww_dev_info_sT pwwDev);
merc_uint_t msgutl_ww_mark_dmadescr_free(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
merc_uint_t msgutl_ww_check_if_dmadescr_used(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
int msdwwutl_ww_read_free_inboundQ_for_mf_address(pmercd_ww_get_mfAddress_sT pmfAddressST);
merc_uint_t msgutl_ww_get_nextfree_dmadescr(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
int msdwwutl_ww_read_posted_outboundQ_for_mf_address(pmercd_ww_get_mfAddress_sT pmfAddressST);
merc_uint_t msgutl_ww_check_free_dmadescr(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);

#else
extern int mid_wwmgr_check_3rdrock_running(pmercd_adapter_block_sT padapter, merc_uint_t KernelType);
extern int mid_wwmgr_process_cancel_request_intr_ack_from_board(pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_shutdown_adapter(pmercd_ww_dev_info_sT pwwDev, merc_uint_t initiator);
extern int mid_wwmgr_read_ww_postlocation(pmercd_adapter_block_sT padapter);
extern int mid_wwmgr_clear_3rdrock_postlocation(pmercd_adapter_block_sT padapter);
extern void mid_wwmgr_configure_brd_to_ww(PMSD_OPEN_BLOCK MsdOpenBlock, pmercd_adapter_block_sT padapter, PSTRM_MSG Msg);
extern int mid_wwmgr_close_streams (pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock, merc_uint_t Normal);
extern merc_uint_t msgutl_ww_mark_dmadescr_used(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
extern int mid_wwmgr_process_abnormal_termination ( pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_send_pending_cancel_bigmsg_mfs( pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_send_pending_cancel_stream_mfs( pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_send_pending_flush_stream_mfs( pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_send_pending_recv_streams_databuf_mfs( pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_post_init_intr_processing(pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_post_init_msg_ready_intr_processing(pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_process_reset_interrupt_from_board(pmercd_ww_dev_info_sT pwwDev);
extern void mid_wwmgr_dump_streamblock_info(pmercd_stream_connection_sT StreamBlock);
extern int mid_wwmgr_alloc_streams_descriptors ( pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock);
extern int mid_wwmgr_send_rcvstrm_datablocks_to_board(pmercd_adapter_block_sT padapter, pmercd_stream_connection_sT StreamBlock);
extern int mid_wwmgr_send_setwwmode_ack_to_admin(pmercd_ww_dev_info_sT pwwDev, merc_uint_t ErrorCode);
extern int mid_wwmgr_allocate_mem_for_mf(pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_allocate_mem_for_bigmsgblks(pmercd_ww_dev_info_sT pwwDev);
extern int mid_wwmgr_generate_doorbell_to_board(pmercd_ww_gen_doorbell_sT doorbell);
extern int mid_wwmgr_read_3rdrock_postlocation(pmercd_adapter_block_sT padapter, merc_uint_t KernelType);
extern int mid_wwmgr_send_init_mf_to_board(pmercd_ww_dev_info_sT pwwDev);
extern merc_uint_t msgutl_ww_mark_dmadescr_free(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
extern merc_uint_t msgutl_ww_check_if_dmadescr_used(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
extern int msdwwutl_ww_read_free_inboundQ_for_mf_address(pmercd_ww_get_mfAddress_sT pmfAddressST);
extern merc_uint_t msgutl_ww_get_nextfree_dmadescr(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
extern int msdwwutl_ww_read_posted_outboundQ_for_mf_address(pmercd_ww_get_mfAddress_sT pmfAddressST);
extern merc_uint_t msgutl_ww_check_free_dmadescr(pmercd_ww_dmaDscr_Index_sT pdmaDscrSt);
#endif // _MSDWWUTL_C_

#endif // _MSDEXTERN_
