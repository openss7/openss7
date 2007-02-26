/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : ctimod.h
 * Description                  : driver kernel interface
 *
 *
 **********************************************************************/

#ifndef _CTIMOD_H_
#define _CTIMOD_H_

#define uint				unsigned int
#define ulong				unsigned long

// Undefine and redefine functions as ctimod

#ifndef CTIMOD_NO_MACROS

#ifdef add_timer
#undef add_timer
#endif
#define add_timer(tlist)		ctimod_add_timer(tlist)

#ifdef add_wait_queue
#undef add_wait_queue
#endif
#define add_wait_queue(list,item)	ctimod_add_wait_queue(list,item)

#ifdef bzero
#undef bzero
#endif
#define bzero(ptr,size)			ctimod_bzero(ptr,size)

#ifdef copy_to_user
#undef copy_to_user
#endif
#define copy_to_user(src,dst,size)	ctimod_copy_to_user(src,dst,size)

#ifdef copy_from_user
#undef copy_from_user
#endif
#define copy_from_user(dst,src,size)	ctimod_copy_from_user(dst,src,size)

#ifdef create_proc_entry 
#undef create_proc_entry 
#endif
#define create_proc_entry(name,mode,entry) ctimod_create_proc_entry(name,mode,entry)

#ifdef del_timer
#undef del_timer
#endif
#define del_timer(tlist)		ctimod_del_timer(tlist)

#ifdef do_gettimeofday
#undef do_gettimeofday
#endif
#define do_gettimeofday(tv)		ctimod_do_gettimeofday(tv)

#ifdef free_pages
#undef free_pages
#endif
#define free_pages(addr,order)		ctimod_free_pages(addr,order)

#ifdef free_irq
#undef free_irq
#endif
#define free_irq(num,ptr)		ctimod_free_irq(num,ptr)

#ifdef __get_free_pages
#undef __get_free_pages
#endif
#define __get_free_pages(gfp,order)	ctimod_get_free_pages(gfp,order)

#ifdef init_timer
#undef init_timer
#endif
#define init_timer(tlist)		ctimod_init_timer(tlist)

#ifdef init_waitqueue_head
#undef init_waitqueue_head
#endif
#define init_waitqueue_head(head)	ctimod_init_waitqueue_head(head)

#ifdef ioremap
#undef ioremap
#endif
#define ioremap(src,dst)		ctimod_ioremap(src,dst)

#ifdef iounmap
#undef iounmap
#endif
#define iounmap(ptr)			ctimod_iounmap(ptr)

#ifdef jiffies
#undef jiffies
#endif
#define jiffies				ctimod_jiffies()

#ifdef kfree
#undef kfree
#endif
#define kfree(ptr,size)			ctimod_kfree(ptr,size)

#ifdef kmalloc
#undef kmalloc
#endif
#define kmalloc(size,gfp)		ctimod_kmalloc(size,gfp)

#ifdef mark_bh
#undef mark_dh
#endif
#define mark_bh(num)			ctimod_mark_bh(num)

#ifdef memcpy
#undef memcpy
#endif
#define memcpy(dst,src,size)		ctimod_memcpy(dst,src,size)

#ifdef memset
#undef memset
#endif
#define memset(ptr,val,size)		ctimod_memset(ptr,val,size)

#ifdef pcibios_present
#undef pcibios_present
#endif
#define pcibios_present()		ctimod_pcibios_present()

#ifdef pci_module_init
#undef pci_module_init
#endif
#define pci_module_init(pci)		ctimod_pci_module_init(pci)

#ifdef pci_register_driver
#undef pci_register_driver
#endif
#define pci_register_driver(pci)	ctimod_pci_register_driver(pci)

#ifdef pci_unregister_driver
#undef pci_unregister_driver
#endif
#define pci_unregister_driver(pci)	ctimod_pci_unregister_driver(pci)

#ifdef pci_find_device
#undef pci_find_device
#endif
#define pci_find_device(clas,type,pci)	ctimod_pci_find_device(clas,type,pci)

#ifdef pci_read_config_byte
#undef pci_read_config_byte
#endif
#define pci_read_config_byte(pci,num,valp) ctimod_pci_read_config_byte(pci,num,valp)

#ifdef pci_read_config_word
#undef pci_read_config_word
#endif
#define pci_read_config_word(pci,num,valp) ctimod_pci_read_config_word(pci,num,valp)

#ifdef pci_read_config_dword
#undef pci_read_config_dword
#endif
#define pci_read_config_dword(pci,num,valp) ctimod_pci_read_config_dword(pci,num,valp)

#ifdef pci_write_config_byte
#undef pci_write_config_byte
#endif
#define pci_write_config_byte(pci,num,val) ctimod_pci_write_config_byte(pci,num,val)

#ifdef pci_write_config_word
#undef pci_write_config_word
#endif
#define pci_write_config_word(pci,num,val) ctimod_pci_write_config_word(pci,num,val)

#ifdef pci_write_config_dword
#undef pci_write_config_dword
#endif
#define pci_write_config_dword(pci,num,val) ctimod_pci_write_config_dword(pci,num,val)

#ifdef printk
#undef printk
#endif
#define printk(fmt,args...)		ctimod_printk(fmt,##args)

#ifdef proc_root
#undef proc_root
#endif
#define proc_root			ctimod_proc_root

#ifdef poll_wait
#undef poll_wait
#endif
#define poll_wait(file,wait,poll)	ctimod_poll_wait(file,wait,poll)

#ifdef remap_page_range
#undef remap_page_range
#endif
#define remap_page_range		ctimod_remap_page_range

#ifdef register_chrdev
#undef register_chrdev
#endif
#define register_chrdev			ctimod_register_chrdev

#ifdef remove_proc_entry 
#undef remove_proc_entry 
#endif
#define remove_proc_entry		ctimod_remove_proc_entry 

#ifdef remove_wait_queue
#undef remove_wait_queue
#endif
#define remove_wait_queue		ctimod_remove_wait_queue

#ifdef request_irq
#undef request_irq
#endif
#define request_irq			ctimod_request_irq

#ifdef schedule
#undef schedule
#endif
#define schedule			ctimod_schedule

#ifdef schedule_timeout
#undef schedule_timeout
#endif
#define schedule_timeout		ctimod_schedule_timeout

#ifdef spin_trylock
#undef spin_trylock
#endif
#define spin_trylock(lockp)		ctimod_spin_trylock(lockp)

#ifdef spin_lock_bh
#undef spin_lock_bh
#endif
#define spin_lock_bh(lockp)		ctimod_spin_lock_bh(lockp)

#ifdef spin_unlock_bh
#undef spin_unlock_bh
#endif
#define spin_unlock_bh(lockp)		ctimod_spin_unlock_bh(lockp)

#ifdef spin_lock
#undef spin_lock
#endif
#define spin_lock(lockp)		ctimod_spin_lock(lockp)

#ifdef spin_unlock
#undef spin_unlock
#endif
#define spin_unlock(lockp)		ctimod_spin_unlock(lockp)

#ifdef spin_lock_irq
#undef spin_lock_irq
#endif
#define spin_lock_irq(lockp)		ctimod_spin_lock_irq(lockp)

#ifdef spin_unlock_irq
#undef spin_unlock_irq
#endif
#define spin_unlock_irq(lockp)		ctimod_spin_unlock_irq(lockp)

#ifdef spin_lock_irqsave
#undef spin_lock_irqsave
#endif
#define spin_lock_irqsave(lockp,flags)	ctimod_spin_lock_irqsave(lockp,&flags)

#ifdef spin_unlock_irqrestore
#undef spin_unlock_irqrestore
#endif
#define spin_unlock_irqrestore(lockp,flags) ctimod_spin_unlock_irqrestore(lockp,flags)

#ifdef spin_lock_init
#undef spin_lock_init
#endif
#define spin_lock_init(lockp)		ctimod_spin_lock_init(lockp)

#ifdef sprintf
#undef sprintf
#endif
#define sprintf(buf,fmt,args...)	ctimod_sprintf(buf,fmt,##args)

#ifdef tasklet_init
#undef tasklet_init
#endif
#define tasklet_init                    ctimod_tasklet_init

#ifdef tasklet_kill
#undef tasklet_kill
#endif
#define tasklet_kill                    ctimod_tasklet_kill

#ifdef udelay
#undef udelay
#endif
#define udelay				ctimod_udelay

#ifdef unregister_chrdev
#undef unregister_chrdev
#endif
#define	unregister_chrdev		ctimod_unregister_chrdev

#ifdef virt_to_page
#undef virt_to_page
#endif
#define virt_to_page			ctimod_virt_to_page

#ifdef vsprintf
#undef vsprintf
#endif
#define vsprintf(buf,fmt,args)		ctimod_vsprintf(buf,fmt,args)

#ifdef wake_up_interruptible
#undef wake_up_interruptible
#endif
#define wake_up_interruptible		ctimod_wake_up_interruptible

#endif /* CTIMOD_NO_MACROS */


// ctimod Prototypes 
void   ctimod_add_timer(struct timer_list *);
void   ctimod_add_wait_queue(wait_queue_head_t *,  wait_queue_t *);
void   ctimod_bzero(void *, size_t);
ulong  ctimod_copy_to_user(void *, const void *, ulong);
ulong  ctimod_copy_from_user(void *, const void *, ulong);
int    ctimod_del_timer(struct timer_list *);
struct proc_dir_entry *ctimod_create_proc_entry(const char *, mode_t, 
					    struct proc_dir_entry *);
void   ctimod_do_gettimeofday(struct timeval *);
void   ctimod_free_irq(int, void *);
void   ctimod_free_pages(ulong, ulong);
ulong  ctimod_get_free_pages(uint, unsigned int);
void   ctimod_init_timer(struct timer_list *);
void   ctimod_init_waitqueue_head(wait_queue_head_t *);
void   *ctimod_ioremap(ulong, ulong);
void   ctimod_iounmap(void *);
ulong  ctimod_jiffies(void);
void   ctimod_kfree(void *, int);
void   *ctimod_kmalloc(size_t, int);
void   ctimod_mark_bh(int);
void   *ctimod_memcpy(void *, const void *, size_t);
void   ctimod_memset(void *, int, size_t);
int    ctimod_pcibios_present(void);
int    ctimod_pci_module_init(struct pci_driver *);
int    ctimod_pci_register_driver(struct pci_driver *);
void   ctimod_pci_unregister_driver(struct pci_driver *);
struct pci_dev *ctimod_pci_find_device(uint, uint, const struct pci_dev *);
int    ctimod_pci_read_config_byte(struct pci_dev *, int, u8 *);
int    ctimod_pci_read_config_word(struct pci_dev *, int, u16 *);
int    ctimod_pci_read_config_dword(struct pci_dev *, int, u32 *);
int    ctimod_pci_write_config_byte(struct pci_dev *, int, u8);
int    ctimod_pci_write_config_word(struct pci_dev *, int, u16);
int    ctimod_pci_write_config_dword(struct pci_dev *, int, u32);
int    ctimod_printk(const char *fmt, ...);
void   ctimod_poll_wait(struct file *, wait_queue_head_t *, poll_table *);
int    ctimod_remap_page_range(struct vm_area_struct *, ulong, ulong, ulong, pgprot_t);
int    ctimod_register_chrdev(uint, const char *, struct file_operations *);
void   ctimod_remove_proc_entry(const char *,  struct proc_dir_entry *);
void   ctimod_remove_wait_queue(wait_queue_head_t *,  wait_queue_t *);
#ifdef LINUX24
int    ctimod_request_irq(uint, void (*handler)(int, void *, struct pt_regs *),
	       				   	  ulong, const char *, void *);
#else
int    ctimod_request_irq(uint, irqreturn_t (*handler)(int, void *, struct pt_regs *),
	       				   	  ulong, const char *, void *);
#endif
void   ctimod_schedule(void);
void   ctimod_schedule_timeout(signed long);
int    ctimod_spin_trylock(spinlock_t *);
void   ctimod_spin_lock_bh(spinlock_t *);
void   ctimod_spin_unlock_bh(spinlock_t *);
void   ctimod_spin_lock(spinlock_t *);
void   ctimod_spin_unlock(spinlock_t *);
void   ctimod_spin_lock_irq(spinlock_t *);
void   ctimod_spin_unlock_irq(spinlock_t *);
#if defined LINUX24 && !defined LFS
void   ctimod_spin_lock_irqsave(spinlock_t *, int *);
void   ctimod_spin_unlock_irqrestore(spinlock_t *, int);
#else
void   ctimod_spin_lock_irqsave(spinlock_t *, unsigned long *);
void   ctimod_spin_unlock_irqrestore(spinlock_t *, unsigned long);
#endif
void   ctimod_spin_lock_init(spinlock_t *);
int    ctimod_sprintf(char *, const char *fmt, ...);
void   ctimod_tasklet_init(struct tasklet_struct *, void (*)(unsigned long), unsigned long);
void   ctimod_tasklet_kill(struct tasklet_struct *);
void   ctimod_udelay(long);
int    ctimod_unregister_chrdev(uint, const char *);
struct page *ctimod_virt_to_page(ulong);
int    ctimod_vsprintf(char *, const char *, va_list);
void   ctimod_wake_up_interruptible(wait_queue_head_t *);

// Ctimod Memory Debug Options
void   ctimod_debug_on(void);
void   ctimod_debug_off(void);
void   ctimod_debug_clear(void);
void   ctimod_debug_print(void);
void   ctimod_debug_toggle(void);

int    ctimod_check_rh(void);

#endif /* _CTIMOD_H_ */
