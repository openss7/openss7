#!/bin/sh
#
# @(#) $RCSfile: strbcm.sh,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2006/10/16 00:21:16 $
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
# strbcm:	start and stop strbcm modules
# update-rc.d:	start 33 S . stop 33 0 6 .
# config:	/etc/default/strbcm
# probe:	false
# hide:		false
# license:	GPL
# description:	This STREAMS init script is part of Linux Fast-STREAMS.  \
#		It is responsible for ensuring that the necessary STREAMS \
#		BCM character devices are present in the /dev directory and \
#		that the STREAMS BCM kernel modules are configured and loaded.
#

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
name='strbcm'
config="/etc/default/$name"
desc="the STREAMS BCM modules"
mknod="${name}_mknod"

[ -e /proc/modules ] || exit 0

if test -z "$STRBCM_MKNOD" ; then
    for STRBCM_MKNOD in /sbin/${mknod} /usr/sbin/${mknod} /bin/${mknod} /usr/bin/${mknod} ; do
	if [ -x $STRBCM_MKNOD ] ; then
	    break
	else
	    STRBCM_MKNOD=
	fi
    done
fi

# Specify defaults

[ -n "$STRBCM_PRELOAD"       ] || STRBCM_PRELOAD=""
[ -n "$STRBCM_DRIVERS"       ] || STRBCM_DRIVERS="streams-tstdrv"
[ -n "$STRBCM_MODULES"       ] || STRBCM_MODULES="streams-tstmod"
[ -n "$STRBCM_MAKEDEVICES"   ] || STRBCM_MAKEDEVICES="yes"
[ -n "$STRBCM_REMOVEDEVICES" ] || STRBCM_REMOVEDEVICES="yes"

# Source config file
for file in $config ; do
    [ -f $file ] && . $file
done

[ -z "$STRBCM_MKNOD" ] && STRBCM_MAKEDEVICES="no"
[ -z "$STRBCM_MKNOD" ] && STRBCM_REMOVEDEVICES="no"

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
    RETVAL=0
    modules=
    for module in $STRBCM_PRELOAD ; do
	modules="${modules:+$modules }$module"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if ! grep "^$modrex\>" /proc/modules $redir ; then
	    echo -n "$module "
	    modprobe -k -q -- $module $redir
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

    if grep '^[[:space:]]*'${name}'[/.]' /etc/sysctl.conf $redir ; then
	echo -n "Reconfiguring kernel parameters: "
	sysctl -p /etc/sysctl.conf $redir
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	fi
    fi

    if [ -f /etc/${name}.conf ] ; then
	echo -n "Configuring STREAMS parameters: "
	sysctl -p /etc/${name}.conf $redir
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	    RETVAL=1
	fi
    fi

    if [ -n "$STRBCM_MKNOD" -a :"$STRBCM_MAKEDEVICES" = ":yes" ] ; then
	echo -n "Making STREAMS BCM devices: "
	$STRBCM_MKNOD
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	    RETVAL=1
	fi
    fi
    return $RETVAL
}

stop() {
    echo "Stopping $desc: $name "
    RETVAL=0
    if [ -n "$STRBCM_MKNOD" -a ":$STRBCM_REMOVEDEVICES" = ":yes" ] ; then
	echo -n "Removing STREAMS devices: "
	$STRBCM_MKNOD --remove
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	    RETVAL=1
	fi
    fi
    echo -n "Unloading STREAMS kernel modules: "
    modules=
    for module in $STRBCM_PRELOAD $STRBCM_DRIVERS $STRBCM_MODULES ; do
	modules="$module${modules:+ $modules}"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if grep "^$modrex\>" /proc/modules $redir ; then
	    echo -n "$module "
	    modprobe -r -q -- $module $redir
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
# @(#) $RCSfile: strbcm.sh,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2006/10/16 00:21:16 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
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
# Last Modified $Date: 2006/10/16 00:21:16 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: strbcm.sh,v $
# Revision 0.9.2.5  2006/10/16 00:21:16  brian
# - do not load too many kernel modules on init
#
# Revision 0.9.2.4  2006/10/13 06:59:51  brian
# - load drivers but not modules by default, but remove modules
#
# Revision 0.9.2.3  2006/10/13 03:59:53  brian
# - corrected init scripts and config files
#
# Revision 0.9.2.2  2006/08/16 07:40:46  brian
# - rework addition and removal of kernel modules
#
# Revision 0.9.2.1  2006/03/08 11:18:05  brian
# - completing package
#
# =============================================================================

# vim: ft=sh sw=4 tw=80
