# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/03/08 06:08:27 $
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
# Last Modified $Date: 2007/03/08 06:08:27 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.6  2007/03/08 06:08:27  brian
# - modutil_DATA should not be distributed (and it reorders uninstall)
#
# Revision 0.9.2.5  2007/03/04 00:23:25  brian
# - add version file creation
#
# Revision 0.9.2.4  2007/03/01 00:10:16  brian
# - update to build process for 2.4 kernels
#
# Revision 0.9.2.3  2007/02/27 08:38:33  brian
# - release corrections for 2.4 kernel builds
#
# Revision 0.9.2.2  2007/02/27 01:40:10  brian
# - final corrections to build
#
# Revision 0.9.2.1  2007/02/25 12:26:05  brian
# - added new files for release package
#
# =============================================================================

##
## General rules
##
include am/openss7.am

##
## Version file
##
include am/version.am

##
## Help
##
include am/help.am

##
## Problem Reports
##
include am/pr.am

lib_LTLIBRARIES			=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

EXTRA_PROGRAMS			+= \
				  @PACKAGE_TARNAME@_mknod

pkglibexec_PROGRAMS		=
if WITH_LFS
pkglibexec_PROGRAMS		+= test-osr61
endif

sbin_PROGRAMS			=
if WITH_LFS
sbin_PROGRAMS			+= \
				  @PACKAGE_TARNAME@_mknod
endif

bin_PROGRAMS			= 

dist_sbin_SCRIPTS		= 

#OSR_EXTRA_TESTS		=
#OSR_EXTRA_TOOLS		=


@PACKAGE_TARNAME@_mknod_SOURCES		= @PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(PKG_INCLUDES) $(USER_CPPFLAGS)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

test_osr61_SOURCES		= src/test/test-osr61.c
test_osr61_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
test_osr61_LDADD		= $(STREAMS_LDADD)
test_osr61_LDFLAGS		= $(STREAMS_LDFLAGS)
test_osr61_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
test_osr61_CFLAGS		= $(USER_CFLAGS)

#xxxxxx_SOURCES			= src/xxxxxx.c
#xxxxxx_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
#xxxxxx_LDADD			= $(STREAMS_LDADD)
#xxxxxx_LDFLAGS			= $(STREAMS_LDFLAGS)
#xxxxxx_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS)
#xxxxxx_CFLAGS			= $(USER_CFLAGS)

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

ADDON_LIBS			= 
ADDON_MODS			= 
ADDON_OBJS			= 

noinst_LIBRARIES		=  libOSRctimod.a $(ADDON_LIBS)

if WITH_DLGN
EXTRA_LIBRARIES			+= libOSRdlgn.a libOSRdvbm.a libOSRgncfd.a libOSRgpio.a libOSRsctmr.a
endif
if WITH_MERC
EXTRA_LIBRARIES			+= libOSRmercd.a
endif
if WITH_PMAC
EXTRA_LIBRARIES			+= libOSRpmacd.a
endif

noinst_LIBRARIES		+= $(EXTRA_LIBRARIES)

if WITH_DLGN
## WITH_DLGN

EXTRA_libOSRdlgn_a_SOURCES	= \
				src/drivers/dlgn/source/andam.c \
				src/drivers/dlgn/source/aninit.c \
				src/drivers/dlgn/source/anprot.c \
				src/drivers/dlgn/source/antbd.c \
				src/drivers/dlgn/source/antlow.c \
				src/drivers/dlgn/source/antpci.c \
				src/drivers/dlgn/source/driver.c \
				src/drivers/dlgn/source/drvprot.c \
				src/drivers/dlgn/source/hardware.c \
				src/drivers/dlgn/source/shrdware.c
libOSRdlgn_a_SOURCES		= \
				src/drivers/dlgn/source/dlgn.c \
				src/drivers/dlgn/source/srbulk.c \
				src/drivers/dlgn/source/srcmd.c \
				src/drivers/dlgn/source/srchk.c \
				src/drivers/dlgn/source/srintr.c \
				src/drivers/dlgn/source/srmap.c \
				src/drivers/dlgn/source/srmisc.c \
				src/drivers/dlgn/source/srpm.c \
				src/drivers/dlgn/source/sbamapin.c \
				src/drivers/dlgn/include/andam.h \
				src/drivers/dlgn/include/andrv.h \
				src/drivers/dlgn/include/anlibdrv.h \
				src/drivers/dlgn/include/anprot.h \
				src/drivers/dlgn/include/anstart.h \
				src/drivers/dlgn/include/antares.h \
				src/drivers/dlgn/include/anterror.h \
				src/drivers/dlgn/include/antglob.h \
				src/drivers/dlgn/include/antpci.h \
				src/drivers/dlgn/include/d40defs.h \
				src/drivers/dlgn/include/d40low.h \
				src/drivers/dlgn/include/dlgclinux.h \
				src/drivers/dlgn/include/dlgcos.h \
				src/drivers/dlgn/include/dlgn2cfd.h \
				src/drivers/dlgn/include/dlgnextern.h \
				src/drivers/dlgn/include/dlgn.h \
				src/drivers/dlgn/include/dpci.h \
				src/drivers/dlgn/include/drvdebug.h \
				src/drivers/dlgn/include/drvprot.h \
				src/drivers/dlgn/include/dxdevdef.h \
				src/drivers/dlgn/include/dxdrv.h \
				src/drivers/dlgn/include/dxlibdrv.h \
				src/drivers/dlgn/include/dxmsg.h \
				src/drivers/dlgn/include/dxstart.h \
				src/drivers/dlgn/include/extern.h \
				src/drivers/dlgn/include/extvar.h \
				src/drivers/dlgn/include/fln.h \
				src/drivers/dlgn/include/fx12low.h \
				src/drivers/dlgn/include/fxphym.h \
				src/drivers/dlgn/include/genbootdef.h \
				src/drivers/dlgn/include/genboot.h \
				src/drivers/dlgn/include/gentypedefs.h \
				src/drivers/dlgn/include/gnbulk.h \
				src/drivers/dlgn/include/gndbg.h \
				src/drivers/dlgn/include/gndefs.h \
				src/drivers/dlgn/include/gndlgn.h \
				src/drivers/dlgn/include/gndrv.h \
				src/drivers/dlgn/include/gnerrno.h \
				src/drivers/dlgn/include/gnlibdrv.h \
				src/drivers/dlgn/include/gnlogmsg.h \
				src/drivers/dlgn/include/gnmsg.h \
				src/drivers/dlgn/include/gnstart.h \
				src/drivers/dlgn/include/gpiolib.h \
				src/drivers/dlgn/include/hardexp.h \
				src/drivers/dlgn/include/hardware.h \
				src/drivers/dlgn/include/include.h \
				src/drivers/dlgn/include/intrface.h \
				src/drivers/dlgn/include/pmswtbl.h \
				src/drivers/dlgn/include/protocol.h \
				src/drivers/dlgn/include/sbadefs.h \
				src/drivers/dlgn/include/sbaextern.h \
				src/drivers/dlgn/include/sbalibdrv.h \
				src/drivers/dlgn/include/sbamapin.h \
				src/drivers/dlgn/include/sbapm.h \
				src/drivers/dlgn/include/sbastart.h \
				src/drivers/dlgn/include/sbdlib.h \
				src/drivers/dlgn/include/sbdmsg.h \
				src/drivers/dlgn/include/srbulk.h \
				src/drivers/dlgn/include/srchk.h \
				src/drivers/dlgn/include/srcmd.h \
				src/drivers/dlgn/include/srdbg.h \
				src/drivers/dlgn/include/srdefs.h \
				src/drivers/dlgn/include/srextern.h \
				src/drivers/dlgn/include/srmap.h \
				src/drivers/dlgn/include/srmisc.h \
				src/drivers/dlgn/include/srpm.h \
				src/drivers/dlgn/include/typedef.h \
				src/drivers/dlgn/include/typedefs.h \
				src/drivers/dlgn/include/var.h \
				src/drivers/dlgn/include/vrxlow.h \
				src/drivers/dlgn/include/vvfwdefs.h \
				src/drivers/dlgn/include/antares.ver

if WITH_ANTARES
libOSRdlgn_a_SOURCES		+= $(EXTRA_libOSRdlgn_a_SOURCES)
endif
libOSRdlgn_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libOSRdlgn_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME_STR=\"dlgn\" \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include
libOSRdlgn_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOSRdlgn_a_OBJECTS):	$(libOSRdlgn_a_DEPENDENCIES)

$(kpre)dlgnDriver.o: stamp-verobjs
	@list=`$(ECHO) " $(libOSRdlgn_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list
$(kpre)dlgnDriver.ver: dlgn.ver
	cat $^ > $@
ADDON_MODS			+= $(kpre)dlgnDriver.o
ADDON_OBJS			+= $(libOSRdlgn_a_OBJECTS)

EXTRA_libOSRdvbm_a_SOURCES	= 
libOSRdvbm_a_SOURCES		= \
				src/drivers/dvbm/source/dvbm.c \
				src/drivers/dvbm/include/dvbmextern.h \
				src/drivers/dvbm/include/dvbm.h
if WITH_LFS
libOSRdvbm_a_SOURCES		+=  $(EXTRA_libOSRdvbm_a_SOURCES)
endif
libOSRdvbm_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libOSRdvbm_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME_STR=\"dvbm\" \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include
libOSRdvbm_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOSRdvbm_a_OBJECTS):	$(libOSRdvbm_a_DEPENDENCIES)

$(kpre)dvbmDriver.o: stamp-verobjs
	@list=`$(ECHO) " $(libOSRdvbm_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list
ADDON_MODS			+= $(kpre)dvbmDriver.o
ADDON_OBJS			+= $(libOSRdvbm_a_OBJECTS)

EXTRA_libOSRgncfd_a_SOURCES	= 
libOSRgncfd_a_SOURCES		= \
				src/drivers/gncfd/source/cfd.c \
				src/drivers/gncfd/include/cfd2dlgn.h \
				src/drivers/gncfd/include/cfdextern.h \
				src/drivers/gncfd/include/cfd.h \
				src/drivers/gncfd/include/cfmsg.h \
				src/drivers/gncfd/include/gncfd.h \
				src/drivers/gncfd/include/gnstatus.h

if WITH_LFS
libOSRgncfd_a_SOURCES		+= $(EXTRA_libOSRgncfd_a_SOURCES)
endif
libOSRgncfd_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libOSRgncfd_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME_STR=\"gncfd\" \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include
libOSRgncfd_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOSRgncfd_a_OBJECTS):	$(libOSRgncfd_a_DEPENDENCIES)

$(kpre)gncfdDriver.o: stamp-verobjs
	@list=`$(ECHO) " $(libOSRgncfd_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list
ADDON_MODS			+= $(kpre)gncfdDriver.o
ADDON_OBJS			+= $(libOSRgncfd_a_OBJECTS)

EXTRA_libOSRgpio_a_SOURCES	= 
libOSRgpio_a_SOURCES		= \
				src/drivers/gpio/source/gpiodrv.c \
				src/drivers/gpio/include/gpiodrv.h
libOSRgpio_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libOSRgpio_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME_STR=\"gpio\" \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include
libOSRgpio_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOSRgpio_a_OBJECTS):	$(libOSRgpio_a_SOURCES) $(libOSRgpio_a_DEPENDENCIES)

$(kpre)gpioDriver.o: stamp-verobjs
	@list=`$(ECHO) " $(libOSRgpio_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list
ADDON_MODS			+= $(kpre)gpioDriver.o
ADDON_OBJS			+= $(libOSRgpio_a_OBJECTS)

EXTRA_libOSRsctmr_a_SOURCES	=
libOSRsctmr_a_SOURCES		= src/drivers/sctmr/source/scttimer.c \
				  src/drivers/sctmr/source/sctqueue.c \
				  src/drivers/sctmr/include/diagkern.h \
				  src/drivers/sctmr/include/sctextern.h \
				  src/drivers/sctmr/include/sctqueue.h \
				  src/drivers/sctmr/include/scttimer.h \
				  src/drivers/sctmr/include/timerlibmod.h

if WITH_LFS
libOSRsctmr_a_SOURCES		+= $(EXTRA_libOSRsctmr_a_SOURCES)
endif
libOSRsctmr_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libOSRsctmr_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME_STR=\"sctmr\" \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include
libOSRsctmr_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOSRsctmr_a_OBJECTS):	$(libOSRsctmr_a_DEPENDENCIES)

$(kpre)sctmrDriver.o: stamp-verobjs
	@list=`$(ECHO) " $(libOSRsctmr_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list
ADDON_MODS			+= $(kpre)sctmrDriver.o
ADDON_OBJS			+= $(libOSRsctmr_a_OBJECTS)

## WITH_DLGN
endif

EXTRA_libOSRctimod_a_SOURCES	=
libOSRctimod_a_SOURCES		= \
				src/drivers/ctimod/ctimod.c

libOSRctimod_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libOSRctimod_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME_STR=\"ctimod\" \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include
libOSRctimod_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOSRctimod_a_OBJECTS): $(libOSRctimod_a_DEPENDENCIES)

ctimod.o: stamp-verobjs
	@list=`$(ECHO) " $(libOSRctimod_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

ADDON_MODS			+= ctimod.o
ADDON_OBJS			+= $(libOSRctimod_a_OBJECTS)

if WITH_MERC
## WITH_MERC

EXTRA_libOSRmercd_a_SOURCES	= \
				src/drivers/mercd/source/mercd_native.c \
				src/drivers/mercd/source/mercd_native_util.c
libOSRmercd_a_SOURCES		= \
				src/drivers/mercd/source/mercd.c \
				src/drivers/mercd/source/mercd_linux_pci_dhal.c \
				src/drivers/mercd/source/mercd_linux_osal.c \
				src/drivers/mercd/source/mercd_ossdl_linux.c \
				src/drivers/mercd/source/mercd_osshl_linux_pci.c \
				src/drivers/mercd/source/mercd_linux_entry.c \
				src/drivers/mercd/source/msdbndmgr.c \
				src/drivers/mercd/source/msdbndutl.c \
				src/drivers/mercd/source/msdbrdmgr.c \
				src/drivers/mercd/source/msdbrdutl.c \
				src/drivers/mercd/source/msddbg.c \
				src/drivers/mercd/source/msddpc.c \
				src/drivers/mercd/source/msddrvrmgr.c \
				src/drivers/mercd/source/msdfwmgr.c \
				src/drivers/mercd/source/msdmsgmgr.c \
				src/drivers/mercd/source/msdqueue.c \
				src/drivers/mercd/source/msdrcv.c \
				src/drivers/mercd/source/msdshrdata.c \
				src/drivers/mercd/source/msdsnd.c \
				src/drivers/mercd/source/msdstrmmgr.c \
				src/drivers/mercd/source/msdstrmutl.c \
				src/drivers/mercd/source/msdsupp.c \
				src/drivers/mercd/source/msdtime.c \
				src/drivers/mercd/source/mercd_linux_dma_pci.c \
				src/drivers/mercd/source/mercd_ww_ossdl_linux.c \
				src/drivers/mercd/source/msdwwdpc.c \
				src/drivers/mercd/source/msdwwinitmngr.c \
				src/drivers/mercd/source/msdwwinitmngrutl.c \
				src/drivers/mercd/source/msdwwmsgutl.c \
				src/drivers/mercd/source/msdwwrcv.c \
				src/drivers/mercd/source/msdwwsnd.c \
				src/drivers/mercd/source/msdwwstrmutl.c \
				src/drivers/mercd/source/msdwwtime.c \
				src/drivers/mercd/source/msdwwutl.c \
				src/drivers/mercd/include/ctimod.h \
				src/drivers/mercd/include/debug.h \
				src/drivers/mercd/include/dlver.h \
				src/drivers/mercd/include/dtrace.h \
				src/drivers/mercd/include/mercd_dhal.h \
				src/drivers/mercd/include/mercd.h \
				src/drivers/mercd/include/mercd_osal.h \
				src/drivers/mercd/include/msddbg.h \
				src/drivers/mercd/include/msddlver.h \
				src/drivers/mercd/include/msddrvparam.h \
				src/drivers/mercd/include/msdextern.h \
				src/drivers/mercd/include/msd.h \
				src/drivers/mercd/include/msdlinux.h \
				src/drivers/mercd/include/msdlinuxlock.h \
				src/drivers/mercd/include/msdlinuxwwmacros.h \
				src/drivers/mercd/include/msdmacro.h \
				src/drivers/mercd/include/msdmercif.h \
				src/drivers/mercd/include/msdmmgr.h \
				src/drivers/mercd/include/msdmsgdef.h \
				src/drivers/mercd/include/msdmsgs.h \
				src/drivers/mercd/include/msdnative.h \
				src/drivers/mercd/include/msdparam.h \
				src/drivers/mercd/include/msdpciif.h \
				src/drivers/mercd/include/msdqueue.h \
				src/drivers/mercd/include/msdshrdata.h \
				src/drivers/mercd/include/msdsram.h \
				src/drivers/mercd/include/msdsramif.h \
				src/drivers/mercd/include/msdstrms.h \
				src/drivers/mercd/include/msdstruct.h \
				src/drivers/mercd/include/msdthread.h \
				src/drivers/mercd/include/msdtypes.h \
				src/drivers/mercd/include/msdvbrd.h \
				src/drivers/mercd/include/msdwwdefs.h \
				src/drivers/mercd/include/msdwwtypes.h \
				src/drivers/mercd/include/qdrverr.h \
				src/drivers/mercd/include/qhosttypes.h \
				src/drivers/mercd/include/qoscommon.h \
				src/drivers/mercd/include/qstrtype.h \
				src/drivers/mercd/include/qtypes.h \
				src/drivers/mercd/source/msdversion.ver

libOSRmercd_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libOSRmercd_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME_STR=\"mercd\" \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include \
				  -I./src/drivers/mercd/include -I$(srcdir)/src/drivers/mercd/include
libOSRmercd_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOSRmercd_a_OBJECTS):	$(libOSRmercd_a_DEPENDENCIES)

$(kpre)mercdDriver.o: stamp-verobjs
	@list=`$(ECHO) " $(libOSRmercd_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list
ADDON_MODS			+= $(kpre)mercdDriver.o
ADDON_OBJS			+= $(libOSRmercd_a_OBJECTS)

## WITH_MERC
endif

if WITH_PMAC
## WITH_PMAC

EXTRA_libOSRpmacd_a_SOURCES	=
libOSRpmacd_a_SOURCES		= \
				src/drivers/pmacd/source/pmacd_board.c \
				src/drivers/pmacd/source/pmacd_control.c \
				src/drivers/pmacd/source/pmacd_driver.c \
				src/drivers/pmacd/source/pmacd_interrupt.c \
				src/drivers/pmacd/source/pmacd_linux_entry.c \
				src/drivers/pmacd/source/pmacd_linux_pci.c \
				src/drivers/pmacd/source/pmacd_mf.c \
				src/drivers/pmacd/source/pmacd_parameters.c \
				src/drivers/pmacd/source/pmacd_pmb.c \
				src/drivers/pmacd/source/pmacd_proc.c \
				src/drivers/pmacd/source/pmacd_stream.c \
				src/drivers/pmacd/source/pmacd_version.c \
				src/drivers/pmacd/include/21554.h \
				src/drivers/pmacd/include/pmacd_board.h \
				src/drivers/pmacd/include/pmacd_control.h \
				src/drivers/pmacd/include/pmacd_driver.h \
				src/drivers/pmacd/include/pmacd.h \
				src/drivers/pmacd/include/pmacd_interrupt.h \
				src/drivers/pmacd/include/pmacd_linux_entry.h \
				src/drivers/pmacd/include/pmacd_linux_pci.h \
				src/drivers/pmacd/include/pmacd_mf.h \
				src/drivers/pmacd/include/pmacd_parameters.h \
				src/drivers/pmacd/include/pmacd_pmb.h \
				src/drivers/pmacd/include/pmacd_proc.h \
				src/drivers/pmacd/include/pmacd_stream.h
libOSRpmacd_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libOSRpmacd_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB) \
				  -DLIS_OBJNAME_STR=\"pmacd\" \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include \
				  -I./src/drivers/mercd/include -I$(srcdir)/src/drivers/mercd/include \
				  -I./src/drivers/pmacd/include -I$(srcdir)/src/drivers/pmacd/include
libOSRpmacd_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOSRpmacd_a_OBJECTS):	$(libOSRpmacd_a_DEPENDENCIES)

pmacd.o: stamp-verobjs
	@list=`$(ECHO) " $(libOSRpmacd_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

ADDON_MODS			+= pmacd.o
ADDON_OBJS			+= $(libOSRpmacd_a_OBJECTS)

## WITH_PMAC
endif

## RPM_BUILD_KERNEL
endif
## PKG_BUILD_ARCH
endif

EXTRA_TEX			+= \
				scripts/texinfo.cnf \
				scripts/texi/args.texi.in \
				scripts/texi/fdl.texi \
				scripts/texi/gpl.texi \
				scripts/texi/install.texi \
				scripts/texi/lesser.texi \
				scripts/texi/testsuite.texi

EXTRA_DIST			+= $(EXTRA_TEX)
dist_noinst_SCRIPTS		+= configure.nexusware
EXTRA_SCRIPTS			+= configure.nexusware

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(ADDON_OBJS)
KERNEL_MODULES			= $(ADDON_MODS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			= $(STREAMS_MODMAP) $(STRCOMP_MODMAP)

if WITH_KO_MODULES
## WITH_KO_MODULES
kpre				= streams_
else
kpre				= streams-
## WITH_KO_MODULES
endif

include am/kernel.am

if WITH_KO_MODULES
else
if PKG_BUILD_INDEP
if RPM_BUILD_USER
nodist_modutil_DATA		= \
				src/util/modutils/@PACKAGE_LCNAME@
endif
endif
endif

##
## Rules for building libraries
##
##LIBRARY_VERSION_FILES		=
##
##include am/libraries.am

EXPORT_SRCFILES			= \
				src/drivers/ctimod/ctimod.c

if WITH_DLGN
## WITH_DLGN
EXPORT_SRCFILES			+= \
				src/drivers/dlgn/source/dlgn.c
## WITH_DLGN
endif

EXPORT_VERFILES			= \
				$(PKGINCL)/versions/ctimod.ver

if WITH_DLGN
## WITH_DLGN
EXPORT_VERFILES			+= \
				$(PKGINCL)/versions/dlgn.ver
## WITH_DLGN
endif


GENKSYMS_CPPFLAGS		= \
				  -I./src/drivers -I$(srcdir)/src/drivers \
				  -I./src/modules -I$(srcdir)/src/modules \
				  -I./src/drivers/dlgn/include -I$(srcdir)/src/drivers/dlgn/include \
				  -I./src/drivers/dvbm/include -I$(srcdir)/src/drivers/dvbm/include \
				  -I./src/drivers/gncfd/include -I$(srcdir)/src/drivers/gncfd/include \
				  -I./src/drivers/gpio/include -I$(srcdir)/src/drivers/gpio/include \
				  -I./src/drivers/sctmr/include -I$(srcdir)/src/drivers/sctmr/include \
				  -I./src/drivers/mercd/include -I$(srcdir)/src/drivers/mercd/include \
				  -I./src/drivers/pmacd/include -I$(srcdir)/src/drivers/pmacd/include \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS)

EXTRA_DIST			+= \
				  $(top_srcdir)/scripts/hot.lds

##
## Rules for generating versioned kernel symbols
##
include am/genksyms.am

##
## Rules for generating STREAMS configuration
##
include am/strconf.am

if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS = src

SUBDIRS += .

if WITH_32BIT_LIBS
SUBDIRS += lib32
endif

SUBDIRS += tests

if DEVELOPMENT
SUBDIRS += doc
endif

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

if INSTALL_INITSCRIPTS
## INSTALL_INITSCRIPTS

if WITH_RCSD_DIRECTORY
## WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				  src/util/init/@PACKAGE_LCNAME@.sh

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				  src/util/init/@PACKAGE_LCNAME@

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= \
				  src/util/sysconfig/@PACKAGE_LCNAME@.conf

dist_config_DATA		= \
				  src/util/sysconfig/@PACKAGE_LCNAME@

## INSTALL_INITSCRIPTS
endif

## PKG_BUILD_INDEP
endif

## RPM_BUILD_USER
endif

##
## Rules for init scripts
##
include am/init.am

##
## Rules for lsms
##
include am/lsm.am

##
## Rules for archives
##
include am/archive.am

##
## Rules for making rpms
##
AM_RPMFLAGS			= --define "_kversion $(kversion)"
AM_SRPMFLAGS			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

##
## Rules for making release files
##
MANUAL_FILE			= doc/manual/$(PACKAGE).txt

include am/release.am

##
## Rules for doxygen documentation.
##
include am/doxy.am

EXTRA_DIST			+= \
				  debian/README.Debian \
				  debian/TODO.Debian

MAINTAINERCLEANFILES		+= \
				  $(top_srcdir)/aclocal.m4 \
				  $(top_srcdir)/config.h.in \
				  $(top_srcdir)/configure \
				  $(top_srcdir)/COPYING \
				  $(top_srcdir)/INSTALL \
				  $(top_srcdir)/scripts/compile \
				  $(top_srcdir)/scripts/config.guess \
				  $(top_srcdir)/scripts/config.sub \
				  $(top_srcdir)/scripts/depcomp \
				  $(top_srcdir)/scripts/install-sh \
				  $(top_srcdir)/scripts/ltmain.sh \
				  $(top_srcdir)/scripts/missing \
				  $(top_srcdir)/scripts/texinfo.tex

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
