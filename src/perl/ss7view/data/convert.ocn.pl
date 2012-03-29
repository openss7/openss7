#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @files = `ls */ocn.csv`;
my @keys = (
	'OCN',
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
				if ($l eq 'OCN') {
					push @fields, $l;
					push @keys, $dir if $l eq 'OCN';
					next;
				}
				push @fields, "$dir:$l";
				push @keys, "$dir:$l";
			}
			$header = undef;
			next;
		}
		my $ocn = $tokens[0];
		$db{$ocn} = {OCN=>$ocn}
			unless exists $db{$ocn};
		my $data = $db{$ocn};
		$data->{$dir} = 'X';
		for (my $i=1;$i<@fields;$i++) {
			$data->{$fields[$i]} = $tokens[$i];
		}
	}
	close($fh);
}

$fn = "ocn.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";
foreach my $ocn (sort keys %db) {
	my $data = $db{$ocn};
	my @values = ();
	foreach my $k (@keys) {
		push @values, $data->{$k};
	}
	print $of '"', join('","',@values), '"', "\n";
}
close($of);

