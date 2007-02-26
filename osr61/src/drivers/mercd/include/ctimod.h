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

#ifdef add_timer
#undef add_timer
#endif
#define add_timer 			ctimod_add_timer

#ifdef add_wait_queue
#undef add_wait_queue
#endif
#define add_wait_queue 			ctimod_add_wait_queue

#ifdef bzero
#undef bzero
#endif
#define bzero				ctimod_bzero

#ifdef copy_to_user
#undef copy_to_user
#endif
#define copy_to_user 			ctimod_copy_to_user

#ifdef copy_from_user
#undef copy_from_user
#endif
#define copy_from_user 			ctimod_copy_from_user

#ifdef create_proc_entry 
#undef create_proc_entry 
#endif
#define create_proc_entry		ctimod_create_proc_entry 

#ifdef del_timer
#undef del_timer
#endif
#define del_timer			ctimod_del_timer

#ifdef do_gettimeofday
#undef do_gettimeofday
#endif
#define do_gettimeofday			ctimod_do_gettimeofday

#ifdef free_pages
#undef free_pages
#endif
#define free_pages			ctimod_free_pages

#ifdef free_irq
#undef free_irq
#endif
#define free_irq			ctimod_free_irq

#ifdef __get_free_pages
#undef __get_free_pages
#endif
#define __get_free_pages		ctimod_get_free_pages

#ifdef init_timer
#undef init_timer
#endif
#define init_timer			ctimod_init_timer

#ifdef init_waitqueue_head
#undef init_waitqueue_head
#endif
#define init_waitqueue_head             ctimod_init_waitqueue_head

#ifdef ioremap
#undef ioremap
#endif
#define ioremap				ctimod_ioremap

#ifdef iounmap
#undef iounmap
#endif
#define iounmap				ctimod_iounmap

#ifdef jiffies
#undef jiffies
#endif
#define jiffies				ctimod_jiffies()

#ifdef kfree
#undef kfree
#endif
#define kfree				ctimod_kfree

#ifdef kmalloc
#undef kmalloc
#endif
#define kmalloc				ctimod_kmalloc

#ifdef mark_bh
#undef mark_dh
#endif
#define mark_bh				ctimod_mark_bh

#ifdef memcpy
#undef memcpy
#endif
#define memcpy				ctimod_memcpy

#ifdef memset
#undef memset
#endif
#define memset				ctimod_memset

#ifdef pcibios_present
#undef pcibios_present
#endif
#define pcibios_present			ctimod_pcibios_present

#ifdef pci_module_init
#undef pci_module_init
#endif
#define pci_module_init			ctimod_pci_module_init

#ifdef pci_register_driver
#undef pci_register_driver
#endif
#define pci_register_driver		ctimod_pci_register_driver

#ifdef pci_unregister_driver
#undef pci_unregister_driver
#endif
#define pci_unregister_driver		ctimod_pci_unregister_driver

#ifdef pci_find_device
#undef pci_find_device
#endif
#define pci_find_device			ctimod_pci_find_device

#ifdef pci_read_config_byte
#undef pci_read_config_byte
#endif
#define pci_read_config_byte		ctimod_pci_read_config_byte

#ifdef pci_read_config_word
#undef pci_read_config_word
#endif
#define pci_read_config_word            ctimod_pci_read_config_word

#ifdef pci_read_config_dword
#undef pci_read_config_dword
#endif
#define pci_read_config_dword           ctimod_pci_read_config_dword

#ifdef pci_write_config_byte
#undef pci_write_config_byte
#endif
#define pci_write_config_byte           ctimod_pci_write_config_byte

#ifdef pci_write_config_word
#undef pci_write_config_word
#endif
#define pci_write_config_word		ctimod_pci_write_config_word

#ifdef pci_write_config_dword
#undef pci_write_config_dword
#endif
#define pci_write_config_dword          ctimod_pci_write_config_dword

#ifdef printk
#undef printk
#endif
#define printk				ctimod_printk

#ifdef proc_root
#undef proc_root
#endif
#define proc_root			ctimod_proc_root

#ifdef poll_wait
#undef poll_wait
#endif
#define poll_wait			ctimod_poll_wait

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
#define spin_trylock                    ctimod_spin_trylock

#ifdef spin_lock_bh
#undef spin_lock_bh
#endif
#define spin_lock_bh                    ctimod_spin_lock_bh

#ifdef spin_unlock_bh
#undef spin_unlock_bh
#endif
#define spin_unlock_bh                  ctimod_spin_unlock_bh

#ifdef spin_lock
#undef spin_lock
#endif
#define spin_lock                       ctimod_spin_lock

#ifdef spin_unlock
#undef spin_unlock
#endif
#define spin_unlock                     ctimod_spin_unlock

#ifdef spin_lock_irq
#undef spin_lock_irq
#endif
#define spin_lock_irq                   ctimod_spin_lock_irq

#ifdef spin_unlock_irq
#undef spin_unlock_irq
#endif
#define spin_unlock_irq                 ctimod_spin_unlock_irq

#ifdef spin_lock_irqsave
#undef spin_lock_irqsave
#endif
#define spin_lock_irqsave		ctimod_spin_lock_irqsave

#ifdef spin_unlock_irqrestore
#undef spin_unlock_irqrestore
#endif
#define spin_unlock_irqrestore          ctimod_spin_unlock_irqrestore

#ifdef spin_lock_init
#undef spin_lock_init
#endif
#define spin_lock_init                  ctimod_spin_lock_init

#ifdef sprintf
#undef sprintf
#endif
#define sprintf				ctimod_sprintf

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
#define vsprintf			ctimod_vsprintf

#ifdef wake_up_interruptible
#undef wake_up_interruptible
#endif
#define wake_up_interruptible		ctimod_wake_up_interruptible



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
ulong  ctimod_get_free_pages(uint, ulong);
void   ctimod_init_timer(struct timer_list *);
void   ctimod_init_waitqueue_head(wait_queue_head_t *);
void   *ctimod_ioremap(ulong, ulong);
void   ctimod_iounmap(void *);
ulong  ctimod_jiffies(void);
void   ctimod_kfree(void *, int);
void   *ctimod_kmalloc(size_t, int);
void   ctimod_mark_bh(int);
void   ctimod_memcpy(void *, const void *, size_t);
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
void   ctimod_spin_lock_irqsave(spinlock_t *, int);
void   ctimod_spin_unlock_irqrestore(spinlock_t *, int);
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
