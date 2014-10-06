#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $codedir = "$progdir/data";
my $datadir = "$codedir/areacodes";

use strict;
use Data::Dumper;
use Encode qw(encode decode);

my $fh = \*INFILE;

my $dd = {};
my $db = {};

my $sts = {};

open($fh,"<","$codedir/states.txt");
while (<$fh>) { chomp;
	next unless /^[A-Z][A-Z]\t/;
	my ($cc,$st,$nm,$tz) = split(/\t/,$_);
	$nm = "\U$nm\E";
	$st = $cc unless $st;
	$sts->{$nm} = $st;
}
close($fh);

open($fh,"<:utf8","$codedir/states.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[A-Z][A-Z]\t/;
	my ($cc,$sts,$name,$tzs) = split(/\t/,$_);
	my ($tz,$dst);
	if ($tzs =~ /\//) {
		($tz,$dst) = split(/\//,$tzs,2);
	} else {
		$tz = $tzs;
	}
	$db->{states}{$name} = {cc=>$cc,sts=>$sts,name=>$name,tz=>$tz,dst=>$dst};
}
close($fh);

open($fh,"<:utf8","$codedir/nanp.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9][0-9][0-9]\t/;
	my ($npa,$cc,$state,$nongeo) = split(/\t/,$_);
	my ($st,$tz,$dst);
	if (my $sta = $db->{states}{$state}) {
		$st = $sta->{sts};
		$tz = $sta->{tz};
		$dst = $sta->{dst};
	}
	$db->{npa}{$npa}{npa}       = $npa;
	$db->{npa}{$npa}{cc}        = $cc;
	$db->{npa}{$npa}{st}        = $st;
	$db->{npa}{$npa}{state}     = $state;
	$db->{npa}{$npa}{tz}{$tz}   = 1;
	$db->{npa}{$npa}{dst}{$dst} = 1;
	$db->{npa}{$npa}{nongeo}    = $nongeo;
}
close($fh);

my $hist = [
	'Unused',
	'NYNEX',
	'Bell Atlantic',
	'Ameritech',
	'BellSouth',
	'SouthWestern Bell Telephone',
	'US West',
	'Pacific Telesis Group',
	'Off Shore and International',
	'Independents',
];

open($fh,"<","$codedir/AREACODEWORLD-GOLD-SAMPLE.csv") or die "can't open file";
my $header = 1;
my @fields = ();
while (<$fh>) { chomp;
	if ($header) {
		@fields = split(/\t/,$_);
		for (my $i=0;$i<@fields;$i++) {
			$fields[$i] =~ s/^"(.*?)"$/\1/;
		}
		$header = undef;
		next;
	}
	my @values = split(/\t/,$_);
	for (my $i=0;$i<@values;$i++) {
		$values[$i] =~ s/^"(.*?)"$/\1/;
	}
	my $data = {};
	if (exists $dd->{$values[0]}{$values[1]}) {
		$data = $dd->{$values[0]}{$values[1]};
	} else {
		$dd->{$values[0]}{$values[1]} = $data;
	}
	for (my $i=0;$i<@fields;$i++) {
		if (exists $data->{$fields[$i]}) {
			if (ref $data->{$fields[$i]} eq 'HASH') {
				$data->{$fields[$i]}{$values[$i]} = 1;
			} else {
				$data->{$fields[$i]} = {$data->{$fields[$i]}=>1,$values[$i]=>1 }
				unless $data->{$fields[$i]} eq $values[$i];
			}
		} else {
			$data->{$fields[$i]} = $values[$i] if $values[$i];
		}
	}
	$dd->{switch}{$data->{SWITCH_CLLI}}{nxx}{"$data->{NPA}-$data->{NXX}"} = 1;
	my ($npa,$nxx) = ($data->{NPA},$data->{NXX});
	my $lata = $data->{LATA};
	my $state = $data->{STATE};
	my $ocn = $data->{OCN};
	my $ovly = $data->{OVERLAY};
	my $company = $data->{COMPANY};
	my $rate = $data->{RATE_CENTER};
	my $clli = $data->{SWITCH_CLLI};
	my $cntry = $data->{COUNTRY};
	my $cty = $data->{CITY};
	my $county = $data->{COUNTY};
	my $lat = $data->{LATITUDE};
	my $lon = $data->{LONGITUDE};
	my $tz = $data->{TIMEZONE};
	my $dst = $data->{OBSERVES_DST};
	my $new = $data->{NEW_NPA};
	my $nty = $data->{NXX_USE_TYPE};
	my $v = $data->{RC_VERTICAL};
	my $h = $data->{RC_HORIZONTAL};
	my $ver = $data->{NXX_INTRO_VERSION};
	my $wire = substr($clli,0,8);
	my $city = substr($clli,0,6);
	if ($npa) {
		$db->{npa}{$npa}{npa}   = $npa;
		$db->{npa}{$npa}{cc}    = $cntry;
		#$db->{npa}{$npa}{state} = $state;
		if (defined $tz) {
			$db->{npa}{$npa}{tz}{$tz}++;
			$db->{npa}{$npa}{dst}{$dst}++;
		}
		$db->{npa}{$npa}{ovly}  = $ovly;
	}
	if ($nxx) {
		$db->{nxx}{$npa}{$nxx} = {} unless exists $db->{nxx}{$npa}{$nxx};
		my $data = $db->{nxx}{$npa}{$nxx};
		$data->{npa}    = $npa;
		$data->{nxx}    = $nxx;
		$data->{ocn}    = $ocn;
		$data->{state}  = $state;
		$data->{lata}   = $lata;
		$data->{clli}   = $clli;
		$data->{wire}   = $wire;
		$data->{rate}   = $rate;
		$data->{city}   = $cty;
		$data->{county} = $county;
		$data->{cc}     = $cntry;
		$data->{nty}    = $nty;
		$data->{ver}    = $ver;
	}
	if ($rate) {
		$db->{rate}{$state}{$rate}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
	if ($clli) {
		$db->{clli}{$clli}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
	if ($wire) {
		$db->{wire}{$wire}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
	if ($city) {
		$db->{city}{$city}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
	if ($state) {
		$db->{state}{$state}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
	if ($ocn) {
		my $cp = $company;
		if ($cp =~ /^(.*?) *- *([A-Z][A-Z])$/) {
			$cp = $1;
			$db->{ocn}{$ocn}{'State'} = $2;
		} elsif ($cp =~ / *- *([A-Z][A-Z]) *- */) {
			$db->{ocn}{$ocn}{'State'} = $1;
		} elsif ($cp =~ /  *([A-Z][A-Z])$/) {
			$db->{ocn}{$ocn}{'State'} = $1;
			$cp =~ s/  *[A-Z][A-Z]$//;
		} elsif ($cp =~ / ([A-Z][A-Z])-CCO/) {
			$db->{ocn}{$ocn}{'State'} = $1;
		} elsif ($cp =~ / *- *([A-Z][A-Z])  *[A-Z]+$/) {
			$db->{ocn}{$ocn}{'State'} = $1;
		} else {
			my $CP = "\U$cp\E";
			foreach my $tok (keys %{$sts}) {
				if (index($CP,$tok) >= 0) {
					$db->{ocn}{$ocn}{'State'} = $sts->{$tok};
					last;
				}
			}
		}
		if ($cp =~ /\b(CAP|CLEC|GENERAL|INTL|L RESELLER|IC|ILEC|IPES|P RESELLER|PCS|RBOC|ULEC|WIRELESS|W RESELLER)\b/) {
			my $category = $1;
			if (exists $db->{ocn}{$ocn}{Category} and $db->{ocn}{$ocn}{Category} ne $category) {
				warn "$ocn $npa-$nxx category changed from $db->{ocn}{$ocn}{Category} to $category";
				$db->{ocn}{$ocn}{LKA} = 'X';
			}
			$db->{ocn}{$ocn}{'Category'} = $category;
		}
		if (exists $db->{ocn}{$ocn}{Company} and $db->{ocn}{$ocn}{Company} ne $cp) {
			warn "$ocn $npa-$nxx company changed from $db->{ocn}{$ocn}{Company} to $cp";
			$db->{ocn}{$ocn}{LKA} = 'X';
		}
		$db->{ocn}{$ocn}{Company} = $cp;
		$db->{ocn}{$ocn}{States}{$state}++;
		$db->{ocn}{$ocn}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
	if ($lata) {
		$db->{lata}{$lata}{lata} = $lata;
		$db->{lata}{$lata}{'NPA NXXs'}{$npa}{$nxx} = 1;
		$db->{lata}{$lata}{hist} = $hist->[int(substr($lata,0,1))];
		if ($lata eq '888') {
			$db->{lata}{$lata}{hist} = 'Canada';
		} elsif ($lata eq '99999') {
			$db->{lata}{$lata}{hist} = 'Not applicable';
		}
	}
}
close($fh);

#open($fh,">","$datadir/dump.pl");
#my $dumper = Data::Dumper->new([$db]);
#print $fh $dumper->Dump;
#close($fh);

#my $dumper = Data::Dumper->new([$dd]);
#print $dumper->Dump;

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
}

my $of = \*OUTFILE;
my $o2 = \*OUTFILE2;

print STDERR "writing $datadir/LERG1.DAT\n";
open($of,">","$datadir/LERG1.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG1.DAT
# ---------
# Telcordia LERGINFO.DOC says: "The LERG1 file provides high-level information
# about Operating Company Numbers (OCNs) such as the name of, and type of,
# company.  This includes contact information (for purposes of LERG data) as
# has been provided to TRA.  Note that contact information is maintained as
# best as possible; however, due to constant changes in personnel in a company,
# reorganizations, mergers, acquisitions, etc., some information will not be up
# to date.  In addition, the contact noted may not necessarily be familiar with
# all aspects of a company's operation (network, billing, etc.).  The contact
# name defaults to the party requesting the assignment of the number used as
# the OCN unless the company has specifically requested another party be listed
# for purposes of addressing LERG data.
#
# Note that the maintenance of company names are the purview of the company
# itself.  Although a company may change its name for various reasons (e.g.
# mergers and acquisitions), at times, the name used at the consumer level may
# not be the legal name of the company.  If you have any questions regarding a
# company's name as listed in LERG1, please contact the company directly to
# either receive an explanation or to prompt the company to have the data
# changed through the proper channels."
# ---------
# You will never find any contact information (personal identifying
# information) in this file.  If you wish to contact a responsible person,
# duckduckgo the operating company name.  If you would like to correct
# information contained in this file, use the ss7view utility to submit your
# corrections to the OpenSS7 project.  Also remember to submit changes to NECA
# or Telcordia as necessary to change other datasets.
# ---------
# OCN/Company Names (Key field 1)
# ---------
#   1-  4  4 Operating Company Number (OCN)
#   4- 54 50 Operating Company Name
#  55- 74 20 Abbreviated OCN Name
#  75- 76  2 State/Province/Territory/Country of OCN
#  77- 86 10 Category
#            CAP        - competitive access provider
#            CLEC       - competitive local exchange carrier
#            GENERAL    - various miscellaneous cases
#            INTL       - international
#            L RESELLER - local reseller
#            IC         - interexchange carrier
#            ILEC       - incumbent local exchange carrier
#            IPES       - internet protocol enabled services
#            P RESELLER - peronsal communication services reseller
#            PCS        - personal communications service
#            RBOC       - regional Bell operating company
#            ULEC       - unbundled local exchange carrier
#            WIRELESS   - wireless provided (non-PCS cellular, paging, radio)
#            W RESELLER - wireless reseller
#  87- 90  4 Overall OCN
#  91- 95  5 filler
#  96- 96  1 Last Known Address indicatory (X=reported address supposedly invalid)
#  97-116 20 Last Name
# 117-126 10 First Name
# 127-127  1 Middle Initial
# 128-177 50 Company Name (contact specific)
# 178-207 30 Title
# 208-237 30 Address 1
# 238-267 30 Address 2
# 268-277 10 Floor
# 278-297 20 Room
# 298-317 20 City
# 318-319  2 State/Province/Territory/Country
# 320-328  9 Zip/Postal Code
# 329-340 12 Phone
# 341-350 10 filler
# ---------
EOF
foreach my $ocn (sort keys %{$db->{ocn}}) {
	my $data = $db->{ocn}{$ocn};
	my $state = $data->{State};
	unless ($state) {
		my ($st,$ct);
		foreach my $key (keys %{$data->{States}}) {
			next unless $key;
			if ($data->{States}{$key} > $ct) {
				$st = $key;
				$ct = $data->{States}{$key};
			}
		}
		$data->{State} = $state = $st if $st;
	}
	printf $of "%-4.4s", $ocn;
	printf $of "%-50.50s", $data->{Company};
	printf $of "%-20.20s", '';
	printf $of "%-2.2s", $data->{State};
	printf $of "%-10.10s", $data->{Category};
	printf $of "%-4.4s", '';
	printf $of "%-5.5s", '';
	printf $of "%-1.1s", $data->{LKA};
	printf $of "%-20.20s", '';
	printf $of "%-10.10s", '';
	printf $of "%-1.1s", '';
	printf $of "%-50.50s", '';
	printf $of "%-30.30s", '';
	printf $of "%-30.30s", '';
	printf $of "%-30.30s", '';
	printf $of "%-10.10s", '';
	printf $of "%-20.20s", '';
	printf $of "%-20.20s", '';
	printf $of "%-2.2s", '';
	printf $of "%-9.9s", '';
	printf $of "%-12.12s", '';
	printf $of "%-10.10s", '';
	print $of "\n";
}
close($of);

print STDERR "writing $datadir/LERG1CON.DAT\n";
open($of,">","$datadir/LERG1CON.DAT");
printnotice($of);
print $of <<'EOF';
# ------------
# LERG1CON.DAT
# ------------
# Telcordia's LERGINFO.DOC says: "The LERG1CON file is information directly
# entered by each OCN or its agent.  All OCNs will have, at minimum, a SERVICE
# OF SUBPOENA contact identified (there may not necessarily be a telephone or
# additional information though).  Note that since these records are
# established on a per company basis, the extent of information may vary.
# Maintenance of the data is the purview of the OCN."
# ------------
# You will never find any contact information (personal identifying
# information) in this file: not even a SERVICE OF SUBPOENA contact.  If you
# wish to contact a responsible person, duckduckgo the operating company name.
# ------------
# OCN/Additional Contacts (Key field 1,4)
# ------------
#   1-  4  4 Operating Company Number (OCN)
#   5- 54 50 Operating Company Name
#  55- 56  2 State/Province/Territory/Country of OCN
#  57- 76 20 Contact Function
#  77- 86 10 Contact Phone
#  87-156 70 Contact Information
# 157-172 16 filler
# ------------
EOF
foreach my $ocn (sort keys %{$db->{ocn}}) {
	my $data = $db->{ocn}{$ocn};
	printf $of "%-4.4s", $ocn;
	printf $of "%-50.50s", $data->{Company};
	printf $of "%-2.2s", $data->{State};
	printf $of "%-20.20s", '';
	printf $of "%-10.10s", '';
	printf $of "%-70.70s", '';
	printf $of "%-16.16s", '';
	print $of "\n";
}
close($of);

print STDERR "writing $datadir/LERG3.DAT\n";
open($of,">","$datadir/LERG3.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG3.DAT
# ---------
# Telcordia's LERGINFO.DOC says: "The LERG3 file contains high-level
# information about NPAs (Area Codes).  This includes the date the NPA went or
# is going into effect, permissive dialing periods for splits, the NPA that had
# previously served the area (or continues to do so in a case of an overlay),
# etc."
# ---------
# This file contains next to no information about permissive dialing periods,
# splits or overlays.  It does not contain test numbers.  It also only contains
# one timezone (the primary one) for each NPA.  This is basically a list of
# NPAs.  The same information (in a different format) can be obtained from
# http://en.wikipedia.com/ under NANP.
# ---------
# NPA (Key field 1)
# ---------
#   1-  3   3 NPA
#   4-  5   2 State/Province/Territory/Country
#   6- 25  20 NPA Location - spelled out
#  26- 26   1 Time Zone 1 (see LERGINFO for values)
#  27- 27   1 Time Zone 2 (see LERGINFO for values)
#  28- 28   1 Time Zone 3 (see LERGINFO for values)
#  29- 29   1 Time Zone 4 (see LERGINFO for values)
#  30- 37   8 Test Number 1 (NXX-line)
#  38- 43   6 Test Number 1 start date (mmddyy)
#  44- 49   6 Test Number 1 end date (mmddyy)
#  50- 57   8 Test Number 2 (NXX-line)
#  58- 63   6 Test Number 2 start date (mmddyy)
#  64- 69   6 Test Number 2 end date (mmddyy)
#  70- 77   8 Test Number 3 (NXX-line)
#  78- 83   6 Test Number 3 start date (mmddyy)
#  84- 89   6 Test Number 3 end date (mmddyy)
#  90- 97   8 Test Number 4 (NXX-line)
#  98-103   6 Test Number 4 start date (mmddyy)
# 104-109   6 Test Number 4 end date (mmddyy)
# 110-110   1 Action (S=geographic split,O=overlay)
# 111-116   6 Permissive (dual NPA) Dialing Start Date (mmddyy)
# 117-122   6 Permissive (dual NPA) Dialing End Date (mmddyy)
# 123-125   3 Derived from NPA 1
# 126-128   3 Derived from NPA 2
# 129-131   3 Derived from NPA 3
# 132-132   1 Action (S=geographic split,O=overlay)
# 133-138   6 Permissive (dual NPA) Dialing Start Date (mmddyy)
# 139-144   6 Permissive (dual NPA) Dialing End Date (mmddyy)
# 145-147   3 Derived from NPA 1
# 148-150   3 Derived from NPA 2
# 151-153   3 Derived from NPA 3
# 154-213  60 Footnote (for clarification needs)
# 214-214   1 Action (S=geographic split,O=overlay)
# 215-220   6 Permissive (dual NPA) Dialing Start Date (mmddyy)
# 221-226   6 Permissive (dual NPA) DIaling End Date (mmddyy)
# 227-229   3 NPA
# 230-469 240 Above 4 fields repeated 15 more times
# 470-480  11 filler
# ---------
EOF
foreach my $key (sort keys %{$db->{npa}}) {
	my $npa = $db->{npa}{$key};
	printf $of "%-3.3s", $npa->{npa};
	printf $of "%-2.2s", ($npa->{cc} eq 'US' or $npa->{cc} eq 'CA') ? $npa->{st} : $npa->{cc};
	my $state = $npa->{state};
	#$state = "\U$state\E";
	printf $of "%-20.20s", $state;
	my $i = 0;
	foreach my $tz (sort {$a<=>$b} keys %{$npa->{tz}}) {
		if (!$tz) {
			$tz = 0;
		} elsif ($tz == 10) {
			$tz = 1;
		} elsif ($tz == -10 or $tz == -11) {
			$tz = 2;
		} elsif ($tz == -9) {
			$tz = 3;
		} elsif ($tz == -8) {
			$tz = 4;
		} elsif ($tz == -7) {
			$tz = 5;
		} elsif ($tz == -6) {
			$tz = 6;
		} elsif ($tz == -5) {
			$tz = 7;
		} elsif ($tz == -4) {
			$tz = 8;
		} elsif ($tz == -3.5 or $tz == -3.3) {
			$tz = 9;
		}
		printf $of "%-1.1s", $tz;
		$i++;
		last if $i >= 4;
	}
	for (;$i<4;$i++) {
		printf $of "%-1.1s", '';
	}
	for (my $i=0;$i<4;$i++) {
		printf $of "%-8.8s", ''; # Test Number
		printf $of "%-6.6s", ''; # Test Number start date (mmddyy)
		printf $of "%-6.6s", ''; # Test Number end date (mmddyy)
	}
	for (my $i=0;$i<2;$i++) {
		printf $of "%-1.1s", ''; # Action (S|O)
		printf $of "%-6.6s", ''; # Permissive dialing start date
		printf $of "%-6.6s", ''; # Permissive dialing end date
		printf $of "%-3.3s", ''; # Derivfed from NPA 1
		printf $of "%-3.3s", ''; # Derivfed from NPA 2
		printf $of "%-3.3s", ''; # Derivfed from NPA 3
	}
	printf $of "%-60.60s", ''; # Footnote
	for (my $i=0;$i<16;$i++) {
		printf $of "%-1.1s", ''; # Action (S|O)
		printf $of "%-6.6s", ''; # Permissive dialing start date
		printf $of "%-6.6s", ''; # Permissive dialing end date
		printf $of "%-3.3s", ''; # NPA
	}
	printf $of "%-11.11s", ''; # filler
	print $of "\n";
}
close($of);

print STDERR "writing $datadir/LERG5.DAT\n";
open($of,">","$datadir/LERG5.DAT");
printnotice($of);
printf $of <<'EOF';
# ---------
# LERG5.DAT
# ---------
# The LERG5 file identifies the NPAs in use within a LATA.  The LATA
# information is then grouped by Region.
# ---------
# LATA codes (by Region) (Key field 2)
# ---------
#   1- 40 40 Historical Region
#  41- 45  5 LATA
#  46- 65 20 LATA Name
#  66- 68  3 NPA 1
#  69- 71  3 NPA 2
#  ...
# 153-155  3 NPA 30
# 156-165 10 filler
# ---------
#--------------------------------------++---++------------------++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++---------+
EOF
foreach my $key (sort keys %{$db->{lata}}) {
	my $lata = $db->{lata}{$key};
	printf $of "%-40.40s", $lata->{hist};
	printf $of "%-5.5s", $lata->{lata};
	printf $of "%-20.20s", '';
	my $i = 0;
	foreach my $npa (sort keys %{$lata->{'NPA NXXs'}}) {
		printf $of "%-3.3s", $npa;
		$i++;
		last if $i >= 30;
	}
	for (;$i<30;$i++) {
		printf $of "%-3.3s", '';
	}
	printf $of "%-10.10s", ''; # filler
	print $of "\n";
}
close($of);

print STDERR "writing $datadir/LERG6.DAT\n";
open($of,">","$datadir/LERG6.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG6.DAT
# ---------
# The LERG6 file provides NPA NXX (central office code) information and, where
# applicable, BLOCK assignments within the NPA NXX.  NPA NXX assignments (those
# made by the CO Code Administrator) are represented with a BLOCK value of "A".
# Thousands Block Pooling assignments (those made by the Pooling Admnistrator)
# are represented by numeric BLOCK values (0-9).  Only those BLOCKs actually
# assigned are provided (i.e., not all blocks 0-9, may appear).  This file
# includes amoung other data, the Rate Center associated with each record, the
# OCN (company assigned the record), the serving switch/POI with an associated
# switch homing arrangement (SHA) indicator, the identified "use" of the record
# (e.g. cellular, wireline -- see COCTYPE/SSC), and LATA (both switch and Rate
# Center LATA).
# ---------
# NPA NXX Block Assignments (Key field 5,6,7)
# ---------
#   1-  5  5 LATA (of Switch Location)
#   6- 25 20 LATA Name
#  26- 26  1 Status ( |E|M|D)
#  27- 32  6 Effective Date (mmddyy)
#  33- 35  3 NPA
#  36- 38  3 NXX (Central Office Code)
#  39- 39  1 Block ID (A, 0-9)
#  40- 43  4 filler
#  44- 46  3 COC Type
#  47- 50  4 Special Service Code (SSC)
#  51- 51  1 Dialable Indicator (DIND)(Y/N)
#  52- 53  2 Terminating Digits - End Office (EO)
#  54- 55  2 Terminating Digits - Access Tandem (AT)
#  56- 56  1 Portable Indicator (Y/N)
#  57- 60  4 Administrative OCN (AOCN)
#  61- 61  1 filler
#  62- 65  4 Operating Company Number (OCN)
#  66- 75 10 Locality Name
#  76- 77  2 Locality Country
#  78- 79  2 Locality State/Province/Territory/Country
#  80- 89 10 RC Name Abbreviation
#  90- 90  1 RC Type
#  91- 94  4 Lines from (0000,1000,...) (1212 for NXX 555)
#  95- 98  4 Lines to   (0999,1999,...) (1212 for NXX 555)
#  99-109 11 Switch
# 110-111  2 Switch Homing Arrangement (SHA) Indicator
# 112-115  4 filler
# 116-119  4 Test line number (XXXX) (i.e. NPA NXX XXXX)
# 120-120  1 Response (A)nnouncement/(M)illiwat Tone
# 121-126  6 Expiration Date (mmddyy)
# 127-127  1 Thousands Block Pooling Indicator
# 128-132  5 LATA (of Rate Center Location)
# 133-133  1 filler
# 134-139  6 Creation Date in BIRRDS (mmddyy)
# 140-140  1 filler
# 141-146  6 "E" STATUS date (mmddyy)
# 147-147  1 filler
# 148-153  6 Last Modification Date (mmddyy)
# 154-170 17 filler
# ---------
EOF
foreach my $key (sort keys %{$db->{nxx}}) {
	foreach my $ke2 (sort keys %{$db->{nxx}{$key}}) {
		my $nxx = $db->{nxx}{$key}{$ke2};
		printf $of "%-5.5s", $nxx->{lata};
		printf $of "%-20.20s", '';
		printf $of "%-1.1s", ' '; # Status ( |E|M|D)
		printf $of "%-6.6s",
			substr($nxx->{ver},5,2).
			substr($nxx->{ver},8,2).
			substr($nxx->{ver},2,2);
		printf $of "%-3.3s", $nxx->{npa};
		printf $of "%-3.3s", $nxx->{nxx};
		printf $of "%-1.1s", 'A'; # Block ID
		printf $of "%-4.4s", ''; # filler
		my $coc = '???';
		my $ssc = 'N';
		my $dind = 'N';
		my $treo = 'NA';
		my $trat = 'NA';
		if ($nxx->{nty} eq 'L') {
			$coc = 'EOC'; $ssc = 'N'; $dind = 'Y';
		} elsif ($nxx->{nty} eq 'W') {
			$coc = 'PMC'; $ssc = 'C'; $dind = 'Y';
		} else {
			warn "unknown NXX type '$nxx->{nty}'";
		}
		printf $of "%-3.3s", $coc;
		printf $of "%-4.4s", $ssc;
		printf $of "%-1.1s", $dind;
		printf $of "%-2.2s", $treo;
		printf $of "%-2.2s", $trat;
		printf $of "%-1.1s", 'Y';
		printf $of "%-4.4s", 'NANP';
		printf $of "%-1.1s", '';
		printf $of "%-4.4s", $nxx->{ocn};
		printf $of "%-10.10s", $nxx->{wire};
		printf $of "%-2.2s", $nxx->{county};
		printf $of "%-10.10s", $nxx->{rate};
		printf $of "%-1.1s", ' ';
		printf $of "%-4.4s", '0000';
		printf $of "%-4.4s", '9999';
		printf $of "%-11.11s", $nxx->{clli};
		printf $of "%-2.2s", '00'; # SHA (00 or 01-99)
		printf $of "%-4.4s", ''; # filler
		printf $of "%-4.4s", ''; # test line number
		printf $of "%-1.1s", ''; # response (A|M)
		printf $of "%-6.6s", ''; # expiration date
		printf $of "%-1.1s", 'Y'; # 1000's block pooling
		printf $of "%-5.5s", $nxx->{lata};
		printf $of "%-1.1s", ''; # filler
		printf $of "%-6.6s",
			substr($nxx->{ver},5,2).
			substr($nxx->{ver},8,2).
			substr($nxx->{ver},2,2);
		printf $of "%-1.1s", ''; # filler
		printf $of "%-6.6s", ''; # E STATUS date
		printf $of "%-1.1s", ''; # filler
		printf $of "%-6.6s", ''; # Last modified date
		printf $of "%-17.17s", ''; # filler
		print $of "\n";
	}
}
close($of);
