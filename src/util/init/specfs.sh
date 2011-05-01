#!/bin/sh
#
# @(#) $RCSfile: specfs.sh,v $ $Name:  $($Revision: 1.1.2.5 $) $Date: 2011-04-11 06:13:44 $
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
# update-rc.d:	start 33 S .
# lockfile:	/var/lock/subsys/specfs
# config:	/etc/default/specfs
# probe:	true
# hide:		false
# license:	AGPL
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
# Default-Start: S
# Default-Stop:
# X-Start-Before: $syslog
# X-Stop-After: $null
# X-UnitedLinux-Default-Enabled: yes
# Short-Description: install and remove specfs filesystem
# License: AGPL
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
elif [ -r /sbin/start-stop-daemon ] ; then                             init_mode="debian"    ;
fi

case "$init_mode" in
    (chkconfig)
	;;
    (insserv)
	action () {
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
	action () {
	    echo -en "$1"
	    shift 1
	    ${1+"$@"} >/dev/null
	    RETVAL=$?
	    [ $RETVAL -eq 0 ] && log_success_msg || log_failure_msg
	    return $RETVAL
	}
	;;
    (debian)
	action () {
	    echo -en "$1"
	    shift 1
	    eval "\${1+\"\$@\"} $redir"
	    RETVAL=$?
	    [ $RETVAL -eq 0 ] || echo "(failed)"; echo "."
	    return $RETVAL
	}
	;;
    (standalone|*)
	action () {
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
script='specfs.sh'
ucname="SPECFS"
lockfile="/var/lock/subsys/$name"
config="/etc/sysconfig/$name /etc/default/$name"
mkdev="${name}_mkdev"
desc="the STREAMS special shadow filesystem"

if [ ! -e /proc/modules     ] ; then if [ "$1" = 'stop' ] ; then exit 0 ; else exit 5 ; fi ; fi
if [ ! -e /proc/filesystems ] ; then if [ "$1" = 'stop' ] ; then exit 0 ; else exit 5 ; fi ; fi

# Specify defaults

[ -n "$SPECFS_KUPDATE"        ] || SPECFS_KUPDATE='yes'
[ -n "$SPECFS_BOOTLOAD"       ] || SPECFS_BOOTLOAD='no'
[ -n "$SPECFS_BOOTMOUNT"      ] || SPECFS_BOOTMOUNT='no'

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
    for prog in ./${mkdev} /sbin/${mkdev} /usr/sbin/${mkdev} /bin/${mkdev} /usr/bin/${mkdev}; do
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

build_options () {
    # Build up the options string
    SPECFS_OPTIONS=
    [ -n "$SPECFS_UID" ] && \
	SPECFS_OPTIONS="${SPECFS_OPTIONS}${SPECFS_OPTIONS:--o }${SPECFS_OPTIONS:+,}uid=${SPECFS_UID}"
    [ -n "$SPECFS_GID" ] && \
	SPECFS_OPTIONS="${SPECFS_OPTIONS}${SPECFS_OPTIONS:--o }${SPECFS_OPTIONS:+,}gid=${SPECFS_GID}"
    [ -n "$SPECFS_MODE" ] && \
	SPECFS_OPTIONS="${SPECFS_OPTIONS}${SPECFS_OPTIONS:--o }${SPECFS_OPTIONS:+,}mode=${SPECFS_MODE}"
    [ "$1" = remount ] && \
	SPECFS_OPTIONS="${SPECFS_OPTIONS}${SPECFS_OPTIONS:--o }${SPECFS_OPTIONS:+,}remount"
}

modprobe_name () {
    module=$1
    shift
    modname="kernel module $module"
    case $module in
    (specfs)
	modname="Special Shadow Filesystem (specfs)"
	;;
    (streams)
	modname="SVR 4.2MP Linux Fast-STREAMS"
	;;
    (streams-sth)
	modname="SVR 4.2MP Linux Fast-STREAMS Stream Head"
	;;
    (streams-clone)
	modname="SVR 4.2MP Linux Fast-STREAMS Clone Driver"
	;;
    (streams-log)
	modname="SVR 4.2MP Linux Fast-STREAMS Log Driver"
	;;
    (streams-aixcompat)
	modname="AIX(R) 5L Version 5.1 Portable STREAMS Environment"
	;;
    (streams-hpuxcompat)
	modname="HP-UX(R) 11.0i v2 STREAMS/UX"
	;;
    (streams-irixcompat)
	modname="IRIX(R) 6.5.17"
	;;
    (streams-liscompat)
	modname="Linux(R) STREAMS 2.18.4"
	;;
    (streams-maccompat)
	modname="Mac(R) OS 9 Open Transport"
	;;
    (streams-mpscompat)
	modname="MPS(R) Mentat Portable Streams"
	;;
    (streams-osfcompat)
	modname="Digital(R) UNIX (OSF/1.2)"
	;;
    (streams-suncompat)
	modname="Solaris(R) 9/SunOS(R) 5.9"
	;;
    (streams-svr3compat)
	modname="UNIX(R) System V Release 3.2"
	;;
    (streams-svr4compat)
	modname="UNIX(R) System V Release 4.2"
	;;
    (streams-uw7compat)
	modname="UnixWare(R) 7.1.3"
	;;
    esac
    echo "$modname"
}

modprobe_action () {
    module=$1
    shift
    modname=`modprobe_name $module`
    action "Loading $modname:" \
	modprobe -q -- $module
    RETVAL=$?
    return $RETVAL
}

modprobe_remove () {
    module=$1
    shift
    modname=`modprobe_name $module`
    action "Unloading $modname:" \
	modprobe -q -r -- $module
    RETVAL=$?
    return $RETVAL
}

#
# remove_depends() - A recursive function that attempts to remove all dependent
#   modules starting at a target module and finally removing the target module.
#   When any of the removals fail, the function escapes with a non-zero return
#   value.
#
remove_depends () {
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
# start_update() - Checks that kernel modules have been appropriately modified
#   for the booting kernel before inserting the 'specfs' kernel module.  Note
#   that this must be run regardless of the presence or absence of any given
#   kernel module in the booting kernel.  The existing kernel modules may have
#   to be relinked to match a booting kernel with the same ABI as an overwritten
#   kernel.  This is the case for Debian, Ubuntu, SuSE and OpenSuSE, but not, it
#   seems, RedHat or Fedora.  Note that this should really only be run once on
#   kernel boot, and not any time that the Special Filesystem is restarted, so
#   the 'runlevel' and 'previous' environment variables are consulted to ensure
#   that we are starting from boot (and not just switching run levels or
#   manually invoked).
#
start_update () {
    RETVAL=0
    if [ ":$previous" = 'N' -a ":$runlevel" != ':' ] ; then
	if [ ":$SPECFS_KUPDATE" = ':yes' -o -e /.openss7_update ] ; then
	    for command in /sbin/openss7-modules /usr/sbin/openss7-modules ; do
		if [ -x $command ] ; then
		    action "Updating kernel modules" \
			$command -- -q --boot-kernel
		    RETVAL=$?
		    if [ $RETVAL -eq 0 ] ; then
			rm -f -- /.openss7_update
		    fi
		    break
		fi
	    done
	fi
    fi
    return $RETVAL
}

#
# stop_update() - Checks that kernel modules have been appropriately modified
#   for the running kernel before shutting down.  Note that this need only be
#   run when updates have been marked.  The existing kernel modules may have to
#   be relinked to match a rebooting kernel with the same ABI as an overwritten
#   kernel.  This is the case for Debian, Ubuntu, SuSE and OpenSuSE, but not, it
#   seems, RedHat, CentOS nor Fedora.  Note that this sould really only be run
#   once on kernel shutdown or restart, and not any time that the Special
#   Filesystem is stopped, so the 'runlevel' and 'previous' environment
#   variables are consulted to ensure that we are shutting down or rebooting
#   (and not just switching run levels or manually invoked).
#
stop_update () {
    RETVAL=0
    if [ ":$previous" != 'N' -a \( ":$runlevel" = ':0' -o ":$runlevel" = ':6' ]
    then
	if [ ":$SPECFS_KUPDATE" = ':yes' ]
	then :
	    if [ -e /.openss7_update-@kversion@ ]; then
		for command in /sbin/openss7-modules /usr/sbin/openss7-modules
		do
		    [ -x $command ] || continue
		    action "Updating kernel modules" \
			$command -- -q --boot-kernel
		    RETVAL=$?
		    if [ $RETVAL -eq 0 ]; then
			rm -f -- /.openss7_update-@kversion@
		    fi
		    break
		done
	    fi
	fi
    fi
    :
}

#
# start_specfs() - Inserts the 'specfs' kernel module.  This is done even when
#   we are not mounting the specfs filesystem: the specfs filesystem is kernel
#   mounted and can still be accessed through external character devices.
#
start_specfs () {
    if ! grep '^specfs[[:space:]]' /proc/modules >/dev/null 2>&1 ; then
	modprobe_action 'specfs' || RETVAL=$?
    fi
    return $RETVAL
}

#
# reload_specfs() - Simply ensures that the 'specfs' kernel module is loaded, so
#   it simply call start_specfs().
#
reload_specfs () {
    start_specfs || RETVAL=$?
    return $RETVAL
}

probe_specfs () {
    # check that specfs is loaded (and filesystem available)
    if ! grep '^specfs[[:space:]]' /proc/modules >/dev/null 2>&1 ; then
	# specfs is not loaded
	if [ ! -f $lockfile ] ; then echo 'start'; else echo 'reload'; fi
    else
	if ! grep -qc '[[:space:]]specfs\>' /proc/filesystems >/dev/null 2>&1 ; then
	    # filesystem is not present even though specfs module loaded
	    if [ -f $lockfile ] ; then echo 'stop'; fi
	fi
    fi
    return 0
}

status_specfs () {
    local RESULT=0
    if ! grep '^specfs[[:space:]]' /proc/modules 2>&1 ; then
	RESULT=1
    fi
    return $RESULT
}

#
# stop_specfs() - Attempts to remove the 'specfs' kernel module.  This can only
#   be successful when the specfs filesystem is unmounted.  This also attempts
#   to remove any dependent modules.
#
stop_specfs () {
    if [ ":$SPECFS_UMOUNTSPECFS" = ':yes' ] ; then
	remove_depends 'specfs' || RETVAL=$?
    fi
    return $RETVAL
}

#
# start_mountpoint() - Attempts to create all of the mount point directories
#   specified when the script is set to mount the specfs filesystem at those
#   points.
#
start_mountpoint () {
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if [ ! -d "$mountpoint" ] ; then
		action "Creating mount point $mountpoint: " \
		    mkdir -p -- "$mountpoint" \
		    || RETVAL=$?
	    fi
	done
    fi
    return $RETVAL
}

#
# reload_mountpoint() - Simply ensures that all of the specified mount points
#   exist when the script is configured to mount the specfs filesystem at those
#   points.  It simply calls start_mountpoint().
#
reload_mountpoint () {
    start_mountpoint || RETVAL=$?
    return $RETVAL
}

probe_mountpoint () {
    # check that all the mount points exist
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if [ ! -d $mountpoint ] ; then
		if [ ! -f $lockfile ] ; then echo 'start'; else echo 'reload'; fi
		break
	    fi
	done
    fi
    return 0
}

status_mountpoint () {
    local RESULT=0
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if [ -d $mountpoint ] ; then
		ls -ld "$mountpoint"
	    else
		RESULT=1
	    fi
	done
    fi
    return $RESULT
}

#
# stop_mountpoint() - Attempts to remove all of the mount point directories
#   specified when the script is set to unmount the specfs filesystem from those
#   mount points.
#
stop_mountpoint () {
    if [ ":$SPECFS_UMOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if [ -d "$mountpoint" ] ; then
		action "Removing mount point $mountpoint: " \
		rmdir -- "$mountpoint" \
		|| RETVAL=$?
	    fi
	done
    fi
    return $RETVAL
}

#
# start_mount() - Attempts to mount the specfs filesystem on all of the
#   specified mount points and with the specified options.
#
start_mount () {
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	build_options
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if ! mount | grep " on $mountpoint type specfs" >/dev/null 2>&1 ; then
		action "Mounting SPECFS filesystem on $mountpoint: " \
		    mount -t specfs ${SPECFS_OPTIONS} -- specfs "$mountpoint" \
		    || RETVAL=$?
	    fi
	done
    fi
    return $RETVAL
}

#
# reload_mount() - Attempts to unmount and remove the directory for any mount
#   point that is no longer specified but has a specfs filesystem mounted at
#   that point.  It also remounts any mount point that is specified and
#   currently mounted so that mount parameters are updated.  It then uses
#   start_mount() to ensure that a mount point and mount exists for all the
#   specified mount points.
#
reload_mount () {
    local device mountpoint fstype options
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' -a -f /etc/mtab ] ; then
	build_options remount
	while read device mountpoint fstype options; do
	    if [ ":$fstype" = ':specfs' ] ; then
		case " $SPECFS_MOUNTPOINT " in
		(*" $mountpoint "*)
		    action "Remounting SPECFS on $mountpoint: " \
			mount -t specfs ${SPECFS_OPTIONS} -- specfs "$mountpoint" \
			|| RETVAL=$?
		    continue
		    ;;
		esac
		action "Unmounting SPECFS filesystem from $mountpoint: " \
		    umount -t specfs -- "$mountpoint" \
		    || RETVAL=$?
		action "Removing mount point $mountpoint: " \
		    rmdir -- "$mountpoint" \
		    || RETVAL=$?
	    fi
	done </etc/mtab
    fi
    start_mount || RETVAL=$?
    return $RETVAL
}

probe_mount () {
    local device mountpoint fstype options
    # check that specfs is mounted in the right place
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if ! grep "^[^[:space:]]* $mountpoint specfs " /etc/mtab >/dev/null 2>&1 ; then
		if [ -f $lockfile ] ; then echo 'reload'; else echo 'start'; fi
		return 0
	    fi
	done
    fi
    # check that specfs is not mounted in the wrong place
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' -a -f /etc/mtab ] ; then
	while read device mountpoint fstype options; do
	    if [ ":$fstype" = ':specfs' ] ; then
		case " $SPECFS_MOUNTPOINT " in
		    (*" $mountpoint "*) continue ;;
		esac
		if [ -f $lockfile ] ; then echo 'reload'; else echo 'restart'; fi
		return 0
	    fi
	done </etc/mtab
    fi
    return 0
}

status_mount () {
    local RESULT=0
    if [ ":$SPECFS_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $SPECFS_MOUNTPOINT ; do
	    if ! grep "^[^[:space:]]* $mountpoint specfs " /etc/mtab 2>&1 ; then
		RESULT=1
	    fi
	done
    fi
    return $RESULT
}

#
# stop_mount() - Attempts to unmount the specfs filesystem from wherever it is
#   mounted (in user space).  The filesystem should unmount regardless of
#   whether a Stream is open or not.  Failure to unmount a mount point is an
#   error.  The only thing that could cause a failure is a busy filesystem (i.e.
#   some process has a subdirectory as the current working directory).  Note
#   that we should always unmount the filesystems when performing a forced
#   remove so that killed processes cannot reopen drivers.
#
stop_mount () {
    if [ ":$SPECFS_UMOUNTSPECFS" = ':yes' -o ":$SPECFS_FORCEREMOVE" = ':yes' ] ; then
	if grep '^[^[:space:]]* [^[:space:]]* specfs ' /etc/mtab >/dev/null 2>&1 ; then
	    action "Unmounting SPECFS filesystem: " \
		umount -a -t specfs \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

#
# start_preload() - Used to insert all preload specified modules.  Typically for
#   specfs this is the 'specfs' module itself.  The 'specfs' module must be
#   loaded before sysctl parameters can be configured and before the specfs
#   filesystem can be mounted.  On some systems it is possible to identify this
#   module as a system preloaded module, but on others the module is only loaded
#   when the initscript is run.
#
start_preload () {
    # insert in forward order
    modules=
    for module in $SPECFS_PRELOAD ; do
	modules="$modules${modules:+ }$module"
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
# reload_preload() - Used when reloading.  The only necessity is to ensure that
#   preloaded modules are present to be reconfigured, so start_preload() is
#   simply called.
#
reload_preload () {
    start_preload || RETVAL=$?
    return $RETVAL
}

probe_preload () {
    # check that preloads are loaded
    for module in $SPECFS_PRELOAD ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if ! grep "^$modrex[[:space:]]" /proc/modules >/dev/null 2>&1 ; then
	    # a preload is not loaded
	    if [ ! -f $lockfile ] ; then echo 'start'; else echo 'reload'; fi
	    break
	fi
    done
    return 0
}

status_preload () {
    local RESULT=0
    for module in $SPECFS_PRELOAD ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if ! grep "^$modrex[[:space:]]" /proc/modules 2>&1 ; then
	    RESULT=1
	fi
    done
    return $RESULT
}

#
# stop_preload() - Used to remove specified preloaded modules.  Typically for
#   specfs this would include the 'specfs' module itself.  This function also
#   attempts to remove all dependent modules.  Note that for specfs the specfs
#   filesystem must be unmounted before attempting this or it wil fail.
#
stop_preload () {
    # remove in reverse order
    modules=
    for module in $SPECFS_PRELOAD ; do
	modules="$module${modules:+ }$modules"
    done
    for module in $modules ; do
	remove_depends $module || ( RETVAL=$? ; break )
    done
    return $RETVAL
}

#
# start_params() - Used to configure sysctl parameters associated with preloaded
#   STREAMS modules and drivers, and, in particular the 'streams' kernel module.
#
start_params () {
    if grep '^[[:space:]]*'${name}'[/.]' /etc/sysctl.conf >/dev/null 2>&1 ; then
	action "Reconfiguring kernel parameters: " \
	    sysctl -q -e -p /etc/sysctl.conf 2>/dev/null || :
    fi
    if [ -f /etc/${name}.conf ] ; then
	action "Configuring $ucname parameters: " \
	    sysctl -q -e -p /etc/${name}.conf 2>/dev/null || :
    fi
    return $RETVAL
}

reload_params () {
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

probe_params () {
    # check update of config file
    if [ -f $lockfile ] ; then
	for file in $config /etc/sysctl.conf /etc/${name}.conf ; do
	    if [ -f $file -a $file -nt $lockfile ] ; then
		# config file update, need to reload
		echo 'reload'
		break;
	    fi
	done
    fi
    return 0
}

status_params () {
    return 0
}

#
# stop_params() - Used to deconfigure sysctl parameters associated with
#   preloaded STREAMS modules and drivers, and, in particular the 'streams'
#   kernel module.  There are no actions required to deconfigure sysctl
#   parameters.
#
stop_params () {
    return $RETVAL
}

#
# start_devices() - Used to configure devices associated with the STREAMS
#   subsystem.
#
start_devices () {
    if [ ":$SPECFS_MAKEDEVICES" = ':yes' ] ; then
	if [ -n "$SPECFS_MKDEV" -a -x "$SPECFS_MKDEV" ] ; then
	    action "Making $ucname devices: " \
		$SPECFS_MKDEV --create \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

reload_devices () {
    if [ ":$SPECFS_MAKEDEVICES" = ':yes' ] ; then
	if [ -n "$SPECFS_MKDEV" -a -x "$SPECFS_MKDEV" ] ; then
	    action "Making $ucname devices: " \
		$SPECFS_MKDEV --create \
		|| RETVAL=$?
	fi
    else
	if [ -n "$SPECFS_MKDEV" -a -x "$SPECFS_MKDEV" ] ; then
	    action "Removing $ucname devices: " \
		$SPECFS_MKDEV --remove \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

probe_devices () {
    return 0
}

status_devices () {
    return 0
}

#
# stop_devices() - Used to deconfigure devices associated with the STREAMS
#   susbsystem.
#
stop_devices () {
    if [ ":$SPECFS_REMOVEDEVICES" = ':yes' ] ; then
	if [ -n "$SPECFS_MKDEV" -a -x "$SPECFS_MKDEV" ] ; then
	    action "Removing $ucname devices: " \
		$SPECFS_MKDEV --remove \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

#
# start_modules() - Used to insert kernel modules at startup.  Because modules
#   are autoloaded as required, there is nothing to do here.
#
start_modules () {
    return $RETVAL
}

reload_modules () {
    start_modules
    return $RETVAL
}

probe_modules () {
    return 0
}

status_modules () {
    return 0
}

#
# stop_modules() - Gets rid of any modules using the specfs kernel module or
#   using the specfs file system.  We have an advantage here over that used by
#   the streams init script in that devices have been removed at this point and
#   we can unmount the specfs filesystem before calling this function.  This
#   means that any user processes that may attempt to reopen a stream will fail
#   because there are no devices available to open.
#
stop_modules () {
    local module more
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
		    echo "Sending STREAMS processes the $signal signal..."
		    # COMMAND PID USER FD TYPE DEVICE SIZE NODE NAME...
		    lsof | grep '\<STR\>' | \
		    while read fields ; do
			set -- $fields
			if [ ":$5" != ':CHR' ] ; then continue; fi
			ind=`expr $# - 1`
			if eval "[ \":\${$ind}\" != ':STR' ]" ; then continue; fi
			echo "Killing $1($2) for device $9"
			eval "kill -$signal $2"
		    done
		    # sleep 5 sec to allow killed process to complete
		    echo "Waiting for 5 seconds..."
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
    while read module more; do
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
	modules="${module}${modules:+ }$modules"
    done
    for module in $modules ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if grep "^$modrex[[:space:]]" /proc/modules >/dev/null 2>&1 ; then
	    modprobe_remove $module 2>/dev/null || :
	fi
    done
    return $RETVAL
}

start () {
    start_update	# check kernel module weak updates
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

stop () {
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

restart () {
    stop  || RETVAL=$?
    start || RETVAL=$?
    return $RETVAL
}

reload () {
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

show () {
    echo "$script: show: not yet implemented." >&2
    RETVAL=1
    return $RETVAL
}

usage () {
    echo "Usage: $script (start|stop|status|restart|try-restart|condrestart|force-reload|reload|probe|show)" >&2
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
    (status)
	RESULT='running'
	status_specfs     || RESULT='stopped'
	status_mountpoint || RESULT='stopped'
	status_mount      || RESULT='stopped'
	status_preload    || RESULT='stopped'
	status_params     || RESULT='stopped'
	status_modules    || RESULT='stopped'
	status_devices    || RESULT='stopped'
	if [ ":$RESULT" = ':running' ] ; then
	    if [ -f $lockfile ] ; then RETVAL=0; else RETVAL=1; fi
	else
	    if [ -f $lockfile ] ; then RETVAL=3; else RETVAL=2; fi
	fi
	case $RETVAL in
	    (0) echo "$name is running" ;;
	    (1) echo "$name is running but subsys unlocked" ;;
	    (2) echo "$name is stopped" ;;
	    (3) echo "$name is stopped by subsys locked" ;;
	    (*) echo "$name status unknown" ;;
	esac
	;;
    (try-restart|condrestart)
	[ -f $lockfile ] && restart || RETVAL=$?
	;;
    (probe)
	RESULT=
	[ -n "$RESULT" ] || RESULT=`probe_specfs`
	[ -n "$RESULT" ] || RESULT=`probe_mountpoint`
	[ -n "$RESULT" ] || RESULT=`probe_mount`
	[ -n "$RESULT" ] || RESULT=`probe_preload`
	[ -n "$RESULT" ] || RESULT=`probe_params`
	[ -n "$RESULT" ] || RESULT=`probe_modules`
	[ -n "$RESULT" ] || RESULT=`probe_devices`
	[ -z "$RESULT" -a ! -f $lockfile ] && RESULT='start'
	[ -n "$RESULT" ] && echo "$RESULT"
	;;
    (*)
	usage || RETVAL=$?
	;;
esac

[ "${0##*/}" = "$script" ] && exit $RETVAL

# =============================================================================
# 
# @(#) $RCSfile: specfs.sh,v $ $Name:  $($Revision: 1.1.2.5 $) $Date: 2011-04-11 06:13:44 $
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
# Last Modified $Date: 2011-04-11 06:13:44 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: specfs.sh,v $
# Revision 1.1.2.5  2011-04-11 06:13:44  brian
# - working up weak updates
#
# Revision 1.1.2.4  2011-02-08 23:39:03  brian
# - last minute release updates
#
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
