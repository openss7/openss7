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
my ($sth, $sth2);

$sql = 'CREATE TABLE IF NOT EXISTS ccodes (cc CHARACTER(2),ccc CHARACTER(3),nnn CHARACTER(3),desc TEXT PRIMARY KEY ON CONFLICT REPLACE)';
print STDERR "S: $sql\n";
$dbh->do($sql);
$fn = "$progdir/ccodes.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO ccodes (cc,ccc,nnn,desc) VALUES (?,?,?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
while (<$fh>) { chomp;
	next unless /^[A-Z]{2}\t/;
	$sth->execute(split(/\t/,$_));
}
close($fh);

$sql = 'CREATE TABLE IF NOT EXISTS e212a (mcc VCHAR(3) PRIMARY KEY ON CONFLICT REPLACE,country TEXT,note CHAR(1))';
print STDERR "S: $sql\n";
$dbh->do($sql);
$sql = 'CREATE TABLE IF NOT EXISTS e212a_notes (flag CHARACTER(1) PRIMARY KEY ON CONFLICT REPLACE,note TEXT)';
print STDERR "S: $sql\n";
$dbh->do($sql);
$fn = "$progdir/e212a.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO e212a (mcc,country,note) VALUES (?,?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
$sql = 'INSERT OR REPLACE INTO e212a_notes (flag,note) VALUES (?,?)';
print STDERR "S: $sql\n";
$sth2 = $dbh->prepare($sql);
while (<$fh>) { chomp;
	if (/^[0-9]+\t/) {
		my ($mcc,$country,$note) = split(/\t/,$_);
		$sth->execute($mcc,$country,$note);
	} elsif (/^[a-z]\t/) {
		$sth2->execute(split(/\t/,$_));
	}
}
close($fh);

$sql = 'CREATE TABLE IF NOT EXISTS e212b (mcc VCHAR(3),mnc VCHAR(3),company TEXT)';

undef $sth;
undef $sth2;
print STDERR "I: disconnecting database\n";
$dbh->disconnect;
undef $dbh;
print STDERR "I: database disconnected\n";

exit;

1;

__END__
