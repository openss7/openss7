#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $progname = $0;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;

my $program = $0; $program =~ s/^.*\///;
my $ident = '$RCSfile: scapident.pl,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2010-11-28 14:34:48 $';
my $version = '$Revision: 1.1.2.3 $';
my $date = '$Date: 2010-11-28 14:34:48 $';
my $title = 'OpenSS7 SS7 Analyzer';

my $debug	= 0;
my $trace	= 0;
my $verbose	= 0;
my $indefault	= "/dev/stdin";
my $outdefault	= "/dev/stdout";
my $infile;
my $outfile;
my @args = ();
my $nomoreopts = 0;

while (@ARGV) {
	$_ = shift;
	unless (/^-/ or $nomoreopts) {
		push @args, $_;
		next;
	}
	if (/^--$/) {
		$nomoreopts = 1;
		next;
	}
	if (/^-n/) { $nflag++;	    next; }
	if (/^-d/) { $debug = 1;    next; }
	if (/^-t/) { $trace = 1;    next; }
	if (/^-v/) { $verbose = 1;  next; }
	if (/^-f(.*)/) { if ($1) { $infile  = $1; } else { $infile  = shift; } next; }
	if (/^-o(.*)/) { if ($1) { $outfile = $1; } else { $outfile = shift; } next; }
	if (/^--help/) {
		print<<EOF;
Usage:
  $0 [-d] [-t] [-v] [ [-f] infile [ [-o] outfile ] ]
     -d - turn on debug mode
     -t - turn on trace mode (lost of info)
     -v - turn on verbose mode (lots of info)
     [-f] infile  -  input file (default: $indefault)
     [-o] outfile - output file (default: $outdefault)
     --help - print usage information
     --version - print version information
     --copying - print copying permissions
EOF
		exit 0;
	}
	if (/^--version/) {
		print<<'EOF';
$RCSfile$ $Name$($Revision$) $Date$
Copyright (c) 2008, 2009, 2010, 2011, 2012  Monavacon Limited.
Copyright (c) 2002, 2003, 2004, 2005, 2006, 2007  OpenSS7 Corporation.
Copyright (c) 1996, 1997, 1998, 1999, 2000, 2001  Brian Bidulock.
All Rights Reserved.  Distributed my OpenSS7 Corporation under AGPL
Version 3, included here by reference.  See `snmpcheck --copying' for
copying permissions.
EOF
		exit 0;
	}
	if (/^--copying/) {
		print<<'EOF';
-----------------------------------------------------------------------------
# @(#) $RCSfile$ $Name$($Revision$) $Date$
-----------------------------------------------------------------------------
Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the Free
Software Foundation; version 3 of the License.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
details.

You should have received a copy of the GNU Affero General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
-----------------------------------------------------------------------------
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
behalf of the U.S. Government ("Government"), the following provisions apply
to you.  If the Software is supplied by the Department of Defense ("DoD"), it
is classified as "Commercial Computer Software" under paragraph 252.227-7014
of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
successor regulations) and the Government is acquiring only the license rights
granted herein (the license rights customarily provided to non-Government
users).  If the Software is supplied to any unit or agency of the Government
other than DoD, it is classified as "Restricted Computer Software" and the
Government's rights in the Software are defined in paragraph 52.227-19 of the
Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
(or any successor regulations).
-----------------------------------------------------------------------------
Commercial licensing and support of this software is available from OpenSS7
Corporation at a fee.  See http://www.openss7.com/
-----------------------------------------------------------------------------
EOF
		exit 0;
	}
	if (/^--copying/) {
		print<<'EOF';
-----------------------------------------------------------------------------
# @(#) $RCSfile$ $Name$($Revision$) $Date$
-----------------------------------------------------------------------------
Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

All Rights Reserved.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Affero General Public License as published by the Free
Software Foundation; version 3 of the License.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
details.

You should have received a copy of the GNU Affero General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>, or write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
-----------------------------------------------------------------------------
U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
behalf of the U.S. Government ("Government"), the following provisions apply
to you.  If the Software is supplied by the Department of Defense ("DoD"), it
is classified as "Commercial Computer Software" under paragraph 252.227-7014
of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
successor regulations) and the Government is acquiring only the license rights
granted herein (the license rights customarily provided to non-Government
users).  If the Software is supplied to any unit or agency of the Government
other than DoD, it is classified as "Restricted Computer Software" and the
Government's rights in the Software are defined in paragraph 52.227-19 of the
Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
(or any successor regulations).
-----------------------------------------------------------------------------
Commercial licensing and support of this software is available from OpenSS7
Corporation at a fee.  See http://www.openss7.com/
-----------------------------------------------------------------------------
EOF
		exit 0;
	}
	print STDERR <<EOF;
I: $0 Usage:
I:   $0 [-d] [-t] [-v] [ [-f] infile [ [-o] outfile ] ]
I:      -d - turn on debug mode
I:      -t - turn on trace mode (lots of info)
I:      -v - turn on verbose mode (lots of info)
I:      -x - report crossreferences in output
I:      [-f] infile  -  input file (default stdin)
I:      [-o] outfile - output file (default stdout)
I:      --help - print usage information
I:      --version - print version information
I:      --copying - print copying permissions
EOF
	die "E: I don't recognize this switch: $_\n";
}

$infile  = shift @args unless defined $infile;
$outfile = shift @args unless defined $outfile;

die "E: excess arguments ".join(' ',@args)."\n" if @args;

$infile  = $indefault  unless defined $infile;
$outfile = $outdefault unless defined $outfile;

if ($debug) {
	print STDERR "Output file: $outfile\n";
	print STDERR "Input  file: $infile\n";
}

$printit++ unless $nflag;

#$\ = "\n";	# add new-line to print
#$* = 0;	$ do singleline matching
undef $/;	# read entire file

use Carp;
use Net::Pcap qw(:bpf :datalink :pcap :mode :openflag :functions);
use Date::Parse;
use Data::Dumper;

my $err = '';
my $pcap_i;
unless ($pcap_i = pcap_open_offline($infile, \$err)) {
	die "E: could not open file $infile\n"."E: error was: $err\n";
}
my $type_i = pcap_datalink($pcap_i);
my $snap_i = pcap_snapshot($pcap_i);

die "E: input file has wrong data link type: $type_i (".pcap_datalink_val_to_name($type_i).")\n"
	unless $type_i == 139;

my $type_o = 1;
my $snap_o = 9000;
my $pcap_o;
unless ($pcap_o = pcap_open_dead($type_o,$snap_o)) {
	die "E: could not get pcap descriptor\n"."E: error was: ".pcap_geterr($pcap_o)."\n";
}
my $dumper;
unless ($dumper = pcap_dump_open($pcap_o, $outfile)) {
	die "E: could not open file $output\n"."E: error was: ".pcap_geterr($pcap_o)."\n";
}
my $count = 0;
for (;;) {
	my %hdr;
	my $dat;
	my $ret = pcap_next_ex($pcap_i, \%hdr, \$dat);
	last if $ret != 1;
	$count++;
	printf STDOUT "\r$count";
	#print "  length = $hdr{len}\n";
	#print "  caplen = $hdr{caplen}\n";
	#print " seconds = $hdr{tv_sec}\n";
	#print "useconds = $hdr{tv_usec}\n";
}
pcap_close($pcap_i);
pcap_dump_flush($dumper);
pcap_dump_close($dumper);
