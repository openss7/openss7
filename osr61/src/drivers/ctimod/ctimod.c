/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/////////////////////////////////////////////////////////////////////////////
//// File Name: ctimod.c
////
//// Intel Driver Kernel Interface Module
/////////////////////////////////////////////////////////////////////////////

#include <linux/version.h>
#if (LINUX_VERSION_CODE < 0x020500)
#define LINUX24
#endif

#ifndef LFS
#include <linux/config.h>
#endif
#ifdef LINUX24
#include <linux/modversions.h>
#endif

#include <linux/module.h> 
#include <linux/kernel.h>
#include <linux/timer.h> 
#include <linux/errno.h> 
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/slab.h> 
#include <linux/init.h> 
#include <linux/pci.h>
#include <linux/mm.h>
#include <linux/fs.h> 
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/proc_fs.h>

#ifdef LFS
/* header file verification */
#define CTIMOD_NO_MACROS
#include <src/drivers/mercd/include/ctimod.h>
#undef CTIMOD_NO_MACROS
#endif

// Licensing 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Intel Corp. (c) 2005");
MODULE_DESCRIPTION("Driver Kernel Interface Modlue.");

#ifdef LFS
#ifdef MODULE_ALIAS
MODULE_ALIAS("ctimod");
MODULE_ALIAS("streams-ctimod");
MODULE_ALIAS("streams-ctimodDriver");
#endif
#endif

// Pound Defines 
#ifndef LFS
#define VERSION				9
#endif
#define ctimod_dbg(fmt, args...)	//ctimod_printk(fmt, ##args)
#define ctimod_dbg_locks(fmt, args...)  //ctimod_printk(fmt, ##args)

struct proc_dir_entry ctimod_proc_root;

// Modlue Patameters
static char ctimod_name[]	= "ctimod";
static int  ctimod_major_number = 0;

// Statistics
int         ctimod_debug         = 0;
int	    ctimod_kmalloced     = 0;
int         ctimod_kmalloced_sz  = 0;
int	    ctimod_kfreeed       = 0;
int	    ctimod_kfreeed_sz    = 0;
int         ctimod_kfreeed_my_sz = 0;
int         ctimod_print_mem_fl  = 0;

void        ctimod_debug_on(void);
void        ctimod_debug_off(void);
void        ctimod_debug_clear(void);
void        ctimod_debug_print(void);
void        ctimod_debug_toggle(void);

#ifdef MYCHECK
struct pointer_sT {
            struct pointer_sT *next;
            void *mem_ptr;
            int   size;
};

struct pointer_sT *memInfo;
struct pointer_sT *tail;
#endif

// File operations structure used by the OS
static struct file_operations ctimod_fops =
{
	.owner 	= THIS_MODULE
};


int ctimod_printk(const char *fmt, ...) {
	int len;
	va_list args;
	char ctimod_buf[1024];

	va_start(args, fmt);
	len = vsprintf(ctimod_buf, fmt, args);
	va_end(args);
	printk("%s", ctimod_buf);
	return len;
}

void ctimod_add_timer(struct timer_list *timer) {
	ctimod_dbg("ctimod: add_timer \n");
	add_timer(timer);
	return;
}

void ctimod_add_wait_queue(wait_queue_head_t *head, wait_queue_t *q) {
	ctimod_dbg("ctimod: add_wait_queue \n");
	add_wait_queue(head, q);
	return;
}

#ifdef LFS
int ctimod_check_rh(void);
#endif
int ctimod_check_rh() {
#ifdef CONFIG_RH
        return 1;
#else
        return 0;
#endif
}

ulong ctimod_copy_to_user(void *to, const void*from, ulong n) {
	ctimod_dbg("ctimod: copy_to_user \n");
	return copy_to_user(to,from,n);
}

ulong ctimod_copy_from_user(void *to, const void*from, ulong n) {
	ctimod_dbg("ctimod: copy_from_user \n");
	return copy_from_user(to,from,n);
}

struct proc_dir_entry *ctimod_create_proc_entry(const char *name, mode_t mode,
		                                struct proc_dir_entry *parent) {
	ctimod_dbg("ctimod: create_proc_entry \n");
	return create_proc_entry(name, mode, parent);
}

int  ctimod_del_timer(struct timer_list *timer) {
	ctimod_dbg("ctimod: del_timer \n");
 	return del_timer(timer);
}

void ctimod_do_gettimeofday(struct timeval *tv) {
	ctimod_dbg("ctimod: do_gettimeofday \n");
	do_gettimeofday(tv);
	return;
}

void ctimod_free_irq(int irq,  void *id) {
	ctimod_dbg("ctimod: free_irq \n");
	free_irq(irq, id);
	return;
}

void ctimod_free_pages(ulong addr, ulong order) {
	ctimod_dbg("ctimod: free_pages \n");
	free_pages(addr, order);
	return;
}

ulong ctimod_get_free_pages(uint gfp_mask, unsigned int order) {
	ctimod_dbg("ctimod: __get_free_pages \n");
	return __get_free_pages(gfp_mask, order);
}

void ctimod_init_timer(struct timer_list *timer) {
	ctimod_dbg("ctimod: init_timer \n");
	init_timer(timer);
	return;
}

void ctimod_init_waitqueue_head(wait_queue_head_t *wq) {
        ctimod_dbg("ctimod: ctimod_init_waitqueue_head \n");
        init_waitqueue_head(wq);
        return;
}

void *ctimod_ioremap(ulong offset, ulong sz) {
	ctimod_dbg("ctimod: ioremap \n");
	return ioremap(offset, sz);
}

void ctimod_iounmap(void *addr) {
	ctimod_dbg("ctimod: iounmap \n");
	iounmap(addr);
	return;
}

ulong ctimod_jiffies(void) {
	ctimod_dbg("ctimod: jiffies \n");
	return (jiffies);
}

void ctimod_kfree(void *addr, int size) {
#ifdef MYCHECK
        struct pointer_sT *curr = memInfo;
        struct pointer_sT *prev = memInfo;
#endif
	if (ctimod_print_mem_fl)
	    ctimod_printk("ctimod: kfree (%d)\n", size);

	if (ctimod_debug) {
	    ctimod_kfreeed++;
	    ctimod_kfreeed_sz += size;
#ifdef MYCHECK
	    while (curr != NULL) {
	       if (curr->mem_ptr == addr) {
		   ctimod_kfreeed_my_sz += curr->size;
		   if (prev == curr) {
		       kfree(curr);
		       memInfo = tail = NULL;
		       break;
		   } else {
		       prev = curr->next;
		       kfree(curr);
		       break;
		   }
	       } else {
		   prev = curr;
		   curr = curr->next;
	       }
	    }
#endif
	}

	kfree(addr);
	return;
}

void *ctimod_kmalloc(size_t sz, int priority) {
	void *ret;
#ifdef MYCHECK
        struct pointer_sT *newLink;
#endif
	if (ctimod_print_mem_fl)
	    ctimod_printk("ctimod: kmalloc (%d)\n",sz);
	
	ret = kmalloc(sz, priority);

	if (ctimod_debug) {
	    ctimod_kmalloced++;
	    ctimod_kmalloced_sz += sz;
#ifdef MYCHECK
	    newLink = kmalloc(sizeof(struct pointer_sT), priority);
	    newLink->mem_ptr = tmp;
	    newLink->size = sz;
	    newLink->next = NULL;
	    if (memInfo == NULL)
		memInfo = tail = newLink;
	    else {
		tail->next = newLink;
		tail = newLink;
	    }
#endif
	}

	return ret;
}

void *ctimod_memcpy(void* s, const void* c, size_t n) {
	ctimod_dbg("ctimod: memcpy\n");
	return memcpy(s, c, n);
}

void ctimod_memset(void *s, int c, size_t n) {
	ctimod_dbg("ctimod: memset \n");
	memset(s, c, n);
	return;
}

void ctimod_bzero(void *s, size_t n) {
	ctimod_dbg("ctimod: bzero \n");
	memset(s, 0, n);
	return;
}

int ctimod_pci_module_init(struct pci_driver *drv) {
	ctimod_dbg("ctimod: pci_module_init \n");
	return pci_module_init(drv);
}

int ctimod_pci_register_driver(struct pci_driver *drv) {
	ctimod_dbg("ctimod: pci_register_driver \n");
	return pci_register_driver(drv);
}

void ctimod_pci_unregister_driver(struct pci_driver *drv) {
	ctimod_dbg("ctimod: pci_unregister_driver \n");
	pci_unregister_driver(drv);
	return;
}

int ctimod_pcibios_present(void) {
	ctimod_dbg("ctimod: pcibios_present \n");
#ifdef LINUX24
	return pcibios_present();
#else
	return 1;
#endif
}

struct pci_dev *ctimod_pci_find_device(uint ven, uint dev, const struct pci_dev *from) {
	ctimod_dbg("ctimod: pci_find_device \n");
	return pci_find_device(ven, dev, from);
}

int ctimod_pci_read_config_byte(struct pci_dev *dev, int where, u8 *val) {
	ctimod_dbg("ctimod: pci_read_config_byte \n");
	return pci_read_config_byte(dev, where, val);
}

int ctimod_pci_read_config_word(struct pci_dev *dev, int where, u16 *val) {
	ctimod_dbg("ctimod: pci_read_config_word \n");
	return pci_read_config_word(dev, where, val);
}

int ctimod_pci_read_config_dword(struct pci_dev *dev, int where, u32 *val) {
	ctimod_dbg("ctimod: pci_read_config_dword \n");
	return pci_read_config_dword(dev, where, val);
}

int ctimod_pci_write_config_byte(struct pci_dev *dev, int where, u8 val) {
	ctimod_dbg("ctimod: pci_write_config_byte \n");
	return pci_write_config_byte(dev, where, val);
}

int ctimod_pci_write_config_word(struct pci_dev *dev, int where, u16 val) {
	ctimod_dbg("ctimod: pci_write_config_word \n");
	return pci_write_config_word(dev, where, val);
}

int ctimod_pci_write_config_dword(struct pci_dev *dev, int where, u32 val) {
	ctimod_dbg("ctimod: pci_write_config_dword \n");
	return pci_write_config_dword(dev, where, val);
}

void ctimod_poll_wait(struct file *filp, wait_queue_head_t *q, poll_table *p) {
	ctimod_dbg("ctimod: poll_wait \n");
	poll_wait(filp, q, p);
	return;
}

int ctimod_remap_page_range(struct vm_area_struct *vma, ulong from, ulong to, ulong sz,pgprot_t prot) {
        ctimod_dbg("ctimod: remap_page_range \n");
#ifdef LFS
#if   defined HAVE_KFUNC_REMAP_PFN_RANGE
        return remap_pfn_range(vma, from, to, sz, prot);
#elif defined HAVE_KFUNC_REMAP_PAGE_RANGE
#if   defined HAVE_KFUNC_REMAP_PAGE_RANGE_4ARGS
        return remap_page_range(from, to, sz, prot);
#else
        return remap_page_range(vma, from, to, sz, prot);
#endif
#else
#error HAVE_KFUNC_REMAP_PFN_RANGE or HAVE_KFUNC_REMAP_PAGE_RANGE must be defined.
#endif
#else
#if (LINUX_VERSION_CODE == KERNEL_VERSION(2,4,21) || !defined(LINUX24))
        // for RHEL and 2.6 kernel uses 5 parameters
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,11))
	// for 2.6.11 and new kernels use thisn api..
        return remap_pfn_range(vma, from, to, sz, prot);
#else
        return remap_page_range(vma, from, to, sz, prot);
#endif

#else
        return remap_page_range(from, to, sz, prot);
#endif
#endif
}

int ctimod_register_chrdev(uint major, const char *name, struct file_operations *fops) {
	ctimod_dbg("ctimod: register_chrdev \n");
	return register_chrdev(major, name, fops);
}

void ctimod_remove_proc_entry(const char *name, struct proc_dir_entry *parent) {
	ctimod_dbg("ctimod: remove_proc_entry \n");
	remove_proc_entry(name, parent);
	return;
}

void ctimod_remove_wait_queue(wait_queue_head_t *head,  wait_queue_t *q) {
	ctimod_dbg("ctimod: remove_wait_queue \n");
	remove_wait_queue(head, q);
	return;
}

#ifdef HAVE_KTYPE_IRQ_HANDLER_T
int ctimod_request_irq(uint irq, irq_handler_t handler,
	       				  ulong flag, const char *dev, void *id) {
#else
#ifdef HAVE_KTYPE_IRQRETURN_T
int ctimod_request_irq(uint irq, irqreturn_t (*handler)(int, void *, struct pt_regs *),
	       				  ulong flag, const char *dev, void *id) {
#else
int ctimod_request_irq(uint irq, void (*handler)(int, void *, struct pt_regs *),
	       				  ulong flag, const char *dev, void *id) {
#endif
#endif
	ctimod_dbg("ctimod: request_irq \n");

	return request_irq(irq, handler, flag, dev, id);
}

void ctimod_schedule(void) {
	ctimod_dbg("ctimod: schedule \n");
	schedule();
	return;
}

void ctimod_schedule_timeout(signed long to) {
	ctimod_dbg("ctimod: schedule_timeout \n");
	schedule_timeout(to);
	return;
}

int ctimod_spin_trylock(spinlock_t *mutex) {
	ctimod_dbg_locks("ctimod: spin_trylock \n");
#if 0
        // If trylock should not return 0 under uniprocessor kernels
	return 1;
#endif
	return (spin_trylock(mutex));
}

void ctimod_spin_lock_bh(spinlock_t *mutex) {
	ctimod_dbg_locks("ctimod: spin_lock_bh \n");
	spin_lock_bh(mutex);
	return;
}

void ctimod_spin_unlock_bh(spinlock_t *mutex) {
        ctimod_dbg_locks("ctimod: spin_unlock_bh \n");
        spin_unlock_bh(mutex);
        return;
}

void ctimod_spin_lock(spinlock_t *mutex) {
        ctimod_dbg_locks("ctimod: spin_lock \n");
        spin_lock(mutex);
        return;
}

void ctimod_spin_unlock(spinlock_t *mutex) {
        ctimod_dbg_locks("ctimod: spin_unlock \n");
        spin_unlock(mutex);
        return;
}

void ctimod_spin_lock_irq(spinlock_t *mutex) {
        ctimod_dbg_locks("ctimod: spin_lock_irq \n");
        spin_lock_irq(mutex);
        return;
}

void ctimod_spin_unlock_irq(spinlock_t *mutex) {
        ctimod_dbg_locks("ctimod: spin_unlock_irq \n");
        spin_unlock_irq(mutex);
        return;
}

void ctimod_spin_lock_irqsave(spinlock_t *mutex, unsigned long *flagp)
{
	unsigned long flags;

	ctimod_dbg_locks("ctimod: spin_unlock_irqsave \n");
	/* Whomever wrote this function obviously does not understand the spin_lock_irqsave macro.
	   The processor status word is saved to the flag argument (&flag) which previously was
	   discarded.  I have corrected this function and the corresponding macro. Not suprisingly
	   use of this function is commented out in the MERCD code and avoided altogether in the
	   PMACD code.  PMACD calls the bottom half versions (where is shouldn't). --bb */
	spin_lock_irqsave(mutex, flags);
	*flagp = flags;
	return;
}

void ctimod_spin_unlock_irqrestore(spinlock_t *mutex, unsigned long flag)
{
	ctimod_dbg_locks("ctimod: spin_unlock_irqrestore \n");
	spin_unlock_irqrestore(mutex, flag);
	return;
}

void ctimod_spin_lock_init(spinlock_t *mutex) {
	ctimod_dbg_locks("ctimod: spin_lock_init \n");
  	spin_lock_init(mutex);
	return;
}

void ctimod_tasklet_init(struct tasklet_struct *t, void (*func)(ulong), ulong data){
	ctimod_dbg("ctimod_tasklet_init \n");
  	tasklet_init(t, func, data);
}

void ctimod_tasklet_kill(struct tasklet_struct *t){
	ctimod_dbg("ctimod_tasklet_kill \n");
  	tasklet_kill(t);
}

int ctimod_vsprintf(char *buf, const char *fmt, va_list ap) {
	ctimod_dbg("ctimod: vsprintf \n");
	return vsprintf(buf, fmt, ap);
}

int ctimod_sprintf(char *buf, const char *fmt, ...) {
	int             len ;
	va_list         args;

	va_start(args, fmt);
	len = ctimod_vsprintf(buf, fmt, args);
	va_end(args);
	return len;
}

void ctimod_udelay(long delay) {
	ctimod_dbg("ctimod: udelay \n");
	udelay(delay);
	return;
}

int ctimod_unregister_chrdev(uint major, const char *name) {
	ctimod_dbg("ctimod: unregister_chrdev \n");
	return unregister_chrdev(major, name);
}

struct page *ctimod_virt_to_page(ulong addr) {
        ctimod_dbg("ctimod: virt_to_page \n");
        return virt_to_page(addr);
}

void ctimod_wake_up_interruptible(wait_queue_head_t *q) {
	ctimod_dbg("ctimod: wake_up_interruptible \n");
	wake_up_interruptible(q);
	return;
}

#if 0
#ifndef LINUX24
struct workqueue_struct *ctimod_create_workqueue(char *name) {
	ctimod_dbg("ctimod: ctimod_create_workqueue %s\n", name);
	return NULL; // create_workqueue(name);
}

int ctimod_queue_work(struct workqueue_struct *wq, struct work_struct *ws) {
 	ctimod_dbg("ctimod: queue_work\n");
	return 0; // queue_work(wq, ws);
}

int ctimod_wake_up_process(struct task_struct *ts) {
	ctimod_dbg("ctimod: wake_up_process \n");
	return wake_up_process(ts);
}
#endif
#endif



// Debug Statistics
void ctimod_debug_print() {
	ctimod_printk("CTIMOD: Debug Print Stats\n");
	ctimod_printk(" kmalloc: count=%d  size=%d\n", ctimod_kmalloced, ctimod_kmalloced_sz);
	ctimod_printk(" kfree:   count=%d  size=%d\n", ctimod_kfreeed, ctimod_kfreeed_sz);
	ctimod_printk("   diffCount=%d diffSize=%d\n", ctimod_kmalloced - ctimod_kfreeed, ctimod_kmalloced_sz - ctimod_kfreeed_sz);
}
void ctimod_debug_on() {
        ctimod_printk("CTIMOD: Debug Enabled\n");
        ctimod_debug = 1;
	ctimod_debug_print();
}
void ctimod_debug_off() {
        ctimod_printk("CTIMOD: Debug Disabled\n");
        ctimod_debug = 0;
}
void ctimod_debug_toggle() {
        ctimod_print_mem_fl ^= 1;
        ctimod_printk("CTIMOD: Debug Mem Trigger %d\n", ctimod_print_mem_fl);
}
void ctimod_debug_clear() {
	ctimod_printk("CTIMOD: Debug Clear Stats\n");
	ctimod_kmalloced    = 0;
	ctimod_kmalloced_sz = 0;
	ctimod_kfreeed      = 0;
	ctimod_kfreeed_sz   = 0;
	ctimod_debug_print();
}

/*
 *  FunctionName:  ctimod_init_module()
 *  	  Inputs:  none
 *  	 Outputs:  none
 *  	 Returns:  ErrorCode. 0=Success. Negative=Error
 *   Description:  Called by the OS when the module is first loaded.
 *   	Comments:
 *
 */
static int __init ctimod_init_module(void)
{
   int ret;

   // Register the module by requesting a dynamic major number
   ret = register_chrdev(ctimod_major_number, ctimod_name, &ctimod_fops);

   if (ret < 0) {
       ctimod_printk("ctimod: init_module unable to get major number\n");
       return (-EIO);
   }
   
   ctimod_major_number = ret;
#if 0
   ctimod_printk("\n*******************************************\n");
   ctimod_printk("Driver Kernel Interface Module Loaded (%d)\n", ret);
#ifdef CONFIG_SMP
   ctimod_printk("SMP Support - Linux %s\n", UTS_RELEASE);
#else
   ctimod_printk("            Linux %s\n", UTS_RELEASE);
#endif
   ctimod_printk("*******************************************\n\n");
#endif

   ctimod_proc_root    = proc_root;
   
   return (0);
}
module_init(ctimod_init_module);

/*
 *  FunctionName:  ctimod_cleanup_modlue()
 *        Inputs:  none
 *       Outputs:  none
 *       Returns:  none
 *   Description:  Called by the OS right before modlue is unloaded.
 *      Comments:
 *
 */
static void __exit ctimod_cleanup_module(void)
{

   unregister_chrdev(ctimod_major_number, ctimod_name);

#if 0
   ctimod_printk("Driver Kernel Interface Module Unloaded (%d)\n", ctimod_major_number);
   ctimod_printk("\n   Memory Statistics\n");
   ctimod_printk("=======================\n");
   ctimod_printk("kmalloc = %d (sz)\n", ctimod_kmalloced);
   ctimod_printk("kfree   = %d (sz)\n", ctimod_kfreeed);
#endif
   return;
}
module_exit(ctimod_cleanup_module);

#ifdef LFS
EXPORT_SYMBOL_GPL(ctimod_printk);
EXPORT_SYMBOL_GPL(ctimod_kmalloc);
EXPORT_SYMBOL_GPL(ctimod_free_irq);
EXPORT_SYMBOL_GPL(ctimod_remove_wait_queue);
EXPORT_SYMBOL_GPL(ctimod_do_gettimeofday);
EXPORT_SYMBOL_GPL(ctimod_init_timer);
EXPORT_SYMBOL_GPL(ctimod_debug_print);
EXPORT_SYMBOL_GPL(ctimod_memcpy);
EXPORT_SYMBOL_GPL(ctimod_pci_read_config_byte);
EXPORT_SYMBOL_GPL(ctimod_debug_off);
EXPORT_SYMBOL_GPL(ctimod_vsprintf);
EXPORT_SYMBOL_GPL(ctimod_sprintf);
#if 0
#ifndef LINUX24
EXPORT_SYMBOL_GPL(ctimod_wake_up_process);
#endif
#endif
EXPORT_SYMBOL_GPL(ctimod_spin_unlock);
EXPORT_SYMBOL_GPL(ctimod_spin_unlock_bh);
EXPORT_SYMBOL_GPL(ctimod_add_timer);
EXPORT_SYMBOL_GPL(ctimod_spin_lock);
EXPORT_SYMBOL_GPL(ctimod_spin_lock_bh);
EXPORT_SYMBOL_GPL(ctimod_spin_lock_irq);
EXPORT_SYMBOL_GPL(ctimod_wake_up_interruptible);
EXPORT_SYMBOL_GPL(ctimod_pcibios_present);
EXPORT_SYMBOL_GPL(ctimod_pci_find_device);
#if 0
#ifndef LINUX24
EXPORT_SYMBOL_GPL(ctimod_queue_work);
#endif
#endif
EXPORT_SYMBOL_GPL(ctimod_spin_lock_init);
EXPORT_SYMBOL_GPL(ctimod_tasklet_init);
EXPORT_SYMBOL_GPL(ctimod_tasklet_kill);
EXPORT_SYMBOL_GPL(ctimod_udelay);
EXPORT_SYMBOL_GPL(ctimod_pci_register_driver);
EXPORT_SYMBOL_GPL(ctimod_pci_unregister_driver);
EXPORT_SYMBOL_GPL(ctimod_request_irq);
EXPORT_SYMBOL_GPL(ctimod_pci_module_init);
EXPORT_SYMBOL_GPL(ctimod_add_wait_queue);
EXPORT_SYMBOL_GPL(ctimod_pci_write_config_word);
EXPORT_SYMBOL_GPL(ctimod_debug_toggle);
EXPORT_SYMBOL_GPL(ctimod_spin_unlock_irq);
EXPORT_SYMBOL_GPL(ctimod_del_timer);
EXPORT_SYMBOL_GPL(ctimod_pci_read_config_dword);
#if 0
#ifndef LINUX24
EXPORT_SYMBOL_GPL(ctimod_create_workqueue);
#endif
#endif
EXPORT_SYMBOL_GPL(ctimod_iounmap);
EXPORT_SYMBOL_GPL(ctimod_pci_read_config_word);
EXPORT_SYMBOL_GPL(ctimod_schedule_timeout);
EXPORT_SYMBOL_GPL(ctimod_ioremap);
EXPORT_SYMBOL_GPL(ctimod_remap_page_range);
EXPORT_SYMBOL_GPL(ctimod_poll_wait);
EXPORT_SYMBOL_GPL(ctimod_schedule);
EXPORT_SYMBOL_GPL(ctimod_pci_write_config_byte);
EXPORT_SYMBOL_GPL(ctimod_debug_on);
EXPORT_SYMBOL_GPL(ctimod_jiffies);
EXPORT_SYMBOL_GPL(ctimod_kfree);
EXPORT_SYMBOL_GPL(ctimod_register_chrdev);
EXPORT_SYMBOL_GPL(ctimod_debug_clear);
EXPORT_SYMBOL_GPL(ctimod_pci_write_config_dword);
EXPORT_SYMBOL_GPL(ctimod_unregister_chrdev);
EXPORT_SYMBOL_GPL(ctimod_bzero);
EXPORT_SYMBOL_GPL(ctimod_init_waitqueue_head);
EXPORT_SYMBOL_GPL(ctimod_spin_trylock);
EXPORT_SYMBOL_GPL(ctimod_copy_to_user);
EXPORT_SYMBOL_GPL(ctimod_copy_from_user);
EXPORT_SYMBOL_GPL(ctimod_virt_to_page);
EXPORT_SYMBOL_GPL(ctimod_free_pages);
EXPORT_SYMBOL_GPL(ctimod_get_free_pages);
EXPORT_SYMBOL_GPL(ctimod_check_rh);
EXPORT_SYMBOL_GPL(ctimod_memset);
EXPORT_SYMBOL_GPL(ctimod_remove_proc_entry);
EXPORT_SYMBOL_GPL(ctimod_create_proc_entry);
EXPORT_SYMBOL_GPL(ctimod_spin_lock_irqsave);
EXPORT_SYMBOL_GPL(ctimod_spin_unlock_irqrestore);
#else				/* LFS */
EXPORT_SYMBOL(ctimod_printk);
EXPORT_SYMBOL(ctimod_kmalloc);
EXPORT_SYMBOL(ctimod_free_irq);
EXPORT_SYMBOL(ctimod_remove_wait_queue);
EXPORT_SYMBOL(ctimod_do_gettimeofday);
EXPORT_SYMBOL(ctimod_init_timer);
EXPORT_SYMBOL(ctimod_debug_print);
EXPORT_SYMBOL(ctimod_memcpy);
EXPORT_SYMBOL(ctimod_pci_read_config_byte);
EXPORT_SYMBOL(ctimod_debug_off);
EXPORT_SYMBOL(ctimod_vsprintf);
EXPORT_SYMBOL(ctimod_sprintf);
#if 0
#ifndef LINUX24
EXPORT_SYMBOL(ctimod_wake_up_process);
#endif
#endif
EXPORT_SYMBOL(ctimod_spin_unlock);
EXPORT_SYMBOL(ctimod_spin_unlock_bh);
EXPORT_SYMBOL(ctimod_add_timer);
EXPORT_SYMBOL(ctimod_spin_lock);
EXPORT_SYMBOL(ctimod_spin_lock_bh);
EXPORT_SYMBOL(ctimod_spin_lock_irq);
EXPORT_SYMBOL(ctimod_wake_up_interruptible);
EXPORT_SYMBOL(ctimod_pcibios_present);
EXPORT_SYMBOL(ctimod_pci_find_device);
#if 0
#ifndef LINUX24
EXPORT_SYMBOL(ctimod_queue_work);
#endif
#endif
EXPORT_SYMBOL(ctimod_spin_lock_init);
EXPORT_SYMBOL(ctimod_tasklet_init);
EXPORT_SYMBOL(ctimod_tasklet_kill);
EXPORT_SYMBOL(ctimod_udelay);
EXPORT_SYMBOL(ctimod_pci_register_driver);
EXPORT_SYMBOL(ctimod_pci_unregister_driver);
EXPORT_SYMBOL(ctimod_request_irq);
EXPORT_SYMBOL(ctimod_pci_module_init);
EXPORT_SYMBOL(ctimod_add_wait_queue);
EXPORT_SYMBOL(ctimod_pci_write_config_word);
EXPORT_SYMBOL(ctimod_debug_toggle);
EXPORT_SYMBOL(ctimod_spin_unlock_irq);
EXPORT_SYMBOL(ctimod_del_timer);
EXPORT_SYMBOL(ctimod_pci_read_config_dword);
#if 0
#ifndef LINUX24
EXPORT_SYMBOL(ctimod_create_workqueue);
#endif
#endif
EXPORT_SYMBOL(ctimod_iounmap);
EXPORT_SYMBOL(ctimod_pci_read_config_word);
EXPORT_SYMBOL(ctimod_schedule_timeout);
EXPORT_SYMBOL(ctimod_ioremap);
EXPORT_SYMBOL(ctimod_remap_page_range);
EXPORT_SYMBOL(ctimod_poll_wait);
EXPORT_SYMBOL(ctimod_schedule);
EXPORT_SYMBOL(ctimod_pci_write_config_byte);
EXPORT_SYMBOL(ctimod_debug_on);
EXPORT_SYMBOL(ctimod_jiffies);
EXPORT_SYMBOL(ctimod_kfree);
EXPORT_SYMBOL(ctimod_register_chrdev);
EXPORT_SYMBOL(ctimod_debug_clear);
EXPORT_SYMBOL(ctimod_pci_write_config_dword);
EXPORT_SYMBOL(ctimod_unregister_chrdev);
EXPORT_SYMBOL(ctimod_bzero);
EXPORT_SYMBOL(ctimod_init_waitqueue_head);
EXPORT_SYMBOL(ctimod_spin_trylock);
EXPORT_SYMBOL(ctimod_copy_to_user);
EXPORT_SYMBOL(ctimod_copy_from_user);
EXPORT_SYMBOL(ctimod_virt_to_page);
EXPORT_SYMBOL(ctimod_free_pages);
EXPORT_SYMBOL(ctimod_get_free_pages);
EXPORT_SYMBOL(ctimod_check_rh);
EXPORT_SYMBOL(ctimod_memset);
EXPORT_SYMBOL(ctimod_remove_proc_entry);
EXPORT_SYMBOL(ctimod_create_proc_entry);
EXPORT_SYMBOL(ctimod_spin_lock_irqsave);
EXPORT_SYMBOL(ctimod_spin_unlock_irqrestore);
#endif				/* LFS */

