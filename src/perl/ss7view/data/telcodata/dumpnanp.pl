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

my $fn;

$fn = "$datadir/db.pm";
print STDERR "reading $fn\n";
my $db;
unless ($db = do "$fn") {
	die "couldn't parse file $fn: $@" if $@;
	die "couldn't do file $fn: $!" unless defined $db;
	die "couldn't run file $fn" unless $db;
}
die "no db" unless ref $db eq 'HASH';

my $of = \*OUTFILE;
$fn = "$datadir/nanpst.txt";
print STDERR "writing $fn\n";
open($of,">",$fn) or die "can't open $fn for writing";
foreach my $npa (sort keys %{$db->{npa}}) {
	if (exists $db->{npa}{$npa}{state}) {
		foreach my $state (sort keys %{$db->{npa}{$npa}{state}}) {
			print $of "$npa\t$state\n";
		}
	} else {
		print $of "$npa\tXX\n";
	}
}
close($of);
