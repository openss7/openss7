#!/bin/sh
#
# @(#) $RCSfile: specfs.sh,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-02-07 04:44:28 $
# Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
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
# specfs:	mount and unmount specfs
# update-rc.d:	start 35 S .
# config:	/etc/default/specfs
# probe:	false
# hide:		false
# license:	GPL
# description:	The SPECFS is a special shadow filesystem used for Linux \
#		Fast-STREAMS.  The purpose of this init script is to detect \
#		whether the specfs is supported for the running kernel, and if \
#		it is, to configure and load the kernel module associated with \
#		the special filesystem.
#
# LSB init script conventions
#
### BEGIN INIT INFO
# Provides: specfs
# Required-Start: $local_fs
# Required-Stop: $local_fs
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# X-UnitedLinux-Default-Enabled: yes
# Short-Description: install and remove specfs filesystem
# License: GPL
# Description:	The SPECFS is a special shadow filesystem used for Linux
#	Fast-STREAMS.  The purpose of this init script is to detect whether the
#	specfs is supported for the running kernel, and if it is, to configure
#	and load the kernel module associated with the special filesystem.
### END INIT INFO

PATH=/usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin

# Source init script functions library.
init_mode="standalone"
  if [ -r /etc/init.d/functions   ] ; then . /etc/init.d/functions   ; init_mode="chkconfig" ;
elif [ -r /etc/rc.status          ] ; then . /etc/rc.status          ; init_mode="insserv"   ;
elif [ -r /lib/lsb/init-functions ] ; then . /lib/lsb/init-functions ; init_mode="lsb"       ;
fi

case "$init_mode" in
    (chkconfig)
	;;
    (insserv)
	action() {
	    echo -en "$1"
	    shift 1
	    ${1+"$@"} >/dev/null
	    RETVAL=$?
	    [ $RETVAL -eq 0 ] || rc_failed $RETVAL
	    rc_status -v
	    return $RETVAL
	}
	;;
    (lsb)
	action() {
	    echo -en "$1"
	    shift 1
	    ${1+"$@"} >/dev/null
	    RETVAL=$?
	    [ $RETVAL -eq 0 ] && log_success_msg || log_failure_msg
	    return $RETVAL
	}
	;;
    (standalone|*)
	action() {
	    echo -en "$1"
	    shift 1
	    ${1+"$@"} >/dev/null
	    RETVAL=$?
	    [ $RETVAL -eq 0 ] && echo -e "\t...SUCCESS" || echo -e "\t....FAILED"
	    return $?
	}
	;;
esac

name='specfs'
ucname=$"SPECFS"
lockfile="/var/lock/subsys/$name"
config="/etc/default/$name"
mkdev="${name}_mkdev"
desc="the STREAMS special shadow filesystem"

if [ ! -e /proc/modules     ] ; then if [ "$1" = 'stop' ] ; then exit 0 ; else exit 5 ; fi ; fi
if [ ! -e /proc/filesystems ] ; then if [ "$1" = 'stop' ] ; then exit 0 ; else exit 5 ; fi ; fi

# Specify defaults

[ -n "$SPECFS_MOUNTSPECFS"    ] || SPECFS_MOUNTSPECFS='yes'
[ -n "$SPECFS_UMOUNTSPECFS"   ] || SPECFS_UMOUNTSPECFS='yes'
[ -n "$SPECFS_MOUNTPOINT"     ] || SPECFS_MOUNTPOINT='/dev/streams'
[ -n "$SPECFS_UID"            ] || SPECFS_UID='0'
[ -n "$SPECFS_GID"            ] || SPECFS_GID='0'
[ -n "$SPECFS_MODE"           ] || SPECFS_MODE='0777'
[ -n "$SPECFS_OPTIONS"        ] || SPECFS_OPTIONS='-o uid=0,gid=0,mode=0777'

[ -n "$SPECFS_PRELOAD"        ] || SPECFS_PRELOAD=
[ -n "$SPECFS_DRIVERS"        ] || SPECFS_DRIVERS=
[ -n "$SPECFS_MODULES"        ] || SPECFS_MODULES=
[ -n "$SPECFS_MAKEDEVICES"    ] || SPECFS_MAKEDEVICES='yes'
[ -n "$SPECFS_REMOVEDEVICES"  ] || SPECFS_REMOVEDEVICES='yes'
[ -n "$SPECFS_FORCEREMOVE"    ] || SPECFS_FORCEREMOVE='yes'

# Source redhat and/or debian config file
for file in $config ; do
    [ -f $file ] && . $file
done

if [ -z "$SPECFS_MKDEV" -o ! -x "$SPECFS_MKDEV" ] ; then
    SPECFS_MKDEV=
    for prog in ./${mkdev} /sbin/${mkdev} /usr/sbin/${mkdev} /bin/${mkdev} /usr/bin/${mkdev} ; do
	if [ -x $prog ] ; then
	    SPECFS_MKDEV=$prog
	    break
	fi
    done
fi

[ -z "$SPECFS_MKDEV" ] && SPECFS_MAKEDEVICES='no'
[ -z "$SPECFS_MKDEV" ] && SPECFS_REMOVEDEVICES='no'

RETVAL=0

umask 077

case ":$VERBOSE" in
    :no|:NO|:false|:FALSE|:0|:)
	redir='>/dev/null 2>&1'
	;;
    *)
	redir=
	;;
esac

build_options() {
    # Build up the options string
    [ -n "$SPECFS_UID" ] && \
	SPECFS_OPTIONS="${SPECFS_OPTIONS:--o }${SPECFS_OPTIONS:+,}uid=${SPECFS_UID}"
    [ -n "$SPECFS_GID" ] && \
	SPECFS_OPTIONS="${SPECFS_OPTIONS:--o }${SPECFS_OPTIONS:+,}gid=${SPECFS_GID}"
    [ -n "$SPECFS_MODE" ] && \
	SPECFS_OPTIONS="${SPECFS_OPTIONS:--o }${SPECFS_OPTIONS:+,}mode=${SPECFS_MODE}"
    [ "$1" = remount ] && \
	SPECFS_OPTIONS="${SPECFS_OPTIONS:--o }${SPECFS_OPTIONS:+,}remount"
}

modprobe_name() {
    module=$1
    shift
    modname="kernel module $module"
    case $module in
    (specfs)
	modname=$"Special Shadow Filesystem (specfs)"
	;;
    (streams)
	modname=$"SVR 4.2MP Linux Fast-STREAMS"
	;;
    (streams-sth)
	modname=$"SVR 4.2MP Linux Fast-STREAMS Stream Head"
	;;
    (streams-clone)
	modname=$"SVR 4.2MP Linux Fast-STREAMS Clone Driver"
	;;
    (streams-log)
	modname=$"SVR 4.2MP Linux Fast-STREAMS Log Driver"
	;;
    (streams-aixcompat)
	modname=$"AIX(R) 5L Version 5.1 Portable STREAMS Environment"
	;;
    (streams-hpuxcompat)
	modname=$"HP-UX(R) 11.0i v2 STREAMS/UX"
	;;
    (streams-irixcompat)
	modname=$"IRIX(R) 6.5.17"
	;;
    (streams-liscompat)
	modname=$"Linux(R) STREAMS 2.18.4"
	;;
    (streams-maccompat)
	modname=$"Mac(R) OS 9 Open Transport"
	;;
    (streams-mpscompat)
	modname=$"MPS(R) Mentat Portable Streams"
	;;
    (streams-osfcompat)
	modname=$"Digital(R) UNIX (OSF/1.2)"
	;;
    (streams-suncompat)
	modname=$"Solaris(R) 9/SunOS(R) 5.9"
	;;
    (streams-svr3compat)
	modname=$"UNIX(R) System V Release 3.2"
	;;
    (streams-svr4compat)
	modname=$"UNIX(R) System V Release 4.2"
	;;
    (streams-uw7compat)
	modname=$"UnixWare(R) 7.1.3"
	;;
    esac
    echo "$modname"
}

modprobe_action() {
    module=$1
    shift
    modname=`modprobe_name $module`
    action $"Loading $modname:" \
	modprobe -q -- $module
    return $?
}

modprobe_remove() {
    module=$1
    shift
    modname=`modprobe_name $module`
    action $"Unloading $modname:" \
	modprobe -q -r -- $module
    return $?
}

#
# remove_depends() is a recursive function that attempts to remove all dependent
# modules starting at a target module and finally removing the target module.
# When any of the removals fail, the function escapes with a non-zero return
# value.
#
remove_depends() {
    local depend depends
    modrex=`echo $1 | sed -e 's,[-_],[-_],g'`
    depends=`grep "^$modrex[[:space:]]" /proc/modules 2>/dev/null | cut -f4 '-d ' | sed -e 's|^-$||;s|,$||;s|,| |g'`
    for depend in $depends ; do
	remove_depends $depend || return $?
    done
    modprobe_remove $1
    return $?
}

#
# start_specfs() inserts the 'specfs' kernel module.  This is done even when we
# are not mounting the specfs filesystem: the specfs filesystem is kernel
# mounted and can still be accessed through external character devices.
#
start_specfs() {
    if ! grep '^specfs[[:space:]]' /proc/modules >/dev/null 2>&1 ; then
	modprobe_action 'specfs' || RETVAL=$?
    fi
    return $RETVAL
}

#
# reload_specfs() simply ensures that the 'specfs' kernel module is loaded, so
# it simply call start_specfs().
#
reload_specfs() {
    start_specfs || RETVAL=$?
    return $RETVAL
}

#
# stop_specfs() attempts to remove the 'specfs' kernel module.  This can only be
# successful when the specfs filesystem is unmounted.  This also attempts to
# remove any dependent modules.
#
stop_specfs() {
    if [ ":$SPECFS_UMOUNTSPECFS" = ':yes' ] ; then
	remove_depends 'specfs' || RETVAL=$?
    fi
    return $RETVAL
}

#
# start_mountpoint() attempts to create all of the mount point directories
# specified when the script is set to mount the specfs filesystem at those
# points.
#
start_mountpoint() {
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if [ ! -d "$mountpoint" ] ; then
		action $"Creating mount point $mountpoint: " \
		    mkdir -p -- "$mountpoint" \
		    || RETVAL=$?
	    fi
	done
    fi
    return $RETVAL
}

#
# reload_mountpoint() simply ensures that all of the specified mount points
# exist when the script is configured to mount the specfs filesystem at those
# points.  It simply calls start_mountpoint().
#
reload_mountpoint() {
    start_mountpoint || RETVAL=$?
    return $RETVAL
}

#
# stop_mountpoint() attempts to remove all of the mount point directories
# specified when the script is set to unmount the specfs filesystem from those
# mount points.
#
stop_mountpoint() {
    if [ ":$SPECFS_UMOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if [ -d "$mountpoint" ] ; then
		action $"Removing mount point $mountpoint: " \
		rmdir -- "$mountpoint" \
		|| RETVAL=$?
	    fi
	done
    fi
    return $RETVAL
}

#
# start_mount() attempts to mount the specfs filesystem on all of the specified
# mount points and with the specified options.
#
start_mount() {
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if ! mount | grep " on $mountpoint type specfs" >/dev/null 2>&1 ; then
		build_options
		action $"Mounting SPECFS filesystem on $mountpoint: " \
		    mount -t specfs ${SPECFS_OPTIONS} -- specfs "$mountpoint" \
		    || RETVAL=$?
	    fi
	done
    fi
    return $RETVAL
}

#
# reload_mount() attempts to unmount and remove the directory for any mount
# point that is no longer specified but has a specfs filesystem mounted at that
# point.  It also remounts any mount point that is specified and currently
# mounted so that mount parameters are updated.  It then uses start_mount() to
# ensure that a mount point and mount exists for all the specified mount points.
#
reload_mount() {
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	while read -a fields ; do
	    if [ ":${fields[2]}" = ':specfs' ] ; then
		mountpoint=${fields[1]}
		if [ -n "$mountpoint" ] ; then
		    case " $SPECFS_MOUNTPOINT " in
		    (*" $mountpoint "*)
			build_options remount
			action $"Remounting SPECFS on $mountpoint: " \
			    mount -t specfs ${SPECFS_OPTIONS} -- specfs "$mountpoint" \
			    || RETVAL=$?
			continue
			;;
		    esac
		    action $"Unmounting SPECFS filesystem from $mountpoint: " \
			umount -t specfs -- "$mountpoint" \
			|| RETVAL=$?
		    action $"Removing mount point $mountpoint: " \
			rmdir -- "$mountpoint" \
			|| RETVAL=$?
		fi
	    fi
	done </etc/mtab
    fi
    start_mount || RETVAL=$?
    return $RETVAL
}

#
# stop_mount() attempts to unmount the specfs filesystem from wherever it is
# mounted (in user space).  The filesystem should unmount regardless of whether
# a Stream is open or not.  Failure to unmount a mount point is an error.  The
# only thing that could cause a failure is a busy filesystem (i.e. some process
# has a subdirectory as the current working directory).  Note that we should
# always unmount the filesystems when performing a forced remove so that killed
# processes cannot reopen drivers.
#
stop_mount() {
    if [ ":$SPECFS_UMOUNTSPECFS" = ':yes' -o ":$SPECFS_FORCEREMOVE" = ':yes' ] ; then
	if grep '^[^[:space:]]* [^[:space:]]* specfs ' /etc/mtab >/dev/null 2>&1 ; then
	    action $"Unmounting SPECFS filesystem: " \
		umount -a -t specfs \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

#
# start_preload() is used to insert all preload specified modules.  Typically
# for specfs this is the 'specfs' module itself.  The 'specfs' module must be
# loaded before sysctl parameters can be configured and before the specfs
# filesystem can be mounted.
# On some systems it is possible to identify this module as a system preloaded
# module, but on others the module is only loaded when the initscript is run.
#
start_preload() {
    # insert in forward order
    modules=
    for module in $SPECFS_PRELOAD ; do
	modules="${modules:+$modules }$module"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if ! grep "^$modrex[[:space:]]" /proc/modules >/dev/null 2>&1 ; then
	    modprobe_action $module \
	    || RETVAL=$?
	fi
    done
    return $RETVAL
}

#
# reload_preload() is used when reloading.  The only necessity is to ensure that
# preloaded modules are present to be reconfigured, so start_preload() is simply
# called.
#
reload_preload() {
    start_preload || RETVAL=$?
    return $RETVAL
}

#
# stop_preload() is used to remove specified preloaded modules.  Typically for
# specfs this would include the 'specfs' module itself.  This function also
# attempts to remove all dependent modules.  Note that for specfs the specfs
# filesystem must be unmounted before attempting this or it wil fail.
#
stop_preload() {
    # remove in reverse order
    modules=
    for module in $SPECFS_PRELOAD ; do
	modules="$module${modules:+ $modules}"
    done
    for module in $modules ; do
	remove_depends $module || ( RETVAL=$? ; break )
    done
    return $RETVAL
}

#
# start_params() is used to configure sysctl parameters associated with
# preloaded STREAMS modules and drivers, and, in particular the 'streams' kernel
# module.
#
start_params() {
    if grep '^[[:space:]]*'${name}'[/.]' /etc/sysctl.conf >/dev/null 2>&1 ; then
	action $"Reconfiguring kernel parameters: " \
	    sysctl -q -e -p /etc/sysctl.conf 2>/dev/null || :
    fi
    if [ -f /etc/${name}.conf ] ; then
	action $"Configuring $ucname parameters: " \
	    sysctl -q -e -p /etc/${name}.conf 2>/dev/null || :
    fi
    return $RETVAL
}

reload_params() {
    local needconfig='no'
    if [ ! -f $lockfile ] ; then
	needconfig='yes'
    else
	for file in $config /etc/sysctl.conf /etc/${name}.conf ; do
	    if [ -f $file -a $file -nt $lockfile ] ; then
		needconfig='yes'
		break
	    fi
	done
    fi
    if [ ":$needconfig" = ':yes' ] ; then
	start_params || RETVAL=$?
    fi
    return $RETVAL
}

#
# stop_params() is used to deconfigure sysctl parameters associated with
# preloaded STREAMS modules and drivers, and, in particular the 'streams' kernel
# module.  There are no actions required to deconfigure sysctl parameters.
#
stop_params() {
    return $RETVAL
}

#
# start_devices() is used to configure devices associated with the STREAMS
# subsystem.
#
start_devices() {
    if [ ":$SPECFS_MAKEDEVICES" = ':yes' ] ; then
	if [ -n "$SPECFS_MKDEV" -a -x "$SPECFS_MKDEV" ] ; then
	    action $"Making $ucname devices: " \
		$SPECFS_MKDEV --create \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

reload_devices() {
    if [ ":$SPECFS_MAKEDEVICES" = ':yes' ] ; then
	if [ -n "$SPECFS_MKDEV" -a -x "$SPECFS_MKDEV" ] ; then
	    action $"Making $ucname devices: " \
		$SPECFS_MKDEV --create \
		|| RETVAL=$?
	fi
    else
	if [ -n "$SPECFS_MKDEV" -a -x "$SPECFS_MKDEV" ] ; then
	    action $"Removing $ucname devices: " \
		$SPECFS_MKDEV --remove \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

#
# stop_devices() is used to deconfigure devices associated with the STREAMS
# susbsystem.
#
stop_devices() {
    if [ ":$SPECFS_REMOVEDEVICES" = ':yes' ] ; then
	if [ -n "$SPECFS_MKDEV" -a -x "$SPECFS_MKDEV" ] ; then
	    action $"Removing $ucname devices: " \
		$SPECFS_MKDEV --remove \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

#
# start_modules() is used to insert kernel modules at startup.  Because modules
# are autoloaded as required, there is nothing to do here.
#
start_modules() {
    return $RETVAL
}

reload_modules() {
    start_modules
    return $RETVAL
}

#
# stop_modules() gets rid of any modules using the specfs kernel module or using
# the specfs file system.  We have an advantage here over that used by the
# streams init script in that devices have been removed at this point and we can
# unmount the specfs filesystem before calling this function.  This means that
# any user processes that may attempt to reopen a stream will fail because there
# are no devices available to open.
#
stop_modules() {
    #
    # Typically we do not want to do a forced removal at this point.  This is
    # because terminating daemon processes might simply result in the reopening
    # of a Stream and reinsertion of any removed module.  Forced removal is more
    # appropriate in the specfs init script where the specfs can first be
    # unmounted and devices removed before terminating user processes.  In the
    # specfs script, attempts to reopen devices will be thwarted by the absence
    # of the device.  User processes can be rewritten to wait for some period of
    # time before attempting to reopen devices.
    #
    if [ ":$SPECFS_FORCEREMOVE" = ':yes' ] ; then
	# kill any process holding a specfs device open
	if lsof | grep '\<STR\>' >/dev/null 2>&1 ; then
	    # try to fdetach any fattached Streams
	    if [ -x /usr/sbin/fdetach ] ; then
		/usr/sbin/fdetach -a 2>/dev/null || :
	    fi
	    # kill any processes holding open a STREAMS device
	    for signal in TERM KILL ; do
		if ( lsof | grep '\<STR\>' ) >/dev/null 2>&1 ; then
		    echo $"Sending STREAMS processes the $signal signal..."
		    # COMMAND PID USER FD TYPE DEVICE SIZE NODE NAME...
		    lsof | grep '\<STR\>' | \
		    while read -a fields ; do
			if [ ":${fields[4]}" != ':CHR' ] ; then continue; fi
			if [ ":${fields[7]}" != ':STR' ] ; then continue; fi
			echo $"Killing ${fields[0]}(${fields[1]}) for device ${fields[8]}"
			eval "kill -$signal ${fields[1]}"
		    done
		    # sleep 5 sec to allow killed process to complete
		    echo $"Waiting for 5 seconds..."
		    sleep 5
		fi
	    done
	    # A problem with the above approach is that it only handles simple
	    # cases of open STREAMS devices.  The difficulty occurs when there
	    # is a Stream permanently linked under a multiplexing driver: the
	    # STREAMS and specfs modules will still be held by any permanent
	    # link.  In that case, only a system restart will do.
	fi
    fi
    # try to remove modules that we know about in lsmod order
    while read -a fields ; do
	module=${fields[0]}
	if [ -n "$module" ] ; then
	    modrex=`echo "$module" | sed -e 's,[-_],[-_],g'`
	    case " $SPECFS_DRIVERS $SPECFS_MODULES " in
		(*" $modrex "*)
		    modprobe_remove $module 2>/dev/null || :
		    ;;
	    esac
	fi
    done </proc/modules
    # try to remove modules that we know about in reverse order
    modules=
    for module in $SPECFS_DRIVERS $SPECFS_MODULES ; do
	modules="${module}${modules:+ $modules}"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if grep "^$modrex[[:space:]]" /proc/modules >/dev/null 2>&1 ; then
	    modprobe_remove $module 2>/dev/null || :
	fi
    done
    return $RETVAL
}

start() {
    start_specfs	# start the specfs kernel module
    start_preload	# insert any STREAMS preloads
    start_params	# configure sysctl parameters
    start_mountpoint	# create mount point (typ. /dev/streams)
    start_mount		# mount the specfs (typ. /dev/streams)
    start_modules	# insert additional modules
    start_devices	# create devices
    [ $RETVAL -eq 0 ] && touch $lockfile
    return $RETVAL
}

stop() {
    stop_mount		# unmount the specfs filesystem
    stop_devices	# remove device files
    stop_modules	# kill processes and remove modules
    stop_params		# deconfigure parameters
    stop_preload	# remove preloads
    stop_specfs		# remove the specfs kernel module
    stop_mountpoint	# remove the mountpoint (usually /dev/streams)
    [ $RETVAL -eq 0 ] && rm -f -- $lockfile
    return $RETVAL
}

restart() {
    stop  || RETVAL=$?
    start || RETVAL=$?
    return $RETVAL
}

reload() {
    reload_specfs
    reload_mountpoint
    reload_mount
    reload_preload
    reload_params
    reload_modules
    reload_devices
    [ $RETVAL -eq 0 ] && touch $lockfile
    return $RETVAL
}

show() {
    echo "$0: show: not yet implemented." >&2
    RETVAL=1
    return $RETVAL
}

usage() {
    echo "Usage: $0 (start|stop|restart|reload|force-reload|show)" >&2
    RETVAL=1
    return $RETVAL
}

case "$1" in
    (start|stop|reload|restart|show)
	$1 || RETVAL=$?
	;;
    (force-reload)
	reload || RETVAL=$?
	;;
    (try-restart|condrestart)
	[ -f $lockfile ] && restart || RETVAL=$?
	;;
    (*)
	usage || RETVAL=$?
	;;
esac

[ "${0##*/}" = "$name.sh" ] && exit $RETVAL

# =============================================================================
# 
# @(#) $RCSfile: specfs.sh,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-02-07 04:44:28 $
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
# Last Modified $Date: 2011-02-07 04:44:28 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: specfs.sh,v $
# Revision 1.1.2.3  2011-02-07 04:44:28  brian
# - updated init scripts
#
# Revision 1.1.2.2  2011-01-12 00:19:32  brian
# - modprobe no longer accepts k option
#
# Revision 1.1.2.1  2009-06-21 11:47:57  brian
# - added files to new distro
#
# Revision 0.9.2.15  2008-10-21 22:42:15  brian
# - handle verbose better in debian initscripts
#
# Revision 0.9.2.14  2008-04-28 12:54:11  brian
# - update file headers for release
#
# Revision 0.9.2.13  2007/08/13 22:46:39  brian
# - GPLv3 header updates
#
# =============================================================================

# vim: ft=sh sw=4 tw=80
