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
	my $rsltdir = "$progdir/results";
	my @files = `find $rsltdir -name '*.html.xz' | sort`;
	foreach my $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn\n";
		open(INFILE,"xzgrep '^<tr>' $fn | grep -v '<th' | grep '<a href' | cut -f2- -d: |") or die "can't process files";
		my $sect = $fn; $sect =~ s/.*\///; $sect =~ s/\.html\.xz//;
		my $fdate = stat($fn)->mtime;
		while (<INFILE>) { chomp;
			next unless /^<tr><td><a href=[0-9]{3}-[0-9]{3}>([0-9]{3})-([0-9]{3})-0000<\/a><\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td><td>([^<]*)<\/td>/;
			my %data = (sect=>$sect,fdate=>$fdate);
			$data{npa} = $1;
			$data{nxx} = $2;
			$data{use} = substr($3,0,1);
			$data{carrier} = $4;
			$data{switch} = $5;
			$data{rc} = $6;
			$data{st} = $7;
			makedb::updatedata(\%data,$fdate,$sect);
		}
		close(INFILE);
	}
	$dbh->commit;
}

makedb::makedb('pingdata',\&dodata);

exit;

1;

__END__
