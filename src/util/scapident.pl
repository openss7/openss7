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

use Carp;

use Net::Pcap qw(:functions);
use Tk;
use Tk::Xrm;
use Tk::Event;
use Tk::Trace;
require Tk::Toplevel;
require Tk::Adjuster;
require Tk::Dialog;
require Tk::HList;
require Tk::ItemStyle;
require Tk::ROText;
require Tk::NoteBook;
require Tk::Pane;
require Tk::Balloon;
require Tk::DragDrop;
require Tk::DropSite;
require Tk::TableMatrix;
require Tk::TableMatrix::Spreadsheet;
require Tk::Frame;
require Tk::TFrame;

use Data::Dumper;
use Date::Parse;
use XML::Simple;
use XML::SAX;
use XML::SAX::Base;
#use XML::SAX::ParseFactory;
use XML::Dumper;

use Time::HiRes qw(gettimeofday);

# distributions of message priorities should be:
# priority 3 0%-5%, 2 0%-20%, 1 30%-45%, 0 30%-45%
#
my %mtypes = (
	0=>{
		0x00 => ['snmm', 0x0, 'SnmmMessage'],
		0x11 => ['coo',  0x8, 'CooMessage'],
		0x12 => ['coa',  0x8, 'CoaMessage'],
		0x15 => ['cbd',  0x8, 'CbdMessage'],
		0x16 => ['cba',  0x8, 'CbaMessage'],
		0x21 => ['eco',  0x8, 'EcoMessage'],
		0x22 => ['eca',  0x8, 'EcaMessage'],
		0x31 => ['rct',  0x8, 'RctMessage'],
		0x32 => ['tfc',  0x8, 'TfcMessage'],
		0x41 => ['tfp',  0x8, 'TfpMessage'],
		0x42 => ['tcp',  0x8, 'TcpMessage'],
		0x43 => ['tfr',  0x8, 'TfrMessage'],
		0x44 => ['tcr',  0x8, 'TcrMessage'],
		0x45 => ['tfa',  0x8, 'TfaMessage'],
		0x46 => ['tca',  0x8, 'TcaMessage'],
		0x51 => ['rst',  0xf, 'RstMessage'],
		0x52 => ['rsr',  0x8, 'RsrMessage'],
		0x53 => ['rcp',  0x8, 'RcpMessage'],
		0x54 => ['rcr',  0x8, 'RcrMessage'],
		0x61 => ['lin',  0x8, 'LinMessage'],
		0x62 => ['lun',  0x8, 'LunMessage'],
		0x63 => ['lia',  0x8, 'LiaMessage'],
		0x64 => ['lua',  0x8, 'LuaMessage'],
		0x65 => ['lid',  0x8, 'LidMessage'],
		0x66 => ['lfu',  0x8, 'LfuMessage'],
		0x67 => ['llt',  0x8, 'LltMessage'],
		0x68 => ['lrt',  0x8, 'LrtMessage'],
		0x71 => ['tra',  0x8, 'TraMessage'],
		0x72 => ['trw',  0x8, 'TrwMessage'],
		0x81 => ['dlc',  0x8, 'DlcMessage'],
		0x82 => ['css',  0x8, 'CssMessage'],
		0x83 => ['cns',  0x8, 'CnsMessage'],
		0x84 => ['cnp',  0x8, 'CnpMessage'],
		0xa1 => ['upu',  0x8, 'UpuMessage'],
		0xa2 => ['upa',  0x8, 'UpaMessage'],
		0xa3 => ['upt',  0x8, 'UptMessage'],
	},
	1=>{
		0x00 => ['sntm', 0x0, 'SntmMessage'],
		0x11 => ['sltm', 0x8, 'SltmMessage'],
		0x12 => ['slta', 0x8, 'SltaMessage'],
	},
	2=>{
		0x00 => ['snsm', 0x0, 'SnsmMessage'],
		0x11 => ['sltm', 0x8, 'SltmMessage'],
		0x12 => ['slta', 0x8, 'SltaMessage'],
	},
	3=>{
		0x00 => ['sccp',  0x0, 'SccpMessage'],
		0x01 => ['cr',    0x3, 'CrMessage'],
		0x02 => ['cc',    0x2, 'CcMessage'],
		0x03 => ['cref',  0x2, 'CrefMessage'],
		0x04 => ['rlsd',  0x4, 'RlsdMessage'],
		0x05 => ['rlc',   0x4, 'RlcMessage'],
		0x06 => ['dt1',   0x3, 'Dt1Message'],
		0x07 => ['dt2',   0x3, 'Dt2Message'],
		0x08 => ['ak',    0x3, 'AkMessage'],
		0x09 => ['udt',   0x7, 'UdtMessage'],
		0x0a => ['udts',  0x7, 'UdtsMessage'],
		0x0b => ['ed',    0x2, 'EdMessage'],
		0x0c => ['ea',    0x2, 'EaMessage'],
		0x0d => ['rsr',   0x2, 'RsrMessage'],
		0x0e => ['rsc',   0x2, 'RscMessage'],
		0x0f => ['err',   0x2, 'ErrMessage'],
		0x10 => ['it',    0x2, 'ItMessage'],
		0x11 => ['xudt',  0x7, 'XudtMessage'],
		0x12 => ['xudts', 0x7, 'XudtsMessage'],
		0x13 => ['ludt',  0x7, 'LudtMessage'],
		0x14 => ['ludts', 0x7, 'LudtsMessage'],
	},
	5=>{
		0x00 => ['isup', 0x0, 'IsupMessage'],
		0x01 => ['iam',  0x3, 'IamMessage'],
		0x02 => ['sam',  0x0, 'SamMessage'],
		0x03 => ['inr',  0x2, 'InrMessage'],
		0x04 => ['inf',  0x2, 'InfMessage'],
		0x05 => ['cot',  0x2, 'CotMessage'],
		0x06 => ['acm',  0x2, 'AcmMessage'],
		0x07 => ['con',  0x0, 'ConMessage'],
		0x08 => ['fot',  0x2, 'FotMessage'],
		0x09 => ['anm',  0x4, 'AnmMessage'],
		0x0c => ['rel',  0x2, 'RelMessage'],
		0x0d => ['sus',  0x2, 'SusMessage'],
		0x0e => ['res',  0x2, 'ResMessage'],
		0x10 => ['rlc',  0x4, 'RlcMessage'],
		0x11 => ['ccr',  0x2, 'CcrMessage'],
		0x12 => ['rsc',  0x1, 'RscMessage'],
		0x13 => ['blo',  0x1, 'BloMessage'],
		0x14 => ['ubl',  0x1, 'UblMessage'],
		0x15 => ['bla',  0x1, 'BlaMessage'],
		0x16 => ['uba',  0x1, 'UbaMessage'],
		0x17 => ['grs',  0x1, 'GrsMessage'],
		0x18 => ['cgb',  0x1, 'CgbMessage'],
		0x19 => ['cgu',  0x1, 'CguMessage'],
		0x1a => ['cgba', 0x1, 'CgbaMessage'],
		0x1b => ['cgua', 0x1, 'CguaMessage'],
		0x1c => ['cmr',  0x0, 'CmrMessage'],
		0x1d => ['cmc',  0x0, 'CmcMessage'],
		0x1e => ['cmrj', 0x0, 'CmrjMessage'],
		0x1f => ['far',  0x0, 'FarMessage'],
		0x20 => ['faa',  0x0, 'FaaMessage'],
		0x21 => ['frj',  0x0, 'FrjMessage'],
		0x22 => ['fad',  0x0, 'FadMessage'],
		0x23 => ['fai',  0x0, 'FaiMessage'],
		0x24 => ['lpa',  0x1, 'LpaMessage'],
		0x25 => ['csvq', 0x0, 'CsvqMessage'],
		0x26 => ['csvr', 0x0, 'CsvrMessage'],
		0x27 => ['drs',  0x0, 'DrsMessage'],
		0x28 => ['pam',  0x7, 'PamMessage'],
		0x29 => ['gra',  0x1, 'GraMessage'],
		0x2a => ['cqm',  0x1, 'CqmMessage'],
		0x2b => ['cqr',  0x1, 'CqrMessage'],
		0x2c => ['cpg',  0x2, 'CpgMessage'],
		0x2d => ['usr',  0x0, 'UsrMessage'],
		0x2e => ['ucic', 0x2, 'UcicMessage'],
		0x2f => ['cfn',  0x1, 'CfnMessage'],
		0x30 => ['olm',  0x0, 'OlmMessage'],
		0x31 => ['crg',  0x0, 'CrgMessage'],
		0x32 => ['nrm',  0x0, 'NrmMessage'],
		0x33 => ['fac',  0x7, 'FacMessage'],
		0x34 => ['upt',  0x0, 'UptMessage'],
		0x35 => ['upa',  0x0, 'UpaMessage'],
		0x36 => ['idr',  0x0, 'IdrMessage'],
		0x37 => ['irs',  0x0, 'IrsMessage'],
		0x38 => ['sgm',  0x1, 'SgmMessage'],
		0xe9 => ['cra',  0x1, 'CraMessage'],
		0xea => ['crm',  0x1, 'CrmMessage'],
		0xeb => ['cvr',  0x1, 'CvrMessage'],
		0xec => ['cvt',  0x1, 'CvtMessage'],
		0xed => ['exm',  0x7, 'ExmMessage'],
		0xf8 => ['non',  0x0, 'NonMessage'],
		0xfc => ['llm',  0x0, 'LlmMessage'],
		0xfd => ['cak',  0x0, 'CakMessage'],
		0xfe => ['tcm',  0x0, 'TcmMessage'],
		0xff => ['mcp',  0x0, 'McpMessage'],
	},
);

our $pc_assignments;

if (0) {
	my $pcassign;
	eval { $pcassign = XMLin("$progdir/pc_assign.xml") };
	unless ($@) {
		#print STDERR "D: using PC assignments from $progdir/pcassignments.xml\n";
		# XML::Simple does not like tag names of '0' so we save them as 256.
		$pc_assignments = $pcassign;
		foreach my $k (keys %{$pc_assignments->{5}}) {
			if (exists $pc_assignments->{5}->{$k}->{256}) {
				$pc_assignments->{5}->{$k}->{0} =
				$pc_assignments->{5}->{$k}->{256};
				delete $pc_assignments->{5}->{$k}->{256};
			}
		}
	}
}

if (1) {
	my $pcassign;
	my $dumper = new XML::Dumper;
	eval { $pcassign = $dumper->xml2pl("$progdir/pc_assignments.xml.gz") };
	unless ($@) {
		#print STDERR "D: using PC assignments from $progdir/pc_assignments.xml.gz\n";
		$pc_assignments = $pcassign;
	}
}

use constant {
	COL_NOD => 5,
	COL_SSP => 4,
	COL_SCP => 3,
	COL_GTT => 2,
	COL_ADJ => 1,

	CTS_UNINIT	=> 0,
	CTS_IDLE	=> 1,
	CTS_WAIT_ACM	=> 2,
	CTS_WAIT_ANM	=> 3,
	CTS_ANSWERED	=> 4,
	CTS_SUSPENDED	=> 5,
	CTS_WAIT_RLC	=> 6,
	CTS_SEND_RLC	=> 7,
	CTS_COMPLETE	=> 8,

	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4,	# also RL length in octets
	RT_24BIT_PC => 7,	# also RL length in octets

	HT_UNKNOWN => 0,
	HT_BASIC => 3,		# also link level header length
	HT_EXTENDED => 6,	# also link level header length

	MP_UNKNOWN => 0,
	MP_JAPAN => 1,
	MP_NATIONAL => 2,
	MP_INTERNATIONAL => 3,
};
our @rtoptions = (
	['Unknown'		=> RT_UNKNOWN		],
	['14-bit point code'	=> RT_14BIT_PC		],
	['24-bit point code'	=> RT_24BIT_PC		],
);
our @htoptions = (
	['Unknown'		=> HT_UNKNOWN		],
	['Basic'		=> HT_BASIC		],
	['Extended'		=> HT_EXTENDED		],
);
our @mpoptions = (
	['Unknown'		=> MP_UNKNOWN		],
	['Japan'		=> MP_JAPAN		],
	['National'		=> MP_NATIONAL		],
	['International'	=> MP_INTERNATIONAL	],
);

# -------------------------------------
package style;
use strict;
# -------------------------------------

my %tframestyle = (
	-relief=>'groove',
	-borderwidth=>2,
);
my %tframepack = (
	-expand=>1,
	-fill=>'both',
	-side=>'top',
	-anchor=>'n',
	-padx=>3,
	-pady=>1,
);
my %tframescrunch = (
	-expand=>0,
	-fill=>'x',
	-side=>'top',
	-anchor=>'n',
	-padx=>3,
	-pady=>1,
);
my %labelright = (
	-anchor=>'e',
	-justify=>'right',
	#-relief=>'groove',
	#-borderwidth=>2,
);
my %labelcenter = (
	-anchor=>'s',
	-justify=>'center',
	#-relief=>'groove',
	#-borderwidth=>2,
);
my %labelgrid = (
);
my %entryinput = (
	-background=>'white',
	-exportselection=>1,
);
my %entryleft = (
	%entryinput,
	-state=>'readonly',
	#-relief=>'groove',
	#-borderwidth=>2,
);
my %entryright = (
	%entryleft,
	-justify=>'right',
	-width=>8,
);
my %entrycenter = (
	%entryleft,
	-justify=>'center',
);
my %entrygrid = (
);
my %buttonleft = (
	-anchor=>'w',
	-justify=>'left',
	-indicatoron=>1,
	-state=>'disabled',
);
my %optionleft = (
	-anchor=>'w',
	-justify=>'left',
	-indicatoron=>0,
);

# -------------------------------------
package Logging;
use strict;
# -------------------------------------

#package Logging;
sub init {
	my ($self,$top) = @_;
	$self->{logs} = [];
	$self->{top} = $top;
}

#package Logging;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Logging;
sub showlog {
	my ($self,$top,$X,$Y) = @_;

	my $tw;
	if ($tw = $self->{logw}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
		$tw->MapWindow;
		return;
	}
	my $title = $self->identify." Logs";
	my $tl = $top->widget->toplevel;
	$tw = $tl->Toplevel(
		-title=>$title,
		Name=>'logWindow',
		-class=>'SS7view',
	);
	$tw->group($tl);
	#$tw->transient($tl);
	$tw->iconimage('icon');
	$tw->iconname($title);
	#$tw->resizable(0,0);
	$tw->positionfrom('user');
	$tw->geometry("+$X+$Y");
	$tw->protocol('WM_DELETE_WINDOW', [sub {
		my ($self) = @_;
		my $tw = $self->{logw};
		delete $self->{logw};
		$tw->destroy;
	},$self]);
	$self->{logw} = $tw;
	my $sc = $tw->Scrolled('ROText',
		-scrollbars=>'osoe',
	)->pack(
		-expand=>1,
		-fill=>'both',
		-side=>'left',
		-anchor=>'w',
	);
	my $ro = $sc->Subwidget('scrolled');
	$ro->delete('0.0', 'end');
	$ro->insert('end', join("\n",@{$self->{logs}}));
	$tw->update;
	$tw->MapWindow;
}

#package Logging;
sub log {
	my ($self,$text) = @_;
	push @{$self->{logs}}, $text;
	$self->{top}->setstatus($self->identify.' '.$text);
}

# -------------------------------------
package Counts;
use strict;
# -------------------------------------

#package Counts;
sub init {
	my ($self,$interval,@args) = @_;
	$self->{interval} = $interval;
}

#package Counts;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Counts;
sub addto {
	my ($self,$othr,@args) = @_;
	return $othr->addfrom($self,@args);
}

# -------------------------------------
package MsgCounts;
use strict;
use vars qw(@ISA);
@ISA = qw(Counts);
# -------------------------------------

#package MsgCounts;
sub init {
	my ($self,$top,@args) = @_;
	$self->Counts::init(@args);
	$self->{incs} = {0=>{},1=>{},2=>{}};
	$self->{top} = $top;
}

sub DESTROY {
	my $self = shift;
	$self->{TopLevel}->traceVdelete(\$self->{mcnt_text});
}

#package MsgCounts;
sub inc {
	my ($self,$msg,$dir) = @_;
	#print STDERR "MsgCounts::inc called by ", $self->identify, "\n";
	my $li = $msg->{li};
	$self->{incs}->{$dir}->{sus} += 1;
	$self->{incs}->{2}->{sus} += 1 unless $dir == 2;
	if (exists $self->{mcnt}) {
		$self->{top}->canvas->itemconfigure($self->{mcnt},-text=>$self->{incs}->{2}->{sus});
	}
	if ($li == 0) {
		$self->{incs}->{$dir}->{fisus} += 1;
		$self->{incs}->{2}->{fisus} += 1 unless $dir == 2;
		return;
	}
	if ($li == 1) {
		$self->{incs}->{$dir}->{lssus} += 1;
		$self->{incs}->{2}->{lssus} += 1 unless $dir == 2;
		return;
	}
	if ($li == 2) {
		$self->{incs}->{$dir}->{lss2s} += 1;
		$self->{incs}->{2}->{lss2s} += 1 unless $dir == 2;
		return;
	}
	my $si = $msg->{si};
	my $mt = $msg->{mt};
	my $mp = $msg->{mp};
	$self->{incs}->{$dir}->{msus} += 1;
	$self->{incs}->{$dir}->{sis}->{$si}->{msus} += 1;
	$self->{incs}->{$dir}->{sis}->{$si}->{$mt}->{msus} += 1;
	$self->{incs}->{$dir}->{sis}->{$si}->{$mt}->{$mp} += 1;
	$self->{incs}->{$dir}->{mp}->{$mp} += 1;
	unless ($dir == 2) {
		$self->{incs}->{2}->{msus} += 1;
		$self->{incs}->{2}->{sis}->{$si}->{msus} += 1;
		$self->{incs}->{2}->{sis}->{$si}->{$mt}->{msus} += 1;
		$self->{incs}->{2}->{sis}->{$si}->{$mt}->{$mp} += 1;
		$self->{incs}->{2}->{mp}->{$mp} += 1;
	}
}

#package MsgCounts;
sub addfrom {
	my ($self,$othr,$flip,@args) = @_;

	return unless $othr->isa('MsgCounts');

	foreach my $odir (keys %{$othr->{incs}}) {
		my $sdir = $odir ^ $flip;
		foreach my $su (qw/sus fisus lssus lss2s msus/) {
			next unless exists $othr->{incs}->{$odir}->{$su};
			$self->{incs}->{$sdir}->{$su} +=
			$othr->{incs}->{$odir}->{$su};
		}
		if (exists $othr->{incs}->{$odir}->{mp}) {
			foreach my $mp (keys %{$othr->{incs}->{$odir}->{mp}}) {
				$self->{incs}->{$sdir}->{mp}->{$mp} +=
				$othr->{incs}->{$odir}->{mp}->{$mp};
			}
		}
		if (exists $othr->{incs}->{$odir}->{sis}) {
			foreach my $si (keys %{$othr->{incs}->{$odir}->{sis}}) {
				foreach my $mt (keys %{$othr->{incs}->{$odir}->{sis}->{$si}}) {
					if ($mt eq 'msus') {
						$self->{incs}->{$sdir}->{sis}->{$si}->{msus} +=
						$othr->{incs}->{$odir}->{sis}->{$si}->{msus};
						next;
					}
					foreach my $mp (keys %{$othr->{incs}->{$odir}->{sis}->{$si}->{$mt}}) {
						$self->{incs}->{$sdir}->{sis}->{$si}->{$mt}->{$mp} +=
						$othr->{incs}->{$odir}->{sis}->{$si}->{$mt}->{$mp};
					}

				}
			}
		}
	}
}


# -------------------------------------
package CallCounts;
use strict;
use vars qw(@ISA);
@ISA = qw(Counts);
# -------------------------------------

#package CallCounts;
sub init {
	my ($self,$top,@args) = @_;
	$self->{pegs} = {0=>{},1=>{},2=>{}};
	$self->{sims} = {0=>{},1=>{},2=>{}};
	$self->{durs} = {0=>{},1=>{},2=>{}};
	$self->{iats} = {0=>{},1=>{},2=>{}};
	$self->{itvs} = {0=>{},1=>{},2=>{}};
	$self->{plots} = {};
	$self->{plots}->{pdfh} = {0=>{},1=>{},2=>{}};
	$self->{plots}->{cdfh} = {0=>{},1=>{},2=>{}};
	$self->{plots}->{phsh} = {0=>{},1=>{},2=>{}};
	$self->{plots}->{pdfa} = {0=>{},1=>{},2=>{}};
	$self->{plots}->{cdfa} = {0=>{},1=>{},2=>{}};
	$self->{plots}->{phsa} = {0=>{},1=>{},2=>{}};
	$self->{ciccnt} = 0;
	$self->{actcnt} = {0=>0,1=>0,2=>0,3=>0};
	$self->Counts::init(@args);
}

#package CallCounts;
sub peg {
	my ($self,$top,$event,$ts,$dir) = @_;
	print STDERR "E: peg called with direction of $dir\n" if $dir != 0 && $dir != 1 && $dir != 2;
	$self->{pegs}->{$dir}->{$event} += 1;
	$self->{pegs}->{2}->{$event} += 1 unless $dir == 2;
	#$self->iat($top,$event,$ts,$dir);
}

#package CallCounts;
sub act {
	my ($self,$top) = @_;
	$self->{actcnt}->{0} += 1;
}

#package CallCounts;
sub cnt {
	my ($self,$top) = @_;
	if (($self->{ciccnt} += 1) == 1) {
		if ($self->isa('Relation')) {
			my $c = $top->canvas;
			$c->addtag('circuits',withtag=>$self->{item}) if defined $c and $self->{item};
			my $state = ($top->{show}->{circuits}) ? 'normal' : 'hidden';
			$c->itemconfigure($self->{item},-state=>$state);
		}
		$self->identify;
	}
}

#package CallCounts;
sub uncnt {
	my ($self,$top) = @_;
	if (($self->{ciccnt} -= 1) == 0) {
		if ($self->isa('Relation')) {
			$top->canvas->dtag('circuits',withtag=>$self->{item}) if $self->{item};
		}
	}
}

#package CallCounts;
sub sim_up {
	my ($self,$event,$dir) = @_;
	if (my $new = $self->{sims}->{$dir}->{$event}) {
		$new->{curr}++;
		$new->{hiwa} = $new->{curr} if $new->{curr} > $new->{hiwa};
	} else {
		$self->{sims}->{$dir}->{$event} = { curr=>1,hiwa=>1,lowa=>0 };
	}
}
#package CallCounts;
sub sim_dn {
	my ($self,$event,$dir) = @_;
	if (my $old = $self->{sims}->{$dir}->{$event}) {
		$old->{curr}--;
		$old->{lowa} = $old->{curr} if $old->{curr} < $old->{lowa} || $old->{lowa} <= 0;
		if ($old->{curr} < 0) {
			$old->{hiwa} += -$old->{curr};
			$old->{lowa} += -$old->{curr};
			$old->{curr} = 0;
		}
	} else {
		$self->{sims}->{$dir}->{$event} = { curr=>0,hiwa=>0,lowa=>0 };
	}
}
#package CallCounts;
sub sim {
	my ($self,$top,$oldevent,$olddir,$newevent,$newdir) = @_;
	$self->sim_up($newevent,$newdir);
	$self->sim_up($newevent,2) unless $newdir == 2;
	$self->sim_dn($oldevent,$olddir);
	$self->sim_dn($oldevent,2) unless $olddir == 2;
}

#package CallCounts;
sub itv_dir {
	my ($self,$event,$itv,$dir) = @_;
	if (my $itvs = $self->{itvs}->{$dir}->{$event}) {
		push @{$itvs}, $itv;
	} else {
		$self->{itvs}->{$dir}->{$event} = [ $itv ];
	}
}
sub itv {
	my ($self,$top,$event,$start,$end,$dir) = @_;
	my $itv = { beg=>$start, end=>$end };
	$self->itv_dir($event,$itv,$dir);
	$self->itv_dir($event,$itv,2) unless $dir == 2;
	$self->dur($top,$event,$start,$end,$dir);
}

#package CallCounts;
sub dur {
	my ($self,$top,$event,$start,$end,$dir) = @_;
	my $dsecs = $end->{tv_sec} - $start->{tv_sec};
	my $dusec = $end->{tv_usec} - $start->{tv_usec};
	while ($dusec < 0 ) { $dsecs++; $dusec += 1000000; }
	my $dur = $dsecs + $dusec/1000000;
	$self->{durs}->{$dir}->{$event} += $dur;
	$self->{durs}->{2}->{$event} += $dur unless $dir == 2;
	my $int = $dsecs;
	$int++ if $dusec > 0;
	$self->{durs}->{dist}->{$dir}->{$event}->{$int} += 1;
	$self->{durs}->{dist}->{2}->{$event}->{$int} += 1 unless $dir == 2;
}

#package CallCounts;
sub iat_dir {
	my ($self,$event,$ts,$dir) = @_;
	if (my $iats = $self->{iats}->{$dir}->{$event}) {
		push @{$iats}, $ts;
	} else {
		$self->{iats}->{$dir}->{$event} = [ $ts ];
	}
}
sub iat {
	my ($self,$top,$event,$ts,$dir) = @_;
	$self->iat_dir($event,$ts,$dir);
	$self->iat_dir($event,$ts,2) unless $dir == 2;
}

#package CallCounts;
sub addfrom {
	my ($self,$othr,@args) = @_;

	return unless $othr->isa('CallCounts');

	# FIXME: complete this
}

# -------------------------------------
package PktCounts; use strict;
@PktCounts::ISA = qw(Counts);
# -------------------------------------

#package PktCounts;
sub init {
	my ($self,$top,@args) = @_;
	$self->Counts::init(@args);
	$self->{hits} = {0=>{},1=>{},2=>{}};
	$self->{top} = $top;
}
sub destroy {
	my ($self,$top) = @_;
	$top->widget->toplevel->traceVdelete(\$self->{pcnt_text});
	delete $self->{hits};
	delete $self->{top};
	$self->Counts::destroy($top);
}

sub DESTROY {
	my $self = shift;
	$self->{TopLevel}->traceVdelete(\$self->{pcnt_text});
}

#package PktCounts;
sub hit {
	my ($self,$msg,$dir) = @_;
}

#package PktCounts;
sub addfrom {
	my ($self,$othr,$flip,@args) = @_;

	return unless $othr->isa('PktCounts');
}

# -------------------------------------
package History; use strict;
@History::ISA = qw(Counts);
# -------------------------------------

#package History;
sub init {
	my ($self,@args) = @_;
	$self->Counts::init(@args);
	$self->{hist} = {};
}

#package History;
sub interval {
	my ($self,$ts) = @_;
	my $int = $ts->{tv_sec};
	$int++ if ($ts->{tv_usec} > 0);
	$int = int(($int + 299)/300);
	return $int;
}

#package History;
sub hist {
	my ($self,$int) = @_;
	my $obj;
	unless ($obj = $self->{hist}->{$int}) {
		$obj = $self->{hist}->{$int} = $self->newcounts($int);
	}
	return $obj;
}

##package History;
#sub newcounts {
#	my ($self,@args) = @_;
#	return Counts->new(@args);
#}

sub addfrom {
	my ($self,$othr,@args) = @_;

	return unless $othr->isa('History');

	foreach my $int (keys %{$othr->{hist}}) {
		$self->{hist}->{$int} = $self->newcounts($int) unless $self->{hist}->{$int};
		$self->{hist}->{$int}->addrom($othr->{hist}->{$int},@args);
	}
}

# -------------------------------------
package MsgHistory;
use strict;
use vars qw(@ISA);
@ISA = qw(History MsgCounts);
# -------------------------------------

sub init {
	my ($self,$top,@args) = @_;
	$self->History::init(@args);
	$self->MsgCounts::init($top,@args);
}

#package MsgHistory;
sub newcounts {
	my ($self,$int) = @_;
	return MsgCounts->new($int);
}

#package MsgHistory;
sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->MsgCounts::addfrom($othr,@args);
	$self->History::addfrom($othr,@args);
}


#package MsgHistory;
sub inc { my ($self,$msg,$dir) = @_;
	#print STDERR "MsgHistory::inc called by ", $self->identify, "\n";
#	my $int = $self->interval($msg->{hdr});
#	my $hist = $self->hist($int);
#	$hist->inc($msg,$dir);
	$self->MsgCounts::inc($msg,$dir);
}

# -------------------------------------
package CallHistory;
use strict;
use vars qw(@ISA);
@ISA = qw(History CallCounts);
# -------------------------------------

sub init {
	my ($self,$top,@args) = @_;
	$self->History::init(@args);
	$self->CallCounts::init($top,@args);
}

#package CallHistory;
sub newcounts {
	my ($self,$int) = @_;
	return CallCounts->new($int);
}

#package CallHistory;
sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->CallCounts::addfrom($othr,@args);
	$self->History::addfrom($othr,@args);
}

#package CallHistory;
sub peg { my ($self,$top,$event,$ts,@args) = @_;
#	my $int = $self->interval($ts);
#	my $hist = $self->hist($int);
#	$hist->peg($top,$event,$ts,@args);
	$self->CallCounts::peg($top,$event,$ts,@args);
}
#package CallHistory;
sub act { my ($self,@args) = @_;
	$self->CallCounts::act(@args);
}
#package CallHistory;
sub cnt { my ($self,@args) = @_;
	$self->CallCounts::cnt(@args);
}
#package CallHistory;
sub sim { my ($self,@args) = @_;
	$self->CallCounts::sim(@args);
}

#package CallHistory;
sub itv { my ($self,@args) = @_;
	$self->CallCounts::itv(@args);
}
#package CallHistory;
sub dur { my ($self,$top,$event,$beg,$end,@args) = @_;
#	my $s = $self->interval($beg);
#	my $e = $self->interval($end);
#	for (my $t = $s; $t <= $e; $t += 300) {
#		my $hist = $self->hist($t);
#		if ($t == $s) {
#			$hist->dur($top,$event,$beg,{tv_sec=>$t+300,tv_usec=>0},@args);
#		} elsif ($t == $e) {
#			$hist->dur($top,$event,{tv_sec=>$t,tv_usec=>0},$end,@args);
#		} else {
#			$hist->dur($top,$event,{tv_sec=>$t,tv_usec=>0},{tv_sec=>$t+300,tv_usec=>0},@args);
#		}
#	}
	$self->CallCounts::dur($top,$event,$beg,$end,@args);
}
#package CallHistory;
sub iat { my ($self,@args) = @_;
	$self->CallCounts::iat(@args);
}

# -------------------------------------
package Plot;
use strict;
# -------------------------------------

my %centerblack = ( -anchor=>'center', -fill=>'black', -justify=>'center' );
my %rightblack = ( -anchor=>'e', -fill=>'black', -justify=>'right' );
my %leftblack = ( -anchor=>'w', -fill=>'black', -justify=>'left' );
my %blackline = ( -arrow=>'none', -capstyle=>'round', -fill=>'black', -joinstyle=>'round', -smooth=>0, -width=>0.1 );
my %greyline = ( -arrow=>'none', -capstyle=>'round', -fill=>'grey', -joinstyle=>'round', -smooth=>0, -width=>0.1 );
my %dashline = ( -arrow=>'none', -capstyle=>'round', -fill=>'grey', -joinstyle=>'round', -smooth=>0, -width=>0.1, -dash=>[2,2] );
my %datacurve = ( -arrow=>'none', -capstyle=>'round', -fill=>'blue', -joinstyle=>'round', -smooth=>1, -width=>0.2 );
my %dataline = ( -arrow=>'none', -capstyle=>'round', -fill=>'black', -joinstyle=>'round', -smooth=>0, -width=>0.3 );
my %redcurve = ( -arrow=>'none', -capstyle=>'round', -fill=>'red', -joinstyle=>'round', -smooth=>1, -width=>0.2, -dash=>[2,2] );

my @dirlabels = ( 'I/C trunks', 'O/G trunks', 'All trunks');

my @evtlabels = (
	'Init',
	'Idle',
	'Setup',
	'Ringing',
	'Call',
	'Suspension',
	'Release',
	'Complettions',
);

#package Plot;
sub new {
	my ($type,$me,$stat,$event,$dir,$w,$X,$Y,@args) = @_;
	my $self;
	if ($self = $$me) {
		$self->display;
		return $self;
	}
	$self = {};
	bless $self,$type;
	$$me = $self;
	$self->{stat} = $stat;
	$self->{event} = $event;
	$self->{dir} = $dir;
	my $title = $self->gettitle;
	my $tw = $self->{tw} = $w->toplevel->Toplevel(
		-title=>$title,
		Name=>'plotWindow',
		-class=>'SS7view',
	);
	$tw->group($w->toplevel);
	#$tw->transient($w->toplevel);
	$tw->iconimage('icon');
	$tw->iconname($title);
	#$tw->resizable(0,0);
	$tw->positionfrom('user');
	$tw->geometry("+$X+$Y");
	$tw->protocol('WM_DELETE_WINDOW', [sub {
		my $me = shift;
		$$me->{tw}->destroy;
		delete $$me->{tw};
		$$me = undef;
	},$me]);
	my $dimx = $self->{dimx} = $self->getdimx;
	my $dimy = $self->{dimy} = $self->getdimy;
	my $c = $self->{canvas} = $tw->Canvas(
		-confine=>1,
		-width=>$dimx,
		-height=>$dimy,
		-xscrollincrement=>0,
		-yscrollincrement=>0,
		-background=>'white',
	)->pack(
		-expand=>1,
		-fill=>'both',
		-side=>'top',
		-anchor=>'nw',
	);
	$c->createText($dimx/2,20,%centerblack,-text=>"\U$title\E",-tags=>['title']);
	my $dirname = $dirlabels[$dir];
	my $id = $stat->identify;
	$c->createText($dimx/2,35,%centerblack,-text=>"$dirname for $id",-tags=>['subtitle']);
	$self->createplot;
	$c->raise('min','sub');
	$c->raise('maj','min');
	$c->raise('end','maj');
	$c->raise('lab','end');
	$c->raise('dat','lab') if $self->{normal};
	$c->raise('smo','lab') if $self->{smooth};
	$c->raise('the','lab') if $self->{theory};
	$c->raise('smo','dat') if $self->{smooth} && $self->{normal};
	$c->raise('the','dat') if $self->{theory} && $self->{normal};
	$c->raise('the','smo') if $self->{smooth} && $self->{theory};
	$c->update;
	$c->CanvasBind('<Configure>',[sub{
			my ($c,$self,$w,$h) = @_;
			my $ow = $self->{dimx};
			my $oh = $self->{dimy};
			$c->scale('all',0,0,$w/$ow,$h/$oh);
			$self->{dimx} = $w;
			$self->{dimy} = $h;
		}, $self, Tk::Ev('w'), Tk::Ev('h')]);
	$c->CanvasBind('<ButtonPress-3>', [sub {
		my ($c,@args) = @_;
		Plot::popup(@args);
	},$self,$me,Tk::Ev('X'),Tk::Ev('Y')]);
	$tw->update;
	$tw->MapWindow;
	return $self;
}

#package Plot;
sub popup {
	my ($self,$me,$X,$Y) = @_;
	my $m = $self->{tw}->Menu(
		-tearoff=>1,
		-title=>'Plot Menu',
	);
	$m->add('command',
		-label=>'Redraw',
		-underline=>0,
		-command=>[\&Plot::redraw,$self,$X,$Y],
	);
	$m->add('command',
		-label=>'Export PS...',
		-underline=>0,
		-command=>[\&Plot::exportps,$self,$X,$Y],
	);
	$m->add('separator');
	$m->add('radiobutton',
		-value=>0,
		-variable=>\$self->{xscale},
		-label=>'X scale linear',
	);
	$m->add('radiobutton',
		-value=>1,
		-variable=>\$self->{xscale},
		-label=>'X scale logarithmic',
	);
	$m->add('separator');
	$m->add('radiobutton',
		-value=>0,
		-variable=>\$self->{yscale},
		-label=>'Y scale linear',
	);
	$m->add('radiobutton',
		-value=>1,
		-variable=>\$self->{yscale},
		-label=>'Y scale logarithmic',
	);
	$m->add('separator');
	$m->add('checkbutton',
		-onvalue=>1,
		-offvalue=>0,
		-variable=>\$self->{normal},
		-label=>'Plot line data',
	);
	$m->add('checkbutton',
		-onvalue=>1,
		-offvalue=>0,
		-variable=>\$self->{smooth},
		-label=>'Plot smooth curve',
	);
	$m->add('checkbutton',
		-onvalue=>1,
		-offvalue=>0,
		-variable=>\$self->{theory},
		-label=>'Plot theoretical',
	);
	$m->add('separator');
	$m->add('command',
		-label=>'Close',
		-underline=>0,
		-command=>[sub {
				my ($self,$me) = @_;
				$self->{tw}->destroy;
				delete $self->{tw};
				$$me = undef;
			},$self,$me,$X,$Y],
	);
	$m->Popup(
		-popanchor=>'nw',
		-popover=>'cursor',
	);
}

#package Plot;
sub exportps {
	my ($self,$X,$Y) = @_;
	my $tw = $self->{tw};
	my $file = $tw->getSaveFile(
		-defaultextension=>'.eps',
		-initialdir=>'./testdata',
		-initialfile=>'plot.eps',
		-title=>'Export PS Dialog',
	);
	return unless $file;
	my $c = $self->{canvas};
	$c->postscript(
		-colormode=>'color',
		-file=>$file,
		#-height=>$self->{dimy},
		#-width=>$self->{dimx},
		#-pageanchor=>'center',
	);
}

#package Plot;
sub redraw {
	my ($self,$X,$Y) = @_;
	my $c = $self->{canvas};
	$c->delete('sub||min||maj||end||lab||dat||smo||the');
	$self->createplot;
	$c->raise('min','sub');
	$c->raise('maj','min');
	$c->raise('end','maj');
	$c->raise('lab','end');
	$c->raise('dat','lab');
	$c->raise('dat','lab') if $self->{normal};
	$c->raise('smo','lab') if $self->{smooth};
	$c->raise('the','lab') if $self->{theory};
	$c->raise('smo','dat') if $self->{smooth} && $self->{normal};
	$c->raise('the','dat') if $self->{theory} && $self->{normal};
	$c->raise('the','smo') if $self->{smooth} && $self->{theory};
	$c->update;
}

#package Plot;
sub gettitle {
	my $self = shift;
	my $prefix = $self->getprefix;
	my $suffix = $self->getsuffix;
	my $evtname = $evtlabels[$self->{event}];
	return "$prefix of $evtname $suffix";
}

#package Plot;
sub getdimx { return 600; }
#package Plot;
sub getdimy { return 400; }

#package Plot;
sub display {
	my $self = shift;
	my $tw = $self->{tw};
	if ($tw->state eq 'iconic') {
		$tw->deiconify;
	} else {
		$tw->UnmapWindow;
	}
	$tw->update;
	$tw->MapWindow;
}

#package Plot;
sub createplot {
	my $self = shift;
	$self->compiledata;
	if ($self->{data}->{num} == 0) {
		$self->{canvas}->createText($self->{dimx}/2,$self->{dimy}/2,%centerblack,-text=>'NO DATA');
		return;
	}
	$self->adjustdata;
	$self->plotdata;
	# throw away the data store
	$self->{data} = {};
	# throw away the plot hash
	$self->{hash} = {};
}

#package Plot;
sub setlogx {
	my $self = shift;
	$self->{xscale} = 1;
	my $c = $self->{canvas};
	my $label = $self->labelx;
	my ($xmin,$xmax) = (50,$self->{dimx}-50);
	my ($ymin,$ymax) = ($self->{dimy}-50,50);
	my $scale = $self->{scalex} = ($xmax-$xmin)/log($self->{maxx}); $scale *= log(10);
	my $powr = log($self->{maxx})/log(10);
	my $tick = int($powr);
	my ($subw,$minw,$majw,$endw) = (1,4,6,6);
	# sub-minor lines
	foreach  my $s ( 3, 4, 6, 7, 8, 9 ) {
		for (my $x = $tick-3 + log($s)/log(10); $x < $powr; $x++) {
			my $X = $xmin + $x * $scale;
			next if $X < $xmin;
			$c->createLine($X,$ymax,$X,$ymin+$subw,%dashline,-tags=>['sub']);
		}
	}
	# minor lines
	foreach my $s ( 2, 5 ) {
		for (my $x = $tick-3 + log($s)/log(10); $x < $powr; $x++) {
			my $X = $xmin + $x * $scale;
			next if $X < $xmin;
			$c->createLine($X,$ymax,$X,$ymin+$minw,%greyline,-tags=>['min']);
		}
	}
	# major lines and labels
	for (my $x = $tick-3; $x < $powr; $x++) {
		my $X = $xmin + $x * $scale;
		next if $X < $xmin;
		$c->createLine($X,$ymax,$X,$ymin+$majw, %blackline,-tags=>['maj']);
		next if $X > $xmax - 60 || $X < $xmin + 50;
		$c->createText($X,$ymin+18, %centerblack,-text=>((10**$x)*$self->{intx}),-tags=>['lab']);
	}
	# min-x line and label
	$c->createLine($xmin,$ymax,$xmin,$ymin+$endw,%blackline,-tags=>['end']);
	$c->createText($xmin,$ymin+18,%centerblack,-text=>1*$self->{intx},-tags=>['lab']);
	# max-x line and label
	$c->createLine($xmax,$ymax,$xmax,$ymin+$endw,%blackline,-tags=>['end']);
	$c->createText($xmax,$ymin+18,%centerblack,-text=>$self->{maxx}*$self->{intx},-tags=>['lab']);
	$c->createText(($xmin+$xmax)/2,$ymin+35,%centerblack,-text=>$label,-tags=>['lab']) if $label;
	$c->createText($xmax,$ymin+35,%rightblack,-text=>"total calls = $self->{data}->{num}",-tags=>['lab']);
	$c->raise('min','sub');
	$c->raise('maj','min');
	$c->raise('end','maj');
	$c->raise('lab','end');
}

#package Plot;
sub setlogy {
	my $self = shift;
	$self->{yscale} = 1;
	my $c = $self->{canvas};
	my $label = $self->labely;
	my ($ymin,$ymax) = ($self->{dimy}-50,50);
	my ($xmin,$xmax) = (50,$self->{dimx}-50);
	my $tot = $self->{toty};
	my $max = $self->{maxy};
	my $scale = $self->{scaley} = ($ymin-$ymax)/log($max); $scale *= log(10);
	my $powr = (log($max)-log($tot))/log(10);
	my $tock = (         -log($tot))/log(10);
	my $tick = int($tock)-1;
	my ($subw,$minw,$majw,$endw) = (1,4,6,6);
	# sub-minor lines
	foreach  my $s ( 3, 4, 6, 7, 8, 9 ) {
		for (my $y = $tick + log($s)/log(10); $y < $powr; $y++) {
			my $Y = $ymin - ($y + log($tot)/log(10)) * $scale;
			next if $Y > $ymin;
			$c->createLine($xmax,$Y,$xmin-$subw,$Y,%dashline,-tags=>['sub']);
		}
	}
	# minor lines
	foreach my $s ( 2, 5 ) {
		for (my $y = $tick + log($s)/log(10); $y < $powr; $y++) {
			my $Y = $ymin - ($y + log($tot)/log(10)) * $scale;
			next if $Y > $ymin;
			$c->createLine($xmax,$Y,$xmin-$minw,$Y,%greyline,-tags=>['min']);
		}
	}
	# major lines and labels
	for (my $y = $tick; $y < $powr; $y++) {
		my $Y = $ymin - ($y + log($tot)/log(10)) * $scale;
		next if $Y > $ymin;
		$c->createLine($xmax,$Y,$xmin-$majw,$Y, %blackline,-tags=>['maj']);
		next if $Y > $ymin - 15 || $Y < $ymax + 15;
		next if $label && ($Y > ($ymin+$ymax)/2-15 && $Y < ($ymin+$ymax)/2+15);
		$c->createText($xmin-$endw-2,$Y,%rightblack,-text=>sprintf('%.2g',10**$y),-tags=>['lab']);
	}
	# min-y line and label
	$c->createLine($xmax,$ymin,$xmin-$endw,$ymin,%blackline,-tags=>['end']);
	$c->createText($xmin-$endw-2,$ymin,%rightblack,-text=>sprintf('%.2g',1/$tot),-tags=>['lab']);
	# max-y line and label
	$c->createLine($xmax,$ymax,$xmin-$endw,$ymax,%blackline,-tags=>['end']);
	$c->createText($xmin-$endw-2,$ymax,%rightblack,-text=>sprintf('%.2g',$max/$tot),-tags=>['lab']);
	$c->createText($xmin-25,($ymin+$ymax)/2,%centerblack,-text=>$label,-tags=>['lab']) if $label;
	$c->raise('min','sub');
	$c->raise('maj','min');
	$c->raise('end','maj');
	$c->raise('lab','end');
}

#package Plot;
sub setlinx {
	my $self = shift;
	$self->{xscale} = 0;
	my $c = $self->{canvas};
	my $label = $self->labelx;
	my ($xmin,$xmax) = (50,$self->{dimx}-50);
	my ($ymin,$ymax) = ($self->{dimy}-50,50);
	my $max = $self->{maxx};
	my $scale = $self->{scalex} = ($xmax-$xmin)/$max;
	my $tick = 10**(int(log($max)/log(10)));
	my ($subw,$minw,$majw,$endw) = (1,4,6,6);
	# sub-minor lines
	foreach my $s ( 1, 2, 3, 4, 6, 7, 8, 9 ) {
		for (my $x = $s*$tick/10; $x < $max; $x += $tick) {
			my $X = $xmin + $x * $scale;
			$c->createLine($X,$ymax,$X,$ymin+$subw,%dashline,-tags=>['sub']);
		}
	}
	# minor lines
	for (my $x = $tick/2; $x < $max; $x += $tick) {
		my $X = $xmin + $x * $scale;
		$c->createLine($X,$ymax,$X,$ymin+$minw,%greyline,-tags=>['min']);
	}
	# major lines and labels
	for (my $x = 0; $x < $max; $x += $tick) {
		my $X = $xmin + $x * $scale;
		next if $X < $xmin;
		$c->createLine($X,$ymax,$X,$ymin+$majw,%blackline,-tags=>['maj']);
		next if $X > $xmax - 60 || $X < $xmin + 20;
		$c->createText($X,$ymin+18,%centerblack,-text=>$x,-tags=>['lab']);
	}
	# min-x line and label
	$c->createLine($xmin,$ymax,$xmin,$ymin+$endw,%blackline,-tags=>['end']);
	$c->createText($xmin,$ymin+18,%centerblack,-text=>0,-tags=>['lab']);
	# max-x line and label
	$c->createLine($xmax,$ymax,$xmax,$ymin+$endw,%blackline,-tags=>['end']);
	$c->createText($xmax,$ymin+18,%centerblack,-text=>sprintf('%.3g', $max),-tags=>['lab']);
	$c->createText(($xmin+$xmax)/2,$ymin+35,%centerblack,-text=>$label,-tags=>['lab']) if $label;
	$c->createText($xmax,$ymin+35,%rightblack,-text=>"total calls = $self->{data}->{num}",-tags=>['lab']);
}

#package Plot;
sub setliny {
	my $self = shift;
	$self->{yscale} = 0;
	my $c = $self->{canvas};
	my $label = $self->labely;
	my ($xmin,$xmax) = (50,$self->{dimx}-50);
	my ($ymin,$ymax) = ($self->{dimy}-50,50);
	my $max = $self->{maxy};
	my $tot = $self->{toty};
	my $scale = $self->{scaley} = ($ymin-$ymax)/$max; $scale *= $tot;
	my $powr = log($max/$tot)/log(10);
	my $tick = 10**(int($powr));
	if (int($powr) == $powr) { $tick /= 10; }
	my ($subw,$minw,$majw,$endw) = (1,4,6,6);
	# sub-minor lines
	foreach my $s ( 1, 2, 3, 4, 6, 7, 8, 9 ) {
		for (my $y = $s*$tick/10; $y < $max/$tot; $y += $tick) {
			my $Y = $ymin - $y * $scale;
			$c->createLine($xmax,$Y,$xmin-$subw,$Y,%dashline,-tags=>['sub']);
		}
	}
	# minor lines
	for (my $y = $tick/2; $y < $max/$tot; $y += $tick) {
		my $Y = $ymin - $y * $scale;
		$c->createLine($xmax,$Y,$xmin-$minw,$Y,%greyline,-tags=>['min']);
	}
	# major lines and labels
	for (my $y = 0; $y < $max/$tot; $y += $tick) {
		my $Y = $ymin - $y * $scale;
		next if $Y > $ymin;
		$c->createLine($xmax,$Y,$xmin-$majw,$Y,%blackline,-tags=>['maj']);
		next if $Y > $ymin - 15 || $Y < $ymax + 15;
		next if $label && ($Y > ($ymin+$ymax)/2-15 && $Y < ($ymin+$ymax)/2+15);
		$c->createText($xmin-$endw-2,$Y,%rightblack,-text=>($y),-tags=>['lab']);
	}
	# min-y line and label
	$c->createLine($xmax,$ymin,$xmin-$endw,$ymin,%blackline,-tags=>['end']);
	$c->createText($xmin-$endw-2,$ymin,%rightblack,-text=>0,-tags=>['lab']);
	# max-y line and label
	$c->createLine($xmax,$ymax,$xmin-$endw,$ymax,%blackline,-tags=>['end']);
	$c->createText($xmin-$endw-2,$ymax,%rightblack,-text=>sprintf('%.3g', $max/$tot),-tags=>['lab']);
	# y-axis label
	$c->createText($xmin-25,($ymin+$ymax)/2,%centerblack,-text=>$label,-tags=>['lab']) if $label;
}

# -------------------------------------
package PlotPdf;
use strict;
use vars qw(@ISA);
@ISA = qw(Plot);
# -------------------------------------

#package PlotPdf;
sub new {
	my ($type,@args) = @_;
	return Plot::new($type,@args);
}

#package PlotPdf;
sub getprefix {
	return 'probability density';
}

#package PlotPdf;
sub labely { return 'p(x)'; }

#package PlotPdf;
sub adjustdata {
	my $self = shift;
	my $max = $self->{data}->{max};
	my $int = 10**(int(log($max)/log(10))-2);
	my ($maxx,$maxy) = (0,0);
	my $h = {};
	foreach my $v (@{$self->{data}->{vals}}) {
		next if $v > $max;
		my $i = $v/$int;
		if (($v = int($i)) < $i) { $v++; }
		if ($v <= 0) { $v = 1; }
		$h->{$v}++;
		if ($v > $maxx) { $maxx = $v; }
		if ($h->{$v} > $maxy) { $maxy = $h->{$v}; }
	}

	for (my $i = 1; $i <= $maxx; $i++) { unless (exists $h->{$i}) { $h->{$i} = 0; } }
	my $t = 0;
	for (my $i = 1; $i <= $maxx; $i++) { $t += $h->{$i}; }
	$self->{intx} = $int;
	$self->{maxx} = $maxx;
	$self->{maxy} = $maxy;
	$self->{hash} = $h;
	$self->{toty} = $t;
	$self->{expv} = $self->{data}->{exv};
}

#package PlotPdf;
sub labely { return 'p(x)'; }

#package PlotPdf;
sub plotdata {
	my $self = shift;
	my $c = $self->{canvas};
	$self->{normal} = 1 unless exists $self->{normal};
	$self->{smooth} = 1 unless exists $self->{smooth};
	$self->{theory} = 1 unless exists $self->{theory};
	$self->{xscale} = 0 unless exists $self->{xscale};
	if ($self->{xscale}) { $self->setlogx; } else { $self->setlinx; }
	$self->{yscale} = 1 unless exists $self->{yscale};
	if ($self->{yscale}) { $self->setlogy; } else { $self->setliny; }
	my ($dimx,$dimy) = ($self->{dimx},$self->{dimy});
	my ($xmin,$xmax) = (50,$dimx-50);
	my ($ymin,$ymax) = ($dimy-50,50);
	my ($scalex,$scaley) = ($self->{scalex},$self->{scaley});
	my $intx = $self->{intx};
	my $tot = $self->{toty};
	my $lambda = 1/$self->{expv};
	$c->createText($xmin,$ymin+35,%leftblack,-text=>"lambda = ".sprintf('%.3g',$lambda),-tags=>['lab']);
	my @coords = ();
	my @theory = ();
	foreach my $k (sort {$a <=> $b} keys %{$self->{hash}}) {
		my $x = $k;
		next if $x <= 0 && $self->{xscale};
		$x = log($x) if $self->{xscale};
		my $X = $xmin + $x * $scalex;
		next if $xmin > $X || $X > $xmax;
		if ($self->{theory}) {
			my $tx = $k*$intx;
			my $ty = $lambda * exp(-$lambda*$tx) * $tot;
			if ($self->{yscale} == 0 || $ty > 0) {
				$ty = log($ty) if $self->{yscale};
				my $TY = $ymin - $ty * $scaley;
				if ($ymax <= $TY && $TY <= $ymin) {
					push @theory, ($X, $TY);
				}
			}
		}
		my $y = $self->{hash}->{$k};
		next if $y <= 0 && $self->{yscale};
		$y = log($y) if $self->{yscale};
		my $Y = $ymin - $y * $scaley;
		next if $ymax > $Y || $Y > $ymin;
		push @coords, ( $X, $Y );
	}
	$c->createLine(@coords, %dataline,  -tags=>['dat']) if $self->{normal};
	$c->createLine(@coords, %datacurve, -tags=>['smo']) if $self->{smooth};
	$c->createLine(@theory, %redcurve,  -tags=>['the']) if $self->{theory};
}

# -------------------------------------
package PlotCdf;
use strict;
use vars qw(@ISA);
@ISA = qw(Plot);
# -------------------------------------

#package PlotCdf;
sub new {
	my ($type,@args) = @_;
	return Plot::new($type,@args);
}

#package PlotCdf;
sub getprefix {
	return 'cummulative distribution';
}

#package PlotCdf;
sub adjustdata {
	my $self = shift;
	my $max = $self->{data}->{max};
	my $int = 10**(int(log($max)/log(10))-2);
	my $maxx = 0;
	my $h = {};
	foreach my $v (@{$self->{data}->{vals}}) {
		next if $v > $max;
		my $i = $v/$int;
		if (($v = int($i)) < $i) { $v++; }
		if ($v <= 0) { $v = 1; }
		$h->{$v}++;
		if ($v > $maxx) { $maxx = $v; }
	}

	for (my $i = 0; $i <= $maxx; $i++) { unless (exists $h->{$i}) { $h->{$i} = 0; } }
	my $t = 0;
	for (my $i = 0; $i <= $maxx; $i++) { $t += $h->{$i}; $h->{$i} = $t; }
	$self->{intx} = $int;
	$self->{maxx} = $maxx;
	$self->{maxy} = $t;
	$self->{hash} = $h;
	$self->{toty} = $t;
	$self->{expv} = $self->{data}->{exv};
}

#package PlotCdf;
sub labely { return 'A(x)'; }

#package PlotCdf;
sub plotdata {
	my $self = shift;
	my $c = $self->{canvas};
	$self->{normal} = 1 unless exists $self->{normal};
	$self->{smooth} = 1 unless exists $self->{smooth};
	$self->{theory} = 1 unless exists $self->{theory};
	$self->{xscale} = 1 unless exists $self->{xscale};
	if ($self->{xscale}) { $self->setlogx; } else { $self->setlinx; }
	$self->{yscale} = 0 unless exists $self->{yscale};
	if ($self->{yscale}) { $self->setlogy; } else { $self->setliny; }
	my ($dimx,$dimy) = ($self->{dimx},$self->{dimy});
	my ($xmin,$xmax) = (50,$dimx-50);
	my ($ymin,$ymax) = ($dimy-50,50);
	my ($scalex,$scaley) = ($self->{scalex},$self->{scaley});
	my $intx = $self->{intx};
	my $tot = $self->{toty};
	my $lambda = 1/$self->{expv};
	$c->createText($xmin,$ymin+35,%leftblack,-text=>"lambda = ".sprintf('%.3g',$lambda),-tags=>['lab']);
	my @coords = ();
	my @theory = ();
	foreach my $k (sort {$a <=> $b} keys %{$self->{hash}}) {
		my $x = $k;
		next if $x <= 0 && $self->{xscale};
		$x = log($x) if $self->{xscale};
		my $X = $xmin + $x * $scalex;
		next if $xmin > $X || $X > $xmax;
		if ($self->{theory}) {
			my $tx = $k*$intx;
			my $ty = (1 - exp(-$lambda*$tx)) * $tot;
			if ($self->{yscale} == 0 || $ty > 0) {
				$ty = log($ty) if $self->{yscale};
				my $TY = $ymin - $ty * $scaley;
				if ($ymax <= $TY && $TY <= $ymin) {
					push @theory, ($X, $TY);
				}
			}
		}
		my $y = $self->{hash}->{$k};
		next if $y <= 0 && $self->{yscale};
		$y = log($y) if $self->{yscale};
		my $Y = $ymin - $y * $scaley;
		next if $ymax > $Y || $Y > $ymin;
		push @coords, ( $X, $Y );
	}
	$c->createLine(@coords, %dataline,  -tags=>['dat']) if $self->{normal};
	$c->createLine(@coords, %datacurve, -tags=>['smo']) if $self->{smooth};
	$c->createLine(@theory, %redcurve,  -tags=>['the']) if $self->{theory};
}

# -------------------------------------
package PlotPhase;
use strict;
use vars qw(@ISA);
@ISA = qw(Plot);
# -------------------------------------

#package PlotPhase;
sub new {
	my ($type,@args) = @_;
	return Plot::new($type,@args);
}

#package PlotPhase;
sub getprefix {
	return 'phase plot';
}

#package PlotPhase;
sub getdimx { return 600; }
#package PlotPhase;
sub getdimy { return 600; }

#package PlotPhase;
sub adjustdata {
	my $self = shift;
	my $max = $self->{data}->{max};
	my $int = 10**(int(log($max)/log(10))-2);

	$self->{intx} = $int;
	$self->{maxx} = $max;
	$self->{maxy} = $max;
	$self->{toty} = 1;
}

#package PlotPhase;
sub labelx { return 'X[j] = (seconds)'; }
sub labely { return 'X[j+1]'; }

#package PlotPhase;
sub plotdata {
	my $self = shift;
	my $c = $self->{canvas};
	$self->{normal} = 1;
	$self->{smooth} = 0;
	$self->{theory} = 0;
	$self->setlinx;
	$self->setliny;
	my ($dimx,$dimy) = ($self->{dimx},$self->{dimy});
	my ($xmin,$xmax) = (50,$dimx-50);
	my ($ymin,$ymax) = ($dimy-50,50);
	my ($scalex,$scaley) = ($self->{scalex},$self->{scaley});
	my $ti = shift @{$self->{data}->{vals}};
	foreach my $tip1 (@{$self->{data}->{vals}}) {
		my $X = $xmin + $ti   * $scalex;
		next if $xmin > $X || $X > $xmax;
		my $Y = $ymin - $tip1 * $scaley;
		next if $ymax > $Y || $Y > $ymin;
		$c->createOval($X-1,$Y-1,$X+1,$Y+1,
			-fill=>'black',
			-outline=>'black',
			-width=>0,
			-tags=>['dat'],
		);
		$ti = $tip1;
	}
}

# -------------------------------------
package PlotHolding;
use strict;
use vars qw(@ISA);
@ISA = qw(Plot);
# -------------------------------------

#package PlotHolding;
sub getsuffix {
	return 'holding times';
}

#package PlotHolding;
sub compiledata {
	my $self = shift;
	my @durs = ();
	my ($e,$n) = (0, 0);
	foreach my $i (@{$self->{stat}->{itvs}->{$self->{dir}}->{$self->{event}}}) {
		my $v = { tv_sec=>($i->{end}->{tv_sec}-$i->{beg}->{tv_sec}), tv_usec=>($i->{end}->{tv_usec}-$i->{beg}->{tv_usec}) };
		while ($v->{tv_usec} < 0) {
			$v->{tv_sec}--;
			$v->{tv_usec} += 1000000;
		}
		my $d = $v->{tv_sec} + $v->{tv_usec}/1000000;
		push @durs, $d;
		$e += $d;
		$n += 1;
	}
	my $num = scalar(@durs);
	my @vals = sort {$a <=> $b} @durs;
	$self->{data}->{vals} = \@durs;
	$self->{data}->{min} = $vals[0];
	my $ignore = 10;
	if ($num > $ignore) {
		$self->{data}->{max} = $vals[-$ignore-1];
		for (my $i = -$ignore; $i < 0; $i++) {
			$e -= $vals[$i];
			$n--;
		}
		$self->{data}->{num} = $num-$ignore;
	} else {
		$self->{data}->{max} = $vals[-1];
		$self->{data}->{num} = $num;
	}
	$self->{data}->{exv} = $e/$n;
}

#package PlotHolding;
sub labelx { return 'x = holding time (seconds)'; }

# -------------------------------------
package PlotArrival;
use strict;
use vars qw(@ISA);
@ISA = qw(Plot);
# -------------------------------------

#package PlotArrival;
sub getsuffix {
	return 'interarrival times';
}

#package PlotArrival;
sub tscmp {
	my ($a,$b) = @_;
	if ($a->{tv_sec} < $b->{tv_sec}) {
		return -1;
	}
	if ($a->{tv_sec} > $b->{tv_sec}) {
		return 1;
	}
	if ($a->{tv_usec} < $b->{tv_usec}) {
		return -1;
	}
	if ($a->{tv_usec} > $b->{tv_usec}) {
		return 1;
	}
	return 0;
}

#package PlotArrival;
sub compiledata {
	my $self = shift;
	my @iats = ();
	my $t;
	my ($e,$n) = (0, 0);
	foreach my $i (sort {tscmp($a->{beg},$b->{beg})} @{$self->{stat}->{itvs}->{$self->{dir}}->{$self->{event}}}) {
		if (defined $t) {
			my $v = { tv_sec=>($i->{beg}->{tv_sec}-$t->{tv_sec}), tv_usec=>($i->{beg}->{tv_usec}-$t->{tv_usec}) };
			while ($v->{tv_usec} < 0) {
				$v->{tv_sec}--;
				$v->{tv_usec} += 1000000;
			}
			my $d = $v->{tv_sec} + $v->{tv_usec}/1000000;
			push @iats, $d;
			$e += $d;
			$n += 1;
		}
		$t = $i->{beg};
	}
	my $num = scalar(@iats);
	my @vals = sort {$a <=> $b} @iats;
	$self->{data}->{vals} = \@iats;
	$self->{data}->{min} = $vals[0];
	my $ignore = 10;
	if ($num > $ignore) {
		$self->{data}->{max} = $vals[-$ignore-1];
		for (my $i = -$ignore; $i < 0; $i++) {
			$e -= $vals[$i];
			$n--;
		}
		$self->{data}->{num} = $num-$ignore;
	} else {
		$self->{data}->{max} = $vals[-1];
		$self->{data}->{num} = $num;
	}
	$self->{data}->{exv} = $e/$n;
}

#package PlotArrival;
sub labelx { return 'x = interarrival time (seconds)'; }

# -------------------------------------
package PlotPdfHolding;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotPdf PlotHolding);
# -------------------------------------

#package PlotPdfHolding;
sub get {
	my ($type,$stat,$event,$dir,@args) = @_;
	my $me = \$stat->{plots}->{pdfh}->{$dir}->{$event};
	return PlotPdf::new($type,$me,$stat,$event,$dir,@args);
}

# -------------------------------------
package PlotCdfHolding;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotCdf PlotHolding);
# -------------------------------------

#package PlotCdfHolding;
sub get {
	my ($type,$stat,$event,$dir,@args) = @_;
	my $me = \$stat->{plots}->{cdfh}->{$dir}->{$event};
	return PlotCdf::new($type,$me,$stat,$event,$dir,@args);
}

# -------------------------------------
package PlotPhaseHolding;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotPhase PlotHolding);
# -------------------------------------

#package PlotPhaseHolding;
sub get {
	my ($type,$stat,$event,$dir,@args) = @_;
	my $me = \$stat->{plots}->{phsh}->{$dir}->{$event};
	return PlotPhase::new($type,$me,$stat,$event,$dir,@args);
}

# -------------------------------------
package PlotPdfArrival;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotPdf PlotArrival);
# -------------------------------------

#package PlotPdfArrival;
sub get {
	my ($type,$stat,$event,$dir,@args) = @_;
	my $me = \$stat->{plots}->{pdfa}->{$dir}->{$event};
	return PlotPdf::new($type,$me,$stat,$event,$dir,@args);
}

# -------------------------------------
package PlotCdfArrival;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotCdf PlotArrival);
# -------------------------------------

#package PlotCdfArrival;
sub get {
	my ($type,$stat,$event,$dir,@args) = @_;
	my $me = \$stat->{plots}->{cdfa}->{$dir}->{$event};
	return PlotCdf::new($type,$me,$stat,$event,$dir,@args);
}

# -------------------------------------
package PlotPhaseArrival;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotPhase PlotArrival);
# -------------------------------------

#package PlotPhaseArrival;
sub get {
	my ($type,$stat,$event,$dir,@args) = @_;
	my $me = \$stat->{plots}->{phsa}->{$dir}->{$event};
	return PlotPhase::new($type,$me,$stat,$event,$dir,@args);
}

# -------------------------------------
package MsgStats;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgHistory);
# -------------------------------------

#package MsgStats;
sub init {
	my ($self,@args) = @_;
	my $self = shift;
	$self->MsgHistory::init(@args);
	$self->{dist} = {};
}

#package MsgStats;
sub addfrom {
	shift->MsgHistory::addfrom(@_);
}

#package MsgStats;
sub stats {
	my ($self,$top,$X,$Y) = @_;
	my $row = 0;
	my ($tw,$w,$bmsg);
	if ($tw = $self->{stats}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
	} else {
		my $title = $self->identify." Message Statistics";
		my $tl = $top->widget->toplevel;
		$tw = $tl->Toplevel(
			-title=>$title,
			Name=>'messageStatistics',
			-class=>'SS7view',
		);
		my $wi = $tw->toplevel->Toplevel(
			-title=>$title,
			Name=>'messageStatistics',
			-class=>'SS7view',
		);
		$wi->geometry("48x48");
		$wi->Label(
			-justify=>'center',
			-text=>'SUs:',
			-width=>6,
		)->grid(-row=>0,-column=>0,-sticky=>'ewns');
		$wi->Entry(
			-justify=>'center',
			-textvariable=>\$self->{incs}->{2}->{sus},
			-width=>6,
		)->grid(-row=>2,-column=>0,-sticky=>'ewns');
		$wi->bind('<Double-Button-1>',[sub{ my ($wi,$self) = @_; $self->{stats}->deiconify; },$self]);
		$tw->iconwindow($wi);
		$tw->group($tl);
		#$tw->transient($tl);
		#$tw->iconimage('icon');
		$tw->iconname($title);
		#$tw->resizable(0,0);
		$tw->positionfrom('user');
		$tw->sizefrom('program');
		#$tw->minsize(600,400);
		$tw->geometry("+$X+$Y");
		$tw->protocol('WM_DELETE_WINDOW', [sub {
			my $self = shift;
			my $tw = $self->{stats};
			delete $self->{stats};
			$tw->destroy;
		},$self]);
		$self->{stats} = $tw;
		my $balloon = $tw->Balloon(-statusbar=>$top->statusbar);
		my $p = $tw->Scrolled('Pane',
			-scrollbars=>'osoe',
			-sticky=>'ewns',
		)->pack(%tframepack);
		my $f = $p->Subwidget('scrolled');
		my $s = $f->Frame->pack(%tframepack);
		my $show2 = ($self->{incs}->{0}->{sus} + $self->{incs}->{1}->{sus} == 0);
		if ($self->{incs}->{2}->{fisus}) {
			my $col = 0;
			$w = $s->Label(%labelright,
				-text=>'FISUs:',
			)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of FISUs sent and received.",
			);
			if ($show2) {
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{2}->{fisus},
				)->grid(-row=>$row++,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of FISUs.",
				);
			} else {
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{0}->{fisus},
				)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of FISUs sent.",
				);
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{1}->{fisus},
				)->grid(-row=>$row++,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of FISUs received.",
				);
			}
		}
		if ($self->{incs}->{2}->{lssus}) {
			my $col = 0;
			$w = $s->Label(%labelright,
				-text=>'1-octet LSSUs:',
			)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of 1-octet LSSUs sent and received.",
			);
			if ($show2) {
				$s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{2}->{lssus},
				)->grid(-row=>$row++,-column=>$col++,-sticky=>'ewns');
				$w = $balloon->attach($w,
					-balloonmsg=>"Number of 1-octet LSSUs.",
				);
			} else {
				$s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{0}->{lssus},
				)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
				$w = $balloon->attach($w,
					-balloonmsg=>"Number of 1-octet LSSUs sent.",
				);
				$s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{1}->{lssus},
				)->grid(-row=>$row++,-column=>$col++,-sticky=>'ewns');
				$w = $balloon->attach($w,
					-balloonmsg=>"Number of 1-octet LSSUs received.",
				);
			}
		}
		if ($self->{incs}->{2}->{lss2s}) {
			my $col = 0;
			$w = $s->Label(%labelright,
				-text=>'2-octet LSSUs:',
			)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of 2-octet LSSUs sent and received.",
			);
			if ($show2) {
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{2}->{lss2s},
				)->grid(-row=>$row++,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of 2-octet LSSUs.",
				);
			} else {
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{0}->{lss2s},
				)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of 2-octet LSSUs sent.",
				);
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{1}->{lss2s},
				)->grid(-row=>$row++,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of 2-octet LSSUs received.",
				);
			}
		}
		if ($self->{incs}->{2}->{msus}) {
			my $col = 0;
			$w = $s->Label(%labelright,
				-text=>'MSUs:',
			)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of MSUs sent and received.",
			);
			if ($show2) {
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{2}->{msus},
				)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of MSUs.",
				);
			} else {
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{0}->{msus},
				)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of MSUs sent.",
				);
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{1}->{msus},
				)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of MSUs received.",
				);
			}
			for (my $mp = 0; $mp < 4; $mp++) {
				my $col2 = $col;
				my $p;
				my $mpp0 = sprintf('%6.2f', ($self->{incs}->{0}->{mp}->{$mp}*100)/(($p=$self->{incs}->{0}->{msus})?$p:1));
				my $mpp1 = sprintf('%6.2f', ($self->{incs}->{1}->{mp}->{$mp}*100)/(($p=$self->{incs}->{1}->{msus})?$p:1));
				my $mpp2 = sprintf('%6.2f', ($self->{incs}->{2}->{mp}->{$mp}*100)/(($p=$self->{incs}->{2}->{msus})?$p:1));
				if ($mp == 3) {
					$bmsg = "\nThis value should be between 0% and 5%.";
				} elsif ($mp == 2) {
					$bmsg = "\nThis value should be between 0% and 20%.";
				} else {
					$bmsg = "\nThis value should be between 30% and 45%.";
				}
				$w = $s->Label(%labelright,
					-text=>"mp($mp)",
				)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of MP($mp) MSUs sent and received.",
				);
				if ($show2) {
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{incs}->{2}->{mp}->{$mp},
						-validatecommand=>[sub{
							my ($mpp,$mp,$self,$row,$col,$old,$proposed,$index,@args) = @_;
							my $p;
							$$mpp = sprintf('%6.2f', ($self->{incs}->{2}->{mp}->{$mp}*100)/(($p=$self->{incs}->{2}->{msus})?$p:1));
							return 1;
						},\$mpp2,$mp,$self],
						-validate=>'all',
					)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of MP($mp) MSUs.",
					);
				} else {
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{incs}->{0}->{mp}->{$mp},
						-validatecommand=>[sub{
							my ($mpp,$mp,$self,$row,$col,$old,$proposed,$index,@args) = @_;
							my $p;
							$$mpp = sprintf('%6.2f', ($self->{incs}->{0}->{mp}->{$mp}*100)/(($p=$self->{incs}->{0}->{msus})?$p:1));
							return 1;
						},\$mpp0,$mp,$self],
						-validate=>'all',
					)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of MP($mp) MSUs sent.",
					);
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{incs}->{1}->{mp}->{$mp},
						-validatecommand=>[sub{
							my ($mpp,$mp,$self,$row,$col,$old,$proposed,$index,@args) = @_;
							my $p;
							$$mpp = sprintf('%6.2f', ($self->{incs}->{1}->{mp}->{$mp}*100)/(($p=$self->{incs}->{1}->{msus})?$p:1));
							return 1;
						},\$mpp1,$mp,$self],
						-validate=>'all',
					)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of MP($mp) MSUs received.",
					);
				}
				$w = $s->Label(%labelright,
					-text=>"mp($mp)%",
				)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Percentage of MP($mp) MSUs sent and received.$bmsg",
				);
				if ($show2) {
					$w = $s->Entry(%entryright,
						-textvariable=>\$mpp2,
					)->grid(-row=>$row++,-column=>$col2++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Percentage of MP($mp) MSUs.$bmsg",
					);
				} else {
					$w = $s->Entry(%entryright,
						-textvariable=>\$mpp0,
					)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Percentage of MP($mp) MSUs sent.$bmsg",
					);
					$w = $s->Entry(%entryright,
						-textvariable=>\$mpp1,
					)->grid(-row=>$row++,-column=>$col2++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Percentage of MP($mp) MSUs received.$bmsg",
					);
				}
			}
		}
		if ($self->{incs}->{2}->{sis}) {
			my %keys = ();
			foreach my $k (keys %{$self->{incs}->{2}->{sis}}) {
				$keys{$k} = 1;
			}
			foreach my $si (sort {$a <=> $b} keys %keys) {
				next unless (exists $mtypes{$si});
				my $col = 0;
				$w = $s->Label(%labelright,
					-text=>"$mtypes{$si}->{0x00}->[0]($si):",
				)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of \U$mtypes{$si}->{0x00}->[0]($si)\E messages sent and received.",
				);
				if ($show2) {
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{incs}->{2}->{sis}->{$si}->{msus},
					)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of \U$mtypes{$si}->{0x00}->[0]($si)\E messages.",
					);
				} else {
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{incs}->{0}->{sis}->{$si}->{msus},
					)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of \U$mtypes{$si}->{0x00}->[0]($si)\E messages sent.",
					);
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{incs}->{1}->{sis}->{$si}->{msus},
					)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of \U$mtypes{$si}->{0x00}->[0]($si)\E messages received.",
					);
				}
				my $mtcount = 0;
				my %keys2 = ();
				foreach my $k (keys %{$self->{incs}->{2}->{sis}->{$si}}) {
					$keys2{$k} = 1;
				}
				foreach my $mt (sort {$a <=> $b} keys %keys2) {
					next if $mt eq 'msus';
					next unless exists $mtypes{$si}->{$mt};
					my $col2 = $col;
					$mtcount += 1;
					$w = $s->Label(%labelright,
						-text=>"$mtypes{$si}->{$mt}->[0]($mt):",
					)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of \U$mtypes{$si}->{$mt}->[0]($mt)\E messages sent and received.",
					);
					if ($show2) {
						$w = $s->Entry(%entryright,
							-textvariable=>\$self->{incs}->{2}->{sis}->{$si}->{$mt}->{msus},
						)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
						$balloon->attach($w,
							-balloonmsg=>"Number of \U$mtypes{$si}->{$mt}->[0]($mt)\E messages.",
						);
					} else {
						$w = $s->Entry(%entryright,
							-textvariable=>\$self->{incs}->{0}->{sis}->{$si}->{$mt}->{msus},
						)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
						$balloon->attach($w,
							-balloonmsg=>"Number of \U$mtypes{$si}->{$mt}->[0]($mt)\E messages sent.",
						);
						$w = $s->Entry(%entryright,
							-textvariable=>\$self->{incs}->{1}->{sis}->{$si}->{$mt}->{msus},
						)->grid(-row=>$row,-column=>$col2++,-sticky=>'ewns');
						$balloon->attach($w,
							-balloonmsg=>"Number of \U$mtypes{$si}->{$mt}->[0]($mt)\E messages received.",
						);
					}
					my $mpcount = 0;
					my %keys3 = ();
					foreach my $k (keys %{$self->{incs}->{2}->{sis}->{$si}->{$mt}}) {
						$keys3{$k} = 1;
					}
					foreach my $mp (sort {$a <=> $b} keys %keys3) {
						next if $mp eq 'msus';
						my $col3 = $col2;
						$mpcount += 1;
						my $color = 'black';
						$bmsg = '';
						unless ($mtypes{$si}->{$mt}->[1] & (1<<$mp)) {
							$color = 'red';
							$bmsg = "\nMP($mp) is not valid for \U$mtypes{$si}->{$mt}->[0]($mt)\E messages.";
						}
						$w = $s->Label(%labelright,
							-fg=>$color,
							-text=>"mp($mp):",
						)->grid(-row=>$row,-column=>$col3++,-sticky=>'ewns');
						$balloon->attach($w,
							-balloonmsg=>"Number of MP($mp) \U$mtypes{$si}->{$mt}->[0]($mt)\E messages sent and received.$bmsg",
						);
						if ($show2) {
							$w = $s->Entry(%entryright,
								-fg=>$color,
								-textvariable=>\$self->{incs}->{2}->{sis}->{$si}->{$mt}->{$mp},
							)->grid(-row=>$row++,-column=>$col3++,-sticky=>'ewns');
							$balloon->attach($w,
								-balloonmsg=>"Number of MP($mp) \U$mtypes{$si}->{$mt}->[0]($mt)\E messages.$bmsg",
							);
						} else {
							$w = $s->Entry(%entryright,
								-fg=>$color,
								-textvariable=>\$self->{incs}->{0}->{sis}->{$si}->{$mt}->{$mp},
							)->grid(-row=>$row,-column=>$col3++,-sticky=>'ewns');
							$balloon->attach($w,
								-balloonmsg=>"Number of MP($mp) \U$mtypes{$si}->{$mt}->[0]($mt)\E messages sent.$bmsg",
							);
							$w = $s->Entry(%entryright,
								-fg=>$color,
								-textvariable=>\$self->{incs}->{1}->{sis}->{$si}->{$mt}->{$mp},
							)->grid(-row=>$row++,-column=>$col3++,-sticky=>'ewns');
							$balloon->attach($w,
								-balloonmsg=>"Number of MP($mp) \U$mtypes{$si}->{$mt}->[0]($mt)\E messages received.$bmsg",
							);
						}
					}
					$row++ unless $mpcount;
				}
				$row++ unless $mtcount;
			}
		}
		$s->update;
		my $x = $s->width+40;
		my $y = $s->height+40;
		$tw->geometry("=$x"."x$y");
	}
	$tw->update;
	$tw->MapWindow;
}

# -------------------------------------
package CallStats;
use strict;
use vars qw(@ISA);
@ISA = qw(CallHistory);
# -------------------------------------

#package CallStats;
sub init {
	my ($self,@args) = @_;
	$self->CallHistory::init(@args);
	$self->{dist} = {};
}

#package CallStats;
sub addfrom {
	shift->CallHistory::addfrom(@_);
}

#package CallStats;
sub plotmenu {
	my ($self,$tw,$event,$dir,$prefix,$label,$X,$Y) = @_;
	my $m = $tw->Menu(
		-tearoff=>1,
		-title=>'Plot Menu',
	);
	$m->add('command',
		-label=>'Holding PDF...',
		-underline=>0,
		-command=>[\&PlotPdfHolding::get,'PlotPdfHolding',$self,$event,$dir,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Holding CDF...',
		-underline=>0,
		-command=>[\&PlotCdfHolding::get,'PlotCdfHolding',$self,$event,$dir,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Holding phase...',
		-underline=>0,
		-command=>[\&PlotPhaseHolding::get,'PlotPhaseHolding',$self,$event,$dir,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Interarrival PDF...',
		-underline=>0,
		-command=>[\&PlotPdfArrival::get,'PlotPdfArrival',$self,$event,$dir,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Interarrival CDF...',
		-underline=>0,
		-command=>[\&PlotCdfArrival::get,'PlotCdfArrival',$self,$event,$dir,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Interarrival phase...',
		-underline=>0,
		-command=>[\&PlotPhaseArrival::get,'PlotPhaseArrival',$self,$event,$dir,$tw,$X,$Y],
	);
	$m->Popup(
		-popanchor=>'nw',
		-popover=>'cursor',
	);
}

#package CallStats;
sub updatedur {
	my ($index,$value,$op,$self,$top,$dur,$event,$dir,$span) = @_;
	if (ref $self eq 'CallCounts') {
		$span = 300;
	} else {
		my ($b,$e) = ($top->{begtime},$top->{endtime});
		$span = $e->{tv_sec}  - $b->{tv_sec}
		     + ($e->{tv_usec} - $b->{tv_usec})/1000000;
	}
	$$dur = sprintf("%12.2f", $self->{durs}->{$dir}->{$event} / $span);
	return $value;
}

#package CallStats;
sub dircstat {
	my ($self,$top,$tw,$balloon,$row,$span,$event,$dir,$prefix,$label) = @_;
	my ($p,$w,$bmsg);
	$bmsg =
"$prefix $label occupancy.\
This is the total duration of time (in Erlangs) that\
circuits have been in the $label state for the study\
period.";
	my $dur = sprintf("%12.2f", $self->{durs}->{$dir}->{$event} / $span);
	$tw->traceVariable(\$self->{durs}->{$dir}->{$event},'w'=>[\&CallStats::updatedur,$self,$top,\$dur,$event,$dir,$span]);
	$tw->traceVariable($top->{endtime},'w'=>[\&CallStats::updatedur,$self,$top,\$dur,$event,$dir,$span]);
	$w = $tw->Entry(%entryright,
		-textvariable=>\$dur,
	)->grid(-row=>$$row,-column=>2,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label periods.
This is the total count of the times that circuits\
transitioned to the $label state in the study\
period.";
	$w = $tw->Entry(%entryright,
		-textvariable=>\$self->{pegs}->{$dir}->{$event},
	)->grid(-row=>$$row,-column=>3,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label duration.\
This is the average duration (in seconds) that circuits\
were in the $label state for the study period.";
	my $del = sprintf("%7.2f", $self->{durs}->{$dir}->{$event}/(($p =
				$self->{pegs}->{$dir}->{$event})?$p:1));
	$tw->traceVariable(\$self->{durs}->{$dir}->{$event},'w'=>[sub{
		my ($index,$value,$op,$self,$event,$dir,$del,$p) = @_;
		$$del = sprintf("%7.2f", $value/(($p = $self->{pegs}->{$dir}->{$event})?$p:1));
		return $value;
	},$self,$event,$dir,\$del]);
	$tw->traceVariable(\$self->{pegs}->{$dir}->{$event},'w'=>[sub{
		my ($index,$value,$op,$self,$event,$dir,$del,$p) = @_;
		$$del = sprintf("%7.2f", $self->{durs}->{$dir}->{$event}/(($p = $value)?$p:1));
		return $value;
	},$self,$event,$dir,\$del]);
	$w = $tw->Entry(%entryright,
		-textvariable=>\$del,
	)->grid(-row=>$$row,-column=>4,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label pegs.\
This is the current number of circuits that are in the\
$label state.";
	$w = $tw->Entry(%entryright,
		-textvariable=>\$self->{sims}->{$dir}->{$event}->{curr},
	)->grid(-row=>$$row,-column=>5,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label high water mark.\
This is the high water mark for the number of circuits\
in the $label state for the study period.";
	$w = $tw->Entry(%entryright,
		-textvariable=>\$self->{sims}->{$dir}->{$event}->{hiwa},
	)->grid(-row=>$$row,-column=>6,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label low water mark.\
This is the low (non-zero) water mark for the number of\
circuits in the $label state for the study period.";
	$w = $tw->Entry(%entryright,
		-textvariable=>\$self->{sims}->{$dir}->{$event}->{lowa},
	)->grid(-row=>$$row,-column=>7,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	my $X = $tw->toplevel->rootx;
	my $Y = $tw->toplevel->rooty;
	$bmsg =
"$prefix $label plots.\
Press this button (using left or right mouse button) to\
generate plots of the study period for the $label state.";
	$w = $tw->Button(
		-command=>[\&CallStats::plotmenu,$self,$tw,$event,$dir,$prefix,$label,$X,$Y],
		-text=>'Plot',
		-pady=>0,
		-pady=>0,
	)->grid(-row=>$$row,-column=>8,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$w->bind('<ButtonPress-3>',[sub {
		my ($w,@args) = @_;
		CallStats::plotmenu(@args);
	},$self,$tw,$event,$dir,$prefix,$label,Tk::Ev('X'),Tk::Ev('Y')]);
	$$row++;
}

#package CallStats;
sub addcstat {
	my ($self,$top,$tw,$balloon,$row,$span,$event,$label) = @_;
	for (my $dir=0; $dir<=2; $dir++) {
		my ($prefix,$w,$bmsg);
		$prefix = ['O/G','I/C','Total']->[$dir];
		$bmsg = "$label counts.";
		if ($dir == 2) {
			$w = $tw->Label(%labelright,
				-text=>$label,
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$balloon->attach($w,-balloonmsg=>$bmsg,);
		}
		$bmsg = "$prefix $label counts.";
		$w = $tw->Label(%labelright,
			-text=>$prefix,
		)->grid(-row=>$$row,-column=>1,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$self->dircstat($top,$tw,$balloon,$row,$span,$event,$dir,$prefix,$label);
	}
}

sub cctstatnew {
	my ($self,$top,$tw,$balloon)= @_;
	my ($w,$bmsg);
	my $span;
	if (ref $self eq 'CallCounts') {
		$span = 300;
	} else {
		my ($b,$e) = ($top->{begtime},$top->{endtime});
		$span = $e->{tv_sec}  - $b->{tv_sec}
		     + ($e->{tv_usec} - $b->{tv_usec})/1000000;
	}
	my $f = $tw->TFrame(%tframestyle,
		-label=>'Circuits:',
	)->pack(%tframepack);
	my $s = $f->Scrolled('Spreadsheet',
		-scrollbars=>'osoe',
		-anchor=>'n',
		-exportselection=>1,
		-invertselected=>1,
		-takefocus=>1,
		-autoclear=>0,
		-cache=>1,
		-drawmode=>'compatible',
		-flashmode=>1,
		-multiline=>1,
		-resizeborders=>'both',
		-selectmode=>'extended',
		-selecttitle=>0,
		-selecttype=>'cell',
		-colseparator=>';',
		-colstretchmode=>'none',
		-colwidth=>12,
		-cols=>9,
		-titlecols=>2,
		#-maxwidth=>?, (800 pixels)
		-rowseparator=>',',
		-rowstretchmode=>'none',
		-rows=>22,
		-titlerows=>1,
		#-maxheight=>?, (600 pixels)
		#-usecommand=>1,
		#-validate=>1,
		#-variable=>$self->{array},
		-wrap=>0,
	)->pack(
		-expand=>1,
		-fill=>'x',
		-side=>'top',
		-anchor=>'n',
	);
	my $m = $s->Subwidget('scrolled');
	$m->bind('<Return>',
		[sub {
			my $w = shift;
			my $index;
			eval { $index = $w->index('active') };
			$index = '' if $@ || $index eq '0,0';
			if ($index) {
				$w->MoveCell(1,0);
				return;
			}
			eval { $index = $w->index('anchor') };
			$index = '' if $@ || $index eq '0,0';
			if ($index) {
				if ($w->selectionIncludes('anchor')) {
					$w->activate($index);
					return;
				}
			}
			$w->MoveCell(1,0);
		}]);
#	$m->configure(
#		-browsecommand=>[\&CallStats::browsecommand,$self,$m],
#		-command=>[\&CallStats::accesscommand,$self],
#		-selectioncommand=>[\&CallStats::selectioncommand,$self],
#		-coltagcommand=>[\&CallStats::coltagcommand,$self],
#		-rowtagcommand=>[\&CallStats::rowtagcommand,$self],
#		-validatecommand=>[\&CallStats::validatecommand,$self],
#	);
	$m->bind('<Shift-KP_Tab>', undef);
	$m->bind('<<LeftTab>>', [sub { shift->MoveCell(0,-1); Tk->break; }]);
	$m->selectionClear('all');
	$m->selectionAnchor(0,0);
	$m->selectionSet(0,0);
	$m->actiave('0,0');
}

sub cctstat {
	my ($self,$top,$tw,$balloon)= @_;
	my ($w,$bmsg);
	my $row = 0;
	my $span;
	if (ref $self eq 'CallCounts') {
		$span = 300;
	} else {
		my ($b,$e) = ($top->{begtime},$top->{endtime});
		$span = $e->{tv_sec}  - $b->{tv_sec}
		     + ($e->{tv_usec} - $b->{tv_usec})/1000000;
	}
	my $f = $tw->TFrame(%tframestyle,
		-label=>'Circuits:',
	)->pack(%tframepack);
	$bmsg =
"Number of defined circuits.  Defined circuits may\
or may not be active.  Unactive defined circuits\
are circuits that were blocked for the entire study\
period.";
	$w = $f->Label(%labelright,
		-text=>'Defined circuits:',
	)->grid(-row=>$row,-column=>0,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg,);
	$w = $f->Entry(%entryright,
		-textvariable=>\$self->{ciccnt},
	)->grid(-row=>$row,-column=>1,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg,);
	$bmsg =
"Number of active circuits.  Active circuits are\
actively carrying calls or have actively carried\
calls at some time within the study period.";
	$w = $f->Label(%labelright,
		-text=>'Active circuits:',
	)->grid(-row=>$row,-column=>2,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg,);
	$w = $f->Entry(%entryright,
		-textvariable=>\$self->{actcnt}->{0},
	)->grid(-row=>$row,-column=>3,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg,);
	if (ref $self eq 'SSP' or ref $self eq 'SP') {
		$bmsg = "Number of active outgoing circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active O/G circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actcnt}->{1},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "Number of active incoming circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active I/C circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actcnt}->{2},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "Number of active bothway circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active 2/W circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actcnt}->{3},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
	}
	if (ref $self eq 'Relation' or ref $self eq 'Linkset') {
		$bmsg = "number of active circuits in the forward direction.";
		$w = $f->Label(%labelright,
			-text=>'Active forward circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actcnt}->{1},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "number of active circuits in the reverse direction.";
		$w = $f->Label(%labelright,
			-text=>'Active reverse circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actcnt}->{2},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "number of active circuits in both directions.";
		$w = $f->Label(%labelright,
			-text=>'Active bothway circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actcnt}->{3},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
	}
	if (ref $self eq 'Network') {
		$bmsg = "number of active one-way circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active one-way circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actcnt}->{1},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "number of active bothway circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active bothway circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actcnt}->{2},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
	}
	if ($self->{actcnt}->{0}) {
		$w = $f = $tw->TFrame(%tframestyle,
			-label=>'Call stats:',
		)->pack(%tframepack);
		$row = 0;
		$bmsg =
"Occupancy: this is the total duration of time (in\
Erlangs) that the indicated circuits have been in\
the corresponding state for the study period.";
		$w = $f->Label(%labelcenter,
			-text=>'Occupancy',
		)->grid(-row=>$row,-column=>2,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg =
"Periods: this is the total count of the times that\
the circuit state transitioned to the indicated state\
during the study period.";
		$w = $f->Label(%labelcenter,
			-text=>'Periods',
		)->grid(-row=>$row,-column=>3,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg =
"Duration: this is the average duration (in seconds)\
that the indicated circuits have been in the\
corresponding state for the study period.";
		$w = $f->Label(%labelcenter,
			-text=>'Duration',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg =
"Pegs: this is the current number of circuits that\
are in the corresponding state.";
		$w = $f->Label(%labelcenter,
			-text=>'Pegs',
		)->grid(-row=>$row,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg =
"High Water Mark: this is the maximum number of\
circuits that were simutaneously in the corresponding\
state for the study period.";
		$w = $f->Label(%labelcenter,
			-text=>'Hi WM',
		)->grid(-row=>$row,-column=>6,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg =
"Low Water Mark: this is the minimum (non-zero)\
number of circuits that were simutaneously in the\
corresponding state for the study period.";
		$w = $f->Label(%labelcenter,
			-text=>'Lo WM',
		)->grid(-row=>$row,-column=>7,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg =
"Plots: press the corresponding button (using the\
left or right mouse button) to generate plots of\
holding and interarrival times for the corresponding\
circuits and state for the study period.";
		$w = $f->Label(%labelcenter,
			-text=>'Distribution',
		)->grid(-row=>$row++,-column=>8,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$self->addcstat($top,$f,$balloon,\$row,$span,::CTS_UNINIT,'Unused');
		$self->addcstat($top,$f,$balloon,\$row,$span,::CTS_IDLE,'Idle');
		$self->addcstat($top,$f,$balloon,\$row,$span,::CTS_WAIT_ACM,'Setup');
		$self->addcstat($top,$f,$balloon,\$row,$span,::CTS_WAIT_ANM,'Alerting');
		$self->addcstat($top,$f,$balloon,\$row,$span,::CTS_ANSWERED,'Connected');
		$self->addcstat($top,$f,$balloon,\$row,$span,::CTS_SUSPENDED,'Suspended');
		$self->addcstat($top,$f,$balloon,\$row,$span,::CTS_WAIT_RLC,'Releasing');
	}
}

#package CallStats;
sub cstat {
	my ($self,$top,$X,$Y) = @_;
	my $row = 0;
	my ($tw,$w,$bmsg);
	if ($tw = $self->{cstat}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
	} else {
		my $title = $self->identify." Call Statistics";
		my $tl = $top->widget->toplevel;
		$tw = $tl->Toplevel(
			-title=>$title,
			Name=>'callStatistics',
			-class=>'SS7view',
		);
		$tw->group($tl);
		#$tw->transient($tl);
		$tw->iconimage('icon');
		$tw->iconname($title);
		#$tw->resizable(0,0);
		$tw->positionfrom('user');
		$tw->geometry("+$X+$Y");
		$tw->protocol('WM_DELETE_WINDOW', [sub {
			my $self = shift;
			foreach my $type (values %{$self->{plots}}) {
				foreach my $dir (values %{$type}) {
					while (my ($l,$m) = each %{$dir}) {
						if ($m) {
							delete $dir->{$l};
							$m->{tw}->destroy;
							delete $m->{tw};
						}
					}
				}
			}
			my $tw = $self->{cstat};
			delete $self->{cstat};
			$tw->destroy;
		},$self]);
		$self->{cstat} = $tw;

		my $balloon = $tw->Balloon(-statusbar=>$top->statusbar);
		$self->cctstat($top,$tw,$balloon);
	}
	$tw->update;
	$tw->MapWindow;
}

# -------------------------------------
package PktStats; use strict;
@PktStats::ISA = qw(PktHistory);
# -------------------------------------

#package PktStats;;
sub destroy {
	my ($self,@args) = @_;
	if (my $tw = $self->{pstat}) { $tw->destroy; }
	delete $self->{pstat};
	$self->PktHistory::destroy(@args);
}
#package PktStats;
sub addfrom {
	shift->SUPER::addfrom(@_);
}

#package PktStats;
sub pstat {
	my ($self,$top,$X,$Y) = @_;
	my $row = 0;
	my ($tw,$w,$bmsg);
	if ($tw = $self->{pstat}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
	} else {
		my $title = $self->identify." Packet Statistics";
		my $tl = $top->widget->toplevel;
		$tw = $tl->Toplevel(
			-title=>$title,
			Name=>'packetStatistics',
			-class=>'SS7view',
		);
		my $wi = $tw->toplevel->Toplevel(
			-title=>$title,
			Name=>'packetStatistics',
			-class=>'SS7view',
		);
		$wi->geometry("48x48");
		$wi->Label(
			-justify=>'center',
			-text=>'Pkts:',
			-width=>6,
		)->grid(-row=>0,-column=>0,-sticky=>'ewns');
		$wi->Entry(
			-justify=>'center',
			-textvariable=>\$self->{hits}->{2}->{pkts},
			-width=>6,
		)->grid(-row=>2,-column=>0,-sticky=>'ewns');
		$wi->bind('<Double-Button-1>',[sub{
			my ($wi,$self) = @_;
			$self->{pstat}->deiconify;
		},$self]);
		$tw->iconwindow($wi);
		$tw->group($tl);
		#$tw->transient($tl);
		#$tw->iconimage('icon');
		$tw->iconname($title);
		#$tw->resizable(0,0);
		$tw->positionfrom('user');
		$tw->sizefrom('program');
		#$tw->minsize(600,400);
		$tw->geometry("+$X+$Y");
		$tw->protocol('WM_DELETE_WINDOW', [sub {
			my $self = shift;
			my $tw = $self->{pstat};
			delete $self->{pstat};
			$tw->destroy;
		},$self]);
		$self->{pstat} = $tw;
		my $balloon = $tw->Balloon(-statusbar=>$top->statusbar);
		my $p = $tw->Scrolled('Pane',
			-scrollbars=>'osoe',
			-sticky=>'ewns',
		)->pack(%tframepack);
		my $f = $p->Subwidget('scrolled');
		my $s = $f->Frame->pack(%tframepack);
		my @show = ();
		$show[0] = $self->{hits}->{0}->{pkts} != 0;
		$show[1] = $self->{hits}->{1}->{pkts} != 0;
		$show[2] = $self->{hits}->{2}->{pkts} != 0 and (($show[0] and $show[1]) or (!$show[0] and !$show[1]));
		if ($self->{hits}->{2}->{pkts}) {
			my $col = 0;
			$w = $s->Label(%labelright,
				-text=>'Packets:',
			)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>'Number of packets sent and received.',
			);
			for (my $i=0;$i<3;$i++) {
				if ($show[$i]) {
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{hits}->{$i}->{pkts},
					)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
					$balloon=>attach($w,
						-balloonmsg=>[
							'Number of packets sent.',
							'Number of packets received.',
							'Number of packets.',
						]->[$i],
					);
				}
			}
			$row++;
		}
		if ($self->{hits}->{2}->{chks}) {
			my $col = 0;
			$w = $s->Label(%labelright,
				-text=>'Chunks:',
			)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>'Number of chunks sent and received.',
			);
			for (my $i=0;$i<3;$i++) {
				if ($show[$i]) {
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{hits}->{$i}->{chks},
					)->grid(-row=>$row,-column=>$col++,-sticky=>'ewns');
					$balloon=>attach($w,
						-balloonmsg=>[
							'Number of chunks sent.',
							'Number of chunks received.',
							'Number of chunks.',
						]->[$i],
					);
				}
			}
			$row++;
		}
		$s->update;
		my $x = $s->width+40;
		my $y = $s->height+40;
		$tw->geometry("=$x"."x$y");
	}
	$tw->update;
	$tw->MapWindow;
}


# -------------------------------------
package MsgCollector; use strict;
@MsgCollector::ISA = qw(MsgStats);
# -------------------------------------

#package MsgCollector;
sub init {
	my ($self,@args) = @_;
	$self->MsgStats::init(@args);
	$self->{msgs} = [];
	$self->{msg} = undef;
	$self->{msgcnt} = 0;
}
#package MsgCollector;
sub pushmsg {
	my ($self,$msg) = @_;
	push @{$self->{msgs}}, $msg;
	$self->{msg} = undef;
	$self->{msgcnt}++;
	$self->updatelist($msg) if $self->{show};
}
#package MsgCollector;
sub popmsg {
	my $self = shift;
	$self->{msgcnt}--;
	return pop @{$self->{msgs}};
}
#package MsgCollector;
sub clearmsgs {
	my $self = shift;
	$self->{msgs} = [];
	$self->{msg} = undef;
	$self->{msgcnt} = 0;
}
#package MsgCollector;
sub msgcnt {
	my $self = shift;
	return scalar(@{$self->{msgs}});
}
#package MsgCollector;
sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->MsgStats::addfrom($othr,@args);
	return unless $othr->isa('MsgCollector');
	splice(@{$self->{msgs}},@{$self->{msgs}},0,@{$othr->{msgs}});
}
#package MsgCollector;
sub msgs {
	my ($self,$top,$X,$Y) = @_;
	my ($tw,$w,$bmsg);
	if ($tw = $self->{show}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
		$tw->MapWindow;
		return;
	}
	my $title = $self->identify." Messages";
	my $tl = $top->widget->toplevel;
	$tw = $tl->Toplevel(
		-title=>$title,
		Name=>'messagesTable',
		-class=>'SS7view',
	);
	my $wi = $tw->toplevel->Toplevel(
		-title=>$title,
		Name=>'messagesTable',
		-class=>'SS7view',
	);
	$tw->iconwindow($wi);
	$wi->geometry('48x48');
	$wi->Label(
		-justify=>'center',
		-text=>'Msgs',
		-width=>6,
	)->grid(-row=>0,-column=>0,-sticky=>'ewns');
	$wi->Entry(
		-justify=>'center',
		-textvariable=>\$self->{msgcnt},
		-width=>6,
	)->grid(-row=>2,-column=>0,-sticky=>'ewns');
	$wi->bind('<Double-Button-1>',[sub{ my ($wi,$self) = @_; $self->{show}->deiconify; },$self]);
	#$tw->group($tl);
	#$tw->transient($tl);
	#$tw->iconimage('icon');
	$tw->iconname($title);
	#$tw->resizeable(0,0);
	#$tw->positionfrom('user');
	#$tw->minsize(600,400);
	#$tw->geometry("+$X+$Y");
	$tw->protocol('WM_DELETE_WINDOW', [sub {
		my $self = shift;
		my $tw = $self->{show};
		delete $self->{show};
		$tw->destroy;
	},$self]);
	$self->{show} = $tw;
	$self->showmsgs($tw,$tl);
	$tw->update;
	$tw->MapWindow;
}
#package MsgCollector;
sub showmsgs {
	my ($self,$tw,$tl) = @_;
	my $s = $tw->Scrolled('Spreadsheet',
		-scrollbars=>'osoe',
		-anchor=>'n',
		-exportselection=>1,
		-invertselected=>1,
		-takefocus=>1,
		-autoclear=>0,
		-cache=>1,
		-drawmode=>'compatible',
		-flashmode=>1,
		-multiline=>1,
		-resizeborders=>'none',
		-selectmode=>'extended',
		-selecttitle=>0,
		-selecttype=>'row',
		-colseparator=>';',
		-colstretchmode=>'none',
		-colwidth=>12,
		-cols=>9,
		-titlecols=>0,
		#-maxwidth=>?, (800 pixels)
		-rowseparator=>',',
		-rowstretchmode=>'none',
		-rows=>(scalar @{$self->{msgs}})+1,
		-titlerows=>1,
		#-maxheight=>?, (600 pixels)
		-usecommand=>1,
		#-validate=>1,
		-command=>[\&MsgCollector::access,$self],
		#-selectioncommand=>[\&MsgCollector::select,$self],
		#-browsecommand=>[\&MsgCollector::browse,$self],
		-variable=>$self->{array},
		-wrap=>0,
		-width=>0,
		-maxwidth=>$tw->screenwidth,
		-state=>'disabled',
	)->pack(
		-expand=>1,
		-fill=>'both',
		-side=>'top',
		-anchor=>'n',
	);
	my $tm = $self->{tm} = $s->Subwidget('scrolled');
	$self->{list} = [];
	@{$self->{list}} = @{$self->{msgs}};
	$tm->colWidth(0,12);
	$tm->colWidth(1,24);
	$tm->colWidth(2,6);
	$tm->colWidth(3,6);
	$tm->colWidth(4,12);
	$tm->colWidth(5,12);
	$tm->colWidth(6,5);
	$tm->colWidth(7,5);
	$tm->colWidth(8,48);
	$tm->bind('<Button-3>',[sub{
				my ($tm,$self,$x,$y) = @_;
				my $row = $tm->index("\@$x,$y",'row');
				my $msg;
				return unless $msg = $self->{list}->[$row-1];
				my $m = $tm->Menu(
					-tearoff=>0,
					-title=>'Message Menu',
				);
				$m->add('command',
					-label=>'Decode',
					-underline=>0,
					-command=>[\&Message::showmsg,$msg,$tl],
				);
				$m->Popup(
					-popanchor=>'nw',
					-popover=>'cursor',
				);
			},$self,Tk::Ev('x'),Tk::Ev('y')]);
	$tm->bind('<Double-Button-1>',[sub{
				my ($tm,$self,$tl,$x,$y) = @_;
				my $row = $tm->index("\@$x,$y",'row');
				my $msg;
				return unless $msg = $self->{list}->[$row-1];
				$msg->showmsg($tl);
			},$self,$tl,Tk::Ev('x'),Tk::Ev('y')]);
	$tm->bind('<Button-1><ButtonRelease-1>',[sub{
				my ($tm,$self,$x,$y,@args) = @_;
				my $index = $tm->index("\@$x,$y");
				my ($row,$col) = split(/,/,$index);
				if ($row == 0) {
					$tm->selectionClear('all');
					if (abs($self->{tmcol}) == $col) {
						$self->{tmcol} = -$self->{tmcol};
					} else {
						$self->{tmcol} = $col;
					}
					$self->colchange;
					Tk->break;
				} else {
					$tm->activate("\@$x,$y");
					if ($row > 0) {
						if ($self->{tmrow} != $row) {
							$self->{tmrow} = $row;
							$self->rowchange;
						} else {
							$tm->selectionClear('all');
							$self->{tmrow} = 0;
						}
					}
				}
			},$self,Tk::Ev('x'),Tk::Ev('y')]);
	$tm->bind('<Shift-4>',['xview','scroll',-1,'units']);
	$tm->bind('<Shift-5>',['xview','scroll',1,'units']);
	$tm->bind('<Button-6>',['xview','scroll',-1,'units']);
	$tm->bind('<Button-7>',['xview','scroll',1,'units']);
	$tm->bind('<4>',['yview','scroll',-1,'pages']);
	$tm->bind('<5>',['yview','scroll',1,'pages']);
	$tm->bind('<Up><Down>',[sub{
			my ($tm,$self) = @_;
			my $row;
			eval { $row = join(':',$tm->curselection) };
			return if ($@);
			$row=~s/,.*$//;
			if ($row > 0 and $self->{tmrow} != $row) {
				$self->{tmrow} = $row;
				$self->rowchange;
			}
		},$self]);
	#$tm->selectionSet('1,0');
	#$tm->selectionAnchor('0,0');
	$self->{tmrow} = 1;
	$self->{tmcol} = 0;
	$tw->Adjuster(
		-side=>'top',
		-widget=>$s,
		-restore=>0,
	)->pack(
		-expand=>0,
		-fill=>'x',
		-side=>'top',
		-anchor=>'n',
	);
	$self->{tree}->{0} = 'minus';
	$self->{tree}->{1} = 'minus';
	$self->{tree}->{2} = 'minus';
	$self->{tree}->{3} = 'minus';
	$self->{tree}->{4} = 'minus';
	$s = $tw->Scrolled('HList',
		-scrollbars=>'osoe',
		-columns=>4,
		#-browsecmd=>,
		#-command=>,
		-background=>'white',
		-drawbranch=>1,
		-header=>0,
		-itemtype=>'text',
		-indent=>18,
		-indicator=>1,
		-indicatorcmd=>[sub{
			my ($self,$entry,$action) = @_;
			my $hl = $self->{hl};
			if ($action eq '<Activate>') {
				delete $hl->{armed};
				if (exists $self->{tree}->{$entry}) {
					if ($self->{tree}->{$entry} eq 'plus') {
						foreach my $e ($hl->info('children',$entry)) {
							$hl->show('entry',$e);
						}
						$hl->indicator('configure', $entry, -image=>'minus',);
						$self->{tree}->{$entry} = 'minus';
					} else {
						foreach my $e ($hl->info('children',$entry)) {
							$hl->hide('entry',$e);
						}
						$hl->indicator('configure', $entry, -image=>'plus',);
						$self->{tree}->{$entry} = 'plus';
					}
				}
			} elsif ($action eq '<Arm>') {
				$hl->{armed} = $entry;
			} elsif ($action eq '<Disarm>') {
				delete $hl->{armed};
			}
		},$self],
		-selectmode=>'browse',
		-separator=>'.',
		#-width=>,
		-exportselection=>1,
	)->pack(
		-expand=>1,
		-fill=>'both',
		-side=>'top',
		-anchor=>'n',
	);
	my $hl = $self->{hl} = $s->Subwidget('scrolled');
	#$hl->header('create', 0, -itemtype=>'text',-text=>'Field',);
	#$hl->header('create', 1, -itemtype=>'text',-text=>'Bits',);
	#$hl->header('create', 2, -itemtype=>'text',-text=>'Value',);
	#$hl->header('create', 3, -itemtype=>'text',-text=>'Description',);
	my $ro = $self->{ro} = $tw->ROText(
		-background=>'white',
		-wrap=>'word',
		-height=>8,
	)->pack(
		-expand=>0,
		-fill=>'both',
		-side=>'top',
		-anchor=>'s',
	);
	$ro->insert('0.0','Message bytes...');
	my $font = $ro->cget('-font');
	$hl->configure(-font=>$font);
	$self->{fldstyle} = $hl->ItemStyle('text', -refwindow=>$hl, -pady=>0, -background=>'white', -font=>$font, -justify=>'right'); #, -anchor=>'e');
	$self->{bitstyle} = $hl->ItemStyle('text', -refwindow=>$hl, -pady=>0, -background=>'white', -font=>$font, -justify=>'left'); #, -anchor=>'e');
	$self->{valstyle} = $hl->ItemStyle('text', -refwindow=>$hl, -pady=>0, -background=>'white', -font=>$font, -justify=>'right'); #, -anchor=>'e');
	$self->{desstyle} = $hl->ItemStyle('text', -refwindow=>$hl, -pady=>0, -background=>'white', -font=>$font, -justify=>'left'); #, -anchor=>'w');
	#$self->rowchange;
}
#package MsgCollector;
sub updatelist {
	my ($self,$msg) = @_;
	my $col = $self->{tmcol};
	my $msgs = $self->{msgcnt};
	my $row = $msgs;
	my $list = $self->{list};
	if ($msgs == 1) {
		push @{$self->{list}}, $msg;
	} elsif ($col == 0) {
		push @{$self->{list}}, $msg;
	} else {
		$self->colchange;
		return;
		my $func;
		if (abs($col) == 1) {
			$func = ($col > 0) ? \&MsgCollector::cmpts : \&MsgCollector::pmcts;
		} elsif (abs($col) == 2) {
			$func = ($col > 0) ? \&MsgCollector::cmpli : \&MsgCollector::pmcli;
		} elsif (abs($col) == 3) {
			$func = ($col > 0) ? \&MsgCollector::cmpsi : \&MsgCollector::pmcsi;
		} elsif (abs($col) == 4) {
			$func = ($col > 0) ? \&MsgCollector::cmpopc : \&MsgCollector::pmcopc;
		} elsif (abs($col) == 5) {
			$func = ($col > 0) ? \&MsgCollector::cmpdpc : \&MsgCollector::pmcdpc;
		} elsif (abs($col) == 6) {
			$func = ($col > 0) ? \&MsgCollector::cmpsls : \&MsgCollector::pmcsls;
		} elsif (abs($col) == 7) {
			$func = ($col > 0) ? \&MsgCollector::cmpmt : \&MsgCollector::pmcmt;
		} elsif (abs($col) == 8) {
			$func = ($col > 0) ? \&MsgCollector::cmpcic : \&MsgCollector::pmccic;
		}
		my ($f,$l,$r,$t) = (0,$msgs-2,0);
		while (($t = int(($f+$l)/2)) != $f && $t != $l) {
			#print STDERR "D: f=$f, t=$t, l=$l\n";
			$r = $func->($msg,$list->[$t]);
			if ($r > 0) { $l = $t; } else { $f = $t; }
		}
		   if ($func->($msg,$list->[$f]) < 0) { $row = $f; }
		elsif ($func->($msg,$list->[$t]) >= 0) { $row = $l+1; }
		elsif ($func->($msg,$list->[$t]) < 0) { $row = $t; }
		elsif ($func->($msg,$list->[$t]) >= 0) { $row = $t+1; }
		else                                  { $row = $f+1; }
		splice(@{$self->{list}},$row,0,$msg);
		$row += 1;
	}
	my $tm = $self->{tm};
	$tm->configure(-rows=>$msgs+1);
	unless ($tm->curselection) {
		$tm->see("$row,0");
	}
	$tm->clearCache("$row,0",'end');
}
#package MsgCollector;
sub rowchange {
	my ($self,$msg) = @_;
	my $row = $self->{tmrow};
	return unless ($row > 0) and ($msg = $self->{list}->[$row-1]);
	$msg->rotext($self->{ro});
	$msg->hlist($self->{hl});
}
#package MsgCollector;
sub cmpts {
	my ($l,$m) = @_;
	return -1 if $l->{hdr}->{tv_sec} < $m->{hdr}->{tv_sec};
	return  1 if $l->{hdr}->{tv_sec} > $m->{hdr}->{tv_sec};
	return -1 if $l->{hdr}->{tv_usec} < $m->{hdr}->{tv_usec};
	return  1 if $l->{hdr}->{tv_usec} > $m->{hdr}->{tv_usec};
	return  0;
}
sub pmcts { return -&MsgCollector::cmpli(@_); }
sub cmpli {
	my ($l,$m) = @_;
	return -1 if $l->{li} < 3 && $l->{li} < $m->{li};
	return  1 if $m->{li} < 3 && $m->{li} < $l->{li};
	return  0;
}
sub pmcli { return -&MsgCollector::cmpli(@_); }
sub cmpsi {
	my ($l,$m) = @_;
	return -1 if $l->{li} < 3 && $l->{li} < $m->{li};
	return  1 if $m->{li} < 3 && $m->{li} < $l->{li};
	return  0 if $l->{li} < 3 || $m->{li} < 3;
	return $l->{si} <=> $m->{si};
}
sub pmcsi { return -&MsgCollector::cmpsi(@_); }
sub cmpopc {
	my ($l,$m) = @_;
	return -1 if $l->{li} < 3 && $l->{li} < $m->{li};
	return  1 if $m->{li} < 3 && $m->{li} < $l->{li};
	return  0 if $l->{li} < 3 || $m->{li} < 3;
	return -1 if $l->{si} < $m->{si};
	return  1 if $l->{si} > $m->{si};
	return $l->{opc} <=> $m->{opc};
}
sub pmcopc { return -&MsgCollector::cmpopc(@_); }
sub cmpdpc {
	my ($l,$m) = @_;
	return -1 if $l->{li} < 3 && $l->{li} < $m->{li};
	return  1 if $m->{li} < 3 && $m->{li} < $l->{li};
	return  0 if $l->{li} < 3 || $m->{li} < 3;
	return -1 if $l->{si} < $m->{si};
	return  1 if $l->{si} > $m->{si};
	return $l->{dpc} <=> $m->{dpc};
}
sub pmcdpc { return -&MsgCollector::cmpdpc(@_); }
sub cmpsls {
	my ($l,$m) = @_;
	return -1 if $l->{li} < 3 && $l->{li} < $m->{li};
	return  1 if $m->{li} < 3 && $m->{li} < $l->{li};
	return  0 if $l->{li} < 3 || $m->{li} < 3;
	return -1 if $l->{si} < $m->{si};
	return  1 if $l->{si} > $m->{si};
	return $l->{sls} <=> $m->{sls};
}
sub pmcsls { return -&MsgCollector::cmpsls(@_); }
sub cmpmt {
	my ($l,$m) = @_;
	return -1 if $l->{li} < 3 && $l->{li} < $m->{li};
	return  1 if $m->{li} < 3 && $m->{li} < $l->{li};
	return  0 if $l->{li} < 3 || $m->{li} < 3;
	return -1 if $l->{si} < $m->{si};
	return  1 if $l->{si} > $m->{si};
	return $l->{mt} <=> $m->{mt};
}
sub pmcmt { return -&MsgCollector::cmpmt(@_); }
sub cmpcic {
	my ($l,$m) = @_;
	return -1 if $l->{li} < 3 && $l->{li} < $m->{li};
	return  1 if $m->{li} < 3 && $m->{li} < $l->{li};
	return  0 if $l->{li} < 3 || $m->{li} < 3;
	return -1 if $l->{si} < $m->{si};
	return  1 if $l->{si} > $m->{si};
	return $l->{cic} <=> $m->{cic};
}
sub pmccic { return -&MsgCollector::cmpcic(@_); }
#package MsgCollector;
sub colchange {
	my $self = shift;
	my $col = $self->{tmcol};
	if ($col == 0) {
		@{$self->{list}} = @{$self->{msgs}};
	} elsif (abs($col) == 1) {
		if ($col > 0) {
			@{$self->{list}} = sort
#			MsgCollector::cmpts
			{
				return -1 if $a->{hdr}->{tv_sec} < $b->{hdr}->{tv_sec};
				return  1 if $a->{hdr}->{tv_sec} > $b->{hdr}->{tv_sec};
				return -1 if $a->{hdr}->{tv_usec} < $b->{hdr}->{tv_usec};
				return  1 if $a->{hdr}->{tv_usec} > $b->{hdr}->{tv_usec};
				return  0;
			}
			@{$self->{msgs}};
		} else {
			@{$self->{list}} = sort
#			MsgCollector::pmcts
			{
				return  1 if $a->{hdr}->{tv_sec} < $b->{hdr}->{tv_sec};
				return -1 if $a->{hdr}->{tv_sec} > $b->{hdr}->{tv_sec};
				return  1 if $a->{hdr}->{tv_usec} < $b->{hdr}->{tv_usec};
				return -1 if $a->{hdr}->{tv_usec} > $b->{hdr}->{tv_usec};
				return  0;
			}
			@{$self->{msgs}};
		}
	} elsif (abs($col) == 2) {
		if ($col > 0) {
			@{$self->{list}} = sort
#			MsgCollector::cmpli
			{
				return -1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return  1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0;
			}
			@{$self->{msgs}};
		} else {
			@{$self->{list}} = sort
#			MsgCollector::pmcli
			{
				return  1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return -1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0;
			}
			@{$self->{msgs}};
		}
	} elsif (abs($col) == 3) {
		if ($col > 0) {
			@{$self->{list}} = sort
#			MsgCollector::cmpsi
			{
				return -1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return  1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return $a->{si} <=> $b->{si};
			}
			@{$self->{msgs}};
		} else {
			@{$self->{list}} = sort
#			MsgCollector::pmcsi
			{
				return  1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return -1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return 0-($a->{si} <=> $b->{si});
			}
			@{$self->{msgs}};
		}
	} elsif (abs($col) == 4) {
		if ($col > 0) {
			@{$self->{list}} = sort
#			MsgCollector::cmpopc
			{
				return -1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return  1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return -1 if $a->{si} < $b->{si};
				return  1 if $a->{si} > $b->{si};
				return $a->{opc} <=> $b->{opc};
			}
			@{$self->{msgs}};
		} else {
			@{$self->{list}} = sort
#			MsgCollector::pmcopc
			{
				return  1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return -1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return  1 if $a->{si} < $b->{si};
				return -1 if $a->{si} > $b->{si};
				return 0-($a->{opc} <=> $b->{opc});
			}
			@{$self->{msgs}};
		}
	} elsif (abs($col) == 5) {
		if ($col > 0) {
			@{$self->{list}} = sort
#			MsgCollector::cmpdpc
			{
				return -1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return  1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return -1 if $a->{si} < $b->{si};
				return  1 if $a->{si} > $b->{si};
				return $a->{dpc} <=> $b->{dpc};
			}
			@{$self->{msgs}};
		} else {
			@{$self->{list}} = sort
#			MsgCollector::pmcdpc
			{
				return  1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return -1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return  1 if $a->{si} < $b->{si};
				return -1 if $a->{si} > $b->{si};
				return 0-($a->{dpc} <=> $b->{dpc});
			}
			@{$self->{msgs}};
		}
	} elsif (abs($col) == 6) {
		if ($col > 0) {
			@{$self->{list}} = sort
#			MsgCollector::cmpsls
			{
				return -1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return  1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return -1 if $a->{si} < $b->{si};
				return  1 if $a->{si} > $b->{si};
				return $a->{sls} <=> $b->{sls};
			}
			@{$self->{msgs}};
		} else {
			@{$self->{list}} = sort
#			MsgCollector::pmcsls
			{
				return  1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return -1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return  1 if $a->{si} < $b->{si};
				return -1 if $a->{si} > $b->{si};
				return 0-($a->{sls} <=> $b->{sls});
			}
			@{$self->{msgs}};
		}
	} elsif (abs($col) == 7) {
		if ($col > 0) {
			@{$self->{list}} = sort
#			MsgCollector::cmpmt
			{
				return -1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return  1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return -1 if $a->{si} < $b->{si};
				return  1 if $a->{si} > $b->{si};
				return $a->{mt} <=> $b->{mt};
			}
			@{$self->{msgs}};
		} else {
			@{$self->{list}} = sort
#			MsgCollector::pmcmt
			{
				return  1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return -1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return  1 if $a->{si} < $b->{si};
				return -1 if $a->{si} > $b->{si};
				return 0-($a->{mt} <=> $b->{mt});
			}
			@{$self->{msgs}};
		}
	} elsif (abs($col) == 8) {
		if ($col > 0) {
			@{$self->{list}} = sort
#			MsgCollector::cmpcic
			{
				return -1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return  1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return -1 if $a->{si} < $b->{si};
				return  1 if $a->{si} > $b->{si};
				return $a->{cic} <=> $b->{cic};
			}
			@{$self->{msgs}};
		} else {
			@{$self->{list}} = sort
#			MsgCollector::pmccic
			{
				return  1 if $a->{li} < 3 && $a->{li} < $b->{li};
				return -1 if $b->{li} < 3 && $b->{li} < $a->{li};
				return  0 if $a->{li} < 3 || $b->{li} < 3;
				return  1 if $a->{si} < $b->{si};
				return -1 if $a->{si} > $b->{si};
				return 0-($a->{cic} <=> $b->{cic});
			}
			@{$self->{msgs}};
		}
	}
	my $tm = $self->{tm};
	$tm->configure(-rows=>scalar(@{$self->{list}})+1);
	$tm->clearCache;
}
#package MsgCollector;
sub access {
	my ($self,$set,$row,$col,$value) = @_;
	return $self->header($set,$row,$col,$value) if $row == 0;
	return undef if $row < 0;
	my $msg;
	unless ($msg = $self->{list}->[$row-1]) {
		return undef;
	}
	if ($col == 0) {
		return $msg->{msgnum};
	}
	if ($col == 1) {
		return sprintf('%.6f', $msg->{hdr}->{tv_sec} + $msg->{hdr}->{tv_usec}/1000000);
	}
	if ($col == 2) {
		return 'FISU' if $msg->{li} == 0;
		return 'LSSU' if $msg->{li} == 1;
		return 'LSS2' if $msg->{li} == 2;
		return 'MSU';
	}
	return '' if $msg->{li} < 3;
	if ($col == 3) {
		return '' unless exists $mtypes{$msg->{si}};
		return "\U$mtypes{$msg->{si}}->{0x00}->[0]\E";
	}
	return SP::pcstring($msg->{opc}) if $col == 4;
	return SP::pcstring($msg->{dpc}) if $col == 5;
	return $msg->{sls} if $col == 6;
	if ($col == 7) {
		return '' unless exists $mtypes{$msg->{si}}->{$msg->{mt}};
		return "\U$mtypes{$msg->{si}}->{$msg->{mt}}->[0]\E";
	}
	if ($col == 8) {
		if ($msg->{si} == 5) {
			return "CIC = $msg->{cic}";
		}
	}
	return '';
}
#package MsgCollector;
sub header {
	my ($self,$set,$row,$col,$value) = @_;
	return 'Msg No.' if $col == 0;
	return 'Time'    if $col == 1;
	return 'Type'    if $col == 2;
	return 'Service' if $col == 3;
	return 'OPC'     if $col == 4;
	return 'DPC'	 if $col == 5;
	return 'SLS'	 if $col == 6;
	return 'MT'	 if $col == 7;
	return 'Params'  if $col == 8;
	return '';
}

# -------------------------------------
package CallCollector;
use strict;
use vars qw(@ISA);
@ISA = qw(CallStats);
# -------------------------------------

sub init {
	my ($self,@args) = @_;
	$self->CallStats::init(@args);
	$self->{calls} = [];
	$self->{call} = undef;
}

sub pushcall {
	my ($self,$top,$call) = @_;
	push @{$self->{calls}}, $call;
	$self->{call} = undef;
}

sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->CallStats::addfrom($othr,@args);
	return unless $othr->isa('CallCollector');
	splice(@{$self->{calls}},@{$self->{calls}},0,@{$othr->{calls}});
}

sub calls {
	my ($self,$top,$X,$Y) = @_;
	my ($tw,$w,$bmsg);
	if ($tw = $self->{show}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
		$tw->MapWindow;
		return;
	}
	my $title = $self->identify." Calls";
	my $tl = $top->widget->toplevel;
	$tw = $tl->Toplevel(
		-title=>$title,
		Name=>'callTable',
		-class=>'SS7view',
	);
	$tw->group($tl);
	#$tw->transient($tl);
	$tw->iconimage('icon');
	$tw->iconname($title);
	#$tw->resizeable(0,0);
	$tw->positionfrom('user');
	#$tw->minsize(600,400);
	$tw->geometry("+$X+$Y");
	$tw->protocol('WM_DELETE_WINDOW', [sub {
		my $self = shift;
		my $tw = $self->{show};
		delete $self->{show};
		$tw->destroy;
	},$self]);
	$self->{show} = $tw;
	$self->showcalls($tw);
	$tw->update;
	$tw->MapWindow;
}

sub showcalls {
	my ($self,$tw) = @_;
}

# -------------------------------------
package DualCollector;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgCollector CallCollector);
# -------------------------------------
#package DualCollector;
sub init {
	my ($self,@args) = @_;
	$self->MsgCollector::init(@args);
	$self->CallCollector::init(@args);
}
#package PktCollector;
sub destroy {
	my ($self,$top,@args) = @_;
	delete $self->{pkts};
	delete $self->{pkt};
	delete $self->{pktcnt};
	$self->PktStats::destroy($top,@args);
}
#package PktCollector;
sub pushpkt {
	my ($self,$pkt) = @_;
	push @{$self->{pkts}}, $pkt;
	$self->{pkt} = undef;
	$self->{pktcnt}++;
	$self->updatelist($pkt) if $self->{show};
}

# -------------------------------------
package MsgBuffer;
use strict;
# -------------------------------------
#package MsgBuffer;
sub init {
	my ($self,@args) = @_;
	$self->{msgbuf} = [];
}
#package MsgBuffer;
sub pushbuf {
	my ($self,$msg) = @_;
	return push @{$self->{msgbuf}}, $msg;
}
#package PktCollector;
sub clearpkts {
	my $self = shift;
	$self->{pkts} = [];
	$self->{pkt} = undef;
	$self->{pktcnt} = 0;
}
#package PktCollector;
sub pktcnt {
	my $self = shift;
	return scalar(@{$self->{pkts}});
}
#package PktCollector;
sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->PktStats::addfrom($othr,@args);
	return unless $othr->isa('PktCollector');
	splice(@{$self->{pkts}},@{$self->{pkts}},0,@{$othr->{pkts}});
}
#package PktCollector;
sub pkts {
	my ($self,$top,$X,$Y) = @_;
	my ($tw,$wi,$bmsg);
	if ($tw = $self->{list}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconnify;
		} else {
			$tw->UnmapWindow;
		}
		$tw->MapWindow;
		return;
	}
	my $title = $self->identify." Packets";
	my $tl = $top->widget->toplevel;
	$tw = $tl->Toplevel(
		-title=>$title,
		Name=>'packetsTable',
		-class=>'SS7view',
	);
	$tw->iconwindow($wi);
	$wi->geometry('48x48');
	$wi->Label(%labelcenter, -width=>6, -text=>'Pkts',
	)->grid(-row=>0,-column=>0);
	$wi->Entry(%entrycenter, -width=>6, -textvariable=>\$self->{pktcnt},
	)->grid(-row=>2,-column=>0);
	$wi->bind('<Double-Button-1>',[sub{
		my ($wi,$self) = @_;
		$self->{list}->deiconify;
	},$self]);
	#$tw->group($tl);
	#$tw->transient($tl);
	#$tw->iconimage('icon');
	$tw->iconname($title);
	#$tw->resizeable(0,0);
	#$tw->positionfrom('user');
	#$tw->minsize(600,400);
	#$tw->geometry("+$X+$Y");
	$tw->protocol('WM_DELETE_WINDOW', [sub {
		my $self = shift;
		my $tw = $self->{list};
		delete $self->{list};
		$tw->destroy;
	},$self]);
	$self->{list} = $tw;
	$self->showpkts($tw,$tl);
	$tw->update;
	$tw->MapWindow;
}
#package PktCollector;
sub showpkts {
	my ($self,$tw,$tl) = @_;

	# FIXME: write rest of packet lister and disector.
}

# -------------------------------------
package PairCollector; use strict;
@PairCollector::ISA = qw(PktCollector MsgCollector);
# -------------------------------------
#package PairCollector;
sub init {
	my ($self,@args) = @_;
	$self->PktCollector::init(@args);
	$self->MsgCollector::init(@args);
}
#package PairCollector;
sub destroy {
	my ($self,@args) = @_;
	$self->MsgCollector::destroy(@args);
	$self->PktCollector::destroy(@args);
}
#package PairCollector;
sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->PktCollector::addfrom($othr,@args);
	$self->MsgCollector::addfrom($othr,@args);
}

# -------------------------------------
package DualCollector; use strict;
@DualCollector::ISA = qw(MsgCollector CallCollector);
# -------------------------------------
#package DualCollector;
sub init {
	my ($self,@args) = @_;
	$self->MsgCollector::init(@args);
	$self->CallCollector::init(@args);
}
#package DualCollector;
sub destroy {
	my ($self,@args) = @_;
	$self->CallCollector::destroy(@args);
	$self->MsgCollector::destroy(@args);
}
#package DualCollector;
sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->MsgCollector::addfrom($othr,@args);
	$self->CallCollector::addfrom($othr,@args);
}

# -------------------------------------
package TripleCollector; use strict;
@TripleCollector::ISA = qw(PktCollector MsgCollector CallCollector);
# -------------------------------------
#package TripleCollector;
sub init {
	my ($self,@args) = @_;
	$self->PktCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->CallCollector::init(@args);
}
#package TripleCollector;
sub destroy {
	my ($self,@args) = @_;
	$self->CallCollector::destroy(@args);
	$self->MsgCollector::destroy(@args);
	$self->PktCollector::destroy(@args);
}
#package TripleCollector;
sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->PktCollector::addfrom($othr,@args);
	$self->MsgCollector::addfrom($othr,@args);
	$self->CallCollector::addfrom($othr,@args);
}

# -------------------------------------
package MsgBuffer; use strict;
# -------------------------------------
#package MsgBuffer;
sub init {
	my ($self,@args) = @_;
	$self->{msgbuf} = [];
}
#package MsgBuffer;
sub destroy {
	my ($self,@args) = @_;
	delete $self->{msgbuf};
}
#package MsgBuffer;
sub pushbufmsg {
	my ($self,$msg) = @_;
	return push @{$self->{msgbuf}}, $msg;
}
#package MsgBuffer;
sub shiftbufmsg {
	my $self = shift;
	return shift @{$self->{msgbuf}};
}
#package MsgBuffer;
sub peekbuf {
	my $self = shift;
	return $self->{msgbuf}->[0];
}

# -------------------------------------
package Properties;
use strict;
# -------------------------------------

#package Properties;
sub init {
	my ($self,@args) = @_;
	$self->{props} = undef;
}

#package Properties;
sub props {
	my ($self,$top,$X,$Y) = @_;
	my $row = 0;
	my $tw;

	if ($tw = $self->{props}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
	} else {
		my $title = $self->shortid." Properties";
		my $tl = $top->widget->toplevel;
		$tw = $tl->Toplevel(
			-title=>$title,
			Name=>'objectProperties',
			-class=>'SS7view',
		);
		$tw->group($tl);
		#$tw->transient($tl);
		$tw->iconimage('icon');
		$tw->iconname($title);
		#$tw->resizable(0,0);
		$tw->positionfrom('user');
		$tw->geometry("+$X+$Y");
		$tw->protocol('WM_DELETE_WINDOW', [sub {
			my $self = shift;
			my $tw = $self->{props};
			$self->{props} = undef;
			$tw->destroy;
		},$self]);
		$self->{props} = $tw;

		my $v = $self->identify;
		$tw->Label(%labelright,
			-text=>'Object type:',
		)->grid(-row=>$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryleft,
			-textvariable=>\$v,
		)->grid(-row=>$row++,-column=>1,-sticky=>'ewns');

		$self->fillprops($top,$tw,\$row);
	}
	$tw->update;
	$tw->MapWindow;
}

#package Properties;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;
}

# -------------------------------------
package Status;
use strict;
# -------------------------------------

#package Status;
sub init {
	my ($self,@args) = @_;
	$self->{statu} = undef;
}

#package Status;
sub status {
	my ($self,$top,$X,$Y) = @_;
	my $row = 0;
	my $tw;
	if ($tw = $self->{statu}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
	} else {
		my $title = $self->shortid." Status";
		my $tl = $top->widget->toplevel;
		$tw = $tl->Toplevel(
			-title=>$title,
			Name=>'objectStatus',
			-class=>'SS7view',
		);
		$tw->group($tl);
		#$tw->transient($tl);
		$tw->iconimage('icon');
		$tw->iconname($title);
		#$tw->resizable(0,0);
		$tw->positionfrom('user');
		$tw->geometry("+$X+$Y");
		$tw->protocol('WM_DELETE_WINDOW', [sub {
			my $self = shift;
			my $tw = $self->{statu};
			$self->{statu} = undef;
			$tw->destroy;
		},$self]);
		$self->{statu} = $tw;
		$self->fillstatus($top,$tw,\$row);
	}
	$tw->update;
	$tw->MapWindow;
}

sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
}

# -------------------------------------
package Clickable;
use strict;
# -------------------------------------

#package Clickable;
sub init {
	my ($self) = @_;
}

#package Clickable;
sub attach {
	my ($self,$top) = @_;
	if (exists $self->{items}) {
		foreach my $i (@{$self->{items}}) {
			$top->canvas->bind($i,'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
		}
		$top->mycanvas->addballoon($self,$self->{items});
	} else {
		$top->canvas->bind($self->{item},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
		$top->mycanvas->addballoon($self,[$self->{item}]);
	}
	#print STDERR "D: discovered ".$self->identify."\n";
	$top->setstatus("Discovered ".$self->identify);
	$top->{updatenow} = 1;
}

#package Clickable;
sub getmenu {
	my ($self,$m,$top,$X,$Y) = @_;
	my $ref = ref $self;
	my $len = length($ref) + 1;
	if ($self->isa('Properties')) {
		$m->add('command',
			#-accelerator=>'p',
			-command=>[\&Properties::props, $self, $top, $X, $Y],
			-label=>"$ref Properties...",
			-underline=>$len,
		);
	}
	if ($self->isa('Status')) {
		$m->add('command',
			#-accelerator=>'s',
			-command=>[\&Status::status, $self, $top, $X, $Y],
			-label=>"$ref Status...",
			-underline=>$len,
		);
	}
	if ($self->isa('Logging')) {
		$m->add('command',
			#-accelerator=>'l',
			-command=>[\&Logging::showlog, $self, $top, $X, $Y],
			-label=>"$ref Logs...",
			-underline=>$len,
			-state=>scalar @{$self->{logs}} ? 'normal' : 'disabled',
		);
	}
	if ($self->isa('MsgStats')) {
		$m->add('command',
			#-accelerator=>'m',
			-command=>[\&MsgStats::stats, $self, $top, $X, $Y],
			-label=>'Message Statistics...',
			-underline=>0,
			-state=>$self->{incs}->{2}->{sus} > 0 ? 'normal' : 'disabled',
		);
	}
	if ($self->isa('CallStats')) {
		$m->add('command',
			#-accelerator=>'c',
			-command=>[\&CallStats::cstat, $self, $top, $X, $Y],
			-label=>'Call statistics...',
			-underline=>0,
			-state=>$self->{ciccnt} ? 'normal' : 'disabled',
		);
	}
	if ($self->isa('MsgCollector')) {
		$m->add('command',
			#-accelerator=>'e',
			-command=>[\&MsgCollector::msgs, $self, $top, $X, $Y],
			-label=>'Messages...',
			-underline=>1,
			-state=>scalar @{$self->{msgs}} ? 'normal' : 'disabled',
		);
	}
	if ($self->isa('CallCollector')) {
		$m->add('command',
			#-accelerator=>'a',
			-command=>[\&CallCollector::calls, $self, $top, $X, $Y],
			-label=>'Calls...',
			-underline=>1,
			-state=>scalar @{$self->{calls}} ? 'normal' : 'disabled',
		);
	}
}

#package Clickable;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
}

#package Clickable;
sub button3 {
	my ($widget,$self,$top,$X,$Y) = @_;
	my $title = $self->identify;
	my $m = $widget->toplevel->Menu(-title=>"$title Menu");
	$self->getmenu($m,$top,$X,$Y,1);
	$self->getmore($m,$top,$X,$Y,1);
	$m->Popup(-popanchor=>'nw',-popover=>'cursor');
}

#package Clickable;
sub bindtowidget {
	my ($self,$top,$widget) = @_;
	$widget->bind('<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
}

# -------------------------------------
package Stateful;
use strict;
# -------------------------------------

#package Stateful;
sub init {
	my ($self,$top,$state,$options,$colors) = @_;
	$self->{options} = $options;
	$self->{colors} = $colors;
	$self->{state} = $self->{oldstate} = $state;
	$self->{color} = $colors->[$state];
	my $tl = $top->widget;
	$top->widget->traceVariable(\$self->{state},'w'=>[\&Stateful::tracestate,$self,$top]);
}
#package Stateful;
sub destroy {
	my ($self,$top) = @_;
	$top->widget->traceVdelete(\$self->{state});
}
#package Stateful;
sub tracestate {
	my ($ind,$val,$op,$self,$top) = @_;
	if ($op eq 'w') {
		if ($self->{oldstate} != $val) {
			my $color = $self->{colors}->[$val];
			if ($self->{color} ne $color) {
				$self->{color} = $color;
				$top->canvas->itemconfigure($self->{items}->[0], -fill=>$color)
					if exists $self->{items} and $self->{items}->[0];
			}
			$top->setstatus(
				$self->identify.' state changed from '.$self->{options}->[$self->{oldstate}]->[0].' to '.$self->{options}->[$val]->[0],
			);
			$self->statechange($top,$val);
			$self->{oldstate} = $val;
		}
	}
	return $val;
}
#package Stateful;
sub statechange {
	my ($self,$top,$val) = @_;
}
#package Stateful;
sub updatestate {
	my ($self,$top,$obj) = @_;
}
#package Stateful;
sub statetext { my $self = shift; return $self->{options}->[$self->{state}]->[0]; }
#package Stateful;
sub fillstate {
	my ($self,$top,$w,$row) = @_;
	$w->Label(%labelright,
		-text=>'State:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $statetext = $self->statetext;
	$w->Optionmenu(%optionleft,
		-options=>$self->{options},
		-variable=>\$self->{state},
		-textvariable=>\$statetext,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

# -------------------------------------
package Tristate;
use strict;
use vars qw(@ISA);
@ISA = qw(Stateful);
# -------------------------------------

#package Tristate;
use constant {
	TS_AVAILABLE => 0,
	TS_DEGRADED => 1,
	TS_UNAVAILABLE => 2,
};
#package Tristate;
$Tristate::options = [
	['Available'	=> TS_AVAILABLE    ],
	['Degraded'	=> TS_DEGRADED	    ],
	['Unavailable'	=> TS_UNAVAILABLE  ],
];
#package Tristate;
$Tristate::colors = {
	white=>['white', 'orange', 'red'],
	black=>['black', 'orange', 'red'],
	grey =>['grey',  'orange', 'red'],
};
#package Tristate;
sub init {
	my ($self,$top,$color) = @_;
	$self->Stateful::init($top,TS_AVAILABLE,$Tristate::options,
		$Tristate::colors->{$color});
}

# -------------------------------------
package Hexstate;
use strict;
use vars qw(@ISA);
@ISA = qw(Stateful);
# -------------------------------------

#package Hexstate;
use constant {
	HS_AVAILABLE => 0,
	HS_DEGRADED => 1,
	HS_CONGESTED_1 => 2,
	HS_CONGESTED_2 => 3,
	HS_CONGESTED_3 => 4,
	HS_UNAVAILABLE => 5,
};
#package Hexstate;
$Hexstate::options = [
	['Available'	=> HS_AVAILABLE	    ],
	['Degraded'	=> HS_DEGRADED	    ],
	['Congested(1)'	=> HS_CONGESTED_1   ],
	['Congested(2)'	=> HS_CONGESTED_2   ],
	['Congested(3)'	=> HS_CONGESTED_3   ],
	['Unavailable' 	=> HS_UNAVAILABLE   ],
];
#package Hexstate;
$Hexstate::colors = {
	white=>['white', 'orange', 'yellow1', 'yellow2', 'yellow3', 'red'],
	black=>['black', 'orange', 'yellow1', 'yellow2', 'yellow3', 'red'],
	grey =>['grey',  'orange', 'yellow1', 'yellow2', 'yellow3', 'red'],
};
#package Hexstate;
sub init {
	my ($self,$top,$color) = @_;
	$self->Stateful::init($top,HS_AVAILABLE,$Hexstate::options,
		$Hexstate::colors->{$color});
}

# -------------------------------------
package Pentastate;
use strict;
use vars qw(@ISA);
@ISA = qw(Stateful);
# -------------------------------------

#package Pentastate;
use constant {
	PS_INSERVICE => 0,
	PS_BUSY => 1,
	PS_INHIBITED => 2,
	PS_BLOCKED => 3,
	PS_OUTOFSERVICE => 4,
};
#package Pentastate;
$Pentastate::options = [
	['Inservice'	  => PS_INSERVICE	],
	['Busy'		  => PS_BUSY		],
	['Inhibited'	  => PS_INHIBITED	],
	['Blocked'	  => PS_BLOCKED		],
	['Out of service' => PS_OUTOFSERVICE	],
];
#package Pentastate;
$Pentastate::colors = {
	white=>['white', 'orange', 'blue', 'red3', 'red'],
	black=>['black', 'orange', 'blue', 'red3', 'red'],
	grey =>['grey',  'orange', 'blue', 'red3', 'red'],
};
#package Pentastate;
sub init {
	my ($self,$top,$color) = @_;
	$self->Stateful::init($top,PS_INSERVICE,$Pentastate::options,
		$Pentastate::colors->{$color});
}

# -------------------------------------
package Arcend;
use strict;
# -------------------------------------

#package Arcend;
sub init {
	my ($self,$top,$x,$y,@args) = @_;
	$self->{x} = $x;
	$self->{y} = $y;
	$self->{arcs} = {};
	$self->{arcs}->{a} = [];
	$self->{arcs}->{b} = [];
}
#package Arcend;
sub destroy {
	my ($self,$top) = @_;
	while (my $arc = shift @{$self->{arcs}->{a}}) {
		$arc->destroy($top);
	}
	while (my $arc = shift @{$self->{arcs}->{b}}) {
		$arc->destroy($top);
	}
}

#package Arcend;
sub moveit {
	my ($self,$top,$x,$y) = @_;
	if ($self->{x} != $x or $self->{y} != $y) {
		$self->{x} = $x;
		$self->{y} = $y;
		#print STDERR "D: moving Arcend ".$self->identify."\n";
		foreach my $side (keys %{$self->{arcs}}) {
			foreach my $arc (@{$self->{arcs}->{$side}}) {
				$arc->moveit($top,$side,$x,$y);
			}
		}
		return 1;
	}
	return undef;
}

#package Arcend;
sub absorb {
	my ($self,$top,$end) = @_;
	#print STDERR "D: absorbing Arcend ".$end->identify." into ".$self->identify."\n";
	# move all the arcs from $end to attach instead to $self.
	while (my $arc = shift @{$end->{arcs}->{a}}) {
		print STDERR "E: object mismatch $end and $arc->{obja}\n" if $end ne $arc->{obja};
		$arc->{obja} = $self;
		push @{$self->{arcs}->{a}}, $arc;
	}
	while (my $arc = shift @{$end->{arcs}->{b}}) {
		print STDERR "E: object mismatch $end and $arc->{objb}\n" if $end ne $arc->{objb};
		$arc->{objb} = $self;
		push @{$self->{arcs}->{b}}, $arc;
	}
	# note: any arcs that went from $end to $self will wind up going
	# from $self to $self.
}

# -------------------------------------
package Nodeitem;
use strict;
use vars qw(@ISA);
@ISA = qw(Arcend);
# -------------------------------------

#package Nodeitem;
sub new {
	my ($type,$top,$row,$col,$off,$nodeno,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->{off} = $off;
	$self->{col} = $col;
	$self->{row} = $row;
	$self->{items} = [];
	$self->{color} = 'white';
	my $x = $top->mycanvas->colpos($col+$off);
	my $y = $top->mycanvas->rowpos($row);
	$self->Arcend::init($top,$x,$y);
	my $state = $top->{show}->{"\L$type\Es"} ? 'normal' : 'hidden';
	my $c = $top->canvas;
	push @{$self->{items}}, $self->{item} = $c->createOval(
		$x-40,$y-40,$x+40,$y+40,
		-fill=>$self->{color},
		-outline=>'blue', -width=>2,
		-activeoutline=>'cyan', -activewidth=>3,
		-state=>$state,
		-tags=>[$type,'node'],
	);
	push @{$self->{items}}, $self->{scri} = $c->createLine(
		$x-23,$y-23,$x+23,$y-23,$x+23,$y+23,$x-23,$y+23,$x-23,$y-23,
		-arrow=>'none', -capstyle=>'round', -joinstyle=>'round', -smooth=>0,
		-fill=>'gray', -width=>0.1,
		#-activefill=>'cyan', -activewidth=>2,
		-state=>$state,
		-tags=>[$type,'scri'],
	);
	push @{$self->{items}}, $self->{ownr} = $c->createText($x,$y+10,
		-anchor=>'center', -fill=>'black', -justify=>'center',
		-text=>$nodeno,
		-state=>$state,
		-tags=>[$type,'text'],
	);
	push @{$self->{items}}, $self->{ttxt} = $c->createText($x,$y-20,
		-anchor=>'center', -fill=>'black', -justify=>'center',
		-text=>$type,
		-state=>$state,
		-tags=>[$type,'text'],
	);
	push @{$self->{items}}, $self->{mcnt} = $c->createText($x,$y+25,
		-anchor=>'center', -fill=>'blue', -justify=>'center',
		-text=>'0',
		-state=>$state,
		-tags=>[$type,'text'],
	);
	push @{$self->{items}}, $self->{text} = $c->createText($x,$y-5,
		-anchor=>'center', -fill=>'black', -justify=>'center',
		-text=>'?',
		-state=>$state,
		-tags=>[$type,'text'],
	);
	my $olditem = 'all';
	foreach (@{$self->{items}}) {
		$c->raise($_,$olditem);
		$olditem = $_;
	}
	#print STDERR "D: must regroup\n";
	$top->{regroupnow} = 1;
	#$top->regroup();
	return $self;
}

#package Nodeitem;
sub moveit {
	my ($self,$top,$x,$y) = @_;
	if ($self->{x} != $x or $self->{y} != $y) {
		#print STDERR "D: moving Nodeitem ".$self->identify."\n";
		my ($dx,$dy) = ($x-$self->{x},$y-$self->{y});
		my $c = $top->canvas;
		foreach my $item (@{$self->{items}}) { $c->move($item,$dx,$dy); }
		$self->Arcend::moveit($top,$x,$y);
	}
}

#package Nodeitem;
sub movenode {
	my ($self,$top,$col,$row,$off) = @_;
	($self->{col},$self->{row},$self->{off}) = ($col,$row,$off);
	my $x = $top->mycanvas->colpos($col+$off);
	my $y = $top->mycanvas->rowpos($row);
	$self->moveit($top,$x,$y);
}

#package Nodeitem;
sub makecol {
	my ($self,$top,$col) = @_;
	# move node to new column but on same side of canvas
	my $new = abs($col);
	my $old = abs($self->{col});
	if ($new != ::COL_ADJ) {
		return undef if ($old == ::COL_ADJ); # stick to adjacent column
		return undef if ($old == ::COL_GTT); # stick to alias column
	}
	return undef if $old == $new;
	$new = -$new if $self->{col} < 0;
	$self->{col} = $new;
	#print STDERR "D: must regroup\n";
	$top->{regroupnow} = 1;
	#$top->regroup();
	return 1;
}

#package Nodeitem;
sub swapcol {
	my ($self,$top) = @_;
	# move node to opposite side of canvas
	$self->{col} = -$self->{col};
	#print STDERR "D: must regroup\n";
	$top->{regroupnow} = 1;
	#$top->regroup();
	return 1;
}

#package Nodeitem;
sub dashme {
	my ($self,$top) = @_;
	$top->canvas->itemconfigure($self->{item},-dash=>[5,2]);
	$top->canvas->itemconfigure($self->{scri},-dash=>[5,2]);
	#print STDERR "D: discovered ".$self->identify."\n";
	$top->setstatus("Discovered ".$self->identify." is an alias.\n");
}

#package Nodeitem;
sub absorb {
	my ($self,$top,$node) = @_;
	#print STDERR "D: absorbing Nodeitem ".$node->identify." into ".$self->identify."\n";
	# put ourselves in the same place so arcs don't have to be moved
	$self->movenode($top,$node->{col},$node->{row},$node->{off});
	$self->Arcend::absorb($top,$node);
	my $c = $top->canvas;
	while (my $item = shift @{$node->{items}}) {
		$c->delete($item);
	}
	foreach (qw(item scri ttxt text ownr mcnt off col row)) { delete $node->{$_}; }
	#print STDERR "D: must regroup\n";
	$top->{regroupnow} = 1;
	#$top->regroup();
}

# -------------------------------------
package Arcitem;
use strict;
# -------------------------------------

$Arcitem::style = {
	line=>{
		-arrow=>'none',
		-capstyle=>'round',
		-joinstyle=>'round',
		-smooth=>0,
	},
	last=>{
		-arrow=>'last',
		-capstyle=>'round',
		-joinstyle=>'round',
		-smooth=>0,
	},
	dash=>{
		-arrow=>'none',
		-capstyle=>'round',
		-joinstyle=>'round',
		-smooth=>0,
		-dash=>[5,2],
	},
};

#package Arcitem;
sub init {
	my ($self,$top,$obja,$objb,$under,$above,$format,$yoff) = @_;
	$format = 'line' unless defined $format;
	my $type = ref $self;
	my $show = "\L$type\Es";
	my $tags = "\L$type\E";
	my $c = $top->canvas;
	$self->{yoff} = $yoff;
	$self->{obja} = $obja;
	$self->{objb} = $objb;
	$obja->{arcs} = {} unless exists $obja->{arcs};
	$obja->{arcs}->{a} = [] unless exists $obja->{arcs}->{a};
	push @{$obja->{arcs}->{a}}, $self;
	$objb->{arcs} = {} unless exists $objb->{arcs};
	$objb->{arcs}->{b} = [] unless exists $objb->{arcs}->{b};
	push @{$objb->{arcs}->{b}}, $self;
	my $xa = $self->{xa} = $obja->{x};
	my $ya = $self->{ya} = $obja->{y};
	my $xb = $self->{xb} = $objb->{x};
	my $yb = $self->{yb} = $objb->{y};
	my $x = $self->{x} = ($xa + $xb)/2;
	my $y = $self->{y} = ($ya + $yb)/2;
	$self->{items} = [];
	$self->Arcend::init($top,$x,$y) if $self->isa('Arcend');
	$self->{item} = $c->createLine($xa,$ya+$yoff,$xb,$yb+$yoff,%{$Arcitem::style->{$format}},
		-fill=>$self->{color}, -width=>0.1,
		-activefill=>'cyan', -activewidth=>2,
		-state=>$top->{show}->{$show}?'normal':'hidden',
		-tags=>[$tags],
	);
	push @{$self->{items}}, $self->{item};
	foreach my $o (@{$above}) { $c->raise($self->{item},$o); }
	foreach my $u (@{$under}) { $c->lower($self->{item},$u); }
}

#package Arcitem;
sub moveit {
	my ($self,$top,$side,$xi,$yi) = @_;
	my $obja = $self->{obja};
	my $objb = $self->{objb};
	my $xa = $obja->{x};
	my $ya = $obja->{y};
	my $xb = $objb->{x};
	my $yb = $objb->{y};
	return undef if $xa == $self->{xa} &&
			$ya == $self->{ya} &&
			$xb == $self->{xb} &&
			$yb == $self->{yb};
			#print STDERR "D: moving Arcitem ".$self->identify."\n";
	my $yoff = $self->{yoff};
	$top->canvas->coords($self->{item},$xa,$ya+$yoff,$xb,$yb+$yoff);
	$self->{xa} = $xa;
	$self->{ya} = $ya;
	$self->{xb} = $xb;
	$self->{yb} = $yb;
	my $x = ($xa + $xb)/2;
	my $y = ($ya + $yb)/2;
	return $self->Arcend::moveit($top,$x,$y) if $self->isa('Arcend');
	$self->{x} = $x;
	$self->{y} = $y;
	return 1;
}

# -------------------------------------
package Relation;
use strict;
use vars qw(@ISA);
@ISA = qw(Hexstate Arcitem Logging Properties Status Clickable DualCollector);
# -------------------------------------
# A relation is an association between signalling points that communicate with
# each other.  This object is used to track these interactions, primarily for
# identifying the role of nodes.
# -------------------------------------

#package Relation;
sub init {
	my ($self,$top,$relationno,$nodea,$nodeb,@args) = @_;
	$self->Hexstate::init($top,'black');
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->DualCollector::init($top,@args);
	$self->{key} = "$nodea->{pc},$nodeb->{pc}";
	$self->{cics} = {};
	$self->{slccnt} = 0;
	$self->{node}->{a} = $nodea;
	$self->{node}->{b} = $nodeb;
	$nodea->{relate}->{$nodeb->{pc}} = $self;
	$nodeb->{relate}->{$nodea->{pc}} = $self;
	$self->Arcitem::init($top,$nodea,$nodeb,['node'],[],'line',0);
	$self->Clickable::attach($top,@args);


}

#package Relation;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Relation;
sub identify {
	my $self = shift;
	my $id = $self->{ciccnt} ? 'Circuits ' : 'Relation ';
	$id .= $self->{node}->{a}->shortid;
	$id .= ', ';
	$id .= $self->{node}->{b}->shortid;
	return ($self->{id} = $id);
}

#package Relation;
sub shortid {
	my $self = shift;
	return "$self->{node}->{a}->{pcode}::$self->{node}->{b}->{pcode}";
}

#package Relation;
sub getCircuit {
	my ($self,$top,$cic,@args) = @_;
	return $self->{cics}->{$cic} if $self->{cics}->{$cic};
	my $circuit = Circuit->new($top,$self,$cic,@args);
	$self->{cics}->{$cic} = $circuit;
	return $circuit;
}

#package Relation;
sub add_forw { # from nodea to nodeb
	my ($self,$top,$network,$msg) = @_;
	my $si = $msg->{si};
	my $mt = $msg->{mt};
	if ($si == 5) {
		if ($mt == 0x10 || (0x12 <= $mt && $mt <= 0x1b) || $mt == 0x2a || $mt == 0x2b) {
			unless ($self->{xchg_isup}) {
				$self->{xchg_isup} = 1;
				$self->{reanalyze} = 1;
			}
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
	if ($si == 3) {
		if ($mt == 0x09 || $mt == 0x11 || $mt == 0x13) { # udt,xudt,ludt
			unless ($self->{forw_tcap}) {
				$self->{forw_tcap} = 1;
				$self->{reanalyze} = 1;
			}
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
	if ($si == 2 || $si == 1) {
		if ($mt == 0x11 || $mt == 0x12) { # sltm,slta
			unless ($self->{exch_sltm}) {
				$self->{exch_sltm} = 1;
				$self->{reanalyze} = 1;
			}
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
}

#package Relation;
sub add_revs { # from nodeb to nodea
	my ($self,$top,$network,$msg) = @_;
	my $si = $msg->{si};
	my $mt = $msg->{mt};
	if ($si == 5) {
		if ($mt == 0x10 || (0x12 <= $mt && $mt <= 0x1b) || $mt == 0x2a || $mt == 0x2b) {
			unless ($self->{xchg_isup}) {
				$self->{xchg_isup} = 1;
				$self->{reanalyze} = 1;
			}
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
	if ($si == 3) {
		if ($mt == 0x09 || $mt == 0x11 || $mt == 0x13) { # udt,xudt,ludt
			unless ($self->{revs_tcap}) {
				$self->{revs_tcap} = 1;
				$self->{reanalyze} = 1;
			}
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
	if ($si == 2 || $si == 1) {
		if ($mt == 0x11 || $mt == 0x12) { # sltm,slta
			unless ($self->{xchg_sltm}) {
				$self->{xchg_sltm} = 1;
				$self->{reanalyze} = 1;
			}
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
}

#package Relation;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	if ($msg->{si} == 5) {
		my ($mt,$cic) = ($msg->{mt},$msg->{cic});
		if ($self->{ciccnt} or ($mt == 0x10 || (0x12 <= $mt && $mt <= 0x1b) || $mt == 0x2a || $mt == 0x2b)) {
			$self->getCircuit($top,$cic)->add_msg($top,$network,$msg,$dir);
		}
	}
	if (defined $dir) {
		if ($dir)   { $self->add_revs($top,$network,$msg); }
		else	    { $self->add_forw($top,$network,$msg); }
		$self->inc($msg,$dir);
		$self->pushmsg($msg);
		return;
	}
	print STDERR "W: Relation::add_msg called without a direction\n";
	if ($self->{key} eq "$msg->{opc},$msg->{dpc}") {
		$self->add_forw($top,$network,$msg);
		$self->inc($msg,0);
		$self->pushmsg($msg);
		return;
	}
	if ($self->{key} eq "$msg->{dpc},$msg->{opc}") {
		$self->add_revs($top,$network,$msg);
		$self->inc($msg,1);
		$self->pushmsg($msg);
		return;
	}
	print STDERR "E: key=$self->{key}, opc=$msg->{opc}, dpc=$msg->{dpc}\n";
}

#package Relation;
sub reanalyze {
	my ($self,$top,$network) = @_;
	my $c = $top->canvas;
	if ($self->{xchg_sltm}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill});
		$c->addtag('linkset',withtag=>$self->{item});
		$c->addtag('SLTM',withtag=>$self->{item});
		return;
	}
	if ($self->{xchg_isup}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill});
		$c->addtag('circuits',withtag=>$self->{item});
		$c->addtag('ISUP',withtag=>$self->{item});
		return;
	}
	if ($self->{forw_tcap} && $self->{revs_tcap}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill},-dash=>[5,3]);
		$c->addtag('association',withtag=>$self->{item});
		$c->addtag('TCAP',withtag=>$self->{item});
		return;
	}
	if ($self->{forw_tcap}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill},-dash=>[5,3]);
		$c->addtag('association',withtag=>$self->{item});
		$c->addtag('TCAP',withtag=>$self->{item});
		return;
	}
	if ($self->{revs_tcap}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill},-dash=>[5,3]);
		$c->addtag('association',withtag=>$self->{item});
		$c->addtag('TCAP',withtag=>$self->{item});
		return;
	}
	$self->{fill} = 'gray';
	$c->itemconfigure($self->{item}, -fill=>$self->{fill});
	return;
	if ($self->{xchg_isup}) {
		if ($self->{forw_tcap}) {
			if ($self->{revs_tcap}) {
				if ($self->{xchg_sltm}) {
				} else {
				}
			} else {
				if ($self->{xchg_sltm}) {
				} else {
				}
			}
		} else {
			if ($self->{revs_tcap}) {
				if ($self->{xchg_sltm}) {
				} else {
				}
			} else {
				if ($self->{xchg_sltm}) {
				} else {
				}
			}
		}
	} else {
		if ($self->{forw_tcap}) {
			if ($self->{revs_tcap}) {
				if ($self->{xchg_sltm}) {
				} else {
				}
			} else {
				# SCP's originate but never terminate
				if ($self->{xchg_sltm}) {
				} else {
				}
			}
		} else {
			if ($self->{revs_tcap}) {
				# GTT functions never originate, just terminate
				if ($self->{xchg_sltm}) {
				} else {
				}
			} else {
				if ($self->{xchg_sltm}) {
				} else {
				}
			}
		}
	}
	delete $self->{reanalyze};
}

#package Relation;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $have = {};
	my $have->{linkset} = $self->{linkset} ? 1 : 0;
	my $have->{links} = scalar keys %{$self->{links}};
	$m->add('separator'); # if $have->{linkset} + $have->{links};
	{
		my $label = $self->{node}->{a}->shortid;
		my $mc = $m->Menu(-title=>"$label Menu");
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			$self->{node}->{a}->getmenu($mc,$top,$X,$Y);
			$self->{node}->{a}->getmore($mc,$top,$X,$Y);
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>$label);
	}
	{
		my $label = $self->{node}->{b}->shortid;
		my $mc = $m->Menu(-title=>"$label Menu");
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			$self->{node}->{b}->getmenu($mc,$top,$X,$Y);
			$self->{node}->{b}->getmore($mc,$top,$X,$Y);
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>$label);
	}
	if ($have->{linkset}) {
		my $linkset = $self->{linkset};
		unless ($Linkset::gotmenu) {
			my $mc = $m->Menu(-title=>'Linkset Menu');
			$mc->configure(-postcommand=>[sub {
				my ($self,$mc,$top,$X,$Y) = @_;
				$mc->delete(0,'end');
				$linkset->getmenu($mc,$top,$X,$Y);
				$linkset->getmore($mc,$top,$X,$Y);
			},$self,$mc,$top,$X,$Y]);
			$m->add('cascade',-menu=>$mc,-label=>'Linkset');
		}

	}
	if ($have->{links}) {
		my ($mc,$m3);
		$mc = $m->Menu(-title=>'Links Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $slc (sort {$a <=> $b} keys %{$self->{links}}) {
				my $link = $self->{links}->{$slc};
				$m3 = $mc->Menu(-title=>"Link $slc Menu");
				$link->getmenu($m3,$top,$X,$Y);
				$link->getmore($m3,$top,$X,$Y);
				$mc->add('cascade', -menu=>$m3, -label=>"Link $slc");
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade', -menu=>$mc, -label=>'Links');
	}
#	$mc = $m->Menu(
#		-tearoff=>1,
#		-title=>'Circuits Menu',
#	);
#	foreach my $cic (sort {$a <=> $b} keys %{$self->{cics}}) {
#		my $count = 0;
#		my $circuit;
#		$m3 = $mc->Menu(
#			-tearoff=>0,
#			-title=>"Circuit $cic Menu",
#		);
#		$circuit = $self->{cics}->{$cic};
#		$count += $circuit->getmenu($m3,$top,$X,$Y) if $circuit;
#		$mc->add('cascade',
#			-menu=>$m3,
#			-label=>"Circuit $cic",
#		);
#	}
#	if ($count) {
#		$m->add('cascade',
#			-menu=>$mc,
#			-label=>'Circuits',
#			-state=>((keys %{$self->{cics}})?'normal':'disabled'),
#		);
#	}
}

#package Relation;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'SP A point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-text=>$self->{node}->{a}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-text=>$self->{node}->{b}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
}

#package Relation;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Exchange SLTM:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Checkbutton(%buttonleft,
		-text=>'Exchanges SLTM',
		-variable=>\$self->{xchg_sltm},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Exchange ISUP:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Checkbutton(%buttonleft,
		-text=>'Exchanges ISUP',
		-variable=>\$self->{xchg_isup},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Forward TCAP:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Checkbutton(%buttonleft,
		-text=>'Forward TCAP',
		-variable=>\$self->{forw_tcap},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Reverse TCAP:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Checkbutton(%buttonleft,
		-text=>'Reverse TCAP',
		-variable=>\$self->{revs_tcap},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$self->fillstate($top,$tw,$row);
	if ($self->{slccnt}) {
		$tw->Label(%labelright,
			-text=>'Signalling links:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryright,
			-textvariable=>\$self->{slccnt},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	}
	if ($self->{ciccnt}) {
		$tw->Label(%labelright,
			-text=>'Defined circuits:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryright,
			-textvariable=>\$self->{ciccnt},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		if ($self->{actcnt}->{0}) {
			$tw->Label(%labelright,
				-text=>'Active circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actcnt}->{0},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actcnt}->{1}) {
			$tw->Label(%labelright,
				-text=>'Active forward circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actcnt}->{1},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actcnt}->{2}) {
			$tw->Label(%labelright,
				-text=>'Active reverse circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actcnt}->{2},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actcnt}->{3}) {
			$tw->Label(%labelright,
				-text=>'Active bothway circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actcnt}->{3},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
	}
}

#package Relation;
sub log {
	my ($self,@args) = @_;
	$self->Logging::log(@args);
	$self->{node}->{a}->log(@args);
	$self->{node}->{b}->log(@args);
}

# -------------------------------------
package Routeset;
use strict;
use vars qw(@ISA);
@ISA = qw(Hexstate Arcitem Logging Properties Status Clickable MsgCollector);
# -------------------------------------
# A routeset is a collection of routes from one node (node a) to another (node b).
# In contrast to relations, a routeset is a unidirectional concept.
# -------------------------------------

#package Routeset;
sub init {
	my ($self,$top,$network,$routesetno,$nodea,$nodeb,@args) = @_;
	$self->Hexstate::init($top,'black');
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init($top,@args);
	$self->{no} = $routesetno;
	$self->{key} = "$nodea->{pc},$nodeb->{pc}";
	$self->{node}->{a} = $nodea;
	$self->{node}->{b} = $nodeb;
	$nodea->{routesets}->{a}->{$nodeb->{pc}} = $self;
	$nodeb->{routesets}->{b}->{$nodea->{pc}} = $self;
	my $relation = $self->{relation} = $network->getRelation($top,$nodea,$nodeb,@args);
	my $yoff = 0;
	if ($relation->{node}->{a}->{pc} == $nodea->{pc}) {
		$relation->{routesetforw} = $self;
		$self->{dir} = 0;
		$yoff =  3;
	} else {
		$relation->{routesetrevs} = $self;
		$self->{dir} = 1;
		$yoff = -3;
	}
	$self->Arcitem::init($top,$nodea,$nodeb,['node','relation'],[],'last',$yoff);
	$self->Clickable::attach($top,@args);
}

#package Routeset;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Routeset;
sub identify {
	my $self = shift;
	my $type = ref $self;
	my $id = "$type ";
	$id .= $self->{node}->{a}->shortid;
	$id .= ' -> ';
	$id .= $self->{node}->{b}->shortid;
	return ($self->{id} = $id);
}
#package Routeset;
sub shortid {
	my $self = shift;
	return "$self->{node}->{a}->{pcode}\->$self->{node}->{b}->{pcode}";
}

#package Routeset;
sub add_msg {
	my ($self,$top,$network,$msg) = @_;
	$self->inc($msg,2);
	$self->pushmsg($msg);
	$self->{node}->{a}->add_msg($top,$network,$msg,0);
	$self->{node}->{b}->add_msg($top,$network,$msg,1);
	$self->{relation}->add_msg($top,$network,$msg,$self->{dir});
}

# -------------------------------------
package Call;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgCollector);
# -------------------------------------

#package Call;
sub init {
	my ($self,$top,$circuit,@args) = @_;
	$self->MsgCollector::init($top,@args);
	$self->{circuit} = $circuit;
	$self->{state} = 0;
	if ($circuit->{call}) {
		$circuit->pushcall($top,$circuit->{call});
	}
	$circuit->{call} = $self;
}

#package Call;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Call;
sub setstate {
	my ($self,$msg,$newstate) = @_;
	$self->{state} = $newstate;
}

#package Call;
sub add_msg {
	my ($self,$msg,$state) = @_;
	$self->pushmsg($msg);
	$self->setstate($state);
}

#package Call;
sub clear {
	my ($self,$msg,$state) = @_;
	$self->clearmsgs;
	$self->setstate($state);
}


# -------------------------------------
package Circuit;
use strict;
use vars qw(@ISA);
@ISA = qw(Logging Properties Status Clickable DualCollector);
# -------------------------------------

#package Circuit;
sub get {
	my ($type,$top,$relation,$cic,@args) = @_;
	return $relation->{cics}->{$cic} if $relation->{cics}->{$cic};
	my $self = {
		cic=>$cic,
		relation=>$relation,
		ts=>$top->{begtime},
		dir=>0,
		active=>0,
		state=>::CTS_UNINIT,
	};
	bless $self,$type;
	$self->{group} = $group;
	$self->{cic} = $cic;
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->DualCollector::init($top,@args);
	$self->{dir} = 0;
	$self->{ts} = $top->{begtime};
	$group->{cics}->{$cic} = $self;
	$self->cnt($top);
	$self->{active} = 0;
	$self->{state} = ::CTS_UNINIT;
	return $self;
}

#package Circuit
sub setstate {
	my ($self,$top,$msg,$newstate,$newdir) = @_;
	return if $self->{state} == $newstate;
	my $oldstate = $self->{state};
	my $olddir   = $self->{dir};
	if ($oldstate == ::CTS_UNINIT) {
		$self->act($top);
		$olddir = $newdir;
		$self->peg($top,$oldstate,$msg->{hdr},$olddir); # XXX
	}
	$self->sim($top,$oldstate,$olddir,$newstate,$newdir);
	$self->itv($top,$oldstate,$self->{ts},$msg->{hdr},$olddir);
	$self->peg($top,$newstate,$msg->{hdr},$newdir);
	$self->{ts} = $msg->{hdr};
	$self->{state} = $newstate;
	$self->{dir} = $newdir;
}

#package Circuit;
sub peg { my ($self,@args) = @_;
	my ($top,$dir) = ($args[0],\$args[3]);
	$self->CallHistory::peg(@args);
	$self->{group}->peg(@args);
	$self->{group}->{node}->{a}->peg(@args);
	$$dir ^= 0x1; $self->{group}->{node}->{b}->peg(@args);
	$$dir = 0x02; $top->{network}->peg(@args);
}
#package Circuit;
sub act { my ($self,@args) = @_;
	my $top = $args[0];
	$self->CallHistory::act(@args);
	$self->{group}->act(@args);
	$self->{group}->{node}->{a}->act(@args);
	$self->{group}->{node}->{b}->act(@args);
	$top->{network}->act(@args);
}
#package Circuit;
sub cnt { my ($self,@args) = @_;
	my $top = $args[0];
	$self->CallHistory::cnt(@args);
	$self->{group}->cnt(@args);
	$self->{group}->{node}->{a}->cnt(@args);
	$self->{group}->{node}->{b}->cnt(@args);
	$top->{network}->cnt(@args);
}
#package Circuit;
sub sim { my ($self,@args) = @_;
	my ($top,$olddir,$newdir) = ($args[0],\$args[2],\$args[4]);
	$self->CallHistory::sim(@args);
	$self->{group}->sim(@args);
	$self->{group}->{node}->{a}->sim(@args);
	$$olddir ^= 0x1; $$newdir ^= 0x1;
	$self->{group}->{node}->{b}->sim(@args);
	$$olddir = 0x02; $$newdir = 0x02;
	$top->{network}->sim(@args);
}
#package Circuit
sub itv { my ($self,@args) = @_;
	my ($top,$dir) = ($args[0],\$args[4]);
	$self->CallHistory::itv(@args);
	$self->{group}->itv(@args);
	$self->{group}->{node}->{a}->itv(@args);
	$$dir ^= 0x1; $self->{group}->{node}->{b}->itv(@args);
	$$dir = 0x02; $top->{network}->itv(@args);
}
#package Circuit
sub dur { my ($self,@args) = @_;
	my ($top,$dir) = ($args[0],\$args[4]);
	$self->CallHistory::dur(@args);
	$self->{group}->dur(@args);
	$self->{group}->{node}->{a}->dur(@args);
	$$dir ^= 0x1; $self->{group}->{node}->{b}->dur(@args);
	$$dir = 0x02; $top->{network}->dur(@args);
}
#package Circuit
sub iat { my ($self,@args) = @_;
	my ($top,$dir) = ($args[0],\$args[2]);
	$self->CallHistory::iat(@args);
	$self->{group}->iat(@args);
	$self->{group}->{node}->{a}->iat(@args);
	$$dir ^= 0x1; $self->{group}->{node}->{b}->iat(@args);
	$$dir = 0x02; $top->{network}->iat(@args);
}
#package Circuit;
sub pushcall {
	my ($self,@args) = @_;
	my $top = $args[0];
	$self->CallCollector::pushcall(@args);
	$self->{group}->pushcall(@args);
	$self->{group}->{node}->{a}->pushcall(@args);
	$self->{group}->{node}->{b}->pushcall(@args);
	$top->{network}->pushcall(@args);
}

#package Circuit;
sub end_of_call {
	my ($self,$top,$call,$msg,$dir) = @_;
	$self->setstate($top,$msg,::CTS_IDLE,$dir);
	$call->add_msg($msg,::CTS_IDLE);
	$self->pushcall($top,$call);
}

#package Circuit;
sub clear_call {
	my ($self,$top,$call,$msg,$dir) = @_;
	$self->setstate($top,$msg,::CTS_IDLE,$dir);
	$call->clear($msg,::CTS_IDLE);
}

#package Circuit;
sub restart_call {
	my ($self,$top,$call,$msg,$dir) = @_;
	$self->setstate($top,$msg,::CTS_IDLE,$dir);
	if ($self->msgcnt) {
		$self->pushcall($top,$call);
	} else {
		$call->clear($msg,::CTS_IDLE);
	}
}

#package Circuit;
sub activate {
	my ($self,$network,$msg) = @_;
	if ($msg->{opc} == $self->{group}->{node}->{a}->{pc}) {
		unless ($self->{active} & 0x1) {
			$self->{active} |= 0x1;
			if ($self->{active} & 0x2) {
				$self->{group}->{actcnt}->{3}++;
				$self->{group}->{node}->{a}->{actcnt}->{3}++;
				$self->{group}->{node}->{b}->{actcnt}->{3}++;
				$network->{actcnt}->{2}++;
				$self->{group}->{actcnt}->{2}--;
				$self->{group}->{node}->{a}->{actcnt}->{2}--;
				$self->{group}->{node}->{b}->{actcnt}->{1}--;
				$network->{actcnt}->{1}--;
			} else {
				$self->{group}->{actcnt}->{1}++;
				$self->{group}->{node}->{a}->{actcnt}->{1}++;
				$self->{group}->{node}->{b}->{actcnt}->{2}++;
				$network->{actcnt}->{1}++;
			}
		}
		return 0;
	} else {
		unless ($self->{active} & 0x2) {
			$self->{active} |= 0x2;
			if ($self->{active} & 0x1) {
				$self->{group}->{actcnt}->{3}++;
				$self->{group}->{node}->{a}->{actcnt}->{3}++;
				$self->{group}->{node}->{b}->{actcnt}->{3}++;
				$network->{actcnt}->{2}++;
				$self->{group}->{actcnt}->{1}--;
				$self->{group}->{node}->{a}->{actcnt}->{1}--;
				$self->{group}->{node}->{b}->{actcnt}->{2}--;
				$network->{actcnt}->{1}--;
			} else {
				$self->{group}->{actcnt}->{2}++;
				$self->{group}->{node}->{a}->{actcnt}->{2}++;
				$self->{group}->{node}->{b}->{actcnt}->{1}++;
				$network->{actcnt}->{1}++;
			}
		}
		return 1;
	}
}

#package Circuit;
sub add_msg {
	my ($self,$top,$network,$msg) = @_;
	$self->pushmsg($msg);
	my $mt = $msg->{mt};
	my $call;
	my $dir = $self->{dir};
	while (1) {
		unless ($call = $self->{call}) {
			$call = $self->{call} = Call->new($top,$self);
		}
		if ($mt == 0x01) { # iam
			if ($self->{state} <= ::CTS_WAIT_ACM) {
				$dir = $self->activate($network,$msg);
				$self->setstate($top,$msg,::CTS_WAIT_ACM,$dir);
				last;
			}
			$self->restart_call($top,$call,$msg,$dir);
			next;
		}
		if ($mt == 0x02) { # sam
			last;
		}
		if ($mt == 0x03) { # inr
			last;
		}
		if ($mt == 0x04) { # inf
			last;
		}
		if ($mt == 0x05) { # cot
			last;
		}
		if ($mt == 0x06 || $mt == 0xed) { # acm exm
			if ($self->{state} == ::CTS_WAIT_ACM) {
				$self->setstate($top,$msg,::CTS_WAIT_ANM,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x07) { # con
			if ($self->{state} == ::CTS_WAIT_ANM) {
				$self->setstate($top,$msg,::CTS_ANSWERED,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x08) { # fot
			last;
		}
		if ($mt == 0x09) { # anm
			if ($self->{state} == ::CTS_WAIT_ANM) {
				$self->setstate($top,$msg,::CTS_ANSWERED,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x0c) { # rel
			if ($self->{state} > ::CTS_IDLE) {
				$self->setstate($top,$msg,::CTS_WAIT_RLC,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x0d) { # sus
			if ($self->{state} == ::CTS_ANSWERED || $self->{state} == ::CTS_SUSPENDED) {
				$self->setstate($top,$msg,::CTS_SUSPENDED,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x0e) { # res
			if ($self->{state} == ::CTS_ANSWERED || $self->{state} == ::CTS_SUSPENDED) {
				$self->setstate($top,$msg,::CTS_ANSWERED,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x10) { # rlc
			if ($self->{state} == ::CTS_WAIT_RLC) {
				$self->end_of_call($top,$call,$msg,$dir);
				return;
			}
			return;
		}
		if ($mt == 0x11) { # ccr
			$self->end_of_call($top,$call,$msg,$dir);
			return;
		}
		if ($mt == 0x12) { # rsc
			if ($self->{state} > ::CTS_WAIT_ANM) {
				$self->end_of_call($top,$call,$msg,$dir);
				return;
			}
			$self->clear_call($top,$call,$msg,$dir);
			return;
		}
		if ($mt == 0x13) { # blo
			if ($self->{state} < ::CTS_WAIT_ANM) {
				$self->clear_call($top,$call,$msg,$dir);
				return;
			}
			return;
		}
		if ($mt == 0x14) { # ubl
			return;
		}
		if ($mt == 0x15) { # bla
			if ($self->{state} < ::CTS_WAIT_ANM) {
				$self->clear_call($top,$call,$msg,$dir);
				return;
			}
			return;
		}
		if ($mt == 0x16) { # uba
			return;
		}
		if ($mt == 0x17) { # grs
			if ($self->{state} > ::CTS_WAIT_ANM) {
				$self->end_of_call($top,$call,$msg,$dir);
				return;
			}
			$self->clear_call($top,$call,$msg,$dir);
			return;
		}
		if ($mt == 0x19) { # cgu
			return;
		}
		if ($mt == 0x1a) { # cgba
			if ($self->{state} < ::CTS_WAIT_ANM) {
				$self->clear_call($top,$call,$msg,$dir);
				return;
			}
			return;
		}
		if ($mt == 0x1b) { # cgua
			return;
		}
		if ($mt == 0x1c) { # cmr
			return;
		}
		if ($mt == 0x1d) { # cmc
			return;
		}
		if ($mt == 0x1e) { # cmrj
			return;
		}
		if ($mt == 0x1f) { # far
			return;
		}
		if ($mt == 0x20) { # faa
			return;
		}
		if ($mt == 0x21) { # frj
			return;
		}
		if ($mt == 0x22) { # fad
			return;
		}
		if ($mt == 0x23) { # fai
			return;
		}
		if ($mt == 0x24) { # lpa
			last;
		}
		if ($mt == 0x25) { # csvq
			last;
		}
		if ($mt == 0x26) { # csvr
			last;
		}
		if ($mt == 0x27) { # drs
			last;
		}
		if ($mt == 0x28) { # pam
			last;
		}
		if ($mt == 0x29) { # gra
			if ($self->{state} > ::CTS_WAIT_ANM) {
				$self->end_of_call($top,$call,$msg,$dir);
				return;
			}
			$self->clear_call($top,$call,$msg,$dir);
			return;
		}
		if ($mt == 0x2a) { # cqm
			return;
		}
		if ($mt == 0x2b) { # cqr
			return;
		}
		if ($mt == 0x2c) { # cpg
			if ($self->{state} == ::CTS_WAIT_ACM || $self->{state} == ::CTS_WAIT_ANM) {
				return;
			}
			last;
		}
		if ($mt == 0x2d) { # usr
			return;
		}
		if ($mt == 0x2e) { # ucic
			$self->clear_call($top,$call,$msg,$dir);
			return;
		}
		if ($mt == 0x2f) { # cfn
			if ($self->{state} < ::CTS_ANSWERED) {
				$self->clear_call($top,$call,$msg,$dir);
				return;
			}
			last;
		}
		if ($mt == 0x30) { # olm
			return;
		}
		if ($mt == 0x31) { # crg
			return;
		}
		if ($mt == 0x32) { # nrm
			return;
		}
		if ($mt == 0x33) { # fac
			last;
		}
		if ($mt == 0x34) { # upt
			return;
		}
		if ($mt == 0x35) { # upa
			return;
		}
		if ($mt == 0x36) { # idr
			return;
		}
		if ($mt == 0x37) { # irs
			return;
		}
		if ($mt == 0x38) { # sgm
			last;
		}
		if ($mt == 0xe9) { # cra
			if ($self->{state} == ::CTS_WAIT_ACM) {
				$self->setstate($top,$msg,::CTS_WAIT_ACM,$dir);
				last;
			}
			return;
		}
		if ($mt == 0xea) { # crm
			if ($self->{state} != ::CTS_IDLE && $self->{state} != ::CTS_WAIT_ACM) {
				$self->end_of_call($top,$call,$msg,$dir);
				return;
			}
			$self->setstate($top,$msg,::CTS_WAIT_ACM,$dir);
			last;
		}
		if ($mt == 0xeb) { # cvr
			return;
		}
		if ($mt == 0xec) { # cvt
			return;
		}
		if ($mt == 0xf8) { # non
			return;
		}
		if ($mt == 0xfc) { # llm
			return;
		}
		if ($mt == 0xfd) { # cak
			return;
		}
		if ($mt == 0xfe) { # tcm
			return;
		}
		if ($mt == 0xff) { # mcp
			return;
		}
		return;
	}
	$call->add_msg($msg, $self->{state});
	return;
}

#package Circuit;
sub identify {
	my $self = shift;
	my $group = $self->{group};
	my $id = "Circuit $self->{cic} ";
	$id .= $group->{node}->{a}->shortid;
	$id .= ', ';
	$id .= $group->{node}->{b}->shortid;
	return ($self->{id} = $id);
}

#package Circuit;
sub shortid {
	my $self = shift;
	my $group = $self->{group};
	return "$group->{node}->{a}->{pcode},$group->{node}->{b}->{pcode}:$self->{cic}";
}

#package Circuit;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Circuit id code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{cic}, -width=>8,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'SP A point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entrycenter,
		-text=>$self->{group}->{node}->{a}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entrycenter,
		-text=>$self->{group}->{node}->{b}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
}

#package Circuit;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Active:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-textvariable=>\$self->{active},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

# -------------------------------------
package Linkset;
use strict;
use vars qw(@ISA);
@ISA = qw(Tristate Arcitem Arcend Logging Properties Status Clickable MsgCollector);
# -------------------------------------

#package Linkset;
sub init {
	my ($self,$top,$linksetno,$relation,@args) = @_;
	$self->Tristate::init($top,'black');
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init($top,@args);
	$self->{no} = $linksetno;
	$self->{relation} = $relation; $relation->{linkset} = $self;
	$self->{slccnt} = 0;
	$self->{links} = {};
	my $nodea = $self->{node}->{a} = $relation->{node}->{a};
	my $nodeb = $self->{node}->{b} = $relation->{node}->{b};
	$self->{routes} = {};
	$self->{routes}->{a} = {}; # routes from the a-side
	$self->{routes}->{b} = {}; # routes from the b-side
	$self->Arcitem::init($top,$nodea,$nodeb,['node'],[],'line',0);
	$self->Clickable::attach($top,@args);
}

#package Linkset;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Linkset;
#sub DESTROY {
#	my $self = shift;
#	my $c = $self->canvas;
#	$c->toplevel->traceVdelete(\$self->{state});
#}

#package Linkset;
sub identify {
	my $self = shift;
	my $letter = ['?','A','B','C','D','E','F']->[$self->checktype];
	my $id = "$letter-Linkset ";
	$id .= $self->{node}->{a}->shortid;
	$id .= ', ';
	$id .= $self->{node}->{b}->shortid;
	return ($self->{id} = $id);
}

#package Linkset;
sub shortid {
	my $self = shift;
	return "$self->{node}->{a}->{pcode}:$self->{node}->{b}->{pcode}";
}

#package Linkset;
sub statechange {
	my ($self,$top,$val) = @_;
	my $c = $top->canvas;
	foreach my $side (keys %{$self->{routes}}) {
		foreach my $route (values %{$self->{routes}->{$side}}) {
			$route->updatestate($top,$self);
		}
	}
}

#package Linkset;
sub updatestate {
	my ($self,$top,$obj) = @_;
	my ($inserv,$busy,$ooserv) = (0,0,0);
	foreach my $link (values %{$self->{links}}) {
		my $lstate = $link->{state};
		if ($lstate == Tristate::TS_AVAILABLE) {
			$inserv++;
		} elsif ($lstate == Tristate::TS_DEGRADED) {
			$busy++;
		} elsif ($lstate == Tristate::TS_UNAVAILABLE) {
			$ooserv++;
		}
	}
	my $state;
	if ($inserv == 0 && $busy == 0 && $ooserv > 0) {
		$state = Tristate::TS_UNAVAILABLE;
	} elsif ($busy + $ooserv > 0) {
		$state = Tristate::TS_DEGRADED;
	} else {
		$state = Tristate::TS_AVAILABLE;
	}
	return if $self->{state} == $state;
	$self->{state} = $state;
}

#package Linkset;
sub getRoute {
	my ($self,$top,$network,$side,$node,@args) = @_;
	my $pc = $node->{pc};
	return $self->{routes}->{$side}->{$pc} if $self->{routes}->{$side}->{$pc};
	my $route = $network->getRoute($top,$self,$side,$node);
	$self->{routes}->{$side}->{$pc} = $route;
	return $route;
}

#package Linkset;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	$self->inc($msg,$dir);
	$self->pushmsg($msg);
	if (exists $msg->{dpc}) {
		my ($side,$col);
		if ($dir) { # from b to a to dpc
			$side = 'a'; $col = $self->{node}->{a}->{col};
		} else { # from a to b to dpc
			$side = 'b'; $col = $self->{node}->{b}->{col};
		}
		my $node = $network->getSp($top,$msg->{dpc},$col);
		$self->getRoute($top,$network,$side,$node)->add_msg($top,$network,$msg,2);
	}
}

#package Linkset;
sub moveit {
	my ($self,@args) = @_;
	# just so we do Arcitem::moveit instead of Arcend::moveit
	return $self->Arcitem::moveit(@args);
}

use constant {
	LS_UNKNOWN=>0,
	LS_ALINK=>1,
	LS_BLINK=>2,
	LS_CLINK=>3,
	LS_DLINK=>4,
	LS_ELINK=>5,
	LS_FLINK=>6,
};

#package Linkset;
sub checktype {
	my $self = shift;
	my ($refa,$refb) = (ref $self->{node}->{a},ref $self->{node}->{b});
	return ($self->{type} = {
		SP =>{ SP =>LS_UNKNOWN, SSP=>LS_UNKNOWN, STP=>LS_UNKNOWN, GTT=>LS_UNKNOWN, SCP=>LS_UNKNOWN, },
		SSP=>{ SP =>LS_UNKNOWN, SSP=>LS_FLINK,   STP=>LS_ALINK,   GTT=>LS_ALINK,   SCP=>LS_ELINK,   },
		STP=>{ SP =>LS_UNKNOWN, SSP=>LS_ALINK,   STP=>LS_BLINK,   GTT=>LS_BLINK,   SCP=>LS_ALINK,   },
		GTT=>{ SP =>LS_UNKNOWN, SSP=>LS_ALINK,   STP=>LS_BLINK,   GTT=>LS_BLINK,   SCP=>LS_ALINK,   },
		SCP=>{ SP =>LS_UNKNOWN, SSP=>LS_ELINK,   STP=>LS_ALINK,   GTT=>LS_ALINK,   SCP=>LS_FLINK,   },
	}->{$refa}->{$refb});
}

#package Linkset;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $have = {};
	$have->{links} = scalar keys %{$self->{links}};
	$have->{routes}->{a} = scalar keys %{$self->{routes}->{a}};
	$have->{routes}->{b} = scalar keys %{$self->{routes}->{b}};
	$m->add('separator') if $have->{links} + $have->{routes}->{a} + $have->{routes}->{b};
	if ($have->{links}) {
		my ($mc,$m3);
		$mc = $m->Menu(-title=>'Links Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $slc (sort {$a <=> $b} keys %{$self->{links}}) {
				my $link = $self->{links}->{$slc};
				$m3 = $mc->Menu(-title=>"Link $slc Menu");
				$link->getmenu($m3,$top,$X,$Y);
				$link->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>"Link $slc");
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Links');
	}
	{
		my $node = $self->{node}->{a};
		my $label = 'Node A '.$node->shortid;
		my $mc = $m->Menu(-title=>"$label Menu");
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $pc (sort {$a <=> $b} keys %{$self->{routes}->{b}}) {
				my $route = $self->{routes}->{b}->{$pc};
				$m3 = $mc->Menu(-title=>"Route $route->{node}->{b}->{pcode} Menu");
				$route->getmenu($m3,$top,$X,$Y);
				$route->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>"Route $route->{node}->{b}->{pcode}");

			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Routes Forward');
	}
	{
		my $node = $self->{node}->{b};
		my $label = 'Node B '.$node->shortid;
		my $mc = $m->Menu(-title=>"$label Menu");
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $pc (sort {$a <=> $b} keys %{$self->{routes}->{a}}) {
				my $route = $self->{routes}->{a}->{$pc};
				$m3 = $mc->Menu(-title=>"Route $route->{node}->{b}->{pcode} Menu");
				$route->getmenu($m3,$top,$X,$Y);
				$route->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>"Route $route->{node}->{b}->{pcode}");

			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Routes Reverse');
	}
}

#package Linkset;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'SP A point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entrycenter,
		-text=>$self->{node}->{a}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entrycenter,
		-text=>$self->{node}->{b}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
}

#package Linkset;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Exchange SLTM',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Checkbutton(%buttonleft,
		-text=>'Exchanges SLTM',
		-variable=>\$self->{xchg_sltm},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Exchange ISUP:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Checkbutton(%buttonleft,
		-text=>'Exchanges ISUP',
		-variable=>\$self->{xchg_isup},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Forward TCAP',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Checkbutton(%buttonleft,
		-text=>'Forward TCAP',
		-variable=>\$self->{forw_tcap},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Reverse TCAP',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Checkbutton(%buttonleft,
		-text=>'Reverse TCAP',
		-variable=>\$self->{revs_tcap},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$self->fillstate($top,$tw,$row);
	if ($self->{slccnt}) {
		$tw->Label(%labelright,
			-text=>'Signalling links:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryright,
			-textvariable=>\$self->{slccnt},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	}
}

# -------------------------------------
package Combined;
use strict;
use vars qw(@ISA);
@ISA = qw(Tristate Arcitem Logging Properties Status Clickable MsgCollector);
# -------------------------------------

#package Combined;
sub init {
	my ($self,$top,$combinedno,$linkseta,$linksetb,@args) = @_;
	my $type = ref $self;
	$self->Tristate::init($top,'black');
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init($top,@args);
	$self->{no} = $combinedno;
	$self->{linkseta} = $linkseta;
	$self->{linksetb} = $linksetb;
	$self->{lnkcnt} = 0;
	$self->Arcitem::init($top,$linkseta,$linksetb,['node'],[],'dash',0);
	$self->Clickable::attach($top,@args);
}


# -------------------------------------
package Link;
use strict;
use vars qw(@ISA);
@ISA = qw(Tristate Arcitem Logging Properties Status Clickable MsgCollector);
# -------------------------------------

$Link::offsets = [ -3, 3, -8, 8, -13, 13, -18, 18, -23, 23, -28, 28, -33, 33, -38, 38 ];

#package Link;
sub init {
	my ($self,$top,$linkno,$linkset,$slc,@args) = @_;
	$self->Tristate::init($top,'black');
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init($top,@args);
	$self->{no} = $linkno;
	$self->{linkset} = $linkset;
	$linkset->{links}->{$slc} = $self;
	$self->{slc} = $slc;
	my $nodea = $self->{node}->{a} = $linkset->{node}->{a};
	my $nodeb = $self->{node}->{b} = $linkset->{node}->{b};
	$self->{channelforw} = undef;
	$self->{channelrevs} = undef;
	$self->Arcitem::init($top,$nodea,$nodeb,['node','linkset'],[],'line',$Link::offsets->[$slc]);
	$self->Clickable::attach($top,@args);
}

#package Link;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Link;
sub addfrom {
	my ($self,$othr,@args) = @_;
	$self->SUPER::addfrom($othr,@args);
	$self->{linkset}->addfrom($othr,@args); # note dir is same as link
}

#package Link;
sub statechange {
	my ($self,$top,$val) = @_;
	$self->{linkset}->updatestate($top,$self);
}

#package Link;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	$self->inc($msg,$dir);
	$self->pushmsg($msg);
	$self->{linkset}->add_msg($top,$network,$msg,$dir);
}

#package Link;
sub identify {
	my $self = shift;
	my $id = "Link $self->{slc} ";
	$id .= $self->{node}->{a}->shortid;
	$id .= ', ';
	$id .= $self->{node}->{b}->shortid;
	return ($self->{id} = $id);
}

#package Link;
sub shortid {
	my $self = shift;
	return "$self->{slc}:$self->{node}->{a}->{pcode},$self->{node}->{b}->{pcode}";
}

#package Link;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $have = {};
	$have->{linkset} = $self->{linkset} ? 1 : 0;
	$have->{forw}    = $self->{channelforw} ? 1 : 0;
	$have->{revs}    = $self->{channelrevs} ? 1 : 0;
	$m->add('separator') if
		$have->{linkset} +
		$have->{forw} +
		$have->{revs};
	if ($have->{linkset}) {
		my $linkset = $self->{linkset};
		my $label = 'Linkset';
		my $mc = $m->Menu(-title=>"$label Menu");
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			$linkset->getmenu($mc,$top,$X,$Y);
			$linkset->getmore($mc,$top,$X,$Y);
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade', -menu=>$mc, -label=>$label);
	}
	if ($have->{forw}) {
		my $channel = $self->{channelforw};
		my $label = 'Forward channel';
		my $mc = $m->Menu(-title=>"$label Menu");
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $dli (sort {$a <=> $b} keys %{$self->{datalinks}}) {
				my $datalink = $self->{datalinks}->{$dli};
				my $type = ref($datalink); $type=~s/_/ /g;
				my $label = $type.' '.$datalink->shortid;
				$m3 = $mc->Menu(-title=>"$label Menu");
				$datalink->getmenu($m3,$top,$X,$Y);
				$datalink->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade', -menu=>$mc, -label=>$label);
	}
}

#package Link;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;
}

#package Link;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
	$self->fillstate($top,$tw,$row);
}

# -------------------------------------
package Route;
use strict;
use vars qw(@ISA);
@ISA = qw(Tristate Arcitem Logging Properties Status Clickable MsgCollector);
# -------------------------------------

#package Route
sub new {
	my ($type,$top,$routeno,$linkset,$side,$nodeb,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->Tristate::init($top,'black');
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init($top,@args);
	$self->{no} = $routeno;
	$self->{linkset} = $linkset;
	$self->{side} = $side;
	my $nodea = $linkset->{node}->{$side};
	$self->{node}->{a} = $nodea;
	$self->{node}->{b} = $nodeb;
	$nodea->{routes}->{a}->{$nodeb->{pc}} = $self;
	$nodeb->{routes}->{b}->{$nodea->{pc}} = $self;
	$nodeb->makealias($top,$nodea);
	$self->Arcitem::init($top,$nodea,$nodeb,['node','link','linkset'],[],'last',0);
	$self->Clickable::attach($top,@args);
	return $self;
}

#package Route;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	$self->inc($msg,$dir);
	$self->pushmsg($msg);
}

#package Route;
sub statechange {
	my ($self,$top,$val) = @_;
	$self->{node}->{b}->updatestate($top,$self);
}

#package Route;
sub updatestate {
	my ($self,$top,$obj) = @_;
}

#package Route;
sub identify {
	my $self = shift;
	my $id = "Route ";
	$id .= $self->{node}->{a}->shortid;
	$id .= " -> ";
	$id .= $self->{node}->{b}->shortid;
	return ($self->{id} = $id);
}

#package Route;
sub shortid {
	my $self = shift;
	my $id = $self->{node}->{a}->{pcode};
	$id .= '->';
	$id .= $self->{node}->{b}->{pcode};
	return $id;
}

#package Route;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	$m->add('separator');
	my ($mc,$linkset,$node);

	$linkset = $self->{linkset};
	$mc = $m->Menu(-title=>'Linkset Menu');
	$mc->configure(-postcommand=>[sub {
		my ($self,$mc,$top,$X,$Y) = @_;
		$mc->delete(0,'end');
		$linkset->getmenu($mc,$top,$X,$Y);
		$linkset->getmore($mc,$top,$X,$Y);
	},$self,$mc,$top,$X,$Y]);
	$m->add('cascade',-menu=>$mc,-label=>'Linkset');

	$node =  $self->{node}->{a};
	$mc = $m->Menu(-title=>'Transit Node Menu');
	$mc->configure(-postcommand=>[sub {
		my ($self,$mc,$top,$X,$Y) = @_;
		$mc->delete(0,'end');
		$node->getmenu($mc,$top,$X,$Y);
		$node->getmore($mc,$top,$X,$Y);
	},$self,$mc,$top,$X,$Y]);
	$m->add('cascade',-menu=>$mc,-label=>'Transit Node');

	$node = $self->{node}->{b};
	$mc = $m->Menu(-title=>'Destination Node Menu');
	$mc->configure(-postcommand=>[sub {
		my ($self,$mc,$top,$X,$Y) = @_;
		$mc->delete(0,'end');
		$node->getmenu($mc,$top,$X,$Y);
		$node->getmore($mc,$top,$X,$Y);
	},$self,$mc,$top,$X,$Y]);
	$m->add('cascade',-menu=>$mc,-label=>'Destination Node');
}

#package Route;
sub fillprops {
	my ($self,$top,$tw,$row,$id,$node,$w) = @_;

	$tw->Label(%labelright,
		-text=>'Destination node:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$node = $self->{node}->{b};
	$id = $node->identify;
	$w = $tw->Entry(%entryleft,
		-text=>$id, -width=>length($id),
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$node->bindtowidget($top,$w);
	$tw->Label(%labelright,
		-text=>'Transit node:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$node = $self->{node}->{a};
	$id = $node->identify;
	$w = $tw->Entry(%entryleft,
		-text=>$id, -width=>length($id),
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$node->bindtowidget($top,$w);
}

#package Route;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
	$self->fillstate($top,$tw,$row);
}

# -------------------------------------
package Path;
use strict;
use vars qw(@ISA);
@ISA = qw(Arcitem Tristate Logging Properties Status Clickable MsgCollector);
# -------------------------------------

#package Path;
sub new {
	my ($type,$top,$pathno,$channel,$side,$node,@args) = @_;
	my $self = {};
	bless $self, $type;
	$self->Tristate::init($top,'grey');
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init($top,@args);
	$self->{no} = $pathno;
	$self->{channel} = $channel;
	$self->{side} = $side;
	$self->{node} = $node;
	my ($nodea,$nodeb,$adjacent,$yoff);
	if ($side eq 'a') {
		$adjacent = $self->{adjacent} = $channel->{node}->{a};
		$nodea = $self->{node}->{a} = $node;
		$nodeb = $self->{node}->{b} = $adjacent;
		$yoff = -3;
	} else {
		$adjacent = $self->{adjacent} = $channel->{node}->{b};
		$nodea = $self->{node}->{a} = $adjacent;
		$nodeb = $self->{node}->{b} = $node;
		$yoff =  3;
	}
	$node->makealias($top,$adjacent);
	$nodea->{paths}->{a}->{$channel->{ppa}} = $self;
	$nodeb->{paths}->{b}->{$channel->{ppa}} = $self;
	$self->Arcitem::init($top,$nodea,$nodeb,['node','channel','route','link','linkset'],[],'last',$yoff);
	$self->Clickable::attach($top,@args);
	$channel->swap($top) if $node->{col} < 0 and $adjacent->{col} > 0;
	return $self;
}

#package Path;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	$self->inc($msg,$dir);
	$self->pushmsg($msg);
}

#package Path;
sub statechange {
	my ($self,$top,$val) = @_;
	$self->{node}->updatestate($top,$self);
}

#package Path;
sub identify {
	my $self = shift;
	my $id = "Path ";
	$id .= '('.$self->{channel}->shortid.')';
	$id .= ($self->{side} eq 'a') ? ' <- ' : ' -> ';
	$id .= $self->{node}->shortid;
	return ($self->{id} = $id);
}

#package Path;
sub shortid {
	my $self = shift;
	my $id = '('.$self->{channel}->shortid.')';
	$id .= ($self->{side} eq 'a') ? '<-' : '->';
	$id .= "($self->{node}->{pcode})";
	return $id;
}

#package Path;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Signalling channel:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-text=>$self->{channel}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'Signalling point:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-text=>$self->{node}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
}

#package Path;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
	$self->fillstate($top,$tw,$row);
}

# -------------------------------------
package Node;
use strict;
use vars qw(@ISA);
@ISA = qw(Nodeitem Properties Status Clickable);
# -------------------------------------
# Nodes are unidentified signalling points.  We know that there are two nodes on either side of a
# channel, we just do not know which node yet.  A bare Node object is an unknown signalling point.
# It just acts as a place holder for when the channel is bound to an actual node.  Nodes need a
# destroy method so that the channel can destroy these nodes once they are no longer necessary.
#
# Nodes are created in reference to a channel, the side of the channel on which they exist.
#
# Note that when nodes are created they should position themselves and rely on the channel's Arcitem
# methods to eventually update it.

#package Node;
sub new {
	my ($type,$top,$nodeno,$col,@args) = @_;
	my $self = Nodeitem::new($type,$top,0,$col,0,$nodeno);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->{no} = $nodeno;
	$self->{channels} = {};
	$self->{channels}->{a} = {};
	$self->{channels}->{b} = {};
	$self->{paths} = {};
	$self->{paths}->{a} = {}; # paths that originate here
	$self->{paths}->{b} = {}; # paths that terminate here
	$self->Clickable::attach($top,@args);
	#print STDERR "D: must regroup\n";
	$top->{regroupnow} = 1;
	#$top->regroup();
	return $self;
}

#package Node;
sub identify {
	my $self = shift;
	my $ref = ref $self;
	my $id = 'Unknown ';
	$id .= "$ref $self->{no}";
	return ($self->{id} = $id);
}

#package Node;
sub shortid {
	my $self = shift;
	my $ref = ref $self;
	return "$ref($self->{no})";
}

#package Node;
sub makealias {
	my ($self,$top,$primary) = @_;
	return if $primary->{pc} == $self->{pc};
	return unless $primary->isa('SSP') or $primary->isa('SCP');
	return unless abs($primary->{col}) == ::COL_ADJ;
	unless ($self->{alias}) {
		$self->{alias} = 1;
		$self->{primary} = $primary;
		$primary->{aliases}->{$self->{pc}} = $self;
		$self->makecol($top,::COL_GTT);
		$self->dashme($top);
	}
}

#package Node;
sub findaliases {
	my ($self,$top) = @_;
	return unless abs($self->{col}) == ::COL_ADJ;
	return unless $self->isa('SSP') or $self->isa('SCP');
	foreach my $route (values %{$self->{routes}->{a}}) {
		$route->{node}->{b}->makealias($top,$self);
	}
}

#package Node;
sub absorb {
	my ($self,$top,$node) = @_;
	#print STDERR "D: absorbing Node ".$node->identify." into ".$self->identify."\n";
	# handle arcs
	$self->Nodeitem::absorb($top,$node);
	foreach my $side (keys %{$node->{channels}}) {
		while (my ($ppa,$channel) = each %{$node->{channels}->{$side}}) {
			$self->{channels}->{$side}->{$ppa} = $channel;
		}
	}
	delete $node->{channels};
	foreach my $side (keys %{$node->{paths}}) {
		while (my ($ppa,$path) = each %{$node->{paths}->{$side}}) {
			my $exnode;
			$exnode = $path->{node}->{$side};
			if ($node ne $exnode) {
				print STDERR "E: node $side mismatch $node and $exnode\n";
			} else {
				$path->{node}->{$side} = $self;
			}
			$exnode = $path->{adjacent};
			if ($node ne $exnode) {
				print STDERR "E: adjacent mismatch $node and $exnode\n";
			} else {
				$path->{adjacent} = $self;
			}
			$self->{paths}->{$side}->{$ppa} = $path;
		}
	}
	delete $node->{paths};
	delete $top->{network}->{pnodes}->{$self->{no}};

}

#package Node;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
}

# -------------------------------------
package Host; use strict;
@Host::ISA = qw(Node Logging PairCollector);
# -------------------------------------

#package Host;
sub get {
	my ($type,$top,$network,$ipaddr,$col,@args) = @_;
	my $self = $network->{nodes}->{$ipaddr} if $ipaddr;
	return $self if $self;
	$self = Node::get($type,$top,$network,$col,@args);
	if ($ipaddr) {
		$network->{nodes}->{$ipaddr} = $self;
		$self->{ipaddr} = $ipaddr;
		$self->{ipaddrs} = {$ipaddr=>1};
		$self->{iaddr} = Host::iaddr($ipaddr);
		$self->{iname} = Host::iname($ipaddr);
	} else {
		$self->{ipaddr} = undef;
		$self->{ipaddrs} = {};
		$self->{iaddr} = '';
		$self->{iname} = '';
	}
	$self->Logging::init($top,@args);
	$self->PairCollector::init($top,@args);
	if ($ipaddr) {
		$top->canvas->itemconfigure($self->{text},-text=>$self->{iaddr});
		$top->canvas->itemconfigure($self->{ownr},-text=>$self->{iname});
	}
	$top->{regroupnow} = 1;
	return $self;
}

#package Host;
sub addipaddr {
	my ($self,$top,$network,$ipaddr) = @_;
	if (my $host = $network->{nodes}->{$ipaddr}) {
		return $self->absorb($host);
	}
	$self->{ipaddrs}->{$ipaddr} = 1;
	$ipaddr = [sort {$a<=>$b} keys %{$self->{ipaddrs}}]->[0];
	if ($self->{ipaddr} != $ipaddr) {
		$self->{ipaddr} = $ipaddr;
		$self->{iaddr} = Host::iaddr($ipaddr);
		$self->{iname} = Host::iname($ipaddr);
	}
	return $self;
}

#package Host;
# call this as Host::makeone($node,$top,$network,$ipaddr);
sub makeone {
	my ($node,$top,$network,$ipaddr) = @_;
	if (ref $node eq 'Node') {
		$node->{ipaddr} = $ipaddr;
		$node->{ipaddrs}->{$ipaddr} = 1;
		$node->{iaddr} = Host::iaddr($ipaddr);
		$node->{iname} = Host::iname($ipaddr);
		SUPER::xform('Host',$node,$top);
	} else {
		print STDERR "E: $node: attempt to make a Host DENIED\n";
	}
	return $node;
}

#package Host;
sub mergefrom {
	my ($self,$top,$node) = @_;

	foreach (values %{$node->{ipaddrs}}) {
		$self->{ipaddres}->{$_} = 1;
	}
	foreach (qw/ipaddr ipaddrs iaddr iname/) {
		delete $node->{$_};
	}
	$self->addfrom($node,0);
	$self->SUPER::mergefrom($top,$node);
}

#package Host;
sub adjitems {
	my ($self,$top,$c,$oldtags) = @_;
	my ($x,$y) = ($self->{x},$self->{y});
	$self->{item} = $c->createRectangle(
		$x-40,$y-30,$x+40,$y+30,
		-fill=>'white',
		-outline=>'black', -width=>2,
		-activeoutline=>'cyan', -activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	unshift @{$self->{items}}, $self->{item};
	$c->coords($self->{scri},
		$x-23,$y-23,$x+23,$y-23,$x+23,$y+23,$x-23,$y+23,$x-23,$y-23,
	);
	$c->itemconfigure($self->{ttxt},-text=>ref($self));
	$c->itemconfigure($self->{text},-text=>$self->{iaddr});
	$c->itemconfigure($self->{ownr},-text=>$self->{iname});
	$self->makecol($top,::COL_ADJ);
}

#package Host;
sub iaddr {
	my $ipaddr = shift;
	return undef unless defined $ipaddr;
	return '' unless $ipaddr;
	return join('.',unpack('C*',pack('N',$ipaddr)));
}

#package Host;
sub iname {
	my $ipaddr = shift;
	return undef unless defined $ipaddr;
	return '' unless $ipaddr;
	# TODO: reverse DNS lookup of ip address
	return '(unknown)';
}

# -------------------------------------
package SP;
use strict;
use vars qw(@ISA);
@ISA = qw(Node Tristate Logging DualCollector);
# -------------------------------------

#package SP;
sub new {
	my ($type,$top,$nodeno,$pc,$half,@args) = @_;
	my $col = ::COL_NOD; $col = -$col if $half < 0;
	my $self = Node::new($type,$top,$nodeno,$col,@args);
	$self->Tristate::init($top,'white');
	$self->Logging::init($top,@args);
	$self->DualCollector::init($top,@args);
	$self->{pc} = $pc;
	$self->{tqueries} = {};
	$self->{circuits} = {};
	$self->{responds} = {};
	$self->{routes} = {}; # routes that term or orig here
	$self->{routes}->{a} = {}; # routes for which I am node-a (i.e. O/G route)
	$self->{routes}->{b} = {}; # routes for which I am node-b (i.e. I/C route)
	$self->{relate} = {}; # relations in which this is a node
	$self->{linksets} = {}; # linksets that attach here
	$self->{pcode} = SP::pcstring($pc);
	$self->{pownr} = $self->pcowner(0);
	my $c = $top->canvas;
	$c->itemconfigure($self->{ttxt},-text=>$type);
	$c->itemconfigure($self->{text},-text=>$self->{pcode});
	$c->itemconfigure($self->{ownr},-text=>$self->{pownr});
	#print STDERR "D: must regroup\n";
	$top->{regroupnow} = 1;
	#$top->regroup();
	return $self;
}

#package SP;
sub xform {
	my ($type,$self,$top) = @_;
	return if ref $self eq $type;
	my $oldtype = ref $self;
	bless $self,$type;
	my $c = $top->canvas;
	my $mc = $top->mycanvas;
	$mc->delballoon($self->{items});

	if ($node->isa('SP')) {
		print STDERR "E: $self and $node attempt to merge DENIED\n";
		return;
	}

	my @oldtags = ();
	push @oldtags, 'SLTM' if $self->{xchg_sltm};
	push @oldtags, 'ISUP' if $self->{xchg_isup};
	push @oldtags, 'TCAP' if $self->{orig_tcap} or $self->{term_tcap};
	push @oldtags, 'circuits' if $self->{ciccnt} > 0;

#package SP;
sub adjitems {
	my ($self,$top,$c,$oldtags) = @_;
	my ($x,$y) = ($self->{x},$self->{y});
	$self->{item} = $c->createOval(
		$x-40,$y-40,$x+40,$y+40,
		-fill=>'white',
		-outline=>'black', -width=>2,
		-activeoutline=>'cyan', -activeWidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	unshift @{$self->{items}}, $self->{item};
	$c->coords($self->{scri},
		$x-23,$y-23,$x+23,$y-23,$x+23,$y+23,$x-23,$y+23,$x-23,$y-23,
	);
	$c->itemconfigure($self->{ttxt},-text=>ref($self));
	$c->itemconfigure($self->{text},-text=>$self->{pcode});
	$c->itemconfigure($self->{ownr},-text=>$self->{pownr});
	$self->makecol($top,::COL_NOD);
}

#package SP;
sub identify {
	my $self = shift;
	my $ref = ref $self;
	my $id = $self->{alias} ? 'Alias ' : '';
	$id .= "$ref $self->{pcode}";
	$id .= " ($self->{pownr})" if $self->{pownr};
	return ($self->{id} = $id);
}

#package SP;
sub shortid {
	my $self = shift;
	my $ref = ref $self;
	return "$ref($self->{pcode})";
}

#package SP;
sub statechange {
	my ($self,$top,$val) = @_;
}

#package SP;
sub updatestate {
	my ($self,$top,$obj) = @_;
	my ($avail,$degra,$unava) = (0,0,0);
	foreach my $route (values %{$self->{routes}->{a}}) {
		my $rstate = $route->{state};
		if ($rstate == Tristate::TS_AVAILABLE) {
			$avail++;
		} elsif ($rstate == Tristate::TS_DEGRADED) {
			$degra++;
		} elsif ($rstate == Tristate::TS_UNAVAILABLE) {
			$unava++;
		}
	}
	foreach my $k (keys %{$self->{paths}}) {
		# maybe should just be the term paths ('b').
		foreach my $path (values %{$self->{paths}->{$k}}) {
			next if $path->{channel}->{link};
			my $rstate = $path->{state};
			if ($rstate == Tristate::TS_AVAILABLE) {
				$avail++;
			} elsif ($rstate == Tristate::TS_DEGRADED) {
				$degra++;
			} elsif ($rstate == Tristate::TS_UNAVAILABLE) {
				$unava++;
			}
		}
	}
	my ($state,$color);
	if ($avail == 0 && $degra == 0 && $unava >= 0) {
		$state = Tristate::TS_UNAVAILABLE;
	} elsif ($degra + $unava > 0) {
		$state = Tristate::TS_DEGRADED;
	} else {
		$state = Tristate::TS_AVAILABLE;
	}
	return if $self->{state} == $state;
	$self->{state} = $state;
}

#package SP;
sub pcstate {
	my ($self,$i) = @_;
	my $pc = $self->{pc};
	my ($ntw,$cls,$mem);
	$ntw = $pc >> 16;
	$cls = ($pc >> 8) & 0xff;
	$mem = $pc & 0xff;
	if ($ntw == 5) {
		if ($cls != 0) {
			my $cluster;
			if ($cluster = $pc_assignments->{$ntw}->{$cls}) {
				if (exists $cluster->{state}) {
					return $cluster->{state}->[$i];
				}
			}
		}
	}
	return '';
}

sub pctype {
	my ($self) = @_;
	my $pc = $self->{pc};
	my ($ntw,$cls,$mem);
	$ntw = $pc >> 16;
	$cls = ($pc >> 8) & 0xff;
	$mem = $pc & 0xff;
	if (5 < $ntw && $ntw < 255) {
		if ($cls != 0) {
			return 'large network code';
		}
		return "invalid cluster $cls";
	}
	if (1 <= $ntw && $ntw <= 4) {
		if ($cls != 0) {
			return 'small network code';
		}
		return "invalid cluster $cls";
	}
	if ($ntw == 5) {
		if ($cls != 0) {
			my $cluster;
			if ($cluster = $pc_assignments->{$ntw}->{$cls}) {
				if (exists $cluster->{state}) {
					if ($mem > 3) {
						return "point code block for $cluster->{state}->[1]";
					}
					return 'unused point code block 0 to '.($mem+1);
				} else {
					return "point code block";
				}
			}
			return 'no information for cluster '.$cls;
		}
		return "invalid cluster $cls";
	}
	return "invalid point code network $ntw";
}

sub pcowner {
	my ($self,$i) = @_;
	my $pc = $self->{pc};
	my ($ntw,$cls,$mem,$own);
	$ntw = $pc >> 16;
	$cls = ($pc >> 8) & 0xff;
	$mem = $pc & 0xff;
	if (5 < $ntw && $ntw < 255) {
		if ($cls != 0) {
			if ($own = $pc_assignments->{$ntw}) {
				return $own->[$i];
			}
			if ($i) {
				return 'Unknown large network';
			}
			return '';
		}
	}
	if (1 <= $ntw && $ntw <= 4) {
		if ($cls != 0) {
			my $small;
			if ($small = $pc_assignments->{$ntw}) {
				if ($own = $small->{$cls}) {
					return $own->[$i];
				}
			}
			if ($i) {
				return 'Unknown small network.';
			}
			return '';
		}
	}
	if ($ntw == 5) {
		if ($cls != 0) {
			my $cluster;
			if ($cluster = $pc_assignments->{$ntw}->{$cls}) {
				my $member = $mem & ~0x03;
#				if (exists $cluster->{state}) {
#					if ($mem > 3 && ($own = $cluster->{$member})) {
#						return "$own->[$i] $cluster->{state}->[$i]";
#					}
#				} else {
					if ($own = $cluster->{$member}) {
						return $own->[$i];
					}
#				}
			}
			if ($i) {
				return 'Unknown point code block.';
			}
			return '';
		}
	}
	if ($i) {
		return 'Invalid point code.';
	}
	return '';
}

sub pcstring {
	my ($pc) = @_;
	my ($ntw,$cls,$mem,$own);
	if ($pc & ~0x3fff) {
		$ntw = $pc >> 16;
		$cls = ($pc >> 8) & 0xff;
		$mem = $pc & 0xff;
	} else {
		$ntw = $pc >> 11;
		$cls = ($pc >> 3) & 0xff;
		$mem = $pc & 0x7;
	}
	return "$ntw-$cls-$mem";
}

#package SP;
sub add_orig {
	my ($self,$top,$network,$msg) = @_;
	$self->inc($msg,0);
	$self->pushmsg($msg);
	my $si = $msg->{si};
	my $mt = $msg->{mt};
	if ($si == 5) {
		if ($mt == 0x10 || (0x12 <= $mt && $mt <= 0x1b) || $mt == 0x2a || $mt == 0x2b) {
			unless ($self->{xchg_isup}) {
				$self->{xchg_isup} = 1;
				$self->{reanalyze} = 1;
			}
			$self->{circuits}->{$msg->{dpc}} = $network->{nodes}->{$msg->{dpc}};
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
	if ($si == 3) {
		if ($mt == 0x09 || $mt == 0x11 || $mt == 0x13) { # udt,xudt,ludt
			unless ($self->{orig_tcap}) {
				$self->{orig_tcap} = 1;
				$self->{reanalyze} = 1;
			}
			$self->{tqueries}->{$msg->{dpc}} = $network->{nodes}->{$msg->{dpc}};
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
	if ($si == 2 || $si == 1) {
		if ($mt == 0x11 || $mt == 0x12) { # sltm,slta
			unless ($self->{exch_sltm}) {
				$self->{exch_sltm} = 1;
				$self->{reanalyze} = 1;
			}
			$self->{adjacent}->{$msg->{dpc}} = $network->{nodes}->{$msg->{dpc}};
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
}

#package SP;
sub add_term {
	my ($self,$top,$network,$msg) = @_;
	$self->inc($msg,1);
	$self->pushmsg($msg);
	my $si = $msg->{si};
	my $mt = $msg->{mt};
	if ($si == 5) {
		if ($mt == 0x10 || (0x12 <= $mt && $mt <= 0x1b) || $mt == 0x2a || $mt == 0x2b) {
			unless ($self->{xchg_isup}) {
				$self->{xchg_isup} = 1;
				$self->{reanalyze} = 1;
			}
			$self->{circuits}->{$msg->{opc}} = $network->{nodes}->{$msg->{opc}};
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
	if ($si == 3) {
		if ($mt == 0x09 || $mt == 0x11 || $mt == 0x13) { # udt,xudt,ludt
			unless ($self->{term_tcap}) {
				$self->{term_tcap} = 1;
				$self->{reanalyze} = 1;
			}
			$self->{responds}->{$msg->{opc}} = $network->{nodes}->{$msg->{opc}};
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
	if ($si == 2 || $si == 1) {
		if ($mt == 0x11 || $mt == 0x12) { # sltm,slta
			unless ($self->{xchg_sltm}) {
				$self->{xchg_sltm} = 1;
				$self->{reanalyze} = 1;
			}
			$self->{adjacent}->{$msg->{opc}} = $network->{nodes}->{$msg->{opc}};
		}
		$self->reanalyze($top,$network) if $self->{reanalyze};
		return;
	}
}

#package SP;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	if ($dir == 1) {
		$self->add_term($top,$network,$msg);
	} else {
		$self->add_orig($top,$network,$msg);
	}
}

#package SP;
sub reanalyze {
	my ($self,$top,$network) = @_;
	my $c = $top->canvas;
	if ($self->{xchg_isup}) {
		foreach (@{$self->{items}}) { $c->addtag('ISUP',withtag=>$_); }
	}
	if ($self->{orig_tcap} or $self->{term_tcap}) {
		foreach (@{$self->{items}}) { $c->addtag('TCAP',withtag=>$_); }
	}
	if ($self->{xchg_sltm}) {
		foreach (@{$self->{items}}) { $c->addtag('SLTM',withtag=>$_); }
	}
	if ($self->{xchg_sltm}) {
		$self->makecol($top,::COL_ADJ);
		$self->findaliases($top);
	} elsif ($self->{xchg_isup} || ($self->{orig_tcap} && $self->{term_tcap})) {
		$self->makecol($top,::COL_SSP);
	} elsif ($self->{orig_tcap} && !$self->{term_tcap}) {
		$self->makecol($top,::COL_SCP);
	} elsif (!$self->{orig_tcap} && $self->{term_tcap}) {
		$self->makecol($top,::COL_GTT);
	} 
	if ($self->{xchg_isup} || ($self->{orig_tcap} && $self->{term_tcap})) {
		SP::xform('SSP',$self,$top);
	} elsif ($self->{orig_tcap} && !$self->{term_tcap}) {
		SP::xform('SCP',$self,$top);
	} elsif (!$self->{orig_tcap} && $self->{term_tcap}) {
		SP::xform('GTT',$self,$top);
	} elsif ($self->{xchg_sltm}) {
		SP::xform('STP',$self,$top);
	}
	if ($self->{xchg_isup}) {
		if ($self->{orig_tcap}) {
			if ($self->{term_tcap}) {
				if ($self->{xchg_sltm}) {
				} else {
				}
			} else {
				if ($self->{xchg_sltm}) {
				} else {
				}
			}
		} else {
			if ($self->{term_tcap}) {
				if ($self->{xchg_sltm}) {
				} else {
				}
			} else {
				if ($self->{xchg_sltm}) {
				} else {
				}
			}
		}
	} else {
		if ($self->{orig_tcap}) {
			if ($self->{term_tcap}) {
				if ($self->{xchg_sltm}) {
				} else {
				}
			} else {
				# SCP's originate but never terminate
				if ($self->{xchg_sltm}) {
				} else {
				}
			}
		} else {
			if ($self->{term_tcap}) {
				# GTT functions never originate, just terminate
				if ($self->{xchg_sltm}) {
				} else {
				}
			} else {
				if ($self->{xchg_sltm}) {
				} else {
				}
			}
		}
	}
	delete $self->{reanalyze};
}

#package SP;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $have = {};
	$have->{primary} = $self->{primary} ? 1 : 0;
	$have->{aliases}  = scalar keys %{$self->{aliases}};
	$have->{adjacent} = scalar keys %{$self->{adjacent}};
	$have->{combined} = scalar keys %{$self->{combined}};
	$have->{linksets} = scalar keys %{$self->{linksets}};
	$have->{routes}   = scalar keys %{$self->{routes}->{a}};
	$have->{relate}   = scalar keys %{$self->{relate}};
	$have->{circuits} = scalar keys %{$self->{circuits}};
	$m->add('separator') if
		$have->{primary} +
		$have->{aliases} +
		$have->{adjacent} +
		$have->{combined} +
		$have->{linksets} +
		$have->{routes} +
		$have->{relate};
	if ($have->{primary}) {
		my $node = $self->{primary};
		my $label = 'Primary '.$node->shortid;
		my $mc = $m->Menu(-title=>"$label Menu");
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			$node->getmenu($mc,$top,$X,$Y);
			$node->getmore($mc,$top,$X,$Y);
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>$label);
	}
	if ($have->{aliases}) {
		my $mc = $m->Menu(-title=>'Alias Point Codes Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $pc (sort {$a <=> $b} keys %{$self->{aliases}}) {
				my $node = $self->{aliases}->{$pc};
				my $label = 'Alias '.$node->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$node->getmenu($m3,$top,$X,$Y);
				$node->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Alias Point Codes');
	}
	if ($have->{adjacent}) {
		my $mc = $m->Menu(-title=>'Adjacent Nodes Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $pc (sort {$a <=> $b} keys %{$self->{adjacent}}) {
				my $node = $self->{adjacent}->{$pc};
				my $label = 'Adjacent '.$node->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$node->getmenu($m3,$top,$X,$Y);
				$node->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Adjacent Nodes');
	}
	if ($have->{combined}) {
		my $mc = $m->Menu(-title=>'Combined Linksets Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $key (sort keys %{$self->{combined}}) {
				my $combined = $self->{combined}->{$key};
				my $nodea = $combined->{node}->{a};
				my $nodeb = $combined->{node}->{b};
				my $label = 'Combined Linkset to '.$nodea->shortid.' and '.$nodeb->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$combined->getmenu($m3,$top,$X,$Y);
				$combined->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Combined Linksets');
	}
	if ($have->{linksets}) {
		my $mc = $m->Menu(-title=>'Linksets Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $pc (sort {$a <=> $b} keys %{$self->{linksets}}) {
				my $linkset = $self->{linksets}->{$pc};
				my $node;
				if ($linkset->{node}->{a}->{pc} == $self->{pc}) {
					$node = $linkset->{node}->{b};
				} else {
					$node = $linkset->{node}->{a};
				}
				my $label = 'Linkset to '.$node->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$linkset->getmenu($m3,$top,$X,$Y);
				$linkset->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Linksets');
	}
	if ($have->{routes}) {
		my $mc = $m->Menu(-title=>'Routesets Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $pc (sort {$a <=> $b} keys %{$self->{routes}->{a}}) {
				my $route = $self->{routes}->{a}->{$pc};
				my $node = $route->{node}->{b};
				my $label = 'Route to '.$node->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$route->getmenu($m3,$top,$X,$Y);
				$route->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Routesets');
	}
	if ($have->{relate}) {
		my $mc = $m->Menu(-title=>'Relations Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $pc (sort {$a <=> $b} keys %{$self->{relate}}) {
				my $relation = $self->{relate}->{$pc};
				my $node;
				if ($relation->{node}->{a}->{pc} == $self->{pc}) {
					$node = $relation->{node}->{b};
				} else {
					$node = $relation->{node}->{a};
				}
				my $label = 'Routset to '.$node->shortid;
				my $m3 = $mc->Menu( -title=>"$label Menu");
				$relation->getmenu($m3,$top,$X,$Y);
				$relation->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Relations');
	}
}

#package SP;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;

	my ($rt,$rttext);
	if ($self->{pc} < (1<<14)) {
		$rt = ::RT_14BIT_PC;
		$rttext = $rtoptions[1]->[0];
	} else {
		$rt = ::RT_24BIT_PC;
		$rttext = $rtoptions[2]->[0];
	}
	$tw->Label(%labelright,
		-text=>'Point code type:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Optionmenu(%optionleft,
		-options=>\@rtoptions,
		-variable=>\$rt,
		-textvariable=>\$rttext, -width=>12,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'Point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-textvariable=>\$self->{pcode}, -width=>12,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'Point code type:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pctyp = $self->pctype();
	$tw->Entry(%entryinput,
		-textvariable=>\$pctyp, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'Point code owner:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $lownr = $self->pcowner(1);
	$tw->Entry(%entryinput,
		-textvariable=>\$lownr, -width=>40,
	)->grid(-row=>$$row,-column=>1,-sticky=>'wns');
	$tw->Entry(%entryinput,
		-textvariable=>\$self->{pownr}, -width=>16,
	)->grid(-row=>$$row++,-column=>2,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'Point code location:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pcloc = $self->pcstate(1);
	$tw->Entry(%entryinput,
		-textvariable=>\$pcloc, -width=>32,
	)->grid(-row=>$$row,-column=>1,-sticky=>'wns');
	my $pcsta = $self->pcstate(0);
	$tw->Entry(%entryinput,
		-textvariable=>\$pcsta, -width=>16,
	)->grid(-row=>$$row++,-column=>2,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'CLLI:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryinput,
		-textvariable=>\$self->{clli}, -width=>24,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
}

#package SP;
sub fillstatus {
	my ($self,$top,$tw,$row,$ptxt,$atxt,$ltxt,$ctxt,$qtxt,$rtxt) = @_;
	my $f;

	$ptxt = 'Primary'  unless defined $ptxt;
	$atxt = 'Aliases'  unless defined $atxt;
	$ltxt = 'Linksets' unless defined $ltxt;
	$ctxt = 'Circuits' unless defined $ctxt;
	$qtxt = 'Queries'  unless defined $qtxt;
	$rtxt = 'Replies'  unless defined $rtxt;

	$f = $tw->Frame(%tframestyle,
	)->pack(%tframescrunch);
	$f->Checkbutton(%buttonleft,
		-text=>'Exchange SLTM',
		-variable=>\$self->{xchg_sltm},
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$f->Checkbutton(%buttonleft,
		-text=>'Exchange ISUP',
		-variable=>\$self->{xchg_isup},
	)->grid(-row=>$$row,-column=>1,-sticky=>'ewns');
	$f->Checkbutton(%buttonleft,
		-text=>'Originate TCAP',
		-variable=>\$self->{orig_tcap},
	)->grid(-row=>$$row,-column=>2,-sticky=>'ewns');
	$f->Checkbutton(%buttonleft,
		-text=>'Terminate TCAP',
		-variable=>\$self->{term_tcap},
	)->grid(-row=>$$row++,-column=>3,-sticky=>'ewns');
	$f = $tw->TFrame(%tframestyle,
		-label=>'State:',
	)->pack(%tframescrunch);
	$$row = 0;
	$self->fillstate($top,$f,$row);
	if ($self->{primary} or keys %{$self->{aliases}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Aliases:',
		)->pack(%tframescrunch);
		$$row = 0;
		my $col = 0;
		my $node;
		if ($node = $self->{primary}) {
			$f->Label(%labelright,-text=>"$ptxt:",
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			my $w = $f->Entry(%entrycenter,-text=>$node->shortid,
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
			$node->bindtowidget($top,$w);
		}
		foreach my $pc (sort {$a <=> $b} keys %{$self->{aliases}}) {
			$f->Label(%labelright,-text=>"$atxt:",
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns') if $col == 0; $col++;
			$node = $self->{aliases}->{$pc};
			my $w = $f->Entry(%entrycenter,-text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$node->bindtowidget($top,$w);
			if ($col > 4) { $col = 0; $$row++; }
		}
	}
	if (keys %{$self->{adjacent}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Adjacent:')->pack(%tframescrunch);
		$$row = 0;
		my $col = 0;
		foreach my $pc (sort {$a <=> $b} keys %{$self->{adjacent}}) {
			$f->Label(%labelright,-text=>"Adjacent:",
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns') if $col == 0; $col++;
			my $node = $self->{adjacent}->{$pc};
			my $w = $f->Entry(%entrycenter,-text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$node->bindtowidget($top,$w);
			if ($col > 4) { $col = 0; $$row++; }
		}
	}
	if (keys %{$self->{linksets}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Linksets:')->pack(%tframescrunch);
		$$row = 0;
		my $col = 0;
		foreach my $pc (sort {$a <=> $b} keys %{$self->{linksets}}) {
			$f->Label(%labelright,-text=>"$ltxt:",
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns') if $col == 0; $col++;
			my $linkset = $self->{linksets}->{$pc};
			my $node;
			if ($self->{pc} == $linkset->{node}->{a}->{pc}) {
				$node = $linkset->{node}->{b};
			} else {
				$node = $linkset->{node}->{a};
			}
			my $w = $f->Entry(%entrycenter,-text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$linkset->bindtowidget($top,$w);
			if ($col > 4) { $col = 0; $$row++; }
		}
	}
	if (keys %{$self->{routes}->{a}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Routes:');
		my $p = $f;
		if (keys %{$self->{routes}->{a}} > 16) {
			$f->pack(%tframepack);
			$p = $f->Scrolled('Pane',
				-scrollbars=>'osoe',
				-sticky=>'we',
				-gridded=>'y',
			)->pack(%tframepack);
		} else {
			$f->pack(%tframescrunch);
		}
		$$row = 0;
		my $col = 0;
		$p->Label(%labelright,-text=>'Routes to:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{routes}->{a}}) {
			my $route = $self->{routes}->{a}->{$pc};
			my $node = $route->{node}->{b};
			$col++;
			my $w = $p->Entry(%entrycenter,-text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$route->bindtowidget($top,$w);
			if ($col > 3) { $col = 0; $$row++; }
		}
	}
	if (keys %{$self->{relate}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Routesets:');
		my $p = $f;
		if (keys %{$self->{relate}} > 16) {
			$f->pack(%tframepack);
			$p = $f->Scrolled('Pane',
				-scrollbars=>'osoe',
				-sticky=>'we',
				-gridded=>'y',
			)->pack(%tframepack);
		} else {
			$f->pack(%tframescrunch);
		}
		$$row = 0;
		my $col = 0;
		$p->Label(%labelcenter,-text=>'Routesets to:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{relate}}) {
			my $relation = $self->{relate}->{$pc};
			my $node;
			if ($relation->{node}->{a}->{pc} == $self->{pc}) {
				$node = $relation->{node}->{b};
			} else {
				$node = $relation->{node}->{a};
			}
			$col++;
			my $w = $p->Entry(%entrycenter, -text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$relation->bindtowidget($top,$w);
			if ($col > 3) { $col = 0; $$row++; }
		}
	}
	if (keys %{$self->{circuits}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Circuits:');
		my $p = $f;
		if (keys %{$self->{circuits}} > 4) {
			$f->pack(%tframepack);
			$p = $f->Scrolled('Pane',
				-scrollbars=>'osoe',
				-sticky=>'we',
				-gridded=>'y',
			)->pack(%tframepack);
		} else {
			$f->pack(%tframescrunch);
		}
		$$row = 0;
		$p->Label(%labelcenter, -text=>'Point Code',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$p->Label(%labelcenter, -text=>'Alloc.',
		)->grid(-row=>$$row,-column=>1,-sticky=>'ewns');
		$p->Label(%labelcenter, -text=>'Act.',
		)->grid(-row=>$$row,-column=>2,-sticky=>'ewns');
		$p->Label(%labelcenter, -text=>'O/G',
		)->grid(-row=>$$row,-column=>3,-sticky=>'ewns');
		$p->Label(%labelcenter, -text=>'I/C',
		)->grid(-row=>$$row,-column=>4,-sticky=>'ewns');
		$p->Label(%labelcenter, -text=>'2/W',
		)->grid(-row=>$$row++,-column=>5,-sticky=>'ewns');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{circuits}}) {
			my $node = $self->{circuits}->{$pc};
			my $w = $p->Entry(%entrycenter, -text=>$node->shortid,
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$node->bindtowidget($top,$w);
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{ciccnt},
			)->grid(-row=>$$row,-column=>1,-sticky=>'ewns');
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{actcnt}->{0},
			)->grid(-row=>$$row,-column=>2,-sticky=>'ewns');
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{actcnt}->{1},
			)->grid(-row=>$$row,-column=>3,-sticky=>'ewns');
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{actcnt}->{2},
			)->grid(-row=>$$row,-column=>4,-sticky=>'ewns');
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{actcnt}->{3},
			)->grid(-row=>$$row++,-column=>5,-sticky=>'ewns');
		}
	}
	if (keys %{$self->{tqueries}} || keys %{$self->{responds}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Queries:',
		)->pack(%tframescrunch);
		$$row = 0;
		my $col = 0;
		foreach my $pc (sort {$a <=> $b} keys %{$self->{tqueries}}) {
			$f->Label(%labelright, -text=>"$qtxt:",
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns') if $col == 0; $col++;
			my $node = $self->{tqueries}->{$pc};
			my $w = $f->Entry(%entrycenter, -text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$node->bindtowidget($top,$w);
			if ($col > 4) { $col = 0; $$row++; }
		}
		if ($col != 0) {
			$col = 0;
			$$row++;
		}
		foreach my $pc (sort {$a <=> $b} keys %{$self->{responds}}) {
			$f->Label(%labelright, -text=>"$rtxt:",
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns') if $col == 0; $col++;
			my $node = $self->{responds}->{$pc};
			my $w = $f->Entry(%entrycenter, -text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$node->bindtowidget($top,$w);
			if ($col > 4) { $col = 0; $$row++; }
		}
	}
	$self->Node::fillstatus($top,$tw,$row);
}

# -------------------------------------
package SSP;
use strict;
use vars qw(@ISA);
@ISA = qw(SP);
# -------------------------------------

sub adjitems {
	my ($self,$c,$oldtags) = @_;
	my ($x,$y) = ($self->{x},$self->{y});
	$self->{item} = $c->createOval(
		$x-40,$y-40,$x+40,$y+40,
		-fill=>'white',
		-outline=>'black', -width=>2,
		-activeoutline=>'cyan', -activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	unshift @{$self->{items}}, $self->{item};
	$c->coords($self->{scri},
		$x-26,$y-26,$x+26,$y+26,$x+26,$y-26,$x-26,$y+26,$x-26,$y-26,
	);
	$c->itemconfigure($self->{ttxt},-text=>ref($self));
	$c->itemconfigure($self->{text},-text=>$self->{pcode});
	$c->itemconfigure($self->{ownr},-text=>$self->{pownr});
	$self->makecol($top,::COL_SSP);
}

#package SSP;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
	$self->SP::fillstatus($top,$tw,$row,
		'Alias of',
		'Primary for',
		'Linksets to',
		'Circuits to',
		'Query to',
		'Reply from',
	);
}

# -------------------------------------
package SCP;
use strict;
use vars qw(@ISA);
@ISA = qw(SP);
# -------------------------------------

sub adjitems {
	my ($self,$c,$oldtags) = @_;
	my ($x,$y) = ($self->{x},$self->{y});
	$self->{item} = $c->createOval(
		$x-40,$y-30,$x+40,$y+30,
		-fill=>'white',
		-outline=>'black', -width=>2,
		-activeoutline=>'cyan', -activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	unshift @{$self->{items}}, $self->{item};
	$c->coords($self->{scri},
		$x-23,$y-23,$x+23,$y-23,$x+23,$y+23,$x-23,$y+23,$x-23,$y-23,
	);
	$c->itemconfigure($self->{ttxt},-text=>ref($self));
	$c->itemconfigure($self->{text},-text=>$self->{pcode});
	$c->itemconfigure($self->{ownr},-text=>$self->{pownr});
	$self->makecol($top,::COL_SCP);
}

#package SCP;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
	$self->SP::fillstatus($top,$tw,$row,
		'Alias of',
		'Primary for',
		'Linksets to',
		'Circuits to',
		'Responds to',
		'Responds to',
	);
}

# -------------------------------------
package STP;
use strict;
use vars qw(@ISA);
@ISA = qw(SP);
# -------------------------------------

sub adjitems {
	my ($self,$c,$oldtags) = @_;
	my ($x,$y) = ($self->{x},$self->{y});
	$self->{item} = $c->createRectangle(
		$x-38,$y-38,$x+38,$y+38,
		-fill=>'white',
		-outline=>'black', -width=>2,
		-activeoutline=>'cyan', -activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	unshift @{$self->{items}}, $self->{item};
	$c->coords($self->{scri},
		$x+38,$y-38,$x-38,$y+38,
	);
	$c->itemconfigure($self->{ttxt},-text=>ref($self));
	$c->itemconfigure($self->{text},-text=>$self->{pcode});
	$c->itemconfigure($self->{ownr},-text=>$self->{pownr});
	$self->makecol($top,::COL_ADJ);
}

#package STP;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
	$self->SP::fillstatus($top,$tw,$row,
		'Alias of',
		'Primary for',
		'Linksets to',
		'Circuits to',
		'Queries',
		'Translates from',
	);
}

# -------------------------------------
package GTT;
use strict;
use vars qw(@ISA);
@ISA = qw(SP);
# -------------------------------------

sub adjitems {
	my ($self,$c,$oldtags) = @_;
	my ($x,$y) = ($self->{x},$self->{y});
	$self->{alias} = 1;
	$self->{item} = $c->createRectangle(
		$x-38,$y-38,$x+38,$y+38,
		-fill=>'white',
		-outline=>'black', -width=>2,
		-activeoutline=>'cyan', -activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	unshift @{$self->{items}}, $self->{item};
	$c->coords($self->{scri},
		$x+38,$y-38,$x-38,$y+38,
	);
	$c->itemconfigure($self->{ttxt},-text=>ref($self));
	$c->itemconfigure($self->{text},-text=>$self->{pcode});
	$c->itemconfigure($self->{ownr},-text=>$self->{pownr});
	$self->makecol($top,::COL_GTT);
}

#package GTT;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;
	$self->SP::fillstatus($top,$tw,$row,
		'Alias of',
		'Primary for',
		'Linksets to',
		'Circuits to',
		'Queries',
		'Translates from',
	);
}

# -------------------------------------
package Network;
use strict;
use vars qw(@ISA);
@ISA = qw(Logging Properties Status Clickable DualCollector);
# -------------------------------------

my $network;

#package Network;
sub new {
	my ($type,$top,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->{top} = $top;
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->DualCollector::init($top,@args);
	$self->{msgnum} = 0;
	$self->{nodes} = {};
	$self->{nodeno} = 0;
	$self->{channels} = {};
	$self->{channelno} = 0;
	$self->{relations} = {};
	$self->{relationno} = 0;
	my $c = $top->canvas;
	$c->CanvasBind('<ButtonPress-3>',[\&Network::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y'),Tk::Ev('x'),Tk::Ev('y')]);
	$Network::network = $self;
	#print STDERR "D: discovered ".$self->identify."\n";
	$top->setstatus("Discovered ".$self->identify);
	return $self;
}

#package Network;
sub identify {
	my $self = shift;
	my $id = 'Newtork';
	return ($self->{id} = $id);
}

#package Network;
sub shortid {
	my $self = shift;
	return "NET";
}

#package Network;
sub getChannel {
	my ($self,$top,$ppa,$dir,$xsn,@args) = @_;
	return $self->{channels}->{$ppa} if $self->{channels}->{$ppa};
	my $channelno = $self->{channelno} + 1;
	my $loc = ::COL_ADJ;
	my $nodea = $self->getNode($top,-$loc);
	my $nodeb = $self->getNode($top, $loc);
	my $channel = Channel->new($top,$self,$channelno,$ppa,$nodea,$nodeb,$dir,$xsn,@args);
	$self->{channels}->{$ppa} = $channel;
	$self->{channelno} = $channelno;
	return $channel;
}

#package Network;
sub getPath {
	my ($self,$top,$channel,$side,$node,@args) = @_;
	my $key = "$channel->{ppa}($side)$node->{pc}";
	return $self->{paths}->{$key} if $self->{paths}->{$key};
	my $pathno = $self->{pathno} + 1;
	my $path = Path->new($top,$pathno,$channel,$side,$node,@args);
	$self->{paths}->{$key} = $path;
	$self->{pathno} = $pathno;
	return $path;
}

#package Network;
sub getNode {
	my ($self,$top,$half,@args) = @_;
	my $nodeno = $self->{nodeno} + 1;
	my $node = Node->new($top,$nodeno,$half,@args);
	$self->{pnodes}->{$nodeno} = $node;
	$self->{nodeno} = $nodeno;
	return $node;
}

#package Network;
sub getSp {
	my ($self,$top,$pc,@args) = @_;
	return $self->{nodes}->{$pc} if $self->{nodes}->{$pc};
	my $nodeno = $self->{nodeno} + 1;
	my $node = SP->new($top,$nodeno,$pc,@args);
	$self->{nodes}->{$pc} = $node;
	$self->{nodeno} = $nodeno;
	return $node;
}

#package Network;
sub getRelation {
	my ($self,$top,$nodea,$nodeb,@args) = @_;
	my $key = "$nodea->{pc},$nodeb->{pc}";
	return $self->{relations}->{$key} if $self->{relations}->{$key};
	my $relationno = $self->{relationno} + 1;
	my $relation = Relation->new($top,$relationno,$nodea,$nodeb,@args);
	$self->{relations}->{$key} = $relation;
	$key = "$nodeb->{pc},$nodea->{pc}";
	$self->{relations}->{$key} = $relation;
	$self->{relationno} = $relationno;
	return $relation;
}

#package Network;
sub getRouteset {
	my ($self,$top,$channel,$nodea,$nodeb,@args) = @_;
	my $key = "$nodea->{pc},$nodeb->{pc}";
	return $self->{routesets}->{$key} if $self->{routesets}->{$key};
	my $routesetno = $self->{routesetno} + 1;
	my $routeset = Routeset->new($top,$self,$routesetno,$nodea,$nodeb,@args);
	$self->{routesets}->{$key} = $routeset;
	$self->{routesetno} = $routesetno;
	return $routeset;
}

#package Network;
sub getRoute {
	my ($self,$top,$linkset,$side,$nodeb,@args) = @_;
	my $nodea = $linkset->{node}->{$side};
	my $key = "$nodea->{pc},$nodeb->{pc}";
	return $self->{routes}->{$key} if $self->{routes}->{$key};
	my $routeno = $self->{routeno} + 1;
	my $route = Route->new($top,$routeno,$linkset,$side,$nodeb,@args);
	$self->{routes}->{$key} = $route;
	$self->{routeno} = $routeno;
	return $route;
}

#package Network;
sub getCombined {
	my ($self,$top,$linkseta,$linksetb,@args) = @_;
	my $key = "$linkseta->{key}::$linksetb->{key}";
	return $self->{combineds}->{$key} if $self->{combined}->{$key};
	my $combinedno = $self->{combinedno} + 1;
	my $combined = Combined->new($top,$combinedno,$linkseta,$linksetb,@args);
	$combined->{key} = $key;
	$self->{combineds}->{$key} = $combined;
	$key = "$linksetb->{key}::$linkseta->{key}";
	$self->{combineds}->{$key} = $combined;
	$self->{combinedno} = $combinedno;
	return $combined;
}

#package Network;
sub getLinkset {
	my ($self,$top,$nodea,$nodeb,@args) = @_;
	my $key = "$nodea->{pc},$nodeb->{pc}";
	return $self->{linksets}->{$key} if $self->{linksets}->{$key};
	my $linksetno = $self->{linksetno} + 1;
	my $relation = $self->getRelation($top,$nodea,$nodeb,@args);
	my $linkset = Linkset->new($top,$linksetno,$relation,@args);
	$nodea->{linksets}->{$nodeb->{pc}} = $linkset;
	$nodeb->{linksets}->{$nodea->{pc}} = $linkset;
	$linkset->{key} = $key;
	$relation->{linkset} = $linkset;
	$self->{linksets}->{$key} = $linkset;
	$key = "$nodeb->{pc},$nodea->{pc}";
	$self->{linksets}->{$key} = $linkset;
	$self->{linksetno} = $linksetno;
	return $linkset;
}

#package Network;
sub getLink {
	my ($self,$top,$nodea,$nodeb,$slc,@args) = @_;
	my $key = "$nodea->{pc},$nodeb->{pc}:$slc";
	return $self->{links}->{$key} if $self->{links}->{$key};
	my $linkno = $self->{linkno} + 1;
	my $linkset = $self->getLinkset($top,$nodea,$nodeb,@args);
	my $link = Link->new($top,$linkno,$linkset,$slc,@args);
	$self->{links}->{$key} = $link;
	$key = "$nodeb->{pc},$nodea->{pc}:$slc";
	$self->{links}->{$key} = $link;
	$self->{linkno} = $linkno;
	return $link;
}

#package Network;
sub add_msg {
	my ($self,$top,$msg) = @_;
	$self->inc($msg,2);
	$self->{msgnum}++;
	$msg->{msgnum} = $self->{msgnum};
	$self->pushmsg($msg);
}

#package Network;
sub button3 {
	my ($canvas,$self,$top,$X,$Y,$x,$y) = @_;
	my $cx = $canvas->canvasx($x,1);
	my $cy = $canvas->canvasy($y,1);
	if ($canvas->find('overlapping', $cx - 2, $cy - 2, $cx + 2, $cy + 2)) {
		return;
	}
	return shift->Clickable::button3(@_);
}

#package Network;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;
}

sub cmpkeys {
	my ($j,$k) = @_;
	my ($ja,$jb) = split(/,/,$j);
	my ($ka,$kb) = split(/,/,$k);
	return -1 if ($ja < $ka);
	return  1 if ($ja > $ka);
	return -1 if ($jb < $kb);
	return  1 if ($jb > $kb);
	return 0;
}

sub cmppaths {
	my ($j,$k) = @_;
	my ($ja,$jb) = split(/[(]/,$j); my ($jb,$jc) = split(/[)]/,$jb);
	my ($ka,$kb) = split(/[(]/,$k); my ($kb,$kc) = split(/[)]/,$kb);
	return -1 if ($ja < $ka);
	return  1 if ($ja > $ka);
	return -1 if ($jc < $kc);
	return  1 if ($jc > $kc);
	return -1 if ($jb lt $kb);
	return  1 if ($jb gt $kb);
	return 0;
}

#package Network;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $have = {};
	$have->{channels}  = scalar keys %{$self->{channels}};
	$have->{paths}     = scalar keys %{$self->{paths}};
	$have->{pnodes}	   = scalar keys %{$self->{pnodes}};
	$have->{nodes}     = scalar keys %{$self->{nodes}};
	$have->{relations} = scalar keys %{$self->{relations}};
	$have->{routesets} = scalar keys %{$self->{routesets}};
	$have->{routes}    = scalar keys %{$self->{routes}};
	$have->{combined}  = scalar keys %{$self->{combined}};
	$have->{linksets}  = scalar keys %{$self->{linksets}};
	$have->{links}     = scalar keys %{$self->{links}};
	$m->add('separator') if
		$have->{channels} +
		$have->{paths} +
		$have->{pnodes} +
		$have->{nodes} +
		$have->{relations} +
		$have->{routesets} +
		$have->{routes} +
		$have->{combined} +
		$have->{linksets} +
		$have->{links};
	if ($have->{channels}) {
		my $mc = $m->Menu(-title=>'Channels Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $ppa (sort {$a <=> $b} keys %{$self->{channels}}) {
				my $channel = $self->{channels}->{$ppa};
				next unless $channel;
				my $label = 'Channel '.$channel->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$channel->getmenu($m3,$top,$X,$Y);
				$channel->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Channels');
	}
	if ($have->{paths}) {
		my $mc = $m->Menu(-title=>'Paths Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $key (sort {Network::cmppaths($a,$b)} keys %{$self->{paths}}) {
				my $path = $self->{paths}->{$key};
				next unless $path;
				my $label = ref($path).' '.$path->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$path->getmenu($m3,$top,$X,$Y);
				$path->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Paths');
	}
	if ($have->{nodes}) {
		my $mc = $m->Menu(-title=>'Nodes Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $pc (sort {$a <=> $b} keys %{$self->{nodes}}) {
				my $node = $self->{nodes}->{$pc};
				next unless $node;
				my $label = ref($node).' '.$node->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$node->getmenu($m3,$top,$X,$Y);
				$node->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Nodes');
	}
	if ($have->{relations}) {
		my $mc = $m->Menu(-title=>'Relations Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $key (sort {Network::cmpkeys($a,$b)} %{$self->{relations}}) {
				my $relation = $self->{relations}->{$key};
				next unless $relation;
				next unless $relation->{key} eq $key;
				my $label = ref($relation).' '.$relation->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$relation->getmenu($m3,$top,$X,$Y);
				$relation->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Relations');
	}
	if ($have->{routesets}) {
		my $mc = $m->Menu(-title=>'Routesets Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $key (sort {Network::cmpkeys($a,$b)} %{$self->{routesets}}) {
				my $routeset = $self->{routesets}->{$key};
				next unless $routeset;
				my $label = ref($routeset).' '.$routeset->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$routeset->getmenu($m3,$top,$X,$Y);
				$routeset->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Routesets');
	}
	if ($have->{routes}) {
		my $mc = $m->Menu(-title=>'Routes Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $key (sort {Network::cmpkeys($a,$b)} %{$self->{routes}}) {
				my $route = $self->{routes}->{$key};
				next unless $route;
				my $label = ref($route).' '.$route->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$route->getmenu($m3,$top,$X,$Y);
				$route->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Routes');
	}
	if ($have->{linksets}) {
		my $mc = $m->Menu(-title=>'Linksets Menu');
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			foreach my $key (sort {Network::cmpkeys($a,$b)} %{$self->{linksets}}) {
				my $linkset = $self->{linksets}->{$key};
				next unless $linkset;
				next unless $linkset->{key} eq $key;
				my $label = ref($linkset).' '.$linkset->shortid;
				my $m3 = $mc->Menu(-title=>"$label Menu");
				$linkset->getmenu($m3,$top,$X,$Y);
				$linkset->getmore($m3,$top,$X,$Y);
				$mc->add('cascade',-menu=>$m3,-label=>$label);
			}
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>'Linksets');
	}
}

#package Network;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Number of channels:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{channelno},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Number of nodes:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{nodeno},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Number of signalling relations:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{relationno},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Number of circuits:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{ciccnt},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Number of active circuits:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{actcnt}->{0},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Number of active one-way circuits:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{actcnt}->{1},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Number of active both-way circuits:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{actcnt}->{2},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

# -------------------------------------
package Channel;
use strict;
use vars qw(@ISA);
@ISA = qw(Arcitem Pentastate Logging Properties Status Clickable MsgCollector MsgBuffer);
# -------------------------------------

use constant {
	MTP2_ANNEX_A_NOT_USED	    => 0,
	MTP2_ANNEX_A_USED	    => 1,
	MTP2_ANNEX_A_USED_UNKNOWN   => 2,
};

#package Channel;
sub init {
	my ($self,$top,$network,$channelno,$ppa,$nodea,$nodeb,$dir,$xsn,@args) = @_;
	$self->Pentastate::init($top,'black');
	$self->Logging::init($top,@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init($top,@args);
	$self->MsgBuffer::init(@args);
	$self->{no} = $channelno;
	$self->{ppa} = $ppa;
	my ($card,$span,$slot) = Channel::chan($ppa);
	#print STDERR "D: creating channel\n";
	#print STDERR "D: creation message direction is ", $dir & 0x1 ? 'received' : 'transmitted', "\n";
	#print STDERR "D: will swap roles of ",$nodea->shortid," and ",$nodeb->shortid,"\n";
	if ($dir & 0x2) {
		# monitored half-duplex channel in a known direction
		#print STDERR "D: channel is a monitored half-duplex channel in a known direction\n";
		my $ppa2 = Channel::ppa($card,$span^0x1,$slot);
		# tell messages from the other span how to find us
		$network->{channels}->{$ppa2} = $self;
		$self->{ppa} = $ppa2 unless $dir & 0x1;
	} elsif ($dir & 0x4) {
		# monitored half-duplex channel in an unknown direction
		#print STDERR "D: channel is a monitored half-duplex channel in an unknown direction\n";
	} else {
		# active full-duplex channel in a known direction
		#print STDERR "D: channel is an active full-duplex channel\n";
	}
	($nodea,$nodeb) = ($nodeb,$nodea) unless $dir & 0x1;
	$self->{node}->{a} = $nodea;
	$self->{node}->{b} = $nodeb;
	$self->{xsn} = $xsn; # just to save it
	$xsn = MTP2_ANNEX_A_USED_UNKNOWN unless 0 <= $xsn and $xsn <= 2;
	$xsn = MTP2_ANNEX_A_NOT_USED     if $xsn == 2 and $slot != 0;
	$xsn = MTP2_ANNEX_A_USED	 if $xsn == 2 and $slot == 0;
	$self->{ht}     = $htoptions[[1,2,0]->[$xsn]]->[1];
	$self->{pr}     = ::MP_UNKNOWN;
	$self->{rt}     = ::RT_UNKNOWN;
	$self->{orig} = {};
	$self->{dest} = {};
	$self->{paths} = {};
	$self->{paths}->{a} = {};  # paths from the a-side
	$self->{paths}->{b} = {};  # paths from the b-side
	$self->Arcitem::init($top,$nodea,$nodeb,['node','relation','link','linkset'],[],'last',0);
	$self->Clickable::attach($top,@args);
	$self->log("header type set to ".$self->httext." by capture") if $self->{ht};
}

#package Channel;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Channel;
sub ppa {
	my ($card,$span,$slot) = @_;
	return (($card & 0x03) << 7) | (($span & 0x03) << 5) | (($slot & 0x1f) << 0);
}
#package Channel;
sub chan {
	my $ppa = shift;
	return ((($ppa >> 0) & 0x1f), (($ppa >> 5) & 0x03), (($ppa >> 7) & 0x03));
}

#package Channel;
sub bindchannel {
	my ($self,$top,$network,$nodea,$nodeb,$msg) = @_;
	return if $self->{link}; # already bound
	#print STDERR "D: binding channel ".$self->identify."\n";
	($nodea,$nodeb) = ($nodeb,$nodea) if $msg->{dir};
	my $slc = $self->{slc} = $msg->{slc};
	my ($pnodea,$pnodeb) = ($self->{node}->{a},$self->{node}->{b});
	$nodea->absorb($top,$pnodea);
	$nodeb->absorb($top,$pnodeb);
	($self->{node}->{a},$self->{node}->{b}) = ($nodea,$nodeb);
	my $link = $network->getLink($top,$nodea,$nodeb,$slc);
	$self->{link} = $link;
	$self->{dir} = $nodea->{pc} == $link->{node}->{a}->{pc} ? 0 : 1;
	if ($msg->{dir}^$self->{dir}) {
		$link->{channelrevs} = $self;
		$self->{yoff} = $link->{yoff} + 1.5;
	} else {
		$link->{channelforw} = $self;
		$self->{yoff} = $link->{yoff} - 1.5;
	}
	$link->addfrom($self,$self->{dir});
	my $linkset = $link->{linkset};
	foreach my $side (keys %{$self->{paths}}) {
		foreach my $path (values %{$self->{paths}->{$side}}) {
			my $lside = (($side eq 'a' and !$self->{dir}) or
				     ($side eq 'b' and $self->{dir})) ? 'a' : 'b';
			my $route = $linkset->getRoute($top,$network,$lside,$path->{node});
			# TODO add path stats to route
			my $pdir = ($path->{node}->{a} eq $route->{node}->{a}) ? 0 : 1;
			$route->addfrom($path,$pdir);
		}
	}
}

#package Channel;
sub getPath {
	my ($self,$top,$network,$side,$node,@args) = @_;
	my $pc = $node->{pc};
	return $self->{paths}->{$side}->{$pc} if $self->{paths}->{$side}->{$pc};
	my $path = $network->getPath($top,$self,$side,$node,@args);
	$self->{paths}->{$side}->{$pc} = $path;
	return $path;
}

#package Channel;
sub add_msg {
	my ($self,$top,$network,$msg,@args) = @_;
	unless ($self->{detected}) {
		if ($self->detecting) {
			$self->pushbuf($msg);
			return;
		}
		$self->{detected} = 1;
		$self->pushbuf($msg);
		while ($msg = $self->shiftbuf()) {
			$msg->decode($self);
			$self->complete($top,$network,$msg);
		}
	} else {
		$self->complete($top,$network,$msg);
	}
}
my %msghandler = (
	0=>{
	},
	1=>{
		0x11=>\&Channel::bindchannel,
		0x12=>\&Channel::bindchannel,
	},
	2=>{
		0x11=>\&Channel::bindchannel,
		0x12=>\&Channel::bindchannel,
	},
);
#package Channel;
sub complete {
	my ($self,$top,$network,$msg,@args) = @_;
	$self->inc($msg,$msg->{dir});
	$self->pushmsg($msg);
	$network->add_msg($top,$msg);
	if (exists $msg->{dpc}) {
		my ($cola,$colb,$sidea,$sideb,$dira,$dirb);
		if ($msg->{dir}) {
			$cola = $self->{node}->{b}->{col}; $sidea = 'b'; $dira = 1;
			$colb = $self->{node}->{a}->{col}; $sideb = 'a'; $dirb = 0;
		} else {
			$cola = $self->{node}->{a}->{col}; $sidea = 'a'; $dira = 0;
			$colb = $self->{node}->{b}->{col}; $sideb = 'b'; $dirb = 1;
		}
		my $nodea = $network->getSp($top,$msg->{opc},$cola);
		my $nodeb = $network->getSp($top,$msg->{dpc},$colb);
		if (my $sub = $msghandler{$msg->{si}}->{$msg->{mt}}) {
			&{$sub}($self,$top,$network,$nodea,$nodeb,$msg);
		}
		$self->getPath($top,$network,$sidea,$nodea)->add_msg($top,$network,$msg,$dira);
		$self->getPath($top,$network,$sideb,$nodeb)->add_msg($top,$network,$msg,$dirb);
		$network->getRouteset($top,$self,$nodea,$nodeb)->add_msg($top,$network,$msg,0);
	}
	$self->{link}->add_msg($top,$network,$msg,$msg->{dir}^$self->{dir}) if $self->{link};
}
#package Channel;
sub swap {
	my ($self,$top) = @_;

	my $nodea = $self->{node}->{a}; $nodea->swapcol($top);
	my $nodeb = $self->{node}->{b}; $nodeb->swapcol($top);
	foreach my $path (values %{$self->{opcs}}) {
		my $node = $path->{node};
		next if $node eq $nodea or $node eq $nodeb;
		$node->swapcol($top) if $nodea->{col} < 0 && $node->{col} > 0;
	}
	foreach my $path (values %{$self->{dpcs}}) {
		my $node = $path->{node};
		next if $node eq $nodea or $node eq $nodeb;
		$node->swapcol($top) if $nodeb->{col} < 0 && $node->{col} > 0;
	}
}
#package Channel;
sub detecting {
	my $self = shift;
	return 0 if ($self->{ht} != 0 && $self->{pr} != 0 && $self->{rt} != 0);
	return 1;
}

#package Channel;
sub httext { return $htoptions[{0=>0,3=>1,6=>2}->{shift->{ht}}]->[0]; }
#package Channel;
sub setht {
	my ($self,$ht,$oldht) = @_;
	if (($oldht = $self->{ht}) != $ht) { $self->{ht} = $ht;
		#print STDERR "D: detected header type ", $self->httext, " for ", $self->identify, "\n";
		$self->log("detected header type ".$self->httext) if $oldht  == ::HT_UNKNOWN;
	}
}

#package Channel;
sub rttext { return $rtoptions[{0=>0,4=>1,7=>2}->{shift->{rt}}]->[0]; }
#package Channel;
sub setrt {
	my ($self,$rt,$oldrt) = @_;
	if (($oldrt = $self->{rt}) != $rt) { $self->{rt} = $rt;
		#print STDERR "D: detected routing label ", $self->rttext, " for ", $self->identify, "\n";
		$self->log("detected routing label ".$self->rttext) if $oldrt  == ::RT_UNKNOWN;
	}
}
#package Channel;
sub prtext { return $mpoptions[shift->{pr}]->[0]; }
#package Channel;
sub setpr {
	my ($self,$pr,$oldpr) = @_;
	if (($oldpr = $self->{pr}) != $pr) { $self->{pr} = $pr;
		#print STDERR "D: detected priority type ", $self->prtext, " for ", $self->identify, "\n";
		$self->log("detected priority type ".$self->prtext) if $oldpr == ::MP_UNKNOWN;
	}
}

# -------------------------------------
package Channel; use strict;
@Channel::ISA = qw(Datalink);
# -------------------------------------

use constant {
	MTP2_ANNEX_A_NOT_USED	    => 0,
	MTP2_ANNEX_A_USED	    => 1,
	MTP2_ANNEX_A_USED_UNKNOWN   => 2,
};

#package Channel;
sub dli {
	return pack('n',shift);
}
#package Channel;
sub get {
	my ($type,$top,$network,$msg,@args) = @_;
	my $ppa = $msg->{ppa};
	my $dli = Channel::dli($ppa);
	my $self = $network->{datalinks}->{$dli};
	return $self if $self;
	my $way = $msg->{dir};
	my $nodea = Node->get($top,$network,0 - (::COL_ADJ));
	my $nodeb = Node->get($top,$network,0 + (::COL_ADJ));
	($nodea,$nodeb) = ($nodeb,$nodea) unless $way & 0x1;
	$self = Datalink::new($type,$top,$network,$dli,$nodea,$nodeb,$msg,@args);
	$self->{ppa} = $ppa;
	my ($card,$span,$slot) = Channel::chan($ppa);
	if ($way & 0x2) {
		# monitored half-duplex channel in a known direction
		my $ppa2 = $self->{ppa2} = Channel::ppa($card,$span^0x1,$slot);
		# tell messages from the other span how to find us
		$network->{datalinks}->{$ppa2} = $self;
		$self->{dli} = $ppa2 unless $way & 0x1;
	} elsif ($way & 0x4) {
		# monitored half-duplex channel in an unknown direction
	} else {
		$self->{duplex} = 1;
		# active full-duplex channel in a known direction
	}
	return $self;
}

#package Channel;
sub ppa {
	my ($card,$span,$slot) = @_;
	return (($card & 0x03) << 7) | (($span & 0x03) << 5) | (($slot & 0x1f) << 0);
}
#package Channel;
sub chan {
	my $ppa = shift;
	return ((($ppa >> 0) & 0x1f), (($ppa >> 5) & 0x03), (($ppa >> 7) & 0x03));
}

#package Channel;
sub identify {
	my $self = shift;
	my ($card,$span,$slot) = Channel::chan($self->{ppa});
	my $id = "Channel $card:$span:$slot";
	$id .= ", $self->{node}->{a}->{pcode} -> $self->{node}->{b}->{pcode} link $self->{slc}"
		if $self->{node}->{a} and $self->{node}->{a}->isa('SP');
	return ($self->{id} = $id);
}

#package Channel;
sub shortid {
	my $self = shift;
	my ($card,$span,$slot) = Channel::chan($self->{ppa});
	return "$card:$span:$slot";
}

#package Channel;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $have = {};
	$have->{link} = $self->{link} ? 1 : 0;
	$m->add('separator') if $have->{link};
	if ($have->{link}) {
		my $link = $self->{link};
		my $label = 'Link';
		my $mc = $m->Menu(-title=>"$label Menu");
		$mc->configure(-postcommand=>[sub {
			my ($self,$mc,$top,$X,$Y) = @_;
			$mc->delete(0,'end');
			$link->getmenu($mc,$top,$X,$Y);
			$link->getmore($mc,$top,$X,$Y);
		},$self,$mc,$top,$X,$Y]);
		$m->add('cascade',-menu=>$mc,-label=>$label);
	}
}

#package Channel;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;

	my $ppa = $self->shortid;
	$tw->Label(%labelright,
		-text=>'Channel:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-textvariable=>\$ppa,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

#package Channel;
sub fillstatus {
	my ($self,$top,$tw,$row) = @_;

	my $ppa = $self->shortid;
	if ($self->{node}->{a} && $self->{node}->{b}) {
		$tw->Label(%labelright,
			-text=>'Signalling link code:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryleft,
			-textvariable=>\$self->{slc},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	}
	if ($self->{node}->{a}) {
		$tw->Label(%labelright,
			-text=>'SP A point code:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryleft,
			-textvariable=>\$self->{node}->{a}->{pcode},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	}
	if ($self->{node}->{b}) {
		$tw->Label(%labelright,
			-text=>'SP B point code:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryleft,
			-textvariable=>\$self->{node}->{b}->{pcode},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	}
	$tw->Label(%labelright,
		-text=>'Header type:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $httext = $self->httext;
	$tw->Optionmenu(%optionleft,
		-options=>\@htoptions,
		-variable=>\$self->{ht},
		-textvariable=>\$httext,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Message priority:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $prtext = $self->prtext;
	$tw->Optionmenu(%optionleft,
		-options=>\@mpoptions,
		-variable=>\$self->{pr},
		-textvariable=>\$prtext,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Routing label type:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $rttext = $self->rttext;
	$tw->Optionmenu(%optionleft,
		-options=>\@rtoptions,
		-variable=>\$self->{rt},
		-textvariable=>\$rttext,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$self->fillstate($top,$tw,$row);
}

# -------------------------------------
# -------------------------------------
# -------------------------------------
# -------------------------------------
my %ttcodes = (
	0x00=>'reserved',
	0x01=>'identification cards',
	0x02=>'reserved - 14 Digit Calling Card.',
	0x03=>'cellular nationwide roaming service/OTA provisioning and IS41 roaming',
	0x04=>'global title = point code',
	0x05=>'calling name delivery',
	0x06=>'reserved - Call Management',
	0x07=>'message waiting',
	0x08=>'SCP assisted call processing',
	0x09=>'national and international cellular/PCS roaming',
	0x0a=>'network entity addressing/LNP wireline',
	0x0b=>'internetwork NP query/response (NP Q/R)/Wireless LNP',
	0x0c=>'wireless MIN-based short message service/MOSMS',
	0x0d=>'wireless IMSI-based short message service',
	0x0e=>'mobile subscriber addressing',
	0x0f=>'packet data interworking',
	0x10=>'cellular/PCS interworking',
	0x11=>'mobile subscriber message center addressing',
	0x12=>'internetwork applications',
	0x13=>'internetwork applications',
	0x14=>'internetwork applications',
	0x15=>'internetwork applications',
	0x16=>'internetwork applications',
	0x17=>'internetwork applications',
	0x18=>'internetwork applications',
	0x19=>'internetwork applications',
	0x1a=>'internetwork applications',
	0x1b=>'internetwork applications',
	0x1c=>'14-digit telecom calling cards - post 10-digit (NP) GTT (loop code)',
	0x1d=>'calling name delivery - post-10-digit (NP) GTT (loop code)',
	0x1e=>'call management - post-10-digit (NP) GTT (loop code)',
	0x1f=>'message waiting - post-10-digit (NP) GTT (loop code)',
	0x77=>'E911 (another E911 assigment)',
	0x8c=>'GSM Voice roaming MSISDN to HLR',
	0xbf=>'E911',
	0xc0=>'E911 location testing',
	0xc1=>'E911 testing',
	0xc2=>'E911 testing',
	0xc0=>'network specific applications',
	# ...
	0xd3=>'IN type Wireline LNP query',
	# ...
	0xf9=>'network specific applications',
	0xfa=>'network specific applications',
	0xfb=>'network specific or Call Management',
	0xfc=>'network specific applications',
	0xfd=>'network specific or 14 Digit Calling Card/LIDB GR-945-CORE',
	0xfe=>'800 service',
	0xff=>'reserved',
);

my %ssncodes = (
	0x00=>'SSN not known/not used',
	0x01=>'SCCP Management',
	0x02=>'Telephone User Part',
	0x03=>'ISDN User Part',
	0x04=>'OMAP',
	0x05=>'Mobile Application Part (MAP)/IS-41 MAP',
	0x06=>'Home Location Register (HLR)',
	0x07=>'Visited Location Register (VLR)',
	0x08=>'Mobile Switching Center (MSC)/E911',
	0x09=>'Equipment Identification Register (EIR)',
	0x0a=>'Authentication Center (AC)',
	0x0b=>'Reserved for ITU-T for ISDN suppl. services/MOSMS',
	0x0c=>'Reserved for international use',
	0x0d=>'Reserved for ITU-T for broadband ISDN edge-to-edge applications',
	0x0e=>'Reserved for ITU-T for TC test responder',
	0x0f=>'Reserved for international use',
	# ...
	0x1f=>'Reserved for international use',
	0x20=>'Spare',
	# ...
	0xc0=>'E911 location testing',
	0xc1=>'E911 testing',
	0xeb=>'Voicemail (message waiting)',
	0xf7=>'LNP',
	0xf8=>'LNP',
	0xfa=>'LNP Wireline and Wireless',
	# ...
	0xfb=>'Spare',
	0xfa=>'One-way outgoing CMS',
	0xfb=>'CLASS/Call Management services',
	0xfc=>'PVN/Billed Number Screening',
	0xfd=>'LIDB Query (GR-945-CORE)/Automated Calling Card Services',
	0xfe=>'800 Number Translation/800 Plus/Toll Free services',
	0xff=>'reserved for expansion',
	# network specific number should be assigned in decending order starting with 0xfe
);



# -------------------------------------
package Field;
use strict;
# -------------------------------------
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	return $self;
}

# -------------------------------------
package BitField; use strict; use vars qw(@ISA); @ISA = qw(Field);
# -------------------------------------

#package Association;
sub dli {
	my $hdr = shift;
	return pack('nnN', $hdr->{sport},$hdr->{dport},$hdr->{vtag});
}
#package Association;
sub find {
	my ($type,$top,$network,$msg,@args) = @_;
	my $sub = $type->can('dli');
	my $dli = &$sub($msg);
	return $network->{datalinks}->{$dli} if $network->{datalinks}->{$dli};
	return undef;
}
#package Association;
sub get {
	my ($type,$top,$network,$msg,@args) = @_;
	my $dli = Association::dli($msg);
	my $self = $network->{datalinks}->{$dli};
	return $self if $self;
	my ($addra,$addrb,$cola,$colb);
	if ($msg->{dir} == ::SLL_DIR_RECEIVED) {
		$addra = $msg->{daddr}; $cola = 0 + (::COL_ADJ);
		$addrb = $msg->{saddr}; $colb = 0 - (::COL_ADJ);
	} else {
		$addra = $msg->{saddr}; $cola = 0 - (::COL_ADJ);
		$addrb = $msg->{daddr}; $colb = 0 + (::COL_ADJ);
	}
	my $hosta = Host->get($top,$network,$addra,$cola);
	my $hostb = Host->get($top,$network,$addrb,$colb);
	$self = Datalink::new($type,$top,$network,$dli,$hosta,$hostb,$msg,@args);
	$self->PktCollector::init($top,@args);
	$self->PktBuffer::init(@args);
	$self->{sport} = $msg->{sport};
	$self->{dport} = $msg->{dport};
	$self->{vtag} = $msg->{vtag};
	$self->{duplex} = 1;
	$self->{mtype} = $type; $self->{mtype} =~ s/Association/Message/;
	$self->{acks}     = {0=>undef,1=>undef};    # cumm. acks
	$self->{tsns}     = {0=>{},   1=>{}};	    # data chunks
	$self->{streams}  = {0=>{},   1=>{}};	    # streams
	$self->{chunks}   = {0=>[],   1=>[]};	    # reassembled messages
	$self->{ulps}     = {0=>[],   1=>[]};
	return $self;
}

sub bits {
	my $self = shift;
	my $name = ref($self); $name=~s/_/ /g;
	my $id = sprintf("$name %d::%d(%08x)", $self->{sport},$self->{dport},$self->{vtag});
	$id .= ", $self->{node}->{a}->{pcode} <=> $self->{node}->{b}->{pcode} link $self->{slc}"
		if $self->{node}->{a} and $self->{node}->{a}->isa('SP');
	$id .= ", $self->{node}->{a}->{ipaddr} <=> $self->{node}->{b}->{ipaddr}"
		if $self->{node}->{a} and $self->{node}->{a}->isa('Host');
	return ($self->{id} = $id);
}

#package Association;
sub shortid {
	my $self = shift;
	return sprintf("%d::%d(%08x)", $self->{sport},$self->{dport},$self->{vtag});
}

#package Association;
sub hit {
	my ($self,$msg,$dir) = @_;
	$self->SUPER::hit($msg,$dir);
	$self->{node}->{a}->hit($msg,$dir^0x0);
	$self->{node}->{b}->hit($msg,$dir^0x1);
}

	}
	if (($self->{end}&0x7) != 0x7) {
		for (my $i=$self->{end}+1; ($i&0x7) != 0; $i++) {
			$bits = '-'.$bits;
		}
	}
	if (length($bits)) {
		push @bytes, sprintf('%3d %s', $p + scalar(@bytes), $bits);
	}
	return join("\n",@bytes);
}

sub makeentry {
	my ($self,$hl,$entry,$ind) = @_;
	$hl->add($entry,-text=>$self->{name});
	$hl->itemCreate($entry,1,-text=>$self->bits);
	$hl->itemCreate($entry,2," $self->{val}");
	if (ref($self->{desc}) eq 'HASH') {
		$hl->itemCreate($entry,3,-text=>" $self->{desc}->{$self->{val}}");
	} elsif (ref($self->{desc}) eq 'ARRAY') {
		$hl->itemCreate($entry,3,-text=>" $self->{desc}->[$self->{val}]");
	} else {
		$hl->itemCreate($entry,3,-text=>" $self->{desc}");
	}
	$hl->hide('entry',$entry) if $ind eq '+';
}

# ------------------------------------
package SnField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p,0,6,@args);
}
# ------------------------------------
package IbField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p,7,7,@args);
}
# ------------------------------------
package XsnField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p,0,11,@args);
}
# ------------------------------------
package Xsn0Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p,4,6,@args);
}

#package M2PA_Association;
sub add_pkt {
	my ($self,$top,$network,$pkt) = @_;

	# process chunks into complete messages
	$self->SUPER::add_pkt($top,$network,$pkt);

	foreach my $dir (%{$self->{ulps}}) {
		foreach my $msg (@{$self->{ulps}->{$dir}}) {
			bless $msg,'M2paMessage';
			if ($msg->decode($self) >= 0) {
				$self->add_msg($top,$network,$msg,$dir);
				next;
			}
			print STDERR "W: $msg: decoding error, message discarded\n";
		}
	}
}
# ------------------------------------
package BibField; use strict; use vars qw(@ISA); @ISA = qw(IbField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return SnField::new($type,$b,$p+0,@args);
}
# ------------------------------------
package FsnField; use strict; use vars qw(@ISA); @ISA = qw(SnField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return SnField::new($type,$b,$p+1,@args);
}
# ------------------------------------
package FibField; use strict; use vars qw(@ISA); @ISA = qw(IbField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return IbField::new($type,$b,$p+1,@args);
}

#package M2UA_Association;
sub add_pkt {
	my ($self,$top,$network,$pkt) = @_;

	# process chunks into complete messages
	$self->SUPER::add_pkt($top,$network,$pkt);

	foreach my $dir (%{$self->{ulps}}) {
		foreach my $msg (@{$self->{ulps}->{$dir}}) {
			bless $msg,'M2uaMessage';
			if ($msg->decode($top,$network,$self) >= 0) {
				$msg->{interface}->add_msg($top,$network,$msg,$dir);
				next;
			}
			print STDERR "W: $msg: decoding error, message discarded\n";
		}
	}
}
# ------------------------------------
package Li0Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+2,6,7,@args);
}
# ------------------------------------
package JmpField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+2,6,7,@args);
}
# ------------------------------------
package XBsnField; use strict; use vars qw(@ISA); @ISA = qw(XsnField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return XsnField::new($type,$b,$p+0,@args);
}

#package M3UA_Association;
sub add_pkt {
	my ($self,$top,$network,$pkt) = @_;

	# process chunks into complete messages
	$self->SUPER::add_pkt($top,$network,$pkt);

	foreach my $dir (%{$self->{ulps}}) {
		foreach my $msg (@{$self->{ulps}->{$dir}}) {
			bless $msg,'M3uaMessage';
			if ($msg->decode($top,$network,$self) >= 0) {
				$msg->{routing}->add_msg($top,$network,$msg,$dir);
				next;
			}
			print STDERR "W: $msg: decoding error, message discarded\n";
		}
	}
}
# ------------------------------------
package XBibField; use strict; use vars qw(@ISA); @ISA = qw(IbField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return IbField::new($type,$b,$p+1,@args);
}
# ------------------------------------
package XFsnField; use strict; use vars qw(@ISA); @ISA = qw(XsnField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return XsnField::new($type,$b,$p+2,@args);
}
# ------------------------------------
package XFsn0Field; use strict; use vars qw(@ISA); @ISA = qw(Xsn0Field);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return Xsn0Field::new($type,$b,$p+3,@args);
}
# ------------------------------------
package XFibField; use strict; use vars qw(@ISA); @ISA = qw(IbField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return IbField::new($type,$b,$p+3,@args);
}

#package SUA_Association;
sub add_pkt {
	my ($self,$top,$network,$pkt) = @_;

	# process chunks into complete messages
	$self->SUPER::add_pkt($top,$network,$pkt);

	foreach my $dir (%{$self->{ulps}}) {
		foreach my $msg (@{$self->{ulps}->{$dir}}) {
			bless $msg,'SuaMessage';
			if ($msg->decode($top,$network,$self) >= 0) {
				$msg->{routing}->add_msg($top,$network,$msg,$dir);
				next;
			}
			print STDERR "W: $msg: decoding error, message discarded\n";
		}
	}
}
# -------------------------------------
package SfField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+0,0,7,@args);
}
# -------------------------------------
package Sf2Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$top,$file) = @_;
	my $self = PcapSource::new(@_);
	my $err = '';
	my $pcap;

	unless ($pcap = Net::Pcap::pcap_open_offline($file, \$err)) {
		my $d = $top->Dialog(
			-text=>"Non-Fatal Error",
			-text=>"Could not open file:\n\n\t$file\n\nError was: $err",
			-default_button=>'Dismiss',
			-buttons=>qw/Dismiss/,
		);
		$d->Show;
		$d->destroy;
		return undef;
	}
	$top->{network} = new Network($top);
}
# -------------------------------------
package SiField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+0,0,3,@args);
}
# -------------------------------------
package MpField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+0,5,6,@args);
}
# -------------------------------------
package NiField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+0,6,7,@args);
}
# -------------------------------------
package Dpc24Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+0,0,23,@args);
}
# -------------------------------------
package Opc24Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+3,0,23,@args);
}

sub service {
	my ($self,$top,$network) = @_;

	my %hdr = ();
	my $dat = '';
	my $ret;

	while (($ret = $self->getmsg(\%hdr,\$dat)) == 0) {
		if (my $msg = create PcapMessage($top,$network,$self,\%hdr,$dat,$self->{dltype})) {
			$msg->process($top,$network);
		}
	}
}
# -------------------------------------
package Dpc14Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+0,0,13,@args);
}
# -------------------------------------
package Opc14Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+1,6,19,@args);
}
# -------------------------------------
package Slc14Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+3,4,7,@args);
}
# -------------------------------------
package CicField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+0,0,15,@args);
}

# -------------------------------------
package ClrfField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p,0,23,@args);
}

# -------------------------------------
package SpcField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,$rt,@args) = @_;
	if ($rt == ::RT_14BIT_PC) {
		return BitField::new($type,$b,$p,0,13,@args);
	} else {
		return BitField::new($type,$b,$p,0,23,@args);
	}
}

# -------------------------------------
package TmrField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
my %enum = (
	0x00=>'speech',
	0x01=>'spare',
	0x02=>'64 kbit/s unrestricted',
	0x03=>'3.1 kHz audio',
	0x04=>'reserved for alternate speech(s2)/64 kbit/s unrestricted (s1)',
	0x05=>'reserved for alternate 64 kbit/s unrestricted (s1), speech (s2)',
	0x06=>'64 kbit/s preferred',
	0x07=>'2 x 64 kbit/s unrestricted',
	0x08=>'384 kbit/s unrestricted',
	0x09=>'1536 kbit/s unrestricted',
	0x0a=>'1920 kbit/s unrestricted',
	0x0b=>'spare',
	0x0c=>'spare',
	0x0d=>'spare',
	0x0e=>'spare',
	0x0f=>'spare',
	0x10=>'3 x 64 kbit/s unrestricted',
	0x11=>'4 x 64 kbit/s unrestricted',
	0x12=>'5 x 64 kbit/s unrestricted',
	0x13=>'spare', # note: 6x64 same as 384 kbit/s
	0x14=>'7 x 64 kbit/s unrestricted',
	0x15=>'8 x 64 kbit/s unrestricted',
	0x16=>'9 x 64 kbit/s unrestricted',
	0x17=>'10 x 64 kbit/s unrestricted',
	0x18=>'11 x 64 kbit/s unrestricted',
	0x19=>'12 x 64 kbit/s unrestricted',
	0x1a=>'13 x 64 kbit/s unrestricted',
	0x1b=>'14 x 64 kbit/s unrestricted',
	0x1c=>'15 x 64 kbit/s unrestricted',
	0x1d=>'16 x 64 kbit/s unrestricted',
	0x1e=>'17 x 64 kbit/s unrestricted',
	0x1f=>'18 x 64 kbit/s unrestricted',
	0x20=>'19 x 64 kbit/s unrestricted',
	0x21=>'20 x 64 kbit/s unrestricted',
	0x22=>'21 x 64 kbit/s unrestricted',
	0x23=>'22 x 64 kbit/s unrestricted',
	0x24=>'23 x 64 kbit/s unrestricted',
	0x25=>'spare', # note: 24x64 same as 1536 kbit/s
	0x26=>'25 x 64 kbit/s unrestricted',
	0x27=>'26 x 64 kbit/s unrestricted',
	0x28=>'27 x 64 kbit/s unrestricted',
	0x29=>'28 x 64 kbit/s unrestricted',
	0x2a=>'29 x 64 kbit/s unrestricted',
	0x2b=>'spare', # note: 30x64 same as 1920 kbit/s
);
sub new {
	my ($type,$b,$p,$rt,@args) = @_;
	my $self = BitField::new($type,$b,$p,0,7,@args);
	$self->{name} = 'TMR';
	$self->{dflt} = 'spare';
	$self->{enum} = \%enum;
	$self->{desc} = 'Transmission Medium Requirement';
	return $self;
}


# -------------------------------------
package Parameter;
use strict;
# -------------------------------------
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	return $self;
}

# -------------------------------------
package IsupParameter; use strict; use vars qw(@ISA); @ISA = qw(Parameter);
# -------------------------------------
sub new {
	my ($type,$b,$p,$e,@args) = @_;
	my $self = Parameter::new($type,$b,$p,$e,@args);
	return $self;
}
sub opt {
	my ($type,$b,$p,$e,@args) = @_;
	my $self = Parameter::new($type,$b,$p,$e,@args);
	$self->dec($b,$p,$e,@args);
	return $self;
}
# -------------------------------------
package FixIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
sub new {
	my ($type,$b,$p,$e,@args) = @_;
	my $self = IsupParameter::new($type,$b,$p,$e,@args);
	$self->{len} = $e - $p;
	$self->{ptr} = $p;
	if ($self->{len} <= 4) {
		$self->{val} = 0;
		for (my $i = $e-1; $i >= $p; $i--) {
			$self->{val} <<= 8;
			$self->{val} |= $b->[$i];
		}
	} else {
		$self->{val} = pack('C*', $b->[$p..($e-1)]);
	}
	return $self;
}
sub dec {
	my ($self,$b,$p,$e,$l,@args) = @_;
	$self->{len} = $l;
	$self->{ptr} = $$p;
	if ($l <= 4) {
		$self->{val} = 0;
		for (my $i = $$p + $l - 1; $i >= $$p; $i--) {
			$self->{val} <<= 8;
			$self->{val} |= $b->[$i];
		}
	} else {
		$self->{val} = pack('C*', $b->[$$p..($$p+$l-1)]);
	}
}
sub man {
	my ($type,$b,$p,$e,@args) = @_;
	my $self = IsupParameter::new($type,$b,$p,$e,@args);
	$self->dec($b,$p,$e,$e-$$p,@args);
	return $self;
}
sub opt {
	my ($type,$b,$p,$e,@args) = @_;
	my $self = IsupParameter::new($type,$b,$p,$e,@args);
	my $l = $b->[$$p++];
	$self->dec($b,$p,$e,$l,@args);
	return $self;
}


# -------------------------------------
package VarIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
sub new {
	my ($type,$b,$p,$e,@args) = @_;
	my $self = IsupParameter::new($type,$b,$p,$e,@args);
	$self->{len} = $b->[$p];
	$p++;
	$e = $p + $self->{len};
	$self->{ptr} = $p;
	if ($self->{len} <= 4) {
		$self->{val} = 0;
		for (my $i = $e-1; $i >= $p; $i--) {
			$self->{val} <<= 8;
			$self->{val} |= $b->[$i];
		}
	} else {
		$self->{val} = pack('C*', $b->[$p..($e-1)]);
	}
	return $self;
}
sub dec {
	my ($self,$b,$p,$e,$l,@args) = @_;
	$self->{len} = $l;
	$self->{ptr} = $$p;
	$self->{val} = pack('C*', $b->[$$p..($$p+$l-1)]);
}
sub man {
	my ($type,$b,$p,$e,@args) = @_;
	my $self = IsupParameter::new($type,$b,$p,$e,@args);
	my $l = $b->[$$p++];
	$self->dec($b,$p,$e,$l,@args);
	return $self;
}
sub opt {
	return VarIsupParameter::man(@_);
}

# -------------------------------------
package EopIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
sub man {
	my ($type,$b,$p,$e,@args) = @_;
	return SUPER::man($type,$b,$p,$$p,@args);
}
sub opt {
	my ($type,$b,$p,$e,@args) = @_;
	return SUPER::opt($type,$b,$p,$$p,@args);
}
# -------------------------------------
package ClrfIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
sub dec {
	my ($self,$b,$p,$e,$l,$rt,@args) = @_;
	$self->{fields} = [];
	push @{$self->{fields}}, ClrfField->new($b,$self->{ptr});
	push @{$self->{fileds}}, SpcField->new($b,$self->{ptr}+3);
}
# -------------------------------------
package TmrIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
sub dec {
	my ($self,$b,$p,$e,$l,$rt,@args) = @_;
	$self->{fields} = [];
	push @{$self->{fields}}, TmrField->new($b,$self->{ptr});
}
# -------------------------------------
package AtpIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package CdpnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package SubnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package NciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package FciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package OfciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CpcIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CgpnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package RdgnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package RdnnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package ConrIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package InriIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package InfiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CotiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package BciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CausIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package RdiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CgiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package RsIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CmiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package FaciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package FaiiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CugiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package IndexIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CugcriIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package UsiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package SpcIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package UuiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package ConnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package SrisIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package TnsIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package EvntIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CamIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package CsiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package AclIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package OcdnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package ObciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package UuindIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package IspcIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package GnotIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package ChiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package AdiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package NsfIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package UsipIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package PropIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package RopsIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package SaIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package UtiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package TmuIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CdiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package EciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package MciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package PciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package MlppIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package MciqIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package McirIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package HopcIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package TmrpIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package LnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package RdnrIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package FreepIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package GrefIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package RcapIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package NmcIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package RcntIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package PcapIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package PriIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package GglIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package PstaIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package PcntIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package PrfiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package PrbiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package RstaIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package RfiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package RbiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package GnumIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package GdigIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package OsiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package EgressIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package JurIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CidcIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package BgroupIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package GnamIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package NotiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package SvactIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CspiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package TrnreqIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package LspiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package CgciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CvriIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package OtgnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CinIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package ClliIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package OliIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package ChgnIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package SvcdIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package SprIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package CselIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package NtIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(IsupParameter);
# -------------------------------------
# -------------------------------------
package OriIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package IriIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------
# -------------------------------------
package RateIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package IicIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package ToiIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package TonIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package CriIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(FixIsupParameter);
# -------------------------------------
# -------------------------------------
package IcciIsupParameter; use strict; use vars qw(@ISA); @ISA = qw(VarIsupParameter);
# -------------------------------------


# -------------------------------------
package Message;
use strict;
# -------------------------------------
# $msg = Message::create($pcap);
#package Message;
sub create {
	my $type = shift;
	my $self = {};
	bless $self, $type;
	my $pcap = shift;
	my %hdr;
	my $dat = '';
	$self->{hdr} = {};
	$self->{dat} = '';
	my $ret = Net::Pcap::pcap_next_ex($pcap, \%hdr, \$dat);
	$self->{hdr}->{len} = $hdr{len} - 4;
	$self->{hdr}->{caplen} = $hdr{caplen} - 4;
	$self->{hdr}->{tv_sec} = $hdr{tv_sec};
	$self->{hdr}->{tv_usec} = $hdr{tv_usec};
	$self->{dat} = substr($dat, 4);
	return undef if $ret != 1;
	my ( $dir, $xsn, $lkno0, $lkno1 ) = unpack('CCCC', $dat);
	$self->{dir} = $dir;
	$self->{xsn} = $xsn;
	$self->{ppa} = ($lkno0 << 8) | $lkno1;
	return $self;
}
#package Message;
sub process {
	my ($self,$top,$network) = @_;
	my $channel = $network->getChannel($top,$self->{ppa},$self->{dir},$self->{xsn});
	$self->{dir} = ($self->{dir} ^ 0x1) & 0x1;
	if ($self->decode($channel) >= 0) {
		$channel->add_msg($top,$network,$self);
		return;
	}
	print STDERR "W: decoding error, message discarded\n";
}
#package Message;
sub decode {
	my ($self,$channel,@args) = @_;
	$self->{channel} = $channel;
	my @b = (unpack('C*', substr($self->{dat}, 0, 8)));
	if (!exists $self->{mtp2decode}) {
		my $len = $self->{hdr}->{len};
		if (3 <= $len && $len <= 5) {
			$channel->setht(::HT_BASIC);
		} elsif (6 <= $len && $len <= 8) {
			$channel->setht(::HT_EXTENDED);
		}
		return 0 unless $channel->{ht} != ::HT_UNKNOWN;
		my ($bsn,$bib,$fsn,$fib,$li,$li0);
		if ($channel->{ht} == ::HT_BASIC) {
			$self->{bsn} = $b[0] & 0x7f;
			$self->{bib} = $b[0] >> 7;
			$self->{fsn} = $b[1] & 0x7f;
			$self->{fib} = $b[1] >> 7;
			$self->{li} = $b[2] & 0x3f;
			$self->{li0} = $b[2] >> 6;
		} elsif ($channel->{ht} == ::HT_EXTENDED) {
			$self->{bsn} = $b[0];
			$self->{bsn} |= ($b[1] & 0x0f) << 8;
			$self->{bib} = $b[1] >> 7;
			$self->{fsn} = $b[2];
			$self->{fsn} |= ($b[3] & 0x0f) << 8;
			$self->{fib} = $b[3] >> 7;
			$self->{li} = $b[4];
			$self->{li} |= ($b[5] & 0x1) << 8;
			$self->{li0} = $b[5] >> 6;
		}
		my $inf = $len - $channel->{ht};
		if (($self->{li} != $inf) && ($inf <= 63 || $self->{li} != 63)) {
			print STDERR "W: bad length indicator $self->{li} != $inf\n";
		}
		if ($self->{li0} != 0) {
			$channel->setrt(::RT_24BIT_PC);
			$channel->setpr(::MP_JAPAN);
		}
		$self->{mtp2decode} = 1;
	}
	return 1 if $self->{li} == 0;
	@b = (unpack('C*', substr($self->{dat}, $channel->{ht}, 11)));
	$self->{si} = $b[0];
	return 1 if $self->{li} < 3;
	$self->{ni} = ($b[0] & 0xc0) >> 6;
	$self->{mp} = ($b[0] & 0x30) >> 4;
	$self->{si} = ($b[0] & 0x0f);
	if ($self->{ni} == 0) {
		$channel->setrt(::RT_14BIT_PC);
		$channel->setpr(::MP_INTERNATIONAL);
	}
	if ($channel->{pr} == ::MP_UNKNOWN) {
		$channel->setpr(::MP_NATIONAL) if $self->{mp} != 0;
	} elsif ($channel->{pr} == ::MP_JAPAN) {
		$self->{mp} = $self->{li0};
	} elsif ($channel->{pr} == ::MP_INTERNATIONAL) {
		$self->{mp} = 0;
	}
	if ($self->{li} < ::HT_EXTENDED) {
		print STDERR "W: too short for RL, li = $self->{li}\n";
		return -1;
	}
	if ($self->{li} < 9 || ($self->{si} == 5 && $self->{li} < 11)) {
		$channel->setrt(::RT_14BIT_PC);
	}
	if ($channel->{rt} == ::RT_UNKNOWN) {
		my $ret = $self->checkRoutingLabelType($self->{si},$channel,$self->{li},\@b);
		if ($ret <= 0) {
			#print STDERR "D: check failed on si=$self->{si} for ",$channel->identify,"\n";
			return $ret;
		}
		#print STDERR "D: check succeeded on si=$self->{si} for ",$channel->identify,"\n";
	}
	if (!exists $self->{mtp3decode}) {
		return 0 if $channel->detecting;
		if ($channel->{rt} == ::RT_14BIT_PC) {
			if ($self->{li} < 6) {
				print STDERR "W: too short for 14-bit RL, li = $self->{li}\n";
				return -1;
			}
			$self->{dpc} = $b[1];
			$self->{dpc} |= ($b[2] & 0x3f) << 8;
			$self->{opc} = ($b[2] & 0xc0) >> 6;
			$self->{opc} |= $b[3] << 2;
			$self->{opc} |= ($b[4] & 0x0f) << 10;
			$self->{sls} = $b[4] >> 4;
		} else {
			if ($self->{li} < 9) {
				print STDERR "W: too short for 24-bit RL, li = $self->{li}\n";
				return -1;
			}
			$self->{dpc} = $b[1];
			$self->{dpc} |= $b[2] << 8;
			$self->{dpc} |= $b[3] << 16;
			$self->{opc} = $b[4];
			$self->{opc} |= $b[5] << 8;
			$self->{opc} |= $b[6] << 16;
			$self->{sls} = $b[7];
		}
		@b = (unpack('C*', substr($self->{dat}, $channel->{ht} + 1 + $channel->{rt}, 3)));
		if ($self->{si} == 5) {
			$self->{cic} = $b[0];
			$self->{cic} |= $b[1] << 8;
			$self->{mt} = $b[2];
		} elsif ($self->{si} < 3) {
			$self->{mt} = (($b[0] & 0x0f) << 4) | ($b[0] >> 4);
		} else {
			$self->{mt} = $b[0];
		}
		unless (exists $mtypes{$self->{si}}) {
			print STDERR "W: no message type for si=$self->{si}\n";
		}
		unless (exists $mtypes{$self->{si}}->{$self->{mt}}) {
			print STDERR "W: no message type for si=$self->{si}, mt=$self->{mt}\n";
		}
		if ($self->{si} == 1 || $self->{si} == 2) {
			if ($channel->{rt} == ::RT_14BIT_PC) {
				$self->{slc} = $self->{sls};
				$self->{dlen0} = $b[1] & 0x0f;
			} else {
				$self->{slc} = $b[1] & 0x0f;
			}
			$self->{dlen} = $b[1] >> 4;
		}
		$self->{mtp3decode} = 1;
	}
	return 1;
}
#package Message;
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
#package Message;
sub checkSnmm {
	my ($self,$channel,@args) = @_;
	my $ansi = $self->checkAnsiSnmm($channel,@args);
	my $itut = $self->checkItutSnmm($channel,@args);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$channel->setrt(::RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(::RT_14BIT_PC);
		} else {
			$channel->setrt(::RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(::RT_14BIT_PC);
		return 1;
	}
}
#package Message;
sub checkItutSnmm {
	my ($self,$channel,$li,$b) = @_;
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
#package Message;
sub checkAnsiSnmm {
	my ($self,$channel,$li,$b) = @_;
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
#package Message;
sub checkSntm {
	my ($self,$channel,@args) = @_;
	my $ansi = $self->checkAnsiSntm($channel,@args);
	my $itut = $self->checkItutSntm($channel,@args);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$channel->setrt(::RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(::RT_14BIT_PC);
		} else {
			$channel->setrt(::RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(::RT_14BIT_PC);
		return 1;
	}
}
#package Message;
sub checkItutSntm {
	my ($self,$channel,$li,$b) = @_;
	my $mt = (($b->[5] & 0x0f) << 4) | ($b->[5] >> 4);
	return PT_NO if 7 > $li || $li > 22;
	return PT_NO if $mt != 0x11 && $mt != 0x12;
	if (($b->[6] >> 4) == $li - 7) {
		return PT_YES if 7 <= $li && $li <= 9;
		return PT_MAYBE;
	}
	return PT_NO;
}
#package Message;
sub checkAnsiSntm {
	my ($self,$channel,$li,$b) = @_;
	my $mt = (($b->[8] & 0x0f) << 4) | ($b->[8] >> 4);
	return PT_NO if 10 > $li || $li > 25;
	return PT_NO if $mt != 0x11 && $mt != 0x12;
	if (($b->[9] >> 4) == $li - 10) {
		return PT_YES if 23 <= $li && $li <= 25;
		return PT_MAYBE;
	}
	return PT_NO;
}
#package Message;
sub checkSnsm {
	my ($self,$channel,@args) = @_;
	my $ansi = $self->checkAnsiSnsm($channel,@args);
	my $itut = $self->checkItutSntm($channel,@args);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$channel->setrt(::RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(::RT_14BIT_PC);
		} else {
			$channel->setrt(::RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(::RT_14BIT_PC);
		return 1;
	}
}
#package Message;
sub checkItutSnsm {
	return PT_NO;
}
#package Message;
sub checkAnsiSnsm {
	my ($self,@args) = @_;
	return $self->checkAnsiSntm(@args);
}
#package Message;
sub checkSccp {
	my ($self,$channel,$li,$b) = @_;
	my $ansi = $self->checkAnsiSccp($channel,$b);
	my $itut = $self->checkItutSccp($channel,$b);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$channel->setrt(::RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(::RT_14BIT_PC);
		} else {
			$channel->setrt(::RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(::RT_14BIT_PC);
		return 1;
	}
}
#package Message;
sub checkItutSccp {
	my ($self,$channel,$li,$b) = @_;
	my $mt = $b->[5];
	return PT_MAYBE if 0x01 <= $mt && $mt <= 0x14;
	return PT_NO;
}
#package Message;
sub checkAnsiSccp {
	my ($self,$channel,$li,$b) = @_;
	my $mt = $b->[8];
	return PT_MAYBE if 0x01 <= $mt && $mt <= 0x14;
	return PT_NO;
}
#package Message;
sub checkIsup {
	my ($self,$channel,@args) = @_;
	my $ansi = $self->checkAnsiIsup($channel,@args);
	my $itut = $self->checkItutIsup($channel,@args);
	if ($ansi == PT_YES) {
		return 0 if $itut == PT_YES;
		$channel->setrt(::RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(::RT_14BIT_PC);
		} else {
			$channel->setrt(::RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(::RT_14BIT_PC);
		return 1;
	}
}
#package Message;
sub checkItutIsup {
	my ($self,$channel,$li,$b) = @_;
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
#package Message;
sub checkAnsiIsup {
	my ($self,$channel,$li,$b) = @_;
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
	if (exists $mtypes{5}->{$mt}) {
		return PT_MAYBE;
	}
	return PT_MAYBE;
}
#package Message;
sub decodeit {
	my $self = shift;
	return if $self->{dec};
	my $channel = $self->{channel};
	my @l2h = ();
	my $p = 0;
	my $li = 0;
	if ($channel->{ht} == ::HT_EXTENDED) {
		push @l2h, XBsnField->new($b,$p);
		push @l2h, XBsn0Field->new($b,$p);
		push @l2h, XBibField->new($b,$p);
		push @l2h, XFsnField->new($b,$p);
		push @l2h, XFsn0Field->new($b,$p);
		push @l2h, XFibField->new($b,$p);
		push @l2h, ($li = XliField->new($b,$p));
		push @l2h, Xli0Field->new($b,$p);
		push @l2h, XjmpField->new($b,$p) if $channel->{pr} == ::MP_JAPAN;
		$p += ::HT_EXTENDED;
	} else {
		push @l2h, BsnField->new($b,$p);
		push @l2h, BibField->new($b,$p);
		push @l2h, FsnField->new($b,$p);
		push @l2h, FibField->new($b,$p);
		push @l2h, ($li = LiField->new($b,$p));
		if ($channel->{pr} == ::MP_JAPAN) {
			push @l2h, JmpFiled->new($b,$p);
		} else {
			push @l2h, LiField->new($b,$p);
		}
		$p += ::HT_BASIC;
	}
	$self->{dec}->{l2h} = \@l2h;
	return if $li->{val} == 0;
	if ($li->{val} == 1 or $li->{val} == 2) {
		my @sf = ();
		push @sf, SfField->new($b,$p);
		push @sf, Sf2Field->new($b,$p) if ($li->{val} == 2);
		$self->{dec}->{sf} = \@sf;
		return;
	}
	my $si;
	my @sio = ();
	push @sio, ($si = SiField->new($b,$p));
	push @sio, MpField->new($b,$p);
	push @sio, NiField->new($b,$p);
	$p++;
	my @rl = ();
	if ($channel->{rt} == ::RT_24BIT_PC) {
		push @rl, Dpc24Field->new($b,$p);
		push @rl, Opc24Field->new($b,$p);
		push @rl, Sls24Field->new($b,$p);
		$p += ::RT_24BIT_PC;
	} else {
		push @rl, Dpc14Field->new($b,$p);
		push @rl, Opc14Field->new($b,$p);
		push @rl, Slc14Field->new($b,$p);
		$p += ::RT_14BIT_PC;
	}
}
#package Message;
sub showmsg {
	my ($self,$tl) = @_;
	my ($tw,$w,$bmsg);
	if ($tw = $self->{show}) {
		if ($tw->state eq 'iconic') {
			$tw->deiconify;
		} else {
			$tw->UnmapWindow;
		}
		$tw->MapWindow;
		return;
	}
	my $title = "Message $self->{msgnum}";
	$tw = $tl->Toplevel(
		-title=>$title,
		Name=>'messageWindow',
		-class=>'SS7view',
	);
	#$tw->group($tl);
	#$tw->transient($tl);
	$tw->iconimage('icon');
	$tw->iconname($title);
	#$tw->resizeable(0,0);
	#$tw->positionfrom('user');
	#$tw->minsize(600,400);
	#$tw->geometry("+$X+$Y");
	$tw->protocol('WM_DELETE_WINDOW', [sub {
		my $self = shift;
		my $tw = $self->{show};
		delete $self->{show};
		$tw->destroy;
	},$self]);
	$self->{show} = $tw;
	$self->showdecode($tw);
	$tw->update;
	$tw->MapWindow;
}
#package Message;
sub showdecode {
	my ($self,$tw) = @_;
	$self->{tree}->{0} = 'minus';
	$self->{tree}->{1} = 'minus';
	$self->{tree}->{2} = 'minus';
	$self->{tree}->{3} = 'minus';
	$self->{tree}->{4} = 'minus';
	my $s = $tw->Scrolled('HList',
		-scrollbars=>'osoe',
		-columns=>4,
		-background=>'white',
		-drawbranch=>0,
		-header=>0,
		-itemtype=>'text',
		-indent=>16,
		-indicator=>1,
		-indicatorcmd=>[sub{
			my ($self,$entry,$action) = @_;
			my $hl = $self->{hl};
			if ($action eq '<Activate>') {
				delete $hl->{armed};
				if (exists $self->{tree}->{$entry}) {
					if ($self->{tree}->{$entry} eq 'plus') {
						foreach my $e ($hl->info('children',$entry)) {
							$hl->show('entry',$e);
						}
						$hl->indicator('configure', $entry, -image=>'minus',);
						$self->{tree}->{$entry} = 'minus';
					} else {
						foreach my $e ($hl->info('children',$entry)) {
							$hl->hide('entry',$e);
						}
						$hl->indicator('configure', $entry, -image=>'plus',);
						$self->{tree}->{$entry} = 'plus';
					}
				}
			} elsif ($action eq '<Arm>') {
				$hl->{armed} = $entry;
			} elsif ($action eq '<Disarm>') {
				delete $hl->{armed};
			}
		},$self],
		-selectmode=>'browse',
		-separator=>'.',
		-exportselection=>1,
	)->pack(
		-expand=>1,
		-fill=>'both',
		-side=>'top',
		-anchor=>'n',
	);
	my $hl = $self->{hl} = $s->Subwidget('scrolled');
	my $ro = $self->{ro} = $tw->ROText(
		-background=>'white',
		-wrap=>'word',
		-height=>8,
	)->pack(
		-expand=>0,
		-fill=>'both',
		-side=>'top',
		-anchor=>'s',
	);
	$ro->insert('0.0', 'Message bytes...');
	my $font = $ro->cget('-font');
	$hl->configure(-font=>$font);
	$self->rotext($ro);
	$self->hlist($hl);
}
#package Message;
sub rotext {
	my ($self,$ro) = @_;
	$ro->delete('0.0','end');
	my @b = (unpack('C*',$self->{dat}));
	my @lines = ();
	my ($i,$c,$line);
	while (@b) {
		for ($i = 0; ($i < 16) and @b; $i++) {
			$line .= ' ' if $line;
			$line .= ' ' unless $i & 0x7;
			$line .= sprintf('%02X',shift @b);
		}
		push @lines, $line;
		$line = undef;
	}
	$ro->insert('0.0',join("\n",@lines));
}
#package Message;
sub makeentry {
	my ($self,$hl,$byte,$beg,$end,$val,$name,$entry,$desc,$sub) = @_;
	my @bytes = ();
	my $bits = '';
	my $value;
	if ($sub) {
		my $func = shift @{$sub};
		$value = &$func(@{$sub});
	} else {
		$value = $val;
	}
	my $word = $val;
	for (my $i = 0; $i < $beg; $i++) {
		$bits = '-'.$bits;
	}
	for (my $i = $beg; $i <= $end; $i++) {
		$bits = (($word & 0x1) ? '1' : '0') . $bits;
		$word >>= 1;
		if (($i & 0x7) == 0x7) {
			push @bytes, " $bits";
			$bits = '';
		}
	}
	if (($end & 0x7) != 0x7) {
		for (my $i = $end+1; ($i&0x7) != 0; $i++) {
			$bits = '-'.$bits;
		}
	}
	if (length($bits)) {
		push @bytes, " $bits";
	}
	$bits = join("\n", @bytes);
	my $parent = $entry;
	$parent=~s/\.[^\.]*$//;
	$hl->add($entry,-itemtype=>'text', -text=>$name, -style=>$self->{fldstyle});
	#$hl->indicator('create', $entry, -itemtype=>'image', -image=>'box');
	$hl->itemCreate($entry, 1, -itemtype=>'text', -text=>$bits, -style=>$self->{bitstyle});
	$hl->itemCreate($entry, 2, -itemtype=>'text', -text=>" $value", -style=>$self->{valstyle});
	if (ref($desc) eq 'HASH') {
		$hl->itemCreate($entry, 3, -itemtype=>'text', -text=>" $desc->{$val}", -style=>$self->{desstyle});
	} elsif (ref($desc) eq 'ARRAY') {
		$hl->itemCreate($entry, 3, -itemtype=>'text', -text=>" $desc->[$val]", -style=>$self->{desstyle});
	} else {
		$hl->itemCreate($entry, 3, -itemtype=>'text', -text=>" $desc", -style=>$self->{desstyle});
	}
	$hl->hide('entry',$entry) if $self->{tree}->{$parent} eq '+';
}
#package Message;
sub hlist {
	my ($self,$hl) = @_;
	$hl->delete('all');
	$hl->add('0',-text=>'L2',);
	$hl->itemCreate('0',1,-text=>' ********');
	$hl->itemCreate('0',3,-text=>'L2 Pseudoheader');
	$hl->indicator('create','0',-itemtype=>'image',-image=>$self->{tree}->{0});
	$hl->add('1',-itemtype=>'text',-text=>'L2',);
	$hl->itemCreate('1',1,-text=>' ********');
	$hl->itemCreate('1',3,-text=>'L2 Header');
	$hl->indicator('create','1',-itemtype=>'image',-image=>$self->{tree}->{1});
	my $sio;
	if ($self->{datalink}->{ht} == ::HT_EXTENDED) {
		$self->makeentry($hl,0,0,11,$self->{bsn},'BSN','1.2','Backward Sequence Number');
		$self->makeentry($hl,1,7,7,$self->{bib},'BIB','1.1','Backward Indicator Bit');
		$self->makeentry($hl,2,0,11,$self->{fsn},'FSN','1.4','Forward Sequence Number');
		$self->makeentry($hl,3,7,7,$self->{fib},'FIB','1.3','Forward Indicator Bit');
		$self->makeentry($hl,4,0,8,$self->{li},'LI','1.5','Length Indicator');
		$self->makeentry($hl,5,2,7,$self->{li0},'LI0','1.6','Spare Bits');
		$sio = 6;
	} else {
		$self->makeentry($hl,0,0,6,$self->{bsn},'BSN','1.2','Backward Sequence Number');
		$self->makeentry($hl,0,7,7,$self->{bib},'BIB','1.1','Backward Indicator Bit');
		$self->makeentry($hl,1,0,6,$self->{fsn},'FSN','1.4','Forward Sequence Number');
		$self->makeentry($hl,1,7,7,$self->{fib},'FIB','1.3','Forward Indicator Bit');
		$self->makeentry($hl,2,0,5,$self->{li},'LI','1.5','Length Indicator');
		$self->makeentry($hl,2,6,7,$self->{li0},'LI0','1.6','Spare Bits');
		$sio = 3;
	}
	if ($self->{li} > 3) {
		$hl->add('2',-itemtype=>'text',-text=>'L3',);
		$hl->itemCreate('2',1,-text=>' ********');
		$hl->itemCreate('2',3,-text=>'L3 Header');
		$hl->indicator('create','2',-itemtype=>'image',-image=>$self->{tree}->{2});
		my ($desc,$mtyp);
		if (exists $mtypes{$self->{si}}) {
			$mtyp = "\U$mtypes{$self->{si}}->{0}->[0]\E";
			$desc = "Service Indicator: $mtyp";
		} else {
			$mtyp = 'L4';
			$desc = "Service Indicator: ($self->{si}) Unknown.";
		}
		$self->makeentry($hl,$sio,0,3,$self->{si},'SI','2.3',$desc,);
		$self->makeentry($hl,$sio,4,5,$self->{mp},'MP','2.2',{
				0=>'Message Priority 0',
				1=>'Message Priority 1',
				2=>'Message Priority 2',
				3=>'Message Priority 3',});
		$self->makeentry($hl,$sio,6,7,$self->{ni},'NI','2.1',{
				0=>'Network Indicator: International',
				1=>'Network Indicator: International (spare)',
				2=>'Network Indicator: National',
				3=>'Network Indicator: National (spare)',});
		$self->makeentry($hl,4,0,23,$self->{opc},"\nOPC\n",'2.4',"Originating Point Code",[\&SP::pcstring,$self->{opc}]);
		$self->makeentry($hl,7,0,23,$self->{dpc},"\nDPC\n",'2.5',"Destination Point Code",[\&SP::pcstring,$self->{dpc}]);
		$self->makeentry($hl,10,0,7,$self->{sls},'SLS','2.6','Signalling Link Selection');
		$hl->add('3',-itemtype=>'text',-text=>$mtyp,);
		$hl->itemCreate('3',1,-text=>' ********');
		$hl->itemCreate('3',3,-text=>"$mtyp Header");
		$hl->indicator('create','3',-itemtype=>'image',-image=>$self->{tree}->{3});
		if (exists $mtypes{$self->{si}}->{$self->{mt}}) {
			$desc = "Message Type: \U$mtypes{$self->{si}}->{$self->{mt}}->[0]\E";
		} else {
			$desc = "Message Type: ($self->{mt}) Unknown.";
		}
		if ($self->{si} == 0) {
			$self->makeentry($hl,11,0,7,$self->{mt},'MT','3.1',$desc,);
		} elsif ($self->{si} == 1 || $self->{si} == 2) {
			$self->makeentry($hl,11,0,7,$self->{mt},'MT','3.1',$desc,);
			if ($self->{datalink}->{rt} == ::RT_24BIT_PC) {
				$self->makeentry($hl,12,0,3,$self->{slc},'SLC','3.2','Signalling Link Code');
			} else {
				$self->makeentry($hl,12,0,3,$self->{slc},'SLC','3.2','Signalling Link Code');
			}
			$self->makeentry($hl,12,4,7,$self->{dlen},'DLEN','3.3','Test Data Length');
		} elsif ($self->{si} == 3) {
			$self->makeentry($hl,11,0,7,$self->{mt},'MT','3.1',$desc,);
			$hl->add('4',-itemtype=>'text',-text=>'L5',);
			$hl->itemCreate('4',1,-text=>' ********');
			$hl->itemCreate('4',3,-text=>'L5 Header');
			$hl->indicator('create','4',-itemtype=>'image',-image=>$self->{tree}->{4});
		} elsif ($self->{si} == 5) {
			$self->makeentry($hl,12,0,15,$self->{cic},"CIC",'3.1','Circuit Identification Code');
			$self->makeentry($hl,14,0,7,$self->{mt},'MT','3.2',$desc,);
			$hl->add('4',-itemtype=>'text',-text=>'L5',);
			$hl->itemCreate('4',1,-text=>' ********');
			$hl->itemCreate('4',3,-text=>'Parameters');
			$hl->indicator('create','4',-itemtype=>'image',-image=>$self->{tree}->{4});
			my $mt = $self->{mt};
		}
	}
}

# -------------------------------------
package M2paMessage; use strict;
@M2paMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	my ( $vers, $resv, $clas, $type,)
		= unpack('CCCC',substr($$dat,$$off,4));
	return -1 if $vers != 1 || $clas != 11 || $type != 1 || $$len < 20;
	( $self->{li}, $self->{bsn}, $self->{fsn}, )
		= unpack('NNN',substr($$dat,$$off+4,12));
	$self->{bsn} &= 0x00ffffff;
	$self->{fsn} &= 0x00ffffff;
	$self->{li} -= 16;
	return -1 if $self->{li} < 8;
	$$off += 16; $$len -= 16;
	$self->{li0} = unpack('C',substr($$dat,$$off,1)) >> 6;
	$$off += 1; $$len -= 1;
	if ($$len < $self->{li}) {
		print STDERR "W: $self: bad length indicator $self->{li} != $$len\n";
	}
	if ($self->{li0} != 0) {
		$datalink->setrt(::RT_24BIT_PC);
		$datalink->setpr(::MP_JAPAN);
	}
	bless $self, 'Mtp3Message';
	return $self->decode($datalink,@args);
}

# -------------------------------------
package M2uaMessage; use strict;
@M2uaMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$top,$network,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	my ( $vers, $resv, $clas, $type )
		= unpack('CCCC',substr($$dat,$$off,4));
	my $mlen = unpack('N',substr($$dat,$$off+4,4));
	return -1 if $vers != 1 || $clas != 6 || $type != 1 || $$len < 28 || $$len < $mlen;
	$$off += 8; $$len -= 8; $self->{li} -= 8;
	my ( $t, $l ) = unpack('nn',substr($$dat,$$off,4));
	return -1 if $t != 0x1 and $t != 0x3;
	return -1 if $t == 0x1 and $l != 8;
	$$off += 4; $$len -= 4;
	if ($t == 0x1) {
		$self->{iid} = unpack('N',substr($$dat,$$off,4));
	} else {
		$self->{iid} = substr($$dat,$$off,$l-4);
	}
	my $plen = ($l+3)&(~0x3) - 4;
	$$off += $plen; $$len -= $plen;
	( $t,$l ) = unpack('nn',substr($$dat,$$off,4));
	return -1 if $t != 0x300 && $t != 0x301;
	$$off += 4; $$len -= 4;
	$plen = ($l+3)&(~0x3) - 4;
	$$len = $$off + $plen;
	if ($t == 0x301) {
		$datalink->setrt(::RT_24BIT_PC);
		$datalink->setpr(::MP_JAPAN);
		$self->{li0} = unpack('C',substr($$dat,$$off,1)) >> 6;
		$self->{mp} = $self->{li0};
		$$off++; $$len--;
	}
	my $interface = M2UA_Interface->get($top,$network,$datalink,$self->{iid},$self);
	$self->{interface} = $interface;
	bless $self, 'Mtp3Message';
	return $self->decode($interface,@args);
}

# -------------------------------------
package M3uaMessage; use strict;
@M3uaMessage::ISA = qw(Message);
# -------------------------------------
# The M3UA message contains an M3UA messag header (4-bytes), some number of
# optional TLV parametes, an RC TLV parameter, a protocol data parameter that
# contains the sio and all routing label fields.  The pdu starts with the first
# by of the MTP User part message following the routing label.
# -------------------------------------

sub decode {
	my ($self,$top,$network,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	return -1 if 4 < $$len; 
	return -1 if unpack('N',substr($$dat,$$off,4)) != 0x01000101;
	$$off+=4; $$len-=4;
	my ($doff,$dlen);
	while ($$len >= 4) {
		my ($ptag,$plen) = unpack('nn',substr($$dat,$$off,4));
		     if ($ptag == 0x0200) { # NA
			return -1 if $plen != 8 || $plen > $$len;
			$self->{na} = unpack('N',substr($$dat,$$off+4,4));
		} elsif ($ptag == 0x0006) { # RC
			return -1 if $plen != 8 || $plen > $$len;
			$self->{rc} = unpack('N',substr($$dat,$$off+4,4));
		} elsif ($ptag == 0x0210) { # Protocol data
			return -1 if $plen < 12 || $plen > $len;
			(
				$self->{opc},
				$self->{dpc},
				$self->{si},
				$self->{ni},
				$self->{mp},
				$self->{sls},
			) =
			unpack('NNCCCC', substr($$dat,$$off+4,12));
			$dlen = $plen - 16;
			$doff = $$off + 16;
		} elsif ($ptag == 0x0013) { # Correlation Id
			return -1 if $plen != 8 || $plen > $$len;
			$self->{cid} = unpack('N',substr($$dat,$$off+4,4));
		} else {
		}
		$plen = ($plen+3)&(~0x3);
		return -1 if $plen > $$len; 
		$$off += $plen; $$len -= $plen;
	}
	return -1 unless defined $dlen and defined $doff;

	unless (exists $mtypes{$self->{si}}) {
		print STDERR "W: $self: no message type for si=$self->{si}\n";
	}
	if ($self->{ni} == 0) {
		$datalink->setrt(::RT_14BIT_PC);
		$datalink->setpr(::MP_INTERNATIONAL);
	}
	if ($datalink->{pr} == ::MP_UNKNOWN) {
		$datalink->setpr(::MP_NATIONAL) if $self->{mp} != 0;
	} elsif ($datalink->{pr} == ::MP_INTERNATIONAL) {
		$self->{mp} = 0;
	}
	$self->setrt(::RT_24BIT_PC) if ($self->{dpc}|$self->{opc})&(~0x3fff);
	$self->Setpr(::MP_NATIONAL) if $self->{mp} != 0;

	$self->{rc} = 0 unless exists $self->{rc};
	my $routing = M3UA_Routing->get($top,$network,$datalink,$self->{rc},$self);
	$self->{routing} = $routing;
	return 1;
}

# -------------------------------------
package SuaMessage; use strict;
@SuaMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
}

# -------------------------------------
package Mtp2Message; use strict;
@Mtp2Message::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	if ($datalink->{ht} == ::HT_UNKNOWN) {
		if (3 <= $$len && $$len <= 5) {
			$datalink->setht(::HT_BASIC);
		} elsif (6 <= $$len && $$len <= 8) {
			$datalink->setht(::HT_EXTENDED);
		} else {
			return 0;
		}
	}
	if ($datalink->{ht} == ::HT_BASIC) {
		(
			$self->{bsn},
			$self->{fsn},
			$self->{li},
		)
		= unpack('CCC',substr($$dat,$$off,3));

		$self->{bib} = ($self->{bsn} >> 7) & 0x01; $self->{bsn} &= 0x7f;
		$self->{fib} = ($self->{fsn} >> 7) & 0x01; $self->{fsn} &= 0x7f;
		$self->{li0} = ($self->{li}  >> 6) & 0x03; $self->{li}  &= 0x3f;
		$$off += 3; $$len -= 3;
	} else {
		(
			$self->{bsn},
			$self->{fsn},
			$self->{li},
		)
		= unpack('vvv',substr($$dat,$$off,6));

		$self->{bib} = ($self->{bsn} >> 15) & 0x0001; $self->{bsn} &= 0x0fff;
		$self->{fib} = ($self->{fsn} >> 15) & 0x0001; $self->{fsn} &= 0x0fff;
		$self->{li0} = ($self->{li}  >> 14) & 0x0003; $self->{li}  &= 0x01ff;
		$$off += 6; $$len -= 6;
	}
	if (($self->{li} != $$len) && ($$len <= 63 || $self->{li} != 63)) {
		print STDERR "W: $self: bad length indicator $self->{li} != $$len\n";
	}
	if ($self->{li0} != 0) {
		$datalink->setrt(::RT_24BIT_PC);
		$datalink->setpr(::MP_JAPAN);
	}
	if ($self->{li} == 0) {
		bless $self,'FisuMessage';
		return 1;
	}
	$self->{si} = unpack('C',substr($$dat,$$off,1));
	if ($self->{li} < 2) {
		$$off += 1; $$len -= 1;
		bless $self,'LssuMessage';
		return 1;
	}
	if ($self->{li} < 3) {
		$$off += 2; $$len -= 2;
		bless $self,'Lss2Message';
		return 1;
	}
	bless $self,'Mtp3Message';
	return $self->decode($datalink,@args);
}

# -------------------------------------
package Mtp3Message; use strict;
@Mtp3Message::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});

	$self->{si} = unpack('C',substr($$dat,$$off,1));
	$self->{ni} = ($self->{si} >> 6) & 0x03;
	$self->{mp} = ($self->{si} >> 4) & 0x03;
	$self->{si} &= 0x0f;
	unless (exists $mtypes{$self->{si}}) {
		print STDERR "W: $self: no message type for si=$self->{si}\n";
	}
	if ($self->{ni} == 0) {
		$datalink->setrt(::RT_14BIT_PC);
		$datalink->setpr(::MP_INTERNATIONAL);
	}
	if ($datalink->{pr} == ::MP_UNKNOWN) {
		$datalink->setpr(::MP_NATIONAL) if $self->{mp} != 0;
	} elsif ($datalink->{pr} == ::MP_JAPAN) {
		$self->{mp} = $self->{li0};
	} elsif ($datalink->{pr} == ::MP_INTERNATIONAL) {
		$self->{mp} = 0;
	}
	if ($self->{li} < ::HT_EXTENDED) {
		print STDERR "W: $self: too short for RL, li = $self->{li}\n";
		return -1;
	}
	if ($self->{li} < 9 || ($self->{si} == 5 && $self->{li} < 11)) {
		$datalink->setrt(::RT_14BIT_PC);
	}
	if ($datalink->{rt} == ::RT_UNKNOWN) {
		my @b = (unpack('C*', substr($$dat, $$off, 11)));
		my $ret = $self->checkRoutingLabelType($self->{si},$datalink,$self->{li},\@b);
		if ($ret <= 0) {
			return $ret;
		}
	}
	return 0 if $datalink->detecting;
	$$off++; $$len--;
	if ($datalink->{rt} == ::RT_14BIT_PC) {
		if ($self->{li} < 6) {
			print STDERR "W: $self: too short for 14-bit RL, li = $self->{li}\n";
			return -1;
		}
		$self->{dpc} = (unpack('v',substr($$dat,$$off+0,2)) >> 0) & 0x3fff;
		$self->{opc} = (unpack('V',substr($$dat,$$off+1,4)) >> 2) & 0x3fff;
		$self->{sls} = (unpack('C',substr($$dat,$$off+3,1)) >> 4) & 0x0f;
		$$off += 4; $$len -= 4;
	} else {
		if ($self->{li} < 9) {
			print STDERR "W: $self: too short for 24-bit RL, li = $self->{li}\n";
			return -1;
		}
		$self->{dpc} = unpack('V',substr($$dat,$$off+0,4)) & 0x00ffffff;
		$self->{opc} = unpack('V',substr($$dat,$$off+3,4)) & 0x00ffffff;
		$self->{sls} = unpack('C',substr($$dat,$$off+6,1));
		$$off += 7; $$len -= 7;
	}
	my $type;
	if ($self->{si} < 4 || $self->{si} == 5) {
		$type = {
			0=>'SnmmMessage',
			1=>'SntmMessage',
			2=>'SnsmMessage',
			3=>'SccpMessage',
			5=>'IsupMessage',
		}->{$self->{si}};
	} else {
		$type = 'UserMessage';
	}
	bless $self,$type;
	return 1;
}

# -------------------------------------
package SnmmMessage; use strict;
@SnmmMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	$self->{mt} = unpack('C',substr($$dat,$$off,1));
	$self->{mt} = (($self->{mt} & 0x0f) << 4) | ($self->{mt} >> 4);
	unless (exists $mtypes{$self->{si}}->{$self->{mt}}) {
		print STDERR "W: $self: no message type for si=$self->{si}, mt=$self->{mt}\n";
		return -1;
	}
	return 1;
}

# -------------------------------------
package SntmMessage; use strict;
@SntmMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	$self->{mt} = unpack('C',substr($$dat,$$off,1));
	$self->{mt} = (($self->{mt} & 0x0f) << 4) | ($self->{mt} >> 4);
	unless (exists $mtypes{$self->{si}}->{$self->{mt}}) {
		print STDERR "W: $self: no message type for si=$self->{si}, mt=$self->{mt}\n";
		return -1;
	}
	$$off++; $$len--;
	$self->{dlen0} = unpack('C',substr($$dat,$$off,1)); $$off++; $$len--;
	$self->{dlen} = $self->{dlen0} >> 4; $self->{dlen0} &= 0x0f;
	if ($datalink->{rt} == ::RT_14BIT_PC) {
		$self->{slc} = $self->{sls};
	} else {
		$self->{slc} = $self->{dlen0};
		delete $self->{dlen0};
	}
	$self->{tdata} = substr($$dat,$$off,$self->{dlen});
	$$off+=$self->{dlen}; $$len-=$self->{dlen};
	bless $self, {0x11=>'SltmMessage',0x12=>'SltaMessage'}->{$self->{mt}};
	return 1;
}

# -------------------------------------
package SnsmMessage; use strict;
@SnsmMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	$self->{mt} = unpack('C',substr($$dat,$$off,1));
	$self->{mt} = (($self->{mt} & 0x0f) << 4) | ($self->{mt} >> 4);
	unless (exists $mtypes{$self->{si}}->{$self->{mt}}) {
		print STDERR "W: $self: no message type for si=$self->{si}, mt=$self->{mt}\n";
		return -1;
	}
	$$off++; $$len--;
	$self->{dlen0} = unpack('C',substr($$dat,$$off,1)); $$off++; $$len--;
	$self->{dlen} = $self->{dlen0} >> 4; $self->{dlen0} &= 0x0f;
	if ($datalink->{rt} == ::RT_14BIT_PC) {
		$self->{slc} = $self->{sls};
	} else {
		$self->{slc} = $self->{dlen0};
		delete $self->{dlen0};
	}
	$self->{tdata} = substr($$dat,$$off,$self->{dlen});
	$$off+=$self->{dlen}; $$len-=$self->{dlen};
	bless $self, {0x11=>'SltmMessage',0x12=>'SltaMessage'}->{$self->{mt}};
	return 1;
}

# -------------------------------------
package SccpMessage; use strict;
@SccpMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	$self->{mt} = unpack('C',substr($$dat,$$off,1));
	unless (exists $mtypes{$self->{si}}->{$self->{mt}}) {
		print STDERR "W: $self: no message type for si=$self->{si}, mt=$self->{mt}\n";
	}
	return 1;
}

# -------------------------------------
package IsupMessage; use strict;
@IsupMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	($self->{cic},$self->{mt}) = unpack('vC',substr($$dat,$$off,3));
	unless (exists $mtypes{$self->{si}}->{$self->{mt}}) {
		print STDERR "W: $self: no message type for si=$self->{si}, mt=$self->{mt}\n";
	}
	return 1;
}

# -------------------------------------
package UserMessage; use strict;
@UserMessage::ISA = qw(Message);
# -------------------------------------

sub decode {
	my ($self,$datalink,@args) = @_;
	my ($off,$len,$dat) = (\$self->{off},\$self->{len},\$self->{dat});
	$self->{mt} = unpack('C',substr($$dat,$$off,1));
	unless (exists $mtypes{$self->{si}}->{$self->{mt}}) {
		print STDERR "W: $self: no message type for si=$self->{si}, mt=$self->{mt}\n";
	}
	return 1;
}




# -------------------------------------
package Layer; use strict;
# -------------------------------------

# -------------------------------------
package PcapLayer; use strict;
@PcapLayer::ISA = qw(Layer);
# -------------------------------------

sub create {
	my ($type,$top,$network,$msg) = @_;
	my $dltype = $msg->{dltype};
	my %dltypes = (
		  1=>'EthernetLayer',
		113=>'LinuxCookedLayer',
		139=>'Mtp2WithPhdrLayer',
	);
	unless (exists $dltypes{$dltype}) {
		print STDERR "E: $msg: wrong data link type $dltype\n";
		return 2;
	}
	return $dltypes{$dltype}->create($top,$network,$msg);
}

# -------------------------------------
package Mtp2WithPhdrLayer; use strict;
@Mtp2WithPhdrLayer::ISA = qw(Layer);
# -------------------------------------

sub create {
	my ($type,$top,$network,$msg) = @_;
	return 2 if 4 > $msg->{len};
	my $hdr = $msg->gethdr(4);
	my ($dir,$xsn,$ppa) = unpack('CCn',$hdr);
	$msg->{ppa} = $ppa;
	$msg->{dir} = ($dir ^ 0x1) & 0x1;
	$msg->{xsn} = $xsn;
	$msg->{ht} = $htoptions[[1,2,0]->[$xsn]]->[1];
	$msg->{datalink} = Channel->get($top,$network,$msg);
	$msg->{datalink}->process($top,$network,$msg);
	return 1;
}

# -------------------------------------
package EthernetLayer; use strict;
@EthernetLayer::ISA = qw(Layer);
# -------------------------------------

sub create {
	my ($type,$top,$network,$msg) = @_;
	return 2 if 14 > $msg->{len};
	my $hdr = $msg->gethdr(14);
	my $etyp = unpack('n',substr($hdr,12,2));
	if ($etyp != 0x0800) {
		printf STDERR "E: $msg: wrong ether type 0x%x08\n", $etyp;
		return 2;
	}
	return IPLayer->create($top,$network,$msg);
}

# -------------------------------------
package LinuxCookedLayer; use strict;
@LinuxCookedLayer::ISA = qw(Layer);
# -------------------------------------

sub create {
	my ($type,$top,$network,$msg) = @_;
	return 2 if 16 > $msg->{len};
	my $hdr = $msg->gethdr(16);
	my ($dir,$etyp) = unpack('nn', substr($hdr,0,2).substr($hdr,14,2));
	if ($etyp != 0x0800) {
		printf STDERR "E: $msg: wrong ether type 0x%x08\n", $etyp;
		return 2;
	}
	$msg->{dir} = $dir;
	return IPLayer->create($top,$network,$msg);
}

# -------------------------------------
package IPLayer; use strict;
@IPLayer::ISA = qw(Layer);
# -------------------------------------

sub create {
	my ($type,$top,$network,$msg) = @_;
	return 2 if 20 > $msg->{len};
	my $byte = unpack('C',substr($msg->{dat},$msg->{off},1));
	my $vers = ($byte & 0xf0) >> 4;
	if ($vers != 4) {
		printf STDERR "E: $msg: only IPv4 supported but vers is $vers\n";
		return 2;
	}
	my $hlen = ($byte & 0x0f) << 2;
	if ($msg->{len} < $hlen) {
		print STDERR "E: $msg: packet too short $hlen > $msg->{len}\n";
		return 2;
	}
	my $hdr = $msg->gethdr($hlen);
	my $plen = unpack('n',substr($hdr,2,2));
	my ($proto,$csum,$saddr,$daddr) = unpack('CnNN',substr($hdr,9,12));
	if ($proto != 132) {
		unless (exists $top->{error}->{ipproto}) {
			print STDERR "E: $msg: packet is not SCTP packet, proto is $proto\n";
			print STDERR "E: $msg: improve your IP capture filter.\n";
			print STDERR "E: $msg: this is your first and only warning.\n";
			$top->{error}->{ipproto} = 1;
		return 2;
	}
	my $dlen = $plen - $hlen;
	if ($dlen > $msg->{len}) {
		print STDERR "E: $msg: packet too short $dlen > $msg->{len}\n";
		return 2;
	}
	$msg->{len} = $dlen;
	$msg->{saddr} = $saddr;
	$msg->{daddr} = $daddr;
	return SCTPLayer->create($top,$network,$msg);
}

# -------------------------------------
package Chunk; use strict;
@Chunk::ISA = qw(Layer);
# -------------------------------------
# We don't keep packets explicitly: we keep chunks.  Chunks refer to the packets that
# contain them, packets do not refer to their chunks.
# -------------------------------------

sub new {
	my ($type,$top,$network,$pkt) = @_;
	my $self = {off=>$pkt->{off},len=>$pkt->{len}};
	bless $self,$type;
	return undef if 4 > $self->{len}; 
	$self->{pkt} = $pkt;
	my $hdr = $pkt->gethdr(4);
	(
		$self->{ctype},
		$self->{flags},
		$self->{clen},
	)
	= unpack('CCn', $hdr);
	$self->{off} += 4;
	$self->{len} = $self->{clen} - 4;
	my $plen = ($self->{clen}+3)^(~0x3) - 4;
	return undef if 0 > $plen;
	$pkt->{off} += $plen;
	$pkt->{len} -= $plen;
	my %ctypes = (
		0=>'DataChunk',		# DATA (0)
		#1=>'InitChunk',	# INIT (1)
		#2=>'InitAckChunk',	# INIT ACK (2)
		3=>'SackChunk',		# SACK (3)
		#4=>'BeatChunk',	# HEARTBEAT (4)
		#5=>'BeatAckChunk',	# HEARTBEAT ACK (5)
		#6=>'AbortChunk',	# ABORT (6)
		7=>'ShutdownChunk',	# SHUTDOWN (7)
		#8=>'ShutdownAckChunk',	# SHUTDOWN ACK (8)
		#9=>'ErrorChunk',	# ERROR (9)
		#10=>'CookieEchoChunk',	# COOKIE ECHO (10)
		#11=>'CookieAckChunk',	# COOKIE ACK (11)
		#14=>'CompleteChunk',	# SHUTDOWN COMPLETE (14)
	);
	if (exists $ctypes{$self->{ctype}}) {
		bless $self, $ctypes{$self->{ctype}};
	} else {
		bless $self,'OtherChunk';
	}
	return $self->decode($top,$network,$pkt);
	if ($self->{ctype} == 0) { # DATA
		return undef if 12 > $pkt->{len};
		(
			$self->{tsn},
			$self->{stream},
			$self->{ssn},
			$self->{ppi},
		)
		= unpack('NnnN', $pkt->gethdr(12));
		$self->{off} += 12;
		$self->{len} -= 12;
		$plen -= 12;
		$pkt->{ppi} = $self->{ppi};
	}
	return $self;
}

# -------------------------------------
package DataChunk; use strict;
@DataChunk::ISA = qw(Chunk);
# -------------------------------------

sub decode {
	my ($self,$top,$network,$pkt) = @_;
	return undef if 12 > $self->{len};
	(
		$self->{tsn},
		$self->{stream},
		$self->{ssn},
		$self->{ppi},
	) = unpack('NnnN', substr($pkt->{dat}, $self->{off}, 12));
	$self->{off} += 12; $self->{len} -= 12;
	$pkt->{ppi} = $self->{ppi};
	return $self;
}

# -------------------------------------
package SackChunk; use strict;
@SackChunk::ISA = qw(Chunk);
# -------------------------------------

sub decode {
	my ($self,$top,$network,$pkt) = @_;
	return undef if 12 > $self->{len};
	(
		$self->{cumm},
		$self->{a_rwnd},
		$self->{nacks},
		$self->{ndups},
	) = unpack('NNnn', substr($pkt->{dat}, $self->{off}, 12));
	$self->{off} += 12; $self->{len} -= 12;
	return $self;
}

# -------------------------------------
package ShutdownChunk; use strict;
@ShutdownChunk::ISA = qw(Chunk);
# -------------------------------------

sub decode {
	my ($self,$top,$network,$pkt) = @_;
	return undef if 4 > $self->{len};
	(
		$self->{cumm},
	) = unpack('N', $pkt->{dat}, $self->{off}, 4);
	$self->{off} += 4; $self->{len} -= 4;
	return $self;
}

# -------------------------------------
package SCTPLayer; use strict;
@SCTPLayer::ISA = qw(Layer);
# -------------------------------------

sub create {
	my ($type,$top,$network,$msg) = @_;
	return 2 if 12 > $msg->{len};
	(
		$self->{sport},
		$self->{dport},
		$self->{vtag},
	) =
	unpack('nnNN', $msg->gethdr(12));
	my ($datalink,$ppi);

	# Unfortunately I want to use SACK and SHUTDOWN chunks to clock out data
	# that was actually receieved by each end.  To do so, requires
	# identifying the association without knowing the PPI value (because a
	# packet could contain just SACK or SHUTDOWN chunks).  This also means,
	# however, that it is not possible to filter on PPI, just on well-known
	# port number because there is no PPi in these desired SACK or SHUTDOWN
	# packets.  So, DATA chunks must belong to associations that have at
	# least one well-known port number and either a corresponding PPI or a
	# PPI of zero.
	#
	if ($datalink = $self->{datalink} = Association->find($top,$network,$msg)) {
		$msg->{ppi} = $datalink->{ppi};
	} else {
		my %ports = ( 3565=>5, 2904=>2, 2905=>3, 14001=>4 );
		$ppi = $ports{$self->{sport}} if !defined $ppi and exists $ports{$self->{sport}};
		$ppi = $ports{$self->{dport}} if !defined $ppi and exists $ports{$self->{dport}};
		if (defined $ppi) {
			my %ppis = ( 2=>'M2UA', 3=>'M3UA', 4=>'SUA', 5=>'M2PA' );
			$datalink->{ppi} = $ppi;
			$datalink = $self->{datalink} = $ppis{$ppi}.'_Association'->get($top,$network,$msg);
		}
	}
	my @chunks = ();
	while ($msg->{len} > 0) {
		my $chunk = Chunk->new($top,$network,$msg);
		return 2 unless $chunk;
		push @chunks, $chunk;
	}
	unless (defined $datalink) {
		return 2 unless defined $msg->{ppi};
		$ppi = $msg->{ppi};
		my %ppis = ( 2=>'M2UA', 3=>'M3UA', 4=>'SUA', 5=>'M2PA' );
		$datalink->{ppi} = $ppi;
		$datalink = $self->{datalink} = $ppis{$ppi}.'_Association'->get($top,$network,$msg);
	}
	$datalink->add_pkt($top,$network,$msg,\@chunks);
	return 1;
}

# -------------------------------------
package ULPLayer; use strict;
@ULPLayer::ISA = qw(Message);
# -------------------------------------

sub new {
	my ($type,$top,$network,$dir,$chunks) = @_;
	my $self = {};
	bless $self,$type;
	$self->{chunks} = $chunks;
	my $first = $chunks->[0];
	$self->{stream} = $first->{stream};
	$self->{ppi} = $first->{ppi};
	$self->{hdr} = $first->{pkt}->{hdr};
	$self->{dir} = $dir;
	$self->{dat} = '';
	foreach my $chunk (@{$chunks}) {
		$self->{dat} .= substr($chunk->{pkt}->{dat},$chunk->{off},$chunk->{len});
	}
	$self->{off} = 0;
	$self->{len} = length($self->{dat});
	return $self;
}


# -------------------------------------
package PcapMessage; use strict;
# -------------------------------------

sub create {
	my ($type,$top,$network,$source,$hdr,$dat,$dltype) = @_;
	my $self = {};
	bless $self,$type;

	$self->{hdr} = {};
	foreach (qw/len caplen tv_sec tv_usec/) {
		$self->{hdr}->{$_} = $hdr->{$_};
	}
	$self->{dat} = $dat;
	$self->{dltype} = $dltype;

	my %dltypes = (
		  1=>'EthernetMessage',
		113=>'LinuxCookedMessage',
		139=>'Mtp2WithPhdrMessage',
	);
	unless (exists $dltypes{$dltype}) {
		print STDERR "E: ".ref($self).": wrong data link type $dltype\n";
		return undef;
	}
	bless $self,$dltypes{$dltype};

	return $self->create($top,$network);
}

sub process {
	my ($self,$top,$network) = @_;

	my $datalink = $self->{type}->get($top,$network,$self);
	$datalink->process($top,$network,$self);
}

# -------------------------------------
package Mtp2WithPhdrMessage; use strict;
@Mtp2WithPhdrMessage::ISA = qw(PcapMessage);
# -------------------------------------

use constant {
	MTP2_ANNEX_A_NOT_USED	    => 0,
	MTP2_ANNEX_A_USED	    => 1,
	MTP2_ANNEX_A_USED_UNKNOWN   => 2,
};

sub create {
	my ($self,$top,$network) = @_;

	$self->{psu} = substr($msg->{dat},0,4,'');
	my ($dir,$xsn,$ppa) = unpack('CCn',$self->{psu});
	$xsn = MTP2_ANNEX_A_USED_UNKNOWN unless 0 <= $xsn and $xsn <= 2;
	my $ht = $htoptions[[1,2,0]->[$xsn]]->[1];
	unless ($ht) {
		my ($card,$span,$slot) = Channel::chan($ppa);
		$ht = $slot ? ::HT_BASIC : ::HT_EXTENDED;
	}
	($self->{dir},$self->{xsn},$self->{ppa},$self->{ht})
		= ($dir,$xsn,$ppa,$ht);

	my $datalink = Channel->get($top,$network,$ppa,$dir,$ht);
	# correct direction after creating datalink
	$self->{dir} = ($self->{dir} ^ 0x1) & 0x1;

	$self->{type} = 'Channel';
	return $self;
}
#package Message;
sub rotext {
	my ($self,$ro) = @_;
	$ro->delete('0.0','end');
	my @b = (unpack('C*',$self->{dat}));
	my @lines = ();
	my ($i,$c,$line);
	while (@b) {
		for ($i = 0; ($i < 16) and @b; $i++) {
			$line .= ' ' if $line;
			$line .= ' ' unless $i & 0x7;
			$line .= sprintf('%02X',shift @b);
		}
		push @lines, $line;
		$line = undef;
	}
	$ro->insert('0.0',join("\n",@lines));
}
#package Message;
sub makeentry {
	my ($self,$hl,$byte,$beg,$end,$val,$name,$entry,$desc,$sub) = @_;
	my @bytes = ();
	my $bits = '';
	my $value;
	if ($sub) {
		my $func = shift @{$sub};
		$value = &$func(@{$sub});
	} else {
		$value = $val;
	}
	my $word = $val;
	for (my $i = 0; $i < $beg; $i++) {
		$bits = '-'.$bits;
	}
	for (my $i = $beg; $i <= $end; $i++) {
		$bits = (($word & 0x1) ? '1' : '0') . $bits;
		$word >>= 1;
		if (($i & 0x7) == 0x7) {
			push @bytes, " $bits";
			$bits = '';
		}
	}
	if (($end & 0x7) != 0x7) {
		for (my $i = $end+1; ($i&0x7) != 0; $i++) {
			$bits = '-'.$bits;
		}
	}
	if (length($bits)) {
		push @bytes, " $bits";
	}
	$bits = join("\n", @bytes);
	my $parent = $entry;
	$parent=~s/\.[^\.]*$//;
	$hl->add($entry,-itemtype=>'text', -text=>$name, -style=>$self->{fldstyle});
	#$hl->indicator('create', $entry, -itemtype=>'image', -image=>'box');
	$hl->itemCreate($entry, 1, -itemtype=>'text', -text=>$bits, -style=>$self->{bitstyle});
	$hl->itemCreate($entry, 2, -itemtype=>'text', -text=>" $value", -style=>$self->{valstyle});
	if (ref($desc) eq 'HASH') {
		$hl->itemCreate($entry, 3, -itemtype=>'text', -text=>" $desc->{$val}", -style=>$self->{desstyle});
	} elsif (ref($desc) eq 'ARRAY') {
		$hl->itemCreate($entry, 3, -itemtype=>'text', -text=>" $desc->[$val]", -style=>$self->{desstyle});
	} else {
		$hl->itemCreate($entry, 3, -itemtype=>'text', -text=>" $desc", -style=>$self->{desstyle});
	}
	$hl->hide('entry',$entry) if $self->{tree}->{$parent} eq '+';
}
#package Message;
sub hlist {
	my ($self,$hl) = @_;
	$hl->delete('all');
	$hl->add('0',-text=>'L2',);
	$hl->itemCreate('0',1,-text=>' ********');
	$hl->itemCreate('0',3,-text=>'L2 Pseudoheader');
	$hl->indicator('create','0',-itemtype=>'image',-image=>$self->{tree}->{0});
	$hl->add('1',-itemtype=>'text',-text=>'L2',);
	$hl->itemCreate('1',1,-text=>' ********');
	$hl->itemCreate('1',3,-text=>'L2 Header');
	$hl->indicator('create','1',-itemtype=>'image',-image=>$self->{tree}->{1});
	my $sio;
	if ($self->{channel}->{ht} == ::HT_EXTENDED) {
		$self->makeentry($hl,0,0,11,$self->{bsn},'BSN','1.2','Backward Sequence Number');
		$self->makeentry($hl,1,7,7,$self->{bib},'BIB','1.1','Backward Indicator Bit');
		$self->makeentry($hl,2,0,11,$self->{fsn},'FSN','1.4','Forward Sequence Number');
		$self->makeentry($hl,3,7,7,$self->{fib},'FIB','1.3','Forward Indicator Bit');
		$self->makeentry($hl,4,0,8,$self->{li},'LI','1.5','Length Indicator');
		$self->makeentry($hl,5,2,7,$self->{li0},'LI0','1.6','Spare Bits');
		$sio = 6;
	} else {
		$self->makeentry($hl,0,0,6,$self->{bsn},'BSN','1.2','Backward Sequence Number');
		$self->makeentry($hl,0,7,7,$self->{bib},'BIB','1.1','Backward Indicator Bit');
		$self->makeentry($hl,1,0,6,$self->{fsn},'FSN','1.4','Forward Sequence Number');
		$self->makeentry($hl,1,7,7,$self->{fib},'FIB','1.3','Forward Indicator Bit');
		$self->makeentry($hl,2,0,5,$self->{li},'LI','1.5','Length Indicator');
		$self->makeentry($hl,2,6,7,$self->{li0},'LI0','1.6','Spare Bits');
		$sio = 3;
	}
	if ($self->{li} > 3) {
		$hl->add('2',-itemtype=>'text',-text=>'L3',);
		$hl->itemCreate('2',1,-text=>' ********');
		$hl->itemCreate('2',3,-text=>'L3 Header');
		$hl->indicator('create','2',-itemtype=>'image',-image=>$self->{tree}->{2});
		my ($desc,$mtyp);
		if (exists $mtypes{$self->{si}}) {
			$mtyp = "\U$mtypes{$self->{si}}->{0}->[0]\E";
			$desc = "Service Indicator: $mtyp";
		} else {
			$mtyp = 'L4';
			$desc = "Service Indicator: ($self->{si}) Unknown.";
		}
		$self->makeentry($hl,$sio,0,3,$self->{si},'SI','2.3',$desc,);
		$self->makeentry($hl,$sio,4,5,$self->{mp},'MP','2.2',{
				0=>'Message Priority 0',
				1=>'Message Priority 1',
				2=>'Message Priority 2',
				3=>'Message Priority 3',});
		$self->makeentry($hl,$sio,6,7,$self->{ni},'NI','2.1',{
				0=>'Network Indicator: International',
				1=>'Network Indicator: International (spare)',
				2=>'Network Indicator: National',
				3=>'Network Indicator: National (spare)',});
		$self->makeentry($hl,4,0,23,$self->{opc},"\nOPC\n",'2.4',"Originating Point Code",[\&SP::pcstring,$self->{opc}]);
		$self->makeentry($hl,7,0,23,$self->{dpc},"\nDPC\n",'2.5',"Destination Point Code",[\&SP::pcstring,$self->{dpc}]);
		$self->makeentry($hl,10,0,7,$self->{sls},'SLS','2.6','Signalling Link Selection');
		$hl->add('3',-itemtype=>'text',-text=>$mtyp,);
		$hl->itemCreate('3',1,-text=>' ********');
		$hl->itemCreate('3',3,-text=>"$mtyp Header");
		$hl->indicator('create','3',-itemtype=>'image',-image=>$self->{tree}->{3});
		if (exists $mtypes{$self->{si}}->{$self->{mt}}) {
			$desc = "Message Type: \U$mtypes{$self->{si}}->{$self->{mt}}->[0]\E";
		} else {
			$desc = "Message Type: ($self->{mt}) Unknown.";
		}
		if ($self->{si} == 0) {
			$self->makeentry($hl,11,0,7,$self->{mt},'MT','3.1',$desc,);
		} elsif ($self->{si} == 1 || $self->{si} == 2) {
			$self->makeentry($hl,11,0,7,$self->{mt},'MT','3.1',$desc,);
			if ($self->{channel}->{rt} == ::RT_24BIT_PC) {
				$self->makeentry($hl,12,0,3,$self->{slc},'SLC','3.2','Signalling Link Code');
			} else {
				$self->makeentry($hl,12,0,3,$self->{slc},'SLC','3.2','Signalling Link Code');
			}
			$self->makeentry($hl,12,4,7,$self->{dlen},'DLEN','3.3','Test Data Length');
		} elsif ($self->{si} == 3) {
			$self->makeentry($hl,11,0,7,$self->{mt},'MT','3.1',$desc,);
			$hl->add('4',-itemtype=>'text',-text=>'L5',);
			$hl->itemCreate('4',1,-text=>' ********');
			$hl->itemCreate('4',3,-text=>'L5 Header');
			$hl->indicator('create','4',-itemtype=>'image',-image=>$self->{tree}->{4});
		} elsif ($self->{si} == 5) {
			$self->makeentry($hl,12,0,15,$self->{cic},"CIC",'3.1','Circuit Identification Code');
			$self->makeentry($hl,14,0,7,$self->{mt},'MT','3.2',$desc,);
			$hl->add('4',-itemtype=>'text',-text=>'L5',);
			$hl->itemCreate('4',1,-text=>' ********');
			$hl->itemCreate('4',3,-text=>'Parameters');
			$hl->indicator('create','4',-itemtype=>'image',-image=>$self->{tree}->{4});
			my $mt = $self->{mt};
		}
	}
}

# -------------------------------------
package FisuMessage; use strict; use vars qw(@ISA); @ISA = qw(Message);
# -------------------------------------
# -------------------------------------
package LssuMessage; use strict; use vars qw(@ISA); @ISA = qw(Message);
# -------------------------------------
# -------------------------------------
package Lss2Message; use strict; use vars qw(@ISA); @ISA = qw(Message);
# -------------------------------------
# -------------------------------------
package MsuMessage; use strict; use vars qw(@ISA); @ISA = qw(Message);
# -------------------------------------
# -------------------------------------
package SnmmMessage; use strict; use vars qw(@ISA); @ISA = qw(MsuMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o) = @_;
	$self->{mt} = $b->[$$o];
	$$o++;
	my $mt = (($self->{mt} & 0x0f) << 4) | ($self->{mt} >> 4);
	if (exists $mtypes{$self->{si}}->{$mt}) {
		bless $self,$mtypes{$self->{si}}->{$mt}->[2];
		$self->decode($b,$$o);
	}
	for ($self->{pdu} = ''; defined $b->[$$o]; $$o++) {
		$self->{pdu} .= sprintf('%02X', $b->[$o]);
	}
}
# -------------------------------------
package ComMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o) = @_;
	if ($self->{channel}->{rt} == ::RT_14BIT_PC) {
		$self->{slc} = $self->{sls};
		$self->{fsnl} = $b->[$$o] & 0x7f;
		$self->{fsnl0} = $b->[$$o] >> 7;
		$$o++;
	} else {
		$self->{slc} = $b->[$$o] & 0x0f;
		$self->{fsnl} = $b->[$$o] >> 4;
		$$o++;
		$self->{fsnl} |= ($b->[$$o] & 0x07) << 4;
		$self->{fsnl0} = $b->[$$o] >> 3;
		$$o++;
	}
}
# -------------------------------------
package CbmMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o) = @_;
	if ($self->{channel}->{rt} == ::RT_14BIT_PC) {
		$self->{slc} = $self->{sls};
		$self->{cbc} = $b->[$$o];
		$$o++;
	} else {
		$self->{slc} = $b->[$$o] & 0x0f;
		$self->{cbc} = $b->[$$o] >> 4;
		$$o++;
		$self->{cbc} |= ($b->[$$o] & 0x0f) << 4;
		$self->{cbc0} = $b->[$$o] >> 4;
		$$o++;
	}
}
# -------------------------------------
package SlmMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o) = @_;
	if ($self->{channel}->{rt} == ::RT_14BIT_PC) {
		$self->{slc} = $self->{sls};
	} else {
		$self->{slc} = $b->[$$o] & 0x0f;
		$self->{slc0} = $b->[$$o] >> 4;
		$$o++;
	}
}
# -------------------------------------
package TfmMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{dest} = $b->[$$o];
		$$o++;
		$self->{dest} |= ($b->[$$o] & 0x3f) << 8;
		$self->{dest0} = $b->[$$o] >> 6;
		$$o++;
	} else {
		$self->{dest} = $b->[$$o];
		$$o++;
		$self->{dest} |= $b->[$$o] << 8;
		$$o++;
		$self->{dest} |= $b->[$$o] << 16;
		$$o++;
	}
}
# -------------------------------------
package UpmMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{dest} = $b->[$$o];
		$$o++;
		$self->{dest} |= ($b->[$$o] & 0x3f) << 8;
		$self->{dest0} = $b->[$$o] >> 6;
		$$o++;
	} else {
		$self->{dest} = $b->[$$o];
		$$o++;
		$self->{dest} |= $b->[$$o] << 8;
		$$o++;
		$self->{dest} |= $b->[$$o] << 16;
		$$o++;
	}
	$self->{upi} = $b->[$$o] & 0x0f;
	$self->{upi0} = $b->[$$o] >> 4;
	$$o++;
}
# -------------------------------------
package CooMessage; use strict; use vars qw(@ISA); @ISA = qw(ComMessage);
# -------------------------------------
# -------------------------------------
package CoaMessage; use strict; use vars qw(@ISA); @ISA = qw(ComMessage);
# -------------------------------------
# -------------------------------------
package CbdMessage; use strict; use vars qw(@ISA); @ISA = qw(CbmMessage);
# -------------------------------------
# -------------------------------------
package CbaMessage; use strict; use vars qw(@ISA); @ISA = qw(CbmMessage);
# -------------------------------------
# -------------------------------------
package EcoMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package EcaMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package RctMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
# -------------------------------------
package TfcMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o) = @_;
	if ($self->{channel}->{rt} == ::RT_14BIT_PC) {
		$self->{dest} = $b->[$$o];
		$$o++;
		$self->{dest} |= ($b->[$$o] & 0x3f) << 8;
		$self->{stat} = $b->[$$o] >> 6;
		$$o++;
	} else {
		$self->{dest} = $b->[$$o];
		$$o++;
		$self->{dest} |= $b->[$$o] << 8;
		$$o++;
		$self->{dest} |= $b->[$$o] << 16;
		$$o++;
		$self->{stat} = $b->[$$o] & 0x3;
		$self->{stat0} = $b->[$$o] >> 2;
		$$o++;
	}
}
# -------------------------------------
package TfpMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package TcpMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package TfrMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package TcrMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package TfaMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package TcaMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package RstMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package RsrMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package RcpMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package RcrMessage; use strict; use vars qw(@ISA); @ISA = qw(TfmMessage);
# -------------------------------------
# -------------------------------------
package LinMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package LunMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package LiaMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package LuaMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package LidMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package LfuMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package LltMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package LrtMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package TraMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
# -------------------------------------
package TrwMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
# -------------------------------------
package DlcMessage; use strict; use vars qw(@ISA); @ISA = qw(SnmmMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{slc} = $self->{sls};
		$self->{sdli} = $b->[$$o];
		$$o++;
		$self->{sdli} |= ($b->[$$o] & 0x0f) << 8;
		$self->{sdli0} = $b->[$$o] >> 4;
		$$o++;
	} else {
		$self->{slc} = $b->[$$o] & 0x0f;
		$self->{sdli} = $b->[$$o] >> 4;
		$$o++;
		$self->{sdli} |= $b->[$$o] << 4;
		$$o++;
		$self->{sdli} |= ($b->[$$o] & 0x03) << 12;
		$self->{sdli0} = $b->[$$o] >> 2;
		$$o++;
	}
}
# -------------------------------------
package CssMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package CnsMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package CnpMessage; use strict; use vars qw(@ISA); @ISA = qw(SlmMessage);
# -------------------------------------
# -------------------------------------
package UpuMessage; use strict; use vars qw(@ISA); @ISA = qw(UpmMessage);
# -------------------------------------
# -------------------------------------
package UpaMessage; use strict; use vars qw(@ISA); @ISA = qw(UpmMessage);
# -------------------------------------
# -------------------------------------
package UptMessage; use strict; use vars qw(@ISA); @ISA = qw(UpmMessage);
# -------------------------------------

# -------------------------------------
package SntmMessage; use strict; use vars qw(@ISA); @ISA = qw(MsuMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$o,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{slc} = $self->{sls};
		$self->{dlen0} = $b->[$$o] & 0x0f;
	} else {
		$self->{slc} = $b->[$$o] & 0x0f;
	}
	$self->{dlen} = $b->[$$o] >> 4;
	$self->{tmsg} = '';
	$$o++;
	for (my $i = 0; $i < $self->{dlen}; $i++) {
		$self->{tmsg} .= sprintf('%02X', $b->[$$o+$i]);
	}
	$$o += $self->{dlen};
}
# -------------------------------------
package SnsmMessage; use strict; use vars qw(@ISA); @ISA = qw(MsuMessage);
# -------------------------------------
sub decode {
	my ($self,@args) = @_;
	SntmMessage::decode(@args);
}
# -------------------------------------
package SltmMessage; use strict; use vars qw(@ISA); @ISA = qw(SntmMessage SnsmMessage);
# -------------------------------------
# -------------------------------------
package SltaMessage; use strict; use vars qw(@ISA); @ISA = qw(SntmMessage SnsmMessage);
# -------------------------------------

# -------------------------------------
package SccpMessage; use strict; use vars qw(@ISA); @ISA = qw(MsuMessage);
# -------------------------------------
# -------------------------------------
package CrMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package CcMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package CrefMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package RlsdMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package RlcMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package Dt1Message; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package Dt2Message; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package AkMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package UdtMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package UdtsMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package EdMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package EaMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package RsrMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package RscMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package ErrMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package ItMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package XudtMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package XudtsMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package LudtMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------
# -------------------------------------
package LudtsMessage; use strict; use vars qw(@ISA); @ISA = qw(SccpMessage);
# -------------------------------------

# -------------------------------------
package IsupMessage; use strict; use vars qw(@ISA); @ISA = qw(MsuMessage);
# -------------------------------------
my %isupparm = (
	0x00 => ['eop',		'End of optional parameters',			'EopIsupParameter'	],
	0x01 => ['clrf',	'Call reference',				'ClrfIsupParameter'	],
	0x02 => ['tmr',		'Transmission medium requirement',		'TmrIsupParameter'	],
	0x03 => ['atp',		'Access transport',				'AtpIsupParameter'	],
	0x04 => ['cdpn',	'Called party number',				'CdpnIsupParameter'	],
	0x05 => ['subn',	'Subsequent number',				'SubnIsupParameter'	],
	0x06 => ['nci',		'Nature of connection indicators',		'NciIsupParameter'	],
	0x07 => ['FCI',		'Forward call indicators',			'FciIsupParameter'	],
	0x08 => ['OFCI',	'Optional forward call indicators',		'OfciIsupParameter'	],
	0x09 => ['CPC',		'Calling party\'s category',			'CpcIsupParameter'	],
	0x0a => ['CGPN',	'Calling party number',				'CgpnIsupParameter'	],
	0x0b => ['RDGN',	'Redirecting number',				'RdgnIsupParameter'	],
	0x0c => ['RDNN',	'Redirection number',				'RdnnIsupParameter',	],
	0x0d => ['CONR',	'Connection request',				'ConrIsupParameter',	],
	0x0e => ['INRI',	'Information request indicators',		'InriIsupParameter',	],
	0x0f => ['INFI',	'Information indicators @',			'InfiIsupParameter',	],
	0x10 => ['COTI',	'Continuity indicators',			'CotiIsupParameter',	],
	0x11 => ['BCI',		'Backward call indicators',			'BciIsupParameter',	],
	0x12 => ['CAUS',	'Cause indicators',				'CausIsupParameter',	],
	0x13 => ['RDI',		'Redirection information',			'RdiIsupParameter',	],
	0x15 => ['CGI',		'Circuit group supervision msg type ind',	'CgiIsupParameter',	],
	0x16 => ['RS',		'Range and status',				'RsIsupParameter',	],
	0x17 => ['CMI',		'Call modification indicators (Blue Book)',	'CmiIsupParameter',	],
	0x18 => ['FACI',	'Facility indicator',				'FaciIsupParameter',	],
	0x19 => ['FAII',	'Facility information indicators (Bellcore)',	'FaiiIsupParameter',	],
	0x1a => ['CUGI',	'Closed user group interlock code',		'CugiIsupParameter',	],
	0x1b => ['INDEX',	'Index (Bellcore)',				'IndexIsupParameter',	],
	0x1c => ['CUGCRI',	'Closed user group check response indicators',	'CugcriIsupParameter',	],
	0x1d => ['USI',		'User service information',			'UsiIsupParameter',	],
	0x1e => ['SPC',		'Signalling point code @ (Bellcore)',		'SpcIsupParameter',	],
	0x20 => ['UUI',		'User to user information',			'UuiIsupParameter',	],
	0x21 => ['CONN',	'Connected number',				'ConnIsupParameter',	],
	0x22 => ['SRIS',	'Suspend/resume indicators',			'SrisIsupParameter',	],
	0x23 => ['TNS',		'Transit network selection @',			'TnsIsupParameter',	],
	0x24 => ['EVNT',	'Event information',				'EvntIsupParameter',	],
	0x25 => ['CAM',		'Circuit assignment map',			'CamIsupParameter',	],
	0x26 => ['CSI',		'Circuit state indicator @',			'CsiIsupParameter',	],
	0x27 => ['ACL',		'Automatic congestion level',			'AclIsupParameter',	],
	0x28 => ['OCDN',	'Original called number',			'OcdnIsupParameter',	],
	0x29 => ['OBCI',	'Optional backward call indicators',		'ObciIsupParameter',	],
	0x2a => ['UUIND',	'User to user indicators',			'UuindIsupParameter',	],
	0x2b => ['ISPC',	'Origination ISC point code (not ANSI)',	'IspcIsupParameter',	],
	0x2c => ['GNOT',	'Generic notification (not ANSI)',		'GnotIsupParameter',	],
	0x2d => ['CHI',		'Call history information (not ANSI)',		'ChiIsupParameter',	],
	0x2e => ['ADI',		'Access delivery information (not ANSI)',	'AdiIsupParameter',	],
	0x2f => ['NSF',		'Network specific facilities @',		'NsfIsupParameter',	],
	0x30 => ['USIP',	'User service information prime',		'UsipIsupParameter',	],
	0x31 => ['PROP',	'Propagation delay counter (not ANSI)',		'PropIsupParameter',	],
	0x32 => ['ROPS',	'Remote operations @',				'RopsIsupParameter',	],
	0x33 => ['SA',		'Service activation @',				'SaIsupParameter',	],
	0x34 => ['UTI',		'User teleservice information (not ANSI)',	'UtiIsupParameter',	],
	0x35 => ['TMU',		'Transmission medium used',			'TmuIsupParameter',	],
	0x36 => ['CDI',		'Call diversion information (not ANSI)',	'CdiIsupParameter',	],
	0x37 => ['ECI',		'Echo control information (not ANSI)',		'EciIsupParameter',	],
	0x38 => ['MCI',		'Message compatibility information',		'MciIsupParameter',	],
	0x39 => ['PCI',		'Parameter compatibility information',		'PciIsupParameter',	],
	0x3a => ['MLPP',	'MLPP preference (Precedence ANSI)',		'MlppIsupParameter',	],
	0x3b => ['MCIQ',	'MCID request indicator (not ANSI)',		'MciqIsupParameter',	],
	0x3c => ['MCIR',	'MCID response indicator (not ANSI)',		'McirIsupParameter',	],
	0x3d => ['HOPC',	'Hop counter (reserved)',			'HopcIsupParameter',	],
	0x3e => ['TMRP',	'Transmission medium requirement prime (not ANSI)','TmrpIsupParameter',	],
	0x3f => ['LN',		'Location number (not ANSI)',			'LnIsupParameter',	],
	0x40 => ['RDNR',	'Redirection number restriction (not ANSI)',	'RdnrIsupParameter',	],
	0x41 => ['FREEP',	'Freephone indicators (reserved) (not ANSI)',	'FreepIsupParameter',	],
	0x42 => ['GREF',	'Generic reference (reserved) (not ANSI)',	'GrefIsupParameter',	],
	0x4e => ['RCAP',	'Redirect capability (ANSI)',			'RcapIsupParameter',	],
	0x5b => ['NMC',		'Network management controls (ANSI)',		'NmcIsupParameter',	],
	0x77 => ['RCNT',	'Redirect counter (ANSI)',			'RcntIsupParameter',	],
	0x7b => ['PCAP',	'Pivot capability (ANSI)',			'PcapIsupParameter',	],
	0x7c => ['PRI',		'Pivot routing indicator (ANSI)',		'PriIsupParameter',	],
	0x81 => ['CGL',		'Calling geodetic location (ANSI)',		'GglIsupParameter',	],
	0x86 => ['PSTA',	'Pivot status (ANSI)',				'PstaIsupParameter',	],
	0x87 => ['PCNT',	'Pivot counter (ANSI)',				'PcntIsupParameter',	],
	0x88 => ['PRFI',	'Pivot routing forward information (ANSI)',	'PrfiIsupParameter',	],
	0x89 => ['PRBI',	'Pivot routing backward information (ANSI)',	'PrbiIsupParameter',	],
	0x8a => ['RSTA',	'Redirect status (ANSI)',			'RstaIsupParameter',	],
	0x8b => ['RFI',		'Redirect forward information (ANSI)',		'RfiIsupParameter',	],
	0x8c => ['RBI',		'Redirect backward information (ANSI)',		'RbiIsupParameter',	],
	0xc0 => ['GNUM',	'Generic number (Generic address ANSI, Bellcore)','GnumIsupParameter',	],
	0xc1 => ['GDIG',	'Generic digits @',				'GdigIsupParameter',	],
	0xc2 => ['OSI',		'Operator services information (ANSI)',		'OsiIsupParameter',	],
	0xc3 => ['EGRESS',	'Egress (ANSI)',				'EgressIsupParameter',	],
	0xc4 => ['JUR',		'Jurisdiction (ANSI)',				'JurIsupParameter',	],
	0xc5 => ['CIDC',	'Carrier identification code (ANSI)',		'CidcIsupParameter',	],
	0xc6 => ['BGROUP',	'Business group (ANSI)',			'BgroupIsupParameter',	],
	0xc7 => ['GNAM',	'Generic name (ANSI)',				'GnamIsupParameter',	],
	0xe1 => ['NOTI',	'Notification indicator (ANSI)',		'NotiIsupParameter',	],
	0xe2 => ['SVACT',	'Service activation (ANSI)',			'SvactIsupParameter',	],
	0xe2 => ['CSPI',	'Carrier service provider identification (ANSI)','CspiIsupParameter',	],
	0xe3 => ['TRNSRQ',	'Transaction request (ANSI, Bellcore)',		'TrnreqIsupParameter',	],
	0xe4 => ['LSPI',	'Local service provider information (ANSI)',	'LspiIsupParameter',	],
	0xe5 => ['CGCI',	'Cc\'t group char ind (ANSI, Bellcore)',	'CgciIsupParameter',	],
	0xe6 => ['CVRI',	'Cc\'t validation resp ind (ANSI, Bellcore)',	'CvriIsupParameter',	],
	0xe7 => ['OTGN',	'Outgoing trunk group numb (ANSI, Bellcore)',	'OtgnIsupParameter',	],
	0xe8 => ['CIN',		'Circuit ident name (ANSI, Bellcore)',		'CinIsupParameter',	],
	0xe9 => ['CLLI',	'Common language loc id (ANSI, Bellcore)',	'ClliIsupParameter',	],
	0xea => ['OLI',		'Originating line info (ANSI, Bellcore)',	'OliIsupParameter',	],
	0xeb => ['CHGN',	'Charge number (ANSI, Bellcore)',		'ChgnIsupParameter',	],
	0xec => ['SVCD',	'Service code indicator (ANSI, Bellcore)',	'SvcdIsupParameter',	],
	0xed => ['SPR',		'Special processing request (ANSI, Bellcore)',	'SprIsupParameter',	],
	0xee => ['CSEL',	'Carrier selection info (ANSI, Bellcore)',	'CselIsupParameter',	],
	0xef => ['NT',		'Network transport (ANSI)',			'NtIsupParameter',	],
	0xf3 => ['ORI',		'Outgoing route identification (Spain)',	'OriIsupParameter',	],
	0xf4 => ['IRI',		'Incoming route identification (Spain)',	'IriIsupParameter',	],
	0xf8 => ['RATE',	'Rate (Spain)',					'RateIsupParameter',	],
	0xf9 => ['IIC',		'Identifier of incoming circuit (Spain)',	'IicIsupParameter',	],
	0xfd => ['TOI',		'Trunk offering information (Singapore)',	'ToiIsupParameter',	],
	0xfd => ['TON',		'Type of notification (Spain)',			'TonIsupParameter',	],
	0xfe => ['CRI',		'Charge rate information (Singapore)',		'CriIsupParameter',	],
	0xff => ['ICCI',	'Call charge information (Singapore)',		'IcciIsupParameter',	],
);
sub oparms {
	my ($self,$b,$p,$e,$rt) = @_;
	my @parms = ();
	for (;;) {
		my $pt = $b->[$$p];
		last unless exists $isupparm{$pt};
		$$p++;
		push @parms, IsupParameter::opt($isupparm{$pt}->[2],$b,$p,$e,$rt);
		last if $pt == 0 or ref $parms[-1] eq 'EopIsupParameter';
	}
	return \@parms;
}
# -------------------------------------
package IamMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{nci} = NciIsupParameter->man($b,$p,$e,$rt);
		$self->{fci} = FciIsupParameter->man($b,$p,$e,$rt);
		$self->{cpc} = CpcIsupParameter->man($b,$p,$e,$rt);
		$self->{tmr} = TmrIsupParameter->man($b,$p,$e,$rt);
		$self->{cdpn} = CdpnIsupParameter->man($b,$p,$e,$rt);
	} else {
		$self->{nci} = NciIsupParameter->man($b,$p,$e,$rt);
		$self->{fci} = FciIsupParameter->man($b,$p,$e,$rt);
		$self->{cpc} = CpcIsupParameter->man($b,$p,$e,$rt);
		$self->{usi} = UsiIsupParameter->man($b,$p,$e,$rt);
		$self->{cdpn} = CdpnIsupParameter->man($b,$p,$e,$rt);
	}
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package SamMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{subn} = SubnIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package InrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{inri} = InriIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package InfMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{infi} = InfiIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package CotMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{coti} = CotiIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package AcmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{bci} = BciIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package ConMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{bci} = BciIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package FotMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package AnmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package RelMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{caus} = CausIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package SusMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{sris} = SrisIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package ResMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{sris} = SrisIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package RlcMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CcrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package RscMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package BloMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package UblMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package BlaMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package UbaMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package GrsMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{rs} = RsIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package CgbMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{cgi} = CgiIsupParameter->man($b,$p,$e,$rt);
	$self->{rs} = RsIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package CguMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{cgi} = CgiIsupParameter->man($b,$p,$e,$rt);
	$self->{rs} = RsIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package CgbaMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{cgi} = CgiIsupParameter->man($b,$p,$e,$rt);
	$self->{rs} = RsIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package CguaMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{cgi} = CgiIsupParameter->man($b,$p,$e,$rt);
	$self->{rs} = RsIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package CmrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{cmi} = CmiIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CmcMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{cmi} = CmiIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CmrjMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{cmi} = CmiIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package FarMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{faci} = FaciIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package FaaMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{faci} = FaciIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package FrjMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{faci} = FaciIsupParameter->man($b,$p,$e,$rt);
	$self->{caus} = CausIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package FadMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_24BIT_PC) {
		$self->{faci} = FaciIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package FaiMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_24BIT_PC) {
		$self->{faci} = FaciIsupParameter->man($b,$p,$e,$rt);
		$self->{faii} = FaiiIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package LpaMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package CsvqMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package CsvrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package DrsMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package PamMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package GraMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{rs} = RsIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package CqmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{rs} = RsIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package CqrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{rs} = RsIsupParameter->man($b,$p,$e,$rt);
	$self->{csi} = CsiIsupParameter->man($b,$p,$e,$rt);
}
# -------------------------------------
package CpgMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{evnt} = EvntIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package UsrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{uui} = UuiIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package UcicMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package CfnMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	$self->{caus} = CausIsupParameter->man($b,$p,$e,$rt);
	$self->{oparms} = $self->oparms($b,$p,$e,$rt);
}
# -------------------------------------
package OlmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
# -------------------------------------
package CrgMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		#FIXME only for Singapore
		$self->{icci} = IcciIsupParameter->man($b,$p,$e,$rt);
		#FIXME only for Singapore and Spain
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package NrmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package FacMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package UptMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package UpaMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package IdrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package IrsMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package SgmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CraMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_24BIT_PC) {
		return;
	}
}
# -------------------------------------
package CrmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_24BIT_PC) {
		$self->{nci} = NciIsupParameter->man($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CvrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_24BIT_PC) {
		$self->{cvri} = CvriIsupParameter->man($b,$p,$e,$rt);
		$self->{cgci} = CgciIsupParameter->man($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CvtMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_24BIT_PC) {
		return;
	}
}
# -------------------------------------
package ExmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_24BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package NonMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		#FIXME Spain only
		$self->{ton} = TonIsupParameter->man($b,$p,$e,$rt);
	}
}
# -------------------------------------
package LlmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		#FIXME Spain only
		return;
	}
}
# -------------------------------------
package CakMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		#FIXME Singapore only
		return;
	}
}
# -------------------------------------
package TcmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		#FIXME Singapore only
		$self->{cri} = CriIsupParameter->man($b,$p,$e,$rt);
		return;
	}
}
# -------------------------------------
package McpMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == ::RT_14BIT_PC) {
		#FIXME Singapore only
		return;
	}
}

# -------------------------------------
package MyOptions;
use strict;
# -------------------------------------

#package MyOptions;
sub assign {
	my $mw = shift;
	#$mw->optionAdd('*font'=>'-*-helvetica-medium-r-*--*-120-*-*-*-*-*-*');
	#$mw->optionAdd('*.Button.*.font'=>'-*-helvetica-bold-r-*--*-120-*-*-*-*-*-*');
	#$mw->optionAdd('*.Label.*.font'=>'-*-helvetica-bold-r-*--*-120-*-*-*-*-*-*');
	#$mw->optionAdd('*font'=>'Arial 9');
	#$mw->optionAdd('*font'=>'Helvetica 10');
	#$mw->optionAdd('*Button*font'=>'Arial-Bold 9');
	#$mw->optionAdd('*Label*font'=>'Arial-Bold 9');
	#$mw->optionAdd('*disabledBackground'=>'dark grey');
	#$mw->optionAdd('*highlightBackground'=>'light grey');
	#$mw->optionAdd('*highlightThickness'=>1);
	#$mw->optionAdd('*borderThickness'=>1);
	#$mw->optionAdd('*borderWidth'=>1);
	#$mw->optionAdd('*border'=>0);
	#$mw->optionAdd('*Menu*Width'=>1);
	#$mw->optionAdd('*Menu*Height'=>1);

	$mw->optionAdd('*Balloon*font'=>'Arial 8');
	#$mw->optionAdd('*padX'=>0);
	#$mw->optionAdd('*padY'=>0);
	#$mw->optionAdd('*relief'=>'groove');
	$mw->optionAdd('*Scrollbar*Width'=>12);
}

# -------------------------------------
package MyPixmaps;
use strict;
# -------------------------------------

my $plusImage = <<'EOF';
/* XPM */
static char * mini_plus_xpm[] = {
"16 14 4 1",
" 	c None",
"a	c black",
"b	c white",
"c	c gray50",
"                ",
"                ",
"  aaaaaaaaaaa   ",
"  a         ab  ",
"  a         ab  ",
"  a    a    ab  ",
"  a    a    ab  ",
"  a  aaaaa  aaaa",
"  a    a    abbb",
"  a    a    ab  ",
"  a         ab  ",
"  a         ab  ",
"  aaaaaaaaaaab  ",
"   bbbbbbbbbbb  "};
EOF

my $minusImage = <<'EOF';
/* XPM */
static char * mini_minus_xpm[] = {
"16 14 4 1",
" 	c None",
"a	c black",
"b	c white",
"c	c gray50",
"                ",
"                ",
"  aaaaaaaaaaa   ",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  a  aaaaa  aaaa",
"  a         abbb",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  aaaaaaaaaaab  ",
"   bbbbbbbbbbb  "};
EOF

my $boxImage = <<'EOF';
/* XPM */
static char * mini_box_xpm[] = {
"16 14 4 1",
" 	c None",
"a	c black",
"b	c white",
"c	c gray50",
"       ab       ",
"       ab       ",
"  aaaaaaaaaaa   ",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  a         aaaa",
"  a         abbb",
"  a         ab  ",
"  a         ab  ",
"  a         ab  ",
"  aaaaaaaaaaab  ",
"   bbbbbbbbbbb  "};
EOF

my $logo8Image = <<'EOF';
/* XPM */
static char * logo8_xpm[] = {
"64 48 424 2",
"  	c None",
". 	c #FFFFFF",
"+ 	c #E4E5F6",
"@ 	c #868BD9",
"# 	c #9EA2E0",
"$ 	c #F2F2F2",
"% 	c #717171",
"& 	c #4C4C4C",
"* 	c #A9A9A9",
"= 	c #E7E8F7",
"- 	c #242EBB",
"; 	c #565EC9",
"> 	c #4B54C6",
", 	c #2832BB",
"' 	c #F7F7FC",
") 	c #EAEAEA",
"! 	c #060606",
"~ 	c #3A3A3A",
"{ 	c #898989",
"] 	c #090909",
"^ 	c #A8A8A8",
"/ 	c #2E38BE",
"( 	c #AEB1E5",
"_ 	c #CCCEEE",
": 	c #3840AE",
"< 	c #121213",
"[ 	c #ACACAC",
"} 	c #171717",
"| 	c #F1F1F1",
"1 	c #DDDEF3",
"2 	c #3841C0",
"3 	c #FAFAFA",
"4 	c #080808",
"5 	c #2B3292",
"6 	c #FCFCFE",
"7 	c #7A7A7A",
"8 	c #9C9C9C",
"9 	c #6269CE",
"0 	c #B5B8E7",
"a 	c #6A6A6A",
"b 	c #4E4E4E",
"c 	c #9499DD",
"d 	c #7E84D6",
"e 	c #353535",
"f 	c #252FBB",
"g 	c #F0F0FA",
"h 	c #DCDCDC",
"i 	c #131313",
"j 	c #4C54C7",
"k 	c #DDDEF4",
"l 	c #2E2E2E",
"m 	c #F9F9F9",
"n 	c #F3F4FB",
"o 	c #373FC0",
"p 	c #F6F6FC",
"q 	c #4D4D4D",
"r 	c #838383",
"s 	c #C3C6EC",
"t 	c #636ACE",
"u 	c #373737",
"v 	c #DBDBDB",
"w 	c #EAEBF8",
"x 	c #343DBF",
"y 	c #FAFAFD",
"z 	c #E4E4E4",
"A 	c #1C1C1C",
"B 	c #5C63CC",
"C 	c #313131",
"D 	c #C7C7C7",
"E 	c #E5E6F7",
"F 	c #2933BC",
"G 	c #FDFDFE",
"H 	c #535353",
"I 	c #5F5F5F",
"J 	c #B0B4E6",
"K 	c #5E65CC",
"L 	c #363636",
"M 	c #BCBCBC",
"N 	c #E3E4F6",
"O 	c #252FBA",
"P 	c #242424",
"Q 	c #F0F0F0",
"R 	c #4D55C7",
"S 	c #3C3C3C",
"T 	c #B7B7B7",
"U 	c #E3E5F6",
"V 	c #2630BB",
"W 	c #383838",
"X 	c #9196DC",
"Y 	c #ACB0E5",
"Z 	c #3B3B3B",
"` 	c #B9B9B9",
" .	c #E6E7F7",
"..	c #2C35BD",
"+.	c #2C2C2C",
"@.	c #929292",
"#.	c #D6D8F2",
"$.	c #4951C6",
"%.	c #343434",
"&.	c #C0C0C0",
"*.	c #333CBF",
"=.	c #1D1D1D",
"-.	c #ECECEC",
";.	c #4049C3",
">.	c #FEFEFE",
",.	c #333333",
"'.	c #CDCDCD",
").	c #FDFDFD",
"!.	c #F0F0F9",
"~.	c #3740C0",
"{.	c #D5D5D9",
"].	c #FBFBFB",
"^.	c #F8F8F8",
"/.	c #7F85D4",
"(.	c #D5D6ED",
"_.	c #D7D7D7",
":.	c #EDEDED",
"<.	c #FCFCFC",
"[.	c #EEEEEE",
"}.	c #E9E9EC",
"|.	c #2C35BB",
"1.	c #535356",
"2.	c #6D6D6D",
"3.	c #DADADA",
"4.	c #D6D6D6",
"5.	c #CFCFCF",
"6.	c #C9C9C9",
"7.	c #C3C3C3",
"8.	c #BEBEBE",
"9.	c #BDBDBD",
"0.	c #A5A7BB",
"a.	c #5A60BA",
"b.	c #292929",
"c.	c #B4B4B4",
"d.	c #989898",
"e.	c #030303",
"f.	c #000000",
"g.	c #767676",
"h.	c #CACACA",
"i.	c #AEAEAE",
"j.	c #0A0A0A",
"k.	c #5D5D5D",
"l.	c #D9D9D9",
"m.	c #F5F5F5",
"n.	c #F3F3F3",
"o.	c #2730BA",
"p.	c #0A0A0B",
"q.	c #B8B8B8",
"r.	c #E6E6E6",
"s.	c #E7E7E7",
"t.	c #E1E1E1",
"u.	c #D5D6DE",
"v.	c #3A43BE",
"w.	c #EBEBEB",
"x.	c #232323",
"y.	c #DEDEDE",
"z.	c #E9E9E9",
"A.	c #E8E8E8",
"B.	c #949494",
"C.	c #9E9E9E",
"D.	c #F7F7F7",
"E.	c #5F66C8",
"F.	c #131317",
"G.	c #DDDDDD",
"H.	c #EFEFEF",
"I.	c #E7E7E8",
"J.	c #4049C1",
"K.	c #414244",
"L.	c #B3B3B3",
"M.	c #6E6E6E",
"N.	c #8E8E8E",
"O.	c #5E5E5E",
"P.	c #A3A3A3",
"Q.	c #F4F4F4",
"R.	c #9CA0D7",
"S.	c #09090F",
"T.	c #3F4596",
"U.	c #73737D",
"V.	c #191919",
"W.	c #151515",
"X.	c #D4D4D4",
"Y.	c #B0B0B0",
"Z.	c #AAAAB3",
"`.	c #111438",
" +	c #E3E3E3",
".+	c #35374D",
"++	c #8183A8",
"@+	c #B1B1B1",
"#+	c #C5C5C5",
"$+	c #7F7F7F",
"%+	c #9A9A9A",
"&+	c #868686",
"*+	c #969696",
"=+	c #4B4B4B",
"-+	c #919191",
";+	c #0B0B0B",
">+	c #414141",
",+	c #282828",
"'+	c #7C7C7D",
")+	c #191E67",
"!+	c #E6E6ED",
"~+	c #1A1A1C",
"{+	c #6F73BA",
"]+	c #141414",
"^+	c #9F9F9F",
"/+	c #575757",
"(+	c #C8C8C8",
"_+	c #B5B5B5",
":+	c #676767",
"<+	c #C6C6C6",
"[+	c #9D9D9D",
"}+	c #999999",
"|+	c #ADADAD",
"1+	c #4F4F4F",
"2+	c #828282",
"3+	c #E2E2E2",
"4+	c #7B7B7B",
"5+	c #8C8C8C",
"6+	c #878787",
"7+	c #404040",
"8+	c #979797",
"9+	c #424242",
"0+	c #242B93",
"a+	c #CCCDE8",
"b+	c #2B2B2B",
"c+	c #5860C4",
"d+	c #505050",
"e+	c #BABABA",
"f+	c #A1A1A1",
"g+	c #C1C1C1",
"h+	c #CECECE",
"i+	c #747474",
"j+	c #494949",
"k+	c #0F0F0F",
"l+	c #5C62B8",
"m+	c #8A8FD2",
"n+	c #E5E5E5",
"o+	c #C4C4C4",
"p+	c #454EC1",
"q+	c #858585",
"r+	c #B6B6B6",
"s+	c #565656",
"t+	c #AFAFAF",
"u+	c #AAAAAA",
"v+	c #2A2A2A",
"w+	c #555555",
"x+	c #757575",
"y+	c #F6F6F6",
"z+	c #070707",
"A+	c #B7B8CD",
"B+	c #4850C1",
"C+	c #E0E0E0",
"D+	c #656565",
"E+	c #474FC0",
"F+	c #464646",
"G+	c #D2D2D2",
"H+	c #D5D5D5",
"I+	c #D0D0D0",
"J+	c #323232",
"K+	c #515151",
"L+	c #5A5A5A",
"M+	c #525252",
"N+	c #5B5B5B",
"O+	c #3942BE",
"P+	c #D7D8E4",
"Q+	c #262626",
"R+	c #5058C3",
"S+	c #4D4E55",
"T+	c #181818",
"U+	c #252525",
"V+	c #737373",
"W+	c #DFDFDF",
"X+	c #4E55C7",
"Y+	c #989CDE",
"Z+	c #5A62CB",
"`+	c #3E3F4A",
" @	c #1E1E1E",
".@	c #C9CCEE",
"+@	c #5057C8",
"@@	c #5B62CB",
"#@	c #9999A6",
"$@	c #454DC5",
"%@	c #E0E1F5",
"&@	c #5D64CC",
"*@	c #EDEEF9",
"=@	c #8C91DB",
"-@	c #545CC9",
";@	c #8B8B8B",
">@	c #5A61CB",
",@	c #707070",
"'@	c #606060",
")@	c #3F3F3F",
"!@	c #7D7D7D",
"~@	c #727272",
"{@	c #111111",
"]@	c #3E47C2",
"^@	c #444444",
"/@	c #5159C8",
"(@	c #909090",
"_@	c #787878",
":@	c #B2B2B2",
"<@	c #D8D8D8",
"[@	c #9B9B9B",
"}@	c #595959",
"|@	c #818181",
"1@	c #5860CB",
"2@	c #6067CD",
"3@	c #C2C2C2",
"4@	c #797979",
"5@	c #CBCBCB",
"6@	c #3A43C1",
"7@	c #E9EAF8",
"8@	c #3E3E3E",
"9@	c #DCDDF4",
"0@	c #989DDF",
"a@	c #D1D1D1",
"b@	c #D9DBF2",
"c@	c #39393A",
"d@	c #656CCE",
"e@	c #DFE0F4",
"f@	c #272727",
"g@	c #2C2D3F",
"h@	c #292E76",
"i@	c #F8F8FC",
"j@	c #414AC3",
"k@	c #FBFBFD",
"l@	c #202020",
"m@	c #A2A2A2",
"n@	c #656CCF",
"o@	c #4F57C8",
"p@	c #B8BBE8",
"q@	c #BFC2EB",
"r@	c #5058C8",
"s@	c #CFD1F0",
"t@	c #8A8A8A",
"u@	c #D9DAF3",
"v@	c #686FD0",
"w@	c #D9DAF2",
"x@	c #F8F9FD",
"y@	c #F5F5FB",
"z@	c #F1F1FA",
"A@	c #F9FAFD",
"B@	c #DADBF3",
"C@	c #7F84D7",
"D@	c #D4D6F1",
"E@	c #7A80D5",
"F@	c #A4A8E2",
"G@	c #9DA2E0",
"H@	c #858AD9",
"I@	c #BDC0EA",
"J@	c #848AD8",
"K@	c #D7D9F2",
"L@	c #C0C3EB",
"M@	c #A0A4E1",
"N@	c #ABAFE5",
"O@	c #D2D4F1",
"P@	c #8F94DC",
"Q@	c #A5A9E3",
"R@	c #F9F9FD",
"S@	c #9A9EDF",
"T@	c #D1D3F0",
"U@	c #D9DBF3",
"V@	c #9095DC",
"W@	c #F2F3FB",
"X@	c #DBDCF3",
"Y@	c #E1E2F5",
"Z@	c #8A8FDA",
"`@	c #CBCDEF",
" #	c #EFF0FA",
".#	c #EEEFF9",
"+#	c #B9BCE9",
"@#	c #D3D5F1",
"##	c #EAEAF8",
"$#	c #F1F2FA",
"%#	c #CBCDEE",
"&#	c #CDCFEF",
"*#	c #989CDF",
"=#	c #BCBFEA",
"-#	c #6D74D1",
";#	c #CCCEEF",
">#	c #D0D2F0",
",#	c #C8CBED",
"'#	c #A1A6E2",
")#	c #C4C7EC",
"!#	c #A7ABE3",
"~#	c #C1C4EB",
"{#	c #797FD5",
"]#	c #5159C9",
"^#	c #C7CAED",
"/#	c #555CCA",
"(#	c #E2E3F6",
"_#	c #5860CA",
":#	c #ABAEE4",
"<#	c #E8E9F7",
"[#	c #9DA1E0",
"}#	c #D8DAF3",
"|#	c #B7BAE8",
"1#	c #6E75D1",
"2#	c #C5C7EC",
"3#	c #8F94DB",
"4#	c #B5B8E8",
"5#	c #A1A5E1",
"6#	c #DADCF3",
"7#	c #6168CD",
"8#	c #7177D2",
"9#	c #CFD2F0",
"0#	c #C6C8ED",
"a#	c #ECEDF9",
"b#	c #BBBEE9",
"c#	c #7F85D7",
"d#	c #8186D7",
"e#	c #D5D6F1",
"f#	c #9CA1E0",
"g#	c #BABDE9",
"h#	c #969ADE",
"i#	c #C9CBEE",
"j#	c #BEC1EA",
"k#	c #878CD9",
"l#	c #999EDF",
"m#	c #7B81D6",
"n#	c #969BDE",
"o#	c #EFEFFA",
"p#	c #B9BDE9",
"q#	c #898FDA",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . + @ # . . . $ % & * . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . = - ; > , ' ) ! ~ { ] ^ . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . / ( . . _ : < [ . . $ } | . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . 1 2 . . . 3 4 5 6 . . . 7 8 . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . 9 0 . . . a b c d . . . 3 e . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . f g . . h i . . j k . . . l m . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . n o p . . q r . . s t . . . u v . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . w x y . z A . . . . B = . . C D . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . E F G . H I . . . . J K . . L M . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . N O . . P Q . . . . . R . . S T . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . U V . * W . . . . . . X Y . Z ` . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . .  ...G +.@.. . . . . . #.$.. %.&.. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . w *.y =.-.. . . . . . . ;.>.,.'.. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . >.>.).).!.~.{.C ].].].3 m m ^.^./.(.W _.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.^.:.3 3 m m :.].].].].<.).>.. . . . . . . . . ",
". . ).^.$ [.:.}.|.1.2.3.3.4.5.6.7.8.9.0.a.b.c.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.9.d.e.f.g.h.i.j.f.k.l.L f.f.f.i >.. . . . . . . . ",
". . ].3 m m.n.Q o.p.q.r.r.s.s.s.z t.t.u.v.P t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.t.b.h h f.w.x.y.| e.^.z.A.) B.C.. . . . . . . . . ",
". >.].].].^.D.$ E.F.G.H.H.H.H.[.) z.z.I.J.K.z.z.z.z.z.z.z.z.z.z.z.z.z.z.z.z.z.L.M.[.:.N.) O.m.m.P.m ^.^.D.x.).. . . . . . . . . ",
". >.m m m m m Q.R.S.$ $ $ $ $ [.-.-.-.-.T.U.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-.-._.V._.$ $ | W.X.m m m m ^.7 Y.. . . . . . . . . . ",
". . <.m.$ | | Q Z.`. + + + + +y.y.y.y.y..+++y.@+` 7 @+y.y.#+$+%+y._.c.&+*+y.y.y.$+4 =+s.z.-+;+>+Q.m m T ,+z.. . . . . . . . . . ",
". . . . ].D.D.D.'+)+!+[.[.[.w.A.A.A.A.A.~+{+A.]+^+D /+(+_+:+<+[+}+|+1+D 2+8.A.A.A.3+4+5+H.H.) 6+O.^.m 7+8+:.. . . . . . . . . . ",
". . . . ).m ^.^.9+0+a+$ n.$ :.[.[.[.[.3+b+c+[.d+[.[.[.*+7+6.e+` >+% <+[.'.f+:.g+[.[.h+i+9.n.$ h.7+^.$ j+. . . . . . . . . . . . ",
">.3 3 3 D.m.H.[.k+l+m+z.z.n+z z z z z o+j+p+` q+z z 5.r+s+t+u+t+8.+.3+z @+_.z v+z.z.w+v j.Q.n.x+7.^.C _.. . . . . . . . . . . . ",
">.m D.y+Q H.w.A.z+A+B+C+y.3.3.3.3.3.3.D+_+E+&+j+3.D C _.F+G+H+M.I++.3.3.4+3.3.J+K+K+L+A.7+b O.M+^.n+N+. . . . . . . . . . . . . ",
". >.m Q.$ $ $ ) ]+v O+P+n+ + + + + + +Q+3+R+S+}+T+>+4. +6.U++.7. +V+ + +i+ + +C+X.W+[.[.w.G.s.m 3 D.3 . . . . . . . . . . . . . ",
". . . . . . . D.=.n.X+Y+. . . . . . h+W . Z+`+. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . y+ @Q..@+@. . . . . . x+&.. @@#@. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . ^.=.n.. $@%@. . . . ^.S . . &@*@C+}+u+2.. <.. . . . ].. . . m.. . <.. 3 . 8.:.o+. $ . . y+. . . . . . . . . . . . ",
". . . . . . . >.]+Q . =@-@. . . . ;@D+. . >@p *+. . . 3+,@'@s.w+)@!@s+. '@g.w.& o+k.~@s+C r+* !@$+(+~ r 5+. . . . . . . . . . . ",
". . . . . . . . {@-.. 6 ]@>.. . . ^@>.. . /@. (@. . . _@. :@<@. :+. |+[@. . ^ &.}+. D.C.}@3.|@W+. b ;@. }+. . . . . . . . . . . ",
". . . . . . . . M+5.. . c 1@. . 2+_@. . . 2@. r D.) f+|@3 f+3@. % D.2+M 3  +| I+{ <.(+s.4@B.^+ +3+&.5@. d.. . . . . . . . . . . ",
". . . . . . . . c.g.. . 6 6@7@. 8@).. . 9@0@. ) q.h.. _.q.r.h D.6.q.A.y+r+h.3 | [.c.h.. C+a@. T (+. D.. G.. . . . . . . . . . . ",
". . . . . . . . >.T+. . . b@x c@D . . . d@e@. . . . . . . . . ^.^.. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . f@|+. . . g@h@. . . i@j@k@. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . D.l@5+m@K+:+n@o@p@q@r@s@. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . H.t@{ 3+. . u@v@&@w@. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . y . . x@. y@>.. . . k@#.' . . . . . . . . k . . . . . . . z@. . . . . . . . . . . . . . . . . . . 6 . A@. . . . . . . ",
". . . . B@C@. D@E@F@G@H@. . . I@*@G . . . . . . . y J@. . . . . . K@p@L@. . . . . . . . . . . . . . . . U I@ .. ( . . . . . . . ",
". . . . M@N@O@P@Q@. . 0 . . R@S@. T@. U@O@. . 7@V@W@X@' Y@W@. . Z@6 . . `@G . p *@7@7@#.6 U  #x@.#. . >.+#@#X = Q@. p ##$##.. . ",
". . . . . %#X@. + n &#U@. . . *@9@*#=#-#>.. . ;#>#n ,#+ p@'#. . x . . U c k@)#!#~#{#]#@@^#/#!#( (#. . . . X@_#I@:#<#[#}#|#1#. . ",
". . . 2#3#X@4#5#6#. 7#. . . + *@8#9#0#a#. . . Q@s .@b#T@@ . . . s c#>#d#g e#%#f#K@g#h#N@d@i#b#j#. . . k#2#. l#. c#m#n#.#o#p#. . ",
". . . . . . . . . . .#. . . q#p . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .#. . . . . . . ",
". . . . . . . . . . . . . . F@G . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . "};
EOF
my $logoImage = <<'EOF';
/* XPM */
static char *openss7-logo-small[] = {
/* columns rows colors chars-per-pixel */
"64 64 191 2",
"   c #020202",
".  c #020206",
"X  c #060606",
"o  c gray4",
"O  c #0e0e0e",
"+  c gray7",
"@  c #161616",
"#  c #161a1a",
"$  c gray10",
"%  c #1a1a1e",
"&  c #1e1e1e",
"*  c #060a22",
"=  c #0e1232",
"-  c #1a1a22",
";  c #222222",
":  c gray15",
">  c #2a2a2a",
",  c gray18",
"<  c #262632",
"1  c #323232",
"2  c gray21",
"3  c #3a3a3a",
"4  c #3e3e3e",
"5  c #1a1e46",
"6  c #3e3e52",
"7  c gray26",
"8  c #464646",
"9  c gray29",
"0  c #4e4e4e",
"q  c #4e4e52",
"w  c gray32",
"e  c #565656",
"r  c #5a5a5a",
"t  c gray37",
"y  c #626262",
"u  c gray40",
"i  c #6a6a6a",
"p  c gray43",
"a  c #6e6e72",
"s  c #727272",
"d  c #767676",
"f  c gray48",
"g  c #7e7e7e",
"h  c #1e2696",
"j  c #1e2ab2",
"k  c #2a2e86",
"l  c #222aba",
"z  c #222eba",
"x  c #262eba",
"c  c #2632ba",
"v  c #2632be",
"b  c #2a32ba",
"n  c #2a32be",
"m  c #2a36be",
"M  c #2e36be",
"N  c #2e3abe",
"B  c #323abe",
"V  c #2e3ac2",
"C  c #363ec2",
"Z  c #3642c2",
"A  c #3a42c2",
"S  c #3e46c2",
"D  c #3e46c6",
"F  c #7e7e8a",
"G  c #7276a2",
"H  c #424ac6",
"J  c #464ec6",
"K  c #4a52c6",
"L  c #4e56ca",
"P  c #525aca",
"I  c #565aca",
"U  c #565eca",
"Y  c #5a62ca",
"T  c #5a62ce",
"R  c #5e62ce",
"E  c #5e66ce",
"W  c #626ace",
"Q  c #666ece",
"!  c #6a72d2",
"~  c #6e76d2",
"^  c #7276d2",
"/  c #7276d6",
"(  c #727ad2",
")  c #727ad6",
"_  c #767ad6",
"`  c #767ed6",
"'  c #7a7ed6",
"]  c #7a82d6",
"[  c #7e86da",
"{  c gray51",
"}  c #868686",
"|  c gray54",
" . c #8e8e8e",
".. c #8a8a9e",
"X. c #929292",
"o. c gray59",
"O. c #9a9a9a",
"+. c gray62",
"@. c #a2a2a2",
"#. c gray65",
"$. c #aaaaaa",
"%. c #aeaeae",
"&. c #b2b2b2",
"*. c #b6b6b6",
"=. c gray73",
"-. c gray",
";. c #8286da",
":. c #828ada",
">. c #868ada",
",. c #868eda",
"<. c #8a8eda",
"1. c #8a92da",
"2. c #8e92da",
"3. c #8e92de",
"4. c #8e96de",
"5. c #9296de",
"6. c #929ade",
"7. c #969ade",
"8. c #969ede",
"9. c #9a9ee2",
"0. c #9ea2e2",
"q. c #9ea6e2",
"w. c #bebece",
"e. c #b6b6d6",
"r. c #a2a2e2",
"t. c #a2a6e2",
"y. c #a6aae2",
"u. c #aaaae6",
"i. c #aaaee6",
"p. c #aeb2e6",
"a. c #b2b6e6",
"s. c #b6b6ea",
"d. c #b6baea",
"f. c #babeea",
"g. c #bebeea",
"h. c #bec2ea",
"j. c #bec2ee",
"k. c gray76",
"l. c #c6c6c6",
"z. c #c6c6ca",
"x. c #cacaca",
"c. c #cecece",
"v. c #cacad2",
"b. c #cacade",
"n. c #cecede",
"m. c #d2d2d2",
"M. c gray84",
"N. c #d6d6de",
"B. c #dadada",
"V. c #dadade",
"C. c gray87",
"Z. c #c2c2ee",
"A. c #c2c6ee",
"S. c #c6c6ee",
"D. c #c6caee",
"F. c #cacee6",
"G. c #cacaee",
"H. c #caceee",
"J. c #ceceee",
"K. c #ced2f2",
"L. c #d6d6e2",
"P. c #dadae2",
"I. c #dedee6",
"U. c #d2d2f2",
"Y. c #d2d6f2",
"T. c #d6d6f2",
"R. c #d6daf2",
"E. c #dadaf2",
"W. c #dadef6",
"Q. c #dedef6",
"!. c #dee2f6",
"~. c #e2e2e2",
"^. c #e6e6e6",
"/. c #eaeaea",
"(. c #eeeeee",
"). c #e2e2f6",
"_. c #e6e6f6",
"`. c #e6eafa",
"'. c #eaeafa",
"]. c #eaeefa",
"[. c #eeeefa",
"{. c gray95",
"}. c #f6f6f6",
"|. c #f2f2fa",
" X c #f6f6fa",
".X c #f6f6fe",
"XX c #f6fafe",
"oX c gray98",
"OX c #fafafe",
"+X c #fafefe",
"@X c #fefefe",
/* pixels */
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XOXa.:.` ;.y.'.@X@X@X@X@X@XoX%.r , & , t c.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XJ.S j x J H z c ` '.@X@X@Xg >   X 7 u 1 X X } @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XH.m l Q _.@X@XQ.' c C h.@Xd   X i ^.@X@X@X&.@    .@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X|.A l ` @X@X@X@X@X@XJ.S z 6   $ g @X@X@X@X@X@Xc.O o m.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X,.l J .X@X@X@X@X@X@X@X_.5   - C.@X@X@X@X@X@X@X@XX.  0 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X|.N l Z.@X@X@X@X@X@X@X@Xs   * h i.@X@X@X@X@X@X@X@X}.; X B.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xu.z J @X@X@X@X@X@X@X@X@.  O w.S n K.@X@X@X@X@X@X@X@X}   g @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XW j 4.@X@X@X@X@X@X@XM.O   %.@XT.m A [.@X@X@X@X@X@X@XC.  1 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XXXM l Y.@X@X@X@X@X@XoX2   p @X@X@Xy.z ^ @X@X@X@X@X@X@X@X> X (.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XT.l B .X@X@X@X@X@X@X|   : {.@X@X@X@XW j d.@X@X@X@X@X@X@Xu   =.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xp.l U @X@X@X@X@X@X~.o   =.@X@X@X@X@X_.M A .X@X@X@X@X@X@XX.   .@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X3.l _ @X@X@X@X@X@Xe   r @X@X@X@X@X@X@X9.x >.@X@X@X@X@X@X*.  p @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X) j 5.@X@X@X@X@Xl.  o ~.@X@X@X@X@X@X@XXXJ N ).@X@X@X@X@Xm.  w @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XE z y.@X@X@X@X@X9 X d @X@X@X@X@X@X@X@X@Xa.x ` @X@X@X@X@X/.  3 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XL l s.@X@X@X@Xk.  O /.@X@X@X@X@X@X@X@X@X@XP V ).@X@X@X@X}.O 1 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XK l f.@X@X@X@X9 X f @X@X@X@X@X@X@X@X@X@X@Xf.M ] @X@X@X@X}.O : @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XK z h.@X@X@Xm.X o ^.@X@X@X@X@X@X@X@X@X@X@XoXP H ].@X@X@XoX$ , @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XK c A.@X@X@Xp O y @X@X@X@X@X@X@X@X@X@X@X@X@Xp.A 7.@X@X@X}.& , @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XR A h.@X@X(.$ @ x.@X@X@X@X@X@X@X@X@X@X@X@X@X|.T W @X@X@XoX2 0 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X[ Y A.@X@X$., 0 @X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xt.! J.@X@X(.0 p @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xy.,.H.@X@Xg e *.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X`.1.p.@X@X(.p X.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XG.u.U.@X(.{ g (.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xa.i..X@X^.X.=.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XW.h.E.@XM.+.*.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XW.d.!.@X^.@.m.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X_.G.R.@Xx.*.C.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X|.h.J.@XC.*.C.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@Xx.$.x.@X@X@X@X@X@X@X@X|.U.E.}.x.l.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xz.$.l.Y.G.OXB.-.(.^.$.$.@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X$.& 2 e $ & x.@X@X@X@X@X@XXXR.E.^.c.B.@X@X@X@X@X@X@X@X@X@X@X@X@X@X^.3 & e 3 % e.'.m.k.p   e e   $.@X@X .e e e e e e @X@X",
"@X@X@Xx.  $.@X@X@X .  ^.@X@X@X@X@X@XR.R.V.m./.@X@X@X@X@X@X@X@X@X@X@X@X@X@Xs 3 @X@X@Xw @ W.c.O.  ~.@X@X$.  x.@Xl.$.$.$.$.e e @X@X",
"@X@X@X2 s @X@X@X@X@X3 2 @X@X@X@Xx.$.!.E.m.m.}.^.$.x.@X@X@X@X@X@X$.x.@X@X@X  $.@X@X@X@X  ..l.8 w @X@X@X@Xe w @X@X@X@X@X~.& ~.@X@X",
"@X@X^.  x.@X@X@X@X@X$.  ^.$.& w 3 3 % #.m.M.w & w & e @X@Xe e 3 w   e @X@X  s @X@X@X@XQ.b.l.9 & @X@X@X@X@X@X@X@X@X@X@X2  .@X@X@X",
"@X@X$.  @X@X@X@X@X@X@X  $.$.  s @X@Xq # m.%.& ~.@X~.&  .@Xe   ~.@Xl.  $.@X .  & s $.@X].F.l.*.&   e $.~.@X@X@X@X@X@Xx.3 @X@X@X@X",
"@X@X$.  @X@X@X@X@X@X@X  $.$.  ~.@X@XP.  #.0 s @X@X@X .e @Xw e @X@X@X  $.@X@Xl. .3     p v.l.}.~.$.e     3 ^.@X@X@X@X3  .@X@X@X@X",
"@X@X$.  ^.@X@X@X@X@X~.  $.$.  @X@X@X@X  | w           e @Xe e @X@X@X  $.@X@X@X@X@X@X .  a l.oX@X@X@X@Xl.& 2 @X@X@Xl.& @X@X@X@X@X",
"@X@X@X   .@X@X@X@X@X .  @X$.  @X@X@X X   .q  .@X@X@X@X@X@Xe e @X@X@X  $.$.  @X@X@X@X@Xw 7 v.  $.@X@X@X@X$.  @X@X@Xw s @X@X@X@X@X",
"@X@X@Xs & ^.@X@X@X^.$ s @X$.   .@X@X}   m.F 3 @X@X@X3 s @Xw e @X@X@X  $.^.  $.@X@X@X@X$ r b.2 e @X@X@X@Xs & @X@X@X  $.@X@X@X@X@X",
"@X@X@X@Xw &  .$. .& e @X@X$.  &  . .$ t N.k.& 3 $.s & ^.@Xe e @X@X@X  $.@X$.  s $. .& @ %.F.n.& 3 $.$.3   $.@X@Xl.  @X@X@X@X@X@X",
"@X@X@X@X@Xl.e e s l.@X@X@X$.  ~.e e g m.Y.R.P.s e s ~.@X@Xl.l.@X@X@X$.~.@X@Xl.s e e  .l.x.J.!.~. .e e s ~.@X@X@X~.$.@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X$.  @X@X@XV.m.L.R.'.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X(.-.N.G.Y.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X$.  @X@X@Xm.x.`.K.R.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xc.&.(.A.A.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X-.=.}.S.j.|.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XoX#.#.oXg.p.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X{.+.@.OXA.r.D.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XM.g =.@Xd.6.|.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X^.p g @XW./ [ +X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X} w ~.@Xi.` _.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xc.7 t @XOXQ J S.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X(., w oX@Xt.I Y.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X-.; w @X@X0.Z ~ @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X| + +.@X@Xq.D D.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X=.+ 9 @X@X'.b V _.@X@X@X@X@X@X@X@X@X@X@X@X@XoX: $ (.@X@X0.M f.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X&.X 8 @X@X@X^ j :.@X@X@X@X@X@X@X@X@X@X@X@X@X#.  f @X@X@Xq.b f.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X*.X 7 @X@X@XT.x n [.@X@X@X@X@X@X@X@X@X@X@X@X2 X C.@X@X@X0.n d.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xk.  2 @X@X@X@XW z 5.@X@X@X@X@X@X@X@X@X@X@X&.X u @X@X@X@X7.c A.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xm.  > @X@X@X@XY.l M [.@X@X@X@X@X@X@X@X@X@X2 X C.@X@X@X@X<.x J.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X^.  @ @X@X@X@X@X! z >.@X@X@X@X@X@X@X@X@X#.. s @X@X@X@X@X' c !.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XoXo   (.@X@X@X@XQ.v n !.@X@X@X@X@X@X@X}.; @ /.@X@X@X@X@XE n |.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X1   x.@X@X@X@X@X,.l W @X@X@X@X@X@X@Xf   O.@X@X@X@X@X@XH S @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xt   +.@X@X@X@X@X XS z a.@X@X@X@X@Xm.o 4 oX@X@X@X@X@X|.M W @X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xo.  y @X@X@X@X@X@XS.x C '.@X@X@X}.1 O B.@X@X@X@X@X@XD.x 5.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@XB.  & oX@X@X@X@X@X@X:.l T @X@X@Xp   O.@X@X@X@X@X@X@X>.x G.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X1   k.@X@X@X@X@X@XXXU j <.@X+.  y @X@X@X@X@X@X@X@XJ Z OX@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xo.  w @X@X@X@X@X@X@X'.S z G X 7 oX@X@X@X@X@X@X@Xh.l :.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X{.$ X l.@X@X@X@X@X@X@XW.k . < (.@X@X@X@X@X@X@X@XP x _.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X$.  ; /.@X@X@X@X@XOX| X = j 2.@X@X@X@X@X@X@X5.j <.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@Xd   , c.@X@X@Xx.7 X s I.P j T T.@X@X@X@X8.x U .X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X .O X 3 r ,   4 k.@X@XOX7.M z T 5.3.J l W [.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X/.#.} } O.B.@X@X@X@X@X@X|.y.( Y E ) g.@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X",
"@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X@X"
};
EOF

my $iconImage = <<'EOF';
/* XPM */
static char * ss7view_xpm[] = {
"48 48 510 2",
"  	c None",
". 	c #FFFFFF",
"+ 	c #FEFEFE",
"@ 	c #FBFBFB",
"# 	c #F8F8F8",
"$ 	c #F8F9FD",
"% 	c #D6D8F2",
"& 	c #ACAFE4",
"* 	c #8A8FDA",
"= 	c #8187D7",
"- 	c #9499DD",
"; 	c #B3B6E7",
"> 	c #D4D6F1",
", 	c #F5F5FB",
"' 	c #F0F0F0",
") 	c #C7C7C7",
"! 	c #8F8F8F",
"~ 	c #5A5A5A",
"{ 	c #404040",
"] 	c #3F3F3F",
"^ 	c #595959",
"/ 	c #989898",
"( 	c #DCDCDC",
"_ 	c #D5D7F2",
": 	c #686FCF",
"< 	c #323BBF",
"[ 	c #2A34BC",
"} 	c #313ABF",
"| 	c #3841C1",
"1 	c #363FC0",
"2 	c #373FBF",
"3 	c #424BC4",
"4 	c #5D65CC",
"5 	c #A7ABE2",
"6 	c #F5F5FC",
"7 	c #F4F4F4",
"8 	c #999999",
"9 	c #373737",
"0 	c #151515",
"a 	c #101010",
"b 	c #1D1D1D",
"c 	c #303030",
"d 	c #2F2F2F",
"e 	c #1A1A1A",
"f 	c #0E0E0E",
"g 	c #1E1E1E",
"h 	c #878787",
"i 	c #F7F7F7",
"j 	c #BEC1EA",
"k 	c #333CBF",
"l 	c #2731BB",
"m 	c #5960CB",
"n 	c #C0C2EA",
"o 	c #F3F4FB",
"p 	c #FCFCFE",
"q 	c #DCDDF3",
"r 	c #9EA2E0",
"s 	c #5E66CC",
"t 	c #3D46C2",
"u 	c #4E56C7",
"v 	c #898CA9",
"w 	c #282828",
"x 	c #7C7C7C",
"y 	c #D3D3D3",
"z 	c #B3B3B3",
"A 	c #2D2D2D",
"B 	c #121212",
"C 	c #555555",
"D 	c #FAFAFA",
"E 	c #D1D3F0",
"F 	c #3039BD",
"G 	c #252EBA",
"H 	c #888EDA",
"I 	c #F7F7FC",
"J 	c #C1C4EB",
"K 	c #25295E",
"L 	c #030308",
"M 	c #07070C",
"N 	c #ECECEC",
"O 	c #3B3B3B",
"P 	c #161616",
"Q 	c #686868",
"R 	c #636ACE",
"S 	c #2C35BC",
"T 	c #6168CD",
"U 	c #F1F1F1",
"V 	c #4F4F4F",
"W 	c #0A0A0A",
"X 	c #13131F",
"Y 	c #2932A3",
"Z 	c #4C54C6",
"` 	c #EEEFF9",
" .	c #191919",
"..	c #BABABA",
"+.	c #BFC2EA",
"@.	c #2B35BD",
"#.	c #2932BC",
"$.	c #E5E6F7",
"%.	c #EAEAEA",
"&.	c #484848",
"*.	c #0D0D0D",
"=.	c #3C3C3C",
"-.	c #F6F6F6",
";.	c #B0B3E6",
">.	c #343DBF",
",.	c #4D54C7",
"'.	c #E0E1F5",
").	c #979797",
"!.	c #FAFAFD",
"~.	c #7F85D7",
"{.	c #323CBE",
"].	c #6B72D0",
"^.	c #FBFBFD",
"/.	c #4B4B4B",
"(.	c #0F0F0F",
"_.	c #434343",
":.	c #F3F3F3",
"<.	c #B8BBE8",
"[.	c #4048C3",
"}.	c #545BC9",
"|.	c #DDDFF4",
"1.	c #F2F2F2",
"2.	c #242424",
"3.	c #D1D1D1",
"4.	c #E7E8F7",
"5.	c #444DC4",
"6.	c #2F39BE",
"7.	c #BBBEE9",
"8.	c #535353",
"9.	c #EBEBEB",
"0.	c #ADB1E5",
"a.	c #434CC4",
"b.	c #5961CA",
"c.	c #EAEAF8",
"d.	c #626262",
"e.	c #2E2E2E",
"f.	c #A3A3A3",
"g.	c #D8DAF3",
"h.	c #313ABE",
"i.	c #333DBF",
"j.	c #DCDDF4",
"k.	c #636363",
"l.	c #0B0B0B",
"m.	c #252525",
"n.	c #E3E3E3",
"o.	c #9CA0DF",
"p.	c #9C9C9C",
"q.	c #292929",
"r.	c #3039BE",
"s.	c #424BC3",
"t.	c #838383",
"u.	c #060606",
"v.	c #D9D9D9",
"w.	c #7E83D6",
"x.	c #3E46C2",
"y.	c #6F76D1",
"z.	c #C2C2C2",
"A.	c #272727",
"B.	c #5F5F5F",
"C.	c #A9ACE3",
"D.	c #565EC9",
"E.	c #EFEFFA",
"F.	c #BCBCBC",
"G.	c #020202",
"H.	c #B7B7B7",
"I.	c #4F57C8",
"J.	c #3A43C1",
"K.	c #A0A4E1",
"L.	c #525252",
"M.	c #9A9DDD",
"N.	c #3C45C0",
"O.	c #6970CF",
"P.	c #353535",
"Q.	c #6E6E6E",
"R.	c #EAEBF8",
"S.	c #2F38BE",
"T.	c #3B43C1",
"U.	c #D9DAF3",
"V.	c #D5D5D5",
"W.	c #333333",
"X.	c #4A4A4A",
"Y.	c #E6E6E6",
"Z.	c #9FA4E0",
"`.	c #5259C8",
" +	c #797DD3",
".+	c #B2B5E6",
"++	c #424AC1",
"@+	c #686FCE",
"#+	c #F7F8FC",
"$+	c #D7D7D7",
"%+	c #363636",
"&+	c #E5E5E5",
"*+	c #B6BAE7",
"=+	c #777DD4",
"-+	c #8F94DA",
";+	c #F4F4FB",
">+	c #1F1F1F",
",+	c #131313",
"'+	c #6269CD",
")+	c #B7BBE7",
"!+	c #3E3E3E",
"~+	c #4E4E4E",
"{+	c #FDFDFE",
"]+	c #D4D5F0",
"^+	c #A0A5E0",
"/+	c #ADB0E4",
"(+	c #F5F5FA",
"_+	c #FDFDFD",
":+	c #3D3D3D",
"<+	c #6A6A6A",
"[+	c #DEDFF3",
"}+	c #878DD9",
"|+	c #969ADB",
"1+	c #F1F1F9",
"2+	c #DDDDDD",
"3+	c #5E5E5E",
"4+	c #696969",
"5+	c #E9EAF6",
"6+	c #C4C7EB",
"7+	c #C6C8EC",
"8+	c #E9E9E9",
"9+	c #898989",
"0+	c #767676",
"a+	c #D4D4D4",
"b+	c #C6C9EC",
"c+	c #AEB2E5",
"d+	c #D9DBF2",
"e+	c #939393",
"f+	c #9D9D9D",
"g+	c #F4F5FB",
"h+	c #D8D9F2",
"i+	c #D7D9F1",
"j+	c #F9F9FC",
"k+	c #EEEEF9",
"l+	c #CACDED",
"m+	c #CED1EE",
"n+	c #FBFBFC",
"o+	c #C3C3C3",
"p+	c #CDCDCD",
"q+	c #FCFCFC",
"r+	c #E1E2F4",
"s+	c #E0E1F4",
"t+	c #F8F8FB",
"u+	c #DADBF2",
"v+	c #DBDBDB",
"w+	c #E8E8E8",
"x+	c #E5E6F6",
"y+	c #E3E4F5",
"z+	c #F7F7FB",
"A+	c #CACACA",
"B+	c #D6D6D6",
"C+	c #F6F6FB",
"D+	c #E2E3F5",
"E+	c #E2E2E2",
"F+	c #DADADA",
"G+	c #6C6C6C",
"H+	c #444444",
"I+	c #A1A1A1",
"J+	c #E8E9F6",
"K+	c #E3E5F5",
"L+	c #CFD0D5",
"M+	c #656565",
"N+	c #4D4D4D",
"O+	c #959595",
"P+	c #EDEDED",
"Q+	c #B5B5B5",
"R+	c #B4B4B4",
"S+	c #7B7B7B",
"T+	c #A2A2A2",
"U+	c #F9F9F9",
"V+	c #E6E8F7",
"W+	c #F3F3F9",
"X+	c #E4E4E4",
"Y+	c #4C4C4C",
"Z+	c #010101",
"`+	c #474747",
" @	c #090909",
".@	c #B9BAC1",
"+@	c #DCDDEE",
"@@	c #5B5B5D",
"#@	c #000000",
"$@	c #414141",
"%@	c #050505",
"&@	c #A8A8A8",
"*@	c #CECECE",
"=@	c #080808",
"-@	c #181818",
";@	c #2A2A2A",
">@	c #707070",
",@	c #F3F3FA",
"'@	c #E4E6F6",
")@	c #E5E6F3",
"!@	c #E1E1E1",
"~@	c #494949",
"{@	c #969696",
"]@	c #5C5D5F",
"^@	c #D2D3E2",
"/@	c #383838",
"(@	c #D2D2D2",
"_@	c #BFBFBF",
":@	c #3A3A3A",
"<@	c #BBBBBB",
"[@	c #B2B2B2",
"}@	c #D8D8D8",
"|@	c #AAAAAA",
"1@	c #C0C0C0",
"2@	c #A5A5A5",
"3@	c #8A8A8A",
"4@	c #C0C1D0",
"5@	c #E1E2EC",
"6@	c #B5B5B6",
"7@	c #A9A9A9",
"8@	c #DEDEDE",
"9@	c #BDBDBD",
"0@	c #F3F3F6",
"a@	c #C9CAD3",
"b@	c #1C1C1C",
"c@	c #141414",
"d@	c #7D7D7D",
"e@	c #C4C4C4",
"f@	c #171717",
"g@	c #B6B6B6",
"h@	c #646464",
"i@	c #1F1F21",
"j@	c #BDBDC1",
"k@	c #6A6A6B",
"l@	c #030303",
"m@	c #CBCBCB",
"n@	c #070707",
"o@	c #B9B9B9",
"p@	c #D9DADE",
"q@	c #5A5A5B",
"r@	c #343434",
"s@	c #A7A7A7",
"t@	c #737374",
"u@	c #909091",
"v@	c #B0B0B0",
"w@	c #797979",
"x@	c #2C2C2C",
"y@	c #828282",
"z@	c #E2E3E4",
"A@	c #D5D5D6",
"B@	c #77777C",
"C@	c #5D5D5D",
"D@	c #747474",
"E@	c #9B9B9B",
"F@	c #474748",
"G@	c #B8B8BC",
"H@	c #040404",
"I@	c #202020",
"J@	c #666666",
"K@	c #9A9A9A",
"L@	c #E0E0E1",
"M@	c #AFAFB0",
"N@	c #CBCCDA",
"O@	c #F3F4FA",
"P@	c #EFEFEF",
"Q@	c #F5F5F5",
"R@	c #929292",
"S@	c #727272",
"T@	c #C5C5C5",
"U@	c #323232",
"V@	c #69696A",
"W@	c #E4E4EE",
"X@	c #161617",
"Y@	c #E7E7E7",
"Z@	c #3A3B3F",
"`@	c #D3D4E2",
" #	c #676767",
".#	c #B1B1B1",
"+#	c #868686",
"@#	c #9F9FA0",
"##	c #E5E6F5",
"$#	c #4D4E51",
"%#	c #616162",
"&#	c #353539",
"*#	c #404041",
"=#	c #111111",
"-#	c #575757",
";#	c #393939",
">#	c #505050",
",#	c #5C5C5C",
"'#	c #EEEEEE",
")#	c #8B8B8C",
"!#	c #86878E",
"~#	c #E0E0E0",
"{#	c #616161",
"]#	c #636369",
"^#	c #121214",
"/#	c #111112",
"(#	c #8D8D8D",
"_#	c #E8E8E9",
":#	c #E6E7F7",
"<#	c #C5C6D2",
"[#	c #3D3D3E",
"}#	c #7A7A7A",
"|#	c #F4F5F8",
"1#	c #D6D7EB",
"2#	c #D3D4EA",
"3#	c #EFF0F6",
"4#	c #F0F0F6",
"5#	c #DFDFDF",
"6#	c #DEE0F4",
"7#	c #CED0EF",
"8#	c #DFE1F4",
"9#	c #CCCCCC",
"0#	c #F0F0F9",
"a#	c #9297DC",
"b#	c #878CD8",
"c#	c #CDCFEE",
"d#	c #B9BCE8",
"e#	c #A4A4A4",
"f#	c #A3A6E1",
"g#	c #535AC8",
"h#	c #757CD3",
"i#	c #F1F2FA",
"j#	c #CCCEEE",
"k#	c #808080",
"l#	c #525AC9",
"m#	c #3C45C1",
"n#	c #ABADE2",
"o#	c #5B5B5B",
"p#	c #565656",
"q#	c #A2A7E0",
"r#	c #6C72D0",
"s#	c #BBBEE8",
"t#	c #B9BCE9",
"u#	c #323BBE",
"v#	c #333CBE",
"w#	c #818181",
"x#	c #959ADC",
"y#	c #5B63CC",
"z#	c #B5B8E6",
"A#	c #7B81D5",
"B#	c #535AC9",
"C#	c #B8B8B8",
"D#	c #1B1B1B",
"E#	c #868CD8",
"F#	c #575FCA",
"G#	c #4851C5",
"H#	c #2D37BD",
"I#	c #8086D7",
"J#	c #232323",
"K#	c #7076D2",
"L#	c #5C63CC",
"M#	c #C8CAED",
"N#	c #787878",
"O#	c #757575",
"P#	c #DADBF3",
"Q#	c #353EC0",
"R#	c #2D36BD",
"S#	c #9CA1E0",
"T#	c #A6A6A6",
"U#	c #CDCFEF",
"V#	c #313ABD",
"W#	c #A9ADE4",
"X#	c #E3E4F6",
"Y#	c #3F48C3",
"Z#	c #676ECF",
"`#	c #ABABAB",
" $	c #999DDE",
".$	c #9297DD",
"+$	c #CFD2F0",
"@$	c #ACACAC",
"#$	c #3942C1",
"$$	c #4851C6",
"%$	c #3B43C2",
"&$	c #2A33BB",
"*$	c #797DB5",
"=$	c #262626",
"-$	c #8C92DB",
";$	c #4A52C6",
">$	c #7F84D7",
",$	c #CDCEDA",
"'$	c #2B2F65",
")$	c #06071A",
"!$	c #13152F",
"~$	c #B4B5D2",
"{$	c #F9FAFD",
"]$	c #B6B9E8",
"^$	c #8E8E8E",
"/$	c #7F7F7F",
"($	c #AFAFAF",
"_$	c #5D5F82",
":$	c #4049C3",
"<$	c #676DCF",
"[$	c #EDEEF9",
"}$	c #E1E2F5",
"|$	c #848AD8",
"1$	c #555DCA",
"2$	c #313131",
"3$	c #909090",
"4$	c #E8E9F7",
"5$	c #969BDE",
"6$	c #565ECA",
"7$	c #414AC3",
"8$	c #3C44C1",
"9$	c #4149C4",
"0$	c #3D45C2",
"a$	c #4B53C6",
"b$	c #878BD8",
"c$	c #EBECF8",
"d$	c #F0F0FA",
"e$	c #CDD0EE",
"f$	c #AFB3E5",
"g$	c #959BDD",
"h$	c #8C91DA",
"i$	c #979CDE",
"j$	c #B4B7E6",
"k$	c #DFE0F4",
". . . . . . . . . . . . . . . + + . . . . . . . . . . . . @ # # @ . . . . . . . . . . . . . . . ",
". . . . . . . . . . . . $ % & * = - ; > , . . . . . ' ) ! ~ { ] ^ / ( . . . . . . . . . . . . . ",
". . . . . . . . . . . _ : < [ } | 1 2 3 4 5 6 . 7 8 9 0 a b c d e f g h i . . . . . . . . . . . ",
". . . . . . . . . . j k l m n o . p q r s t u v d a w x y + . . i z A B C D . . . . . . . . . . ",
". . . . . . . . . E F G H $ . . . . . . I J K L M h N . . . . . . . ' O P Q . . . . . . . . . . ",
". . . . . . . . $ R S T . . . . . . . . U V W X Y Z ` . . . . . . . . ' 0  .... . . . . . . . . ",
". . . . . . . . +.@.#.$.. . . . . . . %.&.*.=.-.;.>.,.'.. . . . . . . . ).b C . . . . . . . . . ",
". . . . . . . !.~.{.].^.. . . . . . N /.(._.:.. . <.[.}.|.. . . . . . . 1.2.c 3.. . . . . . . . ",
". . . . . . . 4.5.6.7.. . . . . . -.8.(.O 9.. . . . 0.a.b.c.. . . . . . D d.e.f.. . . . . . . . ",
". . . . . . . g.h.i.j.. . . . . . k.l.m.n.. . . . . . o.[.4 p . . . . . + p.q.x + . . . . . . . ",
". . . . . . . J r.s.4.. . . . . t.u.*.v.. . . . . . . . w.x.y.. . . . . . z.A.B.7 . . . . . . . ",
". . . . . . + C.k D.E.. . . . F.W G.H.. . . . . . . . . . I.J.K.. . . . . 3.e.L.9.. . . . . . . ",
". . . . . . p M.N.O.o . . . %.P.*.Q.. . . . . . . . . . . R.S.T.U.. . . . V.W.X.Y.. . . . . . . ",
". + . . + + p Z.`. +, . + + h (.w # . . . . . . . . . + . + .+++@+#++ + . $+%+&.&+. . . . . . . ",
"+ . . + + + p *+=+-+;+. + $+>+,+H.+ . . . . + . + . + . + + + w.'+)+. . . v.!+~+Y.. . . + . + + ",
". + . . . + {+]+^+/+(++ _+h :+<+-.. + + . + + + + + + . + . . [+}+|+1++ . 2+3+4+%.. . . . . . . ",
". . + + . + + 5+6+7+I + 8+9+0+a+. + . + + + + + + + + . + . + ^.b+c+d+{+. Y.e+f+7 + . . . + + . ",
". + + + + . + g+h+i+j+. 3...) + + . . . + + . + . + . . + . + . k+l+m+n++ 9.o+p+q++ . + . . + . ",
"+ + . . + + . j+r+s+t+i V.a+U + . . + . + . . . + . + . + . + + . '.u+` _+9.v+w+. . + . + + . . ",
"+ + . . _+8+1.!.x+y+z+Y.A+B+. + . . + . + . + . . + . . . _++ + + C+D+y+q+%.E+U . . + . + . . . ",
"+ . + F+G+H+8.I+J+K+L+M+=.N+O++ P+..Q+R+R+F.-.+ . + + . w+S+T+U++ + V+K+W+X+X+-.. . + . . + . . ",
". + # Y+Z+`+,+ @.@+@@@#@$@e %@&@*@ .u.=@-@;@X++ + . . . ( P.>@7 + . ,@'@)@!@&+D + . . . . + + . ",
"+ . 1.#@~@1.{@#@]@^@e /@(@_@#@:@+ <@H.z Z+P.%.[@X+. F+<@}@|@1@@ + 2@3@4@5@6@7@D 8@9@U ( ... + + ",
". . :.#@w *@. i 0@a@b@c@o+. i -.+ . . d@#@z.e@#@x + N+f@g@w h@+ 3+#@#@i@j@k@l@m@0+n@e@4+=@+ . + ",
"+ . i Y+#@#@=.o@q+p@q@#@#@r@s@+ . . 7 c@%++ F+q.`+D f q.X+g k.a+Z+!+o@Z+t@u@#@f+C #@v@:+q.+ + . ",
"+ . . 7 w@x@ @#@y@z@A@B@d f #@B.. + O+#@&@. i C@P o+#@D@n.g 4+E@Z+C ! -@F@G@H@G+{ *.d@I@J@+ + . ",
"+ + @ _@w+. K@=@B L@M@N@O@Q+H@-@P@+ /.#@Q@. + R@#@S@#@T@( >+4+R@l@e U@W.V@W@X@:+r@~+O =@f.. + + ",
"+ . 7 (.Y+# Y@*.,+v.e.Z@`@. W -@1.w+;@e.+ . . z.b@,+*.. }@g  #.##@J@%.+#@###$#l.c 9+f #@8@+ . + ",
"+ . U+C #@!+%+#@Q !@%##@&#*##@N++ T@=#-#+ + + -.;##@>#. V.-@,#'#f@,+N+ @)#K+!##@U@..=@f Q@+ + + ",
"+ + + ~#{#=#0 S@8@~#y ]#^#/#d.8+. A+~+/ . + + . (#] g@+ !@,#3@@ ) e.(.B._#:#<#[#}#n.^ 0+D + + + ",
". . . . D -.-.# Y.~#U |#1#2#3#. + _+U+@ + . . . @ # q++ _+D @ + . F+3.N q+4.y+4#D + U+D + . + + ",
"+ + . . + + + D 5#( :.. 6#7+7#{++ . + + + . + + + . . . + . + . P+A+p+@ _+8#h+` . . . + . + + . ",
". . . . + . + U+y 9#' . 0#a#b#h+. + . . . + + + + . . + + + . D F.s@( + ^.c#d#s++ + . . + + + + ",
". . + . + . . :.Q+e#Y.. + f#g#h#i#. . . . . + . . + . + + . + (@w@).Q@. p *+-+j#+ . + . . . . . ",
". . + + . . . P+k#M+v.+ . 4.l#m#n#. + + . . + . . . + . + + U+o#p#A++ . p q#r#s#+ + . . . . + . ",
". . + + . + + Y.V W.p+. . + t#u#v#:#. . . + + + . + + . . . }#9 w#+ . . ^.x#y#z#. . . . . + + . ",
". + + . + + . Y@O b@o+. . . . A#r.B#+ . . . . + . . . . . C#D#%+_+. + . !.E#F#<.+ . . . . . + . ",
". . . . . . + U /.b@7@+ . . . i#G#H#I#. . . . . . . . . V.g J#8@. . . . $ K#L#M#. . + . . . . . ",
". . . . . . . _+N#>+O#@ . . . . P#Q#R#S#. . . . . . . ( /@q.9@. . . . . I l#T 6#. . . . . . . . ",
". . . . . . . . T#>+O D . . . . . U#>.V#W#. . . . . X+X.e.v@. . . . . . X#Y#Z#p . . . . . . . . ",
". . . . . . . . $+2.=#( . . . . . . 7#Q#< .+. . . '#Y+e.`#. . . . . . .  $B#.$. . . . . . . . . ",
". . . . . . . . . k.D#M+. . . . . . . +$1 S.; . ' !+;@@$. . . . . . . . #$$$'.. . . . . . . . . ",
". . . . . . . . . a+c@n@9#. . . . . . . _ %$&$*$=$w ... . . . . . . . -$;$>$. . . . . . . . . . ",
". . . . . . . . . . e+=# .V.. . . . . . . ,$'$)$!$~${$. . . . . . . ]$J.;${+. . . . . . . . . . ",
". . . . . . . . . . . ^$D#>+/$F+. . 9.($Q.:+:@_$:$J.<$;.[$. . . }$|$:$1$$.. . . . . . . . . . . ",
". . . . . . . . . . . . z.L.q.J#2.q.2$!+C 3$Y.. 4$5$6$7$8$s.;$9$0$a$b$c$. . . . . . . . . . . . ",
". . . . . . . . . . . . . # a+.#2@T#z 3.:.. . . . . d$e$f$g$h$i$j$k${+. . . . . . . . . . . . . ",
". . . . . . . . . . . . . . . . . . . . . . . . . . . . . + _+. . . . . . . . . . . . . . . . . "};
EOF

#package MyPixmaps;
sub assign {
	my $mw = shift;
	$mw->Pixmap('icon', -data=>$iconImage,);
	$mw->Pixmap('logo8', -data=>$logo8Image,);
	$mw->Pixmap('logo', -data=>$logoImage,);
	$mw->Pixmap('plus', -data=>$plusImage,);
	$mw->Pixmap('minus', -data=>$minusImage,);
	$mw->Pixmap('box', -data=>$boxImage,);
}

# -------------------------------------
package MyWidget;
use strict;
# -------------------------------------

#package MyWidget;
sub new {
	my ($type,$parent,@args) = @_;
	my $self = {};
	bless $self, $type;
	$self->{parent} = $parent;
	return $self;
}
#package MyWidget;
sub destroy {
	my ($self,@args) = @_;
	$self->SUPER::destroy(@args);
	delete $self->{parent};
	delete $self->{widget};
	delete $self->{tree};
}
#package MyWidget;
sub setframe {
	my ($self,$frame,@args) = @_;
	$self->{frame} = $frame;
	$frame->{_myobject} = $self;
	return $frame;
}
#package MyWidget;
sub setwidget {
	my ($self,$widget,@args) = @_;
	$self->{widget} = $widget;
	$widget->{_myobject} = $self;
	return $widget;
}
#package MyWidget;
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
#package MyWidget;
sub parent {
	shift->{parent}
}
#package MyWidget;
sub top {
	my $self = shift;
	my $top = $self;
	while (defined $top->{parent}) {
		$top = $top->{parent};
	}
	return $top;
}
#package MyWidget;
sub toplevel {
	my $self = shift;
	if (defined $self->widget) {
		return $self->widget->toplevel;
	}
	return $self->parent->toplevel;
}
#package MyWidget;
sub widget {
	shift->{widget};
}
#package MyWidget;
sub frame {
	shift->{frame};
}
#package MyWidget;
sub attachballoon {
	my ($self,@args) = @_;
	return $self->parent->attachballoon(@args);
}
#package MyWidget;
sub statusmsg {
	my ($self,@args) = @_;
	return $self->parent->statusmsg(@args);
}
#package MyWidget;
sub configure {
	my ($self,@args) = @_;
	return $self->widget->configure(@args);
}
#package MyWidget;
sub cget {
	my ($self,@args) = @_;
	return $self->widget->cget(@args);
}
#package MyWidget;
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

#package MyMainWindow;
sub new {
	my ($type,$title,@args) = @_;
	my $self = MyWidget::new($type,undef,@args);
	$self->{data}->{title} = $title;
	my $mw = Tk::MainWindow->new(-class=>'SS7view',);
	$self->setmainwindow($mw);
	$mw->title($title);
	$mw->minsize(600,400);
	$mw->geometry('1024x768');
	#$mw->protocol('WM_DELETE_WINDOW',[\&wm_delete_window,$self],);
	#$mw->protocol('WM_SAVE_YOURSELF',[\&wm_save_yourself,$self],);
	#$mw->protocol('WM_TAKE_FOCUS',   [\&wm_take_focus,   $self],);
	$mw->client(`hostname -f`);
	$mw->command("$progname");
	#$mw->overrideredirect(0);
	push @MyMainWindow::windows, $self;
	MyOptions::assign($mw);
	MyPixmaps::assign($mw);
	$mw->iconimage('icon');
	$mw->iconname($title);
	return $self;
}

#package MyMainWindow;
sub destroy {
	my ($self,@args) = @_;
	for (my $i = 0; $i < @MyMainWindow::windows; $i++) {
		if ($MyMainWindow::windows[$i] eq $self) {
			splice @MyMainWindow::windows, $i, 1;
			last;
		}
	}
}

#package MyMainWindow;
sub wm_delete_window {
	my ($self,@args) = @_;
}

#package MyMainWindow;
sub wm_save_yourself {
	my ($self,@args) = @_;
}

#package MyMainWindow;
sub wm_take_focus {
	my ($self,@args) = @_;
}

# -------------------------------------
package MyCanvas;
use strict;
use vars qw(@ISA);
@ISA = qw(MyWidget);
# -------------------------------------

#package MyCanvas;
sub new {
	my ($type,$top,$width,@args) = @_;
	my $self = MyWidget::new($type,$top,$width,@args);
	$width = 1024 unless defined $width;
	my $c = $self->{Canvas} = $top->widget->Canvas(
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
	$self->{top} = $top->widget;
	$self->setwidget($c);
	$self->{balloon} = $c->toplevel->Balloon(-statusbar=>$top->statusbar);
	$c->update;
	$c->CanvasBind('<Configure>',[sub{
				my ($c,$self,$top,$w,$h) = @_;
				$self->{w} = $c->width;
				$self->{h} = $c->height;
				$top->regroup();
			},$self,$top,Tk::Ev('w'),Tk::Ev('h')]);
	# try creating some bindings by tag
#	$c->bind('ssp', '<ButtonPress-3>',[\&SSP::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
#	$c->bind('stp', '<ButtonPress-3>',[\&STP::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
#	$c->bind('scp', '<ButtonPress-3>',[\&SCP::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
#	$c->bind('node','<ButtonPress-3>',[\&SP::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
#	$c->bind('channel','<ButtonPress-3>',[\&Channel::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
#	$c->bind('path','<ButtonPress-3>',[\&Path::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
#	$c->bind('relation','<ButtonPress-3>',[\&Relation::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
	my $w = $self->{w} = $c->width;
	my $h = $self->{h} = $c->height;
	$self->{maxcol} =  5;
	$self->{mincol} = -5;
	return $self;
}

#package MyCanvas;
sub canvas {
	return shift->{Canvas};
}

#package MyCanvas;
sub addballoon {
	my ($self,$obj,$items) = @_;
	$obj->identify;
	foreach my $i (@{$items}) { $self->{balloonmsgs}->{$i} = \$obj->{id}; }
	$self->{balloon}->attach($self->widget,
		-balloonposition=>'mouse',
		-msg=>$self->{balloonmsgs},
	);
}

#package MyCanvas;
sub delballoon {
	my ($self,$items) = @_;
	foreach my $i (@{$items}) { delete $self->{balloonmsgs}->{$i}; }
	$self->{balloon}->attach($self->widget,
		-balloonposition=>'mouse',
		-msg=>$self->{balloonmsgs},
	);
}

#package MyCanvas;
sub relayer {
	my $self = shift;
	my $c = $self->{Canvas};
	$c->raise('node','all');
	$c->raise('scri','node');
	$c->raise('text','scri');
	$c->lower('relation','node');
	$c->lower('path','node');
	$c->lower('channel','node');
	$c->lower('relation','linkset');
	$c->lower('path','linkset');
	$c->lower('channel','linkset');
	$c->lower('relation','channel');
	$c->lower('path','channel');
}

# unknown nodes are in columns +-7
# remote/alias SSP nodes are in columns +-6
# remote/alias SCP nodes are in columns +-5
# remote/alias STP nodes are in columns +-4 (GTT cap codes)
# remote/alias STP nodes are in columns +-3
# adjacent nodes are in columns +-2

#package MyCanvas;
sub colpos {
	my ($self,$col) = @_;
	my $w = $self->{w};
	my $dw = $w / ($self->{maxcol} - $self->{mincol} + 1);
	#my $dw = $w / 10; # 10 columns
	my $off = (abs($self->{mincol}) - abs($self->{maxcol}))/2;
	if ($col < 0) {
		return ($col+$off) * $dw + $dw/2 + $w/2;
	} elsif ($col > 0) {
		return ($col+$off) * $dw - $dw/2 + $w/2;
	} else {
		return (0+$off) * $dw + $w/2;
	}
}

#package MyCanvas;
sub rowpos {
	my ($self,$row) = @_;
	my $h = $self->{h};
	return $row * 42 + $h/2;
}

# -------------------------------------
package MyTop;
use strict;
use vars qw(@ISA);
@ISA = qw(MyMainWindow);
# -------------------------------------

@MyTop::mytops = ();
$MyTop::appnum = 0;

#package MyTop;
sub new {
	my ($type,$filename,$number,@args) = @_;
	my $title = 'OpenSS7 SS7 Analyzer';
	$title .= ": $filename" if defined $filename;
	my $appno = defined $number ? $number : $MyTop::appnum;
	my $self = MyMainWindow::new($type,$title,$filename,$appno,@args);
	$self->createmenubar;
	$self->createstatusbar;
	#$self->createballoon;
	$self->createcanvas;
	$self->{topno} = scalar(@MyTop::mytops);
	push @MyTop::mytops, $self;
	$MyTop::appnum = $appno if $appno > $MyTop::appnum;
	return $self;
}

#package MyTop;
sub createmenubar {
	my $self = shift;
	my $w = $self->widget;
	my ($mb,$mi,$mc);
	$mb = $self->{MenuBar} = $w->Menu(
		-type=>'menubar',
	);
	my $balloon = $w->Balloon(-statusbar=>$self->statusbar);
	$w->configure(-menu=>$mb);
	$self->{Dialog} = $self->toplevel->Dialog(
		-buttons=>[qw/Ok Cancel Help/],
		-default_button=>'Cancel',
		-text=>'Please input capture file or interface from which to load.',
		-title=>'Source Selection',
	);
	$mi = $self->{FileMenu} = $mb->Menu(
		-tearoff=>1,
		-title=>'File Menu',
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
		-label=>'Play...',
		-underline=>0,
		-command=>[\&MyTop::menuFilePlay,$self],
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
	$self->{RecentMenu} = $mc;
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
			my $parent = shift->{FileMenu};
			my $w;
			while ($w = $parent) {
				$parent = $w->parent;
				print $w;
				foreach my $o (@{$w->configure}) {
					print "Option: ".join(', ',@$o);
				}
			}
		}, $self],
	);
	$balloon->attach($mi,
		-balloonposition=>'mouse',
		-msg=>[
			"Tearoff this menu.",
			"New canvas.",
			"Read an existing capture file.",
			"Open an existing capture file.",
			"Play back a capture file in simulated real-time.",
			"Save configuration.",
			"Save configuration in another file.",
			"Close capture.",
			undef, # separator
			"Show file properties.",
			"Select recent files.",
			undef, # separator
			"Exit all windows.",
			"Dump debuging information to stdout."
		]);
	$mb->add('cascade',
		-menu=>$mi,
		-label=>'File',
		-underline=>0,
	);
	$mi = $self->{ViewMenu} = $mb->Menu(
		-tearoff=>1,
		-title=>'View Menu',
	);
	my $m2 = $mi->Menu(
		-tearoff=>1,
		-title=>'New View Menu',
	);
	$m2->add('command',
		-label=>'Full view...',
		-underline=>0,
		-command=>[sub{ },$self],
	);
	$m2->add('command',
		-label=>'Signalling view..',
		-underline=>0,
		-command=>[sub{ },$self],
	);
	$m2->add('command',
		-label=>'Circuit view..',
		-underline=>0,
		-command=>[sub{ },$self],
	);
	$m2->add('command',
		-label=>'Transaction view..',
		-underline=>0,
		-command=>[sub{ },$self],
	);
	$balloon->attach($m2,
		-balloonposition=>'mouse',
		-msg=>[
			"Tearoff this menu.",
			"Full view with all nodes and arcs.",
			"Signalling view with linksets, adjacent SPs\nand signalling paths.",
			"Circuit view with SSPs and circuits.",
			"Transaction view with SSPs, GTTs and SCPs.",
		]);
	$mi->add('cascade',
		-menu=>$m2,
		-label=>'New view...',
		-underline=>0,
	);
	$mi->add('separator',);
	$mi->add('command',
		-label=>'Zoom In',
		-underline=>5,
		-command=>[sub{ },$self],
	);
	$mi->add('command',
		-label=>'Zoom Out',
		-underline=>5,
		-command=>[sub{ },$self],
	);
	$mi->add('separator',);
	$self->{show} = {
		circuits=>0,
		assocs=>0,
		relations=>0,
		routesets=>0,
		routes=>1,
		paths=>1,
		combineds=>1,
		linksets=>1,
		links=>1,
		channels=>1,
		nodes=>1,
		sps=>1,
		ssps=>1,
		gtts=>1,
		stps=>1,
		scps=>1,
		aliases=>1,
		all=>0,
	};
	$self->{view} = 1; # signalling
	$mi->add('radiobutton',
		-value=>0,
		-label=>'Full view',
		-variable=>\$self->{view},
		-underline=>0,
		-command=>[sub{
				my $self = shift;
				my $c = $self->canvas;
				foreach my $k (keys %{$self->{show}}) {
					$self->{show}->{$k} = 1;
				}
				$c->itemconfigure('all',-state=>'normal');
				$self->{show}->{channels} = 0;
				$c->itemconfigure('channel',-state=>'hidden');
				$self->{show}->{paths} = 0;
				$c->itemconfigure('path',-state=>'hidden');
				$self->mycanvas->relayer();
			},$self],
	);
	$mi->add('radiobutton',
		-value=>1,
		-label=>'Signalling view',
		-variable=>\$self->{view},
		-underline=>0,
		-command=>[sub{
				my $self = shift;
				my $c = $self->canvas;
				foreach my $k (keys %{$self->{show}}) {
					$self->{show}->{$k} = 0;
				}
				$c->itemconfigure('all',-state=>'hidden');
				$self->{show}->{all} = 0;
				$c->itemconfigure('route',-state=>'normal');
				$self->{show}->{routes} = 1;
				$c->itemconfigure('path',-state=>'normal');
				$self->{show}->{paths} = 1;
				$c->itemconfigure('combined',-state=>'normal');
				$self->{show}->{combineds} = 1;
				$c->itemconfigure('linkset',-state=>'normal');
				$self->{show}->{linksets} = 1;
				$c->itemconfigure('channel',-state=>'normal');
				$self->{show}->{channels} = 1;
				$c->itemconfigure('Node',-state=>'normal');
				$self->{show}->{nodes} = 1;
				$c->itemconfigure('SP',-state=>'normal');
				$self->{show}->{sps} = 1;
				$c->itemconfigure('SSP',-state=>'normal');
				$self->{show}->{ssps} = 1;
				$c->itemconfigure('STP',-state=>'normal');
				$self->{show}->{stps} = 1;
				$c->itemconfigure('GTT',-state=>'normal');
				$self->{show}->{gtts} = 1;
				$c->itemconfigure('SCP',-state=>'normal');
				$self->{show}->{scps} = 1;
				$c->itemconfigure('alias',-state=>'normal');
				$self->{show}->{aliases} = 1;
				$self->mycanvas->relayer();
			},$self],
	);
	$mi->add('radiobutton',
		-value=>2,
		-label=>'Circuit view',
		-variable=>\$self->{view},
		-underline=>0,
		-command=>[sub{
				my $self = shift;
				my $c = $self->canvas;
				foreach my $k (keys %{$self->{show}}) {
					$self->{show}->{$k} = 0;
				}
				$c->itemconfigure('all',-state=>'hidden');
				$c->itemconfigure('circuits',-state=>'normal');
				$self->{show}->{circuits} = 1;
				$c->itemconfigure('SSP&&ISUP',-state=>'normal');
				$self->{show}->{ssps} = 1;
				$self->mycanvas->relayer();
			},$self],
	);
	$mi->add('radiobutton',
		-value=>3,
		-label=>'Transaction view',
		-variable=>\$self->{view},
		-underline=>0,
		-command=>[sub{
				my $self = shift;
				my $c = $self->canvas;
				foreach my $k (keys %{$self->{show}}) {
					$self->{show}->{$k} = 0;
				}
				$c->itemconfigure('all',-state=>'hidden');
				$c->itemconfigure('association',-state=>'normal');
				$self->{show}->{assocs} = 1;
				$c->itemconfigure('SSP&&TCAP',-state=>'normal');
				$self->{show}->{ssps} = 1;
				$c->itemconfigure('SCP',-state=>'normal');
				$self->{show}->{scps} = 1;
				$c->itemconfigure('GTT',-state=>'normal');
				$self->{show}->{gtts} = 1;
				$self->mycanvas->relayer();
			},$self],
	);
	$mi->add('separator',);
	$mi->add('checkbutton', -label=>'All',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{all},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{all};
				foreach my $k (keys %{$self->{show}}) {
					$self->{show}->{$k} = $v;
				}
				$self->{view} = $v ? 0 : 4;
				my $c = $self->canvas;
				if ($self->{show}->{all}) {
					$c->itemconfigure('all',-state=>'normal');
					$self->mycanvas->relayer();
				} else {
					$c->itemconfigure('all',-state=>'hidden');
				}
			},$self],
	);
	$mi->add('separator',);
	$mi->add('checkbutton', -label=>'Relations',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{relations},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{relations};
				$self->{view} = 4;
				$self->{show}->{circuits} = $v;
				$self->{show}->{assocs} = $v;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('relation',-state=>$s) if $c->find(withtag=>'relation');
			},$self],
	);
	$mi->add('checkbutton', -label=>'Routesets',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{routesets},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{routesets};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('routeset',-state=>$s) if $c->find(withtag=>'routeset');
			},$self],
	);
	$mi->add('checkbutton', -label=>'Circuit Groups',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{circuits},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{circuits};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('circuits',-state=>$s) if $c->find(withtag=>'circuits');
				$self->{show}->{relations} = $v
					if $self->{show}->{assocs} == $v;
			},$self],
	);
	$mi->add('checkbutton', -label=>'Associations',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{assocs},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{assocs};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('association',-state=>$s) if $c->find(withtag=>'association');
				$self->{show}->{relations} = $v
					if $self->{show}->{circuits} == $v;
			},$self],
	);
	$mi->add('checkbutton', -label=>'Combined Linksets',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{combineds},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{combineds};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('combined',-state=>$s) if $c->find(withtag=>'combined');
			},$self],
	);
	$mi->add('checkbutton', -label=>'Linksets',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{linksets},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{linksets};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('linkset',-state=>$s) if $c->find(withtag=>'linkset');
			},$self],
	);
	$mi->add('checkbutton',-label=>'Links',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{links},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{links};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('link',-state=>$s) if $c->find(withtag=>'link');
			},$self],
	);
	$mi->add('separator',);
	$mi->add('checkbutton', -label=>'Routes',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{routes},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{routes};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('route',-state=>$s) if $c->find(withtag=>'route');
			},$self],
	);
	$mi->add('checkbutton', -label=>'Paths',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{paths},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{paths};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('path',-state=>$s) if $c->find(withtag=>'path');
			},$self],
	);
	$mi->add('checkbutton', -label=>'Channels',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{channels},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{channels};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('channel',-state=>$s) if $c->find(withtag=>'channel');
			},$self],
	);
	$mi->add('separator',);
	$mi->add('checkbutton', -label=>'Nodes',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{nodes},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{nodes};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('Node',-state=>$s) if $c->find(withtag=>'Node');
			},$self],
	);
	$mi->add('checkbutton', -label=>'SPs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{sps},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{sps};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('SP',-state=>$s) if $c->find(withtag=>'SP');
			},$self],
	);
	$mi->add('checkbutton', -label=>'SSPs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{ssps},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{ssps};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('SSP',-state=>$s) if $c->find(withtag=>'SSP');
			},$self],
	);
	$mi->add('checkbutton', -label=>'GTTs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{gtts},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{gtts};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('GTT',-state=>$s) if $c->find(withtag=>'GTT');
			},$self],
	);
	$mi->add('checkbutton', -label=>'STPs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{stps},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{stps};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('STP',-state=>$s) if $c->find(withtag=>'STP');
			},$self],
	);
	$mi->add('checkbutton', -label=>'SCPs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{scps},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{scps};
				$self->{view} = 4;
				my $c = $self->canvas;
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('SCP',-state=>$s) if $c->find(withtag=>'SCP');
			},$self],
	);
	$mi->add('checkbutton', -label=>'Aliases',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{aliases},
		-command=>[\&MyTop::menuViewAliases,$self],
	);
	$balloon->attach($mi,
		-balloonposition=>'mouse',
		-msg=>[
			"Tearoff this menu.",
			"New...",
			undef, # separator
			"Zoom in on canvas.",
			"Zoom out from canvas.",
			undef, # separator
			"Full view with all nodes and arcs.",
			"Signalling view with linksets, adjacent SPs\nand signalling paths.",
			"Circuit view with SSPs and circuits.",
			"Transaction view with SSPs, GTTs and SCPs.",
			undef, # separator
			"Show/hide all items.",
			undef, # separator
			"Show/hide signalling relations.",
			"Show/hide routesets.",
			"Show/hide circuit groups.",
			"Show/hide transaction associations.",
			"Show/hide combined linksets.",
			"Show/hide signalling linksets.",
			"Show/hide signalling links.",
			undef, # separator
			"Show/hide signalling routes.",
			"Show/hide signalling paths.",
			"Show/hide signalling channels.",
			undef, # separator
			"Show/hide Nodes.",
			"Show/hide SPs.",
			"Show/hide SSPs.",
			"Show/hide GTTs.",
			"Show/hide STPs.",
			"Show/hide SCPs.",
			"Show/hide Aliases.",
		]);
	$mb->add('cascade',
		-menu=>$mi,
		-label=>'View',
		-underline=>0,
	);
	$mi = $mb->Menu(
		-tearoff=>0,
		-title=>'About Menu',
		-type=>'normal',
	);
	$mi->add('command',
		-label=>'Version',
		-underline=>0,
		-command=>[\&MyTop::menuAboutVersion,$self],
	);
	$mi->add('command',
		-label=>'Usage',
		-underline=>0,
		-command=>[\&MyTop::menuAboutUsage,$self],
	);
	$mi->add('command',
		-label=>'Copying',
		-underline=>0,
		-command=>[\&MyTop::menuAboutCopying,$self],
	);
	$mi->add('command',
		-label=>'Help',
		-underline=>0,
		-command=>[\&MyTop::menuAboutHelp,$self],
	);
	$mb->add('separator');
	$mb->add('cascade',
		-menu=>$mi,
		-label=>'About',
		-underline=>0,
	);
}

#package MyTop;
sub menuAboutVersion {
    my $self = shift;
    my $dialog = $self->widget->DialogBox(
	-title=>'Program Version',
	-default_button=>'Dismiss',
	-buttons=>[qw/Dismiss/],
    );
    $dialog->resizable(0,0);
    $dialog->Label(
	-image=>'logo',
	-text=>$Title,
	-compound=>'left',
	-justify=>'left',
    )->pack(
	-side=>'top',
	-fill=>'x',
	-expand=>0,
    );
    my $lines = $Version;
    $lines =~ s/[^\n]//g;
    $lines = length($lines) + 1;
    my $text = $dialog->Scrolled('ROText',
	-scrollbars=>'onow',
	-wrap=>'none',
	-height=>$lines,
    )->pack(
	-side=>'top',
	-fill=>'both',
	-expand=>1,
    );
    $text->insert('0.0', $Version);
    $dialog->Show;
}

#package MyTop;
sub menuAboutUsage {
    my $self = shift;
    my $dialog = $self->widget->DialogBox(
	-title=>'Program Copying Conditions',
	-default_button=>'Dismiss',
	-buttons=>[qw/Dismiss/],
    );
    $dialog->resizable(0,0);
    $dialog->Label(
	-image=>'logo',
	-text=>$Title,
	-compound=>'left',
	-justify=>'left',
    )->pack(
	-side=>'top',
	-fill=>'x',
	-expand=>0,
    );
    my $lines = $Usage;
    $lines =~ s/[^\n]//g;
    $lines = length($lines) + 1;
    my $tx = $dialog->Scrolled('ROText',
	-scrollbars=>'onow',
	-wrap=>'none',
	-height=>$lines,
    )->pack(
	-side=>'top',
	-fill=>'both',
	-expand=>1,
    );
    $tx->insert('0.0', $Usage);
    $dialog->Show;
}

#package MyTop;
sub menuAboutCopying {
    my $self = shift;
    my $dialog = $self->widget->DialogBox(
	-title=>'Program Copying Conditions',
	-default_button=>'Accept',
	-buttons=>[qw/Accept Refuse/],
    );
    $dialog->resizable(0,0);
    $dialog->Label(
	-image=>'logo',
	-text=>$Title,
	-compound=>'left',
	-justify=>'left',
    )->pack(
	-side=>'top',
	-fill=>'x',
	-expand=>0,
    );
    my $lines = $Copying;
    $lines =~ s/[^\n]//g;
    $lines = length($lines) + 1 + 15;
    my $tx = $dialog->Scrolled('ROText',
	-scrollbars=>'onow',
	-wrap=>'word',
	-height=>$lines,
    )->pack(
	-side=>'top',
	-fill=>'both',
	-expand=>1,
    );
    $tx->insert('0.0', $Copying);
    exit 0 if ($dialog->Show eq 'Refuse');
}

#package MyTop;
sub menuAboutHelp {
    my $self = shift;
    my $dialog = $self->widget->DialogBox(
	-title=>'Help',
	-default_button=>'Dismiss',
	-buttons=>[qw/Dismiss/],
    );
    $dialog->resizable(0,0);
    $dialog->Label(
	-image=>'logo',
	-text=>$Title,
	-compound=>'left',
	-justify=>'left',
    )->pack(
	-side=>'top',
	-fill=>'x',
	-expand=>0,
    );
    my $lines = $Help;
    $lines =~ s/[^\n]//g;
    $lines = length($lines) + 1 + 5;
    my $tx = $dialog->Scrolled('ROText',
	-scrollbars=>'onow',
	-wrap=>'none',
	-height=>$lines,
    )->pack(
	-side=>'top',
	-fill=>'both',
	-expand=>1,
    );
    $tx->insert('0.0', $Help);
    $dialog->Show;
}

#package MyTop;
sub createstatusbar {
	my $self = shift;
	my $sb = $self->{StatusBar} = $self->toplevel->Message(
		-text=>"Status bar.",
		-justify=>'left',
		-width=>'800',
		-anchor=>'w',
	);
	$sb->pack(
		-expand=>0,
		-fill=>'x',
		-side=>'bottom',
		-anchor=>'sw',
	);
	$self->{Message} = $sb;
}

#package MyTop;
sub statusbar {
	return shift->{StatusBar};
}

#package MyTop;
sub setstatus {
	my ($self,@lines) = @_;
	my $text = join("\n",@lines);
	$self->{StatusBar}->configure(-text=>$text);
}

#package MyTop;
sub createballoon {
	my $self = shift;
	my $balloon = $self->{Balloon} = $self->toplevel->Balloon(
		#-initwait=>1000,
		-statusbar=>$self->{Message},
	);
	$::balloonwidget = $balloon;
	return $balloon;
}

#package MyTop;
sub createcanvas {
	my ($self,$width,@args) = @_;
	my $c = $self->{MyCanvas} = MyCanvas->new($self,$width,@args);
}

#package MyTop;
sub mycanvas {
	return shift->{MyCanvas};
}

#package MyTop;
sub canvas {
	return shift->{MyCanvas}->canvas;
}

#package MyTop;
sub attachballoon {
	my ($self,@args) = @_;
	return $self->{Balloon}->attach(@args) if $self->{Balloon};
}
#package MyTop;
sub statusmsg {
	my ($self,$msg) = @_;
	$self->{Message}->configure(-text=>$msg) if $self->{Message};
	printf STDERR "$msg\n";
}

#package MyTop;
sub menuViewSps {
}

#package MyTop;
sub menuViewSsps {
}

#package MyTop;
sub menuViewGtts {
}

#package MyTop;
sub menuViewStps {
}

#package MyTop;
sub menuViewScps {
}

#package MyTop;
sub menuViewAliases {
}

#package MyTop;
sub menuFileClose {
	my $self = shift;
	my $w = $self->widget;
	$w->destroy();
}

#package MyTop;
sub menuFileOpen {
	my $self = shift;
	my $data = $self->{data};
	my $w = $self->widget;
	my $file = $w->getOpenFile(
		-defaultextension=>'.pcap',
		-initialdir=>'./testdata',
		-initialfile=>'ss7capall.pcap',
		-title=>'Open Dialog',
	);
	return unless $file;
	my $err = '';
	my $pcap;
	unless ($pcap = Net::Pcap::pcap_open_offline($file, \$err)) {
		my $d = $w->Dialog(
			-title=>'Could not open file.',
			-text=>$err,
			-default_button=>'Ok',
			-buttons=>[ 'Ok' ],
		);
		$d->Show;
		$d->destroy;
		return;
	}
	my $network = $self->{network} = Network->new($self);
#	my $fh = Net::Pcap::pcap_file($pcap);
#	Tk::Event::IO->fileevent($fh, 'readable' => [\&MyTop::readmsg,$self,$pcap,$fh,$network]);
#	$w->Busy(-recurse=>1);
	if (my $msg = Message->create($pcap)) {
		$self->{begtime} = $msg->{hdr};
		$self->{endtime} = $msg->{hdr};
		$msg->process($self,$network);
#		$w->update;
#		if ($self->{updatenow}) {
#			$w->update;
#			delete $self->{updatenow};
#		}
		while ($msg = Message->create($pcap)) {
			$self->{endtime} = $msg->{hdr};
			$msg->process($self,$network);
#			$w->update;
			if ($self->{regroupnow}) {
				$self->regroup();
				$self->canvas->update;
				delete $self->{regroupnow};
			}
			if ($self->{updatenow}) {
				$w->update;
				delete $self->{updatenow};
			}
		}
	}
	Net::Pcap::pcap_close($pcap);
#	$w->update;
#	$w->Unbusy;
#	if ($self->{updatenow}) {
#		$w->update;
#		delete $self->{updatenow};
#	}
}

#package MyTop;
sub readmsg {
	my ($self,$pcap,$fh,$network,@args) = @_;
	if (my $msg = Message->create($pcap)) {
		$msg->process($self,$network);
	} else {
		Tk::Event::IO->fileevent($fh, 'readable' => '');
		Net::Pcap::pcap_close($pcap);
	}
}

#package MyTop;
sub menuFilePlay {
	my $self = shift;
	my $data = $self->{data};
	my $w = $self->widget;
	my $file = $w->getOpenFile(
		-defaultextension=>'.pcap',
		-initialdir=>'./testdata',
		-initialfile=>'ss7capall.pcap',
		-title=>'Play Dialog',
	);
	return unless $file;
	my $err = '';
	unless ($self->{pcap} = Net::Pcap::pcap_open_offline($file, \$err)) {
		my $d = $w->Dialog(
			-title=>'Could not open file.',
			-text=>$err,
			-default_button=>'Ok',
			-buttons=>[ 'Ok' ],
		);
		$d->Show;
		$d->destroy;
		return;
	}
	my $network = $self->{network} = Network->new($self);
	if (my $msg = Message->create($self->{pcap})) {
		$self->{begtime} = $msg->{hdr};
		$self->{endtime} = $msg->{hdr};
		my ($secs,$usec) = Time::HiRes::gettimeofday;
		$secs -= $msg->{hdr}->{tv_sec};
		$usec -= $msg->{hdr}->{tv_usec};
		while ($usec < 0) {
			$secs -= 1;
			$usec += 1000000;
		}
		$self->{ofs} = { tv_sec=>$secs, tv_usec=>$usec };
		$msg->process($self,$network);
		if ($msg = Message->create($self->{pcap})) {
			$self->contmsg($msg);
			return;
		}
	}
	Net::Pcap::pcap_close($self->{pcap});
	delete $self->{pcap};
}

#package MyTop;
sub contmsg {
	my ($self,$msg) = @_;
	my $w = $self->widget;
	my ($osecs,$ousec) = ($self->{ofs}->{tv_sec},$self->{ofs}->{tv_usec});
	do {
		my ($secs,$usec) = Time::HiRes::gettimeofday;
		$secs -= $osecs + $msg->{hdr}->{tv_sec};
		$usec -= $ousec + $msg->{hdr}->{tv_usec};
		while ($secs > 0 && $usec < 0) {
			$secs -= 1;
			$usec += 1000000;
		}
		if ($secs > 0 || ($secs == 0 && $usec > 0)) {
			$self->{endtime} = $msg->{hdr};
			$msg->process($self,$self->{network});
			if ($self->{regroupnow}) {
				$self->regroup();
				$self->canvas->update;
				delete $self->{regroupnow};
			}
			if ($self->{updatenow}) {
				$w->update;
				delete $self->{updatenow};
			}
		} else {
			my $ms = (-$secs*1000) + int((-$usec+999)/1000);
			$w->after($ms, [\&MyTop::contmsg, $self, $msg]);
			return;
		}

	} while ($msg = Message->create($self->{pcap}));
	Net::Pcap::pcap_close($self->{pcap});
	delete $self->{pcap};
}

#package MyTop;
sub regroup {
	my $self = shift;
	#print STDERR "D: regrouping...\n";
	my $network = $self->{network};
	return unless $network;
	my $mc = $self->mycanvas;
	my %totals;
	my %offset;
	my ($mincol,$maxcol) = (0,0);
	while (my ($no,$node) = each %{$network->{pnodes}}) {
		my $col = $node->{col};
		$totals{$col}++;
		$offset{$col}=0;
		$mincol = $col if $col < $mincol;
		$maxcol = $col if $col > $maxcol;
	}
	while (my ($pc,$node) = each %{$network->{nodes}}) {
		my $col = $node->{col};
		$totals{$col}++;
		$offset{$col}=0;
		$mincol = $col if $col < $mincol;
		$maxcol = $col if $col > $maxcol;
	}
	$mincol = -5 if $mincol == 0;
	$maxcol =  5 if $maxcol == 0;
	$mc->{mincol} = $mincol;
	$mc->{maxcol} = $maxcol;
	my $max = int($mc->{h}/42/2);
	my %counts;
	foreach my $no (sort {$a <=> $b} keys %{$network->{pnodes}}) {
		my $node = $network->{pnodes}->{$no};
		my $col = $node->{col};
		my $tot = $totals{$col};
		my $row = $counts{$col} - $tot;
		my $off = $offset{$col};
		$counts{$col} += 2;
		if ($tot > $max) {
			if ($off != 0)   { $offset{$col} =  0; }
			elsif ($col > 0) { $offset{$col} =  1; }
			else		 { $offset{$col} = -1; }
			$row /= 2;
		}
		$node->movenode($self,$col,$row,$off/2);
	}
	foreach my $pc (sort {$a <=> $b} keys %{$network->{nodes}}) {
		my $node = $network->{nodes}->{$pc};
		my $col = $node->{col};
		my $tot = $totals{$col};
		my $row = $counts{$col} - $tot;
		my $off = $offset{$col};
		$counts{$col} += 2;
		if ($tot > $max) {
			if ($off != 0)   { $offset{$col} =  0; }
			elsif ($col > 0) { $offset{$col} =  1; }
			else		 { $offset{$col} = -1; }
			$row /= 2;
		}
		$node->movenode($self,$col,$row,$off/2);
	}
	#print STDERR "D: ...regrouped.\n";
	delete $self->{regroupnow};
	$self->{updatenow} = 1;
}

# -------------------------------------
package main;
use strict;
# -------------------------------------

if (length @infiles == 0) {
	@infiles = ( $infile );
}

my $t = MyTop->new;

Tk::MainLoop;

if (0) {
	# XML::Simple does not like tag names of '0'
	foreach my $k (keys %{$pc_assignments->{5}}) {
		if (exists $pc_assignments->{5}->{$k}->{0}) {
			$pc_assignments->{5}->{$k}->{256} =
			$pc_assignments->{5}->{$k}->{0};
			delete $pc_assignments->{5}->{$k}->{0};
		}
	}
	my $xml = XMLout($pc_assignments);
	print $xml;
}

if ($pc_assignments) {
	my $dumper = new XML::Dumper;
	$dumper->pl2xml($pc_assignments, "$progdir/pc_assignments.xml.gz");
}

#my $handler = MyParser->new();
#my $p = XML::SAX::ParserFactory->parser(Handler => $handler);
#$p->parse_usr($filename);

exit;

1;
