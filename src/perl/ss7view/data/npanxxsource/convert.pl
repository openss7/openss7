#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @keys = (
	'NPA',
	'NXX',
	'X',
	'NPA NXX Record',
	'Line From',
	'Line To',
	'Wireless Block',
	'NXX Type',
	'Portable Block',
	'1,000 Block Pooling',
	'Block Contaminated',
	'Block Retained',
	'Date Assigned',
	'Effective Date',
	'Date Activated',
	'Last Modified',
	'Common Name',
	'OCN',
	'OCN Type',
	'Carrier Name',
	'Abbreviation',
	'DBA',
	'FKA',
	'Carrier Address',
	'Carrier City',
	'Carrier State',
	'Carrier Zip',
	'Carrier Country',
	'Wire Center LATA',
	'Other switches in WC',
	'Wire Center Address',
	'Wire Center County',
	'Wire Center City',
	'Class 4/5 Switch',
	'Wire Center State',
	'Trunk Gateway',
	'Point Code',
	'Switch type',
	'Tandem IntraLATA',
	'Tandem Feature Group C',
	'Tandem Feature Group D',
	'Tandem Local',
	'Tandem Feature Group B',
	'Tandem Operator Services',
	'Wire Center Zip',
	'SS7 STP 1',
	'Wire Center Google maps',
	'Wire Center Latitude/Longitude',
	'Wire Center V&H',
	'Wire Center CLLI',
	'Switch',
	'SS7 STP 2',
	'Actual Switch',
	'Wire Center Country',
	'Call Agent',
	'Host',
	'Wire Center Name',
	'LATA',
	'LATA Name',
	'Historical Region',
	'Area Codes in LATA',
	'Remark',
	'Rate Center LATA',
	'Rate Center Name',
	'Rate Center State',
	'Rate Center Country',
	'LERG Abbreviation',
	'Zone',
	'Zone Type',
	'Number Pooling',
	'Point Identifier',
	'Rate Step',
	'Area Codes in Rate Center',
	'Embedded Overlays',
	'Major V&H',
	'Minor V&H',
	'Rate Center Latitude/Longitude',
	'Rate Center Google maps',
	'Time Zone',
	'DST Recognized',
	'Location within Rate Center Type',
	'Location within Rate Center Name',
	'County or Equivalent',
	'Federal Feature ID',
	'FIPS County Code',
	'FIPS Place Code',
	'Land Area',
	'Water Area',
	'Population 2000',
	'Population 2009',
	'Rural-Urban Continuum Code',
	'CBSA',
	'CBSA Level',
	'County Relation to CBSA',
	'Metro Division',
	'Part of CSA',
	'Cellular Market',
	'Major Trading Area',
	'Basic Trading Area',
);

my %mapping = (
	'NPA NXX'=>{
		'NPA NXX'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			if ($val =~ /([0-9]{3})-([0-9]{3})(-([0-9A]))?/) {
				$data->{NPA} = $1;
				$data->{NXX} = $2;
				if ($4) {
					$data->{X} = $4;
				}
			}
		},
		'NPA NXX Record'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
		},
		'Line From'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Line To'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Wireless Block'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'NXX Type'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Portable Block'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'1,000 Block Pooling'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Block Contaminated'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Block Retained'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Date Assigned'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Effective Date'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Date Activated'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Last Modified'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
	},
	'Carrier'=>{
		'Common Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'OCN'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'OCN Type'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val;
		},
		'Abbreviation'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'DBA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'FKA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Address'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val;
		},
		'City'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val;
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val;
		},
		'Zip'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val;
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val;
		},
	},
	'Wire Center'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val;
		},
		'Other switches in WC'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Address'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val;
		},
		'County'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val;
		},
		'City'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val;
		},
		'Operator Services'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Class 4/5 Switch'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val;
		},
		'Trunk Gateway'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Point Code'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Switch type'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Tandems'=>{
			'IntraLATA'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val;
			},
			'Feature Group C'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val;
			},
			'Feature Group D'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val;
			},
			'Local'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val;
			},
			'Feature Group B'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val;
			},
			'Operator Services'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val;
			},
		},
		'Zip'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val;
		},
		'SS7 STP 1'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Coordinates'=>{
			'Google maps'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Wire Center '.$fld;
				$data->{$fld} = $val;
			},
			'Latitude/Longitude'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Wire Center '.$fld;
				$data->{$fld} = $val;
			},
			'V&H'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Wire Center '.$fld;
				$data->{$fld} = $val;
			},
		},
		'Wire Center CLLI'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Switch'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'SS7 STP 2'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Actual Switch'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val;
		},
		'Call Agent'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Host'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Wire Center Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
	},
	'LATA'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'LATA '.$fld;
			$data->{$fld} = $val;
		},
		'Historical Region'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Area Codes in LATA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Remark'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
	},
	'Rate Center - Locality'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Rate Center '.$fld;
			$data->{$fld} = $val;
		},
		'Rate Center Name'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Rate Center '.$fld;
			$data->{$fld} = $val;
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$fld = 'Rate Center '.$fld;
			$data->{$fld} = $val;
		},
		'LERG Abbreviation'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Zone'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Zone Type'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Number Pooling'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Point Identifier'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		}, # Canada only
		'Rate Step'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Area Codes in Rate Center'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Embedded Overlays'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Coordinates'=>{
			'Major V&H'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
			'Minor V&H'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
			'Latitude/Longitude'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Rate Center '.$fld;
				$data->{$fld} = $val;
			},
			'Google maps'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Rate Center '.$fld;
				$data->{$fld} = $val;
			},
		},
		'Time Zone'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'DST Recognized'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Location within Rate Center'=>{
			'Type'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Location within Rate Center '.$fld;
				$data->{$fld} = $val;
			},
			'Name'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$fld = 'Location within Rate Center '.$fld;
				$data->{$fld} = $val;
			},
		},
		'County or Equivalent'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Federal Feature ID'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'FIPS County Code'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'FIPS Place Code'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Land Area'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Water Area'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Latitude/Longitude'=>sub{
		},
		'Population 2000'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Population 2009'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Rural-Urban Continuum Code'=>sub{
			my ($data,$val,$fld) = @_;
			return unless $val;
			$data->{$fld} = $val;
		},
		'Core Based Statistical Area'=>{
			'CBSA'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
			'CBSA Level'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
			'County Relation to CBSA'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
			'Metro Division'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
			'Part of CSA'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
		},
		'Market Area'=>{
			'Cellular Market'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
			'Major Trading Area'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
			'Basic Trading Area'=>sub{
				my ($data,$val,$fld) = @_;
				return unless $val;
				$data->{$fld} = $val;
			},
		},
	},
);

$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","', @keys), '"', "\n";
my @files = `find $datadir -name '*.html.xz' | sort`;
foreach $fn (@files) { chomp $fn;
	print STDERR "I: processing $fn\n";
	open($fh,"xzcat $fn |") or die "can't open $fn";
	my $entry = '';
	while (<$fh>) { chomp; s/\r//g;
		$entry .= $_;
	}
	close($fh);
	my $data = {};
	my ($hdr,$key,$val);
	while ($entry =~ /(<h3>(.*?)<\/h3>|<div class="n30">(.*?)<\/div>[[:space:]]*<div class="n70">(.*?)<\/div>|<div class="n100">(&nbsp;)<\/div>)/g) {
		if ($2) {
			my $tit = $2;
			my $npa;
			if ($tit =~ /^NPA NXX ([0-9]+-[0-9]+)$/) {
				$tit = 'NPA NXX';
				$npa = $1;
			}
			$key = $tit;
			$hdr = undef;
			if ($npa) {
				if (exists $mapping{$key}{$tit}) {
					#print STDERR "D:(699) mapping '$key', '$tit', '$npa'\n";
					&{$mapping{$key}{$tit}}($data,$npa,$tit);
				} else {
					print STDERR "E: no mapping for '$key', '$tit'\n";
				}
			}
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
					$hdr = $1;
				} elsif ($val =~ /<a href/) {
					$fld = 'Google maps';
				} else {
					$fld = 'Remark';
				}
			}
			unless ($fld eq '&nbsp;') {
				$val = "$sub $val" if $sub;
				if ($hdr) {
					if (exists $mapping{$key}{$hdr}{$fld}) {
						#print STDERR "D:(742) mapping '$key', '$hdr', '$fld', '$val'\n";
						&{$mapping{$key}{$hdr}{$fld}}($data,$val,$fld);
					} else {
						print STDERR "E: no mapping for '$key', '$hdr', '$fld'\n";
					}
				} else {
					if (exists $mapping{$key}{$fld}) {
						#print STDERR "D:(749) mapping '$key', '$fld', '$val'\n";
						&{$mapping{$key}{$fld}}($data,$val,$fld);
					} else {
						print STDERR "E: no mapping for '$key', '$fld'\n";
					}
				}
			}
		}
	}
	if (exists $data->{NPA} and exists $data->{NXX}) {
		my @values = ();
		foreach (@keys) { push @values, $data->{$_}; }
		print $of '"', join('","', @values), '"', "\n";
	}
}
close($of);

exit;

1;

__END__