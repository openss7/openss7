#!/bin/sh
#
# @(#) $RCSfile: streams.sh,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-12 00:19:32 $
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
# All Rights Reserved.
#
# Distributed by OpenSS7 Corporation.  See the bottom of this script for copying
# permissions.
#
# These are arguments to update-rc.d ala chkconfig and lsb.  They are recognized
# by openss7 install_initd and remove_initd scripts.  Each line specifies
# arguments to add and remove links after the the name argument:
#
# streams:	start and stop streams subsystem
# update-rc.d:	start 33 S . stop 33 0 6 .
# config:	/etc/default/streams
# probe:	false
# hide:		false
# license:	AGPL
# description:	This STREAMS init script is part of Linux Fast-STREAMS.  \
#		It is responsible for ensuring that the necessary STREAMS \
#		character devices are present in the /dev directory and \
#		that the STREAMS subsystem is configured and loaded.
#
# LSB init script conventions
#
### BEGIN INIT INFO
# Provides: streams
# Required-Start: specfs
# Required-Stop: specfs
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# X-UnitedLinux-Default-Enabled: yes
# Short-Description: start and stop streams subsystem
# License: AGPL
# Description:	This STREAMS init script is part of Linux Fast-STREAMS.  It is
#	reponsible for ensuring that the necessary STREAMS character devices are
#	present in the /dev directory and that the STREAMS subsystem is
#	configured and loaded.
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
name='streams'
config="/etc/default/$name"
desc="the STREAMS subsystem"
mkdev="${name}_mkdev"

[ -e /proc/modules ] || exit 0

if test -z "$STREAMS_MKDEV" ; then
    for STREAMS_MKDEV in /sbin/${mkdev} /usr/sbin/${mkdev} /bin/${mkdev} /usr/bin/${mkdev} ; do
	if [ -x $STREAMS_MKDEV ] ; then
	    break
	else
	    STREAMS_MKDEV=
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

[ -n "$STREAMS_PRELOAD"       ] || STREAMS_PRELOAD="streams"
[ -n "$STREAMS_DRIVERS"       ] || STREAMS_DRIVERS="streams-echo streams-fifo streams-log streams-loop streams-mux streams-nsdev streams-nuls streams-pipe streams-sad streams-sfx streams-spx"
[ -n "$STREAMS_MODULES"       ] || STREAMS_MODULES="streams-bufmod streams-connld streams-nullmod streams-pipemod streams-sc streams-testmod"
[ -n "$STREAMS_MAKEDEVICES"   ] || STREAMS_MAKEDEVICES="yes"
[ -n "$STREAMS_REMOVEDEVICES" ] || STREAMS_REMOVEDEVICES="yes"
[ -n "$STREAMS_MOUNTSPECFS"   ] || STREAMS_MOUNTSPECFS="yes"
[ -n "$STREAMS_MOUNTPOINT"    ] || STREAMS_MOUNTPOINT="/dev/streams"

# Source config file
for file in $config ; do
    [ -f $file ] && . $file
done

[ -z "$STREAMS_MKDEV" ] && STREAMS_MAKEDEVICES="no"
[ -z "$STREAMS_MKDEV" ] && STREAMS_REMOVEDEVICES="no"

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

modprobe_name() {
	module=$1
	shift
	modname=$module
	case $module in
	(streams-aixcompat)
	    modname=$"AIX"
	    ;;
	(streams-hpuxcompat)
	    modname=$"HP-UX"
	    ;;
	(streams-irixcompat)
	    modname=$"IRIX"
	    ;;
	(streams-liscompat)
	    modname=$"LiS"
	    ;;
	(streams-maccompat)
	    modname=$"MacOT"
	    ;;
	(streams-mpscompat)
	    modname=$"MPS"
	    ;;
	(streams-osfcompat)
	    modname=$"OSF/1"
	    ;;
	(streams-suncompat)
	    modname=$"Solaris"
	    ;;
	(streams-svr3compat)
	    modname=$"SVR3.2"
	    ;;
	(streams-svr4compat)
	    modname=$"SVR4.2"
	    ;;
	(streams-uw7compat)
	    modname=$"UnixWare"
	    ;;
	esac
	echo -n "$modname "
}

build_options() {
    # Build up the options string
    :
}

start() {
    echo -n "Loading STREAMS kernel modules: "
    RETVAL=0
    modules=
    for module in $STREAMS_PRELOAD ; do
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

    if [ -n "$STREAMS_MKDEV" -o -n "$INET_MKDEV" ] ; then
	if [ ":$STREAMS_MAKEDEVICES" = ":yes" ] ; then
	    if [ -n "$INET_MKDEV" ] ; then
		echo -n "Making iBCS devices: "
		$INET_MKDEV
	if [ $? -eq 0 ] ; then
	    echo "."
	else
	    echo "(failed.)"
	    RETVAL=1
	fi
	    if [ -n "$STREAMS_MKDEV" ] ; then
		echo -n "Making STREAMS devices: "
		$STREAMS_MKDEV --create
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
    if [ -n "$STREAMS_MKDEV" -o -n "$INET_RMDEV" ] ; then
	if [ ":$STREAMS_REMOVEDEVICES" = ":yes" ] ; then
	    echo -n "Removing STREAMS devices: "
	    $STREAMS_MKDEV --remove
	    if [ $? -eq 0 ] ; then
		echo "."
	    else
		echo "(failed.)"
		RETVAL=1
	    fi
	    echo -n "Removing iBCS devices: "
	    $INET_RMDEV
	    if [ $? -eq 0 ] ; then
		echo "."
	    else
		echo "(failed.)"
		RETVAL=1
	    fi
	fi
    fi
    echo -n "Unloading STREAMS kernel modules: "
    modules=
    for module in $STREAMS_PRELOAD $STREAMS_DRIVERS $STREAMS_MODULES ; do
	modules="$module${modules:+ $modules}"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if eval "grep '^$modrex\>' /proc/modules $redir" ; then
	    modprobe_name $module
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
# @(#) $RCSfile: streams.sh,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-12 00:19:32 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date: 2011-01-12 00:19:32 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: streams.sh,v $
# Revision 1.1.2.3  2011-01-12 00:19:32  brian
# - modprobe no longer accepts k option
#
# Revision 1.1.2.2  2009-07-24 13:49:46  brian
# - updates for release build
#
# Revision 1.1.2.1  2009-06-21 11:47:57  brian
# - added files to new distro
#
# Revision 0.9.2.21  2008-10-21 22:42:16  brian
# - handle verbose better in debian initscripts
#
# Revision 0.9.2.20  2008-04-28 12:54:11  brian
# - update file headers for release
#
# Revision 0.9.2.19  2007/08/13 22:46:39  brian
# - GPLv3 header updates
#
# =============================================================================

# vim: ft=sh sw=4 tw=80
