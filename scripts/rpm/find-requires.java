#!/bin/bash

echo "D: Executing: $0" >&2

args=("$@")
script=$(basename $0)
where=`(cd $(dirname $0); pwd)`

which_tool() {
    old_PATH="$PATH"
    PATH="$PATH:$where:/usr/lib/rpm/redhat:/usr/lib/rpm"
    which $1
    PATH="$old_PATH"
}

tool=$(which_tool "javadeps")

[ -n "$tool" -a -x "$tool" ] || { cat >/dev/null ; exit 0; }

awk '
    /\.(jar|war|zip)$/ {
	file = $1
	prov = "unzip -p " file " | " tool " --provides --rpmformat --keywords --starprov -- - 2>/dev/null"
	while ((prov | getline line)) {
	    provides[line] = 1
	}
	close(prov)
	reqd = "unzip -p " file " | " tool " --requires --rpmformat --keywords -- - 2>/dev/null"
	while ((reqd | getline line)) {
	    requires[line] = 1
	}
	close(reqd)
    }
    END {
	j = 1
	for (r in requires) {
	    if (provides[r])
		continue
	    indices[j] = r
	    j++
	}
	n = asort(indices)
	for (i = 1; i <= n; i++)
	    print indices[i]
    }
' tool="$tool"

exit 0

# =============================================================================
#
# @(#) scripts/find-requires.java
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
# vim: ft=sh sw=4 tw=80 nocin nosi fo+=tcqlorn
