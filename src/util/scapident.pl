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
	254=>['AT&T',	    'AT&T Communications'],
	253=>['Sprint',	    'US Sprint'],
	252=>['BellSouth',  'BellSouth Services'],
	#251=>['Pacific',    'Pacific Bell'],
	251=>['Pacific',    'Pacific Telesis'],
	#250=>['Ameritech',  'Ameritech'],
	250=>['Ameritech',  'Ameritech Services'],
	249=>['SWB',	    'Southwestern Bell Telephone'],
	#248=>['Bell',	    'Bell Tri-Co Services'],
	248=>['US WEST',    'US WEST Communications'],
	247=>['Nynex',	    'Nynex Service Co.'],
	246=>['BA',	    'Bell Atlantic'],
	#245=>['Canada',	    'Telecom Canada'],
	245=>['Canada',	    'AT&T Canada Long Distance Services'],
	244=>['MCI',	    'MCI Telecommunications Group'],
	243=>['SNET',	    'Southern New England Telephone'],
	242=>['Allnet',	    'Allnet Communications Services Inc.'],
	241=>['DCA',	    'Defense Communications Agency'],
	240=>['GTE',	    'GTE Service Corporation/Telephone Operations'],
	#239=>['United',	    'United Telephone Sys'],
	239=>['Sprint',	    'Sprint Local Telecommunications Division'],
	238=>['Independent','Independent Telecommunications Network'],
	237=>['C&W',	    'Cable & Wireless'],
	#236=>['CNCP',	    'CNCP Telecommunications'],
	236=>['Canada',	    'AT&T Canada Long Distance Services'],
	#235=>['Comtel',	    'Comtel'],
	235=>['CONTEL',	    'CONTEL (GTE Wireless)'],
	234=>['Alltel',	    'Alltel'],
	#233=>['Rochester',  'Rochester Telephone'],
	233=>['Frontier',   'Frontier Communications Int\'l, Inc.'],
	232=>['Century',    'Century Telephone Enterprises, Inc.'],
	231=>['AT&T',	    'AT&T (VSN)'],
	#230=>['Centel',	    'Centel'],
	230=>['Sprint',	    'Sprint Local Telecommunications Division'],
	229=>['Test',	    'Test Code'],
	# Point codes within this network identifier may be assigned by
	# any network operator to real or simulated signalling points
	# for test purposes.  Due to possible duplicated assigments of
	# these codes, messages address with one of these point codes
	# should not be sent across network boudnaries except with
	# agreement of the networks involved.
	228=>['Pacific',    'Pacific Telecom'],
	227=>['NACN',	    'North America Cellular Network'],
	226=>['Cantel',	    'Cantel'],
	225=>['Lincoln',    'Lincoln Telephone Company'],
	224=>['Cincinnati', 'Cincinnati Bell Telephone'],
	223=>['TDS',	    'TDS Telecom'],
	222=>['MEANS/INS/SDN',	'MEANS/INS/SDN'],
	221=>['CTIA',	    'CTIA'],
	220=>['SWBM',	    'Southwestern Bell Mobile Systems'],
	219=>['Nextel',	    'Nextel (was OneComm, Inc.)'],
	218=>['Citizens',   'Citizens Telecom'],
	217=>['ICG',	    'ICG Network Services'],
	216=>['MCImetro',   'MCI Metro'],
	215=>['AT&T',	    'AT&T'],
	214=>['Sprint',	    'Sprint Spectrum L.P.'],
	213=>['Canada',	    'Mobility Canada'],
	212=>['Teleglobe',  'Teleglobe USA'],
	211=>['Omnipoint',  'Omnipoint Communications Inc.'],
	210=>['NextWave',   'NextWave Telecom'],
	209=>['BellSouth',  'BellSouth Enterprises'],
	208=>['GST',	    'GST Telecom'],
);
my %small_networks = (
	1=>{
		  1=>['Puerto Rico',	'Puerto Rico Telephone Company'],
		  2=>['Cincinnati',	'Cincinnati Bell'],
		  #3=>['LDS',		'LDS Metromedia Ltd'],
		  3=>['WorldCom',	'WorldCom'],
		  4=>['Schneider ',	'Schneider Communications'],
		  #5=>['Microtel',	'Microtel Inc'],
		  5=>['WorldCom',	'WorldCom'],
		  6=>['ACC',		'ACC Long Distance Corp'],
		  7=>['SouthernNet',	'SouthernNet'],
		  8=>['Teleconnect',	'Teleconnect Co'],
		  #9=>['Telepshere',	'Telesphere Network'],
		  #10=>['MidAmerican',	'MidAmerican'],
		  9=>['WorldCom',	'WorldCom'],
		 10=>['WorldCom',	'WorldCom'],
		 11=>['LDUSA',		'Long Distance USA/Sprint'],
		 12=>['RCI',		'RCI Corporation'],
		 13=>['Phoenix',	'Phoenix Network'],
		 14=>['Teledial',	'Teledial America'],
		 #15=>['USTS',		'U.S. Transmission Systems'],
		 15=>['Antigua',	'Antigua Public Utilities Authority'],
		 #16=>['Litel',		'Litel Telecommunication Corp'],
		 16=>['LCI',		'LCI International/Litel'],
		 17=>['Chadwick',	'Chadwick Telephone'],
		 18=>['LDS',		'Long Distance Service Inc'],
		 19=>['WITS',		'Washington Interagency Telecommunications Service'],
		 20=>['PBS',		'Phone Base Systems, Inc'],
		 21=>['General',	'General Communication Incorporated'],
		 22=>['CTI',		'CTI Telecommunications Incorporated'],
		 #23=>['SouthTel',	'SouthTel'],
		 23=>['MicroNet',	'MicroNet Communications Group, Inc.'],
		 24=>['Roseville',	'Roseville Telephone Company'],
		 #25=>['TCL',		'TelaMarketing Corporation of Louisiana'],
		 25=>['Rio',		'Rio Communications'],
		 #26=>['LDDS',		'LDDS'],
		 26=>['WorldCom',	'WorldCom'],
		 27=>['Capital',	'Capital Telecommunications, Inc.'],
		 28=>['Transtel',	'Transtel Corporation'],
		 #29=>['ComSystem',	'ComSystem'],
		 29=>['WorldCom',	'WorldCom'],
		 30=>['Mid Altantic',	'Mid Altantic/Frontier'],
		 #33=>['NTC',		'NTC Inc.'],
		 33=>['WorldCom',	'WorldCom'],
		 34=>['NDC',		'National Data Corporation'],
		 35=>['FTC',		'FTC Communications, Inc.'],
		 #36=>['Lincoln',	'Lincoln Telephone Company'],
		 36=>['MediaOne',	'Media One'],
		 37=>['Alascom',	'Alascom'],
		 38=>['Motorola',	'Motorola, Inc.'],
		 39=>['West Coast',	'West Coast Telecommunications'],
		 40=>['LDS',		'Long Distance Savers, Inc./LDS'],
		 #41=>['EdTel',		'Ed Tel'],
		 41=>['TCE',		'TELUS Communications (Edmonton) Inc./TCE'],
		 42=>['NPT',		'North Pittsburg Telephone Company'],
		 43=>['Rock Hill',	'Rock Hill Telephone Company'],
		 44=>['Teleglobe',	'Teleglobe Canada'],
		 45=>['Jamaica',	'Jamaica Telephone Company Limited'],
		 46=>['Sugarland',	'Sugarland Telephone Company'],
		 47=>['Lakedale',	'Lakedale Telephone Company'],
		 48=>['Chillicothe ',	'Chillicothe Telephone Company'],
		 49=>['NSTC',		'North State Telephone Company'],
		 50=>['PSTC',		'Public Service Telephone Company'],
		 51=>['Cincinnati',	'Cincinnati Bell Long Distance'],
		 52=>['NTX',		'National Telephone Exchange/NTX'],
		 53=>['Lake States',	'Lake States Long Distance'],
		 54=>['NTF',		'National Telecommunications of Florida'],
		 55=>['DET&T',		'Denver & Ephrata Telephone & Telegraph Company'],
		 56=>['VarTec',		'VarTec National, Incorporated'],
		 57=>['ETS',		'Eastern Telephone Systems, Inc.'],
		 58=>['TEXP',		'Telephone Express'],
		 59=>['SNET',		'SNET Protocol Conversion'],
		 60=>['EATC',		'East Ascension Telephone Company, Inc.'],
		 61=>['Codetel',	'Codetel'],
		 62=>['KTC',		'Kerrville Telephone Company'],
		 63=>['BCTC',		'Barry County Telephone Company'],
		 64=>['CALNET',		'CALNET'],
		 65=>['DET&T',		'Denver & Ephrata Telephone & Telegraph Company'],
		 66=>['DET&T',		'Denver & Ephrata Telephone & Telegraph Company'],
		 67=>['NetServ',	'Network Services'],
		 68=>['RCTC',		'Redwood County Telephone Company/Clements Telephone Company'],
		 69=>['HTHT',		'Horry Telephone/Hargray Telephone'],
		 70=>['HTHT',		'Horry Telephone/Hargray Telephone'],
		 71=>['HTHT',		'Horry Telephone/Hargray Telephone'],
		 72=>['HTHT',		'Horry Telephone/Hargray Telephone'],
		 73=>['CGRP',		'CommuniGroup, Inc.'],
		 74=>['CTC',		'The Concord Telephone Company'],
		 75=>['CTC',		'The Concord Telephone Company'],
		 76=>['CTC',		'The Concord Telephone Company'],
		 77=>['CTC',		'The Concord Telephone Company'],
		 78=>['WKRTC',		'West Kentucky Rural Telephone Cooperative'],
		 79=>['PTA',		'Pioneer Telephone Association'],
		 80=>['DCTCC',		'Duo County Telephone Cooperative Corporation'],
		 81=>['CFW',		'CFW Telephone'],
		 82=>['LCTE',		'Lufkin-Conroe Telephone Exch., Inc.'],
		 83=>['LCTE',		'Lufkin-Conroe Telephone Exch., Inc.'],
		 84=>['LCTE',		'Lufkin-Conroe Telephone Exch., Inc.'],
		 85=>['Puerto Rico',	'Puerto Rico Telephone Company'],
		 86=>['Puerto Rico',	'Puerto Rico Telephone Company'],
		 87=>['Puerto Rico',	'Puerto Rico Telephone Company'],
		 88=>['ICTC',		'Illinois Consolidated Telphone Company'],
		 89=>['WestHouse',	'Westinghouse Communications'],
		 90=>['ATU',		'Anchorage Telephone Utility'],
		 91=>['USWEST',		'US WEST New Vector Group'],
		 92=>['USWEST',		'US WEST New Vector Group'],
		 93=>['USWEST',		'US WEST New Vector Group'],
		 94=>['USWEST',		'US WEST New Vector Group'],
		 95=>['Lakedale',	'Lakedale Telephone Company'],
		 96=>['Lakedale',	'Lakedale Telephone Company'],
		 97=>['Lakedale',	'Lakedale Telephone Company'],
		 98=>['CTTC',		'Central Texal Telephone Cooperative'],
		 99=>['CTTC',		'Central Texal Telephone Cooperative'],
		100=>['CTTC',		'Central Texal Telephone Cooperative'],
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
		111=>['Common',		'Commonwealth Telephone Company'],
		112=>['WorldCom',	'WorldCom'],
		113=>['Matanuska',	'Matanuska Telephone Association, Inc.'],
		114=>['Reserve',	'Reserve Telephone Company'],
		115=>['Brazoria',	'Brazoria Telephone Company'],
		116=>['Common',		'Commonwealth Telephone Company'],
		117=>['Common',		'Commonwealth Telephone Company'],
		118=>['USC',		'United States Cellular'],
		119=>['USC',		'United States Cellular'],
		120=>['Videotron',	'Videotron Telecom Ltee.'],
		121=>['Novus',		'Novus Telecom Inc.'],
		122=>['BA Mobile',	'Bell Atlantic Mobile'],
		123=>['USLink',		'USLink Long Distance'],
		124=>['NEPTC',		'North-Eastern Pennsylvania Telephone Company'],
		125=>['Gulf',		'Gulf Telephone Company'],
		126=>['Gulf',		'Gulf Telephone Company'],
		127=>['BA Mobile',	'Bell Atlantic Mobile'],
		128=>['BA Mobile',	'Bell Atlantic Mobile'],
		129=>['BA Mobile',	'Bell Atlantic Mobile'],
		130=>['NYNEX',		'NYNEX Mobile Communications'],
		131=>['Ameritech',	'Ameritech Cellular Services, Inc.'],
		132=>['YVTMC',		'Yadkin Valey Telephone Membership Corporation'],
		133=>['Intermedia',	'Intermedia Communications, Inc.'],
		134=>['Advantis',	'Advantis'],
		135=>['Advantis',	'Advantis'],
		136=>['Advantis',	'Advantis'],
		137=>['Skyline',	'Skyline Tel. Membership Corporation'],
		138=>['Pac Tel',	'Pac Tel Corporation'],
		139=>['Com Net',	'Com Net'],
		140=>['Hickory',	'Hickory Telephone Company'],
		141=>['Barbados',	'Barbados Telephone Company, Ltd.'],
		142=>['Barbados',	'Barbados Telephone Company, Ltd.'],
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
		158=>['Trinidad',	'Telecommunication Services of Trinidad/Tobago'],
		159=>['PMTC',		'Project Mutual Telephone Company'],
		160=>['Sprint',		'Sprint Canada'],
		161=>['WCCTA',		'Webster Calhoun Cooperative Telephone Association'],
		162=>['Trenton',	'Trenton Telephone Company/Ringgold Telephone Company'],
		163=>['WilTel',		'WilTel'],
		164=>['fONOROLA',	'fONOROLA'],
		165=>['Comcast',	'Comcast Cellular'],
		166=>['MicroCell',	'MicroCell 1-2-1 Inc.'],
		167=>['ITC',		'Independent Telephone Company'],
		168=>['Ameritech',	'Ameritech Communication Inc.'],
		169=>['Ameritech',	'Ameritech Communication Inc.'],
		170=>['Ameritech',	'Ameritech Communication Inc.'],
		171=>['Ameritech',	'Ameritech Communication Inc.'],
		172=>['Pembroke',	'Pembroke Telephone Company'],
		173=>['PacBell',	'Pacific Bell Mobile Services'],
		174=>['BellSouth',	'BellSouth PCS'],
		175=>['Brooks',		'Brooks Fiber Communications'],
		176=>['Intermedia',	'Intermedia Communications, Inc.'],
		177=>['PrimeCO',	'PrimeCO Personal Communications, L.P.'],
		178=>['Vital',		'Vital Inc.'],
		179=>['Western',	'Western Wireless Corporation'],
		180=>['SNS',		'SNS Shared Network Scvs. Inc.'],
		181=>['BellSouth',	'BellSouth Cellular Corporation'],
		182=>['BellSouth',	'BellSouth Cellular Corporation'],
		183=>['PBTC',		'Pine Belt Telephone Company, Inc.'],
		184=>['Avantel',	'Avantel S.A.'],
		185=>['Classic',	'Classic Telephone'],
		186=>['US One',		'US One Communications, Inc.'],
		187=>['Clearnet',	'Clearnet Communications, Inc.'],
		188=>['Clearnet',	'Clearnet Communications, Inc.'],
		189=>['Sierra',		'Sierra Cellular, Inc.'],
		190=>['American',	'American Telco., Inc.'],
		191=>['American',	'American Portable Telecom'],
		192=>['American',	'American Portable Telecom'],
		193=>['American',	'American Portable Telecom'],
		194=>['WorldCom',	'WorldCom'],
		195=>['WorldCom',	'WorldCom'],
		196=>['WorldCom',	'WorldCom'],
		197=>['WorldCom',	'WorldCom'],
		198=>['WorldCom',	'WorldCom'],
		199=>['WorldCom',	'WorldCom'],
		200=>['WorldCom',	'WorldCom'],
		201=>['WorldCom',	'WorldCom'],
		202=>['WorldCom',	'WorldCom'],
		203=>['Global',		'Global Telemedia International, Inc.'],
		204=>['BA NYNEX',	'Bell Atlantic NYNEX Mobile'],
		205=>['BA NYNEX',	'Bell Atlantic NYNEX Mobile'],
		206=>['BA NYNEX',	'Bell Atlantic NYNEX Mobile'],
		207=>['BA NYNEX',	'Bell Atlantic NYNEX Mobile'],
		208=>['BA NYNEX',	'Bell Atlantic NYNEX Mobile'],
		209=>['BA NYNEX',	'Bell Atlantic NYNEX Mobile'],
		210=>['BA NYNEX',	'Bell Atlantic NYNEX Mobile'],
		211=>['BA NYNEX',	'Bell Atlantic NYNEX Mobile'],
		212=>['Headwaters',	'Headwaters Telephone Company'],
		213=>['Wood',		'Wood County Telephone Company'],
		214=>['Pocket',		'Pocket COmmunications, Inc.'],
		215=>['BA Virginia',	'Bell Atlantic - Virginia, Inc.'],
		216=>['360',		'360 Communications'],
		217=>['Bermuda',	'TeleBermuda International Ltd.'],
		218=>['Cox',		'Cox Communication'],
		219=>['Winstar',	'Winstar Telecommunications Group'],
		220=>['MIDCOM',		'MIDCOM Communications, Inc.'],
		221=>['Excel',		'Excel Switch Facility, Inc.'],
		222=>['Western',	'Western Wireless Corporation'],
		223=>['TeleHub',	'TeleHub Network SErvices Corporation'],
		224=>['IXC',		'IXC Communication, Inc.'],
		225=>['PrimeCo',	'PrimeCo Personal Communications'],
		226=>['PrimeCo',	'PrimeCo Personal Communications'],
		227=>['TPC',		'The Phone Company'],
		228=>['Iridium',	'Iridium LLC'],
		229=>['Teligent',	'Teligent'],
		230=>['SpectraNet',	'SpectraNet Orange'],
		231=>['USX',		'US Xchange, L.L.C.'],
		232=>['BA Mobile',	'Bell Atlantic Mobile'],
		233=>['BA Mobile',	'Bell Atlantic Mobile'],
		234=>['BA Mobile',	'Bell Atlantic Mobile'],
		235=>['BA Mobile',	'Bell Atlantic Mobile'],
		236=>['BA Mobile',	'Bell Atlantic Mobile'],
		237=>['Oregon',		'Oregon Telco Services, Inc.'],
		238=>['Telecorp',	'Telecorp Holding Corporation, Inc.'],
		239=>['Magnacom',	'Magnacom, LLC dba PCS Plus, Inc.'],
		240=>['Global Naps',	'Global Naps'],
		241=>['Tiniton',	'Triniton PCS Inc.'],
		242=>['Strikenet',	'Strikenet'],
		243=>['Connect',	'Connect Holding Corporation'],
		244=>['NetPlus',	'Network Plus'],
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
	1=>{
		  0=>['Cleartel',	'Cleartel Communications'],
		  4=>['Business',	'Business Telecom'],
		  #8=>['PA',		'Phone America'],
		  8=>['WorldCOm',	'WorldCom'],
		 12=>['National',	'National Telephone'],
		 #16=>['Vyvx',		'Vyvx Telecom'],
		 20=>['NTA',		'National Telecommunications of Austin'],
		 #24=>['FPNE',		'First Phone of New England'],
		 24=>['WorldCom',	'WorldCom'],
		 28=>['SIS',		'Southern Interexchange Services, Inc.'],
		 32=>['NTS',		'NTS Communications'],
		 36=>['Digital',	'Digital Network'],
		 40=>['ATX',		'ATX Telecommunications Services'],
		 #44=>['Vyvx',		'Vyvx Telecom'],
		 48=>['NNC',		'National Network Corp'],
		 52=>['StarTel',	'StarTel'],
		 56=>['TMC',		'TMC of San Diego'],
		 #60=>['First',		'First Phone'],
		 #64=>['CNI',		'CNI'],
		 #68=>['WorldCom',	'WorldCom'],
		 72=>['WT&EC',		'Wilkes Telephone & Electric Company'],
		 76=>['Sandhill',	'Sandhill Telephone Cooperative'],
		 80=>['Chester',	'Chester Telephone'],
		 84=>['SJT&T',		'St. Joseph Telephone & Telegraph Company'],
		 88=>['ITC',		'Interstate Telephone Company'],
		 92=>['VUT',		'Vista-United Telecommunications'],
		 96=>['Brandenburg',	'Brandenburg Telephone Company'],
		 #100=>['FirstPhone',	'FirstPhone'],
		104=>['Chesnee',	'Chesnee Telephone Company'],
		#108=>['Hargray',	'Hargray Telephone Company'],
		108=>['Access',		'Access Long Distance'],
		112=>['LaFourche',	'LaFourche Telephone Company'],
		116=>['Evans',		'Evans Telephone Company'],
		#120=>['ICTC',		'Illinois Consolidated Telephone Company'],
		120=>['JBN',		'JBN Telephone Company, Inc.'],
		#124=>['ICTC',		'Illinois Consolidated Telephone Company'],
		124=>['Iowa',		'Iowa Communications Network'],
		128=>['Roanoke',	'Roanoke Telephone Company'],
		#132=>['INS',		'Iowa Network Services'],
		132=>['IEX',		'IEX\'s Corporation'],
		136=>['Telefonica',	'Telefonica Larga Distancia'],
		#140=>['Cherokee',	'Cherokee Telephone Company'],
		140=>['Alabama',	'National Telephone of Alabama'],
		144=>['Sears',		'Sears Technology Services, Inc.'],
		148=>['Phone One',	'Phone One'],
		152=>['Orwell',		'Orwell Telephone Company'],
		156=>['Call-Net',	'Call-Net Telecommunications Ltd.'],
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
		200=>['Peoples',	'Peoples Mutual Telephone Company'],
		204=>['BA Mobile',	'Bell Atlantic Mobile Systems'],
		208=>['Logan',		'Logan Telephone Cooperative'],
		212=>['TresCom',	'TresCom U.S.A., Inc.'],
		216=>['Rochester',	'Rochester Telephone Company'],
		220=>['Ponderosa',	'Ponderosa Telephone Company'],
		224=>['Ponderosa',	'Ponderosa Telephone Company'],
		228=>['Ponderosa',	'Ponderosa Telephone Company'],
		232=>['Ellijay',	'Ellijay Telephone Company'],
		236=>['American',	'American Telephone Company'],
		240=>['Loretto',	'Loretto Telephone Company, Inc.'],
		244=>['Millry',		'Millry Telephone Company, Inc.'],
		248=>['NW Ind.',	'Northwest Indiana Telephone Company'],
		252=>['Pond Br.',	'Pond Branch Telephone Company, Inc.'],
	},
	2=>{
		  0=>['Farmers',	'Farmers Telephone Cooperative (AL.)'],
		  4=>['Total-Tel',	'Total-Tel USA'],
		  8=>['Farmers',	'Farmers Telephone Cooperative (SC.)'],
		 12=>['Farmers',	'Farmers Telephone Cooperative (SC.)'],
		 16=>['Farmers',	'Farmers Telephone Cooperative (SC.)'],
		 20=>['Farmers',	'Farmers Telephone Cooperative (SC.)'],
		 24=>['Bentley',	'Bentleyville Telephone Company'],
		 28=>['Communique',	'Communique Telecomm.'],
		 32=>['Santa Rosa',	'Santa Rosa Telephone Cooperative'],
		 36=>['SW Oklahoma',	'Southwest Oklahoma Telephone Company'],
		 40=>['SW Oklahoma',	'Southwest Oklahoma Telephone Company'],
		 44=>['Piedmont',	'Piedmont Rural Telephone Cooperative'],
		 48=>['Piedmont',	'Piedmont Rural Telephone Cooperative'],
		 52=>['LDD',		'LDD, Inc.'],
		 56=>['Saco River',	'Saco River Telephone & Telegraph Company'],
		 60=>['Bruce',		'Bruce Telephone Company'],
		 64=>['Bledsoe',	'Bledsoe Telephone Cooperative'],
		 68=>['Bledsoe',	'Bledsoe Telephone Cooperative'],
		 72=>['Foothills',	'Foothills Rural Telephone Cooperative'],
		 76=>['Germantown',	'Germantown Telephone Company, Inc.'],
		 80=>['Elkhart',	'Elkhart Telephone Company'],
		 84=>['Yell County',	'Yell County Telephone Company'],
		 88=>['Yell County',	'Yell County Telephone Company'],
		 92=>['Calif-Oregon',	'California-Oregon Telephone Company'],
		 96=>['Crown',		'Crown Point Telephone'],
		100=>['Berkshire',	'Berkshire Telephone Corporation'],
		104=>['Cobboss.',	'Cobbosseecontee Telephone & Telegraph Company'],
		108=>['Siskiyou',	'Siskiyou Telephone Company'],
		112=>['Siskiyou',	'Siskiyou Telephone Company'],
		116=>['Millry',		'Millry Telephone Company, Inc.'],
		120=>['Mountain',	'Mountain Rural Telephone'],
		124=>['Taconic',	'Taconic Telephone Corporation'],
		128=>['Champlain',	'Champlain Telephone Company'],
		132=>['Planters',	'Planters Rural Telephone Cooperative, Inc.'],
		136=>['Dekalb',		'Dekalb Telephone Cooperative'],
		140=>['Citizens',	'Citizens Telephone Cooperative'],
		144=>['Bulloch',	'Bulloch Telephone Cooperative, Inc.'],
		148=>['NE Florida',	'Northeast Florida Telephone Company, Inc.'],
		152=>['Camden',		'Camden Telephone and Telegraph Company'],
		156=>['N Arkansas',	'Northern Arkansas Telephone Company'],
		160=>['Access',		'Acess Long Distance'],
		164=>['Access',		'Acess Long Distance'],
		168=>['Access',		'Acess Long Distance'],
		172=>['Ballard',	'Ballard Rural Telephone Cooperative'],
		176=>['Hancock',	'Hancock Rural Telephone Company'],
		180=>['Madison',	'Madison County Telephone Company'],
		184=>['Valu-Line',	'Valu-Line Long Distance'],
		188=>['Mtn View',	'Mountain View Telephone Company'],
		192=>['Mtn View',	'Mountain View Telephone Company'],
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
		 28=>['Wheat St',	'Wheat State Telephone, Inc.'],
		 32=>['Wheat St',	'Wheat State Telephone, Inc.'],
		 36=>['Westel',		'Westel'],
		 40=>['Westel',		'Westel'],
		 44=>['Westel',		'Westel'],
		 48=>['Smithville',	'Smithville Telephone'],
		 52=>['Smithville',	'Smithville Telephone'],
		 56=>['Smithville',	'Smithville Telephone'],
		 60=>['Blackfoot',	'Blackfoot Telephone Cooperative, Inc.'],
		 64=>['Blackfoot',	'Blackfoot Telephone Cooperative, Inc.'],
		 68=>['Blackfoot',	'Blackfoot Telephone Cooperative, Inc.'],
		 72=>['American',	'American Sharecom'],
		 76=>['American',	'American Sharecom'],
		 80=>['American',	'American Sharecom'],
		 84=>['Spencer',	'Perry Spencer Rural Telephone Cooperative'],
		 88=>['Stellville',	'Stellville Telephone Exchange'],
		 92=>['Chibardun',	'Chibardun Telephone Cooperative, Inc.'],
		 96=>['Chibardun',	'Chibardun Telephone Cooperative, Inc.'],
		100=>['S Central',	'South Central Rural Telephone Cooperative Corporation, Inc.'],
		104=>['United',		'United Telephone Company, Inc.'],
		108=>['National',	'National TeleService'],
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
		164=>['Palmetto',	'Palmetto Rural Telephone Company'],
		168=>['Green Hills',	'Green Hills Telephone Company'],
		172=>['Conestoga',	'Conestoga Telephone & Telegraph Company'],
		176=>['Roanoke',	'Roanoke & Botetourt Telephone Company'],
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
		224=>['Spencer',	'Perry Spencer Rural Telephone Cooperative'],
		228=>['Utelco',		'Utelco, Inc.'],
		232=>['Ontario',	'Ontario Telephone Compan, Inc.'],
		236=>['Germantown',	'Germantown Independent Telephone'],
		240=>['Kerman',		'Kerman Telephone Company.Inc.'],
		244=>['Hayneville',	'Hayneville Telephone Company, Inc.'],
		248=>['Bruce',		'Bruce Municipal Telephone System'],
		252=>['Granite',	'Granite State Telephone'],
	},
	4=>{
		  0=>['Skyline',	'Skyline Telephone Membership Corporation'],
		  4=>['Skyline',	'Skyline Telephone Membership Corporation'],
		  8=>['Skyline',	'Skyline Telephone Membership Corporation'],
		 12=>['Skyline',	'Skyline Telephone Membership Corporation'],
		 16=>['Community',	'Community Service Telephone Company'],
		 20=>['Community',	'Community Service Telephone Company'],
	         24=>['Headwaters',	'Headwaters Telephone Company'],
		 28=>['W Tennessee',	'West Tennessee Telephone Company'],
		 32=>['Peoples',	'Peoples Mutual Telephone Company'],
		 36=>['Crockett',	'Crockett Telephone Company'],
		 40=>['Bay Springs',	'Bay Springs Telephone Company, Inc.'],
		 44=>['Ellerbe',	'Ellerbe Telephone Company'],
		 48=>['Pineville',	'Pineville Telephone Company'],
		 52=>['Randolph',	'Randolph Telephone Company'],
		 56=>['Lomand',		'Ben Lomand Rural Telephone Cooperative, Inc.'],
		 60=>['Lomand',		'Ben Lomand Rural Telephone Cooperative, Inc.'],
		 64=>['Lomand',		'Ben Lomand Rural Telephone Cooperative, Inc.'],
		 68=>['Lomand',		'Ben Lomand Rural Telephone Cooperative, Inc.'],
		 72=>['Mountain',	'Mountain Rural Telephone'],
		 76=>['Gila River',	'Gila River Telecommunications, Inc.'],
		 80=>['Gila River',	'Gila River Telecommunications, Inc.'],
		 84=>['Gila River',	'Gila River Telecommunications, Inc.'],
		 88=>['NE Missouri',	'NE Missouri Rural Telephone Company'],
		 92=>['Highland',	'Highland Telephone Company'],
		 96=>['Highland',	'Highland Telephone Company'],
		100=>['Highland',	'Highland Telephone Company'],
		104=>['RTSC',		'Rural Telephone Service Company, Inc.'],
		108=>['RTSC',		'Rural Telephone Service Company, Inc.'],
		112=>['Peoples',	'Peoples Rural Telephone Cooperative Corporation'],
		116=>['EDS',		'EDS Personal Communicaitons Corporation'],
		120=>['EDS',		'EDS Personal Communicaitons Corporation'],
		124=>['West Side',	'West Side Telephone'],
		128=>['Citizens',	'Citizens Telephone Cooperative, Inc.'],
		132=>['W Carolina',	'West Carolina Rural Telephone Cooperative, Inc.'],
		136=>['W Carolina',	'West Carolina Rural Telephone Cooperative, Inc.'],
		140=>['Twin Valley',	'Twin Valley Telephone, Inc.'],
		144=>['Twin Valley',	'Twin Valley Telephone, Inc.'],
		#148=>['SPARE',		'SPARE - UNASSIGNED'],
		#152=>['SPARE',		'SPARE - UNASSIGNED'],
		156=>['Teleport',	'Teleport Valley Telephone, Inc.'],
		160=>['Teleport',	'Teleport Valley Telephone, Inc.'],
		164=>['Teleport',	'Teleport Valley Telephone, Inc.'],
		168=>['Teleport',	'Teleport Valley Telephone, Inc.'],
		172=>['Atlas',		'Atlas Telephone Company'],
		176=>['Atlas',		'Atlas Telephone Company'],
	        180=>['Hickory',	'Hickory Telephone Company'],
	        184=>['Spavinaw',	'Salina Spavinaw Telephone Company, Inc.'],
	        188=>['Spavinaw',	'Salina Spavinaw Telephone Company, Inc.'],
	        192=>['La Ward',	'La Ward Telephone Exchange, Inc.'],
	        196=>['Rib Lake',	'Rib Lake Telephone Company'],
		200=>['Citizens',	'Citizens Telephone Company of Kecksburg'],
		204=>['Kingdom',	'Kingdom Telephone Company'],
		208=>['Kingdom',	'Kingdom Telephone Company'],
		212=>['Kingdom',	'Kingdom Telephone Company'],
		216=>['Kingdom',	'Kingdom Telephone Company'],
		220=>['Carnegie',	'Carnegie Telephone Company'],
		224=>['Ace',		'Ace Telephone Company of Michigan, Inc.'],
		228=>['Ace',		'Ace Telephone Company of Michigan, Inc.'],
		232=>['Hurontario',	'Hurontario Telephone Ltd.'],
		236=>['N Norwich',	'North Norwich Telephones Ltd.'],
		240=>['Durham',		'Durham Telephones Ltd.'],
		244=>['Vadacom',	'Vadacom'],
		248=>['Vadacom',	'Vadacom'],
		252=>['Gosfiled',	'Gosfiled North Municipal Telephone Ssytem'],
	},
	5=>{
		  0=>['NE Missouri',	'NE Missouri Rural Telephone Company'],
		  4=>['NE Missouri',	'NE Missouri Rural Telephone Company'],
		  8=>['NE Missouri',	'NE Missouri Rural Telephone Company'],
		 12=>['Corporate',	'Corporate Telemanagement Group'],
		 16=>['Corporate',	'Corporate Telemanagement Group'],
		 20=>['Mustang',	'Mustang Telecom, Inc.'],
		 24=>['Mustang',	'Mustang Telecom, Inc.'],
		 28=>['Mustang',	'Mustang Telecom, Inc.'],
		 32=>['Zenda',		'Zenda Telephone Company, Inc.'],
		 36=>['San Marcos',	'San Marcos Telephone Company'],
		 40=>['Pymatunning',	'Pymatunning Independent Telephone Company'],
		 44=>['Pine',		'Pine Telephone Company'],
		 48=>['Pine',		'Pine Telephone Company'],
		 52=>['Ft Mojave',	'Fort Mojave Telecommunications, Inc.'],
		 56=>['Ft Mojave',	'Fort Mojave Telecommunications, Inc.'],
		 60=>['Oklahoma',	'Oklahoma Western Telephone Company'],
		 64=>['Oklahoma',	'Oklahoma Western Telephone Company'],
		 68=>['Oklahoma',	'Oklahoma Western Telephone Company'],
		 72=>['Oklahoma',	'Oklahoma Western Telephone Company'],
		 76=>['Hinton',		'Hinton Telephone Company'],
		 80=>['Hinton',		'Hinton Telephone Company'],
		 84=>['Hinton',		'Hinton Telephone Company'],
		 88=>['American',	'American Long Lines'],
		 92=>['Vernon',		'Vernon Telephone Cooperative'],
		 96=>['W Wisconsin',	'West Wisconsin Telecommunications Cooperative, Inc.'],
		100=>['W Wisconsin',	'West Wisconsin Telecommunications Cooperative, Inc.'],
		104=>['Northeast',	'Northeast Telephone Company'],
		108=>['Coon Valley',	'Coon Valley Farmers Telephone Company'],
		112=>['Coon Valley',	'Coon Valley Farmers Telephone Company'],
		116=>['Cross',		'Cross Telephone'],
		120=>['Cross',		'Cross Telephone'],
		124=>['Cross',		'Cross Telephone'],
		128=>['Cambridge',	'Cambridge Telephone Company'],
		132=>['Peoples',	'Peoples Mutual Telephone Company'],
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
		188=>['Richland-Grant',	'Richland-Grant Telephone Cooperative, Inc.'],
		192=>['Richland-Grant',	'Richland-Grant Telephone Cooperative, Inc.'],
		196=>['La Valle',	'La Valle Telephone Cooperative, Inc.'],
		200=>['Arthur Mutual',	'The Arthur Mutual Telephone Company'],
		204=>['Price County',	'Price County Telephone Company'],
		208=>['Nova',		'The Nova Telephone Company'],
		212=>['New Ulm',	'New Ulm Telecom, Inc.'],
		216=>['Pottawatomic',	'Pottawatomic Telephone Company, Inc.'],
		220=>['Pottawatomic',	'Pottawatomic Telephone Company, Inc.'],
		224=>['Pottawatomic',	'Pottawatomic Telephone Company, Inc.'],
		228=>['H&B',		'H&B Communications, Inc.'],
		232=>['Milltown',	'Milltown Mutual Telephone Company'],
		236=>['Wamego',		'Wamego Telephone Company, Inc.'],
		240=>['Luck',		'Luck Telephone Company'],
		244=>['Farmers Ind.',	'Farmers Independent Telephone Company'],
		248=>['Mid-Iowa',	'Mid-Iowa Telephone Co-Op Association'],
		252=>['Lewis River',	'Lewis River Telephone'],
	},
	6=>{
		  0=>['Kin',		'Kin Network, Inc.'],
		  4=>['Conneaut',	'Conneaut Telephone Company'],
		  8=>['Madison',	'Madison Telephone Company, Inc.'],
		 12=>['Hartman',	'Hartman Telephone Exchanges'],
		 16=>['Rainbow',	'Rainbow Telephone Cooperative Associaiton, Inc.'],
		 20=>['Golden Belt',	'Golden Belt Telephone Association, Inc.'],
		 24=>['Middleburgh',	'Middleburgh Telephone Company'],
		 28=>['Ganado',		'Ganado Telephone Company'],
		 32=>['Coleman',	'Coleman County Telephone Cooperative, Inc.'],
		 36=>['Comanche',	'Comanche County Telephone Company, Inc.'],
		 40=>['Comanche',	'Comanche County Telephone Company, Inc.'],
		 44=>['Lake Dallas',	'Lake Dallas Telephone Company'],
		 48=>['Curtis',		'The Curtis Telephone Company'],
		 52=>['Gridley',	'Gridley Telephone Company'],
		 56=>['SP Telecom',	'SP Telecom'],
		 60=>['Guadalupe',	'Guadalupe Valley Telephone Cooperative'],
		 64=>['Guadalupe',	'Guadalupe Valley Telephone Cooperative'],
		 68=>['Guadalupe',	'Guadalupe Valley Telephone Cooperative'],
		 72=>['Guadalupe',	'Guadalupe Valley Telephone Cooperative'],
		 76=>['Twin Lakes',	'Twin Lakes Telephone Cooperative Corporation'],
		 80=>['Twin Lakes',	'Twin Lakes Telephone Cooperative Corporation'],
		 84=>['Twin Lakes',	'Twin Lakes Telephone Cooperative Corporation'],
		 88=>['Twin Lakes',	'Twin Lakes Telephone Cooperative Corporation'],
		 92=>['Stellars',	'Stellars Telecommunications Services'],
		 96=>['Intercontinental',	'Intercontinental Communications Corp.'],
		100=>['Intercontinental',	'Intercontinental Communications Corp.'],
		104=>['Switch 2000',	'Switch 2000'],
		108=>['Switch 2000',	'Switch 2000'],
		112=>['Colorado',	'Colorado Valley Telephone Cooperative'],
		116=>['Colorado',	'Colorado Valley Telephone Cooperative'],
		120=>['Hooper',		'Hooper Telephone Company'],
		124=>['Ottoville',	'Ottoville Mutual Telephone Company'],
		128=>['Keystone',	'Keystone-Arthur Telephone Company'],
		132=>['S&W',		'S&W Telephone Company'],
		136=>['Cochrane',	'Cochrane Cooperative Telephone Company'],
		140=>['New Knoxville',	'The New Knoxville Telephone Company'],
		144=>['Climax',		'Climax Telephone Company'],
		148=>['Middle Point',	'Middle Point Home Telephone Company'],
		152=>['E. Ritter',	'E. Ritter Telephone Company'],
		156=>['Union',		'Union Telephone Company'],
		160=>['Atlantic',	'Atlantic Telephone Membership Corporation'],
		164=>['Ayersville',	'Ayersville Telephone Company'],
		168=>['Arkansas',	'Arkansas Telephone Company, Inc.'],
		172=>['Daviess-Martin',	'Daviess-Martin County Rurual Telephone Corporation'],
		176=>['Granite State',	'Granite State Telephone'],
		180=>['Totah',		'Totah Telephone Company'],
		184=>['Totah',		'Totah Telephone Company'],
		188=>['Totah',		'Totah Telephone Company'],
		192=>['Villisca',	'Villisca Farmers Telephone Company'],
		196=>['Standard',	'Standard Telephone Company'],
		200=>['N Central',	'North Central Telephone Cooperative'],
		204=>['N Central',	'North Central Telephone Cooperative'],
		208=>['TMC',		'TMC of Lexington'],
		212=>['Network',	'Network'],
		216=>['McClure',	'The McClure Telephone Company'],
		220=>['Yukon-Waltz',	'Yukon-Waltz Telephone COmpany'],
		224=>['Fonorola',	'Fonorola'],
		228=>['Glasford',	'Glasford Telephone Company'],
		232=>['Benton',		'Benton Cooperative Telephone Company'],
		236=>['Union Springs',	'Union Springs Telephone Company'],
		240=>['Ft Jennings',	'Fort Jennings Telephone Company'],
		244=>['Farmers',	'Farmers Mutual Telephone Company'],
		248=>['Pattersonville',	'Pattersonville Telephone Company'],
		252=>['ITG',		'International Telemanagement Group'],
	},
	7=>{
		  0=>['Pacific G/W',	'Pacific Gateway Exchange'],
		  4=>['Pacific G/W',	'Pacific Gateway Exchange'],
		  8=>['Electric L/W',	'Electrict Ligthwave'],
		 12=>['Allendale',	'Allendale Telephone Company'],
		 16=>['Decatur',	'Decatur Telephone Company, Inc.'],
		 20=>['Bell',		'Bell Mobility Cellular'],
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
		 96=>['Peoples',	'Peoples Mutual Telephone Company'],
		100=>['Cameron',	'Cameron Telephone Company'],
		104=>['Action',		'Action Telephone Company'],
		108=>['TCC',		'TCC Communications'],
		112=>['Smithville',	'Smithville Telephone Company, Inc.'],
		116=>['Oneida',		'Oneida County Rural Telephone Company'],
		120=>['Emily',		'Emily Cooperative Telephone Company'],
		124=>['Beehive',	'Beehive Telephone Company, Inc.'],
		128=>['Custer',		'CUster Telephone Cooperative, Inc.'],
		132=>['MGW',		'MGW Telephone'],
		136=>['Call America',	'Call America'],
		140=>['Blanchard',	'Blanchard Telephone Assocation, Inc.'],
		144=>['Tonica',		'Tonica Telephone Company'],
		148=>['Pine Belt',	'Pine Belt Telephone Company, Inc.'],
		152=>['Rockland',	'Rockland Telephone Company, Inc.'],
		156=>['Sogetel',	'Sogetel, Inc.'],
		160=>['Hamilton',	'Hamilton County Telephone Cooperative'],
		164=>['Fleet Call',	'Fleet Call'],
		168=>['New Lisbon',	'New Lisbon Telephone Company'],
		172=>['W River',	'West River Telephone Cooperative'],
		176=>['W River',	'West River Telephone Cooperative'],
		180=>['Nebraska',	'Nebraska Central Telephone Company'],
		184=>['Coastal',	'Coastal Utilities, Inc.'],
		188=>['Diller',		'Diller Telephone Company'],
		192=>['HunTel',		'HunTel Systems'],
		196=>['Progressive',	'Progressive Rural Telephone Cooperative, Inc.'],
		200=>['Delta',		'Delta Telephone Company, Inc.'],
		204=>['K&M',		'K&M Telephone Company, Inc.'],
		208=>['Wash. City',	'Washington City Rural Telephone Cooperative, Inc.'],
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
		  4=>['Deerfield',	'Deerfield Farmers\' Telephone Company'],
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
		 52=>['Craigville',	'Craigville Telephone Company Inc.'],
		 56=>['Clarence',	'Clarence Telephone Company'],
		 60=>['NIU',		'Northern Illinois University'],
		 64=>['One call',	'One call Communications, Inc.'],
		 68=>['One call',	'One call Communications, Inc.'],
		 72=>['Churchill',	'Churchill City Telephone & Telegraph System'],
		 76=>['Chippewa',	'Chippewa City Telephone Company'],
		 80=>['Roberts',	'Roberts City Telephone Cooperative Association'],
		 84=>['Bretton',	'Bretton Woods Telephone Company'],
		 88=>['Eastern',	'Eastern Telecom Corporation'],
		 92=>['Northwest',	'Northwest Telephone Company'],
		 96=>['Northwest',	'Northwest Telephone Company'],
		100=>['ICON',		'ICON'],
		104=>['Bittel',		'Bittel Telecommunications Corporation'],
		108=>['Bittel',		'Bittel Telecommunications Corporation'],
		112=>['TeleSystems',	'Communications TeleSystems International'],
		116=>['Thacker',	'Thacker-Grigsby Telephone Company, Inc.'],
		120=>['Thrifty',	'Thrifty Telephone, Inc.'],
		124=>['Telco',		'Telco Communications, Inc.'],
		128=>['ASC',		'Applied Signal Corporation'],
		132=>['Wilton',		'Wilton Telephone Company'],
		136=>['ANS',		'Alternative Network Services'],
		140=>['ANS',		'Alternative Network Services'],
		144=>['Telstar',	'Telstar Communications, Inc.'],
		148=>['Telstar',	'Telstar Communications, Inc.'],
		152=>['ITC',		'ITC Companies'],
		154=>['ITC',		'ITC Companies'],
		160=>['UNICOM',		'United Communications (UNICOM)'],
		164=>['BNI',		'BNI Telecommunications'],
		168=>['Wabash',		'Wabash Telephone Company'],
		172=>['Flat Rock',	'Flat Rock Telephone Company'],
		176=>['Woodhull',	'Woodhull Telephone Company'],
		180=>['BellSouth',	'BellSouth Mobility'],
		184=>['BellSouth',	'BellSouth Mobility'],
		188=>['BellSouth',	'BellSouth Mobility'],
		192=>['BellSouth',	'BellSouth Mobility'],
		196=>['Hi-Rim',		'Hi-Rim Communications'],
		200=>['Hi-Rim',		'Hi-Rim Communications'],
		204=>['Prarie Grove',	'Prarie Grove Telephone Company'],
		208=>['Prarie Grove',	'Prarie Grove Telephone Company'],
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
		 68=>['Dakota',		'Dakota Central Telecommunication Cooperative'],
		 72=>['Teleport',	'Teleport Communications Group'],
		 76=>['Teleport',	'Teleport Communications Group'],
		 80=>['Teleport',	'Teleport Communications Group'],
		 84=>['Teleport',	'Teleport Communications Group'],
		 88=>['Teleport',	'Teleport Communications Group'],
		 92=>['Huntel',		'Huntel Systems, Inc.'],
		 96=>['TRI',		'TRI Touch America'],
		100=>['TRI',		'TRI Touch America'],
		104=>['star',		'star Telephone Membership Corporation'],
		108=>['star',		'star Telephone Membership Corporation'],
		112=>['star',		'star Telephone Membership Corporation'],
		116=>['Indianhead',	'Indianhead Telephone Company'],
		120=>['Polar',		'Polar Communications Mutual Aid Corporation'],
		124=>['Multimedia',	'Multimedia Telephone Service'],
		128=>['Multimedia',	'Multimedia Telephone Service'],
		#132=>['SPARE',		'SPARE - UNASSIGNED'],
		136=>['TWT',		'Total World Telecom'],
		140=>['TWT',		'Total World Telecom'],
		144=>['TWT',		'Total World Telecom'],
		148=>['TWT',		'Total World Telecom'],
		152=>['ATCI',		'ATCI Inc.'],
		156=>['ATCI',		'Econo Call/ATCI (USA)'],
		160=>['ATCI',		'Econo Call/ATCI (USA)'],
		164=>['Ardmore',	'Ardmore Telephone Company, Inc.'],
		168=>['Niagra',		'Niagra Telephone Company'],
		172=>['Mabel',		'Mabel Cooperative Telephone Company'],
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
		236=>['Clay County',	'Clay County Rural Telephone Cooperative, Inc.'],
		#240=>['SPARE',		'SPARE - UNASSIGNED'],
		244=>['McDonough',	'McDonoughClay Telephone Company'],
		248=>['Oneida',		'Oneida Telephone Company'],
		252=>['LaHarpe',	'LaHarpe Telephone Company'],
	},
	10=>{
		  0=>['Great Plains',	'Great Plains Communications'],
		  4=>['Great Plains',	'Great Plains Communications'],
		  8=>['Great Plains',	'Great Plains Communications'],
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
		 56=>['Logan',		'Logan Telephone Cooperative, Inc.'],
		 60=>['Logan',		'Logan Telephone Cooperative, Inc.'],
		 64=>['Ex. TeleCard',	'Executive TeleCard International'],
		 68=>['Farmers',	'Farmers Telephone'],
		 72=>['S C Utah',	'South Central Utah Telephone Association'],
		 76=>['Griggs City',	'Griggs City Telephone Company'],
		 80=>['Teltrust',	'Teltrust'],
		 84=>['Teltrust',	'Teltrust'],
		 88=>['Western',	'Western Telephone Company'],
		 92=>['Tidewater',	'Tidewater Telecom'],
		 96=>['Glens Falls',	'Glens Falls Communications Corporation'],
		100=>['Pac-West',	'Pac-West Telecomm, Inc. dba AmeriCall'],
		#104=>['SPARE',		'SPARE - UNASSIGNED'],
		#108=>['SPARE',		'SPARE - UNASSIGNED'],
		112=>['Clay County',	'Clay County Rural Telephone Cooperative, Inc.'],
		116=>['Clay County',	'Clay County Rural Telephone Cooperative, Inc.'],
		120=>['CCI',		'Communication Consultants, Inc.'],
		124=>['Mornington',	'Mornington Municipal Telephone System'],
		128=>['Blanchard',	'Blanchard Municipal Telephone System'],
		132=>['Citizens',	'Citizens Telephone'],
		136=>['Siren',		'Siren Telephone Company, Inc.'],
		140=>['Time Warner',	'Time Warner Communications'],
		144=>['Time Warner',	'Time Warner Communications'],
		148=>['Minkfort',	'Minkfort Intelligent Network'],
		152=>['Surry',		'Surry Telephone Membership Corporation'],
		156=>['Surry',		'Surry Telephone Membership Corporation'],
		160=>['Wes-Tex',	'Wes-Tex Telephone Cooperative, Inc.'],
		164=>['Cytel',		'Cypress Telecommunications, Inc./Cytel'],
		168=>['Cytel',		'Cypress Telecommunications, Inc./Cytel'],
		172=>['Cytel',		'Cypress Telecommunications, Inc./Cytel'],
		176=>['LaJiacarita',	'LaJiacarita Rural Telephone Coop'],
		180=>['Rye',		'Rye Telephone Company'],
		184=>['Alma',		'Alma Telephone Company'],
		188=>['Cal-North',	'Cal-North Cellular'],
		192=>['Five Area',	'Five Area Telephone Cooperative, Inc.'],
		196=>['Eastex',		'Eastex Telephone Cooperative, Inc.'],
		200=>['Wood',		'Wood County Telephone Company'],
		204=>['Warwick',	'La Compagnie de Tel. de Warwick'],
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
		252=>['MTC',		'MTC Telemanagement'],
	},
	11=>{
		  0=>['Winn',		'Winn Telephone Company'],
		  4=>['Orwell',		'Orwell Telephone Company'],
		  8=>['CellularOne',	'CellularOne (Puerto Rico)'],
		#12=>['SPARE',		'SPARE - UNASSIGNED'],
		#16=>['SPARE',		'SPARE - UNASSIGNED'],
		 20=>['Econophone',	'Econophone, Inc.'],
		 24=>['Econophone',	'Econophone, Inc.'],
		 28=>['Econophone',	'Econophone, Inc.'],
		 32=>['SE IN',		'Southeastern IN Rural Telephone Cooperative Inc.'],
		 36=>['Leaco',		'Leaco Rural Telephone Cooperative, Inc.'],
		 40=>['Mt. Horeb',	'Mt. Horeb Telephone Company'],
		 44=>['Otelco',		'Otelco'],
		 48=>['CallAmerica',	'CallAmerica'],
		 52=>['CallAmerica',	'CallAmerica'],
		 56=>['Alhambra',	'Alhambra-Grantfork Telephone Company'],
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
		108=>['Brantely',	'Brantely Telephone Company'],
		112=>['TelRoute',	'TelRoute Communications, Inc.'],
		116=>['TelRoute',	'TelRoute Communications, Inc.'],
		120=>['Star',		'Star Telephone'],
		124=>['Star',		'Star Telephone'],
		128=>['Star',		'Star Telephone'],
		132=>['SCN',		'South Carolina Net, Inc.'],
		136=>['Armour',		'Armour Independent Telephone Company'],
		140=>['Union',		'Union Telephone Company'],
		144=>['Baldwin',	'Baldwin Telecom, Inc.'],
		148=>['Cass County',	'Cass County Telephone Company'],
		152=>['C-R',		'C-R Telephone Company'],
		156=>['E TeleLogic',	'Eastern TeleLogic Corporation'],
		160=>['Tri-County',	'Tri-County Telephone Cooperative'],
		164=>['Citizens',	'Citizens Telephone Company'],
		168=>['Marquette',	'Marquette Adams Telephone Cooperative'],
		172=>['St. Liboire',	'Le Tel de St. Liboire'],
		176=>['Austin Bestline','Austin Bestline'],
		180=>['N Dakota',	'North Dakota Telephone Company'],
		184=>['TSC',		'Telecommunications Service Center, Inc. (TSC)'],
		188=>['TSC',		'Telecommunications Service Center, Inc. (TSC)'],
		192=>['Elec. Lightwave','Electric Lightwave, Inc.'],
		196=>['Elec. Lightwave','Electric Lightwave, Inc.'],
		#200=>['SPARE',		'SPARE - UNASSIGNED'],
		204=>['Lansdowne',	'Lansdowne Rural Telephone Company Ltd.'],
		208=>['PTCF',		'People\'s Telephone Company of Forest Ltd.'],
		212=>['Home',		'Home Telephone Company, Inc.'],
		216=>['Commnet',	'Commnet Cellular'],
		220=>['Commnet',	'Commnet Cellular'],
		224=>['Commnet',	'Commnet Cellular'],
		228=>['ISI',		'ISI Telecommunications'],
		232=>['Pulaski',	'Pulaski-White Telephone Company'],
		236=>['OFMTC',		'Oregon Farmers Mutual Telephone Company'],
		240=>['Mulberry',	'Mulberry Cooperativef Telephone Company'],
		244=>['ITC',		'International Telecomm. Corporation'],
		248=>['Mountaineer',	'Mountaineer Long Distance, Inc.'],
		252=>['Champaign',	'Champaign Telephone Company'],
	},
	12=>{
	},
	13=>{
	},
	14=>{
	},
	15=>{
	},
	16=>{
	},
	17=>{
	},
	18=>{
	},
	19=>{
	},
	20=>{
	},
	21=>{
	},
	22=>{
	},
	23=>{
	},
	24=>{
	},
	25=>{
	},
	26=>{
	},
);

my $pc_assigments = {
	large_networks=>\%large_networks,
	small_networks=>\%small_networks,
	pc_blocks=>\%pc_blocks,
};

sub pcowner {
	my ($pc,$i) = @_;
	my ($ntw,$cls,$mem,$own);
	$ntw = $pc >> 16;
	$cls = ($pc >> 8) & 0xff;
	$mem = $pc & 0xff;
	if (5 < $ntw && $ntw < 255) {
		if ($cls != 0) {
			if ($own = $pc_assigments->{large_networks}->{$ntw}) {
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
			if ($small = $pc_assigments->{small_networks}->{$ntw}) {
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
			if ($cluster = $pc_assigments->{pc_blocks}->{$cls}) {
				my $member = $mem & ~0x03;
				if ($own = $cluster->{$member}) {
					return $own->[$i];
				}
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
			$c->addtag('circuits','withtag',$self->{item}) if defined $c and $self->{item};
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
						foreach ($hl->info('children',$entry)) {
							$hl->show('entry',$_);
						}
						$hl->indicator('configure', $entry, -image=>'minus',);
						$self->{tree}->{$entry} = 'minus';
					} else {
						foreach ($hl->info('children',$entry)) {
							$hl->hide('entry',$_);
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
	return main::pcstring($msg->{'opc'}) if $col == 4;
	return main::pcstring($msg->{'dpc'}) if $col == 5;
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

		$self->fillprops($tw,\$row);
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
		$self->fillstatus($tw,\$row);
	}
	$tw->update;
	$tw->MapWindow;
}

# -------------------------------------
package Clickable;
use strict;
# -------------------------------------

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
sub button3 {
	my ($canvas,$self,$top,$X,$Y) = @_;
	my $ref = ref $self;
	my $m = $canvas->toplevel->Menu(
		-tearoff=>0,
		-title=>"$ref Menu",
		-type=>'normal',
	);
	$self->getmenu($m,$top,$X,$Y);
	$m->Popup(
		-popanchor=>'nw',
		-popover=>'cursor',
	);

}

# -------------------------------------
package Relation;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgStats Logging Properties Status Clickable CallCollector MsgCollector);
# -------------------------------------
# A relation is an association between signalling points that communicate with
# each other.  This object is used to track these interactions, primarily for
# identifying the role of nodes.
# -------------------------------------

use constant {
	RS_AVAILABLE => 0,
	RS_DEGRADED => 1,
	RS_CONGESTED_1 => 2,
	RS_CONGESTED_2 => 3,
	RS_CONGESTED_3 => 4,
	RS_UNAVAILABLE => 5,
};

my @rsoptions = (
	['Available'    => RS_AVAILABLE  ],
	['Degraded'     => RS_DEGRADED   ],
	['Congested'    => RS_CONGESTED_1],
	['Congested(2)' => RS_CONGESTED_2],
	['Congested(3)' => RS_CONGESTED_3],
	['Unavailable'  => RS_UNAVAILABLE],
);

my %relations;
my $relationno = 0;

#package Relation;
sub init {
	my ($self,$top,$network,$relationno,$nodea,$nodeb,@args) = @_;
	$self->{network} = $network;
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->CallCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->{key} = "$nodea->{pc},$nodeb->{pc}";
	$self->{state} = RS_AVAILABLE;
	$self->{statetext} = 'Available';
	$self->{cics} = {};
	$self->{ciccnt} = 0;
	$self->{actcnt} = 0;
	$self->{actforw} = 0;
	$self->{actrevs} = 0;
	$self->{actboth} = 0;
	$self->{slcs} = {};
	$self->{slccnt} = 0;
	$self->{nodea} = $nodea;
	$self->{nodeb} = $nodeb;
	$nodea->{relate}->{$nodeb->{pc}} = $self;
	$nodeb->{relate}->{$nodea->{pc}} = $self;
	my $xa = $self->{xa} = $nodea->{x};
	my $ya = $self->{ya} = $nodea->{y};
	my $xb = $self->{xb} = $nodeb->{x};
	my $yb = $self->{yb} = $nodeb->{y};
	my $c = $top->canvas;
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	$self->{fill} = 'grey';
	$self->{item} = $c->createLine($xa,$ya,$xb,$yb,
		-arrow=>'none',
		-capstyle=>'round',
		-fill=>$self->{fill},
		-activefill=>'green',
		-joinstyle=>'round',
		-smooth=>0,
		-tags=>['relation'],
		-width=>0.1,
		-activewidth=>2,
	);
	my $type = ref $self;
	my $state = ($top->{show}->{"\L$type\Es"}) ? 'normal' : 'hidden';
	$c->itemconfigure($self->{item},-state=>$state);
	$c->bind($self->{item},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$c->lower($self->{item},'node');
	$c->lower($self->{item},'path');
	$top->mycanvas->addballoon($self, $self->{item});
	$top->statusbar->configure(-text=>"New ".$self->identify);
	$top->{updatenow} = 1;


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
	$id .= "$self->{nodea}->{pcode}";
	$id .= " ($self->{nodea}->{pownr})" if $self->{nodea}->{pownr};
	$id .= ", $self->{nodeb}->{pcode}";
	$id .= " ($self->{nodeb}->{pownr})" if $self->{nodeb}->{pownr};
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
	my ($self,$top,$network,$msg) = @_;
	if ($msg->{si} == 5) {
		my ($mt,$cic) = ($msg->{mt},$msg->{cic});
		if ($self->{ciccnt} or ($mt == 0x10 || (0x12 <= $mt && $mt <= 0x1b) || $mt == 0x2a || $mt == 0x2b)) {
			my $circuit = Circuit->get($top,$network,$self,$cic);
			$circuit->add_msg($top,$network,$msg);
		}
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
		$c->addtag('linkset','withtag',$self->{item});
		$c->addtag('SLTM','withtag',$self->{item});
		return;
	}
	if ($self->{xchg_isup}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill});
		$c->addtag('circuits','withtag',$self->{item});
		$c->addtag('ISUP','withtag',$self->{item});
		return;
	}
	if ($self->{forw_tcap} && $self->{revs_tcap}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill},-dash=>[5,3]);
		$c->addtag('association','withtag',$self->{item});
		$c->addtag('TCAP','withtag',$self->{item});
		return;
	}
	if ($self->{forw_tcap}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill},-dash=>[5,3]);
		$c->addtag('association','withtag',$self->{item});
		$c->addtag('TCAP','withtag',$self->{item});
		return;
	}
	if ($self->{revs_tcap}) {
		$self->{fill} = 'black';
		$c->itemconfigure($self->{item}, -fill=>$self->{fill},-dash=>[5,3]);
		$c->addtag('association','withtag',$self->{item});
		$c->addtag('TCAP','withtag',$self->{item});
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
sub move {
	my ($self,$top) = @_;
	my $nodea = $self->{nodea};
	my $nodeb = $self->{nodeb};
	my $xa = $nodea->{x};
	my $ya = $nodea->{y};
	my $xb = $nodeb->{x};
	my $yb = $nodeb->{y};
	my $c = $top->canvas;
	return if $xa == $self->{xa} &&
		  $ya == $self->{ya} &&
		  $xb == $self->{xb} &&
		  $yb == $self->{yb};
	$self->{cola} = $nodea->{col};
	$self->{rowa} = $nodea->{row};
	$self->{colb} = $nodeb->{col};
	$self->{rowb} = $nodeb->{row};
	$c->coords($self->{item},$xa,$ya,$xb,$yb);
	$self->{xa} = $xa;
	$self->{ya} = $ya;
	$self->{xb} = $xb;
	$self->{yb} = $yb;
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
}

#package Relation;
sub getmenu {
	my ($self,$m,$top,$X,$Y) = @_;
	shift->Clickable::getmenu(@_);
	if (keys %{$self->{links}}) {
		$m->add('separator');
		my ($mc,$m3);
		$mc = $m->Menu(
			-tearoff=>0,
			-title=>'Links Menu',
		);
		foreach my $slc (sort {$a <=> $b} keys %{$self->{links}}) {
			my $link = $self->{links}->{$slc};
			$m3 = $mc->Menu(
				-tearoff=>0,
				-title=>"Link $slc Menu",
			);
			$link->getmenu($m3,$top,$X,$Y);
			$mc->add('cascade',
				-menu=>$m3,
				-label=>"Link $slc",
			);
		}
		$m->add('cascade',
			-menu=>$mc,
			-label=>'Links',
		);
	}
#	$mc = $m->Menu(
#		-tearoff=>1,
#		-title=>'Circuits Menu',
#	);
#	foreach my $cic (sort {$a <=> $b} keys %{$self->{cics}}) {
#		my $circuit;
#		$m3 = $mc->Menu(
#			-tearoff=>0,
#			-title=>"Circuit $cic Menu",
#		);
#		$circuit = $self->{cics}->{$cic};
#		$circuit->getmenu($m3,$top,$X,$Y) if $circuit;
#		$mc->add('cascade',
#			-menu=>$m3,
#			-label=>"Circuit $cic",
#		);
#	}
#	$m->add('cascade',
#		-menu=>$mc,
#		-label=>'Circuits',
#		-state=>((keys %{$self->{cics}})?'normal':'disabled'),
#	);
}

#package Relation;
sub fillprops {
	my ($self,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'SP A point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pca = $self->{nodea}->{pcode};
	$pca .= " ($self->{nodea}->{pownr})" if $self->{nodea}->{pownr};
	$tw->Entry(%entryleft,
		-textvariable=>\$pca,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pcb = $self->{nodeb}->{pcode};
	$pcb .= " ($self->{nodeb}->{pownr})" if $self->{nodeb}->{pownr};
	$tw->Entry(%entryleft,
		-textvariable=>\$pcb,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

#package Relation;
sub fillstatus {
	my ($self,$tw,$row) = @_;

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
	$tw->Label(%labelright,
		-text=>'State:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Optionmenu(%optionleft,
		-options=>\@rsoptions,
		-variable=>\$self->{state},
		-textvariable=>\$self->{statetext},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
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
	$self->{'nodea'}->log($text);
	$self->{'nodeb'}->log($text);
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
	$id .= "$group->{nodea}->{pcode}";
	$id .= " ($group->{nodea}->{pownr})" if $group->{nodea}->{pownr};
	$id .= ", $group->{nodeb}->{pcode}";
	$id .= " ($group->{nodeb}->{pownr})" if $group->{nodeb}->{pownr};
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
	my ($self,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Circuit id code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{cic},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	my $group = $self->{group};
	$tw->Label(%labelright,
		-text=>'SP A point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pca = $group->{nodea}->{pcode};
	$pca .= " ($group->{nodea}->{pownr})" if $group->{nodea}->{pownr};
	$tw->Entry(%entrycenter,
		-textvariable=>\$pca,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pcb = $group->{nodeb}->{pcode};
	$pcb .= " ($group->{nodeb}->{pownr})" if $group->{nodeb}->{pownr};
	$tw->Entry(%entrycenter,
		-textvariable=>\$pcb,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

#package Circuit;
sub fillstatus {
	my ($self,$tw,$row) = @_;

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
@ISA = qw(Relation);
# -------------------------------------

use constant {
	COL_NOD => 6,
	COL_SSP => 5,
	COL_SCP => 4,
	COL_GTT => 3,
	COL_ADJ => 2,
};

use constant {
	LS_AVAILABLE => 0,
	LS_DEGRADED => 1,
	LS_UNAVAILABLE => 2,
};

#package Linkset;
sub reposition {
	my ($self,$top,$node) = @_;
	my $col = COL_ADJ;
	if ($node->{col} < 0) { $col = -$col; }
	$node->movesp($top,$col,$node->{row});
}

#package Linkset;
sub xform {
	my ($type,$self,$top) = @_;
	return if ref $self eq $type;
	bless $self,$type;
	$self->{fill} = 'black';
	$self->{width} = 2;
	my $c = $top->canvas;
	$c->itemconfigure($self->{item},
		-fill=>$self->{file},
		-width=>$self->{width},
	);
	$c->addtag('linkset','withtag',$self->{item});
	my $state = ($top->{show}->{"\L$type\Es"}) ? 'normal' : 'hidden';
	$c->itemconfigure($self->{item},-state=>$state);
	$c->bind($self->{item},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	my $mc = $top->mycanvas;
	$mc->addballoon($self, $self->{item});
	$self->reposition($top,$self->{nodea});
	$self->reposition($top,$self->{nodeb});
	$self->checktype;
	$top->statusbar->configure(-text=>"Discovered ".$self->identify);
	$top->{updatenow} = 1;
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
sub getLink {
	my ($self,$top,$nodea,$nodeb,$slc,@args) = @_;
	return $self->{links}->{$slc} if exists $self->{links}->{$slc};
	my $link = Link->new($top,$nodea,$nodeb,$slc,@args);
	$self->{links}->{$slc} = $link;
	return $link;
}

#package Linkset;
sub move {
	my ($self,$top) = @_;
	$self->Relation::move($top);
	foreach my $link (values %{$self->{links}}) { $link->move($top); }
}

#package Linkset;
sub identify {
	my $self = shift;
	my $letter = $Linkset::letters[$self->checktype];
	my $id = "$letter-Linkset ";
	$id .= "$self->{nodea}->{pcode}";
	$id .= " ($self->{nodea}->{pownr})" if $self->{nodea}->{pownr};
	$id .= ", $self->{nodeb}->{pcode}";
	$id .= " ($self->{nodeb}->{pownr})" if $self->{nodeb}->{pownr};
	return ($self->{id} = $id);
}

#package Linkset;
sub shortid {
	my $self = shift;
	return "$self->{nodea}->{pcode}:$self->{nodeb}->{pcode}";
}

#package Linkset;
sub fillprops {
	my ($self,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'SP A point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pca = $self->{nodea}->{pcode};
	$pca .= " ($self->{nodea}->{pownr})" if $self->{nodea}->{pownr};
	$tw->Entry(%entrycenter,
		-textvariable=>\$pca,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'SP B point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pcb = $self->{nodeb}->{pcode};
	$pcb .= " ($self->{nodeb}->{pownr})" if $self->{nodeb}->{pownr};
	$tw->Entry(%entrycenter,
		-textvariable=>\$pcb,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

#package Linkset;
sub fillstatus {
	my ($self,$tw,$row) = @_;

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
package Link;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgStats Logging Properties Status Clickable MsgCollector);
# -------------------------------------

use constant {
	LK_INSERVICE => 0,
	LK_BUSY => 1,
	LK_OUTOFSERVICE => 2,
};

#package Link;
sub init {
	my ($self,$top,$nodea,$nodeb,$slc,@args) = @_;
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->MsgCollector::init(@args);
	$self->{nodea} = $nodea;
	$self->{nodeb} = $nodeb;
	$self->{slc} = $slc;
	$self->{state} = LK_INSERVICE;
	$self->{statetext} = 'In Service';
	$self->{pathforw} = undef;
	$self->{pathrevs} = undef;
	$top->statusbar->configure(-text=>"New ".$self->identify);
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
sub addPathForw {
	my ($self,$path) = @_;
	$self->{pathforw} = $path;
}

#package Link;
sub addPathRevs {
	my ($self,$path) = @_;
	$self->{pathrevs} = $path;
}

#package Link;
sub addPath {
	my ($self,$path) = @_;
	if ($path->{nodea}->{pc} == $self->{nodea}->{pc}) {
		$self->addPathForw($path);
	} else {
		$self->addPathRevs($path);
	}
}

#package Link;
sub add {
	my ($self,$msg) = @_;
	if ($msg->{'opc'} == $self->{'nodea'}->{'pc'}) {
		$self->inc($msg,0);
		$self->pushmsg($msg);
	} else {
		$self->inc($msg,1);
		$self->pushmsg($msg);
	}
}

#package Link;
sub move {
	my ($self,$top) = @_;
	my $path;
	if ($path = $self->{pathforw}) { $path->move($top); }
	if ($path = $self->{pathrevs}) { $path->move($top); }
}

#package Link;
sub identify {
	my $self = shift;
	my $id = "Link $self->{slc} ";
	$id .= "$self->{nodea}->{pcode}";
	$id .= " ($self->{nodea}->{pownr})" if $self->{nodea}->{pownr};
	$id .= ", $self->{nodeb}->{pcode}";
	$id .= " ($self->{nodeb}->{pownr})" if $self->{nodeb}->{pownr};
	return ($self->{id} = $id);
}

#package Link;
sub shortid {
	my $self = shift;
	return "$self->{slc}:$self->{nodea}->{pcode},$self->{nodeb}->{pcode}";
}

#package Link;
sub getmenu {
	my ($self,$m,$top,$X,$Y) = @_;
	shift->Clickable::getmenu(@_);
	$m->add('separator');
	my ($path,$mc);
	$path = $self->{pathforw};
	$mc = $m->Menu(
		-tearoff=>0,
		-title=>'Forward path menu',
	);
	$path->getmenu($mc,$top,$X,$Y) if $path;
	$m->add('cascade',
		-menu=>$mc,
		-label=>'Forward path',
		-state=>($path?'normal':'disabled'),
	);
	$path = $self->{pathrevs};
	$mc = $m->Menu(
		-tearoff=>0,
		-title=>'Revers path menu',
	);
	$path->getmenu($mc,$top,$X,$Y) if $path;
	$m->add('cascade',
		-menu=>$mc,
		-label=>'Reverse path',
		-state=>($path?'normal':'disabled'),
	);
}

#package Link;
sub fillprops {
	my ($self,$tw,$row) = @_;
}

#package Link;
sub fillstatus {
	my ($self,$tw,$row) = @_;
}

# -------------------------------------
package Route;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgStats Logging Properties Status Clickable MsgCollector);
# -------------------------------------

use constant {
	RT_AVAILABLE => 0,
	RT_RESTRICTED => 1,
	RT_PROHIBITED => 2,
};

my @rtoptions = (
	['Available'  => RT_AVAILABLE ],
	['Restricted' => RT_RESTRICTED],
	['Prohibited' => RT_PROHIBITED],
);

#package Route;
sub new {
	my ($type,$top,$path,$side,$node,@args) = @_;
	my $self = {};
	bless $self, $type;
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->MsgCollector::init(@args);
	$self->{state} = RT_AVAILABLE;
	$self->{statetext} = 'Available';
	$self->{path} = $path;
	$self->{side} = $side;
	$self->{node} = $node;
	$node->{routes}->{$path->{ppa}} = $self;
	$path->{routes}->{$node->{pc}} = $self;
	my ($cola,$rowa,$colb,$rowb);
	if ($side eq 'a') {
		$cola = $self->{cola} = $node->{col};
		$rowa = $self->{rowa} = $node->{row};
		$colb = $self->{colb} = $path->{cola};
		$rowb = $self->{rowb} = $path->{rowa};
	} else {
		$cola = $self->{cola} = $path->{colb};
		$rowa = $self->{rowa} = $path->{rowb};
		$colb = $self->{colb} = $node->{col};
		$rowb = $self->{rowb} = $node->{row};
	}
	my $mc = $top->mycanvas;
	my $xa = $self->{xa} = $mc->colpos($cola);
	my $ya = $self->{ya} = $mc->rowpos($rowa);
	my $xb = $self->{xb} = $mc->colpos($colb);
	my $yb = $self->{yb} = $mc->rowpos($rowb);
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	$self->{color} = 'gray';
	my $c = $top->canvas;
	$self->{item} = $c->createLine($xa,$ya,$xb,$yb,
		-arrow=>'last',
		-capstyle=>'round',
		-fill=>$self->{color},
		-activefill=>'green',
		-joinstyle=>'round',
		-smooth=>0,
		-tags=>['route'],
		-width=>0.1,
		-activewidth=>2,
	);
	my $state = ($top->{show}->{"\L$type\Es"}) ? 'normal' : 'hidden';
	$c->itemconfigure($self->{item},-state=>$state);
	$c->bind($self->{item},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$c->traceVariable(\$self->{state},'w'=>[\&Route::tracestate,$self]);
	$c->traceVariable(\$self->{color},'w'=>[\&Route::tracecolor,$self,$c]);
	$c->lower($self->{item},'path');
	$c->lower($self->{item},'node');
	$mc->addballoon($self, $self->{item});
	$top->statusbar->configure(-text=>"New ".$self->identify);
	$top->{updatenow} = 1;
	return $self;
}

##package Route;
#sub DESTROY {
#	my $self = shift;
#	my $c = $self->canvas;
#	$c->traceVdelete(\$self->{state});
#	$c->traceVdelete(\$self->{color});
#}

#package Route;
sub add_msg {
	my ($self,$top,$network,$msg,$dir) = @_;
	$self->inc($msg,$dir);
	$self->pushmsg($msg);
}

#package Route;
sub tracestate {
	my ($ind,$val,$op,$self) = @_;
	if ($op eq 'w') {
		if ($val == RT_AVAILABLE) {
			$self->{color} = 'gray';
		} elsif ($val == RT_RESTRICTED) {
			$self->{color} = 'orange';
		} elsif ($val == RT_PROHIBITED) {
			$self->{color} = 'red';
		}
		$self->{node}->updatestate;
	}
	return $val;
}

#package Route;
sub tracecolor {
	my ($ind,$val,$op,$self,$c) = @_;
	$c->itemconfigure($self->{item},-fill=>$val) if $op eq 'w';
	return $val;
}

#package Route;
sub identify {
	my $self = shift;
	my $id = "Route ";
	$id .= "($self->{path}->{card}:$self->{path}->{span}:$self->{path}->{slot})";
	if ($self->{side} eq 'a') {
		$id .= " <- ";
	} else {
		$id .= " -> ";
	}
	$id .= "$self->{node}->{pcode}";
	$id .= " ($self->{node}->{pownr})" if $self->{node}->{pownr};
	return ($self->{id} = $id);
}

#package Route;
sub shortid {
	my $self = shift;
	my $id = "($self->{path}->{card}:$self->{path}->{span}:$self->{path}->{slot})";
	if ($self->{side} eq 'a') {
		$id .= "<-";
	} else {
		$id .= "->";
	}
	$id .= "($self->{node}->{pcode})";
	return $id;
}

#package Route;
sub move {
	my ($self,$top) = @_;
	my $node = $self->{node};
	my $path = $self->{path};
	my $side = $self->{side};
	my ($xa,$ya,$xb,$yb);
	if ($side eq 'a') {
		$xa = $node->{x};
		$ya = $node->{y};
		$xb = $path->{xa};
		$yb = $path->{ya};
	} else {
		$xa = $path->{xb};
		$ya = $path->{yb};
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
		$self->{colb} = $path->{cola};
		$self->{rowb} = $path->{rowa};
	} else {
		$self->{cola} = $path->{colb};
		$self->{rowa} = $path->{rowb};
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

#package Route;
sub fillprops {
	my ($self,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Signalling path:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-text=>"$self->{card}:$self->{span}:$self->{slot}",
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Signalling point:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	my $pcode = $self->{node}->{pcode};
	$pcode .= " ($self->{node}->{pwonr})" if $self->{node}->{pownr};
	$tw->Entry(%entryleft,
		-textvariable=>\$pcode,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

#package Route;
sub fillstatus {
	my ($self,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Status:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Optionmenu(%optionleft,
		-options=>\@rtoptions,
		-variable=>\$self->{state},
		-textvariable=>\$self->{statetext},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

# -------------------------------------
package SP;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgStats Logging Properties Status Clickable CallCollector MsgCollector);
# -------------------------------------

my %nodes;
my $nodeno = 0;

use constant {
	COL_NOD => 6,
	COL_SSP => 5,
	COL_SCP => 4,
	COL_GTT => 3,
	COL_ADJ => 2,
};
use constant {
	SP_AVAILABLE => 0,
	SP_DEGRADED => 1,
	SP_UNAVAILABLE => 2,
};
my @spoptions = (
	['Available'	=> SP_AVAILABLE  ],
	['Degraded'	=> SP_DEGRADED	 ],
	['Unavailable'	=> SP_UNAVAILABLE],
);

#package SP;
sub init {
	my ($self,$top,$network,$nodeno,$pc,$path,$side,$way,@args) = @_;
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
	$self->CallCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->{pc} = $pc;
	$self->{rt} = $path->{rt};
	$self->{rttext} = $path->{rttext};
	$self->{side} = $side;
	$self->{way} = $way;
	$self->{state} = SP_AVAILABLE;
	$self->{statetext} = 'Available';
	$self->{ciccnt} = 0;
	$self->{actcnt} = 0;
	$self->{actog} = 0;
	$self->{actic} = 0;
	$self->{act2w} = 0;
	$self->{tqueries} = {};
	$self->{circuits} = {};
	$self->{responds} = {};
	$self->{routes} = {}; # routes that term or orig here
	$self->{relate} = {}; # relations in which this is a node
	my $c = $top->canvas;
	my $mc = $top->mycanvas;
	my $x;
	if ($side < 0) {
		$x = $mc->colpos(0 - COL_NOD);
		$self->{col} = 0 - COL_NOD;
	} else {
		$x = $mc->colpos(0 + COL_NOD);
		$self->{col} = 0 + COL_NOD;
	}
	my $y = $mc->rowpos(0);
	$self->{row} = 0;
	$self->{x} = $x;
	$self->{y} = $y;
	$self->{color} = 'white';
	$self->{item} = $c->createOval(
		$x-33,$y-33,$x+33,$y+33,
		-fill=>$self->{color},
		-outline=>'blue',
		-activeoutline=>'green',
		-width=>2,
		-activewidth=>3,
		-tags=>['SP','node'],
	);
	$c->bind($self->{item},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$self->{scri} = $c->createLine(
		$x-23,$y-23,$x+23,$y-23,$x+23,$y+23,$x-23,$y+23,$x-23,$y-23,
		-arrow=>'none',
		-capstyle=>'round',
		-fill=>'gray',
		-activefill=>'green',
		-joinstyle=>'round',
		-smooth=>0,
		-tags=>['SP','scri'],
		-width=>0.1,
		-activewidth=>2,
	);
	$c->bind($self->{scri},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$self->{pcode} = main::pcstring($pc);
	$self->{lownr} = main::pcowner($pc,1);
	if ($self->{pownr} = main::pcowner($pc,0)) {
		$self->{ownr} = $c->createText($x,$y+15,
			-anchor=>'center',
			-fill=>'black',
			-justify=>'center',
			-text=>$self->{pownr},
			-tags=>['SP','text'],
		);
		$c->bind($self->{ownr},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	}
	$self->{ttxt} = $c->createText($x,$y-15,
		-anchor=>'center',
		-fill=>'black',
		-justify=>'center',
		-text=>'SP',
		-tags=>['SP','text'],
	);
	$c->bind($self->{ttxt},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$self->{text} = $c->createText($x,$y,
		-anchor=>'center',
		-fill=>'black',
		-justify=>'center',
		-text=>$self->{pcode},
		-tags=>['SP','text'],
	);
	my $type = ref $self;
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
	$c->bind($self->{text},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$c->traceVariable(\$self->{state},'w'=>[\&SP::tracestate,$self]);
	$c->traceVariable(\$self->{color},'w'=>[\&SP::tracecolor,$self,$c]);
	$mc->addballoon($self, $self->{item}, $self->{scri}, $self->{ttxt}, $self->{text});
	$mc->addballoon($self, $self->{ownr}) if $self->{ownr};
	$network->regroupsps($top);
	$top->statusbar->configure(-text=>"New ".$self->identify);
	$top->{updatenow} = 1;
}

#sub DESTROY {
#	my $self = shift;
#	my $c = $self->canvas;
#	$c->traceVdelete(\$self->{state});
#}

#package SP;
sub xform {
	my ($type,$self,$top) = @_;
	return if ref $self eq $type;
	my $oldtype = ref $self;
	bless $self,$type;
	my $c = $top->canvas;
	my $mc = $top->mycanvas;
	$mc->delballoon($self->{item}, $self->{scri}, $self->{ttxt}, $self->{text});
	$mc->delballoon($self->{ownr}) if $self->{ownr};

	$c->dtag($self->{item},$oldtype); $c->addtag($type,'withtag',$self->{item});
	$c->dtag($self->{scri},$oldtype); $c->addtag($type,'withtag',$self->{scri});
	$c->dtag($self->{ttxt},$oldtype); $c->addtag($type,'withtag',$self->{ttxt});
	$c->dtag($self->{text},$oldtype); $c->addtag($type,'withtag',$self->{text});
	if ($self->{ownr})
      { $c->dtag($self->{ownr},$oldtype); $c->addtag($type,'withtag',$self->{ownr}); }

	my @oldtags = ();
	push @oldtags, 'SLTM' if $self->{xchg_sltm};
	push @oldtags, 'ISUP' if $self->{xchg_isup};
	push @oldtags, 'TCAP' if $self->{orig_tcap} or $self->{term_tcap};
	push @oldtags, 'circuits' if $self->{ciccnt} > 0;

	$c->delete($self->{item});
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
	$c->bind($self->{item},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$c->bind($self->{scri},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$c->bind($self->{ttxt},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$c->bind($self->{text},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$c->bind($self->{ownr},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]) if $self->{ownr};
	$mc->addballoon($self, $self->{item}, $self->{scri}, $self->{ttxt}, $self->{text});
	$mc->addballoon($self, $self->{ownr}) if $self->{owner};
	$top->statusbar->configure(-text=>"Discovered ".$self->identify);
	$top->{updatenow} = 1;
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
sub tracestate {
	my ($ind,$val,$op,$self) = @_;
	if ($op eq 'w') {
		if ($val == SP_AVAILABLE) {
			$self->{color} = 'white';
		} elsif ($val == SP_DEGRADED) {
			$self->{color} = 'orange';
		} elsif ($val == SP_UNAVAILABLE) {
			$self->{color} = 'red';
		}
	}
	return $val;
}

#package SP;
sub tracecolor {
	my ($ind,$val,$op,$self,$c) = @_;
	$c->itemconfigure($self->{item},-fill=>$val) if $op eq 'w';
	return $val;
}

#package SP;
sub updatestate {
	my $self = shift;
	my ($avail,$degra,$unava) = (0,0);
	foreach my $route (values %{$self->{routes}}) {
		my $rstate = $route->{'state'};
		if ($rstate == Route::RT_AVAILABLE) {
			$avail++;
		} elsif ($rstate == Route::RT_RESTRICTED) {
			$degra++;
		} elsif ($rstate == Route::RT_PROHIBITED) {
			$unava++;
		}
	}
	if ($avail == 0 && $degra == 0 && $unava >= 0) {
		$self->{state} = SP_UNAVAILABLE;
	} elsif ($degra + $unava > 0) {
		$self->{state} = SP_DEGRADED;
	} else {
		$self->{state} = SP_AVAILABLE;
	}
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
			$self->{circuits}->{$msg->{dpc}} = 1;
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
			$self->{tqueries}->{$msg->{dpc}} = 1;
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
			$self->{adjacent}->{$msg->{dpc}} = 1;
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
			$self->{circuits}->{$msg->{opc}} = 1;
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
			$self->{responds}->{$msg->{opc}} = 1;
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
			$self->{adjacent}->{$msg->{opc}} = 1;
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
		$c->addtag('ISUP','withtag',$self->{item});
		$c->addtag('ISUP','withtag',$self->{scri});
		$c->addtag('ISUP','withtag',$self->{ttxt});
		$c->addtag('ISUP','withtag',$self->{text});
		$c->addtag('ISUP','withtag',$self->{ownr}) if $self->{ownr};
	}
	if ($self->{orig_tcap} or $self->{term_tcap}) {
		$c->addtag('TCAP','withtag',$self->{item});
		$c->addtag('TCAP','withtag',$self->{scri});
		$c->addtag('TCAP','withtag',$self->{ttxt});
		$c->addtag('TCAP','withtag',$self->{text});
		$c->addtag('TCAP','withtag',$self->{ownr}) if $self->{ownr};
	}
	if ($self->{xchg_sltm}) {
		$c->addtag('SLTM','withtag',$self->{item});
		$c->addtag('SLTM','withtag',$self->{scri});
		$c->addtag('SLTM','withtag',$self->{ttxt});
		$c->addtag('SLTM','withtag',$self->{text});
		$c->addtag('SLTM','withtag',$self->{ownr}) if $self->{ownr};
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
	my ($self,$top,$col,$row) = @_;
	#return if $col == $self->{col} && $row == $self->{row};
	my $mc = $top->mycanvas;
	my $nx = $mc->colpos($col);
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
	$self->{col} = $col;
	$self->{row} = $row;
	if ($dx or $dy) {
		foreach my $r (values %{$self->{routes}}) { $r->move($top); }
		foreach my $r (values %{$self->{relate}}) { $r->move($top); }
	}
}

#package SP;
sub getmenu {
	my ($self,$m,$top,$X,$Y) = @_;
	shift->Clickable::getmenu(@_);
	if (keys %{$self->{relate}}) {
		$m->add('separator');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{relate}}) {
			my $relation = $self->{relate}->{$pc};
			my $node;
			if ($relation->{nodea}->{pc} == $self->{pc}) {
				$node = $relation->{nodeb};
			} else {
				$node = $relation->{nodea};
			}
			my $mc = $m->Menu(
				-tearoff=>0,
				-title=>'Routeset to '.$node->shortid.' Menu',
			);
			$relation->getmenu($mc,$top,$X,$Y);
			$m->add('cascade',
				-menu=>$mc,
				-label=>'Routeset to '.$node->shortid,
			);
		}
	}
}

#package SP;
sub fillprops {
	my ($self,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Point code type:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Optionmenu(%optionleft,
		-options=>\@Path::rloptions,
		-variable=>\$self->{rt},
		-textvariable=>\$self->{rttext},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Point code:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-textvariable=>\$self->{pcode},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	$tw->Label(%labelright,
		-text=>'Point code owner:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryinput,
		-textvariable=>\$self->{lownr},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

#package SP;
sub fillstatus {
	my ($self,$tw,$row,$lnktxt,$cirtxt,$quetxt,$restxt) = @_;
	my $f;

	$lnktxt = 'Linksets' unless defined $lnktxt;
	$cirtxt = 'Circuits' unless defined $cirtxt;
	$quetxt = 'Queries' unless defined $quetxt;
	$restxt = 'Replies' unless defined $restxt;

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
	$f->Label(%labelright,
		-text=>'State:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$f->Optionmenu(%optionleft,
		-options=>\@spoptions,
		-variable=>\$self->{state},
		-textvariable=>\$self->{statetext},
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
	if (keys %{$self->{adjacent}}) {
		$f = $tw->TFrame(%tframestyle,
			-label=>'Linksets:',
		)->pack(%tframepack);
		$$row = 0;
		foreach my $pc (sort {$a <=> $b} keys %{$self->{adjacent}}) {
			$f->Label(%labelright,
				-text=>"$lnktxt:",
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			my $pcode = main::pcstring($pc);
			my $pownr = main::pcowner($pc,0);
			$pcode .= " ($pownr)" if $pownr;
			$f->Entry(%entrycenter,
				-textvariable=>\$pcode,
			)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
		}
	}
	if (keys %{$self->{circuits}}) {
		$f = $tw->TFrame(%tframestyle,
			-label=>'Circuits:',
		)->pack(%tframepack);
		$$row = 0;
		$f->Label(%labelcenter,
			-text=>'Point Code',
		)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
		$f->Label(%labelcenter,
			-text=>'Alloc.',
		)->grid(-row=>$$row,-column=>1,-sticky=>'ewns');
		$f->Label(%labelcenter,
			-text=>'Act.',
		)->grid(-row=>$$row,-column=>2,-sticky=>'ewns');
		$f->Label(%labelcenter,
			-text=>'O/G',
		)->grid(-row=>$$row,-column=>3,-sticky=>'ewns');
		$f->Label(%labelcenter,
			-text=>'I/C',
		)->grid(-row=>$$row,-column=>4,-sticky=>'ewns');
		$f->Label(%labelcenter,
			-text=>'2/W',
		)->grid(-row=>$$row++,-column=>5,-sticky=>'ewns');
		foreach my $pc (sort {$a <=> $b} keys %{$self->{circuits}}) {
			my ($ogcol,$iccol);
			if ($self->{way} eq 'O')
			{ $ogcol = 3; $iccol = 4; } else
			{ $ogcol = 4; $iccol = 3; }
			my $pcode = main::pcstring($pc);
			my $pownr = main::pcowner($pc,0);
			$pcode .= " ($pownr)" if $pownr;
			$f->Entry(%entrycenter,
				-textvariable=>\$pcode,
			)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
			$f->Entry(%entryright,
				-textvariable=>\$self->{relate}->{$pc}->{ciccnt},
			)->grid(-row=>$$row,-column=>1,-sticky=>'ewns');
			$f->Entry(%entryright,
				-textvariable=>\$self->{relate}->{$pc}->{actcnt},
			)->grid(-row=>$$row,-column=>2,-sticky=>'ewns');
			$f->Entry(%entryright,
				-textvariable=>\$self->{relate}->{$pc}->{actforw},
			)->grid(-row=>$$row,-column=>$ogcol,-sticky=>'ewns');
			$f->Entry(%entryright,
				-textvariable=>\$self->{relate}->{$pc}->{actrevs},
			)->grid(-row=>$$row,-column=>$iccol,-sticky=>'ewns');
			$f->Entry(%entryright,
				-textvariable=>\$self->{relate}->{$pc}->{actboth},
			)->grid(-row=>$$row++,-column=>5,-sticky=>'ewns');
		}
	}
	if (keys %{$self->{tqueries}} || keys %{$self->{responds}}) {
		$f = $tw->TFrame(%tframestyle,
			-label=>'Queries:',
		)->pack(%tframepack);
		$$row = 0;
		my $col = 0;
		foreach my $pc (sort {$a <=> $b} keys %{$self->{tqueries}}) {
			if ($col == 0) {
				$f->Label(%labelright,
					-text=>"$quetxt:",
				)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			}
			$col++;
			my $pcode = main::pcstring($pc);
			my $pownr = main::pcowner($pc,0);
			$pcode .= " ($pownr)" if $pownr;
			$f->Entry(%entrycenter,
				-textvariable=>\$pcode,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			if ($col > 5) {
				$col = 0;
				$$row++;
			}
		}
		if ($col != 0) {
			$col = 0;
			$$row++;
		}
		foreach my $pc (sort {$a <=> $b} keys %{$self->{responds}}) {
			if ($col == 0) {
				$f->Label(%labelright,
					-text=>"$restxt:",
				)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			}
			$col++;
			my $pcode = main::pcstring($pc);
			my $pownr = main::pcowner($pc,0);
			$pcode .= " ($pownr)" if $pownr;
			$f->Entry(%entrycenter,
				-textvariable=>\$pcode,
			)->grid(-row=>$$row,-column=>$col,-sticky=>'ewns');
			if ($col > 5) {
				$col = 0;
				$$row++;
			}
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
		$x-33,$y-33,$x+33,$y+33,
		-fill=>'white',
		-outline=>'black',
		-activeoutline=>'green',
		-width=>2,
		-activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	$c->coords($self->{scri},
		$x-23,$y-23,$x+23,$y+23,$x+23,$y-23,$x-23,$y+23,$x-23,$y-23,
	);
}

#package SSP;
sub fillstatus {
	my ($self,$tw,$row) = @_;
	$self->SP::fillstatus($tw,$row,
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
		$x-33,$y-23,$x+33,$y+23,
		-fill=>'white',
		-outline=>'black',
		-activeoutline=>'green',
		-width=>2,
		-activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
}

#package SCP;
sub fillstatus {
	my ($self,$tw,$row) = @_;
	$self->SP::fillstatus($tw,$row,
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
		$x-28,$y-28,$x+28,$y+28,
		-fill=>'white',
		-outline=>'black',
		-activeoutline=>'green',
		-width=>2,
		-activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	$c->coords($self->{scri},
		$x+28,$y-28,$x-28,$y+28,
	);
}

#package STP;
sub fillstatus {
	my ($self,$tw,$row) = @_;
	$self->SP::fillstatus($tw,$row,
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
		$x-28,$y-28,$x+28,$y+28,
		-fill=>'white',
		-outline=>'black',
		-activeoutline=>'green',
		-width=>2,
		-activewidth=>3,
		-tags=>[ref($self),'node',@{$oldtags}],
	);
	$c->coords($self->{scri},
		$x+28,$y-28,$x-28,$y+28,
	);
}

#package GTT;
sub fillstatus {
	my ($self,$tw,$row) = @_;
	$self->SP::fillstatus($tw,$row,
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
	$self->CallCollector::init(@args);
	$self->MsgCollector::init(@args);
	$self->{msgnum} = 0;
	$self->{ciccnt} = 0;
	$self->{actcnt} = 0;
	$self->{act1w} = 0;
	$self->{act2w} = 0;
	$self->{nodes} = {};
	$self->{nodeno} = 0;
	$self->{paths} = {};
	$self->{pathno} = 0;
	$self->{relations} = {};
	$self->{relationno} = 0;
	my $c = $top->canvas;
	$c->CanvasBind('<ButtonPress-3>',[\&Network::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y'),Tk::Ev('x'),Tk::Ev('y')]);
	$Network::network = $self;
	$top->statusbar->configure(-text=>"New ".$self->identify);
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
sub getPath {
	my ($self,$top,$ppa,@args) = @_;
	return $self->{paths}->{$ppa} if exists $self->{paths}->{$ppa};
	my $pathno = $self->{pathno} + 1;
	my $path = Path->new($top,$pathno,$ppa,@args);
	$self->{paths}->{$ppa} = $path;
	$self->{pathno} = $pathno;
	return $path;
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
sub getLinkSet {
	my ($self,$top,@args) = @_;
	my $linkset = $self->getRelation($top,@args); 
	if (ref $linkset eq 'Relation') {
		Linkset->xform($linkset,$top);
	}
	return $linkset;
}

#package Network;
sub regroupsps {
	my ($self,$top) = @_;
	my %totals;
	while (my ($pc,$node) = each %{$self->{nodes}}) {
		my $col = $node->{col};
		$totals{$col} += 1;
	}
	my %counts;
	foreach my $pc (sort {$a <=> $b} keys %{$self->{nodes}}) {
		my $node = $self->{nodes}->{$pc};
		my $col = $node->{col};
		my $row = $counts{$col} - $totals{$col};
		$counts{$col} += 2;
		$node->movesp($top,$col,$row);
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
	my ($self,$tw,$row) = @_;
}

#package Network;
sub fillstatus {
	my ($self,$tw,$row) = @_;

	$tw->Label(%labelright,
		-text=>'Number of paths:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryright,
		-textvariable=>\$self->{pathno},
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
package Path;
use strict;
use vars qw(@ISA);
@ISA = qw(MsgStats Logging Properties Status Clickable MsgCollector);
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

my %paths;
my $pathno = 0;

#package Path;
sub init {
	my ($self,$top,$pathno,$ppa,@args) = @_;
	$self->MsgStats::init(@args);
	$self->Logging::init(@args);
	$self->Properties::init(@args);
	$self->Status::init(@args);
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
	$self->{routes} = {};
	$self->{msgbuf} = [];
	#print "Created new path ($self->{card}:$self->{span}:$self->{slot}).\n";
	$self->{fill} = 'red';
	my $cola = $self->{cola} = 0 - SP::COL_ADJ;
	my $rowa = $self->{rowa} = $pathno * 2;
	my $colb = $self->{colb} = 0 + SP::COL_ADJ;
	my $rowb = $self->{rowb} = $pathno * 2;
	my $mc = $top->mycanvas;
	my $xa = $self->{xa} = $mc->colpos($cola);
	my $ya = $self->{ya} = $mc->rowpos($rowa);
	my $xb = $self->{xb} = $mc->colpos($colb);
	my $yb = $self->{yb} = $mc->rowpos($rowb);
	$self->{x} = ($xa + $xb)/2;
	$self->{y} = ($ya + $yb)/2;
	my $c = $top->canvas;
	$self->{item} = $c->createLine($xa,$ya,$xb,$yb,
		-arrow=>'last',
		-capstyle=>'round',
		-fill=>$self->{fill},
		-activefill=>'green',
		-joinstyle=>'round',
		-smooth=>0,
		-tags=>['path'],
		-width=>0.1,
		-activewidth=>2,
	);
	my $state = ($top->{show}->{paths}) ? 'normal' : 'hidden';
	$c->itemconfigure($self->{item},-state=>$state);
	$c->bind($self->{item},'<ButtonPress-3>',[\&Clickable::button3,$self,$top,Tk::Ev('X'),Tk::Ev('Y')]);
	$c->lower($self->{item},'node');
	$mc->addballoon($self, $self->{item});
	$top->statusbar->configure(-text=>"New ".$self->identify);
	$top->{updatenow} = 1;
}

#package Path;
sub new {
	my ($type,@args) = @_;
	my $self = {};
	bless $self,$type;
	$self->init(@args);
	return $self;
}

#package Path;
sub get {
	my ($type,$ppa,@args) = @_;
	return $paths{$ppa} if exists $paths{$ppa};
	my $self = {};
	bless $self, $type;
	$paths{$ppa} = $self;
	$pathno = $pathno + 1;
	$self->init($pathno,$ppa,@args);
	return $self;
}

#package Path;
sub findalias {
	my ($self,$top,$network,$node,@routes) = @_;
	return unless ref $node eq 'SSP' or ref $node eq 'SCP';
	my $col = Linkset::COL_GTT;
	foreach my $route (@routes) {
		my $alias = $route->{node};
		next unless $alias->{'pc'} != $node->{'pc'};
		if (abs($alias->{col}) != $col) {
			my $newcol = $col;
			if ($alias->{col} < 0) { $newcol = -$col; }
			$alias->movesp($top,$newcol,$alias->{row});
			$alias->{alias} = 1;
			my $c = $top->canvas;
			$c->itemconfigure($alias->{item}, -dash=>[5,2]);
			$c->itemconfigure($alias->{scri}, -dash=>[5,2]);
			$top->statusbar->configure(-text=>"Discovered alias ".$alias->identify);
		}
	}
}

#package Path;
sub bindpath {
	my ($self,$top,$network,$nodea,$nodeb,$slc) = @_;
	$self->{slc} = $slc;
	$self->{nodea} = $nodea;
	$self->{nodeb} = $nodeb;
	$self->{slc} = $slc;
	my $linkset = $network->getLinkSet($top,$nodea,$nodeb);
	my $link = $linkset->getLink($top,$nodea,$nodeb,$slc);
	$self->{link} = $link;
	$link->addPath($self);
	$self->findalias($top,$network,$nodea,(values %{$self->{opcs}}));
	$self->findalias($top,$network,$nodeb,(values %{$self->{dpcs}}));
	my $c = $top->canvas;
	$c->lower($self->{item},$linkset->{item});
	#$c->itemconfigure($self->{item}, -state=>'hidden');
	$self->move($top);
	$network->regroupsps($top);
	$top->{updatenow} = 1;
}

#package Path;
sub cardnum {
	shift->{card};
}
#package Path;
sub spannum {
	shift->{span};
}
#package Path;
sub slotnum {
	shift->{slot};
}
#package Path;
sub ppa {
	shift->{ppa};
}
#package Path;
sub add {
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
#package Path;
sub complete {
	my ($self,$top,$network,$msg,@args) = @_;
	my ($nodeb,$nodea,$pc,$route);
	$self->inc($msg,0);
	$self->pushmsg($msg);
	$network->add_msg($top,$msg);
	if (exists $msg->{opc}) {
		$pc = $msg->{opc};
		if ($route = $self->{opcs}->{$pc}) {
			$nodea = $route->{node};
			$self->swap($top) if $nodea->{col} < 0 && $self->{cola} > 0;
		} else {
			$nodea = $network->getSp($top,$pc, $self, $self->{cola},'O');
			$self->swap($top) if $nodea->{col} < 0 && $self->{cola} > 0;
			$route = Route->new($top,$self,'a',$nodea);
			$self->{opcs}->{$pc} = $route;
			$self->findalias($top,$network,$self->{nodea},(values %{$self->{opcs}})) if $self->{nodea};
			$self->findalias($top,$network,$self->{nodeb},(values %{$self->{dpcs}})) if $self->{nodeb};
			$network->regroupsps($top);
		}
		$route->add_msg($top,$network,$msg,0);
		$nodea->add_msg($top,$network,$msg,0);
	}
	if (exists $msg->{dpc}) {
		$pc = $msg->{dpc};
		if ($route = $self->{dpcs}->{$pc}) {
			$nodeb = $route->{node};
			$self->swap($top) if $nodeb->{col} < 0 && $self->{colb} > 0;
		} else {
			$nodeb = $network->getSp($top,$pc, $self, $self->{colb},'I');
			$self->swap($top) if $nodeb->{col} < 0 && $self->{colb} > 0;
			$route = Route->new($top,$self,'b',$nodeb);
			$self->{dpcs}->{$pc} = $route;
			$self->findalias($top,$network,$self->{nodea},(values %{$self->{opcs}})) if $self->{nodea};
			$self->findalias($top,$network,$self->{nodeb},(values %{$self->{dpcs}})) if $self->{nodeb};
			$network->regroupsps($top);
		}
		$route->add_msg($top,$network,$msg,1);
		$nodeb->add_msg($top,$network,$msg,1);
	}
	if (exists $msg->{dpc} && exists $msg->{opc}) {
		my $rela = $network->getRelation($top,$nodea,$nodeb);
		$rela->add_msg($top,$network,$msg);
		if ($msg->{si} == 1 || $msg->{si} == 2) {
			if ($msg->{mt} == 0x11 || $msg->{mt} == 0x12) {
				$self->bindpath($top,$network,$nodea,$nodeb,$msg->{slc});
			}
		}
	}
}
#package Path;
sub swap {
	my ($self,$top) = @_;
	$self->movepath($top,-$self->{cola},$self->{rowa}, -$self->{colb},$self->{rowb});
	while (my ($k,$node) = each %{$self->{opcs}}) {
		$node->swap($top) if $self->{cola} < 0 && $node->{col} > 0;
	}
	while (my ($k,$node) = each %{$self->{dpcs}}) {
		$node->swap($top) if $self->{colb} < 0 && $node->{col} > 0;
	}
}
#package Path;
sub detecting {
	my $self = shift;
	return 0 if ($self->{ht} != 0 && $self->{pr} != 0 && $self->{rt} != 0);
	return 1;
}

#package Path;
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
#package Path;
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
#package Path;
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

#package Path;
sub move {
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
	$self->{cola} = $nodea->{col};
	$self->{rowa} = $nodea->{row};
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
	while (my ($k,$r) = each %{$self->{routes}}) { $r->move($top); }
}

#package Path;
sub movepath {
	my ($self,$top,$cola,$rowa,$colb,$rowb) = @_;
	return if $cola == $self->{cola} &&
	          $rowa == $self->{rowa} &&
		  $colb == $self->{colb} &&
		  $rowb == $self->{rowb};
	my $mc = $top->mycanvas;
	my $xa = $self->{xa} = $mc->colpos($cola);
	my $ya = $self->{ya} = $mc->rowpos($rowa);
	my $xb = $self->{xb} = $mc->colpos($colb);
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
	while (my ($k,$v) = each %{$self->{routes}}) { $v->move($top); }
}

#package Path;
sub identify {
	my $self = shift;
	my $id = "Path $self->{card}:$self->{span}:$self->{slot},";
	$id .= " $self->{nodea}->{pcode} -> $self->{nodeb}->{pcode} link $self->{slc}" if $self->{nodea};
	return ($self->{id} = $id);
}

#package Path;
sub shortid {
	my $self = shift;
	return "$self->{card}:$self->{span}:$self->{slot}";
}

#package Path;
sub fillprops {
	my ($self,$tw,$row) = @_;

	my $ppa = "$self->{card}:$self->{span}:$self->{slot}";
	$tw->Label(%labelright,
		-text=>'Channel:',
	)->grid(-row=>$$row,-column=>0,-sticky=>'ewns');
	$tw->Entry(%entryleft,
		-textvariable=>\$ppa,
	)->grid(-row=>$$row++,-column=>1,-sticky=>'ewns');
}

#package Path;
sub fillstatus {
	my ($self,$tw,$row) = @_;

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
	my $path = $network->getPath($top,$self->{ppa});
	if ($self->decode($path) >= 0) {
		$path->add($top,$network,$self);
		return;
	}
	#print STDERR "decoding error\n";
}
#package Message;
sub decode {
	my ($self,$path,@args) = @_;
	$self->{path} = $path;
	my @b = (unpack('C*', substr($self->{dat}, 0, 8)));
	if (!exists $self->{mtp2decode}) {
		my $len = $self->{hdr}->{len};
		if (3 <= $len && $len <= 5) {
			$path->setht(HT_BASIC);
		} elsif (6 <= $len && $len <= 8) {
			$path->setht(HT_EXTENDED);
		}
		return 0 unless $path->{ht} != HT_UNKNOWN;
		my ($bsn,$bib,$fsn,$fib,$li,$li0);
		if ($path->{ht} == HT_BASIC) {
			$self->{bsn} = $b[0] & 0x7f;
			$self->{bib} = $b[0] >> 7;
			$self->{fsn} = $b[1] & 0x7f;
			$self->{fib} = $b[1] >> 7;
			$self->{li} = $b[2] & 0x3f;
			$self->{li0} = $b[2] >> 6;
		} elsif ($path->{ht} == HT_EXTENDED) {
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
		my $inf = $len - $path->{ht};
		if (($self->{li} != $inf) && ($inf <= 63 || $self->{li} != 63)) {
			print STDERR "bad length indicator $self->{li} != $inf\n";
		}
		if ($self->{li0} != 0) {
			$path->setrt(RT_24BIT_PC);
			$path->setpr(MP_JAPAN);
		}
		$self->{mtp2decode} = 1;
	}
	return 1 if $self->{li} == 0;
	@b = (unpack('C*', substr($self->{dat}, $path->{ht}, 11)));
	$self->{si} = $b[0];
	return 1 if $self->{li} < 3;
	$self->{ni} = ($b[0] & 0xc0) >> 6;
	$self->{mp} = ($b[0] & 0x30) >> 4;
	$self->{si} = ($b[0] & 0x0f);
	if ($self->{ni} == 0) {
		$path->setrt(RT_14BIT_PC);
		$path->setpr(MP_INTERNATIONAL);
	}
	if ($path->{pr} == MP_UNKNOWN) {
		$path->setpr(MP_NATIONAL) if $self->{mp} != 0;
	} elsif ($path->{pr} == MP_JAPAN) {
		$self->{mp} = $self->{li0};
	} elsif ($path->{pr} == MP_INTERNATIONAL) {
		$self->{mp} = 0;
	}
	if ($self->{li} < HT_EXTENDED) {
		print STDERR "too short for RL, li = $self->{li}\n";
		return -1;
	}
	if ($self->{li} < 9 || ($self->{si} == 5 && $self->{li} < 11)) {
		$path->setrt(RT_14BIT_PC);
	}
	if ($path->{rt} == RT_UNKNOWN) {
		my $ret = $self->checkRoutingLabelType($self->{si},$path,$self->{li},\@b) < 0;
		return $ret if $ret <= 0;
		print "check succeeded on si=$self->{si}, mt=$self->{mt}";
	}
	if (!exists $self->{mtp3decode}) {
		return 0 if $path->detecting;
		if ($path->{rt} == RT_14BIT_PC) {
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
		@b = (unpack('C*', substr($self->{dat}, $path->{ht} + 1 + $path->{rt}, 3)));
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
			if ($path->{rt} == RT_14BIT_PC) {
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
#package Message;
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
#package Message;
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
#package Message;
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
#package Message;
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
#package Message;
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
#package Message;
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
#package Message;
sub checkItutSccp {
	my ($self,$path,$li,$b) = @_;
	my $mt = $b->[5];
	return PT_MAYBE if 0x01 <= $mt && $mt <= 0x14;
	return PT_NO;
}
#package Message;
sub checkAnsiSccp {
	my ($self,$path,$li,$b) = @_;
	my $mt = $b->[8];
	return PT_MAYBE if 0x01 <= $mt && $mt <= 0x14;
	return PT_NO;
}
#package Message;
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
#package Message;
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
#package Message;
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
	if (exists $mtypes{5}->{$mt}) {
		return PT_MAYBE;
	}
	return PT_MAYBE;
}
#package Message;
sub decodeit {
	my $self = shift;
	return if $self->{dec};
	my $path = $self->{path};
	my @l2h = ();
	my $p = 0;
	my $li = 0;
	if ($path->{ht} == HT_EXTENDED) {
		push @l2h, XBsnField->new($b,$p);
		push @l2h, XBsn0Field->new($b,$p);
		push @l2h, XBibField->new($b,$p);
		push @l2h, XFsnField->new($b,$p);
		push @l2h, XFsn0Field->new($b,$p);
		push @l2h, XFibField->new($b,$p);
		push @l2h, ($li = XliField->new($b,$p));
		push @l2h, Xli0Field->new($b,$p);
		push @l2h, XjmpField->new($b,$p) if $path->{pr} == MP_JAPAN;
		$p += HT_EXTENDED;
	} else {
		push @l2h, BsnField->new($b,$p);
		push @l2h, BibField->new($b,$p);
		push @l2h, FsnField->new($b,$p);
		push @l2h, FibField->new($b,$p);
		push @l2h, ($li = LiField->new($b,$p));
		if ($path->{pr} == MP_JAPAN) {
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
	if ($path->{rt} == RT_24BIT_PC) {
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
						foreach ($hl->info('children',$entry)) {
							$hl->show('entry',$_);
						}
						$hl->indicator('configure', $entry, -image=>'minus',);
						$self->{tree}->{$entry} = 'minus';
					} else {
						foreach ($hl->info('children',$entry)) {
							$hl->hide('entry',$_);
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
	if ($self->{path}->{ht} == HT_EXTENDED) {
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
		$self->makeentry($hl,4,0,23,$self->{opc},"\nOPC\n",'2.4',"Originating Point Code",[\&main::pcstring,$self->{opc}]);
		$self->makeentry($hl,7,0,23,$self->{dpc},"\nDPC\n",'2.5',"Destination Point Code",[\&main::pcstring,$self->{dpc}]);
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
			if ($self->{path}->{rt} == RT_24BIT_PC) {
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
	if ($self->{path}->{rt} == RT_14BIT_PC) {
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
	if ($self->{path}->{rt} == RT_14BIT_PC) {
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
	if ($self->{path}->{rt} == RT_14BIT_PC) {
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
	if ($self->{path}->{rt} == RT_14BIT_PC) {
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
#	$c->bind('path','<ButtonPress-3>',[\&Path::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
#	$c->bind('route','<ButtonPress-3>',[\&Route::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
#	$c->bind('relation','<ButtonPress-3>',[\&Relation::popup,Tk::Ev('b'),Tk::Ev('x'),Tk::Ev('y'),Tk::Ev('X'),Tk::Ev('Y')]);
	my $w = $self->{w} = $c->width;
	my $h = $self->{h} = $c->height;
	return $self;
}

#package MyCanvas;
sub canvas {
	return shift->{Canvas};
}

#package MyCanvas;
sub addballoon {
	my $self = shift;
	my $obj  = shift;
	$obj->identify;
	foreach (@_) { $self->{balloonmsgs}->{$_} = \$obj->{id}; }
	$self->{balloon}->attach($self->widget,
		-balloonposition=>'mouse',
		-msg=>$self->{balloonmsgs},
	);
}

#package MyCanvas;
sub delballoon {
	my $self = shift;
	foreach (@_) { delete $self->{balloonmsgs}->{$_}; }
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
	$c->lower('route','node');
	$c->lower('path','node');
	$c->lower('relation','path');
	$c->lower('route','path');
	$c->raise('linkset','path');
}

# unknown nodes are in columns +-7
# remote/alias SSP nodes are in columns +-6
# remote/alias SCP nodes are in columns +-5
# remote/alias STP nodes are in columns +-4 (GTT cap codes)
# remote/alias STP nodes are in columns +-3
# adjacent nodes are in columns +-2

sub colpos {
	my ($self,$col) = @_;
	my $w = $self->{w};
	my $dw = $w / 12; # 12 columns
	if ($col < 0) {
		return $col * $dw + $dw/2 + $w/2;
	} elsif ($col > 0) {
		return $col * $dw - $dw/2 + $w/2;
	} else {
		return 0 + $w/2;
	}
}

sub rowpos {
	my ($self,$row) = @_;
	my $h = $self->{h};
	return $row * 35 + $h/2;
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
				foreach (@{$w->configure}) {
					print "Option: ".join(', ',@$_);
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
			"Signalling view with linksets, adjacent SPs\nand signalling routes.",
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
		routes=>1,
		linksets=>1,
		paths=>1,
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
				my $c = $self->{MyCanvas}->{Canvas};
				foreach (keys %{$self->{show}}) {
					$self->{show}->{$_} = 1;
				}
				$c->itemconfigure('all',-state=>'normal');
				$self->{show}->{paths} = 0;
				$c->itemconfigure('path',-state=>'hidden');
				$self->{MyCanvas}->relayer();
			},$self],
	);
	$mi->add('radiobutton',
		-value=>1,
		-label=>'Signalling view',
		-variable=>\$self->{view},
		-underline=>0,
		-command=>[sub{
				my $self = shift;
				my $c = $self->{MyCanvas}->{Canvas};
				foreach (keys %{$self->{show}}) {
					$self->{show}->{$_} = 0;
				}
				$c->itemconfigure('all',-state=>'hidden');
				$c->itemconfigure('route',-state=>'normal');
				$self->{show}->{routes} = 1;
				$c->itemconfigure('linkset',-state=>'normal');
				$self->{show}->{linksets} = 1;
				$c->itemconfigure('path',-state=>'normal');
				$self->{show}->{paths} = 1;
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
				$self->{MyCanvas}->relayer();
			},$self],
	);
	$mi->add('radiobutton',
		-value=>2,
		-label=>'Circuit view',
		-variable=>\$self->{view},
		-underline=>0,
		-command=>[sub{
				my $self = shift;
				my $c = $self->{MyCanvas}->{Canvas};
				foreach (keys %{$self->{show}}) {
					$self->{show}->{$_} = 0;
				}
				$c->itemconfigure('all',-state=>'hidden');
				$c->itemconfigure('circuits',-state=>'normal');
				$self->{show}->{circuits} = 1;
				$c->itemconfigure('SSP&&ISUP',-state=>'normal');
				$self->{show}->{ssps} = 1;
				$self->{MyCanvas}->relayer();
			},$self],
	);
	$mi->add('radiobutton',
		-value=>3,
		-label=>'Transaction view',
		-variable=>\$self->{view},
		-underline=>0,
		-command=>[sub{
				my $self = shift;
				my $c = $self->{MyCanvas}->{Canvas};
				foreach (keys %{$self->{show}}) {
					$self->{show}->{$_} = 0;
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
				$self->{MyCanvas}->relayer();
			},$self],
	);
	$mi->add('separator',);
	$mi->add('checkbutton', -label=>'All',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{all},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{all};
				foreach (keys %{$self->{show}}) {
					$self->{show}->{$_} = $v;
				}
				$self->{view} = $v ? 0 : 4;
				my $c = $self->{MyCanvas}->{Canvas};
				if ($self->{show}->{all}) {
					$c->itemconfigure('all',-state=>'normal');
					$self->{MyCanvas}->relayer();
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
				$self->{show}->{linksets} = $v;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('relation',-state=>$s) if $c->find('withtag', 'relation');
			},$self],
	);
	$mi->add('checkbutton', -label=>'Circuit Groups',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{circuits},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{circuits};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('circuits',-state=>$s) if $c->find('withtag', 'circuits');
				$self->{show}->{relations} = $v
					if $self->{show}->{assocs} == $v and
					   $self->{show}->{linksets} == $v;
			},$self],
	);
	$mi->add('checkbutton', -label=>'Associations',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{assocs},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{assocs};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('association',-state=>$s) if $c->find('withtag', 'association');
				$self->{show}->{relations} = $v
					if $self->{show}->{circuits} == $v and
					   $self->{show}->{linksets} == $v;
			},$self],
	);
	$mi->add('checkbutton', -label=>'Link Sets',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{linksets},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{linksets};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('linkset',-state=>$s) if $c->find('withtag', 'linkset');
				$self->{show}->{relations} = $v
					if $self->{show}->{circuits} == $v and
					   $self->{show}->{assocs} == $v;
			},$self],
	);
	$mi->add('separator',);
	$mi->add('checkbutton', -label=>'Routes',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{routes},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{routes};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('route',-state=>$s) if $c->find('withtag', 'route');
			},$self],
	);
	$mi->add('checkbutton', -label=>'Paths',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{paths},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{paths};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('path',-state=>$s) if $c->find('withtag', 'path');
			},$self],
	);
	$mi->add('separator',);
	$mi->add('checkbutton', -label=>'SPs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{sps},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{sps};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('SP',-state=>$s) if $c->find('withtag', 'SP');
			},$self],
	);
	$mi->add('checkbutton', -label=>'SSPs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{ssps},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{ssps};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('SSP',-state=>$s) if $c->find('withtag', 'SSP');
			},$self],
	);
	$mi->add('checkbutton', -label=>'GTTs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{gtts},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{gtts};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('GTT',-state=>$s) if $c->find('withtag', 'GTT');
			},$self],
	);
	$mi->add('checkbutton', -label=>'STPs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{stps},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{stps};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('STP',-state=>$s) if $c->find('withtag', 'STP');
			},$self],
	);
	$mi->add('checkbutton', -label=>'SCPs',
		-onvalue=>1, -offvalue=>0, -variable=>\$self->{show}->{scps},
		-command=>[sub{
				my $self = shift;
				my $v = $self->{show}->{scps};
				$self->{view} = 4;
				my $c = $self->{MyCanvas}->{Canvas};
				my $s = $v ? 'normal' : 'hidden';
				$c->itemconfigure('SCP',-state=>$s) if $c->find('withtag', 'SCP');
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
			"Signalling view with linksets, adjacent SPs\nand signalling routes.",
			"Circuit view with SSPs and circuits.",
			"Transaction view with SSPs, GTTs and SCPs.",
			undef, # separator
			"Show/hide all items.",
			undef, # separator
			"Show/hide signalling relations.",
			"Show/hide circuit groups.",
			"Show/hide transaction associations.",
			"Show/hide signalling linksets.",
			undef, # separator
			"Show/hide signalling routes.",
			"Show/hide signalling paths.",
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
		-width=>'400',
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
	$_ = $self->{Balloon} = $self->toplevel->Balloon(
		#-initwait=>1000,
		-statusbar=>$self->{Message},
	);
	$::balloonwidget = $_;
	return $_;
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

my $xml = XMLout($pc_assigments);
print $xml;

#my $handler = MyParser->new();
#my $p = XML::SAX::ParserFactory->parser(Handler => $handler);
#$p->parse_usr($filename);

exit;

1;
