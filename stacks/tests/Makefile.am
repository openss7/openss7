## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2007/08/13 15:22:14 $
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
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007/08/13 15:22:14 $ by $Author: brian $
#
# =============================================================================

include ../am/openss7.am

include ../am/autotest.am

TESTSUITE_DEPS		+= \
			local.at \
			kernel.at \
			$(TESTSUITE).at \
			$(TESTSUITE)-m2pa.at \
			$(TESTSUITE)-x400p-sl.at

if WITH_TLI
## WITH_TLI
TESTSUITE_DEPS		+= \
			$(TESTSUITE)-timod.at \
			$(TESTSUITE)-tirdwr.at
## WITH_TLI
endif

if WITH_XNET
## WITH_XNET
TESTSUITE_DEPS		+= \
			$(TESTSUITE)-xnet.at \
			$(TESTSUITE)-xnet_thr.at
## WITH_XNET
endif

if WITH_INET
## WITH_INET
TESTSUITE_DEPS		+= \
			$(TESTUITE)-inet_udp.at \
			$(TESTUITE)-inet_raw.at \
			$(TESTUITE)-inet_tcp.at \
			$(TESTUITE)-inet_udp2.at \
			$(TESTUITE)-inet_raw2.at
if WITH_OPENSS7_SCTP
## WITH_OPENSS7_SCTP
TESTSUITE_DEPS		+= \
			$(TESTSUITE)-inet_sctp.at
## WITH_OPENSS7_SCTP
endif
## WITH_INET
endif

if WITH_SCTP
## WITH_SCTP
TESTSUITE_DEPS		+= \
			$(TESTSUITE)-sctp_t.at \
			$(TESTSUITE)-sctp_n.at \
			$(TESTSUITE)-sctp_n2.at
## WITH_SCTP
endif

if WITH_SCTP2
## WITH_SCTP2
TESTSUITE_DEPS		+= \
			$(TESTSUITE)-sctp_t.at \
			$(TESTSUITE)-sctp_n.at \
			$(TESTSUITE)-sctp_n2.at
## WITH_SCTP2
endif

if WITH_SIGTRAN
## WITH_SIGTRAN
TESTSUITE_DEPS		+= \
			$(TESTSUITE)-m2pa.at
## WITH_SIGTRAN
endif

## =============================================================================
##
## $Log: Makefile.am,v $
## Revision 0.9.2.17  2007/08/13 15:22:14  brian
## - header updates, added references
##
## Revision 0.9.2.16  2006/12/18 10:51:45  brian
## - subpackaging changes for release
##
## Revision 0.9.2.15  2006/12/06 11:45:37  brian
## - updated X400P driver and test suites
##
## Revision 0.9.2.14  2006/09/26 00:52:44  brian
## - rationalized to embedded packages
##
## Revision 0.9.2.13  2006/09/25 22:21:59  brian
## - added testsuites for optional components
##
## Revision 0.9.2.12  2006/03/06 11:38:26  brian
## - updated headers
##
## Revision 0.9.2.11  2005/12/20 15:11:50  brian
## - result of SMP kernel testing for LiS
##
## Revision 0.9.2.10  2005/12/20 04:49:22  brian
## - added autotest.am Makefile fragment
##
## Revision 0.9.2.9  2005/12/14 11:32:06  brian
## - added way to pass options to testsuite from make installcheck
##
## Revision 0.9.2.8  2005/06/15 00:46:04  brian
## - corrected a couple targets
##
## Revision 0.9.2.7  2005/05/30 14:25:55  brian
## - packaged standalong test suites
##
## Revision 0.9.2.6  2005/05/21 07:39:52  brian
## - new testsuite organization
##
## Revision 0.9.2.5  2005/05/20 04:44:58  brian
## - updated autotest makefiles
##
## =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
