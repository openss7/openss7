#ifndef LIS_MODULE_H
#define LIS_MODULE_H
/************************************************************************
*                        LiS Module Header File                         *
*************************************************************************
*									*
* A STREAMS driver that is capable of being a separately loadable	*
* module can include this file at the very beginning.  This header file	*
* will sense the MODULE and MODVERSIONS kernel parameters and include	*
* the proper kernel header files in the proper order.			*
* 									*
* There is a difference between 2.4 and 2.6 in this respect.  In 2.4	*
* the kernel defines "module_info", a standard STREAMS symbol.  If we	*
* include <sys/stream.h> BEFORE <linux/module.h> then the kernel's	*
* definition clashes with ours.						*
* 									*
* In 2.6 the kernel declares "dev_t" in such a way that it interferes	*
* with the LiS definition.  We need to define it away before including	*
* any kernel includes.							*
* 									*
* The bottom line is that the order of includes makes a difference	*
* depending upon whether it is a 2.4 or 2.6 kernel.  So this file exists*
* in order to make it easy for the driver write to get around this.	*
*									*
************************************************************************/

#ident "@(#) LiS LiS/module.h 1.4 08/23/04 11:43:26 "

#if defined(LINUX) && defined(__KERNEL__)

#include <sys/LiS/linux-mdep.h>

#if defined(KERNEL_2_4)
#undef module_info
#endif

#ifdef MODULE
#  if defined(LINUX) && defined(__KERNEL__)
#    ifdef MODVERSIONS
#     ifdef LISMODVERS
#      include <sys/modversions.h>      /* /usr/src/LiS/include/sys */
#     else
#      include <linux/modversions.h>
#     endif
#    endif
#    include <linux/module.h>
#  else
#    error This can only be a module in the Linux kernel environment
#  endif
#endif

#elif defined(USER)		/* defined(LINUX) && defined(__KERNEL__) */

#include <sys/LiS/user-mdep.h>

#endif				/* defined(LINUX) && defined(__KERNEL__) */

#include <sys/LiS/modcnt.h>	/* MODGET(), MODPUT() */

#endif		/* LIS_MODULE_H */
