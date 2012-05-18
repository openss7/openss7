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
use Time::gmtime;
use File::stat;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @keys = (
	'NPA',
	'NXX',
	'X',
	'XXXX',
	'YYYY',
	'NPA NXX Record',
	'Line From',
	'Line To',
	'Numbers From',
	'Numbers To',
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
	'File Updated',
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
			$data->{$fld} = $val if length($val);
		},
		'Line From'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
			$data->{XXXX} = substr($val,8,4) if length($val) >= 12;
		},
		'Line To'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
			$data->{YYYY} = substr($val,8,4) if length($val) >= 12;
			$data->{X} = substr($data->{XXXX},0,1) unless length($data->{X});
			if ($data->{XXXX} eq '0000' and $data->{YYYY} eq '9999') {
				delete $data->{X};
				delete $data->{XXXX};
				delete $data->{YYYY};
			} elsif ($data->{XXXX} eq "$data->{X}000" and $data->{YYYY} eq "$data->{X}999") {
				delete $data->{XXXX};
				delete $data->{YYYY};
			}
		},
		'Numbers From'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
			$data->{XXXX} = substr($val,8,4) if length($val) >= 12;
		},
		'Numbers To'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
			$data->{YYYY} = substr($val,8,4) if length($val) >= 12;
			$data->{X} = substr($data->{XXXX},0,1) unless length($data->{X});
			if ($data->{XXXX} eq '0000' and $data->{YYYY} eq '9999') {
				delete $data->{X};
				delete $data->{XXXX};
				delete $data->{YYYY};
			} elsif ($data->{XXXX} eq "$data->{X}000" and $data->{YYYY} eq "$data->{X}999") {
				delete $data->{XXXX};
				delete $data->{YYYY};
			}
		},
		'Wireless Block'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'NXX Type'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Portable Block'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'1,000 Block Pooling'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Block Contaminated'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Block Retained'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Date Assigned'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Effective Date'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Date Activated'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Last Modified'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
	},
	'Carrier'=>{
		'Common Name'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'OCN'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'OCN Type'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Name'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Abbreviation'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'DBA'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'FKA'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Address'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'City'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Zip'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Carrier '.$fld;
			$data->{$fld} = $val if length($val);
		},
	},
	'Wire Center'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Other switches in WC'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Address'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'County'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'City'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Operator Services'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Class 4/5 Switch'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Trunk Gateway'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Point Code'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Switch type'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Tandems'=>{
			'IntraLATA'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'Feature Group C'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'Feature Group D'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'Local'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'Feature Group B'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'Operator Services'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Tandem '.$fld;
				$data->{$fld} = $val if length($val);
			},
		},
		'Zip'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'SS7 STP 1'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Coordinates'=>{
			'Google maps'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Wire Center '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'Latitude/Longitude'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Wire Center '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'V&H'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Wire Center '.$fld;
				$data->{$fld} = $val if length($val);
			},
		},
		'Wire Center CLLI'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Switch'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'SS7 STP 2'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Actual Switch'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Wire Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Call Agent'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Host'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Wire Center Name'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
	},
	'LATA'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Name'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'LATA '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Historical Region'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Area Codes in LATA'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Remark'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
	},
	'Rate Center - Locality'=>{
		'LATA'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Rate Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Rate Center Name'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'State'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Rate Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'Country'=>sub{
			my ($data,$val,$fld) = @_;
			$fld = 'Rate Center '.$fld;
			$data->{$fld} = $val if length($val);
		},
		'LERG Abbreviation'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Zone'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Zone Type'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Number Pooling'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Point Identifier'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		}, # Canada only
		'Rate Step'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Area Codes in Rate Center'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Embedded Overlays'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Coordinates'=>{
			'Major V&H'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
			'Minor V&H'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
			'Latitude/Longitude'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Rate Center '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'Google maps'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Rate Center '.$fld;
				$data->{$fld} = $val if length($val);
			},
		},
		'Time Zone'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'DST Recognized'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Location within Rate Center'=>{
			'Type'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Location within Rate Center '.$fld;
				$data->{$fld} = $val if length($val);
			},
			'Name'=>sub{
				my ($data,$val,$fld) = @_;
				$fld = 'Location within Rate Center '.$fld;
				$data->{$fld} = $val if length($val);
			},
		},
		'County or Equivalent'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Federal Feature ID'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'FIPS County Code'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'FIPS Place Code'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Land Area'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Water Area'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Latitude/Longitude'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Population 2000'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Population 2009'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Rural-Urban Continuum Code'=>sub{
			my ($data,$val,$fld) = @_;
			$data->{$fld} = $val if length($val);
		},
		'Core Based Statistical Area'=>{
			'CBSA'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
			'CBSA Level'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
			'County Relation to CBSA'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
			'Metro Division'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
			'Part of CSA'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
		},
		'Market Area'=>{
			'Cellular Market'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
			'Major Trading Area'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
			},
			'Basic Trading Area'=>sub{
				my ($data,$val,$fld) = @_;
				$data->{$fld} = $val if length($val);
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
	$data->{'File Updated'} = stat($fn)->mtime;
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
						print STDERR "E: no mapping for '$key', '$hdr', '$fld', val='$val'\n";
					}
				} else {
					if (exists $mapping{$key}{$fld}) {
						#print STDERR "D:(749) mapping '$key', '$fld', '$val'\n";
						&{$mapping{$key}{$fld}}($data,$val,$fld);
					} else {
						print STDERR "E: no mapping for '$key', '$fld' val='$val'\n";
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
