#!/bin/sh

KCHECK_SH_VERSION="1.1 9/9/02"

#
# agument parsing
#
arg1()
{
    printf "%s" $1
}
arg2()
{
    printf "%s" $2
}
arg3()
{
    printf "%s" $3
}
arg4()
{
    printf "%s" $4
}
arg5()
{
    printf "%s" $5
}

#
# default_ksrc
#
# Figure out a good default for kernel source directory
# Do not retain any "../" isms in the path name.
#
default_ksrc()
{
    ksrc=/lib/modules/`uname -r`/build
    if [ -L $ksrc ]; then
	ksrc=`ls -l $ksrc | sed -e 's/^.*-> //' -e 's/\.\.\//\//g'`
        if [ -d "$ksrc" -o -L "$ksrc" ]; then
	    echo $ksrc
	else
	    echo "Unknown"
	fi
    elif [ -d /usr/src/linux -o -L /usr/src/linux ]; then
        echo "/usr/src/linux"
    else
        echo "Unknown"
    fi
}

#
# check_ksrc
#
# Check to see if the directory $KSRC is present.  If it is set to
# /usr/src/linux and that directory is not present then feel around
# a bit for a substitute to which a symbolic link can be constructed.
#
check_ksrc()
{
    NOKSRC=0
    if [ -d $KSRC ]; then
	return 0				# present
    fi

    if [ -L $KSRC ]; then
	return 0			# symbolic link, presume present
    fi

    if [ "$KSRC" != "/usr/src/linux" ]; then
	return 1				# not present
    fi

    #
    # pick up version string.  It might be like "2.4.2" or it
    # might be like "2.4.2-21smp".
    #
    vers=`cat /proc/version`
    vers=`arg3 $vers`
    trunc_vers=`echo $vers | sed -e 's/-.*//'`
    num_vers=`echo $vers | sed -e 's/[a-zA-Z][a-zA-Z]*//'`
    if [ -d /usr/src/$vers ]; then
	KSRC=/usr/src/$vers
    elif [ -d /usr/src/linux-$vers ]; then
	KSRC=/usr/src/linux-$vers
    elif [ -d /usr/src/linux$trunc_vers ]; then
	KSRC=/usr/src/linux$trunc_vers
    elif [ -d /usr/src/linux-$trunc_vers ]; then
	KSRC=/usr/src/linux-$trunc_vers
    elif [ -d /usr/src/linux$num_vers ]; then
	KSRC=/usr/src/linux$num_vers
    elif [ -d /usr/src/linux-$num_vers ]; then
	KSRC=/usr/src/linux-$num_vers
    else
	NOKSRC=1
	return 1				# not present
    fi

    return 0					# present
}

#
# Get config variable from config.h/autoconf.h.  If config.h is not present
# or does not have the config variable, try the "include/config" 
# subdirectory.
#
# $1 is the name of the variable to find.
#
# Return via stdout a "y" if defined or "" if not.
#
ac_get_config()
{
    dmy=/tmp/dummy$$.c
    if [ ! -f $dmy ]; then
	echo '/* dummy file to untangle autoconf.h/version.h */' >$dmy
    fi

    if [ -f $KSRC/include/linux/config.h ]; then
	cc -I$KSRC/include -E -dM \
		-include $KSRC/include/linux/config.h $dmy >/tmp/kconfig
	rm -f $dmy 2>/dev/null
	x=`grep $1 /tmp/kconfig 2>&1`
	if [ $? -eq 0 ]; then			# found it
	    if expr "$x" : "#define" >/dev/null 2>/dev/null; then
		echo -n "y"			# option selected
		return 0			# found
	    fi
	    if expr "$x" : "#undef" >/dev/null 2>/dev/null; then
		return 0			# found, but empty
	    fi
	fi
	#
	# If autoconf.h is present but our option is not mentioned
	# in a #define, then it must be in a #undef.  So assume
	# that the option is "found" but not set.
	#
	return 0				# found, but empty
    fi
						# no autoconf.h
    return 1					# not found
}

#
# Check for places that version.h may be hiding.  Try to establish it
# in the kernel source tree where it belongs.
#
check_kver()
{
    #
    # SuSE 7.2 lacks a version.h in the kernel source tree but has one
    # in /boot -- presumably paired with the running kernel version.
    # We will attempt to copy it if it is there and not care whether or
    # not the copy fails.  If we need a version.h in kernel source and
    # it isn't there then we will just write a message and terminate the
    # install.
    #
    if [    ! -f $KSRC/include/linux/version.h \
    	 -a -f /boot/vmlinuz.version.h \
	 -a -d $KSRC/include/linux \
       ]
    then
	cp /boot/vmlinuz.version.h $KSRC/include/linux/version.h \
		>/dev/null 2>/dev/null
    fi
}

#
# Figure out kernel version
# Return the version string in KVER (e.g. 2.2.6)
#
get_kver()
{
    check_kver
    if [ -f $KSRC/include/linux/version.h ]; then
	dmy=/tmp/dummy$$.c
	if [ ! -f $dmy ]; then
	    echo '/* dummy file to untangle autoconf.h/version.h */' >$dmy
	fi

	cc -I$KSRC/include -E -dM \
	    -include $KSRC/include/linux/version.h $dmy >/tmp/kver
	rm -f $dmy 2>/dev/null
	KVER=`grep UTS_RELEASE /tmp/kver | \
	     sed -e 's/^.*UTS_RELEASE.//' -e's/"//g' -e's/  *$//'`
    else
        echo "The file $KSRC/include/linux/version.h is not present."
        echo "Kernel version checking cannot proceed."
        exit 1
    fi
    # Remove the extra version glarf from the end of kernel version
    NKVER=`echo $KVER | sed -e 's/-.*//'`
}

#
# Get SMP or not
# Return result in KSMP
#
get_smp()
{
    KSMP=`ac_get_config CONFIG_SMP`	# check autoconf.h
    if [ $? -eq 0 ]; then		# found
	: OK
    else
	echo "The option CONFIG_SMP cannot be found in the kernel\'s"
	echo "config.h file.  Kernel version checking cannot proceed."
	exit 1
    fi

    if [ "$KSMP..." = "y..." ]; then
	echo "Kernel support for SMP detected."
    else
	echo "No kernel support for SMP detected."
    fi
}

#
# Get Modules or not
# Return result in KMODULES
#
get_modules()
{
    KMODULES=`ac_get_config CONFIG_MODULES`	# check autoconf.h
    if [ $? -eq 0 ]; then			# found
	KMODVERS=`ac_get_config CONFIG_MODVERSIONS`
    else
	echo "The option CONFIG_MODULES cannot be found in the kernel\'s"
	echo "config.h file.  Kernel version checking cannot proceed."
	exit 1
    fi

    if [ "$KMODVERS..." = "y..." ]; then
	echo "Kernel support for mod-versions detected."
    else
	echo "No kernel support for mod-versions detected."
    fi
}

#
# Having dug out the SMP setting from the kernel source info, now
# verify that the currently running kernel has the same settings.
#
smp_error()
{
    echo
    echo "Your kernel source indicates SMP=$XKSMP, but your running"
    echo "kernel appears to have SMP=$NKSMP.  This is a good indication"
    echo "that you are not running on the kernel represented"
    echo "by the kernel source."
    exit 1
}

verify_smp()
{
    if [ "$KSMP..." = "y..." ]; then		# compute "not" SMP
	XKSMP=y
	NKSMP=n
    else
	XKSMP=n
	NKSMP=y
    fi

    export NKSMP XKSMP
    if [ ! -f /proc/ksyms ]; then
	echo
	echo "The kernel SMP option cannot be verified because there"
	echo "is no /proc/ksyms present on the system.  Kernel version"
	echo "checking cannot proceed."
	exit 1
    fi

    if grep -q smp_num_cpus /proc/ksyms; then
	# running kernel has SMP set
	if [ $XKSMP = "n" ]; then	# kernel source does not
	    smp_error
	fi
    else
					# running kernel does not have SMP set
	if [ $XKSMP = "y" -a "$MACHINE..." != "s390..." ]
	then				# kernel source does
	    smp_error
	fi
    fi
    echo "SMP option verified between kernel source and running kernel."
}

#
# Having dug out the kernel version from the kernel source info, now
# verify that the currently running kernel has the same settings.
#
verify_kver()
{
    if [ ! -f /proc/sys/kernel/osrelease ]; then
	echo
	echo "The kernel version cannot be verified because there"
	echo "is no /proc/sys/kernel/osrelease present on the system."
	exit 1
    fi

    PKVER=`cat /proc/sys/kernel/osrelease`
    NPKVER=`echo $PKVER | sed -e 's/-.*//'`
    if [ "$NKVER" != "$NPKVER" ]; then
	echo
	echo "Your kernel source is for kernel version $NKVER, but"
	echo "your running kernel is version $NPKVER.  This is a"
	echo "good indication that you are not running on the kernel"
	echo "represented by the kernel source.  Kernel version"
	echo "checking cannot proceed."
	exit 1
    fi
    echo "Kernel version verified between kernel source and running kernel."
}

#
# Having dug out the mod-versions option from the kernel source info, now
# verify that the currently running kernel has the same settings.
#
modver_error()
{
    echo
    echo "Your kernel source indicates MODVERSIONS=$XKMODVERS, but your running"
    echo "kernel appears to have MODVERSIONS=$NKMODVERS.  This is a good indication"
    echo "that you are not running on the kernel represented"
    echo "by the kernel source.  Kernel version checking cannot proceed."
    exit 1
}

verify_modver()
{
    if [ "$KMODVERS..." = "y..." ]; then		# compute "not" SMP
	XKMODVERS=y
	NKMODVERS=n
    else
	XKMODVERS=n
	NKMODVERS=y
    fi

    export NKMODVERS XKMODVERS
    if [ ! -f /proc/ksyms ]; then
	echo
	echo "The kernel module versions option cannot be verified because"
	echo "there is no /proc/ksyms present on the system."
	exit 1
    fi

    if grep -q schedule_R /proc/ksyms; then
	# running kernel has mod-versions set
	NKMODVERS=y
	if [ $XKMODVERS = "n" ]; then	# kernel source does not
	    modver_error
	fi
    else
	# running kernel does not have mod-versions set
	NKMODVERS=n
	if [ $XKMODVERS = "y" ]; then	# kernel source does
	    modver_error
	fi
    fi

    if [ $NOKSRC -eq 0 ]; then
	if [ -f $KSRC/include/linux/modversions.h -a \
	     -f $KSRC/include/linux/modules/ksyms.ver ]; then
	    # kernel source is set up for mod versions in compiles
	    XKMODVERS=y
	    if [ $NKMODVERS = "n" ]; then
		modver_error
	    fi
	else
	    # kernel source is not set up for mod versions in compiles
	    XKMODVERS=n
	    if [ $NKMODVERS = "y" ]; then
		modver_error
	    fi
	fi
    fi

    echo "MODVERS option verified between kernel source and running kernel."
}

main()
{
    KSRC=`default_ksrc`
    check_ksrc
    if [ $? -eq 0 ]; then
	echo "Kernel source located in $KSRC."
    else
	echo "No kernel source found.  It is not possible to compile"
	echo "kernel drivers without at least a minimal kernel source tree."
	exit 1
    fi

    get_kver
    get_smp
    get_modules
    verify_kver
    verify_smp
    verify_modver
}

echo "kcheck.sh version $KCHECK_SH_VERSION"
main
