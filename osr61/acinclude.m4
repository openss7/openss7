# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
# =============================================================================
# BEGINNING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# 
# @(#) $RCSfile: acinclude.m4,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/02/28 06:30:24 $
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
# Last Modified $Date: 2007/02/28 06:30:24 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: acinclude.m4,v $
# Revision 0.9.2.4  2007/02/28 06:30:24  brian
# - updates and corrections, #ifdef instead of #if
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
m4_include([m4/openss7.m4])
m4_include([m4/dist.m4])
m4_include([m4/pr.m4])
m4_include([m4/init.m4])
m4_include([m4/kernel.m4])
m4_include([m4/devfs.m4])
m4_include([m4/genksyms.m4])
m4_include([m4/man.m4])
m4_include([m4/public.m4])
m4_include([m4/rpm.m4])
m4_include([m4/deb.m4])
m4_include([m4/libraries.m4])
m4_include([m4/autotest.m4])
m4_include([m4/strconf.m4])
m4_include([m4/streams.m4])
m4_include([m4/strcomp.m4])
dnl m4_include([m4/xopen.m4])
m4_include([m4/xns.m4])
m4_include([m4/xti.m4])
m4_include([m4/doxy.m4])

# =============================================================================
# AC_OSR61
# -----------------------------------------------------------------------------
AC_DEFUN([AC_OSR61], [dnl
    _OPENSS7_PACKAGE([OSR61], [Dialogic Open Systems Release 6.1])
    _OSR61_OPTIONS
    _AUTOPR
    _MAN_CONVERSION
    _PUBLIC_RELEASE
    _INIT_SCRIPTS
    _RPM_SPEC
    _DEB_DPKG
    AC_CONFIG_FILES([debian/osr61-core.postinst
		     debian/osr61-core.postrm
		     debian/osr61-core.preinst
		     debian/osr61-core.prerm
		     debian/osr61-devel.preinst
		     debian/osr61-dev.postinst
		     debian/osr61-dev.preinst
		     debian/osr61-dev.prerm
		     debian/osr61-doc.postinst
		     debian/osr61-doc.preinst
		     debian/osr61-doc.prerm
		     debian/osr61-init.postinst
		     debian/osr61-init.postrm
		     debian/osr61-init.preinst
		     debian/osr61-init.prerm
		     debian/osr61-lib.preinst
		     debian/osr61-source.preinst
		     debian/osr61-util.preinst
		     src/util/modutils/osr61
		     src/include/sys/osr61/version.h])
    _LDCONFIG
    USER_CPPFLAGS="$CPPFLAGS"
    USER_CFLAGS="$CFLAGS"
    USER_LDFLAGS="$LDFLAGS"
    _OSR61_SETUP
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/config.h'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-imacros ${top_builddir}/${STRCONF_CONFIG}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_srcdir}'
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XTI_CPPFLAGS:+ }}${XTI_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${XNS_CPPFLAGS:+ }}${XNS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STRCOMP_CPPFLAGS:+ }}${STRCOMP_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+${STREAMS_CPPFLAGS:+ }}${STREAMS_CPPFLAGS}"
    PKG_INCLUDES="${PKG_INCLUDES}${PKG_INCLUDES:+ }"'-I${top_builddir}/src/include -I${top_srcdir}/src/include'
dnl if echo "$KERNEL_MODFLAGS" | grep 'modversions\.h' >/dev/null 2>&1 ; then
dnl	PKG_MODFLAGS='-include $(top_builddir)/$(MODVERSIONS_H)'
dnl fi
    PKG_MODFLAGS='$(STREAMS_MODFLAGS) $(STRCOMP_MODFLAGS)'
dnl Just check config.log if you want to see these...
dnl AC_MSG_NOTICE([final user    CPPFLAGS  = $USER_CPPFLAGS])
dnl AC_MSG_NOTICE([final user    CFLAGS    = $USER_CFLAGS])
dnl AC_MSG_NOTICE([final user    LDFLAGS   = $USER_LDFLAGS])
dnl AC_MSG_NOTICE([final package INCLUDES  = $PKG_INCLUDES])
dnl AC_MSG_NOTICE([final package MODFLAGS  = $PKG_MODFLAGS])
dnl AC_MSG_NOTICE([final kernel  MODFLAGS  = $KERNEL_MODFLAGS])
dnl AC_MSG_NOTICE([final kernel  NOVERSION = $KERNEL_NOVERSION])
dnl AC_MSG_NOTICE([final kernel  CPPFLAGS  = $KERNEL_CPPFLAGS])
dnl AC_MSG_NOTICE([final kernel  CFLAGS    = $KERNEL_CFLAGS])
dnl AC_MSG_NOTICE([final kernel  LDFLAGS   = $KERNEL_LDFLAGS])
dnl AC_MSG_NOTICE([final streams CPPFLAGS  = $STREAMS_CPPFLAGS])
dnl AC_MSG_NOTICE([final streams MODFLAGS  = $STREAMS_MODFLAGS])
    AC_SUBST([USER_CPPFLAGS])dnl
    AC_SUBST([USER_CFLAGS])dnl
    AC_SUBST([USER_LDFLAGS])dnl
    AC_SUBST([PKG_INCLUDES])dnl
    AC_SUBST([PKG_MODFLAGS])dnl
    PKG_MANPATH='$(mandir)'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    PKG_MANPATH="${STREAMS_MANPATH:+${STREAMS_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${STRCOMP_MANPATH:+${STRCOMP_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XNS_MANPATH:+${XNS_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH="${XTI_MANPATH:+${XTI_MANPATH}${PKG_MANPATH:+:}}${PKG_MANPATH}"
    PKG_MANPATH='$(top_builddir)/doc/man'"${PKG_MANPATH:+:}${PKG_MANPATH}"
    AC_SUBST([PKG_MANPATH])dnl
    CPPFLAGS=
    CFLAGS=
    _OSR61_OUTPUT
    _AUTOTEST
    _DOXY
])# AC_OSR61
# =============================================================================

# =============================================================================
# _OSR61_OPTIONS
# -----------------------------------------------------------------------------
AC_DEFUN([_OSR61_OPTIONS], [dnl
    AC_ARG_ENABLE([dlgn],
	AS_HELP_STRING([--disable-dlgn],
	    [disable Dialogic SpringWare drivers. @<:@default=enabled@:>@]),
	[enable_dlgn="$enableval"],
	[enable_dlgn='yes'])
    AM_CONDITIONAL([WITH_DLGN], [test x"$enable_dlgn" = xyes])dnl
    AC_ARG_ENABLE([antares],
	AS_HELP_STRING([--disable-antares],
	    [disable Antares support for Dialogic SpringWare drivers.
	     @<:@default=enabled@:>@]),
	[enable_antares="$enableval"],
	[enable_antares='yes'])
    AM_CONDITIONAL([WITH_ANTARES], [test x"$enable_dlgn" = xyes -a x"$enable_antares" = xyes])
    AC_ARG_ENABLE([merc],
	AS_HELP_STRING([--disable-merc],
	    [disable Dialogic DM3 drivers. @<:@default=enabled@:>@]),
	[enable_merc="$enableval"],
	[enable_merc='yes'])
    AM_CONDITIONAL([WITH_MERC], [test x"$enable_merc" = xyes])dnl
    AC_ARG_ENABLE([pmac],
	AS_HELP_STRING([--disable-pmac],
	    [disable Dialogic IPT drivers. @<:@default=enabled@:>@]),
	[enable_pmac="$enableval"],
	[enable_pmac='yes'])
    AM_CONDITIONAL([WITH_PMAC], [test x"$enable_pmac" = xyes])dnl
dnl
dnl These are from the Intel make files
dnl
    AH_TEMPLATE([__DRVDEBUG_H__], [When defined, suppresses debug printing for
		 the DLGN driver.  This is defined by the Intel make files.  It
		 is normally defined.])
    AH_TEMPLATE([PMACD_VERSION], [Simply provides a version string for the PMACD
		 driver.])
    AH_TEMPLATE([PKGVER], [Defines the OSR 6.1 package version string.  Do not
		 change this value.])
    AH_TEMPLATE([OSTYPEVER], [Defines the OS type version.])
    AH_TEMPLATE([COPYRIGHT], [Defines the Intel's original copyright string.
		 Do not change this value: it is set Intel's original
		 copyright string from the OSR 6.1 Linux Driver GPL release.])
dnl
dnl These are extras drug out with ifnames:
dnl
    AH_TEMPLATE([AAAAAA], [Causes the complex QStreamRef structure to be defined
		 which contains a 3 element array of 24-bit values (yes, gcc can
		 do this).  This is normally undefined.])
    AH_TEMPLATE([ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS], [Causes some checks for
		 abnormal termination to be use that otherwise would not,
		 particularly in strm_ack2usr for the mercury driver.  This is
		 normally undefined.])
    AH_TEMPLATE([_ABNORMAL_TERM_CLEAN_FOR_NO_CLOSE_ACKS], [Causes additional
		 checks for abnormal termination in a number of places in the
		 mercury driver.  This is normally undefined.])
    AH_TEMPLATE([ANHD_DEBUG], [Causes the rcu array to be copied into the
		 dbg_busy array in delete_rcus found in file drvprot.c.  This
		 is normally undefined.])
    AH_TEMPLATE([ANT_AIX], [Define when Antares drivers are build built for the
		 AIX operating system.  The current build only supports Linux
		 and this is normally undefined.])
    AH_TEMPLATE([ANTARES], [Define when both ANTARES and SRAM PMs are to be
		 enabled and managed in the SpringWare drivers.  This is
		 normally undefined.])
    AH_TEMPLATE([ANT_BRD_DBG], [Enables some additional ANTARES PCI board
		 debugging.  This is normally undefined.])
    AH_TEMPLATE([ATN_CNVRTR_BRD], [Define when the driver is to be built for the
		 ANTARES converted board.  This is normally undefined.])
    AH_TEMPLATE([BIG_ANT], [Define when the ANTARES drivers are compiled for Big
		 Endian hosts.  This is set automagically by autoconf and is not
		 normally set manually.])
    AH_TEMPLATE([BIG_ENDIAN], [Define when the drivers are compiled for Big
		 Endian hosts.  This is set automagically by autoconf and is not
		 normally set manully.])
    AH_TEMPLATE([BRI_SUPPORT], [Enables BRI support in the DLGN drivers in a
		 number of areas.  This is normally defined (according the Intel
		 make files).  We provide an option for it but it defaults to
		 defined.])
    AH_TEMPLATE([BTYE_COPY_ALL_THE_WAY], [The DLGN driver defines sr_copy()
		 which is used to copy bytes.  Defining this macro causes
		 non-optimized byte copies to be performed al the time.  This is
		 normally not defined.])
    AH_TEMPLATE([CONSERVE_MEMORY], [When defined this causes the DLGN driver to
		 reuse the message block in one instance: when making the final
		 reply to a request.  This is not normally defined.])
    AH_TEMPLATE([DBGPRNT], [When defined, prints a few debug statements in the
		 ANTARES device access module.  This is not normally defined.])
    AH_TEMPLATE([DBG_PRT], [When defined, causes the SRAM interrupt service
		 routing to print one debug statement in one circumstance.  This
		 is not normally defined.])
    AH_TEMPLATE([DCB], [When defined, also support the DCB family of cards,
		 DCB320, DCB640, DCB960, in the DLGN driver.  This is normally
		 defined, and was defined in the original Intel make files.])
    AH_TEMPLATE([DEBUG], [When defined causes some debugging messages to be
		 printed at a couple of places across most drivers.  This is not
		 normally defined.])
    AH_TEMPLATE([DEBUGPRT], [This is the principle debugging flag for the DM3
		 driver MERCD.  It is notnormally defined.  This debugging
		 faciliy should be converted to strlog(9).  This is not normally
		 defined.])
    AH_TEMPLATE([DEBUG_ENABLED], [When defined causes some specific manual
		 message trace debugging to be performed according to manual
		 changes in src/drivers/mercd/include/msgdbg.h.  This mechanism
		 is another good candidate for conversion to strlog(9).  This is
		 not normally defined and the settings in msgdbg.h seem to be
		 set to the last values used by the last debugging session.])
    AH_TEMPLATE([DEBUG_SELECT], [When defined, issues a few selected debug
		 statements that were probably for some specific debugging
		 purpose in the MERCD driver.  This is not normally defined.])
    AH_TEMPLATE([DEBUG_TRACE], [When defined, issues a few select debugg
		 statements debugging the trace facility of the MERCD driver.
		 This is not normally defined.])
    AH_TEMPLATE([DIAGALLOUT], [When defined causes all diagnostic settings for
		 the SCTMR module to be defeated.  This is not normally
		 defined. The entire mechanism should be replaced with
		 strlog(9).])
    AH_TEMPLATE([DIAGBUFCHECK_OFF], [When defined, defeats diagnostig debug
		 checking the SCTMR module.  This is not normally defined. The
		 entire mechanism should be replaced with strlog(9).])
    AH_TEMPLATE([DIAGERR_OFF], [When defined, defeats diagnostic error checking
		 in the SCTMR module.  This is not normally defined. The entire
		 mechanism should be replaced with strlog(9).])
    AH_TEMPLATE([DIAGINFO_LEVEL], [Defines the dignostic information level
		 between 2 and 9 for the SCTMR module.  This is not normally
		 defined. The entire mechanism should be replaced with
		 strlog(9).])
    AH_TEMPLATE([DIAGINFO_ON], [When defined, turns diagnostic information on in
		 the SCTMR module.  This can have a value when defined from 2 to
		 9.  This is not normally defined. The entire mechanism should
		 be replaced with strlog(9).])
    AH_TEMPLATE([DIAGTRACE_ON], [More SCTMR module debugging flags.  The entire
		 mechanism should be replaced with strlog(9).])
    AH_TEMPLATE([DIAGWARN_OFF], [More SCTMR module debugging flags.  The entire
		 mechanism should be replaced with strlog(9).])
    AH_TEMPLATE([DIAG_FLAG_VAR], [When defined, specifies the name of the
		 runtime diagnostic flag for the SCTMR module.  The entire
		 mechanism should be replaced by strlog(9).])
    AH_TEMPLATE([DLGN_DEBUG], [When defined, turns on a couple of debug
		 statements for the DLGN driver.  This is normally undefined.])
    AH_TEMPLATE([DLGN_ISDN], [When defined, provide ISDN support for the DLGN
		 driver.  The Intel make files define this.  This is normally
		 defined, but we provide a configuration option to disable it.])
    AH_TEMPLATE([DONGLE_SECURITY], [When defined, turns on some sort of dongle
		 detection for cards for the DLGN driver.  This is normally not
		 defined])
    AH_TEMPLATE([DOWNLOADER_CHANGE], [When defined, enables a check whether the
		 adapter is in the downloaded state in the MERCD driver.  This
		 is not normally defined.])
    AH_TEMPLATE([DRVR_STATISTICS], [When defined, causes the MERCD driver to
		 maintain runtime dianostic statistics.  This is not normally
		 defined.])
    AH_TEMPLATE([DUAL_FLIP_FLOPS], [When defined, adds some minor additional
		 support to the ANTARES PCI driver.  This is not normally
		 defined.])
    AH_TEMPLATE([ENABLE_LATER], [When defined, enables two traces in the MERCD
		 driver.  This is not normally defined.])
    AH_TEMPLATE([ENABLE_NO_REQUIRED_THINGS], [When defined, defines a few unused
		 subclass defines, in src/drivers/mercd/include/msdmsgdef.h.
		 This is not normaly defined.])
    AH_TEMPLATE([ENDIAN_SWAPPED_AT_FUNNEL_TIP], [When defined causes endian
		 swaping on two elements of a data structure in the DLGN driver.
		 This is not normally defined.])
    AH_TEMPLATE([ISA_SOLDEB], [When defined, turns on one debug print for the
		 GPIO driver.  This is not normally defined.])
    AH_TEMPLATE([ISA_SPAN], [When defined, adds support for ISA spans to the
		 DLGN driver.  Note that this is also necessary for PCI support.
		 This is normally defined.])
    AH_TEMPLATE([IST], [When defined, times selected timers in the DLGN driver.
		 This is not normally defined.])
    AH_TEMPLATE([Linux_BuildSegs], [When defined, builds and uses a memory
		 segment mapping table for mapping memory from user to kernel
		 space.  This is not normally defined.])
    AH_TEMPLATE([MERCD_DATATYPE_INTEL], [When defined, the MERCD driver will be
		 informed that integer data type can be used for copy instead of
		 byte data types.  This is defined by default by the Intel make
		 files.  See src/drivers/mercd/include/msdsram.h  This is
		 normally defined.])
    AH_TEMPLATE([MERCD_LINUX], [When defined, the MERCD driver is characterized
		 for Linux operation.  These are mostly LiS'ism characterization
		 but some is applicable to LFS as well.  This is defined by the
		 Intel make files for Linux.  This is normally defined.])
    AH_TEMPLATE([MERCD_MERC], [When defined, secondary MERCD driver board ids
		 will be used.  MERCD_PCI is normally defined instead.  This is
		 not defined by the Intel make files for Linux.])
    AH_TEMPLATE([MERCD_PCI], [When defined, provides PCI support for the MERCD
		 driver.  This is normally defined instead of MERCD_VME and is
		 defined in the Intel make file for Linux.])
    AH_TEMPLATE([MERCD_VME], [When defined, provides VME support for the MERCD
		 driver.  This is normally defined instead of MERCD_PCI but is
		 not defined in the Intel make file for Linux.])
    AH_TEMPLATE([MYCHECK], [When defined, adds some checks to the malloc
		 functions in the CTIMOD module.  This is normally undefined.])
    AH_TEMPLATE([NOSBA], [When defined, a SpringBoard adapter is present and
		 supported.  The Intel make files define this macro.  This is
		 normally defined.  An option is provided for this flag.])
    AH_TEMPLATE([NOSEARCH], [When defined, multiple SMs per LDP are supported
		 for the DLGN driver.  This is normally undefined.])
    AH_TEMPLATE([NO_FIX], [When defined, disables some Lucent fix for the DVBM
		 module.  This is normally undefined.])
    AH_TEMPLATE([PCI], [When defined, the host supports a PCI bus.  This is
		 normally defined in qhosttypes.h for the MERCD driver, but is
		 also defined by the Intel make files.  This is normally
		 defined.  For Linux we define it when CONFIG_PCI is defined.])
    AH_TEMPLATE([PCI_ANT], [When defined, provides PCI support for ANTARES for
		 the DLGN driver.  This is normally defined.  For Linux we
		 define it when CONFIG_PCI is defined.])
    AH_TEMPLATE([PCI_DEBUG], [When defined, a debug statement will be printed
		 when there are no ANTARES PCI boards present in the system.
		 This is normally undefined.])
    AH_TEMPLATE([PCI_DEBUG2], [When defined additional debug information will be
		 printed concerning ANTARES PCI boards present in the system.
		 This is normally undefined.])
    AH_TEMPLATE([PCI_INTR_DEBUG], [WHen defined ANTARES PCI boards will have
		 edge/level interrupt flip-flops reset.  This is normally
		 undefined.])
    AH_TEMPLATE([PCI_SPAN], [When defined, PCI boards will be supported by the
		 DLGN driver.  This is normally defined.  For Linux, we define
		 it when CONFIG_PCI is defined.])
    AH_TEMPLATE([POST_DMA_FREE], [When defined, the MERCD driver will defer
		 freeing until after DMA.  This is not required according to the
		 inline documentation.  This is normally undefined.])
    AH_TEMPLATE([PPS], [When defined, suppresses some board check and virtual
		 mapping functions in the MERCD driver.  The Intel make files
		 define this.  This is normally defined.  We also provide a
		 configuration option for it.])
    AH_TEMPLATE([RTDL_ANT], [When defined, enables the remote download feature
		 of the ANTARES in the DLGN driver.  This is normally undefined.
		 We also provide a configuration option for it.])
    AH_TEMPLATE([SE_NOSLP], [Actually does not do anything for Linux, but it is
		 defined by the Intel make files, so we define it too.  Normally
		 defined to 1.])
    AH_TEMPLATE([SKIP], [Adds some tracing to the MERCD driver.  Do not define
		 this.  Normally undefined.])
    AH_TEMPLATE([SRAM_DRVR_DMA_SUPPORT], [When defined, adds shared RAM DMA
		 support to the MERCD driver.  This is defined by the Inte make
		 files.  This is normally defined.  We provide a configure
		 option to disable it.])
    AH_TEMPLATE([UNIX], [The DLGN driver checks this to disable "far", "huge"
		 and "near" pointer attributes.  This should always be defined
		 for Linux.])
    AH_TEMPLATE([USE_LOGICALID], [When defined, the MERCD driver uses logical
		 identifiers for boards.  This is not defined by the Intel make
		 files and is therefore normally undefined.  We provide a
		 configure option to enable it.])
    AH_TEMPLATE([VME], [When defined, the host supports a VME bus.  This is
		 normally defined in qhosttypes.h for the MERCD driver, but is
		 not defined by the Intel make files.  This is normally
		 undefined.  For Linux we define it when CONFIG_VME is defined.])
    AH_TEMPLATE([VME_ANT], [When defined, provides VME support for ANTARES for
		 the DLGN driver.  This is normally defined.  For Linux we
		 define it when CONFIG_VME is defined.])
    AH_TEMPLATE([VME_DEBUG], [When defined, additional debug information will be
		 printed concerning ANTARES VME boards present in the system.
		 This is the primary debug flag for VME systems.  This is
		 normally undefined.])
    AH_TEMPLATE([VME_DEBUG_2], [When defined, adds additional debug prints.
		 This is normally undefined.])
    AH_TEMPLATE([VME_DEBUG_DONGLE], [When defined, adds additional debug prints.
		 This is normally undefined.])
    AH_TEMPLATE([VME_LIVE_INSERTION], [When defined, enables VME live
		 insertion.  This is normally undefined.])
    AH_TEMPLATE([VME_LIVE_INSERTION_B], [When defined, enables VME live
		 insertion.  This is normally undefined.])
    AH_TEMPLATE([VME_SPAN], [When defined, VME boards will be supported by the
		 DLGN driver.  This is normally undefined.  For Linux, we
		 defined it when CONFIG_VME is defined.])
    AH_TEMPLATE([XPRT], [When defined, print debugging information for the GPIO
		 driver.  This is normally undefined.])
    AH_TEMPLATE([_STATS_], [When defined, provide statistics support for the
		 DLGN driver.  The Intel make files define this.  This is
		 normally defined, but we provide a configuration option to
		 disable it.])
dnl
dnl These are extras drug out with ifnames:
dnl
    AC_ARG_ENABLE([dlgn-bri],
	AS_HELP_STRING([--disable-dlgn-bri],
	    [disable BRI support for Dialogic SpringWare drivers. @<:@default=enabled@:>@]),
	[enable_dlgn_bri="$enableval"],
	[enable_dlgn_bri='yes'])
    AC_ARG_ENABLE([dlgn-dcb],
	AS_HELP_STRING([--disable-dlgn-dcb],
	    [disable DCB support for Dialogic SpringWare drivers.  @<:@default=enabled@:>@]),
	[enable_dlgn_dcb="$enableval"],
	[enable_dlgn_dcb='yes'])
    AC_ARG_ENABLE([dlgn-isa],
	AS_HELP_STRING([--disable-dlgn-isa],
	    [disable ISA support for Dialogic SpringWare drivers.  @<:@default=enabled@:>@]),
	[enable_dlgn_isa="$enableval"],
	[enable_dlgn_isa='yes'])
    AC_ARG_ENABLE([dlgn-sba],
	AS_HELP_STRING([--enable-dlgn-sba],
	    [enable SBA support for Dialogic SpringWare drivers.  @<:@default=disabled@:>@]),
	[enable_dlgn_sba="$enableval"],
	[enable_dlgn_sba='no'])
    AC_ARG_ENABLE([dlgn-msm],
	AS_HELP_STRING([--enable-dlgn-msm],
	    [enable multiple SMs per LDP for Dialogic SpringWare drivers.  @<:@default=disabled@:>@]),
	[enable_dlgn_msm="$enableval"],
	[enable_dlgn_msm='no'])
    AC_ARG_ENABLE([mercd-pps],
	AS_HELP_STRING([--disable-mercd-pps],
	    [disable PPS for MERCD driver. @<:@default=enabled@:>@]),
	[enable_mercd_pps="$enableval"],
	[enable_mercd_pps='yes'])
    AC_ARG_ENABLE([antares-rtdl],
	AS_HELP_STRING([--enable-antares-rtdl],
	    [enable remote download for ANTARES. @<:@default=disabled@:>@]),
	[enable_antares_rtdl="$enableval"],
	[enable_antares_rtdl='no'])
    AC_ARG_ENABLE([mercd-dma],
	AS_HELP_STRING([--disable-mercd-dma],
	    [disable SRAM DMA support for MERCD.  @<:@default=enabled@:>@]),
	[enable_mercd_dma="$enableval"],
	[enable_mercd_dma='yes'])
    AC_ARG_ENABLE([mercd-lid],
	AS_HELP_STRING([--enable-mercd-lid],
	    [enable Logical ID support for MERCD.  @<:@default=disabled@:>@]),
	[enable_mercd_lid="$enableval"],
	[enable_mercd_lid='no'])
    AC_ARG_ENABLE([dlgn-isdn],
	AS_HELP_STRING([--disable-dlgn-isdn],
	    [disable ISDN support for DLGN.  @<:@default=enabled@:>@]),
	[enable_dlgn_isdn="$enableval"],
	[enable_dlgn_isdn='yes'])
    AC_ARG_ENABLE([dlgn-stats],
	AS_HELP_STRING([--disable-dlgn-stats],
	    [disable statistics support for DLGN. @<:@default=enabled@:>@]),
	[enable_dlgn_stats="$enableval"],
	[enable_dlgn_stats='yes'])
])# _OSR61_OPTIONS
# =============================================================================

# =============================================================================
# _OSR61_SETUP
# -----------------------------------------------------------------------------
AC_DEFUN([_OSR61_SETUP], [dnl
    _LINUX_KERNEL
    _LINUX_DEVFS
    _GENKSYMS
    # here we have our flags set and can perform preprocessor and compiler
    # checks on the kernel
    _LINUX_STREAMS
    _STRCOMP
dnl _XOPEN
    _XNS
    _XTI
    _OSR61_CHECKS
])# _OSR61_SETUP
# =============================================================================

# =============================================================================
# _OSR61_CHECKS
# -----------------------------------------------------------------------------
AC_DEFUN([_OSR61_CHECKS], [dnl
    _LINUX_CHECK_HEADERS([linux/namespace.h linux/kdev_t.h linux/statfs.h linux/namei.h \
			  linux/locks.h asm/softirq.h linux/slab.h linux/cdev.h \
			  linux/hardirq.h linux/cpumask.h linux/kref.h linux/security.h \
			  asm/uaccess.h linux/kthread.h linux/compat.h linux/ioctl32.h \
			  asm/ioctl32.h linux/syscalls.h linux/rwsem.h linux/smp_lock.h \
			  linux/devfs_fs_kernel.h linux/compile.h linux/utsrelease.h], [:], [:], [
#include <linux/compiler.h>
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/timer.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#include <linux/sched.h>
])
    _LINUX_CHECK_FUNCS([remap_pfn_range remap_page_range], [:], [:], [
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/timer.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_CPUMASK_H
#include <linux/cpumask.h>
#endif
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#include <linux/mm.h>
])
    _LINUX_KERNEL_ENV([dnl
	AC_CACHE_CHECK([for kernel remap_page_range with 4 arguments], [linux_cv_have_remap_page_range_4args], [dnl
	    AC_COMPILE_IFELSE([
		AC_LANG_PROGRAM([[
#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/timer.h>
#ifdef HAVE_KINC_LINUX_LOCKS_H
#include <linux/locks.h>
#endif
#ifdef HAVE_KINC_LINUX_SLAB_H
#include <linux/slab.h>
#endif
#include <linux/fs.h>
#ifdef HAVE_KINC_LINUX_CPUMASK_H
#include <linux/cpumask.h>
#endif
#include <linux/sched.h>
#include <linux/wait.h>
#ifdef HAVE_KINC_LINUX_KDEV_T_H
#include <linux/kdev_t.h>
#endif
#include <linux/interrupt.h>	/* for cpu_raise_softirq */
#ifdef HAVE_KINC_LINUX_HARDIRQ_H
#include <linux/hardirq.h>	/* for in_interrupt */
#endif
#include <linux/mm.h>]],
	    [[int (*my_autoconf_function_pointer)(ulong,ulong,ulong,pgprot_t) = &remap_page_range;]]) ],
	    [linux_cv_have_remap_page_range_4args='yes'],
	    [linux_cv_have_remap_page_range_4args='no'])
	])
	if test x$linux_cv_have_remap_page_range_4args = xyes ; then
	    AC_DEFINE([HAVE_KFUNC_REMAP_PAGE_RANGE_4ARGS], [1], [Define this for older
		       2.4 kernels where the remap_page_range() function only
		       takes 4 arguments.])
	fi
    ])
    AC_MSG_CHECKING([for DLGN BRI support])
    if test x$enable_dlgn_bri = xyes ; then
	AC_DEFINE([BRI_SUPPORT], [1])
    fi
    AC_MSG_RESULT([$enable_dlgn_bri])
    AC_MSG_CHECKING([for DLGN DCB support])
    if test x$enable_dlgn_dcb = xyes ; then
	AC_DEFINE([DCB], [1])
    fi
    AC_MSG_RESULT([$enable_dlgn_dcb])
    AC_MSG_CHECKING([for DLGN ISA support])
    if test x$enable_dlgn_isa = xyes ; then
	AC_DEFINE([ISA_SPAN], [1])
    fi
    AC_MSG_RESULT([$enable_dlgn_isa])
    AC_DEFINE([MERCD_DATATYPE_INTEL], [1])
    AC_DEFINE([MERCD_LINUX], [1])
    AC_MSG_CHECKING([for DLGN SBA support])
    if test x$enable_dlgn_sba = xno ; then
	AC_DEFINE([NOSBA], [1])
    fi
    AC_MSG_RESULT([$enable_dlgn_sba])
    AC_MSG_CHECKING([for DLGN mutliple SM support])
    if test x$enable_dlgn_msm = xyes ; then
	AC_DEFINE([NOSEARCH], [1])
    fi
    AC_MSG_RESULT([$enable_dlgn_msm])
    AC_MSG_CHECKING([for DLGN ISDN support])
    if test x$enable_dlgn_isdn = xyes ; then
	AC_DEFINE([DLGN_ISDN], [1])
	AH_VERBATIM([__ac_dummy_2], m4_text_wrap([When defined, provide ISDN
		     support for the DLGN driver.  The Intel make files define
		     this.  This is normally defined, but we provide a
		     configuration option to disable it.  This is a bit of a
		     diversion to get around an autoconf macro named _ISDN. */],
		     [   ], [/* ])[
#undef DLGN_ISDN
#ifdef DLGN_ISDN
#define _ISDN 1
#endif])
    fi
    AC_MSG_RESULT([$enable_dlgn_isdn])
    AC_MSG_CHECKING([for DLGN statistics support])
    if test x$enable_dlgn_stats = xyes ; then
	AC_DEFINE([_STATS_], [1])
    fi
    AC_MSG_RESULT([$enable_dlgn_stats])
    AC_MSG_CHECKING([for ANTARES RTDL support])
    if test x$enable_antares_rtdl = xyes ; then
	AC_DEFINE([RTDL_ANT], [1])
    fi
    AC_MSG_RESULT([$enable_antares_rtdl])
    AC_MSG_CHECKING([for MERCD PPS support])
    if test x$enable_mercd_pps = xyes ; then
	AC_DEFINE([PPS], [1])
    fi
    AC_MSG_RESULT([$enable_mercd_pps])
    AC_DEFINE([SE_NOSLP], [1])
    AC_MSG_CHECKING([for MERCD SRAM DMA support])
    if test x$enable_mercd_dma = xyes ; then
	AC_DEFINE([SRAM_DRVR_DMA_SUPPORT], [1])
    fi
    AC_MSG_RESULT([$enable_mercd_dma])
    AC_DEFINE([UNIX], [1])
    AC_MSG_CHECKING([for MERCD Logical ID support])
    if test x$enable_mercd_lid = xyes ; then
	AC_DEFINE([USE_LOGICALID], [1])
    fi
    AC_MSG_RESULT([$enable_mercd_lid])
    _LINUX_CHECK_KERNEL_CONFIG([for PCI bus support], [CONFIG_PCI], [
	AC_DEFINE_UNQUOTED([PCI], [])
	AC_DEFINE([MERCD_PCI], [1])
	AC_DEFINE([PCI_ANT], [1])
	AC_DEFINE([PCI_SPAN], [1])
    ])
    _LINUX_CHECK_KERNEL_CONFIG([for VME bus support], [CONFIG_VME], [
	AC_DEFINE_UNQUOTED([VME], [])
	AC_DEFINE([MERCD_VME], [1])
	AC_DEFINE([VME_ANT], [1])
	AC_DEFINE([VME_SPAN], [1])
    ])
    AC_DEFINE([__DRVDEBUG_H__], [1])
    AC_DEFINE([PKGVER], ["Version 6.1"])
    AC_DEFINE([OSTYPEVER], ["Linux"])
    AC_DEFINE([COPYRIGHT], ["Copyright (C) 2005 Intel Corporation"])
    AC_DEFINE([PMACD_VERSION], [4.0.0])
    AC_DEFINE([_LIS_SOURCE], [1], [Define because most of this source was LiS
	       source beforehand.])
    AH_VERBATIM([__ac_dummy_1], m4_text_wrap([Some of the drivers need LINUX24
		 defined when they are being build for a 2.4 Linx kernel.  We
		 already have LINUX_2_4 defined in this case, so just
		 bootstrap off of that */], [   ], [/* ])[
#undef LINUX_2_4
#ifdef LINUX_2_4
#define LINUX24  1
#endif])
    AC_C_BIGENDIAN([
	AC_DEFINE([BIG_ANT], [1])
	AC_DEFINE([BIG_ENDIAN], [1])
    ])
])# _OSR61_CHECKS
# =============================================================================

# =============================================================================
# _OSR61_OUTPUT
# -----------------------------------------------------------------------------
AC_DEFUN([_OSR61_OUTPUT], [dnl
    _OSR61_STRCONF
])# _OSR61_OUTPUT
# =============================================================================

# =============================================================================
# _OSR61_STRCONF
# -----------------------------------------------------------------------------
AC_DEFUN([_OSR61_STRCONF], [dnl
    strconf_cv_stem='Config'
    strconf_cv_input='Config.master'
    strconf_cv_majbase=252
    strconf_cv_midbase=30
    if test ${streams_cv_package:-LfS} = LfS ; then
	if test ${linux_cv_minorbits:-8} -gt 8 ; then
dnl
dnl Tired of device conflicts on 2.6 kernels.
dnl
	    ((strconf_cv_majbase+=2000))
	fi
dnl
dnl Get these away from device numbers.
dnl
	((strconf_cv_midbase+=5000))
    fi
    strconf_cv_config='strconf.h'
    strconf_cv_modconf='modconf.h'
    strconf_cv_drvconf='drvconf.mk'
    strconf_cv_confmod='conf.modules'
    strconf_cv_makedev='devices.lst'
    strconf_cv_mknodes="${PACKAGE_TARNAME}_mknod.c"
    strconf_cv_strsetup='strsetup.conf'
    strconf_cv_strload='strload.conf'
    AC_REQUIRE([_LINUX_STREAMS])
    strconf_cv_package=${streams_cv_package:-LiS}
    strconf_cv_minorbits="${linux_cv_minorbits:-8}"
    _STRCONF
])# _OSR61_STRCONF
# =============================================================================

# =============================================================================
# _OSR61_
# -----------------------------------------------------------------------------
AC_DEFUN([_OSR61_], [dnl
])# _OSR61_
# =============================================================================

# =============================================================================
# 
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# 
# =============================================================================
# ENDING OF SEPARATE COPYRIGHT MATERIAL
# =============================================================================
# vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
