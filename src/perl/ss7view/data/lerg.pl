#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;

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
}

sub printlerg1 {
	my $fn;
	$fn = "LERG1.DAT";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
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
	my @keys = (
		'Operating Company Number (OCN)',
		'Operating Company Name',
		'Abbreviated OCN Name',
		'State/Province/Territory/Country of OCN',
		'Category',
		'Overall OCN',
		'filler',
		'Last Known Address Indicator',
		'Last Name',
		'First Name',
		'Middle Initial',
		'Company Name',
		'Title',
		'Address 1',
		'Address 2',
		'Floor',
		'Room',
		'City',
		'State/Province/Territory/Country',
		'Zip/Postal Code',
		'Phone',
		'filler',
	);
	my %merges = (
		'Operating Company Name'=>[
			'areacodes:COMPANY',
			'localcallingguide:COMPANY-NAME',
			'telcodata:Company',
			'npanxxsource:Carrier Name',
			'neca4:companyname',
			'neca4:company',
			'nanpa:Company',
		],
		'Abbreviated OCN Name'=>[
			'npanxxsource:Abbreviation',
		],
		'Category'=>[
			'localcallingguide:COMPANY-TYPE',
			'npanxxsource:OCN Type',
		],
		'Overall OCN'=>[
			'neca4:trgt',
		],
		'Last Known Address Indicator'=>[
		],
		'Last Name'=>[
			'neca4:name',
		],
		'First Name'=>[
			'neca4:name',
		],
		'Middle Initial'=>[
			'neca4:name',
		],
		'Company Name'=>[
			'neca4:companyname',
			'neca4:org',
			'neca4:careof',
		],
		'Title'=>[
			'neca4:title',
		],
		'Address 1'=>[
			'neca4:pobox',
			'npanxxsource:Carrier Address',
			'neca4:street',
		],
		'Address 2'=>[
			'neca4:suite',
		],
		'Floor'=>[
			'neca4:suite',
		],
		'Room'=>[
			'neca4:suite',
		],
		'City'=>[
			'neca4:pocity',
			'npanxxsource:Carrier City',
			'neca4:city',
		],
		'State/Province/Territory/Country'=>[
			'neca4:pocountry',
			'neca4:pocity',
			'npanxxsource:Carrier Country',
			'npanxxsource:Carrier State',
			'neca4:country',
			'neca4:city',
		],
		'Zip/Postal Code'=>[
			'neca4:pocity',
			'npanxxsource:Carrier Zip',
			'neca4:city',
		],
		'Phone'=>[
		],
	);
	my %mapping = (
		'Operating Company Name'=>{
			'areacodes:COMPANY'=>sub{
				my ($val,$dat) = @_;
				return $val;
				return "\U$val\E";
			},
			'localcallingguide:COMPANY-NAME'=>sub{
				my ($val,$dat) = @_;
				$val =~ s/\&amp;/\&/g;
				return $val;
				return "\U$val\E";
			},
			'telcodata:Company'=>sub{
				my ($val,$dat) = @_;
				return $val;
				return "\U$val\E";
			},
			'npanxxsource:Carrier Name'=>sub{
				my ($val,$dat) = @_;
				return $val;
				return "\U$val\E";
			},
			'neca4:companyname'=>sub{
				my ($val,$dat) = @_;
				return $val;
				return "\U$val\E";
			},
			'neca4:company'=>sub{
				my ($val,$dat) = @_;
				$val =~ s/\(FORMERLY\)\s*/fka /;
				return $val;
				return "\U$val\E";
			},
			'nanpa:Company'=>sub{
				my ($val,$dat) = @_;
				return $val;
				return "\U$val\E";
			},
		},
		'Category'=>{
			'localcallingguide:COMPANY-TYPE'=>sub{
				my ($val,$dat) = @_;
				$val =~ s/^I$/ILEC/;
				$val =~ s/^C$/CLEC/;
				$val =~ s/^W$/WIRELESS/;
				return $val;
			},
		},
		'Last Name'=>{
			'neca4:name'=>sub{
				my ($val,$dat) = @_;
				$val =~ s/(Mr\.?|Mrs\.?|Ms\.?|Dr\.?)\s+//;
				$val =~ s/\s+(Esq\.?|Jr\.?|Sr\.?|II|III|IV)//;
				$val =~ s/^\s+//;
				$val =~ s/\s+$//;
				my @names = split(/\s+/,$val);
				return $names[-1];
			},
		},
		'First Name'=>{
			'neca4:name'=>sub{
				my ($val,$dat) = @_;
				$val =~ s/(Mr\.?|Mrs\.?|Ms\.?|Dr\.?)\s+//;
				$val =~ s/\s+(Esq\.?|Jr\.?|Sr\.?|II|III|IV)//;
				$val =~ s/^\s+//;
				$val =~ s/\s+$//;
				my @names = split(/\s+/,$val);
				return $names[0];
			},
		},
		'Middle Initial'=>{
			'neca4:name'=>sub{
				my ($val,$dat) = @_;
				$val =~ s/(Mr\.?|Mrs\.?|Ms\.?|Dr\.?)\s+//;
				$val =~ s/\s+(Esq\.?|Jr\.?|Sr\.?|II|III|IV)//;
				$val =~ s/^\s+//;
				$val =~ s/\s+$//;
				my @names = split(/\s+/,$val);
				return '' unless @names > 2;
				my $val = $names[1];
				$val =~ s/(.).*/\U\1\E./;
				return $val;
			},
		},
		'Address 2'=>{
			'neca4:suite'=>sub{
				my ($val,$dat) = @_;
				if ($val =~ /Room|Rm|Suite|Ste|Floor|Flr/) {
					return undef;
				}
				return $val
			},
		},
		'Floor'=>{
			'neca4:suite'=>sub{
				my ($val,$dat) = @_;
				if ($val !~ /Floor|Flr/) {
					return undef;
				}
				return $val
			},
		},
		'Room'=>{
			'neca4:suite'=>sub{
				my ($val,$dat) = @_;
				if ($val !~ /Room|Rm|Suite|Ste/) {
					return undef;
				}
				return $val
			},
		},
		'City'=>{
			'neca4:pocity'=>sub{
				my ($val,$dat) = @_;
				$val =~ s/\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)//;
				return $val;
			},
			'neca4:city'=>sub{
				my ($val,$dat) = @_;
				$val =~ s/\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)//;
				return $val;
			},
		},
		'State/Province/Territory/Country'=>{
			'neca4:pocity'=>sub{
				my ($val,$dat) = @_;
				if ($val =~ /\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)/) {
					return $1;
				}
				return undef;
			},
			'neca4:city'=>sub{
				my ($val,$dat) = @_;
				if ($val =~ /\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)/) {
					return $1;
				}
				return undef;
			},
		},
		'Zip/Postal Code'=>{
			'neca4:pocity'=>sub{
				my ($val,$dat) = @_;
				if ($val =~ /\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)/) {
					return $2;
				}
				return undef;
			},
			'neca4:city'=>sub{
				my ($val,$dat) = @_;
				if ($val =~ /\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)/) {
					return $2;
				}
				return undef;
			},
		},
	);
	my %mismatches = ();
	my %matches = ();
	my %totals = ();
	$fn = "ocn.csv";
	my $header = 1;
	my @fields = ();
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $data = {};
		for (my $i=0;$i<@fields;$i++) {
			$data->{$fields[$i]} = $tokens[$i] if !exists $data->{$fields[$i]} or length($tokens[$i]);
		}
		my $disagrees = 0;
		foreach my $key (sort keys %merges) {
			my $mismatch = 0;
			my @vals = ();
			my @srcs = ();
			foreach my $fld (@{$merges{$key}}) {
				my $val = $data->{$fld};
				$val = &{$mapping{$key}{$fld}}($val,$data)
					if length($val) and exists $mapping{$key}{$fld};
				if (length($val)) {
					if (my $pre = $data->{$key}) {
						if ($pre ne $val) {
							my $upre = "\U$pre\E";
							my $uval = "\U$val\E";
							if ($upre ne $uval) {
								$mismatch++;
								$disagrees++;
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
						$totals{$key}{$src}++;
					} else {
						$flag = '****';
						$mismatches{$key}{$src}++;
						$totals{$key}{$src}++;
					}
					printf STDERR "E: %-4.4s %-48.48s %s $flag\n",
						$data->{OCN},
						$key.' '.$src,
						"'$val'";
				}
			} elsif (@vals > 1) {
				for (my $i=0;$i<@vals;$i++) {
					$matches{$key}{$srcs[$i]}++;
					$totals{$key}{$srcs[$i]}++;
				}
			}
		}
		if ($disagrees) {
			print STDERR "V: --------------------\n";
			foreach my $key (sort keys %merges) {
				foreach my $fld (@{$merges{$key}}) {
					my ($src) = split(/:/,$fld);
					printf STDERR "V: %-40.40s: '%s'\n",
						$fld, $data->{$fld}
						if length($data->{$src});
				}
			}
		}
		if ($data->{OCN}) {
			$data->{'Operating Company Number (OCN)'} = $data->{OCN};
			printf $of "%-4.4s", $data->{'Operating Company Number (OCN)'};
			printf $of "%-50.50s", $data->{'Operating Company Name'};
			printf $of "%-20.20s", $data->{'Abbreviated OCN Name'};
			printf $of "%-2.2s", $data->{'State/Province/Territory/Country of OCN'};
			printf $of "%-10.10s", $data->{'Category'};
			printf $of "%-4.4s", $data->{'Overall OCN'};
			printf $of "%-5.5s", $data->{'filler'};
			printf $of "%-1.1s", $data->{'Last Known Address Indicator'};
			printf $of "%-20.20s", $data->{'Last Name'};
			printf $of "%-10.10s", $data->{'First Name'};
			printf $of "%-1.1s", $data->{'Middle Initial'};
			printf $of "%-50.50s", $data->{'Company Name'};
			printf $of "%-30.30s", $data->{'Title'};
			printf $of "%-30.30s", $data->{'Address 1'};
			printf $of "%-30.30s", $data->{'Address 2'};
			printf $of "%-10.10s", $data->{'Floor'};
			printf $of "%-20.20s", $data->{'Room'};
			printf $of "%-20.20s", $data->{'City'};
			printf $of "%-2.2s", $data->{'State/Province/Territory/Country'};
			printf $of "%-9.9s", $data->{'Zip/Postal Code'};
			printf $of "%-12.12s", $data->{'Phone'};
			printf $of "%-10.10s", $data->{'filler'};
			print $of "\n";
		}
	}
	close($fh);
	close($of);
	foreach my $k (keys %mismatches) {
		foreach my $s (keys %{$mismatches{$k}}) {
			my $m = $mismatches{$k}{$s};
			my $t = $totals{$k}{$s} if exists $totals{$k}{$s};
			my $p = 100 * ($m/$t) if $t;
			printf "I: %9d mismatches of %9d (%3d\%) on %-32.32s by %s\n", $m, $t, $p, $k, $s;
		}
	}
	foreach my $k (keys %matches) {
		foreach my $s (keys %{$matches{$k}}) {
			my $m = $matches{$k}{$s};
			my $t = $totals{$k}{$s} if exists $totals{$k}{$s};
			my $p = 100 * ($m/$t) if $t;
			printf "I: %9d    matches of %9d (%3d\%) on %-32.32s by %s\n", $m, $t, $p, $k, $s;
		}
	}
}

sub printlerg1con {
	my $fn = "LERG1CON.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
	close($of);
}

sub printlerg2 {
	my $fn = "LERG2.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
	close($of);
}

sub printlerg3 {
	my $fn = "LERG3.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
	close($of);
}

sub printlerg4 {
	my $fn = "LERG4.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
EOF
	close($of);
}

sub printlerg5 {
	my $fn = "LERG5.DAT";
	open($of,">",$fn) or die "can't open $fn";
	printnotice($of);
	print $of <<'EOF';
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
EOF
	close($of);
}

sub printlerg6 {
	my $fn;
	my %latas = ();
	$fn = "$datadir/lata.txt";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	while (<$fh>) { chomp;
		next if /^#/;
		next unless /^[0-9]/;
		my ($lata,$name) = split(/\t/,$_);
		$latas{$lata} = $name unless exists $latas{$lata};
	}
	close($fh);
	my %ocns = ();
	$fn = "$datadir/neca4/ocn.csv";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $ocn_header = 1;
	my @ocn_fields = ();
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($ocn_header) {
			@ocn_fields = @tokens;
			$ocn_header = undef;
			next;
		}
		my $data = {};
		if ($data->{OCN} = $tokens[0]) {
			for (my $i=1;$i<@ocn_fields;$i++) {
				$data->{"neca4:$ocn_fields[$i]"} = $tokens[$i] if $tokens[$i];
			}
			$ocns{$data->{OCN}} = $data;
		}
	}
	close($fh);
	$fn = "LERG6.DAT";
	print STDERR "I: writing $fn...\n";
	open($of,">",$fn) or die "can't open $fn";
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
	my @keys = (
		'LATA (of Switch)',
		'LATA Name',
		'Status',
		'Effective Date',
		'NPA',
		'NXX',
		'Block ID',
		'filler',
		'COC Type',
		'Special Service Code (SSC)',
		'Dialable Indicator (DIND)',
		'Terminating Digits - End Office (EO)',
		'Terminating Digits - Access Tandem (AT)',
		'Portable Indicator',
		'Administrative OCN (AOCN)',
		'filler',
		'Operating Company Number (OCN)',
		'Locality Name',
		'Locality Country',
		'Locality State/Province/Territory/Country',
		'RC Name Abbreviation',
		'RC Type',
		'Lines from',
		'Lines to',
		'Switch',
		'Switch Homing Arrangement (SHA) Indicator',
		'filler',
		'Test line number',
		'Response',
		'Expiration Date',
		'Thousands Block Pooling Indicator',
		'LATA (of Rate Center Location)',
		'filler',
		'Creation Date in BIRRDS',
		'filler',
		'E Status Date',
		'filler',
		'Last Modification Date',
		'filler',
	);
	my %merges = (
		'LATA (of Switch)'=>[
#			'areacodes:LATA', # out of date
			'telcodata:LATA',
			'localcallingguide:LATA',
			'npanxxsource:Rate Center LATA',
			'npanxxsource:Wire Center LATA',
			'npanxxsource:LATA',
			'neca4:lata',
		],
		'LATA Name'=>[
#			'telcodata:LATA',
#			'npanxxsource:LATA Name',
		],
		'Status'=>[
		],
		'Effective Date'=>[
#			'telcodata:Date',   # really assignment date
#			'nanpa:AssignDate', # really assignment date
			'npanxxsource:Effective Date',
			'localcallingguide:EFFDATE',
			'nanpa:EffectiveDate',
		],
		'NPA'=>[
			'NPA',
		],
		'NXX'=>[
			'NXX',
		],
		'Block ID'=>[
			'X',
		],
		'COC Type'=>[
		],
		'Special Service Code (SSC)'=>[
		],
		'Dialable Indicator (DIND)'=>[
		],
		'Terminating Digits - End Office (EO)'=>[
		],
		'Terminating Digits - Access Tandem (AT)'=>[
		],
		'Portable Indicator'=>[
			'neca4:feat',
			'npanxxsource:Portable Block',
		],
		'Administrative OCN (AOCN)'=>[
#			'areacodes:OCN', # out of date
#			'areacodes:COMPANY', # out of date
#			'telcodata:OCN',
#			'telcodata:Company',
#			'npanxxsource:OCN',
#			'npanxxsource:Carrier Name',
#			'localcallingguide:OCN',
#			'localcallingguide:COMPANY-NAME',
#			'localcallingguide:ILEC-OCN',
#			'localcallingguide:ILEC-NAME',
			'nanpa:OCN',
			'nanpa:Company',
			'nanpa:Status',
			'nanpa:Use',
		],
		'Operating Company Number (OCN)'=>[
#			'neca4:ocn',
#			'neca4:company',
#			'areacodes:OCN', # out of date
#			'areacodes:COMPANY', # out of date
			'telcodata:OCN',
			'telcodata:Company',
			'npanxxsource:OCN',
			'npanxxsource:Carrier Name',
			'localcallingguide:OCN',
			'localcallingguide:COMPANY-NAME',
			'nanpa:OCN',
			'nanpa:Company',
			'nanpa:Status',
			'nanpa:Use',
		],
		'Locality Name'=>[
			#'neca4:loc', # really switch location
			'pinglo:Rate Center',
			'areacodes:RATE_CENTER',
			'localcallingguide:RC',
		],
		'Locality Country'=>[
			'areacodes:COUNTRY',
		],
		'Locality State/Province/Territory/Country'=>[
			#'neca4:state', # really switch state
			'pinglo:State',
			'areacodes:STATE',
			'telcodata:State',
			'localcallingguide:REGION',
			'nanpa:State',
		],
		'RC Name Abbreviation'=>[
#			'areacodes:RATE_CENTER', # out of date
			'nanpa:RateCenter',
			'localcallingguide:RCSHORT',
			'npanxxsource:LERG Abbreviation',
		],
		'RC Type'=>[
			'npanxxsource:Zone Type',
		],
		'Lines from'=>[
			'npanxxsource:Line From',
			'neca4:rng',
		],
		'Lines to'=>[
			'npanxxsource:Line To',
			'neca4:rng',
		],
		'Switch'=>[
#			'pinglo:Switch',
#			'areacodes:SWITCH_CLLI', # really bad data
			'telcodata:Switch',
			'localcallingguide:SWITCH',
			'neca4:clli',
		],
		'Switch Homing Arrangement (SHA) Indicator'=>[
		],
		'Test line number'=>[
		],
		'Response'=>[
		],
		'Expiration Date'=>[
		],
		'Thousands Block Pooling Indicator'=>[
		],
		'LATA (of Rate Center Location)'=>[
		],
		'Creation Date in BIRRDS'=>[
		],
		'E Status Date'=>[
		],
		'Last Modification Date'=>[
		],
	);
	my %stnames = (
		'ALABAMA'=>'AL',
		'ALASKA'=>'AK',
		'AMERICAN SAMOA'=>'AS',
		'ARIZONA'=>'AZ',
		'ARKANSAS'=>'AR',
		'CALIFORNIA'=>'CA',
		'COLORADO'=>'CO',
		'CONNECTICUT'=>'CT',
		'DELAWARE'=>'DE',
		'DISTRICT OF COLUMBIA'=>'DC',
		'FLORIDA'=>'FL',
		'GEORGIA'=>'GA',
		'GUAM'=>'GU',
		'HAWAII'=>'HI',
		'IDAHO'=>'ID',
		'ILLINOIS'=>'IL',
		'INDIANA'=>'IN',
		'IOWA'=>'IA',
		'KANSAS'=>'KS',
		'KENTUCKY'=>'KY',
		'LOUISIANA'=>'LA',
		'MAINE'=>'ME',
		'MARYLAND'=>'MD',
		'MASSACHUSETTS'=>'MA',
		'MICHIGAN'=>'MI',
		'MINNESOTA'=>'MN',
		'MISSISSIPPI'=>'MS',
		'MISSOURI'=>'MO',
		'MONTANA'=>'MT',
		'NEBRASKA'=>'NE',
		'NEVADA'=>'NV',
		'NEW HAMPSHIRE'=>'NH',
		'NEW JERSEY'=>'NJ',
		'NEW MEXICO'=>'NM',
		'NEW YORK'=>'NY',
		'NORTH CAROLINA'=>'NC',
		'NORTH DAKOTA'=>'ND',
		'OHIO'=>'OH',
		'OKLAHOMA'=>'OK',
		'OREGON'=>'OR',
		'PENNSYLVANIA'=>'PA',
		'PUERTO RICO'=>'PR',
		'RHODE ISLAND'=>'RI',
		'SOUTH CAROLINA'=>'SC',
		'SOUTH DAKOTA'=>'SD',
		'TENNESSEE'=>'TN',
		'TEXAS'=>'TX',
		'UTAH'=>'UT',
		'VERMONT'=>'VT',
		'VIRGINIA'=>'VA',
		'VIRGIN ISLANDS'=>'VI',
		'WASHINGTON'=>'WA',
		'WEST VIRGINIA'=>'WV',
		'WISCONSIN'=>'WI',
		'WYOMING'=>'wY',
	);
	my %mapping = (
		'LATA (of Switch)'=>{
			'telcodata:LATA'=>sub{
				my $val = shift;
				if ($val =~ /(.*?)\s+\(([0-9]+)\)/) {
					return $2;
				}
				return undef;
			},
		},
		'LATA Name'=>{
			'telcodata:LATA'=>sub{
				my $val = shift;
				if ($val =~ /(.*?)\s+\(([0-9]+)\)/) {
					return $1;
				}
				return undef;
			},
		},
		'Status'=>{
		},
		'Effective Date'=>{
			'telcodata:Date'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq '00/00/0000';
				return undef if $val eq '0000-00-00';
				if ($val =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{4})/) {
					return "$1$2".substr($3,2,2);
				}
				if ($val =~ /([0-9]{4})-([0-9]{2})-([0-9]{2})/) {
					return "$2$3".substr($1,2,2);
				}
				return undef;
			},
			'nanpa:AssignDate'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq '00/00/0000';
				return undef if $val eq '0000-00-00';
				if ($val =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{4})/) {
					return "$1$2".substr($3,2,2);
				}
				if ($val =~ /([0-9]{4})-([0-9]{2})-([0-9]{2})/) {
					return "$2$3".substr($1,2,2);
				}
				return undef;
			},
			'npanxxsource:Effective Date'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq '00/00/0000';
				return undef if $val eq '0000-00-00';
				if ($val =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{4})/) {
					return "$1$2".substr($3,2,2);
				}
				if ($val =~ /([0-9]{4})-([0-9]{2})-([0-9]{2})/) {
					return "$2$3".substr($1,2,2);
				}
				return undef;
			},
			'localcallingguide:EFFDATE'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq '00/00/0000';
				return undef if $val eq '0000-00-00';
				if ($val =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{4})/) {
					return "$1$2".substr($3,2,2);
				}
				if ($val =~ /([0-9]{4})-([0-9]{2})-([0-9]{2})/) {
					return "$2$3".substr($1,2,2);
				}
				return undef;
			},
			'nanpa:EffectiveDate'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq '00/00/0000';
				return undef if $val eq '0000-00-00';
				if ($val =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{4})/) {
					return "$1$2".substr($3,2,2);
				}
				if ($val =~ /([0-9]{4})-([0-9]{2})-([0-9]{2})/) {
					return "$2$3".substr($1,2,2);
				}
				return undef;
			},
		},
		'NPA'=>{
		},
		'NXX'=>{
		},
		'Block ID'=>{
		},
		'COC Type'=>{
		},
		'Special Service Code (SSC)'=>{
		},
		'Dialable Indicator (DIND)'=>{
		},
		'Terminating Digits - End Office (EO)'=>{
		},
		'Terminating Digits - Access Tandem (AT)'=>{
		},
		'Portable Indicator'=>{
			'neca4:feat'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return ($val =~ /\bGR\b/) ? 'Y' : 'N';
			},
			'npanxxsource:Portable Block'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return ($val =~ /Yes/i) ? 'Y' : 'N';
			},
		},
		'Administrative OCN (AOCN)'=>{
			'localcallingguide:ILEC-OCN'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				#return undef if exists $dat->{X} and $dat->{X} ne '';
				return $val;
			},
			'localcallingguide:ILEC-NAME'=>sub{
				return undef;
			},
			'areacodes:OCN'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				return undef if exists $dat->{X} and $dat->{X} ne '';
				return $val;
			},
			'areacodes:COMPANY'=>sub{
				return undef;
			},
			'telcodata:OCN'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				return undef if exists $dat->{X} and $dat->{X} ne '';
				return $val;
			},
			'telcodata:Company'=>sub{
				return undef;
			},
			'npanxxsource:OCN'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				return undef if exists $dat->{X} and $dat->{X} ne '';
				return $val;
			},
			'npanxxsource:Carrier Name'=>sub{
				return undef;
			},
			'localcallingguide:OCN'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				return undef if exists $dat->{X} and $dat->{X} ne '';
				return $val;
			},
			'localcallingguide:COMPANY-NAME'=>sub{
				return undef;
			},
			'nanpa:OCN'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				#return undef if exists $dat->{X} and $dat->{X} ne '';
				return $val;
			},
			'nanpa:Company'=>sub{
				return undef;
			},
			'nanpa:Status'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if exists $dat->{X} and $dat->{X} ne '';
				return 'XXXX' if $val eq 'Not Available' and !$dat->{'nanpa:OCN'};
				return undef;
			},
			'nanpa:Use'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if exists $dat->{X} and $dat->{X} ne '';
				return 'XXXX' if $val eq 'UA' and !$dat->{'nanpa:OCN'};
				return undef;
			},
		},
		'Operating Company Number (OCN)'=>{
			'telcodata:OCN'=>sub{
				my $val = shift;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				return $val;
			},
			'npanxxsource:OCN'=>sub{
				my $val = shift;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				return $val;
			},
			'areacodes:COMPANY'=>sub{
				return undef;
			},
			'localcallingguide:COMPANY-NAME'=>sub{
				return undef;
			},
			'nanpa:OCN'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return undef if $val eq 'N/A';
				return undef if $val eq 'MULT';
				return $val;
			},
			'nanpa:Company'=>sub{
				return undef;
			},
			'npanxxsource:Carrier Name'=>sub{
				return undef;
			},
			'telcodata:Company'=>sub{
				return undef;
			},
			'neca4:company'=>sub{
				return undef;
			},
			'nanpa:Status'=>sub{
				my $val = shift;
				return undef unless $val;
				return 'XXXX' if $val eq 'Not Available';
				return undef;
			},
			'nanpa:Use'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return 'XXXX' if $val eq 'UA' and !$dat->{'nanpa:OCN'};
				return undef;
			},
		},
		'Locality Name'=>{
		},
		'Locality Country'=>{
		},
		'Locality State/Province/Territory/Country'=>{
			'neca4:state'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				if (exists $stnames{$val}) {
					return $stnames{$val};
				}
				return undef;
			},
		},
		'RC Name Abbreviation'=>{
			'nanpa:RateCenter'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				$val = substr("\U$val\E",0,10);
				$val = s/\s+$//;
				return $val;
			},
		},
		'RC Type'=>{
		},
		'Lines from'=>{
			'npanxxsource:Line From'=>sub{
				my $val = shift;
				return undef unless $val;
				if ($val =~ /([0-9]{4})\s*$/) {
					return $1;
				}
				return undef;
			},
			'neca4:rng'=>sub{
				my $val = shift;
				return undef unless $val;
				if ($val =~ /([0-9]{4})-([0-9]{4})/) {
					return $1;
				}
				return undef;
			},
		},
		'Lines to'=>{
			'npanxxsource:Line To'=>sub{
				my $val = shift;
				return undef unless $val;
				if ($val =~ /([0-9]{4})\s*$/) {
					return $1;
				}
				return undef;
			},
			'neca4:rng'=>sub{
				my $val = shift;
				return undef unless $val;
				if ($val =~ /([0-9]{4})-([0-9]{4})/) {
					return $2;
				}
				return undef;
			},
		},
		'Switch'=>{
			'pinglo:Switch'=>sub{
				my $val = shift;
				return undef unless $val;
				return undef if $val eq 'XXXXXXXXXXX';
				return undef if $val eq '__VARIOUS__';
				return undef if $val eq 'NOCLLIKNOWN';
				return $val;
			},
			'telcodata:Switch'=>sub{
				my $val = shift;
				return undef unless $val;
				return undef if $val eq 'XXXXXXXXXXX';
				return undef if $val eq '__VARIOUS__';
				return undef if $val eq 'NOCLLIKNOWN';
				return $val;
			},
		},
		'Switch Homing Arrangement (SHA) Indicator'=>{
		},
		'Test line number'=>{
		},
		'Response'=>{
		},
		'Expiration Date'=>{
		},
		'Thousands Block Pooling Indicator'=>{
		},
		'LATA (of Rate Center Location)'=>{
		},
		'Creation Date in BIRRDS'=>{
		},
		'E Status Date'=>{
		},
		'Last Modification Date'=>{
		},
	);
	my %mismatches = ();
	my %matches = ();
	my %totals = ();
	$fn = "db.csv";
	my $header = 1;
	my @fields = ();
	my $all = undef;
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $data = {};
		for (my $i=0;$i<@fields;$i++) {
			$data->{$fields[$i]} = $tokens[$i] if !exists $data->{$fields[$i]} or length($tokens[$i]);
		}
		if (my $ocn = $data->{'neca4:ocn'}) {
			if (exists $ocns{$ocn}) {
				for (my $i=1;$i<@ocn_fields;$i++) {
					my $fld = "neca4:$ocn_fields[$i]";
					$data->{$fld} = $ocns{$ocn}{$fld} if $ocns{$ocn}{$fld};
				}
			}
		}
		my $disagrees = 0;
		foreach my $key (sort keys %merges) {
			my $mismatch = 0;
			my @vals = ();
			my @srcs = ();
			foreach my $fld (@{$merges{$key}}) {
				my $val = $data->{$fld};
				$val = &{$mapping{$key}{$fld}}($val,$data)
					if length($val) and exists $mapping{$key}{$fld};
				if (length($val)) {
					if (my $pre = $data->{$key}) {
						if ($pre ne $val) {
							my $upre = "\U$pre\E";
							my $uval = "\U$val\E";
							if ($upre ne $uval) {
								$mismatch++;
								$disagrees++;
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
						$totals{$key}{$src}++;
					} else {
						$flag = '****';
						$mismatches{$key}{$src}++;
						$totals{$key}{$src}++;
					}
					printf STDERR "E: %-3.3s %-3.3s %-1.1s %-48.48s %s $flag\n",
						$data->{NPA},
						$data->{NXX},
						$data->{'Block ID'},
						$key.' '.$src,
						"'$val'";
				}
			} elsif (@vals > 1) {
				for (my $i=0;$i<@vals;$i++) {
					$matches{$key}{$srcs[$i]}++;
					$totals{$key}{$srcs[$i]}++;
				}
			}
		}
		if ($disagrees) {
			print STDERR "V: --------------------\n";
			foreach my $key (sort keys %merges) {
				foreach my $fld (@{$merges{$key}}) {
					my ($src) = split(/:/,$fld);
					printf STDERR "V: %-40.40s: '%s'\n",
						$fld, $data->{$fld}
						if length($data->{$src});
				}
			}
		}
		if ($all) {
			if ($all->{NPA} eq $data->{NPA} and $all->{NXX} eq $data->{NXX}) {
				for (my $i=3;$i<@keys;$i++) {
					my $k = $keys[$i];
					unless (length($data->{$k})) {
						$data->{$k} = $all->{$k};
					}
				}
			} else {
				$all = undef;
			}
		}
		if ($data->{NPA} and $data->{NXX}) {
			my $lata = $data->{'LATA (of Switch)'};
			my $name = $latas{$lata} if $lata;
			$data->{'LATA Name'} = $name if $name;
			$data->{'Lines from'} = '1212' if $data->{NXX} eq '555';
			$data->{'Lines to'}   = '1212' if $data->{NXX} eq '555';
			$data->{'Lines from'} = '0000' unless length($data->{'Lines from'});
			$data->{'Lines to'}   = '9999' unless length($data->{'Lines to'});
			$data->{'Lines from'} = '' if $data->{NXX} =~ /[2-9]11/;
			$data->{'Lines to'}   = '' if $data->{NXX} =~ /[2-9]11/;
			$data->{'Block ID'}   = 'A' unless length($data->{'Block ID'});
			printf $of "%-5.5s", $data->{'LATA (of Switch)'};
			printf $of "%-20.20s", $data->{'LATA Name'};
			printf $of "%-1.1s", $data->{'Status'};
			printf $of "%-6.6s", $data->{'Effective Date'};
			printf $of "%-3.3s", $data->{'NPA'};
			printf $of "%-3.3s", $data->{'NXX'};
			printf $of "%-1.1s", $data->{'Block ID'};
			printf $of "%-4.4s", $data->{'filler'};
			printf $of "%-3.3s", $data->{'COC Type'};
			printf $of "%-4.4s", $data->{'Special Service Code (SSC)'};
			printf $of "%-1.1s", $data->{'Dialable Indicator (DIND)'};
			printf $of "%-2.2s", $data->{'Terminating Digits - End Office (EO)'};
			printf $of "%-2.2s", $data->{'Terminating Digits - Access Tandem (AT)'};
			printf $of "%-1.1s", $data->{'Portable Indicator'};
			printf $of "%-4.4s", $data->{'Administrative OCN (AOCN)'};
			printf $of "%-1.1s", $data->{'filler'};
			printf $of "%-4.4s", $data->{'Operating Company Number (OCN)'};
			printf $of "%-10.10s", $data->{'Locality Name'};
			printf $of "%-2.2s", $data->{'Locality Country'};
			printf $of "%-2.2s", $data->{'Locality State/Province/Territory/Country'};
			printf $of "%-10.10s", $data->{'RC Name Abbreviation'};
			printf $of "%-1.1s", $data->{'RC Type'};
			printf $of "%-4.4s", $data->{'Lines from'};
			printf $of "%-4.4s", $data->{'Lines to'};
			printf $of "%-11.11s", $data->{'Switch'};
			printf $of "%-2.2s", $data->{'Switch Homing Arrangement (SHA) Indicator'};
			printf $of "%-4.4s", $data->{'filler'};
			printf $of "%-4.4s", $data->{'Test line number'};
			printf $of "%-1.1s", $data->{'Response'};
			printf $of "%-6.6s", $data->{'Expiration Date'};
			printf $of "%-1.1s", $data->{'Thousands Block Pooling Indicator'};
			printf $of "%-5.5s", $data->{'LATA (of Rate Center Location)'};
			printf $of "%-1.1s", $data->{'filler'};
			printf $of "%-6.6s", $data->{'Creation Date in BIRRDS'};
			printf $of "%-1.1s", $data->{'filler'};
			printf $of "%-6.6s", $data->{'E Status Date'};
			printf $of "%-1.1s", $data->{'filler'};
			printf $of "%-6.6s", $data->{'Last Modification Date'};
			printf $of "%-17.17s", $data->{'filler'};
			print $of "\n";
			unless ($all) {
				if (!exists $data->{X} or !length($data->{X})) {
					$all = $data;
				}
			}
		}
	}
	close($fh);
	close($of);
	foreach my $k (keys %mismatches) {
		foreach my $s (keys %{$mismatches{$k}}) {
			my $m = $mismatches{$k}{$s};
			my $t = $totals{$k}{$s} if exists $totals{$k}{$s};
			my $p = 100 * ($m/$t) if $t;
			printf "I: %9d mismatches of %9d (%3d\%) on %-32.32s by %s\n", $m, $t, $p, $k, $s;
		}
	}
	foreach my $k (keys %matches) {
		foreach my $s (keys %{$matches{$k}}) {
			my $m = $matches{$k}{$s};
			my $t = $totals{$k}{$s} if exists $totals{$k}{$s};
			my $p = 100 * ($m/$t) if $t;
			printf "I: %9d    matches of %9d (%3d\%) on %-32.32s by %s\n", $m, $t, $p, $k, $s;
		}
	}
}

sub printlerg6atc {
	my $fn = "LERG6ATC.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
	close($of);
}

sub printlerg7 {
	my $fn = "LERG7.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
	close($of);
}

sub printlerg7sha {
	my $fn = "LERG7SHA.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
	close($of);
}

sub printlerg8 {
	my $fn;
	$fn = "LERG8.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
# within its boundaries (based on NXXs defined to the Rate Center) are
# considered local (non-toll) calls.  However, note that local calls can also
# extend to other (e.g. adjoining) Rate Centers (information regarding this
# extension of local calling area is not provided in the LERG).
#
# Rate Centers are generally defined in tariffs filed by principal wireline
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
# 150-150  1 Split Indicator (Y/N) (If RC is split across NPAs)
# 151-153  3 Embedded Overlay NPA 1
# 154-156  3 Embedded Overlay NPA 2
# 157-159  3 Embedded Overlay NPA 3
# 160-162  3 Embedded Overlay NPA 4
# 163-172 10 filler
# ---------
EOF
	my @keys = (
		'LATA',
		'LATA Name',
		'STatus',
		'Effective Date',
		'RC State/Province/Territory/Country',
		'RC Center (RC) Name Abbreviation',
		'RC Type',
		'RC Full Name',
		'Major Vertical Coordinate',
		'Major Horizontal Coordinate',
		'Minor Vertical Coordinate',
		'Minor Horizontal Coordinate',
		'NPA 1 (Area Code)',
		'NPA 2',
		'NPA 3',
		'NPA 4',
		'NPA 5',
		'NPA 6',
		'NPA 7',
		'NPA 8',
		'NPA 9',
		'NPA 10',
		'Major Trading Area (MTA) - 1',
		'Major Trading Area (MTA) - 2',
		'Split Indicator (Y/N)',
		'Embedded Overlay NPA 1',
		'Embedded Overlay NPA 2',
		'Embedded Overlay NPA 3',
		'Embedded Overlay NPA 4',
		'filler',
	);
	my %merges = (
		'LATA'=>[
			'telcodata:LATA',
			'localcallingguide:LATA',
			'npanxxsource:Rate Center LATA',
		],
		'LATA Name'=>[
			#'telcodata:LATA',
		],
		'STatus'=>[
		],
		'Effective Date'=>[
		],
		'RC State/Province/Territory/Country'=>[
			'REGION',
		],
		'RC Center (RC) Name Abbreviation'=>[
			'RCSHORT',
		],
		'RC Type'=>[
		],
		'RC Full Name'=>[
			'localcallingguide:RC',
			'npanxxsource:Rate Center Name',
		],
		'Major Vertical Coordinate'=>[
			'npanxxsource:Major V&H',
			'RCVH',
			'localcallingguide:RC-V',
		],
		'Major Horizontal Coordinate'=>[
			'npanxxsource:Major V&H',
			'RCVH',
			'localcallingguide:RC-H',
		],
		'Minor Vertical Coordinate'=>[
			'npanxxsource:Minor V&H',
		],
		'Minor Horizontal Coordinate'=>[
			'npanxxsource:Minor V&H',
		],
		'NPA 1 (Area Code)'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 2'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 3'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 4'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 5'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 6'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 7'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 8'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 9'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'NPA 10'=>[
			'telcodata:NPA',
			'localcallingguide:NPA',
			'nanpa:NPA',
			'npanxxsource:NPA',
			'npanxxsource:Area Codes in Rate Center',
		],
		'Major Trading Area (MTA) - 1'=>[
		],
		'Major Trading Area (MTA) - 2'=>[
		],
		'Split Indicator (Y/N)'=>[
		],
		'Embedded Overlay NPA 1'=>[
			'npanxxsource:Embedded Overlays',
		],
		'Embedded Overlay NPA 2'=>[
			'npanxxsource:Embedded Overlays',
		],
		'Embedded Overlay NPA 3'=>[
			'npanxxsource:Embedded Overlays',
		],
		'Embedded Overlay NPA 4'=>[
			'npanxxsource:Embedded Overlays',
		],
		'filler'=>[
		],
	);
	my %mapping = (
		'LATA'=>{
			'telcodata:LATA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				if ($val =~ /\(([0-9]{3})\)/) {
					return $1;
				}
				return undef;
			},
		},
		'LATA Name'=>{
			'telcodata:LATA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				if ($val =~ s/\s*\([0-9]{3}\)//) {
					return $val;
				}
				return undef;
			},
		},
		'STatus'=>{
		},
		'Effective Date'=>{
		},
		'RC State/Province/Territory/Country'=>{
		},
		'RC Center (RC) Name Abbreviation'=>{
		},
		'RC Type'=>{
		},
		'RC Full Name'=>{
		},
		'Major Vertical Coordinate'=>{
			'npanxxsource:Major V&H'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my ($v,$h) = split(/,/,$val);
				return undef unless $v;
				return sprintf "%05d", $v;
			},
			'RCVH'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my ($v,$h) = split(/,/,$val);
				return undef unless $v;
				return sprintf "%05d", $v;
			},
			'localcallingguide:RC-V'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return sprintf "%05d", $val;
			},
		},
		'Major Horizontal Coordinate'=>{
			'npanxxsource:Major V&H'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my ($v,$h) = split(/,/,$val);
				return undef unless $h;
				return sprintf "%05d", $h;
			},
			'RCVH'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my ($v,$h) = split(/,/,$val);
				return undef unless $h;
				return sprintf "%05d", $h;
			},
			'localcallingguide:RC-H'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				return sprintf "%05d", $val;
			},
		},
		'Minor Vertical Coordinate'=>{
			'npanxxsource:Minor V&H'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my ($v,$h) = split(/,/,$val);
				return undef unless $v;
				return sprintf "%05d", $v;
			},
		},
		'Minor Horizontal Coordinate'=>{
			'npanxxsource:Minor V&H'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my ($v,$h) = split(/,/,$val);
				return undef unless $h;
				return sprintf "%05d", $h;
			},
		},
		'NPA 1 (Area Code)'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[0];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[0];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[0];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[0];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[0];
			},
		},
		'NPA 2'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[1];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[1];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[1];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[1];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[1];
			},
		},
		'NPA 3'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[2];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[2];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[2];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[2];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[2];
			},
		},
		'NPA 4'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[3];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[3];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[3];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[3];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[3];
			},
		},
		'NPA 5'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[4];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[4];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[4];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[4];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[4];
			},
		},
		'NPA 6'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[5];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[5];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[5];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[5];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[5];
			},
		},
		'NPA 7'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[6];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[6];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[6];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[6];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[6];
			},
		},
		'NPA 8'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[7];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[7];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[7];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[7];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[7];
			},
		},
		'NPA 9'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[8];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[8];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[8];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[8];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[8];
			},
		},
		'NPA 10'=>{
			'localcallingguide:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[9];
			},
			'telcodata:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[9];
			},
			'nanpa:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[9];
			},
			'npanxxsource:NPA'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/,/,$val);
				return $npas[9];
			},
			'npanxxsource:Area Codes in Rate Center'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[9];
			},
		},
		'Major Trading Area (MTA) - 1'=>{
		},
		'Major Trading Area (MTA) - 2'=>{
		},
		'Split Indicator (Y/N)'=>{
		},
		'Embedded Overlay NPA 1'=>{
			'npanxxsource:Embedded Overlays'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[0];
			},
		},
		'Embedded Overlay NPA 2'=>{
			'npanxxsource:Embedded Overlays'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[1];
			},
		},
		'Embedded Overlay NPA 3'=>{
			'npanxxsource:Embedded Overlays'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[2];
			},
		},
		'Embedded Overlay NPA 4'=>{
			'npanxxsource:Embedded Overlays'=>sub{
				my ($val,$dat) = @_;
				return undef unless $val;
				my @npas = split(/\s+/,$val);
				return $npas[3];
			},
		},
		'filler'=>{
		},
	);
	my %mismatches = ();
	my %matches = ();
	my %totals = ();
	$fn = "rc.csv";
	my $header = 1;
	my @fields = ();
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my $data = {};
		for (my $i=0;$i<@fields;$i++) {
			$data->{$fields[$i]} = $tokens[$i] if !exists $data->{$fields[$i]} or length($tokens[$i]);
		}
		my $disagrees = 0;
		foreach my $key (sort keys %merges) {
			my $mismatch = 0;
			my @vals = ();
			my @srcs = ();
			foreach my $fld (@{$merges{$key}}) {
				my $val = $data->{$fld};
				$val = &{$mapping{$key}{$fld}}($val,$data)
					if length($val) and exists $mapping{$key}{$fld};
				if (length($val)) {
					if (my $pre = $data->{$key}) {
						if ($pre ne $val) {
							my $upre = "\U$pre\E";
							my $uval = "\U$val\E";
							if ($upre ne $uval) {
								$mismatch++;
								$disagrees++;
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
						$totals{$key}{$src}++;
					} else {
						$flag = '****';
						$mismatches{$key}{$src}++;
						$totals{$key}{$src}++;
					}
					printf STDERR "E: RC %-2.2s %-10.10s %-48.48s %s $flag\n",
						$data->{REGION},
						$data->{RCSHORT},
						$key.' '.$src,
						"'$val'";
				}
			} elsif (@vals > 1) {
				for (my $i=0;$i<@vals;$i++) {
					$matches{$key}{$srcs[$i]}++;
					$totals{$key}{$srcs[$i]}++;
				}
			}
		}
		if ($disagrees) {
			print STDERR "V: --------------------\n";
			foreach my $key (sort keys %merges) {
				foreach my $fld (@{$merges{$key}}) {
					my ($src) = split(/:/,$fld);
					printf STDERR "V: %-40.40s: '%s'\n",
						$fld, $data->{$fld}
						if length($data->{$src});
				}
			}
		}
		if ($data->{RCSHORT} and $data->{REGION}) {
			$data->{'RC Center (RC) Name Abbreviation'} = $data->{RCSHORT};
			$data->{'RC State/Province/Territory/Country'} = $data->{REGION};
			printf $of "%-5.5s", $data->{'LATA'};
			printf $of "%-20.20s", $data->{'LATA Name'};
			printf $of "%-1.1s", $data->{'STatus'};
			printf $of "%-6.6s", $data->{'Effective Date'};
			printf $of "%-2.2s", $data->{'RC State/Province/Territory/Country'};
			printf $of "%-10.10s", $data->{'RC Center (RC) Name Abbreviation'};
			printf $of "%-1.1s", $data->{'RC Type'};
			printf $of "%-50.50s", $data->{'RC Full Name'};
			printf $of "%-5.5s", $data->{'Major Vertical Coordinate'};
			printf $of "%-5.5s", $data->{'Major Horizontal Coordinate'};
			printf $of "%-5.5s", $data->{'Minor Vertical Coordinate'};
			printf $of "%-5.5s", $data->{'Minor Horizontal Coordinate'};
			printf $of "%-3.3s", $data->{'NPA 1 (Area Code)'};
			printf $of "%-3.3s", $data->{'NPA 2'};
			printf $of "%-3.3s", $data->{'NPA 3'};
			printf $of "%-3.3s", $data->{'NPA 4'};
			printf $of "%-3.3s", $data->{'NPA 5'};
			printf $of "%-3.3s", $data->{'NPA 6'};
			printf $of "%-3.3s", $data->{'NPA 7'};
			printf $of "%-3.3s", $data->{'NPA 8'};
			printf $of "%-3.3s", $data->{'NPA 9'};
			printf $of "%-3.3s", $data->{'NPA 10'};
			printf $of "%-2.2s", $data->{'Major Trading Area (MTA) - 1'};
			printf $of "%-2.2s", $data->{'Major Trading Area (MTA) - 2'};
			printf $of "%-1.1s", $data->{'Split Indicator (Y/N)'};
			printf $of "%-3.3s", $data->{'Embedded Overlay NPA 1'};
			printf $of "%-3.3s", $data->{'Embedded Overlay NPA 2'};
			printf $of "%-3.3s", $data->{'Embedded Overlay NPA 3'};
			printf $of "%-3.3s", $data->{'Embedded Overlay NPA 4'};
			printf $of "%-10.10s", $data->{'filler'};
			print $of "\n";
		}
	}
	close($fh);
	close($of);
}

sub printlerg8loc {
	my $fn = "LERG8LOC.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
	close($of);
}

sub printlerg9 {
	my $fn = "LERG9.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
}

sub printlerg9atc {
	my $fn = "LERG9ATC.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
}

sub printlerg10 {
	my $fn = "LERG10.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
}

sub printlerg11 {
	my $fn = "LERG11.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
}

sub printlerg12 {
	my $fn = "LERG12.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
}

sub printlerg14 {
	my $fn = "LERG14.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
}

sub printlerg15 {
	my $fn = "LERG15.DAT";
	open($of,">",$fn) or die "can't open $fn";
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
}

#printlerg1();
#printlerg6();
printlerg8();

exit;
