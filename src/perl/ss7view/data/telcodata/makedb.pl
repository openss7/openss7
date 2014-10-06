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
		'NPA NXX'=>sub{
			my ($dat,$fld,$val) = @_;
			return unless length($val);
			my ($npa,$nxx,$x) = split(/-/,$val);
			$dat->{npa} = $npa if length($npa);
			$dat->{nxx} = $nxx if length($nxx);
			$dat->{x}   = $x   if length($x);
		},
		'State'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'st',$val);
			makedb::simplefield($dat,'region',$val);
		},
		'Company'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'companyname',$val);
		},
		'OCN'=>sub{
			my ($dat,$fld,$val) = @_;
			$val = undef if $val eq 'N/A';
			makedb::simplefield($dat,$fld,$val);
		},
		'Rate Center'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'rc',$val);
		},
		'Switch'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,$fld,$val);
		},
		'Date'=>sub{
			my ($dat,$fld,$val) = @_;
			return unless length($val);
			if (my $time = str2time($val)) {
				makedb::simplefield($dat,'udate',$time);
			} else {
				unless ($val eq '00/00/0000' or $val eq 'N/A') {
					print STDERR "E: cannot grok date '$val'\n";
				}
			}
		},
		'Prefix Type'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::mappedfield($dat,'nxxtype',$val,{
					'CLEC'=>'C',
					'WIRELESS'=>'W',
					'PCS'=>'P',
					'RBOC'=>'R',
					'ICO'=>'I',
					'UNKNOWN'=>'?',
					'CAP'=>'A',
					'L RESELLER'=>'L',
					'N/A'=>'-',
					'ULEC'=>'U',
					'W RESELLER'=>'E',
					'GENERAL'=>'G',
				});
			makedb::mappedfield($dat,'companytype',$val,{
					'CLEC'=>'CLEC',
					'WIRELESS'=>'WIRE',
					'PCS'=>'PCS',
					'RBOC'=>'RBOC',
					'ICO'=>'ILEC',
					'UNKNOWN'=>undef,
					'CAP'=>'CAP',
					'L RESELLER'=>'LRES',
					'N/A'=>undef,
					'ULEC'=>'ULEC',
					'W RESELLER'=>'WRES',
					'GENERAL'=>'GEN',
				});
		},
		'Switch Name'=>\&makedb::simplefield,
		'Switch Type'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'switchdesc',$val);
		},
		'LATA'=>sub{
			my ($dat,$fld,$val) = @_;
			$val = undef if $val eq 'N/A';
			makedb::simplefield($dat,'rclata',$val);
			makedb::simplefield($dat,$fld,$val);
		},
		'Tandem'=>sub{
			my ($dat,$fld,$val) = @_;
			makedb::simplefield($dat,'tdm',$val);
		},
	);
	print STDERR "I: finding results files...\n";
	my $rsltdir = "$datadir/results";
	my @files = `find $rsltdir -name '*.html.xz' | sort`;
	foreach my $fn (@files) { chomp $fn;
		print STDERR "I: processing $fn\n";
		open(INFILE,"xzcat $fn|") or die "can't process $fn";
		my $sect = $fn; $sect =~ s/.*\///; $sect =~ s/\.html\.xz//;
		my $fdate = stat($fn)->mtime;
		while (<INFILE>) { chomp;
			next unless /<tr class="results"/;
			/<tr class="results">(.*?)<\/tr>/;
			$_ = $1;
			my %data = (sect=>$sect,fdate=>$fdate);
			my $fno = 0;
			while (/<td.*?>(.*?)<\/td>/g) {
				my $fld = $1; $fno++;
				$fld =~ s/<a href=.*?>//g;
				$fld =~ s/<\/a>//g;
				if ($fld =~ /<br>/) {
					my @fields = split(/<br>/,$fld);
					foreach my $datum (@fields) {
						next unless $datum =~ /: +/;
						my ($key,$val) = split(/: +/,$datum);
						if (exists $mapping{$key}) {
							&{$mapping{$key}}(\%data,$key,$val);
						} else {
							print STDERR "E: no mapping for $key: $val\n";
						}
					}
				} else {
					my $key = [
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
						'FDATE',
					]->[$fno];
					if (exists $mapping{$key}) {
						&{$mapping{$key}}(\%data,$key,$fld);
					} else {
						print STDERR "E: no mapping for $key: $fld\n";
					}
				}
			}
			makedb::updatedata(\%data,$fdate,$sect);
		}
		close(INFILE);
	}
	$dbh->commit;
}

makedb::makedb('telcdata',\&dodata);

exit;

1;

__END__
