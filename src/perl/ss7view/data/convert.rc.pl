#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @files = `ls */rc.csv`;
my @keys = (
	'RCSHORT',
	'REGION',
	'RCVH',
);
my %db = ();
my %d2 = ();

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
				if ($l eq 'RCSHORT' or $l eq 'REGION' or $l eq 'RCVH') {
					push @fields, $l;
					push @keys, $dir if $l eq 'RCSHORT';
					next;
				}
				push @fields, "$dir:$l";
				push @keys, "$dir:$l";
			}
			$header = undef;
			next;
		}
		my $rc = $tokens[0];
		my $st = $tokens[1];
		$st = 'PQ' if $st eq 'QC';
		$st = 'NF' if $st eq 'NL';
		$st = 'VU' if $st eq 'NU';
		my $vh = $tokens[2];
		next if $rc eq 'XXXXXXXXXX' or $rc =~ /DIR ASST/i or $rc eq 'N/A' or $rc =~ /FGB ACCESS/i or $rc =~ /7 DIGIT SE/i or $rc =~ /INFO PROV/i;
		my $data;
		if (exists $db{$st}{$rc}) {
			$data = $db{$st}{$rc};
		} elsif ($vh and $vh ne '00000,00000' and exists $d2{$vh}) {
			$data = $d2{$vh};
			print  STDERR "E: ---------------\n";
			printf STDERR "E: conflicting keys %-10.10s %-2.2s %-11.11s\n",
			$data->{RCSHORT}, $data->{REGION}, $data->{RCVH};
			$data = {RCSHORT=>$rc,REGION=>$st,RCVH=>$vh};
			printf STDERR "E: conflicting keys %-10.10s %-2.2s %-11.11s\n",
			$data->{RCSHORT}, $data->{REGION}, $data->{RCVH};
			$db{$st}{$rc} = $data;
		} else {
			$data = {RCSHORT=>$rc,REGION=>$st,RCVH=>$vh};
			$db{$st}{$rc} = $data;
			$d2{$vh} = $data;
		}
		$data->{$dir} = 'X';
		for (my $i=3;$i<@fields;$i++) {
			$data->{$fields[$i]} = $tokens[$i];
		}
	}
	close($fh);
}

$fn = "rc.csv";
print STDERR "I: writing $fn...\n";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";
foreach my $st (sort keys %db) {
	foreach my $rc (sort keys %{$db{$st}}) {
		my $data = $db{$st}{$rc};
		my @values = ();
		foreach my $k (@keys) {
			push @values, $data->{$k};
		}
		print $of '"', join('","',@values), '"', "\n";
	}
}
close($of);
