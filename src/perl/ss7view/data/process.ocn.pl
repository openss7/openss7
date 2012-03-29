#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $fn;

my %merges = (
	'company'=>[
		'areacodes:COMPANY',
		'localcallingguide:COMPANY-NAME',
		'telcodata:Company',
		'npanxxsource:Carrier Name',
		'neca4:companyname',
		'neca4:company',
		'nanpa:Company',
	],
	'type'=>[
		'localcallingguide:COMPANY-TYPE',
		'npanxxsource:OCN Type',
	],
);

my %mapping = (
	'company'=>{
		'localcallingguide:COMPANY-NAME'=>sub{
			my $val = shift;
			$val =~ s/\&amp;/\&/g;
			return $val;
		},
#		'npanxxsource:Carrier Name'=>sub{
#			my $val = shift;
#			$val =~ s/\s+-\s+[A-Z]{2}\s*$//;
#			return $val;
#		},
	},
	'type'=>{
		'localcallingguide:COMPANY-TYPE'=>sub{
			my $val = shift;
			$val =~ s/^I$/ILEC/;
			$val =~ s/^C$/CLEC/;
			$val =~ s/^W$/WIRELESS/;
			return $val;
		},
	},
);

my %mismatches = ();
my %matches = ();

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
}
close($fh);

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

