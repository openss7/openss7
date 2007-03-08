#!/bin/sh
#
# @(#) $RCSfile: strinet.sh,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2007/03/08 22:42:27 $
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com>
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
# strinet:	start and stop strinet subsystem
# update-rc.d:	start 33 S . stop 33 0 6 .
# config:	/etc/default/strinet
# probe:	false
# hide:		false
# license:	GPL
# description:	This STREAMS init script is part of Linux Fast-STREAMS.  \
#		It is responsible for ensuring that the necessary STREAMS \
#		character devices are present in the /dev directory and \
#		that the STREAMS INET subsystem is configured and loaded.
#
# LSB init script conventions
#
### BEGIN INIT INFO
# Provides: strinet
# Required-Start: streams $network
# Required-Stop: streams $network
# Default-Start: 3 4 5
# Default-Stop: 0 1 2 6
# X-UnitedLinux-Default-Enabled: yes
# Short-Description: start and stop STREAMS INET subsystem
# License: GPL
# Description:	This STREAMS INET init script is part of Linux Fast-STREAMS.
#	It is reponsible for ensuring that the necessary STREAMS INET character
#	devices are present in the /dev directory and that the STREAMS INET
#	subsystem is configured and loaded.
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
name='strinet'
config="/etc/default/$name"
desc="the STREAMS INET subsystem"
mknod="${name}_mknod"

[ -e /proc/modules ] || exit 0

if test -z "$STRINET_MKNOD" ; then
    for STRINET_MKNOD in /sbin/${name}_mknod /usr/sbin/${name}_mknod /bin/${name}_mknod /usr/bin/${name}_mknod ; do
	if [ -x $STRINET_MKNOD ] ; then
	    break
	else
	    STRINET_MKNOD=
	fi
    done
fi

if test -z "$INET_MKDEV" ; then
    for INET_MKDEV in /sbin/inet_mkdev /usr/sbin/inet_mkdev /bin/inet_mkdev /usr/bin/inet_mkdev ; do
	if [ -x $INET_MKDEV ] ; then
	    break
	else
	    INET_MKDEV=
	fi
    done
fi

if test -z "$INET_RMDEV" ; then
    for INET_RMDEV in /sbin/inet_rmdev /usr/sbin/inet_rmdev /bin/inet_rmdev /usr/bin/inet_rmdev ; do
	if [ -x $INET_RMDEV ] ; then
	    break
	else
	    INET_RMDEV=
	fi
    done
fi

# Specify defaults

[ -n "$STRINET_PRELOAD"       ] || STRINET_PRELOAD=""
[ -n "$STRINET_DRIVERS"       ] || STRINET_DRIVERS="streams-inet streams-rawip streams-udp"
[ -n "$STRINET_MODULES"       ] || STRINET_MODULES=""
[ -n "$STRINET_MAKEDEVICES"   ] || STRINET_MAKEDEVICES="yes"
[ -n "$STRINET_REMOVEDEVICES" ] || STRINET_REMOVEDEVICES="yes"

# Source config file
for file in $config ; do
    [ -f $file ] && . $file
done

[ -z "$STRINET_MKNOD" -a -z "$INET_MKDEV" ] && STRINET_MAKEDEVICES="no"
[ -z "$STRINET_MKNOD" -a -z "$INET_RMDEV" ] && STRINET_REMOVEDEVICES="no"

RETVAL=0

umask 077

if [ "${VERBOSE:-0}" -eq 0 ] ; then
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
    RETVAL=0
    modules=
    for module in $STRINET_PRELOAD ; do
	modules="${modules:+$modules }$module"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if ! eval "grep '^$modrex\>' /proc/modules $redir" ; then
	    echo -n "$module "
	    eval "modprobe -k -q -- $module $redir"
	    [ $? -eq 0 ] || echo -n "(failed)"
	fi
    done
    echo "."

    echo -n "Starting $desc: $name "
    build_options
    if [ $? -eq 0 ] ; then
	echo "."
    else
	echo "(failed.)"
	RETVAL=1
    fi

    if eval "grep '^[[:space:]]*${name}[/.]' /etc/sysctl.conf $redir" ; then
	echo -n "Reconfiguring kernel parameters: "
	eval "sysctl -p /etc/sysctl.conf $redir"
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	fi
    fi

    if [ -f /etc/${name}.conf ] ; then
	echo -n "Configuring STREAMS parameters: "
	eval "sysctl -p /etc/${name}.conf $redir"
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	    RETVAL=1
	fi
    fi

    if [ -n "$STRINET_MKNOD" -o -n "$INET_MKDEV" ] ; then
	if [ :"$STRINET_MAKEDEVICES" = ":yes" ] ; then
	    if [ -n "$INET_MKDEV" ] ; then
		echo -n "Making STREAMS iBCS devices: "
		$INET_MKDEV
		if [ $? -eq 0 ] ; then
		    echo "."
		else
		    echo "(failed.)"
		    RETVAL=1
		fi
	    fi
	    if [ -n "$STRINET_MKNOD" ] ; then
		echo -n "Making STREAMS INET devices: "
		$STRINET_MKNOD
		if [ $? -eq 0 ] ; then
		    echo "."
		else
		    echo "(failed.)"
		    RETVAL=1
		fi
	    fi
	fi
    fi
    return $RETVAL
}

stop() {
    echo "Stopping $desc: $name "
    RETVAL=0
    if [ -n "$STRINET_MKNOD" -a ":$STRINET_REMOVEDEVICES" = ":yes" ] ; then
	echo -n "Removing STREAMS devices: "
	$STRINET_MKNOD --remove
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	    RETVAL=1
	fi
    fi
    echo -n "Unloading STREAMS kernel modules: "
    modules=
    for module in $STRINET_PRELOAD $STRINET_DRIVERS $STRINET_MODULES ; do
	modules="$module${modules:+ $modules}"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if eval "grep '^$modrex\>' /proc/modules $redir" ; then
	    echo -n "$module "
	    eval "modprobe -r -q -- $module $redir"
	    if [ $? -ne 0 ] ; then
		echo -n "(failed) "
		RETVAL=1
	    fi
	fi
    done
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
# @(#) $RCSfile: strinet.sh,v $ $Name:  $($Revision: 0.9.2.15 $) $Date: 2007/03/08 22:42:27 $
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
# Last Modified $Date: 2007/03/08 22:42:27 $ by $Author: brian $
#
# =============================================================================

# vim: ft=sh sw=4 tw=80
