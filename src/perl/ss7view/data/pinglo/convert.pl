#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"NPA","NXX","X","Type","Carrier","Switch","Rate Center","State"',"\n";
open($fh,"xzgrep '^<tr>' results/*.xz | grep -v '<th' | grep '<a href' | cut -f2- -d: |") or die "can't process files";
while (<$fh>) { chomp;
	my $data = {};
	next unless /^<tr><td><a href=[0-9]{3}-[0-9]{3}>([0-9]{3})-([0-9]{3})-0000<\/a><\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td>/;
	printf $of '"%s","%s","%s","%s","%s","%s","%s","%s"'."\n",$1,$2,'',$3,$4,$5,$6,$7;
}
close($fh);
close($of);
