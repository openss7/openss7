#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my %sws = ();

my $count = 0;
$fn = "sw.csv";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	s/^"//; s/"$//;
	my ($sw) = split(/","/,$_);
	next unless length($sw) == 11;
	$count++ unless exists $sws{$sw};
	$sws{$sw}++;
}
close($fh);
$fn = "host.csv";
print STDERR "I: reading $fn...\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	s/^"//; s/"$//;
	my ($sw) = split(/","/,$_);
	next unless length($sw) == 11;
	$count++ unless exists $sws{$sw};
	$sws{$sw}++;
}
close($fh);

print STDERR "I: there are $count lookups to perform...\n";

my $got = 0;
foreach my $sw (sort keys %sws) {
	my $st = substr($sw,4,2);
	my $ct = substr($sw,0,4);
	my $bl = substr($sw,6,2);
	my $sw = substr($sw,8,3);
	my $dir = "switches/$st/$ct/$bl/$sw";
	$dir =~ s/ /\\ /g;
	system("mkdir -p $dir") unless -d $dir;
	my $fn = "$dir/data.html.xz";
	my $escape = $sw;
	$escape =~ s/-/%2D/g;
	$escape =~ s/ /%20/g;
	my $url = "http://localcallingguide.com/lca_prefix.php?switch=$escape";
	my $cmd = "wget -O - '$url' | xz > $fn";
	$got++;
	next if -f $fn;
	print STDERR "I: getting $sw $got of $count\n";
	system($cmd);
}
