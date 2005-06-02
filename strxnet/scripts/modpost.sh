#!/bin/bash
# =============================================================================
# 
# @(#) $RCSfile: modpost.sh,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/06/02 03:05:12 $
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
# Last Modified $Date: 2005/06/02 03:05:12 $ by $Author: brian $
#
# =============================================================================

# 2.6 Kernel modpost ala bash

# Note that modules changed in 2.5.48.

ECHO='echo'
SHELL=${CONFIG_SHELL:-/bin/sh}
SED='sed'

# Check that we have a working $ECHO.
if test "X$1" = X--no-reexec; then
    # Discard the --no-reexec flag, and continue
    shift
elif test "X`($ECHO '\t')` 2>/dev/null " = 'X\t'; then
    :
else
    exec $SHELL "$0" --no-reexec ${1+"$@"}
fi

if test "X$1" = X--fallback-echo ; then
    shift
    cat <<EOF
$*
EOF
    exit 0
fi

program=`$ECHO "$0" | ${SED} -e 's%^.*/%%'`
modename="$program"
reexec="$SHELL $0"

version="3.0.0"
ident='$RCSfile: modpost.sh,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2005/06/02 03:05:12 $'

# Sed substitution that helps us do robust quoting.  It backslashifies
# metacharacters that are still active within double-quoted strings.
Xsed="${SED}"' -e 1s/^X//'
sed_quote_subst='s/\([\\`\\"$\\\\]\)/\\\1/g'
# test EBCDIC or ASCII
case `$ECHO A|od -x` in
 *[Cc]1*) # EBCDIC based system
  SP2NL="tr '\100' '\n'"
  NL2SP="tr '\r\n' '\100\100'"
  ;;
 *) # Assume ASCII based system
  SP2NL="tr '\040' '\012'"
  NL2SP="tr '\015\012' '\040\040'"
  ;;
esac

# NLS nuisances.
# Only set LANG and LC_ALL to C if already set.
# These must not be set unconditionally because not all systems understand
# e.g. LANG=C (notably SCO).
# We save the old values to restore during execute mode.
if test "${LC_ALL+set}" = set; then
  save_LC_ALL="$LC_ALL"; LC_ALL=C; export LC_ALL
fi
if test "${LANG+set}" = set; then
  save_LANG="$LANG"; LANG=C; export LANG
fi

# Make sure IFS has a sensible default
: ${IFS=" 	"}

modpost_letters='abcdefghijklmnopqrstuvwxyz'
modpost_LETTERS='ABCDEFGHIJKLMNOPQRSTUVWXYZ'
modpost_Letters=$modpost_letters$modpost_LETTERS
modpost_numbers='0123456789'
modpost_alphano=$modpost_Letters$modpost_numbers
modpost_uppercase="$SED y%*$modpost_letters%P$modpost_LETTERS%;s%[^_$modpost_alphano]%_%g"

modpost_tokenize="$SED s%[^a-zA-Z0-9]%_%g"
modpost_fnameize="$SED -r -e s%.*/%%;s%(\.mod)?\.(k)?o(\.gz)?$%%"
modpost_fpathize="$SED -r -e s%(\.mod)?\.(k)?o(\.gz)?$%%"

# defaults

defaults="sysmap moddir infile outfile sysfile unload modversions allsrcversion modules command cachefile"

#default_sysmap=/boot/System.map-`uname -r`
#default_moddir=/lib/modules/`uname -r`
#default_infile=/lib/modules/`uname -r`/build/Module.symvers
#default_outfile=./Module.symvers
#default_sysfile=./System.symvers
#default_unload=${CONFIG_MODULE_UNLOAD:-n}
#default_modversions=${CONFIG_MODVERSIONS:-n}
#default_allsrcversion=${CONFIG_MODULE_SRCVERSION_ALL:-n}
#default_modules="$(find `pwd` -name '*.o')"
#default_command=process

default_sysmap=
default_moddir=
default_infile=
default_outfile=
default_sysfile=
default_unload=n
default_modversions=n
default_allsrcversion=n
default_modules=
default_command=process
default_cachefile=$MODPOST_CACHE

debug=0
verbose=1

function version()
{
    if test ${show:-yes} = no ; then
	return
    fi
    cat <<EOF
Version $version
$ident
Copyright (c) 2001-2005  OpenSS7 Corporation.  All Rights Reserved.
Distributed under GPL Version 2, included here by reference.
See \`$program --copying' for copying permissions.
EOF
}

function usage()
{
    if test ${show:-yes} = no ; then
	return
    fi
    cat <<EOF
$program:
    $ident
Usage:
    $program [options] [MODULE ...]
    $program {-h|--help}
    $program {-V|--version}
    $program {-C|--copying}
EOF
}

function help()
{
    if test ${show:-yes} = no ; then
	return
    fi
    usage
    cat <<EOF
Arguments:
    MODULE ...
        modules for which to generate symbols
Options:
    -c, --cachefile CACHEFILE
        where to cache system symbol versions
    -d, --moddir MODDIR
        module directory
    -F, --filename SYSMAP
        specify system map file
    -i, --infile INFILE
        input file (may be repeated or escaped space separated list)
    -o, --outfile OUTFILE
        output file for module symbols
    -s, --sysfile SYSFILE
        output file for system symbols
    -u, --unload
        module unloading is supported ['$unload']
    -m, --modversions
        module versions are supported ['$modversions']
    -a, --allsrcversion
        source version all modules ['$allsrcversion']
    -n, --dryrun
        don't perform the actions, just check them
    -q, --quiet
        suppress normal output
    -D, --debug [LEVEL]
        increase or set debugging verbosity
    -v, --verbose [LEVEL]
        increase or set output verbosity
    -h, --help
        prints this usage information and exits
    -V, --version
        prints the version and exits
    -C, --copying
        prints copying permissions and exits
EOF
}

function copying()
{
    if test ${show:-yes} = no ; then
	return
    fi
    cat <<EOF
--------------------------------------------------------------------------------
$ident
--------------------------------------------------------------------------------
Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

All Rights Reserved.
--------------------------------------------------------------------------------
This program is free software; you can  redistribute  it and/or modify  it under
the terms  of the GNU General Public License  as  published by the Free Software
Foundation; either  version  2  of  the  License, or (at  your option) any later
version.

This program is distributed in the hope that it will  be useful, but WITHOUT ANY
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should  have received a copy of the GNU  General  Public License  along with
this program; if not, write to the Free Software Foundation, Inc., 675 Mass Ave,
Cambridge, MA 02139, USA.
--------------------------------------------------------------------------------
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf
of the U.S. Government ("Government"), the following provisions apply to you. If
the Software is supplied by the  Department of Defense ("DoD"), it is classified
as "Commercial  Computer  Software"  under  paragraph  252.227-7014  of the  DoD
Supplement  to the  Federal Acquisition Regulations  ("DFARS") (or any successor
regulations) and the  Government  is acquiring  only the  license rights granted
herein (the license rights customarily provided to non-Government users). If the
Software is supplied to any unit or agency of the Government  other than DoD, it
is  classified as  "Restricted Computer Software" and the Government's rights in
the Software  are defined  in  paragraph 52.227-19  of the  Federal  Acquisition
Regulations ("FAR")  (or any successor regulations) or, in the cases of NASA, in
paragraph  18.52.227-86 of  the  NASA  Supplement  to the FAR (or any  successor
regulations).
--------------------------------------------------------------------------------
Commercial  licensing  and  support of this  software is  available from OpenSS7
Corporation at a fee.  See http://www.openss7.com/
--------------------------------------------------------------------------------
EOF
}

function syntax_error()
{
    if test ${verbose:-0} -gt 0 ; then
	$ECHO "$program: syntax error -- $1" >&2
	( usage ) >&2
    fi
    exit 2
}

function option_unrec()
{
    opt=`$ECHO -n "X$1" | $Xsed -e 's|=.*||'`
    syntax_error "\`$opt' unrecognized"
}

function option_noarg()
{
    opt=`$ECHO -n "X$1" | $Xsed -e 's|=.*||'`
    syntax_error "\`$opt' does not accept an argument"
}

function option_needarg()
{
    syntax_error "\`$1' requires an argument"
}

function option_after()
{
    syntax_error "\`$1' cannot occur after \`$2'"
}

function option_with()
{
    syntax_error "\`$1' cannot occur with \`$2'"
}

# Parse our command line options once, thoroughly.
while test "$#" -gt 0 -o :"$more" != :
do
    if test :"$more" != : ; then arg="-$more" ; more= ; else arg="$1" ; shift ; fi
    # check for attached option argument
    case $arg in
	(--filename=*|--filenam=*|--filena=*|--filen=*|--file=*|--fil=*|--fi=*|--f=*|\
	 --moddir=*|--moddi=*|--modd=*|--mod=*|--mo=*|--m=*|\
	 --infile=*|--infil=*|--infi=*|--inf=*|--in=*|--i=*|\
	 --outfile=*|--outfil=*|--outfi=*|--outf=*|--out=*|--ou=*|--o=*|\
	 --sysfile=*|--sysfil=*|--sysfi=*|--sysf=*|--sys=*|--sy=*|--s=*|\
	 --cachefile=*|--cachefil=*|--cachefi=*|--cachef=*|--cache=*|--cach=*|--cac=*|--ca=*|--c=*)
	    optarg=`$ECHO "X$arg" | $Xsed -e 's/[-_a-zA-Z0-9]*=//'` ;;
	(--*=*)
	    option_noarg $arg ;;
	(-[umanqDvhVC])
	    optarg= ;;
	(-[umanqDvhVC]*)
	    optarg=
	    more=`$ECHO "X$arg" | $Xsed -e 's|-[umanqDvhVC]||'`
	    eval "arg=\`$ECHO \"X$arg\" | $Xsed -e 's|$more||'\`"
	    ;;
	(-[fdiosc])
	    optarg= ;;
	(-[fdiosc]*)
	    optarg=`$ECHO "X$arg" | $Xsed -e 's|-[fdiosc]||'` ;;
	(*)
	    optarg= ;;
    esac
    # check for optional or required option argument
    if test :"$prev" != : ; then
	case $arg in
	    (-*) # optional arguments not forthcoming
		case $prev in
		    (debug|verbose) eval "(($prev++))" ;;
		    # the rest have required arguments
		    (sysmap|moddir|infile|outfile|sysfile|cachefile)
			option_needarg $prevopt ;;
		esac
		prev= ; prevopt=
		;;
	    (*) # if the previous option needs an argument, assign it.
		case $prev in
		    # these can be repeated
		    (infile)
			eval $prev'="$'$prev'${'$prev':+ }$arg"'
			;;
		    # the rest override
		    (*)
			eval $prev'="$arg"'
			;;
		esac
		prev= ; prevopt=
		continue
		;;
	esac
    fi
    # Have we seen a non-optional argument yet?
    case $arg in
	(--help|--h|--Help|--H|-h|-H|-\?|--\?)
	    show_help=yes
	    if test :"$command" = : ; then command=none ; fi
	    ;;
	(--version|--versio|--versi|--vers|-V)
	    show_version=yes
	    if test :"$command" = : ; then command=none ; fi
	    ;;
	(--copying|--copyin|--copyi|--copy|--cop|--co|--c|-C)
	    show_copying=yes
	    if test :"$command" = : ; then command=none ; fi
	    ;;
	(--verbose|--verbos|--verbo|--verb)
	    prevopt="$arg"
	    prev=verbose
	    ;;
	(-v)
	    ((verbose++))
	    ;;
	(--verbose=*|--verbos=*|--verbo=*|--verb=*)
	    verbose="$optarg"
	    ;;
	(--debug|--debu|--deb)
	    prevopt="$arg"
	    prev=debug
	    ;;
	(-D)
	    $ECHO "$program: enabling shell trace mode" 1>&2
	    set -x
	    ;;
	(--debug=*|--debu=*|--deb=*)
	    debug="$optarg"
	    ;;
	(--dry-run|--dryrun|--n|-n)
	    run=no
	    ;;
	(--quiet|--silent|-q)
	    show=no
	    verbose=0
	    debug=0
	    ;;
	(--filename|--filenam|--filena|--filen|--file|--fis|--fi|--f|-F)
	    prevopt="$arg"
	    prev=sysmap
	    ;;
	(--filename=*|--filenam=*|--filena=*|--filen=*|--file=*|--fil=*|--fi=*|--f=*|-F*)
	    sysmap="$optarg"
	    ;;
	(--moddir|--moddi|--modd|--mod|--mo|--m|-d)
	    prevopt="$arg"
	    prev=moddir
	    ;;
	(--moddir=*|--moddi=*|--modd=*|--mod=*|--mo=*|--m=*|-d*)
	    moddir="$optarg"
	    ;;
	(--infile|--infil|--infi|--inf|--in|--i|-i)
	    prevopt="$arg"
	    prev=infile
	    ;;
	(--infile=*|--infil=*|--infi=*|--inf=*|--in=*|--i=*|-i*)
	    infile="$infile${infile:+ }$optarg"
	    ;;
	(--outfile|--outfil|--outfi|--outf|--out|--ou|--o|-o)
	    prevopt="$arg"
	    prev=outfile
	    ;;
	(--outfile=*|--outfil=*|--outfi=*|--outf=*|--out=*|--ou=*|--o=*|-o*)
	    outfile="$optarg"
	    ;;
	(--sysfile|--sysfil|--sysfi|--sysf|--sys|--sy|--s|-s)
	    prevopt="$arg"
	    prev=sysfile
	    ;;
	(--sysfile=*|--sysfil=*|--sysfi=*|--sysf=*|--sys=*|--sy=*|--s=*|-s*)
	    sysfile="$optarg"
	    ;;
	(--cachefile|--cachefil|--cachefi|--cachef|--cache|--cach|--cac|--ca|--c|-c)
	    prevopt="$arg"
	    prev=cachefile
	    ;;
	(--cachefile=*|--cachefil=*|--cachefi=*|--cachef=*|--cache=*|--cach=*|--cac=*|--ca=*|--c=*|-c*)
	    cachefile="$optarg"
	    ;;
	(--unload|--unloa|--unlo|--unl|--un|--u|-u)
	    unload=y
	    ;;
	(--modversions|--modversion|--modversio|--modversi|--modvers|--modver|--modve|--modv|--mod|--mo|--m|-m)
	    modversions=y
	    ;;
	(--allsrcversion|--allsrcversio|--allsrcversi|--allsrcvers|--allsrcver|--allsrcve|--allsrcv|--allsrc|--allsr|--alls|--all|--al|--a|-a)
	    allsrcversion=y
	    ;;
	(--)
	    # end of options
	    break
	    ;;
	(-*)
	    option_unrec $arg
	    ;;
	(*)
	    nonopt="${nonopt}${nonopt:+ }'$arg'"
	    ;;
    esac
done

# hit end of list wanting arguments
case $prev in
    # these have optional arguments
    (debug|verbose) eval "(($prev++))" ;;
    # the rest have required arguments
    (sysmap|moddir|infile|outfile|sysfile|cachefile)
	option_needarg $prevopt ;;
esac

if test x"$nonopt${nonopt:+ }${1+$@}" != "x" ; then
    eval "set $nonopt $@"
    modules=$*
fi

# assign defaults to all unassigned variables
for d in $defaults ; do
	eval "test :\${$d+set} = :set || $d=\"\$default_$d\""
done

if test $debug -gt 1 -o $verbose -gt 1 ; then
    exec 3>&2
else
    exec 3>/dev/null
fi

retval=0
errors=0
warnings=0

command_error() {
    $ECHO "$program: E: ${1+$@}" >&3
    ((errors++))
    retval=1
}

command_warn() {
    $ECHO "$program: W: ${1+$@}" >&3
    ((warnings++))
}

command_info() {
    $ECHO "$program: D: ${1+$@}" >&3
}

#
# This function is the same for 2.6.3 through 2.6.10
#
add_header() {
	cat<<_ACEOF
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
_ACEOF
}

#
# This function is post-2.6.3.  I will have to dig out the kernels between 2.6.3 and 2.6.10 to
# figure out which one the change was made in.  However, it looks unnecessary unless the module
# refers to itself.
#
# And well, yes it is unnecessary unless the module refers to itself.  In 2.6.3 this section was
# generated when linux/module.h was included and both MODULE and KBUILD_MODNAME was defined.  Now in
# 2.6.10 it appears that this has been removed to modpost.  We should probably be able to determine
# whether it is necessary if __this_module is undefined.
#
# It is interesting that when we build modules even for 2.6.3 we do not set KBUILD_MODNAME as so
# this definition is never included, however, any reference to __this_module is.  Strange but 2.6.10
# sets KBUILD_MODNAME when compiling the result, which includes module.h which does not have this
# definition.  Whereas, 2.6.3 does not set KBUILD_MODNAME when compiling the result but does for
# each object, so many objects will have this section defined.  But it is defined as link once.  If
# we define KBUILD_MODNAME when compiling this and it was also present in module.h then we have two
# definitions.  If we set KBUILD_MODNAME when we compile everything, we should have a bunch of
# occurences of these for 2.6.3, but one for 2.6.10.  Or, another approach is to never set
# KBUILD_MODNAME when compiling this or anything else and to substitute it here.
#
# There is another slight variation.  The 2.6.10 way does not require an init_module definition in
# each module.  The 2.6.3 way does.
#
# We define KBUILD_MODNAME right in this file.  That way, under 2.6.3 compiled without
# -DKBUILD_MODNAME and this file results in only one __this_module section.  Under 2.6.10, the
# result is the same when modules are compiled without -DKBUILD_MODNAME.
#
add_this_module() {
    name="$1"
    token=`echo "$name" | $modpost_tokenize`
    eval "this=\"\${mod_${token}_this:-no}\""
    test :$this = :no || return 0
    cat<<_ACEOF

#undef unix

#define KBUILD_MODNAME $1

#ifdef KBUILD_MODNAME
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = __stringify(KBUILD_MODNAME),
_ACEOF
    # the symbol must be defined
    eval "init=\"\${mod_${token}_init:-no}\""
    if test :$init = :yes ; then
	    cat<<_ACEOF
	.init = init_module,
_ACEOF
    fi
    # the symbol must be defined
    eval "exit=\"\${mod_${token}_exit:-no}\""
    if test :$exit = :yes ; then
	    cat<<_ACEOF
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
_ACEOF
    fi
    cat<<_ACEOF
};
#endif
_ACEOF
}

#
# This function is the same from 2.6.3 through 2.6.10.  This section does not need to get included
# if modversions is not set.
#
add_versions() {
    test :$modversions = :y || return
    name="$1"
    token=`echo "$name" | $modpost_tokenize`
    cat<<_ACEOF

#ifdef CONFIG_MODVERSIONS
static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
_ACEOF
    eval "syms=\"\$mod_${token}_undefs\""
    for sym in $syms ; do
	eval "crc=\"\$sym_${sym}_crc\""
	test :"$crc" != : || { command_warn "symbol $sym has no version" ; continue ; }
	printf "\t{ 0x%08x, \"%s\" },\n" $crc $sym
	case " $(eval '$ECHO "$mod_'${token}'_deps"') " in
	    (*" `eval '$ECHO "$sym_'${sym}'_name"'` "*) ;;
	    (*) eval "mod_${token}_deps=\"\$mod_${token}_deps \$sym_${sym}_name\"" ;;
	esac
    done
    cat<<_ACEOF
};
#endif
_ACEOF
}

#
# This function is the same from 2.6.3 through 2.6.10
#
add_depends() {
    name="$1"
    token=`echo "$name" | $modpost_tokenize`
    cat<<_ACEOF

static const char __module_depends[]
__attribute_used__
__attribute__((section(".modinfo"))) =
_ACEOF
    printf "\"depends="
    local sep=
    eval "deps=\"\$mod_${token}_deps\""
    for dep in $deps ; do
	if test :$dep != :vmlinux ; then
	    printf "%s%s" "$sep" "$dep"
	    sep=','
	fi
    done
    printf "\";\n"
}

#
# This function is post-2.6.3.  I will have to dig out the kernels between 2.6.3 and 2.6.10 to
# figure out which one the change was made in.  Addition is not optional.  This figures out whether
# any pci, usb or other aliases should be generated.  This is because there are no more depmod
# tables.
#
add_moddevtable() {
    :
}

#
# This function is post-2.6.3.  I will have to dig out the kernels between 2.6.3 and 2.6.10 to
# figure out which one the change was made in.  Addition of source version is optional.
#
add_srcversion() {
    name="$1"
    token=`echo "$name" | $modpost_tokenize`
    shift
    # also set with -a flag to modpost
    test :${allsrcversion:-n} = :y || return 0
    case " $(eval '$ECHO "$mod_'${token}'_syms"') " in
	    ( __mod_version[0-9][0-9]* | __mod_version[0-9][0-9]* ) ;;
	    (*) return 0 ;;
    esac
    cat /dev/null >.mytmp$$.sources
    for file in ${1+@} ; do
	    cat $file >> .mytmp$$.sources
    done
    srcversion=`openssl -md4 .my4mp$$.sources`
    rm -f -- .mytmp$$.sources
    cat<<_ACEOF
#ifdef CONFIG_MODULE_SRCVERSION_ALL
	MODULE_INFO(srcversion, "$srcversion");
#endif
_ACEOF
}

write_modsrc() {
    name="$1"
    add_header $name
    add_this_module $name
    add_versions $name
    add_depends $name
    add_moddevtable $name
    # need to add sources to this, need to get them from dep
    add_srcversion $name
}

write_sources() {
    if test :"$my_mod_names" != : ; then
	command_info "writing module source files"
	for name in $my_mod_names ; do
	    command_info "writing module source file $name.mod.c"
	    write_modsrc $name >$name.mod.c
	done
    fi
}

#
# Dump out defined crcs symbols and module names for our modules in Module.symvers format to
# --outfile.  Nothing more.  This is so that we can save these definitions for use by other kernel
# modules.
#
write_dump_ours() {
    modcnt=0
    symcnt=0
    for name in $my_mod_names ; do
	token=`echo "$name" | $modpost_tokenize`
	eval "path=\"\$mod_${token}_path\""
	eval "syms=\"\$mod_${token}_syms\""
	command_info "dumping module symbols for module $name"
	((modcnt++))
	count=0
	for sym in $syms ; do
	    eval "crc=\"\$mod_${token}_sym_${sym}_crc\""
	    if test :"$crc" != : ; then
		((count++))
		printf "0x%08x\t%s\t%s\n" $crc $sym ${path:-$name}
	    else
		command_warn "symbol without crc $sym"
	    fi
	done
	command_info "wrote $count symbols for module $name"
	((symcnt=symcnt+count))
    done
    command_info "wrote $symcnt symbols for $modcnt modules"
}

write_outfile() {
    test :"$outfile" != : || return 0
    touch $outfile || return 1
    command_info "dumping module symbols to $outfile"
    write_dump_ours >$outfile
}

write_dump_others() {
    modcnt=0
    symcnt=0
    for name in $mod_names ; do
	case " $my_mod_names " in
	    (*" $name "*)
		command_info "skipping $name"
		continue
		;;
	esac
	token=`echo "$name" | $modpost_tokenize`
	eval "syms=\"\$mod_${token}_syms\""
	command_info "dumping system symbols for module $name"
	((modcnt++))
	count=0
	for sym in $syms ; do
	    eval "crc=\"\$mod_${token}_sym_${sym}_crc\""
	    if test :"$crc" != : ; then
		((count++))
		printf "0x%08x\t%s\t%s\n" $crc $sym $name
	    else
		command_warn "symbol without crc $sym"
	    fi
	done
	command_info "wrote $count symbols for module $name"
	((symcnt=symcnt+count))
    done
    command_info "wrote $symcnt symbols for $modcnt modules"
}

write_sysfile() {
    test :"$sysfile" != : || return 0
    touch $sysfile || return 1
    command_info "dumping system sybols to $sysfile"
    write_dump_others >$sysfile
}

write_cache() {
    for var in ${!sym_*} ; do
	eval "printf \"%s=%q\n\" \"$var\" \"\$$var\""
    done
}

#
# Read in defined crcs symbols and module name for kernel and kernel modules in Module.symvers
# format from and --infile files.  Nothing more.  This is if a Module.symvers exists somewhere that
# we can read.
#
read_dump() {
    count=0
    progress=0
    while read crc sym path junk ; do
	((count++))
	((progress++))
	if test $progress -ge 100 ; then
	    progress=0
	    command_info "processed $count symbols"
	fi
	if test :"$junk" != : ; then
	    command_warn "line $count junk on line $junk"
	    continue
	fi
	name=`echo "$path" | $modpost_fnameize`
	case " $mod_names " in
	    (*" $name "*) ;;
	    (*) # command_info "adding sys module name $name"
		mod_names="$mod_names${mod_names:+ }$name" ;;
	esac
	tok=`echo "$name" | $modpost_tokenize`
	eval "mod_${tok}_path=\"$path\""
	eval "mod_${tok}_syms=\"\$mod_${tok}_syms $sym\""
	eval "mod_${tok}_sym_${sym}_crc=\"$crc\""
	eval "sym_${sym}_name=\"$name\""
	eval "sym_${sym}_crc=\"$crc\""
	cache_dirty=yes
    done
    command_info "processed $count symbols"
}

read_infiles() {
    if test :"$infile" != : ; then
	command_info "processing input files"
	for file in $infile ; do
	    if test -f "$file" -a -r "$file" ; then
		command_info "processing input file $file"
		read_dump <$file
	    fi
	done
    fi
}

#
# Read in any nm -Bs listing.  This can be a System.map file, a Modules.map file, or the result of
# nm -Bs run on a kernel object.
#
read_map() {
    name=$1
    token=`echo "$name" | $modpost_tokenize`
    test :"$name" != : || { command_error "no module name" ; return 1 ; }
    count=0
    progress=0
    while read crc flag expsym modname junk ; do
	((count++))
	((progress++))
	if test $progress -ge 100 ; then
	    progress=0
	    command_info "processed $count symbols"
	fi
	test -z "$junk" || { command_error "got junk $junk" ; continue ; }
	if test :$flag = :A -o :$flag = :w -o :$flag = :a -o :$flag = :t ; then
	    $ECHO "$expsym" | egrep -q '^(_)?__crc_' &>/dev/null || { command_error "not crc sym $expsym" ; continue ; }
	    sym=`$ECHO "$expsym" | sed -r -e 's|^(_)?__crc_||'`
	    if test :"$modname" != : ; then
		nam=`echo "$modname" | sed -e 's|[[]\(.*\)[]]|\1|'`
		tok=`echo "$nam" | $modpost_tokenize`
		case " $mod_names " in
		    (*" $nam "*) ;;
		    (*) command_info "adding sys module name $nam"
			mod_names="$mod_names${mod_names:+ }$nam" ;;
		esac
	    else
		nam="$name"
		tok="$token"
	    fi
	    eval "mod_${tok}_syms=\"\$mod_${tok}_syms $sym\""
	    eval "mod_${tok}_sym_${sym}_crc=\"0x$crc\""
	    eval "sym_${sym}_name=\"$nam\""
	    eval "sym_${sym}_crc=\"0x$crc\""
	    cache_dirty=yes
	elif test :$crc = :U ; then
	    undef="$flag"
	    case "$undef" in
		(__this_module|___this_module)
		    eval "mod_${token}_this='no'"
		    continue
		    ;;
	    esac
	    eval "mod_${token}_undefs=\"\$mod_${token}_undefs $undef\""
	    case " $(eval '$ECHO "$undef_'${undef}'_names"') " in
		(*" $name "*) ;;
		(*) eval "undef_${undef}_names=\"\$undef_${undef}_names $name\"" ;;
	    esac
	    eval "((undef_${token}_count++))"
	    eval "((undef_count++))"
	else
	    case "$expsym" in
		(init_module|_init_module)
		    eval "mod_${token}_init='yes'"
		    continue
		    ;;
		(cleanup_module|_cleanup_module)
		    eval "mod_${token}_exit='yes'"
		    continue
		    ;;
		(__this_module|___this_module)
		    eval "mod_${token}_this='yes'"
		    continue
		    ;;
	    esac
	fi
    done
    #
    # trick for adding struct_module version (get rid of tainting)
    #
    undef='struct_module'
    eval "mod_${token}_undefs=\"\$mod_${token}_undefs $undef\""
    case " $(eval '$ECHO "$undef_'${undef}'_names"') " in
	(*" $name "*) ;;
	(*) eval "undef_${undef}_names=\"\$undef_${undef}_names $name\"" ;;
    esac
    eval "((undef_${token}_count++))"
    eval "((undef_count++))"
    command_info "processed $count symbols"
}

#
# This one is for reading system map files (System.map).
#
read_sysmap() {
    if test :"$sysmap" != : ; then
	command_info "processing system map files"
	for map in $sysmap ; do
	    if test -f "$map" -a -r "$map" ; then
		case " $mod_names " in
		    (*" vmlinux "*) ;;
		    (*) # command_info "adding sys module name $name"
			mod_names="$mod_names${mod_names:+ }vmlinux" ;;
		esac
		command_info "processing system map $map"
		egrep '\<(_)?__crc_' $map >.tmp.$$.map
		read_map vmlinux <.tmp.$$.map
		rm -f -- .tmp.$$.map
	    fi
	done
    fi
}

#
# This one is for reading in an existing kernel object module (.ko), possibly gzipped, and reading
# the exported and undefined symbols from the module.  These are identified by the A or U flag.
# This is for collecting information about our own kernel modules.  Note that we also have to watch
# out for several special symbols.
#
read_module() {
    filename="$1"
    test :"filename" != : -a -f "$filename" || return 1
    name=`echo "$filename" | $modpost_fnameize`
    test :"$name" != : || return 1
    if $ECHO "$filename" | grep -qc1 '\.gz$' ; then
	# shoot, we have a .gz ending
	file=".tmp.$$.$name.ko"
	remv="$file"
	gzip -dc $filename > $file || continue
    else
	file="$filename"
	remv=
    fi
    case " $my_mod_names " in
	(*" $name "*) ;;
	(*) # command_info "adding ext module name $name"
	    my_mod_names="$my_mod_names${my_mod_names:+ }$name"
	    path=`echo "$filename" | $modpost_fpathize`
	    tok=`echo "$name" | $modpost_tokenize`
	    eval "mod_${tok}_path=\"$path\""
	    ;;
    esac
    command_info "processing module $name in file $filename"
    nm -s $file | egrep '(\<A\>|\<U\>|\<(_)?init_module\>|\<(_)?cleanup_module\>|\<(_)?__this_module\>)' >.tmp.$$.map
    read_map "$name" <.tmp.$$.map
    rm -f -- .tmp.$$.map
    test :"$remv" != : && rm -f -- "$remv"
}

#
# Read in all the modules
#
read_modules() {
    if test :"$modules" != : ; then
	command_info "processing modules"
	for filename in $modules ; do
	    test -f "$filename" -a -r "$filename" || continue
	    read_module "$filename"
	done
    fi
}

#
# This one is for reading in an existing kernel object module (.ko), possibly gzipped, and reading
# the exported symbols from the module.  These are identified by the __crc_ prefix.  This is in case
# we do not have a Module.symvers hanging around and need to collect the symbol information from the
# existing kernel modules themselves.
#
read_kobject() {
    filename="$1"
    test -n "$filename" -a -f "$filename" || return 1
    # not all files have appropriate syms
    zgrep -E -qc1 '\<(_)?__crc_' $filename || return 0
    name=`echo "$filename" | $modpost_fnameize`
    test -n "$name" || return 1
    if $ECHO "$filename" | grep -qc1 '\.gz$' ; then
	# shoot, we have a .gz ending
	file=".tmp.$$.$name.ko"
	remv="$file"
	gzip -dc $filename > $file
    else
	file="$filename"
	remv=
    fi
    case " $mod_names " in
	(*" $name "*) ;;
	(*) # command_info "adding sys module name $name"
	    mod_names="$mod_names${mod_names:+ }$name"
	    path=`echo "$filename" | $modpost_fpathize`
	    tok=`echo "$name" | $modpost_tokenize`
	    eval "mod_${tok}_path=\"$path\""
	    ;;
    esac
    base=`echo "$filename" | sed -e 's|^'$moddir/'||;s|^kernel/||'`
    command_info "processing module $name in file $base"
    nm -s $file | egrep '\<(_)?__crc_' >.tmp.$$.map
    read_map "$name" <.tmp.$$.map
    rm -f -- .tmp.$$.map
    test -n "$remv" && rm -f -- $remv
}

read_kobjects() {
    if test -n "$moddir" -a -d "$moddir"; then
	command_info "processing kernel object files"
	for kobj in $(find $moddir -name '*.ko' -o -name '*.ko.gz' 2>/dev/null) ; do
	    read_kobject $kobj
	done
    fi
}

function process_command()
{
    # .modpost is just a (quick) way of caching system maps and module maps
    if test :"$cachefile" != : -a -f "$cachefile" ; then
	command_info "processing cache file $cachefile"
	. $cachefile
	cache_dirty=no
    else
	test :"$sysmap" != :  && command_info "sysmaps= \`$sysmap'"
	read_sysmap
	test :"$infile" != : && command_info "infiles= \`$infile'"
	read_infiles
	test :"$moddir" != :  && command_info "moddir=  \`$moddir'"
	read_kobjects
    fi
    if test :"$modules" != : ; then
	test :"$modules" != : && command_info "modules= \`$modules'"
	read_modules
	write_sources
	test :"$outfile" != : && command_info "outfile= \`$outfile'"
	write_outfile
	test :"$sysfile" != : && command_info "sysfile= \`$sysfile'"
	write_sysfile
    fi
    if test :$cache_dirty = :yes -a :"$cachefile" != : ; then
	command_info "writing cache file $cachefile"
	write_cache >"$cachefile"
    fi
}

#
# The symvers file format is 0x%08x\t%s\t%s\n crc symname modname.
#
function none_command()
{
    if test ":$show_version$show_help$show_copying" != : ; then
	if test ${verbose:-0} -gt 1 ; then
	    $ECHO "Displaying information"
	fi
	if test :"$show_version" != : ; then
	    version
	fi
	if test :"$show_help" != : ; then
	    help
	fi
	if test :"$show_copying" != : ; then
	    copying
	fi
    fi
}

case "$command" in
    (none|process)
	eval "${command}_command"
	;;
    *)
	( usage ) >&2
	exit 1
	;;
esac

exit $retval

# vim: ft=sh sw=4
