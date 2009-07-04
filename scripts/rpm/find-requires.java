#!/bin/bash

args=("$@")
script=$(basename $0)
where=`(cd $(dirname $0); pwd)`

which_tool() {
    old_PATH="$PATH"
    PATH="$PATH:$where:/usr/lib/rpm:/usr/lib/rpm/redhat"
    which $1
    PATH="$old_PATH"
}

save_IFS="$IFS"
IFS='
'
jarlist=($(cat))
IFS="$save_IFS"

tool=$(which_tool "javadeps")

[ -n "$tool" -a -x "$tool" ] || { cat >/dev/null ; exit 0; }

all_provides() {
    for jar in "$@" ; do
	unzip -p "$jar" | $tool --provides --rpmformat --keywords --starprov -- - | sort -b -u
    done | sort -b -u
}

all_requires() {
    for jar in "$@" ; do
	unzip -p "$jar" | $tool --requires --rpmformat --keywords -- - | sort -b -u
    done | sort -b -u
}

# filter what we provide from what we require
(join -v 2 \
    <(all_provides "${jarlist[@]}") \
    <(all_requires "${jarlist[@]}"))

# =============================================================================
#
# @(#) $RCSfile: find-requires.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-07-04 03:51:40 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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
# -----------------------------------------------------------------------------
#
# Last Modified $Date: 2009-07-04 03:51:40 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: find-requires.java,v $
# Revision 1.1.2.1  2009-07-04 03:51:40  brian
# - updates for release
#
# =============================================================================
# vim: ft=sh sw=4 tw=80 nocin nosi fo+=tcqlorn
