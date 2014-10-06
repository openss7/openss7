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
# corrections to the OpenSS7 project.  Also remember to submit changes to NANPA,
# NECA or Telcordia as necessary to change other datasets.
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
}

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
			my $val = shift;
			return $val;
			return "\U$val\E";
		},
		'localcallingguide:COMPANY-NAME'=>sub{
			my $val = shift;
			$val =~ s/\&amp;/\&/g;
			return $val;
			return "\U$val\E";
		},
		'telcodata:Company'=>sub{
			my $val = shift;
			return $val;
			return "\U$val\E";
		},
		'npanxxsource:Carrier Name'=>sub{
			my $val = shift;
			return $val;
			return "\U$val\E";
		},
		'neca4:companyname'=>sub{
			my $val = shift;
			return $val;
			return "\U$val\E";
		},
		'neca4:company'=>sub{
			my $val = shift;
			$val =~ s/\(FORMERLY\)\s*/fka /;
			return $val;
			return "\U$val\E";
		},
		'nanpa:Company'=>sub{
			my $val = shift;
			return $val;
			return "\U$val\E";
		},
	},
	'Category'=>{
		'localcallingguide:COMPANY-TYPE'=>sub{
			my $val = shift;
			$val =~ s/^I$/ILEC/;
			$val =~ s/^C$/CLEC/;
			$val =~ s/^W$/WIRELESS/;
			return $val;
		},
	},
	'Last Name'=>{
		'neca4:name'=>sub{
			my $val = shift;
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
			my $val = shift;
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
			my $val = shift;
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
			my $val = shift;
			if ($val =~ /Room|Rm|Suite|Ste|Floor|Flr/) {
				return undef;
			}
			return $val
		},
	},
	'Floor'=>{
		'neca4:suite'=>sub{
			my $val = shift;
			if ($val !~ /Floor|Flr/) {
				return undef;
			}
			return $val
		},
	},
	'Room'=>{
		'neca4:suite'=>sub{
			my $val = shift;
			if ($val !~ /Room|Rm|Suite|Ste/) {
				return undef;
			}
			return $val
		},
	},
	'City'=>{
		'neca4:pocity'=>sub{
			my $val = shift;
			$val =~ s/\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)//;
			return $val;
		},
		'neca4:city'=>sub{
			my $val = shift;
			$val =~ s/\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)//;
			return $val;
		},
	},
	'State/Province/Territory/Country'=>{
		'neca4:pocity'=>sub{
			my $val = shift;
			if ($val =~ /\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)/) {
				return $1;
			}
			return undef;
		},
		'neca4:city'=>sub{
			my $val = shift;
			if ($val =~ /\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)/) {
				return $1;
			}
			return undef;
		},
	},
	'Zip/Postal Code'=>{
		'neca4:pocity'=>sub{
			my $val = shift;
			if ($val =~ /\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)/) {
				return $2;
			}
			return undef;
		},
		'neca4:city'=>sub{
			my $val = shift;
			if ($val =~ /\b([A-Z]{2})\s+([0-9]{5}(-[0-9]+)?)/) {
				return $2;
			}
			return undef;
		},
	},
);

my %mismatches = ();
my %matches = ();

sub closerecord {
	my $data = shift;
	if (my $ocn = $data->{OCN}) {
		$data->{'Operating Company Number (OCN)'} = $ocn;
		print STDERR "--------\n";
		my @values = ();
		foreach (@keys) {
			printf STDERR "V: %-30.30s: %s\n", $_, $data->{$_};
			push @values, $data->{$_};
		}
		print $of '"', join('","', @values), '"', "\n";
		printf $o2 "%-4.4s", $data->{'Operating Company Number (OCN)'};
		printf $o2 "%-50.50s", $data->{'Operating Company Name'};
		printf $o2 "%-20.20s", $data->{'Abbreviated OCN Name'};
		printf $o2 "%-2.2s", $data->{'State/Province/Territory/Country of OCN'};
		printf $o2 "%-10.10s", $data->{'Category'};
		printf $o2 "%-4.4s", $data->{'Overall OCN'};
		printf $o2 "%-5.5s", $data->{'filler'};
		printf $o2 "%-1.1s", $data->{'Last Known Address Indicator'};
		printf $o2 "%-20.20s", $data->{'Last Name'};
		printf $o2 "%-10.10s", $data->{'First Name'};
		printf $o2 "%-1.1s", $data->{'Middle Initial'};
		printf $o2 "%-50.50s", $data->{'Company Name'};
		printf $o2 "%-30.30s", $data->{'Title'};
		printf $o2 "%-30.30s", $data->{'Address 1'};
		printf $o2 "%-30.30s", $data->{'Address 2'};
		printf $o2 "%-10.10s", $data->{'Floor'};
		printf $o2 "%-20.20s", $data->{'Room'};
		printf $o2 "%-20.20s", $data->{'City'};
		printf $o2 "%-2.2s", $data->{'State/Province/Territory/Country'};
		printf $o2 "%-9.9s", $data->{'Zip/Postal Code'};
		printf $o2 "%-12.12s", $data->{'Phone'};
		printf $o2 "%-10.10s", $data->{'filler'};
		print $o2 "\n";
	}
}

$fn = "LERG1.DAT";
open($o2,">",$fn) or die "can't open $fn";
printnotice($o2);
$fn = "lerg1.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";
$fn = "ocn.csv";
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

