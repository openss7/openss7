#!/usr/bin/perl -w
# =============================================================================
#
# @(#) $RCSfile: modpost.pl,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-12 03:44:13 $
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
# Last Modified $Date: 2011-01-12 03:44:13 $ by $Author: brian $
#
# =============================================================================

# 2.6 Kernel modpost ala perl

# Note that modules change in 2.5.48.

eval 'exec perl -S $0 ${1+"@"}'
    if $running_under_some_shell;

$\ = "\n";

no strict;

$program = $0;
$program =~ s/^.*\///;
$modename = "$program";

$default{'sysmap'} = '';
$default{'moddir'} = '';
$default{'infile'} = '';
$default{'outfile'} = '';
$default{'sysfile'} = '';
$default{'unload'} = 'n';
$default{'modversions'} = 'n';
$default{'allsrcversion'} = 'n';
$default{'modules'} = '';
$default{'command'} = 'process';
$default{'cachefile'} = $ENV{MODPOST_CACHE};

$debug = 0;
$verbose = 1;

$ident = '$RCSfile: modpost.pl,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-12 03:44:13 $';
$version = '3.0.0';

$show = 'yes';

sub version()
{
	return if $show eq 'no';
	print<<EOF;
Version $version
$ident
Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved.
Distributed under AGPL Version 3, included here by reference.
See `$program --copying' for copying permissions.
EOF
}

sub usage()
{
    return if $show eq 'no';
    print STDERR<<EOF;
$program:
    $ident
Usage:
    $program [options] [MODULE ...]
    $program {-h|--help}
    $program {-V|--version}
    $program {-C|--copying}
EOF
}

sub help()
{
    return if $show eq 'no';
    print<<EOF;
$program:
    $ident
Usage:
    $program [options] [MODULE ...]
    $program {-h|--help}
    $program {-V|--version}
    $program {-C|--copying}
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
        module unloading is supported ['$default{'unload'}']
    -m, --modversions
        module versions are supported ['$default{'modversions'}']
    -a, --allsrcversion
        source version all modules ['$default{'allsrcversion'}']
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

sub copying()
{
    return if $show eq 'no';
    print<<EOF;
--------------------------------------------------------------------------------
$ident
--------------------------------------------------------------------------------
Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock\@openss7.org>

All Rights Reserved.
--------------------------------------------------------------------------------
This program is free software; you can  redistribute  it and/or modify  it under
the terms of the  GNU  Affero  General  Public  License as published by the Free
Software Foundation; version 3 of the License.

This program is distributed in the hope that it will  be useful, but WITHOUT ANY
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the  GNU  Affero General Public License along
with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the
Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

sub syntax_error($)
{
    my $error = shift;
    if ($verbose > 0) {
	print STDERR "$program: syntax error -- $error";
	usage;
    }
    exit 2;
}

sub option_unrec($)
{
    my $opt = shift;
    $opt =~ s/=.*//;
    syntax_error "`$opt' unrecognized";
}

sub option_noarg($)
{
    my $opt = shift;
    syntax_error "`$opt' does accept an argument";
}

sub option_needarg($)
{
    my $opt = shift;
    syntax_error "`$opt' requires an argument";
}

sub option_needarg($)
{
    my $opt = shift;
    syntax_error "`$opt' requires an argument";
}

sub option_after($$)
{
    my $opt = shift;
    my $oth = shift;
    syntax_error "`$opt' cannot occur with `$oth'";
}

while ($#ARGV > 0 or $more ne '') {
    if ( $more ne '' ) {
	$arg = "-$more";
	$more = '';
    } else {
	$arg = $ARGV[0];
    }
    # check for attached option argument
    if ($arg =~ /^--(f(i(l(e(n(a(m(e)?)?)?)?)?)?|m(o(d(d(i(r)?)?)?)?)?|i(n(f(i(l(e)?)?)?)?)?|o(u(t(f(i(l(e)?)?)?)?)?)?|s(y(s(f(i(l(e)?)?)?)?)?)?|c(a(c(h(e(f(i(l(e)?)?)?)?)?)?)?)?)=/) {
	$arg =~ /^[-_a-zA-Z0-9]*=(.*)$/;
	$optarg = $1;
    } elsif ($arg =~ /^--.*=/) {
	option_noarg $arg;
    } elsif ($arg =~ /^-[umanqDvhVC]$/) {
	$optarg = '';
    } elsif ($arg =~ /^(-[umanqDvhVC])(.*)$/) {
	$arg = $1;
	$more = $2;
	$optarg = '';
    } elsif ($arg =~ /^-[fdiosc]$/) {
	$optarg = '';
    } elsif ($arg =~ /^(-[fdiosc])(.*)$/) {
	$arg = $1;
	$optarg = $2;
    } else {
	$optarg = '';
    }
    # check for optional or required option argument
    if ( $prev ne '' ) {
	if ($arg =~ /^-/) {
	    if ($prev =~ /^(debug|verbose)$/) {
		eval "\$$prev += 1";
	    } elsif ($prev =~ /^(sysmap|moddir|infile|outfile|sysfile|cachefile)$/) {
		option_needarg $prevopt;
	    }
	    $prev = ''; $prevopt = '';
	} else {
	    if ($prev =~ /^(infile)$/) {
		# these can be repeated
		eval "\$$prev = \"\$$prev \$arg\"";
	    } else {
		# the rest override
		eval "\$$prev=\"\$arg\"";
	    }
	    $prev = ''; $prevopt ='';
	    continue;
	}
    }
    # Have we seen a non-optional argument yet?
         if ($arg =~ /^-(-help|-h|-Help|-H|-\?|h|H|\?)$/) {
    } elsif ($arg =~ /^-(-version|-versio|-versi|-vers|V)$/) {
    } elsif ($arg =~ /^-(-copying|-copyin|-copy|-cop|-co|-c|C)$/) {
    } elsif ($arg =~ /^-(-verbose|-verbos|-verb)$/) {
    } elsif ($arg =~ /^-(v)$/) {
    } elsif ($arg =~ /^-(-verbose=|-verbos=|-verb=).*$/) {
    } elsif ($arg =~ /^-(-debug|-debu|-deb)$/) {
    } elsif ($arg =~ /^-(D)$/) {
    } elsif ($arg =~ /^-(-debug=|-debu=|-deb=|D).*$/) {
    } elsif ($arg =~ /^-(-dry-run|-dryrun|-n|n)$/) {
    } elsif ($arg =~ /^-(-quiet|-silent|q)$/) {
    } elsif ($arg =~ /^-(-filename|-filenam|-filena|-filen|-file|-fis|-fi|-f|F)$/) {
    } elsif ($arg =~ /^-(-filename=|-filenam=|-filena=|-filen=|-file=|-fis=|-fi=|-f=|F).*$/) {
    } elsif ($arg =~ /^-(-moddir|-moddi|-modd|-mod|-mo|-m|d)$/) {
    } elsif ($arg =~ /^-(-moddir=|-moddi=|-modd=|-mod=|-mo=|-m=|d).*$/) {
    } elsif ($arg =~ /^-(-infile|-infil|-infil|-inf|-in|-i|i)$/) {
    } elsif ($arg =~ /^-(-infile=|-infil=|-infil=|-inf=|-in=|-i=|i).*$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } elsif ($arg =~ /^-()$/) {
    } else {
    }
}

my *CACHEFILE;

sub tokenize($)
{
    my $token = shift;
    $token =~ s/[^a-zA-Z0-9]/_/g;
    return $token;
}

sub fnameize($)
{
    my $fname = shift;
    $fname =~ s/.*\///;
    $fname =~ s/(\.mod)?\.(k)?o(\.gz)?$//;
    return $fname;
}

sub fpathize($)
{
    my $fpath = shift;
    $fpath =~ s/(\.mod)?\.(k)?o(\.gz)?$//;
    return $fpath;
}

sub mnameize($)
{
    my $mname = shift;
    $mname =~ s/[[]\(.*\)[]]/\1/;
    return $mname;
}

my $warnings = 0;
my $errors = 0;
my $retval = 0;

sub command_error()
{
    print STDLOG "$program: E: \@_";
    $errors = $errors + 1;
    $retval = 1;
}

sub command_warn()
{
    print STDLOG "$program: W: \@_";
    $warnings = $warnings + 1;
}

sub command_info()
{
    print STDLOG "$program: D: \@_";
}

#
# This function is the same for 2.6.3 through 2.6.10
#
sub add_header(\*)
{
    local \*OUTFILE = shift;
    print OUTFILE<<EOF;
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
EOF
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
sub add_this_module()
{
    local \*OUTFILE = shift;
    my $name = shift;
    my $token = tokenize($name);
    return 0 unless defined $mod_this{$token};
    my $this = $mod_this{$token};
    print OUTFILE<<EOF;

#undef unix

#ifndef __attribute_used__
#define __attribute_used__ __used
#endif

#undef KBUILD_MODNAME
#define KBUILD_MODNAME $name

#ifdef KBUILD_MODNAME
struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
	.name = __stringify(KBUILD_MODNAME),
EOF
    # the symbol must be defined
    if (defined $mod_init{$token}) {
	print OUTFILE<<EOF;
	.init = init_module,
EOF
    }
    # the symbol must be defined
    if (defined $mod_exit{$token}) {
	print OUTFILE<<EOF;
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
EOF
    }
    print OUTFILE<<EOF
};
#endif
EOF
}

#
# This function is the same from 2.6.3 through 2.6.10.  This section does not need to get included
# if modversions is not set.
#
sub add_versions()
{
    local \*OUTFILE = shift;
    return unless $modversions eq 'y';
    my $name = shift;
    my $token = tokenize($name);
    print OUTFILE<<EOF;

#ifdef CONFIG_MODVERSIONS
static const struct modversion_info ____versions[]
__attribute_used__
__attribute__((section("__versions"))) = {
EOF
    foreach my $sym ( @{$mod_undefs{$token}} ) {
	unless (defined $sym_crc{$sym}) {
	    command_warn "symbol $sym has no version";
	    next;
	}
	my $crc = $sym_crc{$sym};
	printf OUTFILE "\t{ 0x%08x, \"%s\" },\n", $crc, $sym;
	unless (defined $mod_deps{$token}{$sym_name{$sym}}) {
	    $mod_deps{$token}{$sym_name{$sym}} = 1;
	}
    }
    print OUTFILE<<EOF;
};
#endif
EOF
}

sub write_cache($)
{
    my $cachefile = shift;
    open (CACHEFILE, >$cachefile);
    foreach my $key ( sort keys $sym_name ) {
	print CACHEFILE "$sym_name{$key} $sym_crc{$key}";
    }
    close (CACHEFILE);
}

sub read_dump()
{
    my @smap = shift;
    my $count = 0;
    my $progress = 0;
    foreach $smap ( @smap ) {
	my ($crc,$sym,$path,$exp,$junk) = split(/\b*/,$smap);
	$count = $count + 1;
	$progress = $progress + 1;
	if ($progress >= 100) {
	    $progress = 0;
	    command_info "processed $count symbols";
	}
	if ($exp =~ /^EXPORT_.*SYMBOL/) {
	    if ($junk ne '') {
		$junk = "$exp $junk";
	    } else {
		$junk = $exp;
	    }
	}
	if ($junk ne '') {
	    command_warn "line $count junk on lin $junk";
	    next;
	}
	my $name = fnameize($path);
	unless (defined $mod_names{$name}) {
	    $mod_names{$name} = $name;
	}
	my $tokn = tokenize($name);
	$mod_path{$tokn} = $path;
	push @{$mod_syms{$tokn}}, $sym;
	$mod_sym_crc{$tokn} = $crc;
	$sym_name{$sym} = $name;
	$sym_crc{$sym} = $crc;
	$cache_dirty = 'yes';
    }
    command_info "processed $count symbols";
}

sub read_infiles()
{
    if ($infile ne '') {
	command_info "processing input files";
	foreach file ( @infile ) {
	    if ( -r $file ) {
		command_info "processing input file $file";
		read_dump ( `cat $file` );
	    }
	}
    }
}

sub read_map($@)
{
    my $name = shift;
    my @smap = shift;
    my ($tokn,$count,$progress,$sym,$nam,$tok);
    $tokn = tokenize($name);
    unless ( $name ) {
	command_error "no module name";
	return 1;
    }
    $count = 0;
    $progress = 0;
    foreach $map ( @smap ) {
	my ($flag,$expsym,$modname,$junk) = split(/\b*/,$map);
	$count = $count + 1;
	$progress = $progress + 1;
	if ( $progress > 100 ) {
	    $progress = 0;
	    command_info "processed $count symbols";
	}
	if ( $junk ne '' ) {
	    command_error "got junk $junk";
	    next;
	}
	if ( $flag =~ /^(A|w|a|t)$/ ) {
	    unless ( $expsym =~ /^(_)?__crc_/ ) {
		command_error "not crc sym $expsym";
		next;
	    }
	    $sym = $';
	    if ( $modname ne '' ) {
		$nam = mnameize($modname);
		$tok = tokenize($nam);
		unless ( defined $mod_names{$nam} ) {
		    command_info "adding sys module name $nam";
		    $mod_names{$nam} = $nam;
		}
	    } else {
		$nam = $name;
		$tok = $token;
	    }
	    push @{$mod_syms{$tok}}, $sym;
	    $mod_sym{"$tok-$sym"} = $sym;
	    $sym_name{$sym} = $nam;
	    eval "$sym_crc{$sym} = 0x$crc";
	    $cache_dirty = 'yes';
	} elsif ( $crc =~ /^(U)$/ ) {
	    $undef = $flag;
	    if ($undef =~ /^(_)?__this_module$/) {
		$mod_this{$tokn} = 'no';
		next;
	    }
	    push @{$mod_undefs{$tokn}}, $undef;
	    unless ( defined $mod_undefs{$tokn} ) {
		$mod_undefs{$tokn} = $tokn;
		unless ( defined $undef_names{$undef} ) {
		    $undef_names{$undef} = $name;
		}
	    }
	    $undef_count{$tokn} = $undef_count{$tokn} + 1;
	    $undef_count = $undef_count + 1;
	} else {
	    if      ( $expsym =~ /^(init_module|_init_odule)$/ ) {
		$mod_init{$tokn} = 'yes';
	    } elsif ( $expsym =~ /^(cleanup_module|_cleanup_module)$/ ) {
		$mod_exit{$tokn} = 'yes';
	    } elsif ( $expsym =~ /^(__this_module|___this_module)$/ ) {
		$mod_this{$tokn} = 'yes';
	    }
	}
    }
    #
    # Trick for adding struct_module version (get rid of tainting)
    #
    $undef = 'struct_module';
    unless ( defined $mod_undefs{$tokn} ) {
	unless ( defined $undef_names{$tokn} ) {
	    $undef_names{$tokn} = $name;
	}
    }
    $undef_count{$tokn} = $undef_count{$tokn} + 1;
    $undef_count = $undef_count + 1;
    command_info "processed $count symbols";
}

#
# This one is for reading system map files (System.map).
#
sub read_sysmap()
{
    my $file;
    return unless $#sysmap;
    command_info "processing system map files";
    foreach $file ( @sysmap ) {
	continue unless -r $file;
	unless ( defined $mod_names{'vmlinux'} ) {
	    # command_info "adding sys modulename $name"
	    $mod_names{'vmlinux'} = 'vmlinux';
	}
	command_info "processing system map $file";
	read_map "vmlinux" ( `egrep '\<(_)?__crc_' $file` );
    }
}

#
# This one is for reading in an existing kernel object module (.ko), possibly gzipped, and reading
# the exported and undefined symbols from the module.  These are identified by the A or U flag.
# This is for collecting information about our own kernel modules.  Note that we also have to watch
# out for several special symbols.
#
sub read_module()
{
    my $filename = shift;
    my ($file,$name,$tokn);
    return 1 unless $filename ne '' and -f $filename;
    $name = fnameize($filename);
    return 1 unless $name ne '';
    if ($filename =~ /\.gz$/) {
	$file = ".tmp.$$.$name.ko";
	system("gzip -dc $filename > $file");
    } else {
	$file = $filename;
    }
    unless ( defined $my_mod_names{$name} ) {
	$my_mod_name{$name} = $name;
	$tokn = tokenize($name);
	$mod_path{$tokn} = fpathize($filename);
    }
    command_info "processing module $name in file $filename";
    read_map $name ( `nm -s $file | egrep '(\<A\>|\<U\>|\<(_)?init_module\>|\<(_)?cleanup_module\>|\<(_)?__this_module\>)'` );
    system("rm -f -- $file") unless $file eq $filename;
}

#
# Read in all the modules
#
sub read_modules()
{
    if ($modules ne '') {
	command_info "processing modules"
	foreach $filename ($modules) {
	    continue unless -f $filename -a -r $filename;
	    read_module "$filename"
	}
    }
}

#
# This one is fo reading in an existing kernel object module (.ko), possibly gzipped, and reading the
# exported symbols from the module.  These are identified by the __crc_ prefix.  This is in case we
# do not have a Module.symvers hanging around and need to collect the symbol information from the
# existing kernel modules themselves.
#
sub read_kobject($)
{
    my $filename = shift;
    my ($file,$name,$base,$tokn);
    return 1 unless $filename ne '' and -f $filename;
    # not all files have appropriate syms
    return 0 unless system("zgrep -E -qcm1 '\<(_)?__crc_' $filename");
    $name = fnameize($filename);
    if ($filename =~ /\.gz$/) {
	# shoot, we have a .gz ending
	$file = ".tmp.$$.$name.ko";
	system("gzip -dc $filename > $file");
    } else {
	$file = $filename;
    }
    unless ( defined $mod_names{$name} ) {
	# command_info "adding sys module name $filename"
	$mod_names{$name} = $name;
	$tokn = tokenize($name);
	$mod_path{$tokn} = fpathize($filename);
    }
    $filename =~ /^($moddir\/)?(kernel\/)?/;
    $base = $';
    command_info "processing module $name in file $base";
    read_map $name ( `nm -s $file | egrep '\<(_)?__crc_'` );
    system("rm -f -- $file") unless $file eq $filename;
}

sub read_kobjects()
{
    if ($moddir ne '' and -d $moddir) {
	comand_info "processing kernel object files";
	foreach $kobj ( shell("find $moddir -name '*.ko' -o -name '*.ko.gz' 2>/dev/null") ) {
	    read_kobject "$kobj";
	}
    }
}

sub process_command()
{
    # .modpost is just a (quick) way of caching system maps and module maps
    if ($cachefile ne '' and -f $cachefile) {
	command_info "processing cache file $cachefile";
	$cache_dirty=no;
    } else {
	command_info "sysmaps=`$sysmap'" unless $sysmap eq '';
	read_sysmap;
	command_info "infiles=`$infile'" unless $infile eq '';
	read_infiles;
	command_info "moddirs=`$moddir'" unless $moddir eq '';
	read_kobjects;
    }
    if ($modules ne '') {
	command_info "modules=`$modules'" unless $modules eq '';
	read_modules;
	write_sources;
	command_info "outfile=`$outfile'" unless $outfile eq '';
	write_outfile;
	command_info "sysfile=`$sysfile'" unless $sysfile eq '';
	write_sysfile;
    }
    if ($cachefile ne '') {
	if ($cache_dirty eq 'yes') {
	    command_info "writing cache file $cachefile"
	    write_cache $cachefile;
	} elsif (not -f $cachefile) {
	    command_info "creating cache file $cachefile"
	    system("cat /dev/null >$cachefile");
	}
    }
}

#
# The symvers file format is 0x%08x\t%s\t%s\n crc symname modname.
#
sub none_command()
{
    if ($show_version.$show_help.$show_copying ne '') {
	print "Displaying information" unless $verbose <= 2;
	version unless $show_version eq '';
	help    unless $show_help    eq '';
	copying unless $show_copying eq '';
    }
}

if ($command =~ /^(none|process)$/) {
    eval "$commmand\_command";
} else {
    usage;
    exit 1;
}

exit $retval;

# =============================================================================
#
# $Log: modpost.pl,v $
# Revision 1.1.2.3  2011-01-12 03:44:13  brian
# - update awk scripts and work around gawk close bug
#
# Revision 1.1.2.2  2009-06-21 12:41:53  brian
# - updated headers
#
# Revision 1.1.2.1  2009-06-21 11:16:41  brian
# - added files to new distro
#
# Revision 0.9.2.1  2008-10-29 18:14:57  brian
# - commit working files for release
#
# =============================================================================
# vim: ft=perl sw=4
