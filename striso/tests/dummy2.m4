dnl vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
dnl ==========================================================================
dnl 
dnl @(#) $RCSfile: dummy2.m4,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/04/07 22:01:34 $
dnl
dnl --------------------------------------------------------------------------
dnl
dnl Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
dnl Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
dnl
dnl All Rights Reserved.
dnl
dnl Unauthorized distribution or duplication is prohibited.
dnl
dnl This software and related documentation is protected by copyright and
dnl distributed under licenses restricting its use, copying, distribution
dnl and decompilation.  No part of this software or related documentation
dnl may be reproduced in any form by any means without the prior written
dnl authorization of the copyright holder, and licensors, if any.
dnl
dnl The recipient of this document, by its retention and use, warrants that
dnl the recipient will protect this information and keep it confidential,
dnl and will not disclose the information contained in this document
dnl without the written permission of its owner.
dnl
dnl The author reserves the right to revise this software and documentation
dnl for any reason, including but not limited to, conformity with standards
dnl promulgated by various agencies, utilization of advances in the state
dnl of the technical arts, or the reflection of changes in the design of any
dnl techniques, or procedures embodied, described, or referred to herein.
dnl The author is under no obligation to provide any feature listed herein.
dnl
dnl --------------------------------------------------------------------------
dnl
dnl As an exception to the above, this software may be distributed under the
dnl GNU General Public License (GPL) Version 2, so long as the software is
dnl distributed with, and only used for the testing of, OpenSS7 modules,
dnl drivers, and libraries.
dnl
dnl --------------------------------------------------------------------------
dnl
dnl U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
dnl behalf of the U.S. Government ("Government"), the following provisions
dnl apply to you.  If the Software is supplied by the Department of Defense
dnl ("DoD"), it is classified as "Commercial Computer Software" under
dnl paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
dnl Regulations ("DFARS") (or any successor regulations) and the Government is
dnl acquiring only the license rights granted herein (the license rights
dnl customarily provided to non-Government users).  If the Software is
dnl supplied to any unit or agency of the Government other than DoD, it is
dnl classified as "Restricted Computer Software" and the Government's rights
dnl in the Software are defined in paragraph 52.227-19 of the Federal
dnl Acquisition Regulations ("FAR") (or any successor regulations) or, in the
dnl cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
dnl (or any successor regulations).
dnl
dnl --------------------------------------------------------------------------
dnl
dnl Commercial licensing and support of this software is available from OpenSS7
dnl Corporation at a fee.  See http://www.openss7.com/
dnl
dnl --------------------------------------------------------------------------
dnl
dnl Last Modified $Date: 2006/04/07 22:01:34 $ by $Author: brian $
dnl
dnl ==========================================================================

AT_SETUP([dummy test2])
AT_KEYWORDS([dummy])
AT_CHECK([exit 1], [], [ignore])
AT_XFAIL_IF(true)
echo "This is another dummy test"
exit 1
AT_CLEANUP

dnl ==========================================================================
dnl $Log: dummy2.m4,v $
dnl Revision 0.9.2.1  2006/04/07 22:01:34  brian
dnl - initial branch
dnl
dnl Revision 0.9  2006/04/07 21:56:45  brian
dnl - initial addition of striso files
dnl
dnl
dnl ==========================================================================
dnl vim: ft=config sw=4 noet nocin nosi com=b\:#,b\:dnl,b\:***,b\:@%\:@ fo+=tcqlorn
