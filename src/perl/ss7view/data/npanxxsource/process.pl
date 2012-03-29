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

require "$codedir/database.pm";

my $fh = \*INFILE;
my $fn;

my %mapping = (
	'NPA NXX'=>{
		'NPA NXX'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			if ($val =~ /([0-9]{3})-([0-9]{3})/) {
				$data->{npa} = $1;
				$data->{nxx} = $2;
			} else {
				print STDERR "E: can't grok NPA NXX: '$val'\n";
			}
		},
		'NPA NXX Record'=>sub{
			my ($data,$val) = @_;
		},
		'Line From'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			if ($val =~ /[0-9]{3} [0-9]{3}-([0-9]{4})/) {
				my $f = $data->{linesfrom} = $1;
				my $t = $data->{linesto} if exists $data->{linesto};
				my $blk = substr($f,0,1);
				$t = int($blk.'999') unless defined $t;
				$blk = 'A' if $f eq '0000' and $t = '9999';
				$data->{blk} = $blk;
				$data->{rng} = sprintf("%04d-%04d",$f,$t);
				$data->{lines} = $t - $f + 1;
			} else {
				print STDERR "E: can't grok Line From: '$val'\n";
			}
		},
		'Line To'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			if ($val =~ /[0-9]{3} [0-9]{3}-([0-9]{4})/) {
				my $t = $data->{linesto} = $1;
				my $f = $data->{linesfrom} if exists $data->{linesfrom};
				$f = '0000' unless defined $f;
				my $blk = substr($f,0,1) if defined $f;
				$blk = 'A' if $f eq '0000' and $t = '9999';
				$data->{blk} = $blk if defined $blk;
				$data->{rng} = sprintf("%04d-%04d",$f,$t);
				$data->{lines} = $t - $f + 1;
			} else {
				print STDERR "E: can't grok Line To: '$val'\n";
			}
		},
		'Wireless Block'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			if ($val eq 'Yes') {
				$data->{use} = 'W';
			} elsif ($val eq 'No') {
				$data->{use} = 'L';
			} else {
				print STDERR "E: can't grok Wireless Block: '$val'\n";
			}
		},
		'NXX Type'=>sub{
			my ($data,$val) = @_;
		},
		'Portable Block'=>sub{
			my ($data,$val) = @_;
		},
		'1,000 Block Pooling'=>sub{
			my ($data,$val) = @_;
		},
		'Block Contaminated'=>sub{
			my ($data,$val) = @_;
		},
		'Block Retained'=>sub{
			my ($data,$val) = @_;
		},
		'Date Assigned'=>sub{
			my ($data,$val) = @_;
		},
		'Effective Date'=>sub{
			my ($data,$val) = @_;
		},
		'Date Activated'=>sub{
			my ($data,$val) = @_;
		},
		'Last Modified'=>sub{
			my ($data,$val) = @_;
		},
	},
	'Carrier'=>{
		'Common Name'=>sub{
			my ($data,$val) = @_;
		},
		'OCN'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{ocn} = $val;
		},
		'OCN Type'=>sub{
			my ($data,$val) = @_;
		},
		'Name'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{company} = $val;
		},
		'Abbreviation'=>sub{
			my ($data,$val) = @_;
		},
		'DBA'=>sub{
			my ($data,$val) = @_;
		},
		'FKA'=>sub{
			my ($data,$val) = @_;
		},
		'Address'=>sub{
			my ($data,$val) = @_;
		},
		'City'=>sub{
			my ($data,$val) = @_;
		},
		'State'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{ocn_state} = $val;
		},
		'Zip'=>sub{
			my ($data,$val) = @_;
		},
		'Country'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{ocn_country} = $val;
		},
	},
	'Wire Center'=>{
		'Other switches in WC'=>sub{
			my ($data,$val) = @_;
		},
		'Address'=>sub{
			my ($data,$val) = @_;
		},
		'County'=>sub{
			my ($data,$val) = @_;
		},
		'Feature Group B'=>sub{
			my ($data,$val) = @_;
		},
		'City'=>sub{
			my ($data,$val) = @_;
		},
		'Operator Services'=>sub{
			my ($data,$val) = @_;
		},
		'Class 4/5 Switch'=>sub{
			my ($data,$val) = @_;
		},
		'State'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{wc_state} = $val;
		},
		'Trunk Gateway'=>sub{
			my ($data,$val) = @_;
		},
		'Point Code'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{pcode} = $val;
		},
		'Switch type'=>sub{
			my ($data,$val) = @_;
		},
		'Feature Group D'=>sub{
			my ($data,$val) = @_;
		},
		'Tandems'=>{
			'IntraLATA'=>sub{
				my ($data,$val) = @_;
			},
			'Feature Group C'=>sub{
				my ($data,$val) = @_;
			},
			'Feature Group D'=>sub{
				my ($data,$val) = @_;
				return unless $val;
				$data->{tandem} = $val;
			},
			'Local'=>sub{
				my ($data,$val) = @_;
			},
			'Feature Group B'=>sub{
				my ($data,$val) = @_;
			},
			'Operator Services'=>sub{
				my ($data,$val) = @_;
			},
		},
		'Zip'=>sub{
			my ($data,$val) = @_;
		},
		'SS7 STP 1'=>sub{
			my ($data,$val) = @_;
		},
		'Coordinates'=>{
			'Google maps'=>sub{
				my ($data,$val) = @_;
			},
			'Latitude/Longitude'=>sub{
				my ($data,$val) = @_;
			},
			'V&H'=>sub{
				my ($data,$val) = @_;
				return unless $val;
				my ($v,$h) = split(/,/,$val);
				if ($v and $h) {
					$data->{wcv} = $v;
					$data->{wch} = $h;
					$data->{wcvh} = $val;
				} else {
					print STDERR "E: can't grok V&H: '$val'\n";
				}
			},
		},
		'IntraLATA'=>sub{
			my ($data,$val) = @_;
		},
		'Wire Center CLLI'=>sub{
			my ($data,$val) = @_;
		},
		'Switch'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			return if $val eq 'XXXXXXXXXXX';
			$data->{clli} = $val;
		},
		'SS7 STP 2'=>sub{
			my ($data,$val) = @_;
		},
		'Actual Switch'=>sub{
			my ($data,$val) = @_;
		},
		'Country'=>sub{
			my ($data,$val) = @_;
		},
		'Call Agent'=>sub{
			my ($data,$val) = @_;
		},
		'Host'=>sub{
			my ($data,$val) = @_;
		},
		'Wire Center Name'=>sub{
			my ($data,$val) = @_;
			return unless $val;
		},
	},
	'LATA'=>{
		'LATA'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			return if $val eq '99999';
			$data->{lata} = $val;
		},
		'Name'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{name} = $val;
		},
		'Historical Region'=>sub{
			my ($data,$val) = @_;
		},
		'Area Codes in LATA'=>sub{
			my ($data,$val) = @_;
		},
		'Remark'=>sub{
			my ($data,$val) = @_;
		},
	},
	'Rate Center - Locality'=>{
		'Rate Center Name'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{rate} = $val;
		},
		'State'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{state} = $val;
		},
		'Country'=>sub{
			my ($data,$val) = @_;
		},
		'LERG Abbreviation'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{abbr} = $val;
		},
		'Zone'=>sub{
			my ($data,$val) = @_;
		},
		'Zone Type'=>sub{
			my ($data,$val) = @_;
		},
		'Number Pooling'=>sub{
			my ($data,$val) = @_;
		},
		'Point Identifier'=>sub{
			my ($data,$val) = @_;
		}, # Canada only
		'Rate Step'=>sub{
			my ($data,$val) = @_;
		},
		'Area Codes in Rate Center'=>sub{
			my ($data,$val) = @_;
		},
		'Embedded Overlays'=>sub{
			my ($data,$val) = @_;
		},
		'Coordinates'=>{
			'Major V&H'=>sub{
				my ($data,$val) = @_;
				return unless $val;
				my ($v,$h) = split(/,/,$val);
				if ($v and $h) {
					$data->{rcmjv} = $v;
					$data->{rcmjh} = $h;
				} else {
					print STDERR "E: can't grok Major V&H: '$val'\n";
				}
			},
			'Minor V&H'=>sub{
				my ($data,$val) = @_;
				return unless $val;
				my ($v,$h) = split(/,/,$val);
				if ($v and $h) {
					$data->{rcmnv} = $v;
					$data->{rcmnh} = $h;
				} else {
					print STDERR "E: can't grok Minor V&H: '$val'\n";
				}
			},
			'Latitude/Longitude'=>sub{
				my ($data,$val) = @_;
			},
			'Google maps'=>sub{
				my ($data,$val) = @_;
			},
		},
		'Time Zone'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			return if $val eq 'X';
			return if $val eq 'Not Applicable';
			if ($val =~ /UTC\s*([-+ ])\s*([0-9]+)/) {
				$data->{tz} = $2;
				$data->{tz} = 0 - $data->{tz} if $1 eq '-';
			} else {
				print STDERR "E: can't grok Time Zone: '$val'\n";
			}
		},
		'DST Recognized'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			if ($val eq 'Y') {
				$data->{dst} = 1;
			} elsif ($val eq 'N') {
				delete $data->{dst};
			} else {
				print STDERR "E: can't grok Time Zone: '$val'\n";
			}
		},
		'Location within Rate Center'=>{
			'Type'=>sub{
				my ($data,$val) = @_;
			},
			'Name'=>sub{
				my ($data,$val) = @_;
			},
		},
		'County or Equivalent'=>sub{
			my ($data,$val) = @_;
			return unless $val;
			$data->{county} = $val;
		},
		'Federal Feature ID'=>sub{
			my ($data,$val) = @_;
		},
		'FIPS County Code'=>sub{
			my ($data,$val) = @_;
		},
		'FIPS Place Code'=>sub{
			my ($data,$val) = @_;
		},
		'Land Area'=>sub{
			my ($data,$val) = @_;
		},
		'Water Area'=>sub{
			my ($data,$val) = @_;
		},
		'Latitude/Longitude'=>sub{
			my ($data,$val) = @_;
		},
		'Population 2000'=>sub{
			my ($data,$val) = @_;
		},
		'Population 2009'=>sub{
			my ($data,$val) = @_;
		},
		'Rural-Urban Continuum Code'=>sub{
			my ($data,$val) = @_;
		},
		'Core Based Statistical Area'=>{
			'CBSA'=>sub{
				my ($data,$val) = @_;
			},
			'CBSA Level'=>sub{
				my ($data,$val) = @_;
			},
			'County Relation to CBSA'=>sub{
				my ($data,$val) = @_;
			},
			'Metro Division'=>sub{
				my ($data,$val) = @_;
			},
			'Part of CSA'=>sub{
				my ($data,$val) = @_;
			},
		},
		'Market Area'=>{
			'Cellular Market'=>sub{
				my ($data,$val) = @_;
				return unless $val;
				if ($val =~ /^RSA([0-9]+)/) {
					$data->{rsa} = $1;
				} elsif ($val =~ /^MSA([0-9]+)/) {
					$data->{msa} = $1;
				} else {
					print STDERR "E: can't grok Cellular Market: '$val'\n";
				}
			},
			'Major Trading Area'=>sub{
				my ($data,$val) = @_;
				return unless $val;
				if ($val =~ /^MTA([0-9]+)/) {
					$data->{mta} = $1;
				} else {
					print STDERR "E: can't grok Major Trading Area: '$val'\n";
				}
			},
			'Basic Trading Area'=>sub{
				my ($data,$val) = @_;
				return unless $val;
				if ($val =~ /^BTA([0-9]+)/) {
					$data->{bta} = $1;
				} else {
					print STDERR "E: can't grok Basic Trading Area: '$val'\n";
				}
			},
		},
	},
);

my @files = `find $datadir -name '*.html.xz' | sort`;

foreach my $file (@files) { chomp $file;
	print STDERR "processing $file\n";
	my @lines = `xzcat $file`;
	my $entry = join('',@lines);
	my $data = {};
	my ($key,$hdr);
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
					&{$mapping{$key}{$tit}}($data,$npa);
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
						&{$mapping{$key}{$hdr}{$fld}}($data,$val);
					} else {
						print STDERR "E: no mapping for '$key', '$hdr', '$fld'\n";
					}
				} else {
					if (exists $mapping{$key}{$fld}) {
						&{$mapping{$key}{$fld}}($data,$val);
					} else {
						print STDERR "E: no mapping for '$key', '$fld'\n";
					}
				}
			}
		}
	}
	closerecord($data);
}

dumpem($datadir);
