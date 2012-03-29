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

my %mapping = (
	'NPA NXX'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		my ($npa,$nxx,$blk) = split(/-/,$val);
		$data->{npa} = $npa;
		if (defined $nxx) {
			$data->{nxx} = $nxx;
			if (defined $blk) {
				$data->{blk} = $blk;
				$data->{rng} = $blk.'000-'.$blk.'999';
				$data->{lines} = 1000;
			} else {
				$data->{blk} = 'A';
				$data->{rng} = '0000-9999';
				$data->{lines} = 10000;
			}
		}
	},
	'State'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{state} = $val;
	},
	'Company'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{company} = $val;
	},
	'OCN'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{ocn} = $val;
	},
	'Rate Center'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{rate} = $val;
	},
	'Switch'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		return if $val eq 'UNKNOWNCLLI';
		return if $val eq 'NOCLLIKNOWN';
		$data->{clli} = $val;
	},
	'Date'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		return if $val eq '00/00/0000';
		$data->{date} = $val;
	},
	'Prefix Type'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		return if $val eq 'UNKNOWN';
		$data->{category} = $val;
	},
	'Switch Name'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{loc} = $val;
	},
	'Switch Type'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{type} = $val;
	},
	'LATA'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		if ($val =~ /^(.*?)\s+\(([0-9]{3,5})\)$/) {
			$data->{name} = $1;
			$data->{lata} = $2;
		}
	},
	'Tandem'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{tandem} = $val;
	},
);

my $fh = \*INFILE;

foreach my $file (@files) { chomp $file;
	print "processing file $file...\n";
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
					next unless $datum =~ /: +/;
					my ($key,$val) = split(/: +/,$datum);
					&{$mapping{$key}}($data,$val) if exists $mapping{$key};
				}
			} else {
				my $key = $names->[$fno];
				&{$mapping{$key}}($data,$field) if exists $mapping{$key};
			}
		}
		if ($data->{nxx}) {
			$db->{nxx}{$data->{npa}}{$data->{nxx}}{$data->{blk}}{$data->{rng}} = $data;
		}
		if ($data->{npa}) {
			$db->{npa}{$data->{npa}} = {} unless exists $db->{npa}{$data->{npa}};
			my $npa = $db->{npa}{$data->{npa}};
			foreach (qw/state clli ocn lata/) {
				if ($data->{$_}) {
					$npa->{$_}{$data->{$_}} += $data->{lines};
				}
			}
			$npa->{lines} += $data->{lines};
		}
		if ($data->{state}) {
			$db->{state}{$data->{state}} = {} unless exists $db->{state}{$data->{state}};
			my $stat = $db->{state}{$data->{state}};
			$stat->{state} = $data->{state};
			foreach (qw/npa clli ocn lata/) {
				if ($data->{$_}) {
					$stat->{$_}{$data->{$_}} += $data->{lines};
				}
			}
			$stat->{lines} += $data->{lines};
		}
		if ($data->{lata}) {
			$db->{lata}{$data->{lata}} = {} unless exists $db->{lata}{$data->{lata}};
			my $lata = $db->{lata}{$data->{lata}};
			$lata->{lata} = $data->{lata};
			if ($data->{name}) {
				if ($lata->{name} and $lata->{name} ne $data->{name}) {
					warn "LATA name changing from $lata->{name} to $data->{name}";
				}
				$lata->{name} = delete $data->{name};
			}
			foreach (qw/state npa clli ocn/) {
				if ($data->{$_}) {
					$lata->{$_}{$data->{$_}} += $data->{lines};
				}
			}
			$lata->{lines} += $data->{lines};
		}
		if ($data->{clli}) {
			$db->{clli}{$data->{clli}} = {} unless exists $db->{clli}{$data->{clli}};
			my $clli = $db->{clli}{$data->{clli}};
			$clli->{clli} = $data->{clli};
			if ($data->{loc}) {
				if ($clli->{loc} and $clli->{loc} ne $data->{loc}) {
					warn "CLLI location changing from $clli->{loc} to $data->{loc}";
				}
				$clli->{loc} = $data->{loc};
			}
			if ($data->{name}) {
				if ($clli->{name} and $clli->{name} ne $data->{name}) {
					warn "CLLI name changing from $clli->{name} to $data->{name}";
				}
				$clli->{name} = $data->{name};
			}
			if ($data->{type}) {
				if ($clli->{type} and $clli->{type} ne $data->{type}) {
					warn "CLLI type changing from $clli->{type} to $data->{type}";
				}
				$clli->{type} = delete $data->{type};
			}
			if ($data->{tandem}) {
				if ($clli->{tandem} and  $clli->{tandem} ne $data->{tandem}) {
					warn "CLLI tandem changing from $clli->{tandem} to $data->{tandem}";
				}
				$clli->{tandem} = delete $data->{tandem};
			}
			if ($data->{nxx}) {
				$clli->{nxx}{$data->{npa}}{$data->{nxx}}{$data->{blk}}{$data->{rng}} += $data->{lines};
			}
			foreach (qw/state npa lata ocn/) {
				if ($data->{$_}) {
					$clli->{$_}{$data->{$_}} += $data->{lines};
				}
			}
			$clli->{lines} += $data->{lines};
		}
		if ($data->{ocn}) {
			$db->{ocn}{$data->{ocn}} = {} unless exists $db->{ocn}{$data->{ocn}};
			my $ocn = $db->{ocn}{$data->{ocn}};
			$ocn->{ocn} = $data->{ocn};
			if ($data->{company}) {
				if ($ocn->{company} and $ocn->{company} ne $data->{company}) {
					warn "OCN company changing from $ocn->{company} to $data->{company}";
				}
				$ocn->{company} = $data->{company};
			}
			foreach (qw/state lata npa clli/) {
				if ($data->{$_}) {
					$ocn->{$_}{$data->{$_}} += $data->{lines};
				}
			}
			$ocn->{lines} += $data->{lines};
		}
	}
}

sub dumpit {
	my ($file,$hash,$indent) = @_;
	foreach my $key (keys %{$hash}) {
		my $val = $hash->{$key};
		if (ref $val) {
			print $file "$indent'$key'=>{\n";
			dumpit($file,$val,$indent."\t");
			print $file "$indent},\n";
		} else {
			print $file "$indent'$key'=>\"$val\",\n";
		}
	}
}

my $of = \*OUTFILE;

foreach my $part (qw/nxx npa state lata clli ocn/) {
	my $fn = "$datadir/$part.pm";
	print STDERR "writing $fn\n";
	open($of,">","$fn");
	print $of "\$VAR1 = {\n";
		dumpit($of,$db->{$part},"\t");
	print $of "};\n";
	close($of);
}
