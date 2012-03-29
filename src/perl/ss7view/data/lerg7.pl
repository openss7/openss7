#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $o2 = \*OUTFILE2;
my $fn;

sub printnotice {
	my $file = shift;
	print $file <<'EOF';
#----------------------------------------------------------------------------
# NOTICE:  This file contains simple data gleened from public sources.  As
# such, the data is not subject to provisions under copyright law.  You can
# make as many copies of or manipulate the data in any way you see fit.  The
# OpenSS7 Project does not charge any fee for obtaining or using this data.  It
# is provided for our customers to use with OpenSS7 application software.  The
# data is defective and might not even be 80% accurate and might not be even
# 30% complete.
#----------------------------------------------------------------------------
# THERE IS NO WARRANTY FOR THE DATA, TO THE EXTENT PERMITTED BY APPLICABLE LAW.
# EXCEPT WHEN OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER
# PARTIES PROVIDE THE DATA "AS IS" WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESSED OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
# TO THE QUALITY AND PERFORMANCE OF THE DATA IS WITH YOU.  SHOULD THE DATA
# PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING, REPAIR OR
# CORRECTION.
#----------------------------------------------------------------------------
# IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
# WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MODIFIES AND/OR CONVEYS
# THE DATA AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES, INCLUDING ANY
# GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE
# USE OR INABILITY TO USE THE DATA (INCLUDING BUT NOT LIMITED TO LOSS OF DATA
# OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY YOU OR THIRD
# PARTIES OR A FAILURE OF THE DATA TO OPERATE WITH ANY OTHER PROGRAMS),
# EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGES.
#----------------------------------------------------------------------------
# Note that this is not the Telcordia LERG product.  If you have license to
# Telcordia Technologies LERG data, you should substitute the corresponding
# file from Telcordia for this file.  See the LERGINFO.DOC and LERGSPEC.DOC
# documents available from Telcordia at www.trainfo.com for further information
# about the formatting of these files and for information on obtaining the
# Telcordia LERG Routing Guide.  Telcordia is a registered trademark and LERG
# Routing Guide is a trademark of Telcordia Technologies, Inc.
EOF
	print $file <<'EOF';
# ---------
# LERG7.DAT
# ---------
# The LERG7 file contains information regarding switches.  Such switches are
# identified by an eleven-character code that, with few exceptions, should be a
# CLLI codes as is trademarked and supported by the COMMON LANGUAGE
# organization in Telcordia Technologies, Inc.  Although the field is termed a
# "switch," there may be multiple CLLI codes for a single physical switch for
# various reaons.  Such reasons can include the switch performing multiple
# functions (e.g. as an end office and as a tandem).  CLLIs may also reflect a
# "Point of Interface (POI)" established as the inteconnection point between
# two carriers.  Although POIs may, at times, be at the same location as an
# actual physical switch, they do not have to be.  All assigned CLLIs are not
# listed in the LERG -- only those that are relevant to accessing the local
# network and routing calls within the local network are included.  This file
# provides high-level switch information such as address, V&H coordinates,
# switch functionalities, equipment type, etc.
# ---------
# Switching Entity data (Key field 5)
# ---------
#   1-  5  5 LATA
#   6- 25 20 LATA Name
#  26- 26  1 Status
#  27- 32  6 Effective Date (mmddyy)
#  33- 43 11 Switch
#  44- 48  5 Equipment Type
#  49- 52  4 Administrative OCN (AOCN)
#  53- 53  1 filler
#  54- 57  4 Operating Company Number (OCN)
#  58- 62  5 Vertical Coordinate (VC)
#  63- 67  5 Horizontal Coordinate (HC)
#  68- 68  1 Intenational DDD (IDDD) (Y/N)
#  69-128 60 Street (Switch)
# 129-158 30 City (Switch)
# 159-160  2 State/Province/Territory/Country (Switch)
# 161-169  9 Zip/Postal Code
# 170-170  1 Point Code Flag
# 171-181 11 CLASS 4/5 Switch
# 182-182  1 Functionality 1 - END OFC
# 183-183  1 Functionality 2 - HOST
# 184-184  1 Functionality 3 - REMOTE
# 185-185  1 Functionality 4 - DA OFC
# 186-186  1 Functionality 5 - CLASS 4/5
# 187-187  1 Functionality 6 - WIRELESS OFC
# 188-188  1 Functionality 7 - FG D ADJ EO
# 189-189  1 Functionality 8 - filler
# 190-190  1 Functionality 9 - filler
# 191-191  1 Functionality 10 - filler
# 192-192  1 Functionality 11 - filler
# 193-193  1 Functionality 12 - FG B TDM
# 194-194  1 Functionality 13 - FG C TDM
# 195-195  1 Functionality 14 - FG D TDM
# 196-196  1 Functionality 15 - OS TDM
# 197-197  1 Functionality 16 - ITERMED OFC
# 198-198  1 Functionality 17 - DA TDM
# 199-199  1 Functionality 18 - 911 TDM
# 200-200  1 Functionality 19 - FG D AJD TDM
# 201-201  1 Functionality 20 - LOCAL TDM
# 202-202  1 Functionality 21 - INTRA TDM
# 203-203  1 Functionality 22 - CS DATA TDM
# 204-204  1 Functionality 23 - BCR5
# 205-205  1 Functionality 24 - BCR6
# 206-206  1 Functionality 25 - PRI 64
# 207-207  1 Functionality 26 - ISDN MULTIRT
# 208-208  1 Functionality 27 - ISDN FS OFC
# 209-209  1 Functionality 28 - X.75 GATEWAY
# 210-210  1 Functionality 29 - PACKET X.121
# 211-211  1 Functionality 30 - PACKET E.164
# 212-212  1 Functionality 31 - filler
# 213-213  1 Functionality 32 - filler
# 214-214  1 Functionality 33 - filler
# 215-215  1 Functionality 34 - STP
# 216-216  1 Functionality 35 - CCS AC OFC
# 217-217  1 Functionality 36 - filler
# 218-218  1 Functionality 37 - 800 SSP
# 219-219  1 Functionality 38 - LNP CAPABLE
# 220-220  1 Functionality 39 - filler
# 221-221  1 Functionality 40 - filler
# 222-222  1 Functionality 41 - filler
# 223-223  1 Functionality 42 - CIP
# 224-224  1 Functionality 43 - CSP
# 225-225  1 Functionality 44 - filler
# 226-226  1 Functionality 45 - SW 56
# 227-227  1 Functionality 46 - FGD 56
# 228-228  1 Functionality 47 - FGD 64
# 229-229  1 Functionality 48 - INTRA PRSUB
# 230-230  1 Functionality 49 - CALL AGENT
# 231-231  1 Functionality 50 - TRUNK GATEWAY
# 232-232  1 Functionality 51 - ACCESS GATEWAY
# 233-233  1 Functionality 52 - filler
# 234-234  1 Functionality 53 - filler
# 235-235  1 Functionality 54 - filler
# 236-236  1 Functionality 55 - filler
# 237-272 36 filler
# 273-278  6 Creation Date in BIRRDS (mmddyy)
# 279-279  1 filler
# 280-285  6 "E" STATUS date (mmddyy)
# 286-286  1 filler
# 287-292  6 Last Modification Date (mmddyy)
# 293-293  1 filler
# 294-294  1 CLN Indicator
# 295-297  3 Bill-to RAO
# 298-300  3 Send-to RAO
# 301-312 12 filler
# ---------
EOF
}

my @keys = (
	'LATA',
	'LATA Name',
	'Status',
	'Effective Date',
	'Switch',
	'Equipment Type',
	'Administrative OCN (AOCN)',
	'filler',
	'Operating Company Number (OCN)',
	'Vertical Coordinate (VC)',
	'Horizontal Coordinate (HC)',
	'International DDD (IDDD)',
	'Street',
	'City',
	'State/Province/Territory/Country',
	'Zip/Postal Code',
	'Point Code Flag',
	'Class 4/5 Switch',
	'END OFC',
	'HOST',
	'REMOTE',
	'DA OFC',
	'CLASS 4/5',
	'WIRELESS OFC',
	'FGD ADJ EO',
	'filler',
	'filler',
	'filler',
	'filler',
	'FGB TDM',
	'FGC TDM',
	'FGD TDM',
	'OS TDM',
	'INTERMED OFC',
	'DA TDM',
	'911 TDM',
	'FGD ADJ TDM',
	'LOCAL TDM',
	'INTRA TDM',
	'BCR5',
	'BCR6',
	'PRI 64',
	'ISDN MULTIRT',
	'ISDN FS OFC',
	'X.75 GATEWAY',
	'PACKET X.121',
	'PACKET E.164',
	'filler',
	'filler',
	'filler',
	'STP',
	'CCS AC OFC',
	'filler',
	'800 SSP',
	'LNP CAPABLE',
	'filler',
	'filler',
	'filler',
	'CIP',
	'CSP',
	'filler',
	'SW 56',
	'FGD 56',
	'FGD 64',
	'INTRA PRSUB',
	'CALL AGENT',
	'TRUNK GATEWAY',
	'ACCESS GATEWAY',
	'filler',
	'filler',
	'filler',
	'filler',
	'filler',
	'Creation Date in BIRRDS',
	'filler',
	'E STATUS Date',
	'filler',
	'Last Modificaiton Date',
	'filler',
	'CLN Indicator',
	'Bill-to RAO',
	'Send-to RAO',
	'filler',
);

my %merges = (
	'LATA'=>[
		'areacodes:LATA',
		'telcodata:LATA',
		'localcallingguide:LATA',
		'npanxxsource:LATA',
		'npanxxsource:Wire Center LATA',
		'neca4:lata',
	],
	'LATA Name'=>[
	],
	'Status'=>[
	],
	'Effective Date'=>[
	],
	'Switch'=>[
		'CLLI',
	],
	'Equipment Type'=>[
		'telcodata:Switch Type',
		'npanxxsource:Switch type',
		'localcallingguide:SWITCHTYPE',
	],
	'Administrative OCN (AOCN)'=>[
	],
	'Operating Company Number (OCN)'=>[
		'npanxxsource:OCN',
		'neca4:ocn',
	],
	'Vertical Coordinate (VC)'=>[
		'areacodes:RC_VERTICAL',
		'localcallingguide:RC-V',
		'npanxxsource:Wire Center V&H',
		'neca4:wcvh',
	],
	'Horizontal Coordinate (HC)'=>[
		'areacodes:RC_HORIZONTAL',
		'localcallingguide:RC-H',
		'npanxxsource:Wire Center V&H',
		'neca4:wcvh',
	],
	'International DDD (IDDD)'=>[
	],
	'Street'=>[
		'npanxxsource:Wire Center Address',
	],
	'City'=>[
		'npanxxsource:Wire Center City',
	],
	'State/Province/Territory/Country'=>[
	],
	'Zip/Postal Code'=>[
		'npanxxsource:Wire Center Zip',
	],
	'Point Code Flag'=>[
		'neca4:feat',
	],
	'Class 4/5 Switch'=>[
		'npanxxsource:Class 4/5 Switch',
	],
	'END OFC'=>[
		'neca4:feat',
	],
	'HOST'=>[
		'npanxxsource:Is Host',
		'neca4:feat',
	],
	'REMOTE'=>[
		'npanxxsource:Is Remote',
		'neca4:feat',
	],
	'DA OFC'=>[
		'npanxxsource:Is Tandem Operator Services',
		'neca4:feat',
	],
	'CLASS 4/5'=>[
		'npanxxsource:Is Class 4/5 Switch',
		'neca4:feat',
	],
	'WIRELESS OFC'=>[
		'neca4:feat',
	],
	'FGD ADJ EO'=>[
		'npanxxsource:Is Feature Group D EO',
		'neca4:feat',
	],
	'FGB TDM'=>[
		'npanxxsource:Is Tandem Feature Group B',
		'neca4:feat',
	],
	'FGC TDM'=>[
		'npanxxsource:Is Tandem Feature Group C',
		'neca4:feat',
	],
	'FGD TDM'=>[
		'npanxxsource:Is Tandem Feature Group D',
		'neca4:feat',
	],
	'OS TDM'=>[
		'npanxxsource:Is Tandem Operator Services',
		'neca4:feat',
	],
	'INTERMED OFC'=>[
		'neca4:feat',
	],
	'DA TDM'=>[
		'neca4:feat',
	],
	'911 TDM'=>[
		'neca4:feat',
	],
	'FGD ADJ TDM'=>[
		'neca4:feat',
	],
	'LOCAL TDM'=>[
		'npanxxsource:Is Tandem Local',
		'neca4:feat',
	],
	'INTRA TDM'=>[
		'npanxxsource:Is Tandem IntraLATA',
		'neca4:feat',
	],
	'BCR5'=>[
		'neca4:feat',
	],
	'BCR6'=>[
		'neca4:feat',
	],
	'PRI 64'=>[
		'neca4:feat',
	],
	'ISDN MULTIRT'=>[
		'neca4:feat',
	],
	'ISDN FS OFC'=>[
		'neca4:feat',
	],
	'X.75 GATEWAY'=>[
		'neca4:feat',
	],
	'PACKET X.121'=>[
		'neca4:feat',
	],
	'PACKET E.164'=>[
		'neca4:feat',
	],
	'STP'=>[
		'npanxxsource:Is SS7 STP',
		'neca4:feat',
	],
	'CCS AC OFC'=>[
		'neca4:feat',
	],
	'800 SSP'=>[
		'neca4:feat',
	],
	'LNP CAPABLE'=>[
		'neca4:feat',
	],
	'CIP'=>[
		'neca4:feat',
	],
	'CSP'=>[
		'neca4:feat',
	],
	'SW 56'=>[
		'neca4:feat',
	],
	'FGD 56'=>[
		'neca4:feat',
	],
	'FGD 64'=>[
		'neca4:feat',
	],
	'INTRA PRSUB'=>[
		'neca4:feat',
	],
	'CALL AGENT'=>[
		'npanxxsource:Is Call Agent',
		'neca4:feat',
	],
	'TRUNK GATEWAY'=>[
		'npanxxsource:Is Trunk Gateway',
		'neca4:feat',
	],
	'ACCESS GATEWAY'=>[
		'neca4:feat',
	],
	'Creation Date in BIRRDS'=>[
	],
	'E STATUS Date'=>[
	],
	'Last Modificaiton Date'=>[
	],
	'CLN Indicator'=>[
	],
	'Bill-to RAO'=>[
	],
	'Send-to RAO'=>[
	],
);

my %switchtype = ();

$fn = "equip.txt";
print STDERR "reading $fn...";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^#/;
	my ($equip,$desc) = split(/\t/,$_);
	$switchtype{$desc} = $equip;
}
close($fh);

my $header;

my @rc_keys = (
	'LATA',
	'Rate Center CLLI',
	'Vertical Coordinate (VC)',
	'Horizontal Coordinate (HC)',
	'State/Province/Territory/Country',
);

my %ratecenters = ();

$fn = "rc.csv";
print STDERR "reading $fn...";
open($fh,"<",$fn) or die "can't open $fn";
$header = 1;
while (<$fh>) { chomp;
	if ($header) {
		$header = undef;
		next;
	}
	s/^"//; s/"$//;
	my @fields = split(/","/,$_);
	for (my $i=0;$i<@rc_keys;$i++) {
		my $k = $rc_keys[$i];
		if (my $clli = $fields[1]) {
			$ratecenters{$clli}{$k} = $fields[$i] if $fields[$i];
		}
	}
}
close($fh);

my @wc_keys = (
	'LATA',
	'Wire Center CLLI',
	'Vertical Coordinate (VC)',
	'Horizontal Coordinate (HC)',
	'Street',
	'City',
	'State/Province/Territory/Country',
	'Zip/Postal Code',
);

my %wirecenters = ();

$fn = "wc.csv";
print STDERR "reading $fn...";
open($fh,"<",$fn) or die "can't open $fn";
$header = 1;
while (<$fh>) { chomp;
	if ($header) {
		$header = undef;
		next;
	}
	s/^"//; s/"$//;
	my @fields = split(/","/,$_);
	for (my $i=0;$i<@wc_keys;$i++) {
		my $k = $wc_keys[$i];
		if (my $clli = $fields[1]) {
			$wirecenters{$clli}{$k} = $fields[$i] if $fields[$i];
		}
	}
}
close($fh);

my %mapping = (
	'LATA'=>{
		'telcodata:LATA'=>sub{
			my $val = shift;
			return undef unless $val;
			if ($val =~ /\(([0-9]+)\)/) {
				return $1;
			}
			return undef;
		},
	},
	'Equipment Type'=>{
		'telcodata:Switch Type'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef if $val eq 'N/A';
			return undef if $val eq 'Unknown';
			if (exists $switchtype{$val}) {
				return $switchtype{$val};
			}
			return $val;
		},
	},
	'Vertical Coordinate (VC)'=>{
		'neca4:wcvh'=>sub{
			my $val = shift;
			return undef unless $val;
			my ($v,$h) = split(/,/,$val);
			return sprintf "%05d", $v;
		},
		'npanxxsource:Wire Center V&H'=>sub{
			my $val = shift;
			return undef unless $val;
			my ($v,$h) = split(/,/,$val);
			return sprintf "%05d", $v;
		},
		'areacodes:RC_VERTICAL'=>sub{
			my $val = shift;
			return undef unless $val;
			return sprintf "%05d", $val;
		},
		'localcallingguide:RC-V'=>sub{
			my $val = shift;
			return undef unless $val;
			return sprintf "%05d", $val;
		},
	},
	'Horizontal Coordinate (HC)'=>{
		'neca4:wcvh'=>sub{
			my $val = shift;
			return undef unless $val;
			my ($v,$h) = split(/,/,$val);
			return sprintf "%05d", $h;
		},
		'npanxxsource:Wire Center V&H'=>sub{
			my $val = shift;
			return undef unless $val;
			my ($v,$h) = split(/,/,$val);
			return sprintf "%05d", $h;
		},
		'areacodes:RC_HORIZONTAL'=>sub{
			my $val = shift;
			return undef unless $val;
			return sprintf "%05d", $val;
		},
		'localcallingguide:RC-H'=>sub{
			my $val = shift;
			return undef unless $val;
			return sprintf "%05d", $val;
		},
	},
	'Point Code Flag'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(CC|HA)\b/) ? 'X' : undef;
		},
	},
	'END OFC'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'HOST'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'REMOTE'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'DA OFC'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'CLASS 4/5'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'WIRELESS OFC'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'FGD ADJ EO'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(DA|DC)\b/) ? 'X' : undef;
		},
	},
	'FGB TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'FGC TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'FGD TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'OS TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(G8|G9|GB)\b/) ? 'X' : undef;
		},
	},
	'INTERMED OFC'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'DA TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(G1)\b/) ? 'X' : undef;
		},
	},
	'911 TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'FGD ADJ TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\bD1\b/) ? 'X' : undef;
		},
	},
	'LOCAL TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'INTRA TDM'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'BCR5'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'BCR6'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'PRI 64'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\bGJ\b/) ? 'X' : undef;
		},
	},
	'ISDN MULTIRT'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(TI)\b/) ? 'X' : undef;
		},
	},
	'ISDN FS OFC'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'X.75 GATEWAY'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(TF)\b/) ? 'X' : undef;
		},
	},
	'PACKET X.121'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(TE)\b/) ? 'X' : undef;
		},
	},
	'PACKET E.164'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'STP'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(HC)\b/) ? 'X' : undef;
		},
	},
	'CCS AC OFC'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'800 SSP'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\b(HB|HI)\b/) ? 'X' : undef;
		},
	},
	'LNP CAPABLE'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'CIP'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\bGQ\b/) ? 'X' : undef;
		},
	},
	'CSP'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'SW 56'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'FGD 56'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'FGD 64'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'INTRA PRSUB'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'CALL AGENT'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'TRUNK GATEWAY'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return undef;
		},
	},
	'ACCESS GATEWAY'=>{
		'neca4:feat'=>sub{
			my $val = shift;
			return undef unless $val;
			return ($val =~ /\bIP\b/) ? 'X' : undef;
		},
	},
);

my %mismatches = ();
my %matches = ();

sub closerecord {
	my $data = shift;
	if (my $clli = $data->{CLLI}) {
		return if $clli =~ /ALL SWITCHE/;
		return if $clli =~ /ALLSWITCHES/;
		return if $clli =~ /ODD BALL/;
		return if $clli =~ /XXXXXXXXXXX/;
		if ($clli eq 'BFLYONFRXEY') {
			$clli = $data->{CLLI} = 'BFLONYFRXEY'; # missspelled CLLI
		} elsif ($clli eq 'HNVAMBAYCM2') {
			$clli = $data->{CLLI} = 'HNVAMDAYCM2'; # missspelled CLLI
		} elsif ($clli eq 'LENYSKCJCM2') {
			$clli = $data->{CLLI} = 'LENYKSCJCM2'; # missspelled CLLI
		} elsif ($clli eq 'MTVINNAACM0') {
			$clli = $data->{CLLI} = 'MTNVINAACM0'; # missspelled CLLI
		} elsif ($clli eq 'WCHTSKSBRXF') {
			$clli = $data->{CLLI} = 'WCHTKSSBRXF'; # missspelled CLLI
		}
		$data->{'Switch'} = $clli;
		print STDERR "--------\n";
		my @values = ();
		foreach (@keys) {
			printf STDERR "V: %-30.30s: %s\n", $_, $data->{$_};
			push @values, $data->{$_};
		}
		print $of '"', join('","', @values), '"', "\n";

#		unless ($data->{LATA}) {
			if (my $st = substr($clli,4,2)) {
				if ($st =~ /(BC|AB|SK|MB|ON|PQ|NF|NS|PE|NT|UV|YT)/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 888 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '888';
					$data->{LATA} = 888;
				} elsif ($st =~ /GU/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 871 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '871';
					$data->{LATA} = 871;
				} elsif ($st =~ /PR/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 820 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '820';
					$data->{LATA} = 820;
				} elsif ($st =~ /VI/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 822 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '822';
					$data->{LATA} = 822;
				} elsif ($st =~ /BB/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 824 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '824';
					$data->{LATA} = 824;
				} elsif ($st =~ /JM/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 826 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '826';
					$data->{LATA} = 826;
				} elsif ($st =~ /DO/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 828 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '828';
					$data->{LATA} = 828;
				} elsif ($st =~ /SA/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 830 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '830';
					$data->{LATA} = 830;
				} elsif ($st =~ /MX/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 838 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '838';
					$data->{LATA} = 838;
				} elsif ($st =~ /NN/) {
					print STDERR "E: correcting LATA from $data->{LATA} to 870 base on CLLI $clli\n" if $data->{LATA} and $data->{LATA} ne '870';
					$data->{LATA} = 870;
				}
			}
#		}
		unless ($data->{'State/Province/Territory/Country'}) {
			if (my $st = substr($clli,4,2)) {
				$st = 'AS' if $st eq 'AM';
				$data->{'State/Province/Territory/Country'} = $st;
			}
		}
		if (my $clli8 = substr($clli,0,8)) {
			$data->{'Wire Center CLLI'} = $clli8;
			for (my $i=0;$i<@wc_keys;$i++) {
				my $k = $wc_keys[$i];
				$wirecenters{$clli8}{$k} = $data->{$k} if $data->{$k};
				$data->{$k} = $wirecenters{$clli8}{$k}
					unless $data->{$k} or !$wirecenters{$clli8}{$k};
			}
		}
		if (my $clli6 = substr($clli,0,6)) {
			$data->{'Rate Center CLLI'} = $clli6;
			for (my $i=0;$i<@rc_keys;$i++) {
				my $k = $rc_keys[$i];
				$ratecenters{$clli6}{$k} = $data->{$k} if $data->{$k};
				$data->{$k} = $ratecenters{$clli6}{$k}
					unless $data->{$k} or !$ratecenters{$clli6}{$k};
			}
		}

		printf $o2 "%-5.5s", $data->{'LATA'};
		printf $o2 "%-20.20s", $data->{'LATA Name'};
		printf $o2 "%-1.1s", $data->{'Status'};
		printf $o2 "%-6.6s", $data->{'Effective Date'};
		printf $o2 "%-11.11s", $data->{'Switch'};
		printf $o2 "%-5.5s", $data->{'Equipment Type'};
		printf $o2 "%-4.4s", $data->{'Administrative OCN (AOCN)'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-4.4s", $data->{'Operating Company Number (OCN)'};
		printf $o2 "%-5.5s", $data->{'Vertical Coordinate (VC)'};
		printf $o2 "%-5.5s", $data->{'Horizontal Coordinate (HC)'};
		printf $o2 "%-1.1s", $data->{'International DDD (IDDD)'};
		printf $o2 "%-60.60s", $data->{'Street'};
		printf $o2 "%-30.30s", $data->{'City'};
		printf $o2 "%-2.2s", $data->{'State/Province/Territory/Country'};
		printf $o2 "%-9.9s", $data->{'Zip/Postal Code'};
		printf $o2 "%-1.1s", $data->{'Point Code Flag'};
		printf $o2 "%-11.11s", $data->{'Class 4/5 Switch'};
		printf $o2 "%-1.1s", $data->{'END OFC'};
		printf $o2 "%-1.1s", $data->{'HOST'};
		printf $o2 "%-1.1s", $data->{'REMOTE'};
		printf $o2 "%-1.1s", $data->{'DA OFC'};
		printf $o2 "%-1.1s", $data->{'CLASS 4/1'};
		printf $o2 "%-1.1s", $data->{'WIRELESS OFC'};
		printf $o2 "%-1.1s", $data->{'FGD ADJ EO'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'FGB TDM'};
		printf $o2 "%-1.1s", $data->{'FGC TDM'};
		printf $o2 "%-1.1s", $data->{'FGD TDM'};
		printf $o2 "%-1.1s", $data->{'OS TDM'};
		printf $o2 "%-1.1s", $data->{'INTERMED OFC'};
		printf $o2 "%-1.1s", $data->{'DA TDM'};
		printf $o2 "%-1.1s", $data->{'911 TDM'};
		printf $o2 "%-1.1s", $data->{'FGD ADJ TDM'};
		printf $o2 "%-1.1s", $data->{'LOCAL TDM'};
		printf $o2 "%-1.1s", $data->{'INTRA TDM'};
		printf $o2 "%-1.1s", $data->{'BCR5'};
		printf $o2 "%-1.1s", $data->{'BCR6'};
		printf $o2 "%-1.1s", $data->{'PRI 64'};
		printf $o2 "%-1.1s", $data->{'ISDN MULTIRT'};
		printf $o2 "%-1.1s", $data->{'ISDN FS OFC'};
		printf $o2 "%-1.1s", $data->{'X.75 GATEWAY'};
		printf $o2 "%-1.1s", $data->{'PACKET X.121'};
		printf $o2 "%-1.1s", $data->{'PACKET E.164'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'STP'};
		printf $o2 "%-1.1s", $data->{'CCS AC OFC'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'800 SSP'};
		printf $o2 "%-1.1s", $data->{'LNP CAPABLE'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'CIP'};
		printf $o2 "%-1.1s", $data->{'CSP'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'SW 56'};
		printf $o2 "%-1.1s", $data->{'FGD 56'};
		printf $o2 "%-1.1s", $data->{'FGD 64'};
		printf $o2 "%-1.1s", $data->{'INTRA PRSUB'};
		printf $o2 "%-1.1s", $data->{'CALL AGENT'};
		printf $o2 "%-1.1s", $data->{'TRUNK GATEWAY'};
		printf $o2 "%-1.1s", $data->{'ACCESS GATEWAY'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-36.36s", $data->{'filler'};
		printf $o2 "%-6.6s", $data->{'Creation Date in BIRRDS'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-6.6s", $data->{'E STATUS Date'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-6.6s", $data->{'Last Modificaiton Date'};
		printf $o2 "%-1.1s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'CLN Indicator'};
		printf $o2 "%-3.3s", $data->{'Bill-to RAO'};
		printf $o2 "%-3.3s", $data->{'Send-to RAO'};
		printf $o2 "%-12.12s", $data->{'filler'};
		print $o2 "\n";
	}
}

$fn = "LERG7.DAT";
open($o2,">",$fn) or die "can't open $fn";
printnotice($o2);
$fn = "lerg7.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";
$fn = "clli.csv";
open($fh,"<",$fn) or die "can't open $fn";
my $header = 1;
my @fields = ();
while (<$fh>) { chomp;
	s/^"//; s/"$//;
	my @tokens = split(/","/,$_);
	if ($header) {
		@fields = @tokens;
		$header = undef;
		next;
	}
	my $data = {};
	for (my $i=0;$i<@fields;$i++) {
		$data->{$fields[$i]} = $tokens[$i];
	}
	foreach my $key (sort keys %merges) {
		my $mismatch = 0;
		my @vals = ();
		my @srcs = ();
		foreach my $fld (@{$merges{$key}}) {
			my $val = $data->{$fld};
			$val = &{$mapping{$key}{$fld}}($val)
				if $val and exists $mapping{$key}{$fld};
			if ($val) {
				if (my $pre = $data->{$key}) {
					if ($pre ne $val) {
						my $upre = "\U$pre\E";
						my $uval = "\U$val\E";
						if ($upre ne $uval) {
							$mismatch++;
						}
					}
				}
				$data->{$key} = $val;
				push @srcs, $fld;
				push @vals, $val;
			}
		}
		if ($mismatch) {
			printf STDERR "E: --- --- -\n";
			for (my $i=0;$i<@vals;$i++) {
				my $flag;
				my $val = $vals[$i];
				my $src = $srcs[$i];
				my $cur = $data->{$key};
				my $ucur = "\U$cur\E";
				my $uval = "\U$val\E";
				if (($cur eq $val) or ($ucur eq $uval)) {
					$flag = '';
					$matches{$key}{$src}++;
				} else {
					$flag = '****';
					$mismatches{$key}{$src}++;
				}
				printf STDERR "E: %-4.4s %-32.32s %s $flag\n", $data->{OCN}, $key.' '.$src,"'$val'";
			}
		} elsif (@vals > 1) {
			printf STDERR "I: --- --- -\n";
			for (my $i=0;$i<@vals;$i++) {
				printf STDERR "I: %-4.4s %-32.32s %s\n", $data->{OCN}, $key.' '.$srcs[$i],"'$vals[$i]'";
				$matches{$key}{$srcs[$i]}++;
			}
		}
	}
	closerecord($data);
}
close($fh);
close($of);
close($o2);

foreach my $k (keys %mismatches) {
	foreach my $s (keys %{$mismatches{$k}}) {
		printf "I: %9d mismatches on %-10.10s by %s\n", $mismatches{$k}{$s}, $k, $s;
	}
}
foreach my $k (keys %matches) {
	foreach my $s (keys %{$matches{$k}}) {
		printf "I: %9d    matches on %-10.10s by %s\n", $matches{$k}{$s}, $k, $s;
	}
}

$fn = "rc.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@rc_keys), '"', "\n";
foreach (sort keys %ratecenters) {
	my @values = ();
	for (my $i=0;$i<@rc_keys;$i++) {
		my $k = $rc_keys[$i];
		push @values, $ratecenters{$_}{$k};
	}
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

$fn = "wc.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@wc_keys), '"', "\n";
foreach (sort keys %wirecenters) {
	my @values = ();
	for (my $i=0;$i<@wc_keys;$i++) {
		my $k = $wc_keys[$i];
		push @values, $wirecenters{$_}{$k};
	}
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

