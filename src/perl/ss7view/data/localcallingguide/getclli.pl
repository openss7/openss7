#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my %cllis = ();

my $count = 0;
$fn = "clli.csv";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	s/^"//; s/"$//;
	my ($clli) = split(/","/,$_);
	next unless length($clli) == 11;
	$count++ unless exists $cllis{$clli};
	$cllis{$clli}++;
}
close($fh);
$fn = "host.csv";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	s/^"//; s/"$//;
	my ($clli) = split(/","/,$_);
	next unless length($clli) == 11;
	$count++ unless exists $cllis{$clli};
	$cllis{$clli}++;
}
close($fh);

print STDERR "I: there are $count lookups to perform...\n";

my $got = 0;
foreach my $clli (sort keys %cllis) {
	my $st = substr($clli,4,2);
	my $ct = substr($clli,0,4);
	my $bl = substr($clli,6,2);
	my $sw = substr($clli,8,3);
	my $dir = "switches/$st/$ct/$bl/$sw";
	$dir =~ s/ /\\ /g;
	system("mkdir -p $dir") unless -d $dir;
	my $fn = "$dir/data.html.xz";
	my $escape = $clli;
	$escape =~ s/-/%2D/g;
	$escape =~ s/ /%20/g;
	my $url = "http://localcallingguide.com/lca_prefix.php?switch=$escape";
	my $cmd = "wget -O - '$url' | xz > $fn";
	$got++;
	next if -f $fn;
	print STDERR "I: getting $clli $got of $count\n";
	system($cmd);
}
