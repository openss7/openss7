#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $progname = $0;

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
Copyright (c) 2008, 2009, 2010, 2011  Monavacon Limited.
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
Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

use Net::Pcap qw(:functions);
use Tk;
use Tk::Xrm;
use Tk::Event;
require Tk::Adjuster;
require Tk::Dialog;
require Tk::HList;
require Tk::ROText;
require Tk::NoteBook;
require Tk::Pane;
require Tk::Balloon;
require Tk::DragDrop;
require Tk::DropSite;
require Tk::TableMatrix;
require Tk::TableMatrix::Spreadsheet;
require Tk::Frame;

my $canvas;

# -------------------------------------
package Route;
use strict;
# -------------------------------------

sub new {
	my ($type,$path,$end,$node) = @_;
	my $self = {};
	bless $self, $type;
	$self->{'path'} = $path;
	$self->{'end'} = $end;
	$self->{'node'} = $node;
	my $x1 = $path->{"x$end"};
	my $y1 = $path->{"y$end"};
	my $x2 = $node->{'x'};
	my $y2 = $node->{'y'};
	$self->{'item'} = $main::canvas->createLine($x1,$y1,$x2,$y2,
		-arrow=>'none',
		-capstyle=>'round',
		-fill=>'gray',
		-joinstyle=>'round',
		-smooth=>0,
		-tags=>('path'),
		-width=>1,
	);
	$main::canvas->idletasks;
	$main::canvas->update;
	return $self;
}

# -------------------------------------
package Node;
use strict;
# -------------------------------------

my %nodes = {};
my $nodeno = 0;

sub get {
	my ($type,$pc) = @_;
	return $nodes{$pc} if exists $nodes{$pc};
	my $self = {};
	bless $self, $type;
	$nodes{$pc} = $self;
	$nodeno = $nodeno + 1;
	$self->{'pc'} = $pc;
	my $x = 50;
	my $y = $nodeno * 30;
	$self->{'x'} = $x;
	$self->{'y'} = $y;
	$self->{'item'} = $main::canvas->createOval(
		$x-12.5,$y-12.5,$x+12.5,$y+12.5,
	);
	my ($text,$ntw,$cls,$mem);
	if ($pc & ~0x3fff) {
		$ntw = $pc >> 16;
		$cls = ($pc >> 8) & 0xff;
		$mem = $pc & 0xff;
	} else {
		$ntw = $pc >> 11;
		$cls = ($pc >> 3) & 0xff;
		$mem = $pc & 0x7;
	}
	$text = "$ntw-$cls-$mem";
	$self->{'text'} = $main::canvas->createText($x,$y,
		-anchor=>'center',
		-fill=>'black',
		-justify=>'center',
		-text=>$text,
	);
	$main::canvas->idletasks;
	return $self;
};

# -------------------------------------
package Path;
use strict;
# -------------------------------------

use constant {
	HT_UNKNOWN => 0,
	HT_BASIC => 3,	 # also link level header length
	HT_EXTENDED => 6 # also link level header length
};
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
use constant {
	MP_UNKNOWN => 0,
	MP_JAPAN => 1,
	MP_NATIONAL => 2,
	MP_INTERNATIONAL => 3
};

my %paths = {};
my $pathno = 0;

sub get {
	my $type = shift;
	my $ppa = shift;
	return $paths{$ppa} if exists $paths{$ppa};
	my $self = {};
	bless $self, $type;
	$paths{$ppa} = $self;
	$pathno = $pathno + 1;
	$self->{'ppa'} = $ppa;
	$self->{'slot'} = (($ppa >> 0) & 0x1f);
	$self->{'span'} = (($ppa >> 5) & 0x03);
	$self->{'card'} = (($ppa >> 7) & 0x01);
	$self->{'msgs'} = ();
	$self->{'ht'} = HT_UNKNOWN;
	$self->{'pr'} = MP_UNKNOWN;
	$self->{'rt'} = RT_UNKNOWN;
	$self->{'orig'} = {};
	$self->{'dest'} = {};
	print "Created new path ($self->{'card'}:$self->{'span'}:$self->{'slot'}).\n";
	my ($x,$y,$w,$h,$c) = (100,$pathno*100,300,0,'red');
	$self->{'xopc'} = $x;
	$self->{'yopc'} = $y+$h;
	$self->{'xdpc'} = $x+$w;
	$self->{'ydpc'} = $y;
	$self->{'color'} = $c;
	$self->{'item'} = $main::canvas->createLine($x,$y+$h,$x+$w,$y,
		-arrow=>'none',
		-capstyle=>'round',
		-fill=>$c,
		-joinstyle=>'round',
		-smooth=>0,
		-tags=>('path'),
		-width=>4,
	);
	$self->{'count'} = 0;
	$self->{'text'} = $main::canvas->createText($x+$w/2,$y+$h/2,
		-anchor=>'center',
		-fill=>'black',
		-justify=>'center',
		-text=>'0',
	);
	$self->{'httext'} = $main::canvas->createText($x+$w/2,$y+$h/2+15,
		-anchor=>'center',
		-fill=>'black',
		-justify=>'center',
		-text=>'HT_UNKNOWN',
	);
	$self->{'rttext'} = $main::canvas->createText($x+$w/2,$y+$h/2+30,
		-anchor=>'center',
		-fill=>'black',
		-justify=>'center',
		-text=>'RT_UNKNOWN',
	);
	$self->{'prtext'} = $main::canvas->createText($x+$w/2,$y+$h/2+45,
		-anchor=>'center',
		-fill=>'black',
		-justify=>'center',
		-text=>'MP_UNKNOWN',
	);
	$main::canvas->idletasks;
	return $self;
}

sub cardnum {
	shift->{'card'};
}
sub spannum {
	shift->{'span'};
}
sub slotnum {
	shift->{'slot'};
}
sub ppa {
	shift->{'ppa'};
}
sub add {
	my ($self,$msg,@args) = @_;
	if ($self->detecting) {
		push @{$self->{'msgs'}}, $msg;
		return;
	}
	push @{$self->{'msgs'}}, $msg;
	while ($msg = pop @{$self->{'msgs'}}) {
		$self->complete($msg);
	}
}
sub complete {
	my ($self,$msg,@args) = @_;
	$self->{'count'} += 1;
#	if ($self->{'count'} % 100 == 0) {
#		$main::canvas->itemconfigure($self->{'text'},
#			-text=>"$self->{'count'}",
#		);
#		$main::canvas->idletasks;
#	}
	if (exists $msg->{'dpc'}) {
		my $pc = $msg->{'dpc'};
		if (!exists $self->{'dpcs'}->{$pc}) {
			my $node = Node->get($pc);
			my $route = Route->new($self,'dpc',$node);
			$self->{'dpcs'}->{$pc} = $route;
		}
	}
	if (exists $msg->{'opc'}) {
		my $pc = $msg->{'opc'};
		if (!exists $self->{'opcs'}->{$pc}) {
			my $node = Node->get($pc);
			my $route = Route->new($self,'opc',$node);
			$self->{'opcs'}->{$pc} = $route;
		}
	}
}
sub detecting {
	my $self = shift;
	return 0 if ($self->{'ht'} != 0 && $self->{'pr'} != 0 && $self->{'rt'} != 0);
	return 1;
}

sub setht {
	my ($self,$ht) = @_;
	if ($self->{'ht'} != $ht) {
		$self->{'ht'} = $ht;
		my $text;
		if ($ht == HT_BASIC) {
			$text = 'HT_BASIC';
		} elsif ($ht == HT_EXTENDED) {
			$text = 'HT_EXTENDED';
		} else {
			$text = 'HT_UNKNOWN';
		}
		$main::canvas->itemconfigure($self->{'httext'},
			-text=>$text,
		);
		$main::canvas->idletasks;
	}
}
sub setrt {
	my ($self,$rt) = @_;
	if ($self->{'rt'} != $rt) {
		$self->{'rt'} = $rt;
		my $text;
		if ($rt == RT_14BIT_PC) {
			$text = 'RT_14BIT_PC';
		} elsif ($rt == RT_24BIT_PC) {
			$text = 'RT_24BIT_PC';
		} else {
			$text = 'RT_UNKNOWN';
		}
		$main::canvas->itemconfigure($self->{'rttext'},
			-text=>$text,
		);
		$main::canvas->idletasks;
	}
}
sub setpr {
	my ($self,$pr) = @_;
	if ($self->{'pr'} != $pr) {
		$self->{'pr'} = $pr;
		my $text;
		if ($pr == MP_INTERNATIONAL) {
			$text = 'MP_INTERNATIONAL';
		} elsif ($pr == MP_NATIONAL) {
			$text = 'MP_NATIONAL';
		} elsif ($pr == MP_JAPAN) {
			$text = 'MP_JAPAN';
		} else {
			$text = 'MP_UNKNOWN';
		}
		$main::canvas->itemconfigure($self->{'prtext'},
			-text=>$text,
		);
		$main::canvas->idletasks;
	}
}

# -------------------------------------
package Message;
use strict;
# -------------------------------------

use constant {
	HT_UNKNOWN => 0,
	HT_BASIC => 3,	 # also link level header length
	HT_EXTENDED => 6 # also link level header length
};
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
use constant {
	MP_UNKNOWN => 0,
	MP_JAPAN => 1,
	MP_NATIONAL => 2,
	MP_INTERNATIONAL => 3
};
my $count = 0;

my %snmm_mt = {
	0x11 => "coo",
	0x12 => "coa",
	0x15 => "cbd",
	0x16 => "cba",
	0x21 => "eco",
	0x22 => "eca",
	0x31 => "rct",
	0x32 => "tfc",
	0x41 => "tfp",
	0x42 => "tcp",
	0x43 => "tfr",
	0x44 => "tcr",
	0x45 => "tfa",
	0x46 => "tca",
	0x51 => "rst",
	0x52 => "rsr",
	0x53 => "rcp",
	0x54 => "rcr",
	0x61 => "lin",
	0x62 => "lun",
	0x63 => "lia",
	0x64 => "lua",
	0x65 => "lid",
	0x66 => "lfu",
	0x67 => "llt",
	0x68 => "lrt",
	0x71 => "tra",
	0x72 => "trw",
	0x81 => "dlc",
	0x82 => "css",
	0x83 => "cns",
	0x84 => "cnp",
	0xa1 => "upu",
	0xa2 => "upa",
	0xa3 => "upt",
};

my %sntm_mt = {
	0x11 => "sltm",
	0x12 => "slta",
};

my %sccp_mt = {
	0x01 => "cr",
	0x02 => "cc",
	0x03 => "cref",
	0x04 => "rlsd",
	0x05 => "rlc",
	0x06 => "dt1",
	0x07 => "dt2",
	0x08 => "ak",
	0x09 => "udt",
	0x0a => "udts",
	0x0b => "ed",
	0x0c => "ea",
	0x0d => "rsr",
	0x0e => "rsc",
	0x0f => "err",
	0x10 => "it",
	0x11 => "xudt",
	0x12 => "xudts",
	0x13 => "ludt",
	0x14 => "ludts",
};

my %isup_mt = {
	0x01 =>"iam",
	0x02 =>"sam",
	0x03 =>"inr",
	0x04 =>"inf",
	0x05 =>"cot",
	0x06 =>"acm",
	0x07 =>"con",
	0x08 =>"fot",
	0x09 =>"anm",
	0x0c =>"rel",
	0x0d =>"sus",
	0x0e =>"res",
	0x10 =>"rlc",
	0x11 =>"ccr",
	0x12 =>"rsc",
	0x13 =>"blo",
	0x14 =>"ubl",
	0x15 =>"bla",
	0x16 =>"uba",
	0x17 =>"grs",
	0x18 =>"cgb",
	0x19 =>"cgu",
	0x1a =>"cgba",
	0x1b =>"cgua",
	0x1c =>"cmr",
	0x1d =>"cmc",
	0x1e =>"cmrj",
	0x1f =>"far",
	0x20 =>"faa",
	0x21 =>"frj",
	0x22 =>"fad",
	0x23 =>"fai",
	0x24 =>"lpa",
	0x25 =>"csvq",
	0x26 =>"csvr",
	0x27 =>"drs",
	0x28 =>"pam",
	0x29 =>"gra",
	0x2a =>"cqm",
	0x2b =>"cqr",
	0x2c =>"cpg",
	0x2d =>"usr",
	0x2e =>"ucic",
	0x2f =>"cfn",
	0x30 =>"olm",
	0x31 =>"crg",
	0x32 =>"nrm",
	0x33 =>"fac",
	0x34 =>"upt",
	0x35 =>"upa",
	0x36 =>"idr",
	0x37 =>"irs",
	0x38 =>"sgm",
	0xe9 =>"cra",
	0xea =>"crm",
	0xeb =>"cvr",
	0xec =>"cvt",
	0xed =>"exm",
	0xf8 =>"non",
	0xfc =>"llm",
	0xfd =>"cak",
	0xfe =>"tcm",
	0xff =>"mcp",
};

my @mtypes = (
	\%snmm_mt,
	\%sntm_mt,
	\%sntm_mt,
	\%sccp_mt,
	undef,
	\%isup_mt,
);

# $msg = Message::create($pcap);
sub create {
	my $type = shift;
	my $self = {};
	bless $self, $type;
	my $pcap = shift;
	my %hdr = {};
	my $dat = '';
	$self->{'hdr'} = {};
	$self->{'dat'} = '';
	my $ret = Net::Pcap::pcap_next_ex($pcap, \%hdr, \$dat);
	$self->{'hdr'}->{'len'} = $hdr{'len'} - 4;
	$self->{'hdr'}->{'caplen'} = $hdr{'caplen'} - 4;
	$self->{'hdr'}->{'tv_sec'} = $hdr{'tv_sec'};
	$self->{'hdr'}->{'tv_usec'} = $hdr{'tv_usec'};
	$self->{'dat'} = substr($dat, 4);
	return undef if $ret != 1;
	$count = $count + 1;
	my ( $dir, $xsn, $lkno0, $lkno1 ) = unpack('CCCC', $dat);
	$self->{'dir'} = $dir;
	$self->{'xsn'} = $xsn;
	$self->{'ppa'} = ($lkno0 << 8) | $lkno1;
	return $self;
}

sub process {
	my $self = shift;
	my $path = Path->get($self->{'ppa'});
	if ($self->decode($path) >= 0) {
		$path->add($self);
		return;
	}
	#print STDERR "decoding error\n";
}

sub getCount {
	return $count;
}

sub decode {
	my ($self,$path,@args) = @_;
	my $self = shift;
	my @b = (unpack('C*', substr($self->{'dat'}, 0, 8)));
	if (!exists $self->{'mtp2decode'}) {
		my $len = $self->{'hdr'}->{'len'};
		if (3 <= $len && $len <= 5) {
			$path->setht(HT_BASIC);
		} elsif (6 <= $len && $len <= 8) {
			$path->setht(HT_EXTENDED);
		}
		return 0 unless $path->{'ht'} != HT_UNKNOWN;
		my ($bsn,$bib,$fsn,$fib,$li,$li0);
		if ($path->{'ht'} == HT_BASIC) {
			$self->{'bsn'} = $b[0] & 0x7f;
			$self->{'bib'} = $b[0] >> 7;
			$self->{'fsn'} = $b[1] & 0x7f;
			$self->{'fib'} = $b[1] >> 7;
			$self->{'li' } = $b[2] & 0x3f;
			$self->{'li0'} = $b[2] >> 6;
		} elsif ($path->{'ht'} == HT_EXTENDED) {
			$self->{'bsn'} = $b[0];
			$self->{'bsn'} |= ($b[1] & 0x0f) << 8;
			$self->{'bib'} = $b[1] >> 7;
			$self->{'fsn'} = $b[2];
			$self->{'fsn'} |= ($b[3] & 0x0f) << 8;
			$self->{'fib'} = $b[3] >> 7;
			$self->{'li' } = $b[4];
			$self->{'li' } |= ($b[5] & 0x1) << 8;
			$self->{'li0'} = $b[5] >> 6;
		}
		my $inf = $len - $path->{'ht'};
		if (($self->{'li'} != $inf) && ($inf <= 63 || $self->{'li'} != 63)) {
			print STDERR "bad length indicator $self->{'li'} != $inf\n";
		}
		if ($self->{'li0'} != 0) {
			$path->setrt(RT_24BIT_PC);
			$path->setpr(MP_JAPAN);
		}
		$self->{'mtp2decode'} = 1;
	}
	return 1 if $self->{'li'} == 0;
	@b = (unpack('C*', substr($self->{'dat'}, $path->{'ht'}, 11)));
	$self->{'si'} = $b[0];
	return 1 if $self->{'li'} < 3;
	$self->{'ni'} = ($b[0] & 0xc0) >> 6;
	$self->{'mp'} = ($b[0] & 0x30) >> 4;
	$self->{'si'} = ($b[0] & 0x0f);
	if ($self->{'ni'} == 0) {
		$path->setrt(RT_14BIT_PC);
		$path->setpr(MP_INTERNATIONAL);
	}
	if ($path->{'pr'} == MP_UNKNOWN) {
		$path->setpr(MP_NATIONAL) if $self->{'mp'} != 0;
	} elsif ($path->{'pr'} == MP_JAPAN) {
		$self->{'mp'} = $self->{'li0'};
	} elsif ($path->{'pr'} == MP_INTERNATIONAL) {
		$self->{'mp'} = 0;
	}
	if ($self->{'li'} < HT_EXTENDED) {
		print STDERR "too short for RL, li = $self->{'li'}\n";
		return -1;
	}
	if ($self->{'li'} < 9 || ($self->{'si'} == 5 && $self->{'li'} < 11)) {
		$path->setrt(RT_14BIT_PC);
	}
	if ($path->{'rt'} == RT_UNKNOWN) {
		my $ret = $self->checkRoutingLabelType($self->{'si'},$path,$self->{'li'},\@b) < 0;
		return $ret if $ret <= 0;
		print "check succeeded on si=$self->{'si'}, mt=$self->{'mt'}";
	}
	if (!exists $self->{'mtp3decode'}) {
		return 0 if $path->detecting;
		if ($path->{'rt'} == RT_14BIT_PC) {
			if ($self->{'li'} < 6) {
				print STDERR "too short for 14-bit RL, li = $self->{'li'}\n";
				return -1;
			}
			$self->{'dpc'} = $b[1];
			$self->{'dpc'} |= ($b[2] & 0x3f) << 8;
			$self->{'opc'} = ($b[2] & 0xc0) >> 6;
			$self->{'opc'} |= $b[3] << 2;
			$self->{'opc'} |= ($b[4] & 0x0f) << 10;
			$self->{'sls'} = $b[4] >> 4;
			$self->{'mt' } = $b[5];
		} else {
			if ($self->{'li'} < 9) {
				print STDERR "too short for 24-bit RL, li = $self->{'li'}\n";
				return -1;
			}
			$self->{'dpc'} = $b[1];
			$self->{'dpc'} |= $b[2] << 8;
			$self->{'dpc'} |= $b[3] << 16;
			$self->{'opc'} = $b[4];
			$self->{'opc'} |= $b[5] << 8;
			$self->{'opc'} |= $b[6] << 16;
			$self->{'sls'} = $b[7];
			$self->{'mt' } = $b[8];
		}
		@b = (unpack('C*', substr($self->{'dat'}, $path->{'ht'} + 1 + $self->{'rt'}, 3)));
		if ($self->{'si'} == 5) {
			$self->{'cic'} = $b[0];
			$self->{'cic'} |= $b[1] << 8;
			$self->{'mt'} = $b[2];
		} elsif ($self->{'si'} < 3) {
			$self->{'mt'} = (($b[0] & 0x0f) << 4);
			$self->{'mt'} |= $b[0] >> 4;
		} else {
			$self->{'mt'} = $b[0];
		}
		unless (defined $mtypes[$self->{'si'}]) {
			print STDERR "no message type for si=$self->{'si'}\n";
			return -1;
		}
#		unless (exists $mtypes[$self->{'si'}]{$self->{'mt'}}) {
#			print STDERR "no message type for si=$self->{'si'}, mt=$self->{'mt'}\n";
#			return -1;
#		}
		if ($self->{'si'} == 1 || $self->{'si'} == 2) {
			if ($path->{'rt'} == RT_14BIT_PC) {
				$self->{'slc'} = $self->{'sls'};
			} else {
				$self->{'slc'} = $b[1] & 0x0f;
			}
			$self->{'dlen'} = $b[1] >> 4;
		}
		$self->{'mtp3decode'} = 1;
	}
	return 1;
}

sub checkRoutingLabelType {
	my ($self,$si,@args) = @_;
	if ($si == 0) {
		return $self->checkSnmm(@args);
	} elsif ($si == 1) {
		return $self->checkSntm(@args);
	} elsif ($si == 2) {
		return $self->checkSnsm(@args);
	} elsif ($si == 3) {
		return $self->checkSccp(@args);
	} elsif ($si == 5) {
		return $self->checkIsup(@args);
	}
	return 0;
}

use constant {
	PT_YES => 1,
	PT_MAYBE => 0,
	PT_NO => -1
};

sub checkSnmm {
	my ($self,$path,@args) = @_;
	my $ansi = $self->checkAnsiSnmm($path,@args);
	my $itut = $self->checkItutSnmm($path,@args);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$path->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$path->setrt(RT_14BIT_PC);
		} else {
			$path->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$path->setrt(RT_14BIT_PC);
		return 1;
	}
}
sub checkItutSnmm {
	my ($self,$path,$li,$b) = @_;
	my $mt = (($b->[5] & 0x0f) << 4) | ($b->[5] >> 4);

	if ($li == 6) {
		return PT_YES if $mt == 0x17 || $mt == 0x27 || $mt == 0x13 || $mt == 0x28
			|| $mt == 0x38 || $mt == 0x48 || $mt == 0x12 || $mt == 0x22 || $mt == 0x16
			|| $mt == 0x26 || $mt == 0x36 || $mt == 0x46 || $mt == 0x56 || $mt == 0x66
			|| $mt == 0x76 || $mt == 0x86;
		return PT_NO;
	}
	if ($li == 7) {
		return PT_YES if $mt == 0x11 || $mt == 0x21 || $mt == 0x51 || $mt == 0x61;
		return PT_NO;
	}
	if ($li == 8) {
		return PT_YES if $mt == 0x23 || $mt == 0x14 || $mt == 0x24 || $mt == 0x34
			|| $mt == 0x44 || $mt == 0x54 || $mt == 0x64 || $mt == 0x15 || $mt == 0x25
			|| $mt == 0x35 || $mt == 0x45;
		return PT_NO;
	}
	if ($li == 9) {
		return PT_YES if $mt == 0x1a || $mt == 0x2a || $mt == 0x3a;
		return PT_NO;
	}
	return PT_NO;
}
sub checkAnsiSnmm {
	my ($self,$path,$li,$b) = @_;
	my $mt = (($b->[8] & 0x0f) << 4) | ($b->[8] >> 4);

	return PT_NO if $li < 9;

	if ($li == 9) {
		return PT_YES if $mt == 0x17 || $mt == 0x27 || $mt == 0x13;
		return PT_NO;
	}
	if ($li == 10) {
		return PT_YES if $mt == 0x28 || $mt == 0x38 || $mt == 0x38 || $mt == 0x48
			|| $mt == 0x12 || $mt == 0x22 || $mt == 0x16 || $mt == 0x26 || $mt == 0x36
			|| $mt == 0x46 || $mt == 0x56 || $mt == 0x66 || $mt == 0x76 || $mt == 0x86;
		return PT_NO;
	}
	if ($li == 11) {
		return PT_YES if $mt == 0x11 || $mt == 0x21 || $mt == 0x51 || $mt == 0x61;
		return PT_NO;
	}
	if ($li == 12) {
		return PT_YES if $mt == 0x14 || $mt == 0x24 || $mt == 0x34 || $mt == 0x44
			|| $mt == 0x54 || $mt == 0x64 || $mt == 0x15 || $mt == 0x25 || $mt == 0x35
			|| $mt == 0x45 || $mt == 0x18;
		return PT_NO;
	}
	if ($li == 13) {
		return PT_YES if $mt == 0x23 || $mt == 0x1a || $mt == 0x2a || $mt == 0x3a;
		return PT_NO;
	}
	return PT_NO;
}

sub checkSntm {
	my ($self,$path,@args) = @_;
	my $ansi = $self->checkAnsiSntm($path,@args);
	my $itut = $self->checkItutSntm($path,@args);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$path->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$path->setrt(RT_14BIT_PC);
		} else {
			$path->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$path->setrt(RT_14BIT_PC);
		return 1;
	}
}
sub checkItutSntm {
	my ($self,$path,$li,$b) = @_;
	my $mt = (($b->[5] & 0x0f) << 4) | ($b->[5] >> 4);

	return PT_NO if 7 > $li || $li > 22;
	return PT_NO if $mt != 0x11 && $mt != 0x12;

	if (($b->[6] >> 4) == $li - 7) {
		return PT_YES if 7 <= $li && $li <= 9;
		return PT_MAYBE;
	}
	return PT_NO;
}
sub checkAnsiSntm {
	my ($self,$path,$li,$b) = @_;
	my $mt = (($b->[8] & 0x0f) << 4) | ($b->[8] >> 4);

	return PT_NO if 10 > $li || $li > 25;
	return PT_NO if $mt != 0x11 && $mt != 0x12;

	if (($b->[9] >> 4) == $li - 10) {
		return PT_YES if 23 <= $li && $li <= 25;
		return PT_MAYBE;
	}
	return PT_NO;
}
sub checkSnsm {
	my ($self,$path,@args) = @_;
	my $ansi = $self->checkAnsiSnsm($path,@args);
	my $itut = $self->checkItutSntm($path,@args);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$path->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$path->setrt(RT_14BIT_PC);
		} else {
			$path->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$path->setrt(RT_14BIT_PC);
		return 1;
	}
}
sub checkItutSnsm {
	return PT_NO;
}
sub checkAnsiSnsm {
	my ($self,@args) = @_;
	return $self->checkAnsiSntm(@args);
}

sub checkSccp {
	my ($self,$path,$li,$b) = @_;
	my $ansi = $self->checkAnsiSccp($path,$b);
	my $itut = $self->checkItutSccp($path,$b);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$path->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$path->setrt(RT_14BIT_PC);
		} else {
			$path->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$path->setrt(RT_14BIT_PC);
		return 1;
	}
}
sub checkItutSccp {
	my ($self,$path,$li,$b) = @_;
	my $mt = $b->[5];
	return PT_MAYBE if 0x01 <= $mt && $mt <= 0x14;
	return PT_NO;
}
sub checkAnsiSccp {
	my ($self,$path,$li,$b) = @_;
	my $mt = $b->[8];
	return PT_MAYBE if 0x01 <= $mt && $mt <= 0x14;
	return PT_NO;
}

sub checkIsup {
	my ($self,$path,@args) = @_;
	my $ansi = $self->checkAnsiIsup($path,@args);
	my $itut = $self->checkItutIsup($path,@args);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$path->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$path->setrt(RT_14BIT_PC);
		} else {
			$path->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$path->setrt(RT_14BIT_PC);
		return 1;
	}
}
sub checkItutIsup {
	my ($self,$path,$li,$b) = @_;
	my $mt = $b->[7];

	return PT_NO if $li < 8;

	if ($mt == 0x11 || $mt == 0x12 || $mt == 0x13 || $mt == 0x14 || $mt == 0x15 || $mt == 0x16 || $mt == 0x24 || $mt == 0x2e || $mt == 0x30 || $mt == 0x31) {
		return PT_NO if $li != 8;
		return PT_YES;
	}
	if ($mt == 0x05) {
		return PT_NO if $li != 9;
		return PT_YES;
	}
	if ($mt == 0x10 || $mt == 0x27 || $mt == 0x08 || $mt == 0x09 || $mt == 0x34 || $mt == 0x33 || $mt == 0x35 || $mt == 0x36 || $mt == 0x37 || $mt == 0x38 || $mt == 0x32) {
		return PT_NO if $li < 9;
		if ($li > 9) {
			return PT_NO if $b->[8] == 0;
			return PT_YES if $li < 11;
			return PT_MAYBE;
		}
		return PT_NO if $b->[8] != 0;
		return PT_YES;
	}
	if ($mt == 0x03 || $mt == 0x04 || $mt == 0x06 || $mt == 0x0d || $mt == 0x0e || $mt == 0x1c || $mt == 0x1d || $mt == 0x1e || $mt == 0x1f || $mt == 0x20 || $mt == 0x2c) {
		return PT_NO if $li < 10;
		if ($li > 10) {
			return PT_NO if $b->[8] == 0;
			return PT_MAYBE;
		}
		return PT_NO if $b->[8] != 0;
		return PT_YES;
	}
	if ($mt == 0x01) {
		return PT_NO if $li < 18;
		return PT_MAYBE;
	}
	if ($mt == 0x21 || $mt == 0x2b) {
		return PT_NO if $li < 14;
		return PT_MAYBE;
	}
	if ($mt == 0x02 || $mt == 0x0c || $mt == 0x2f) {
		return PT_NO if $li < 13;
		return PT_MAYBE;
	}
	if ($mt == 0x17 || $mt == 0x2d || $mt == 0x18 || $mt == 0x1a || $mt == 0x19 || $mt == 0x1b) {
		return PT_NO if $li < 12;
		return PT_MAYBE;
	}
	if ($mt == 0x07) {
		return PT_NO if $li < 11;
		if ($li > 11) {
			return PT_NO if $b->[10] == 0;
			return PT_MAYBE;
		}
		return PT_NO if $b->[10] != 0;
		return PT_YES;
	}
	if ($mt == 0x29 || $mt == 0x2a) {
		return PT_NO if $li < 11;
		return PT_MAYBE;
	}
	if ($mt == 0x28) {
		return PT_MAYBE;
	}
	if ($mt == 0x22 || $mt == 0x23 || $mt == 0xe9 || $mt == 0xea || $mt == 0xeb || $mt == 0xec || $mt == 0xed || $mt == 0xf8 || $mt == 0xfc || $mt == 0xfd || $mt == 0xfe || $mt == 0xff || $mt == 0x25 || $mt == 0x26) {
		return PT_NO;
	}
	return PT_NO;
}
sub checkAnsiIsup {
	my ($self,$path,$li,$b) = @_;
	my $mt = $b->[10];

	return PT_NO if $li < 11;

	if ($mt == 0x10 || $mt == 0x11 || $mt == 0x12 || $mt == 0x13 || $mt == 0x14 || $mt == 0x15 || $mt == 0x16 || $mt == 0x24 || $mt == 0x2e || $mt == 0xe9 || $mt == 0xec) {
		return PT_NO if $li != 11;
		return PT_MAYBE;
	}
	if ($mt == 0xed) {
		return PT_NO if $li < 12;
		if ($li > 12) {
			return PT_NO if $b->[11] == 0;
			return PT_MAYBE;
		}
		return PT_NO if $b->[11] != 0;
		return PT_MAYBE;
	}
	if ($mt == 0x05 || $mt == 0xea) {
		return PT_NO if $li != 12;
		return PT_MAYBE;
	}
	if (exists $isup_mt{$mt}) {
		return PT_MAYBE;
	}
	return PT_MAYBE;
}


# -------------------------------------
package MyOptions;
use strict;
# -------------------------------------

sub assign {
	my $mw = shift;
	#$mw->optionAdd('*font'=>'-*-helvetica-medium-r-*--*-100-*-*-*-*-*-*');
	#$mw->optionAdd('*.Button.*.font'=>'-*-helvetica-bold-r-*--*-100-*-*-*-*-*-*');
	#$mw->optionAdd('*.Label.*.font'=>'-*-helvetica-bold-r-*--*-100-*-*-*-*-*-*');
	#$mw->optionAdd('*font'=>'Arial 9');
	#$mw->optionAdd('*font'=>'Helvetica 10');
	#$mw->optionAdd('*Button*font'=>'Helvetica-Bold 10');
	#$mw->optionAdd('*Label*font'=>'Helvetica-Bold 10');
	#$mw->optionAdd('*disabledBackground'=>'dark grey');
	#$mw->optionAdd('*highlightBackground'=>'light grey');
	#$mw->optionAdd('*highlightThickness'=>1);
	#$mw->optionAdd('*borderThickness'=>1);
	#$mw->optionAdd('*borderWidth'=>1);
	#$mw->optionAdd('*border'=>0);
	#$mw->optionAdd('*Menu*Width'=>1);
	#$mw->optionAdd('*Menu*Heigth'=>1);

	#$mw->optionAdd('*Balloon*font'=>'Arial 8');
	#$mw->optionAdd('*padX'=>0);
	#$mw->optionAdd('*padY'=>0);
	#$mw->optionAdd('*relief'=>'groove');
	#$mw->optionAdd('*Scrollbar*Width'=>8);
}

# -------------------------------------
package MyPixmaps;
use strict;
# -------------------------------------

my $iconImage = <<EOF;
/* XPM */
static char * streams_icon_xpm[] = {
"48 48 484 2",
"  	c #575757",
". 	c #C4C4C4",
"+ 	c #FFFFFF",
"\@ 	c #FBFBFE",
"# 	c #DCDDF4",
"\$ 	c #969BDD",
"\% 	c #6B73D2",
"\& 	c #6F75D4",
"* 	c #8389D8",
"= 	c #C4C6ED",
"- 	c #ECEDF9",
"; 	c #FEFEFF",
"> 	c #F0F0F0",
", 	c #CECECE",
"' 	c #797979",
") 	c #3D3D3D",
"! 	c #313131",
"~ 	c #333333",
"{ 	c #6B6B6B",
"] 	c #CACACA",
"^ 	c #FCFCFC",
"/ 	c #FCFDFE",
"( 	c #C1C5EC",
"_ 	c #5057C8",
": 	c #434CC1",
"< 	c #8D91DB",
"[ 	c #A0A4E2",
"} 	c #8489D9",
"| 	c #4F58C8",
"1 	c #7178D4",
"2 	c #CCCFEF",
"3 	c #EFEFEF",
"4 	c #808080",
"5 	c #242424",
"6 	c #414141",
"7 	c #878787",
"8 	c #ACACAC",
"9 	c #8E8E8E",
"0 	c #3B3B3B",
"a 	c #323232",
"b 	c #C2C2C2",
"c 	c #E7E8F7",
"d 	c #4B53C7",
"e 	c #4750C7",
"f 	c #C5C8ED",
"g 	c #FAFAFD",
"h 	c #F7F7FD",
"i 	c #CCCEEF",
"j 	c #6D73D1",
"k 	c #4751C5",
"l 	c #5D5D68",
"m 	c #1A1A1A",
"n 	c #4A4A4A",
"o 	c #DADADA",
"p 	c #FBFBFB",
"q 	c #CBCBCB",
"r 	c #3E3E3E",
"s 	c #424242",
"t 	c #EAEAEA",
"u 	c #FDFDFE",
"v 	c #7E85D7",
"w 	c #3840C1",
"x 	c #B9BBEA",
"y 	c #FDFDFF",
"z 	c #E2E3EA",
"A 	c #40435F",
"B 	c #090B17",
"C 	c #72738A",
"D 	c #F0F0F4",
"E 	c #C5C5C5",
"F 	c #212121",
"G 	c #858585",
"H 	c #E2E2F6",
"I 	c #414AC4",
"J 	c #6E74D2",
"K 	c #F2F2FB",
"L 	c #F3F3F3",
"M 	c #151515",
"N 	c #525581",
"O 	c #4C53BE",
"P 	c #C6C9EE",
"Q 	c #585858",
"R 	c #454545",
"S 	c #E0E0E0",
"T 	c #AAAEE5",
"U 	c #353FBC",
"V 	c #A8ADE5",
"W 	c #FEFEFE",
"X 	c #B2B2B2",
"Y 	c #1C1C1C",
"Z 	c #636363",
"` 	c #DDDEEE",
" .	c #6D73D2",
"..	c #535BCB",
"+.	c #F0F1FB",
"\@.	c #B0B0B0",
"#.	c #B9B9B9",
"\$.	c #F9FAFE",
"\%.	c #777DD5",
"\&.	c #3940C1",
"*.	c #E5E7F8",
"=.	c #E2E2E2",
"-.	c #262626",
";.	c #DBDCF3",
">.	c #555ECA",
",.	c #7F84D6",
"'.	c #F6F7FD",
").	c #909090",
"!.	c #E8E9F8",
"~.	c #5F65CD",
"{.	c #5158C8",
"].	c #FCFCFF",
"^.	c #F8F8F8",
"/.	c #6E6E6E",
"(.	c #1F1F1F",
"_.	c #B3B3B3",
":.	c #FDFDFD",
"<.	c #B9BCE9",
"[.	c #3C45C0",
"}.	c #B5B9E9",
"|.	c #EDEDED",
"1.	c #4D4D4D",
"2.	c #686868",
"3.	c #D8D9F3",
"4.	c #4F55C9",
"5.	c #7176D4",
"6.	c #202020",
"7.	c #6F6F6F",
"8.	c #F5F5F5",
"9.	c #6E73D3",
"0.	c #6268CE",
"a.	c #E4E5F7",
"b.	c #4B4B4B",
"c.	c #ECECEC",
"d.	c #C9CCEF",
"e.	c #3E48C0",
"f.	c #8F94DC",
"g.	c #E6E6E6",
"h.	c #525252",
"i.	c #E9E9E9",
"j.	c #D3D7F3",
"k.	c #454EC6",
"l.	c #9CA1E1",
"m.	c #FCFCFE",
"n.	c #F9F9F9",
"o.	c #E7E7E7",
"p.	c #BFC2EC",
"q.	c #363FC1",
"r.	c #A2A5E2",
"s.	c #AAAAAA",
"t.	c #171717",
"u.	c #8C8C8C",
"v.	c #F6F7FC",
"w.	c #8F94DB",
"x.	c #4D56C9",
"y.	c #E9E9F9",
"z.	c #979797",
"A.	c #232323",
"B.	c #E3E3E3",
"C.	c #BBBEEA",
"D.	c #323ABF",
"E.	c #ABAEE6",
"F.	c #4F4F4F",
"G.	c #D7D7D7",
"H.	c #D1D4F1",
"I.	c #4F57C9",
"J.	c #A5AAE4",
"K.	c #9E9E9E",
"L.	c #B9BDE9",
"M.	c #2F3ABB",
"N.	c #AFB4E5",
"O.	c #C1C1C1",
"P.	c #111111",
"Q.	c #898989",
"R.	c #FBFBFD",
"S.	c #5D66CE",
"T.	c #EAECF9",
"U.	c #A4A4A4",
"V.	c #BDC1EB",
"W.	c #3C46C3",
"X.	c #B2B6E7",
"Y.	c #656565",
"Z.	c #363636",
"`.	c #CCCCCC",
" +	c #5A61CC",
".+	c #B5B7E8",
"++	c #353535",
"\@+	c #E4E4E4",
"#+	c #CBCCEE",
"\$+	c #5E64CD",
"\%+	c #B8BCEA",
"\&+	c #727272",
"*+	c #F7F7F7",
"=+	c #8C92DB",
"-+	c #989DDF",
";+	c #F3F4FB",
">+	c #DEE0F5",
",+	c #9499DD",
"'+	c #C7CAEE",
")+	c #B6B6B6",
"!+	c #676767",
"~+	c #D8DAF3",
"{+	c #9CA1E0",
"]+	c #E3E4F6",
"^+	c #BFBFBF",
"/+	c #888888",
"(+	c #EDEDF9",
"_+	c #BCBFEA",
":+	c #D3D5F1",
"<+	c #B1B1B1",
"[+	c #999999",
"}+	c #F5F5FC",
"|+	c #B9BDEA",
"1+	c #D6D8F3",
"2+	c #FAFAFB",
"3+	c #C9C9C9",
"4+	c #D0D3F0",
"5+	c #D9DBF3",
"6+	c #EEEEEE",
"7+	c #BBBBBB",
"8+	c #CBCDEF",
"9+	c #F6F6F9",
"0+	c #CDCDCD",
"a+	c #FAFAFA",
"b+	c #F1F1F1",
"c+	c #EFEFF7",
"d+	c #F4F4F4",
"e+	c #E1E1E1",
"f+	c #DEDEDE",
"g+	c #DCDCDD",
"h+	c #D2D3E3",
"i+	c #C6C7DD",
"j+	c #BFBFC1",
"k+	c #D9D9D9",
"l+	c #E8E8E8",
"m+	c #D6D8F2",
"n+	c #D4D5E5",
"o+	c #D5D5D5",
"p+	c #E5E5E5",
"q+	c #404040",
"r+	c #777777",
"s+	c #7C7C7C",
"t+	c #7D7E88",
"u+	c #6D6D79",
"v+	c #333334",
"w+	c #3C3C3C",
"x+	c #BEBEBE",
"y+	c #B5B5B5",
"z+	c #464646",
"A+	c #989898",
"B+	c #DEDFF5",
"C+	c #9495A8",
"D+	c #F6F6F6",
"E+	c #C0C0C0",
"F+	c #494949",
"G+	c #C6C6C6",
"H+	c #535353",
"I+	c #9D9D9D",
"J+	c #F2F2F2",
"K+	c #8B8B8B",
"L+	c #444444",
"M+	c #9B9B9B",
"N+	c #4E4E4E",
"O+	c #DFE0E9",
"P+	c #55555C",
"Q+	c #D3D3D3",
"R+	c #D8D8D8",
"S+	c #3A3A3A",
"T+	c #EBEBEB",
"U+	c #6D6D6D",
"V+	c #090909",
"W+	c #2E2E2E",
"X+	c #E9EAF8",
"Y+	c #7E7F8D",
"Z+	c #030303",
"`+	c #929292",
" \@	c #131313",
".\@	c #0D0D0D",
"+\@	c #474747",
"\@\@	c #6C6C6C",
"#\@	c #A2A2A2",
"\$\@	c #D1D1D1",
"\%\@	c #9C9C9C",
"\&\@	c #828282",
"*\@	c #DCDCDC",
"=\@	c #646464",
"-\@	c #57575B",
";\@	c #B8B8B8",
">\@	c #7E7E7E",
",\@	c #0F0F0F",
"'\@	c #9F9F9F",
")\@	c #666666",
"!\@	c #1D1D1D",
"~\@	c #F5F6FC",
"{\@	c #7F7F88",
"]\@	c #3F3F3F",
"^\@	c #959595",
"/\@	c #B4B4B4",
"(\@	c #343434",
"_\@	c #A3A3A3",
":\@	c #585859",
"<\@	c #252525",
"[\@	c #C3C3C3",
"}\@	c #0A0A0A",
"|\@	c #484848",
"1\@	c #6A6A6A",
"2\@	c #F6F7F8",
"3\@	c #7F8082",
"4\@	c #383838",
"5\@	c #A0A0A0",
"6\@	c #505050",
"7\@	c #5C5C5C",
"8\@	c #707070",
"9\@	c #303030",
"0\@	c #DDDDDD",
"a\@	c #696969",
"b\@	c #4B4B4E",
"c\@	c #E2E2E6",
"d\@	c #2A2A2A",
"e\@	c #161616",
"f\@	c #A5A5A5",
"g\@	c #4C4C4C",
"h\@	c #1B1B1B",
"i\@	c #D2D2D2",
"j\@	c #3F3F40",
"k\@	c #626263",
"l\@	c #8A8A8A",
"m\@	c #717171",
"n\@	c #565656",
"o\@	c #4D4E55",
"p\@	c #ABABAB",
"q\@	c #9A9A9A",
"r\@	c #7A7A7A",
"s\@	c #3C3C3F",
"t\@	c #8F8F93",
"u\@	c #969696",
"v\@	c #DFDFDF",
"w\@	c #838390",
"x\@	c #EBECF9",
"y\@	c #F7F7F8",
"z\@	c #7F7F7F",
"A\@	c #8F8F8F",
"B\@	c #97979A",
"C\@	c #787887",
"D\@	c #C4C4CA",
"E\@	c #D4D4D4",
"F\@	c #838383",
"G\@	c #CDCDCE",
"H\@	c #D1D2E4",
"I\@	c #E1E2F5",
"J\@	c #C8C8C8",
"K\@	c #CFCFD5",
"L\@	c #C4C7E6",
"M\@	c #EAEBF6",
"N\@	c #CBCBCC",
"O\@	c #E0E2F1",
"P\@	c #CDD0F0",
"Q\@	c #E4E4EA",
"R\@	c #BEC2EC",
"S\@	c #E4E6F7",
"T\@	c #B3B4B5",
"U\@	c #E7E8F6",
"V\@	c #B6B9E9",
"W\@	c #D3D5F2",
"X\@	c #A6A6A6",
"Y\@	c #ECEDF3",
"Z\@	c #AEB2E6",
"`\@	c #D5D7F2",
" #	c #8A8A8B",
".#	c #8990DB",
"+#	c #AEAEAE",
"\@#	c #F0F0F8",
"##	c #979DDF",
"\$#	c #BCBEEA",
"\%#	c #ADB2E6",
"\&#	c #525AC9",
"*#	c #C4C6EC",
"=#	c #8085D8",
"-#	c #9DA2E0",
";#	c #F8F9FD",
">#	c #DFE1F6",
",#	c #4C55C7",
"'#	c #7C83D7",
")#	c #F8F8FD",
"!#	c #626262",
"~#	c #767DD5",
"{#	c #8A90DB",
"]#	c #515151",
"^#	c #8084D6",
"/#	c #434EC2",
"(#	c #CCCFF0",
"_#	c #C7C7C7",
":#	c #A8A8A8",
"<#	c #7379D4",
"[#	c #8288D8",
"}#	c #DCDCF4",
"|#	c #333CBF",
"1#	c #858ADA",
"2#	c #F6F6FC",
"3#	c #F1F2FB",
"4#	c #6F76D3",
"5#	c #8289DA",
"6#	c #7980D6",
"7#	c #4149C4",
"8#	c #D6D7F2",
"9#	c #F0F0FA",
"0#	c #676ECF",
"a#	c #8B90DA",
"b#	c #C8CBEE",
"c#	c #4049C4",
"d#	c #8287D9",
"e#	c #686869",
"f#	c #484849",
"g#	c #5B62CC",
"h#	c #979DDE",
"i#	c #FAFBFE",
"j#	c #8C92DD",
"k#	c #323BC0",
"l#	c #ADADAD",
"m#	c #292929",
"n#	c #474FC7",
"o#	c #ABAEE5",
"p#	c #1E1E1E",
"q#	c #DEE0F4",
"r#	c #6971D0",
"s#	c #EBECF8",
"t#	c #DEDFF4",
"u#	c #BDC0EA",
"v#	c #B7B9E9",
"w#	c #333BBF",
"x#	c #9FA3E2",
"y#	c #FAFAFE",
"z#	c #373737",
"A#	c #4D54C7",
"B#	c #D2D4F1",
"C#	c #2D2D2D",
"D#	c #5F5F5F",
"E#	c #838ADA",
"F#	c #3D46C2",
"G#	c #BABDE9",
"H#	c #2B2B2B",
"I#	c #787DD5",
"J#	c #6C72D1",
"K#	c #E3E5F7",
"L#	c #676ECE",
"M#	c #4B53C6",
"N#	c #9C9EAF",
"O#	c #4B52C7",
"P#	c #9AA0E1",
"Q#	c #848484",
"R#	c #D6D9F3",
"S#	c #4A4FA5",
"T#	c #1D1D2A",
"U#	c #6F6F75",
"V#	c #9DA1E1",
"W#	c #454CC4",
"X#	c #E1E3F6",
"Y#	c #A1A1A1",
"Z#	c #E6E6E7",
"`#	c #979798",
" \$	c #3F4376",
".\$	c #4D56C4",
"+\$	c #B8BCE9",
"\@\$	c #B6B9E8",
"#\$	c #464FC2",
"\$\$	c #A6A9E3",
"\%\$	c #787878",
"\&\$	c #5A5A5A",
"*\$	c #DEDEEF",
"=\$	c #8288D7",
"-\$	c #4A52C7",
";\$	c #8388D8",
">\$	c #B8BBEA",
",\$	c #C6C8EE",
"'\$	c #979CDF",
")\$	c #F2F3FC",
"!\$	c #7B7B7B",
"~\$	c #CCCDEE",
"{\$	c #8289D7",
"]\$	c #6A72D0",
"^\$	c #6970D1",
"/\$	c #7077D4",
"(\$	c #B2B4E7",
"_\$	c #EFF0FA",
"                                                                                              . ",
"  + + + + + + + + + + + + + + \@ # \$ \% \& * = - ; + > , ' ) ! ~ { ] ^ + + + + + + + + + + + + + . ",
"  + + + + + + + + + + + + + / ( _ : < [ } | 1 2 3 4 5 6 7 8 9 0 a b + + + + + + + + + + + + + . ",
"  + + + + + + + + + + + + + c d e f g + h i j k l m n o p + ^ q r s t + + + + + + + + + + + + . ",
"  + + + + + + + + + + + + u v w x y + + + + z A B C D + + + + + E F G ^ + + + + + + + + + + + . ",
"  + + + + + + + + + + + + H I J K + + + + L G M N O P ; + + + + ^ Q R S + + + + + + + + + + + . ",
"  + + + + + + + + + + + + T U V + + + + W X Y Z `  ...+.+ + + + + \@.Y #.+ + + + + + + + + + + . ",
"  + + + + + + + + + + + \$.\%.\&.*.+ + + + =.-.n S + ;.>.,.'.+ + + + =.5 ).p + + + + + + + + + + . ",
"  + + + + + + + + + + + !.~.{.].+ + + ^./.(._.:.+ + <.[.}.; + + + |.1.2.L + + + + + + + + + + . ",
"  + + + + + + + + + + + 3.4.5.+ + + + #.6.7.8.+ + + g 9.0.a.+ + + L { b.c.+ + + + + + + + + + . ",
"  + + + + + + + + + + + d.e.f.+ + + g.h.(.i.+ + + + + j.k.l.m.+ + n.G a o.+ + + + + + + + + + . ",
"  + + + + + + + + + + + p.q.r.+ + :.s.t.u.+ + + + + + v.w.x.y.+ + :.z.A.B.+ + + + + + + + + + . ",
"  + + + + + + + + + + + C.D.E.+ + 8.F.~ G.+ + + + + + + H.I.J.+ + W K.6.=.+ + + + + + + + + + . ",
"  + + + + + + + + + + + L.M.N.+ + O.P.Q.n.+ + + + + + + R.f.S.T.+ W U.5 =.+ + + + + + + + + + . ",
"  + + + + + + + + + + + V.W.X.+ p Y.Z.`.+ + + + + + + + + #  +.+; W s.++\@++ + + + + + + + + + . ",
"  + + + + + + + + + + + #+\$+\%++ o s \&+*++ + + + + + + + + u =+-+;+:.X Q i.+ + + + + + + + + + . ",
"  + + + + + + + + + + + >+,+'+W )+!+`.+ + + + + + + + + + + ~+{+]+^ ^+/+> + + + + + + + + + + . ",
"  + + + + + + + + + + + (+_+:+8.<+[+n.+ + + + + + + + + + + }+|+1+2+3+X *++ + + + + + + + + + . ",
"  + + + + + + + + + + + }+4+5+6+7+, + + + + + + + + + + + + m.~+8+9+0+] a++ + + + + + + + + + . ",
"  + + + + + + + + + + + g >+;.o.3+b++ + + + + + + + + + + + + - i c+, o ^ + + + + + + + + + + . ",
"  + + d+e+=.n.+ i.f+f+f+g+h+i+j+7+k+l++ W f+f+f+f+l+^ + 8.e+d+h m+n+X o+:.*+=.S + |.f+o.+ + + . ",
"  + p+/.q+s r+B.s+) ) ) ) t+u+v+Z.w+1.x+^.) ) ) ) s+|.+ y+z+A+y B+C+! /+D+E+s F+G+H+) b.I+J++ . ",
"  + K+r . G+L+M+l+=.N+Q =.O+P+s Q+R+S+Y.o.M [+=.=.T+^ J+U+V+W+t X+Y+Z+F+l+`+ \@.\@+\@\@\@o #\@S+\$\@+ . ",
"  + \%\@(.\&\@X <+*\@W + Q =\@+ v.-\@0 )+;\@+\@>\@T+,\@\@\@'\@'\@0++ k+! )\@!\@s.~\@{\@Y a o+Y.]\@t.Q S+^\@;\@/\@> + . ",
"  + t K+L+(\@~ _\@p + Q =\@+ m.:\@Y H+s <\@[\@D+}\@|\@1\@1\@X + #\@! q ) 7.2\@3\@4\@4\@5\@6\@7\@(.0+8\@w+9\@n 0++ . ",
"  + ^+)+e+0\@a\@R d++ Q =\@+ p   b\@c\@o d\@^\@D+e\@f\@d+d+n.:.g\@M 9\@h\@0 i\@>\@j\@k\@{ q+' m f\@G+l+[\@S+l\@+ . ",
"  + ).<\@G A+R m\@^.+ Q =\@+ d+n\@o\@K L 0 7 d+,\@7._\@_\@O.`.h\@).3+p\@F q\@r\@s\@t\@M Y.M+e\@N+6 A+G 9\@\@.+ . ",
"  + c.u\@\@\@{ `+v\@W + M+_\@+ 3 Q.w\@x\@y\@l\@p\@d+!+!+!+!+A+;\@z\@B.+ b+A\@M+B\@C\@D\@' )+. ' E\@F\@!+m\@s.J++ . ",
"  + + p d+L a++ + + *+*++ i.G\@H\@I\@R.8.*+W J+J+J+J+*+^.d+:.+ W 3 J\@K\@L\@M\@d+a+a+d++ ^.J+8.:.+ + . ",
"  + + + + + + + + + + + + f+N\@O\@P\@;++ + + + + + + + + + + + + \@+y+Q\@R\@S\@+ + + + + + + + + + + . ",
"  + + + + + + + + + + + ^ J\@T\@U\@V\@W\@+ + + + + + + + + + + + a+O.X\@Y\@Z\@`\@; + + + + + + + + + + . ",
"  + + + + + + + + + + + 8.K. #K [ .#g + + + + + + + + + + + t r++#\@###\$#m.+ + + + + + + + + + . ",
"  + + + + + + + + + + + T+m\@2.; \%#\&#*#+ + + + + + + + + + + /\@s q K =#-#;#+ + + + + + + + + + . ",
"  + + + + + + + + + + + g.  Q + >#,#'#)#+ + + + + + + + + :.H+!#6+K ~#{#v.+ + + + + + + + + + . ",
"  + + + + + + + + + + + B.n ]#+ / ^#/#(#+ + + + + + + + + _#<\@:#W K <#[#~\@+ + + + + + + + + + . ",
"  + + + + + + + + + + + p+|\@|\@+ + }#|#1#2#+ + + + + + + d+Z z+J++ 3#4#5#2#+ + + + + + + + + + . ",
"  + + + + + + + + + + + T+N+]\@+ + ;#6#7#8#+ + + + + + + x+-.8 + + 9#0#a#)#+ + + + + + + + + + . ",
"  + + + + + + + + + + + d+Q W+W + ; b#c#d#; + + + + + 3 e#f#3 + + (+g#h#i#+ + + + + + + + + + . ",
"  + + + + + + + + + + + W U+Y |.+ + )#j#k#P\@+ + + + + l#m##.W + + !.n#o#; + + + + + + + + + + . ",
"  + + + + + + + + + + + + A\@p#0++ + + q#_ r#s#+ + + |.r 7.> + + + t#7#u#+ + + + + + + + + + + . ",
"  + + + + + + + + + + + + ;\@6.'\@+ + + W v#w#x#y#+ n.z\@z#R++ + + + _+A#B#+ + + + + + + + + + + . ",
"  + + + + + + + + + + + + 6+C#D#D++ + + m.E#F#G#^.\%\@H#E++ + + + + I#J#9#+ + + + + + + + + + + . ",
"  + + + + + + + + + + + + + m\@9\@0++ + + + K#L#M#N#S+K+^ + + + + X+O#P#; + + + + + + + + + + + . ",
"  + + + + + + + + + + + + + 0+t.Q#*++ + + ; R#S#T#U#6++ + + + ; V#W#X#+ + + + + + + + + + + + . ",
"  + + + + + + + + + + + + + n.Q.Y Y#D++ + Z#`#~  \$.\$+\$}++ + i#\@\$#\$\$\$+ + + + + + + + + + + + + . ",
"  + + + + + + + + + + + + + + l+' d\@\%\$U.A+\&\$H+f\@*\$=\$-\$;\$>\$,\$'\$\&#;\$)\$+ + + + + + + + + + + + + . ",
"  + + + + + + + + + + + + + + + 3 ;\@!\$\%\$4 <+S p + K ~\${\$]\$^\$/\$(\$_\$+ + + + + + + + + + + + + + . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . "};
EOF

sub assign {
	my $mw = shift;
	$mw->Pixmap('icon', -data=>$iconImage,);
}

# -------------------------------------
package MyWidget;
use strict;
# -------------------------------------

sub new {
	my ($type,$parent,@args) = @_;
	my $self = {};
	bless $self, $type;
	$self->{'parent'} = $parent;
	return $self;
}
sub destroy {
	my ($self,@args) = @_;
	$self->SUPER::destroy(@args);
	delete $self->{'parent'};
	delete $self->{'widget'};
	delete $self->{'tree'};
}
sub setframe {
	my ($self,$frame,@args) = @_;
	$self->{'frame'} = $frame;
	$frame->{_myobject} = $self;
	return $frame;
}
sub setwidget {
	my ($self,$widget,@args) = @_;
	$self->{'widget'} = $widget;
	$widget->{_myobject} = $self;
	return $widget;
}
sub setmainwindow {
	my ($self,$widget,@args) = @_;
	$self->setwidget($widget);
	$widget->bind('<Destroy>',
		[sub {
			my ($widget,$self,@args) = @_;
			if ($widget) {
				if ($self = $widget->{_myobject}) {
					delete $widget->{_myobject};
					$self->destroy;
				}
			}
		}, $self],
	);
	return $widget;
}
sub parent {
	shift->{'parent'}
}
sub top {
	my $self = shift;
	my $top = $self;
	while (defined $top->{'parent'}) {
		$top = $top->{'parent'};
	}
	return $top;
}
sub toplevel {
	my $self = shift;
	if (defined $self->widget) {
		return $self->widget->toplevel;
	}
	return $self->parent->toplevel;
}
sub widget {
	shift->{'widget'};
}
sub frame {
	shift->{'frame'};
}
sub attachballoon {
	my ($self,@args) = @_;
	return $self->parent->attachballoon(@args);
}
sub statusmsg {
	my ($self,@args) = @_;
	return $self->parent->statusmsg(@args);
}
sub configure {
	my ($self,@args) = @_;
	return $self->widget->configure(@args);
}
sub cget {
	my ($self,@args) = @_;
	return $self->widget->cget(@args);
}
sub pack {
	my ($self,@args) = @_;
	return $self->widget->pack(@args);
}


# -------------------------------------
package MyMainWindow;
use strict;
use vars qw(@ISA);
@ISA = qw(MyWidget);
# -------------------------------------

@MyMainWindow::windows = ();

sub new {
	my ($type,$title,@args) = @_;
	my $self = MyWidget::new($type,undef,@args);
	$self->{'data'}->{'title'} = $title;
	my $mw = Tk::MainWindow->new;
	$self->setmainwindow($mw);
	$mw->title($title);
	$mw->minsize(600,400);
	$mw->geometry('1024x768');
	#$mw->protocol('WM_DELETE_WINDOW',[\&wm_delete_window,$self],);
	#$mw->protocol('WM_SAVE_YOURSELF',[\&wm_save_yourself,$self],);
	#$mw->protocol('WM_TAKE_FOCUS',   [\&wm_take_focus,   $self],);
	push @MyMainWindow::windows, $self;
	MyOptions::assign($mw);
	MyPixmaps::assign($mw);
	$mw->iconimage('icon');
	$mw->iconname($title);
	return $self;
}

sub destroy {
	my ($self,@args) = @_;
	for (my $i = 0; $i < @MyMainWindow::windows; $i++) {
		if ($MyMainWindow::windows[$i] eq $self) {
			splice @MyMainWindow::windows, $i, 1;
			last;
		}
	}
}

sub wm_delete_window {
	my ($self,@args) = @_;
}

sub wm_save_yourself {
	my ($self,@args) = @_;
}

sub wm_take_focus {
	my ($self,@args) = @_;
}

# -------------------------------------
package MyCanvas;
use strict;
use vars qw(@ISA);
@ISA = qw(MyWidget);
# -------------------------------------

sub new {
	my ($type,$parent,$width,@args) = @_;
	my $self = MyWidget::new($type,$parent,$width,@args);
	$width = 1024 unless defined $width;
	my $c = $parent->widget->Canvas(
		-confine=>1,
		-width=>$width,
		-xscrollincrement=>25,
		-yscrollincrement=>25,
		-background=>'white',
	)->pack(
		-expand=>1,
		-fill=>'both',
		-side=>'left',
		-anchor=>'w',
	);
	$self->{'top'} = $parent->widget;
	$self->setwidget($c);
	$main::canvas = $c;
	return $self;
}

# -------------------------------------
package MyTop;
use strict;
use vars qw(@ISA);
@ISA = qw(MyMainWindow);
# -------------------------------------

@MyTop::myapps = ();
$MyTop::appnum = 0;

sub new {
	my ($type,$filename,$number,@args) = @_;
	my $title = 'OpenSS7 SS7 Analyzer';
	$title .= ": $filename" if defined $filename;
	my $appno = defined $number ? $number : $MyTop::appnum;
	my $self = MyMainWindow::new($type,$title,$filename,$appno,@args);
	$self->createmenubar;
	$self->createstatusbar;
	$self->createballoon;
	$self->createcanvas;
	push @MyTop::myapps, $self;
	$MyTop::appnum = $appno if $appno > $MyTop::appnum;
	return $self;
}

sub createmenubar {
	my $self = shift;
	my $w = $self->widget;
	my ($mb,$mi,$mc);
	$mb = $self->{'MenuBar'} = $w->Menu(
		-type=>'menubar',
	);
	$w->configure(-menu=>$mb);
	$self->{'Dialog'} = $self->toplevel->Dialog(
		-buttons=>[qw/Ok Cancel Help/],
		-default_button=>'Cancel',
		-text=>'Please input capture file or interface from which to load.',
		-title=>'Source Selection',
	);
	$mi = $self->{'FileMenu'} = $mb->Menu(
		-tearoff=>1,
		-title=>'File Menu',
		-title=>'normal',
	);
	$mi->add('command',
		-label=>'New',
		-underline=>0,
		-command=>[\&MyTop::menuFileNew,$self],
	);
	$mi->add('command',
		-label=>'Read...',
		-underline=>0,
		-command=>[\&MyTop::menuFileRead,$self],
	);
	$mi->add('command',
		-label=>'Open...',
		-underline=>0,
		-command=>[\&MyTop::menuFileOpen,$self],
	);
	$mi->add('command',
		-label=>'Save',
		-underline=>0,
		-command=>[\&MyTop::menuFileSave,$self],
	);
	$mi->add('command',
		-label=>'Save As...',
		-underline=>0,
		-command=>[\&MyTop::menuFileSaveAs,$self],
	);
	$mi->add('command',
		-label=>'Close',
		-underline=>0,
		-command=>[\&MyTop::menuFileClose,$self],
	);
	$mi->add('separator');
	$mi->add('command',
		-label=>'Properties...',
		-underline=>0,
		-command=>[\&MyTop::menuFileProperties,$self],
	);
	$mc = $mi->Menu(
		-tearoff=>1,
		-title=>'Recent Files',
	);
	$mi->add('cascade',
		-menu=>$mc,
		-label=>'Recent Files...',
		-underline=>0,
		-state=>'disabled',
	);
	$self->{'RecentMenu'} = $mc;
	$mi->add('separator');
	$mi->add('command',
		-label=>'Exit All',
		-underline=>1,
		-command=>[\&MyTop::menuFileExitAll,$self],
	);
	$mi->add('command',
		-label=>'Debug Dump',
		-underline=>1,
		-command=>[sub {
			my $parent = shift->{'FileMenu'};
			my $w;
			while ($w = $parent) {
				$parent = $w->parent;
				print $w;
				foreach (@{$w->configure}) {
					print "Option: ".join(', ',@$_);
				}
			}
		}, $self],
	);
	$mb->add('cascade',
		-menu=>$mi,
		-label=>'File',
		-underline=>0,
	);
	$self->{'FileMenu'} = $mi;
}

sub createstatusbar {
	my $self = shift;
	my $sb = $self->toplevel->Message(
		-text=>"Status bar.",
		-justify=>'left',
		-width=>'400',
		-anchor=>'w',
	);
	$sb->pack(
		-expand=>0,
		-fill=>'x',
		-side=>'bottom',
		-anchor=>'sw',
	);
	$self->{'Message'} = $sb;
}

sub createballoon {
	my $self = shift;
	$_ = $self->{'Balloon'} = $self->toplevel->Balloon(
		#-initwait=>1000,
		-statusbar=>$self->{'Message'},
	);
	$::balloonwidget = $_;
	return $_;
}

sub createcanvas {
	my ($self,$width,@args) = @_;
	my $c = MyCanvas->new($self,$width,@args);
#	for (my ($x,$y) = (50,40); $x < 1000; $x=$x+50,$y=$y+40) {
#		$c->widget->createImage($x,$y,
#			-anchor=>'center',
#			-image=>'icon',
#			-tags=>'icon',
#		);
#	}
#	$c->widget->createImage(500,400,
#		-anchor=>'center',
#		-image=>'icon',
#		-tags=>( 'icon' ),
#	);
}

sub attachballoon {
	my ($self,@args) = @_;
	return $self->{'Balloon'}->attach(@args) if $self->{'Balloon'};
}
sub statusmsg {
	my ($self,$msg) = @_;
	$self->{'Message'}->configure(-text=>$msg) if $self->{'Message'};
	printf STDERR "$msg\n";
}

sub menuFileOpen {
	my $self = shift;
	my $data = $self->{'data'};
	my $file = $self->widget->getOpenFile(
		-defaultextension=>'.pcap',
		-initialdir=>'./testdata',
		-initialfile=>'ss7capall.pcap',
		-title=>'Open Dialog',
	);
	return unless $file;
	my $err = '';
	my $pcap;
	unless ($pcap = Net::Pcap::pcap_open_offline($file, \$err)) {
		my $d = $self->widget->Dialog(
			-title=>'Could not open file.',
			-text=>$err,
			-default_button=>'Ok',
			-buttons=>[ 'Ok' ],
		);
		$d->Show;
		$d->destroy;
		return;
	}
	my $fh = Net::Pcap::pcap_file($pcap);
	Tk::Event::IO->fileevent($fh, 'readable' => [\&MyTop::readmsg,$self,$pcap,$fh]);
}

sub readmsg {
	my ($self,$pcap,$fh,@args) = @_;
	if (my $msg = Message->create($pcap)) {
		$msg->process();
	} else {
		Tk::Event::IO->fileevent($fh, 'readable' => '');
		print STDERR "closing file\n";
		Net::Pcap::pcap_close($pcap);
		print STDERR "file closed\n";
	}
}

# -------------------------------------
package main;
use strict;
# -------------------------------------

if (length @infiles == 0) {
	@infiles = ( $infile );
}

#foreach $infile (@infiles) {
#	my $infile = "./testdata/ss7capall.pcap";
#	my $err = '';
#	my $pcap = Net::Pcap::pcap_open_offline($infile, \$err)
#		or die "Can't read '$infile': $err\n";
#
#	my $pcapswap = Net::Pcap::pcap_is_swapped($pcap);
#	my $pcapmaj = Net::Pcap::pcap_major_version($pcap);
#	my $pcapmin = Net::Pcap::pcap_minor_version($pcap);
#	my %stats;
#	Net::Pcap::pcap_stats($pcap, \%stats);
#	my $linktype = Net::Pcap::pcap_datalink($pcap);
#	my $linkname = Net::Pcap::pcap_datalink_val_to_name($linktype);
#	my $linkdesc = Net::Pcap::pcap_datalink_val_to_description($linktype);
#
#	print "File: $infile\n";
#	print "Swapped: $pcapswap\n";
#	print "Major Version: $pcapmaj\n";
#	print "Minor Version: $pcapmin\n";
#	print "Data Link Type: $linktype\n";
#	print "Data Link Type Name: $linkname\n";
#	print "Data Link Type Desc: $linkdesc\n";
#	print "Packets capt: $stats{'ps_recv'}\n";
#	print "Packets drop: $stats{'ps_drop'}\n";
#	print "Packets ifdrop: $stats{'ps_ifdrop'}\n";
#		
#	my $msg;
#
#	while (($msg = Message->create($pcap)) != undef) {
#		$msg->process();
#	}
#	Net::Pcap::pcap_close($pcap);
#
#	print "File '$infile' contains '$count' messages\n";
#}

MyTop->new;

Tk::MainLoop;

exit;

1;
