#!/bin/sh
#
# @(#) $RCSfile: strace.sh,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/02/18 01:36:06 $
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
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
# strace:	start and stop strace facility
# update-rc.d:	stop 20 0 1 6 .
# config:	/etc/default/strace
# processname:	strace
# pidfile:	/var/run/strace.pid
# probe:	false
# hide:		false
# license:	GPL
# description:	This STREAMS init script is part of Linux Fast-STREAMS.  It is \
#		responsible for starting and stopping the STREAMS trace \
#		logger.  The STREAMS trace logger should only be run under \
#		exceptional circumstances and this init script not activated \
#		automatically.
#

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin
name='strace'
config="/etc/default/$name"
processname="$name"
pidfile="/var/run/$processname.pid"
execfile="/usr/sbin/$processname"
desc="the STREAMS trace logger"

[ -x $execfile ] || exit 0

# Specify defaults

STRACEOPTIONS=
STRACE_DIRECTORY="/var/log/streams"
STRACE_BASENAME="trace"
STRACE_OUTFILE=
STRACE_ERRFILE=
STRACE_LOGDEVICE="/dev/streams/log"
STRACE_MODULES=
STRACE_OPTIONS=

# Source config file
for file in $config ; do
    [ -f $file ] && . $file
done

RETVAL=0

umask 077

build_options() {
    # Build up the options string
    STRACE_OPTIONS="-p $pidfile"
    [ -n "$STRACEOPTIONS" ] && \
	STRACE_OPTIONS="${STRACE_OPTIONS:+ }${STRACEOPTIONS}"
    [ -n "$STRACE_DIRECTORY" ] && \
	STRACE_OPTIONS="${STRACE_OPTIONS:+ }-d ${STRACE_DIRECTORY}"
    [ -n "$STRACE_BASENAME" ] && \
	STRACE_OPTIONS="${STRACE_OPTIONS:+ }-b ${STRACE_BASENAME}"
    [ -n "$STRACE_OUTFILE" ] && \
	STRACE_OPTIONS="${STRACE_OPTIONS:+ }-o ${STRACE_OUTFILE}"
    [ -n "$STRACE_ERRFILE" ] && \
	STRACE_OPTIONS="${STRACE_OPTIONS:+ }-e ${STRACE_ERRFILE}"
    [ -n "$STRACE_LOGDEVICE" ] && \
	STRACE_OPTIONS="${STRACE_OPTIONS:+ }-l ${STRACE_LOGDEVICE}"
    [ -n "$STRACE_MODULES" ] && \
	STRACE_OPTIONS="${STRACE_OPTIONS:+ }${STRACE_MODULES}"
}

start() {
    echo -n "Starting $desc: $name "
    build_options
    start-stop-daemon --start --quiet --pidfile $pidfile \
	--exec $execfile -- $STRACE_OPTIONS
    RETVAL=$?
    if [ $RETVAL -eq 0 ] ; then
	echo "."
    else
	echo "(failed.)"
    fi
    return $RETVAL
}

stop() {
    echo -n "Stopping $desc: $name "
    start-stop-daemon --stop --quiet --retry=1 --oknodo --pidfile $pidfile \
	--exec $execfile
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

reload() {
    echo -n "Reloading $desc: $name "
    start-stop-daemon --stop --quiet --signal=1 --pidfile $pidfile \
	--exec $execfile
    RETVAL=$?
    if [ $RETVAL -eq 0 ] ; then
	echo "."
    else
	echo "(failed.)"
    fi
    return $RETVAL
}

show() {
    echo "$name.sh: show: not yet implemented." >&2
    return 1
}

usage() {
    echo "Usage: /etc/init.d/$name.sh (start|stop|restart|reload|force-reload|show)" >&2
    return 1
}

case "$1" in
    (start|stop|reload|restart|show)
	$1 || RETVAL=$?
	;;
    (force-reload)
	reload || RETVAL=$?
	;;
    (*)
	usage || RETVAL=$?
	;;
esac

[ "${0##*/}" = "$name.sh" ] && exit $RETVAL

# =============================================================================
# 
# @(#) $RCSfile: strace.sh,v $ $Name:  $($Revision: 0.9.2.3 $) $Date: 2005/02/18 01:36:06 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
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
# Last Modified $Date: 2005/02/18 01:36:06 $ by $Author: brian $
#
# =============================================================================

# vim: ft=sh sw=4 tw=80
