#!/bin/sh
#
# @(#) $RCSfile: stratm.sh,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-01-12 00:19:32 $
# Copyright (c) 2001-2011  OpenSS7 Corporation <http://www.openss7.com>
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
# stratm:	start and stop stratm subsystem
# update-rc.d:	start 33 S . stop 33 0 6 .
# config:	/etc/default/stratm
# probe:	false
# hide:		false
# license:	GPL
# description:	This STREAMS init script is part of Linux Fast-STREAMS.  \
#		It is responsible for ensuring that the necessary STREAMS \
#		character devices are present in the /dev directory and \
#		that the STREAMS ATM subsystem is configured and loaded.
#
# LSB init script conventions
#
### BEGIN INIT INFO
# Provides: stratm
# Required-Start: streams $network
# Required-Stop: streams $network
# Default-Start: 3 4 5
# Default-Stop: 0 1 2 6
# X-UnitedLinux-Default-Enabled: yes
# Short-Description: start and stop STREAMS ATM subsystem
# License: GPL
# Description:	This STREAMS ATM init script is part of Linux Fast-STREAMS.
#	It is reponsible for ensuring that the necessary STREAMS ATM character
#	devices are present in the /dev directory and that the STREAMS ATM
#	subsystem is configured and loaded.
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
name='stratm'
config="/etc/default/$name"
desc="the STREAMS ATM subsystem"
mknod="${name}_mknod"

[ -e /proc/modules ] || exit 0

if test -z "$STRATM_MKNOD" ; then
    for STRATM_MKNOD in /sbin/${mknod} /usr/sbin/${mknod} /bin/${mknod} /usr/bin/${mknod} ; do
	if [ -x $STRATM_MKNOD ] ; then
	    break
	else
	    STRATM_MKNOD=
	fi
    done
fi

# Specify defaults

[ -n "$STRATM_PRELOAD"       ] || STRATM_PRELOAD=""
[ -n "$STRATM_DRIVERS"       ] || STRATM_DRIVERS="streams-atm"
[ -n "$STRATM_MODULES"       ] || STRATM_MODULES="streams-phys streams-aal1 streams-aal2 streams-aal5 streams-sscop streams-sccopmce streams-mtp3b"
[ -n "$STRATM_MAKEDEVICES"   ] || STRATM_MAKEDEVICES="yes"
[ -n "$STRATM_REMOVEDEVICES" ] || STRATM_REMOVEDEVICES="yes"

# Source config file
for file in $config ; do
    [ -f $file ] && . $file
done

[ -z "$STRATM_MKNOD" ] && STRATM_MAKEDEVICES="no"
[ -z "$STRATM_MKNOD" ] && STRATM_REMOVEDEVICES="no"

RETVAL=0

umask 077

case :$VERBOSE in
    :no|:NO|:false|:FALSE|:0|:)
	redir='>/dev/null 2>&1'
	;;
    *)
	redir=
	;;
esac

build_options() {
    # Build up the options string
    :
}

start() {
    echo -n "Loading STREAMS kernel modules: "
    RETVAL=0
    modules=
    for module in $STRATM_PRELOAD ; do
	modules="${modules:+$modules }$module"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if ! eval "grep '^$modrex\>' /proc/modules $redir" ; then
	    echo -n "$module "
	    eval "modprobe -q -- $module $redir"
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

    if [ -n "$STRATM_MKNOD" -a ":$STRATM_MAKEDEVICES" = ":yes" ] ; then
	echo -n "Making STREAMS ATM devices: "
	$STRATM_MKNOD
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
    if [ -n "$STRATM_MKNOD" -a ":$STRATM_REMOVEDEVICES" = ":yes" ] ; then
	echo -n "Removing STREAMS ATM devices: "
	$STRATM_MKNOD --remove
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	    RETVAL=1
	fi
    fi
    echo -n "Unloading STREAMS kernel modules: "
    modules=
    for module in $STRATM_PRELOAD $STRATM_DRIVERS $STRATM_MODULES ; do
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
# @(#) $RCSfile: stratm.sh,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-01-12 00:19:32 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2011  OpenSS7 Corporation <http://www.openss7.com/>
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
# Last Modified $Date: 2011-01-12 00:19:32 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: stratm.sh,v $
# Revision 1.1.2.2  2011-01-12 00:19:32  brian
# - modprobe no longer accepts k option
#
# Revision 1.1.2.1  2009-06-21 11:47:57  brian
# - added files to new distro
#
# Revision 0.9.2.1  2008-12-06 08:20:53  brian
# - added base release files for stratm package
#
# Revision 0.9.2.2  2008-10-21 22:42:22  brian
# - handle verbose better in debian initscripts
#
# Revision 0.9.2.1  2008-05-03 10:46:39  brian
# - added package files
#
# =============================================================================
# vim: ft=sh sw=4 tw=80

