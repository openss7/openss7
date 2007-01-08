## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/01/08 16:24:35 $
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
## Last Modified $Date: 2007/01/08 16:24:35 $ by $Author: brian $
##
## -----------------------------------------------------------------------------
##
## $Log: Makefile.am,v $
## Revision 0.9.2.4  2007/01/08 16:24:35  brian
## - final updates for release
##
## Revision 0.9.2.3  2006/12/28 05:23:33  brian
## - corrections for make check target
##
## Revision 0.9.2.2  2006/12/23 13:07:27  brian
## - manual page and other package updates for release
##
## Revision 0.9.2.1  2006/10/16 10:54:58  brian
## - added new package files
##
## =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

MANS_UNUSED = \
	man4/bicc.4 \
	man4/bicc_ioctl.4 \
	man4/dl_h225.4 \
	man4/h225.4 \
	man4/h225_ioctl.4 \
	man4/h245.4 \
	man4/h248.4 \
	man4/h248_ioctl.4 \
	man4/mgc.4 \
	man4/rtp.4 \
	man4/sipt.4 \
	man4/sipt_ioctl.4 \
	man4/VOIP.4 \
	man7/bicci.7 \
	man7/cci_bicc.7 \
	man7/cci_h225.7 \
	man7/cci_sipt.7 \
	man7/h225i.7 \
	man7/mgi_h248.7 \
	man7/mgi_mgcp.7 \
	man8/bicc.8 \
	man8/h225.8 \
	man8/h323confd.8 \
	man8/strvoip_mknod.8 \
	man8/test-voip.8 \
	man8/voipconfd.8

man_MANS = \
	man4/bicc.4 \
	man4/bicc_ioctl.4 \
	man4/dl_h225.4 \
	man4/h225.4 \
	man4/h225_ioctl.4 \
	man4/h245.4 \
	man4/h248.4 \
	man4/h248_ioctl.4 \
	man4/mgc.4 \
	man4/rtp.4 \
	man4/sipt.4 \
	man4/sipt_ioctl.4 \
	man4/VOIP.4 \
	man5/strvoip.5 \
	man7/bicci.7 \
	man7/cci_bicc.7 \
	man7/cci_h225.7 \
	man7/cci_sipt.7 \
	man7/h225i.7 \
	man7/mgi_h248.7 \
	man7/mgi_mgcp.7 \
	man8/bicc.8 \
	man8/h225.8 \
	man8/h323confd.8 \
	man8/strvoip_mknod.8 \
	man8/test-voip.8 \
	man8/voipconfd.8


dist_noinst_MANS = \
	man4/bicc.4.man \
	man4/bicc_ioctl.4.man \
	man4/dl_h225.4.man \
	man4/h225.4.man \
	man4/h225_ioctl.4.man \
	man4/h245.4.man \
	man4/h248.4.man \
	man4/h248_ioctl.4.man \
	man4/mgc.4.man \
	man4/rtp.4.man \
	man4/sipt.4.man \
	man4/sipt_ioctl.4.man \
	man4/VOIP.4.man \
	man5/strvoip.5.man \
	man7/bicci.7.man \
	man7/cci_bicc.7.man \
	man7/cci_h225.7.man \
	man7/cci_sipt.7.man \
	man7/h225i.7.man \
	man7/mgi_h248.7.man \
	man7/mgi_mgcp.7.man \
	man8/bicc.8.man \
	man8/h225.8.man \
	man8/h323confd.8.man \
	man8/strvoip_mknod.8.man \
	man8/test-voip.8.man \
	man8/voipconfd.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
