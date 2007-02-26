# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/02/25 12:44:57 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 2 of the License.
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
# Last Modified $Date: 2007/02/25 12:44:57 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.1  2007/02/25 12:44:57  brian
# - added new files for release package
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

man_MANS = \
	man4/ctimod.4 \
	man5/osr61.5 \
	man8/osr61_mknod.8 \
	man8/test-osr61.8 \
	man9/ctimod.9 \
	man9/ctimod_add_timer.9 \
	man9/ctimod_add_wait_queue.9 \
	man9/ctimod_bzero.9 \
	man9/ctimod_check_rh.9 \
	man9/ctimod_copy_from_user.9 \
	man9/ctimod_copy_to_user.9 \
	man9/ctimod_create_proc_entry.9 \
	man9/ctimod_create_workqueue.9 \
	man9/ctimod_debug_clear.9 \
	man9/ctimod_debug_off.9 \
	man9/ctimod_debug_on.9 \
	man9/ctimod_debug_print.9 \
	man9/ctimod_debug_toggle.9 \
	man9/ctimod_del_timer.9 \
	man9/ctimod_do_gettimeofday.9 \
	man9/ctimod_free_irq.9 \
	man9/ctimod_free_pages.9 \
	man9/ctimod_get_free_pages.9 \
	man9/ctimod_init_timer.9 \
	man9/ctimod_init_waitqueue_head.9 \
	man9/ctimod_ioremap.9 \
	man9/ctimod_iounmap.9 \
	man9/ctimod_jiffies.9 \
	man9/ctimod_kfree.9 \
	man9/ctimod_kmalloc.9 \
	man9/ctimod_mark_bh.9 \
	man9/ctimod_memcpy.9 \
	man9/ctimod_memset.9 \
	man9/ctimod_pcibios_present.9 \
	man9/ctimod_pci_find_device.9 \
	man9/ctimod_pci_module_init.9 \
	man9/ctimod_pci_read_config_byte.9 \
	man9/ctimod_pci_read_config_dword.9 \
	man9/ctimod_pci_read_config_word.9 \
	man9/ctimod_pci_register_driver.9 \
	man9/ctimod_pci_unregister_driver.9 \
	man9/ctimod_pci_write_config_byte.9 \
	man9/ctimod_pci_write_config_dword.9 \
	man9/ctimod_pci_write_config_word.9 \
	man9/ctimod_poll_wait.9 \
	man9/ctimod_printk.9 \
	man9/ctimod_queue_work.9 \
	man9/ctimod_register_chrdev.9 \
	man9/ctimod_remap_page_range.9 \
	man9/ctimod_remove_proc_entry.9 \
	man9/ctimod_remove_wait_queue.9 \
	man9/ctimod_request_irq.9 \
	man9/ctimod_schedule.9 \
	man9/ctimod_schedule_timeout.9 \
	man9/ctimod_spin_lock.9 \
	man9/ctimod_spin_lock_bh.9 \
	man9/ctimod_spin_lock_init.9 \
	man9/ctimod_spin_lock_irq.9 \
	man9/ctimod_spin_lock_irqsave.9 \
	man9/ctimod_spin_trylock.9 \
	man9/ctimod_spin_unlock.9 \
	man9/ctimod_spin_unlock_bh.9 \
	man9/ctimod_spin_unlock_irq.9 \
	man9/ctimod_spin_unlock_irqrestore.9 \
	man9/ctimod_sprintf.9 \
	man9/ctimod_tasklet_init.9 \
	man9/ctimod_tasklet_kill.9 \
	man9/ctimod_udelay.9 \
	man9/ctimod_unregister_chrdev.9 \
	man9/ctimod_virt_to_page.9 \
	man9/ctimod_vsprintf.9 \
	man9/ctimod_wake_up_interruptible.9 \
	man9/ctimod_wake_up_process.9

if WITH_DLGN
man_MANS += \
	man4/dlgn.4 \
	man4/dvbm.4 \
	man4/gncfd.4 \
	man4/gpio.4 \
	man4/sctmr.4
endif
if WITH_MERC
man_MANS += \
	man4/mercd.4
endif
if WITH_PMAC
man_MANS += \
	man4/pmacd.4
endif

dist_noinst_MANS = \
	man4/ctimod.4.man \
	man5/osr61.5.man \
	man8/osr61_mknod.8.man \
	man8/test-osr61.8.man \
	man9/ctimod.9.man \
	man9/ctimod_add_timer.9.man \
	man9/ctimod_add_wait_queue.9.man \
	man9/ctimod_bzero.9.man \
	man9/ctimod_check_rh.9.man \
	man9/ctimod_copy_from_user.9.man \
	man9/ctimod_copy_to_user.9.man \
	man9/ctimod_create_proc_entry.9.man \
	man9/ctimod_create_workqueue.9.man \
	man9/ctimod_debug_clear.9.man \
	man9/ctimod_debug_off.9.man \
	man9/ctimod_debug_on.9.man \
	man9/ctimod_debug_print.9.man \
	man9/ctimod_debug_toggle.9.man \
	man9/ctimod_del_timer.9.man \
	man9/ctimod_do_gettimeofday.9.man \
	man9/ctimod_free_irq.9.man \
	man9/ctimod_free_pages.9.man \
	man9/ctimod_get_free_pages.9.man \
	man9/ctimod_init_timer.9.man \
	man9/ctimod_init_waitqueue_head.9.man \
	man9/ctimod_ioremap.9.man \
	man9/ctimod_iounmap.9.man \
	man9/ctimod_jiffies.9.man \
	man9/ctimod_kfree.9.man \
	man9/ctimod_kmalloc.9.man \
	man9/ctimod_mark_bh.9.man \
	man9/ctimod_memcpy.9.man \
	man9/ctimod_memset.9.man \
	man9/ctimod_pcibios_present.9.man \
	man9/ctimod_pci_find_device.9.man \
	man9/ctimod_pci_module_init.9.man \
	man9/ctimod_pci_read_config_byte.9.man \
	man9/ctimod_pci_read_config_dword.9.man \
	man9/ctimod_pci_read_config_word.9.man \
	man9/ctimod_pci_register_driver.9.man \
	man9/ctimod_pci_unregister_driver.9.man \
	man9/ctimod_pci_write_config_byte.9.man \
	man9/ctimod_pci_write_config_dword.9.man \
	man9/ctimod_pci_write_config_word.9.man \
	man9/ctimod_poll_wait.9.man \
	man9/ctimod_printk.9.man \
	man9/ctimod_queue_work.9.man \
	man9/ctimod_register_chrdev.9.man \
	man9/ctimod_remap_page_range.9.man \
	man9/ctimod_remove_proc_entry.9.man \
	man9/ctimod_remove_wait_queue.9.man \
	man9/ctimod_request_irq.9.man \
	man9/ctimod_schedule.9.man \
	man9/ctimod_schedule_timeout.9.man \
	man9/ctimod_spin_lock.9.man \
	man9/ctimod_spin_lock_bh.9.man \
	man9/ctimod_spin_lock_init.9.man \
	man9/ctimod_spin_lock_irq.9.man \
	man9/ctimod_spin_lock_irqsave.9.man \
	man9/ctimod_spin_trylock.9.man \
	man9/ctimod_spin_unlock.9.man \
	man9/ctimod_spin_unlock_bh.9.man \
	man9/ctimod_spin_unlock_irq.9.man \
	man9/ctimod_spin_unlock_irqrestore.9.man \
	man9/ctimod_sprintf.9.man \
	man9/ctimod_tasklet_init.9.man \
	man9/ctimod_tasklet_kill.9.man \
	man9/ctimod_udelay.9.man \
	man9/ctimod_unregister_chrdev.9.man \
	man9/ctimod_virt_to_page.9.man \
	man9/ctimod_vsprintf.9.man \
	man9/ctimod_wake_up_interruptible.9.man \
	man9/ctimod_wake_up_process.9.man

if WITH_DLGN
dist_noinst_MANS += \
	man4/dlgn.4.man \
	man4/dvbm.4.man \
	man4/gncfd.4.man \
	man4/gpio.4.man \
	man4/sctmr.4.man 
endif
if WITH_MERC
dist_noinst_MANS += \
	man4/mercd.4.man
endif
if WITH_PMAC
dist_noinst_MANS += \
	man4/pmacd.4.man
endif

MANS_UNUSED = \
	man5/osr61.5

## PKG_BUILD_INDEP
endif

include ../../am/man.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
