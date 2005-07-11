# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2005/07/11 13:31:53 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"), it
# is classified as "Commercial Computer Software" under paragraph 252.227-7014
# of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
# successor regulations) and the Government is acquiring only the license rights
# granted herein (the license rights customarily provided to non-Government
# users).  If the Software is supplied to any unit or agency of the Government
# other than DoD, it is classified as "Restricted Computer Software" and the
# Government's rights in the Software are defined in paragraph 52.227-19 of the
# Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2005/07/11 13:31:53 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

svr3_compat_pages = \
	man9/svr3compat.9 \
	man9/emajor.9 \
	man9/eminor.9

svr4_compat_pages = \
	man9/svr4compat.9 \
	man9/dtimeout.9 \
	man9/etoimajor.9 \
	man9/getemajor.9 \
	man9/geteminor.9 \
	man9/itimeout.9 \
	man9/itoemajor.9 \
	man9/LOCK.9 \
	man9/LOCK_ALLOC.9 \
	man9/LOCK_DEALLOC.9 \
	man9/LOCK_OWNED.9 \
	man9/MPSTR_QLOCK.9 \
	man9/MPSTR_QRELE.9 \
	man9/MPSTR_STPLOCK.9 \
	man9/MPSTR_STPRELE.9 \
	man9/RW_ALLOC.9 \
	man9/RW_DEALLOC.9 \
	man9/RW_RDLOCK.9 \
	man9/RW_TRYRDLOCK.9 \
	man9/RW_TRYWRLOCK.9 \
	man9/RW_UNLOCK.9 \
	man9/RW_WRLOCK.9 \
	man9/sleep.9 \
	man9/SLEEP_ALLOC.9 \
	man9/SLEEP_DEALLOC.9 \
	man9/SLEEP_LOCK.9 \
	man9/SLEEP_LOCKAVAIL.9 \
	man9/SLEEP_LOCKOWNED.9 \
	man9/SLEEP_LOCK_SIG.9 \
	man9/SLEEP_TRYLOCK.9 \
	man9/SLEEP_UNLOCK.9 \
	man9/spl0.9 \
	man9/spl1.9 \
	man9/spl2.9 \
	man9/spl3.9 \
	man9/spl4.9 \
	man9/spl5.9 \
	man9/spl6.9 \
	man9/spl7.9 \
	man9/spl.9 \
	man9/splbase.9 \
	man9/spldisk.9 \
	man9/splhi.9 \
	man9/splstr.9 \
	man9/spltimeout.9 \
	man9/spltty.9 \
	man9/splx.9 \
	man9/SV_ALLOC.9 \
	man9/SV_BROADCAST.9 \
	man9/SV_DEALLOC.9 \
	man9/SV_SIGNAL.9 \
	man9/SV_WAIT.9 \
	man9/SV_WAIT_SIG.9 \
	man9/TRYLOCK.9 \
	man9/UNLOCK.9 \
	man9/wakeup.9

mps_compat_pages = \
	man9/mpscompat.9 \
	man9/mi_alloc.9 \
	man9/mi_allocq.9 \
	man9/mi_alloc_sleep.9 \
	man9/mi_attach.9 \
	man9/mi_bcmp.9 \
	man9/mi_bufcall.9 \
	man9/mi_close_comm.9 \
	man9/mi_close_detached.9 \
	man9/mi_close_free.9 \
	man9/mi_close_unlink.9 \
	man9/mi_copy_done.9 \
	man9/mi_copyin.9 \
	man9/mi_copyin_n.9 \
	man9/mi_copyout.9 \
	man9/mi_copyout_alloc.9 \
	man9/mi_copy_set_rval.9 \
	man9/mi_copy_state.9 \
	man9/mi_detach.9 \
	man9/mi_first_dev_ptr.9 \
	man9/mi_first_ptr.9 \
	man9/mi_free.9 \
	man9/mi_freeq.9 \
	man9/mi_mpprintf.9 \
	man9/mi_mpprintf_nr.9 \
	man9/mi_next_dev_ptr.9 \
	man9/mi_next_ptr.9 \
	man9/mi_offset_param.9 \
	man9/mi_offset_paramc.9 \
	man9/mi_open_alloc.9 \
	man9/mi_open_alloc_sleep.9 \
	man9/mi_open_comm.9 \
	man9/mi_open_detached.9 \
	man9/mi_open_link.9 \
	man9/mi_prev_ptr.9 \
	man9/mi_reallocb.9 \
	man9/mi_reuse_proto.9 \
	man9/mi_set_sth_copyopt.9 \
	man9/mi_set_sth_hiwat.9 \
	man9/mi_set_sth_lowat.9 \
	man9/mi_set_sth_maxblk.9 \
	man9/mi_set_sth_wroff.9 \
	man9/mi_sprintf.9 \
	man9/mi_sprintf_putc.9 \
	man9/mi_strcmp.9 \
	man9/mi_strlen.9 \
	man9/mi_strlog.9 \
	man9/mi_strol.9 \
	man9/mi_timer.9 \
	man9/mi_timer_MAC.9 \
	man9/mi_timer_SUN.9 \
	man9/mi_timer_alloc.9 \
	man9/mi_timer_alloc_MAC.9 \
	man9/mi_timer_alloc_SUN.9 \
	man9/mi_timer_cancel.9 \
	man9/mi_timer_free.9 \
	man9/mi_timer_move.9 \
	man9/mi_timer_q_switch.9 \
	man9/mi_timer_stop.9 \
	man9/mi_timer_valid.9 \
	man9/mi_zalloc.9 \
	man9/mi_zalloc_sleep.9 \
	man9/mps_become_writer.9 \
	man9/mps_intr_disable.9 \
	man9/mps_intr_enable.9

sun_compat_pages = \
	man9/suncompat.9 \
	man9/DB_BASE.9 \
	man9/DB_LIM.9 \
	man9/DB_REF.9 \
	man9/DB_TYPE.9 \
	man9/ddi_create_minor_node.9 \
	man9/ddi_driver_major.9 \
	man9/ddi_driver_name.9 \
	man9/ddi_get_cred.9 \
	man9/ddi_getiminor.9 \
	man9/ddi_get_instance.9 \
	man9/ddi_get_lbolt.9 \
	man9/ddi_get_pid.9 \
	man9/ddi_get_soft_state.9 \
	man9/ddi_get_time.9 \
	man9/ddi_prop_op.9 \
	man9/ddi_remove_minor_node.9 \
	man9/ddi_removing_power.9 \
	man9/ddi_soft_state.9 \
	man9/ddi_soft_state_fini.9 \
	man9/ddi_soft_state_free.9 \
	man9/ddi_soft_state_init.9 \
	man9/ddi_soft_state_zalloc.9 \
	man9/ddi_umem_alloc.9 \
	man9/ddi_umem_free.9 \
	man9/freezestr_SUN.9 \
	man9/MBLKHEAD.9 \
	man9/MBLKIN.9 \
	man9/MBLKL.9 \
	man9/MBLKSIZE.9 \
	man9/MBLKTAIL.9 \
	man9/mcopyin.9 \
	man9/mcopymsg.9 \
	man9/mcopyout.9 \
	man9/merror.9 \
	man9/mexchange.9 \
	man9/mexpandb.9 \
	man9/mioc2ack.9 \
	man9/miocack.9 \
	man9/miocnak.9 \
	man9/miocpullup.9 \
	man9/mod_info.9 \
	man9/mod_install.9 \
	man9/mod_remove.9 \
	man9/mod_strmops.9 \
	man9/nochpoll.9 \
	man9/nodev.9 \
	man9/nulldev.9 \
	man9/OFFSET.9 \
	man9/qbufcall.9 \
	man9/qtimeout.9 \
	man9/queclass.9 \
	man9/qunbufcall.9 \
	man9/quntimeout.9 \
	man9/qwait.9 \
	man9/qwait_sig.9 \
	man9/qwriter.9 \
	man9/unfreezestr_SUN.9

uw7_compat_pages = \
	man9/uw7compat.9 \
	man9/ATOMIC_INT_ADD.9 \
	man9/ATOMIC_INT_ALLOC.9 \
	man9/ATOMIC_INT_DEALLOC.9 \
	man9/ATOMIC_INT_DECR.9 \
	man9/ATOMIC_INT_INCR.9 \
	man9/ATOMIC_INT_INIT.9 \
	man9/ATOMIC_INT_READ.9 \
	man9/ATOMIC_INT_SUB.9 \
	man9/ATOMIC_INT_WRITE.9 \
	man9/msgphysreq.9 \
	man9/msgpullup_physreq.9 \
	man9/msgscgth.9 \
	man9/printf_UW7.9 \
	man9/strioccall.9

if WITH_LFS
uw7_compat_pages += \
	man9/allocb_physreq.9
endif

osf_compat_pages = \
	man9/osfcompat.9 \
	man9/lbolt.9 \
	man9/puthere.9 \
	man9/streams_close_comm.9 \
	man9/streams_open_comm.9 \
	man9/streams_open_ocomm.9 \
	man9/strmod_add.9 \
	man9/strmod_del.9 \
	man9/time.9

aix_compat_pages = \
	man9/aixcompat.9 \
	man9/str_install_AIX.9 \
	man9/wantio.9 \
	man9/wantmsg.9

hpux_compat_pages = \
	man9/hpuxcompat.9 \
	man9/get_sleep_lock.9 \
	man9/mpsleep.9 \
	man9/str_install_HPUX.9 \
	man9/streams_get_sleep_lock.9 \
	man9/streams_mpsleep.9 \
	man9/streams_put.9

irix_compat_pages = \
	man9/irixcompat.9 \
	man9/cmn_err_tag.9 \
	man9/icmn_err.9

lis_compat_pages = \
	man9/liscompat.9 \
	man9/lisdeprec.9 \
	man9/LiS.9 \
	man9/lis_adjmsg.9 \
	man9/lis_alloc_atomic_fcn.9 \
	man9/lis_allocb.9 \
	man9/lis_allocb_physreq.9 \
	man9/lis_alloc_dma_fcn.9 \
	man9/lis_alloc_kernel_fcn.9 \
	man9/lis_appq.9 \
	man9/lis_apush_get.9 \
	man9/lis_apush_set.9 \
	man9/lis_assert_fail.9 \
	man9/lis_atomic_add.9 \
	man9/lis_atomic_dec.9 \
	man9/lis_atomic_dec_and_test.9 \
	man9/lis_atomic_inc.9 \
	man9/lis_atomic_read.9 \
	man9/lis_atomic_set.9 \
	man9/lis_atomic_sub.9 \
	man9/lis_backq.9 \
	man9/lis_backq_fcn.9 \
	man9/lis_bcanput.9 \
	man9/lis_bcanputnext.9 \
	man9/lis_bcanputnext_anyband.9 \
	man9/lis_bufcall.9 \
	man9/lis_clone_major.9 \
	man9/lis_cmn_err.9 \
	man9/lis_copyb.9 \
	man9/lis_copymsg.9 \
	man9/lis_date.9 \
	man9/lis_debug_mask2.9 \
	man9/lis_debug_mask.9 \
	man9/lis_disable_irq.9 \
	man9/lis_down_fcn.9 \
	man9/lis_dsecs.9 \
	man9/lis_dupb.9 \
	man9/lis_dupmsg.9 \
	man9/lis_enable_irq.9 \
	man9/lis_esballoc.9 \
	man9/lis_esbbcall.9 \
	man9/lis_flushband.9 \
	man9/lis_flushq.9 \
	man9/lis_free.9 \
	man9/lis_freeb.9 \
	man9/lis_free_dma.9 \
	man9/lis_free_irq.9 \
	man9/lis_free_mem_fcn.9 \
	man9/lis_freemsg.9 \
	man9/lis_free_pages_fcn.9 \
	man9/lis_freezestr.9 \
	man9/lis_get_free_pages_atomic_fcn.9 \
	man9/lis_get_free_pages_fcn.9 \
	man9/lis_get_free_pages_kernel_fcn.9 \
	man9/lis_getint.9 \
	man9/lis_getq.9 \
	man9/lis_gettimeofday.9 \
	man9/lis_hitime.9 \
	man9/lis_insq.9 \
	man9/lis_interruptible_sleep_on.9 \
	man9/lis_ioremap.9 \
	man9/lis_ioremap_nocache.9 \
	man9/lis_iounmap.9 \
	man9/lis_jiffies.9 \
	man9/lis_kernel_down.9 \
	man9/lis_kernel_up.9 \
	man9/lis_kernel_version.9 \
	man9/lis_kfree.9 \
	man9/lis_kmalloc.9 \
	man9/lis_linkb.9 \
	man9/lis_major.9 \
	man9/lis_malloc.9 \
	man9/lis_max.9 \
	man9/lis_milli_to_ticks.9 \
	man9/lis_min.9 \
	man9/lis_mknod.9 \
	man9/lis_mount.9 \
	man9/lis_msecs.9 \
	man9/lis_msgdsize.9 \
	man9/lis_msgpullup.9 \
	man9/lis_msgsize.9 \
	man9/lis_msg_type_name.9 \
	man9/lis_num_cpus.9 \
	man9/lis_osif_cli.9 \
	man9/lis_osif_do_gettimeofday.9 \
	man9/lis_osif_do_settimeofday.9 \
	man9/lis_osif_pci_alloc_consistent.9 \
	man9/lis_osif_pci_dac_dma_supported.9 \
	man9/lis_osif_pci_dac_dma_sync_single.9 \
	man9/lis_osif_pci_dac_dma_sync_single_for_cpu.9 \
	man9/lis_osif_pci_dac_dma_sync_single_for_device.9 \
	man9/lis_osif_pci_dac_dma_to_offset.9 \
	man9/lis_osif_pci_dac_dma_to_page.9 \
	man9/lis_osif_pci_dac_page_to_dma.9 \
	man9/lis_osif_pci_dac_set_dma_mask.9 \
	man9/lis_osif_pci_disable_device.9 \
	man9/lis_osif_pci_dma_supported.9 \
	man9/lis_osif_pci_dma_sync_sg.9 \
	man9/lis_osif_pci_dma_sync_single.9 \
	man9/lis_osif_pci_enable_device.9 \
	man9/lis_osif_pci_find_class.9 \
	man9/lis_osif_pci_find_device.9 \
	man9/lis_osif_pci_find_slot.9 \
	man9/lis_osif_pci_free_consistent.9 \
	man9/lis_osif_pci_map_page.9 \
	man9/lis_osif_pci_map_sg.9 \
	man9/lis_osif_pci_map_single.9 \
	man9/lis_osif_pci_module_init.9 \
	man9/lis_osif_pci_read_config_byte.9 \
	man9/lis_osif_pci_read_config_dword.9 \
	man9/lis_osif_pci_read_config_word.9 \
	man9/lis_osif_pci_set_dma_mask.9 \
	man9/lis_osif_pci_set_master.9 \
	man9/lis_osif_pci_unmap_page.9 \
	man9/lis_osif_pci_unmap_sg.9 \
	man9/lis_osif_pci_unmap_single.9 \
	man9/lis_osif_pci_unregister_driver.9 \
	man9/lis_osif_pci_write_config_byte.9 \
	man9/lis_osif_pci_write_config_dword.9 \
	man9/lis_osif_pci_write_config_word.9 \
	man9/lis_osif_sg_dma_address.9 \
	man9/lis_osif_sg_dma_len.9 \
	man9/lis_osif_sti.9 \
	man9/lis_OTHER.9 \
	man9/lis_own_spl.9 \
	man9/lis_pcibios_find_class.9 \
	man9/lis_pcibios_find_device.9 \
	man9/lis_pcibios_init.9 \
	man9/lis_pcibios_present.9 \
	man9/lis_pcibios_read_config_byte.9 \
	man9/lis_pcibios_read_config_dword.9 \
	man9/lis_pcibios_read_config_word.9 \
	man9/lis_pcibios_strerror.9 \
	man9/lis_pcibios_write_config_byte.9 \
	man9/lis_pcibios_write_config_dword.9 \
	man9/lis_pcibios_write_config_word.9 \
	man9/lis_pci_disable_device.9 \
	man9/lis_pci_enable_device.9 \
	man9/lis_pci_find_class.9 \
	man9/lis_pci_find_device.9 \
	man9/lis_pci_find_slot.9 \
	man9/lis_pci_read_config_byte.9 \
	man9/lis_pci_read_config_dword.9 \
	man9/lis_pci_read_config_word.9 \
	man9/lis_pci_set_master.9 \
	man9/lis_pci_write_config_byte.9 \
	man9/lis_pci_write_config_dword.9 \
	man9/lis_pci_write_config_word.9 \
	man9/lis_phys_to_virt.9 \
	man9/lis_print_block.9 \
	man9/lis_print_data.9 \
	man9/lis_printk.9 \
	man9/lis_print_mem.9 \
	man9/lis_print_msg.9 \
	man9/lis_print_queue.9 \
	man9/lis_pullupmsg.9 \
	man9/lis_putbq.9 \
	man9/lis_putbyte.9 \
	man9/lis_putctl1.9 \
	man9/lis_putctl.9 \
	man9/lis_putnextctl1.9 \
	man9/lis_putnextctl.9 \
	man9/lis_putq.9 \
	man9/lis_qcountstrm.9 \
	man9/lis_qenable.9 \
	man9/lis_qprocsoff.9 \
	man9/lis_qprocson.9 \
	man9/lis_qsize.9 \
	man9/lis_queue_name.9 \
	man9/lis_register_strdev.9 \
	man9/lis_register_strmod.9 \
	man9/lis_release_region.9 \
	man9/lis_request_dma.9 \
	man9/lis_request_irq.9 \
	man9/lis_request_region.9 \
	man9/lis_rmvb.9 \
	man9/lis_rmvq.9 \
	man9/lis_rw_lock_alloc_fcn.9 \
	man9/lis_rw_lock_free_fcn.9 \
	man9/lis_rw_lock_init_fcn.9 \
	man9/lis_rw_read_lock_fcn.9 \
	man9/lis_rw_read_lock_irq_fcn.9 \
	man9/lis_rw_read_lock_irqsave_fcn.9 \
	man9/lis_rw_read_unlock_fcn.9 \
	man9/lis_rw_read_unlock_irq_fcn.9 \
	man9/lis_rw_read_unlock_irqrestore_fcn.9 \
	man9/lis_rw_write_lock_fcn.9 \
	man9/lis_rw_write_lock_irq_fcn.9 \
	man9/lis_rw_write_lock_irqsave_fcn.9 \
	man9/lis_rw_write_unlock_fcn.9 \
	man9/lis_rw_write_unlock_irq_fcn.9 \
	man9/lis_rw_write_unlock_irqrestore_fcn.9 \
	man9/lis_safe_canenable.9 \
	man9/lis_safe_enableok.9 \
	man9/lis_safe_noenable.9 \
	man9/lis_safe_OTHERQ.9 \
	man9/lis_safe_putmsg.9 \
	man9/lis_safe_putnext.9 \
	man9/lis_safe_qreply.9 \
	man9/lis_safe_RD.9 \
	man9/lis_safe_SAMESTR.9 \
	man9/lis_safe_WR.9 \
	man9/lis_secs.9 \
	man9/lis_sem_alloc.9 \
	man9/lis_sem_destroy.9 \
	man9/lis_sem_init.9 \
	man9/lis_sleep_on.9 \
	man9/lis_spin_is_locked_fcn.9 \
	man9/lis_spin_lock_alloc_fcn.9 \
	man9/lis_spin_lock_fcn.9 \
	man9/lis_spin_lock_free_fcn.9 \
	man9/lis_spin_lock_init_fcn.9 \
	man9/lis_spin_lock_irq_fcn.9 \
	man9/lis_spin_lock_irqsave_fcn.9 \
	man9/lis_spin_trylock_fcn.9 \
	man9/lis_spin_unlock_fcn.9 \
	man9/lis_spin_unlock_irq_fcn.9 \
	man9/lis_spin_unlock_irqrestore_fcn.9 \
	man9/lis_spl0_fcn.9 \
	man9/lis_splstr_fcn.9 \
	man9/lis_splx_fcn.9 \
	man9/lis_sprintf.9 \
	man9/lis_strm_name.9 \
	man9/lis_strm_name_from_queue.9 \
	man9/lis_stropts_file.9 \
	man9/lis_strqget.9 \
	man9/lis_strqset.9 \
	man9/lis_testb.9 \
	man9/lis_thread_start.9 \
	man9/lis_thread_stop.9 \
	man9/lis_timeout_fcn.9 \
	man9/lis_udelay.9 \
	man9/lis_unbufcall.9 \
	man9/lis_unfreezestr.9 \
	man9/lis_unlink.9 \
	man9/lis_unlinkb.9 \
	man9/lis_unregister_strdev.9 \
	man9/lis_unregister_strmod.9 \
	man9/lis_untimeout.9 \
	man9/lis_up_fcn.9 \
	man9/lis_usecs.9 \
	man9/lis_usectohz.9 \
	man9/lis_version.9 \
	man9/lis_vfree.9 \
	man9/lis_virt_to_phys.9 \
	man9/lis_vmalloc.9 \
	man9/lis_vremap.9 \
	man9/lis_vsprintf.9 \
	man9/lis_wake_up.9 \
	man9/lis_wake_up_interruptible.9 \
	man9/lis_xmsgsize.9 \
	man9/lis_zmalloc.9

lis_deprec_pages = \
	man9/lis_add_timer.9 \
	man9/lis_allocq.9 \
	man9/lis_apushm.9 \
	man9/lis_basename.9 \
	man9/lis_bprintf.9 \
	man9/lis_can_unload.9 \
	man9/lis_check_guard.9 \
	man9/lis_check_mem.9 \
	man9/lis_check_mem_region.9 \
	man9/lis_check_q_magic.9 \
	man9/lis_check_region.9 \
	man9/lis_check_umem.9 \
	man9/lis_copyin.9 \
	man9/lis_copyout.9 \
	man9/lis_dec_mod_cnt_fcn.9 \
	man9/lis_del_timer.9 \
	man9/lis_dobufcall.9 \
	man9/lis_doclose.9 \
	man9/lis_enable_intr.9 \
	man9/lis_fattach.9 \
	man9/lis_fdetach.9 \
	man9/lis_fdetach_all.9 \
	man9/lis_fdetach_stream.9 \
	man9/lis_fifo_close_sync.9 \
	man9/lis_fifo_open_sync.9 \
	man9/lis_fifo_write_sync.9 \
	man9/lis_file_inode.9 \
	man9/lis_file_str.9 \
	man9/lis_find_strdev.9 \
	man9/lis_flush_print_buffer.9 \
	man9/lis_fmod_sw.9 \
	man9/lis_freedb.9 \
	man9/lis_free_passfp.9 \
	man9/lis_freeq.9 \
	man9/lis_fstr_sw.9 \
	man9/lis_get_fifo.9 \
	man9/lis_get_passfp.9 \
	man9/lis_get_pipe.9 \
	man9/lis_inc_mod_cnt_fcn.9 \
	man9/lis_init_bufcall.9 \
	man9/lis_in_syscall.9 \
	man9/lis_ioc_fattach.9 \
	man9/lis_ioc_fdetach.9 \
	man9/lis_ioc_pipe.9 \
	man9/lis__kfree.9 \
	man9/lis_kill_pg.9 \
	man9/lis_kill_proc.9 \
	man9/lis__kmalloc.9 \
	man9/lis_loadable_load.9 \
	man9/lis_maj_min_name.9 \
	man9/lis_mark_mem.9 \
	man9/lis_max_mem.9 \
	man9/lis_old_inode.9 \
	man9/lis_open_cnt.9 \
	man9/lis_pci_cleanup.9 \
	man9/lis_pipe.9 \
	man9/lis_poll_2_1.9 \
	man9/lis_poll_bits.9 \
	man9/lis_poll_events.9 \
	man9/lis_poll_file.9 \
	man9/lis_print_queues.9 \
	man9/lis_print_spl_track.9 \
	man9/lis_print_stream.9 \
	man9/lis_qattach.9 \
	man9/lis_qdetach.9 \
	man9/lis_queuerun_cnts.9 \
	man9/lis_queues_running.9 \
	man9/lis_recvfd.9 \
	man9/lis_release_mem_region.9 \
	man9/lis_request_mem_region.9 \
	man9/lis_runq_cnts.9 \
	man9/lis_runq_req_cnt.9 \
	man9/lis_sendfd.9 \
	man9/lis_set_file_str.9 \
	man9/lis_setq.9 \
	man9/lis_setqsched.9 \
	man9/lis_setqsched_cnts.9 \
	man9/lis_setqsched_isr_cnts.9 \
	man9/lis_stdata_cnt.9 \
	man9/lis_strclose.9 \
	man9/lis_strcount.9 \
	man9/lis_strgetpmsg.9 \
	man9/lis_strioctl.9 \
	man9/lis_stropen.9 \
	man9/lis_strputpmsg.9 \
	man9/lis_strread.9 \
	man9/lis_strstats.9 \
	man9/lis_strwrite.9 \
	man9/lis_umount2.9 \
	man9/lis_valid_mod_list.9

lfs_compat_pages = \
	man9/lfscompat.9 \
	man9/apush_get.9 \
	man9/apush_set.9 \
	man9/bcanget.9 \
	man9/canget.9 \
	man9/copyin.9 \
	man9/copyout.9 \
	man9/ctlmsg.9 \
	man9/delay.9 \
	man9/drv_getparm.9 \
	man9/drv_hztomsec.9 \
	man9/drv_hztousec.9 \
	man9/drv_msectohz.9 \
	man9/drv_priv.9 \
	man9/drv_usectohz.9 \
	man9/drv_usecwait.9 \
	man9/enableq.9 \
	man9/getadmin.9 \
	man9/getmid.9 \
	man9/linkmsg.9 \
	man9/pcmsg.9 \
	man9/putctl2.9 \
	man9/putnextctl2.9 \
	man9/strlog.9 \
	man9/vstrlog_hook.9

mac_compat_pages = \
	man9/maccompat.9 \
	man9/allocbi.9 \
	man9/esballoca.9 \
	man9/mpnotify.9

sux_compat_pages = \
	man9/suxcompat.9

uxp_compat_pages = \
	man9/uxpcompat.9

man_MANS = \
	man8/strcompat_mknod.8 \
	man9/alloc_proto.9 \
	man9/bufq_append.9 \
	man9/bufq_dequeue.9 \
	man9/bufq_dequeue_tail.9 \
	man9/bufq_freehead.9 \
	man9/bufq_head.9 \
	man9/bufq_init.9 \
	man9/bufq_insert.9 \
	man9/bufq_length.9 \
	man9/bufq_lock.9 \
	man9/bufq_purge.9 \
	man9/bufq_queue.9 \
	man9/bufq_queue_head.9 \
	man9/bufq_resupply.9 \
	man9/bufq_size.9 \
	man9/bufq_splice_head.9 \
	man9/bufq_splice_tail.9 \
	man9/bufq_supply.9 \
	man9/bufq_tail.9 \
	man9/bufq_unlink.9 \
	man9/bufq_unlock.9 \
	man9/compat_undoc.9 \
	man9/freechunks.9 \
	man9/mkiocb.9 \
	man9/ss7_allocb.9 \
	man9/ss7_bufcall.9 \
	man9/ss7_bufpool_init.9 \
	man9/ss7_bufpool_release.9 \
	man9/ss7_bufpool_reserve.9 \
	man9/ss7_bufpool_term.9 \
	man9/ss7_bufsrv.9 \
	man9/ss7_copyb.9 \
	man9/ss7_copymsg.9 \
	man9/ss7_do_timeout.9 \
	man9/ss7_dupb.9 \
	man9/ss7_dupmsg.9 \
	man9/ss7_esballoc.9 \
	man9/ss7_esbbcall.9 \
	man9/ss7_fast_allocb.9 \
	man9/ss7_fast_allocb_bh.9 \
	man9/ss7_fast_freeb.9 \
	man9/ss7_fast_freeb_bh.9 \
	man9/ss7_fast_freemsg.9 \
	man9/ss7_fast_freemsg_bh.9 \
	man9/ss7_iput.9 \
	man9/ss7_isrv.9 \
	man9/ss7_oput.9 \
	man9/ss7_osrv.9 \
	man9/ss7_pullupmsg.9 \
	man9/ss7_putq.9 \
	man9/ss7_r_flush.9 \
	man9/ss7_srvq.9 \
	man9/ss7_start_timer.9 \
	man9/ss7_stop_timer.9 \
	man9/ss7_trylockq.9 \
	man9/ss7_unbufcall.9 \
	man9/ss7_unlockq.9 \
	man9/ss7_w_flush.9 \
	man9/str_uninstall.9

if CONFIG_STREAMS_COMPAT_SVR3
man_MANS		    += $(svr3_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_SVR4
man_MANS		    += $(svr4_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_MPS
man_MANS		    += $(mps_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_SUN
man_MANS		    += $(sun_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_UW7
man_MANS		    += $(uw7_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_OSF
man_MANS		    += $(osf_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_AIX
man_MANS		    += $(aix_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_HPUX
man_MANS		    += $(hpux_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_IRIX
man_MANS		    += $(irix_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_LIS
man_MANS		    += $(lis_compat_pages)
man_MANS		    += $(lis_deprec_pages)
endif
if CONFIG_STREAMS_COMPAT_LFS
man_MANS		    += $(lfs_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_MAC
man_MANS		    += $(mac_compat_pages)
endif

if CONFIG_STREAMS_COMPAT_SVR3_MODULE
man_MANS		    += $(svr3_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
man_MANS		    += $(svr4_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
man_MANS		    += $(mps_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
man_MANS		    += $(sun_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
man_MANS		    += $(uw7_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
man_MANS		    += $(osf_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
man_MANS		    += $(aix_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
man_MANS		    += $(hpux_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
man_MANS		    += $(irix_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
man_MANS		    += $(lis_compat_pages)
man_MANS		    += $(lis_deprec_pages)
endif
if CONFIG_STREAMS_COMPAT_LFS_MODULE
man_MANS		    += $(lfs_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
man_MANS		    += $(mac_compat_pages)
endif

dist_noinst_svr3_compat_pages = \
	man9/svr3compat.9.man \
	man9/emajor.9.man \
	man9/eminor.9.man

dist_noinst_svr4_compat_pages = \
	man9/svr4compat.9.man \
	man9/dtimeout.9.man \
	man9/etoimajor.9.man \
	man9/getemajor.9.man \
	man9/geteminor.9.man \
	man9/itimeout.9.man \
	man9/itoemajor.9.man \
	man9/LOCK.9.man \
	man9/LOCK_ALLOC.9.man \
	man9/LOCK_DEALLOC.9.man \
	man9/LOCK_OWNED.9.man \
	man9/MPSTR_QLOCK.9.man \
	man9/MPSTR_QRELE.9.man \
	man9/MPSTR_STPLOCK.9.man \
	man9/MPSTR_STPRELE.9.man \
	man9/RW_ALLOC.9.man \
	man9/RW_DEALLOC.9.man \
	man9/RW_RDLOCK.9.man \
	man9/RW_TRYRDLOCK.9.man \
	man9/RW_TRYWRLOCK.9.man \
	man9/RW_UNLOCK.9.man \
	man9/RW_WRLOCK.9.man \
	man9/sleep.9.man \
	man9/SLEEP_ALLOC.9.man \
	man9/SLEEP_DEALLOC.9.man \
	man9/SLEEP_LOCK.9.man \
	man9/SLEEP_LOCKAVAIL.9.man \
	man9/SLEEP_LOCKOWNED.9.man \
	man9/SLEEP_LOCK_SIG.9.man \
	man9/SLEEP_TRYLOCK.9.man \
	man9/SLEEP_UNLOCK.9.man \
	man9/spl0.9.man \
	man9/spl1.9.man \
	man9/spl2.9.man \
	man9/spl3.9.man \
	man9/spl4.9.man \
	man9/spl5.9.man \
	man9/spl6.9.man \
	man9/spl7.9.man \
	man9/spl.9.man \
	man9/splbase.9.man \
	man9/spldisk.9.man \
	man9/splhi.9.man \
	man9/splstr.9.man \
	man9/spltimeout.9.man \
	man9/spltty.9.man \
	man9/splx.9.man \
	man9/SV_ALLOC.9.man \
	man9/SV_BROADCAST.9.man \
	man9/SV_DEALLOC.9.man \
	man9/SV_SIGNAL.9.man \
	man9/SV_WAIT.9.man \
	man9/SV_WAIT_SIG.9.man \
	man9/TRYLOCK.9.man \
	man9/UNLOCK.9.man \
	man9/wakeup.9.man

dist_noinst_mps_compat_pages = \
	man9/mpscompat.9.man \
	man9/mi_alloc.9.man \
	man9/mi_allocq.9.man \
	man9/mi_alloc_sleep.9.man \
	man9/mi_attach.9.man \
	man9/mi_bcmp.9.man \
	man9/mi_bufcall.9.man \
	man9/mi_close_comm.9.man \
	man9/mi_close_detached.9.man \
	man9/mi_close_free.9.man \
	man9/mi_close_unlink.9.man \
	man9/mi_copy_done.9.man \
	man9/mi_copyin.9.man \
	man9/mi_copyin_n.9.man \
	man9/mi_copyout.9.man \
	man9/mi_copyout_alloc.9.man \
	man9/mi_copy_set_rval.9.man \
	man9/mi_copy_state.9.man \
	man9/mi_detach.9.man \
	man9/mi_first_dev_ptr.9.man \
	man9/mi_first_ptr.9.man \
	man9/mi_free.9.man \
	man9/mi_freeq.9.man \
	man9/mi_mpprintf.9.man \
	man9/mi_mpprintf_nr.9.man \
	man9/mi_next_dev_ptr.9.man \
	man9/mi_next_ptr.9.man \
	man9/mi_offset_param.9.man \
	man9/mi_offset_paramc.9.man \
	man9/mi_open_alloc.9.man \
	man9/mi_open_alloc_sleep.9.man \
	man9/mi_open_comm.9.man \
	man9/mi_open_detached.9.man \
	man9/mi_open_link.9.man \
	man9/mi_prev_ptr.9.man \
	man9/mi_reallocb.9.man \
	man9/mi_reuse_proto.9.man \
	man9/mi_set_sth_copyopt.9.man \
	man9/mi_set_sth_hiwat.9.man \
	man9/mi_set_sth_lowat.9.man \
	man9/mi_set_sth_maxblk.9.man \
	man9/mi_set_sth_wroff.9.man \
	man9/mi_sprintf.9.man \
	man9/mi_sprintf_putc.9.man \
	man9/mi_strcmp.9.man \
	man9/mi_strlen.9.man \
	man9/mi_strlog.9.man \
	man9/mi_strol.9.man \
	man9/mi_timer.9.man \
	man9/mi_timer_MAC.9.man \
	man9/mi_timer_SUN.9.man \
	man9/mi_timer_alloc.9.man \
	man9/mi_timer_alloc_MAC.9.man \
	man9/mi_timer_alloc_SUN.9.man \
	man9/mi_timer_cancel.9.man \
	man9/mi_timer_free.9.man \
	man9/mi_timer_move.9.man \
	man9/mi_timer_q_switch.9.man \
	man9/mi_timer_stop.9.man \
	man9/mi_timer_valid.9.man \
	man9/mi_zalloc.9.man \
	man9/mi_zalloc_sleep.9.man \
	man9/mps_become_writer.9.man \
	man9/mps_intr_disable.9.man \
	man9/mps_intr_enable.9.man

dist_noinst_sun_compat_pages = \
	man9/suncompat.9.man \
	man9/DB_BASE.9.man \
	man9/DB_LIM.9.man \
	man9/DB_REF.9.man \
	man9/DB_TYPE.9.man \
	man9/ddi_create_minor_node.9.man \
	man9/ddi_driver_major.9.man \
	man9/ddi_driver_name.9.man \
	man9/ddi_get_cred.9.man \
	man9/ddi_getiminor.9.man \
	man9/ddi_get_instance.9.man \
	man9/ddi_get_lbolt.9.man \
	man9/ddi_get_pid.9.man \
	man9/ddi_get_soft_state.9.man \
	man9/ddi_get_time.9.man \
	man9/ddi_prop_op.9.man \
	man9/ddi_remove_minor_node.9.man \
	man9/ddi_removing_power.9.man \
	man9/ddi_soft_state.9.man \
	man9/ddi_soft_state_fini.9.man \
	man9/ddi_soft_state_free.9.man \
	man9/ddi_soft_state_init.9.man \
	man9/ddi_soft_state_zalloc.9.man \
	man9/ddi_umem_alloc.9.man \
	man9/ddi_umem_free.9.man \
	man9/freezestr_SUN.9.man \
	man9/MBLKHEAD.9.man \
	man9/MBLKIN.9.man \
	man9/MBLKL.9.man \
	man9/MBLKSIZE.9.man \
	man9/MBLKTAIL.9.man \
	man9/mcopyin.9.man \
	man9/mcopymsg.9.man \
	man9/mcopyout.9.man \
	man9/merror.9.man \
	man9/mexchange.9.man \
	man9/mexpandb.9.man \
	man9/mioc2ack.9.man \
	man9/miocack.9.man \
	man9/miocnak.9.man \
	man9/miocpullup.9.man \
	man9/mod_info.9.man \
	man9/mod_install.9.man \
	man9/mod_remove.9.man \
	man9/mod_strmops.9.man \
	man9/nochpoll.9.man \
	man9/nodev.9.man \
	man9/nulldev.9.man \
	man9/OFFSET.9.man \
	man9/qbufcall.9.man \
	man9/qtimeout.9.man \
	man9/queclass.9.man \
	man9/qunbufcall.9.man \
	man9/quntimeout.9.man \
	man9/qwait.9.man \
	man9/qwait_sig.9.man \
	man9/qwriter.9.man \
	man9/unfreezestr_SUN.9.man

dist_noinst_uw7_compat_pages = \
	man9/uw7compat.9.man \
	man9/ATOMIC_INT_ADD.9.man \
	man9/ATOMIC_INT_ALLOC.9.man \
	man9/ATOMIC_INT_DEALLOC.9.man \
	man9/ATOMIC_INT_DECR.9.man \
	man9/ATOMIC_INT_INCR.9.man \
	man9/ATOMIC_INT_INIT.9.man \
	man9/ATOMIC_INT_READ.9.man \
	man9/ATOMIC_INT_SUB.9.man \
	man9/ATOMIC_INT_WRITE.9.man \
	man9/msgphysreq.9.man \
	man9/msgpullup_physreq.9.man \
	man9/msgscgth.9.man \
	man9/printf_UW7.9.man \
	man9/strioccall.9.man

if WITH_LFS
dist_noinst_uw7_compat_pages += \
	man9/allocb_physreq.9.man
endif

dist_noinst_osf_compat_pages = \
	man9/osfcompat.9.man \
	man9/lbolt.9.man \
	man9/puthere.9.man \
	man9/streams_close_comm.9.man \
	man9/streams_open_comm.9.man \
	man9/streams_open_ocomm.9.man \
	man9/strmod_add.9.man \
	man9/strmod_del.9.man \
	man9/time.9.man

dist_noinst_aix_compat_pages = \
	man9/aixcompat.9.man \
	man9/str_install_AIX.9.man \
	man9/wantio.9.man \
	man9/wantmsg.9.man

dist_noinst_hpux_compat_pages = \
	man9/hpuxcompat.9.man \
	man9/get_sleep_lock.9.man \
	man9/mpsleep.9.man \
	man9/str_install_HPUX.9.man \
	man9/streams_get_sleep_lock.9.man \
	man9/streams_mpsleep.9.man \
	man9/streams_put.9.man

dist_noinst_irix_compat_pages = \
	man9/irixcompat.9.man \
	man9/cmn_err_tag.9.man \
	man9/icmn_err.9.man

dist_noinst_lis_compat_pages = \
	man9/liscompat.9.man \
	man9/lisdeprec.9.man \
	man9/LiS.9.man \
	man9/lis_adjmsg.9.man \
	man9/lis_alloc_atomic_fcn.9.man \
	man9/lis_allocb.9.man \
	man9/lis_allocb_physreq.9.man \
	man9/lis_alloc_dma_fcn.9.man \
	man9/lis_alloc_kernel_fcn.9.man \
	man9/lis_appq.9.man \
	man9/lis_apush_get.9.man \
	man9/lis_apush_set.9.man \
	man9/lis_assert_fail.9.man \
	man9/lis_atomic_add.9.man \
	man9/lis_atomic_dec.9.man \
	man9/lis_atomic_dec_and_test.9.man \
	man9/lis_atomic_inc.9.man \
	man9/lis_atomic_read.9.man \
	man9/lis_atomic_set.9.man \
	man9/lis_atomic_sub.9.man \
	man9/lis_backq.9.man \
	man9/lis_backq_fcn.9.man \
	man9/lis_bcanput.9.man \
	man9/lis_bcanputnext.9.man \
	man9/lis_bcanputnext_anyband.9.man \
	man9/lis_bufcall.9.man \
	man9/lis_clone_major.9.man \
	man9/lis_cmn_err.9.man \
	man9/lis_copyb.9.man \
	man9/lis_copymsg.9.man \
	man9/lis_date.9.man \
	man9/lis_debug_mask2.9.man \
	man9/lis_debug_mask.9.man \
	man9/lis_disable_irq.9.man \
	man9/lis_down_fcn.9.man \
	man9/lis_dsecs.9.man \
	man9/lis_dupb.9.man \
	man9/lis_dupmsg.9.man \
	man9/lis_enable_irq.9.man \
	man9/lis_esballoc.9.man \
	man9/lis_esbbcall.9.man \
	man9/lis_flushband.9.man \
	man9/lis_flushq.9.man \
	man9/lis_free.9.man \
	man9/lis_freeb.9.man \
	man9/lis_free_dma.9.man \
	man9/lis_free_irq.9.man \
	man9/lis_free_mem_fcn.9.man \
	man9/lis_freemsg.9.man \
	man9/lis_free_pages_fcn.9.man \
	man9/lis_freezestr.9.man \
	man9/lis_get_free_pages_atomic_fcn.9.man \
	man9/lis_get_free_pages_fcn.9.man \
	man9/lis_get_free_pages_kernel_fcn.9.man \
	man9/lis_getint.9.man \
	man9/lis_getq.9.man \
	man9/lis_gettimeofday.9.man \
	man9/lis_hitime.9.man \
	man9/lis_insq.9.man \
	man9/lis_interruptible_sleep_on.9.man \
	man9/lis_ioremap.9.man \
	man9/lis_ioremap_nocache.9.man \
	man9/lis_iounmap.9.man \
	man9/lis_jiffies.9.man \
	man9/lis_kernel_down.9.man \
	man9/lis_kernel_up.9.man \
	man9/lis_kernel_version.9.man \
	man9/lis_kfree.9.man \
	man9/lis_kmalloc.9.man \
	man9/lis_linkb.9.man \
	man9/lis_major.9.man \
	man9/lis_malloc.9.man \
	man9/lis_max.9.man \
	man9/lis_milli_to_ticks.9.man \
	man9/lis_min.9.man \
	man9/lis_mknod.9.man \
	man9/lis_mount.9.man \
	man9/lis_msecs.9.man \
	man9/lis_msgdsize.9.man \
	man9/lis_msgpullup.9.man \
	man9/lis_msgsize.9.man \
	man9/lis_msg_type_name.9.man \
	man9/lis_num_cpus.9.man \
	man9/lis_osif_cli.9.man \
	man9/lis_osif_do_gettimeofday.9.man \
	man9/lis_osif_do_settimeofday.9.man \
	man9/lis_osif_pci_alloc_consistent.9.man \
	man9/lis_osif_pci_dac_dma_supported.9.man \
	man9/lis_osif_pci_dac_dma_sync_single.9.man \
	man9/lis_osif_pci_dac_dma_sync_single_for_cpu.9.man \
	man9/lis_osif_pci_dac_dma_sync_single_for_device.9.man \
	man9/lis_osif_pci_dac_dma_to_offset.9.man \
	man9/lis_osif_pci_dac_dma_to_page.9.man \
	man9/lis_osif_pci_dac_page_to_dma.9.man \
	man9/lis_osif_pci_dac_set_dma_mask.9.man \
	man9/lis_osif_pci_disable_device.9.man \
	man9/lis_osif_pci_dma_supported.9.man \
	man9/lis_osif_pci_dma_sync_sg.9.man \
	man9/lis_osif_pci_dma_sync_single.9.man \
	man9/lis_osif_pci_enable_device.9.man \
	man9/lis_osif_pci_find_class.9.man \
	man9/lis_osif_pci_find_device.9.man \
	man9/lis_osif_pci_find_slot.9.man \
	man9/lis_osif_pci_free_consistent.9.man \
	man9/lis_osif_pci_map_page.9.man \
	man9/lis_osif_pci_map_sg.9.man \
	man9/lis_osif_pci_map_single.9.man \
	man9/lis_osif_pci_module_init.9.man \
	man9/lis_osif_pci_read_config_byte.9.man \
	man9/lis_osif_pci_read_config_dword.9.man \
	man9/lis_osif_pci_read_config_word.9.man \
	man9/lis_osif_pci_set_dma_mask.9.man \
	man9/lis_osif_pci_set_master.9.man \
	man9/lis_osif_pci_unmap_page.9.man \
	man9/lis_osif_pci_unmap_sg.9.man \
	man9/lis_osif_pci_unmap_single.9.man \
	man9/lis_osif_pci_unregister_driver.9.man \
	man9/lis_osif_pci_write_config_byte.9.man \
	man9/lis_osif_pci_write_config_dword.9.man \
	man9/lis_osif_pci_write_config_word.9.man \
	man9/lis_osif_sg_dma_address.9.man \
	man9/lis_osif_sg_dma_len.9.man \
	man9/lis_osif_sti.9.man \
	man9/lis_OTHER.9.man \
	man9/lis_own_spl.9.man \
	man9/lis_pcibios_find_class.9.man \
	man9/lis_pcibios_find_device.9.man \
	man9/lis_pcibios_init.9.man \
	man9/lis_pcibios_present.9.man \
	man9/lis_pcibios_read_config_byte.9.man \
	man9/lis_pcibios_read_config_dword.9.man \
	man9/lis_pcibios_read_config_word.9.man \
	man9/lis_pcibios_strerror.9.man \
	man9/lis_pcibios_write_config_byte.9.man \
	man9/lis_pcibios_write_config_dword.9.man \
	man9/lis_pcibios_write_config_word.9.man \
	man9/lis_pci_disable_device.9.man \
	man9/lis_pci_enable_device.9.man \
	man9/lis_pci_find_class.9.man \
	man9/lis_pci_find_device.9.man \
	man9/lis_pci_find_slot.9.man \
	man9/lis_pci_read_config_byte.9.man \
	man9/lis_pci_read_config_dword.9.man \
	man9/lis_pci_read_config_word.9.man \
	man9/lis_pci_set_master.9.man \
	man9/lis_pci_write_config_byte.9.man \
	man9/lis_pci_write_config_dword.9.man \
	man9/lis_pci_write_config_word.9.man \
	man9/lis_phys_to_virt.9.man \
	man9/lis_print_block.9.man \
	man9/lis_print_data.9.man \
	man9/lis_printk.9.man \
	man9/lis_print_mem.9.man \
	man9/lis_print_msg.9.man \
	man9/lis_print_queue.9.man \
	man9/lis_pullupmsg.9.man \
	man9/lis_putbq.9.man \
	man9/lis_putbyte.9.man \
	man9/lis_putctl1.9.man \
	man9/lis_putctl.9.man \
	man9/lis_putnextctl1.9.man \
	man9/lis_putnextctl.9.man \
	man9/lis_putq.9.man \
	man9/lis_qcountstrm.9.man \
	man9/lis_qenable.9.man \
	man9/lis_qprocsoff.9.man \
	man9/lis_qprocson.9.man \
	man9/lis_qsize.9.man \
	man9/lis_queue_name.9.man \
	man9/lis_register_strdev.9.man \
	man9/lis_register_strmod.9.man \
	man9/lis_release_region.9.man \
	man9/lis_request_dma.9.man \
	man9/lis_request_irq.9.man \
	man9/lis_request_region.9.man \
	man9/lis_rmvb.9.man \
	man9/lis_rmvq.9.man \
	man9/lis_rw_lock_alloc_fcn.9.man \
	man9/lis_rw_lock_free_fcn.9.man \
	man9/lis_rw_lock_init_fcn.9.man \
	man9/lis_rw_read_lock_fcn.9.man \
	man9/lis_rw_read_lock_irq_fcn.9.man \
	man9/lis_rw_read_lock_irqsave_fcn.9.man \
	man9/lis_rw_read_unlock_fcn.9.man \
	man9/lis_rw_read_unlock_irq_fcn.9.man \
	man9/lis_rw_read_unlock_irqrestore_fcn.9.man \
	man9/lis_rw_write_lock_fcn.9.man \
	man9/lis_rw_write_lock_irq_fcn.9.man \
	man9/lis_rw_write_lock_irqsave_fcn.9.man \
	man9/lis_rw_write_unlock_fcn.9.man \
	man9/lis_rw_write_unlock_irq_fcn.9.man \
	man9/lis_rw_write_unlock_irqrestore_fcn.9.man \
	man9/lis_safe_canenable.9.man \
	man9/lis_safe_enableok.9.man \
	man9/lis_safe_noenable.9.man \
	man9/lis_safe_OTHERQ.9.man \
	man9/lis_safe_putmsg.9.man \
	man9/lis_safe_putnext.9.man \
	man9/lis_safe_qreply.9.man \
	man9/lis_safe_RD.9.man \
	man9/lis_safe_SAMESTR.9.man \
	man9/lis_safe_WR.9.man \
	man9/lis_secs.9.man \
	man9/lis_sem_alloc.9.man \
	man9/lis_sem_destroy.9.man \
	man9/lis_sem_init.9.man \
	man9/lis_sleep_on.9.man \
	man9/lis_spin_is_locked_fcn.9.man \
	man9/lis_spin_lock_alloc_fcn.9.man \
	man9/lis_spin_lock_fcn.9.man \
	man9/lis_spin_lock_free_fcn.9.man \
	man9/lis_spin_lock_init_fcn.9.man \
	man9/lis_spin_lock_irq_fcn.9.man \
	man9/lis_spin_lock_irqsave_fcn.9.man \
	man9/lis_spin_trylock_fcn.9.man \
	man9/lis_spin_unlock_fcn.9.man \
	man9/lis_spin_unlock_irq_fcn.9.man \
	man9/lis_spin_unlock_irqrestore_fcn.9.man \
	man9/lis_spl0_fcn.9.man \
	man9/lis_splstr_fcn.9.man \
	man9/lis_splx_fcn.9.man \
	man9/lis_sprintf.9.man \
	man9/lis_strm_name.9.man \
	man9/lis_strm_name_from_queue.9.man \
	man9/lis_stropts_file.9.man \
	man9/lis_strqget.9.man \
	man9/lis_strqset.9.man \
	man9/lis_testb.9.man \
	man9/lis_thread_start.9.man \
	man9/lis_thread_stop.9.man \
	man9/lis_timeout_fcn.9.man \
	man9/lis_udelay.9.man \
	man9/lis_unbufcall.9.man \
	man9/lis_unfreezestr.9.man \
	man9/lis_unlink.9.man \
	man9/lis_unlinkb.9.man \
	man9/lis_unregister_strdev.9.man \
	man9/lis_unregister_strmod.9.man \
	man9/lis_untimeout.9.man \
	man9/lis_up_fcn.9.man \
	man9/lis_usecs.9.man \
	man9/lis_usectohz.9.man \
	man9/lis_version.9.man \
	man9/lis_vfree.9.man \
	man9/lis_virt_to_phys.9.man \
	man9/lis_vmalloc.9.man \
	man9/lis_vremap.9.man \
	man9/lis_vsprintf.9.man \
	man9/lis_wake_up.9.man \
	man9/lis_wake_up_interruptible.9.man \
	man9/lis_xmsgsize.9.man \
	man9/lis_zmalloc.9.man

dist_noinst_lis_deprec_pages = \
	man9/lis_add_timer.9.man \
	man9/lis_allocq.9.man \
	man9/lis_apushm.9.man \
	man9/lis_basename.9.man \
	man9/lis_bprintf.9.man \
	man9/lis_can_unload.9.man \
	man9/lis_check_guard.9.man \
	man9/lis_check_mem.9.man \
	man9/lis_check_mem_region.9.man \
	man9/lis_check_q_magic.9.man \
	man9/lis_check_region.9.man \
	man9/lis_check_umem.9.man \
	man9/lis_copyin.9.man \
	man9/lis_copyout.9.man \
	man9/lis_dec_mod_cnt_fcn.9.man \
	man9/lis_del_timer.9.man \
	man9/lis_dobufcall.9.man \
	man9/lis_doclose.9.man \
	man9/lis_enable_intr.9.man \
	man9/lis_fattach.9.man \
	man9/lis_fdetach.9.man \
	man9/lis_fdetach_all.9.man \
	man9/lis_fdetach_stream.9.man \
	man9/lis_fifo_close_sync.9.man \
	man9/lis_fifo_open_sync.9.man \
	man9/lis_fifo_write_sync.9.man \
	man9/lis_file_inode.9.man \
	man9/lis_file_str.9.man \
	man9/lis_find_strdev.9.man \
	man9/lis_flush_print_buffer.9.man \
	man9/lis_fmod_sw.9.man \
	man9/lis_freedb.9.man \
	man9/lis_free_passfp.9.man \
	man9/lis_freeq.9.man \
	man9/lis_fstr_sw.9.man \
	man9/lis_get_fifo.9.man \
	man9/lis_get_passfp.9.man \
	man9/lis_get_pipe.9.man \
	man9/lis_inc_mod_cnt_fcn.9.man \
	man9/lis_init_bufcall.9.man \
	man9/lis_in_syscall.9.man \
	man9/lis_ioc_fattach.9.man \
	man9/lis_ioc_fdetach.9.man \
	man9/lis_ioc_pipe.9.man \
	man9/lis__kfree.9.man \
	man9/lis_kill_pg.9.man \
	man9/lis_kill_proc.9.man \
	man9/lis__kmalloc.9.man \
	man9/lis_loadable_load.9.man \
	man9/lis_maj_min_name.9.man \
	man9/lis_mark_mem.9.man \
	man9/lis_max_mem.9.man \
	man9/lis_old_inode.9.man \
	man9/lis_open_cnt.9.man \
	man9/lis_pci_cleanup.9.man \
	man9/lis_pipe.9.man \
	man9/lis_poll_2_1.9.man \
	man9/lis_poll_bits.9.man \
	man9/lis_poll_events.9.man \
	man9/lis_poll_file.9.man \
	man9/lis_print_queues.9.man \
	man9/lis_print_spl_track.9.man \
	man9/lis_print_stream.9.man \
	man9/lis_qattach.9.man \
	man9/lis_qdetach.9.man \
	man9/lis_queuerun_cnts.9.man \
	man9/lis_queues_running.9.man \
	man9/lis_recvfd.9.man \
	man9/lis_release_mem_region.9.man \
	man9/lis_request_mem_region.9.man \
	man9/lis_runq_cnts.9.man \
	man9/lis_runq_req_cnt.9.man \
	man9/lis_sendfd.9.man \
	man9/lis_set_file_str.9.man \
	man9/lis_setq.9.man \
	man9/lis_setqsched.9.man \
	man9/lis_setqsched_cnts.9.man \
	man9/lis_setqsched_isr_cnts.9.man \
	man9/lis_stdata_cnt.9.man \
	man9/lis_strclose.9.man \
	man9/lis_strcount.9.man \
	man9/lis_strgetpmsg.9.man \
	man9/lis_strioctl.9.man \
	man9/lis_stropen.9.man \
	man9/lis_strputpmsg.9.man \
	man9/lis_strread.9.man \
	man9/lis_strstats.9.man \
	man9/lis_strwrite.9.man \
	man9/lis_umount2.9.man \
	man9/lis_valid_mod_list.9.man

dist_noinst_lfs_compat_pages = \
	man9/lfscompat.9.man \
	man9/apush_get.9.man \
	man9/apush_set.9.man \
	man9/bcanget.9.man \
	man9/canget.9.man \
	man9/copyin.9.man \
	man9/copyout.9.man \
	man9/ctlmsg.9.man \
	man9/delay.9.man \
	man9/drv_getparm.9.man \
	man9/drv_hztomsec.9.man \
	man9/drv_hztousec.9.man \
	man9/drv_msectohz.9.man \
	man9/drv_priv.9.man \
	man9/drv_usectohz.9.man \
	man9/drv_usecwait.9.man \
	man9/enableq.9.man \
	man9/getadmin.9.man \
	man9/getmid.9.man \
	man9/linkmsg.9.man \
	man9/pcmsg.9.man \
	man9/putctl2.9.man \
	man9/putnextctl2.9.man \
	man9/strlog.9.man \
	man9/vstrlog_hook.9.man

dist_noinst_mac_compat_pages = \
	man9/maccompat.9.man \
	man9/allocbi.9.man \
	man9/esballoca.9.man \
	man9/mpnotify.9.man

dist_noinst_sux_compat_pages = \
	man9/suxcompat.9.man

dist_noinst_uxp_compat_pages = \
	man9/uxpcompat.9.man


dist_noinst_MANS = \
	man8/strcompat_mknod.8.man \
	man9/alloc_proto.9.man \
	man9/bufq_append.9.man \
	man9/bufq_dequeue.9.man \
	man9/bufq_dequeue_tail.9.man \
	man9/bufq_freehead.9.man \
	man9/bufq_head.9.man \
	man9/bufq_init.9.man \
	man9/bufq_insert.9.man \
	man9/bufq_length.9.man \
	man9/bufq_lock.9.man \
	man9/bufq_purge.9.man \
	man9/bufq_queue.9.man \
	man9/bufq_queue_head.9.man \
	man9/bufq_resupply.9.man \
	man9/bufq_size.9.man \
	man9/bufq_splice_head.9.man \
	man9/bufq_splice_tail.9.man \
	man9/bufq_supply.9.man \
	man9/bufq_tail.9.man \
	man9/bufq_unlink.9.man \
	man9/bufq_unlock.9.man \
	man9/compat_undoc.9.man \
	man9/freechunks.9.man \
	man9/mkiocb.9.man \
	man9/ss7_allocb.9.man \
	man9/ss7_bufcall.9.man \
	man9/ss7_bufpool_init.9.man \
	man9/ss7_bufpool_release.9.man \
	man9/ss7_bufpool_reserve.9.man \
	man9/ss7_bufpool_term.9.man \
	man9/ss7_bufsrv.9.man \
	man9/ss7_copyb.9.man \
	man9/ss7_copymsg.9.man \
	man9/ss7_do_timeout.9.man \
	man9/ss7_dupb.9.man \
	man9/ss7_dupmsg.9.man \
	man9/ss7_esballoc.9.man \
	man9/ss7_esbbcall.9.man \
	man9/ss7_fast_allocb.9.man \
	man9/ss7_fast_allocb_bh.9.man \
	man9/ss7_fast_freeb.9.man \
	man9/ss7_fast_freeb_bh.9.man \
	man9/ss7_fast_freemsg.9.man \
	man9/ss7_fast_freemsg_bh.9.man \
	man9/ss7_iput.9.man \
	man9/ss7_isrv.9.man \
	man9/ss7_oput.9.man \
	man9/ss7_osrv.9.man \
	man9/ss7_pullupmsg.9.man \
	man9/ss7_putq.9.man \
	man9/ss7_r_flush.9.man \
	man9/ss7_srvq.9.man \
	man9/ss7_start_timer.9.man \
	man9/ss7_stop_timer.9.man \
	man9/ss7_trylockq.9.man \
	man9/ss7_unbufcall.9.man \
	man9/ss7_unlockq.9.man \
	man9/ss7_w_flush.9.man \
	man9/str_uninstall.9.man

if CONFIG_STREAMS_COMPAT_SVR3
dist_noinst_MANS	    += $(dist_noinst_svr3_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_SVR4
dist_noinst_MANS	    += $(dist_noinst_svr4_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_MPS
dist_noinst_MANS	    += $(dist_noinst_mps_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_SUN
dist_noinst_MANS	    += $(dist_noinst_sun_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_UW7
dist_noinst_MANS	    += $(dist_noinst_uw7_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_OSF
dist_noinst_MANS	    += $(dist_noinst_osf_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_AIX
dist_noinst_MANS	    += $(dist_noinst_aix_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_HPUX
dist_noinst_MANS	    += $(dist_noinst_hpux_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_IRIX
dist_noinst_MANS	    += $(dist_noinst_irix_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_LIS
dist_noinst_MANS	    += $(dist_noinst_lis_compat_pages)
dist_noinst_MANS	    += $(dist_noinst_lis_deprec_pages)
endif
if CONFIG_STREAMS_COMPAT_LFS
dist_noinst_MANS	    += $(dist_noinst_lfs_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_MAC
dist_noinst_MANS	    += $(dist_noinst_mac_compat_pages)
endif

if CONFIG_STREAMS_COMPAT_SVR3_MODULE
dist_noinst_MANS	    += $(dist_noinst_svr3_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
dist_noinst_MANS	    += $(dist_noinst_svr4_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
dist_noinst_MANS	    += $(dist_noinst_mps_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
dist_noinst_MANS	    += $(dist_noinst_sun_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
dist_noinst_MANS	    += $(dist_noinst_uw7_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
dist_noinst_MANS	    += $(dist_noinst_osf_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
dist_noinst_MANS	    += $(dist_noinst_aix_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
dist_noinst_MANS	    += $(dist_noinst_hpux_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
dist_noinst_MANS	    += $(dist_noinst_irix_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
dist_noinst_MANS	    += $(dist_noinst_lis_compat_pages)
dist_noinst_MANS	    += $(dist_noinst_lis_deprec_pages)
endif
if CONFIG_STREAMS_COMPAT_LFS_MODULE
dist_noinst_MANS	    += $(dist_noinst_lfs_compat_pages)
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
dist_noinst_MANS	    += $(dist_noinst_mac_compat_pages)
endif


## PKG_BUILD_INDEP
endif

include ../../am/man.am

# vim: ft=automake
