#!/bin/sh
#
# @(#) $RCSfile: openss7.sh,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-05-10 13:45:37 $
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
# openss7:	start and stop openss7 subsystem
# update-rc.d:	start 35 S . stop 35 0 6 .
# lockfile:	/var/lock/subsys/openss7
# config:	/etc/default/openss7
# probe:	true
# hide:		false
# license:	AGPL
# description:	This OpenSS7 init script is part of Linux Fast-STREAMS.  It is \
#		responsible for ensuring that the necessary STREAMS character \
#		devices are present in the /dev directory and that the OpenSS7 \
#		subsystem is configured and loaded.
#
# LSB init script conventions
#
### BEGIN INIT INFO
# Provides: openss7
# Required-Start: streams $network
# Required-Stop: streams $network
# Should-Start: strerr strace
# Should-Stop: strerr strace
# Default-Start: S
# Default-Stop:
# X-UnitedLinux-Default-Enabled: yes
# Short-Description: start and stop STREAMS OpenSS7 Devices
# License: AGPL
# Description:	This OpenSS7 init script is part of Linux Fast-STREAMS.  It is
#	responsible for ensuring that the necessary OpenSS7 character devices
#	are present in the /dev directory and that the OpenSS7 subsystem is
#	configured and loaded.
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

name='openss7'
script='openss7.sh'
ucname="OPENSS7"
lockfile="/var/lock/subsys/$name"
config="/etc/sysconfig/$name /etc/default/$name"
mkdev="${name}_mkdev"
desc="the OPENSS7 subsystem"

if [ ! -e /proc/modules     ] ; then if [ "$1" = 'stop' ] ; then exit 0 ; else exit 5 ; fi ; fi
if [ ! -e /proc/filesystems ] ; then if [ "$1" = 'stop' ] ; then exit 0 ; else exit 5 ; fi ; fi

# Specify defaults

[ -n "$OPENSS7_KUPDATE"       ] || OPENSS7_KUPDATE='no'
[ -n "$OPENSS7_BOOTLOAD"      ] || OPENSS7_BOOTLOAD='no'
[ -n "$OPENSS7_BOOTMOUNT"     ] || OPENSS7_BOOTMOUNT='no'

[ -n "$OPENSS7_MOUNTSPECFS"   ] || OPENSS7_MOUNTSPECFS='yes'
[ -n "$OPENSS7_UMOUNTSPECFS"  ] || OPENSS7_UMOUNTSPECFS='yes'
[ -n "$OPENSS7_MOUNTPOINT"    ] || OPENSS7_MOUNTPOINT='/dev/streams'
[ -n "$OPENSS7_UID"           ] || OPENSS7_UID='0'
[ -n "$OPENSS7_GID"           ] || OPENSS7_GID='0'
[ -n "$OPENSS7_MODE"          ] || OPENSS7_MODE='0777'
[ -n "$OPENSS7_OPTIONS"       ] || OPENSS7_OPTIONS='-o uid=0,gid=0,mode=0777'

[ -n "$OPENSS7_PRELOAD"       ] || OPENSS7_PRELOAD=
[ -n "$OPENSS7_DRIVERS"       ] || OPENSS7_DRIVERS='streams-ip_to_dlpi streams-ldl streams-np_ip streams-np_udp streams-x25-lapb streams-x25-llc2 streams-x25-plp streams-inet streams-rawip streams-udp streams-tcp streams-sctp streams-clns streams-tp streams-socksys streams-ua_as streams-m2ua_as streams-m3ua_as streams-sua_as streams-tua_as streams-isua_as streams-atm streams-tstdrv streams-sl-x400p streams-x100p-ss7 streams-x400p-ss7 streams-sl_x100p streams-sl_x400p streams-sdlm streams-sl_min streams-sl_mux streams-mtp streams-mtp_min streams-sccp streams-tcap streams-isup'
[ -n "$OPENSS7_MODULES"       ] || OPENSS7_MODULES='streams-ip_strm_mod streams-timod streams-tirdwr streams-tpiperf streams-mstr streams-cmot streams-isot streams-itot streams-lpp streams-tcpns streams-xot streams-sockmod streams-m2pa_sl streams-sdl_sctp streams-sdt_sctp streams-sdt_tpi streams-sl_tpi streams-sl_sctp streams-m2tp streams-phys streams-aal1 streams-aal2 streams-aal5 streams-sscop streams-sscopmce streams-mtp3b streams-tstmod streasm-cdpmod streams-ch_pmod streams-mx_pmod streams-sccp_mod streams-npi_sccp streams-tpi_sccp streams-spm streams-sdl_pmod streams-sdt_pmod streams-slpmod streams-sm_mod streams-tc_mod streams-tr_mod streams-sdl_ch streams-mtp_npi streams-mtp_tpi streams-sdl streams-sdt streams-sl streams-dlaix streams-tpi_conv streams-npi_conv streams-dlpi_conv streams-cdi_conv'
[ -n "$OPENSS7_MAKEDEVICES"   ] || OPENSS7_MAKEDEVICES='yes'
[ -n "$OPENSS7_REMOVEDEVICES" ] || OPENSS7_REMOVEDEVICES='yes'
[ -n "$OPENSS7_FORCEREMOVE"   ] || OPENSS7_FORCEREMOVE='no'

# Source redhat and/or debian config file
for file in $config ; do
    [ -f $file ] && . $file
done

if [ -z "$OPENSS7_MKDEV" -o ! -x "$OPENSS7_MKDEV" ] ; then
    OPENSS7_MKDEV=
    for prog in ./${mkdev} /sbin/${mkdev} /usr/sbin/${mkdev} /bin/${mkdev} /usr/bin/${mkdev} ; do
	if [ -x $prog ] ; then
	    OPENSS7_MKDEV=$prog
	    break
	fi
    done
fi

[ -z "$OPENSS7_MKDEV" ] && OPENSS7_MAKEDEVICES="no"
[ -z "$OPENSS7_MKDEV" ] && OPENSS7_REMOVEDEVICES="no"

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
    OPENSS7_OPTIONS=
    [ -n "$OPENSS7_UID" ] && \
	OPENSS7_OPTIONS="${OPENSS7_OPTIONS}${OPENSS7_OPTIONS:--o }${OPENSS7_OPTIONS:+,}uid=${OPENSS7_UID}"
    [ -n "$OPENSS7_GID" ] && \
	OPENSS7_OPTIONS="${OPENSS7_OPTIONS}${OPENSS7_OPTIONS:--o }${OPENSS7_OPTIONS:+,}gid=${OPENSS7_GID}"
    [ -n "$OPENSS7_MODE" ] && \
	OPENSS7_OPTIONS="${OPENSS7_OPTIONS}${OPENSS7_OPTIONS:--o }${OPENSS7_OPTIONS:+,}mode=${OPENSS7_MODE}"
    [ "$1" = remount ] && \
	OPENSS7_OPTIONS="${OPENSS7_OPTIONS}${OPENSS7_OPTIONS:--o }${OPENSS7_OPTIONS:+,}remount"
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
# remove_depends() is a recursive function that attempts to remove all dependent
# modules starting at a target module and finally removing the target module.
# When any of the removals fail, the function escapes with a non-zero return
# value.
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
# start_update() checks that kernel modules have been appropriately modified for
# the booting kernel before inserting the 'specfs' kernel module.  Note that
# this must be run regardless of the presence or absence of any given kernel
# module in the booting kernel.  The existing kernel modules may have to be
# relinked to match a booting kernel with the same ABI as an overwritten kernel.
# This is the case for Debian, Ubuntu, SuSE and OpenSuSE, but not, it seems,
# RedHat or Fedora.  Note that this should really only be run once on kernel
# boot, and not any time that the Special Filesystem is restarted, so the
# 'runlevel' and 'previous' environment variables are consulted to ensure that
# we are starting from boot (and not just switching run levels or manually
# invoked).
#
start_update () {
    RETVAL=0
    if [ ":$previous" = 'N' -a ":$runlevel" != ':' ] ; then
	if [ ":$OPENSS7_KUPDATE" = ':yes' -o -e /.openss7_update ] ; then
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
# start_specfs() inserts the 'specfs' kernel module.  This is done even when we
# are not mounting the specfs filesystem: the specfs filesystem is kernel
# mounted and can still be accessed through external character devices.
#
start_specfs () {
    if ! grep '^specfs[[:space:]]' /proc/modules >/dev/null 2>&1 ; then
	modprobe_action 'specfs' || RETVAL=$?
    fi
    return $RETVAL
}

#
# reload_specfs() simply ensures that the 'specfs' kernel module is loaded, so
# it simply call start_specfs().
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
# stop_specfs() attempts to remove the 'specfs' kernel module.  This can only be
# successful when the specfs filesystem is unmounted.  This also attempts to
# remove any dependent modules.
#
stop_specfs () {
    if [ ":$OPENSS7_UMOUNTSPECFS" = ':yes' ] ; then
	remove_depends 'specfs' || RETVAL=$?
    fi
    return $RETVAL
}

#
# start_mountpoint() attempts to create all of the mount point directories
# specified when the script is set to mount the specfs filesystem at those
# points.
#
start_mountpoint () {
    if [ ":$OPENSS7_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $OPENSS7_MOUNTPOINT ; do
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
# reload_mountpoint() simply ensures that all of the specified mount points
# exist when the script is configured to mount the specfs filesystem at those
# points.  It simply calls start_mountpoint().
#
reload_mountpoint () {
    start_mountpoint || RETVAL=$?
    return $RETVAL
}

probe_mountpoint () {
    # check that all the mount points exist
    if [ ":$OPENSS7_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $OPENSS7_MOUNTPOINT ; do
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
    if [ ":$OPENSS7_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $OPENSS7_MOUNTPOINT ; do
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
# stop_mountpoint() attempts to remove all of the mount point directories
# specified when the script is set to unmount the specfs filesystem from those
# mount points.
#
stop_mountpoint () {
    if [ ":$OPENSS7_UMOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $OPENSS7_MOUNTPOINT ; do
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
# start_mount() attempts to mount the specfs filesystem on all of the specified
# mount points and with the specified options.
#
start_mount () {
    if [ ":$OPENSS7_MOUNTSPECFS" = ':yes' ] ; then
	build_options
	for mountpoint in $OPENSS7_MOUNTPOINT ; do
	    if ! mount | grep " on $mountpoint type specfs" >/dev/null 2>&1 ; then
		action "Mounting SPECFS filesystem on $mountpoint: " \
		    mount -t specfs ${OPENSS7_OPTIONS} -- specfs "$mountpoint" \
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
reload_mount () {
    local device mountpoint fstype options
    if [ ":$OPENSS7_MOUNTSPECFS" = ':yes' -a -f /etc/mtab ] ; then
	build_options remount
	while read device mountpoint fstype options; do
	    if [ ":$fstype" = ':specfs' ] ; then
		case " $OPENSS7_MOUNTPOINT " in
		(*" $mountpoint "*)
		    action "Remounting SPECFS on $mountpoint: " \
			mount -t specfs ${OPENSS7_OPTIONS} -- specfs "$mountpoint" \
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
    if [ ":$OPENSS7_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $OPENSS7_MOUNTPOINT ; do
	    if ! grep "^[^[:space:]]* $mountpoint specfs " /etc/mtab >/dev/null 2>&1 ; then
		if [ -f $lockfile ] ; then echo 'reload'; else echo 'start'; fi
		return 0
	    fi
	done
    fi
    # check that specfs is not mounted in the wrong place
    if [ ":$OPENSS7_MOUNTSPECFS" = ':yes' -a -f /etc/mtab ] ; then
	while read device mountpoint fstype options; do
	    if [ ":$fstype" = ':specfs' ] ; then
		case " $OPENSS7_MOUNTPOINT " in
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
    if [ ":$OPENSS7_MOUNTSPECFS" = ':yes' ] ; then
	for mountpoint in $OPENSS7_MOUNTPOINT ; do
	    if ! grep "^[^[:space:]]* $mountpoint specfs " /etc/mtab 2>&1 ; then
		RESULT=1
	    fi
	done
    fi
    return $RESULT
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
stop_mount () {
    if [ ":$OPENSS7_UMOUNTSPECFS" = ':yes' -o ":$OPENSS7_FORCEREMOVE" = ':yes' ] ; then
	if grep '^[^[:space:]]* [^[:space:]]* specfs ' /etc/mtab >/dev/null 2>&1 ; then
	    action "Unmounting SPECFS filesystem: " \
		umount -a -t specfs \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

#
# start_preload() is used to insert all preload specified modules.  Typically
# for openss7 this is some short list of modules.  These modules must be loaded
# before sysctl parameters can be configured.
# On some systems it is possible to identify this module as a system preloaded
# module, but on others the module is only loaded when the initscript is run.
#
start_preload () {
    # insert in forward order
    modules=
    for module in $OPENSS7_PRELOAD ; do
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
# reload_preload() is used when reloading.  The only necessity is to ensure that
# preloaded modules are present to be reconfigured, so start_preload() is simply
# called.
#
reload_preload () {
    start_preload || RETVAL=$?
    return $RETVAL
}

probe_preload () {
    # check that preloads are loaded
    for module in $OPENSS7_PRELOAD ; do
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
    for module in $OPENSS7_PRELOAD ; do
	modrex=`echo $module | sed -e 's,[-_],[-_],g'`
	if ! grep "^$modrex[[:space:]]" /proc/modules 2>&1 ; then
	    RESULT=1
	fi
    done
    return $RESULT
}

#
# stop_preload() is used to remove specified preloaded modules.  Typically for
# openss7 this would include a list of preloaded modules.  This function also
# attempts to remove all dependent modules.  Note that for openss7 all STREAMS
# devices must be closed and unlinked or this could fail.
#
stop_preload () {
    # remove in reverse order
    modules=
    for module in $OPENSS7_PRELOAD ; do
	modules="$module${modules:+ }$modules"
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
# stop_params() is used to deconfigure sysctl parameters associated with
# preloaded STREAMS modules and drivers, and, in particular the 'streams' kernel
# module.  There are no actions required to deconfigure sysctl parameters.
#
stop_params () {
    return $RETVAL
}

#
# start_devices() is used to configure devices associated with the STREAMS
# subsystem.
#
start_devices () {
    if [ ":$OPENSS7_MAKEDEVICES" = ":yes" ] ; then
	if [ -n "$OPENSS7_MKDEV" -a -x "$OPENSS7_MKDEV" ] ; then
	    action "Making $ucname devices: " \
		$OPENSS7_MKDEV --create \
		|| RETVAL=$?
	fi
    fi
    return $RETVAL
}

reload_devices () {
    if [ ":$OPENSS7_MAKEDEVICES" = ':yes' ] ; then
	if [ -n "$OPENSS7_MKDEV" -a -x "$OPENSS7_MKDEV" ] ; then
	    action "Making $ucname devices: " \
	    $OPENSS7_MKDEV --create \
	    || RETVAL=$?
	fi
    else
	if [ -n "$OPENSS7_MKDEV" -a -x "$OPENSS7_MKDEV" ] ; then
	    action "Removing $ucname devices: " \
	    $OPENSS7_MKDEV --remove \
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
# stop_devices() is used to deconfigure devices associated with the STREAMS
# susbsystem.
#
stop_devices () {
    if [ ":$OPENSS7_REMOVEDEVICES" = ":yes" ] ; then
	if [ -n "$OPENSS7_MKDEV" -a -x "$OPENSS7_MKDEV" ] ; then
	    action "Removing $ucname devices: " \
	    $OPENSS7_MKDEV --remove \
	    || RETVAL=$?
	fi
    fi
    return $RETVAL
}

#
# start_modules() is used to insert kernel modules at startup.  Because modules
# are autoloaded as required, there is nothing to do here.
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
# stop_modules() is used to remove kernel modules at shutdown of the STREAMS
# subsystem.  There are two ways to do this: with or without extreme prejudice.
# The order of removal can be gleened from lsmod: the streams modules that are
# higher on the list have the least dependencies.  Also, when we use the '-r'
# option to modprobe, complete unused module stacks will be removed.
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
    if [ ":$OPENSS7_FORCEREMOVE" = ':yes' ] ; then
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
	    case " $OPENSS7_DRIVERS $OPENSS7_MODULES " in
		(*" $modrex "*)
		    modprobe_remove $module 2>/dev/null || :
		    ;;
	    esac
	fi
    done </proc/modules
    # try to remove modules that we know about in reverse order
    modules=
    for module in $OPENSS7_DRIVERS $OPENSS7_MODULES ; do
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
# @(#) $RCSfile: openss7.sh,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-05-10 13:45:37 $
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
# Last Modified $Date: 2011-05-10 13:45:37 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: openss7.sh,v $
# Revision 1.1.2.4  2011-05-10 13:45:37  brian
# - weak modules workup
#
# Revision 1.1.2.3  2011-01-12 00:19:32  brian
# - modprobe no longer accepts k option
#
# Revision 1.1.2.2  2009-07-24 13:49:46  brian
# - updates for release build
#
# =============================================================================

# vim: ft=sh sw=4 tw=80
