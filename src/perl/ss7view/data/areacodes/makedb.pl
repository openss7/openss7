#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $datadir = $progdir;
my $geondir = "$progdir/../geonames";
my $codedir = "$progdir/..";

use strict;
use Data::Dumper;
use Encode qw(encode decode);
use Text::Levenshtein qw(distance);
use Geo::Coordinates::VandH;
use POSIX;
use File::stat;
use Time::gmtime;
use Date::Parse;
use DBI;

require "$progdir/../makedb.pm";

binmode(STDERR,':utf8');

sub dodata {
	my $dbh = shift;
	$dbh->begin_work;
	my %mapping = (
		'NPA'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,$fld,$val);
		},
		'NXX'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,$fld,$val);
			#$dat->{sect} = "$dat->{npa}-$dat->{nxx}";
		},
		'COUNTRY'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'rccc',$val);
		},
		'STATE'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'rcst',$val);
			makedb::simplefield($dat,'region',$val);
		},
		'CITY'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'rccity',$val);
		},
		'COUNTY'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'rccounty',$val);
		},
		'LATITUDE'=>sub{
			my ($dat,$fld,$val) = @_;
			$val = sprintf('%.8f',$val) if $val;
			makedb::simplefield($dat,'rclat',$val);
		},
		'LONGITUDE'=>sub{
			my ($dat,$fld,$val) = @_;
			$val = sprintf('%.8f',$val) if $val;
			makedb::simplefield($dat,'rclon',$val);
		},
		'LATA'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'rclata',$val);
		},
		'TIMEZONE'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'tzone',$val);
		},
		'OBSERVES_DST'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'dst',$val);
		},
		'COUNTY_POPULATION'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'population',$val);
		},
		'FIPS_COUNTY_CODE'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'fips',$val);
		},
		'MSA_COUNTY_CODE'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'msa',$val);
		},
		'PMSA_COUNTY_CODE'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'pmsa',$val);
		},
		'CBSA_COUNTY_CODE'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'cbsa',$val);
		},
		'ZIPCODE_POSTALCODE'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'rczip',$val);
		},
		'ZIPCODE_COUNT'=>sub{
			my ($dat,$fld,$val) = @_;
		},
		'ZIPCODE_FREQUENCY'=>sub{
			my ($dat,$fld,$val) = @_;
		},
		'NEW_NPA'=>\&makedb::simplefield,
		'NXX_USE_TYPE'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'use',$val);
		},
		'NXX_INTRO_VERSION'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::datefield($dat,'assigndate',$val);
		},
		'OVERLAY'=>\&makedb::simplefield,
		'OCN'=>\&makedb::simplefield,
		'COMPANY'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'companyname',$val);
		},
		'RATE_CENTER'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'rcshort',$val);
		},
		'SWITCH_CLLI'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'switch',$val);
		},
		'RC_VERTICAL'=>sub{
			my ($dat,$fld,$val) = @_;
			$val = sprintf('%05d',$val) if $val;
			makedb::simplefield($dat,'rcv',$val);
		},
		'RC_HORIZONTAL'=>sub{
			my ($dat,$fld,$val) = @_;
			$val = sprintf('%05d',$val) if $val;
			makedb::simplefield($dat,'rch',$val);
		},
	);
	my $fn = "$datadir/AREACODEWORLD-GOLD-SAMPLES.zip";
	print STDERR "I: processing $fn\n";
	open(INFILE,"unzip -p $fn AREACODEWORLD-GOLD-SAMPLE.CSV |") or die "can't process $fn";
	my $fdate = stat($fn)->mtime;
	my $sect = 'GOLD';
	my $header = 1;
	my @fields = ();
	while (<INFILE>) { chomp; s/\r//g; chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($header) {
			@fields = @tokens;
			$header = undef;
			next;
		}
		my %data = (sect=>$sect,fdate=>$fdate);
		for (my $i=0;$i<@fields;$i++) {
			if (exists $mapping{$fields[$i]}) {
				&{$mapping{$fields[$i]}}(\%data,$fields[$i],$tokens[$i]);
			} else {
				print STDERR "E: no maping for $fields[$i]: $tokens[$i]\n";
			}
		}
		makedb::updatedata(\%data,$fdate,$sect);
	}
	close(INFILE);
	$dbh->commit;
}

makedb::makedb('areadata',\&dodata);

exit;

1;

__END__
