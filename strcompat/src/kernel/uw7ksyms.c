/*****************************************************************************

 @(#) uw7ksyms.c,v (1.1.2.4) 2003/10/21 21:50:23

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

 Last Modified 2003/10/21 21:50:23 by brian

 *****************************************************************************/

#ident "@(#) uw7ksyms.c,v (1.1.2.4) 2003/10/21 21:50:23"

static char const ident[] =
    "uw7ksyms.c,v (1.1.2.4) 2003/10/21 21:50:23";

#define __NO_VERSION__

#include <linux/config.h>
#include <linux/module.h>
#include <linux/stddef.h>
#include <linux/types.h>
#include <linux/timer.h>
#include <linux/poll.h>

#define _UW7_SOURCE
#include <linux/stropts.h>
#include <linux/stream.h>
#include <linux/strconf.h>
#include <linux/strsubr.h>
#include <linux/ddi.h>

#include "strdebug.h"

/* from uw7ddi.h */
EXPORT_SYMBOL_NOVERS(getemajor);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(emajor);			/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(geteminor);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(eminor);			/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(etoimajor);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(itoemajor);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(printf);			/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(LOCK_OWNED);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(RW_ALLOC);			/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(RW_DEALLOC);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(RW_RDLOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(RW_TRYRDLOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(RW_TRYWRLOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(RW_UNLOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(RW_WRLOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(MPSTR_QLOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(MPSTR_QRELE);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(MPSTR_STPLOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(MPSTR_STPRELE);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(allocb_physreq);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(msgphysreq);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(msgpullup_physreq);	/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(msgscgth);			/* uw7ddi.h */
// EXPORT_SYMBOL_NOVERS(strioccall);		/* uw7ddi.h see strsyms.c */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_ADD);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_ALLOC);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_DEALLOC);	/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_DECR);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_INCR);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_INIT);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_READ);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_SUB);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(ATOMIC_INT_WRITE);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(SLEEP_ALLOC);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(SLEEP_DEALLOC);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(SLEEP_LOCKAVAIL);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(SLEEP_LOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(SLEEP_LOCKOWNED);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(SLEEP_LOCK_SIG);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(SLEEP_TRYLOCK);		/* uw7ddi.h */
EXPORT_SYMBOL_NOVERS(SLEEP_UNLOCK);		/* uw7ddi.h */

#if 0

EXPORT_SYMBOL_NOVERS(add_intr_handler);		/* D3 */
EXPORT_SYMBOL_NOVERS(add_nmi_handler);		/* D3 */
EXPORT_SYMBOL_NOVERS(all_io);			/* D3 */
EXPORT_SYMBOL_NOVERS(archtobus);		/* D3 */
EXPORT_SYMBOL_NOVERS(atomic);			/* D3 */
EXPORT_SYMBOL_NOVERS(bcb_alloc);		/* D3 */
EXPORT_SYMBOL_NOVERS(bcb_free);			/* D3 */
EXPORT_SYMBOL_NOVERS(bcb_prep);			/* D3 */
EXPORT_SYMBOL_NOVERS(bcmp);			/* D3 */
EXPORT_SYMBOL_NOVERS(bdistributed);		/* D3 */
EXPORT_SYMBOL_NOVERS(biocanblock);		/* D3 */
EXPORT_SYMBOL_NOVERS(biodone);			/* D3 */
EXPORT_SYMBOL_NOVERS(bioerror);			/* D3 */
EXPORT_SYMBOL_NOVERS(bioreset);			/* D3 */
EXPORT_SYMBOL_NOVERS(biowait);			/* D3 */
EXPORT_SYMBOL_NOVERS(biowait_sig);		/* D3 */
EXPORT_SYMBOL_NOVERS(bp_mapin);			/* D3 */
EXPORT_SYMBOL_NOVERS(bp_mapout);		/* D3 */
EXPORT_SYMBOL_NOVERS(brelse);			/* D3 */
EXPORT_SYMBOL_NOVERS(bs_getval);		/* D3 */
EXPORT_SYMBOL_NOVERS(btoc);			/* D3 */
EXPORT_SYMBOL_NOVERS(btop);			/* D3 */
EXPORT_SYMBOL_NOVERS(btopr);			/* D3 */
EXPORT_SYMBOL_NOVERS(buf_breakup);		/* D3 */
EXPORT_SYMBOL_NOVERS(calldebug);		/* D3 */
EXPORT_SYMBOL_NOVERS(call_demon);		/* D3 */
EXPORT_SYMBOL_NOVERS(can_doio);			/* D3 */
EXPORT_SYMBOL_NOVERS(cdistributed);		/* D3 */
EXPORT_SYMBOL_NOVERS(clockb);			/* D3 */
EXPORT_SYMBOL_NOVERS(clrbuf);			/* D3 */
EXPORT_SYMBOL_NOVERS(cm_abort_trans);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_addval);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_AT_putconf);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_begin_trans);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_delkey);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_delval);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_devconfig_size);	/* D3 */
EXPORT_SYMBOL_NOVERS(cm_end_trans);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_getbrdkey);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_getnbrd);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_getval);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_getversion);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_intr_attach);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_intr_detach);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_newkey);		/* D3 */
EXPORT_SYMBOL_NOVERS(cm_read_devconfig16);	/* D3 */
EXPORT_SYMBOL_NOVERS(cm_read_devconfig32);	/* D3 */
EXPORT_SYMBOL_NOVERS(cm_read_devconfig8);	/* D3 */
EXPORT_SYMBOL_NOVERS(cm_read_devconfig);	/* D3 */
EXPORT_SYMBOL_NOVERS(cm_write_devconfig16);	/* D3 */
EXPORT_SYMBOL_NOVERS(cm_write_devconfig32);	/* D3 */
EXPORT_SYMBOL_NOVERS(cm_write_devconfig8);	/* D3 */
EXPORT_SYMBOL_NOVERS(cm_write_devconfig);	/* D3 */
EXPORT_SYMBOL_NOVERS(copyio);			/* D3 */
EXPORT_SYMBOL_NOVERS(cpass);			/* D3 */
EXPORT_SYMBOL_NOVERS(db_alloc);			/* D3 */
EXPORT_SYMBOL_NOVERS(db_read);			/* D3 */
EXPORT_SYMBOL_NOVERS(db_write);			/* D3 */
EXPORT_SYMBOL_NOVERS(deverr);			/* D3 */
EXPORT_SYMBOL_NOVERS(devmem_mapin);		/* D3 */
EXPORT_SYMBOL_NOVERS(devmem_mapout);		/* D3 */
EXPORT_SYMBOL_NOVERS(devmem_ppid);		/* D3 */
EXPORT_SYMBOL_NOVERS(devmem_size);		/* D3 */
EXPORT_SYMBOL_NOVERS(disksort);			/* D3 */
EXPORT_SYMBOL_NOVERS(dma_alloc);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_breakup);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_cascade);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_disable);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_enable);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_free_buf);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_free_cb);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_get_best_mode);	/* D3 */
EXPORT_SYMBOL_NOVERS(dma_get_buf);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_get_cb);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_pageio);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_param);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_physreq);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_prog);			/* D3 */
EXPORT_SYMBOL_NOVERS(dma_relse);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_resid);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_start);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_stop);			/* D3 */
EXPORT_SYMBOL_NOVERS(dma_swsetup);		/* D3 */
EXPORT_SYMBOL_NOVERS(dma_swstart);		/* D3 */
EXPORT_SYMBOL_NOVERS(do_biostart);		/* D3 */
EXPORT_SYMBOL_NOVERS(do_devinfo);		/* D3 */
EXPORT_SYMBOL_NOVERS(do_drvctl);		/* D3 */
EXPORT_SYMBOL_NOVERS(do_ioctl);			/* D3 */
EXPORT_SYMBOL_NOVERS(drv_attach);		/* D3 */
EXPORT_SYMBOL_NOVERS(drv_callback);		/* D3 */
EXPORT_SYMBOL_NOVERS(drv_close);		/* D3 */
EXPORT_SYMBOL_NOVERS(drv_detach);		/* D3 */
EXPORT_SYMBOL_NOVERS(drv_gethardware);		/* D3 */
EXPORT_SYMBOL_NOVERS(drv_mmap);			/* D3 */
EXPORT_SYMBOL_NOVERS(drv_munmap);		/* D3 */
EXPORT_SYMBOL_NOVERS(drv_open);			/* D3 */
EXPORT_SYMBOL_NOVERS(drv_setparm);		/* D3 */
EXPORT_SYMBOL_NOVERS(drv_shutdown);		/* D3 */
EXPORT_SYMBOL_NOVERS(dtimeout);			/* D3 */
EXPORT_SYMBOL_NOVERS(eisa_parse_devconfig);	/* D3 */
EXPORT_SYMBOL_NOVERS(emdupmap);			/* D3 */
EXPORT_SYMBOL_NOVERS(emunmap);			/* D3 */
EXPORT_SYMBOL_NOVERS(flushtlb);			/* D3 */
EXPORT_SYMBOL_NOVERS(freerbuf);			/* D3 */
EXPORT_SYMBOL_NOVERS(fubyte);			/* D3 */
EXPORT_SYMBOL_NOVERS(fuword);			/* D3 */
EXPORT_SYMBOL_NOVERS(getbsvalue);		/* D3 */
EXPORT_SYMBOL_NOVERS(getc);			/* D3 */
EXPORT_SYMBOL_NOVERS(getchar);			/* D3 */
EXPORT_SYMBOL_NOVERS(getcpages);		/* D3 */
EXPORT_SYMBOL_NOVERS(geteblk);			/* D3 */
EXPORT_SYMBOL_NOVERS(geterror);			/* D3 */
EXPORT_SYMBOL_NOVERS(get_intr_arg);		/* D3 */
EXPORT_SYMBOL_NOVERS(getnextpg);		/* D3 */
EXPORT_SYMBOL_NOVERS(getpl);			/* D3 */
EXPORT_SYMBOL_NOVERS(getrbuf);			/* D3 */
EXPORT_SYMBOL_NOVERS(hat_getkpfnum);		/* D3 */
EXPORT_SYMBOL_NOVERS(hat_getppfnum);		/* D3 */
EXPORT_SYMBOL_NOVERS(idistributed);		/* D3 */
EXPORT_SYMBOL_NOVERS(ifstats_attach);		/* D3 */
EXPORT_SYMBOL_NOVERS(ifstats_detach);		/* D3 */
EXPORT_SYMBOL_NOVERS(ilockb);			/* D3 */
EXPORT_SYMBOL_NOVERS(intralloc);		/* D3 */
EXPORT_SYMBOL_NOVERS(iobitmapctl);		/* D3 */
EXPORT_SYMBOL_NOVERS(iodone);			/* D3 */
EXPORT_SYMBOL_NOVERS(iomove);			/* D3 */
EXPORT_SYMBOL_NOVERS(iopolldistributed);	/* D3 */
EXPORT_SYMBOL_NOVERS(iowait);			/* D3 */
EXPORT_SYMBOL_NOVERS(ismpx);			/* D3 */
EXPORT_SYMBOL_NOVERS(kmem_alloc_physcontig);	/* D3 */
EXPORT_SYMBOL_NOVERS(kmem_alloc_phys);		/* D3 */
EXPORT_SYMBOL_NOVERS(kmem_alloc_physreq);	/* D3 */
EXPORT_SYMBOL_NOVERS(kmem_free_physcontig);	/* D3 */
EXPORT_SYMBOL_NOVERS(kmem_zalloc_physreq);	/* D3 */
EXPORT_SYMBOL_NOVERS(KS_HOLD0LOCKS);		/* D3 */
EXPORT_SYMBOL_NOVERS(kthread_exit);		/* D3 */
EXPORT_SYMBOL_NOVERS(kthread_spawn);		/* D3 */
EXPORT_SYMBOL_NOVERS(kthread_wait);		/* D3 */
EXPORT_SYMBOL_NOVERS(kvtoppid);			/* D3 */
EXPORT_SYMBOL_NOVERS(ldistributed);		/* D3 */
EXPORT_SYMBOL_NOVERS(lockb);			/* D3 */
EXPORT_SYMBOL_NOVERS(memget);			/* D3 */
EXPORT_SYMBOL_NOVERS(met_ds_alloc_stats);	/* D3 */
EXPORT_SYMBOL_NOVERS(met_ds_dealloc_stats);	/* D3 */
EXPORT_SYMBOL_NOVERS(met_ds_dequeued);		/* D3 */
EXPORT_SYMBOL_NOVERS(met_ds_hist_stats);	/* D3 */
EXPORT_SYMBOL_NOVERS(met_ds_iodone);		/* D3 */
EXPORT_SYMBOL_NOVERS(met_ds_queued);		/* D3 */
EXPORT_SYMBOL_NOVERS(mod_drvattach);		/* D3 */
EXPORT_SYMBOL_NOVERS(mod_drvdetach);		/* D3 */
EXPORT_SYMBOL_NOVERS(ngeteblk);			/* D3 */
EXPORT_SYMBOL_NOVERS(numtos);			/* D3 */
EXPORT_SYMBOL_NOVERS(ind);			/* D3 */
EXPORT_SYMBOL_NOVERS(ovbcopy);			/* D3 */
EXPORT_SYMBOL_NOVERS(pci_buspresent);		/* D3 */
EXPORT_SYMBOL_NOVERS(pci_findclass);		/* D3 */
EXPORT_SYMBOL_NOVERS(pci_finddevice);		/* D3 */
EXPORT_SYMBOL_NOVERS(pci_read);			/* D3 */
EXPORT_SYMBOL_NOVERS(pci_search);		/* D3 */
EXPORT_SYMBOL_NOVERS(pci_specialcycle);		/* D3 */
EXPORT_SYMBOL_NOVERS(pci_transbase);		/* D3 */
EXPORT_SYMBOL_NOVERS(pci_write);		/* D3 */
EXPORT_SYMBOL_NOVERS(phalloc);			/* D3 */
EXPORT_SYMBOL_NOVERS(phfree);			/* D3 */
EXPORT_SYMBOL_NOVERS(physiock);			/* D3 */
EXPORT_SYMBOL_NOVERS(physio);			/* D3 */
EXPORT_SYMBOL_NOVERS(physmap);			/* D3 */
EXPORT_SYMBOL_NOVERS(physmap_free);		/* D3 */
EXPORT_SYMBOL_NOVERS(physreq_alloc);		/* D3 */
EXPORT_SYMBOL_NOVERS(physreq_free);		/* D3 */
EXPORT_SYMBOL_NOVERS(physreq_prep);		/* D3 */
EXPORT_SYMBOL_NOVERS(phystokv);			/* D3 */
EXPORT_SYMBOL_NOVERS(phystoppid);		/* D3 */
EXPORT_SYMBOL_NOVERS(pio_breakup);		/* D3 */
EXPORT_SYMBOL_NOVERS(pollwakeup);		/* D3 */
EXPORT_SYMBOL_NOVERS(pptophys);			/* D3 */
EXPORT_SYMBOL_NOVERS(printcfg);			/* D3 */
EXPORT_SYMBOL_NOVERS(pr_intr_adderr);		/* D3 */
EXPORT_SYMBOL_NOVERS(pr_intr_rmerr);		/* D3 */
EXPORT_SYMBOL_NOVERS(proc_ref);			/* D3 */
EXPORT_SYMBOL_NOVERS(proc_signal);		/* D3 */
EXPORT_SYMBOL_NOVERS(proc_unref);		/* D3 */
EXPORT_SYMBOL_NOVERS(proc_valid);		/* D3 */
EXPORT_SYMBOL_NOVERS(psignal);			/* D3 */
EXPORT_SYMBOL_NOVERS(ptob);			/* D3 */
EXPORT_SYMBOL_NOVERS(putc);			/* D3 */
EXPORT_SYMBOL_NOVERS(putchar);			/* D3 */
EXPORT_SYMBOL_NOVERS(rdma_filter);		/* D3 */
EXPORT_SYMBOL_NOVERS(remap_driver_cpu);		/* D3 */
EXPORT_SYMBOL_NOVERS(remove_intr_handler);	/* D3 */
EXPORT_SYMBOL_NOVERS(remove_nmi_handler);	/* D3 */
EXPORT_SYMBOL_NOVERS(repinsb);			/* D3 */
EXPORT_SYMBOL_NOVERS(repins);			/* D3 */
EXPORT_SYMBOL_NOVERS(repinsd);			/* D3 */
EXPORT_SYMBOL_NOVERS(repinsw);			/* D3 */
EXPORT_SYMBOL_NOVERS(repoutsb);			/* D3 */
EXPORT_SYMBOL_NOVERS(repoutsd);			/* D3 */
EXPORT_SYMBOL_NOVERS(repoutsw);			/* D3 */
EXPORT_SYMBOL_NOVERS(rmalloc);			/* D3 */
EXPORT_SYMBOL_NOVERS(rmallocmap);		/* D3 */
EXPORT_SYMBOL_NOVERS(rmalloc_wait);		/* D3 */
EXPORT_SYMBOL_NOVERS(rmfree);			/* D3 */
EXPORT_SYMBOL_NOVERS(rmfreemap);		/* D3 */
EXPORT_SYMBOL_NOVERS(rminit);			/* D3 */
EXPORT_SYMBOL_NOVERS(rmsetwant);		/* D3 */
EXPORT_SYMBOL_NOVERS(run_ld);			/* D3 */
EXPORT_SYMBOL_NOVERS(scgth_free);		/* D3 */
EXPORT_SYMBOL_NOVERS(sdistributed);		/* D3 */
EXPORT_SYMBOL_NOVERS(select);			/* D3 */
EXPORT_SYMBOL_NOVERS(seterror);			/* D3 */
EXPORT_SYMBOL_NOVERS(signal);			/* D3 */
EXPORT_SYMBOL_NOVERS(sptalloc);			/* D3 */
EXPORT_SYMBOL_NOVERS(sptfree);			/* D3 */
EXPORT_SYMBOL_NOVERS(startio);			/* D3 */
EXPORT_SYMBOL_NOVERS(subyte);			/* D3 */
EXPORT_SYMBOL_NOVERS(surf);			/* D3 */
EXPORT_SYMBOL_NOVERS(suser);			/* D3 */
EXPORT_SYMBOL_NOVERS(suspend);			/* D3 */
EXPORT_SYMBOL_NOVERS(suword);			/* D3 */
EXPORT_SYMBOL_NOVERS(tc_tlock);			/* D3 */
EXPORT_SYMBOL_NOVERS(TICKS);			/* D3 */
EXPORT_SYMBOL_NOVERS(ttiocom);			/* D3 */
EXPORT_SYMBOL_NOVERS(tty);			/* D3 */
EXPORT_SYMBOL_NOVERS(uiobuf);			/* D3 */
EXPORT_SYMBOL_NOVERS(uiomove);			/* D3 */
EXPORT_SYMBOL_NOVERS(ups);			/* D3 */
EXPORT_SYMBOL_NOVERS(ureadc);			/* D3 */
EXPORT_SYMBOL_NOVERS(uwritec);			/* D3 */
EXPORT_SYMBOL_NOVERS(vas);			/* D3 */
EXPORT_SYMBOL_NOVERS(video);			/* D3 */
#endif

#if 0
EXPORT_SYMBOL_NOVERS(ASSERT);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(itimeout);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(LOCK_ALLOC);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(LOCK_DEALLOC);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(LOCK);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(major);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(makedev);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(minor);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(sleep);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(spl0);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(spl7);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(splbase);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(spldisk);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(splhi);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(splstr);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(spl);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(spltimeout);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(spltty);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(splx);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_ALLOC);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_BROADCAST);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_DEALLOC);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_SIGNAL);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_WAIT_SIG);		/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(SV_WAIT);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(TRYLOCK);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(UNLOCK);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(vtop);			/* svr4ddi.h */
EXPORT_SYMBOL_NOVERS(wakeup);			/* svr4ddi.h */
#endif

#if 0
EXPORT_SYMBOL_NOVERS(bcopy);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(bzero);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(cmn_err);			/* cmn_err.h */
EXPORT_SYMBOL_NOVERS(copyin);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(copyout);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(delay);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_getparm);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_hztousec);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_priv);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_usectohz);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(drv_usecwait);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(getmajor);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(getminor);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(inb);			/* asm/io.h */
EXPORT_SYMBOL_NOVERS(inl);			/* asm/io.h */
EXPORT_SYMBOL_NOVERS(inw);			/* asm/io.h */
EXPORT_SYMBOL_NOVERS(kmem_alloc);		/* kmem.h */
EXPORT_SYMBOL_NOVERS(kmem_free);		/* kmem.h */
EXPORT_SYMBOL_NOVERS(kmem_zalloc);		/* kmem.h */
EXPORT_SYMBOL_NOVERS(makedevice);		/* ddi.h */
EXPORT_SYMBOL_NOVERS(max);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(min);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(outb);			/* asm/io.h */
EXPORT_SYMBOL_NOVERS(outl);			/* asm/io.h */
EXPORT_SYMBOL_NOVERS(outw);			/* asm/io.h */
EXPORT_SYMBOL_NOVERS(strcat);			/* string.h */
EXPORT_SYMBOL_NOVERS(strchr);			/* string.h */
EXPORT_SYMBOL_NOVERS(strcmp);			/* string.h */
EXPORT_SYMBOL_NOVERS(strcpy);			/* string.h */
EXPORT_SYMBOL_NOVERS(strlen);			/* string.h */
EXPORT_SYMBOL_NOVERS(strncat);			/* string.h */
EXPORT_SYMBOL_NOVERS(strncmp);			/* string.h */
EXPORT_SYMBOL_NOVERS(strncpy);			/* string.h */
EXPORT_SYMBOL_NOVERS(strtoul);			/* string.h */
EXPORT_SYMBOL_NOVERS(timeout);			/* ddi.h */
EXPORT_SYMBOL_NOVERS(untimeout);		/* ddi.h */
#endif

#if 0

EXPORT_SYMBOL_NOVERS(adjmsg);			/* stream.h */
EXPORT_SYMBOL_NOVERS(allocb);			/* stream.h */
EXPORT_SYMBOL_NOVERS(backq);			/* stream.h */
EXPORT_SYMBOL_NOVERS(bcanputnext);		/* stream.h */
EXPORT_SYMBOL_NOVERS(bcanput);			/* stream.h */
EXPORT_SYMBOL_NOVERS(bufcall);			/* stream.h */
EXPORT_SYMBOL_NOVERS(canputnext);		/* stream.h */
EXPORT_SYMBOL_NOVERS(canput);			/* stream.h */
EXPORT_SYMBOL_NOVERS(copyb);			/* stream.h */
EXPORT_SYMBOL_NOVERS(copymsg);			/* stream.h */
EXPORT_SYMBOL_NOVERS(datamsg);			/* stream.h */
EXPORT_SYMBOL_NOVERS(dupb);			/* stream.h */
EXPORT_SYMBOL_NOVERS(dupmsg);			/* stream.h */
EXPORT_SYMBOL_NOVERS(enableok);			/* stream.h */
EXPORT_SYMBOL_NOVERS(esballoc);			/* stream.h */
EXPORT_SYMBOL_NOVERS(esbbcall);			/* stream.h */
EXPORT_SYMBOL_NOVERS(flushband);		/* stream.h */
EXPORT_SYMBOL_NOVERS(flushq);			/* stream.h */
EXPORT_SYMBOL_NOVERS(freeb);			/* stream.h */
EXPORT_SYMBOL_NOVERS(freemsg);			/* stream.h */
EXPORT_SYMBOL_NOVERS(freezestr);		/* stream.h */
EXPORT_SYMBOL_NOVERS(getadmin);			/* stream.h */
EXPORT_SYMBOL_NOVERS(getmid);			/* stream.h */
EXPORT_SYMBOL_NOVERS(getq);			/* stream.h */
EXPORT_SYMBOL_NOVERS(insq);			/* stream.h */
EXPORT_SYMBOL_NOVERS(linkb);			/* stream.h */
EXPORT_SYMBOL_NOVERS(msgdsize);			/* stream.h */
EXPORT_SYMBOL_NOVERS(msgpullup);		/* stream.h */
EXPORT_SYMBOL_NOVERS(noenable);			/* stream.h */
EXPORT_SYMBOL_NOVERS(OTHERQ);			/* stream.h */
EXPORT_SYMBOL_NOVERS(pcmsg);			/* stream.h */
EXPORT_SYMBOL_NOVERS(pullupmsg);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putbq);			/* stream.h */
EXPORT_SYMBOL_NOVERS(putctl1);			/* stream.h */
EXPORT_SYMBOL_NOVERS(putctl);			/* stream.h */
EXPORT_SYMBOL_NOVERS(putnextctl1);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putnextctl);		/* stream.h */
EXPORT_SYMBOL_NOVERS(putnext);			/* stream.h */
EXPORT_SYMBOL_NOVERS(putq);			/* stream.h */
EXPORT_SYMBOL_NOVERS(put);			/* stream.h */
EXPORT_SYMBOL_NOVERS(qenable);			/* stream.h */
EXPORT_SYMBOL_NOVERS(qprocsoff);		/* stream.h */
EXPORT_SYMBOL_NOVERS(qprocson);			/* stream.h */
EXPORT_SYMBOL_NOVERS(qreply);			/* stream.h */
EXPORT_SYMBOL_NOVERS(qsize);			/* stream.h */
EXPORT_SYMBOL_NOVERS(RD);			/* stream.h */
EXPORT_SYMBOL_NOVERS(rmvb);			/* stream.h */
EXPORT_SYMBOL_NOVERS(rmvq);			/* stream.h */
EXPORT_SYMBOL_NOVERS(SAMESTR);			/* stream.h */
EXPORT_SYMBOL_NOVERS(strlog);			/* stream.h */
EXPORT_SYMBOL_NOVERS(strqget);			/* stream.h */
EXPORT_SYMBOL_NOVERS(strqset);			/* stream.h */
EXPORT_SYMBOL_NOVERS(testb);			/* stream.h */
EXPORT_SYMBOL_NOVERS(unbufcall);		/* stream.h */
EXPORT_SYMBOL_NOVERS(unfreezestr);		/* stream.h */
EXPORT_SYMBOL_NOVERS(unlinkb);			/* stream.h */
EXPORT_SYMBOL_NOVERS(WR);			/* stream.h */

#endif
