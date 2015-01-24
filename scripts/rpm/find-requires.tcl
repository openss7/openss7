#!/usr/bin/awk -f

# feed tcl script filenames in on standard input,
# required tcl modules on standard output

BEGIN {
    print "D: Executing: " ARGV[0] " -f find-requires.tcl" > "/dev/stderr"
    RPM_BUILD_DIR = ENVIRON[RPM_BUILD_DIR]
    RPM_PACKAGE_NAME = ENVIRON[RPM_PACKAGE_NAME]
    RPM_PACKAGE_VERSION = ENVIRON[RPM_PACKAGE_VERSION]
    RPM_PACKAGE_RELEASE = ENVIRON[RPM_PACKAGE_RELEASE]
    dir = RPM_BUILD_DIR "/" RPM_PACKAGE_NAME "-" RPM_PACKAGE_VERSION
    if (ARGC > 1 && ARGV[1]) {
	dir = ARGV[1]
    }
    RPM_BUILD_ROOT = ENVIRON[RPM_BUILD_ROOT]
    if (ARGC > 2 && ARGV[2]) {
	RPM_BUILD_ROOT = ARGV[2]
    }
    sub(/\/+$/, "", RPM_BUILD_ROOT)
    for (i=1;ARGC>i;i++) { delete ARGV[i] }
}
{
    file = $0
    while ((getline < file) == 1) {
	if (/^[[:space:]]*\$RPM_Requires[[:space:]]*=[[:space:]]*["'].*['"]/) {
	    sub(/^[[:space:]]*\$RPM_Requires[[:space:]]*=[[:space:]]*["']/, "")
	    sub(/['"].*$/, "")
	    for (i = 1; i <= NF; i++)
		requires[$i] = 1
	    continue
	}
	sub(/#.*/, "");
	if (/^[[:space:]]*\$RPM_Requires[[:space:]]*=[[:space:]]*["'].*['"]/) {
	    sub(/^[[:space:]]*\$RPM_Requires[[:space:]]*=[[:space:]]*["']/, "")
	    sub(/['"].*$/, "")
	    for (i = 1; i <= NF; i++)
		requires[$i] = 1
	    continue
	}
	if (/source/) {
	    if (/source[[:space:]]+['"]?[0-9A-Za-z\/\.\_\-\\\(\)\$]+/) {
		sub(/.*source[[:space:]]+['"]?/, "")
		sub(/[^0-9A-Za-z\/\.\_\-\\\(\)\$].*/, "")
		if (/\$/)
		    sub(/.*\//, "")
		if ($0 !~ /\$/)
		    requires["tcl(" $0 ")"] = 1
	    }
	}
    }
    close(file)
}
END {
    for (r in requires) {
	print r
    }
}

# =============================================================================
#
# @(#) scripts/find-requires.tcl
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License along
# with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
# the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# =============================================================================
# vim: ft=awk tw=80 sw=4 nocin nosi fo+=tcqlorn
