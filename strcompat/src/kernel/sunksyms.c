/*****************************************************************************

 @(#) sunksyms.c,v (1.1.2.5) 2003/10/26 17:25:56

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2003  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified 2003/10/26 17:25:56 by brian

 *****************************************************************************/

#ident "@(#) sunksyms.c,v (1.1.2.5) 2003/10/26 17:25:56"

static char const ident[] =
    "sunksyms.c,v (1.1.2.5) 2003/10/26 17:25:56";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/module.h>
#include <linux/stddef.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/poll.h>

#define _SUN_SOURCE
#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strconf.h>
#include <linux/strsubr.h>
#include <linux/ddi.h>

#include "strdebug.h"

#if 0
EXPORT_SYMBOL_NOVERS(qbufcall);		/* sunddi.h see strsyms.c */
EXPORT_SYMBOL_NOVERS(qtimeout);		/* sunddi.h see strsyms.c */
EXPORT_SYMBOL_NOVERS(qunbufcall);	/* sunddi.h see strsyms.c */
EXPORT_SYMBOL_NOVERS(quntimeout);	/* sunddi.h see strsyms.c */
#endif
EXPORT_SYMBOL_NOVERS(qwait_sig);	/* sunddi.h */
EXPORT_SYMBOL_NOVERS(qwait);		/* sunddi.h */
EXPORT_SYMBOL_NOVERS(queclass);		/* sunddi.h */
// EXPORT_SYMBOL_NOVERS(qwriter);	/* sunddi.h see strsyms.c */
EXPORT_SYMBOL_NOVERS(ddi_get_cred);	/* sunddi.h */
EXPORT_SYMBOL_NOVERS(ddi_get_lbolt);	/* sunddi.h */
EXPORT_SYMBOL_NOVERS(ddi_get_pid);	/* sunddi.h */
EXPORT_SYMBOL_NOVERS(ddi_get_time);	/* sunddi.h */
EXPORT_SYMBOL_NOVERS(ddi_getiminor);	/* sunddi.h */
EXPORT_SYMBOL_NOVERS(ddi_umem_alloc);	/* sunddi.h */
EXPORT_SYMBOL_NOVERS(ddi_umem_free);	/* sunddi.h */

/* 
 *  Configuration stuff
 */
EXPORT_SYMBOL_NOVERS(mod_strmops);	/* strconf.h */
EXPORT_SYMBOL_NOVERS(nodev);		/* strconf.h */
EXPORT_SYMBOL_NOVERS(nulldev);		/* strconf.h */
EXPORT_SYMBOL_NOVERS(nochpoll);		/* strconf.h */
EXPORT_SYMBOL_NOVERS(ddi_prop_op);	/* strconf.h *//* dummy */
EXPORT_SYMBOL_NOVERS(mod_install);	/* strconf.h */
EXPORT_SYMBOL_NOVERS(mod_remove);	/* strconf.h */
EXPORT_SYMBOL_NOVERS(mod_info);		/* strconf.h */

#if 0
EXPORT_SYMBOL_NOVERS(ddi_add_intr);
EXPORT_SYMBOL_NOVERS(ddi_add_softintr);
EXPORT_SYMBOL_NOVERS(ddi_binding_name);
EXPORT_SYMBOL_NOVERS(ddi_btop);
EXPORT_SYMBOL_NOVERS(ddi_btopr);
EXPORT_SYMBOL_NOVERS(ddi_check_acc_handle);
EXPORT_SYMBOL_NOVERS(ddi_check_dma_handle);
EXPORT_SYMBOL_NOVERS(ddi_copyin);
EXPORT_SYMBOL_NOVERS(ddi_copyout);
EXPORT_SYMBOL_NOVERS(ddi_create_minor_node);
EXPORT_SYMBOL_NOVERS(ddi_dev_is_needed);
EXPORT_SYMBOL_NOVERS(ddi_dev_is_sid);
EXPORT_SYMBOL_NOVERS(ddi_dev_nintrs);
EXPORT_SYMBOL_NOVERS(ddi_dev_nregs);
EXPORT_SYMBOL_NOVERS(ddi_dev_regsize);
EXPORT_SYMBOL_NOVERS(ddi_dev_report_fault);
EXPORT_SYMBOL_NOVERS(ddi_device_copy);
EXPORT_SYMBOL_NOVERS(ddi_device_zero);
EXPORT_SYMBOL_NOVERS(ddi_devid_compare);
EXPORT_SYMBOL_NOVERS(ddi_devid_free);
EXPORT_SYMBOL_NOVERS(ddi_devid_init);
EXPORT_SYMBOL_NOVERS(ddi_devid_register);
EXPORT_SYMBOL_NOVERS(ddi_devid_sizeof);
EXPORT_SYMBOL_NOVERS(ddi_devid_str_decode);
EXPORT_SYMBOL_NOVERS(ddi_devid_str_encode);
EXPORT_SYMBOL_NOVERS(ddi_devid_str_free);
EXPORT_SYMBOL_NOVERS(ddi_devid_unregister);
EXPORT_SYMBOL_NOVERS(ddi_devid_valid);
EXPORT_SYMBOL_NOVERS(ddi_devmap_segmap);
EXPORT_SYMBOL_NOVERS(ddi_dma_addr_bind_handle);
EXPORT_SYMBOL_NOVERS(ddi_dma_addr_setup);
EXPORT_SYMBOL_NOVERS(ddi_dma_alloc_handle);
EXPORT_SYMBOL_NOVERS(ddi_dma_buf_bind_handle);
EXPORT_SYMBOL_NOVERS(ddi_dma_buf_setup);
EXPORT_SYMBOL_NOVERS(ddi_dma_burstsizes);
EXPORT_SYMBOL_NOVERS(ddi_dma_coff);
EXPORT_SYMBOL_NOVERS(ddi_dma_curwin);
EXPORT_SYMBOL_NOVERS(ddi_dma_devalign);
EXPORT_SYMBOL_NOVERS(ddi_dma_free);
EXPORT_SYMBOL_NOVERS(ddi_dma_free_handle);
EXPORT_SYMBOL_NOVERS(ddi_dma_get_attr);
EXPORT_SYMBOL_NOVERS(ddi_dma_getwin);
EXPORT_SYMBOL_NOVERS(ddi_dma_htoc);
EXPORT_SYMBOL_NOVERS(ddi_dma_mem_alloc);
EXPORT_SYMBOL_NOVERS(ddi_dma_mem_free);
EXPORT_SYMBOL_NOVERS(ddi_dma_movwin);
EXPORT_SYMBOL_NOVERS(ddi_dma_nextcookie);
EXPORT_SYMBOL_NOVERS(ddi_dma_nextseg);
EXPORT_SYMBOL_NOVERS(ddi_dma_nextwin);
EXPORT_SYMBOL_NOVERS(ddi_dma_numwin);
EXPORT_SYMBOL_NOVERS(ddi_dma_segtocookie);
EXPORT_SYMBOL_NOVERS(ddi_dma_set_sbus64);
EXPORT_SYMBOL_NOVERS(ddi_dma_setup);
EXPORT_SYMBOL_NOVERS(ddi_dma_sync);
EXPORT_SYMBOL_NOVERS(ddi_dma_unbind_handle);
EXPORT_SYMBOL_NOVERS(ddi_dmae);
EXPORT_SYMBOL_NOVERS(ddi_dmae_1stparty);
EXPORT_SYMBOL_NOVERS(ddi_dmae_alloc);
EXPORT_SYMBOL_NOVERS(ddi_dmae_disable);
EXPORT_SYMBOL_NOVERS(ddi_dmae_enable);
EXPORT_SYMBOL_NOVERS(ddi_dmae_getattr);
EXPORT_SYMBOL_NOVERS(ddi_dmae_getcnt);
EXPORT_SYMBOL_NOVERS(ddi_dmae_getlim);
EXPORT_SYMBOL_NOVERS(ddi_dmae_prog);
EXPORT_SYMBOL_NOVERS(ddi_dmae_release);
EXPORT_SYMBOL_NOVERS(ddi_dmae_stop);
EXPORT_SYMBOL_NOVERS(ddi_driver_major);
EXPORT_SYMBOL_NOVERS(ddi_driver_name);
EXPORT_SYMBOL_NOVERS(ddi_enter_critical);
EXPORT_SYMBOL_NOVERS(ddi_exit_critical);
EXPORT_SYMBOL_NOVERS(ddi_ffs);
EXPORT_SYMBOL_NOVERS(ddi_fls);
EXPORT_SYMBOL_NOVERS(ddi_get16);
EXPORT_SYMBOL_NOVERS(ddi_get32);
EXPORT_SYMBOL_NOVERS(ddi_get64);
EXPORT_SYMBOL_NOVERS(ddi_get8);
EXPORT_SYMBOL_NOVERS(ddi_get_cred);
EXPORT_SYMBOL_NOVERS(ddi_get_devstate);
EXPORT_SYMBOL_NOVERS(ddi_get_driver_private);
EXPORT_SYMBOL_NOVERS(ddi_get_iblock_cookie);
EXPORT_SYMBOL_NOVERS(ddi_get_instance);
EXPORT_SYMBOL_NOVERS(ddi_get_kt_did);
EXPORT_SYMBOL_NOVERS(ddi_get_lbolt);
EXPORT_SYMBOL_NOVERS(ddi_get_name);
EXPORT_SYMBOL_NOVERS(ddi_get_parent);
EXPORT_SYMBOL_NOVERS(ddi_get_pid);
EXPORT_SYMBOL_NOVERS(ddi_get_soft_iblock_cookie);
EXPORT_SYMBOL_NOVERS(ddi_get_soft_state);
EXPORT_SYMBOL_NOVERS(ddi_get_time);
EXPORT_SYMBOL_NOVERS(ddi_getb);
EXPORT_SYMBOL_NOVERS(ddi_getiminor);
EXPORT_SYMBOL_NOVERS(ddi_getl);
EXPORT_SYMBOL_NOVERS(ddi_getll);
EXPORT_SYMBOL_NOVERS(ddi_getlongprop);
EXPORT_SYMBOL_NOVERS(ddi_getlongprop_buf);
EXPORT_SYMBOL_NOVERS(ddi_getprop);
EXPORT_SYMBOL_NOVERS(ddi_getproplen);
EXPORT_SYMBOL_NOVERS(ddi_getw);
EXPORT_SYMBOL_NOVERS(ddi_in_panic);
EXPORT_SYMBOL_NOVERS(ddi_intr_hilevel);
EXPORT_SYMBOL_NOVERS(ddi_io_get16);
EXPORT_SYMBOL_NOVERS(ddi_io_get32);
EXPORT_SYMBOL_NOVERS(ddi_io_get64);
EXPORT_SYMBOL_NOVERS(ddi_io_get8);
EXPORT_SYMBOL_NOVERS(ddi_io_getb);
EXPORT_SYMBOL_NOVERS(ddi_io_getl);
EXPORT_SYMBOL_NOVERS(ddi_io_getw);
EXPORT_SYMBOL_NOVERS(ddi_io_put16);
EXPORT_SYMBOL_NOVERS(ddi_io_put32);
EXPORT_SYMBOL_NOVERS(ddi_io_put64);
EXPORT_SYMBOL_NOVERS(ddi_io_put8);
EXPORT_SYMBOL_NOVERS(ddi_io_putb);
EXPORT_SYMBOL_NOVERS(ddi_io_putl);
EXPORT_SYMBOL_NOVERS(ddi_io_putw);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_get16);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_get32);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_get64);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_get8);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_getb);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_getl);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_getw);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_put16);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_put32);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_put64);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_put8);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_putb);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_putl);
EXPORT_SYMBOL_NOVERS(ddi_io_rep_putw);
EXPORT_SYMBOL_NOVERS(ddi_iomin);
EXPORT_SYMBOL_NOVERS(ddi_iopb_alloc);
EXPORT_SYMBOL_NOVERS(ddi_iopb_free);
EXPORT_SYMBOL_NOVERS(ddi_log_sysevent);
EXPORT_SYMBOL_NOVERS(ddi_map_regs);
EXPORT_SYMBOL_NOVERS(ddi_mapdev);
EXPORT_SYMBOL_NOVERS(ddi_mapdev_intercept);
EXPORT_SYMBOL_NOVERS(ddi_mapdev_nointercept);
EXPORT_SYMBOL_NOVERS(ddi_mem_alloc);
EXPORT_SYMBOL_NOVERS(ddi_mem_free);
EXPORT_SYMBOL_NOVERS(ddi_mem_get16);
EXPORT_SYMBOL_NOVERS(ddi_mem_get32);
EXPORT_SYMBOL_NOVERS(ddi_mem_get64);
EXPORT_SYMBOL_NOVERS(ddi_mem_get8);
EXPORT_SYMBOL_NOVERS(ddi_mem_getb);
EXPORT_SYMBOL_NOVERS(ddi_mem_getl);
EXPORT_SYMBOL_NOVERS(ddi_mem_getll);
EXPORT_SYMBOL_NOVERS(ddi_mem_getw);
EXPORT_SYMBOL_NOVERS(ddi_mem_put16);
EXPORT_SYMBOL_NOVERS(ddi_mem_put32);
EXPORT_SYMBOL_NOVERS(ddi_mem_put64);
EXPORT_SYMBOL_NOVERS(ddi_mem_put8);
EXPORT_SYMBOL_NOVERS(ddi_mem_putb);
EXPORT_SYMBOL_NOVERS(ddi_mem_putl);
EXPORT_SYMBOL_NOVERS(ddi_mem_putll);
EXPORT_SYMBOL_NOVERS(ddi_mem_putw);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_get16);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_get32);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_get64);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_get8);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_getb);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_getl);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_getll);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_getw);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_put16);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_put32);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_put64);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_put8);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_putb);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_putl);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_putll);
EXPORT_SYMBOL_NOVERS(ddi_mem_rep_putw);
EXPORT_SYMBOL_NOVERS(ddi_mmap_get_model);
EXPORT_SYMBOL_NOVERS(ddi_model_convert_from);
EXPORT_SYMBOL_NOVERS(ddi_node_name);
EXPORT_SYMBOL_NOVERS(ddi_peek);
EXPORT_SYMBOL_NOVERS(ddi_peek16);
EXPORT_SYMBOL_NOVERS(ddi_peek32);
EXPORT_SYMBOL_NOVERS(ddi_peek64);
EXPORT_SYMBOL_NOVERS(ddi_peek8);
EXPORT_SYMBOL_NOVERS(ddi_peekc);
EXPORT_SYMBOL_NOVERS(ddi_peekd);
EXPORT_SYMBOL_NOVERS(ddi_peekl);
EXPORT_SYMBOL_NOVERS(ddi_peeks);
EXPORT_SYMBOL_NOVERS(ddi_poke);
EXPORT_SYMBOL_NOVERS(ddi_poke16);
EXPORT_SYMBOL_NOVERS(ddi_poke32);
EXPORT_SYMBOL_NOVERS(ddi_poke64);
EXPORT_SYMBOL_NOVERS(ddi_poke8);
EXPORT_SYMBOL_NOVERS(ddi_pokec);
EXPORT_SYMBOL_NOVERS(ddi_poked);
EXPORT_SYMBOL_NOVERS(ddi_pokel);
EXPORT_SYMBOL_NOVERS(ddi_pokes);
EXPORT_SYMBOL_NOVERS(ddi_prop_create);
EXPORT_SYMBOL_NOVERS(ddi_prop_exists);
EXPORT_SYMBOL_NOVERS(ddi_prop_free);
EXPORT_SYMBOL_NOVERS(ddi_prop_get_int);
EXPORT_SYMBOL_NOVERS(ddi_prop_get_int64);
EXPORT_SYMBOL_NOVERS(ddi_prop_lookup);
EXPORT_SYMBOL_NOVERS(ddi_prop_lookup_byte_array);
EXPORT_SYMBOL_NOVERS(ddi_prop_lookup_int64_array);
EXPORT_SYMBOL_NOVERS(ddi_prop_lookup_int_array);
EXPORT_SYMBOL_NOVERS(ddi_prop_lookup_string);
EXPORT_SYMBOL_NOVERS(ddi_prop_lookup_string_array);
EXPORT_SYMBOL_NOVERS(ddi_prop_modify);
EXPORT_SYMBOL_NOVERS(ddi_prop_op);
EXPORT_SYMBOL_NOVERS(ddi_prop_remove);
EXPORT_SYMBOL_NOVERS(ddi_prop_remove_all);
EXPORT_SYMBOL_NOVERS(ddi_prop_undefine);
EXPORT_SYMBOL_NOVERS(ddi_prop_update);
EXPORT_SYMBOL_NOVERS(ddi_prop_update_byte_array);
EXPORT_SYMBOL_NOVERS(ddi_prop_update_int);
EXPORT_SYMBOL_NOVERS(ddi_prop_update_int64);
EXPORT_SYMBOL_NOVERS(ddi_prop_update_int64_array);
EXPORT_SYMBOL_NOVERS(ddi_prop_update_string);
EXPORT_SYMBOL_NOVERS(ddi_prop_update_string_array);
EXPORT_SYMBOL_NOVERS(ddi_ptob);
EXPORT_SYMBOL_NOVERS(ddi_put16);
EXPORT_SYMBOL_NOVERS(ddi_put32);
EXPORT_SYMBOL_NOVERS(ddi_put64);
EXPORT_SYMBOL_NOVERS(ddi_put8);
EXPORT_SYMBOL_NOVERS(ddi_putb);
EXPORT_SYMBOL_NOVERS(ddi_putl);
EXPORT_SYMBOL_NOVERS(ddi_putll);
EXPORT_SYMBOL_NOVERS(ddi_putw);
EXPORT_SYMBOL_NOVERS(ddi_regs_map_free);
EXPORT_SYMBOL_NOVERS(ddi_regs_map_setup);
EXPORT_SYMBOL_NOVERS(ddi_remove_intr);
EXPORT_SYMBOL_NOVERS(ddi_remove_minor_node);
EXPORT_SYMBOL_NOVERS(ddi_remove_softintr);
EXPORT_SYMBOL_NOVERS(ddi_removing_power);
EXPORT_SYMBOL_NOVERS(ddi_rep_get16);
EXPORT_SYMBOL_NOVERS(ddi_rep_get32);
EXPORT_SYMBOL_NOVERS(ddi_rep_get64);
EXPORT_SYMBOL_NOVERS(ddi_rep_get8);
EXPORT_SYMBOL_NOVERS(ddi_rep_getb);
EXPORT_SYMBOL_NOVERS(ddi_rep_getl);
EXPORT_SYMBOL_NOVERS(ddi_rep_getll);
EXPORT_SYMBOL_NOVERS(ddi_rep_getw);
EXPORT_SYMBOL_NOVERS(ddi_rep_put16);
EXPORT_SYMBOL_NOVERS(ddi_rep_put32);
EXPORT_SYMBOL_NOVERS(ddi_rep_put64);
EXPORT_SYMBOL_NOVERS(ddi_rep_put8);
EXPORT_SYMBOL_NOVERS(ddi_rep_putb);
EXPORT_SYMBOL_NOVERS(ddi_rep_putl);
EXPORT_SYMBOL_NOVERS(ddi_rep_putll);
EXPORT_SYMBOL_NOVERS(ddi_rep_putw);
EXPORT_SYMBOL_NOVERS(ddi_report_dev);
EXPORT_SYMBOL_NOVERS(ddi_root_node);
EXPORT_SYMBOL_NOVERS(ddi_segmap);
EXPORT_SYMBOL_NOVERS(ddi_segmap_setup);
EXPORT_SYMBOL_NOVERS(ddi_set_driver_private);
EXPORT_SYMBOL_NOVERS(ddi_slaveonly);
EXPORT_SYMBOL_NOVERS(ddi_soft_state);
EXPORT_SYMBOL_NOVERS(ddi_soft_state_fini);
EXPORT_SYMBOL_NOVERS(ddi_soft_state_free);
EXPORT_SYMBOL_NOVERS(ddi_soft_state_init);
EXPORT_SYMBOL_NOVERS(ddi_soft_state_zalloc);
EXPORT_SYMBOL_NOVERS(ddi_trigger_softintr);
EXPORT_SYMBOL_NOVERS(ddi_umem_alloc);
EXPORT_SYMBOL_NOVERS(ddi_umem_free);
EXPORT_SYMBOL_NOVERS(ddi_umem_lock);
EXPORT_SYMBOL_NOVERS(ddi_umem_unlock);
EXPORT_SYMBOL_NOVERS(ddi_unmap_regs);
#endif
