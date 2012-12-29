#!/usr/bin/perl
# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
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
# Last Modified $Date$ by $Author$
#
# =============================================================================

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $progname = $0;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;

my $program = $0; $program =~ s/^.*\///;
my $ident = '$RCSfile$ $Name$($Revision$) $Date$';
my $version = '$Revision$';
my $date = '$Date$';
my $title = 'OpenSS7 PCAP to PCAP-NG Converter';

my $Title = <<EOF;
Linux Fast-STREAMS
$title
$version, $date
EOF

$Title =~ s/\n*$//;
$Title =~ s/^\n*//;

my $Version = <<EOF;
Version $version
$ident
Copyright (c) 2008-2012  Monavacon Limited.    All Rights Reserved.
Copyright (c) 2001-2008  OpenSS7 Corporation.  All Rights Reserved.
Copyright (c) 1997-2000  Brian F. G. Bidulock. All Rights Reserved.
Distributed under Affero GPL Version 3, included here by reference.
See `$program --copying' for copying permissions.
EOF

$Version =~ s/\n*$//;
$Version =~ s/^\n*//;

my $Copying = <<EOF;
________________________________________________________________________________

$ident
________________________________________________________________________________

Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock\@openss7.org>

All Rights Reserved.
________________________________________________________________________________

This program is free software; you can redistribute it and/or modify it under the terms of the GNU Affero General Public License as published by the Free Software Foundation; version 3 of the License.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License along with this program.   If not, see <http://www.gnu.org/licenses/>, or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
________________________________________________________________________________

U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on behalf of the U.S. Government ("Government"), the following provisions apply to you. If the Software is supplied by the Department of Defense ("DoD"), it is classified as "Commercial Computer Software" under paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any successor regulations) and the Government is acquiring only the license rights granted herein (the license rights customarily provided to non-Government users). If the Software is supplied to any unit or agency of the Government other than DoD, it is classified as "Restricted Computer Software" and the Government's rights in the Software are defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations).
________________________________________________________________________________

Commercial licensing and support of this software is available from OpenSS7 Corporation at a fee.  See http://www.openss7.com/
________________________________________________________________________________
EOF

$Copying =~ s/\n*$//;
$Copying =~ s/^\n*//;

my $CopyingMono = <<EOF;
--------------------------------------------------------------------------------
$ident
--------------------------------------------------------------------------------
Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock\@openss7.org>

All Rights Reserved.
--------------------------------------------------------------------------------
This program is free software; you can  redistribute  it and/or modify  it under
the terms  of the  GNU Affero General Public License  as  published by the  Free
Software Foundation; version 3 of the License.

This program is distributed in the hope that it will  be useful, but WITHOUT ANY
WARRANTY; without even  the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Affero General Public License for more details.

You should have received a copy of the  GNU Affero General Public License  along
with this program.   If not,  see  <http://www.gnu.org/licenses/>,  or  write to
the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

$CopyingMono =~ s/\n*$//;
$CopyingMono =~ s/^\n*//;

my $Help = <<EOF;
$program:
    $ident
Usage:
    $program [options] [MODULE ...]
    $program {-h|--help}
    $program {-V|--version}
    $program {-C|--copying}
Arguments:
    NONE
Options:
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

$Help =~ s/\n*$//;
$Help =~ s/^\n*//;

my $Usage = <<EOF;
$program
    $ident
Usage:
    $program [options] [MODULE ...]
    $program {-h|--help}
    $program {-V|--version}
    $program {-C|--copying}
EOF

$Usage =~ s/\n*$//;
$Usage =~ s/^\n*//;

my $debug	= 0;
my $trace	= 0;
my $verbose	= 0;
my @infiles	= ( );
my $infile	= "/dev/stdin";
my $outfile	= "/dev/stdout";

while ($ARGV[0]=~/^-/) {
	$_ = shift;
	last if /^--$/;
	if (/^-n/) { $nflag++;	     next; }
	if (/^-d/) { $debug	= 1; next; }
	if (/^-t/) { $trace	= 1; next; }
	if (/^-v/) { $verbose	= 1; next; }
	if (/^-f(.*)/) { if ($1) { push @infiles, $1; } else { push @infiles, shift; } next; }
	if (/^-o(.*)/) { if ($1) { $outfile = $1; } else { $outfile = shift; } next; }
	if (/^--help/) {
		print<<EOF;
Usage:
  $0 [-d] [-t] [-v] [ [-f] infile [ [-o] outfile ] ]
     -d - turn on debug mode
     -t - turn on trace mode (lost of info)
     -v - turn on verbose mode (lots of info)
     [-f] infile  -  input file (default: $infile)
     [-o] outfile - output file (default: $outfile)
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

foreach my $file (@ARGV) {
	push @infiles, $file;
}

$printit++ unless $nflag;

#$\ = "\n";	# add new-line to print
#$* = 0;	$ do singleline matching
undef $/;	# read entire file

use strict;
use warnings;
use Carp;
use Data::Dumper;
use Date::Parse;
use Time::HiRes qw(gettimeofday);
use Net::Pcap qw(:functions);
use FindBin qw($Bin);
use UUID;
use Digest::MD5 qw(md5);
use lib "$Bin";

require "Pcapng.pm";


# -------------------------------------
package main; use strict;
# -------------------------------------
use Digest::MD5 qw(md5);

use constant {
	DLT_ETHERNET	    =>   1,
	DLT_LINUX_SLL	    => 113,
	DLT_MTP2_WITH_PHDR  => 139,
	DLT_MTP2	    => 140,
	DLT_MTP3	    => 141,
	DLT_SCCP	    => 142,

	PCAPNG_FLAG_DIR_UNKNOWN	    => 0x0000000,
	PCAPNG_FLAG_DIR_INBOUND	    => 0x0000001,
	PCAPNG_FLAG_DIR_OUTBOUND    => 0x0000002,
	PCAPNG_FLAG_DIR_INVALID	    => 0x0000003,

	PCAPNG_FLAG_TYP_UNSPEC	    => 0x0000000,
	PCAPNG_FLAG_TYP_UNICAST	    => 0x0000004,
	PCAPNG_FLAG_TYP_MULTICAST   => 0x0000008,
	PCAPNG_FLAG_TYP_BROADCAST   => 0x000000c,
	PCAPNG_FLAG_TYP_PROMISCUOUS => 0x0000010,
};

if (scalar @infiles == 0) {
	@infiles = ( $infile );
}

my $fh;
open($fh, ">:bytes", $outfile) or die;

my $uuid;
UUID::generate($uuid);

my $unamea = `uname -a`;
my $unamem = `uname -m`;

Net::Pcapng::pcapng_write_shb($fh,{
		major=>1,
		minor=>0,
		comment=>"Converted from PCAP file by pcap2ng.pl",
		hardware=>$unamem,
		os=>$unamea,
		userappl=>"pcap2ng.pl 1.0",
		uuid=>$uuid,
	});

my @pcaps = ();
foreach my $file (@infiles) {
	my $err = '';
	my $pcap = Net::Pcap::pcap_open_offline($file, \$err) or die;
	my $dlt = Net::Pcap::pcap_datalink($pcap);
	push @pcaps, [$pcap,$dlt];
}

my $breakout = 1;
my $if_count = 0;
my %interfaces = ();

sub get_if_by_ppa {
	my ($f,$ppa,$xsn,$dlt) = @_;
	unless (exists $interfaces{$ppa}) {
		my ($comment,$name,$desc,$speed,$physaddr);
		if ($dlt >= DLT_MTP2_WITH_PHDR && $dlt <= DLT_SCCP) {
			my $card = ($ppa >> 7) & 0x3;
			my $span = ($ppa >> 5) & 0x3;
			my $slot = ($ppa >> 0) & 0x1f;
			$comment = "SS7 X400P Card Link";
			$name = "x400p-ss7:$card:$span:$slot";
			$desc = "X400P-SS7 E1/T1/J1 Interface Card $card, Span $span, Slot $slot";
			$speed = $xsn ? 1360000 : 64000;
			$physaddr = {value=>pack('CCC',$card,$span,$slot),length=>3};
		}
		my $idbi = {
			interfaceid=>$if_count,
			linktype=>$dlt,
			reserved=>0,
			snaplen=>65535,
			comment=>$comment,
			name=>$name,
			desc=>$desc,
			speed=>$speed,
			tsresol=>0,
			tzone=>0,
			filter=>{value=>'',length=>0},
			os=>$unamea,
			fcslen=>0,
			tsoffset=>[0,0],
			physaddr=>$physaddr,
			uuid=>$uuid,
			ifindex=>$ppa,
		};
		my $isbi = {
			interfaceid=>$if_count,
			ts=>[0,0],
			comment=>"Interface statistics",
#			starttime=>[0,0],
#			endtime=>[0,0],
			ifrecv=>0,
			ifdrop=>0,
			filteraccept=>0,
			osdrop=>0,
			usrdeliv=>0,
			ifcomp=>0,
		};
		Net::Pcapng::pcapng_write_idb($fh,$idbi);
		$if_count++;
		$interfaces{$ppa} = [$idbi,$isbi];
	}
	return @{$interfaces{$ppa}};
}

my %hdr;
my $buf;

while (@pcaps > 0) {
	my @elems = (@pcaps);
	@pcaps = ();
	foreach my $inp (@elems) {
		my ($pcap,$dlt) = @$inp;
		if (Net::Pcap::pcap_next_ex($pcap,\%hdr,\$buf) == 1) {
			my ($idbi,$isbi);
			my $ts = [ int(($hdr{tv_sec}*1000000)/4294967296),
				   ($hdr{tv_sec}*1000000+$hdr{tv_usec})&0xffffffff ];
			my $newdlt = $dlt;
			my $flags;
			my ($way,$xsn,$ppa) = (0,0,0);
			if ($breakout && $dlt == DLT_MTP2_WITH_PHDR) {
				($way,$xsn,$ppa) = unpack('CCn',substr($buf,0,4));
				$buf = substr($buf,4);
				$hdr{caplen} -= 4;
				$hdr{len} -= 4;
				$newdlt = DLT_MTP2;
			}
			if ($way == 0x0) {
				$flags = 0;
			} elsif ($way == 0x1) {
				$flags = PCAPNG_FLAG_DIR_INBOUND;
			} elsif ($way == 0x2) {
				$flags = PCAPNG_FLAG_DIR_OUTBOUND;
			} elsif ($way == 0x3) {
				$flags = PCAPNG_FLAG_TYP_PROMISCUOUS;
			}
			my $hash = pack('C',3).md5($buf);
			($idbi,$isbi) = get_if_by_ppa($fh,$ppa,$xsn,$newdlt);
			$isbi->{starttime} = $ts unless $isbi->{starttime};
			$isbi->{endtime} = $isbi->{ts} = $ts;
			$isbi->{ifrecv}++;
			$isbi->{filteraccept}++;
			$isbi->{usrdeliv}++;
			Net::Pcapng::pcapng_write_epb($fh,{
				interfaceid=>$idbi->{interfaceid},
				ts=>$ts,
				captlen=>$hdr{caplen},
				packlen=>$hdr{len},
				datalen=>length($buf),
				flags=>$flags,
				hash=>{value=>$hash,length=>length($hash)},
				dropcount=>0,
				compcount=>0,
			},$buf);
			push @pcaps, $inp;
		} else {
			Net::Pcap::pcap_close($pcap);
		}
	}
}

foreach my $val (values %interfaces) {
	my ($idbi,$isbi) = @$val;
	Net::Pcapng::pcapng_write_isb($fh,$isbi);
}

close($fh);

exit;

1;

__END__

# =============================================================================
#
# $Log$
# =============================================================================


