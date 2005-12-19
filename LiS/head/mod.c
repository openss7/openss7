/*****************************************************************************

 @(#) $RCSfile: mod.c,v $ $Name:  $($Revision: 1.1.1.5.4.9 $) $Date: 2005/12/18 06:38:07 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2005/12/18 06:38:07 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: mod.c,v $ $Name:  $($Revision: 1.1.1.5.4.9 $) $Date: 2005/12/18 06:38:07 $"

/*                               -*- Mode: C -*- 
 * mod.c --- module mgmt
 * Author          : Francisco J. Ballesteros Camara
 * Created On      : Thu Jun 29 11:03:59 1995
 * Last Modified By: John A. Boyd Jr.
 * Purpose         : provide LiS STREAMS modules glue
 * ----------------______________________________________________
 *
 *    Copyright (C) 1995  Francisco J. Ballesteros, Denis Froschauer
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *    Copyright (C) 1999  Ole Husgaard <sparre@login.dknet.dk>
 *    Copyright (C) 2000  John A. Boyd Jr.  protologos, LLC
 *
 *    You can reach us by email to any of
 *    nemo@ordago.uc3m.es, 100741.1151@compuserve.com
 */

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#if 0
#include <sys/strport.h>
#include <sys/strconfig.h>	/* config definitions */
#include <sys/stropts.h>	/* struct str_list */
#include <sys/LiS/share.h>	/* streams shared defs */
#include <sys/LiS/queue.h>	/* streams queues */
#include <sys/LiS/mod.h>	/* module interface */
#endif

#include <sys/stream.h>
#if defined(LINUX)		/* compiling for Linux kernel */
#include <linux/sched.h>
#include <linux/ioport.h>
#undef module_info		/* LiS definition */
#define module_info	kernel_module_info
#include <linux/module.h>
#undef module_info
#include <linux/ptrace.h>	/* for pt_regs */
#include <sys/osif.h>
#endif
#include <sys/LiS/modcnt.h>

/*  -------------------------------------------------------------------  */
/*				  Glob. vars                             */

struct fmodsw lis_fstr_sw[MAX_STRDEV];	/* streams devices */
struct fmodsw lis_fmod_sw[MAX_STRMOD];	/* streams modules */
lis_semaphore_t lis_mod_reg;		/* module/driver serialization */
volatile int lis_mod_initialized;

extern struct file_operations lis_streams_fops;

/*  -------------------------------------------------------------------  */
/*				   Symbols                               */

int lis_fmodcnt;			/* (largest module id used) + 1 */

/*
 *  Module autopush list
 */
typedef struct autopush_list {
	unsigned npush;
	modID_t mod[MAXAPUSH];
} autopush_list_t;

/*
 *  Module autopush entry
 *
 *  These form a chain for each major.
 *  The chain is sorted by the minor field.
 */
typedef struct autopush {
	struct autopush *next;
	unsigned cmd;
	minor_t minor;
	minor_t lastminor;
	autopush_list_t push;
} autopush_t;

lis_spin_lock_t lis_apush_lock;		/* normal spin-lock */
static autopush_t *apush[MAX_STRDEV];
static unsigned *apush_nref[MAX_STRMOD];

/*  -------------------------------------------------------------------  */
/*		   Driver and module configuration                       */

/*
 *  The file head/<target>/modconf.c (generated by the strconf utility)
 *  used to be compiled as a seperate object file.
 *  But now it only needs to be known locally in this file.
 *  When we include it here instead, we can move the declarations for the
 *  types used in it from global scope in <sys/LiS/mod.h> to local scope in
 *  this file, reducing overall LiS complexity.
 *  To reflect this change, the name is changed from modconf.c to modconf.inc.
 */

/*
 * Module Configuration
 *
 * In order to initialize configured linked streams modules a table of the
 * following form must be linked into the streams code.
 * There is one entry per configured linked module.
 *
 * The fields are:
 *
 *      cnf_name        The ASCII string name of the module.
 *      cnf_str         A pointer to the streamtab entry for the module.
 *      		Loadable modules have NULL.
 *      cnf_objname	Non-empty means loadable
 * 
 * This table is used at initialization time to register all registered
 * linked modules with streams, thus inserting them into the fmod_sw table.
 * 
 * The final entry in the table consists of {"", NULL, ""}.
 *
 * The file modconf.c provides such a table and is automatically
 * generated by the strconf utility.  DO NOT EDIT modconf.c BY HAND.
 */
typedef struct module_config {
	char cnf_name[LIS_NAMESZ + 1];
	struct streamtab *cnf_str;
	char cnf_objname[LIS_NAMESZ + 1];
	int cnf_qlock_option;

} module_config_t;

/*
 * Driver Configuration
 *
 * In order to initialize configured linked streams drivers a table of the
 * following form must be linked into the streams code.
 * There is one entry per configured linked driver.
 *
 * The fields are as follows:
 *
 *      cnf_name        ASCII string name associated with the driver.
 *      cnf_str         Pointer to the driver's streamtab entry.
 *      cnf_major       The major device numbers to associate with the driver.
 *                      These must correspond with the major device number
 *                      used in a mknod() system call from user level
 *                      to make the devices corresponding to this driver.
 *      cnf_n_majors    The number of majors associated with the driver.
 *      cnf_n_minors    The number of minors associated with the driver.
 *      cnf_init        A pointer to the init routine of the driver.  NULL
 *                      if the driver has no init routine.
 *
 * This table is used at initialization time to register all configured
 * linked drivers with streams, thus getting them into the fstr_sw table.
 *
 * The final entry in the table is of the form:
 *
 *      {"", NULL, NULL, 0, 0, 0, NULL}
 * 
 * The file modconf.c provides such a table and is generated by the
 * strconf utility.  DO NOT EDIT modconf.c BY HAND.
 * 
 */
typedef struct driver_config {
	char cnf_name[LIS_NAMESZ + 1];	/* longer driver names */
	struct streamtab *cnf_str;
	int *cnf_major;
	int cnf_n_majors;
	int cnf_n_minors;
	void _RP(*cnf_init) (void);
	void _RP(*cnf_term) (void);
	int cnf_qlock_option;

} driver_config_t;

#if !defined(LINUX) && !defined(_PPC_LIS_)
struct pt_regs {
	void *nothing;
};
typedef int (*lis_int_handler) (int, void *, struct pt_regs *);
#endif
typedef struct device_config {
	char *name;
	char *prefix;
	struct streamtab *strtb;
	lis_int_handler handler;
	int unit;
	long port;
	int nports;
	int irq_share;
	int irq;
	long mem;
	long mem_size;
	int dma1;
	int dma2;
	int major;
	int minor;
} device_config_t;

#ifdef LIS_LOADABLE_SUPPORT

/*
 * Loadable driver object names
 *
 * This table contains the object names of all configured loadable drivers.
 * It is sorted on the driver major device number field.
 *
 * This tells LiS how to ask kerneld(8) to load the kernel module that
 * contains the streams driver.
 *
 * The file modconf.c provides such a table and is generated by the
 * strconf utility.  DO NOT EDIT modconf.c BY HAND.
 * 
 */
typedef struct driver_obj_name {
	const int *major;		/* The driver major device number */
	const int nmajors;
	const char *initname;		/* The initialization function name */
	const char *objname;		/* The kernel object name */
} driver_obj_name_t;

#endif				/* ifdef LIS_LOADABLE_SUPPORT */

/*
 * Autopush initialzation specification
 *
 * This table contains the initial autopush specification. This is the
 * autopush configuration that is done when streams is started.
 *
 */
typedef struct autopush_init {
	const int major;
	const int minor;
	const int lastminor;
	const int npush;
	const char mods[MAXAPUSH][LIS_NAMESZ + 1];
} autopush_init_t;

/*
 *  The include itself
 *
 *  It is not included when doing dependencies,
 *  but we have a manual dependency in the makefile.
 */
#ifndef DEP
#include "head/modconf.inc"
#endif

/*
 *  We know the size of the tables
 */
#define DRV_CONFIG_SIZE \
	( sizeof(lis_driver_config) / sizeof(driver_config_t) )

#define DEVICE_CONFIG_SIZE \
	( sizeof(lis_device_config) / sizeof(device_config_t) )

#define MOD_CONFIG_SIZE \
	( sizeof(lis_module_config) / sizeof(module_config_t) )

#define AP_CONFIG_SIZE \
	( sizeof(lis_apush_init) / sizeof(autopush_init_t) )

#ifdef LIS_LOADABLE_SUPPORT

#define DRV_OBJNAMES_SIZE \
	( sizeof(lis_drv_objnames) / sizeof(driver_obj_name_t) )

#define MOD_OBJNAMES_SIZE \
	( sizeof(lis_mod_objnames) / sizeof(module_obj_name_t) )

#endif

/*
 *  Return the index into lis_fmod_sw[] of a module name,
 *  or -1 if not found.
 */
static
    int
find_mod(const char *name)
{
	int i;

	for (i = 1; i <= lis_fmodcnt; ++i)
		if (!strcmp(name, lis_fmod_sw[i].f_name))
			return i;
	return -1;
}

#if defined(LINUX)
static
    int
find_dev_objname(const char *name)
{
	int i;

	for (i = 0; i < DEVICE_CONFIG_SIZE; i++) {
		if (strcmp(name, lis_device_config[i].name) == 0)
			return i;
	}
	return -1;
}

static
    int
find_dev_by_major(major_t major)
{
	int i;

	for (i = 0; i < DEVICE_CONFIG_SIZE; i++) {
		if (lis_device_config[i].major == major)
			return i;
	}
	return -1;
}
#endif

/*
 *  Return the index into lis_drv_objnames[] of a driver major,
 *  or -1 if not found.
 */
static
    int
find_drv_objname(major_t major)
{
#ifdef LIS_LOADABLE_SUPPORT
	int i, j;

	for (i = 0; i < DRV_OBJNAMES_SIZE; i++)
		for (j = 0; j < lis_drv_objnames[i].nmajors; j++)
			if (major == lis_drv_objnames[i].major[j])
				return i;
#endif
	return -1;
}

static
    void
initialize_module(int id, const char *name, int state)
{
	lis_fmod_sw[id].f_state = state;
	if (!(lis_fmod_sw[id].f_state & LIS_MODSTATE_INITED)) {	/* do this once */
		lis_fmod_sw[id].f_count = 0;
		strncpy(lis_fmod_sw[id].f_name, name, LIS_NAMESZ);
		lis_sem_init(&lis_fmod_sw[id].f_sem, 1);
		lis_fmod_sw[id].f_state |= LIS_MODSTATE_INITED;
	}
}

static
    int
deinitialize_module(int id)
{
	if (lis_fmod_sw[id].f_state & LIS_MODSTATE_INITED)
		lis_sem_destroy(&lis_fmod_sw[id].f_sem);

	lis_fmod_sw[id].f_state = LIS_MODSTATE_UNKNOWN;
	lis_fmod_sw[id].f_count = 0;
	lis_fmod_sw[id].f_name[0] = 0;
	return (LIS_NULL_MID);
}

/*
 * Find an empty slot in the fmod_sw table.  Caller holds the
 * lis_mod_reg semaphore.
 */
static
    int
find_empty_mod_index(const char *name)
{
	int id;

	for (id = 1; id < lis_fmodcnt; ++id) {
		if (lis_fmod_sw[id].f_name[0] == 0) {
			initialize_module(id, name, LIS_MODSTATE_UNKNOWN);
			break;
		}
	}

	if (id == lis_fmodcnt) {
		if (id == MAX_STRMOD) {
			printk("No available module number for \"%s\"\n", name);
			return LIS_NULL_MID;
		}
		initialize_module(id, name, LIS_MODSTATE_UNKNOWN);
		lis_fmodcnt++;
	}

	return (id);
}

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

/*
 *  Free an autopush entry
 */
static void
apush_free(autopush_t * a)
{
	int i;

	LISASSERT(a->push.npush >= 0);
	LISASSERT(a->push.npush <= MAXAPUSH);

	for (i = 0; i < a->push.npush; ++i) {
		LISASSERT(a->push.mod[i] > 0);
		LISASSERT(a->push.mod[i] < MAX_STRMOD);
		LISASSERT(apush_nref[a->push.mod[i]] > 0);

		--apush_nref[a->push.mod[i]];
	}

	FREE(a);
}

/*
 *  Clear the autopush configuration for a major device number
 */
static void
apush_free_major(major_t major)
{
	autopush_t *a, *b;

	LISASSERT(major < MAX_STRDEV);

	if ((a = apush[major]) == NULL)
		return;

	apush[major] = NULL;

	while (a != NULL) {
		b = a;
		a = a->next;
		apush_free(b);
	}
}

/*
 *  Clear the autopush configuration
 */
static void
apush_free_all(void)
{
	int i;

	for (i = 0; i < MAX_STRDEV; ++i)
		apush_free_major(i);

	/* Pure paranoia: Can go away when we know this stuff works */
	for (i = 0; i < MAX_STRMOD; ++i)
		LISASSERT(apush_nref[i] == 0);
}

/*
 *  Remove a module from all autopush lists
 */
static void
apush_drop_mod(modID_t mod)
{
	int i;

	LISASSERT(mod > 0);
	LISASSERT(mod < MAX_STRMOD);

	if (apush_nref[mod] == 0)
		return;

	for (i = 0; i < MAX_STRDEV; ++i) {
		autopush_t *a;

		for (a = apush[i]; a != NULL; a = a->next) {
			int j = 0;

			while (j < a->push.npush) {
				if (a->push.mod[j] == mod) {
					int k;

					for (k = j + 1; k < a->push.npush; ++k)
						a->push.mod[k - 1] = a->push.mod[k];
					--a->push.npush;
					if (--apush_nref[mod] == 0)
						return;
				} else
					++j;
			}
		}
	}
}

/*
 *  Validate an autopush module list
 */
static int
apush_validate(autopush_list_t * list)
{
	int i;

	if (list->npush < 0 || list->npush > MAXAPUSH)
		return -1;
	for (i = 0; i < list->npush; ++i) {
		modID_t mod = list->mod[i];

		if (lis_fmod_sw[mod].f_state & LIS_MODSTATE_INITED)
			continue;	/* registered */
		return -1;	/* not registered or configured */
	}
	return 0;
}

/*
 *  Do autopush configuration
 */
static int
apush_conf(major_t major, autopush_t * a)
{
	autopush_t *ar, **ap;
	int i;

	if (major >= MAX_STRDEV)
		return -ENODEV;

	if (lis_fstr_sw[major].f_str == NULL) {
		/* Driver is not present */
#if defined(LIS_LOADABLE_SUPPORT)
		/* Is it a configured loadable? */
		if (find_drv_objname(major) < 0)
			return -ENODEV;
#else
		return -ENODEV;
#endif
	}

	/* 
	 *  Check for conflicting entries and insert new entry.
	 *
	 *  This code may look a bit hairy. I have tried to state
	 *  inherited predicates in the form of LISASSERT's to clarify.
	 */
	switch (a->cmd) {
	case SAP_CLEAR:
		if (apush[major] == NULL) {
			/* FREE(a); */
			return -ENODEV;
		}
		ap = &apush[major];
		if (a->minor == 0 && (*ap)->cmd == SAP_ALL) {
			LISASSERT((*ap)->next == NULL);
			ar = *ap;
			*ap = (*ap)->next;
			apush_free(ar);
			FREE(a);
			return 0;
		}
		for (;;) {
			ar = *ap;
			if (ar == NULL) {
				/* FREE(a); */
				return -ERANGE;
			}
			if (ar->minor > a->minor) {
				/* FREE(a); */
				return -ERANGE;
			}
			if (ar->minor == a->minor) {
				*ap = ar->next;
				apush_free(ar);
				FREE(a);
				return 0;
			}
			ap = &ar->next;
		}
	 /*NOTREACHED*/ case SAP_ONE:
		LISASSERT(apush_validate(&a->push) >= 0);
		ap = &apush[major];
		if (*ap != NULL) {
			if ((*ap)->cmd == SAP_ALL)
				return -EEXIST;
			for (;;) {
				ar = *ap;
				if (ar == NULL)
					break;	/* insert at end of chain */
				if (ar->minor > a->minor)
					break;	/* insert here */
				if (ar->cmd == SAP_ONE) {
					if (ar->minor == a->minor)
						return -EEXIST;
				} else {
					LISASSERT(ar->cmd == SAP_RANGE);
					LISASSERT(a->minor >= ar->minor);
					if (a->minor <= ar->lastminor)
						return -EEXIST;
				}
				ap = &ar->next;
			}
		}
		a->next = *ap;
		*ap = a;
		break;
	case SAP_RANGE:
		LISASSERT(apush_validate(&a->push) >= 0);
		if (a->minor >= a->lastminor)
			return -ERANGE;
		ap = &apush[major];
		if (*ap != NULL) {
			if ((*ap)->cmd == SAP_ALL)
				return -EEXIST;
			for (;;) {
				ar = *ap;
				if (ar == NULL)
					break;	/* insert at end of chain */
				if (ar->minor > a->lastminor)
					break;	/* insert here */
				if (ar->minor >= a->minor) {
					LISASSERT(ar->minor <= a->lastminor);
					return -EEXIST;
				}
				if (ar->cmd != SAP_ONE) {
					LISASSERT(ar->cmd == SAP_RANGE);
					LISASSERT(ar->minor <= a->lastminor);
					LISASSERT(ar->minor < a->minor);
					LISASSERT(a->minor < a->lastminor);
					LISASSERT(ar->minor < a->lastminor);
					if (ar->lastminor <= a->lastminor
					    || ar->lastminor >= a->minor)
						return -EEXIST;
				}
				ap = &ar->next;
			}
		}
		a->next = *ap;
		*ap = a;
		break;
	case SAP_ALL:
		LISASSERT(apush_validate(&a->push) >= 0);
		if (apush[major] != NULL)
			return -EEXIST;
		a->next = NULL;
		apush[major] = a;
		break;
	default:
		return -EINVAL;
	}

	for (i = 0; i < a->push.npush; ++i)
		++apush_nref[a->push.mod[i]];

	return 0;
}

static struct autopush *
find_apush_entry(major_t major, minor_t minor)
{
	struct autopush *a;

	LISASSERT(major < MAX_STRDEV);

	for (a = apush[major]; a != NULL; a = a->next) {
		LISASSERT(a->cmd == SAP_ALL || a->cmd == SAP_ONE || a->cmd == SAP_RANGE);

		if (a->cmd == SAP_ALL)
			break;
		if (a->minor > minor)
			return NULL;
		if (a->minor == minor)
			break;
		if (a->cmd == SAP_RANGE && a->lastminor <= minor)
			break;
	}

	LISASSERT(a == NULL || a->minor <= minor);

	return a;
}

/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/*  -------------------------------------------------------------------  */
/* register a new module
 */
modID_t _RP
lis_register_strmod(struct streamtab *strtab, const char *name)
{
	modID_t id = LIS_NULL_MID;
	int inx;
	int new_entry = 0;
	int ret;

	if (name == NULL)
		return LIS_NULL_MID;

	id = lis_findmod_strtab(strtab);
	if (id > 0)
		return (id);

	if ((ret = lis_down(&lis_mod_reg)) < 0)
		return (ret);

	inx = find_mod(name);	/* find by name */
	if (inx > 0)
		id = inx;	/* found */
	else {			/* not in the table */
		id = find_empty_mod_index(name);
		new_entry = 1;
	}

	if (id == lis_fmodcnt) {
		if (id == MAX_STRMOD) {
			lis_up(&lis_mod_reg);
			printk("Unable to register module \"%s\", "
			       "all lis_fmod_sw slots in use.\n", name);
			return LIS_NULL_MID;
		}
		lis_fmodcnt++;
	}

	lis_fmod_sw[id].f_str = strtab;	/* always save strtab */
	initialize_module(id, name,
			  (new_entry && strtab) ? LIS_MODSTATE_LOADED : LIS_MODSTATE_LINKED);

	lis_up(&lis_mod_reg);
	printk("STREAMS module \"%s\" registered%s, id %d\n", lis_fmod_sw[id].f_name,
	       strtab == NULL ? " (loadable)" : "", id);

	return id;
}				/* lis_register_strmod */

int _RP
lis_register_module_qlock_option(modID_t id, int qlock_option)
{
	int ret;

	if ((ret = lis_down(&lis_mod_reg)) < 0)
		return (ret);

	lis_fmod_sw[id].f_qlock_option = qlock_option;

	lis_up(&lis_mod_reg);

	return 0;

}				/* lis_register_module_qlock_option */

/*  -------------------------------------------------------------------  */
/* unregister this module
 */
static int
unregister_module(fmodsw_t * slot)
{
	modID_t id;
	int err;
	char name[LIS_NAMESZ + 1];

	if (!(slot->f_state & LIS_MODSTATE_INITED))
		return (0);	/* already done */

	if (slot->f_count) {
		printk("LiS: unregister module \"%s\" f_count=%d -- busy\n", slot->f_name,
		       slot->f_count);
		return -EBUSY;
	}

	strncpy(name, slot->f_name, LIS_NAMESZ);
	id = slot - lis_fmod_sw;

	if ((err = lis_down(&slot->f_sem)) < 0) {
		printk("LiS: unregister module \"%s\" semaphore-error=%d\n", slot->f_name, err);
		return (err);
	}

	switch (slot->f_state & LIS_MODSTATE_MASK) {
	case LIS_MODSTATE_LINKED:
		lis_up(&slot->f_sem);
		deinitialize_module(id);
		break;

	case LIS_MODSTATE_LOADING:
		lis_up(&slot->f_sem);
		printk("LiS: unregister module \"%s\" -- loading\n", slot->f_name);
		return (-EBUSY);

	case LIS_MODSTATE_LOADED:
	case LIS_MODSTATE_UNLOADED:
	case LIS_MODSTATE_UNKNOWN:
		slot->f_str = NULL;
		deinitialize_module(id);
		break;
	}

	apush_drop_mod(id);

	printk("STREAMS module \"%s\" unregistered, id %d\n", name, id);

	return 0;
}

int _RP
lis_unregister_strmod(struct streamtab *strtab)
{
	fmodsw_t *slot;
	int ret;

	if ((ret = lis_down(&lis_mod_reg)) < 0)
		return (ret);

	for (slot = lis_fmod_sw + lis_fmodcnt; slot != lis_fmod_sw + 1 && slot->f_str != strtab;
	     slot--) ;

	if (slot->f_str != strtab) {
		lis_up(&lis_mod_reg);
		return -EINVAL;
	}

	ret = unregister_module(slot);
	lis_up(&lis_mod_reg);
	return (ret);

}				/* lis_unregister_strmod */

/*  -------------------------------------------------------------------  */
/*  -------------------------------------------------------------------  */
/* Find module by strtab entry.  Called from other routines, so get the 
 * lis_mod_reg semaphore while searching the table.
 */
modID_t
lis_findmod_strtab(struct streamtab *strtab)
{
	int ret;
	modID_t id;

	if (strtab != NULL) {
		if ((ret = lis_down(&lis_mod_reg)) < 0)
			return (ret);

		for (id = 1; id < lis_fmodcnt; ++id) {
			if (lis_fmod_sw[id].f_str == strtab) {
				lis_up(&lis_mod_reg);
				return (id);	/* return existing slot number */
			}
		}
		lis_up(&lis_mod_reg);
	}

	return (LIS_NULL_MID);
}

/* Find module by name in lis_fmod_sw[] 
 */
modID_t _RP
lis_findmod(const char *name)
{
	int id;

	if (!name || !*name)
		return (LIS_NULL_MID);

	/* Look for the module */
	for (id = lis_fmodcnt; id > 0; id--)
		if (!strcmp(lis_fmod_sw[id].f_name, name))
			break;

	return (id > 0 ? id : LIS_NULL_MID);
}				/* lis_findmod */

/*  -------------------------------------------------------------------  */
/* Load module by name into lis_fmod_sw[]
 */
#ifdef LIS_LOADABLE_SUPPORT
/*
 * Having requested a module load, wait until there is some evidence
 * that the module's init function has run.  There is no semaphore
 * type interlock for this, so we just have to wait a little bit
 * in case the module's init function is running on a different CPU
 * from ours.
 */
int
lis_wait_for_module(modID_t id)
{
	int i;

	if (lis_fmod_sw[id].f_str != NULL)
		return (0);	/* OK */

	for (i = 0; i < 50; i++) {
		udelay(100);
		if (lis_fmod_sw[id].f_str != NULL)
			return (0);	/* OK */
	}

	return (1);		/* nothing ever showed up */
}
#endif

modID_t
lis_loadmod(const char *name)
{
	int id = lis_findmod(name);
	int err;
	int configured = 0;
	const char *objname;

	if ((err = lis_down(&lis_mod_reg)) < 0)
		return (err);

	if (id == LIS_NULL_MID && (id = find_empty_mod_index(name)) == LIS_NULL_MID) {	/* mod
											   table is 
											   full */
		lis_up(&lis_mod_reg);
		return (LIS_NULL_MID);
	}

	/* Find object name of this module */
	objname = name;		/* default objname */
	if (lis_fmod_sw[id].f_objname[0]) {
		objname = lis_fmod_sw[id].f_objname;
		configured = 1;
	}

	if ((err = lis_down(&lis_fmod_sw[id].f_sem)) < 0)
		return (LIS_NULL_MID);

	switch (lis_fmod_sw[id].f_state & LIS_MODSTATE_MASK) {
	case LIS_MODSTATE_LINKED:
	case LIS_MODSTATE_LOADED:
		lis_up(&lis_fmod_sw[id].f_sem);
		break;

	case LIS_MODSTATE_UNLOADED:	/* needs loading */
	case LIS_MODSTATE_UNKNOWN:
#ifdef LIS_LOADABLE_SUPPORT
	{
		char req[LIS_NAMESZ + 10];

		/* Ask kerneld to load the module. When the module loads it will call
		   lis_register_strmod() to register itself and will then acquire an available slot 
		   in the fmod_sw table.  So if the module can now be found the load succeeded,
		   otherwise not. */
		sprintf(req, "streams-%s", objname);
		lis_fmod_sw[id].f_state &= ~LIS_MODSTATE_MASK;
		lis_fmod_sw[id].f_state |= LIS_MODSTATE_LOADING;
		lis_up(&lis_fmod_sw[id].f_sem);
		lis_up(&lis_mod_reg);
		if ((err = request_module(req)) < 0 || lis_wait_for_module(id)) {
			if (configured)
				printk("Unable to demand load LiS objname %s, "
				       "STREAMS module %s err=%d\n ", objname,
				       (name) ? name : "(null)", err);
			else
				printk("Unable to demand load STREAMS module %s " "err=%d\n",
				       (name) ? name : "(null)", err);

			return (deinitialize_module(id));
		}

		lis_fmod_sw[id].f_state &= ~LIS_MODSTATE_MASK;
		lis_fmod_sw[id].f_state |= LIS_MODSTATE_LOADED;
		return (id);
	}
#else
		id = deinitialize_module(id);
#endif
		break;

	case LIS_MODSTATE_LOADING:
		printk("Bad module state for %s (LOADING)\n", name);
		lis_up(&lis_fmod_sw[id].f_sem);
		id = LIS_NULL_MID;
		break;
	}

	lis_up(&lis_mod_reg);
	return id;

}				/* lis_loadmod */

/*  -------------------------------------------------------------------  */
void
lis_enable_intr(struct streamtab *strtab, int major, const char *name)
{
#if defined(LINUX)
	int i = 0;
	device_config_t *devptr;
	int retval;

	i = find_dev_objname(name);
	if (i < 0)
		return;

	devptr = &lis_device_config[i];
	if (devptr->irq <= 0)
		return;

#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_))
	retval = request_irq(devptr->irq, devptr->handler, 0, name, NULL);
	if (retval) {
		printk("lis_enable_intr(%s): request_irq(%d) failed: %d\n", name, devptr->irq,
		       retval);
		return;
	}
#endif

	if (devptr->port > 0 && devptr->nports > 0) {
		if (!(retval = check_region(devptr->port, devptr->nports)))
			request_region(devptr->port, devptr->nports, name);

		if (retval) {
			printk("lis_enable_intr(%s): " "check_region(0x%lx,%d) failed: %d\n", name,
			       devptr->port, devptr->nports, retval);
			return;
		}
	}

	printk("Registered IRQ %d for STREAMS driver %s\n", devptr->irq, name);
#endif
}

/*  -------------------------------------------------------------------  */
/* register a new streams device
 */
int _RP
lis_register_strdev(major_t major, struct streamtab *strtab, int nminor, const char *name)
{
	int rslt;
	fmodsw_t *slot;

	if (strtab == NULL) {
		printk("STREAMS driver \"%s\" major %u not registered: " "NULL strtab pointer\n",
		       (name != NULL) ? name : "", (unsigned int) major);
		return (-EINVAL);	/* invalid parameters */
	}

	(void) nminor;		/* ignore minor count, compiler happiness */

	if ((rslt = lis_down(&lis_mod_reg)) < 0)
		return (rslt);

	rslt = register_chrdev(major, name, &lis_streams_fops);
	if (rslt < 0) {
		printk("Unable to get major %u " "for stream driver \"%s\", errno=%d\n", major,
		       (name != NULL) ? name : "", rslt);
		lis_up(&lis_mod_reg);
		return -EIO;
	}

	if (major == 0)		/* register routine assigned it */
		major = rslt;

	slot = &lis_fstr_sw[major];

	if (slot->f_str != strtab && slot->f_str != NULL) {
		printk("Unable to register major %u "
		       "for stream driver \"%s\": In use for driver \"%s\".\n", major,
		       (name != NULL) ? name : "", slot->f_name);
		lis_up(&lis_mod_reg);
		return -EBUSY;
	}

	lis_enable_intr(strtab, major, name);

	slot->f_str = strtab;
	slot->f_count = 0;
	slot->f_qlock_option = LIS_QLOCK_QUEUE;	/* default */
	if (name)
		strncpy(slot->f_name, name, LIS_NAMESZ);
	else
		*slot->f_name = '\0';

	printk("STREAMS driver \"%s\" registered, major %u\n", slot->f_name, major);

	lis_up(&lis_mod_reg);
	return major;

}				/* lis_register_strdev */

int _RP
lis_register_driver_qlock_option(major_t major, int qlock_option)
{
	int rslt;
	fmodsw_t *slot;

	if ((rslt = lis_down(&lis_mod_reg)) < 0)
		return (rslt);

	slot = &lis_fstr_sw[major];
	slot->f_qlock_option = qlock_option;

	lis_up(&lis_mod_reg);

	return 0;

}				/* lis_register_driver_qlock_option */

/*  -------------------------------------------------------------------  */
/* unregister a streams device
 */
int _RP
lis_unregister_strdev(major_t major)
{
	int ret;

	if (major >= MAX_STRDEV || lis_fstr_sw[major].f_str == NULL) {
		printk("STREAMS driver not registered, " "cannot unregister major %u\n", major);
		return -ENODEV;
	}

	if (lis_fstr_sw[major].f_count) {
		printk("STREAMS driver \"%s\" has open count %d, " "cannot unregister major %u\n",
		       lis_fstr_sw[major].f_name, lis_fstr_sw[major].f_count, major);
		return -EBUSY;
	}

	if ((ret = lis_down(&lis_mod_reg)) < 0)
		return (ret);

	apush_free_major(major);

#if defined(LINUX)		/* linux kernel */
	{
		int i;
		device_config_t *devptr;

		i = find_dev_by_major(major);
		if (i >= 0) {
			devptr = &lis_device_config[i];
			if (devptr->irq > 0) {
#if (!defined(_S390_LIS_) && !defined(_S390X_LIS_))
				free_irq(devptr->irq, NULL);
#endif

				if (devptr->port > 0 && devptr->nports > 0)
					release_region(devptr->port, devptr->nports);
			}
		}
	}
#endif
	lis_fstr_sw[major].f_str = NULL;
	unregister_chrdev(major, lis_fstr_sw[major].f_name);

	lis_up(&lis_mod_reg);
	printk("STREAMS driver \"%s\" unregistered, major %u\n", lis_fstr_sw[major].f_name, major);

	return 0;

}				/* lis_unregister_strdev */

/*  -------------------------------------------------------------------  */
/* Find a driver by major device number and return a pointer to
 * its streamtab entry.
 */
streamtab_t *
lis_find_strdev(major_t major)
{
	if (major >= MAX_STRDEV)
		return NULL;

#ifdef LIS_LOADABLE_SUPPORT
	if (lis_fstr_sw[major].f_str == NULL) {
		const char *objname = NULL, *initname = NULL;
		char name[30];
		int i;
		int err;

		/* Find object name of this driver */
		if ((i = find_drv_objname(major)) >= 0) {
			objname = lis_drv_objnames[i].objname;
			initname = lis_drv_objnames[i].initname;
		}
		if (objname == NULL)
			sprintf(name, "char-major-%u", major);
		else
			sprintf(name, "streams-%s", objname);

		err = request_module(name);

		if (lis_fstr_sw[major].f_str == NULL) {
			if (objname != NULL) {
				sprintf(name, "char-major-%u", major);
				err = request_module(name);
				if (lis_fstr_sw[major].f_str == NULL) {
					printk("Unable to demand load "
					       "configured STREAMS object %s, "
					       "device major %u err=%d\n", objname, major, err);
					return NULL;
				}
			} else {
				printk("Unable to demand load " "unconfigured STREAMS "
				       "device major %u err=%d\n", major, err);
				return NULL;
			}
		}
		if (initname != NULL) {	/* call initialization */
			void (*init) (void);

#if HAVE_KFUNC___SYMBOL_GET && HAVE_KFUNC___SYMBOL_PUT
			char symbolname[64];

			snprintf(symbolname, 64, "%s%s", MODULE_SYMBOL_PREFIX, initname);
			init = *(typeof(&init)) (__symbol_get(symbolname));
#else
			init = inter_module_get_request(initname, name);
#endif
			if (init == NULL)
				printk("lis_find_strdev(): " "Unable to resolve init function %s "
				       "in module %s for major %u\n", initname, objname, major);
			else {
				(*init) ();
#if HAVE_KFUNC___SYMBOL_GET && HAVE_KFUNC___SYMBOL_PUT
				__symbol_put(symbolname);
#else
				inter_module_put(initname);
#endif
			}
		}
	}
#endif
	return lis_fstr_sw[major].f_str;
}				/* lis_find_strdev */

/*  -------------------------------------------------------------------  */
/*				 Autopush                                */

/*
 *  Get list of autopush modules for a device
 */
int
lis_apushm(dev_t dev, const char *mods[])
{
	int major = getmajor(dev);
	int minor = getminor(dev);
	struct autopush *a;
	int i;

	if (major < 0 || major >= MAX_STRDEV)
		return 0;

	lis_spin_lock(&lis_apush_lock);
	if ((a = find_apush_entry(major, minor)) == NULL) {
		lis_spin_unlock(&lis_apush_lock);
		return 0;
	}

	for (i = 0; i < a->push.npush; ++i) {
		modID_t mod = a->push.mod[i];

		LISASSERT(mod > 0);
		LISASSERT(mod < MAX_STRMOD);
		LISASSERT(lis_fmod_sw[mod].f_str != NULL);
		mods[i] = lis_fmod_sw[mod].f_name;
	}
	lis_spin_unlock(&lis_apush_lock);

	return a->push.npush;
}				/* lis_apushm */

int _RP
lis_apush_set(struct strapush *ap)
{
	int i, j, err;
	autopush_t *a;

	LISASSERT(ap != NULL);

	if (lis_fstr_sw[ap->sap_major].f_str == NULL) {
		/* Is this a configured loadable driver? */
		if (find_drv_objname(ap->sap_major) < 0)
			return -ENOSTR;
	}

	if (ap->sap_npush > MAXAPUSH)
		return -EINVAL;

	if ((a = ALLOC(sizeof(autopush_t))) == NULL)
		return -ENOSR;

	a->cmd = ap->sap_cmd;
	a->minor = ap->sap_minor;
	a->lastminor = ap->sap_lastminor;
	a->push.npush = ap->sap_npush;

	/* Get module IDs This implies a load for loadable modules since that is the only way to
	   obtain their module id. */
	for (i = 0; i < ap->sap_npush; ++i) {
		j = lis_loadmod(ap->sap_list[i]);
		if (j <= 0) {
			/* Unknown module */
			FREE(a);
			return -EINVAL;
		}
		a->push.mod[i] = j;
	}

	lis_spin_lock(&lis_apush_lock);
	if (apush_validate(&a->push) < 0) {
		lis_spin_unlock(&lis_apush_lock);
		FREE(a);
		return -EINVAL;
	}
	if ((err = apush_conf(ap->sap_major, a)) < 0) {
		lis_spin_unlock(&lis_apush_lock);
		FREE(a);
		return err;
	}
	lis_spin_unlock(&lis_apush_lock);

	return 0;
}				/* lis_apush_set */

int _RP
lis_apush_get(struct strapush *ap)
{
	autopush_t *a;
	int i;

	if (ap->sap_major >= MAX_STRDEV)
		return -EINVAL;

	if (lis_fstr_sw[ap->sap_major].f_str == NULL) {
		/* Is this a configured loadable driver? */
		if (find_drv_objname(ap->sap_major) < 0)
			return -ENOSTR;
	}

	lis_spin_lock(&lis_apush_lock);
	if ((a = find_apush_entry(ap->sap_major, ap->sap_minor)) == NULL) {
		lis_spin_unlock(&lis_apush_lock);
		return -ENODEV;
	}

	ap->sap_cmd = a->cmd;
	LISASSERT(ap->sap_minor == a->minor);
	ap->sap_lastminor = a->lastminor;
	ap->sap_npush = a->push.npush;

	for (i = 0; i < a->push.npush; ++i) {
		modID_t id = a->push.mod[i];

		LISASSERT(id > 0);
		LISASSERT(id < MAX_STRMOD);
		LISASSERT(lis_fmod_sw[id].f_str != NULL);
		strncpy(ap->sap_list[i], lis_fmod_sw[id].f_name, LIS_NAMESZ + 1);
	}
	lis_spin_unlock(&lis_apush_lock);

	return 0;
}				/* lis_apush_get */

int _RP
lis_apush_vml(struct str_list *mlp)
{
	int i, j;
	struct str_mlist *lp;

	if (mlp->sl_nmods <= 0)
		return -EINVAL;

	for (i = 0, lp = mlp->sl_modlist; i < mlp->sl_nmods; ++i, ++lp) {
		/* Nope, is it registered? */
		for (j = lis_fmodcnt; j > 0; j--)
			if (!strcmp(lp->l_name, lis_fmod_sw[j].f_name))
				break;
		if (j == 0)
			return 1;	/* Unknown module */
	}

	return 0;
}				/* lis_validate_mod_list */

/*
 *  Initialization
 */
void
lis_init_mod(void)
{
	int i, j;
	int modid;
	int maj;

	lis_spin_lock_init(&lis_apush_lock, "AutoPush-Lock");
	lis_sem_init(&lis_mod_reg, 1);
	memset(lis_fstr_sw, 0, sizeof lis_fstr_sw);
	memset(lis_fmod_sw, 0, sizeof lis_fmod_sw);
	memset(apush, 0, sizeof apush);
	memset(apush_nref, 0, sizeof apush_nref);

	lis_fmodcnt = 1;

	for (i = 0; i < MOD_CONFIG_SIZE; ++i) {
		LISASSERT(lis_fmodcnt < MAX_STRMOD);

		if (!lis_module_config[i].cnf_name[0])
			continue;	/* no name */

		modid =
		    lis_register_strmod(lis_module_config[i].cnf_str,
					lis_module_config[i].cnf_name);
		if (modid == LIS_NULL_MID) {
			printk("Failed to register module \"%s\".\n",
			       lis_module_config[i].cnf_name);
			continue;
		}

		if (lis_register_module_qlock_option(modid, lis_module_config[i].cnf_qlock_option) <
		    0) {
			printk("Failed to register module \"%s\" qlock option.\n",
			       lis_module_config[i].cnf_name);
			continue;
		}

		/* 
		 * If the module has an object file name then it is
		 * loadable, otherwise it is linked in with LiS.
		 */
		strncpy(lis_fmod_sw[modid].f_objname, lis_module_config[i].cnf_objname, LIS_NAMESZ);
		lis_fmod_sw[i].f_state &= ~LIS_MODSTATE_MASK;
		if (lis_fmod_sw[modid].f_objname[0])
			lis_fmod_sw[modid].f_state |= LIS_MODSTATE_UNLOADED;
		else
			lis_fmod_sw[modid].f_state |= LIS_MODSTATE_LINKED;
	}

	for (i = 0; i < DRV_CONFIG_SIZE; ++i) {
		for (j = 0; j < lis_driver_config[i].cnf_n_majors; j++) {
			if ((maj =
			     lis_register_strdev(lis_driver_config[i].cnf_major[j],
						 lis_driver_config[i].cnf_str,
						 lis_driver_config[i].cnf_n_minors,
						 lis_driver_config[i].cnf_name)) < 0) {
				printk("Failed to register driver \"%s\".\n",
				       lis_driver_config[i].cnf_name);
				continue;
			}
			if (lis_register_driver_qlock_option
			    (maj, lis_driver_config[i].cnf_qlock_option) < 0) {
				printk("Failed to register driver \"%s\" qlock option.\n",
				       lis_driver_config[i].cnf_name);
				continue;
			}
		}
		if (lis_driver_config[i].cnf_init != NULL)
			(*lis_driver_config[i].cnf_init) ();
	}

	for (i = 0; i < AP_CONFIG_SIZE; ++i) {
		autopush_t *a;
		int err;

		if ((a = ALLOC(sizeof(autopush_t))) == NULL) {
			printk("Failed to allocate autopush entry.\n");
			continue;
		}

		if (lis_apush_init[i].minor == -1)
			a->cmd = SAP_ALL;
		else if (lis_apush_init[i].lastminor == 0)
			a->cmd = SAP_ONE;
		else
			a->cmd = SAP_RANGE;
		if (a->cmd == SAP_ALL)
			a->minor = a->lastminor = 0;
		else {
			a->minor = lis_apush_init[i].minor;
			a->lastminor = lis_apush_init[i].lastminor;
		}
		a->push.npush = 0;
		for (j = 0; j < lis_apush_init[i].npush; j++) {
			modid = find_mod(lis_apush_init[i].mods[j]);
			if (modid < 0)
				printk("Auto-push init: no module \"%s\"\n",
				       lis_apush_init[i].mods[j]);
			else
				a->push.mod[a->push.npush++] = modid;
		}

		if ((err = apush_conf(lis_apush_init[i].major, a)) < 0) {
			FREE(a);
			printk("Failed to add initial autopush entry, " "error = %d.\n", -err);
			continue;
		}
	}
	if (AP_CONFIG_SIZE > 0)
		printk("Added initial autopush entries.\n");
	lis_mod_initialized = 1;
}

/*
 *  Shutdown
 */
void
lis_terminate_mod(void)
{
	int i, j;

	apush_free_all();

	for (i = 0; i < DRV_CONFIG_SIZE; ++i) {
		if (lis_driver_config[i].cnf_term != NULL)
			(*lis_driver_config[i].cnf_term) ();
		for (j = 0; j < lis_driver_config[i].cnf_n_majors; j++) {
			if (lis_unregister_strdev(lis_driver_config[i].cnf_major[j]) < 0)
				printk("Failed to unregister driver \"%s\".\n",
				       lis_driver_config[i].cnf_name);
		}
	}

	/* 
	 * The above catches all the configured modules and drivers.
	 * Now we need to unregister all the loadables as well.
	 */
	for (i = 1; i < MAX_STRMOD; i++) {
		if (unregister_module(&lis_fmod_sw[i]) < 0)
			printk("Failed to unregister module \"%s\".\n", lis_fmod_sw[i].f_name);
	}

	for (i = 0; i < MAX_STRDEV; i++) {
		if (lis_fstr_sw[i].f_str && lis_fstr_sw[i].f_count && lis_unregister_strdev(i) < 0)
			printk("Failed to unregister driver \"%s\".\n", lis_fmod_sw[i].f_name);
	}

	if (lis_mod_initialized) {
		lis_mod_initialized = 0;
		lis_sem_destroy(&lis_mod_reg);
	}
}
