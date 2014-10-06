## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
## =============================================================================
## 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:49:03 $
##
## -----------------------------------------------------------------------------
##
## Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
## Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
## Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
##
## All Rights Reserved.
##
## This program is free software; you can redistribute it and/or modify it under
## the terms of the GNU Affero General Public License as published by the Free
## Software Foundation; version 3 of the License.
##
## This program is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
## FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
## details.
##
## You should have received a copy of the GNU Affero General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>, or
## write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
## 02139, USA.
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
## Last Modified $Date: 2009-06-21 11:49:03 $ by $Author: brian $
##
## =============================================================================

include ../am/openss7.am

#
# This automake file is used to specify the autotest definitions and targets
# necessary for performing pre-install and post-install checks.  Pre-install
# checks will work for libraries and application programs, however, it is
# necessary to insert kernel modules into the running kernel to perform most
# pre-install checks.
#

include ../am/autotest.am

TESTSUITE_DEPS		+= \
			local.at \
			kernel.at \
			$(TESTSUITE)-admin.at \
			$(TESTSUITE)-clone.at \
			$(TESTSUITE)-connld.at \
			$(TESTSUITE)-echo.at \
			$(TESTSUITE)-fifo.at \
			$(TESTSUITE)-inet_raw2.at \
			$(TESTSUITE)-inet_raw.at \
			$(TESTSUITE)-inet_sctp.at \
			$(TESTSUITE)-inet_tcp.at \
			$(TESTSUITE)-inet_udp2.at \
			$(TESTSUITE)-inet_udp.at \
			$(TESTSUITE)-ip.at \
			$(TESTSUITE)-log.at \
			$(TESTSUITE)-loop.at \
			$(TESTSUITE)-m2pa.at \
			$(TESTSUITE)-m2ua_as.at \
			$(TESTSUITE)-m3ua-raw.at \
			$(TESTSUITE)-mux.at \
			$(TESTSUITE)-np_ip.at \
			$(TESTSUITE)-nsdev.at \
			$(TESTSUITE)-nuls.at \
			$(TESTSUITE)-pipe.at \
			$(TESTSUITE)-pipemod.at \
			$(TESTSUITE)-sad.at \
			$(TESTSUITE)-sc.at \
			$(TESTSUITE)-sctp.at \
			$(TESTSUITE)-sctp_n2.at \
			$(TESTSUITE)-sctp_n.at \
			$(TESTSUITE)-sctp_t.at \
			$(TESTSUITE)-streams.at \
			$(TESTSUITE)-timod.at \
			$(TESTSUITE)-tirdwr.at \
			$(TESTSUITE)-usage.at \
			$(TESTSUITE)-x400p-sl.at \
			$(TESTSUITE)-xnet.at \
			$(TESTSUITE)-xnet_thr.at \
			$(TESTSUITE)-xnsl.at \
			$(TESTSUITE)-xnsl_thr.at \
			$(TESTSUITE).at

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 1.1.2.1  2009-06-21 11:49:03  brian
## - added files to new distro
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
