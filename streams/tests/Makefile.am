# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.33 $) $Date: 2007/12/15 20:20:22 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
# more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>, or
# write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
# 02139, USA.
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
# Last Modified $Date: 2007/12/15 20:20:22 $ by $Author: brian $
#
# =============================================================================

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
			$(TESTSUITE)-streams.at \
			$(TESTSUITE)-admin.at \
			$(TESTSUITE).at

if CONFIG_STREAMS_CLONE
TESTSUITE_DEPS		+= $(TESTSUITE)-clone.at
endif
if CONFIG_STREAMS_CLONE_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-clone.at
endif
if CONFIG_STREAMS_CONNLD
TESTSUITE_DEPS		+= $(TESTSUITE)-connld.at
endif
if CONFIG_STREAMS_CONNLD_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-connld.at
endif
if CONFIG_STREAMS_ECHO
TESTSUITE_DEPS		+= $(TESTSUITE)-echo.at
endif
if CONFIG_STREAMS_ECHO_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-echo.at
endif
if CONFIG_STREAMS_FIFO
TESTSUITE_DEPS		+= $(TESTSUITE)-fifo.at
endif
if CONFIG_STREAMS_FIFO_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-fifo.at
endif
if CONFIG_STREAMS_LOG
TESTSUITE_DEPS		+= $(TESTSUITE)-log.at
endif
if CONFIG_STREAMS_LOG_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-log.at
endif
if CONFIG_STREAMS_LOOP
TESTSUITE_DEPS		+= $(TESTSUITE)-loop.at
endif
if CONFIG_STREAMS_LOOP_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-loop.at
endif
if CONFIG_STREAMS_NSDEV
TESTSUITE_DEPS		+= $(TESTSUITE)-nsdev.at
endif
if CONFIG_STREAMS_NSDEV_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-nsdev.at
endif
if CONFIG_STREAMS_MUX
TESTSUITE_DEPS		+= $(TESTSUITE)-mux.at
endif
if CONFIG_STREAMS_MUX_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-mux.at
endif
if CONFIG_STREAMS_NULS
TESTSUITE_DEPS		+= $(TESTSUITE)-nuls.at
endif
if CONFIG_STREAMS_NULS_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-nuls.at
endif
if CONFIG_STREAMS_PIPE
TESTSUITE_DEPS		+= $(TESTSUITE)-pipe.at
endif
if CONFIG_STREAMS_PIPE_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-pipe.at
endif
if CONFIG_STREAMS_PIPEMOD
TESTSUITE_DEPS		+= $(TESTSUITE)-pipemod.at
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-pipemod.at
endif
if CONFIG_STREAMS_SAD
TESTSUITE_DEPS		+= $(TESTSUITE)-sad.at
endif
if CONFIG_STREAMS_SAD_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-sad.at
endif
if CONFIG_STREAMS_SC
TESTSUITE_DEPS		+= $(TESTSUITE)-sc.at
endif
if CONFIG_STREAMS_SC_MODULE
TESTSUITE_DEPS		+= $(TESTSUITE)-sc.at
endif

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.33  2007/12/15 20:20:22  brian
# - updates
#
# Revision 0.9.2.32  2007/08/13 22:46:42  brian
# - GPLv3 header updates
#
# Revision 0.9.2.31  2006/03/10 14:05:22  brian
# - corrected kishore's problem with missing testsuite-admin.at
#
# Revision 0.9.2.30  2006/03/06 11:38:39  brian
# - updated headers
#
# Revision 0.9.2.29  2005/12/20 04:49:39  brian
# - added autotest.am Makefile fragment
#
# Revision 0.9.2.28  2005/12/14 11:32:06  brian
# - added way to pass options to testsuite from make installcheck
#
# Revision 0.9.2.27  2005/10/11 10:45:53  brian
# - STREAMS-based pipes are working on RH 7.2 and FC4
# - starting test suites for STREAMS-based pipes
# - completed testing of STREAMS-based FIFOs
# - updated documentation for FIFOs
#
# Revision 0.9.2.26  2005/09/09 20:27:26  brian
# - added mux driver
#
# Revision 0.9.2.25  2005/09/08 05:52:45  brian
# - added nullmod module and loop driver
# - corrections during testing
# - many ioctl(2p) test cases work very well now
#
# Revision 0.9.2.24  2005/07/21 20:47:25  brian
# - sync with notebook
#
# Revision 0.9.2.20  2005/07/21 20:47:25  brian
# - sync with notebook
#
# Revision 0.9.2.19  2005/06/15 00:46:05  brian
# - corrected a couple targets
#
# Revision 0.9.2.18  2005/05/30 14:25:55  brian
# - packaged standalong test suites
#
# Revision 0.9.2.17  2005/05/21 07:39:52  brian
# - new testsuite organization
#
# Revision 0.9.2.16  2005/05/20 04:44:58  brian
# - updated autotest makefiles
#
# Revision 0.9.2.15  2005/05/16 11:25:08  brian
# - minor rearrangement
#
# Revision 0.9.2.14  2005/05/15 23:57:20  brian
# - update tests
#
# Revision 0.9.2.13  2005/05/14 23:59:23  brian
# - getting autotest up and running
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
