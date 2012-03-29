#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $fn;

my %merges = (
	'lata'=>[
		'areacodes:LATA',
		'telcodata:LATA',
		'npanxxsource:Wire Center LATA',
		'npanxxsource:LATA',
		'npanxxsource:Rate Center LATA',
		'localcallingguide:LATA',
		'neca4:lata',
	],
	'ocn'=>[
		'areacodes:OCN',
		'telcodata:OCN',
		'npanxxsource:OCN',
		'localcallingguide:OCN',
		'nanpa:OCN',
		'neca4:ocn',
	],
	'sw-clli'=>[
		'areacodes:SWITCH_CLLI',
		'pinglo:Switch',
		'telcodata:Switch',
		'npanxxsource:Switch',
		'localcallingguide:SWITCH',
		'neca4:clli',
	],
	'wc-clli'=>[
		'areacodes:SWITCH_CLLI',
		'pinglo:Switch',
		'telcodata:Switch',
		'npanxxsource:Switch',
		'npanxxsource:Wire Center CLLI',
		'localcallingguide:SWITCH',
		'neca4:clli',
	],
	'rc-clli'=>[
		'areacodes:SWITCH_CLLI',
		'pinglo:Switch',
		'telcodata:Switch',
		'npanxxsource:Switch',
		'npanxxsource:Wire Center CLLI',
		'localcallingguide:SWITCH',
		'neca4:clli',
	],
);

my %mapping = (
	'lata'=>{
		'telcodata:LATA'=>sub{
			my $val = shift;
			return undef if $val eq 'N/A';
			if ($val =~ /\(([0-9]+)\)/) {
				$val = $1;
			}
			return $val;
		},
	},
	'sw-clli'=>{
		'pinglo:Switch'=>sub{
			my $val = shift;
			return undef if $val eq 'XXXXXXXXXXX';
			return $val;
		},
		'telcodata:Switch'=>sub{
			my $val = shift;
			return undef if $val eq 'NOCLLIKNOWN';
			return $val;
		},
	},
	'wc-clli'=>{
		'pinglo:Switch'=>sub{
			my $val = shift;
			return undef if $val eq 'XXXXXXXXXXX';
			return substr($val,0,8);
		},
		'areacodes:SWITCH_CLLI'=>sub{
			my $val = shift;
			return substr($val,0,8);
		},
		'telcodata:Switch'=>sub{
			my $val = shift;
			return undef if $val eq 'NOCLLIKNOWN';
			return substr($val,0,8);
		},
		'npanxxsource:Switch'=>sub{
			my $val = shift;
			return substr($val,0,8);
		},
		'localcallingguide:SWITCH'=>sub{
			my $val = shift;
			return substr($val,0,8);
		},
		'neca4:clli'=>sub{
			my $val = shift;
			return substr($val,0,8);
		},
	},
	'rc-clli'=>{
		'pinglo:Switch'=>sub{
			my $val = shift;
			return undef if $val eq 'XXXXXXXXXXX';
			return substr($val,0,6);
		},
		'areacodes:SWITCH_CLLI'=>sub{
			my $val = shift;
			return substr($val,0,6);
		},
		'telcodata:Switch'=>sub{
			my $val = shift;
			return undef if $val eq 'NOCLLIKNOWN';
			return substr($val,0,6);
		},
		'npanxxsource:Switch'=>sub{
			my $val = shift;
			return substr($val,0,6);
		},
		'npanxxsource:Wire Center CLLI'=>sub{
			my $val = shift;
			return substr($val,0,6);
		},
		'localcallingguide:SWITCH'=>sub{
			my $val = shift;
			return substr($val,0,6);
		},
		'neca4:clli'=>sub{
			my $val = shift;
			return substr($val,0,6);
		},
	},
);

my %mismatches = ();
my %matches = ();

$fn = "db.csv";
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
				if ($data->{$key}) {
					if ($data->{$key} ne $val) {
						$mismatch++;
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
				if ($data->{$key} eq $vals[$i]) {
					$flag = '';
					$matches{$key}{$srcs[$i]}++;
				} else {
					$flag = '****';
					$mismatches{$key}{$srcs[$i]}++;
				}
				printf STDERR "E: %-3.3s %-3.3s %-1.1s %-32.32s %s $flag\n", $data->{NPA}, $data->{NXX}, $data->{X}, $key.' '.$srcs[$i],$vals[$i];
			}
		} elsif (@vals > 1) {
			printf STDERR "I: --- --- -\n";
			for (my $i=0;$i<@vals;$i++) {
				printf STDERR "I: %-3.3s %-3.3s %-1.1s %-32.32s %s\n", $data->{NPA}, $data->{NXX}, $data->{X}, $key.' '.$srcs[$i],$vals[$i];
				$matches{$key}{$srcs[$i]}++;
			}
		}
	}
}
close($fh);

foreach my $k (keys %mismatches) {
	foreach my $s (keys %{$mismatches{$k}}) {
		printf STDERR "I: %9d mismatches on %-10.10s by %s\n", $mismatches{$k}{$s}, $k, $s;
	}
}
foreach my $k (keys %matches) {
	foreach my $s (keys %{$matches{$k}}) {
		printf STDERR "I: %9d    matches on %-10.10s by %s\n", $matches{$k}{$s}, $k, $s;
	}
}

