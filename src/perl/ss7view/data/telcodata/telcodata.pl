#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = "$progdir/..";

use strict;

my $fh = \*INFILE;

my $total = 0;

my @npas = ();

open($fh,"<","$datadir/nanp.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9][0-9][0-9]\t/;
	my ($npa,$cc,$loc,$geo) = split(/\t/,$_);
	#next if $geo;
	next if -f "results/$npa/data.html.xz";
	push @npas, $npa;
}
close($fh);

my $nnpa = scalar @npas;
my $nnxx = 800;
my $nnum = $nnpa * $nnxx;

print "There are $nnpa NPAs.\n";
print "There are $nnxx NXXs per NPA.\n";
print "There are $nnum NXXs to query.\n";

my $secs = 60 * 60 * 24 * 7;
print "There are $secs seconds in a week.\n";
my $pers = $secs/$nnum;
print "Need to query one per $pers seconds to be done in a week.\n";

while (1) {
	my $npa = $npas[int(rand($nnpa))];
	my $dir = sprintf("results/%03d",$npa);
	my $fn = sprintf("results/%03d/data.html.xz",$npa);
	my $url = sprintf("'http://www.telcodata.us/search-area-code-exchange-detail?npa=%03d&exchange='",$npa);
	my $cmd = "mkdir -p $dir; wget -O - $url | xz > $fn";
	unless (-f $fn) {
		my $sec = 60 + int(rand(120));
		warn "would download $fn from $url";
		warn "using command $cmd";
		warn "sleeping for $sec seconds";
		system($cmd);
		sleep($sec);
	}
}
