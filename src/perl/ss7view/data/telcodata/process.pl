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

my @files = `find $datadir -name '*.html.xz' | sort`;

my $names = [
	'dummy',
	'NPA NXX',
	'State',
	'Company',
	'OCN',
	'Rate Center',
	'Switch',
	'Date',
	'Prefix Type',
	'Switch Name',
	'Switch Type',
	'LATA',
	'Tandem',
];

my %mapping = (
	'NPA NXX'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		my ($npa,$nxx,$blk) = split(/-/,$val);
		$data->{npa} = $npa;
		if (defined $nxx) {
			$data->{nxx} = $nxx;
			if (defined $blk) {
				$data->{blk} = $blk;
				$data->{rng} = $blk.'000-'.$blk.'999';
				$data->{lines} = 1000;
			} else {
				$data->{blk} = 'A';
				$data->{rng} = '0000-9999';
				$data->{lines} = 10000;
			}
		}
	},
	'State'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{state} = $val;
		if (defined $data->{loc} and !defined $data->{stloc}) {
			$data->{stloc} = "$val: $data->{loc}";
		}
	},
	'Company'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{company} = $val;
	},
	'OCN'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'MULT';
		return if $val eq 'N/A';
		$data->{ocn} = $val;
	},
	'Rate Center'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{rate} = $val;
	},
	'Switch'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		return if $val eq 'UNKNOWNCLLI';
		return if $val eq 'NOCLLIKNOWN';
		$data->{clli} = $val;
	},
	'Date'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		return if $val eq '00/00/0000';
		if ($val =~ /([0-9]{2})\/([0-9]{2})\/([0-9]{4})/) {
			$data->{adate} = "$3-$1-$2";
		} else {
			print STDERR "E: can't grok Date '$val'\n";
		}
	},
	'Prefix Type'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		return if $val eq 'UNKNOWN';
		$data->{category} = $val;
	},
	'Switch Name'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		#if ($val =~ /^(.*?)\s+([A-Z]{2})$/) {
		#	my $loc = $1;
		#	my $st = $2;
		#	$data->{loc} = $loc;
		#	$data->{stloc} = "$st - $loc";
		#} else {
			$data->{loc} = $val;
			$data->{stloc} = "$data->{state}: $val" if defined $data->{state};
		#}
	},
	'Switch Type'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{type} = $val;
	},
	'LATA'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		if ($val =~ /^(.*?)\s+\(([0-9]{3,5})\)$/) {
			$data->{name} = $1;
			$data->{lata} = $2;
		}
	},
	'Tandem'=>sub{
		my ($data,$val) = @_;
		return unless $val;
		return if $val eq 'N/A';
		$data->{tandem} = $val;
	},
);

foreach $fn (@files) { chomp $fn;
	print STDERR "processing $fn...\n";
	open($fh,"xzcat $fn|") or die "can't process $fn";
	while (<$fh>) { chomp;
		next unless /<tr class="results"/;
		/<tr class="results">(.*?)<\/tr>/;
		$_ = $1;
		my $data = {};
		my $fno = 0;
		while (/<td.*?>(.*?)<\/td>/g) {
			my $field = $1; $fno++;
			$field =~ s/<a href=.*?>//g;
			$field =~ s/<\/a>//g;
			if ($field =~ /<br>/) {
				my @fields = split(/<br>/,$field);
				foreach my $datum (@fields) {
					next unless $datum =~ /: +/;
					my ($key,$val) = split(/: +/,$datum);
					&{$mapping{$key}}($data,$val) if exists $mapping{$key};
				}
			} else {
				my $key = $names->[$fno];
				&{$mapping{$key}}($data,$field) if exists $mapping{$key};
			}
		}
		closerecord($data,$db);
	}
	close($fh);
}

dumpem($datadir,$db);
dumpem($codedir,$db);
