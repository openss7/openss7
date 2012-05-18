#!/usr/bin/perl

use strict;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my %exs = ();

my $count = 0;
$fn = "ex.nogeo.csv";
print STDER "I: reading $fn...\n";
open($fh,"<:utf8",$fn) or die "can't open $fn";
while (<$fh>) { chomp;
	s/^"//; s/"$//;
	my ($ex) = split(/","/,$_);
	next unless length($ex) == 6;
	$count += 2 unless exists $exs{$ex};
	$exs{$ex}++;
}
close($fh);

print STDERR "I: there are $count lookups to perform...\n";

my $got = 0;
foreach my $ex (keys %exs) {
	my $dir = 'exchs/ob/'.substr($ex,0,1).'/'.substr($ex,1,1).'/'.substr($ex,2,1);
	my $fn = "$dir/$ex.xml.xz";
	unless (-f $fn) {
		my $url = "http://localcallingguide.com/xmllocalexch.php?exch=$ex&dir=1";
		my $cmd = "wget -O - '$url' | xz > $fn";
		print STDERR "I: getting $ex ($got of $count)\n";
		system("mkdir -p $dir") unless -d $dir;
		system($cmd);
	}
	$got++;
	my $dir = 'exchs/ib/'.substr($ex,0,1).'/'.substr($ex,1,1).'/'.substr($ex,2,1);
	my $fn = "$dir/$ex.xml.xz";
	unless (-f $fn) {
		my $url = "http://localcallingguide.com/xmllocalexch.php?exch=$ex&dir=2";
		my $cmd = "wget -O - '$url' | xz > $fn";
		print STDERR "I: getting $ex ($got of $count)\n";
		system("mkdir -p $dir") unless -d $dir;
		system($cmd);
	}
	$got++;
}

exit;

1;

__END__
