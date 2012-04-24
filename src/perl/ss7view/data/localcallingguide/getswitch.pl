#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my %switches = ();

my $count = 0;
$fn = "../LERG7.DAT";
print STDERR "I: reading $fn\n";
open($fh,"<",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	next if /^#/;
	my $sw = substr($_,32,11);
	my $sw6 = substr($sw,0,6);
	my $sw4 = substr($sw6,0,4);
	my $st = substr($sw6,4,2);
	next unless $sw4 and $st;
	$count++ unless exists $switches{$st}{$sw4};
	$switches{$st}{$sw4}++;
}
close($fh);

print STDERR "I: there are $count lookups to perform...\n";

my $got = 0;
foreach my $st (sort keys %switches) {
	foreach my $sw4 (sort keys %{$switches{$st}}) {
		my $dir = "switches/$st/$sw4";
		$fn = "$dir/data.html.xz";
		system("mkdir -p $dir") unless -d $dir;
		my $url = "http://localcallingguide.com/lca_switch.php?switch=$sw4$st&ocn=";
		my $cmd = "wget -O - '$url' | xz > $fn";
		$got++;
		next if -f $fn;
		print STDERR "I: getting $sw4$st $got of $count\n";
		system($cmd);
	}
}
