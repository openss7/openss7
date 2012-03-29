#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $codedir = "$progdir/data";
my $datadir = "$codedir/npanxx";

use strict;
use Data::Dumper;
use Encode qw(encode decode);

my $fh = \*INFILE;

my @files = `find $datadir -name '*.html.xz'`;

my $db = {};

my $fields = {};
my $headings = {};
my $subhead  = {};

#$db->{'NPA NXX'} = {};
#$db->{'Wire Center'} = {};
#$db->{'Switch'} = {};
#$db->{'Rate Center - Locality'} = {};
#$db->{'LATA'} = {};
#$db->{'Carrier'} = {};

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

foreach my $file (@files) {

	my @lines = `xzcat $file`;

#	my @lines = ();

#	open($fh,"<","$datadir/npanxx.tst");
#	while (<$fh>) { chomp;
#		push @lines, $_;
#	}
#	close($fh);

	my $entry = join('',@lines);

	my $rec = {};

	my $end = '';
	my $key;
	my $hdr;
	while ($entry =~ /(<h3>(.*?)<\/h3>|<div class="n30">(.*?)<\/div>[[:space:]]*<div class="n70">(.*?)<\/div>|<div class="n100">(&nbsp;)<\/div>)/g) {
		#print "found '$1','$2','$3','$4'\n";
		if ($2) {
			print $end if $end;
			my $tit = $2;
			my $npa;
			if ($tit =~ /^NPA NXX ([0-9]+-[0-9]+)$/) {
				$tit = 'NPA NXX';
				$npa = $1;
			}
			print "'$tit'=>{\n";
			$end = "},\n";
			$key = $tit;
			$hdr = undef;
			if ($npa) {
				printf "\t'%s'=>'%s',\n", $tit, $npa;
				$rec->{$key}{$tit} = $npa;
			}
			$headings->{$tit}++;
		} elsif ($5) {
			$hdr = undef;
		} else {
			my $fld = $3;
			my $val = $4;
			my $sub;
			$fld =~ s/\&amp;/\&/g;
			$val =~ s/\&amp;/\&/g;
			if ($fld =~ /^CBSA ([0-9]+)$/) {
				$fld = 'CBSA';
				$sub = $1;
			} elsif ($fld =~ /^Part of CSA ([0-9]+)$/) {
				$fld = 'Part of CSA';
				$sub = $1;
			} elsif ($fld =~ /^Metro Division ([0-9]+)$/) {
				$fld = 'Metro Division';
				$sub = $1;
			} elsif ($fld =~ /^Rural-Urban Continuum Code ([0-9]+)$/) {
				$fld = 'Rural-Urban Continuum Code';
				$sub = $1;
#			} elsif ($fld =~ /^Population ([0-9]+)$/) {
#				$fld = 'Population';
#				$sub = $1;
			}
			if ($fld eq '&nbsp;') {
				if ($val =~ /<em>(.*?)<\/em>/) {
					#printf "\t# %s\n", $val;
					$hdr = $1;
					$subhead->{$hdr}++;
				} elsif ($val =~ /<a href/) {
					$fld = 'Google maps';
				} else {
					$fld = 'Remark';
					#printf "\t# %s\n", $val;
				}
			}
			unless ($fld eq '&nbsp;') {
				$val = "$sub $val" if $sub;
				printf "\t'%s'=>'%s',\n", $fld, $val;
				if ($hdr) {
					$rec->{$key}{$hdr}{$fld} = $val;
					$fields->{$key}{$hdr}{$fld}++; #{count}++;
					#$fields->{$key}{$hdr}{$fld}{values}{$val}++;
				} else {
					$rec->{$key}{$fld} = $val;
					$fields->{$key}{$fld}++; #{count}++;
					#$fields->{$key}{$fld}{values}{$val}++;
				}
			}
		}
	}
	my $npax = $rec->{'NPA NXX'}{'NPA NXX'};
	my ($npa,$nxx) = split(/-/,$npax);
	my $ocn = $rec->{'Carrier'}{'OCN'};
	my $lata = $rec->{'LATA'}{'LATA'};
	my $clli = $rec->{'Wire Center'}{'Switch'};
	my $wire = $rec->{'Wire Center'}{'Wire Center CLLI'};
	my $rate = $rec->{'Rate Center - Locality'}{'LERG Abbreviation'};
	my $state = $rec->{'Rate Center - Locality'}{'State'};
	if ($npax) {
		unless (exists $db->{nxx}{$npax}) {
			$rec->{'NPA NXX'}{'Carrier'} = $ocn if $ocn;
			$rec->{'NPA NXX'}{'LATA'} = $lata if $lata;
			$rec->{'NPA NXX'}{'Switch'} = $clli if $clli;
			$rec->{'NPA NXX'}{'Wire Center'} = $wire if $wire;
			$rec->{'NPA NXX'}{'Rate Center'} = $rate if $rate;
			$rec->{'NPA NXX'}{'RC State'} = $state if $state;
			$db->{nxx}{$npax} = $rec->{'NPA NXX'};
		}
		$db->{npa}{$npa}{nxxs}{$nxx} = 1;
	}
	if ($ocn) {
		$db->{ocn}{$ocn} = {} unless exists $db->{ocn}{$ocn};
		foreach ('Common Name', 'OCN', 'OCN Type', 'Name', 'Abbreviation', 'DBA', 'Address', 'City', 'State', 'Zip', 'Country') {
			if (exists $db->{ocn}{$ocn}{$_} and $db->{ocn}{$ocn}{$_} ne $rec->{Carrier}{$_}) {
				warn "$ocn $npa-$nxx $_ changed from $db->{ocn}{$ocn}{$_} to $rec->{Carrier}{$_}";
			}
			$db->{ocn}{$ocn}{$_} = $rec->{Carrier}{$_};
		}
		$db->{ocn}{$ocn}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
	if ($lata) {
		unless (exists $db->{lata}{$lata}) {
			$db->{lata}{$lata} = $rec->{'LATA'};
		}
		$db->{lata}{$lata}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
	if ($clli) {
		unless (exists $db->{clli}{$clli}) {
			$rec->{'Wire Center'}{'Rate Center'} = $rate if $rate;
			$rec->{'Wire Center'}{'RC State'} = $state if $state;
			$rec->{'Wire Center'}{'LATA'} = $lata if $lata;
			$rec->{'Wire Center'}{'OCN'} = $ocn if $ocn;
			$db->{clli}{$clli} = $rec->{'Wire Center'};
		}
		$db->{clli}{$clli}{'NPA NXXs'}{$npa}{$nxx} = 1;
		my $city = substr($clli,0,6);
		$db->{city}{$city}{'NPA NXXs'}{$npa}{$nxx} = 1;
		if ($wire) {
			unless (exists $db->{wire}{$wire}) {
				my $dat = {};
				foreach (
					'Wire Center CLLI',
					'Wire Center Name',
					'Address',
					'City',
					'County',
					'State',
					'Zip',
					'Country',
					'LATA'
				) {
					$dat->{$_} = $rec->{'Wire Center'}{$_};
				}
				foreach (
					'V&H',
					'Latitude/Longitude',
					'Google maps'
				) {
					$dat->{'Coordinates'}{$_} = $rec->{'Wire Center'}{'Coordinates'}{$_};
				}

				if (exists $rec->{'Wire Center'}{'Other switches in WC'}) {
					$dat->{'Switches in WC'} = join(' ',sort split(/ /,$rec->{'Wire Center'}{'Other switches in WC'}." $clli"));
				}
				$db->{wire}{$wire} = $dat;
			}
			$db->{wire}{$wire}{'NPA NXXs'}{$npa}{$nxx} = 1;
		}
	}
	if ($rate) {
		unless (exists $db->{rate}{$state}{$rate}) {
			$rec->{'Rate Center - Locality'}{'LATA'} = $lata if $lata;
			$db->{rate}{$state}{$rate} = $rec->{'Rate Center - Locality'};
		}
		$db->{rate}{$state}{$rate}{'NPA NXXs'}{$npa}{$nxx} = 1;
		$db->{state}{$state}{'NPA NXXs'}{$npa}{$nxx} = 1;
	}
}

#my $dumper = Data::Dumper->new([$db,$headings,$subhead,$fields]);
#print $dumper->Dump;

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
foreach my $key (sort keys %{$db->{ocn}}) {
	my $ocn = $db->{ocn}{$key};
	printf $of "%-4.4s", $ocn->{'OCN'};
	printf $of "%-50.50s",$ocn->{'Name'};
	printf $of "%-20.20s",$ocn->{'Abbreviation'};
	printf $of "%-2.2s", $ocn->{'State'};
	printf $of "%-10.10s",$ocn->{'OCN Type'};
	printf $of "%-4.4s", ''; # Overall OCN XXX
	printf $of "%-5.5s", ''; # filler
	printf $of "%-1.1s", ''; # Last Known Address indicator
	printf $of "%-20.20s", ''; # Last Name
	printf $of "%-10.10s", ''; # First Name
	printf $of "%-1.1s", ''; # Middle Initial
	printf $of "%-50.50s", $ocn->{'Common Name'}; # Company Name (contact specific)
	printf $of "%-30.30s", ''; # Title
	printf $of "%-30.30s", $ocn->{'Address'}; # Address1
	printf $of "%-30.30s", ''; # Address2
	printf $of "%-10.10s", ''; # Floor
	printf $of "%-20.20s", ''; # Room
	printf $of "%-20.20s", $ocn->{'City'}; # City
	printf $of "%-2.2s", ($ocn->{'Country'} eq 'US' or $ocn->{'Country'} eq 'CA') ? $ocn->{'State'} : $ocn->{'Country'}; # State/Prov/Ter/Cty
	my $zip = $ocn->{'Zip'}; $zip =~ s/-//g;
	printf $of "%-9.9s", $zip; # Zip/Postal Code
	printf $of "%-12.12s", ''; # Phone
	printf $of "%-10.10s", ''; # filler
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
foreach my $key (sort keys %{$db->{ocn}}) {
	my $ocn = $db->{ocn}{$key};
	printf $of "%-4.4s", $ocn->{'OCN'};
	printf $of "%-50.50s", $ocn->{'Common Name'};
	printf $of "%-2.2s", ($ocn->{'Country'} eq 'US' or $ocn->{'Country'} eq 'CA') ? $ocn->{'State'} : $ocn->{'Country'}; # State/Prov/Ter/Cty
	printf $of "%-20.20s", ''; # Contact Function
	printf $of "%-10.10s", ''; # Contact Phone
	printf $of "%-70.70s", ''; # Contact Information
	printf $of "%-16.16s", ''; # filler
	print $of "\n";
}
close($of);

open($fh,"<:utf8","$codedir/e164d.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9-]+\t/;
	my ($cc,$cty,$note) = split(/\t/,$_);
	my $pair = $cc;
	if ($cc =~ /^1-([0-9]+)$/) {
		$pair = $1;
		$cc = 1;
	} elsif ($cc =~ /-/) {
		next;
	}
	$cty = encode('latin1', $cty);
	#$cty = "\U$cty\E";
	$db->{cc}{$cc} = {cc=>$cc,pair=>$pair,cty=>$cty,note=>$note};
}
close($fh);

print STDERR "writing $datadir/LERG2.DAT\n";
open($of,">","$datadir/LERG2.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG2.DAT
# ---------
# Telcordia's LERGINFO.DOC says: "The LERG2 file is independent from other LERG
# files.  This file contains high-level Country Code information (e.g. Republic
# of Hungary = 36).  There is no City / Area Code information in this file.
# The file is provided for reference should there be a need for a resource for
# Country Codes."
# ---------
# A file containing this information (in a different format) can be obtained
# without fee from the TSB at http://www.itu.int/.
# ---------
# Country Codes (Key field 3)
# ---------
#   1-  1  1 Action Desc (E=effective,D=delete,M=modify)
#   2-  7  6 Action Date (mmddyy)
#   8- 10  3 Country Code
#  11- 11  1 filler
#  12- 14  3 Paired Code
#  15- 15  1 filler
#  16- 80 65 Country Name
#  81- 81  1 Footnote code (refer to LERGINFO, Country Codes)
#  82- 90  9 filler
# ---------
EOF
foreach my $key (sort keys %{$db->{cc}}) {
	my $code = $db->{cc}{$key};
	printf $of "%-1.1s", 'E';
	printf $of "%-6.6s", ''; # Action Date
	printf $of "%-3.3s", $code->{cc};
	printf $of "%-1.1s", ''; # filler
	printf $of "%03d", $code->{pair};
	printf $of "%-1.1s", ''; # filler
	printf $of "%-65.65s", $code->{cty};
	printf $of "%-1.1s", $code->{note};
	printf $of "%-9.9s", ''; # filler
	print $of "\n";
}
close($of);

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
	$db->{npa}{$npa}{npa}    = $npa;
	$db->{npa}{$npa}{cc}     = $cc;
	$db->{npa}{$npa}{st}     = $st;
	$db->{npa}{$npa}{state}  = $state;
	$db->{npa}{$npa}{tz}     = $tz;
	$db->{npa}{$npa}{dst}    = $dst;
	$db->{npa}{$npa}{nongeo} = $nongeo;
}
close($fh);

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
	my $tz = $npa->{tz};
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
	printf $of "%-1.1s", ''; # Time Zone 2
	printf $of "%-1.1s", ''; # Time Zone 3
	printf $of "%-1.1s", ''; # Time Zone 4
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

open($fh,"<","$codedir/t1.111.8a.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9]+\t/;
	my ($net,$own) = split(/\t/,$_);
	$db->{pc}{$net} = {net=>$net,own=>$own,clu=>{}};
}
close($fh);

open($fh,"<","$codedir/t1.111.8b.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9]+\t/;
	my ($net,$clu,$own) = split(/\t/,$_);
	$db->{pc}{$net}{clu}{$clu} = {net=>$net,clu=>$clu,own=>$own,mem=>{}};
}
close($fh);

open($fh,"<","$codedir/t1.111.8c.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9]+\t/;
	my ($clu,$cc,$st,$loc) = split(/\t/,$_);
	my $net = 5;
	$db->{pc}{$net}{clu}{$clu} = {net=>$net,clu=>$clu,cc=>$cc,st=>$st,loc=>$loc,blk=>{}};
}
close($fh);

open($fh,"<","$codedir/t1.111.8d.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9]+\t/;
	my ($clu,$blk,$own) = split(/\t/,$_);
	my $net = 5;
	$db->{pc}{$net}{clu}{$clu}{blk}{$blk} = {net=>$net,clu=>$clu,blk=>$blk,own=>$own};
}
close($fh);

print STDERR "writing $datadir/LERG4.DAT\n";
open($of,">","$datadir/LERG4.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG4.DAT
# ---------
# Telcordia's LERGINFO.DOC says: "The LERG4 file contains SS7 "Point Code"
# assignments at the network, cluster, or member levels of coding.  Network and
# cluster code assignments provide for a company to assign member level codes
# on its own.  Level of assignment is based on a combination of need/request as
# well as the specifics about a given company's network.  This file simply
# identifies the company to which the code has been assigned; it does not
# associate any network elements to any specific Point Codes.  Due to constant
# changes in personnel in a company, reorganizations, mergers, etc., some
# company names and contact information will not be up to date, but will be
# shown as they were at the time of assignment."
# ---------
# The intial data for this file came from ANSI T1.111.8/1992 and ANSI
# T1.111.8/2000 which listed these point code assignments.  ATIS and network
# operators have since let Telcordia (formerly Bellcore) hold their data
# ransom.  Additions, updates or corrections to this data can be submitted to
# the OpenSS7 Project using the ss7view tool.
# ---------
# SS7 Assignments (Key fields 1,2,3)
# ---------
#   1-  3  3 Network
#   4-  6  3 Cluster
#   7-  9  3 Member (from)
#  10- 12  3 Member (to) (applies to small networks only)
#  13- 62 50 Assignee Company
#  63- 98 36 Operations Center Contact
#  99-118 20 Phone
# 119-120  2 filler
# ---------
#-++-++-++-++------------------------------------------------++------------------+++
EOF
foreach my $key (reverse sort {$a<=>$b} keys %{$db->{pc}}) {
	my $net = $db->{pc}{$key};
	if (exists $net->{own}) {
		printf $of "%-3.3s", $net->{net};
		printf $of "%-3.3s", ''; # cluster
		printf $of "%-3.3s", ''; # member (from)
		printf $of "%-3.3s", ''; # member (to)
		printf $of "%-50.50s", $net->{own}; # assignee company
		printf $of "%-36.36s", ''; # op center contact
		printf $of "%-20.20s", ''; # phone
		printf $of "%-2.2s", ($net->{cc} eq 'US' or $net->{cc} eq 'CA') ? $net->{st} : $net->{cc}; # filler
		print $of "\n";
	}
	if ($net->{clu}) {
		foreach my $ke2 (sort {$a<=>$b} keys %{$net->{clu}}) {
			my $clu = $net->{clu}{$ke2};
			if (exists $clu->{own}) {
				printf $of "%-3.3s", $clu->{net};
				printf $of "%-3.3s", $clu->{clu}; # cluster
				printf $of "%-3.3s", 0; # member (from)
				printf $of "%-3.3s", 255; # member (to)
				printf $of "%-50.50s", $clu->{own}; # assignee company
				printf $of "%-36.36s", ''; # op center contact
				printf $of "%-20.20s", ''; # phone
				printf $of "%-2.2s", ($clu->{cc} eq 'US' or $clu->{cc} eq 'CA') ? $clu->{st} : $clu->{cc}; # filler
				print $of "\n";
			}
			if ($clu->{blk}) {
				foreach my $ke3 (sort {$a<=>$b} keys %{$clu->{blk}}) {
					my $blk = $clu->{blk}{$ke3};
					printf $of "%-3.3s", $blk->{net};
					printf $of "%-3.3s", $blk->{clu}; # cluster
					printf $of "%-3.3s", $blk->{blk}; # member (from)
					printf $of "%-3.3s", $blk->{blk}+3; # member (to)
					printf $of "%-50.50s", $blk->{own}; # assignee company
					printf $of "%-36.36s", ''; # op center contact
					printf $of "%-20.20s", ''; # phone
					printf $of "%-2.2s", ($clu->{cc} eq 'US' or $clu->{cc} eq 'CA') ? $clu->{st} : $clu->{cc}; # filler
					print $of "\n";
				}
			}
		}
	}
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
	printf $of "%-40.40s", $lata->{'Historical Region'};
	printf $of "%-5.5s", $lata->{'LATA'};
	printf $of "%-20.20s", $lata->{'Name'};
	my @areas = split(/ /,$lata->{'Area Codes in LATA'});
	for (my $i=0;$i<30;$i++) {
		if ($areas[$i]) {
			printf $of "%-3.3s", $areas[$i];
		} else {
			printf $of "%-3.3s", '';
		}
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
	my $nxx = $db->{nxx}{$key};
	my $ocn = $db->{ocn}{$nxx->{Carrier}};
	my $lata = $db->{lata}{$nxx->{LATA}};
	my $clli = $db->{clli}{$nxx->{Switch}};
	my $wire = $db->{wire}{$nxx->{'Wire Center'}};
	my $rate = $db->{rate}{$nxx->{'RC State'}}{$nxx->{'Rate Center'}};
	printf $of "%-5.5s", $lata->{LATA};
	printf $of "%-20.20s", $lata->{Name};
	printf $of "%-1.1s", ' '; # Status ( |E|M|D)
	printf $of "%-6.6s",
		substr($nxx->{'Effective Date'},5,2).
		substr($nxx->{'Effective Date'},8,2).
		substr($nxx->{'Effective Date'},2,2);
	printf $of "%-3.3s", substr($nxx->{'NPA NXX'},0,3);
	printf $of "%-3.3s", substr($nxx->{'NPA NXX'},4,3);
	printf $of "%-1.1s", 'A'; # Block ID
	printf $of "%-4.4s", ''; # filler
	my $coc = '???';
	my $ssc = 'N';
	my $dind = 'N';
	my $treo = 'NA';
	my $trat = 'NA';
	if ($nxx->{'NXX Type'} eq 'Plain Old Telephone Service (POTS)') {
		$coc = 'EOC'; $ssc = 'N'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Dedicated to Cellular') {
		$coc = 'PMC'; $ssc = 'C'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Cellular') {
		$coc = 'PMC'; $ssc = 'N'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Dedicated to Paging') {
		$coc = 'PMC'; $ssc = 'B'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Paging') {
		$coc = 'SP1'; $ssc = 'B'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Personal Communications Services (not NPA 500)') {
		$coc = 'SP1'; $ssc = 'N'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Feature Group B Access') {
		$coc = 'FGB'; $ssc = 'N'; $dind = 'Y'; $treo = 'NA'; $trat = 'NA';
	} elsif ($nxx->{'NXX Type'} eq 'Shared between POTS and Paging') {
		$coc = 'EOC'; $ssc = 'B'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Shared between POTS and Cellular') {
		$coc = 'EOC'; $ssc = 'C'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Shared between 3 or more - (POTS, Cellular, Paging, Mobile, or miscellaneous)') {
		$coc = 'EOC'; $ssc = 'BCRS'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Shared between POTS and PCS') {
		$coc = 'EOC'; $ssc = 'S'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Unavailable for Assignment') {
		$coc = 'UFA'; $ssc = ''; $dind = 'N'; $treo = 'NA'; $trat = 'NA';
	} elsif ($nxx->{'NXX Type'} eq 'Information Provider Services') {
		$coc = 'INP'; $ssc = ''; $dind = 'Y'; $treo = 'NA'; $trat = 'NA';
	} elsif ($nxx->{'NXX Type'} eq 'Standard Plant Test Code') {
		$coc = 'TST'; $ssc = ''; $dind = 'N';
	} elsif ($nxx->{'NXX Type'} eq 'Centralized Directory Assistance') {
		$coc = 'CDA'; $ssc = ''; $dind = 'Y'; $treo = 10; $trat = 10;
	} elsif ($nxx->{'NXX Type'} eq 'Shared between POTS and VoIP') {
		$coc = 'EOC'; $ssc = 'V'; $dind = 'Y'; $treo = 'NA'; $trat = 'NA';
	} elsif ($nxx->{'NXX Type'} eq '700 IntraLATA Presubscription') {
		$coc = '700'; $ssc = ''; $dind = 'Y'; $treo = 'NA'; $trat = 'NA';
	}
	printf $of "%-3.3s", $coc; # COC Type
	printf $of "%-4.4s", $ssc; # Special Service Code
	printf $of "%-1.1s", $dind; # Dialable Indicator
	printf $of "%-2.2s", $treo; # terminating digits - EO
	printf $of "%-2.2s", $trat; # terminating digits - AT
	printf $of "%-1.1s", $nxx->{'Portable Block'} eq 'Yes' ? 'Y' : 'N';
	printf $of "%-4.4s", 'NANP'; # adminstrative OCN
	printf $of "%-1.1s", ''; # filler
	printf $of "%-4.4s", $nxx->{'Carrier'};
	printf $of "%-10.10s", $wire->{'Wire Center CLLI'};
	printf $of "%-2.2s", ''; # County
	printf $of "%-2.2s", $wire->{'State'};
	printf $of "%-10.10s", $rate->{'LERG Abbreviation'};
	my $rcty = ' ';
	if ($rate->{'Zone Type'} eq 'Unrestricted') {
		$rcty = ' ';
	} elsif ($rate->{'Zone Type'} eq 'Suburban Zone') {
		$rcty = 'S';
	} elsif ($rate->{'Zone Type'} eq 'Zoned City') {
		$rcty = 'Z';
	} elsif ($rate->{'Zone Type'} eq 'Restricted') {
		$rcty = '+';
	}
	printf $of "%-1.1s", $rcty; # Rate Center Type
	printf $of "%-4.4s", substr($nxx->{'Line From'},8,4);
	printf $of "%-4.4s", substr($nxx->{'Line To'},8,4);
	printf $of "%-11.11s", $nxx->{'Switch'};
	printf $of "%-2.2s", '00'; # SHA (00 or 01-99)
	printf $of "%-4.4s", ''; # filler
	printf $of "%-4.4s", ''; # test line number
	printf $of "%-1.1s", ''; # response (A|M)
	printf $of "%-6.6s", ''; # expiration date
	printf $of "%-1.1s", $nxx->{'1,000 Block Pooling'} eq 'Yes' ? 'Y' : 'N';
	printf $of "%-5.5s", $rate->{'LATA'};
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s",
		substr($nxx->{'Date Activated'},5,2).
		substr($nxx->{'Date Activated'},8,2).
		substr($nxx->{'Date Activated'},2,2);
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # E STATUS date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s",
		substr($nxx->{'Last Modified'},5,2).
		substr($nxx->{'Last Modified'},8,2).
		substr($nxx->{'Last Modified'},2,2);
	printf $of "%-17.17s", ''; # filler
	print $of "\n";
}
close($of);

print STDERR "writing $datadir/LERG6ATC.DAT\n";
open($of,">","$datadir/LERG6ATC.DAT");
printnotice($of);
print $of <<'EOF';
# ------------
# LERG6ATC.DAT
# ------------
# The LERG6ATC file is an expansion of data for those records in the LERG6 file
# that have a COCTYPE value of ATC (Access Tandem Code).  This file expands the
# information by appending the Operator Service "service" codes for each ATC
# record.  Please note that the NPA NXX information for ATCs (besides the OS
# codes) is also included in the primary LERG6 file.  ATC records will not have
# numeric blocks associated with them.
# ------------
# NPA/ATC (Key fields 5,6,7)
# ------------
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
# 171-175  5 OS Code 1
# 176-180  5 OS Code 2
# ...        ...
# 291-295  5 OS Code 25
# ------------
EOF
foreach my $key (sort keys %{$db->{nxx}}) {
	my $nxx = $db->{nxx}{$key};
	my $ocn = $db->{ocn}{$nxx->{Carrier}};
	my $lata = $db->{lata}{$nxx->{LATA}};
	my $clli = $db->{clli}{$nxx->{Switch}};
	my $wire = $db->{wire}{$nxx->{'Wire Center'}};
	my $rate = $db->{rate}{$nxx->{'RC State'}}{$nxx->{'Rate Center'}};
	printf $of "%-5.5s", $lata->{LATA};
	printf $of "%-20.20s", $lata->{Name};
	printf $of "%-1.1s", ' '; # Status ( |E|M|D)
	printf $of "%-6.6s",
		substr($nxx->{'Effective Date'},5,2).
		substr($nxx->{'Effective Date'},8,2).
		substr($nxx->{'Effective Date'},2,2);
	printf $of "%-3.3s", substr($nxx->{'NPA NXX'},0,3);
	printf $of "%-3.3s", substr($nxx->{'NPA NXX'},4,3);
	printf $of "%-1.1s", 'A'; # Block ID
	printf $of "%-4.4s", ''; # filler
	my $coc = '???';
	my $ssc = 'N';
	my $dind = 'N';
	my $treo = 'NA';
	my $trat = 'NA';
	if ($nxx->{'NXX Type'} eq 'Plain Old Telephone Service (POTS)') {
		$coc = 'EOC'; $ssc = 'N'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Dedicated to Cellular') {
		$coc = 'PMC'; $ssc = 'C'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Cellular') {
		$coc = 'PMC'; $ssc = 'N'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Dedicated to Paging') {
		$coc = 'PMC'; $ssc = 'B'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Paging') {
		$coc = 'SP1'; $ssc = 'B'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Personal Communications Services (not NPA 500)') {
		$coc = 'SP1'; $ssc = 'N'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Feature Group B Access') {
		$coc = 'FGB'; $ssc = 'N'; $dind = 'Y'; $treo = 'NA'; $trat = 'NA';
	} elsif ($nxx->{'NXX Type'} eq 'Shared between POTS and Paging') {
		$coc = 'EOC'; $ssc = 'B'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Shared between POTS and Cellular') {
		$coc = 'EOC'; $ssc = 'C'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Shared between 3 or more - (POTS, Cellular, Paging, Mobile, or miscellaneous)') {
		$coc = 'EOC'; $ssc = 'BCRS'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Shared between POTS and PCS') {
		$coc = 'EOC'; $ssc = 'S'; $dind = 'Y';
	} elsif ($nxx->{'NXX Type'} eq 'Unavailable for Assignment') {
		$coc = 'UFA'; $ssc = ''; $dind = 'N'; $treo = 'NA'; $trat = 'NA';
	} elsif ($nxx->{'NXX Type'} eq 'Information Provider Services') {
		$coc = 'INP'; $ssc = ''; $dind = 'Y'; $treo = 'NA'; $trat = 'NA';
	} elsif ($nxx->{'NXX Type'} eq 'Standard Plant Test Code') {
		$coc = 'TST'; $ssc = ''; $dind = 'N';
	} elsif ($nxx->{'NXX Type'} eq 'Centralized Directory Assistance') {
		$coc = 'CDA'; $ssc = ''; $dind = 'Y'; $treo = 10; $trat = 10;
	} elsif ($nxx->{'NXX Type'} eq 'Shared between POTS and VoIP') {
		$coc = 'EOC'; $ssc = 'V'; $dind = 'Y'; $treo = 'NA'; $trat = 'NA';
	} elsif ($nxx->{'NXX Type'} eq '700 IntraLATA Presubscription') {
		$coc = '700'; $ssc = ''; $dind = 'Y'; $treo = 'NA'; $trat = 'NA';
	}
	printf $of "%-3.3s", $coc; # COC Type
	printf $of "%-4.4s", $ssc; # Special Service Code
	printf $of "%-1.1s", $dind; # Dialable Indicator
	printf $of "%-2.2s", $treo; # terminating digits - EO
	printf $of "%-2.2s", $trat; # terminating digits - AT
	printf $of "%-1.1s", $nxx->{'Portable Block'} eq 'Yes' ? 'Y' : 'N';
	printf $of "%-4.4s", 'NANP'; # adminstrative OCN
	printf $of "%-1.1s", ''; # filler
	printf $of "%-4.4s", $nxx->{'Carrier'};
	printf $of "%-10.10s", $wire->{'Wire Center CLLI'};
	printf $of "%-2.2s", ''; # County
	printf $of "%-2.2s", $wire->{'State'};
	printf $of "%-10.10s", $rate->{'LERG Abbreviation'};
	my $rcty = ' ';
	if ($rate->{'Zone Type'} eq 'Unrestricted') {
		$rcty = ' ';
	} elsif ($rate->{'Zone Type'} eq 'Suburban Zone') {
		$rcty = 'S';
	} elsif ($rate->{'Zone Type'} eq 'Zoned City') {
		$rcty = 'Z';
	} elsif ($rate->{'Zone Type'} eq 'Restricted') {
		$rcty = '+';
	}
	printf $of "%-1.1s", $rcty; # Rate Center Type
	printf $of "%-4.4s", substr($nxx->{'Line From'},8,4);
	printf $of "%-4.4s", substr($nxx->{'Line To'},8,4);
	printf $of "%-11.11s", $nxx->{'Switch'};
	printf $of "%-2.2s", '00'; # SHA (00 or 01-99)
	printf $of "%-4.4s", ''; # filler
	printf $of "%-4.4s", ''; # test line number
	printf $of "%-1.1s", ''; # response (A|M)
	printf $of "%-6.6s", ''; # expiration date
	printf $of "%-1.1s", $nxx->{'1,000 Block Pooling'} eq 'Yes' ? 'Y' : 'N';
	printf $of "%-5.5s", $rate->{'LATA'};
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s",
		substr($nxx->{'Date Activated'},5,2).
		substr($nxx->{'Date Activated'},8,2).
		substr($nxx->{'Date Activated'},2,2);
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # E STATUS date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s",
		substr($nxx->{'Last Modified'},5,2).
		substr($nxx->{'Last Modified'},8,2).
		substr($nxx->{'Last Modified'},2,2);
	printf $of "%-17.17s", ''; # filler
	for (my $i=0;$i<25;$i++) {
		printf $of "%-5.5s", ''; # OS Code $i+1
	}
	print $of "\n";
}
close($of);

print STDERR "writing $datadir/LERG7.DAT\n";
open($of,">","$datadir/LERG7.DAT");
printnotice($of);
print $of <<'EOF';
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
foreach my $key (sort keys %{$db->{clli}}) {
	my $clli = $db->{clli}{$key};
	my $lata = $db->{lata}{$clli->{'LATA'}} if $clli->{'LATA'};
	printf $of "%-5.5s", $lata->{'LATA'};
	printf $of "%-20.20s", $lata->{'Name'};
	printf $of "%-1.1s", ''; # Status
	printf $of "%-6.6s", ''; # Effective Date
	printf $of "%-11.11s", $clli->{'Switch'};
	printf $of "%-5.5s", $clli->{'Switch type'}; # Equipment type
	printf $of "%-4.4s", 'TRA'; # Administrative OCN
	printf $of "%-1.1s", ''; # filler
	printf $of "%-4.4s", $clli->{'OCN'}; # OCN
	my ($v,$h) = split(/,/,$clli->{'Coordinates'}{'V&H'});
	printf $of "%-5.5s", $v;
	printf $of "%-5.5s", $h;
	printf $of "%-1.1s", 'Y'; # IDDD
	printf $of "%-60.60s", $clli->{Address};
	printf $of "%-30.30s", $clli->{City};
	printf $of "%-2.2s", ($clli->{Country} eq 'US' or $clli->{Country} eq 'CA') ? $clli->{State} : $clli->{Country};
	my $zip = $clli->{'Zip'}; $zip =~ s/-//g;
	printf $of "%-9.9s", $zip;
	printf $of "%-1.1s", ''; # Point Code Flag
	printf $of "%-11.11s", $clli->{'Class 4/5 Switch'};
	printf $of "%-1.1s", ''; # End OFC
	printf $of "%-1.1s", ''; # Host
	printf $of "%-1.1s", ''; # Remote
	printf $of "%-1.1s", ''; # DA OFC
	printf $of "%-1.1s", ''; # CLASS 4/5
	printf $of "%-1.1s", ''; # WIRELESS OFC
	printf $of "%-1.1s", ''; # FG D ADJ EO
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # FG B TDM
	printf $of "%-1.1s", ''; # FG C TDM
	printf $of "%-1.1s", ''; # FG D TDM
	printf $of "%-1.1s", ''; # OS TDM
	printf $of "%-1.1s", ''; # INTERMED OFC
	printf $of "%-1.1s", ''; # DA TDM
	printf $of "%-1.1s", ''; # 911 TDM
	printf $of "%-1.1s", ''; # FG D ADJ TDM
	printf $of "%-1.1s", ''; # LOCAL TDM
	printf $of "%-1.1s", ''; # INTRA TDM
	printf $of "%-1.1s", ''; # CS DATA TDM
	printf $of "%-1.1s", ''; # BCR5
	printf $of "%-1.1s", ''; # BCR6
	printf $of "%-1.1s", ''; # PRI 64
	printf $of "%-1.1s", ''; # ISDN MULTIRT
	printf $of "%-1.1s", ''; # ISDN FS OFC
	printf $of "%-1.1s", ''; # X.75 GATEWAY
	printf $of "%-1.1s", ''; # PACKET X.121
	printf $of "%-1.1s", ''; # PACKET E.164
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # STP
	printf $of "%-1.1s", ''; # CCS AC OFC
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # 800 SSP
	printf $of "%-1.1s", ''; # LNP CAPABLE
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # CIP
	printf $of "%-1.1s", ''; # CSP
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # SW56
	printf $of "%-1.1s", ''; # FGD 56
	printf $of "%-1.1s", ''; # FGD 64
	printf $of "%-1.1s", ''; # INTRA PRSUB
	printf $of "%-1.1s", ''; # CALL AGENT
	printf $of "%-1.1s", ''; # TRUNK GATEWAY
	printf $of "%-1.1s", ''; # ACCESS GATEWAY
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # filler
	printf $of "%-36.36s", ''; # filler
	printf $of "%-6.6s", ''; # Creation Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # "E" STATUS Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # Last Modificaiton Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-1.1s", ''; # CLN Indicator
	printf $of "%-3.3s", ''; # Bill-to RAO
	printf $of "%-3.3s", ''; # Send-to RAO
	printf $of "%-12.12s", ''; # filler
	print $of "\n";
}
close($of);

print STDERR "writing $datadir/LERG7SHA.DAT\n";
open($of,">","$datadir/LERG7SHA.DAT");
printnotice($of);
print $of <<'EOF';
# ------------
# LERG7SHA.DAT
# ------------
# The LERG7SHA file contains information regarding the homing arrangements with
# a given switch.  Homing arrangements include such situations as mapping, to a
# switch, any appropriate Feature Group B, C and/or D tandems; STPs; in the
# case of remotes, their Hosts; in the case of Points of Interfaces(POIs),
# their ACTUAL SWITCH; etc.  This file expands upon the higher-level
# information about a switch that is provided in LERG7.
#
# Note that the SHA IND field is critical in assessing homing arrangements and
# their application against other LERG data, especially for NPA NXX's that
# route to/from a given switch.  A given switch may have more that one Feature
# Group B, C, or D tandem, etc., associated with it.  This is due to various
# factors.  If/when attempting to identify a specific tandem for a given NXX in
# LERG6, the switch and associated SHA IND in LERG6 must be tied in unison to
# the same combination in LERG7SHA to determine the appropriate homing.
# ------------
# Switch Homing data (Key field 5,6)
# ------------
#   1-  5  5 LATA
#   6- 25 20 LATA Name
#  26- 26  1 Status
#  27- 32  6 Effective Date (mmddyy)
#  33- 43 11 Switch
#  44- 45  2 Switch Homing Arrangement (SHA) Indicator
#  46- 56 11 Originating Feature Group B Tandem
#  57- 67 11 Originating Feature Group C Tandem
#  68- 78 11 Originating Feature Group D Tandem
#  79- 89 11 Originating Operator Services Tandem
#  90-100 11 Originating FG B Intermediate Tandem
# 101-111 11 Originating FG C Intermediate Tandem
# 112-122 11 Originating FG D Intermediate Tandem
# 123-133 11 Originating Local Tandem
# 134-144 11 Originating IntraLATA Tandem
# 145-155 11 Orig. Circuit Switched Data Tandem
# 156-166 11 filler
# 167-177 11 Terminating Feature Group B Tandem
# 178-188 11 Terminating Feature Group C Tandem
# 189-199 11 Terminating Feature Gorup D Tandem
# 200-210 11 Terminating Operator Services Tandem
# 211-221 11 Terminating FG B Intermediate Tandem
# 222-232 11 Terminating FG C Intermediate Tandem
# 233-243 11 Terminating FG D Intermediate Tandem
# 244-254 11 Terminating Local Tandem
# 255-265 11 Terminating IntraLATA Tandem
# 266-276 11 Term. Circuit Switched Data Tandem
# 277-287 11 filler
# 288-298 11 Host
# 299-309 11 STP1 (first of pair)
# 310-320 11 STP2 (second of pair)
# 321-331 11 filler
# 332-342 11 Originating 800 SSP
# 343-353 11 ISDN Foreign Served Office
# 354-364 11 Actual Switch Id
# 365-375 11 Call Agent
# 376-386 11 Trunk Gateway
# 387-397 11 filler
# 398-408 11 filler
# 409-412  4 Operating Company Number (OCN)
# 413-416  4 Administrative OCN (AOCN)
# 417-417  1 filler
# 418-423  6 Creation Date in BIRRDS (mmddyy)
# 424-424  1 filler
# 425-430  6 "E" STATUS date (mmddyy)
# 431-431  1 filler
# 432-437  6 Last Modification Date (mmddyy)
# 438-448 11 filler
# ------------
EOF
foreach my $key (sort keys %{$db->{clli}}) {
	my $clli = $db->{clli}{$key};
	my $lata = $db->{lata}{$clli->{LATA}} if $clli->{LATA};
	printf $of "%-5.5s", $lata->{'LATA'};
	printf $of "%-20.20s", $lata->{Name};
	printf $of "%-1.1s", ''; # Status
	printf $of "%-6.6s", ''; # Effective Date
	printf $of "%-11.11s", $clli->{Switch};
	printf $of "%-2.2s", ''; # Switch Homing Arrangement (SHA) Indicator
	printf $of "%-11.11s", $clli->{Tandems}->{'Feature Group B'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Feature Group C'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Feature Group D'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Operator Services'};
	printf $of "%-11.11s", $clli->{Tandems}->{'FG B Intermediate'};
	printf $of "%-11.11s", $clli->{Tandems}->{'FG C Intermediate'};
	printf $of "%-11.11s", $clli->{Tandems}->{'FG D Intermediate'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Local'};
	printf $of "%-11.11s", $clli->{Tandems}->{'InterLATA'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Circuit Switched Data'};
	printf $of "%-11.11s", ''; # filler
	printf $of "%-11.11s", $clli->{Tandems}->{'Feature Group B'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Feature Group C'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Feature Group D'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Operator Services'};
	printf $of "%-11.11s", $clli->{Tandems}->{'FG B Intermediate'};
	printf $of "%-11.11s", $clli->{Tandems}->{'FG C Intermediate'};
	printf $of "%-11.11s", $clli->{Tandems}->{'FG D Intermediate'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Local'};
	printf $of "%-11.11s", $clli->{Tandems}->{'InterLATA'};
	printf $of "%-11.11s", $clli->{Tandems}->{'Circuit Switched Data'};
	printf $of "%-11.11s", ''; # filler
	printf $of "%-11.11s", $clli->{Host};
	printf $of "%-11.11s", $clli->{'SS7 STP 1'};
	printf $of "%-11.11s", $clli->{'SS7 STP 2'};
	printf $of "%-11.11s", ''; # filler
	printf $of "%-11.11s", $clli->{'800 SSP'};
	printf $of "%-11.11s", $clli->{'ISDN Foreign Served Office'};
	printf $of "%-11.11s", $clli->{'Actual Switch'};
	printf $of "%-11.11s", $clli->{'Call Agent'};
	printf $of "%-11.11s", $clli->{'Trunk Gateway'};
	printf $of "%-11.11s", ''; # filler
	printf $of "%-11.11s", ''; # filler
	printf $of "%-4.4s", $clli->{'OCN'};
	printf $of "%-4.4s", 'TRA';
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # Creation Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # "E" STATUS Date
	printf $of "%-1.1s", ''; # filler
	printf $of "%-6.6s", ''; # Last Modificaiton Date
	printf $of "%-11.11s", ''; # filler
	print $of "\n";
}
close($of);

print STDERR "writing $datadir/LERG8.DAT\n";
open($of,">","$datadir/LERG8.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG8.DAT
# ---------
# The LERG8 file represents Rate Centers (also known as "Exchange Areas," "Rate
# Exchange Areas," and similar terms).  These are geographical areas defined
# historically and ongoing based on vairous factors that can vary by
# state/province.  A Rate Center may be a section of a large city, a specific
# town, or an area that may encompass multiple towns.  Rate Center boundaries
# are within state/province boundaries; however, correlation with other
# features such as geographical limitations (e.g. rivers), town boundaries,
# county boundaries, etc., is not a requirement (these factors may vary by
# state).  In general, Rate Centers historically define an area in which
# wireline (or wireline to cellular) calls that have originated and terminated
# within its boundaries (based on NXXs deifned to the Rate Center) are
# considered local (non-toll) calls.  However, note that local calls can also
# extend to other (e.g. adjoining) Rate Centers (information regarding this
# extension of local calling area is not provided in the LERG).
#
# Rate Centers are generally defined in tariffs field by principal wireline
# carriers in the area and are observed by companies that do not need to file
# tariffs, due to the potential issues surrounding billing that may otherwise
# occur.  Rate Centers also have associated V&H Coordinates, derived from
# Latitude and Longitude, which are then used to compute distances between Rate
# Centers.  Such distances are a component in determining charges for a call
# for rate plans that are based on distances.
# ---------
# Rate Center data (Key field 6,7,5)
# ---------
#   1-  5  5 LATA
#   6- 25 20 LATA Name
#  26- 26  1 Status
#  27- 32  6 Effective Date (mmddyy)
#  33- 34  2 RC State/Province/Territory/Country
#  35- 44 10 RC Center (RC) Name Abbreviation
#  45- 45  1 RC Type
#  46- 95 50 RC Full Name
#  96-100  5 Major Vertical Coordinate
# 101-105  5 Major Horizontal Coordinate
# 106-110  5 Minor Vertical Coordinate
# 111-115  5 Minor Horizontal Coordinate
# 116-118  3 NPA 1 (Area Code)
# 119-121  3 NPA 2
# ..         ...
# 143-145  3 NPA 10
# 146-147  2 Major Trading Area (MTA) - 1
# 148-149  2 Major Trading Area (MTA) - 2
# 150-150  1 Split Indicator (Y/N) (If RC is plit across NPAs)
# 151-153  3 Embedded Overlay NPA 1
# 154-156  3 Embedded Overlay NPA 2
# 157-159  3 Embedded Overlay NPA 3
# 160-162  3 Embedded Overlay NPA 4
# 163-172 10 filler
# ---------
EOF
foreach my $state (sort keys %{$db->{rate}}) {
	foreach my $key (sort keys %{$db->{rate}{$state}}) {
		my $rate = $db->{rate}{$state}{$key};
		my $lata = $db->{lata}{$rate->{LATA}} if $rate->{LATA};
		printf $of "%-5.5s", $lata->{LATA};
		printf $of "%-20.20s", $lata->{Name};
		printf $of "%-1.1s", ''; # Status
		printf $of "%-6.6s", ''; # Effective Date
		printf $of "%-2.2s", ($rate->{Country} eq 'US' or $rate->{Country} eq 'CA') ? $rate->{State} : $rate->{Country};
		printf $of "%-10.10s", $rate->{'LERG Abbreviation'};
		my $rcty = ' ';
		if ($rate->{'Zone Type'} eq 'Unrestricted') {
			$rcty = ' ';
		} elsif ($rate->{'Zone Type'} eq 'Suburban Zone') {
			$rcty = 'S';
		} elsif ($rate->{'Zone Type'} eq 'Zoned City') {
			$rcty = 'Z';
		} elsif ($rate->{'Zone Type'} eq 'Restricted') {
			$rcty = '+';
		}
		printf $of "%-1.1s", $rcty;
		printf $of "%-50.50s", $rate->{'Rate Center Name'};
		my ($v,$h);
		($v,$h) = split(/,/,$rate->{'Coordinates'}{'Major V&H'});
		printf $of "%-5.5s", $v;
		printf $of "%-5.5s", $h;
		($v,$h) = split(/,/,$rate->{'Coordinates'}{'Minor V&H'});
		printf $of "%-5.5s", $v;
		printf $of "%-5.5s", $h;
		my @areas = split(/ /,$rate->{'Area Codes in Rate Center'});
		for (my $i=0;$i<10;$i++) {
			printf $of "%-3.3s", $areas[$i];
		}
		my @mtas = ($rate->{'Market Area'}{'Major Trading Area'});
		for (my $i=0;$i<2;$i++) {
			printf $of "%-2.2s", substr($mtas[$i],4,3);
		}
		printf $of "%-1.1s", ''; # Split indicator (Y|N)
		my @ovlys = split(/ /,$rate->{'Embedded Overlays'});
		for (my $i=0;$i<4;$i++) {
			printf $of "%-3.3s", $ovlys[$i];
		}
		printf $of "%-10.10s", ''; # filler
		print $of "\n";
	}
}
close($of);

print STDERR "writing $datadir/LERG8LOC.DAT\n";
open($of,">","$datadir/LERG8LOC.DAT");
printnotice($of);
print $of <<'EOF';
# ------------
# LERG8LOC.DAT
# ------------
# The LERG8LOC file identifies localities associated with a given Rate Center
# (LERG8).  Often, several localities (e.g. towns) are included in a Rate
# Center.  This information can be useful in cases where a town is known and
# its Rate Center needs to be identified.  Note that the localities identified
# for a given Rate Center are not intendede to be a complete listing of each
# and every defined jurisdiction, alternative name, jurisdictional subsection,
# etc., that exists within the Rate Center; however, the majority of major
# localities should be identified.  County information for most Locality/Rate
# Centers within the United States (where applicable and when not a locality
# covering multiple counties) is also provided.
# ------------
# Localities per Rate Center (Key fields 5,6,7)
# ------------
#   1-  5  5 LATA
#   6- 25 20 LATA Name
#  26- 26  1 Status
#  27- 32  6 Effective Date (mmddyy)
#  33- 42 10 Locality Name Abbreviation
#  43- 44  2 Locality Country
#  45- 46  2 Locality State/Province/Territory/Country
#  47- 96 50 Locality Full Name
#  97-104  8 filler
# 105-114 10 Rate Center (RC) Name Abbreviation
# 115-115  1 RC Type
# 116-125 10 filler
# ------------
EOF
foreach my $state (sort keys %{$db->{rate}}) {
	foreach my $key (sort keys %{$db->{rate}{$state}}) {
		my $rate = $db->{rate}{$state}{$key};
		my $lata = $db->{lata}{$rate->{LATA}} if $rate->{LATA};
		printf $of "%-5.5s", $lata->{LATA};
		printf $of "%-20.20s", $lata->{Name};
		printf $of "%-1.1s", ''; # Status
		printf $of "%-6.6s", ''; # Effective Date
		printf $of "%-10.10s", $rate->{'LERG Abbreviation'};
		printf $of "%-2.2s", $rate->{Country};
		printf $of "%-2.2s", $rate->{State};
		printf $of "%-50.50s", $rate->{'Rate Center Name'};
		printf $of "%-8.8s", ''; # filler
		printf $of "%-10.10s", $rate->{'LERG Abbreviation'};
		printf $of "%-1.1s", ''; # type
		printf $of "%-10.10s", ''; # filler
		print $of "\n";
	}
}
close($of);

print STDERR "writing $datadir/LERG9.DAT\n";
open($of,">","$datadir/LERG9.DAT");
printnotice($of);
print $of <<'EOF';
# ---------
# LERG9.DAT
# ---------
# The LERG9 file is a "processed" file that is a processing of data provided in
# LERG6, LERG7 and LERG7SHA and does not contain any information beyond what is
# in those files.  LERG9 essentially assesses the homing of switches provided
# in LERG7SHA and summarizes that information by homed-to switch (e.g. by STP,
# Tandem, etc.).  In addition, associated NPA NXX information (based on "A"
# BLOCK records only) is drawn from LERG6 data.  Some additional LERG6 field
# values and some from LERG7 are also included.
#
# Homing arrangements provided in LERG9 pertain to the "A" BLOCK records in
# LERG6, not the numeric blocks.  Numeric blocks require routing portability to
# exist.  Such routing is based on the Location Routing Number (LRN) as may be
# associated with each individually ported telephone number that resides in the
# Number Portablity Administration Center's (NPAC's) Service Management System
# (SMS) database.  LRNs can essentially be viewed as telephone numbers on which
# call setup and routing occur, thus LERG9 can be used to "route" LRNs, as if
# the LRN were an NPA NXX.
#
# This file provides a "top-down" view of homing arrangements.  For example, it
# provides the NPA NXX and switches that may subtend a given Feature Group D
# Access Tandem, a Local tandem, a Host, etc.  This information is also
# discernable from LERG7SHA.
#
# LERG9 has historically been the largest file in the LERG, primarily doe to
# all the various combinations of data that are possible.  For exmample, all
# NPA NXXs for a switch may be replicated under multiple homed-to switches
# (e.g. Operator Services, Feature Groups B, C, D, etc.) depending on how many
# switches a given switch may "home to" in LERG7SHA.
#
# Since that data are, in essence, provided in other LERG files, users should
# consider whether then should use LERG9 or LERG6, LERG7, and/or LERG7SHA.
# ---------
# Homing Arrangements (Key field 3)
# ---------
#   1-  5  5 LATA
#   6- 25 10 LATA Name
#            Tandem (Homed-to-switch) data:
#  26- 36 11   Tandem (Homed-to-switch) Switch
#  37- 41  5   Tandem (Homed-to-switch) LATA
#  42- 46  5   Tandem (Homed-to-switch) Equipment Type
#  47- 47  1   Status
#  48- 53  6   Effective Date (mmddyy)
#            Homing Switch data:
#  54- 56  3   Homing Switch COC Type
#  57- 57  1   Change Source
#  58- 58  1   Status
#  59- 64  6   Effective Date (mmddyy)
#  65- 67  3   NPA (Area Code)
#  68- 70  3   NXX (Central Office Code)
#  71- 81 11   Switch
#  82- 86  5   Equipment Type
#  87- 94  8   Function(s) Originating
#  95-102  8   Function(s) Terminating
# 103-109  7 View ID
# 110-113  4 View Line #
# 114-117  4 Total Lines/View
# 118-118  1 Record Type (A=Tandem,B=ATC,C=Homing Switch)
# 119-119  1 filler
# 120-121  2 Homing Switch SHA Indicator
# 122-135 14 filler
# ---------
EOF
close($of);

print STDERR "writing $datadir/LERG9ATC.DAT\n";
open($of,">","$datadir/LERG9ATC.DAT");
printnotice($of);
print $of <<'EOF';
# ------------
# LERG9ATC.DAT
# ------------
# Homing Arrangements (NPA/ATC) (Key field 3)
# ------------
#   1-  5  5 LATA
#   6- 25 10 LATA Name
#            Tandem (Homed-to-switch) data:
#  26- 36 11   Tandem (Homed-to-switch) Switch
#  37- 41  5   Tandem (Homed-to-switch) LATA
#  42- 46  5   Tandem (Homed-to-switch) Equipment Type
#  47- 47  1   Status
#  48- 53  6   Effective Date (mmddyy)
#            Homing Switch data:
#  54- 56  3   Homing Switch COC Type (i.e. ATC)
#  57- 57  1   Change Source
#  58- 58  1   Status
#  59- 64  6   Effective Date (mmddyy)
#  65- 67  3   NPA (Area Code)
#  68- 70  3   NXX (Central Office Code)
#  71- 81 11   Switch
#  82- 86  5   Equipment Type
#  87- 94  8   Function(s) Originating
#  95-102  8   Function(s) Terminating
# 103-109  7 View ID
# 110-113  4 View Line #
# 114-117  4 Total Lines/View
# 118-118  1 Record Type (A=Tandem,B=ATC,C=Homing Switch)
# 119-119  1 filler
# 120-121  2 Homing Switch SHA Indicator
# 122-135 14 filler
# 136-140  5 OS Code 1
# 141-145  5 OS Code 2
# ...        ...
# 256-260  6 OS Code 25
# ---------
EOF
close($of);

print STDERR "writing $datadir/LERG10.DAT\n";
open($of,">","$datadir/LERG10.DAT");
printnotice($of);
print $of <<'EOF';
# ----------
# LERG10.DAT
# ----------
# This file provides information derived from manipulation of data in other
# LERG files.  Its primary purpose is in support of Operator-to-Operator
# interactions.  The file is specifically about Access Tandem Codes (ATCs)
# interrelationships.  LERG10 provides a mapping of NPA NXXs to ATCs.
# ----------
# Operator Service Codes (by NPA NXX) (Key field 3,4)
# ----------
#   1-  1  1 Status
#   2-  7  6 Effective Date (mmddyy)
#   8- 10  3 NPA (Area Code)
#  11- 13  3 NXX
#            Locality data:
#  14- 63 50   Locality Name
#  64- 65  2   Locality State/Province/Territory/Country
#  66- 67  2   Locality County
#  68- 70  3 NPA of Access Tandem Code (ATC)
#  71- 73  3 Access Tandem Code (ATC)
#  74- 78  5 OS Code 1
#  79- 83  5 OS Code 2
#  ...       ...
# 194-198  5 OS Code 25
# 199-203  5 LATA
# ----------
EOF
close($of);

print STDERR "writing $datadir/LERG11.DAT\n";
open($of,">","$datadir/LERG11.DAT");
printnotice($of);
print $of <<'EOF';
# ----------
# LERG11.DAT
# ----------
# This file provides information derived form manipulation of data in other
# LERG files.  Its primary purpose is in support of Operator-to-Operator
# interactions.  The file is specifically about Acess Tandem Codes (ATCs)
# interrelationships.  LERG11 provides a mapping of localities to ATCs.
# ----------
# Operator Services Codes (by Locality) (Key field 4,6,5)
# ----------
#   1-  1  1 Status
#   2-  7  6 Effective Date (mmddyy)
#   8- 13  6 filler
#            Locality data:
#  14- 63 50   Locality Name
#  64- 65  2   Locality State/Province/Territory/Country
#  66- 67  2   Locality County
#  68- 70  3 NPA of Access Tandem Code (ATC)
#  71- 73  3 Access Tandem Code (ATC)
#  74- 78  5 OS Code 1
#  79- 83  5 OS Code 2
#  ...       ...
# 194-198  5 OS Code 25
# 199-203  5 LATA
# ----------
EOF
close($of);

print STDERR "writing $datadir/LERG12.DAT\n";
open($of,">","$datadir/LERG12.DAT");
printnotice($of);
print $of <<'EOF';
# ----------
# LERG12.DAT
# ----------
# The LERG12 file is a means by which companies report the Location Routing
# Numbers (LRNs) that are or will be used in relationship with numbers they
# intended to route via Local Number Portability routing concepts.  This file
# identifies the company that has established the LRN as well as the switch to
# which that LRN should be associated.
# ----------
# Location Routing Numbers (LRNs) (Key field 5)
# ----------
#   1-  5  5 LATA (served by LRN)
#   6- 25 20 LATA Name
#  26- 26  1 Status
#  27- 32  6 Effective Date (mmddyy)
#  33- 42 10 Location Routing Number (LRN) (NPA NXX xxxx)
#  43- 72 30 filler
#  73- 73  1 LRN Type (P|M)
#  74- 84 11 Switch
#  85- 88  4 Administrative OCN (AOCN)
#  89- 89  1 filler
#  90- 93  4 Operating Company Name (OCN) (of NPA NXX)
#            Rate Center (RC) data:
#  94-103 10   RC Name Abbreviation
# 104-104  1   RC Type
# 105-106  2   RC State/Province/Territory/Country
# 107-107  1 filler
# 108-109  2 Switch SHA Indicator
# 110-136 27 filler
# ----------
EOF
close($of);

print STDERR "writing $datadir/LERG14.DAT\n";
open($of,">","$datadir/LERG14.DAT");
printnotice($of);
print $of <<'EOF';
# ----------
# LERG14.DAT
# ----------
# This file provides data provides an opportunity to identify the tandem to
# which specific NPA NXX BLOCKs can be routed.  This would apply to cases where
# the tandem provider is not the NPA NXX BLOCK assignee.  In a similar manner,
# this file can be used by those who choose to enter data as a means to
# identify where specific "Oddball" NXXs that may be associate with an OCN of
# MULT (multiple companies) should be routed if others need to route such calls
# to them -- for example, (NPA) 555-1212 calls (which are handled differently
# by each carrier in a given area).
# ----------
# Diverse Toll Routing (Key field 1)
# ----------
#   1- 11 11 FG D/ OS Tandem
#  12- 14  3 NPA
#  15- 17  3 NXX
#  18- 23  6 Effective Date (ddmmyy)
#  24- 24  1 Status
#  25- 28  4 Lines from #
#  29- 32  4 Lines to #
#  33- 48 16 filler
# ----------
EOF
close($of);

print STDERR "writing $datadir/LERG15.DAT\n";
open($of,">","$datadir/LERG15.DAT");
printnotice($of);
print $of <<'EOF';
# ----------
# LERG15.DAT
# ----------
# The LERG15 file provides, for those companies that have supplied data, a
# relationship between 911 Tandems and the counties (or county equivalents such
# as parishes in Louisiana) that the Tandems serve.  The Tandem may not
# necessarily serve the entire county; however, at least some area within the
# county is served.
# ----------
# 911 Tandem / County (Key field 5)
# ----------
#   1-  2  2 State/Province/Territory/Country
#   3- 22 20 County Name
#  23- 23  1 Status
#  24- 29  6 Effective Date (ddmmyy)
#  30- 40 11 911 Tandem
#  41- 44  4 Administrative OCN (AOCN)
#  45- 48  4 Operating Company Name (OCN)
#  49- 58 10 filler
# ----------
EOF
close($of);

my $npadata = {};
my $nxxdata = {};

open($of,">","$datadir/lata.txt");
foreach my $lata (sort keys %{$db->{lata}}) {
	printf $of "%s\t%s\t%s\t%s\n", $lata, $db->{lata}{$lata}{'Name'},
		$db->{lata}{$lata}{'State'},
		join(',',sort keys %{$db->{lata}{$lata}{'NPA NXXs'}})
		;
}
close($of);

open($of,">","$datadir/npanxx.txt");
open($o2,">","$datadir/npa.txt");
foreach my $npa (sort keys %{$db->{npa}}) {
	foreach my $nxx (sort keys %{$db->{npa}{$npa}{nxxs}}) {
		printf $of "%s\t%s\n", $npa, $nxx;
		$nxxdata->{$npa}{$nxx}{npanxx} = "$npa-$nxx";
	}
	printf $o2 "%s\n", $npa;
	$npadata->{$npa}{npa} = $npa;
}
close($o2);
close($of);

open($of,">","$datadir/latanpanxx.txt");
open($o2,">","$datadir/latanpa.txt");
foreach my $lata (sort keys %{$db->{lata}}) {
	foreach my $npa (sort keys %{$db->{lata}{$lata}{'NPA NXXs'}}) {
		foreach my $nxx (sort keys %{$db->{lata}{$lata}{'NPA NXXs'}{$npa}}) {
			printf $of "%s\t%s\t%s\n", $lata, $npa, $nxx;
			$nxxdata->{$npa}{$nxx}{lata} = $lata;
		}
		printf $o2 "%s\t%s\n", $lata, $npa;
		push @{$npadata->{$npa}{lata}}, $lata;
	}
}
close($o2);
close($of);

open($of,">","$datadir/cllinpanxx.txt");
open($o2,">","$datadir/cllinpa.txt");
foreach my $clli (sort keys %{$db->{clli}}) {
	foreach my $npa (sort keys %{$db->{clli}{$clli}{'NPA NXXs'}}) {
		foreach my $nxx (sort keys %{$db->{clli}{$clli}{'NPA NXXs'}{$npa}}) {
			printf $of "%s\t%s\t%s\n", $clli, $npa, $nxx;
			$nxxdata->{$npa}{$nxx}{clli} = $clli;
		}
		printf $o2 "%s\t%s\n", $clli, $npa;
		push @{$npadata->{$npa}{clli}}, $clli;
	}
}
close($o2);
close($of);

open($of,">","$datadir/wirenpanxx.txt");
open($o2,">","$datadir/wirenpa.txt");
foreach my $wire (sort keys %{$db->{wire}}) {
	foreach my $npa (sort keys %{$db->{wire}{$wire}{'NPA NXXs'}}) {
		foreach my $nxx (sort keys %{$db->{wire}{$wire}{'NPA NXXs'}{$npa}}) {
			printf $of "%s\t%s\t%s\n", $wire, $npa, $nxx;
			$nxxdata->{$npa}{$nxx}{wire} = $wire;
		}
		printf $o2 "%s\t%s\n", $wire, $npa;
		push @{$npadata->{$npa}{wire}}, $wire;
	}
}
close($o2);
close($of);

open($of,">","$datadir/citynpanxx.txt");
open($o2,">","$datadir/citynpa.txt");
foreach my $city (sort keys %{$db->{city}}) {
	foreach my $npa (sort keys %{$db->{city}{$city}{'NPA NXXs'}}) {
		foreach my $nxx (sort keys %{$db->{city}{$city}{'NPA NXXs'}{$npa}}) {
			printf $of "%s\t%s\t%s\n", $city, $npa, $nxx;
			$nxxdata->{$npa}{$nxx}{city} = $city;
		}
		printf $o2 "%s\t%s\n", $city, $npa;
		push @{$npadata->{$npa}{city}}, $city;
	}
}
close($o2);
close($of);

open($of,">","$datadir/statenpanxx.txt");
open($o2,">","$datadir/statenpa.txt");
foreach my $state (sort keys %{$db->{state}}) {
	foreach my $npa (sort keys %{$db->{state}{$state}{'NPA NXXs'}}) {
		foreach my $nxx (sort keys %{$db->{state}{$state}{'NPA NXXs'}{$npa}}) {
			printf $of "%s\t%s\t%s\n", $state, $npa, $nxx;
			$nxxdata->{$npa}{$nxx}{state} = $state;
		}
		printf $o2 "%s\t%s\n", $state, $npa;
		push @{$npadata->{$npa}{state}}, $state;
	}
}
close($o2);
close($of);

open($of,">","$datadir/ratenpanxx.txt");
open($o2,">","$datadir/ratenpa.txt");
foreach my $state (sort keys %{$db->{rate}}) {
	foreach my $rate (sort keys %{$db->{rate}{$state}}) {
		foreach my $npa (sort keys %{$db->{rate}{$state}{$rate}{'NPA NXXs'}}) {
			foreach my $nxx (sort keys %{$db->{rate}{$state}{$rate}{'NPA NXXs'}{$npa}}) {
				printf $of "%s\t%s\t%s\t%s\n", $state, $rate, $npa, $nxx;
				$nxxdata->{$npa}{$nxx}{state} = $state;
				$nxxdata->{$npa}{$nxx}{rate} = $rate;
			}
			printf $o2 "%s\t%s\t%s\n", $state, $rate, $npa;
			push @{$npadata->{$npa}{state}}, $state;
			push @{$npadata->{$npa}{rate}}, $rate;
		}
	}
}
close($o2);
close($of);

open($of,">","$datadir/ocnnpanxx.txt");
open($o2,">","$datadir/ocnnpa.txt");
foreach my $ocn (sort keys %{$db->{ocn}}) {
	foreach my $npa (sort keys %{$db->{ocn}{$ocn}{'NPA NXXs'}}) {
		foreach my $nxx (sort keys %{$db->{ocn}{$ocn}{'NPA NXXs'}{$npa}}) {
			printf $of "%s\t%s\t%s\n", $ocn, $npa, $nxx;
			$nxxdata->{$npa}{$nxx}{ocn} = $ocn;
		}
		printf $o2 "%s\t%s\n", $ocn, $npa;
		push @{$npadata->{$npa}{ocn}}, $ocn;
	}
}
close($o2);
close($of);

open($of,">","$datadir/npanxxlata.txt");
foreach my $npa (sort keys %{$nxxdata}) {
	foreach my $nxx (sort keys %{$nxxdata->{$npa}}) {
		printf $of "%s\t%s\t%s\n", $npa, $nxx, $nxxdata->{$npa}{$nxx}{lata}
		if exists $nxxdata->{$npa}{$nxx}{lata};
	}
}
close($of);

open($of,">","$datadir/npanxxclli.txt");
foreach my $npa (sort keys %{$nxxdata}) {
	foreach my $nxx (sort keys %{$nxxdata->{$npa}}) {
		printf $of "%s\t%s\t%s\n", $npa, $nxx, $nxxdata->{$npa}{$nxx}{clli}
		if exists $nxxdata->{$npa}{$nxx}{clli};
	}
}
close($of);

open($of,">","$datadir/npanxxwire.txt");
foreach my $npa (sort keys %{$nxxdata}) {
	foreach my $nxx (sort keys %{$nxxdata->{$npa}}) {
		printf $of "%s\t%s\t%s\n", $npa, $nxx, $nxxdata->{$npa}{$nxx}{wire}
		if exists $nxxdata->{$npa}{$nxx}{wire};
	}
}
close($of);

open($of,">","$datadir/npanxxcity.txt");
foreach my $npa (sort keys %{$nxxdata}) {
	foreach my $nxx (sort keys %{$nxxdata->{$npa}}) {
		printf $of "%s\t%s\t%s\n", $npa, $nxx, $nxxdata->{$npa}{$nxx}{city}
		if exists $nxxdata->{$npa}{$nxx}{city};
	}
}
close($of);

open($of,">","$datadir/npanxxstate.txt");
foreach my $npa (sort keys %{$nxxdata}) {
	foreach my $nxx (sort keys %{$nxxdata->{$npa}}) {
		printf $of "%s\t%s\t%s\n", $npa, $nxx, $nxxdata->{$npa}{$nxx}{state}
		if exists $nxxdata->{$npa}{$nxx}{state};
	}
}
close($of);

open($of,">","$datadir/npanxxrate.txt");
foreach my $npa (sort keys %{$nxxdata}) {
	foreach my $nxx (sort keys %{$nxxdata->{$npa}}) {
		printf $of "%s\t%s\t%s\n", $npa, $nxx, $nxxdata->{$npa}{$nxx}{rate}
		if exists $nxxdata->{$npa}{$nxx}{rate};
	}
}
close($of);

open($of,">","$datadir/npanxxocn.txt");
foreach my $npa (sort keys %{$nxxdata}) {
	foreach my $nxx (sort keys %{$nxxdata->{$npa}}) {
		printf $of "%s\t%s\t%s\n", $npa, $nxx, $nxxdata->{$npa}{$nxx}{ocn}
		if exists $nxxdata->{$npa}{$nxx}{ocn};
	}
}
close($of);

#open($of,">","$datadir/dump.pl");
#my $dumper = Data::Dumper->new([$nxxdata]);
#print $of $dumper->Dump;
#close($of);

my $dumper = Data::Dumper->new([$headings,$subhead,$fields]);
print $dumper->Dump;
