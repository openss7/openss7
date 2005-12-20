# vim: ft=automake
# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/12/20 15:12:22 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# Unauthorized distribution or duplication is prohibited.
#
# This software and related documentation is protected by copyright and
# distributed under licenses restricting its use, copying, distribution and
# decompilation.  No part of this software or related documentation may be
# reproduced in any form by any means without the prior written
# authorization of the copyright holder, and licensors, if any.
#
# The recipient of this document, by its retention and use, warrants that
# the recipient will protect this information and keep it confidential, and
# will not disclose the information contained in this document without the
# written permission of its owner.
#
# The author reserves the right to revise this software and documentation
# for any reason, including but not limited to, conformity with standards
# promulgated by various agencies, utilization of advances in the state of
# the technical arts, or the reflection of changes in the design of any
# techniques, or procedures embodied, described, or referred to herein.
# The author is under no obligation to provide any feature listed herein.
#
# -----------------------------------------------------------------------------
#
# As an exception to the above, this software may be distributed under the
# GNU General Public License (GPL) Version 2 or later, so long as the
# software is distributed with, and only used for the testing of, OpenSS7
# modules, drivers, and libraries.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"),
# it is classified as "Commercial Computer Software" under paragraph
# 252.227-7014 of the DoD Supplement to the Federal Acquisition Regulations
# ("DFARS") (or any successor regulations) and the Government is acquiring
# only the license rights granted herein (the license rights customarily
# provided to non-Government users).  If the Software is supplied to any unit
# or agency of the Government other than DoD, it is classified as "Restricted
# Computer Software" and the Government's rights in the Software are defined
# in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR") (or
# any successor regulations) or, in the cases of NASA, in paragraph 18.52.227-86
# of the NASA Supplement to the FAR (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2005/12/20 15:12:22 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.12  2005/12/20 15:12:22  brian
# - result of SMP kernel testing for LiS
#
# Revision 0.9.2.11  2005/12/20 04:49:54  brian
# - added autotest.am Makefile fragment
#
# Revision 0.9.2.10  2005/12/14 11:32:07  brian
# - added way to pass options to testsuite from make installcheck
#
# Revision 0.9.2.9  2005/06/16 04:34:05  brian
# - modifications to test-sctp_t
#
# Revision 0.9.2.8  2005/06/15 00:46:05  brian
# - corrected a couple targets
#
# Revision 0.9.2.7  2005/05/30 14:25:56  brian
# - packaged standalong test suites
#
# Revision 0.9.2.6  2005/05/21 07:39:53  brian
# - new testsuite organization
#
# Revision 0.9.2.5  2005/05/20 04:44:59  brian
# - updated autotest makefiles
#
# =============================================================================

include ../am/openss7.am

include ../am/autotest.am

TESTSUITE_DEPS		+= \
			local.at \
			kernel.at \
			$(TESTSUITE)-sctp_t.at \
			$(TESTSUITE).at

# vim: ft=automake
