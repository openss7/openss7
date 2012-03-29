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

require "$codedir/database.pm";

my $fh = \*INFILE;
my $fn;

my $db = {};

$fn = "$codedir/db.pm";
if (-f $fn) {
	print STDERR "reading $fn\n";
	unless ($db = do "$fn") {
		die "can't parse file $fn: $@" if $@;
		die "can't do file $fn: $!" unless defined $db;
		die "can't run file $fn" unless $db;
	}
	die "no db" unless ref $db eq 'HASH';
}

open($fh,"xzgrep '^<tr>' results/*.xz | grep -v '<th' | grep '<a href' | cut -f2- -d: |") or die "can't process files";
while (<$fh>) { chomp;
	my $data = {};
	next unless /^<tr><td><a href=[0-9]{3}-[0-9]{3}>([0-9]{3})-([0-9]{3})-0000<\/a><\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td>/;
	$data->{npa} = $1;
	$data->{nxx} = $2;
	$data->{blk} = 'A';
	$data->{rng} = '0000-9999';
	$data->{use} = $3;
	$data->{company} = $4;
	$data->{clli} = $5;
	$data->{rate} = $6;
	$data->{state} = $7;
	if ($data->{use} eq 'Land Line') {
		$data->{use} = 'L';
	} elsif ($data->{use} eq 'Wireless') {
		$data->{use} = 'W';
	} elsif ($data->{use} eq 'Mixed') {
		$data->{use} = 'X';
	} elsif ($data->{use} eq 'Pager') {
		$data->{use} = 'P';
	} else {
		print STDERR "E: can't grok Use '$data->{use}'\n";
		delete $data->{use};
	}
	closerecord($data,$db);
}
close($fh);

dumpem($datadir,$db);
dumpem($codedir,$db);
