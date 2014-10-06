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
use File::stat;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my $fn;

my @keys = (
	'NPA',
	'NXX',
	'X',
	'XXXX',
	'YYYY',
	'COUNTRY',
	'STATE',
	'CITY',
	'COUNTY',
	'LATITUDE',
	'LONGITUDE',
	'LATA',
	'TIMEZONE',
	'OBSERVES_DST',
	'COUNTY_POPULATION',
	'FIPS_COUNTY_CODE',
	'MSA_COUNTY_CODE',
	'PMSA_COUNTY_CODE',
	'CBSA_COUNTY_CODE',
	'ZIPCODE_POSTALCODE',
	'ZIPCODE_COUNT',
	'ZIPCODE_FREQUENCY',
	'NEW_NPA',
	'NXX_USE_TYPE',
	'NXX_INTRO_VERSION',
	'OVERLAY',
	'OCN',
	'COMPANY',
	'RATE_CENTER',
	'SWITCH_CLLI',
	'RC_VERTICAL',
	'RC_HORIZONTAL',
	'FDATE',
);

$fn = "$datadir/db.csv";
open($of,">",$fn) or die "can't open $fn";
print $of '"', join('","',@keys), '"', "\n";
$fn = "$datadir/AREACODEWORLD-GOLD-SAMPLES.zip";
my $udate = stat($fn)->mtime;
print STDERR "I: processing $fn...\n";
open($fh,"unzip -p $fn AREACODEWORLD-GOLD-SAMPLE.CSV |") or die "can't process $fn";
my $header = 1;
my @fields = ();
while (<$fh>) { chomp; s/\r//g;
	s/^"//;
	s/"$//;
	my @tokens = split(/","/,$_);
	if ($header) {
		@fields = @tokens;
		$header = undef;
		next;
	}
	my $data = {};
	$data->{FDATE} = $udate;
	for (my $i=0;$i<@fields;$i++) {
		$data->{$fields[$i]} = $tokens[$i] if $tokens[$i];
	}
	if (exists $data->{NPA} and exists $data->{NXX}) {
		my @values = ();
		foreach (@keys) { push @values, $data->{$_}; }
		print $of '"', join('","', @values), '"', "\n";
	}
}
close($fh);
close($of);

exit;

1;

__END__

