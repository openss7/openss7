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
# LERG8.DAT
# ---------
# The LERG8 file represents Rate Centers (also known as "Exchange Areas," "Rate
# Exchange Areas," and similar terms).  These are geographical areas defined
# historically and ongoing based on various factors that can vary by
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
}

my @keys = (
	'LATA',
	'LATA Name',
	'Status',
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
	'Split Indicator', # If RC is split across NPAs
	'Embedded Overlay NPA 1',
	'Embedded Overlay NPA 2',
	'Embedded Overlay NPA 3',
	'Embedded Overlay NPA 4',
	'filler',
);

my %merges = (
	'LATA'=>[
	],
	'LATA Name'=>[
	],
	'Status'=>[
	],
	'Effective Date'=>[
	],
	'RC State/Province/Territory/Country'=>[
	],
	'RC Center (RC) Name Abbreviation'=>[
	],
	'RC Type'=>[
	],
	'RC Full Name'=>[
	],
	'Major Vertical Coordinate'=>[
	],
	'Major Horizontal Coordinate'=>[
	],
	'Minor Vertical Coordinate'=>[
	],
	'Minor Horizontal Coordinate'=>[
	],
	'NPA 1 (Area Code)'=>[
	],
	'NPA 2'=>[
	],
	'NPA 3'=>[
	],
	'NPA 4'=>[
	],
	'NPA 5'=>[
	],
	'NPA 6'=>[
	],
	'NPA 7'=>[
	],
	'NPA 8'=>[
	],
	'NPA 9'=>[
	],
	'NPA 10'=>[
	],
	'Major Trading Area (MTA) - 1'=>[
	],
	'Major Trading Area (MTA) - 2'=>[
	],
	'Split Indicator'=>[
	],
	'Embedded Overlay NPA 1'=>[
	],
	'Embedded Overlay NPA 2'=>[
	],
	'Embedded Overlay NPA 3'=>[
	],
	'Embedded Overlay NPA 4'=>[
	],
	'filler'=>[
	],
);

my %mapping = (
	'LATA'=>{
	},
	'LATA Name'=>{
	},
	'Status'=>{
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
	},
	'Major Horizontal Coordinate'=>{
	},
	'Minor Vertical Coordinate'=>{
	},
	'Minor Horizontal Coordinate'=>{
	},
	'NPA 1 (Area Code)'=>{
	},
	'NPA 2'=>{
	},
	'NPA 3'=>{
	},
	'NPA 4'=>{
	},
	'NPA 5'=>{
	},
	'NPA 6'=>{
	},
	'NPA 7'=>{
	},
	'NPA 8'=>{
	},
	'NPA 9'=>{
	},
	'NPA 10'=>{
	},
	'Major Trading Area (MTA) - 1'=>{
	},
	'Major Trading Area (MTA) - 2'=>{
	},
	'Split Indicator'=>{
	},
	'Embedded Overlay NPA 1'=>{
	},
	'Embedded Overlay NPA 2'=>{
	},
	'Embedded Overlay NPA 3'=>{
	},
	'Embedded Overlay NPA 4'=>{
	},
);

my %mismatches = ();
my %matches = ();

sub closerecord {
	my $data = shift;
	if ($data->{'RC Center (RC) Name Abbreviation'} and
	    $data->{'RC Type'} and
	    $data->{'RC State/Province/Territory/Country'})
	{
		print STDERR "--------\n";
		my @values = ();
		foreach (@keys) {
			printf STDERR "V: %-30.30s: %s\n", $_, $data->{$_} if $data->{$_};
			push @values, $data->{$_};
		}
		print $of '"',join('","',@values),'"',"\n";

		printf $o2 "%-5.5s", $data->{'LATA'};
		printf $o2 "%-20.20s", $data->{'LATA Name'};
		printf $o2 "%-1.1s", $data->{'Status'};
		printf $o2 "%-6.6s", $data->{'Effective Date'};
		printf $o2 "%-2.2s", $data->{'RC State/Province/Territory/Country'};
		printf $o2 "%-10.10s", $data->{'RC Center (RC) Name Abbreviation'};
		printf $o2 "%-1.1s", $data->{'RC Type'};
		printf $o2 "%-50.50s", $data->{'RC Full Name'};
		printf $o2 "%-5.5s", $data->{'Major Vertical Coordinate'};
		printf $o2 "%-5.5s", $data->{'Major Horizontal Coordinate'};
		printf $o2 "%-5.5s", $data->{'Minor Vertical Coordinate'};
		printf $o2 "%-5.5s", $data->{'Minor Horizontal Coordinate'};
		printf $o2 "%-3.3s", $data->{'NPA 1 (Area Code)'};
		printf $o2 "%-3.3s", $data->{'NPA 2'};
		printf $o2 "%-3.3s", $data->{'NPA 3'};
		printf $o2 "%-3.3s", $data->{'NPA 4'};
		printf $o2 "%-3.3s", $data->{'NPA 5'};
		printf $o2 "%-3.3s", $data->{'NPA 6'};
		printf $o2 "%-3.3s", $data->{'NPA 7'};
		printf $o2 "%-3.3s", $data->{'NPA 8'};
		printf $o2 "%-3.3s", $data->{'NPA 9'};
		printf $o2 "%-3.3s", $data->{'NPA 10'};
		printf $o2 "%-2.2s", $data->{'Major Trading Area (MTA) - 1'};
		printf $o2 "%-2.2s", $data->{'Major Trading Area (MTA) - 2'};
		printf $o2 "%-1.1s", $data->{'Split Indicator'};
		printf $o2 "%-3.3s", $data->{'Embedded Overlay NPA 1'};
		printf $o2 "%-3.3s", $data->{'Embedded Overlay NPA 2'};
		printf $o2 "%-3.3s", $data->{'Embedded Overlay NPA 3'};
		printf $o2 "%-3.3s", $data->{'Embedded Overlay NPA 4'};
		printf $o2 "%-10.10s", $data->{'filler'};
		print $o2 "\n";
	}
}

$fn = "LERG8.DAT";
open($o2,">",$fn) or die "can't open $fn";
printnotice($o2);
$fn = "lerg8.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"',join('","',@keys),'"',"\n";
$fn = "rc.csv";
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
		$data->{$fields[$i]} = $token[$i] if $tokens[$i];
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
			for (my $i=0;$i<@vals,$i++) {
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
				printf STDERR "E: %-32.32s %s $flag\n", $key.' '.$src,"'$val'";
			}
		} elsif (@vals > 1) {
			printf STDERR "I: --- --- -\n";
			for (my $i=0;$i<@vals;$i++) {
				printf STDERR "I: %-32.32s %s\n", $key.' '.$srcs[$i],"'$vals[$i]'";
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

