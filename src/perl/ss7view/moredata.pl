#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $codedir = "$progdir/data";
my $datadir = "$codedir/telcodata";

use strict;
use Data::Dumper;
use Encode qw(encode decode);

my @files = `find $datadir -name '*.html.xz'`;

my $db = {};

my $names = [
	'dummy',
	'NPA NXX',
	'State',
	'Company',
	'OCN',
	'Rate Center',
	'Switch',
	'Date',
	'Prefix Type',
	'Switch Name',
	'Switch Type',
	'LATA',
	'Tandem',
];

my $fh = \*INFILE;

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

my $nums = {};

my %labels = ();

#print "\$var = {\n";
foreach my $file (@files) { chomp $file;
	print STDERR "processing file $file...\n";
	my @lines = `xzcat $file`;
	foreach my $line (@lines) { chomp $line;
		next unless $line =~ /<tr class="results"/;
		$line =~ /<tr class="results">(.*?)<\/tr>/;
		$line = $1;
		my $data = {};
		my $fno = 0;
		while ($line =~ /<td.*?>(.*?)<\/td>/g) {
			my $field = $1; $fno++;
			$field =~ s/<a href=.*?>//g;
			$field =~ s/<\/a>//g;
			if ($field =~ /<br>/) {
				my @fields = split(/<br>/,$field);
				foreach my $datum (@fields) {
					next unless $datum =~ /: /;
					my ($key,$val) = split(/: /,$datum);
					$data->{$key} = $val;
					#printf "\t\t'%s'=>'%s',\n", $key, $val;
					$labels{$key}++;
				}
			} else {
				my $key = $names->[$fno];
				$data->{$key} = $field;
				$labels{$key}++;
				if ($fno == 1) {
					#printf "\t'%s'=>{\n", $field;
				}
				#printf "\t\t'%s'=>'%s',\n", $key, $field;
			}
		}
		#print "\t},\n";
		my ($npa,$nxx,$blk) = split(/-/, $data->{'NPA NXX'});
		if ($npa and $nxx) {
			$blk = 'A' unless defined $blk;
			warn "record exists for NPA NXX $data->{'NPA NXX'}"
				if exists $db->{nxx}{$npa}{$nxx}{$blk};
			$db->{nxx}{$npa}{$nxx}{$blk} = $data;
		}
		my $rate  = $data->{'Rate Center'};
		my $clli  = $data->{'Switch'};
		my $city  = substr($clli,0,6);
		my $wire  = substr($clli,0,8);
		my $state = $data->{'State'};
		my $ocn   = $data->{'OCN'};
		my $lata  = $data->{'LATA'};

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
			my $cp = $data->{'Company'};
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
			$db->{ocn}{$ocn}{'NPA NXXs'}{$npa}{$nxx} = 1;
			if (exists $db->{ocn}{$ocn}{Company} and $db->{ocn}{$ocn}{Company} ne $cp) {
				warn "$ocn $npa-$nxx company changed from $db->{ocn}{$ocn}{Company} to $cp";
				$db->{ocn}{$ocn}{LKA} = 'X';
			}
			$db->{ocn}{$ocn}{'Company'} = $cp;
			$db->{ocn}{$ocn}{'States'}{$state}++;
		}
		if ($lata) {
			$lata =~ /^(.*?)\(([0-9]+)\)$/;
			my ($name,$numb) = ($1,$2);
			next unless $name and $numb;
			$name =~ s/  *$//;
			$name =~ s/^  *//;
			$db->{lata}{$numb}{'Name'} = $name;
			$db->{lata}{$numb}{'State'} = $data->{'State'};
			$db->{lata}{$numb}{'Area Codes in LATA'}{$npa}{$nxx} = 1;
		}
	}
}
#print "};\n";

my $npas = 0;
my $nxxs = 0;
my $blks = 0;
foreach my $npa (sort keys %{$db->{nxx}}) {
	foreach my $nxx (sort keys %{$db->{nxx}{$npa}}) {
		foreach my $blk (sort keys %{$db->{nxx}{$npa}{$nxx}}) {
			next if $blk eq 'A';
			$blks++;
		}
		$nxxs++;
	}
	$npas++;
}

my $latas = 0;
my $latanpas = 0;
foreach my $lata (sort keys %{$db->{lata}}) {
	foreach my $npa (sort keys %{$db->{lata}{$lata}{'Area Codes in LATA'}}) {
		$latanpas++;
	}
	$latas++;
}
my $states = 0;
my $statenpas = 0;
foreach my $state (sort keys %{$db->{state}}) {
	foreach my $npa (sort keys %{$db->{state}{$state}{'NPA NXXs'}}) {
		$statenpas++;
	}
	$states++;
}

my $rates = 0;
foreach my $state (sort keys %{$db->{rate}}) {
	foreach my $rate (sort keys %{$db->{rate}{$state}}) {
		$rates++;
	}
}
my $cllis = 0;
foreach my $clli (sort keys %{$db->{clli}}) {
	$cllis++;
}
my $wires = 0;
foreach my $wire (sort keys %{$db->{wire}}) {
	$wires++;
}
my $citys = 0;
foreach my $city (sort keys %{$db->{city}}) {
	$citys++;
}
my $ocns = 0;
foreach my $ocn (sort keys %{$db->{ocn}}) {
	$ocns++;
}

print STDERR "there are $states State records\n";
print STDERR "there are $latas LATA records\n";
print STDERR "there are $npas NPA records\n";
print STDERR "there are $statenpas State-NPA records\n";
print STDERR "there are $latanpas LATA-NPA records\n";
print STDERR "there are $ocns OCNs\n";
print STDERR "there are $citys Cities\n";
print STDERR "there are $rates rate centers\n";
print STDERR "there are $wires Wire Centers\n";
print STDERR "there are $cllis CLLIs\n";
print STDERR "there are $nxxs NPA NXX records\n";
print STDERR "there are $blks 1000's block records\n";

foreach my $label (sort keys %labels) {
	printf STDERR "'%s' = %d\n", $label, $labels{$label};
}

my $of = \*OUTFILE;
my $o2 = \*OUTFILE2;

my $npadata = {};
my $nxxdata = {};

open($of,">","$datadir/lata.txt");
foreach my $lata (sort keys %{$db->{lata}}) {
	printf $of "%s\t%s\t%s\t%s\n", $lata, $db->{lata}{$lata}{'Name'},
		$db->{lata}{$lata}{'State'},
		join(',',sort keys %{$db->{lata}{$lata}{'Area Codes in LATA'}})
		;
}
close($of);

open($of,">","$datadir/npanxx.txt");
open($o2,">","$datadir/npa.txt");
foreach my $npa (sort keys %{$db->{nxx}}) {
	foreach my $nxx (sort keys %{$db->{nxx}{$npa}}) {
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
	foreach my $npa (sort keys %{$db->{lata}{$lata}{'Area Codes in LATA'}}) {
		foreach my $nxx (sort keys %{$db->{lata}{$lata}{'Area Codes in LATA'}{$npa}}) {
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
				printf $of "%s\t%s\t%s\n", $rate, $npa, $nxx;
				$nxxdata->{$npa}{$nxx}{rate} = $rate;
			}
			printf $o2 "%s\t%s\n", $rate, $npa;
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

#my $dumper = Data::Dumper->new([$db]);
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
close($of);
