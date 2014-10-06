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
use Time::gmtime;
use DBI;

my $fh = \*INFILE;
my $of = \*OUTFILE;
my ($fn,$fp);

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

$fn = "$progdir/data.sqlite";

print STDERR "I: connecting to database $fn\n";

my $dbh = DBI->connect("dbi:SQLite:dbname=$fn", {
		AutoCommit=>1,
	}) or die "can't open $fn";


my $sql;
my $sth;

$sql = 'CREATE TABLE IF NOT EXISTS ccgeoid (cc CHARACTER(2) PRIMARY KEY ON CONFLICT REPLACE,ccgeoid INTEGER)';
print STDERR "S: $sql\n";
$dbh->do($sql);

$fn = "$progdir/ccgeoid.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO ccgeoid (cc,ccgeoid) VALUES (?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
while (<$fh>) { chomp;
	next unless /^[A-Z]{2}\t/;
	$sth->execute(split(/\t/,$_));
}
close($fh);

undef $sth;

print STDERR "I: disconnecting database\n";

$dbh->disconnect;
undef $dbh;

print STDERR "I: database disconnected\n";

exit;

1;

__END__
