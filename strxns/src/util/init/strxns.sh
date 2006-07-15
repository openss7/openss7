#!/bin/sh
#
# @(#) $RCSfile: strxns.sh,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/07/15 13:06:31 $
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
# All Rights Reserved.
#
# Distributed by OpenSS7 Corporation.  See the bottom of this script for copying
# permissions.
#
# These are arguments to update-rc.d ala chkconfig and lsb.  They are recognized
# by openss7 install_initd and remove_initd scripts.  Each line specifies
# arguments to add and remove links after the the name argument:
#
# streams:	start and stop strxns subsystem
# update-rc.d:	start 33 S . stop 33 0 6 .
# config:	/etc/default/streams
# probe:	false
# hide:		false
# license:	GPL
# description:	This STREAMS init script is part of Linux Fast-STREAMS.  \
#		It is responsible for ensuring that the necessary STREAMS \
#		character devices are present in the /dev directory and \
#		that the STREAMS XNS subsystem is configured and loaded.
#

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
name='strxns'
config="/etc/default/$name"
desc="the STREAMS XNS subsystem"

[ -e /proc/modules ] || exit 0

for STRXNS_MKNOD in /sbin/${name}_mknod /usr/sbin/${name}_mknod /bin/${name}_mknod /usr/bin/${name}_mknod ; do
    if [ -x $STRXNS_MKNOD ] ; then
	break
    else
	STRXNS_MKNOD=
    fi
done

# Specify defaults

STRXNS_MODULES="streams-ip_strm_mod streams-ip_to_dlpi streams-ldl streams-np_ip"
STRXNS_MAKEDEVICES="no"
STRXNS_REMOVEDEVICES="no"

# Source config file
for file in $config ; do
    [ -f $file ] && . $file
done

[ -z "$STRXNS_MKNOD" ] && STRXNS_MAKEDEVICES='no'
[ -z "$STRXNS_MKNOD" ] && STRXNS_REMOVEDEVICES='no'

RETVAL=0

umask 077

if [ "${VERBOSE:-0}" -ne 0 ] ; then
    redir='>/dev/null 2>&1'
else
    redir=
fi

build_options() {
    # Build up the options string
    :
}

start() {
    echo -n "Loading STREAMS kernel modules: "
    for module in $STRXNS_MODULES ; do
	if ! grep "^$module"'[[:space:]]' /proc/modules $redir ; then
	    echo -n "$module "
	    modprobe -k -q -- $module $redir
	    [ $? -eq 0 ] || echo -n "(failed)"
	fi
    done
    echo "."

    echo -n "Starting $desc: $name "
    build_options
    RETVAL=$?
    if [ $RETVAL -eq 0 ] ; then
	echo "."
    else
	echo "(failed.)"
    fi
    if grep '^[[:space:]]*'${name}'[/.]' /etc/sysctl.conf $redir ; then
	echo -n "Reconfiguring kernel parameters: "
	sysctl -p /etc/sysctl.conf $redir
	RETVAL=$?
	if [ $RETVAL -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	fi
    fi
    if [ -f /etc/${name}.conf ] ; then
	echo -n "Configuring STREAMS XNS parameters: "
	sysctl -p /etc/${name}.conf $redir
	RETVAL=$?
	if [ $RETVAL -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	fi
    fi
    if [ -n "$STRXNS_MKNOD" -a ":$STRXNS_MAKEDEVICES" = ":yes" ] ; then
	echo -n "Making STREAMS XNS devices: "
	$STRXNS_MKNOD
	RETVAL=$?
	if [ $RETVAL -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	fi
    fi
    return $RETVAL
}

remove_modules() {
    modules=
    while read -a module ; do
	modules="${modules}${modules:+ }${module[0]}"
    done
    if [ -n "$modules" ] ; then
	echo -n "Removing STREAMS XNS modules: "
	rmmod $modules
	RETVAL=$?
    fi
    return $RETVAL
}

stop() {
    echo -n "Stopping $desc: $name "
    RETVAL=$?
    if [ -n "$STRXNS_MKNOD" -a ":$STRXNS_REMOVEDEVICES" = ":yes" ] ; then
	echo -n "Removing STREAMS XNS devices: "
	$STRXNS_MKNOD --remove
	RETVAL=$?
    fi
    [ $RETVAL -eq 0 ] && egrep '^streams[-_]ip_strm_mod' /proc/modules 2>/dev/null | remove_modules
    RETVAL=$?
    [ $RETVAL -eq 0 ] && egrep '^streams[-_]ip_to_dlpi' /proc/modules 2>/dev/null | remove_modules
    RETVAL=$?
    [ $RETVAL -eq 0 ] && egrep '^streams[-_]ldl' /proc/modules 2>/dev/null | remove_modules
    RETVAL=$?
    [ $RETVAL -eq 0 ] && egrep '^streams[-_]np_ip' /proc/modules 2>/dev/null | remove_modules
    RETVAL=$?
    if [ $RETVAL -eq 0 ] ; then
	echo "."
    else
	echo "(failed.)"
    fi
    return $RETVAL
}

restart() {
    stop
    start
    return $?
}

show() {
    echo "$name.sh: show: not yet implemented." >&2
    return 1
}

usage() {
    echo "Usage: /etc/init.d/$name.sh (start|stop|restart|force-reload|show)" >&2
    return 1
}

case "$1" in
    (start|stop|restart|show)
	$1 || RETVAL=$?
	;;
    (force-reload)
	restart || RETVAL=$?
	;;
    (*)
	usage || RETVAL=$?
	;;
esac

[ "${0##*/}" = "$name.sh" ] && exit $RETVAL

# =============================================================================
# 
# @(#) $RCSfile: strxns.sh,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2006/07/15 13:06:31 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 2 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2006/07/15 13:06:31 $ by $Author: brian $
#
# =============================================================================

# vim: ft=sh sw=4 tw=80

