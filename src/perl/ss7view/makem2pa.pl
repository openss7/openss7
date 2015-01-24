#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $progname = $0;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;

my $program = $0; $program =~ s/^.*\///;
my $ident = 'src/perl/ss7view/scapident.pl 2015-01-24T12:32+0000';
my $version = '1.1.2.3';
my $date = '2010-11-28 14:34:48';
my $title = 'OpenSS7 SS7 Analyzer';

my $debug	= 0;
my $trace	= 0;
my $verbose	= 0;
my $indefault	= "/dev/stdin";
my $outdefault	= "/dev/stdout";
my $infile;
my $outfile;
my @args = ();
my $nomoreopts	= 0;
my $do_m2pa	= 0;
my $do_m2ua	= 0;
my $do_m3ua	= 0;
my $do_sua	= 0;
my $cooked	= 0;

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
	if (/^-c/) { $cooked = 1;   next; }
	if (/^-m2pa/) { $do_m2pa = 1;  next; }
	if (/^-m2ua/) { $do_m2ua = 1;  next; }
	if (/^-m3ua/) { $do_m3ua = 1;  next; }
	if (/^-sua/)  { $do_sua  = 1;  next; }
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
		print<<EOF;
$ident
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
		print<<EOF;
-----------------------------------------------------------------------------
# @(#) $ident
-----------------------------------------------------------------------------
Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock\@openss7.org>

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
		print<<EOF;
-----------------------------------------------------------------------------
# @(#) $ident
-----------------------------------------------------------------------------
Copyright (c) 2008-2012  Monavacon Limited <http://www.monavacon.com/>
Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock\@openss7.org>

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

$do_m2pa = 1 unless $do_m2ua or $do_m3ua or $do_sua;

$printit++ unless $nflag;

#$\ = "\n";	# add new-line to print
#$* = 0;	$ do singleline matching
undef $/;	# read entire file

use Carp;
use Net::Pcap qw(:bpf :datalink :pcap :mode :openflag :functions);
use Date::Parse;
use Data::Dumper;

#---------------------------------
package Cooked;
use strict;
#---------------------------------

sub new {
	my ($type,$ppa,$cook) = @_;
	my $self = {};
	bless $self,$type;

	$self->{ppa} = $ppa;
	$self->{card} = ($ppa >> 7) & 0x03;
	$self->{span} = ($ppa >> 5) & 0x03;
	$self->{slot} = ($ppa >> 0) & 0x1f;

	$self->{cook} = $cook;

	return $self;
}

sub wrap {
	my ($self,$msg) = @_;

	return unless $self->{cook};

	# add a linux cooked header to the packet
	my $dat = '';
	$dat .= pack('n', 3); # sent by somebody else to somebody else
	$dat .= pack('n', 1); # ARPHRD_ value for link layer
	$dat .= pack('n', 6); # 6-byte ethernet address
	if ($msg->{dir}) {
		$dat .= $self->{ohwa} . pack('C*', 0, 0);
	} else {
		$dat .= $self->{thwa} . pack('C*', 0, 0);
	}
	$dat .= pack('C*', 0x08, 0x00); # IP ethernet frame
	$msg->{dat} = $dat . $msg->{dat};
}



#---------------------------------
package EthPair;
use strict;
use vars qw(@ISA);
@ISA = qw(Cooked);
#---------------------------------

sub new {
	my ($type,$ppa,$cook,@args) = @_;

	my $self = Cooked::new($type,$ppa,$cook,@args);

	$self->{ohwa} = pack('C*', 0x00, 0x1c, 0xf0, 0x9f, 0x11, 0x5c+$self->{span});
	$self->{thwa} = pack('C*', 0x00, 0x15, 0x58, 0xd7, 0xd6, 0x2e+$self->{span});
	my @bytes;
	@bytes = (); foreach (unpack('C*',$self->{ohwa})) { push @bytes, sprintf('%02x', $_); }
	print STDERR "\rI: orig ETH address: ", join(':',@bytes), "\n";
	@bytes = (); foreach (unpack('C*',$self->{thwa})) { push @bytes, sprintf('%02x', $_); }
	print STDERR "\rI: term ETH address: ", join(':',@bytes), "\n";

	return $self;
}

sub wrap {
	my ($self,$msg) = @_;

	return $self->SUPER::wrap($msg) if $self->{cook};

	# adds an ethernet header to a message
	my $dat = '';
	if ($msg->{dir}) {
		$dat .= $self->{thwa} . $self->{ohwa};  # Ethernet addresses
	} else {
		$dat .= $self->{ohwa} . $self->{thwa};  # Ethernet addresses
	}
	$dat .= pack('C*', 0x08, 0x00); # IP ethernet frame
	$msg->{dat} = $dat . $msg->{dat};
}

#---------------------------------
package IPPair;
use strict;
use vars qw(@ISA);
@ISA = qw(EthPair);
#---------------------------------

sub new {
	my ($type,$ppa,$cook,@args) = @_;

	my $self = EthPair::new($type,$ppa,$cook,@args);

	$self->{orig} = pack('N', (192<<24)|(168<<16)|(0<<8)|($self->{span}+0));
	$self->{term} = pack('N', (192<<24)|(168<<16)|(0<<8)|($self->{span}+1));
	print STDERR "\rI: orig IP address: ", join('.',unpack('C*', $self->{orig})), "\n";
	print STDERR "\rI: term IP address: ", join('.',unpack('C*', $self->{term})), "\n";

	return $self;
}

sub wrap {
	my ($self,$msg) = @_;

	# adds an ip header to a message
	my $dat = '';
	my $csum = 0;
	my $ilen = 20;
	my $plen = $ilen + length($msg->{dat});
	$dat .= pack('C*', 0x40 + ($ilen>>2), 0x00); # IP version 4, 20 byte header, 0 dsn
	$dat .= pack('n', $plen); # total length of packet including IP header
	$dat .= pack('n', rand 0x10000); # id
	$dat .= pack('C*', 0x40, 0, 64, 132); # don't fragment, frag offset = 0, ttl = 64, sctp
	my $off = length($dat);
	$dat .= pack('n', $csum);
	if ($msg->{dir}) {
		$dat .= $self->{orig} . $self->{term};
	} else {
		$dat .= $self->{term} . $self->{orig};
	}
	my @bytes = unpack('C*',$dat);
	while (@bytes) { $csum += (shift(@bytes)<<8)|(shift(@bytes)); }
	while ($csum >> 16) { $csum = ($csum & 0xffff) + ($csum >> 16); }
	$csum = (~$csum) & 0xffff;
	substr($dat,$off,2) = pack('n', $csum);
	$msg->{dat} = $dat . $msg->{dat};

	$self->SUPER::wrap($msg);
}

#---------------------------------
package SCTPAssoc;
use strict;
use vars qw(@ISA);
@ISA = qw(IPPair);
#---------------------------------

sub new {
	my ($type,$ppa,$sport,$dport,$ppi,$cook,@args) = @_;

	my $self = IPPair::new($type,$ppa,$cook,@args);

	$self->{oport} = pack('n',$sport);
	$self->{tport} = pack('n',$dport);
	print STDERR "\rI: orig IP port: ",unpack('n',$self->{oport}),"\n";
	print STDERR "\rI: term IP port: ",unpack('n',$self->{tport}),"\n";
	$self->{vtag} = pack('N',0x789abcde + $ppa + $ppi);
	print STDERR "\rI: verification tag: ",unpack('N',$self->{vtag}),"\n";
	$self->{otsn} = pack('N',unpack('N',$self->{vtag}) - 1111);
	$self->{ttsn} = pack('N',unpack('N',$self->{vtag}) + 1111);
	print STDERR "\rI: orig tsn: ",unpack('N',$self->{otsn}),"\n";
	print STDERR "\rI: term tsn: ",unpack('N',$self->{ttsn}),"\n";
	$self->{ossn} = pack('n',1);
	$self->{tssn} = pack('n',1);
	print STDERR "\rI: orig stream sequence number: ",unpack('n',$self->{ossn}),"\n";
	print STDERR "\rI: term stream sequence number: ",unpack('n',$self->{tssn}),"\n";
	$self->{ppi} = pack('N', $ppi);
	print STDERR "\rI: payload protocol identifier: ",unpack('N',$self->{ppi}),"\n";

	return $self;
}

sub sack {
	my ($self,$msg) = @_;

	# add a sack chunk to a message
	my $dat = '';
	$dat .= pack('C*', 0x03, 0x00); # sack chunk
	$dat .= pack('n', 16); # sack is 16 bytes long
	if ($msg->{dir}) {
		$dat .= $self->{ttsn}; # cumm TSN ack
	} else {
		$dat .= $self->{otsn}; # cumm TSN ack
	}
	$dat .= pack('N', 0x10000); # advertised receiver window credit
	$dat .= pack('n', 0); # number of gap ack blocks
	$dat .= pack('n', 0); # number of duplicate TSNs
	$msg->{dat} = $dat . $msg->{dat};
}

sub data {
	my ($self,$msg) = @_;

	# add a data chunk header to a message
	my $dat = '';
	my $clen = 16 + length($msg->{dat});
	my $plen = ($clen + 3) & ~0x03;
	$dat .= pack('C*', 0x00, 0x03); # data chunk, first and last
	$dat .= pack('n', $clen); # chunk length
	if ($msg->{dir}) {
		$self->{otsn} = pack('N', unpack('N', $self->{otsn}) + 1);
		$dat .= $self->{otsn}; # transmit sequence number
	} else {
		$self->{ttsn} = pack('N', unpack('N', $self->{ttsn}) + 1);
		$dat .= $self->{ttsn}; # transmit sequence number
	}
	$dat .= pack('n', 1); # stream 1
	if ($msg->{dir}) {
		$self->{ossn} = pack('n', unpack('n', $self->{ossn}) + 1);
		$dat .= $self->{ossn}; # stream sequence number
	} else {
		$self->{tssn} = pack('n', unpack('n', $self->{tssn}) + 1);
		$dat .= $self->{tssn}; # stream sequence number
	}
	$dat .= $self->{ppi}; # M2PA PPI
	$msg->{dat} = $dat . $msg->{dat} . substr(pack('C*', 0, 0, 0, 0), 0, $plen - $clen); # pad chunk

	$self->sack($msg);
}

sub wrap {
	my ($self,$msg) = @_;

	$self->data($msg);

	# add an sctp header to a message
	my $dat = '';
	my $csum = 0;
	if ($msg->{dir}) {
		$dat .= $self->{oport} . $self->{tport};
	} else {
		$dat .= $self->{tport} . $self->{oport};
	}
	$dat .= $self->{vtag};
	$dat .= pack('N', $csum);
	$msg->{dat} = $dat . $msg->{dat};

	$self->SUPER::wrap($msg);
}

#---------------------------------
package M2PALink;
use strict;
use vars qw(@ISA);
@ISA = qw(SCTPAssoc);
#---------------------------------

my $m2pas = {};

sub new {
	my ($type,$msg,$cook,@args) = @_;

	my $ppa = $msg->{ppa};
	return $m2pas->{$ppa} if $m2pas->{$ppa};
	print STDERR "\rI: discovered new m2pa $ppa\n";
	my $self = SCTPAssoc::new($type,$ppa,3565,3565,5,$cook,@args);

	$self->{osno} = 0;
	$self->{tsno} = 0;
	print STDERR "\rI: orig sn offset: $self->{osno}\n";
	print STDERR "\rI: term sn offset: $self->{tsno}\n";

	$m2pas->{$ppa} = $self;
	return $self;
}

sub wrap {
	my ($self,$msg) = @_;

	# add an m2pa header to a message
	my $dat = '';
	my $slen = length($msg->{dat}) - 2; # without sequence numbers
	my $mlen = 16 + $slen;
	$dat .= pack('C*', 1, 0, 11, 1); # M2PA version, reserved, class, type
	$dat .= pack('N', $mlen); # M2PA message length including header
	if ($msg->{dir}) {
		# FIXME: this is not quite right
		$dat .= pack('N', ($msg->{bsn} + $self->{tsno}) & 0xffffff); # fsnr
		$self->{tsno} += 0x80 unless $msg->{bsn};
		$dat .= pack('N', ($msg->{fsn} + $self->{osno}) & 0xffffff); # fsnt
		$self->{osno} += 0x80 unless $msg->{fsn};
	} else {
		$dat .= pack('N', ($msg->{bsn} + $self->{osno}) & 0xffffff); # fsnr
		$self->{osno} += 0x80 unless $msg->{bsn};
		$dat .= pack('N', ($msg->{fsn} + $self->{tsno}) & 0xffffff); # fsnt
		$self->{tsno} += 0x80 unless $msg->{fsn};
	}
	$dat .= pack('C', $msg->{li0} << 6); # li spare bits
	$msg->{dat} = $dat . substr($msg->{dat},3); # strip bsn, fsn, li

	$self->SUPER::wrap($msg);

	$msg->{hdr}->{len} = $msg->{hdr}->{caplen} = length($msg->{dat});
}

#---------------------------------
package M2UALink;
use strict;
use vars qw(@ISA);
@ISA = qw(SCTPAssoc);
#---------------------------------

my $m2ua;

sub new {
	my ($type,$msg,$cook,@args) = @_;

	return $m2ua if $m2ua;
	print STDERR "\rI: discovered new m2ua\n";
	my $ppa = $msg->{ppa};
	my $self = SCTPAssoc::new($type,$ppa,2904,2904,2,$cook,@args);

	$m2ua = $self;
	return $self;
}

sub wrap {
	my ($self,$msg) = @_;

	# add an m2ua header to a message
	my $dat = '';
	my $dlen = length($msg->{dat}) - 3; # without sequence or length indicator
	$dat .= pack('C*', 1, 0, 6, 1); # M2UA version ,reserved, class type
	$dat .= pack('N', 8 + 8 + 4 + $dlen); # message length
	$dat .= pack('n*', 0x0001, 8); # tag 1, len 8 - integer iid
	$dat .= pack('N', $msg->{ppa}); # iid
	$dat .= pack('n*', 0x0300, 4 + $dlen); # tag 0x300, len - protocol data
	$msg->{dat} = $dat . substr($msg->{dat},3); # strip bsn, fsn, li

	$self->SUPER::wrap($msg);

	$msg->{hdr}->{len} = $msg->{hdr}->{caplen} = length($msg->{dat});
}

#---------------------------------
package M3UALink;
use strict;
use vars qw(@ISA);
@ISA = qw(SCTPAssoc);
#---------------------------------

my $m3uas = {};

sub new {
	my ($type,$msg,$cook,@args) = @_;

	my $ppa = $msg->{ppa};
	return $m3uas->{$ppa} if $m3uas->{$ppa};
	print STDERR "\rI: discovered new m3ua $ppa\n";
	my $self = SCTPAssoc::new($type,$ppa,2905,2905,3,$cook,@args);

	$m3uas->{$ppa} = $self;
	return $self;
}

sub wrap {
	my ($self,$msg) = @_;

	# add an m3ua header to a message
	my $dat = '';
	my $dlen = length($msg->{dat}) - 11; # skip routing label
	my $pads = (($dlen+3)&(~0x3)) - $dlen;
	$dat .= pack('C*', 1, 0, 1, 1); # M3UA version transfer payload
	$dat .= pack('N', 8 + 8 + 16 + $dlen + $pads); # message length including header
	$dat .= pack('n*', 0x0006, 8); # tag/len for routing context
	$dat .= pack('N', 1); # always RC 1
	$dat .= pack('n*', 0x0210, 16 + $dlen); # tag/len for protocol data
	$dat .= pack('N*', $msg->{opc}, $msg->{dpc});
	$dat .= pack('C*', $msg->{si}, $msg->{ni}, $msg->{mp}, $msg->{sls});
	$dat .= substr($msg->{dat},11);
	$dat .= substr(pack('C*', 0, 0, 0, 0),0,$pads);

	$msg->{dat} = $dat;

	$self->SUPER::wrap($msg);

	$msg->{hdr}->{len} = $msg->{hdr}->{caplen} = length($msg->{dat});
}

#---------------------------------
package SUALink;
use strict;
use vars qw(@ISA);
@ISA = qw(SCTPAssoc);
#---------------------------------

my $suas = {};

sub new {
	my ($type,$msg,$cook,@args) = @_;

	my $ppa = $msg->{ppa};
	return $suas->{$ppa} if $suas->{$ppa};
	print STDERR "\rI: discovered new sua $ppa\n";
	my $self = SCTPAssoc::new($type,$ppa,14001,14001,4,$cook,@args);

	$suas->{$ppa} = $self;
	return $self;
}

sub wrap {
	my ($self,$msg) = @_;

	# add an sua header to a message
	my $dat = '';
	my $dlen = length{$msg->{sccpdat}};
	my $pads = (($dlen+3)&(~0x3)) - $dlen;

	my $sagtlen = 8 + length($msg->{sa}->{gt}->{digits}) if exists $msg->{sa}->{gt};
	my $sagtpad = (($sagtlen+3)&(~0x3)) - $sagtlen;
	my $salen = 4;
	$salen += 8 if exists $msg->{sa}->{ssn};
	$salen += 8 if exists $msg->{sa}->{pc};
	$salen += $sagtlen if exists $msg->{sa}->{gt};
	my $sapad = (($salen+3)&(~0x3)) - $salen;

	my $dagtlen = 8 + length($msg->{da}->{gt}->{digits}) if exists $msg->{da}->{gt};
	my $dagtpad = (($dagtlen+3)&(~0x3)) - $dagtlen;
	my $dalen = 4;
	$dalen += 8 if exists $msg->{da}->{ssn};
	$dalen += 8 if exists $msg->{da}->{pc};
	$dalen += $dagtlen if exists $msg->{da}->{gt};
	my $dapad = (($dalen+3)&(~0x3)) - $dalen;

	my $mlen = 8 + 8 + 8 + 4 + $salen + $sapad + 4 + $dalen + $dapad + 4 + $dlen + $pads;
	$mlen += 8 if exists $msg->{hop};
	$mlen += 8 if exists $msg->{pri};
	$dat .= pack('C*', 1, 0, 7, 1); # SUA connectionsless data transfer
	$dat .= pack('N', 8 + $dlen + $pads);
	$dat .= pack('n*', 0x0006, 8); # routing context
	$dat .= pack('N', 1);
	$dat .= pack('n*', 0x0115, 8); # protocol class
	$dat .= pack('N', $msg->{pcls});
	$dat .= pack('n*', 0x0102, 4 + $salen); # source address
	$dat .= pack('n*', $msg->{sa}->{ri}, $msg->{sa}->{ai});
	if (exists $msg->{sa}->{ssn}) {
		$dat .= pack('n*', 0x8003, 8); # subsystem number
		$dat .= pack('N', $msg->{sa}->{ssn});
	}
	if (exists $msg->{sa}->{pc}) {
		$dat .= pack('n*', 0x8002, 8); # point code
		$dat .= pack('N', $msg->{sa}->{pc});
	}
	if (exists $msg->{sa}->{gt}) {
		$dat .= pack('n*', 0x8001, 8 + $sagtlen);
		$dat .= pack('C*', $msg->{sa}->{gt}->{no},
				   $msg->{sa}->{gt}->{tt},
				   $msg->{sa}->{gt}->{np},
				   $msg->{sa}->{gt}->{na});
		$dat .= $msg->{sa}->{gt}->{digits};
		$dat .= substr(pack('C*', 0, 0, 0, 0),0,$sagtpad);
	}
	$dat .= substr(pack('C*', 0, 0, 0, 0),0,$sapad);
	$dat .= pack('n*', 0x0103, 4 + $dalen); # destination address
	$dat .= pack('n*', $msg->{da}->{ri}, $msg->{da}->{ai});
	if (exists $msg->{da}->{ssn}) {
		$dat .= pack('n*', 0x8003, 8); # subsystem number
		$dat .= pack('N', $msg->{da}->{ssn});
	}
	if (exists $msg->{da}->{pc}) {
		$dat .= pack('n*', 0x8002, 8); # point code
		$dat .= pack('N', $msg->{da}->{pc});
	}
	if (exists $msg->{da}->{gt}) {
		$dat .= pack('n*', 0x8001, 8 + $dagtlen);
		$dat .= pack('C*', $msg->{da}->{gt}->{no},
				   $msg->{da}->{gt}->{tt},
				   $msg->{da}->{gt}->{np},
				   $msg->{da}->{gt}->{na});
		$dat .= $msg->{da}->{gt}->{digits};
		$dat .= substr(pack('C*', 0, 0, 0, 0),0,$dagtpad);
	}
	$dat .= substr(pack('C*', 0, 0, 0, 0),0,$dapad);
	$dat .= pack('n*', 0x0116, 8); # sequence control
	$dat .= pack('N', $msg->{sls});
	if (exists $msg->{hop}) {
		$dat .= pack('n*', 0x0101, 8); # hop counter
		$dat .= pack('N', $msg->{hop});
	}
	if (exists $msg->{pri}) {
		$dat .= pack('n*', 0x0114, 8); # message priority
		$dat .= pack('N', $msg->{pri});
	}
	$dat .= pack('n*', 0x010b, 4 + $dlen); # data
	$dat .= $msg->{sccpdat};
	$dat .= substr(pack('C*', 0, 0, 0, 0),0,$pads);

	$msg->{dat} = $dat;

	$self->SUPER::wrap($msg);

	$msg->{hdr}->{len} = $msg->{hdr}->{caplen} = length($msg->{dat});
}


#---------------------------------
package PcapOutput;
use strict;
#---------------------------------

sub new {
	my ($type,$file,$cook) = @_;

	my $self = {};
	bless $self,$type;

	if ($cook) {
		$self->{type} = 113;  # linux cooked capture
	} else {
		$self->{type} = 1; # ethernet
	}
	$self->{snap} = 9000;
	unless ($self->{pcap} = Net::Pcap::pcap_open_dead($self->{type},$self->{snap})) {
		die "E: ".ref($self).": could not get pcap descriptor\n".
		    "E: ".ref($self).": error was: ".Net::Pcap::pcap_geterr($self->{pcap})."\n";
	}
	unless ($self->{dumper} = Net::Pcap::pcap_dump_open($self->{pcap}, $file)) {
		die "E: ".ref($self).": could not open file $file\n".
		    "E: ".ref($self).": error was: ".Net::Pcap::pcap_geterr($self->{pcap})."\n";
	}

	return $self;
}

sub destroy {
	my $self = shift;
	if ($self->{dumper}) {
		Net::Pcap::pcap_dump_flush($self->{dumper});
		Net::Pcap::pcap_dump_close($self->{dumper});
	}
	delete $self->{dumper};
}

sub DESTROY {
	shift->destroy;
}

sub putmsg {
	my ($self,$msg) = @_;
	Net::Pcap::pcap_dump($self->{dumper},$msg->{hdr},$msg->{dat});
}

#---------------------------------
package PcapInput;
use strict;
#---------------------------------

sub new {
	my ($type,$file) = @_;
	my $self = {};
	bless $self,$type;

	my $err = '';
	unless ($self->{pcap} = Net::Pcap::pcap_open_offline($file, \$err)) {
		die "E: ".ref($self).": could not open file $file\n".
		    "E: ".ref($self).": error was: $err\n";
	}
	$self->{type} = Net::Pcap::pcap_datalink($self->{pcap});
	$self->{snap} = Net::Pcap::pcap_snapshot($self->{pcap});

	die "E: ".ref($self).": input file has wrong data link type: $self->{type} (".
		Net::Pcap::pcap_datalink_val_to_name($self->{type}).")\n"
		unless $self->{type} == 139;

	return $self;
}

sub destroy {
	my $self = shift;
	if ($self->{pcap}) {
		Net::Pcap::pcap_close($self->{pcap});
	}
	delete $self->{pcap};
}

sub getmsg {
	my ($self,$hdr,$dat) = @_;
	return Net::Pcap::pcap_next_ex($self->{pcap}, $hdr, $dat);
}

sub DESTROY {
	shift->destroy;
}

#---------------------------------
package SS7Link;
use strict;
#---------------------------------

my $links = {};

sub new {
	my ($type,$msg) = @_;
	my $ppa = $msg->{ppa};
	return $links->{$ppa} if $links->{$ppa};
	print STDERR "\rI: discovered new link $ppa\n";
	my $self = {};
	bless $self,$type;
	$self->{ppa} = $ppa;
	$self->{card} = ($ppa >> 7) & 0x03;
	$self->{span} = ($ppa >> 5) & 0x03;
	$self->{slot} = ($ppa >> 0) & 0x1f;
	$links->{$ppa} = $self;
	return $self;
}

#---------------------------------
package SS7Message;
use strict;
#---------------------------------

sub new {
	my ($type,$pcap) = @_;
	my $self = {};
	bless $self,$type;

	$self->{hdr} = {};
	$self->{dat} = '';
	my $ret = $pcap->getmsg($self->{hdr},\$self->{dat});
	return undef unless $ret == 1;

	my $psu = substr($self->{dat},0,4);
	$self->{dat} = substr($self->{dat},4);
	$self->{hdr}->{len} -= 4;
	$self->{hdr}->{caplen} -= 4;
	my ($lkno0,$lkno1);
	($self->{dir},$self->{xsn},$lkno0,$lkno1) = unpack('CCCC', $psu);
	$self->{ppa} = ($lkno0 << 8) | $lkno1;
	my ($bno, $fno, $li) = unpack('CCC', $self->{dat});
	$self->{bib} = ($bno >> 7) & 0x01;
	$self->{bsn} = ($bno >> 0) & 0x7f;
	$self->{fib} = ($fno >> 7) & 0x01;
	$self->{fsn} = ($fno >> 0) & 0x7f;
	$self->{li0} = ($li  >> 6) & 0x03;
	$self->{li}  = ($li  >> 0) & 0x3f;
	return $self if $self->{li} < 3;

	# m3ua needs all this
	my $sio = unpack('C', substr($self->{dat},3,1));
	$self->{ni} = ($sio & 0xc0) >> 6;
	$self->{mp} = ($sio & 0x30) >> 4;
	$self->{si} = ($sio & 0x0f) >> 0;

	my @rl = unpack('C*', substr($self->{dat},4,7));
	$self->{dpc} = ($rl[2] << 16) | ($rl[1] << 8) | $rl[0];
	$self->{opc} = ($rl[5] << 16) | ($rl[4] << 8) | $rl[3];
	$self->{sls} = $rl[6];

	return $self;
}

sub copy {
	my $self = shift;
	my $copy = {};
	bless $copy,ref $self;
	$copy->{hdr} = {};
	foreach (keys %{$self->{hdr}}) {
		$copy->{hdr}->{$_} = $self->{hdr}->{$_};
	}
	foreach (qw/dat dir xsn ppa bib bsn fib fsn li0 li ni mp si dpc opc sls/) {
		$copy->{$_} = $self->{$_} if exists $self->{$_};
	}
	return $copy;
}

#---------------------------------
package Conversion;
use strict;
#---------------------------------

sub new {
	my ($type,$infile,$outfile,$cook) = @_;
	my $self = {};
	bless $self,$type;

	$self->{cook} = $cook;
	$self->{i} = new PcapInput($infile);
	$self->{o} = new PcapOutput($outfile,$cook);

	return $self;
}

sub process {
	my ($self,$do_m2pa,$do_m2ua,$do_m3ua,$do_sua) = @_;
	my $self = shift;
	my $count = 0;
	for (;;) {
		my $ss7_msg = new SS7Message($self->{i});
		last unless $ss7_msg;
		$count++;
		my $link = new SS7Link($ss7_msg);
		printf STDOUT "\r$count";
		next unless $ss7_msg->{li} >= 3;
		if ($do_m2pa) {
			my $m2pa_msg = $ss7_msg->copy();
			M2PALink->new($m2pa_msg,$self->{cook})->wrap($m2pa_msg);
			$self->{o}->putmsg($m2pa_msg);
		}
		if ($do_m2ua) {
			my $m2ua_msg = $ss7_msg->copy();
			M2UALink->new($m2ua_msg,$self->{cook})->wrap($m2ua_msg);
			$self->{o}->putmsg($m2ua_msg);
		}
		next if $ss7_msg->{si} < 3;
		if ($do_m3ua) {
			my $m3ua_msg = $ss7_msg->copy();
			M3UALink->new($m3ua_msg,$self->{cook})->wrap($m3ua_msg);
			$self->{o}->putmsg($m3ua_msg);
		}
		next unless $ss7_msg->{si} == 3;
		if ($do_sua) {
			my $sua_msg = $ss7_msg->copy();
			SUALink->new($sua_msg,$self->{cook})->wrap($sua_msg);
			$self->{o}->putmsg($sua_msg);
		}
	}
	printf STDOUT "\n";
}

#---------------------------------
package main;
#---------------------------------

my $translator = new Conversion($infile,$outfile,$cooked);
$translator->process($do_m2pa,$do_m2ua,$do_m3ua,$do_sua);

exit;

1;
