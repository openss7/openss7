## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2007/01/23 10:00:40 $
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free Software
## Foundation; version 2 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
## details.
##
## You should have received a copy of the GNU General Public License along with
## this program; if not, write to the Free Software Foundation, Inc., 675 Mass
## Ave, Cambridge, MA 02139, USA.
##
## -----------------------------------------------------------------------------
##
## U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
## behalf of the U.S. Government ("Government"), the following provisions apply
## to you.  If the Software is supplied by the Department of Defense ("DoD"), it
## is classified as "Commercial Computer Software" under paragraph 252.227-7014
## of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
## successor regulations) and the Government is acquiring only the license rights
## granted herein (the license rights customarily provided to non-Government
## users).  If the Software is supplied to any unit or agency of the Government
## other than DoD, it is classified as "Restricted Computer Software" and the
## Government's rights in the Software are defined in paragraph 52.227-19 of the
## Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
## the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
## (or any successor regulations).
##
## -----------------------------------------------------------------------------
##
## Commercial licensing and support of this software is available from OpenSS7
## Corporation at a fee.  See http://www.openss7.com/
##
## -----------------------------------------------------------------------------
##
## Last Modified $Date: 2007/01/23 10:00:40 $ by $Author: brian $
##
## -----------------------------------------------------------------------------
##
## $Log: Makefile.am,v $
## Revision 0.9.2.5  2007/01/23 10:00:40  brian
## - added test program and m2ua-as updates
##
## Revision 0.9.2.4  2007/01/08 16:23:02  brian
## - final updates for release
##
## Revision 0.9.2.3  2006/12/23 13:06:47  brian
## - manual page and other package updates for release
##
## Revision 0.9.2.2  2006/12/21 11:00:55  brian
## - updated manual pages for release
##
## Revision 0.9.2.1  2006/10/16 10:42:44  brian
## - added new package files
##
## =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

MANS_UNUSED = \
	man4/bua.4 \
	man4/bua_ioctl.4 \
	man4/dl_dua.4 \
	man4/dl_gr303ua.4 \
	man4/dl_iua.4 \
	man4/dl_v5ua.4 \
	man4/dua.4 \
	man4/dua_ioctl.4 \
	man4/gr303ua.4 \
	man4/gr303ua_ioctl.4 \
	man4/isua.4 \
	man4/isua_ioctl.4 \
	man4/iua.4 \
	man4/iua_ioctl.4 \
	man4/m2ua.4 \
	man4/m2ua_as.4 \
	man4/m2ua-as.4 \
	man4/m2ua_ioctl.4 \
	man4/m3pa.4 \
	man4/m3tp.4 \
	man4/m3ua.4 \
	man4/m3ua_as.4 \
	man4/m3ua_ioctl.4 \
	man4/SIGTRAN.4 \
	man4/sua.4 \
	man4/sua-gtt.4 \
	man4/sua_ioctl.4 \
	man4/sua-mgmt.4 \
	man4/sua-npi.4 \
	man4/sua-sccpi.4 \
	man4/sua-tpi.4 \
	man4/tali.4 \
	man4/tali_ioctl.4 \
	man4/tali-isup.4 \
	man4/tali-mtp.4 \
	man4/tali-sccp.4 \
	man4/tua.4 \
	man4/tua_ioctl.4 \
	man4/ua_ioctl.4 \
	man4/v5ua.4 \
	man4/v5ua_ioctl.4 \
	man8/bua.8 \
	man8/isua.8 \
	man8/iua.8 \
	man8/m2pa.8 \
	man8/m2tp.8 \
	man8/m2ua.8 \
	man8/m3tp.8 \
	man8/m3ua.8 \
	man8/sigtran.8 \
	man8/sua.8 \
	man8/tua.8 \
	man8/uaconfd.8

man_MANS = \
	$(MANS_UNUSED) \
	man4/m2pa.4 \
	man4/m2pa_sl.4 \
	man4/m2tp.4 \
	man4/sdl_sctp.4 \
	man4/sdt_sctp.4 \
	man4/sdt_tpi.4 \
	man4/sl_sctp.4 \
	man4/sl_tpi.4 \
	man5/sigtran.5 \
	man7/dlpi_dua.7 \
	man7/dlpi_gr303ua.7 \
	man7/dlpi_iua.7 \
	man7/dlpli_gr303ua.7 \
	man8/sigtran_mknod.8 \
	man8/test-m2pa.8 \
	man8/test-m2ua_as.8 \
	man8/test-sigtran.8

dist_noinst_MANS = \
	man4/bua.4.man \
	man4/bua_ioctl.4.man \
	man4/dl_dua.4.man \
	man4/dl_gr303ua.4.man \
	man4/dl_iua.4.man \
	man4/dl_v5ua.4.man \
	man4/dua.4.man \
	man4/dua_ioctl.4.man \
	man4/gr303ua.4.man \
	man4/gr303ua_ioctl.4.man \
	man4/isua.4.man \
	man4/isua_ioctl.4.man \
	man4/iua.4.man \
	man4/iua_ioctl.4.man \
	man4/m2pa.4.man \
	man4/m2pa_sl.4.man \
	man4/m2tp.4.man \
	man4/m2ua.4.man \
	man4/m2ua_as.4.man \
	man4/m2ua-as.4.man \
	man4/m2ua_ioctl.4.man \
	man4/m3pa.4.man \
	man4/m3tp.4.man \
	man4/m3ua.4.man \
	man4/m3ua_as.4.man \
	man4/m3ua_ioctl.4.man \
	man4/sdl_sctp.4.man \
	man4/sdt_sctp.4.man \
	man4/sdt_tpi.4.man \
	man4/SIGTRAN.4.man \
	man4/sl_sctp.4.man \
	man4/sl_tpi.4.man \
	man4/sua.4.man \
	man4/sua-gtt.4.man \
	man4/sua_ioctl.4.man \
	man4/sua-mgmt.4.man \
	man4/sua-npi.4.man \
	man4/sua-sccpi.4.man \
	man4/sua-tpi.4.man \
	man4/tali.4.man \
	man4/tali_ioctl.4.man \
	man4/tali-isup.4.man \
	man4/tali-mtp.4.man \
	man4/tali-sccp.4.man \
	man4/tua.4.man \
	man4/tua_ioctl.4.man \
	man4/ua_ioctl.4.man \
	man4/v5ua.4.man \
	man4/v5ua_ioctl.4.man \
	man5/sigtran.5.man \
	man7/dlpi_dua.7.man \
	man7/dlpi_gr303ua.7.man \
	man7/dlpi_iua.7.man \
	man7/dlpli_gr303ua.7.man \
	man8/bua.8.man \
	man8/isua.8.man \
	man8/iua.8.man \
	man8/m2pa.8.man \
	man8/m2tp.8.man \
	man8/m2ua.8.man \
	man8/m3tp.8.man \
	man8/m3ua.8.man \
	man8/sigtran.8.man \
	man8/sigtran_mknod.8.man \
	man8/sua.8.man \
	man8/test-m2pa.8.man \
	man8/test-m2ua_as.8.man \
	man8/test-sigtran.8.man \
	man8/tua.8.man \
	man8/uaconfd.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
