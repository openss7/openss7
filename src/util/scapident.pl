#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $progname = $0;

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

use Time::HiRes qw(gettimeofday);

my %large_networks = (
#	Table B1/T1.111.8 -  List of Large Network Codes 
#	Large Network Code Network Identification Field Signalling Network
#	255=>['',		'RESERVED - FUTURE USE'],
	254=>['AT&T',		'AT&T Communications'],
	253=>['US SPRINT',	'US SPRINT'],
	252=>['BellSouth',	'BellSouth'],
#	251=>['Pac Bell',	'Pacific Bell'],
	251=>['PacificTelesis',	'PacificTelesis'],
#	250=>['Ameritech',	'Ameritech'],
	250=>['Ameritech',	'Ameritech Services'],
	249=>['SWB',		'Southwestern Bell Telephone'],
#	248=>['Bell',		'Bell Tri-Co Services'],
	248=>['U S WEST',	'U S WEST Communications'],
#	247=>['Nynex',		'Nynex Service Co.'],
	247=>['NYNEX',		'NYNEX'],
	246=>['Bell Atlantic',	'Bell Atlantic'],
#	245=>['Canada',		'Telecom Canada'],
	245=>['AT&T Canada',	'AT&T Canada Long Distance Services'],
#	244=>['MCI',		'MCI Telecommunications Group'],
	244=>['MCI',		'MCI Telecommunications Corporation'],
	243=>['SNET',		'Southern New England Telephone'],
	242=>['Allnet',		'Allnet Communications Services, Inc.'],
	241=>['Defense',	'Defense Communications Agency'],
#	240=>['GTE',		'GTE Service Corporation/Telephone Operations'],
	240=>['GTE',		'GTE Service Corporation'],
#	239=>['United',		'United Telephone Sys'],
	239=>['Sprint',		'Sprint Local Telecommunications Division'],
	238=>['Independent',	'Independent Telecommunications Network'],
	237=>['C&W',		'Cable & Wireless'],
#	236=>['CNCP',		'CNCP Telecommunications'],
	236=>['AT&T Canada',	'AT&T Canada Long Distance Services'],
#	235=>['Comtel',		'Comtel'],
	235=>['CONTEL',		'CONTEL (GTE Wireless)'],
	234=>['ALLTEL',		'ALLTEL'],
#	233=>['Rochester',	'Rochester Telephone'],
	233=>['Frontier',	'Frontier Communications Int\'l, Inc.'],
	232=>['Century',	'Century Telephone Enterprises, Inc.'],
	231=>['AT&T',		'AT&T (VSN)'],
#	230=>['Centel',		'Centel'],
	230=>['Sprint',		'Sprint Local Telecommunications Division'],
	229=>['TEST',		'TEST CODE (T1S1)'],
#	Point codes with this network identifier may be assigned by
#	any network operator to real or simulated signalling points
#	for test purposes.  Due to possible duplicate assignments of
#	these codes, messages addressed with one of these point codes
#	should not be sent across network boundaries except with
#	agreements of the networks involved.
	228=>['PacTel',		'Pacific Telecom'],
	227=>['NACN',		'North American Cellular Network'],
	226=>['Cantel',		'Cantel'],
	225=>['Lincoln',	'Lincoln Telephone Company'],
	224=>['Cinc, Bell',	'Cincinnati Bell Telephone'],
	223=>['TDS',		'TDS TELECOM'],
	222=>['MEANS/INS/SDN',	'MEANS/INS/SDN'],
	221=>['CTIA',		'CTIA'],
	220=>['SWB',		'Southwestern Bell Mobile Systems'],
	219=>['Nextel',		'Nextel (Was OneComm, Inc.)'],
	218=>['Citizens',	'Citizens Telecom'],
	217=>['ICG',		'ICG Network Services'],
	216=>['MCI',		'MCImetro'],
	215=>['AT&T',		'AT&T'],
	214=>['Sprint',		'Sprint Spectrum L.P.'],
	213=>['Mobility Canada','Mobility Canada'],
	212=>['Teleglobe',	'Teleglobe USA'],
	211=>['Omnipoint',	'Omnipoint Communications Inc.'],
	210=>['NextWave',	'NextWave Telecom'],
	209=>['BellSouth',	'BellSouth Enterprises'],
	208=>['GST',		'GST Telecom'],
);
my %small_networks = (
	1=>{
		  1=>['Puerto Rico',	'Puerto Rico Telephone Company'],
#		  2=>['Cincinnati',	'Cincinnati Bell'],
		  2=>['Standard',	'Standard Telephone Company'],
#		  3=>['LDS',		'LDS Metromedia Ltd'],
		  3=>['WorldCom',	'WorldCom'],
		  4=>['Schneider',	'Schneider Communications'],
#		  5=>['Microtel',	'Microtel Inc'],
		  5=>['WorldCom',	'WorldCom.'],
		  6=>['ACC',		'ACC Long Distance Corporation'],
#		  7=>['SouthernNet',	'SouthernNet'],
		  7=>['Southernet',	'Southernet'],
		  8=>['Teleconnect',	'Teleconnect Company'],
#		  9=>['Telepshere',	'Telesphere Network'],
		  9=>['WorldCom',	'WorldCom'],
#		 10=>['MidAmerican',	'MidAmerican'],
		 10=>['WorldCom',	'WorldCom'],
		 11=>['Sprint',		'Long Distance USA/Sprint'],
		 12=>['RCI',		'RCI Corporation'],
		 13=>['Phoenix',	'Phoenix Network'],
		 14=>['Teledial',	'Teledial America'],
#		 15=>['USTS',		'U.S. Transmission Systems'],
		 15=>['Antigua',	'Antigua Public Utilities Authority'],
#		 16=>['Litel',		'Litel Telecommunication Corp'],
		 16=>['Litel',		'LCI International/Litel'],
		 17=>['Chadwick',	'Chadwick Telephone'],
		 18=>['LDS',		'Long Distance Service, Inc.'],
		 19=>['WITS',		'Washington Interagency Telecommunications Service'],
		 20=>['PBSI',		'Phone Base Systems, Inc.'],
		 21=>['General',	'General Communication, Inc.'],
		 22=>['CTI',		'CTI Telecom Inc.'],
#		 23=>['SouthTel',	'SouthTel'],
		 23=>['MetroNet',	'MetroNet Communication Group, Inc.'],
		 24=>['Roseville',	'Roseville Telephone Company'],
#		 25=>['TCL',		'TelaMarketing Corporation of Louisiana'],
		 25=>['Rio',		'Rio Communications'],
#		 26=>['LDDS',		'LDDS'],
		 26=>['WorldCom',	'WorldCom'],
		 27=>['Capital',	'Capital Telecom, Inc.'],
		 28=>['Transtel',	'Transtel Corporation'],
#		 29=>['ComSystem',	'ComSystem'],
		 29=>['WorldCom',	'WorldCom'],
		 30=>['Frontier',	'Mid Atlantic/Frontier'],
		 31=>['LDM',		'Long Distance Management'],
		 32=>['Bhamas',		'Bhamas Telecommunications Corp'],
#		 33=>['NTC',		'NTC Inc.'],
		 33=>['WorldCom',	'WorldCom'],
		 34=>['NDC',		'National Data Corporation'],
		 35=>['FTC',		'FTC Communications'],
#		 36=>['Lincoln',	'Lincoln Telephone Company'],
		 36=>['Media One',	'Media One'],
		 37=>['Alascom',	'Alascom'],
		 38=>['Motorola',	'Motorola, Inc.'],
		 39=>['West Coast',	'West Coast Telecom'],
		 40=>['LDS',		'Long Distance Savers, Inc./LDS'],
#		 41=>['EdTel',		'Ed Tel'],
		 41=>['TELUS',		'TELUS Communications (Edmonton) Inc (TCE)'],
		 42=>['N Pittsburgh',	'North Pittsburgh Telephone Company'],
		 43=>['Rock Hill',	'Rock Hill Telephone Company'],
		 44=>['Teleglobe',	'Teleglobe Canada'],
		 45=>['Jamaica',	'Jamaica Telephone Company'],
		 46=>['Sugarland',	'Sugarland Telephone Company'],
		 47=>['Lakedale',	'Lakedale Telephone Company'],
		 48=>['Chillicothe',	'Chillicothe Telephone Company'],
		 49=>['N State',	'North State Telephone Company'],
		 50=>['Pub. Service',	'Public Service Telephone Company'],
		 51=>['Cinc. Bell',	'Cincinnati Bell Long Distance'],
		 52=>['NTX',		'National Telephone Exchange/NTX'],
		 53=>['Lake States',	'Lake States Long Distance'],
		 54=>['NTF',		'National Telecom of Florida'],
		 55=>['D&ET&T',		'Denver & Ephrata Telephone & Telegraph'],
		 56=>['Vartec',		'VarTec National, Inc.'],
		 57=>['Eastern',	'Eastern Telephone'],
		 58=>['Tel.Express',	'Telephone Express'],
		 59=>['SNET',		'SNET Protocol Conversion'],
		 60=>['E Ascension',	'East Ascension Telephone Company, Inc.'],
		 61=>['Codetel',	'Codetel'],
		 62=>['Kerrville',	'Kerrville Telephone Company'],
		 63=>['Barry Cty',	'Barry County Telephone Company'],
		 64=>['CALNET',		'CALNET'],
		 65=>['D&ET&T',		'Denver & Ephrata Telephone & Telegraph'],
		 66=>['D&ET&T',		'Denver & Ephrata Telephone & Telegraph'],
		 67=>['Network Svcs.',	'Network Services'],
		 68=>['Redwood Cty',	'Redwood County Telephone Company/Clements Telephone Company'],
		 69=>['Hargray',	'Horry Telephone/Hargray Telephone'],
		 70=>['Hargray',	'Horry Telephone/Hargray Telephone'],
		 71=>['Hargray',	'Horry Telephone/Hargray Telephone'],
		 72=>['Hargray',	'Horry Telephone/Hargray Telephone'],
		 73=>['CommuniGroup',	'CommuniGroup, Inc.'],
		 74=>['Concord',	'The Concord Telephone Company'],
		 75=>['Concord',	'The Concord Telephone Company'],
		 76=>['Concord',	'The Concord Telephone Company'],
		 77=>['Concord',	'The Concord Telephone Company'],
		 78=>['W Kentucky R',	'West Kentucky Rural Telephone Cooperative'],
		 79=>['Pioneer',	'Pioneer Telephone Association'],
		 80=>['Duo Cty',	'Duo County Telephone Cooperative Corporation'],
		 81=>['CFW',		'CFW Telephone'],
		 82=>['Lufkin-Conroe',	'Lufkin-Conroe Telephone Exch., Inc.'],
		 83=>['Lufkin-Conroe',	'Lufkin-Conroe Telephone Exch., Inc.'],
		 84=>['Lufkin-Conroe',	'Lufkin-Conroe Telephone Exch., Inc.'],
		 85=>['Puerto Rico',	'Puerto Rico Telephone Company'],
		 86=>['Puerto Rico',	'Puerto Rico Telephone Company'],
		 87=>['Puerto Rico',	'Puerto Rico Telephone Company'],
		 88=>['Illinois Cons.',	'Illinois Consolidated Telephone Company'],
		 89=>['Westinghouse',	'Westinghouse Communications'],
		 90=>['Anchorage',	'Anchorage Telephone Utility'],
		 91=>['U S WEST',	'U S WEST New Vector Group'],
		 92=>['U S WEST',	'U S WEST New Vector Group'],
		 93=>['U S WEST',	'U S WEST New Vector Group'],
		 94=>['U S WEST',	'U S WEST New Vector Group'],
		 95=>['Lakedale',	'Lakedale Telephone Company'],
		 96=>['Lakedale',	'Lakedale Telephone Company'],
		 97=>['Lakedale',	'Lakedale Telephone Company'],
		 98=>['C Texas',	'Central Texas Telephone Cooperative'],
		 99=>['C Texas',	'Central Texas Telephone Cooperative'],
		100=>['C Texas',	'Central Texas Telephone Cooperative'],
		101=>['LinkUSA',	'LinkUSA Corporation'],
		102=>['LinkUSA',	'LinkUSA Corporation'],
		103=>['Standish',	'Standish Telephone Company'],
		104=>['China',		'China Telephone Company'],
		105=>['WorldCom',	'WorldCom'],
		106=>['WorldCom',	'WorldCom'],
		107=>['WorldCom',	'WorldCom'],
		108=>['WorldCom',	'WorldCom'],
		109=>['WorldCom',	'WorldCom'],
		110=>['WorldCom',	'WorldCom'],
		111=>['Commonwealth',	'Commonwealth Telephone Company'],
		112=>['WorldCom',	'WorldCom'],
		113=>['Matanuska',	'Matanuska Telephone Association, Inc.'],
		114=>['Reserve',	'Reserve Telephone Company, Inc.'],
		115=>['Brazoria',	'Brazoria Telephone Company'],
		116=>['Commonwealth',	'Commonwealth Telephone Company'],
		117=>['Commonwealth',	'Commonwealth Telephone Company'],
		118=>['US Cellular',	'United States Cellular'],
		119=>['US Cellular',	'United States Cellular'],
		120=>['Videotron',	'Videotron Telecom Ltee'],
		121=>['Novus',		'Novus Telecom Inc.'],
		122=>['Bell Atlantic',	'Bell Atlantic Mobile'],
		123=>['USLink',		'USLink Long Distance'],
		124=>['NE Pennsylvania','North-Eastern Pennsylvania Telephone Company'],
		125=>['Gulf',		'Gulf Telephone Company'],
		126=>['Gulf',		'Gulf Telephone Company'],
		127=>['Bell Atlantic',	'Bell Atlantic Mobile'],
		128=>['Bell Atlantic',	'Bell Atlantic Mobile'],
		129=>['Bell Atlantic',	'Bell Atlantic Mobile'],
		130=>['NYNEX Mobile',	'NYNEX Mobile Communications'],
		131=>['Ameritech',	'Ameritech Cellular Services, Inc.'],
		132=>['Yadkin Vly',	'Yadkin Valley Telephone Membership Corporation'],
		133=>['Intermedia',	'Intermedia Communications, Inc.'],
		134=>['Advantis',	'Advantis'],
		135=>['Advantis',	'Advantis'],
		136=>['Advantis',	'Advantis'],
		137=>['Skyline',	'Skyline Tel. Membership Corporation'],
		138=>['Pac Tel',	'Pac Tel Corporation'],
		139=>['Com Net',	'Com Net'],
		140=>['Hickory',	'Hickory Telephone Company'],
		141=>['Bartel',		'Barbados Telephone Company, Ltd., (Bartel)'],
		142=>['Bartel',		'Barbados Telephone Company, Ltd., (Bartel)'],
		143=>['WorldCom',	'WorldCom'],
		144=>['WorldCom',	'WorldCom'],
		145=>['WorldCom',	'WorldCom'],
		146=>['WorldCom',	'WorldCom'],
		147=>['WorldCom',	'WorldCom'],
		148=>['Thunder Bay',	'Thunder Bay Telephone'],
		149=>['Wilkes',		'Wilkes Telephone Membership Corporation'],
		150=>['Wilkes',		'Wilkes Telephone Membership Corporation'],
		151=>['BellSouth',	'BellSouth Cellular Corporation'],
		152=>['BellSouth',	'BellSouth Cellular Corporation'],
		153=>['ITC',		'ITC Transmission Systems'],
		154=>['BTI',		'BTI Telecommunications Services'],
		155=>['WorldCom',	'WorldCom'],
		156=>['WorldCom',	'WorldCom'],
		157=>['Transaction',	'Transaction Network Services, Inc.'],
		158=>['Trinidad/Tobago','Telecommunication Services of Trinidad/Tobago'],
		159=>['Project Mutual',	'Project Mutual Telephone Company'],
		160=>['Sprint Canada',	'Sprint Canada'],
		161=>['Webster Calhoun','Webster Calhoun Cooperative Telephone Association'],
		162=>['Trenton',	'Trenton Telephone Company/Ringgold Telephone Company'],
		163=>['WilTel',		'WilTel'],
		164=>['fONOROLA',	'fONOROLA'],
		165=>['Comcast',	'Comcast Cellular'],
		166=>['MicroCell',	'MicroCell 1-2-1 Inc.'],
		167=>['Independent',	'Independent Telephone Company'],
		168=>['Ameritech',	'Ameritech Communication Inc.'],
		169=>['Ameritech',	'Ameritech Communication Inc'],
		170=>['Ameritech',	'Ameritech Communication Inc'],
		171=>['Ameritech',	'Ameritech Communication Inc'],
		172=>['Pembroke',	'Pembroke Telephone Company'],
		173=>['Pac Bell',	'Pacific Bell Mobile Services'],
		174=>['BellSouth',	'BellSouth PCS'],
		175=>['Brooks Fiber',	'Brooks Fiber Communications'],
		176=>['Intermedia',	'Intermedia Communication'],
		177=>['PrimeCO',	'PrimeCO Personal Communications , L.P.'],
		178=>['Vital',		'Vital Inc.'],
		179=>['W Wireless',	'Western Wireless Corporation'],
		180=>['SNS',		'SNS Shared Network Svcs. Inc.'],
		181=>['BellSouth',	'BellSouth Cellular Corporation'],
		182=>['BellSouth',	'BellSouth Cellular Corporation'],
		183=>['Pine Belt',	'Pine Belt Telephone Company, Inc.'],
		184=>['Avantel',	'Avantel, SA'],
		185=>['Classic',	'Classic Telephone'],
		186=>['US One',		'US One Communications, Inc.'],
		187=>['Clearnet',	'Clearnet Communications, Inc.'],
		188=>['Clearnet',	'Clearnet Communications, Inc'],
		189=>['Sierra',		'Sierra Cellular, Inc.'],
		190=>['American Telco',	'American Telco, Inc'],
		191=>['APT',		'American Portable Telecom'],
		192=>['APT',		'American Portable Telecom'],
		193=>['APT',		'American Portable Telecom'],
		194=>['WorldCom',	'WorldCom'],
		195=>['WorldCom',	'WorldCom'],
		196=>['WorldCom',	'WorldCom'],
		197=>['WorldCom',	'WorldCom'],
		198=>['WorldCom',	'WorldCom'],
		199=>['WorldCom',	'WorldCom'],
		200=>['WorldCom',	'WorldCom'],
		201=>['WorldCom',	'WorldCom'],
		202=>['WorldCom',	'WorldCom'],
		203=>['Globoal Telemeida','Global Telemedia International Inc.'],
		204=>['Bell Atlantic',	'Bell Atlantic NYNEX Mobile'],
		205=>['Bell Atlantic',	'Bell Atlantic NYNEX Mobile'],
		206=>['Bell Atlantic',	'Bell Atlantic NYNEX Mobile'],
		207=>['Bell Atlantic',	'Bell Atlantic NYNEX Mobile'],
		208=>['Bell Atlantic',	'Bell Atlantic NYNEX Mobile'],
		209=>['Bell Atlantic',	'Bell Atlantic NYNEX Mobile'],
		210=>['Bell Atlantic',	'Bell Atlantic NYNEX Mobile'],
		211=>['Bell Atlantic',	'Bell Atlantic NYNEX Mobile'],
		212=>['Headwaters',	'Headwaters Telephone Company'],
		213=>['Wood Cty',	'Wood County Telephone Company'],
		214=>['Pocket',		'Pocket Communications, Inc.'],
		215=>['Bell Atlantic',	'Bell Atlantic - Virginia, Inc.'],
		216=>['360',		'360 Communications'],
		217=>['TeleBermuda',	'TeleBermuda International Ltd.'],
		218=>['Cox',		'Cox Communication'],
		219=>['Winstar',	'Winstar Telecommunications Group'],
		220=>['MIDCOM',		'MIDCOM Communications, Inc.'],
		221=>['Excel',		'Excel Switch Facility, Inc'],
		222=>['Western Wireless','Western Wireless Corporation'],
		223=>['TeleHub',	'TeleHub Network Services Corporation'],
		224=>['IXC',		'IXC Communication, Inc'],
		225=>['PrimeCo',	'PrimeCo Personal Communications'],
		226=>['PrimeCo',	'PrimeCo Personal Communications'],
		227=>['TPC',		'The Phone Company'],
		228=>['Iridium',	'Iridium LLC'],
		229=>['Teligent',	'Teligent'],
		230=>['SpectraNet',	'SpectraNet Orange'],
		231=>['US Xchange',	'US Xchange, L.L.C'],
		232=>['Bell Atlantic',	'Bell Atlantic Mobile, Inc.'],
		232=>['Bell Atlantic',	'Bell Atlantic Mobile, Inc.'],
		233=>['Bell Atlantic',	'Bell Atlantic Mobile, Inc.'],
		234=>['Bell Atlantic',	'Bell Atlantic Mobile, Inc.'],
		235=>['Bell Atlantic',	'Bell Atlantic Mobile, Inc.'],
		236=>['Pac Bell',	'Pacific Bell Mobile Services'],
		237=>['Oregon TSI',	'Oregon Telco Services, Inc'],
		238=>['Telecorp',	'Telecorp Holding Corporation, Inc.'],
		239=>['Magnacom',	'Magnacom, LLC dba PCS Plus, Inc'],
		240=>['Global Naps',	'Global Naps'],
		241=>['Trinton',	'Trinton PCS Inc.'],
		242=>['Strikenet',	'Strikenet'],
		243=>['Connect',	'Connect Holding Corporation'],
		244=>['Network Plus',	'Network Plus'],
		245=>['CapRock',	'CapRock Communications, Inc.'],
	},
	2=>{
	},
	3=>{
	},
	4=>{
	},
);
my %pc_blocks = (
#		Table B3/T1.111.8 -  List of Signalling Point Code Blocks 
#		Signalling Point Code Block
		1=>{
			  0=>['Cleartel',	'Cleartel Communications'],
			  4=>['Business',	'Business Telecom'],
			  8=>['WorldCom',	'WorldCom'],
			 12=>['National',	'National Telephone'],
#			 16=>['SPARE',		'SPARE - UNASSIGNED (was Vyvx Telephone)'],
			 20=>['Nat\'l Austin',	'National Telecommunications of Austin'],
			 24=>['WorldCom',	'WorldCom'],
			 28=>['SIS',		'Southern Interexchange Services, Inc.'],
			 32=>['NTS',		'NTS Communications'],
			 36=>['Digital Ntwk',	'Digital Network, Inc.'],
			 40=>['ATX',		'ATX Telecommunications Services'],
#			 44=>['SPARE',		'SPARE - UNASSIGNED (was Vyvx Telephone)'],
			 48=>['NNC',		'National Network Corporation'],
			 52=>['Startel',	'Startel'],
			 56=>['TMC',		'TMC of San Diego'],
#			 60=>['SPARE',		'SPARE - UNASSIGNED (was First Phone)'],
			 64=>['CNI',		'CNI'],
#			 68=>['SPARE',		'SPARE - UNASSIGNED (was WorldCom)'],
			 72=>['Wilkes',		'Wilkes Telephone & Electric Company'],
			 76=>['Sandhill',	'Sandhill Telephone Coop'],
			 80=>['Chester',	'Chester Telephone Company'],
			 84=>['St. Joseph',	'St. Joseph Telephone Company'],
			 88=>['Interstate',	'Interstate Telephone Company'],
			 92=>['Vista-United',	'Vista-United Telcom'],
			 96=>['Brandenburg',	'Brandenburg Telephone Company'],
#			100=>['SPARE',		'SPARE - UNASSIGNED (was FirstPhone)'],
			104=>['Chesnee',	'Chesnee Telephone Company'],
			108=>['Access',		'Access Long Distance'],
			112=>['La Fourche',	'La Fourche Telephone Company'],
			116=>['Evans',		'Evans Telephone Company'],
			120=>['JBN',		'JBN Telephone Company, Inc.'],
			124=>['Iowa',		'Iowa Communications Network'],
			128=>['Roanoke',	'Roanoke Telephone Company'],
			132=>['IEX',		'IEX\'s Corporation'],
			136=>['Telefonica',	'Telefonica Larga Distancia'],
			140=>['Nat\'l AL',	'National Telephone of Alabama'],
			144=>['Sears',		'Sears Technology Services'],
			148=>['PHone One',	'Phone One'],
			152=>['Orwell',		'Orwell Telephone Company'],
			156=>['Call-Net',	'Call-Net Telecom, Ltd.'],
			160=>['Fort Bend',	'Fort Bend Telephone Company'],
			164=>['Capital',	'Capital Network Systems, Inc.'],
			168=>['Northwestel',	'Northwestel, Inc.'],
			172=>['Northwestel',	'Northwestel, Inc.'],
			176=>['Gulf',		'Gulf Telephone Company'],
			180=>['Florala',	'Florala Telephone Company'],
			184=>['Marianna',	'Marianna and Scenery Hill Telephone Company'],
			188=>['Panhandle',	'Panhandle Telephone Cooperative'],
			192=>['Panhandle',	'Panhandle Telephone Cooperative'],
			196=>['Panhandle',	'Panhandle Telephone Cooperative'],
			200=>['Peoples Mutual',	'Peoples Mutual Telephone Company'],
			204=>['Bell Atlantic',	'Bell Atlantic Mobile Systems'],
			208=>['Logan',		'Logan Telephone Cooperative'],
			212=>['TresCom',	'TresCom U.S.A., Inc.'],
			216=>['Rochester',	'Rochester Telephone Company'],
			220=>['Ponderosa',	'Ponderosa Telephone Company'],
			224=>['Ponderosa',	'Ponderosa Telephone Company'],
			228=>['Ponderosa',	'Ponderosa Telephone Company'],
			232=>['Ellijay',	'Ellijay Telephone Company'],
			236=>['American',	'American Telephone Company, Inc.'],
			240=>['Loretto',	'Loretto Telephone Company, Inc.'],
			244=>['Millry',		'Millry Telephone Company, Inc.'],
			248=>['NW Indiana',	'Northwestern Indiana Telephone Company'],
			252=>['Pond Branch',	'Pond Branch Telephone Company, Inc.'],
		},
		2=>{
			  0=>['Farmers',	'Farmers Telephone Cooperative (AL)'],
			  4=>['Total-Tel',	'Total-Tel USA'],
			  8=>['Farmers',	'Farmers Telephone Cooperative (SC)'],
			 12=>['Farmers',	'Farmers Telephone Cooperative (SC)'],
			 16=>['Farmers',	'Farmers Telephone Cooperative (SC)'],
			 20=>['Farmers',	'Farmers Telephone Cooperative (SC)'],
			 24=>['Bentleyville',	'Bentleyville Telephone Company'],
			 28=>['Communique',	'Communique Telecomm.'],
			 32=>['Santa Rosa',	'Santa Rosa Telephone Cooperative'],
			 36=>['SW Oklahoma',	'Southwest Oklahoma Telephone Company'],
			 40=>['SW Oklahoma',	'Southwest Oklahoma Telephone Company'],
			 44=>['Piedmont R',	'Piedmont Rural Telephone Cooperative'],
			 48=>['Piedmont R',	'Piedmont Rural Telephone Cooperative'],
			 52=>['LDD',		'LDD, Inc.'],
			 56=>['Saco River',	'Saco River Telephone & Telegraph Company'],
			 60=>['Bruce',		'Bruce Telephone Company'],
			 64=>['Bledsoe',	'Bledsoe Telephone Cooperative'],
			 68=>['Bledsoe',	'Bledsoe Telephone Cooperative'],
			 72=>['Foothills R',	'Foothills Rural Telephone Cooperative'],
			 76=>['Germantown',	'Germantown Telephone Company, Inc.'],
			 80=>['Elkhart',	'Elkhart Telephone Company'],
			 84=>['Yell Cty',	'Yell County Telephone Company'],
			 88=>['Yell Cty',	'Yell County Telephone Company'],
			 92=>['CA-OR',		'California-Oregon Telephone Company'],
			 96=>['Crown Point',	'Crown Point Telephone'],
			100=>['Berkshire',	'Berkshire Telephone Corporation'],
			104=>['Cobbosseecontee','Cobbosseecontee Telephone & Telegraph Company'],
			108=>['Siskiyou',	'Siskiyou Telephone Company'],
			112=>['Siskiyou',	'Siskiyou Telephone Company'],
			116=>['Millry',		'Millry Telephone Company, Inc'],
			120=>['Mountain R',	'Mountain Rural Telephone'],
			124=>['Taconic',	'Taconic Telephone Corporation'],
			128=>['Champlain',	'Champlain Telephone Company'],
			132=>['Planters R',	'Planters Rural Telephone Cooperative, Inc.'],
			136=>['Dekalb',		'Dekalb Telephone Cooperative'],
			140=>['Citizens',	'Citizens Telephone Cooperative'],
			144=>['Bulloch',	'Bulloch Telephone Cooperative, Inc.'],
			148=>['NE Florida',	'Northeast Florida Telephone Company, Inc.'],
			152=>['Camden',		'Camden Telephone and Telegraph Company'],
			156=>['N Arkansas',	'Northern Arkansas Telephone Company'],
			160=>['Access',		'Access Long Distance'],
			164=>['Access',		'Access Long Distance'],
			168=>['Access',		'Access Long Distance'],
			172=>['Ballard R',	'Ballard Rural Telephone Cooperative'],
			176=>['Hancock R',	'Hancock Rural Telephone Company'],
			180=>['Madison Cty',	'Madison County Telephone Company'],
			184=>['Valu-Line',	'Valu-Line Long Distance'],
			188=>['Mountain View',	'Mountain View Telephone Company'],
			192=>['Mountain View',	'Mountain View Telephone Company'],
			196=>['Pine Tree',	'Pine Tree Telephone & Telegraph Company'],
			200=>['McLoud',		'McLoud Telephone Company'],
			204=>['Ardmore',	'Ardmore Telephone Company'],
			208=>['Cimarron',	'Cimarron Telephone Company'],
			212=>['Cimarron',	'Cimarron Telephone Company'],
			216=>['Cimarron',	'Cimarron Telephone Company'],
			220=>['Statesboro',	'Statesboro Telephone Company'],
			224=>['Waitsfield',	'Waitsfield-Flayston Telephone Company'],
			228=>['Mid-Plains',	'Mid-Plains Telephone Company, Inc.'],
			232=>['Mid-Plains',	'Mid-Plains Telephone Company, Inc.'],
			236=>['Columbus',	'Columbus Telephone Company'],
			240=>['KanOkla',	'KanOkla Telephone Association, Inc.'],
			244=>['KanOkla',	'KanOkla Telephone Association, Inc.'],
			248=>['S Central',	'South Central Telephone Association, Inc.'],
			252=>['S Central',	'South Central Telephone Association, Inc.'],
		},
		3=>{
			  0=>['S Kansas',	'Southern Kansas Telephone Company'],
			  4=>['S Kansas',	'Southern Kansas Telephone Company'],
			  8=>['Dobson',		'Dobson Telephone Company'],
			 12=>['Dobson',		'Dobson Telephone Company'],
			 16=>['Dobson',		'Dobson Telephone Company'],
			 20=>['Etex',		'Etex Telephone Cooperative, Inc.'],
			 24=>['Etex',		'Etex Telephone Cooperative, Inc.'],
			 28=>['Wheat State',	'Wheat State Telephone, Inc.'],
			 32=>['Wheat State',	'Wheat State Telephone, Inc.'],
			 36=>['Westel',		'Westel'],
			 40=>['Westel',		'Westel'],
			 44=>['Westel',		'Westel'],
			 48=>['Smithville',	'Smithville Telephone'],
			 52=>['Smithville',	'Smithville Telephone'],
			 56=>['Smithville',	'Smithville Telephone'],
			 60=>['Blackfoot',	'Blackfoot Telephone Cooperative, Inc.'],
			 64=>['Blackfoot',	'Blackfoot Telephone Cooperative, Inc.'],
			 68=>['Blackfoot',	'Blackfoot Telephone Cooperative, Inc.'],
			 72=>['Amer Sharecom',	'American Sharecom'],
			 76=>['Amer Sharecom',	'American Sharecom'],
			 80=>['Amer Sharecom',	'American Sharecom'],
			 84=>['P Spencer R',	'Perry Spencer Rural Telephone Cooperative'],
			 88=>['Steelville',	'Steelville Telephone Exchange'],
			 92=>['Chibardun',	'Chibardun Telephone Cooperative, Inc.'],
			 96=>['Chibardun',	'Chibardun Telephone Cooperative, Inc.'],
			100=>['S C Rural',	'South Central Rural Telephone Cooperative Corporation, Inc.'],
			104=>['United',		'United Telephone Company, Inc.'],
			108=>['Nat.TeleSvc',	'National TeleService'],
			112=>['Execulines',	'Execulines of the Northwest, Inc.'],
			116=>['Wilson',		'Wilson Telephone Company'],
			120=>['Rochester',	'Rochester Telephone'],
			124=>['Rochester',	'Rochester Telephone'],
			128=>['Rochester',	'Rochester Telephone'],
			132=>['Rochester',	'Rochester Telephone'],
			136=>['Rochester',	'Rochester Telephone'],
			140=>['Rochester',	'Rochester Telephone'],
			144=>['Vernon',		'Vernon Telephone Cooperative'],
			148=>['Vernon',		'Vernon Telephone Cooperative'],
			152=>['Union',		'Union Telephone Company'],
			156=>['Union',		'Union Telephone Company'],
			160=>['Dunbarton',	'Dunbarton Telephone Company'],
			164=>['Palmetto R',	'Palmetto Rural Telephone Cooperative, Inc.'],
			168=>['Green Hills',	'Green Hills Telephone Corporation'],
			172=>['Conestoga',	'Conestoga Telephone & Telegraph Company'],
			176=>['Roanoke & B',	'Roanoke & Botetourt Telephone Company'],
			180=>['C Oklahoma',	'Central Oklahoma Telephone Company'],
			184=>['C Oklahoma',	'Central Oklahoma Telephone Company'],
			188=>['Pierce',		'Pierce Telephone Company, Inc.'],
			192=>['Mid-Missouri',	'Mid-Missouri Telephone Company'],
			196=>['Mid-Missouri',	'Mid-Missouri Telephone Company'],
			200=>['Mid-Missouri',	'Mid-Missouri Telephone Company'],
			204=>['Mid-Missouri',	'Mid-Missouri Telephone Company'],
			208=>['Chequamegon',	'Chequamegon Telephone'],
			212=>['Chequamegon',	'Chequamegon Telephone'],
			216=>['Chequamegon',	'Chequamegon Telephone'],
			220=>['Fidelity',	'Fidelity Telephone Company'],
			224=>['P Spencer R',	'Perry Spencer Rural Telephone Cooperative'],
			228=>['Utelco',		'Utelco, Inc.'],
			232=>['Ontario',	'Ontario Telephone Company, Inc.'],
			236=>['Germantown',	'Germantown Independent Telephone'],
			240=>['Kerman',		'Kerman Telephone Company'],
			244=>['Hayneville',	'Hayneville Telephone Company, Inc.'],
			248=>['Bruce Mun.',	'Bruce Municipal Telephone System'],
			252=>['Granite State',	'Granite State Telephone'],
		},
		4=>{
			  0=>['Skyline',	'Skyline Telephone Membership Corporation'],
			  4=>['Skyline',	'Skyline Telephone Membership Corporation'],
			  8=>['Skyline',	'Skyline Telephone Membership Corporation'],
			 12=>['Skyline',	'Skyline Telephone Membership Corporation'],
			 16=>['Community Svc.',	'Community Service Telephone Company'],
			 20=>['Community Svc.',	'Community Service Telephone Company'],
			 24=>['Headwaters',	'Headwaters Telephone Company'],
			 28=>['W Tennessee',	'West Tennessee Telephone Company'],
			 32=>['Peoples',	'Peoples Telephone Company, Inc.'],
			 36=>['Crockett',	'Crockett Telephone Company'],
			 40=>['Bay Springs',	'Bay Springs Telephone Company, Inc.'],
			 44=>['Ellerbe',	'Ellerbe Telephone Company'],
			 48=>['Pineville',	'Pineville Telephone Company'],
			 52=>['Randolph',	'Randolph Telephone Company'],
			 56=>['B Lomand R',	'Ben Lomand Rural Telephone Cooperative, Inc.'],
			 60=>['B Lomand R',	'Ben Lomand Rural Telephone Cooperative, Inc.'],
			 64=>['B Lomand R',	'Ben Lomand Rural Telephone Cooperative, Inc.'],
			 68=>['B Lomand R',	'Ben Lomand Rural Telephone Cooperative, Inc.'],
			 72=>['Mountain R',	'Mountain Rural Telephone'],
			 76=>['Gila River',	'Gila River Telecommunications, Inc.'],
			 80=>['Gila River',	'Gila River Telecommunications, Inc.'],
			 84=>['Gila River',	'Gila River Telecommunications, Inc.'],
			 88=>['NE Missouri R',	'Northeast Missouri Rural Telephone Company'],
			 92=>['Highland',	'Highland Telephone Cooperative, Inc.'],
			 96=>['Highland',	'Highland Telephone Cooperative, Inc.'],
			100=>['Highland',	'Highland Telephone Cooperative, Inc.'],
			104=>['RTSC',		'Rural Telephone Service Company, Inc.'],
			108=>['RTSC',		'Rural Telephone Service Company, Inc.'],
			112=>['Peoples R',	'Peoples Rural Telephone Cooperative Corporation'],
			116=>['EDS Personal',	'EDS Personal Communications Corporation'],
			120=>['EDS Personal',	'EDS Personal Communications Corporation'],
			124=>['West Side',	'West Side Telephone'],
			128=>['Citizens',	'Citizens Telephone Cooperative, Inc.'],
			132=>['W Carolina R',	'West Carolina Rural Telephone Cooperative, Inc.'],
			136=>['W Carolina R',	'West Carolina Rural Telephone Cooperative, Inc.'],
			140=>['Twin Valley',	'Twin Valley Telephone, Inc.'],
			144=>['Twin Valley',	'Twin Valley Telephone, Inc.'],
#			148=>['SPARE',		'SPARE - UNASSIGNED'],
#			152=>['SPARE',		'SPARE - UNASSIGNED'],
			156=>['Teleport',	'Teleport Communications Group'],
			160=>['Teleport',	'Teleport Communications Group'],
			164=>['Teleport',	'Teleport Communications Group'],
			168=>['Teleport',	'Teleport Communications Group'],
			172=>['Atlas',		'Atlas Telephone Company'],
			176=>['Atlas',		'Atlas Telephone Company'],
			180=>['Hickory',	'Hickory Telephone Company'],
			184=>['S Spavinaw',	'Salina Spavinaw Telephone Company, Inc.'],
			188=>['S Spavinaw',	'Salina Spavinaw Telephone Company, Inc.'],
			192=>['La Ward',	'La Ward Telephone Exchange, Inc.'],
			196=>['Rib Lake',	'Rib Lake Telephone Company'],
			200=>['Citizens',	'Citizens Telephone Company of Kecksburg'],
			204=>['Kingdom',	'Kingdom Telephone Company'],
			208=>['Kingdom',	'Kingdom Telephone Company'],
			212=>['Kingdom',	'Kingdom Telephone Company'],
			216=>['Kingdom',	'Kingdom Telephone Company'],
			220=>['Carnegie',		'Carnegie Telephone Company'],
			224=>['Ace',		'Ace Telephone Company of Michigan, Inc.'],
			228=>['Ace',		'Ace Telephone Company of Michigan, Inc.'],
			232=>['Hurontario',	'Hurontario Telephones Ltd.'],
			236=>['N Norwich',	'North Norwich Telephones Ltd.'],
			240=>['Durham',		'Durham Telephones Ltd.'],
			244=>['Vadacom',	'Vadacom'],
			248=>['Vadacom',	'Vadacom'],
			252=>['Gosfield',	'Gosfield North Municipal Telephone System'],
		},
		5=>{
			  0=>['NE Missouri',	'Northeast Missouri Rural Telephone Company'],
			  4=>['NE Missouri',	'Northeast Missouri Rural Telephone Company'],
			  8=>['NE Missouri',	'Northeast Missouri Rural Telephone Company'],
			 12=>['CTG',		'Corporate Telemanagement Group'],
			 16=>['CTG',		'Corporate Telemanagement Group'],
			 20=>['Mustang',	'Mustang Telecom, Inc.'],
			 24=>['Mustang',	'Mustang Telecom, Inc.'],
			 28=>['Mustang',	'Mustang Telecom, Inc.'],
			 32=>['Zenda',		'Zenda Telephone Company, Inc.'],
			 36=>['San Marcos',	'San Marcos Telephone Company'],
			 40=>['Pymatuning',	'Pymatuning Independent Telephone Company'],
			 44=>['Pine',		'Pine Telephone Company'],
			 48=>['Pine',		'Pine Telephone Company'],
			 52=>['Fort Mojave',	'Fort Mojave Telecommunications, Inc.'],
			 56=>['Fort Mojave',	'Fort Mojave Telecommunications, Inc.'],
			 60=>['Oklahoma W',	'Oklahoma Western Telephone Company'],
			 64=>['Oklahoma W',	'Oklahoma Western Telephone Company'],
			 68=>['Oklahoma W',	'Oklahoma Western Telephone Company'],
			 72=>['Oklahoma W',	'Oklahoma Western Telephone Company'],
			 76=>['Hinton',		'Hinton Telephone Company'],
			 80=>['Hinton',		'Hinton Telephone Company'],
			 84=>['Hinton',		'Hinton Telephone Company'],
			 88=>['ALL',		'American Long Lines'],
			 92=>['Vernon',		'Vernon Telephone Cooperative'],
			 96=>['W Wisconsin',	'West Wisconsin Telcommunications Cooperative, Inc.'],
			100=>['W Wisconsin',	'West Wisconsin Telcommunications Cooperative, Inc.'],
			104=>['Northeast',	'Northeast Telephone Company'],
			108=>['Coon Valley',	'Coon Valley Farmers Telephone Company'],
			112=>['Coon Valley',	'Coon Valley Farmers Telephone Company'],
			116=>['Cross',		'Cross Telephone'],
			120=>['Cross',		'Cross Telephone'],
			124=>['Cross',		'Cross Telephone'],
			128=>['Cambridge',	'Cambridge Telephone Company'],
			132=>['Peoples Mutual',	'Peoples Mutual Telephone Company'],
			136=>['Craw-Kan',	'Craw-Kan Telephone Cooperative, Inc.'],
			140=>['Craw-Kan',	'Craw-Kan Telephone Cooperative, Inc.'],
			144=>['Craw-Kan',	'Craw-Kan Telephone Cooperative, Inc.'],
			148=>['Monon',		'Monon Telephone Company, Inc.'],
			152=>['Muenster',	'Muenster Telephone Corporation of Texas'],
			156=>['Blue Valley',	'Blue Valley Telephone Company'],
			160=>['Moundridge',	'Moundridge Telephone Company'],
			164=>['Vaughnsville',	'Vaughnsville Telephone Company, Inc.'],
			168=>['Kalida',		'Kalida Telephone Company, Inc.'],
			172=>['Grand',		'Grand Telephone Company, Inc.'],
			176=>['Scott-Rice',	'Scott-Rice Telephone Company, Inc.'],
			180=>['Tri-County',	'Tri-County Telephone Association, Inc.'],
			184=>['US Wats',	'US Wats'],
			188=>['Richland-G',	'Richland-Grant Telephone Cooperative, Inc.'],
			192=>['Richland-G',	'Richland-Grant Telephone Cooperative, Inc.'],
			196=>['Richland-G',	'LaValle Telephone Cooperative, Inc.'],
			200=>['Arthur Mutual',	'The Arthur Mutual Telephone Company'],
			204=>['Price Cty',	'Price County Telephone Company'],
			208=>['Nova',		'The Nova Telephone Company'],
			212=>['New Ulm',	'New Ulm Telecom, Inc.'],
			216=>['Pottawatomie',	'Pottawatomie Telephone Company, Inc.'],
			220=>['Pottawatomie',	'Pottawatomie Telephone Company, Inc.'],
			224=>['Pottawatomie',	'Pottawatomie Telephone Company, Inc.'],
			228=>['H&B',		'H&B Communications, Inc.'],
			232=>['Milltown',	'Milltown Mutual Telephone Company'],
			236=>['Wamego',		'Wamego Telephone Company, Inc.'],
			240=>['Luck',		'Luck Telephone Company'],
			244=>['Farmers Ind.',	'Farmers Independent Telephone Company'],
			248=>['Mid-Iowa',	'Mid-Iowa Telephone Co-Op Association'],
			252=>['Lewis River',	'Lewis River Telephone'],
		},
		6=>{
			  0=>['Kin Network',	'Kin Network, Inc.'],
			  4=>['Conneaut',	'Conneaut Telephone Company'],
			  8=>['Madison',	'Madison Telephone Company, Inc.'],
			 12=>['Hartman',	'Hartman Telephone Exchanges'],
			 16=>['Rainbow',	'Rainbow Telephone Cooperative Association, Inc.'],
			 20=>['Golden Belt',	'Golden Belt Telephone Association, Inc.'],
			 24=>['Middleburgh',	'Middleburgh Telephone Company'],
			 28=>['Ganado',		'Ganado Telephone Company'],
			 32=>['Coleman Cty',	'Coleman County Telephone Cooperative, Inc.'],
			 36=>['Comanche Cty',	'Comanche County Telephone Company, Inc.'],
			 40=>['Comanche Cty',	'Comanche County Telephone Company, Inc.'],
			 44=>['Lake Dallas',	'Lake Dallas Telephone Company'],
			 48=>['Curtis',		'The Curtis Telephone Company'],
			 52=>['Gridley',	'Gridley Telephone Company'],
			 56=>['SP Telecom',	'SP Telecom'],
			 60=>['Guadalupe Vly',	'Guadalupe Valley Telephone Cooperative'],
			 64=>['Guadalupe Vly',	'Guadalupe Valley Telephone Cooperative'],
			 68=>['Guadalupe Vly',	'Guadalupe Valley Telephone Cooperative'],
			 72=>['Guadalupe Vly',	'Guadalupe Valley Telephone Cooperative'],
			 76=>['Twin Lakes',	'Twin Lakes Telephone Cooperative Corporation'],
			 80=>['Twin Lakes',	'Twin Lakes Telephone Cooperative Corporation'],
			 84=>['Twin Lakes',	'Twin Lakes Telephone Cooperative Corporation'],
			 88=>['Twin Lakes',	'Twin Lakes Telephone Cooperative Corporation'],
			 92=>['Stellars',	'Stellars Telecommunications Services'],
			 96=>['Intercont.',	'Intercontinental Communications Corp.'],
			100=>['Intercont.',	'Intercontinental Communications Corp.'],
			104=>['Switch 2000',	'Switch 2000'],
			108=>['Switch 2000',	'Switch 2000'],
			112=>['Colorado Vly',	'Colorado Valley Telephone Cooperative'],
			116=>['Colorado Vly',	'Colorado Valley Telephone Cooperative'],
			120=>['Hooper',		'Hooper Telephone Company'],
			124=>['Ottoville M',	'Ottoville Mutual Telephone Company'],
			128=>['Keystone-Arthur','Keystone-Arthur Telephone Company'],
			132=>['S&W',		'S&W Telephone Company, Inc.'],
			136=>['Cochrane Coop',	'Cochrane Cooperative Telephone Company'],
			140=>['Knoxville',	'The New Knoxville Telephone Company'],
			144=>['Climax',		'Climax Telephone Company'],
			148=>['Middle Point',	'Middle Point Home Telephone Company'],
			152=>['E. Ritter',	'E. Ritter Telephone Company'],
			156=>['Union',		'Union Telephone Company'],
			160=>['Atlantic',	'Atlantic Telephone Membership Corporation'],
			164=>['Ayersville',	'Ayersville Telephone Company'],
			168=>['Arkansas',	'Arkansas Telephone Company, Inc.'],
			172=>['Daviess-M Cty',	'Daviess-Martin County Rural Telephone Corporation'],
			176=>['Granite State',	'Granite State Telephone'],
			180=>['Totah',		'Totah Telephone Company'],
			184=>['Totah',		'Totah Telephone Company'],
			188=>['Totah',		'Totah Telephone Company'],
			192=>['Villisca F',	'Villisca Farmers Telephone Company'],
			196=>['Standard',	'Standard Telephone Company'],
			200=>['N Central',	'North Central Telephone Cooperative'],
			204=>['N Central',	'North Central Telephone Cooperative'],
			208=>['TMC',		'TMC of Lexington'],
			212=>['Network',	'Network'],
			216=>['McClure',	'The McClure Telephone Company'],
			220=>['Yukon-Waltz',	'Yukon-Waltz Telephone Company'],
			224=>['Fonorola',	'Fonorola'],
			228=>['Glasford',	'Glasford Telephone Company'],
			232=>['Benton',		'Benton Cooperative Telephone Company'],
			236=>['Union Springs',	'Union Springs Telephone Company'],
			240=>['Ft. Jennings',	'Ft. Jennings Telephone Company'],
			244=>['Farmers Mutual',	'Farmers Mutual Telephone Company'],
			248=>['Pattersonville',	'Pattersonville Telephone Company'],
			252=>['ITG',		'International Telemanagement Group'],
		},
		7=>{
			  0=>['Pac Gateway',	'Pacific Gateway Exchange'],
			  4=>['Pac Gateway',	'Pacific Gateway Exchange'],
			  8=>['Elec Lightwave',	'Electric Lightwave'],
			 12=>['Allendale',	'Allendale Telephone Company'],
			 16=>['Decatur',	'Decatur Telephone Company, Inc.'],
			 20=>['Bell Mobility',	'Bell Mobility Cellular'],
			 24=>['Five Star',	'Five Star Telecom'],
			 28=>['TTI',		'TTI Telecommunications'],
			 32=>['Graceba',	'Graceba Total Communications, Inc.'],
			 36=>['SW Arkansas',	'Southwest Arkansas Telephone Cooperative'],
			 40=>['SW Arkansas',	'Southwest Arkansas Telephone Cooperative'],
			 44=>['Ogden',		'Ogden Telephone Company'],
			 48=>['Township',	'Township Telephone Company'],
			 52=>['Bloutsville',	'Bloutsville Telephone Company'],
			 56=>['Coastal',	'Coastal Telephone Company'],
			 60=>['Armstrong',	'Armstrong Telephone Company'],
			 64=>['Armstrong',	'Armstrong Telephone Company'],
			 68=>['Delta',		'Delta Telephone Company, Inc.'],
			 72=>['Franklin',	'Franklin Telephone Company, Inc.'],
			 76=>['Ridgeville',	'Ridgeville Telephone Company'],
			 80=>['Community',	'Community Telephone'],
			 84=>['St. Joseph',	'St. Joseph Telephone and Telegraph Company'],
			 88=>['St. Joseph',	'St. Joseph Telephone and Telegraph Company'],
			 92=>['St. Joseph',	'St. Joseph Telephone and Telegraph Company'],
			 96=>['Peoples Mutual',	'Peoples Mutual Telephone Company'],
			100=>['Cameron',	'Cameron Telephone Company'],
			104=>['Action',		'Action Telcom Company'],
			108=>['TCC',		'TCC Communications'],
			112=>['Smithville',	'Smithville Telephone Company, Inc.'],
			116=>['Oneida Cty',	'Oneida County Rural Telephone Company'],
			120=>['Emily Coop',	'Emily Cooperative Telephone Company'],
			124=>['Beehive',	'Beehive Telephone Company, Inc.'],
			128=>['Custer',		'Custer Telephone Cooperative, Inc.'],
			132=>['MGW',		'MGW Telephone'],
			136=>['Call America',	'Call America'],
			140=>['Blanchard',	'Blanchard Telephone Association, Inc.'],
			144=>['Tonica',		'Tonica Telephone Company'],
			148=>['Pine Belt',	'Pine Belt Telephone Company, Inc.'],
			152=>['Rockland',	'Rockland Telephone Company, Inc.'],
			156=>['Sogetel',	'Sogetel, Inc.'],
			160=>['Hamilton Cty',	'Hamilton County Telephone Cooperative'],
			164=>['Fleet Call',	'Fleet Call'],
			168=>['New Lisbon',	'New Lisbon Telephone Company'],
			172=>['West River',	'West River Telecommunications Cooperative'],
			176=>['West River',	'West River Telecommunications Cooperative'],
			180=>['Nebraska C',	'Nebraska Central Telephone Company'],
			184=>['Coastal',	'Coastal Utilities, Inc.'],
			188=>['Diller',		'Diller Telephone Company'],
			192=>['HunTel',		'HunTel Systems'],
			196=>['Progressive R',	'Progressive Rural Telephone Cooperative, Inc.'],
			200=>['Delta',		'Delta Telephone Company, Inc.'],
			204=>['K&M',		'K&M Telephone Company, Inc.'],
			208=>['Washington City','Washington City Rural Telephone Cooperative, Inc.'],
			212=>['Hanson',		'Hanson Communications'],
			216=>['Hanson',		'Hanson Communications'],
			220=>['Leaf River',	'Leaf River Telephone Company'],
			224=>['New Windsor',	'New Windsor Telephone Company'],
			228=>['Lakefield',	'Lakefield Telephone Company'],
			232=>['Hart',		'Hart Telephone Company'],
			236=>['Margaretville',	'Margaretville Telephone Company, Inc.'],
			240=>['Warwick Valley',	'Warwick Valley Telephone Company'],
			244=>['Cherokee',	'Cherokee Telephone Company'],
			248=>['Cherokee',	'Cherokee Telephone Company'],
			252=>['Cherokee',	'Cherokee Telephone Company'],
		},
		8=>{
			  0=>['Cherokee',	'Cherokee Telephone Company'],
			  4=>['Deerfield F',	'Deerfield Farmers\' Telephone Company'],
			  8=>['Suntel',		'Suntel Network'],
			 12=>['Mid-Century',	'Mid-Century Telephone Company'],
			 16=>['Madison',	'Madison Telephone Company'],
			 20=>['Amnex',		'Amnex Network'],
			 24=>['Amnex',		'Amnex Network'],
			 28=>['ConQuest',	'ConQuest'],
			 32=>['Thunder Bay',	'Thunder Bay Telephone'],
			 36=>['Viola',		'Viola Telephone Company'],
			 40=>['Reynolds',	'Reynolds Telephone Company'],
			 44=>['Yeoman',		'Yeoman Telephone Company'],
			 48=>['Harrisonville',	'Harrisonville Telephone Company'],
			 52=>['Craigville',	'Craigville Telephone Company, Inc.'],
			 56=>['Clarence',	'Clarence Telephone Company'],
			 60=>['N Illinois Univ','Northern Illinois University'],
			 64=>['One call',	'One call Communications, Inc.'],
			 68=>['One call',	'One call Communications, Inc.'],
			 72=>['Churchill City',	'Churchill City Telephone & Telegraph System'],
			 76=>['Chippewa City',	'Chippewa City Telephone Company'],
			 80=>['Roberts City',	'Roberts City Telephone Cooperative Association'],
			 84=>['Bretton Woods',	'Bretton Woods Telephone Company'],
			 88=>['Eastern Telecom','Eastern Telecom Corporation'],
			 92=>['Northwest',	'Northwest Telephone Company'],
			 96=>['Northwest',	'Northwest Telephone Company'],
			100=>['ICON',		'ICON'],
			104=>['Bittel',		'Bittel Telecommunications Corporation'],
			108=>['Bittel',		'Bittel Telecommunications Corporation'],
			112=>['CTI',		'Communications TeleSystems Int\'l'],
			116=>['Thacker-G',	'Thacker - Grigsby Telephone Company, Inc.'],
			120=>['Thrifty',	'Thrifty Telephone, Inc.'],
			124=>['Telco',		'Telco Communications, Inc.'],
			128=>['Applied Signal',	'Applied Signal Corporation'],
			132=>['Wilton',		'Wilton Telephone Company'],
			136=>['ANS',		'Alternative Network Services'],
			140=>['ANS',		'Alternative Network Services'],
			144=>['Telstar',	'Telstar Communications, Inc.'],
			148=>['Telstar',	'Telstar Communications, Inc.'],
			152=>['ITC',		'ITC Companies'],
			156=>['ITC',		'ITC Companies'],
			160=>['United',		'United Communications (UNICOM)'],
			164=>['BN1',		'BN1 Telecommunications'],
			168=>['Wabash',		'Wabash Telephone Company'],
			172=>['Flat Rock',	'Flat Rock Telephone Company'],
			176=>['Woodhull',	'Woodhull Telephone Company'],
			180=>['BellSouth',	'BellSouth Mobility'],
			184=>['BellSouth',	'BellSouth Mobility'],
			188=>['BellSouth',	'BellSouth Mobility'],
			192=>['BellSouth',	'BellSouth Mobility'],
			196=>['Hi-Rim',		'Hi-Rim Communications'],
			200=>['Hi-Rim',		'Hi-Rim Communications'],
			204=>['Prairie Grove',	'Prairie Grove Telephone Company'],
			208=>['Prairie Grove',	'Prairie Grove Telephone Company'],
			212=>['Dakota',		'Dakota Cooperative Telecommunications, Inc.'],
			216=>['Dakota',		'Dakota Cooperative Telecommunications, Inc.'],
			220=>['Dakota',		'Dakota Cooperative Telecommunications, Inc.'],
			224=>['Dakota',		'Dakota Cooperative Telecommunications, Inc.'],
			228=>['Mebtel',		'Mebtel Communications'],
			232=>['Glandorf',	'Glandorf Telephone Company, Inc.'],
			236=>['Waldron',	'Waldron Telephone Company'],
			240=>['Indiantown',	'Indiantown Telephone System, Inc.'],
			244=>['Big Bend',	'Big Bend Telephone'],
			248=>['Fibertech',	'Fibertech Telecom, Inc.'],
			252=>['Fibertech',	'Fibertech Telecom, Inc.'],
		},
		9=>{
			  0=>['Fibertech',	'Fibertech Telecom, Inc.'],
			  4=>['Dial US',	'Dial US'],
			  8=>['Piedmont',	'Piedmont Telephone Membership Corporation'],
			 12=>['MRC',		'MRC Telecommunications, Inc.'],
			 16=>['Telco',		'Telco Communications Group'],
			 20=>['Telco',		'Telco Communications Group'],
			 24=>['Granby',		'Granby Telephone and Telegraph Company'],
			 28=>['STSJ',		'STSJ Telephone Company'],
			 32=>['Big Sandy',	'Big Sandy Telecom'],
			 36=>['State',		'State Telephone Company'],
			 40=>['Ringgold',	'Ringgold Telephone Company'],
			 44=>['InterAmerican',	'InterAmerican Telephone Company'],
			 48=>['Union River',	'Union River Telephone Company'],
			 52=>['Flex',		'Flex Communications'],
			 56=>['Sand Creek',	'Sand Creek Telephone Company'],
			 60=>['TotalNet',	'TotalNet Communications, Inc.'],
			 64=>['BEK',		'BEK Communications Cooperative'],
			 68=>['Dakota Central',	'Dakota Central Telecommunication Cooperative'],
			 72=>['Teleport',	'Teleport Communications Group'],
			 76=>['Teleport',	'Teleport Communications Group'],
			 80=>['Teleport',	'Teleport Communications Group'],
			 84=>['Teleport',	'Teleport Communications Group'],
			 88=>['Teleport',	'Teleport Communications Group'],
			 92=>['Huntel',		'Huntel Systems, Inc.'],
			 96=>['TRI',		'TRI Touch America'],
			100=>['TRI',		'TRI Touch America'],
			104=>['Star',		'Star Telephone Membership Corporation'],
			108=>['Star',		'Star Telephone Membership Corporation'],
			112=>['Star',		'Star Telephone Membership Corporation'],
			116=>['Indianhead',	'Indianhead Telephone Company'],
			120=>['Polar',		'Polar Communications Mutual Aid Corporation'],
			124=>['Multimedia',	'Multimedia Telephone Service'],
			128=>['Multimedia',	'Multimedia Telephone Service'],
#			132=>['SPARE',		'SPARE - UNASSIGNED'],
			136=>['Total',		'Total World Telecom'],
			140=>['Total',		'Total World Telecom'],
			144=>['Total',		'Total World Telecom'],
			148=>['Total',		'Total World Telecom'],
			152=>['ATCI',		'ATCI Inc.'],
			156=>['ATCI',		'Econo Call/ATCI (USA)'],
			160=>['ATCI',		'Econo Call/ATCI (USA)'],
			164=>['Ardmore',	'Ardmore Telephone Company, Inc.'],
			168=>['Niagara',	'Niagara Telephone Company'],
			172=>['Mabel Coop',	'Mabel Cooperative Telephone Company'],
			176=>['Fox',		'Fox Communications Corporation'],
			180=>['Spring Grove',	'Spring Grove Cooperative Telephone'],
			184=>['Ogden',		'Ogden Telephone Company'],
			188=>['Souris River',	'Souris River Telecommunications Cooperative'],
			192=>['Medicine Park',	'Medicine Park Telephone Company'],
			196=>['Yelcot',		'Yelcot Telephone Company'],
			200=>['Sunman',		'Sunman Telephone'],
			204=>['Home',		'Home Telephone Company'],
			208=>['Southeast',	'Southeast Telephone of WI, Inc.'],
			212=>['Springport',	'Springport Telephone Company'],
			216=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			220=>['CGN',		'Communications Gateway Network, Inc.'],
			224=>['CGN',		'Communications Gateway Network, Inc.'],
			228=>['CGN',		'Communications Gateway Network, Inc.'],
			232=>['Vista',		'Vista Telephone'],
			236=>['Clay Cty',	'Clay County Rural Telephone Cooperative, Inc.'],
			244=>['McDonough',	'McDonough Telephone Company'],
			248=>['Oneida',		'Oneida Telephone Company'],
			252=>['LaHarpe',	'LaHarpe Telephone Company'],
		},
		10=>{
			  0=>['Great Plains',	'Great Plains Communications'],
			  4=>['Great Plains',	'Great Plains Communications'],
			  7=>['Great Plains',	'Great Plains Communications'],
			 12=>['Great Plains',	'Great Plains Communications'],
			 16=>['Great Plains',	'Great Plains Communications'],
			 20=>['Great Plains',	'Great Plains Communications'],
			 24=>['Great Plains',	'Great Plains Communications'],
			 28=>['Northland',	'Northland Telephone'],
			 32=>['Brooke',		'Brooke Municipal Telephone System'],
			 36=>['LCT',		'LCT Long Distance'],
			 40=>['Cablevision',	'Cablevision'],
			 44=>['Oxford City',	'Oxford City Telephone and Telegraph Company'],
			 48=>['Oxford City',	'Oxford City Telephone and Telegraph Company'],
			 52=>['Oxford City',	'Oxford City Telephone and Telegraph Company'],
			 56=>['Logan',		'Logan Telephone Cooperative, Inc'],
			 60=>['Logan',		'Logan Telephone Cooperative, Inc'],
			 64=>['ETI',		'Executive TeleCard International'],
			 68=>['Farmers',	'Farmers Telephone'],
			 72=>['S C Utah',	'South Central Utah Telephone Association'],
			 76=>['Griggs City',	'Griggs City Telephone Company'],
			 80=>['Teltrust',	'Teltrust'],
			 84=>['Teltrust',	'Teltrust'],
			 88=>['Western',	'Western Telephone Company'],
			 92=>['Tidewater',	'Tidewater Telecom'],
			 96=>['Glens Falls',	'Glens Falls Communications Corporation'],
			100=>['Pac-West',	'Pac-West Telecomm, Inc. dba AmeriCall'],
#			104=>['SPARE',		'SPARE - UNASSIGNED'],
#			108=>['SPARE',		'SPARE - UNASSIGNED'],
			112=>['Clay Cty',	'Clay County Rural Telephone Cooperative, Inc.'],
			116=>['Clay Cty',	'Clay County Rural Telephone Cooperative, Inc.'],
			120=>['CCI',		'Communication Consultants, Inc.'],
			124=>['Mornington',	'Mornington Municipal Telephone System'],
			128=>['Blanchard',	'Blanchard Municipal Telephone System'],
			132=>['Citizens',	'Citizens Telephone'],
			136=>['Siren',		'Siren Telephone Company, Inc.'],
			140=>['TimeWarner',	'TimeWarner Communications'],
			144=>['TimeWarner',	'TimeWarner Communications'],
			148=>['Minkfort',	'Minkfort Intelligent Network'],
			152=>['Surry',		'Surry Telephone Membership Corporation'],
			156=>['Surry',		'Surry Telephone Membership Corporation'],
			160=>['Wes-Tex',	'Wes-Tex Telephone Cooperative Inc.'],
			164=>['Cypress',	'Cypress Telecommunications, Inc./Cytel.'],
			168=>['Cypress',	'Cypress Telecommunications, Inc./Cytel'],
			172=>['Cypress',	'Cypress Telecommunications, Inc./Cytel'],
			176=>['LaJicarita R',	'LaJicarita Rural Telephone Coop'],
			180=>['Rye',		'Rye Telephone Company'],
			184=>['Alma',		'Alma Telephone Company'],
			188=>['Cal-North',	'Cal-North Cellular'],
			192=>['Five Area',	'Five Area Telephone Cooperative, Inc.'],
			196=>['Eastex',		'Eastex Telephone Cooperative, Inc.'],
			200=>['Wood Cty',	'Wood County Telephone Company'],
			204=>['La Compagnie',	'La Compagnie de Tel. de Warwick'],
			208=>['Amtelecom',	'Amtelecom, Inc.'],
			212=>['Granby',		'Granby Telephone Company'],
			216=>['CellularONE',	'CellularONE'],
			220=>['Thrifty',	'Thrifty Telephone, Inc.'],
			224=>['Kaplan',		'Kaplan Telephone Company'],
			228=>['SW Texas',	'Southwest Texas Telephone Company'],
			232=>['Shawnee',	'Shawnee Telephone Company'],
			236=>['S Canaan',	'South Canaan Telephone Company'],
			240=>['ATG',		'American Telephone Group'],
			244=>['Bruce',		'Bruce Municipal Telephone System'],
			248=>['MTC',		'MTC Telemanagement'],
			252=>['MTC',		'MTC'],
		},
		11=>{
			  0=>['Winn',		'Winn Telephone Company'],
			  4=>['Orwell',		'Orwell Telephone Company'],
			  8=>['CellularOne',	'CellularOne (Puerto Rico)'],
#			 12=>['SPARE',		'SPARE - UNASSIGNED'],
#			 16=>['SPARE',		'SPARE - UNASSIGNED'],
			 20=>['Econophone',	'Econophone Inc.'],
			 24=>['Econophone',	'Econophone Inc.'],
			 28=>['Econophone',	'Econophone Inc.'],
			 32=>['SE IN R',	'Southeastern IN Rural Telephone Cooperative Inc.'],
			 36=>['Leaco R',	'Leaco Rural Telephone Cooperative, Inc.'],
			 40=>['Mt. Horeb',	'Mt. Horeb Telephone Company'],
			 44=>['Otelco',		'Otelco'],
			 48=>['CallAmerica',	'CallAmerica'],
			 52=>['CallAmerica',	'CallAmerica'],
			 56=>['Alhambra-G',	'Alhambra-Grantfork Telephone Company'],
			 60=>['Grafton',	'Grafton Telephone Company'],
			 64=>['Home',		'Home Telephone Company'],
			 68=>['McDonald',	'McDonald Telephone Company'],
			 72=>['McLeod',		'McLeod Network Services'],
			 76=>['Westphalia',	'Westphalia Telephone Company'],
			 80=>['Amherst',	'Amherst Telephone Company'],
			 84=>['Carr',		'Carr Telephone'],
			 88=>['Carr',		'Carr Telephone'],
			 92=>['Cyberlink',	'Cyberlink'],
			 96=>['Pine Drive',	'Pine Drive Telephone Company'],
			100=>['Hyperion',	'Hyperion Telecommunications, Inc.'],
			104=>['Hyperion',	'Hyperion Telecommunications, Inc.'],
			108=>['Brantley',	'Brantley Telephone Company'],
			112=>['TelRoute',	'TelRoute Communications, Inc.'],
			116=>['TelRoute',	'TelRoute Communications, Inc.'],
			120=>['Star',		'Star Telephone'],
			124=>['Star',		'Star Telephone'],
			128=>['Star',		'Star Telephone'],
			132=>['S Carolina Net',	'South Carolina Net, Inc.'],
			136=>['Armour Ind.',	'Armour Independent Telephone Company'],
			140=>['Union',		'Union Telephone Company'],
			144=>['Baldwin',	'Baldwin Telecom, Inc.'],
			148=>['Cass Cty',	'Cass County Telephone Company'],
			152=>['C-R',		'C-R Telephone Company'],
			156=>['ETC',		'Eastern TeleLogic Corporation'],
			160=>['Tri-County',	'Tri-County Telephone Cooperative'],
			164=>['Citizens',	'Citizens Telephone Company'],
			168=>['M Adams',	'Marquette Adams Telephone Cooperative'],
			172=>['St. Liboire',	'Le Tel de St. Liboire'],
			176=>['Bestline',	'Austin Bestline'],
			180=>['N Dakota',	'North Dakota Telephone Company'],
			184=>['TSC',		'Telecommunications Service Center, Inc (TSC)'],
			188=>['TSC',		'Telecommunications Service Center, Inc (TSC)'],
			192=>['Elec Lightwave',	'Electric Lightwave, Inc.'],
			196=>['Elec Lightwave',	'Electric Lightwave, Inc.'],
#			200=>['SPARE',		'SPARE - UNASSIGNED'],
			204=>['Lansdowne R',	'Lansdowne Rural Telephone Company Ltd.'],
			208=>['People\'s',	'People\'s Telephone Company of Forest Ltd.'],
			212=>['Home',		'Home Telephone Company, Inc.'],
			216=>['Commnet',	'Commnet Cellular'],
			220=>['Commnet',	'Commnet Cellular'],
			224=>['Commnet',	'Commnet Cellular'],
			228=>['ISI',		'ISI Telecommunications'],
			232=>['Pulaski-White',	'Pulaski-White Telephone Company'],
			236=>['Oregon F M',	'Oregon Farmers Mutual Telephone Company'],
			240=>['Mulberry Coop',	'Mulberry Cooperative Telephone Company, Inc.'],
			244=>['ITC',		'International Telecomm. Corporation'],
			248=>['Mountaineer',	'Mountaineer Long Distance, Inc.'],
			252=>['Champaign',	'Champaign Telephone Company'],
		},
		12=>{
			  0=>['CTI',		'Communication Telesystems, Int\'l'],
			  4=>['CTI',		'Communication Telesystems, Int\'l'],
			  8=>['Fresh Start',	'Fresh Start Communications'],
			 12=>['Kennebec',	'Kennebec Telephone Company'],
			 16=>['Feist',		'Feist Long Distance'],
			 20=>['Cherry',		'Cherry Communications'],
			 24=>['Cherry',		'Cherry Communications'],
			 28=>['S Plains',	'South Plains Telephone Cooperative, Inc.'],
			 32=>['Hollis',		'Hollis Telephone Company'],
			 36=>['IXC',		'IXC Long Distance, Inc.'],
			 40=>['IXC',		'IXC Long Distance, Inc.'],
			 44=>['Premiere',	'Premiere Comm.'],
			 48=>['LDN',		'Long Distance Network'],
			 52=>['Council Grove',	'Council Grove Telephone Company'],
			 56=>['Sledge',		'Sledge Telephone Company'],
			 60=>['CTI',		'Com Tech International'],
			 64=>['CapRock',	'CapRock Communications'],
			 68=>['Telenational',	'Telenational Communications'],
			 72=>['Baraga',		'Baraga Telephone Company'],
			 76=>['American Per.',	'American Personal Communications'],
			 80=>['Merrimack Cty',	'Merrimack County Telephone'],
			 84=>['Contoocook Vly',	'Contoocook Valley Telephone'],
			 88=>['Contoocook Vly',	'Contoocook Valley Telephone'],
			 92=>['Phillips Cty',	'Phillips County Telephone Company'],
			 96=>['T-One',		'T-One Corporation'],
			100=>['Fulton',		'Fulton Telephone Company, Inc.'],
			104=>['Brazos',		'Brazos Telephone Cooperative, Inc.'],
			108=>['McNabb',		'McNabb Telephone Company'],
			112=>['Cam-Net',	'Cam-Net Communications, Inc.'],
			116=>['El Paso',	'El Paso Telephone Company'],
			120=>['Primus',		'Primus Telecommunications'],
			124=>['Riviera',	'Riviera Telephone Company Inc.'],
			128=>['Northern',	'Northern Telephone Cooperative, Inc.'],
			132=>['Chickamauga',	'Chickamauga Telephone Company'],
			136=>['Mid-Maine',	'Mid-Maine Telecom'],
			140=>['Mid-Maine',	'Mid-Maine Telecom'],
			144=>['CellularONE',	'CellularONE'],
			148=>['WesTel',		'WesTel Telecommunications, Ltd.'],
			152=>['TSI',		'Telecommunication Services, Inc.'],
			156=>['Vermont',	'Vermont Telephone Company, Inc.'],
			160=>['Swayzee',	'Swayzee Telephone Company, Inc.'],
			164=>['Sweetser',	'Sweetser Telephone Company, Inc.'],
			168=>['Bixby',		'Bixby Telephone Company'],
			172=>['Champlain Vly',	'Champlain Valley Telecom'],
			176=>['Champlain Vly',	'Champlain Valley Telecom'],
			180=>['Cellcom',	'Mobile Telephone Switching Office/Cellcom'],
			184=>['ESS',		'ESS Ventures'],
			188=>['NE Louisana',	'Northeast Louisana Telephone Company'],
			192=>['AmeriVision',	'AmeriVision Communications Inc.'],
			196=>['AmeriVision',	'AmeriVision Communications, Inc.'],
			200=>['Blanca',		'Blanca Telephone Company'],
			204=>['Lavaca',		'Lavaca Telephone Company, Inc.'],
			208=>['Five Star',	'Five Star Telecom'],
			212=>['Orwell',		'Orwell Telephone Company'],
			216=>['Bascom M',	'Bascom Mutual Telephone Company'],
			220=>['Delcambre',	'Delcambre Telephone Company'],
			224=>['Bloomingdale',	'Bloomingdale Telephone Company, Inc.'],
			228=>['Cellular One',	'Cellular One (Kansas City)'],
			232=>['Scherers',	'Scherers Communication Group, Inc.'],
			236=>['Scherers',	'Scherers Communication Group, Inc.'],
			240=>['Cowiche',	'Cowiche Telephone'],
			244=>['RT',		'RT Communications'],
			248=>['RT',		'RT Communications'],
			252=>['Seneca',		'Seneca Telephone Company'],
		},
		13=>{
			  0=>['Goodman',	'Goodman Telephone Company'],
			  4=>['Frontenac',	'Frontenac Telephone Company'],
			  8=>['Harold',		'Harold Telephone Company'],
			 12=>['STAR',		'STAR Telecommunications, Inc.'],
			 16=>['Access',		'Access Long Distance'],
			 20=>['C-P Hill',	'Compti-Pleasant Hill Telephone Company'],
			 24=>['C Arkansas',	'Central Arkansas Telephone Cooperative, Inc.'],
			 28=>['N. Renfrew',	'N. Renfrew Telephone Company, Ltd.'],
			 32=>['Westport',	'Westport Telephone Company, Ltd.'],
			 36=>['Harold',		'Harold Telephone Company'],
			 40=>['National',	'National Communications Association'],
			 44=>['Nelson Ball',	'Nelson Ball Ground Telephone Company'],
			 48=>['Brindlee Mtn.',	'Brindlee Mtn. Telephone Company'],
			 52=>['Hill Cty',	'Hill Country Telephone Cooperative, Inc.'],
			 56=>['Hill Cty',	'Hill Country Telephone Cooperative, Inc.'],
			 60=>['Hill Cty',	'Hill Country Telephone Cooperative, Inc.'],
			 64=>['STS',		'Specialized Telecommunication Services'],
			 68=>['Upton',		'Upton, Inc. (La Cie de Telephone)'],
			 72=>['Valcourt',	'Valcourt (Co-op de Telephone de)'],
			 76=>['Milot',		'Telephone Milot, Inc.'],
			 80=>['Athena',		'Athena International L.L.C.'],
			 84=>['Athena',		'Athena International L.L.C.'],
			 88=>['Athena',		'Athena International L.L.C.'],
			 92=>['Tecnet',		'Tecnet, Inc.'],
			 96=>['Tecnet',		'Tecnet, Inc.'],
			100=>['Tri-County',	'Tri-County Telephone Membership Corporation'],
			104=>['Crossville',	'Crossville Communications, Inc.'],
			108=>['Dell Coop',	'Dell Telephone Cooperative'],
			112=>['Automated',	'Automated Communications, Inc.'],
			116=>['Noonan F',	'Noonan Farmers Telephone Company'],
			120=>['Telco',		'Telco Communications Group'],
			124=>['New Florence',	'New Florence Telephone Company'],
			128=>['N Central',	'North Central Telephone Cooperative'],
			132=>['Farber',		'Farber Telephone Company'],
			136=>['Northwest',	'Northwest Communications Cooperative'],
			140=>['ITS',		'ITS'],
			144=>['Intercel',	'Intercel'],
			148=>['Valliant',	'Valliant Telephone Company'],
			152=>['LDM',		'LDM, Inc.'],
			156=>['Star',		'Star Telephone Company'],
			160=>['Georgia',	'Georgia Telephone Corporation'],
			164=>['Carib',		'Carib Comm, L.P.'],
			168=>['Cherry',		'Cherry Communications'],
			172=>['Cherry',		'Cherry Communications'],
			176=>['Valu-Line',	'Valu-Line'],
			180=>['C Utah',		'Central Utah Telephone, Inc.'],
			184=>['TelAmerica',	'TelAmerica Long Distance'],
			188=>['Thrifty',	'Thrifty Call, Inc. L.D.'],
			192=>['Thrifty',	'Thrifty Call, Inc. L.D.'],
			196=>['IT Group',	'The I.T. Group'],
			200=>['Mid-Plains R',	'Mid-Plains Rural Telephone Cooperative, Inc.'],
			204=>['Direct Net',	'Direct Net Telecommunications'],
			208=>['Dallas NextFone','Dallas NextFone Systems, LC'],
			212=>['State LD',	'State Long Distance Telephone Company'],
			216=>['Dobson',		'Dobson Telephone Company'],
			220=>['McLoud',		'McLoud Telephone Company'],
			224=>['Tri-County',	'Tri-County Telephone Company, Inc.'],
			228=>['Tri-County',	'Tri-County Telephone Company, Inc.'],
			232=>['Rock Cty',	'Rock County Telephone Company'],
			236=>['E Nebraska',	'Eastern Nebraska Telephone Company'],
			240=>['Chariton Vly',	'Chariton Valley Telephone Corporation'],
			244=>['Pennsylvania',	'Pennsylvania Telephone Company'],
			248=>['Pineland',	'Pineland Telephone Cooperative, Inc.'],
			252=>['Jones Intercable','Jones Intercable'],
		},
		14=>{
#			  0=>['',		'NOT USED'],
			  4=>['Voyager',	'Voyager Networks, Inc'],
			  8=>['Voyager',	'Voyager Networks, Inc'],
			 12=>['Voyager',	'Voyager Networks, Inc'],
			 16=>['Apollo',		'Apollo Telecom'],
			 20=>['Apollo',		'Apollo Telecom'],
			 24=>['Sprint',		'Sprint Cellular'],
			 28=>['Merchants & F',	'Merchants & Farmers Telephone Company'],
			 32=>['Choctaw',	'Choctaw Telephone Company'],
			 36=>['W Wireless',	'Western Wireless'],
			 40=>['W Wireless',	'Western Wireless'],
			 44=>['W Wireless',	'Western Wireless'],
			 48=>['Kallback',	'Kallback'],
			 52=>['Kallback',	'Kallback'],
			 56=>['Kallback',	'Kallback'],
			 60=>['Tel-Save',	'Tel-Save, Inc.'],
			 64=>['Tel-Save',	'Tel-Save, Inc.'],
			 68=>['Tel-Save',	'Tel-Save, Inc.'],
			 72=>['UTA',		'United Telephone Association, Inc.'],
			 76=>['NVTS',		'NVTS Ameritel'],
			 80=>['CCI',		'C.C.I.'],
			 84=>['CCI',		'C.C.I.'],
			 88=>['CCI',		'C.C.I.'],
			 92=>['York',		'York Telephone Company'],
			 96=>['Midstate',	'Midstate Telephone Company'],
			100=>['JBN',		'JBN Telephone Company, Inc.'],
			104=>['Qwest',		'Qwest Communications'],
			108=>['Qwest',		'Qwest Communications'],
			112=>['Qwest',		'Qwest Communications'],
			116=>['Kadoka',		'Kadoka Telephone Company'],
			120=>['Dominican',	'Dominican Communications'],
			124=>['SITA',		'SITA'],
			128=>['Beresford Mun',	'Beresford Municipal Telephone Company'],
			132=>['Table Top',	'Table Top Telephone Company, Inc.'],
			136=>['Table Top',	'Table Top Telephone Company, Inc.'],
			140=>['Lipan',		'Lipan Telephone Company, Inc.'],
			144=>['Centennial',	'Centennial Cellular Corporation'],
			148=>['Coast to Coast',	'Coast to Coast'],
			152=>['Cooperative',	'Cooperative Communications Inc.'],
			156=>['Egyptian',	'Egyptian Telephone Cooperative'],
			160=>['MICL',		'MICL'],
			164=>['Searsboro',	'Searsboro Telephone Company'],
			168=>['US WATS',	'US WATS'],
			172=>['Sprint',		'Sprint Cellular'],
			176=>['Sprint',		'Sprint Cellular'],
			180=>['Sprint',		'Sprint Cellular'],
			184=>['Ellington',	'Ellington Telephone Company'],
			188=>['Ellington',	'Ellington Telephone Company'],
			192=>['Telnex',		'Telnex, Inc.'],
			196=>['Telnex',		'Telnex, Inc.'],
			200=>['Atlantic',	'Atlantic Cellular Company, L.P.'],
			204=>['TCAST',		'TCAST Communications, Inc.'],
			208=>['TCAST',		'TCAST Communications, Inc.'],
			212=>['TCAST',		'TCAST Communications, Inc.'],
			216=>['Liberty',	'Liberty Cellular, Inc.'],
			220=>['Geetingsville',	'Geetingsville Telepone Company, Inc.'],
			224=>['Laurel H',	'Laurel Highland Telephone Company'],
			228=>['Nokia',		'Nokia Telecom'],
			232=>['FCI',		'FCI Int\'l'],
			236=>['Huron',		'Huron Telecomm.Cooperative'],
			240=>['Hay',		'Hay Communications Cooperative'],
			244=>['KCTC',		'KCTC'],
			248=>['Mosinee',	'Mosinee Telephone Company'],
			252=>['Montrose',	'Montrose Mutual Telephone Company'],
		},
		15=>{
#			  0=>['',		'NOT USED'],
			  4=>['Caribbean',	'Caribbean Telephone and Telegraph, Inc.'],
			  8=>['Caribbean',	'Caribbean Telephone and Telegraph, Inc.'],
			 12=>['iXnet',		'iXnet'],
			 16=>['iXnet',		'iXnet'],
			 20=>['iXnet',		'iXnet'],
			 24=>['iXnet',		'iXnet'],
			 28=>['iXnet',		'iXnet'],
			 32=>['Motorola',	'Motorola'],
			 36=>['Motorola',	'Motorola'],
			 40=>['General',	'General Telecom'],
			 44=>['Western',	'Western Wireless'],
			 44=>['Western',	'Western Wireless'],
			 48=>['Western',	'Western Wireless'],
			 52=>['Western',	'Western Wireless'],
			 56=>['Western',	'Western Wireless'],
			 60=>['Western',	'Western Wireless'],
			 64=>['Yorkville',	'Yorkville Telephone Cooperative'],
			 68=>['Tuckersmith',	'Tuckersmith Communications Cooperative, Ltd.'],
			 72=>['Telegroup',	'Telegroup'],
			 76=>['NAC',		'North American Comm (NAC)'],
			 80=>['Wilkinson Cty',	'Wilkinson County Telephone Company'],
			 84=>['WorldPass',	'WorldPass'],
			 88=>['BayStar',	'BayStar Communications'],
			 92=>['Filer Mutual',	'Filer Mutual Telephone Company'],
			 96=>['Elec Lightwave',	'Electric Lightwave'],
			100=>['Network Plus',	'Network Plus'],
			104=>['Network Plus',	'Network Plus'],
			108=>['KINNET',		'KINNET Fiber Optic Communications'],
			112=>['Peoples',	'Peoples Telephone Company'],
			116=>['Fairwater-B-A',	'Fairwater-Brandon-Alto Telephone Company'],
			120=>['Lackawaxen',	'Lackawaxen Telephone Company'],
			124=>['HSS',		'HSS Vending Distributors'],
			128=>['HSS',		'HSS Vending Distributors'],
			132=>['Eastex',		'Eastex Telephone Cooperative, Inc.'],
			136=>['Eastex',		'Eastex Telephone Cooperative, Inc.'],
			140=>['Eastex',		'Eastex Telephone Cooperative, Inc.'],
			144=>['Eastex',		'Eastex Telephone Cooperative, Inc.'],
			148=>['Eastex',		'Eastex Telephone Cooperative, Inc.'],
			152=>['Eastex',		'Eastex Telephone Cooperative, Inc.'],
			156=>['Ringgold',	'Ringgold Telephone Company, Inc.'],
			160=>['Farmers',	'Farmers Telephone Cooperative, Inc'],
			164=>['US South',	'US South Communications, Inc.'],
			168=>['MediaOne',	'MediaOne, Inc.'],
			172=>['POPP',		'POPP Telecom'],
			176=>['POPP',		'POPP Telecom'],
			180=>['WTS',		'World Telecommunications Services'],
			184=>['Teleport',	'Teleport Communications Group'],
			188=>['Livingston',	'Livingston Telephone Company'],
			192=>['Peoples',	'Peoples Telephone Cooperative, Inc.'],
			196=>['New Hope',	'New Hope Telephone Cooperative'],
			200=>['DataComm',	'DataComm. International Company, Ltd.'],
			204=>['Vista',		'Vista International'],
			208=>['Startec\'s',	'Startec\'s'],
			212=>['Cable Plus',	'Cable Plus Company LP'],
			216=>['Cable Plus',	'Cable Plus Company LP'],
			220=>['Venus',		'Venus Telephone Corp.'],
			224=>['S. F. C.',	'S. F. C. Enterprises'],
			228=>['Tech Ctrl',	'Technology Control, Inc.'],
			232=>['Tech Ctrl',	'Technology Control, Inc.'],
			236=>['Mark Twain R',	'Mark Twain Rural Telephone Company'],
			240=>['Mark Twain R',	'Mark Twain Rural Telephone Company'],
			244=>['Mark Twain R',	'Mark Twain Rural Telephone Company'],
			248=>['Mark Twain R',	'Mark Twain Rural Telephone Company'],
			252=>['',		'RAM Technologies, Inc.'],
		},
		16=>{
#			  0=>['',		'NOT USED'],
			  4=>['Ironton',	'Ironton Telephone Company'],
			  8=>['TCT West',	'TCT West, Inc.'],
			 12=>['La Harpe',	'La Harpe Telephone Company, Inc.'],
			 16=>['United',		'United Telephone Association, Inc.'],
			 20=>['Dubois',		'Dubois Telephone Exchange, Inc'],
			 24=>['Northland',	'Northland Telephone Company'],
			 28=>['Northland',	'Northland Telephone Company'],
			 32=>['Northland',	'Northland Telephone Company'],
			 36=>['Northland',	'Northland Telephone Company'],
			 40=>['Time Warner',	'Time Warner Communications'],
			 44=>['Time Warner',	'Time Warner Communications'],
			 48=>['Time Warner',	'Time Warner Communications'],
			 52=>['AIC',		'AIC Asia International Services Corporation'],
			 56=>['Zenex',		'Zenex Long Distance'],
			 60=>['El Paso Cty',	'El Paso County Telephone Company'],
			 64=>['W Wisconsin',	'West Wisconsin Telecom Cooperative, Inc.'],
			 68=>['W Wisconsin',	'West Wisconsin Telecom Cooperative, Inc.'],
			 72=>['American Per.',	'American Personal Communications'],
			 76=>['Telescan',	'Telescan, Inc.'],
			 80=>['St. Cloud',	'Cellular Mobile Systems of St. Cloud'],
			 84=>['Consolidated',	'Consolidated Telco, Inc.'],
			 88=>['Consolidated',	'Consolidated Telco, Inc.'],
			 92=>['Consolidated',	'Consolidated Telco, Inc.'],
			 96=>['Consolidated',	'Consolidated Telco, Inc.'],
			100=>['Consolidated',	'Consolidated Telco, Inc.'],
			104=>['Consolidated',	'Consolidated Telco, Inc.'],
			108=>['IDS',		'IDS Long Distance, Inc.'],
			112=>['W New Mexico',	'Western New Mexico Telephone Company, Inc.'],
			116=>['Minford',	'Minford Telephone Company'],
			120=>['STAR',		'STAR Telecommunications, Inc.'],
			124=>['American',	'American Communications Services, Inc.'],
			128=>['American',	'American Communications Services, Inc.'],
			132=>['NAGI',		'North American Gateway Inc.'],
			136=>['Bloomingdale',	'Bloomingdale Telephone Company'],
			140=>['NevTEL',		'NevTEL'],
		},
		17=>{
			state=>['AL',		'Alabama'],
			  4=>['Teleport',	'Teleport Communications Group'],
			  8=>['American',	'American Communications Services, Inc.'],
			 12=>['DiGiPH FCS',	'DiGiPH FCS, Inc.'],
			 16=>['KMC',		'KMC Telecom Inc.'],
			 20=>['Moundville',	'Moundville Telephone Company, Inc.'],
			 24=>['BellSouth',	'BellSouth Public Communications, Inc.'],
			 28=>['ROPIR',		'ROPIR Communications, Inc.'],
			 32=>['PC Mgmt',	'PC Management'],
			 36=>['PC Mgmt',	'PC Management'],
			 40=>['UniversalCom',	'UniversalCom,Inc.'],
			 44=>['Ragland',	'Ragland Telephone Co., Inc.'],
		},
		18=>{
			state=>['AK',		'Alaska'],
			  4=>['TelAlaska',	'TelAlaska, Inc.'],
			  8=>['TelAlaska',	'TelAlaska, Inc.'],
			 12=>['TelAlaska',	'TelAlaska, Inc.'],
			 16=>['TelAlaska',	'TelAlaska, Inc.'],
			 20=>['TelAlaska',	'TelAlaska, Inc.'],
			 24=>['Cordova',	'Cordova Telephone Cooperative'],
			 32=>['Alaska Net Sys',	'Alaska Network Systems, Inc.'],
			 36=>['Bristol Bay',	'Bristol Bay Telephone Cooperative, Inc.'],
			 40=>['Bristol Bay',	'Bristol Bay Telephone Cooperative, Inc.'],
			 44=>['Bristol Bay',	'Bristol Bay Telephone Cooperative, Inc.'],
			 48=>['OTZ',		'OTZ Telephone Cooperative, Inc.'],
			 52=>['OTZ',		'OTZ Telephone Cooperative, Inc.'],
			 56=>['OTZ',		'OTZ Telephone Cooperative, Inc.'],
			 60=>['Nashagak',	'Nashagak Telephone Cooperative, Inc.'],
			 64=>['KPU',		'KPU Telecommunications'],
			 68=>['Alaska DigiTel',	'Alaska DigiTel'],
		},
		19=>{
			state=>['AZ',		'Arizona'],
			  4=>['KLP',		'KLP, Inc. dba Call America.'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			 16=>['FTI',		'FTI Communications'],
			 20=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			 24=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			 28=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			 32=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			 36=>['Mountain',	'Mountain Telecommunications'],
			 40=>['Metro One',	'Metro One Telecommunications'],
			 44=>['Dakota',		'Dakota Carrier Services'],
			 48=>['Dakota',		'Dakota Carrier Services'],
			 52=>['Dakota',		'Dakota Carrier Services'],
			 56=>['Dakota',		'Dakota Carrier Services'],
			 60=>['OPTel',		'OPTel, Inc.'],
			 64=>['Authentix',	'Authentix Network, Inc.'],
			 68=>['Authentix',	'Authentix Network, Inc.'],
			 72=>['San Carlos',	'San Carlos Apache Telecommunications Utility'],
			 76=>['Alphanet',	'Alphanet Telecom Inc.'],
		},
		20=>{
			state=>['AR',		'Arkansas'],
			  4=>['Walnut Hill',	'Walnut Hill Telephone Company, Inc.'],
			  8=>['Walnut Hill',	'Walnut Hill Telephone Company, Inc.'],
			 12=>['Yell Cty',	'Yell County Telephone Company'],
			 16=>['Hyperion',	'Hyperion Telecommunications, Inc.'],
			 20=>['S Arkansas',	'South Arkansas Telephone Company'],
			 24=>['Gabriel',	'Gabriel Communications, Inc. - Arkansas Site'],
		},
		21=>{
			state=>['CA',		'California'],
			  4=>['Harris Digital',	'Harris Digital Telephone Systems'],
			  8=>['Universal',	'Universal Communications Network, Inc.'],
			 12=>['Genesis',	'Genesis Communications International, Inc.'],
			 16=>['Primus',		'Primus Telecommunications, Inc.'],
			 20=>['CTI',		'Communications Telesystems International dba'],
			 24=>['Teleport',	'Teleport Communications Group'],
			 28=>['Teleport',	'Teleport Communications Group'],
			 32=>['Phonetime',	'Phonetime, Inc.'],
			 36=>['New Global',	'New Global Telecom'],
			 40=>['Intellicom',	'Intellicom, Inc.'],
			 44=>['Stanford Univ.',	'Stanford University Communications Services'],
			 48=>['SpectraNet',	'SpectraNet Anaheim International'],
			 52=>['Cyberlight',	'Cyberlight International, Inc.'],
			 56=>['Pacific Bell',	'Pacific Bell Communications'],
			 60=>['Five Star',	'Five Star Telecom'],
			 64=>['World Touch',	'World Touch Communications'],
			 68=>['Pac Gateway',	'Pacific Gateway Exchange'],
			 72=>['Paging Net',	'Paging Network Inc.,(AKA PageNet)'],
			 76=>['AmeriCom',	'AmeriCom Communications, LLC'],
			 80=>['Metro One',	'Metro One Telecommunications'],
			 84=>['MGC',		'MGC Communications, Inc.'],
			 88=>['World Exchange',	'World Exchange'],
			 92=>['ILD',		'ILD Teleservices, Inc.'],
			 96=>['Avirnex',	'Avirnex Communications Group'],
			100=>['Pacific',	'Pacific Telecom'],
			104=>['Econophone',	'Econophone, Inc. - California site'],
			108=>['GNP',		'Global Network Providers - California site'],
			112=>['Opentel',	'Opentel Communications, Inc.'],
			116=>['Global One',	'Global One - California site'],
			120=>['Pac-West',	'Pac-West Telecomm, Inc.'],
			124=>['Pac-West',	'Pac-West Telecomm, Inc.'],
			128=>['Teltrust',	'Teltrust Communications Services, Inc.'],
			132=>['Telegroup',	'Telegroup, Inc. - California site'],
			136=>['ACS Systems',	'ACS Systems, Inc. - California site'],
			140=>['Megawats',	'Megawats'],
			144=>['LDIP',		'LD International Provisioning - California'],
			148=>['FDN',		'FDN, Inc. - California site'],
			152=>['New Millennium',	'New Millennium Comm. Corp.- California'],
			156=>['MGC',		'MGC Communications, Inc. - California site'],
			160=>['MGC',		'MGC Communications, Inc. - California site'],
			164=>['C Florida',	'Tele. Co. of Central Florida, Inc. - California'],
			168=>['Allegiance',	'Allegiance Telecom, Inc. - California site'],
			172=>['Justice',	'Justice Technology Corp. - California'],
#			176=>['SPARE',		'SPARE - UNASSIGNED'],
			180=>['FaciliCom',	'FaciliCom International - CA site'],
			184=>['AirTouch',	'AirTouch Satellite Services-CA site'],
			188=>['Focal',		'Focal Communications Corp.'],
			192=>['New Global',	'New Global Telecom'],
			196=>['PanAmSat',	'PanAmSat Corporation - CA site'],
			200=>['Worldport',	'Worldport Communications,Inc.-CA site'],
			204=>['AirTouch',	'AirTouch Satellite Services-CA site'],
			208=>['One. Tel',	'One. Tel, Inc.'],
			212=>['OzEmail',	'OzEmail Interline Pty Ltd. - CA site'],
			216=>['Japan Telecom',	'Japan Telecom America,Inc. - CA site'],
			220=>['Focal',		'Focal Communications Corp. - CA site'],
			224=>['DJB',		'DJB Entterprises Inc. - CA site'],
			228=>['DJB',		'DJB Enterprises Inc. - CA site'],
			232=>['INET',		'INET Interactive Network System - CA site'],
			236=>['New Zealand',	'Telecom New Zealand USA Limited'],
			240=>['Level 3',	'Level 3 - CA site'],
			244=>['Voice and Data',	'Voice and Data Communications - CA site'],
			248=>['LEC Unwired',	'LEC Unwired, LLC'],
			252=>['Teleselect',	'Teleselect Group, Inc. - CA site'],
		},
		22=>{
			state=>['CO',		'Colorado'],
			  4=>['Plains',		'Plains Cooperative Telephone Association, Inc.'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['Intellicom',	'Intellicom, Inc.'],
			 16=>['American',	'American Communications Services, Inc.'],
			 20=>['Bijou',		'The Bijou Telephone Co-op Association'],
			 24=>['NA DigiCom',	'North American DigiCom Corporation'],
			 28=>['NA DigiCom',	'North American DigiCom Corporation'],
			 32=>['Nucla-N',	'Nucla-Naturita Telephone Company'],
			 36=>['Nucla-N',	'Nucla-Naturita Telephone Company'],
			 40=>['ConferTech',	'ConferTech International'],
			 44=>['ConferTech',	'ConferTech International'],
			 48=>['Metro One',	'Metro One Telecommunications'],
			 52=>['Sunflower',	'Sunflower Tele. Co., Inc. - Colorado'],
			 56=>['Columbine',	'Columbine Telecom Company'],
			 60=>['Convergent',	'Convergent Communications Services,Inc.'],
			 64=>['ACS',		'ACS Systems, Inc. - Colorado site'],
			 68=>['Optimum',	'Optimum Network Service, LLC'],
			 72=>['Great West',	'Great West Services, Ltd.'],
			 76=>['Farmers',	'Farmers Telephone Co., Inc.'],
			 80=>['Level 3',	'LeveL 3 - CO site'],
			 84=>['Voice and Data',	'Voice and Data Communications - CO site'],
			 88=>['CellularOne',	'CellularOne of Northeast Colorado'],
			 92=>['Level 3',	'Level 3 Communications - CO site'],
			 96=>['GlobalLink',	'USA Global Link - CO site'],
			100=>['OpTel',		'OpTel, Inc. - CO site'],
		},
		23=>{
			state=>['CT',		'Connecticut'],
			  4=>['Teleport',	'Teleport Communications Group'],
		},
		24=>{
			state=>['DE',		'Delaware'],
		},
		25=>{
			state=>['DC',		'District of Columbia'],
			  4=>['Teleport',	'Teleport Communications Group'],
		},
		26=>{
			state=>['FL',		'Florida'],
			  4=>['Universal',	'Universal Communications Network, Inc.'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['',		'Intellicom, Inc.'],
			 16=>['',		'Phonetime, Inc.'],
			 20=>['American',	'American Communications Services, Inc.'],
			 24=>['',		'International Digital Telecommunications Systems'],
			 28=>['',		'TresCom International'],
			 32=>['',		'BellSouth Long Distance, Inc.'],
			 36=>['Orlando',	'Orlando Telephone Company, Inc.'],
			 40=>['',		'Hyperion Telecommunications, Inc.'],
			 44=>['Time Warner',	'Time Warner Communications'],
			 48=>['Teleport',	'Teleport Communications Group'],
			 52=>['Teleport',	'Teleport Communications Group'],
			 56=>['',		'Long Distance International Inc.'],
			 60=>['L. C.',		'L. C. Communications'],
			 64=>['',		'UniversalCom, Inc.'],
			 68=>['',		'Metro One Telecommunications'],
			 72=>['Pick',		'Pick Communications Corporation'],
			 76=>['',		'US LEC of North Carolina-Florida'],
			 80=>['',		'Star Telecom(Miami, Florida site'],
			 84=>['',		'Econophone, Inc.-Florida site'],
			 88=>['New Millennium',	'New Millennium Communications Corp.'],
			 92=>['',		'Telegroup, Inc.-Florida site'],
			 96=>['',		'KMC Telecom Inc. - Florida site'],
			100=>['',		'Americatel Corporation'],
			104=>['',		'City of Lakewood'],
			108=>['',		'FDN Inc.-Florida site'],
			112=>['MGC',		'MGC Communications,Inc.-Florida site'],
			116=>['',		'Tele.Co. of Central Florida,Inc.-Florida'],
			120=>['',		'Voiceware Systems'],
			124=>['',		'First Data Corp.(FDC) Florida site'],
			128=>['',		'PanAmSat Corporation - Florida site'],
			132=>['Worldport',	'Worldport Communications,Inc. - FL site'],
			136=>['',		'Call Sciences - Florida site'],
			140=>['',		'American International Telephone'],
			144=>['',		'INET Interactive Network Sys.-FL site'],
			148=>['',		'Thrifty Call Inc. - Florida site'],
			152=>['Voice and Data',	'Voice and Data Communications - FL site'],
			156=>['',		'KMC Telecom II,Inc. - FL site'],
			160=>['',		'Business Technology Services, Inc.'],
			164=>['PaeTec',		'PaeTec Communications,Inc. -FL site'],
			168=>['',		'North American Telecomm. Corp'],
			172=>['',		'USTel dba Arcada Commun.-FLsite'],
			176=>['Premiere',	'Premiere Communications - FL site'],
			180=>['',		'US LEC of Florida Inc. - FL site'],
			184=>['',		'Eastland of Orlando Telephone Corp.'],
			188=>['',		'OpTel, Inc. - Florita site'],
			192=>['',		'Florida Digital Network, Inc.'],
			196=>['',		'World Access Commun. Corp.'],
			200=>['',		'Startec Global Commun. Corp.'],
		},
		27=>{
			state=>['GA',		'Georgia'],
			  4=>['Darien',		'Darien Telephone Company, Inc.'],
			  8=>['Glenwood',	'Glenwood Telephone Company'],
			 12=>['',		'Intellicom, Inc.'],
			 16=>['',		'Phonetime, Inc.'],
			 20=>['American',	'American Communications Services, Inc.'],
			 24=>['',		'BellSouth Long Distance, Inc.'],
			 28=>['Citizens',	'Citizens Telephone Company, Inc.'],
			 32=>['',		'KMC Telecom Inc.'],
			 36=>['Teleport',	'Teleport Communications Group'],
			 40=>['Teleport',	'Teleport Communications Group'],
			 44=>['Plant',		'Plant Telephone Company'],
			 48=>['Plant',		'Plant Telephone Company'],
			 52=>['Plant',		'Plant Telephone Company'],
			 56=>['Coastal',	'Coastal Telephone Company'],
			 60=>['',		'Emory University'],
			 64=>['MGC',		'MGC Communications, Inc.'],
			 68=>['',		'ILD Teleservices, Inc.'],
			 72=>['Teltrust',	'Teltrust Communications Services, Inc.'],
			 76=>['',		'Savannah Independent PCS, Inc.'],
			 80=>['',		'Georgia Independent PCS, Inc.'],
			 84=>['',		'US LEC of North Carolina - Georgia'],
			 88=>['',		'SITA - Georgia location'],
			 92=>['',		'Allegiance Telecom,Inc.-Georgia site'],
			 96=>['',		'Econophone, Inc.-Georgia site'],
			100=>['',		'Five Star Telecom - Georgia site'],
			104=>['Enterprise',	'Enterprise Communications'],
			108=>['',		'FaciliCom International - GA site'],
			112=>['',		'Call Sciences - GA site'],
			116=>['',		'N + 1, Inc. - GA site'],
			120=>['Level 3',	'Level 3 Communications-GA site'],
			124=>['',		'Pre-Paid Cellular Services'],
			128=>['',		'USTel dba Arcada Commun.-GA site'],
			132=>['',		'WorldxChange - GA site'],
			136=>['',		'Network Switching Services,Inc.-GA site'],
			140=>['',		'Waverly Hall Telephone Co.,Inc.'],
		},
		28=>{
			state=>['HI',		'Hawaii'],
			  4=>['Time Warner',	'Time Warner Communications'],
			  8=>['',		'TelHawaii, Inc.'],
			 12=>['Sandwich Isles',	'Sandwich Isles Communications,Inc.'],
			 16=>['Sandwich Isles',	'Sandwich Isles Communications,Inc.'],
			 20=>['',		'USTel dba Arcada Commun. - HI site'],
		},
		29=>{
			state=>['ID',		'Idaho'],
			  4=>['American',	'American Communications Services, Inc.'],
			  8=>['',		'Fremont Telcom'],
			 12=>['',		'USTel dba Arcada Commun. - ID site'],
		},
		30=>{
			state=>['IL',		'Illinois'],
			  4=>['Pantel',		'Pantel Communications'],
			  8=>['IXC',		'IXC Communications, Inc.'],
			 12=>['Teleport',	'Teleport Communications Group'],
			 16=>['Teleport',	'Teleport Communications Group'],
			 20=>['',		'Focal communications Corporation'],
			 24=>['',		'Intellicom, Inc.'],
			 28=>['',		'Phonetime, Inc.'],
			 32=>['',		'Odin Telephone Exchange, Inc.'],
			 36=>['FTI',		'FTI Communications'],
			 40=>['',		'Metro One Telecommunications'],
			 44=>['',		'World Exchange'],
			 48=>['',		'Connect America,Inc. - IL site'],
			 52=>['',		'Econophone, Inc. - IL site'],
			 56=>['',		'21ST Century Cable - IL site'],
			 60=>['',		'Telegroup, Inc. - IL site'],
			 64=>['',		'Long Distance Int\'l Provisioning - IL'],
			 68=>['Net',		'Net Communications,Inc. - IL site'],
			 72=>['',		'Tele. Co. of Central Florida,Inc. - IL'],
			 76=>['',		'Allegiance Telecom,Inc. - IL site'],
			 80=>['',		'Motorola Corporation'],
			 84=>['',		'FaciliCom International -IL site'],
			 88=>['',		'Southern Illinois RSA Partnership'],
			 92=>['',		'Thrifty Call Inc. - IL site'],
			 96=>['',		'LEVEL 3 - IL site'],
			100=>['Gallatin River',	'Gallatin River Communications-IL site'],
			104=>['Gallatin River',	'Gallatin River Communications-IL site'],
			108=>['',		'USTel dba Arcada Commun. - IL site'],
			112=>['',		'WorldxChange - IL site'],
			116=>['',		'Megsinet, Inc.'],
			120=>['',		'OpTel, Inc. - IL site'],
		},
		31=>{
			state=>['IN',		'Indiana'],
			  4=>['Bloomingdale H',	'Bloomingdale Home Telephone Company'],
			  8=>['West Point',	'West Point Telephone Company'],
			 12=>['Teleport',	'Teleport Communications Group'],
			 16=>['Time Warner',	'Time Warner Communications'],
			 20=>['',		'US Xchange LLC(Indiana site)'],
			 24=>['',		'Pend Oreille Telephone'],
			 28=>['',		'KMC Telecom II, Inc. - IN site'],
		},
		32=>{
			state=>['IA',		'Iowa'],
			  4=>['Scranton',	'Scranton Telephone Company'],
			  8=>['Templeton',	'Templeton Telephone Company'],
			 12=>['',		'Marne & Elk Horn Telephone Comapny'],
			 16=>['',		'Farmers & Merchants Mutual Telephone'],
			 20=>['Miller',		'Miller Telephone Company'],
			 24=>['Farmers M',	'Farmers Mutual Telephone Company'],
			 28=>['',		'Consolidated Comm. Inc. - Iowa site'],
			 32=>['',		'Amica Wireless Phone Service'],
			 36=>['Dunkerton',	'Dunkerton Telephone Cooperative'],
		},
		33=>{
			state=>['KS',		'Kansas'],
			  4=>['Peoples Mutual',	'Peoples Mutual Telephone Company'],
			  8=>['S&T',		'S&T Telephone Cooperative Association'],
			 12=>['',		'KIN Network, Inc.'],
			 16=>['S&T',		'S&T Telephone Cooperative Assn.'],
			 20=>['',		'Kansas Personal Communication Services Ltd.'],
			 24=>['Home',		'Home Telephone Company, Inc.'],
			 28=>['',		'Hyperion Telecommunications, Inc.'],
			 32=>['',		'Sunflower Telephone Co.,Inc.-Kansas'],
			 36=>['Bluestem',	'Bluestem Telephone Company-Kansas'],
			 40=>['',		'Mercury Cellular & Paging - KS location'],
			 44=>['',		'Cunningham Telephone Co.,Inc.'],
			 48=>['',		'MoKan Dial, Inc. - Kansas site'],
			 52=>['',		'Birch Telecom - Kansas site'],
			 56=>['',		'Sunflower Telephone Co.,Inc. - Kansas'],
			 60=>['',		'KMC Telecom II, Inc. - Kansas site'],
			 64=>['Mutual',		'Mutual Telephone Company - KS site'],
			 68=>['Gabriel',	'Gabriel Communications,Inc. - KS site'],
		},
		34=>{
			state=>['KY',		'Kentucky'],
			  8=>['',		'Alec, Inc.'],
			 12=>['',		'Hyperion Telecomm. Inc. - Kentucky'],
			 16=>['',		'Wireless 2000 PCS - Kentucky site'],
			 20=>['',		'Wireless 2000 PCS - Kentucky site'],
			 24=>['',		'SouthEast Telephone'],
		},
		35=>{
			state=>['LA',		'Louisiana'],
			  4=>['Elizabeth',	'Elizabeth Telephone Company'],
			  8=>['',		'Radiofone, Inc.'],
			 12=>['American',	'American Communications Services, Inc.'],
			 16=>['',		'MobileTel Inc.'],
			 20=>['',		'Mercury Cellular & Paging'],
			 24=>['',		'American MetroComm Corporation'],
			 28=>['',		'BellSouth Long Distance, Inc.'],
			 32=>['',		'Advanced Tel., Inc.'],
			 36=>['Meretel',	'Meretel Communications, L.P.'],
			 40=>['',		'Hyperion Telecommunications, Inc.'],
			 44=>['',		'KMC Telecom Inc.'],
			 48=>['',		'Data & Electronic Services, Inc.'],
			 52=>['',		'Shell Offshore Services, Co.'],
			 56=>['',		'Columbia Telecommunications, Inc.'],
			 60=>['',		'Louisiana Unwired LLC'],
			 64=>['',		'Louisiana Unwired LLC'],
		},
		36=>{
			state=>['ME',		'Maine'],
			  4=>['',		'Unitel Incorporated'],
			  8=>['',		'Saco River Cellular Tele. Co.(DBA S/C)'],
		},
		37=>{
			state=>['MD',		'Maryland'],
			  4=>['Teleport',	'Teleport Communications Group'],
			  8=>['American',	'American Communications Services, Inc.'],
			 12=>['',		'Metro One Telecommunications'],
			 16=>['',		'RCN Telecom Services,Inc. MD site'],
			 20=>['',		'Bell Atlantic - MD'],
			 24=>['PaeTec',		'PaeTec Communications,Inc. - MD site'],
			 28=>['',		'Comav Telco, Inc. - MD site'],
		},
		38=>{
			state=>['MA',		'Massachusetts'],
			  4=>['',		'Xcom Technologies, Inc.'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['Teleport',	'Teleport Communications Group'],
			 16=>['Residential',	'Residential Communications Netwprk, Inc.'],
			 20=>['Arch',		'Arch Communications Group, Inc.'],
			 24=>['',		'Atlantic Connections, Inc.'],
			 28=>['',		'RNK, Inc.'],
			 32=>['',		'Global NAPS'],
			 36=>['',		'XCOM Technologies'],
			 44=>['',		'Norfolk County Internet'],
			 48=>['',		'Thrifty Call Inc. - MA site'],
			 52=>['',		'Bell Atlantic Global Networks - MA site'],
			 56=>['',		'LEVEL 3 - MA site'],
			 60=>['',		'LBC Telephony - MA site'],
			 64=>['',		'Allegiance Telecom, Inc. - MA site'],
			 68=>['PaeTec',		'PaeTec Communications, Inc. - MA site'],
		},
		39=>{
			state=>['MI',		'Michigan'],
			  4=>['Ontonagon Cty',	'Ontonagon County Telephone Company'],
			  8=>['Ontonagon Cty',	'Ontonagon County Telephone Company'],
			 12=>['Ontonagon Cty',	'Midway Telephone Company'],
			 16=>['',		'Chapin telephone Company'],
			 20=>['',		'Peninsula telephone Company'],
			 24=>['Nationwide',	'Nationwide Communications, Inc.'],
			 28=>['BRE',		'BRE Communications dba Phone Michigan'],
			 32=>['',		'FoneTel'],
			 36=>['',		'NPI Wireless'],
			 40=>['',		'NPI Wireless'],
			 44=>['',		'Metro One Telecommunications'],
			 48=>['Hiawatha',	'Hiawatha Telephone Company'],
			 52=>['',		'Anishnabe Comm. Enterprise,Inc. MI'],
			 56=>['',		'Long Distance of Michigan'],
			 60=>['',		'MichTel, Inc. - MI site'],
			 64=>['',		'KMC Telecom II, Inc. - MN site'],
			 68=>['Level 3',	'Level 3 Communications - MI site'],
			 72=>['',		'USTel dba Arcada Commun. - MI site'],
			 76=>['Focal',		'Focal Communications Corporation'],
		},
		40=>{
			state=>['MN',		'Minnesota'],
			  4=>['American',	'American Communications Services, Inc.'],
			  8=>['',		'RCC Network, Inc.'],
			 12=>['OCI',		'OCI Communications of Minnesota, Inc.'],
			 16=>['Woodstock',	'Woodstock Telephone Company'],
			 20=>['',		'Metro One Telecommunications'],
			 24=>['',		'Upsala Cooperative Telephone Assoc.'],
			 32=>['',		'Teleport Comm. Group-Minnesota site'],
			 36=>['',		'Otter Tail Telcom'],
			 40=>['',		'Five Star Telecom - MN site'],
			 44=>['',		'Lismore Cooperative Telephone Co.'],
			 48=>['',		'KMC Telecom II, Inc. - MN site'],
			 52=>['',		'USTel dba Arcada Commun. MN site'],
			 56=>['',		'Digital Telecommunications, Inc.'],
		},
		41=>{
			state=>['MS',		'Mississippi'],
			  4=>['',		'Hyperion Telecomm, Inc.'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['American',	'American Communications Services Inc.'],
			 16=>['Mound Bayou',	'Mound Bayou Telephone & Communications'],
			 20=>['',		'Cellular Holding, Inc.'],
			 24=>['Georgetown',	'Georgetown Telephone Company'],
		},
		42=>{
			state=>['MO',		'Missouri'],
			  4=>['',		'Cass telephone Company'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['',		'Intellicom, Inc.'],
			 16=>['',		'Phonetime, Inc.'],
			 20=>['American',	'American Communications Services Inc.'],
			 24=>['Consolidated',	'Consolidated Communications, Inc.'],
			 28=>['',		'Metro One Telecommunications'],
			 32=>['Miller',		'Miller Telephone Company'],
			 36=>['',		'MoKan Dia, Inc. - Missouri site'],
			 40=>['',		'Digital Teleport, Inc.'],
			 44=>['Alma',		'Alma Telephone Company, Inc.'],
			 48=>['',		'Birch Telecom - Missouri site'],
			 52=>['',		'USTel dba Arcada Commun. - MO site'],
			 56=>['',		'ExOp of Missouri, Inc.'],
			 60=>['BPS',		'BPS Telephone Company'],
			 64=>['Gabriel',	'Gabriel Communications, Inc. - MO site'],
			 68=>['Gabriel',	'Gabriel Communications,Inc. - MO site'],
			 72=>['',		'Chariton Valley Long Distance Corp.'],
			 76=>['',		'Missouri RSA #5'],
		},
		43=>{
			state=>['MT',		'Montana'],
			  4=>['Range',		'Range Telephone Cooperative'],
			  8=>['',		'Montana Wireless, Inc.'],
			 12=>['Ronan',		'Ronan Telephone Company'],
			 16=>['',		'Project Telephone Co. - Montana'],
			 20=>['',		'Project Telephone Co. - Montana'],
			 24=>['Nemont',		'Nemont Telephone Cooperative, Inc.'],
			 28=>['Nemont',		'Nemont Telephone Cooperative, Inc.'],
			 32=>['Nemont',		'Nemont Telephone Cooperative, Inc.'],
			 36=>['Nemont',		'Nemont Telephone Cooperative, Inc.'],
			 40=>['Nemont',		'Nemont Telephone Cooperative, Inc.'],
			 44=>['',		'Wireless II'],
			 48=>['',		'USTel dba Arcada Commun. - MT site'],
			 52=>['InterBel',	'InterBel Telephone Cooperative, Inc.'],
		},
		44=>{
			state=>['NE',		'Nebraska'],
			  4=>['Teleport',	'Teleport Communications Group'],
			  8=>['Wauneta',	'Wauneta Telephone Company'],
			 12=>['Keystone-A',	'Keystone-Arthur Telephone Company'],
			 16=>['',		'USA Global Link'],
			 20=>['',		'Three River Telco'],
			 24=>['Plainview',	'Plainview Telephone Company, Inc.'],
			 28=>['Elsie M',	'Elsie Mutual Telephone Company'],
			 32=>['',		'Hamilton Telecommunications'],
			 36=>['Stanton',	'The Stanton Telephone Company'],
			 40=>['',		'Wauneta Telephone Co.,Inc.'],
			 44=>['',		'Hartington Telecomm. Co.,Inc.'],
			 48=>['ATC',		'ATC Communications'],
			 52=>['',		'Henderson Co-Op Telephone Co.'],
			 56=>['',		'First Data Corporation(FDC)NE site'],
			 60=>['',		'Dalton Telephone Co.,Inc.'],
			 64=>['',		'Curtis Telephone Co.,Inc.'],
			 68=>['',		'Clarks Telecommunications Co.'],
			 72=>['',		'Hershey Cooperative Telephone Co.'],
			 76=>['',		'Northeast Nebraska Telephone Co.'],
			 80=>['',		'Northeast Nebraska Telephone Co.'],
			 84=>['',		'NebCom, Inc.'],
			 88=>['',		'Tracey Corp. II d/b/a Western Total'],
			 92=>['',		'Eastern Nebraska Telephone Co.-NE site'],
			 96=>['Home',		'Home Telephone Company of Nebraska'],
			100=>['',		'Glenwood Telephone Membership Corp.'],
		},
		45=>{
			state=>['NV',		'Nevada'],
			  4=>['',		'Phoenix Fiberlink of Utah, Inc.'],
			  8=>['Moapa Valley',	'Moapa Valley Telephone Company'],
			 12=>['Moapa Valley',	'Moapa Valley Telephone Company'],
			 16=>['',		'The  Lincoln County Telephone System, Inc.'],
			 20=>['',		'NTI Telecom Incorporated'],
			 24=>['',		'ILD Teleservices, Inc.'],
			 28=>['',		'American Comm.Services,Inc. Nevada'],
			 32=>['MGC',		'MGC Communications Inc.'],
			 36=>['Net',		'Net Communications,Inc. - Nevada site'],
			 40=>['',		'USTel dba Arcada Commun. - NV site'],
			 44=>['',		'Rio Virgin Telephone Co. - NV site'],
		},
		46=>{
			state=>['NH',		'New Hampshire'],
			  4=>['',		'Vitts Corporation'],
			  8=>['',		'XCOM Technologies (NH site)'],
			 12=>['Freedom Ring',	'Freedom Ring Communications, Inc.'],
			 16=>['',		'CMG Telecommunications, Inc.'],
		},
		47=>{
			state=>['NJ',		'New Jersey'],
			  4=>['',		'Primus Telecommunications'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['',		' Intellicom, Inc.'],
			 16=>['',		'Phonetime, Inc.'],
			 20=>['',		'Graphnet, Inc.'],
			 24=>['',		'Metro One Telecommunications'],
			 28=>['Pick',		'Pick Communications Corporation'],
			 32=>['Caricall',	'Caricall Communications - NJ site'],
			 36=>['',		'GSI Telecom - NJ site'],
			 40=>['',		'Telephone Co. of Central Florida,Inc.-NJ'],
			 44=>['',		'Call Sciences - NJ site'],
			 48=>['',		'Thrifty Call Inc. - NJ site'],
			 52=>['Viatel',		'Viatel Global Communications-NJ site'],
			 56=>['Premiere',	'Premiere Communications - NJ site'],
			 60=>['L.C.',		'L.C. Communications - NJ site'],
			 64=>['Cooperative',	'Cooperative Communications, Inc- NJ site'],
			 68=>['',		'PAPOSH, LLC -NJ site'],
		},
		48=>{
			state=>['NM',		'New Mexico'],
			  4=>['Baca Valley',	'Baca Valley Telephone Company'],
			  8=>['Penasco Vly',	'Penasco Valley Telephone Cooperative, Inc.'],
			 12=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			 16=>['',		'Tularosa Basin Telephone Co.,Inc.'],
			 20=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			 24=>['Valley',		'Valley Telephone Cooperative, Inc.'],
			 28=>['Roosevelt Cty',	'Roosevelt County Rural Telephone Coop., Inc.'],
			 32=>['',		'Reams Commun.,Inc.dba Valuline Long'],
		},
		49=>{
			state=>['NY',		'New York'],
			  4=>['Universal',	'Universal Communications Network, Inc.'],
			  8=>['',		'Athena International LLC'],
			 16=>['Inter-Community','Inter-Community Telephone Company'],
			 20=>['Teleport',	'Teleport Communications Group'],
			 24=>['Teleport',	'Teleport Communications Group'],
			 28=>['Residential',	'Residential Communications Network, Inc.'],
			 32=>['',		'Telcom International Inc.'],
			 36=>['',		'Cyberlight International, Inc.'],
			 40=>['',		'Access Technologies Group'],
			 44=>['',		'Five Star Telecom'],
			 48=>['',		'TresCom International'],
			 52=>['',		'USFI, Inc.'],
			 56=>['Viatel',		'Viatel Global Communications'],
			 60=>['Time Warner',	'Time Warner Communications'],
			 64=>['',		'USA Global Link'],
			 68=>['',		'Pacific Gateway Exchange'],
			 72=>['Unicall',	'Unicall Communications, Inc.'],
			 76=>['Focal',		'Focal Communications Corporation'],
			 80=>['',		'World Exchange'],
			 84=>['',		'Hyperion Telecommunications, Inc.'],
			 88=>['',		'DirectNet Telecommunications - NY'],
			 92=>['',		'DirectNet Telecommunications - NY'],
			 96=>['',		'Telia North America Inc. - NY'],
			100=>['',		'SITA - New York location'],
			104=>['',		'Startec, Inc. - NY location'],
			108=>['',		'XCOM Technologies ( NY site)'],
			112=>['',		'Graphnet, Inc. (NY site)'],
			116=>['',		'Northland Comm.Group-Syracuse & Utica'],
			120=>['',		'Oneida County Rural Tele. Co.'],
			124=>['',		'Hudson Valley RSA Cellular Partnership'],
			128=>['',		'Econophone, Inc. - NY site'],
			132=>['',		'Chautauqua & Erie Telephone Corp.'],
			136=>['',		'American International Tele.Co.,Inc.-NY'],
			140=>['',		'KDD America, Inc.'],
			144=>['',		'Allegiance Telecom,Inc. - NY site'],
			148=>['',		'AlphaNet Telecom Inc. - NY site'],
			152=>['',		'Long Distance Int\'l Provisioning-NY'],
			156=>['',		'Domtel Comm.Inc. d/b/a Tricom USA-NY'],
			160=>['',		'FDN, Inc. - NY site'],
			164=>['',		'New Millennium Comm.Corp - NY'],
			168=>['Trans Global',	'Trans Global Communications-NY site'],
			172=>['Arbit',		'Arbit Communications, Inc. - NY site'],
			176=>['Arbit',		'Arbit Communications, Inc. -NY site'],
			180=>['',		'Local Fiber, L.L.C.'],
			184=>['',		'Telcom International - NY site'],
			188=>['Net',		'Net Communications,Inc. - NY site'],
			192=>['',		'Justice Technology Corp. - NY'],
			196=>['',		'North American Gateway Inc. - NY site'],
			200=>['',		'North American Telecomm. Corp.'],
			204=>['',		'Transwire LLC'],
			208=>['',		'Transwire LLC'],
			212=>['',		'Transwire LLC'],
			216=>['',		'FaciliCom International - NY site'],
			220=>['Worldport',	'Worldport Communications, Inc. NY site'],
			228=>['',		'OzEmail Interline Pty Limited - NY site'],
			232=>['',		'ITC, Inc. - NY site'],
			236=>['',		'INET Interactive Network System-NY'],
			240=>['',		'Thrifty Call Inc. - NY site'],
			244=>['',		'Bell Atlantic Global Networks -NY site'],
			248=>['Voice Data',	'Voice Data Communicationsl - NY site'],
		},
		50=>{
			state=>['NC',		'North Carolina'],
			  4=>['',		'US LEC of North Carolina, LLC'],
			  8=>['American',	'American Communications Services, Inc.'],
			 12=>['Fiber South',	'Fiber South Communications'],
			 16=>['',		'BellSouth Long Distance, Inc.'],
			 20=>['Time Warner',	'Time Warner Communications'],
			 24=>['Teleport',	'Teleport Communications Group'],
			 28=>['',		'US LEC of North Carolina - NC'],
			 32=>['',		'KMC Telecom Inc. - NC'],
			 36=>['',		'Long Distance Int\'l Provisioning - NC'],
			 40=>['Citizens',	'Citizens Telephone Company'],
			 44=>['',		'Listing Services Solutions, Inc. - NC site'],
			 48=>['Interpath',	'Interpath Communications, Inc.'],
			 52=>['USTel',		'USTel dba Arcada Communications-NC'],
			 56=>['',		'PC Management - NC site'],
			 60=>['',		'PC Management - NC site'],
		},
		51=>{
			state=>['ND',		'North Dakota'],
			  4=>['Dickey Rural',	'Dickey Rural Telephone Company'],
			  8=>['Consolidated',	'Consolidated Telephone Cooperative'],
			 12=>['',		'Dakota Central Telecommunications Cooperative'],
		},
		52=>{
			state=>['OH',		'Ohio'],
			  4=>['Teleport',	'Teleport Communications Group'],
			  8=>['Time Warner',	'Time Warner Communications'],
			 12=>['',		'Buckeye Telesystem - Ohio site'],
			 16=>['',		'Con Quest'],
			 20=>['',		'Con Quest'],
			 24=>['',		'Con Quest'],
		},
		53=>{
			state=>['OK',		'Oklahoma'],
			  4=>['',		'Panhandle telephone Cooperative, Inc.'],
			  8=>['OnQue',		'OnQue Communications, Inc.'],
			 12=>['Scott Cty',	'Scott County Telephone Company'],
			 16=>['American',	'American Communications Services, Inc.'],
			 20=>['',		'Long Distance Int\'l Provisioning - OK'],
			 24=>['',		'Oklahoma Telephone & Telegraph,Inc.'],
			 28=>['Panhandle',	'Panhandle Telephone Cooperative,Inc.'],
		},
		54=>{
			state=>['OR',		'Oregon'],
			  4=>['ECI',		'ECI Communications'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['',		'Metro One Telecommunications'],
			 16=>['',		'OGI Telecomm'],
			 20=>['',		'Great West Services, Ltd.'],
			 24=>['',		'Oregon Telco Services, Inc.'],
			 28=>['Rio',		'Rio Communications, Inc. - OR site'],
			 32=>['USTel',		'USTel dba Arcada Communications-OR'],
			 36=>['',		'Cascade Utilities, Inc. - OR site'],
			 40=>['',		'Oregon Telephone Corporation'],
			 44=>['North-State',	'North-State Telephone Company'],
		},
		55=>{
			state=>['PA',		'Pennsylavania'],
			  4=>['Palmerton',	'Palmerton Telephone Company'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['Teleport',	'Teleport Communications Group'],
			 16=>['Residential',	'Residential Communications Network, Inc.'],
			 20=>['',		'Buffalo Valley Telephone'],
			 24=>['',		'Conestoga Wireless Company'],
			 28=>['',		'Metro One Telecommunications'],
			 32=>['Xtel',		'Xtel Communications, Inc.'],
			 36=>['',		'Hyperion Telecomm.Inc. - PA'],
			 40=>['',		'Hyperion Telecomm. Inc. - PA'],
			 44=>['',		'Hyperion Telecomm. Inc. - PA'],
			 48=>['Focal',		'Focal Communications Corp. - PA site'],
			 52=>['',		'Bell Atlantic Global Networks-PA site'],
			 56=>['PaeTec',		'PaeTec Communications, Inc. - PA site'],
			 60=>['Level 3',	'Level 3 Communications - PA site'],
			 64=>['USTel',		'USTel dba Arcada Communications-PA'],
			 68=>['',		'Allegiance Telcom, Inc. - PA site'],
			 72=>['',		'Comav Telco, Inc. - PA site'],
		},
		56=>{
			state=>['RI',		'Rhode Island'],
			  4=>['Teleport',	'Teleport Communications Group'],
			  8=>['',		'XCOM Technologies - Rhode Island'],
		},
		57=>{
			state=>['SC',		'South Carolina'],
			  4=>['Pond Branch',	'Pond Branch Telephone Company, Inc.'],
			  8=>['W Carolina R',	'West Carolina Rural Telephone Cooperative, Inc.'],
			 12=>['American',	'American Communications Services, Inc.'],
			 16=>['',		'NewSouth Comm.L.L.C. - SC'],
			 20=>['',		'Rhinos International Ltd. Co.'],
			 24=>['',		'Rhinos International Ltd. Co.'],
			 28=>['',		'Rhinos International Ltd. Co.'],
			 32=>['',		'P.V. Tel LLC - SC site'],
		},
		58=>{
			state=>['SD',		'South Dakota'],
			  4=>['RC',		'RC Communications, Inc.'],
			  8=>['',		'Bridgewater-Canistota Independent Telephone'],
			 12=>['Midco',		'Midco Communications'],
			 16=>['',		'Kadoka Telephone Co.-South Dakota'],
		},
		59=>{
			state=>['TN',		'Tennessee'],
			  4=>['Teleport',	'Teleport Communications Group'],
			  8=>['American',	'American Communications Services, Inc.'],
			 12=>['',		'Advantage Cellular Systems'],
			 16=>['',		'BellSouth Long Distance, Inc.'],
			 20=>['',		'US LEC of North Carolina - Tennessee'],
			 24=>['',		'Teleport Comm. Group - Tennessee'],
			 28=>['Chase',		'Chase Communications,Inc. -Tennessee'],
			 32=>['Chase',		'Chase Communications,Inc. -Tennessee'],
			 36=>['Chase',		'Chase Communications,Inc. - Tennessee'],
			 40=>['Chase',		'Chase Communications,Inc. - Tennessee'],
			 44=>['',		'Comm. Depot, Inc. - TN site'],
			 48=>['',		'P.V. Tel LLC - TN site'],
		},
		60=>{
			state=>['TX',		'Texas'],
			  4=>['Future',		'Future Telephone Communications, Inc. dba FTC  Long Distance'],
			  8=>['Universal',	'Universal Communications Network, Inc.'],
			 12=>['Cameron',	'Cameron Telephone Company Texas Division'],
			 16=>['Lk. Livingston',	'Lake Livingston Telephone Company'],
			 20=>['IXC',		'IXC Communications, Inc.'],
			 24=>['Crawford',	'Crawford Telephone Company'],
			 28=>['Teleport',	'Teleport Communications Group'],
			 32=>['Teleport',	'Teleport Communications Group'],
			 36=>['',		'NRPT Corporation'],
			 40=>['',		'Intellicom, Inc.'],
			 44=>['',		'Phonetime, Inc.'],
			 48=>['American',	'American Communications Services, Inc.'],
			 52=>['American',	'American Communications Services, Inc.'],
			 56=>['Border to Border','Border to Border Communications, Inc.'],
			 60=>['OpTel',		'OpTel, Inc.'],
			 64=>['Five Star',	'Five Star Telecom'],
			 68=>['Voice Track',	'Voice Track'],
			 72=>['Taylor',		'Taylor Communications Group, Inc.'],
			 76=>['FTI',		'FTI Communications'],
			 80=>['Hill Cty',	'Hill Country Telephone Cooperative, Inc.'],
			 84=>['',		'TCA Long Distance'],
			 88=>['',		'CSW/ICG ChoiceCom, L.P.'],
			 92=>['',		'CSW/ICG ChoiceCom, L.P.'],
			 96=>['',		'CSW/ICG ChoiceCom, L.P.'],
			100=>['Time Warner',	'Time Warner Communications'],
			104=>['',		'KMC Telecom Inc.'],
			108=>['Telco',		'Telco Communications Group'],
			112=>['',		'SA Telecommunications'],
			116=>['Alenco',		'Alenco Communications, Inc.'],
			120=>['Alenco',		'Alenco Communications, Inc.'],
			124=>['',		'World Exchange'],
			128=>['',		'ILD Teleservices, Inc.'],
			132=>['',		'CoServ L.L.C.'],
			136=>['',		'IWL Comm. d.b.a. IWL Connect-Texas'],
			140=>['',		'Star Telecom (Dallas,Texas site)'],
			144=>['',		'Allegiance Telecom,Inc.-Texas site'],
			148=>['',		'Econophone, Inc. - Texas site'],
			152=>['',		'FiberWave'],
			156=>['',		'West Texas Rural Tele.Cooperative'],
			160=>['',		'West Texas Rural Tele. Cooperative'],
			164=>['',		'West Texas Rural Tele. Cooperative'],
			168=>['',		'WT Services, Inc.'],
			172=>['Revenue',	'Revenue Communications, Inc.-Texas'],
			176=>['',		'Great West Services, Ltd.'],
			180=>['Industry',	'Industry Telephone Company'],
			184=>['',		'Telescape International, Inc.'],
			188=>['',		'FDN, Inc. - Texas site'],
			192=>['',		'New Millennium Comm.Corp. - Texas'],
			196=>['',		'Tele.Co.of Central Florida,Inc.-Texas'],
			200=>['',		'Blossom Telephone Co.,Inc.'],
			204=>['',		'American Telesource Int\'l Inc.'],
			208=>['',		'American Gateway Telecommunications'],
			212=>['',		'FaciliCom International-Texas site'],
			216=>['',		'AirTouch Satellite Services-Texas site'],
			220=>['Tech',		'Tech Telephone Company, Ltd.'],
			224=>['',		'IXE Corporation'],
			228=>['Worldport',	'Worldport Communications,Inc.-TX site'],
			232=>['Logix',		'Logix Communications'],
			236=>['Logix',		'Logix Communications'],
			240=>['',		'Thrifty Call Inc. - TX site'],
			244=>['',		'Paging Network Inc. - TX site'],
			248=>['',		'Nortex Telcom'],
			252=>['',		'Twister Comm. Network, Inc.'],
		},
		61=>{
			state=>['UT',		'Utah'],
			  4=>['All West',	'All West Communications'],
			  8=>['',		'Phoenix Fiberlink of Utah, Inc.'],
			 12=>['Teleport',	'Teleport Communications Group'],
			 16=>['FTI',		'FTI Communications'],
			 20=>['Teltrust',	'Teltrust Communications Services, Inc.'],
			 24=>['',		'Access Long Distance'],
			 28=>['',		'Access Long Distance'],
			 32=>['',		'Access Long Distance'],
			 36=>['Teltrust',	'Teltrust Communications Services, Inc.'],
			 40=>['',		'USTel dba Arcada Commun. - UT side'],
		},
		62=>{
			state=>['VT',		'Vermont'],
			  4=>['',		'Hyperion Telcommunications, Inc.'],
			  8=>['',		'Personal Commun. Network,Inc. - VT'],
			 12=>['',		'Personal Commun. Network,Inc. - VT'],
			 16=>['',		'Personal Commun. Network, Inc. - VT'],
		},
		63=>{
			state=>['VA',		'Virginia'],
			  4=>['',		'Swiftcall (USA) Inc.'],
			  8=>['Global Tone',	'Global Tone Communications, Inc.'],
			 12=>['',		'Hyperion Telecommunications, Inc.'],
			 16=>['',		'US LEC of North Carolina - Virginia'],
			 20=>['',		'XCOM Technologies - Virginia site'],
			 24=>['',		'KMC Telecom Inc. - Virginia site'],
			 28=>['',		'CFW Wireless'],
			 32=>['',		'LEVEL 3 - VA site'],
			 36=>['',		'Allegiance Telecom, Inc. - VA site'],
			 40=>['',		'Ameritech Global Gateway Services'],
		},
		64=>{
			state=>['WV',		'West Virginia'],
			  4=>['',		'ComScape Telecommunications, Inc.'],
			  8=>['',		'Hardy Telecommunications, Inc.'],
		},
		65=>{
			state=>['WA',		'Washington'],
			  4=>['',		'XYPOINT Corporation'],
			  8=>['Teleport',	'Teleport Communications Group'],
			 12=>['',		'Five Star Telecom'],
			 16=>['',		'Phonetime, Inc.'],
			 20=>['St. John',	'St. John Telephone Company'],
			 24=>['',		'USA Global Link'],
			 28=>['',		'Global Connect Partners LLC'],
			 32=>['',		'Metro One Telecommunications'],
			 36=>['',		'World Exchange'],
			 40=>['',		'XYPOINT Corporation'],
			 44=>['',		'XYPOINT Corporation'],
			 48=>['',		'AlphaNet Telecom Inc. - Seattle site'],
			 52=>['',		'Global Mobility Systems, Inc.'],
			 56=>['',		'Pass Word, Inc.'],
			 60=>['',		'Cable Plus Company LP'],
			 64=>['',		'Great West Services'],
			 68=>['',		'USTel dba Arcada Commun. - WA site'],
		},
		66=>{
			state=>['WI',		'Wisconsin'],
			  4=>['Somerset',	'Somerset Telephone Company, Inc.'],
			  8=>['Airadigm',	'Airadigm Communications'],
			 12=>['Airadigm',	'Airadigm Communications'],
			 16=>['Airadigm',	'Airadigm Communications'],
			 20=>['Airadigm',	'Airadigm Communications'],
			 24=>['Airadigm',	'Airadigm Communications'],
			 28=>['Airadigm',	'Airadigm Communications'],
			 32=>['',		'Kendall Telephone Inc.'],
			 36=>['',		'Cuba City Telephone Exchange Company'],
			 40=>['',		'KMC Telecom Inc.'],
			 44=>['Manawa',		'Manawa Telephone Company, Inc.'],
			 48=>['',		'US Xchange LLC(Wisconsin site)'],
			 52=>['',		'Lemonweir Valley Telephone Co.'],
			 56=>['',		'Amery Telcom, Inc.'],
		},
		67=>{
			state=>['WY',		'Wyoming'],
			  4=>['Range',		'Range Telephone Cooperative'],
			  8=>['',		'Silver Star Telephone Co. - WY site'],
			 12=>['RT',		'RT Communications, Inc.'],
		},
		68=>{
			state=>['AB',		'Alberta'],
			  4=>['',		'AirTouch Satellite Services-Calgary site'],
		},
		69=>{
			state=>['BC',		'British Columbia'],
			  4=>['',		'Hongkong Telecom Canada'],
			  8=>['',		'Prince Rupert City Telephone'],
			 12=>['',		'GT Group Telecom Networks, Inc.'],
			 16=>['',		'Vancouver Telephone Co. Ltd.'],
		},
		70=>{
			state=>['MB',		'Manitoba'],
		},
		71=>{
			state=>['NB',		'New Brunswick'],
		},
		72=>{
			state=>['NL',		'Newfoundland'],
		},
		73=>{
			state=>['NT',		'Northwest Territories'],
		},
		74=>{
			state=>['NS',		'Nova Scotia'],
		},
		75=>{
			state=>['ON',		'Ontario'],
			  4=>['',		'Primus Telecommunications, Inc'],
			  8=>['Telehop',	'Telehop Communications, Inc.'],
			 12=>['',		'Dryden Municipal Telephone System'],
			 16=>['',		'Econophone, Inc. - Ontario site'],
			 20=>['Kenora M',	'Kenora Municipal Telephone Company'],
			 24=>['',		'AlphaNet Telecom Inc.'],
			 28=>['',		'Eastern Independent Telecomm. Ltd.'],
			 32=>['',		'AirTouch Satellite Services-Ontario site'],
			 36=>['',		'Wightman Telephone Ltd.'],
			 40=>['Premiere',	'Premiere Communications -Ontario site'],
		},
		76=>{
			state=>['PE',		'Prince Edward Island'],
		},
		77=>{
			state=>['QC',		'Quebec'],
			  4=>['',		'La Compagnie de Telephone de St-Victor'],
			  8=>['',		'Telephone de St-Ephrem'],
			 12=>['',		'Telephone Guevremont Inc.'],
			 16=>['',		'Videotron Telecom Ltee'],
			 20=>['',		'La Corporation de Telephone de la Baie'],
			 24=>['',		'Videotron Telecom Ltee'],
			 28=>['',		'La Compagnie de Telephone de Lambton'],
			 32=>['',		'La Cie de Telephone de Courcelles Inc.'],
		},
		78=>{
			state=>['SK',		'Saskatchewan'],
		},
		79=>{
			state=>['YT',		'Yukon'],
		},
		80=>{
			state=>['Atlantic',	'ATLANTIC and CARIBBEAN ISLANDS (U.S. Territories)'],
			  4=>['',		'Panam Wireless, Inc.'],
			  8=>['',		'IT&E Overseas, Inc.'],
			 12=>['Quantum',	'Quantum Communications Ltd.'],
			 16=>['Quantum',	'Quantum Communications Ltd.'],
			 20=>['',		'AirTouch Satellite Serv.-Puerto Rico site'],
			 24=>['Bermuda',	'Bermuda Digital Communications Ltd.'],
			 28=>['',		'Caribcom'],
		},
		81=>{
			state=>['Pacific',	'PACIFIC ISLANDS (U.S. Territories)'],
			  4=>['',		'Guam Telephone Authority'],
			  8=>['',		'Access Telecom'],
			 12=>['',		'American Samoa Telecommunications'],
			 16=>['',		'American Samoa Telecommunications'],
			 20=>['',		'Guam Cellular & Paging'],
			 24=>['',		'Saipan Cellular & Paging'],
		},
		82=>{
			state=>['TX',		'Texas'],
			  4=>['',		'N + 1, Inc., - TX site'],
			  8=>['Level 3',	'Level 3 Communications - TX site'],
			 12=>['',		'Fort Bend Long Distance Co. - TX site'],
			 16=>['',		'USTel dba Arcada Commun. - TX site'],
			 20=>['',		'Network Switching Services,Inc.-TX site'],
			 24=>['Grande River',	'Grande River Communications,Inc.'],
			 28=>['Southside',	'Southside Communications,Inc.-TX site'],
			 32=>['',		'Alamosa PCS LLC - TX site'],
			 36=>['',		'Allegiance Telecom, Inc. - TX site'],
			 40=>['',		'Millenium Telcom, L.L.C.'],
			 44=>['',		'Logix'],
			 48=>['',		'Logix'],
			 52=>['Cumby',		'Cumby Telephone Cooperative,Inc.'],
			 56=>['',		'U.S. Operations, Inc.'],
		},
		83=>{
			state=>['CA',		'California'],
			  4=>['',		'Allegiance Telecom, Inc. - CA site'],
			  8=>['PaeTec',		'PaeTec Communications,Inc. - CA site'],
			 12=>['Level 3',	'Level 3 Communications - CA site'],
			 16=>['',		'KDD America Inc. - CA site'],
			 20=>['',		'USTel dba Arcada Commun. - CA site'],
			 24=>['',		'WorldxChange - CA site'],
			 28=>['Premiere',	'Premiere Communications - CA site'],
			 32=>['',		'Network Switching Services,Inc.-CA site'],
			 36=>['',		'US TelePacific Corp.dba TelePacific'],
			 40=>['',		'OpTel,Inc. - CA site'],
			 48=>['',		'VIP Calling - CA site'],
			 52=>['',		'Alphanet Telecom Inc. - CA site'],
			 56=>['',		'RCN Telecom of California'],
			 60=>['Startec',	'Startec Global Communications Corp.'],
		},
		84=>{
			state=>['NY',		'New York'],
			  4=>['',		'Carrier 1, Inc. - NY site'],
			  8=>['',		'Telergy'],
			 12=>['',		'USTel dba Arcada Commun. - NY site'],
			 16=>['',		'City Telecom (USA)Inc. - NY site'],
			 20=>['',		'Network Switching Services, Inc.NY site'],
			 24=>['',		'Interoute Telecommunications, Inc.'],
			 28=>['',		'Interoute Telecommunications, Inc.'],
			 32=>['Choice One',	'Choice One Communications-NY site'],
			 36=>['',		'Star/UniSource Carrier Services'],
			 40=>['Choice One',	'Choice One Communications'],
			 44=>['',		'Comav Telco, Inc. - NY site'],
		},
);

our $pc_assignments = {
	large_networks=>\%large_networks,
	small_networks=>\%small_networks,
	pc_blocks=>\%pc_blocks,
};

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
	my $self = shift;
	$self->{logs} = [];
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
}

# -------------------------------------
package Counts;
use strict;
# -------------------------------------

#package Counts;
sub init {
	my ($self,$interval) = @_;
	$self->{interval} = $interval;
	$self->{incs} = {};
	$self->{pegs} = {};
	$self->{sims} = {};
	$self->{durs} = {};
	$self->{iats} = {};
	$self->{itvs} = {};
	$self->{plots} = {};
	$self->{plots}->{pdfh} = {};
	$self->{plots}->{cdfh} = {};
	$self->{plots}->{phsh} = {};
	$self->{plots}->{pdfa} = {};
	$self->{plots}->{cdfa} = {};
	$self->{plots}->{phsa} = {};
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
sub inc {
	my ($self,$msg,$dir) = @_;
	my $li = $msg->{li};
	$self->{incs}->{$dir}->{sus} += 1;
	if ($li == 0) {
		$self->{incs}->{$dir}->{fisus} += 1;
		return;
	}
	if ($li == 1) {
		$self->{incs}->{$dir}->{lssus} += 1;
		return;
	}
	if ($li == 2) {
		$self->{incs}->{$dir}->{lss2s} += 1;
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
}

#package Counts;
sub peg {
	my ($self,$event) = @_;
	$self->{pegs}->{$event} += 1;
}

#package Counts;
sub act {
	my ($self) = @_;
	$self->{actcnt} += 1;
}

#package Counts;
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

#package Counts;
sub sim {
	my ($self,$oldevent,$newevent) = @_;
	my ($new,$old);
	if ($new = $self->{sims}->{$newevent}) {
		$new->{curr}++;
		$new->{hiwa} = $new->{curr} if $new->{curr} > $new->{hiwa};
	} else {
		$self->{sims}->{$newevent} = { curr=>1,hiwa=>1,lowa=>0 };
	}
	if ($old = $self->{sims}->{$oldevent}) {
		$old->{curr}--;
		$old->{lowa} = $old->{curr} if $old->{curr} < $old->{lowa} || $old->{lowa} <= 0;
		if ($old->{curr} < 0) {
			$old->{hiwa} += -$old->{curr};
			$old->{lowa} += -$old->{curr};
			$old->{curr} = 0;
		}
	} else {
		$self->{sims}->{$oldevent} = { curr=>0,hiwa=>0,lowa=>0 };
	}
}

#package Counts;
sub itv {
	my ($self,$event,$start,$end) = @_;
	my $itv = { beg=>$start, end=>$end };
	$self->{itvs}->{$event} = [] unless exists $self->{itvs}->{$event};
	push @{$self->{itvs}->{$event}}, $itv;
	$self->dur($event,$start,$end);
}

#package Counts;
sub dur {
	my ($self,$event,$start,$end) = @_;
	my $dsecs = $end->{tv_sec} - $start->{tv_sec};
	my $dusec = $end->{tv_usec} - $start->{tv_usec};
	while ($dusec < 0 ) { $dsecs++; $dusec += 1000000; }
	my $dur = $dsecs + $dusec/1000000;
	$self->{durs}->{$event} += $dur;
	my $int = $dsecs;
	$int++ if $dusec > 0;
	$self->{durs}->{dist}->{$event}->{$int} += 1;
}

#package Counts;
sub iat {
	my ($self,$event,$ts) = @_;

	$self->{iats}->{$event} = [] unless exists $self->{iats}->{$event};
	push @{$self->{iats}->{$event}}, $ts;
}

# -------------------------------------
package Stats;
use strict;
use vars qw(@ISA);
@ISA = qw(Counts);
# -------------------------------------

#package Stats;
sub init {
	my $self = shift;
	Counts::init($self);
	$self->{hist} = {};
}

#package Stats;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Stats;
sub interval {
	my ($self,$ts) = @_;
	my $int = $ts->{tv_sec};
	$int++ if ($ts->{tv_usec} > 0);
	$int = int(($int + 299)/300);
	return $int;
}

#package Stats;
sub hist {
	my ($self,$int) = @_;
	my $obj;
	unless ($obj = $self->{hist}->{$int}) {
		$obj = $self->{hist}->{$int} = Counts->new($int);
	}
	return $obj;
}

#package Stats;
sub inc {
	my ($self,$msg,$dir) = @_;
#	my $int = $self->interval($msg->{hdr});
#	my $hist = $self->hist($int);
#	$hist->inc($msg,$dir);
	$self->Counts::inc($msg,$dir);
}

#package Stats;
sub peg {
	my ($self,$event,$ts) = @_;
#	my $int = $self->interval($ts);
#	my $hist = $self->hist($int);
#	$hist->peg($event);
	$self->Counts::peg($event);
}

#package Stats;
sub act {
	my ($self) = @_;
	$self->Counts::act;
}

#package Stats;
sub cnt {
	my ($self,@args) = @_;
	$self->Counts::cnt(@args);
}

#package Stats;
sub sim {
	my ($self,$oldevent,$newevent) = @_;
	$self->Counts::sim($oldevent,$newevent);
}

#package Stats;
sub itv {
	my ($self,$event,$beg,$end) = @_;
	$self->Counts::itv($event,$beg,$end);
}

#package Stats;
sub dur {
	my ($self,$event,$beg,$end) = @_;
#	my $s = $self->interval($beg);
#	my $e = $self->interval($end);
#	for (my $t = $s; $t <= $e; $t += 300) {
#		my $hist = $self->hist($t);
#		if ($t == $s) {
#			$hist->dur($event,$beg,{tv_sec=>$t+300,tv_usec=>0});
#		} elsif ($t == $e) {
#			$hist->dur($event,{tv_sec=>$t,tv_usec=>0},$end);
#		} else {
#			$hist->dur($event,{tv_sec=>$t,tv_usec=>0},{tv_sec=>$t+300,tv_usec=>0});
#		}
#	}
	$self->Counts::dur($event,$beg,$end);
}

#package Stats;
sub iat {
	my ($self,$event,$ts) = @_;
	$self->Counts::iat($event,$ts);
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
	my ($type,$me,$stat,$event,$w,$X,$Y,@args) = @_;
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
	my $dirname = $dirlabels[int($event/10)];
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
	my $evtname = $evtlabels[$self->{event} % 10];
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
	foreach my $i (@{$self->{stat}->{itvs}->{$self->{event}}}) {
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
	foreach my $i (sort {tscmp($a->{beg},$b->{beg})} @{$self->{stat}->{itvs}->{$self->{event}}}) {
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
	my ($type,$stat,$event,@args) = @_;
	my $me = \$stat->{plots}->{pdfh}->{$event};
	return PlotPdf::new($type,$me,$stat,$event,@args);
}

# -------------------------------------
package PlotCdfHolding;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotCdf PlotHolding);
# -------------------------------------

#package PlotCdfHolding;
sub get {
	my ($type,$stat,$event,@args) = @_;
	my $me = \$stat->{plots}->{cdfh}->{$event};
	return PlotCdf::new($type,$me,$stat,$event,@args);
}

# -------------------------------------
package PlotPhaseHolding;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotPhase PlotHolding);
# -------------------------------------

#package PlotPhaseHolding;
sub get {
	my ($type,$stat,$event,@args) = @_;
	my $me = \$stat->{plots}->{phsh}->{$event};
	return PlotPhase::new($type,$me,$stat,$event,@args);
}

# -------------------------------------
package PlotPdfArrival;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotPdf PlotArrival);
# -------------------------------------

#package PlotPdfArrival;
sub get {
	my ($type,$stat,$event,@args) = @_;
	my $me = \$stat->{plots}->{pdfa}->{$event};
	return PlotPdf::new($type,$me,$stat,$event,@args);
}

# -------------------------------------
package PlotCdfArrival;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotCdf PlotArrival);
# -------------------------------------

#package PlotCdfArrival;
sub get {
	my ($type,$stat,$event,@args) = @_;
	my $me = \$stat->{plots}->{cdfa}->{$event};
	return PlotCdf::new($type,$me,$stat,$event,@args);
}

# -------------------------------------
package PlotPhaseArrival;
use strict;
use vars qw(@ISA);
@ISA = qw(PlotPhase PlotArrival);
# -------------------------------------

#package PlotPhaseArrival;
sub get {
	my ($type,$stat,$event,@args) = @_;
	my $me = \$stat->{plots}->{phsa}->{$event};
	return PlotPhase::new($type,$me,$stat,$event,@args);
}

# -------------------------------------
package MsgStats;
use strict;
use vars qw(@ISA);
@ISA = qw(Stats);
# -------------------------------------

use constant {
	CTS_UNINIT	=> 0,
	CTS_IDLE	=> 1,
	CTS_WAIT_ACM	=> 2,
	CTS_WAIT_ANM	=> 3,
	CTS_ANSWERED	=> 4,
	CTS_SUSPENDED	=> 5,
	CTS_WAIT_RLC	=> 6,
	CTS_SEND_RLC	=> 7,
	CTS_COMPLETE	=> 8,
};

#package MsgStats;
sub init {
	my ($self,@args) = @_;
	my $self = shift;
	Stats::init($self,@args);
	$self->{dist} = {};
}

#package MsgStats;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
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
			-textvariable=>\$self->{incs}->{0}->{sus},
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
		if ($self->{incs}->{0}->{fisus} or $self->{incs}->{1}->{fisus}) {
			$w = $s->Label(%labelright,
				-text=>'FISUs:',
			)->grid(-row=>$row,-column=>0,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of FISUs sent and received.",
			);
			$w = $s->Entry(%entryright,
				-textvariable=>\$self->{incs}->{0}->{fisus},
			)->grid(-row=>$row,-column=>1,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of FISUs sent.",
			);
			$w = $s->Entry(%entryright,
				-textvariable=>\$self->{incs}->{1}->{fisus},
			)->grid(-row=>$row++,-column=>2,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of FISUs received.",
			);
		}
		if ($self->{incs}->{0}->{lssus} or $self->{incs}->{1}->{lssus}) {
			$w = $s->Label(%labelright,
				-text=>'1-octet LSSUs:',
			)->grid(-row=>$row,-column=>0,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of 1-octet LSSUs sent and received.",
			);
			$s->Entry(%entryright,
				-textvariable=>\$self->{incs}->{0}->{lssus},
			)->grid(-row=>$row,-column=>1,-sticky=>'ewns');
			$w = $balloon->attach($w,
				-balloonmsg=>"Number of 1-octet LSSUs sent.",
			);
			$s->Entry(%entryright,
				-textvariable=>\$self->{incs}->{1}->{lssus},
			)->grid(-row=>$row++,-column=>2,-sticky=>'ewns');
			$w = $balloon->attach($w,
				-balloonmsg=>"Number of 1-octet LSSUs received.",
			);
		}
		if ($self->{incs}->{0}->{lss2s} or $self->{incs}->{1}->{lss2s}) {
			$w = $s->Label(%labelright,
				-text=>'2-octet LSSUs:',
			)->grid(-row=>$row,-column=>0,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of 2-octet LSSUs sent and received.",
			);
			$w = $s->Entry(%entryright,
				-textvariable=>\$self->{incs}->{0}->{lss2s},
			)->grid(-row=>$row,-column=>1,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of 2-octet LSSUs sent.",
			);
			$w = $s->Entry(%entryright,
				-textvariable=>\$self->{incs}->{1}->{lss2s},
			)->grid(-row=>$row++,-column=>2,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of 2-octet LSSUs received.",
			);
		}
		if ($self->{incs}->{0}->{msus} or $self->{incs}->{1}->{msus}) {
			$w = $s->Label(%labelright,
				-text=>'MSUs:',
			)->grid(-row=>$row,-column=>0,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of MSUs sent and received.",
			);
			$w = $s->Entry(%entryright,
				-textvariable=>\$self->{incs}->{0}->{msus},
			)->grid(-row=>$row,-column=>1,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of MSUs sent.",
			);
			$w = $s->Entry(%entryright,
				-textvariable=>\$self->{incs}->{1}->{msus},
			)->grid(-row=>$row,-column=>2,-sticky=>'ewns');
			$balloon->attach($w,
				-balloonmsg=>"Number of MSUs received.",
			);
			for (my $mp = 0; $mp < 4; $mp++) {
				my $p;
				my $mpp0 = sprintf('%6.2f', ($self->{incs}->{0}->{mp}->{$mp}*100)/(($p=$self->{incs}->{0}->{msus})?$p:1));
				my $mpp1 = sprintf('%6.2f', ($self->{incs}->{1}->{mp}->{$mp}*100)/(($p=$self->{incs}->{1}->{msus})?$p:1));
				if ($mp == 3) {
					$bmsg = "\nThis value should be between 0% and 5%.";
				} elsif ($mp == 2) {
					$bmsg = "\nThis value should be between 0% and 20%.";
				} else {
					$bmsg = "\nThis value should be between 30% and 45%.";
				}
				$w = $s->Label(%labelright,
					-text=>"mp($mp)",
				)->grid(-row=>$row,-column=>3,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of MP($mp) MSUs sent and received.",
				);
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{0}->{mp}->{$mp},
					-validatecommand=>[sub{
						my ($mpp,$mp,$self,$row,$col,$old,$proposed,$index,@args) = @_;
						my $p;
						$$mpp = sprintf('%6.2f', ($self->{incs}->{0}->{mp}->{$mp}*100)/(($p=$self->{incs}->{0}->{msus})?$p:1));
						return 1;
					},\$mpp0,$mp,$self],
					-validate=>'all',
				)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
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
				)->grid(-row=>$row,-column=>5,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of MP($mp) MSUs received.",
				);
				$w = $s->Label(%labelright,
					-text=>"mp($mp)%",
				)->grid(-row=>$row,-column=>6,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Percentage of MP($mp) MSUs sent and received.$bmsg",
				);
				$w = $s->Entry(%entryright,
					-textvariable=>\$mpp0,
				)->grid(-row=>$row,-column=>7,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Percentage of MP($mp) MSUs sent.$bmsg",
				);
				$w = $s->Entry(%entryright,
					-textvariable=>\$mpp1,
				)->grid(-row=>$row++,-column=>8,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Percentage of MP($mp) MSUs received.$bmsg",
				);
			}
		}
		if ($self->{incs}->{0}->{sis} or $self->{incs}->{1}->{sis}) {
			my %keys = ();
			foreach my $k (keys %{$self->{incs}->{0}->{sis}},
				       keys %{$self->{incs}->{1}->{sis}}) {
				$keys{$k} = 1;
			}
			foreach my $si (sort {$a <=> $b} keys %keys) {
				next unless (exists $mtypes{$si});
				$w = $s->Label(%labelright,
					-text=>"$mtypes{$si}->{0x00}->[0]($si):",
				)->grid(-row=>$row,-column=>0,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of \U$mtypes{$si}->{0x00}->[0]($si)\E messages sent and received.",
				);
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{0}->{sis}->{$si}->{msus},
				)->grid(-row=>$row,-column=>1,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of \U$mtypes{$si}->{0x00}->[0]($si)\E messages sent.",
				);
				$w = $s->Entry(%entryright,
					-textvariable=>\$self->{incs}->{1}->{sis}->{$si}->{msus},
				)->grid(-row=>$row,-column=>2,-sticky=>'ewns');
				$balloon->attach($w,
					-balloonmsg=>"Number of \U$mtypes{$si}->{0x00}->[0]($si)\E messages received.",
				);
				my $mtcount = 0;
				my %keys2 = ();
				foreach my $k (keys %{$self->{incs}->{0}->{sis}->{$si}},
					       keys %{$self->{incs}->{1}->{sis}->{$si}}) {
					$keys2{$k} = 1;
				}
				foreach my $mt (sort {$a <=> $b} keys %keys2) {
					next if $mt eq 'msus';
					next unless exists $mtypes{$si}->{$mt};
					$mtcount += 1;
					$w = $s->Label(%labelright,
						-text=>"$mtypes{$si}->{$mt}->[0]($mt):",
					)->grid(-row=>$row,-column=>3,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of \U$mtypes{$si}->{$mt}->[0]($mt)\E messages sent and received.",
					);
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{incs}->{0}->{sis}->{$si}->{$mt}->{msus},
					)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of \U$mtypes{$si}->{$mt}->[0]($mt)\E messages sent.",
					);
					$w = $s->Entry(%entryright,
						-textvariable=>\$self->{incs}->{1}->{sis}->{$si}->{$mt}->{msus},
					)->grid(-row=>$row,-column=>5,-sticky=>'ewns');
					$balloon->attach($w,
						-balloonmsg=>"Number of \U$mtypes{$si}->{$mt}->[0]($mt)\E messages received.",
					);
					my $mpcount = 0;
					my %keys3 = ();
					foreach my $k (keys %{$self->{incs}->{0}->{sis}->{$si}->{$mt}},
						       keys %{$self->{incs}->{1}->{sis}->{$si}->{$mt}}) {
						$keys3{$k} = 1;
					}
					foreach my $mp (sort {$a <=> $b} keys %keys3) {
						next if $mp eq 'msus';
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
						)->grid(-row=>$row,-column=>6,-sticky=>'ewns');
						$balloon->attach($w,
							-balloonmsg=>"Number of MP($mp) \U$mtypes{$si}->{$mt}->[0]($mt)\E messages sent and received.$bmsg",
						);
						$w = $s->Entry(%entryright,
							-fg=>$color,
							-textvariable=>\$self->{incs}->{0}->{sis}->{$si}->{$mt}->{$mp},
						)->grid(-row=>$row,-column=>7,-sticky=>'ewns');
						$balloon->attach($w,
							-balloonmsg=>"Number of MP($mp) \U$mtypes{$si}->{$mt}->[0]($mt)\E messages sent.$bmsg",
						);
						$w = $s->Entry(%entryright,
							-fg=>$color,
							-textvariable=>\$self->{incs}->{1}->{sis}->{$si}->{$mt}->{$mp},
						)->grid(-row=>$row++,-column=>8,-sticky=>'ewns');
						$balloon->attach($w,
							-balloonmsg=>"Number of MP($mp) \U$mtypes{$si}->{$mt}->[0]($mt)\E messages received.$bmsg",
						);
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

#package MsgStats;
sub plotmenu {
	my ($self,$tw,$event,$prefix,$label,$X,$Y) = @_;
	my $m = $tw->Menu(
		-tearoff=>1,
		-title=>'Plot Menu',
	);
	$m->add('command',
		-label=>'Holding PDF...',
		-underline=>0,
		-command=>[\&PlotPdfHolding::get,'PlotPdfHolding',$self,$event,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Holding CDF...',
		-underline=>0,
		-command=>[\&PlotCdfHolding::get,'PlotCdfHolding',$self,$event,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Holding phase...',
		-underline=>0,
		-command=>[\&PlotPhaseHolding::get,'PlotPhaseHolding',$self,$event,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Interarrival PDF...',
		-underline=>0,
		-command=>[\&PlotPdfArrival::get,'PlotPdfArrival',$self,$event,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Interarrival CDF...',
		-underline=>0,
		-command=>[\&PlotCdfArrival::get,'PlotCdfArrival',$self,$event,$tw,$X,$Y],
	);
	$m->add('command',
		-label=>'Interarrival phase...',
		-underline=>0,
		-command=>[\&PlotPhaseArrival::get,'PlotPhaseArrival',$self,$event,$tw,$X,$Y],
	);
	$m->Popup(
		-popanchor=>'nw',
		-popover=>'cursor',
	);
}

#package MsgStats;
sub updatedur {
	my ($index,$value,$op,$self,$top,$dur,$event,$span) = @_;
	if (ref $self eq 'Counts') {
		$span = 300;
	} else {
		my ($b,$e) = ($top->{begtime},$top->{endtime});
		$span = $e->{tv_sec}  - $b->{tv_sec}
		     + ($e->{tv_usec} - $b->{tv_usec})/1000000;
	}
	$$dur = sprintf("%12.2f", $self->{durs}->{$event} / $span);
	return $value;
}

#package MsgStats;
sub dircstat {
	my ($self,$top,$tw,$balloon,$row,$span,$event,$prefix,$label) = @_;
	my ($p,$w,$bmsg);
	$bmsg =
"$prefix $label occupancy.\
This is the total duration of time (in Erlangs) that\
circuits have been in the $label state for the study\
period.";
	my $dur = sprintf("%12.2f", $self->{durs}->{$event} / $span);
	$tw->traceVariable(\$self->{durs}->{$event},'w'=>[\&MsgStats::updatedur,$self,$top,\$dur,$event,$span]);
	$tw->traceVariable($top->{endtime},'w'=>[\&MsgStats::updatedur,$self,$top,\$dur,$event,$span]);
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
		-textvariable=>\$self->{pegs}->{$event},
	)->grid(-row=>$$row,-column=>3,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label duration.\
This is the average duration (in seconds) that circuits\
were in the $label state for the study period.";
	my $del = sprintf("%7.2f", $self->{durs}->{$event}/(($p = $self->{pegs}->{$event})?$p:1));
	$tw->traceVariable(\$self->{durs}->{$event},'w'=>[sub{
		my ($index,$value,$op,$self,$event,$del,$p) = @_;
		$$del = sprintf("%7.2f", $value/(($p = $self->{pegs}->{$event})?$p:1));
		return $value;
	},$self,$event,\$del]);
	$tw->traceVariable(\$self->{pegs}->{$event},'w'=>[sub{
		my ($index,$value,$op,$self,$event,$del,$p) = @_;
		$$del = sprintf("%7.2f", $self->{durs}->{$event}/(($p = $value)?$p:1));
		return $value;
	},$self,$event,\$del]);
	$w = $tw->Entry(%entryright,
		-textvariable=>\$del,
	)->grid(-row=>$$row,-column=>4,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label pegs.\
This is the current number of circuits that are in the\
$label state.";
	$w = $tw->Entry(%entryright,
		-textvariable=>\$self->{sims}->{$event}->{curr},
	)->grid(-row=>$$row,-column=>5,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label high water mark.\
This is the high water mark for the number of circuits\
in the $label state for the study period.";
	$w = $tw->Entry(%entryright,
		-textvariable=>\$self->{sims}->{$event}->{hiwa},
	)->grid(-row=>$$row,-column=>6,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$bmsg =
"$prefix $label low water mark.\
This is the low (non-zero) water mark for the number of\
circuits in the $label state for the study period.";
	$w = $tw->Entry(%entryright,
		-textvariable=>\$self->{sims}->{$event}->{lowa},
	)->grid(-row=>$$row,-column=>7,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	my $X = $tw->toplevel->rootx;
	my $Y = $tw->toplevel->rooty;
	$bmsg =
"$prefix $label plots.\
Press this button (using left or right mouse button) to\
generate plots of the study period for the $label state.";
	$w = $tw->Button(
		-command=>[\&MsgStats::plotmenu,$self,$tw,$event,$prefix,$label,$X,$Y],
		-text=>'Plot',
		-pady=>0,
		-pady=>0,
	)->grid(-row=>$$row,-column=>8,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg);
	$w->bind('<ButtonPress-3>',[sub {
		my ($w,@args) = @_;
		MsgStats::plotmenu(@args);
	},$self,$tw,$event,$prefix,$label,Tk::Ev('X'),Tk::Ev('Y')]);
	$$row++;
}

#package MsgStats;
sub addcstat {
	my ($self,$top,$tw,$balloon,$row,$span,$event,$label) = @_;
	for (my $dir = 0; $dir <= 20; $dir += 10) {
		my ($prefix,$w,$bmsg);
		if ($dir > 10) {
			$prefix = 'Total';
		} else {
			if ($self->{way} eq 'O') {
				$prefix = ($dir == 0) ? 'O/G' : 'I/C';
			} elsif ($self->{way} eq 'I') {
				$prefix = ($dir == 0) ? 'I/C' : 'O/G';
			} else {
				$prefix = ($dir == 0) ? 'Forward' : 'Reverse';
			}
		}
		$bmsg = "$label counts.";
		if ($dir == 20) {
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
		$self->dircstat($top,$tw,$balloon,$row,$span,$event+$dir,$prefix,$label);
	}
}

sub cctstatnew {
	my ($self,$top,$tw,$balloon)= @_;
	my ($w,$bmsg);
	my $span;
	if (ref $self eq 'Counts') {
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
#		-browsecommand=>[\&MsgStats::browsecommand,$self,$m],
#		-command=>[\&MsgStats::accesscommand,$self],
#		-selectioncommand=>[\&MsgStats::selectioncommand,$self],
#		-coltagcommand=>[\&MsgStats::coltagcommand,$self],
#		-rowtagcommand=>[\&MsgStats::rowtagcommand,$self],
#		-validatecommand=>[\&MsgStats::validatecommand,$self],
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
	if (ref $self eq 'Counts') {
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
		-textvariable=>\$self->{actcnt},
	)->grid(-row=>$row,-column=>3,-sticky=>'ewns');
	$balloon->attach($w,-balloonmsg=>$bmsg,);
	if (ref $self eq 'SSP' or ref $self eq 'SP') {
		$bmsg = "Number of active outgoing circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active O/G circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actog},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "Number of active incoming circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active I/C circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actic},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "Number of active bothway circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active 2/W circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{act2w},
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
			-textvariable=>\$self->{actforw},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "number of active circuits in the reverse direction.";
		$w = $f->Label(%labelright,
			-text=>'Active reverse circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actrevs},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "number of active circuits in both directions.";
		$w = $f->Label(%labelright,
			-text=>'Active bothway circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{actboth},
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
			-textvariable=>\$self->{act1w},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$bmsg = "number of active bothway circuits.";
		$w = $f->Label(%labelright,
			-text=>'Active bothway circuits:',
		)->grid(-row=>$row,-column=>4,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
		$w = $f->Entry(%entryright,
			-textvariable=>\$self->{act2w},
		)->grid(-row=>$row++,-column=>5,-sticky=>'ewns');
		$balloon->attach($w,-balloonmsg=>$bmsg,);
	}
	if ($self->{actcnt}) {
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
		$self->addcstat($top,$f,$balloon,\$row,$span,CTS_UNINIT,'Unused');
		$self->addcstat($top,$f,$balloon,\$row,$span,CTS_IDLE,'Idle');
		$self->addcstat($top,$f,$balloon,\$row,$span,CTS_WAIT_ACM,'Setup');
		$self->addcstat($top,$f,$balloon,\$row,$span,CTS_WAIT_ANM,'Alerting');
		$self->addcstat($top,$f,$balloon,\$row,$span,CTS_ANSWERED,'Connected');
		$self->addcstat($top,$f,$balloon,\$row,$span,CTS_SUSPENDED,'Suspended');
		$self->addcstat($top,$f,$balloon,\$row,$span,CTS_WAIT_RLC,'Releasing');
	}
}

#package MsgStats;
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
			while (my ($k,$v) = each %{$self->{plots}}) {
				while (my ($l,$m) = each %{$v}) {
					if ($m) {
						delete $v->{$l};
						$m->{tw}->destroy;
						delete $m->{tw};
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
package MsgCollector;
use strict;
# -------------------------------------

use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
#package MsgCollector;
sub init {
	my ($self,@args) = @_;
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
			print STDERR "f=$f, t=$t, l=$l\n";
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
# -------------------------------------

sub init {
	my ($self,@args) = @_;
	$self->{calls} = [];
	$self->{call} = undef;
}

sub pushcall {
	my ($self,$call) = @_;
	push @{$self->{calls}}, $call;
	$self->{call} = undef;
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

# -------------------------------------
package Clickable;
use strict;
# -------------------------------------

#package Clickable;
sub init {
	my ($self) = @_;
	$self->{items} = [];
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
	$top->statusbar->configure(-text=>"Discovered ".$self->identify);
	$top->{updatenow} = 1;
}

#package Clickable;
sub getmenu {
	my ($self,$m,$top,$X,$Y) = @_;
	my $ref = ref $self;
	my $len = length($ref) + 1;
	if (exists $self->{props}) {
		$m->add('command',
			#-accelerator=>'p',
			-command=>[\&Properties::props, $self, $top, $X, $Y],
			-label=>"$ref Properties...",
			-underline=>$len,
		);
	}
	if (exists $self->{statu}) {
		$m->add('command',
			#-accelerator=>'s',
			-command=>[\&Status::status, $self, $top, $X, $Y],
			-label=>"$ref Status...",
			-underline=>$len,
		);
	}
	if (exists $self->{logs} and scalar(@{$self->{logs}}) > 0) {
		$m->add('command',
			#-accelerator=>'l',
			-command=>[\&Logging::showlog, $self, $top, $X, $Y],
			-label=>"$ref Logs...",
			-underline=>$len,
		);
	}
	if (exists $self->{incs} and $self->{incs}->{0}->{sus} + $self->{incs}->{1}->{sus} > 0) {
		$m->add('command',
			#-accelerator=>'m',
			-command=>[\&MsgStats::stats, $self, $top, $X, $Y],
			-label=>'Message Statistics...',
			-underline=>0,
		);
	}
	if (exists $self->{ciccnt} and $self->{ciccnt} > 0) {
		$m->add('command',
			#-accelerator=>'c',
			-command=>[\&MsgStats::cstat, $self, $top, $X, $Y],
			-label=>'Call statistics...',
			-underline=>0,
		);
	}
	if (exists $self->{msgs} and scalar(@{$self->{msgs}}) > 0) {
		$m->add('command',
			#-accelerator=>'e',
			-command=>[\&MsgCollector::msgs, $self, $top, $X, $Y],
			-label=>'Messages...',
			-underline=>1,
		);
	}
	if (exists $self->{calls} and scalar(@{$self->{calls}}) > 0) {
		my $n = scalar @{$self->{calls}};
		$m->add('command',
			#-accelerator=>'a',
			-command=>[\&CallCollector::calls, $self, $top, $X, $Y],
			-label=>'Calls...',
			-underline=>1,
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
	$self->{statetext} = $options->[$state]->[0];
	$self->{color} = $colors->[$state];
	my $tl = $top->widget;
	$top->widget->traceVariable(\$self->{state},'w'=>[\&Stateful::tracestate,$self,$top]);
}
#package Stateful;
sub tracestate {
	my ($ind,$val,$op,$self,$top) = @_;
	if ($op eq 'w') {
		if ($self->{oldstate} != $val) {
			$self->{statetext} = $self->{options}->[$val]->[0];
			my $color = $self->{colors}->[$val];
			if ($self->{color} ne $color) {
				$self->{color} = $color;
				$top->canvas->itemconfigure($self->{items}->[0], -fill=>$color)
					if exists $self->{items} and $self->{items}->[0];
			}
			$top->statusbar->configure(
				-text=>$self->identify.' state changed from '.$self->{options}->[$self->{oldstate}]->[0].' to '.$self->{options}->[$val]->[0],
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
sub fillstate {
	my ($self,$top,$w,$row) = @_;
	$w->Label(%labelright,
		-text=>'State:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$w->Optionmenu(%optionleft,
		-options=>$self->{options},
		-variable=>\$self->{state},
		-textvariable=>\$self->{statetext},
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
	my ($self,$top,$obja,$objb,$under,$format) = @_;
	$format = 'line' unless defined $format;
	my $type = ref $self;
	my $show = "\L$type\Es";
	my $tags = "\L$type\E";
	my $c = $top->canvas;
	$self->{obja} = $obja;
	$self->{objb} = $objb;
	$obja->{arcs} = [] unless exists $obja->{arcs}; push @{$obja->{arcs}}, $self;
	$objb->{arcs} = [] unless exists $objb->{arcs}; push @{$objb->{arcs}}, $self;
	my $xa = $self->{xa} = $obja->{x};
	my $ya = $self->{ya} = $obja->{y};
	my $xb = $self->{xb} = $objb->{x};
	my $yb = $self->{yb} = $objb->{y};
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	$self->{item} = $c->createLine($xa,$ya,$xb,$yb,%{$Arcitem::style->{$format}},
		-fill=>$self->{color}, -width=>0.1,
		-activefill=>'cyan', -activewidth=>2,
		-state=>$top->{show}->{$show}?'normal':'hidden',
		-tags=>[$tags],
	);
	push @{$self->{items}}, $self->{item};
	$c->lower($self->{item},'node');
	$c->lower($self->{item},$under);
}

#package Arcitem;
sub moveit {
	my ($self,$top) = @_;
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
	$top->canvas->coords($self->{item},$xa,$ya,$xb,$yb);
	$self->{xa} = $xa;
	$self->{ya} = $ya;
	$self->{xb} = $xb;
	$self->{yb} = $yb;
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	if (exists $self->{arcs}) { foreach my $a (@{$self->{arcs}}) { $a->moveit($top); } }
	return 1;
}

# -------------------------------------
package Relation;
use strict;
use vars qw(@ISA);
@ISA = qw(Hexstate Arcitem MsgStats Logging Properties Status Clickable CallCollector MsgCollector);
# -------------------------------------
# A relation is an association between signalling points that communicate with
# each other.  This object is used to track these interactions, primarily for
# identifying the role of nodes.
# -------------------------------------

#package Relation;
sub init {
	my ($self,$top,$network,$relationno,$nodea,$nodeb,@args) = @_;
	$self->{network} = $network;
	$self->Hexstate::init($top,'black');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->CallCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->{key} = "$nodea->{pc},$nodeb->{pc}";
	$self->{cics} = {};
	$self->{ciccnt} = 0;
	$self->{actcnt} = 0;
	$self->{actforw} = 0;
	$self->{actrevs} = 0;
	$self->{actboth} = 0;
	$self->{slccnt} = 0;
	$self->{nodea} = $nodea;
	$self->{nodeb} = $nodeb;
	$nodea->{relate}->{$nodeb->{pc}} = $self;
	$nodeb->{relate}->{$nodea->{pc}} = $self;
	$self->Arcitem::init($top,$nodea,$nodeb);
	$self->Clickable::attach($top,@args);


}

#package Relation;
#sub DESTROY {
#	my $self = shift;
#	my $c = $self->canvas;
#	$c->toplevel->traceVdelete(\$self->{state});
#}

#package Relation;
sub statechange {
	my ($self,$top,$val) = @_;
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
sub itv {
	my ($self,@args) = @_;
	$self->Stats::itv(@args);
	$self->{nodea}->itv(@args);
	$self->{nodeb}->itv(@args);
	$self->{network}->itv(@args);
}

#package Relation;
sub dur {
	my ($self,@args) = @_;
	$self->Stats::dur(@args);
	$self->{nodea}->dur(@args);
	$self->{nodeb}->dur(@args);
	$self->{network}->dur(@args);
}

#package Relation;
sub iat {
	my ($self,@args) = @_;
	$self->Stats::iat(@args);
	$self->{nodea}->iat(@args);
	$self->{nodeb}->iat(@args);
	$self->{network}->iat(@args);
}

#package Relation;
sub peg {
	my ($self,@args) = @_;
	$self->Stats::peg(@args);
	$self->{nodea}->peg(@args);
	$self->{nodeb}->peg(@args);
	$self->{network}->peg(@args);
}

#package Relation;
sub act {
	my ($self,@args) = @_;
	$self->Stats::act(@args);
	$self->{nodea}->act(@args);
	$self->{nodeb}->act(@args);
	$self->{network}->act(@args);
}

#package Relation;
sub cnt {
	my ($self,@args) = @_;
	$self->Stats::cnt(@args);
	$self->{nodea}->cnt(@args);
	$self->{nodeb}->cnt(@args);
	$self->{network}->cnt(@args);
}

#package Relation;
sub sim {
	my ($self,@args) = @_;
	$self->Stats::sim(@args);
	$self->{nodea}->sim(@args);
	$self->{nodeb}->sim(@args);
	$self->{network}->sim(@args);
}

#package Relation;
sub pushcall {
	my ($self,$call) = @_;
	$self->CallCollector::pushcall($call);
	$self->{nodea}->pushcall($call);
	$self->{nodeb}->pushcall($call);
	$self->{network}->pushcall($call);
}

#package Relation;
sub identify {
	my $self = shift;
	my $id = $self->{ciccnt} ? 'Circuits ' : 'Relation ';
	$id .= $self->{nodea}->shortid;
	$id .= ', ';
	$id .= $self->{nodeb}->shortid;
	return ($self->{id} = $id);
}

#package Relation;
sub shortid {
	my $self = shift;
	return "$self->{nodea}->{pcode}::$self->{nodeb}->{pcode}";
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
			my $circuit = Circuit->get($top,$network,$self,$cic);
			$circuit->add_msg($top,$network,$msg,$dir);
		}
	}
	if (defined $dir) {
		if ($dir)   { $self->add_revs($top,$network,$msg); }
		else	    { $self->add_forw($top,$network,$msg); }
		$self->inc($msg,$dir);
		$self->pushmsg($msg);
		return;
	}
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
	print STDERR "error key=$self->{key}, opc=$msg->{opc}, dpc=$msg->{dpc}\n";
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
sub moveit {
	my ($self,$top,$ret) = @_;
	if ($ret = $self->Arcitem::moveit($top)) {
		$self->{linkset}->moveit($top) if exists $self->{linkset};
	}
	return $ret;
}

#package Relation;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $have = {};
	my $have->{linkset} = $self->{linkset} ? 1 : 0;
	my $have->{links} = scalar keys %{$self->{links}};
	$m->add('separator'); # if $have->{linkset} + $have->{links};
	{
		my $label = $self->{nodea}->shortid;
		my $mc = $m->Menu(-title=>"$label Menu");
		$self->{nodea}->getmenu($mc,$top,$X,$Y);
		$m->add('cascade',-menu=>$mc,-label=>$label);
	}
	{
		my $label = $self->{nodeb}->shortid;
		my $mc = $m->Menu(-title=>"$label Menu");
		$self->{nodeb}->getmenu($mc,$top,$X,$Y);
		$m->add('cascade',-menu=>$mc,-label=>$label);
	}
	if ($have->{linkset}) {
		my $linkset = $self->{linkset};
		unless ($Linkset::gotmenu) {
			my $mc = $m->Menu(-title=>'Linkset Menu');
			$linkset->getmenu($mc,$top,$X,$Y);
			$m->add('cascade',-menu=>$mc,-label=>'Linkset');
		}

	}
	if ($have->{links}) {
		my ($mc,$m3);
		$mc = $m->Menu(-title=>'Links Menu');
		foreach my $slc (sort {$a <=> $b} keys %{$self->{links}}) {
			my $link = $self->{links}->{$slc};
			$m3 = $mc->Menu(-title=>"Link $slc Menu");
			$link->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade', -menu=>$m3, -label=>"Link $slc");
		}
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
		-text=>$self->{nodea}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-text=>$self->{nodeb}->identify, -width=>32,
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
		if ($self->{actcnt}) {
			$tw->Label(%labelright,
				-text=>'Active circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actcnt},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actforw}) {
			$tw->Label(%labelright,
				-text=>'Active forward circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actforw},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actrevs}) {
			$tw->Label(%labelright,
				-text=>'Active reverse circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actrevs},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actboth}) {
			$tw->Label(%labelright,
				-text=>'Active bothway circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actboth},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
	}
}

#package Relation;
sub log {
	my ($self,$text) = @_;
	$self->Logging::log($text);
	$self->{nodea}->log($text);
	$self->{nodeb}->log($text);
}

# -------------------------------------
package Routeset;
use strict;
use vars qw(@ISA);
@ISA = qw(Hexstate Arcitem MsgStats Logging Properties Status Clickable MsgCollector);
# -------------------------------------
# A routeset is a collection of routes from one node (node a) to another (node b).
# In contrast to relations, a routeset is a unidirectional concept.
# -------------------------------------

#package Routeset;
sub init {
	my ($self,$top,$network,$routesetno,$nodea,$nodeb,@args) = @_;
	$self->Hexstate::init($top,'black');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init(@args);
	$self->{key} = "$nodea->{pc},$nodeb->{pc}";
	$self->{nodea} = $nodea;
	$self->{nodeb} = $nodeb;
	$nodea->{routesets}->{og}->{$nodeb->{pc}} = $self;
	$nodeb->{routesets}->{ic}->{$nodea->{pc}} = $self;
	$self->Arcitem::init($top,$nodea,$nodeb,'relation');
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

# -------------------------------------
package Call;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgCollector);
# -------------------------------------

#package Call;
sub init {
	my ($self,$circuit,@args) = @_;
	$self->{circuit} = $circuit;
	$self->{state} = 0;
	if ($circuit->{call}) {
		$circuit->pushcall($circuit->{call});
	}
	$circuit->{call} = $self;
}

#package Call;
sub new {
	my ($type,$circuit,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->MsgCollector::init(@args);
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
@ISA = qw(MsgStats Logging Properties Status Clickable CallCollector MsgCollector);
# -------------------------------------

use constant {
	CTS_UNINIT	=> 0,
	CTS_IDLE	=> 1,
	CTS_WAIT_ACM	=> 2,
	CTS_WAIT_ANM	=> 3,
	CTS_ANSWERED	=> 4,
	CTS_SUSPENDED	=> 5,
	CTS_WAIT_RLC	=> 6,
	CTS_SEND_RLC	=> 7,
	CTS_COMPLETE	=> 8,
};

#package Circuit;
sub get {
	my ($type,$top,$network,$group,$cic,@args) = @_;
	return $group->{cics}->{$cic} if exists $group->{cics}->{$cic};
	my $self = {};
	bless $self,$type;
	$self->{group} = $group;
	$self->{cic} = $cic;
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->CallCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->{dir} = 0;
	$group->{cics}->{$cic} = $self;
	$self->cnt($top);
	$self->{active} = 0;
	$self->{state} = CTS_UNINIT;
	return $self;
}

#package Circuit
sub setstate {
	my ($self,$top,$msg,$newstate,$newdir) = @_;
	return if $self->{state} == $newstate;
	my $oldstate = $self->{state};
	my $olddir   = $self->{dir};
	if ($oldstate == CTS_UNINIT) {
		$self->act;
		$olddir = $newdir;
		$self->peg($oldstate + $olddir, $msg->{hdr});
		$self->peg($oldstate + 20, $msg->{hdr});
		$self->{ts}->{$oldstate + $olddir} = $top->{begtime};
	}
	$self->{ts}->{$newstate + $newdir} = $msg->{hdr};
	$self->sim($oldstate + $olddir, $newstate + $newdir);
	$self->sim($oldstate + 20     , $newstate + 20     );
	$self->itv($oldstate + $olddir, $self->{ts}->{$oldstate + $olddir}, $msg->{hdr});
	$self->itv($oldstate + 20, $self->{ts}->{$oldstate + $olddir}, $msg->{hdr});
	$self->peg($newstate + $newdir, $msg->{hdr});
	$self->peg($newstate + 20, $msg->{hdr});
	delete $self->{ts}->{$oldstate + $olddir};
	$self->{state} = $newstate;
	$self->{dir} = $newdir;
}

#package Circuit
sub itv {
	my ($self,@args) = @_;
	$self->Stats::itv(@args);
	$self->{group}->itv(@args);
}

#package Circuit
sub dur {
	my ($self,@args) = @_;
	$self->Stats::dur(@args);
	$self->{group}->dur(@args);
}

#package Circuit
sub iat {
	my ($self,@args) = @_;
	$self->Stats::iat(@args);
	$self->{group}->iat(@args);
}

#package Circuit;
sub peg {
	my ($self,@args) = @_;
	$self->Stats::peg(@args);
	$self->{group}->peg(@args);
}

#package Circuit;
sub act {
	my ($self,@args) = @_;
	$self->Stats::act(@args);
	$self->{group}->act(@args);
}

#package Circuit;
sub cnt {
	my ($self,@args) = @_;
	$self->Stats::cnt(@args);
	$self->{group}->cnt(@args);
}

#package Circuit;
sub sim {
	my ($self,@args) = @_;
	$self->Stats::sim(@args);
	$self->{group}->sim(@args);
}

#package Circuit;
sub pushcall {
	my ($self,$call) = @_;
	$self->CallCollector::pushcall($call);
	$self->{group}->pushcall($call);
}

#package Circuit;
sub end_of_call {
	my ($self,$top,$call,$msg,$dir) = @_;
	$self->setstate($top,$msg,CTS_IDLE,$dir);
	$call->add_msg($msg,CTS_IDLE);
	$self->pushcall($call);
}

#package Circuit;
sub clear_call {
	my ($self,$top,$call,$msg,$dir) = @_;
	$self->setstate($top,$msg,CTS_IDLE,$dir);
	$call->clear($msg,CTS_IDLE);
}

#package Circuit;
sub restart_call {
	my ($self,$top,$call,$msg,$dir) = @_;
	$self->setstate($top,$msg,CTS_IDLE,$dir);
	if ($self->msgcnt) {
		$self->pushcall($call);
	} else {
		$call->clear($msg,CTS_IDLE);
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
			$call = $self->{call} = Call->new($self);
		}
		if ($mt == 0x01) { # iam
			if ($self->{state} <= CTS_WAIT_ACM) {
				my $group = $self->{group};
				my $nodea = $group->{nodea};
				my $nodeb = $group->{nodeb};
				if ($msg->{opc} == $nodea->{pc}) {
					$dir = 0;
					if (!($self->{active} & 0x1)) {
						$self->{active} |= 0x1;
						if ($self->{active} & 0x2) {
							$group->{actboth} += 1;
							$nodea->{act2w} += 1;
							$nodeb->{act2w} += 1;
							$network->{act2w} += 1;
							$group->{actrevs} -= 1;
							$nodea->{actic} -= 1;
							$nodeb->{actog} -= 1;
							$network->{act1w} -= 1;
						} else {
							$group->{actforw} += 1;
							$nodea->{actog} += 1;
							$nodeb->{actic} += 1;
							$network->{act1w} += 1;
						}
					}
				} else {
					$dir = 10;
					if (!($self->{active} & 0x2)) {
						$self->{active} |= 0x2;
						if ($self->{active} & 0x1) {
							$group->{actboth} += 1;
							$nodea->{act2w} += 1;
							$nodeb->{act2w} += 1;
							$network->{act2w} += 1;
							$group->{actforw} -= 1;
							$nodea->{actog} -= 1;
							$nodeb->{actic} -= 1;
							$network->{act1w} -= 1;
						} else {
							$group->{actrevs} += 1;
							$nodea->{actic} += 1;
							$nodeb->{actog} += 1;
							$network->{act1w} += 1;
						}
					}
				}
				$self->setstate($top,$msg,CTS_WAIT_ACM,$dir);
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
			if ($self->{state} == CTS_WAIT_ACM) {
				$self->setstate($top,$msg,CTS_WAIT_ANM,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x07) { # con
			if ($self->{state} == CTS_WAIT_ANM) {
				$self->setstate($top,$msg,CTS_ANSWERED,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x08) { # fot
			last;
		}
		if ($mt == 0x09) { # anm
			if ($self->{state} == CTS_WAIT_ANM) {
				$self->setstate($top,$msg,CTS_ANSWERED,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x0c) { # rel
			if ($self->{state} > CTS_IDLE) {
				$self->setstate($top,$msg,CTS_WAIT_RLC,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x0d) { # sus
			if ($self->{state} == CTS_ANSWERED || $self->{state} == CTS_SUSPENDED) {
				$self->setstate($top,$msg,CTS_SUSPENDED,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x0e) { # res
			if ($self->{state} == CTS_ANSWERED || $self->{state} == CTS_SUSPENDED) {
				$self->setstate($top,$msg,CTS_ANSWERED,$dir);
				last;
			}
			return;
		}
		if ($mt == 0x10) { # rlc
			if ($self->{state} == CTS_WAIT_RLC) {
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
			if ($self->{state} > CTS_WAIT_ANM) {
				$self->end_of_call($top,$call,$msg,$dir);
				return;
			}
			$self->clear_call($top,$call,$msg,$dir);
			return;
		}
		if ($mt == 0x13) { # blo
			if ($self->{state} < CTS_WAIT_ANM) {
				$self->clear_call($top,$call,$msg,$dir);
				return;
			}
			return;
		}
		if ($mt == 0x14) { # ubl
			return;
		}
		if ($mt == 0x15) { # bla
			if ($self->{state} < CTS_WAIT_ANM) {
				$self->clear_call($top,$call,$msg,$dir);
				return;
			}
			return;
		}
		if ($mt == 0x16) { # uba
			return;
		}
		if ($mt == 0x17) { # grs
			if ($self->{state} > CTS_WAIT_ANM) {
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
			if ($self->{state} < CTS_WAIT_ANM) {
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
			if ($self->{state} > CTS_WAIT_ANM) {
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
			if ($self->{state} == CTS_WAIT_ACM || $self->{state} == CTS_WAIT_ANM) {
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
			if ($self->{state} < CTS_ANSWERED) {
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
			if ($self->{state} == CTS_WAIT_ACM) {
				$self->setstate($top,$msg,CTS_WAIT_ACM,$dir);
				last;
			}
			return;
		}
		if ($mt == 0xea) { # crm
			if ($self->{state} != CTS_IDLE && $self->{state} != CTS_WAIT_ACM) {
				$self->end_of_call($top,$call,$msg,$dir);
				return;
			}
			$self->setstate($top,$msg,CTS_WAIT_ACM,$dir);
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
	$id .= $group->{nodea}->shortid;
	$id .= ', ';
	$id .= $group->{nodeb}->shortid;
	return ($self->{id} = $id);
}

#package Circuit;
sub shortid {
	my $self = shift;
	my $group = $self->{group};
	return "$group->{nodea}->{pcode},$group->{nodeb}->{pcode}:$self->{cic}";
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
		-text=>$self->{group}->{nodea}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entrycenter,
		-text=>$self->{group}->{nodeb}->identify, -width=>32,
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
@ISA = qw(Tristate Arcitem MsgStats Logging Properties Status Clickable CallCollector MsgCollector);
# -------------------------------------

use constant {
	COL_NOD => 5,
	COL_SSP => 4,
	COL_SCP => 3,
	COL_GTT => 2,
	COL_ADJ => 1,
};
#package Linkset;
sub init {
	my ($self,$top,$network,$relation,@args) = @_;
	$self->{network} = $network;
	$self->Tristate::init($top,'black');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->CallCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->{slccnt} = 0;
	$self->{links} = {};
	my $nodea = $self->{nodea} = $relation->{nodea};
	my $nodeb = $self->{nodeb} = $relation->{nodeb};
	$self->{relation} = $relation; $relation->{linkset} = $self;
	$self->{routesa} = {}; # routes from the a-side
	$self->{routesb} = {}; # routes from the b-side
	$self->Arcitem::init($top,$nodea,$nodeb,'node');
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
	my $letter = $Linkset::letters[$self->checktype];
	my $id = "$letter-Linkset ";
	$id .= $self->{nodea}->shortid;
	$id .= ', ';
	$id .= $self->{nodeb}->shortid;
	return ($self->{id} = $id);
}

#package Linkset;
sub shortid {
	my $self = shift;
	return "$self->{nodea}->{pcode}:$self->{nodeb}->{pcode}";
}

#package Linkset;
sub statechange {
	my ($self,$top,$val) = @_;
	my $c = $top->canvas;
	foreach my $route (values %{$self->{routesa}}) {
		$route->updatestate($top,$self);
	}
	foreach my $route (values %{$self->{routesb}}) {
		$route->updatestate($top,$self);
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
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	$self->inc($msg,$dir);
	$self->pushmsg($msg);
	my ($pc,$side,$node,$route);
	if ($dir) {
		$side = 'b'; $pc = $msg->{opc};
		$node = $network->getSp($top,$pc,$msg->{channel},$self->{nodeb}->{col},'O');
		$route = $self->getRoute($top,$side,$node);
		$route->add_msg($top,$network,$msg,$dir);
		$side = 'a'; $pc = $msg->{dpc};
		$node = $network->getSp($top,$pc,$msg->{channel},$self->{nodea}->{col},'I');
		$route = $self->getRoute($top,$side,$node);
		$route->add_msg($top,$network,$msg,$dir);
	} else {
		$side = 'a'; $pc = $msg->{opc};
		$node = $network->getSp($top,$pc,$msg->{channel},$self->{nodea}->{col},'I');
		$route = $self->getRoute($top,$side,$node);
		$route->add_msg($top,$network,$msg,$dir);
		$side = 'b'; $pc = $msg->{dpc};
		$node = $network->getSp($top,$pc,$msg->{channel},$self->{nodeb}->{col},'O');
		$route = $self->getRoute($top,$side,$node);
		$route->add_msg($top,$network,$msg,$dir);
	}
}

#package Linkset;
sub reposition {
	my ($self,$top,$node) = @_;
	my $col = COL_ADJ;
	if ($node->{col} < 0) { $col = -$col; }
	$node->movesp($top,$col,$node->{row});
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

@Linkset::letters = ( '?', 'A', 'B', 'C', 'D', 'E', 'F', );
%Linkset::types = (
	SP =>{ SP =>LS_UNKNOWN, SSP=>LS_UNKNOWN, STP=>LS_UNKNOWN, GTT=>LS_UNKNOWN, SCP=>LS_UNKNOWN, },
	SSP=>{ SP =>LS_UNKNOWN, SSP=>LS_FLINK,   STP=>LS_ALINK,   GTT=>LS_ALINK,   SCP=>LS_ELINK,   },
	STP=>{ SP =>LS_UNKNOWN, SSP=>LS_ALINK,   STP=>LS_BLINK,   GTT=>LS_BLINK,   SCP=>LS_ALINK,   },
	GTT=>{ SP =>LS_UNKNOWN, SSP=>LS_ALINK,   STP=>LS_BLINK,   GTT=>LS_BLINK,   SCP=>LS_ALINK,   },
	SCP=>{ SP =>LS_UNKNOWN, SSP=>LS_ELINK,   STP=>LS_ALINK,   GTT=>LS_ALINK,   SCP=>LS_FLINK,   },
);

#package Linkset;
sub checktype {
	my $self = shift;
	my ($refa,$refb) = (ref $self->{nodea},ref $self->{nodeb});
	return ($self->{type} = $Linkset::types{$refa}->{$refb});
}

#package Linkset;
sub getRoute {
	my ($self,$top,$side,$node,@args) = @_;
	my $pc = $node->{pc};
	my $route;
	if ($side eq 'a') {
		return $self->{routesa}->{$pc} if exists $self->{routesa}->{$pc};
		$route = Route->new($top,$self,$side,$node,@args);
		$self->{routesa}->{$pc} = $route;
	} else {
		return $self->{routesb}->{$pc} if exists $self->{routesb}->{$pc};
		$route = Route->new($top,$self,$side,$node,@args);
		$self->{routesb}->{$pc} = $route;
	}
	return $route;
}

#package Linkset;
sub getLink {
	my ($self,$top,$slc,@args) = @_;
	return $self->{links}->{$slc} if exists $self->{links}->{$slc};
	my $link = Link->new($top,$self,$slc,@args);
	$self->{links}->{$slc} = $link;
	return $link;
}

#package Linkset;
sub moveit {
	my ($self,$top,$ret) = @_;
	if ($ret = $self->Arcitem::moveit($top)) {
		foreach my $o (values %{$self->{links}}  ) {
			unless (defined $o) {
				print STDERR "undefined value in links for ",$self->identify,"\n";
				print STDERR Data::Dumper->Dump([$self->{links}]);
				die;
			}
			$o->moveit($top);
		}
		foreach my $o (values %{$self->{routesa}}) { $o->moveit($top); }
		foreach my $o (values %{$self->{routesb}}) { $o->moveit($top); }
	}
	return $ret;
}

#package Linkset;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $havelinks = scalar keys %{$self->{links}};
	my $haverouta = scalar keys %{$self->{routesa}};
	my $haveroutb = scalar keys %{$self->{routesb}};
	$m->add('separator') if $havelinks + $haverouta + $haveroutb;
	if ($havelinks) {
		my ($mc,$m3);
		$mc = $m->Menu(-title=>'Links Menu');
		foreach my $slc (sort {$a <=> $b} keys %{$self->{links}}) {
			my $link = $self->{links}->{$slc};
			$m3 = $mc->Menu(-title=>"Link $slc Menu");
			$link->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>"Link $slc");
		}
		$m->add('cascade',-menu=>$mc,-label=>'Links');
	}
	if ($haveroutb) {
		my ($mc,$m3);
		$mc = $m->Menu(-title=>'Routes Forward Menu');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{routesb}}) {
			my $route = $self->{routesb}->{$pc};
			$m3 = $mc->Menu(-title=>"Route $route->{node}->{pcode} Menu");
			$route->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>"Route $route->{node}->{pcode}");

		}
		$m->add('cascade',-menu=>$mc,-label=>'Routes Forward');
	}
	if ($haverouta) {
		my ($mc,$m3);
		$mc = $m->Menu(-title=>'Routes Reverse Menu');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{routesa}}) {
			my $route = $self->{routesa}->{$pc};
			$m3 = $mc->Menu(-title=>"Route $route->{node}->{pcode} Menu");
			$route->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>"Route $route->{node}->{pcode}");

		}
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
		-text=>$self->{nodea}->identify, -width=>32,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'wns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entrycenter,
		-text=>$self->{nodeb}->identify, -width=>32,
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
	if ($self->{ciccnt}) {
		$tw->Label(%labelright,
			-text=>'Defined circuits:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryright,
			-textvariable=>\$self->{ciccnt},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		if ($self->{actcnt}) {
			$tw->Label(%labelright,
				-text=>'Active circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actcnt},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actforw}) {
			$tw->Label(%labelright,
				-text=>'Active forward circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actforw},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actrevs}) {
			$tw->Label(%labelright,
				-text=>'Active reverse circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actrevs},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
		if ($self->{actboth}) {
			$tw->Label(%labelright,
				-text=>'Active bothway circuits:',
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$tw->Entry(%entryright,
				-textvariable=>\$self->{actboth},
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
	}
}

# -------------------------------------
package Combined;
use strict;
use vars qw(@ISA);
@ISA = qw(Tristate Arcitem MsgStats Logging Properties Status Clickable MsgCollector);
# -------------------------------------

#package Combined;
sub init {
	my ($self,$top,$network,$linkseta,$linksetb,@args) = @_;
	my $type = ref $self;
	$self->Tristate::init($top,'black');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init(@args);
	$self->{lnkcnt} = 0;
	$self->{linkseta} = $linkseta;
	$self->{linksetb} = $linksetb;
	$self->Arcitem::init($top,$linkseta,$linksetb,'node','dash');
	$self->Clickable::attach($top,@args);
}


# -------------------------------------
package Link;
use strict;
use vars qw(@ISA);
@ISA = qw(Tristate Arcitem MsgStats Logging Properties Status Clickable MsgCollector);
# -------------------------------------

#package Link;
sub init {
	my ($self,$top,$linkset,$slc,@args) = @_;
	$self->Tristate::init($top,'black');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init(@args);
	$self->{linkset} = $linkset;
	my $nodea = $self->{nodea} = $linkset->{nodea};
	my $nodeb = $self->{nodeb} = $linkset->{nodeb};
	$self->{slc} = $slc;
	$self->{channelforw} = undef;
	$self->{channelrevs} = undef;
	$self->Arcitem::init($top,$nodea,$nodeb,'linkset');
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
sub statechange {
	my ($self,$top,$val) = @_;
	$self->{linkset}->updatestate($top,$self);
}

#package Link;
sub addChannelForw {
	my ($self,$channel) = @_;
	$self->{channelforw} = $channel;
}

#package Link;
sub addChannelRevs {
	my ($self,$channel) = @_;
	$self->{channelrevs} = $channel;
}

#package Link;
sub addChannel {
	my ($self,$channel) = @_;
	if ($channel->{nodea}->{pc} == $self->{nodea}->{pc}) {
		$self->addChannelForw($channel);
	} else {
		$self->addChannelRevs($channel);
	}
}

#package Link;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	$self->inc($msg,$dir);
	$self->pushmsg($msg);
	$self->{linkset}->add_msg($top,$network,$msg,$dir);
}

#package Link;
sub moveit {
	my ($self,$top,$ret) = @_;
	if ($ret = $self->Arcitem::moveit($top)) {
		if (my $o = $self->{channelforw}) { $o->moveit($top); }
		if (my $o = $self->{channelrevs}) { $o->moveit($top); }
	}
	return $ret;
}

#package Link;
sub identify {
	my $self = shift;
	my $id = "Link $self->{slc} ";
	$id .= $self->{nodea}->shortid;
	$id .= ', ';
	$id .= $self->{nodeb}->shortid;
	return ($self->{id} = $id);
}

#package Link;
sub shortid {
	my $self = shift;
	return "$self->{slc}:$self->{nodea}->{pcode},$self->{nodeb}->{pcode}";
}

#package Link;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $haveforw = $self->{channelforw} ? 1 : 0;
	my $haverevs = $self->{channelrevs} ? 1 : 0;
	$m->add('separator') if $haveforw + $haverevs;
	if ($haveforw) {
		my $channel = $self->{channelforw};
		my $mc = $m->Menu(-title=>'Forward channel menu');
		$channel->getmenu($mc,$top,$X,$Y);
		$m->add('cascade', -menu=>$mc, -label=>'Forward channel');
	}
	if ($haverevs) {
		my $channel = $self->{channelrevs};
		my $mc = $m->Menu(-title=>'Revers channel menu');
		$channel->getmenu($mc,$top,$X,$Y) if $channel;
		$m->add('cascade', -menu=>$mc, -label=>'Reverse channel');
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
@ISA = qw(Tristate Arcitem MsgStats Logging Properties Status Clickable MsgCollector);
# -------------------------------------

#package Route
sub new {
	my ($type,$top,$linkset,$side,$node,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->Tristate::init($top,'black');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init(@args);
	$self->{linkset} = $linkset;
	$self->{side} = $side;
	my ($nodea,$nodeb);
	if ($side eq 'a') {
		$nodea = $self->{nodea} = $linkset->{nodeb};
		$nodeb = $self->{nodeb} = $linkset->{nodea};
	} else {
		$nodea = $self->{nodea} = $linkset->{nodea};
		$nodeb = $self->{nodeb} = $linkset->{nodeb};
	}
	$self->{node} = $node;
	$nodeb->{routes}->{og}->{$node->{pc}} = $self;
	$node->{routes}->{ic}->{$nodeb->{pc}} = $self;
	$self->Arcitem::init($top,$nodeb,$node,'linkset');
	$self->Clickable::attach($top,@args);
	return $self;
}

##package Route;
#sub DESTROY {
#	my $self = shift;
#	my $c = $self->canvas;
#	$c->toplevel->traceVdelete(\$self->{state});
#}

#package Route;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	$self->inc($msg,$dir);
	$self->pushmsg($msg);
}

#package Route;
sub statechange {
	my ($self,$top,$val) = @_;
	$self->{node}->updatestate($top,$self);
}

#package Route;
sub updatestate {
	my ($self,$top,$obj) = @_;
}

#package Route;
sub identify {
	my $self = shift;
	my $id = "Route ";
	$id .= $self->{nodea}->shortid;
	$id .= " <=> ";
	$id .= $self->{nodeb}->shortid;
	$id .= " <=> ";
	$id .= $self->{node}->shortid;
	return ($self->{id} = $id);
}

#package Route;
sub shortid {
	my $self = shift;
	my $id = "($self->{nodea}->{pcode})";
	$id .= '<=>';
	$id .= "($self->{nodeb}->{pcode})";
	$id .= '<=>';
	$id .= "($self->{node}->{pcode})";
	return $id;
}

#package Route;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	$m->add('separator');
	my ($mc,$linkset,$node);

	$linkset = $self->{linkset};
	$mc = $m->Menu(-title=>'Linkset Menu');
	$linkset->getmenu($mc,$top,$X,$Y);
	$m->add('cascade',-menu=>$mc,-label=>'Linkset');

	$node =  $self->{nodeb};
	$mc = $m->Menu(-title=>'Transit Node Menu');
	$node->getmenu($mc,$top,$X,$Y);
	$m->add('cascade',-menu=>$mc,-label=>'Transit Node');

	$node = $self->{node};
	$mc = $m->Menu(-title=>'Destination Node Menu');
	$node->getmenu($mc,$top,$X,$Y);
	$m->add('cascade',-menu=>$mc,-label=>'Destination Node');
}

#package Route;
sub fillprops {
	my ($self,$top,$tw,$row,$id,$node,$w) = @_;

	$tw->Label(%labelright,
		-text=>'Destination node:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$node = $self->{node};
	$id = $node->identify;
	$w = $tw->Entry(%entryleft,
		-text=>$id, -width=>length($id),
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$node->bindtowidget($top,$w);
	$tw->Label(%labelright,
		-text=>'Egress node:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$node = $self->{nodeb};
	$id = $node->identify;
	$w = $tw->Entry(%entryleft,
		-text=>$id, -width=>length($id),
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$node->bindtowidget($top,$w);
	$tw->Label(%labelright,
		-text=>'Ingress node:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$node = $self->{nodea};
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
@ISA = qw(Tristate MsgStats Logging Properties Status Clickable MsgCollector);
# -------------------------------------

#package Path;
sub new {
	my ($type,$top,$channel,$side,$node,@args) = @_;
	my $self = {};
	bless $self, $type;
	$self->Tristate::init($top,'grey');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init(@args);
	$self->{channel} = $channel;
	$self->{side} = $side;
	$self->{node} = $node;
	$node->{paths}->{$channel->{ppa}} = $self;
	$channel->{paths}->{$node->{pc}} = $self;
	my ($offa,$cola,$rowa,$offb,$colb,$rowb);
	if ($side eq 'a') {
		$offa = $self->{offa} = $node->{off};
		$cola = $self->{cola} = $node->{col};
		$rowa = $self->{rowa} = $node->{row};
		$offb = $self->{offb} = $channel->{offa};
		$colb = $self->{colb} = $channel->{cola};
		$rowb = $self->{rowb} = $channel->{rowa};
	} else {
		$offa = $self->{offa} = $channel->{offb};
		$cola = $self->{cola} = $channel->{colb};
		$rowa = $self->{rowa} = $channel->{rowb};
		$offb = $self->{offb} = $node->{off};
		$colb = $self->{colb} = $node->{col};
		$rowb = $self->{rowb} = $node->{row};
	}
	my $mc = $top->mycanvas;
	my $xa = $self->{xa} = $mc->colpos($cola+$offa);
	my $ya = $self->{ya} = $mc->rowpos($rowa);
	my $xb = $self->{xb} = $mc->colpos($colb+$offb);
	my $yb = $self->{yb} = $mc->rowpos($rowb);
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	my $c = $top->canvas;
	$self->{item} = $c->createLine($xa,$ya,$xb,$yb,
		-arrow=>'last', -capstyle=>'round', -joinstyle=>'round', -smooth=>0,
		-fill=>$self->{color}, -width=>0.1,
		-activefill=>'cyan', -activewidth=>2,
		-state=>$top->{show}->{"\L$type\Es"} ? 'normal' : 'hidden',
		-tags=>['path'],
	);
	push @{$self->{items}}, $self->{item};
	$c->lower($self->{item},'node');
	$c->lower($self->{item},'channel');
	$c->lower($self->{item},'linkset');
	$self->Clickable::attach($top,@args);
	return $self;
}

##package Path;
#sub DESTROY {
#	my $self = shift;
#	my $c = $self->canvas;
#	$c->toplevel->traceVdelete(\$self->{state});
#}

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
	$id .= "($self->{channel}->{card}:$self->{channel}->{span}:$self->{channel}->{slot})";
	if ($self->{side} eq 'a') {
		$id .= " <- ";
	} else {
		$id .= " -> ";
	}
	$id .= $self->{node}->shortid;
	return ($self->{id} = $id);
}

#package Path;
sub shortid {
	my $self = shift;
	my $id = "($self->{channel}->{card}:$self->{channel}->{span}:$self->{channel}->{slot})";
	if ($self->{side} eq 'a') {
		$id .= "<-";
	} else {
		$id .= "->";
	}
	$id .= "($self->{node}->{pcode})";
	return $id;
}

#package Path;
sub moveit {
	my ($self,$top) = @_;
	my $node = $self->{node};
	my $channel = $self->{channel};
	my $side = $self->{side};
	my ($xa,$ya,$xb,$yb);
	if ($side eq 'a') {
		$xa = $node->{x};
		$ya = $node->{y};
		$xb = $channel->{xa};
		$yb = $channel->{ya};
	} else {
		$xa = $channel->{xb};
		$ya = $channel->{yb};
		$xb = $node->{x};
		$yb = $node->{y};
	}
	return if $xa == $self->{xa} &&
		  $ya == $self->{ya} &&
		  $xb == $self->{xb} &&
		  $yb == $self->{yb};
	if ($side eq 'a') {
		$self->{cola} = $node->{col};
		$self->{rowa} = $node->{row};
		$self->{colb} = $channel->{cola};
		$self->{rowb} = $channel->{rowa};
	} else {
		$self->{cola} = $channel->{colb};
		$self->{rowa} = $channel->{rowb};
		$self->{colb} = $node->{col};
		$self->{rowb} = $node->{row};
	}
	my $c = $top->canvas;
	$c->coords($self->{item},$xa,$ya,$xb,$yb);
	$self->{xa} = $xa;
	$self->{ya} = $ya;
	$self->{xb} = $xb;
	$self->{yb} = $yb;
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
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
package SP;
use strict;
use vars qw(@ISA);
@ISA = qw(Tristate MsgStats Logging Properties Status Clickable CallCollector MsgCollector);
# -------------------------------------

my %nodes;
my $nodeno = 0;

use constant {
	COL_NOD => 5,
	COL_SSP => 4,
	COL_SCP => 3,
	COL_GTT => 2,
	COL_ADJ => 1,
};
#package SP;
sub init {
	my ($self,$top,$network,$nodeno,$pc,$channel,$side,$way,@args) = @_;
	$self->Tristate::init($top,'white');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->CallCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->{pc} = $pc;
	$self->{rt} = $channel->{rt};
	$self->{rttext} = $channel->{rttext};
	$self->{side} = $side;
	$self->{way} = $way;
	$self->{ciccnt} = 0;
	$self->{actcnt} = 0;
	$self->{actog} = 0;
	$self->{actic} = 0;
	$self->{act2w} = 0;
	$self->{tqueries} = {};
	$self->{circuits} = {};
	$self->{responds} = {};
	$self->{paths} = {}; # paths that term or orig here
	$self->{routes} = {}; # routes that term or orig here
	$self->{routes}->{og} = {};
	$self->{routes}->{ic} = {};
	$self->{relate} = {}; # relations in which this is a node
	$self->{linksets} = {}; # linksets that attach here
	my $c = $top->canvas;
	my $mc = $top->mycanvas;
	my $off = $self->{off} = 0;
	my $col = $self->{col} = (($side < 0) ? 0 - COL_NOD : 0 + COL_NOD);
	my $row = $self->{row} = 0;
	my $x = $self->{x} = $mc->colpos($col+$off);
	my $y = $self->{y} = $mc->rowpos($row);
	my $type = ref $self;
	my $state = ($top->{show}->{"\L$type\Es"}) ? 'normal' : 'hidden';
	$self->{items} = [];
	$self->{item} = $c->createOval(
		$x-40,$y-40,$x+40,$y+40,
		-fill=>$self->{color},
		-outline=>'blue', -width=>2,
		-activeoutline=>'cyan', -activewidth=>3,
		-state=>$state,
		-tags=>['SP','node'],
	);
	push @{$self->{items}}, $self->{item};
	$self->{scri} = $c->createLine(
		$x-23,$y-23,$x+23,$y-23,$x+23,$y+23,$x-23,$y+23,$x-23,$y-23,
		-arrow=>'none', -capstyle=>'round', -joinstyle=>'round', -smooth=>0,
		-fill=>'gray', -width=>0.1,
		#-activefill=>'cyan', -activewidth=>2,
		-state=>$state,
		-tags=>['SP','scri'],
	);
	push @{$self->{items}}, $self->{scri};
	$self->{pcode} = SP::pcstring($pc);
	if ($self->{pownr} = $self->pcowner(0)) {
		$self->{ownr} = $c->createText($x,$y+15,
			-anchor=>'center', -fill=>'black', -justify=>'center',
			-text=>$self->{pownr},
			-state=>$state,
			-tags=>['SP','text'],
		);
		push @{$self->{items}}, $self->{ownr};
	}
	$self->{ttxt} = $c->createText($x,$y-15,
		-anchor=>'center', -fill=>'black', -justify=>'center',
		-text=>'SP',
		-state=>$state,
		-tags=>['SP','text'],
	);
	push @{$self->{items}}, $self->{ttxt};
	$self->{text} = $c->createText($x,$y,
		-anchor=>'center', -fill=>'black', -justify=>'center',
		-text=>$self->{pcode},
		-state=>$state,
		-tags=>['SP','text'],
	);
	push @{$self->{items}}, $self->{text};
	$c->raise($self->{item},'all');
	$c->raise($self->{scri},$self->{item});
	$c->raise($self->{ttxt},$self->{scri});
	$c->raise($self->{text},$self->{scri});
	$c->raise($self->{ownr},$self->{scri}) if $self->{ownr};
	$network->regroupsps($top);
	$self->Clickable::attach($top,@args);
}

#sub DESTROY {
#	my $self = shift;
#	my $c = $self->canvas;
#	$c->toplevel->traceVdelete(\$self->{state});
#}

#package SP;
sub xform {
	my ($type,$self,$top) = @_;
	return if ref $self eq $type;
	my $oldtype = ref $self;
	bless $self,$type;
	my $c = $top->canvas;
	my $mc = $top->mycanvas;
	$mc->delballoon($self->{items});

	$c->dtag($self->{item},$oldtype); $c->addtag($type,withtag=>$self->{item});
	$c->dtag($self->{scri},$oldtype); $c->addtag($type,withtag=>$self->{scri});
	$c->dtag($self->{ttxt},$oldtype); $c->addtag($type,withtag=>$self->{ttxt});
	$c->dtag($self->{text},$oldtype); $c->addtag($type,withtag=>$self->{text});
	if ($self->{ownr})
      { $c->dtag($self->{ownr},$oldtype); $c->addtag($type,withtag=>$self->{ownr}); }

	my @oldtags = ();
	push @oldtags, 'SLTM' if $self->{xchg_sltm};
	push @oldtags, 'ISUP' if $self->{xchg_isup};
	push @oldtags, 'TCAP' if $self->{orig_tcap} or $self->{term_tcap};
	push @oldtags, 'circuits' if $self->{ciccnt} > 0;

	$c->delete($self->{item});
	shift @{$self->{items}};
	$self->adjitems($c,\@oldtags);
	$c->itemconfigure($self->{ttxt}, -text=>ref($self));
	if ($self->{alias}) {
		$c->itemconfigure($self->{item}, -dash=>[5,2]);
		$c->itemconfigure($self->{scri}, -dash=>[5,2]);
	}
	$c->raise($self->{item},'all');
	$c->raise($self->{scri},$self->{item});
	$c->raise($self->{ttxt},$self->{scri});
	$c->raise($self->{text},$self->{scri});
	$c->raise($self->{ownr},$self->{scri}) if $self->{ownr};
	my $state = ($top->{show}->{"\L$type\Es"}) ? 'normal' : 'hidden';
	$c->itemconfigure($self->{item},-state=>$state);
	$c->itemconfigure($self->{scri},-state=>$state);
	$c->itemconfigure($self->{ttxt},-state=>$state);
	$c->itemconfigure($self->{text},-state=>$state);
	$c->itemconfigure($self->{ownr},-state=>$state) if $self->{ownr};
	$self->Clickable::attach($top);
}

#package SP;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
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
	foreach my $route (values %{$self->{routes}->{og}}) {
		my $rstate = $route->{state};
		if ($rstate == Tristate::TS_AVAILABLE) {
			$avail++;
		} elsif ($rstate == Tristate::TS_DEGRADED) {
			$degra++;
		} elsif ($rstate == Tristate::TS_UNAVAILABLE) {
			$unava++;
		}
	}
	foreach my $path (values %{$self->{paths}}) {
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
			if ($cluster = $pc_assignments->{pc_blocks}->{$cls}) {
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
			if ($cluster = $pc_assignments->{pc_blocks}->{$cls}) {
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
			if ($own = $pc_assignments->{large_networks}->{$ntw}) {
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
			if ($small = $pc_assignments->{small_networks}->{$ntw}) {
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
			if ($cluster = $pc_assignments->{pc_blocks}->{$cls}) {
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

use constant {
	N_UNKNOWN => 0, # not yet identified
	N_GTT_CC  => 1, # GTT capability code
	N_SCP_DB  => 2, # SCP database
	N_SSP_TA  => 3, # SSP transaction alias
	N_SSP_AP  => 4, # SSP alias point code
	N_SSP_PP  => 5, # SSP primary point code
	N_SSP_NT  => 6, # SSP w/o TCAP
	N_SSP_WT  => 7, # SSP w/ TCAP
	N_STP_NG  => 8, # STP w/o GTT
	N_STP_WG  => 9, # STP w/ GTT
};

#package SP;
sub reanalyze {
	my ($self,$top,$network) = @_;
	my $c = $top->canvas;
	if ($self->{xchg_isup}) {
		$c->addtag('ISUP',withtag=>$self->{item});
		$c->addtag('ISUP',withtag=>$self->{scri});
		$c->addtag('ISUP',withtag=>$self->{ttxt});
		$c->addtag('ISUP',withtag=>$self->{text});
		$c->addtag('ISUP',withtag=>$self->{ownr}) if $self->{ownr};
	}
	if ($self->{orig_tcap} or $self->{term_tcap}) {
		$c->addtag('TCAP',withtag=>$self->{item});
		$c->addtag('TCAP',withtag=>$self->{scri});
		$c->addtag('TCAP',withtag=>$self->{ttxt});
		$c->addtag('TCAP',withtag=>$self->{text});
		$c->addtag('TCAP',withtag=>$self->{ownr}) if $self->{ownr};
	}
	if ($self->{xchg_sltm}) {
		$c->addtag('SLTM',withtag=>$self->{item});
		$c->addtag('SLTM',withtag=>$self->{scri});
		$c->addtag('SLTM',withtag=>$self->{ttxt});
		$c->addtag('SLTM',withtag=>$self->{text});
		$c->addtag('SLTM',withtag=>$self->{ownr}) if $self->{ownr};
	}
	my $col = abs($self->{col});
	my $row = $self->{row};
	if ($self->{xchg_sltm}) {
		$col = COL_ADJ;
	} elsif ($self->{xchg_isup} || ($self->{orig_tcap} && $self->{term_tcap})) {
		$col = COL_SSP;
	} elsif ($self->{orig_tcap} && !$self->{term_tcap}) {
		$col = COL_SCP;
	} elsif (!$self->{orig_tcap} && $self->{term_tcap}) {
		$col = COL_GTT;
	} 
	if (abs($self->{col}) != $col) {
		if ($self->{col} < 0) {
			$col = 0 - $col;
		}
		$self->movesp($top,$col,$row);
		$network->regroupsps($top);
		$top->{updatenow} = 1;
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
sub swap {
	my ($self,$top) = @_;
	$self->movesp($top,-$self->{col},$self->{row});
}

#package SP;
sub movesp {
	my ($self,$top,$col,$row,$off) = @_;
	my $mc = $top->mycanvas;
	my $nx = $mc->colpos($col+$off);
	my $ny = $mc->rowpos($row);
	my $dx = $nx - $self->{x};
	my $dy = $ny - $self->{y};
	if ($dx or $dy) {
		my $c = $top->canvas;
		$c->move($self->{item},$dx,$dy);
		$c->move($self->{scri},$dx,$dy);
		$c->move($self->{ttxt},$dx,$dy);
		$c->move($self->{ownr},$dx,$dy) if $self->{ownr};
		$c->move($self->{text},$dx,$dy);
	}
	$self->{x} = $nx;
	$self->{y} = $ny;
	$self->{off} = $off;
	$self->{col} = $col;
	$self->{row} = $row;
	#if ($dx or $dy) {
		foreach my $o (values %{$self->{paths}}       ) { $o->moveit($top); }
		#foreach my $o (values %{$self->{relate}}      ) { $o->moveit($top); }
		#foreach my $o (values %{$self->{linksets}}    ) { $o->moveit($top); }
		#foreach my $o (values %{$self->{routes}->{og}}) { $o->moveit($top); }
		#foreach my $o (values %{$self->{routes}->{ic}}) { $o->moveit($top); }
	#}
	if (exists $self->{arcs}) { foreach my $a (@{$self->{arcs}}) { $a->moveit($top); } }
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
	$have->{routes}   = scalar keys %{$self->{routes}->{og}};
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
		$node->getmenu($mc,$top,$X,$Y);
		$m->add('cascade',-menu=>$mc,-label=>$label);
	}
	if ($have->{aliases}) {
		my $mc = $m->Menu(-title=>'Alias Point Codes Menu');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{aliases}}) {
			my $node = $self->{aliases}->{$pc};
			my $label = 'Alias '.$node->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$node->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Alias Point Codes');
	}
	if ($have->{adjacent}) {
		my $mc = $m->Menu(-title=>'Adjacent Nodes Menu');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{adjacent}}) {
			my $node = $self->{adjacent}->{$pc};
			my $label = 'Adjacent '.$node->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$node->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Adjacent Nodes');
	}
	if ($have->{combined}) {
		my $mc = $m->Menu(-title=>'Combined Linksets Menu');
		foreach my $key (sort keys %{$self->{combined}}) {
			my $combined = $self->{combined}->{$key};
			my $nodea = $combined->{nodea};
			my $nodeb = $combined->{nodeb};
			my $label = 'Combined Linkset to '.$nodea->shortid.' and '.$nodeb->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$combined->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Combined Linksets');
	}
	if ($have->{linksets}) {
		my $mc = $m->Menu(-title=>'Linksets Menu');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{linksets}}) {
			my $linkset = $self->{linksets}->{$pc};
			my $node;
			if ($linkset->{nodea}->{pc} == $self->{pc}) {
				$node = $linkset->{nodeb};
			} else {
				$node = $linkset->{nodea};
			}
			my $label = 'Linkset to '.$node->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$linkset->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Linksets');
	}
	if ($have->{routes}) {
		my $mc = $m->Menu(-title=>'Routesets Menu');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{routes}->{og}}) {
			my $route = $self->{routes}->{og}->{$pc};
			my $node = $route->{node};
			my $label = 'Route to '.$node->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$route->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Routesets');
	}
	if ($have->{relate}) {
		my $mc = $m->Menu(-title=>'Relations Menu');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{relate}}) {
			my $relation = $self->{relate}->{$pc};
			my $node;
			if ($relation->{nodea}->{pc} == $self->{pc}) {
				$node = $relation->{nodeb};
			} else {
				$node = $relation->{nodea};
			}
			my $label = 'Routset to '.$node->shortid;
			my $m3 = $mc->Menu( -title=>"$label Menu");
			$relation->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Relations');
	}
}

#package SP;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Point code type:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Optionmenu(%optionleft,
		-options=>\@Channel::rloptions,
		-variable=>\$self->{rt},
		-textvariable=>\$self->{rttext}, -width=>12,
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
	)->pack(%tframepack);
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
	)->pack(%tframepack);
	$$row = 0;
	$self->fillstate($top,$f,$row);
	if ($self->{primary} or keys %{$self->{aliases}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Aliases:',
		)->pack(%tframepack);
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
			if ($col > 4) { $col = 0; $$row++ };
		}
	}
	if (keys %{$self->{adjacent}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Adjacent:')->pack(%tframepack);
		$$row = 0;
		my $col = 0;
		foreach my $pc (sort {$a <=> $b} keys %{$self->{adjacent}}) {
			$f->Label(%labelright,-text=>"Adjacent:",
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns') if $col == 0; $col++;
			my $node = $self->{adjacent}->{$pc};
			my $w = $f->Entry(%entrycenter,-text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$node->bindtowidget($top,$w);
			if ($col > 4) { $col = 0; $$row++ };
		}
	}
	if (keys %{$self->{linksets}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Linksets:')->pack(%tframepack);
		$$row = 0;
		my $col = 0;
		foreach my $pc (sort {$a <=> $b} keys %{$self->{linksets}}) {
			$f->Label(%labelright,-text=>"$ltxt:",
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns') if $col == 0; $col++;
			my $linkset = $self->{linksets}->{$pc};
			my $node;
			if ($self->{pc} == $linkset->{nodea}->{pc}) {
				$node = $linkset->{nodeb};
			} else {
				$node = $linkset->{nodea};
			}
			my $w = $f->Entry(%entrycenter,-text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$linkset->bindtowidget($top,$w);
			if ($col > 4) { $col = 0; $$row++ };
		}
	}
	if (keys %{$self->{routes}->{og}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Routes:',
		)->pack(%tframepack);
		my $p = $f;
		$p = $f->Scrolled('Pane',
			-scrollbars=>'osoe',
			-sticky=>'we',
			-gridded=>'y',
		)->pack(%tframepack) if keys %{$self->{routes}->{og}} > 20;
		$$row = 0;
		my $col = 0;
		$p->Label(%labelright,-text=>'Routes to:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{routes}->{og}}) {
			my $route = $self->{routes}->{og}->{$pc};
			my $node = $route->{node};
			$col++;
			my $w = $p->Entry(%entrycenter,-text=>$node->shortid,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			$route->bindtowidget($top,$w);
			if ($col > 4) { $col = 0; $$row++ };
		}
	}
	if (keys %{$self->{relate}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Routesets:',
		)->pack(%tframepack);
		my $p = $f;
		$p = $f->Scrolled('Pane',
			-scrollbars=>'osoe',
			-sticky=>'we',
			-gridded=>'y',
		)->pack(%tframepack) if keys %{$self->{relate}} > 4;
		$$row = 0;
		$p->Label(%labelcenter,-text=>'Point Code',
		)->grid(-row=>$$row++,-column=>0,-sticky=>'ewns');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{relate}}) {
			my $relation = $self->{relate}->{$pc};
			my $node;
			if ($relation->{nodea}->{pc} == $self->{pc}) {
				$node = $relation->{nodeb};
			} else {
				$node = $relation->{nodea};
			}
			my $w = $p->Entry(%entrycenter, -text=>$node->shortid,
			)->grid(-row=>$$row++,-column=>0,-sticky=>'ewns');
			$relation->bindtowidget($top,$w);
		}
	}
	if (keys %{$self->{circuits}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Circuits:',
		)->pack(%tframepack);
		my $p = $f;
		$p = $f->Scrolled('Pane',
			-scrollbars=>'osoe',
			-sticky=>'we',
			-gridded=>'y',
		)->pack(%tframepack) if keys %{$self->{circuits}} > 4;
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
			my ($ogcol,$iccol);
			if ($self->{way} eq 'O')
			{ $ogcol = 3; $iccol = 4; } else
			{ $ogcol = 4; $iccol = 3; }
			my $node = $self->{circuits}->{$pc};
			my $w = $p->Entry(%entrycenter, -text=>$node->shortid,
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$node->bindtowidget($top,$w);
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{ciccnt},
			)->grid(-row=>$$row,-column=>1,-sticky=>'ewns');
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{actcnt},
			)->grid(-row=>$$row,-column=>2,-sticky=>'ewns');
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{actforw},
			)->grid(-row=>$$row,-column=>$ogcol,-sticky=>'ewns');
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{actrevs},
			)->grid(-row=>$$row,-column=>$iccol,-sticky=>'ewns');
			$p->Entry(%entryright, -textvariable=>\$self->{relate}->{$pc}->{actboth},
			)->grid(-row=>$$row++,-column=>5,-sticky=>'ewns');
		}
	}
	if (keys %{$self->{tqueries}} || keys %{$self->{responds}}) {
		$f = $tw->TFrame(%tframestyle, -label=>'Queries:',
		)->pack(%tframepack);
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
@ISA = qw(MsgStats Logging Properties Status Clickable CallCollector MsgCollector);
# -------------------------------------

my $network;

#package Network;
sub new {
	my ($type,$top,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->{top} = $top;
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->CallCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->{msgnum} = 0;
	$self->{ciccnt} = 0;
	$self->{actcnt} = 0;
	$self->{act1w} = 0;
	$self->{act2w} = 0;
	$self->{nodes} = {};
	$self->{nodeno} = 0;
	$self->{channels} = {};
	$self->{channelno} = 0;
	$self->{relations} = {};
	$self->{relationno} = 0;
	my $c = $top->canvas;
	$c->CanvasBind('<ButtonPress-3>',[\&Network::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y'),Tk::Ev('x'),Tk::Ev('y')]);
	$Network::network = $self;
	$top->statusbar->configure(-text=>"Discovered ".$self->identify);
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
	my ($self,$top,$ppa,@args) = @_;
	return $self->{channels}->{$ppa} if exists $self->{channels}->{$ppa};
	my $channelno = $self->{channelno} + 1;
	my $channel = Channel->new($top,$channelno,$ppa,@args);
	$self->{channels}->{$ppa} = $channel;
	$self->{channelno} = $channelno;
	return $channel;
}

#package Network;
sub getSp {
	my ($self,$top,$pc,@args) = @_;
	return $self->{nodes}->{$pc} if exists $self->{nodes}->{$pc};
	my $nodeno = $self->{nodeno} + 1;
	my $node = SP->new($top,$self,$nodeno,$pc,@args);
	$self->{nodes}->{$pc} = $node;
	$self->{nodeno} = $nodeno;
	return $node;
}

#package Network;
sub getRelation {
	my ($self,$top,$nodea,$nodeb,@args) = @_;
	my $key = "$nodea->{pc},$nodeb->{pc}";
	return $self->{relations}->{$key} if exists $self->{relations}->{$key};
	my $relationno = $self->{relationno} + 1;
	my $relation = Relation->new($top,$self,$relationno,$nodea,$nodeb,@args);
	$self->{relations}->{$key} = $relation;
	$key = "$nodeb->{pc},$nodea->{pc}";
	$self->{relations}->{$key} = $relation;
	$self->{relationno} = $relationno;
	return $relation;
}

#package Network;
sub getLinkset {
	my ($self,$top,$nodea,$nodeb,@args) = @_;
	my $key = "$nodea->{pc},$nodeb->{pc}";
	return $self->{linksets}->{$key} if exists $self->{linksets}->{$key};
	my $linksetno = $self->{linksetno} + 1;
	my $relation = $self->getRelation($top,$nodea,$nodeb,@args);
	my $linkset = Linkset->new($top,$self,$relation,@args);
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
sub regroupsps {
	my ($self,$top) = @_;
	my $mc = $top->mycanvas;
	my %totals;
	my %offset;
	my ($mincol,$maxcol) = (0,0);
	while (my ($pc,$node) = each %{$self->{nodes}}) {
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
	foreach my $pc (sort {$a <=> $b} keys %{$self->{nodes}}) {
		my $node = $self->{nodes}->{$pc};
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
		$node->movesp($top,$col,$row,$off/2);
#		$node->movesp($top,$col+$offset{$col}/2,$row);
	}
}

#package Network;
sub add_msg {
	my ($self,$top,$msg) = @_;
	$self->inc($msg,0); # for now
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

#package Network;
sub getmore {
	my ($self,$m,$top,$X,$Y) = @_;
	my $have = {};
	$have->{channels}  = scalar keys %{$self->{channels}};
	$have->{nodes}     = scalar keys %{$self->{nodes}};
	$have->{relations} = scalar keys %{$self->{relations}};
	$have->{linksets}  = scalar keys %{$self->{linksets}};
	$m->add('separator') if
		$have->{channels} +
		$have->{nodes} +
		$have->{relations} +
		$have->{linksets};
	if ($have->{channels}) {
		my $mc = $m->Menu(-title=>'Channels Menu');
		foreach my $ppa (sort {$a <=> $b} keys %{$self->{channels}}) {
			my $channel = $self->{channels}->{$ppa};
			my $label = 'Channel '.$channel->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$channel->getmenu($m3,$top,$X,$Y);
			$channel->getmore($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Channels');
	}
	if ($have->{nodes}) {
		my $mc = $m->Menu(-title=>'Nodes Menu');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{nodes}}) {
			my $node = $self->{nodes}->{$pc};
			my $label = ref($node).' '.$node->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$node->getmenu($m3,$top,$X,$Y);
			$node->getmore($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Nodes');
	}
	if ($have->{relations}) {
		my $mc = $m->Menu(-title=>'Relations Menu');
		foreach my $key (sort keys %{$self->{relations}}) {
			my $relation = $self->{relations}->{$key};
			next unless $relation->{key} eq $key;
			my $label = ref($relation).' '.$relation->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$relation->getmenu($m3,$top,$X,$Y);
			$relation->getmore($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
		$m->add('cascade',-menu=>$mc,-label=>'Relations');
	}
	if ($have->{linksets}) {
		my $mc = $m->Menu(-title=>'Linksets Menu');
		foreach my $key (sort keys %{$self->{linksets}}) {
			my $linkset = $self->{linksets}->{$key};
			next unless $linkset->{key} eq $key;
			my $label = ref($linkset).' '.$linkset->shortid;
			my $m3 = $mc->Menu(-title=>"$label Menu");
			$linkset->getmenu($m3,$top,$X,$Y);
			$linkset->getmore($m3,$top,$X,$Y);
			$mc->add('cascade',-menu=>$m3,-label=>$label);
		}
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
		-textvariable=>\$self->{actcnt},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Number of active one-way circuits:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{act1w},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Number of active both-way circuits:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{act2w},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

# -------------------------------------
package Channel;
use strict;
use vars qw(@ISA);
@ISA = qw(Pentastate MsgStats Logging Properties Status Clickable MsgCollector);
# -------------------------------------

use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
my @rloptions = (
	['Unknown'	     => RT_UNKNOWN ],
	['14-bit point code' => RT_14BIT_PC],
	['24-bit point code' => RT_24BIT_PC],
);
use constant {
	HT_UNKNOWN => 0,
	HT_BASIC => 3,	 # also link level header length
	HT_EXTENDED => 6 # also link level header length
};
my @htoptions = (
	['Unknown'  => HT_UNKNOWN ],
	['Basic'    => HT_BASIC	  ],
	['Extended' => HT_EXTENDED],
);
use constant {
	MP_UNKNOWN => 0,
	MP_JAPAN => 1,
	MP_NATIONAL => 2,
	MP_INTERNATIONAL => 3
};
my @mpoptions = (
	['Unknown'	    => MP_UNKNOWN	],
	['Japan'	    => MP_JAPAN		],
	['National'	    => MP_NATIONAL	],
	['International'    => MP_INTERNATIONAL	],
);

my %channels;
my $channelno = 0;

#package Channel;
sub init {
	my ($self,$top,$channelno,$ppa,@args) = @_;
	$self->Pentastate::init($top,'black');
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->Clickable::init(@args);
	$self->MsgCollector::init(@args);
	$self->{ppa} = $ppa;
	$self->{slot} = (($ppa >> 0) & 0x1f);
	$self->{span} = (($ppa >> 5) & 0x03);
	$self->{card} = (($ppa >> 7) & 0x01);
	$self->{ht} = HT_UNKNOWN;
	$self->{httext} = 'Unknown';
	$self->{pr} = MP_UNKNOWN;
	$self->{prtext} = 'Unknown';
	$self->{rt} = RT_UNKNOWN;
	$self->{rttext} = 'Unknown';
	$self->{orig} = {};
	$self->{dest} = {};
	$self->{opcs} = {};
	$self->{dpcs} = {};
	$self->{paths} = {};
	$self->{msgbuf} = [];
	#print "Created new channel ($self->{card}:$self->{span}:$self->{slot}).\n";
	my $offa = $self->{offa} = 0;
	my $cola = $self->{cola} = 0 - SP::COL_ADJ;
	my $rowa = $self->{rowa} = $channelno * 2;
	my $offb = $self->{offb} = 0;
	my $colb = $self->{colb} = 0 + SP::COL_ADJ;
	my $rowb = $self->{rowb} = $channelno * 2;
	my $mc = $top->mycanvas;
	my $xa = $self->{xa} = $mc->colpos($cola+$offa);
	my $ya = $self->{ya} = $mc->rowpos($rowa);
	my $xb = $self->{xb} = $mc->colpos($colb+$offb);
	my $yb = $self->{yb} = $mc->rowpos($rowb);
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	my $c = $top->canvas;
	$self->{item} = $c->createLine($xa,$ya,$xb,$yb,
		-arrow=>'last', -capstyle=>'round', -joinstyle=>'round', -smooth=>0,
		-fill=>$self->{color}, -width=>0.1,
		-activefill=>'cyan', -activewidth=>2,
		-state=>($top->{show}->{channels}) ? 'normal' : 'hidden',
		-tags=>['channel'],
	);
	push @{$self->{items}}, $self->{item};
	$c->lower($self->{item},'node');
	$c->lower($self->{item},'linkset');
	$self->Clickable::attach($top,@args);
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
sub get {
	my ($type,$ppa,@args) = @_;
	return $channels{$ppa} if exists $channels{$ppa};
	my $self = {};
	bless $self, $type;
	$channels{$ppa} = $self;
	$channelno = $channelno + 1;
	$self->init($channelno,$ppa,@args);
	return $self;
}

#package Channel;
sub findalias {
	my ($self,$top,$network,$node,@paths) = @_;
	return unless ref $node eq 'SSP' or ref $node eq 'SCP';
	my $col = Linkset::COL_GTT;
	foreach my $path (@paths) {
		my $alias = $path->{node};
		next unless $alias->{pc} != $node->{pc};
		if (abs($alias->{col}) != $col) {
			my $newcol = $col;
			if ($alias->{col} < 0) { $newcol = -$col; }
			$alias->movesp($top,$newcol,$alias->{row});
			$alias->{alias} = 1;
			$alias->{primary} = $node;
			$node->{aliases}->{$alias->{pc}} = $alias;
			my $c = $top->canvas;
			$c->itemconfigure($alias->{item}, -dash=>[5,2]);
			$c->itemconfigure($alias->{scri}, -dash=>[5,2]);
			$top->statusbar->configure(-text=>"Discovered alias ".$alias->identify);
		}
	}
}

#package Channel;
sub bindchannel {
	my ($self,$top,$network,$nodea,$nodeb,$msg) = @_;
	my $slc = $self->{slc} = $msg->{slc};
	$self->{nodea} = $nodea;
	$self->{nodeb} = $nodeb;
	my $linkset = $network->getLinkset($top,$nodea,$nodeb);
	my $link = $linkset->getLink($top,$slc);
	$self->{link} = $link;
	$link->addChannel($self);

	$self->findalias($top,$network,$nodea,(values %{$self->{opcs}}));
	$self->findalias($top,$network,$nodeb,(values %{$self->{dpcs}}));
	my $c = $top->canvas;
	$c->lower($self->{item},$linkset->{item});
	#$c->itemconfigure($self->{item}, -state=>'hidden');
	$self->moveit($top);
	$network->regroupsps($top);
	$top->{updatenow} = 1;
}

#package Channel;
sub cardnum {
	shift->{card};
}
#package Channel;
sub spannum {
	shift->{span};
}
#package Channel;
sub slotnum {
	shift->{slot};
}
#package Channel;
sub ppa {
	shift->{ppa};
}
#package Channel;
sub add_msg {
	my ($self,$top,$network,$msg,@args) = @_;
	unless ($self->{detected}) {
		if ($self->detecting) {
			push @{$self->{msgbuf}}, $msg;
			return;
		}
		$self->{detected} = 1;
		push @{$self->{msgbuf}}, $msg;
		while ($msg = pop @{$self->{msgbuf}}) {
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
	my ($nodeb,$nodea,$pc,$path,$link);
	$self->inc($msg,0);
	$self->pushmsg($msg);
	$network->add_msg($top,$msg);
	if (exists $msg->{opc}) {
		$pc = $msg->{opc};
		if ($path = $self->{opcs}->{$pc}) {
			$nodea = $path->{node};
			$self->swap($top) if $nodea->{col} < 0 && $self->{cola} > 0;
		} else {
			$nodea = $network->getSp($top,$pc, $self, $self->{cola},'O');
			$self->swap($top) if $nodea->{col} < 0 && $self->{cola} > 0;
			$path = Path->new($top,$self,'a',$nodea);
			$self->{opcs}->{$pc} = $path;
			$self->findalias($top,$network,$self->{nodea},(values %{$self->{opcs}})) if $self->{nodea};
			$self->findalias($top,$network,$self->{nodeb},(values %{$self->{dpcs}})) if $self->{nodeb};
			$network->regroupsps($top);
		}
		$path->add_msg($top,$network,$msg,0);
		$nodea->add_msg($top,$network,$msg,0);
	}
	if (exists $msg->{dpc}) {
		$pc = $msg->{dpc};
		if ($path = $self->{dpcs}->{$pc}) {
			$nodeb = $path->{node};
			$self->swap($top) if $nodeb->{col} < 0 && $self->{colb} > 0;
		} else {
			$nodeb = $network->getSp($top,$pc, $self, $self->{colb},'I');
			$self->swap($top) if $nodeb->{col} < 0 && $self->{colb} > 0;
			$path = Path->new($top,$self,'b',$nodeb);
			$self->{dpcs}->{$pc} = $path;
			$self->findalias($top,$network,$self->{nodea},(values %{$self->{opcs}})) if $self->{nodea};
			$self->findalias($top,$network,$self->{nodeb},(values %{$self->{dpcs}})) if $self->{nodeb};
			$network->regroupsps($top);
		}
		$path->add_msg($top,$network,$msg,1);
		$nodeb->add_msg($top,$network,$msg,1);
	}
	if (exists $msg->{dpc} && exists $msg->{opc}) {
		my $relation = $network->getRelation($top,$nodea,$nodeb);
		$relation->add_msg($top,$network,$msg);
		if ($link = $self->{link}) {
			my $dir = ($self->{nodea}->{pc} == $link->{nodea}->{pc}) ? 0 : 1;
			$link->add_msg($top,$network,$msg,$dir);
		}
		if (my $sub = $msghandler{$msg->{si}}->{$msg->{mt}}) {
			&{$sub}($self,$top,$network,$nodea,$nodeb,$msg);
		}
	}
}
#package Channel;
sub swap {
	my ($self,$top) = @_;
	$self->movechannel($top,-$self->{offa},-$self->{cola},$self->{rowa}, -$self->{offb},-$self->{colb},$self->{rowb});
	while (my ($k,$node) = each %{$self->{opcs}}) {
		$node->swap($top) if $self->{cola} < 0 && $node->{col} > 0;
	}
	while (my ($k,$node) = each %{$self->{dpcs}}) {
		$node->swap($top) if $self->{colb} < 0 && $node->{col} > 0;
	}
}
#package Channel;
sub detecting {
	my $self = shift;
	return 0 if ($self->{ht} != 0 && $self->{pr} != 0 && $self->{rt} != 0);
	return 1;
}

#package Channel;
sub setht {
	my ($self,$ht) = @_;
	$self->{ht} = $ht;
	if ($ht == HT_UNKNOWN) {
		$self->{httext} = 'Unknown';
	} elsif ($ht == HT_BASIC) {
		$self->{httext} = 'Basic';
	} elsif ($ht == HT_EXTENDED) {
		$self->{httext} = 'Extended';
	}
}
#package Channel;
sub setrt {
	my ($self,$rt) = @_;
	$self->{rt} = $rt;
	if ($rt == RT_UNKNOWN) {
		$self->{rttext} = 'Unknown';
	} elsif ($rt == RT_14BIT_PC) {
		$self->{rttext} = '14-bit point code';
	} elsif ($rt == RT_24BIT_PC) {
		$self->{rttext} = '24-bit point code';
	}
}
#package Channel;
sub setpr {
	my ($self,$pr) = @_;
	$self->{pr} = $pr;
	if ($pr == MP_UNKNOWN) {
		$self->{prtext} = 'Unknown';
	} elsif ($pr == MP_JAPAN) {
		$self->{prtext} = 'Japan';
	} elsif ($pr == MP_NATIONAL) {
		$self->{prtext} = 'National';
	} elsif ($pr == MP_INTERNATIONAL) {
		$self->{prtext} = 'International';
	}
}

#package Channel;
sub moveit {
	my ($self,$top) = @_;
	my $nodea = $self->{nodea};
	my $nodeb = $self->{nodeb};
	my $xa = $nodea->{x};
	my $ya = $nodea->{y};
	my $xb = $nodeb->{x};
	my $yb = $nodeb->{y};
	return if $xa == $self->{xa} &&
		  $ya == $self->{ya} &&
		  $xb == $self->{xb} &&
		  $yb == $self->{yb};
	$self->{offa} = $nodea->{off};
	$self->{cola} = $nodea->{col};
	$self->{rowa} = $nodea->{row};
	$self->{offb} = $nodeb->{off};
	$self->{colb} = $nodeb->{col};
	$self->{rowb} = $nodeb->{row};
	my $c = $top->canvas;
	$c->coords($self->{item},$xa,$ya,$xb,$yb);
	$self->{xa} = $xa;
	$self->{ya} = $ya;
	$self->{xb} = $xb;
	$self->{yb} = $yb;
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	foreach my $o (values %{$self->{paths}}) { $o->moveit($top); }
}

#package Channel;
sub movechannel {
	my ($self,$top,$offa,$cola,$rowa,$offb,$colb,$rowb) = @_;
	return if $cola == $self->{cola} && $offa == $self->{offa} &&
	          $rowa == $self->{rowa} &&
		  $colb == $self->{colb} && $offb == $self->{offb} &&
		  $rowb == $self->{rowb};
	my $mc = $top->mycanvas;
	my $xa = $self->{xa} = $mc->colpos($cola+$offa);
	my $ya = $self->{ya} = $mc->rowpos($rowa);
	my $xb = $self->{xb} = $mc->colpos($colb+$offb);
	my $yb = $self->{yb} = $mc->rowpos($rowb);
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	$self->{cola} = $cola;
	$self->{rowa} = $rowa;
	$self->{colb} = $colb;
	$self->{rowb} = $rowb;
	my $c = $top->canvas;
	$c->coords($self->{item},$xa,$ya,$xb,$yb);
	$c->lower($self->{item},'node');
	$c->lower($self->{item},'linkset');
	foreach my $o (values %{$self->{paths}}) { $o->moveit($top); }
}

#package Channel;
sub identify {
	my $self = shift;
	my $id = "Channel $self->{card}:$self->{span}:$self->{slot},";
	$id .= " $self->{nodea}->{pcode} -> $self->{nodeb}->{pcode} link $self->{slc}" if $self->{nodea};
	return ($self->{id} = $id);
}

#package Channel;
sub shortid {
	my $self = shift;
	return "$self->{card}:$self->{span}:$self->{slot}";
}

#package Channel;
sub fillprops {
	my ($self,$top,$tw,$row) = @_;

	my $ppa = "$self->{card}:$self->{span}:$self->{slot}";
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

	my $ppa = "$self->{card}:$self->{span}:$self->{slot}";
	if ($self->{nodea} && $self->{nodeb}) {
		$tw->Label(%labelright,
			-text=>'Signalling link code:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryleft,
			-textvariable=>\$self->{slc},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	}
	if ($self->{nodea}) {
		$tw->Label(%labelright,
			-text=>'SP A point code:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryleft,
			-textvariable=>\$self->{nodea}->{pcode},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	}
	if ($self->{nodeb}) {
		$tw->Label(%labelright,
			-text=>'SP B point code:',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$tw->Entry(%entryleft,
			-textvariable=>\$self->{nodeb}->{pcode},
		)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	}
	$tw->Label(%labelright,
		-text=>'Header type:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Optionmenu(%optionleft,
		-options=>\@htoptions,
		-variable=>\$self->{ht},
		-textvariable=>\$self->{httext},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Message priority:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Optionmenu(%optionleft,
		-options=>\@mpoptions,
		-variable=>\$self->{pr},
		-textvariable=>\$self->{prtext},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Routing label type:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Optionmenu(%optionleft,
		-options=>\@rloptions,
		-variable=>\$self->{rt},
		-textvariable=>\$self->{rttext},
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
sub new {
	my ($type,$b,$p,$s,$e,@args) = @_;
	my $self = Field::new($type,$b,$p,@args);
	$self->{len} = int($e/8) + 1;
	$self->{ptr} = $p;
	$self->{beg} = $s;
	$self->{end} = $e;
	$self->{wid} = $e+1-$s;
	#TODO unpack bits
	my $sb = $p + int($s/8);
	my $eb = $p + int($e/8);
	my ($v,$i);
	for (($v,$i) = (0,$eb); $i >= $sb; $i--) {
		my ($m,$c) = (0xff,$b->[$i]);
		if ($i == $eb) { $m &=  ((1<<(8-$e))-1); }
		if ($i == $sb) { $m &= ~((1<<($s+1))-1); }
		$v |= ($c & $m);
		$v <<= 8 unless $i == $sb;
	}
	$v >>= $s;
	$self->{val} = $v;
	return $self;
}

sub bits {
	my $self = shift;
	my @bytes = ();
	my $bits = '';
	my $word = $self->{val};
	for (my $i = 0; $i < $self->{beg}; $i++) {
		$bits = '-'.$bits;
	}
	my $p = $self->{ptr};
	for (my $i = $self->{beg}; $i <= $self->{end}; $i++) {
		$bits = (($word & 0x1)?'1':'0').$bits;
		$word >>= 1;
		if (($i&0x7) == 0x7) {
			push @bytes, sprintf('%3d %s', $p + scalar(@bytes), $bits);
			$bits = '';
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
# ------------------------------------
package BsnField; use strict; use vars qw(@ISA); @ISA = qw(SnField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return SnField::new($type,$b,$p+0,@args);
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
# ------------------------------------
package LiField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+2,0,5,@args);
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
# ------------------------------------
package XBsn0Field; use strict; use vars qw(@ISA); @ISA = qw(Xsn0Field);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return Xsn0Field::new($type,$b,$p+1,@args);
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
# ------------------------------------
package XliField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+4,0,8,@args);
}
# ------------------------------------
package Xli0Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+5,1,7,@args);
}
# ------------------------------------
package XjmpField; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# ------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+5,6,7,@args);
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
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+1,0,7,@args);
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
# -------------------------------------
package Sls24Field; use strict; use vars qw(@ISA); @ISA = qw(BitField);
# -------------------------------------
sub new {
	my ($type,$b,$p,@args) = @_;
	return BitField::new($type,$b,$p+6,0,7,@args);
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub new {
	my ($type,$b,$p,$rt,@args) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
use constant {
	HT_UNKNOWN => 0,
	HT_BASIC => 3,	 # also link level header length
	HT_EXTENDED => 6 # also link level header length
};
use constant {
	MP_UNKNOWN => 0,
	MP_JAPAN => 1,
	MP_NATIONAL => 2,
	MP_INTERNATIONAL => 3
};
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
	my $channel = $network->getChannel($top,$self->{ppa});
	if ($self->decode($channel) >= 0) {
		$channel->add_msg($top,$network,$self);
		return;
	}
	#print STDERR "decoding error\n";
}
#package Message;
sub decode {
	my ($self,$channel,@args) = @_;
	$self->{channel} = $channel;
	my @b = (unpack('C*', substr($self->{dat}, 0, 8)));
	if (!exists $self->{mtp2decode}) {
		my $len = $self->{hdr}->{len};
		if (3 <= $len && $len <= 5) {
			$channel->setht(HT_BASIC);
		} elsif (6 <= $len && $len <= 8) {
			$channel->setht(HT_EXTENDED);
		}
		return 0 unless $channel->{ht} != HT_UNKNOWN;
		my ($bsn,$bib,$fsn,$fib,$li,$li0);
		if ($channel->{ht} == HT_BASIC) {
			$self->{bsn} = $b[0] & 0x7f;
			$self->{bib} = $b[0] >> 7;
			$self->{fsn} = $b[1] & 0x7f;
			$self->{fib} = $b[1] >> 7;
			$self->{li} = $b[2] & 0x3f;
			$self->{li0} = $b[2] >> 6;
		} elsif ($channel->{ht} == HT_EXTENDED) {
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
			print STDERR "bad length indicator $self->{li} != $inf\n";
		}
		if ($self->{li0} != 0) {
			$channel->setrt(RT_24BIT_PC);
			$channel->setpr(MP_JAPAN);
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
		$channel->setrt(RT_14BIT_PC);
		$channel->setpr(MP_INTERNATIONAL);
	}
	if ($channel->{pr} == MP_UNKNOWN) {
		$channel->setpr(MP_NATIONAL) if $self->{mp} != 0;
	} elsif ($channel->{pr} == MP_JAPAN) {
		$self->{mp} = $self->{li0};
	} elsif ($channel->{pr} == MP_INTERNATIONAL) {
		$self->{mp} = 0;
	}
	if ($self->{li} < HT_EXTENDED) {
		print STDERR "too short for RL, li = $self->{li}\n";
		return -1;
	}
	if ($self->{li} < 9 || ($self->{si} == 5 && $self->{li} < 11)) {
		$channel->setrt(RT_14BIT_PC);
	}
	if ($channel->{rt} == RT_UNKNOWN) {
		my $ret = $self->checkRoutingLabelType($self->{si},$channel,$self->{li},\@b) < 0;
		return $ret if $ret <= 0;
		print "check succeeded on si=$self->{si}, mt=$self->{mt}";
	}
	if (!exists $self->{mtp3decode}) {
		return 0 if $channel->detecting;
		if ($channel->{rt} == RT_14BIT_PC) {
			if ($self->{li} < 6) {
				print STDERR "too short for 14-bit RL, li = $self->{li}\n";
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
				print STDERR "too short for 24-bit RL, li = $self->{li}\n";
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
			print STDERR "no message type for si=$self->{si}\n";
		}
		unless (exists $mtypes{$self->{si}}->{$self->{mt}}) {
			print STDERR "no message type for si=$self->{si}, mt=$self->{mt}\n";
		}
		if ($self->{si} == 1 || $self->{si} == 2) {
			if ($channel->{rt} == RT_14BIT_PC) {
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
		$channel->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(RT_14BIT_PC);
		} else {
			$channel->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(RT_14BIT_PC);
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
		$channel->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(RT_14BIT_PC);
		} else {
			$channel->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(RT_14BIT_PC);
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
		$channel->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(RT_14BIT_PC);
		} else {
			$channel->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(RT_14BIT_PC);
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
		$channel->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(RT_14BIT_PC);
		} else {
			$channel->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(RT_14BIT_PC);
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
		$channel->setrt(RT_24BIT_PC);
		return 1;
	}
	elsif ($ansi == PT_MAYBE) {
		return 0 if $itut == PT_MAYBE;
		if ($itut == PT_YES) {
			$channel->setrt(RT_14BIT_PC);
		} else {
			$channel->setrt(RT_24BIT_PC);
		}
		return 1;
	}
	else {
		return -1 if $itut == PT_NO;
		$channel->setrt(RT_14BIT_PC);
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
	if ($channel->{ht} == HT_EXTENDED) {
		push @l2h, XBsnField->new($b,$p);
		push @l2h, XBsn0Field->new($b,$p);
		push @l2h, XBibField->new($b,$p);
		push @l2h, XFsnField->new($b,$p);
		push @l2h, XFsn0Field->new($b,$p);
		push @l2h, XFibField->new($b,$p);
		push @l2h, ($li = XliField->new($b,$p));
		push @l2h, Xli0Field->new($b,$p);
		push @l2h, XjmpField->new($b,$p) if $channel->{pr} == MP_JAPAN;
		$p += HT_EXTENDED;
	} else {
		push @l2h, BsnField->new($b,$p);
		push @l2h, BibField->new($b,$p);
		push @l2h, FsnField->new($b,$p);
		push @l2h, FibField->new($b,$p);
		push @l2h, ($li = LiField->new($b,$p));
		if ($channel->{pr} == MP_JAPAN) {
			push @l2h, JmpFiled->new($b,$p);
		} else {
			push @l2h, LiField->new($b,$p);
		}
		$p += HT_BASIC;
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
	if ($channel->{rt} == RT_24BIT_PC) {
		push @rl, Dpc24Field->new($b,$p);
		push @rl, Opc24Field->new($b,$p);
		push @rl, Sls24Field->new($b,$p);
		$p += RT_24BIT_PC;
	} else {
		push @rl, Dpc14Field->new($b,$p);
		push @rl, Opc14Field->new($b,$p);
		push @rl, Slc14Field->new($b,$p);
		$p += RT_14BIT_PC;
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
	if ($self->{channel}->{ht} == HT_EXTENDED) {
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
			if ($self->{channel}->{rt} == RT_24BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$o) = @_;
	if ($self->{channel}->{rt} == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$o) = @_;
	if ($self->{channel}->{rt} == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$o) = @_;
	if ($self->{channel}->{rt} == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$o,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$o,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$o) = @_;
	if ($self->{channel}->{rt} == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$o,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$o,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{cmi} = CmiIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CmcMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{cmi} = CmiIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CmrjMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_24BIT_PC) {
		$self->{faci} = FaciIsupParameter->man($b,$p,$e,$rt);
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package FaiMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_24BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		#FIXME only for Singapore
		$self->{icci} = IcciIsupParameter->man($b,$p,$e,$rt);
		#FIXME only for Singapore and Spain
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package NrmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package FacMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package UptMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package UpaMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package IdrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package IrsMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package SgmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CraMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_24BIT_PC) {
		return;
	}
}
# -------------------------------------
package CrmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_24BIT_PC) {
		$self->{nci} = NciIsupParameter->man($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CvrMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_24BIT_PC) {
		$self->{cvri} = CvriIsupParameter->man($b,$p,$e,$rt);
		$self->{cgci} = CgciIsupParameter->man($b,$p,$e,$rt);
	}
}
# -------------------------------------
package CvtMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_24BIT_PC) {
		return;
	}
}
# -------------------------------------
package ExmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_24BIT_PC) {
		$self->{oparms} = $self->oparms($b,$p,$e,$rt);
	}
}
# -------------------------------------
package NonMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		#FIXME Spain only
		$self->{ton} = TonIsupParameter->man($b,$p,$e,$rt);
	}
}
# -------------------------------------
package LlmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		#FIXME Spain only
		return;
	}
}
# -------------------------------------
package CakMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		#FIXME Singapore only
		return;
	}
}
# -------------------------------------
package TcmMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
		#FIXME Singapore only
		$self->{cri} = CriIsupParameter->man($b,$p,$e,$rt);
		return;
	}
}
# -------------------------------------
package McpMessage; use strict; use vars qw(@ISA); @ISA = qw(IsupMessage);
# -------------------------------------
use constant {
	RT_UNKNOWN => 0,
	RT_14BIT_PC => 4, # also RL length in octets
	RT_24BIT_PC => 7  # also RL length in octets
};
sub decode {
	my ($self,$b,$p,$e,$rt) = @_;
	if ($rt == RT_14BIT_PC) {
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
				$top->{network}->regroupsps($top) if $top->{network};
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
		circuits=>1,
		assocs=>1,
		relations=>1,
		routesets=>0,
		routes=>1,
		paths=>0,
		combineds=>0,
		linksets=>1,
		links=>0,
		channels=>0,
		sps=>1,
		ssps=>1,
		gtts=>1,
		stps=>1,
		scps=>1,
		aliases=>1,
		all=>1,
	};
	$self->{view} = 0; # full
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
				$c->itemconfigure('route',-state=>'normal');
				$self->{show}->{routes} = 1;
				$c->itemconfigure('path',-state=>'normal');
				$self->{show}->{paths} = 1;
				$c->itemconfigure('linkset',-state=>'normal');
				$self->{show}->{linksets} = 1;
				$c->itemconfigure('channel',-state=>'normal');
				$self->{show}->{channels} = 1;
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
		} else {
			my $ms = (-$secs*1000) + int((-$usec+999)/1000);
			$w->after($ms, [\&MyTop::contmsg, $self, $msg]);
			return;
		}

	} while ($msg = Message->create($self->{pcap}));
	Net::Pcap::pcap_close($self->{pcap});
	delete $self->{pcap};
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

my $xml = XMLout($pc_assignments);
print $xml;

#my $handler = MyParser->new();
#my $p = XML::SAX::ParserFactory->parser(Handler => $handler);
#$p->parse_usr($filename);

exit;

1;
