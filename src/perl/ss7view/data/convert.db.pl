#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @files = `ls */db.csv`;

my @keys = (
	'NPA',
	'NXX',
	'X',
);
my %db = ();

foreach $fn (@files) { chomp $fn;
	$fn =~ /(.*?)\//; my $dir = $1;
	my @fields = ();
	print STDERR "I: processing $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	my $header = 1;
	while (<$fh>) { chomp;
		s/^"//; s/"$//;
		my @tokens = split(/","/,$_);
		if ($header) {
			foreach my $l (@tokens) {
				if ($l eq 'NPA' or $l eq 'NXX' or $l eq 'X') {
					push @fields, $l;
					push @keys, $dir if $l eq 'X';
					next;
				}
				push @fields, "$dir:$l";
				push @keys, "$dir:$l";
			}
			$header = undef;
			next;
		}
		my $npa = $tokens[0];
		my $nxx = $tokens[1];
		my $x   = $tokens[2];
		$db{$npa}{$nxx}{$x} = {NPA=>$npa,NXX=>$nxx,X=>$x}
			unless exists $db{$npa}{$nxx}{$x};
		my $data = $db{$npa}{$nxx}{$x};
		$data->{$dir} = 'X';
		for (my $i=3;$i<@fields;$i++) {
			$data->{$fields[$i]} = $tokens[$i];
		}
	}
	close($fh);
}

$fn = "db.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";
foreach my $npa (sort keys %db) {
	print STDERR "I: writing NPA $npa...\n";
	foreach my $nxx (sort keys %{$db{$npa}}) {
		foreach my $x (sort keys %{$db{$npa}{$nxx}}) {
			my $data = $db{$npa}{$nxx}{$x};
			my @values = ();
			foreach my $k (@keys) {
				push @values, $data->{$k};
			}
			print $of '"', join('","',@values), '"', "\n";
		}
	}
}
close($of);
exit;

#if (0) {
#my %merges = (
#);
#my %mapping = (
#);
#
#foreach my $npa (sort keys %db) {
#	print STDERR "I: processing NPA $npa...\n";
#	foreach my $nxx (sort keys %{$db{$npa}}) {
#		foreach my $x (sort keys %{$db{$npa}{$nxx}}) {
#			my $data = $db{$npa}{$nxx}{$x};
#			foreach my $key (sort keys %merges) {
#				my $mismatch = 0;
#				my @vals = ();
#				my @srcs = ();
#				foreach my $fld (@{$merges{$key}}) {
#					my $val = $data->{$fld};
#					$val = &{$mapping{$key}{$fld}}($val)
#						if $val and exists $mapping{$key}{$fld};
#					if ($val) {
#						if (my $pre = $data->{$key}) {
#							if ($pre ne $val) {
#								my $upre = "\U$pre\E";
#								my $uval = "\U$val\E";
#								if ($upre ne $uval) {
#									$mismatch++;
#								}
#							}
#						}
#						$data->{$key} = $val;
#						push @srcs, $fld;
#						push @vals, $val;
#					}
#				}
#				if ($mismatch) {
#					print STDERR "E: --- --- -\n";
#					for (my $i=0;$i<@vals;$i++) {
#						my $flag;
#						my $val = $vals[$i];
#						my $src = $srcs[$i];
#						my $cur = $data->{$key};
#						my $ucur = "\U$cur\E";
#						my $uval = "\U$val\E";
#						if (($cur eq $val) or ($ucur eq $uval)) {
#							$flag = '';
#							$matches{$key}{$src}++;
#						} else {
#							$flag = '****';
#							$mismatches{$key}{$src}++;
#						}
#						printf STDERR "E: %-4.4s %-32.32s %s $flag\n", $data->{OCN}, $key.' '.$src,"'$val'";
#					}
#				} elsif (@vals > 1) {
#					printf STDERR "I: --- --- -\n";
#					for (my $i=0;$i<@vals;$i++) {
#						printf STDERR "I: %-4.4s %-32.32s %s\n", $data->{OCN}, $key.' '.$srcs[$i],"'$vals[$i]'";
#						$matches{$key}{$srcs[$i]}++;
#					}
#				}
#			}
#		}
#	}
#	closerecord($data);
#}
#}
