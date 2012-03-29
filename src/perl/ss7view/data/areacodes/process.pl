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

my %stnames = ();

$fn = "$codedir/states.txt";
print STDERR "reading $fn\n";

if (0) {
open($fh,"<","$fn") or die "can't open $fn";
while (<$fh>) { chomp;
	next unless /^[A-Z][A-Z]\t/;
	my ($cc,$st,$nm,$tz) = split(/\t/,$_);
	$nm = "\U$nm\E";
	$st = $cc unless $st;
	$stnames{$nm} = $st;
}
close($fh);
}

my $hist = [
	'Unused',
	'NYNEX',
	'Bell Atlantic',
	'Ameritech',
	'BellSouth',
	'SouthWestern Bell Telephone',
	'US West',
	'Pacific Telesis Group',
	'Off Shore and International',
	'Independents',
];

my %mapping = (
	NPA=>sub{
		my ($data,$val) = @_;
		$data->{npa} = $val;
	},
	NXX=>sub{
		my ($data,$val) = @_;
		$data->{nxx} = $val;
		$data->{blk} = 'A';
		$data->{rng} = '0000-9999';
		$data->{lines} = 10000;
	},
	COUNTRY=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{cc} = $val;
	},
	STATE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{state} = $val;
	},
	CITY=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{city} = $val;
	},
	COUNTY=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{county} = $val;
	},
	LATITUDE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{lat} = $val;
	},
	LONGITUDE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{lon} = $val;
	},
	LATA=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{lata} = $val;
	},
	TIMEZONE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{tz} = $val;
	},
	OBSERVES_DST=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{dst} = $val;
	},
	COUNTY_POPULATION=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{pop} = $val;
	},
	FIPS_COUNTY_CODE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{fips} = $val;
	},
	MSA_COUNTY_CODE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{msa} = $val;
	},
	PMSA_COUNTY_CODE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{pmsa} = $val;
	},
	CBSA_COUNTY_CODE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{cbsa} = $val;
	},
	ZIPCODE_POSTALCODE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{cbsa} = $val;
	},
	ZIPCODE_COUNT=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{zip} = $val;
	},
	ZIPCODE_FREQUENCY=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{freq} = $val;
	},
	NEW_NPA=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{new} = $val;
	},
	NXX_USE_TYPE=>sub{
		my ($data,$val) = @_;
		return unless $val;
		# L for landline; W for wireless
		$data->{use} = $val;
	},
	NXX_INTRO_VERSION=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{vdate} = $val;
	},
	OVERLAY=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{ovly} = $val;
	},
	OCN=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{ocn} = $val;
	},
	COMPANY=>sub{
		my ($data,$val) = @_;
		return unless $val;
		if ($val =~ /^(.*?) - [A-Z][A-Z]$/) {
			$data->{company} = $1;
			$data->{st} = $2;
		} else {
			$data->{company} = $val;
		}
	},
	RATE_CENTER=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{rate} = $val;
	},
	SWITCH_CLLI=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{clli} = $val;
	},
	RC_VERTICAL=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{wcv} = $val;
		$data->{wcvh} = sprintf("%05d,%05d", $data->{wcv},$data->{wch}) if exists $data->{wch};
		#$data->{wcvh} = "$data->{wcv},$data->{wch}" if exists $data->{wch};
	},
	RC_HORIZONTAL=>sub{
		my ($data,$val) = @_;
		return unless $val;
		$data->{wch} = $val;
		$data->{wcvh} = sprintf("%05d,%05d", $data->{wcv},$data->{wch}) if exists $data->{wcv};
		#$data->{wcvh} = "$data->{wcv},$data->{wch}" if exists $data->{wcv};
	},
);


$fn = "$datadir/AREACODEWORLD-GOLD-SAMPLES.zip";
print STDERR "processing $fn\n";
open($fh,"unzip -p $fn AREACODEWORLD-GOLD-SAMPLE.CSV |") or die "can't process $fn";
{
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
	for (my $i=0;$i<@fields;$i++) {
		if (exists $mapping{$fields[$i]}) {
			&{$mapping{$fields[$i]}}($data,$tokens[$i]);
		} else {
			print STDERR "W: no mapping for $fields[$i]\n";
		}
	}
	closerecord($data,$db);
}
}
close($fh);

dumpem($datadir,$db);
dumpem($codedir,$db);
