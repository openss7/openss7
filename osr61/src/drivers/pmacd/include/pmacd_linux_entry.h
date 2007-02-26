/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_linux_entry.h
//
// Defines prototypes for driver entry functions.
/////////////////////////////////////////////////////////////////////////////

#ifndef _PMACD_LINUX_ENTRY_H
#define _PMACD_LINUX_ENTRY_H

#include "pmacd.h"
#include "pmacd_pmb.h"
#include "pmacd_board.h"

extern int          init_module(void);

extern void         cleanup_module(void);

extern int          pmacd_open(struct inode *inode, 
			       struct file *file);

extern int          pmacd_release(struct inode *inode, 
				  struct file *file);

extern ssize_t      pmacd_read(struct file *, 
			       char *, 
			       size_t, 
			       loff_t *);

extern ssize_t      pmacd_write(struct file *, 
				const char *, 
				size_t, 
				loff_t *);

extern unsigned int pmacd_poll(struct file *filp, 
			       struct poll_table_struct *wait);

extern int          pmacd_mmap(struct file *file, 
			       struct vm_area_struct *vma);

extern int          pmacd_ioctl(struct inode *inode, 
				struct file *file, 
				unsigned int functionId, 
				unsigned long arg);

// Driver Specific Functions

int                 pmacd_register_irq(pmacd_board_t *board);

ssize_t             pmacd_queueUserMessage(pmacd_pmbqueue_t *appQueue,
					   const char *buffer,
					   size_t size);

#endif /* _PMACD_LINUX_ENTRY_H */
