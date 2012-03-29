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

my %rcs = ();

my @keys = (
	'RCSHORT',
	'REGION',
	'NPAS',
	'EXCH',
	'RC',
	'SEE-EXCH',
	'SEE-RC',
	'SEE-REGION',
	'LATA',
	'OCN',
	'COMPANY-NAME',
	'ILEC-OCN',
	'ILEC-NAME',
	'RC-V',
	'RC-H',
	'RC-LAT',
	'RC-LON',
	'UDATE',
);
