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

my ($dbh,$sql,$sth);

$fn = "$progdir/itudata.sqlite";
print STDERR "I: connecting to database $fn\n";
$dbh = DBI->connect("dbi:SQLite:dbname=$fn", {
		AutoCommit=>1,
	}) or die "can't open $fn";

$sql = q{PRAGMA foreign_keys = ON;};
print STDERR "S: $sql\n";
$dbh->do($sql);

$sql = q{
	CREATE TABLE IF NOT EXISTS cnames (
		alpha2 CHARACTER(2),
		alpha3 CHARACTER(3),
		numeric CHARACTER(3),
		country TEXT PRIMARY KEY
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);

$fn = "$progdir/ccodes.txt";
print STDERR "I: processsing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO cnames (alpha2,alpha3,numeric,country) VALUES (?,?,?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
while (<$fh>) { chomp;
	next unless /^[A-Z]{2}\t/;
	my ($alpha2,$alpha3,$numeric,$country) = split(/\t/,$_);
	unless ($sth->execute($alpha2,$alpha3,$numeric,$country)) {
		print STDERR "E: ",$sth->errstr,"\n";
		print STDERR "E: input was $alpha2,$alpha3,$numeric,$country\n";
	}
}
close($fh);

$sql = q{
	CREATE TABLE IF NOT EXISTS q708a (
		zone CHARACTER(1),
		sanc CHARACTER(3),
		country TEXT,
		PRIMARY KEY(zone, sanc),
		FOREIGN KEY(country) REFERENCES cnames(country)
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);

$fn = "$progdir/T-SP-Q.708A-2011-PDF-E.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO q708a (zone,sanc,country) VALUES (?,?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
while (<$fh>) { chomp;
	next unless /^([2-7])-([0-9]{3})\s+(.*)$/;
	unless ($sth->execute($1,$2,$3)) {
		print STDERR "E: ",$sth->errstr,"\n";
		print STDERR "E: input was $1,$2,$3\n";
	}
}
close($fh);

$sql = q{
	CREATE TABLE IF NOT EXISTS q708b (
		zone CHARACTER(1),
		sanc CHARACTER(3),
		ispc CHARACTER(1),
		dec INTEGER UNIQUE,
		switch TEXT,
		oper TEXT,
		PRIMARY KEY(zone, sanc, ispc),
		FOREIGN KEY(zone, sanc) REFERENCES q708a(zone, sanc)
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);

$fn = "$progdir/T-SP-Q.708B-2011-PDF-E.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO q708b (zone,sanc,ispc,dec,switch,oper) VALUES (?,?,?,?,?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
while (<$fh>) { chomp;
	next unless /^([2-7])-([0-9]{3})-([0-7])\s+([0-9]+)\s+(.*?)\s{3,}(.*)$/;
	unless ($sth->execute($1,$2,$3,$4,$5,$6)) {
		print STDERR "E: ",$sth->errstr,"\n";
		print STDERR "E: input was $1,$2,$3\n";
	}
}
close($fh);

$sql = q{
	CREATE TABLE IF NOT EXISTS e164b (
		country TEXT,
		ccode VCHAR(5) PRIMARY KEY,
		mobile TEXT,
		FOREIGN KEY(country) REFERENCES cnames(country)
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);

$fn = "$progdir/T-SP-E.164B-2011-PDF-E.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO e164b (country,ccode,mobile) VALUES (?,?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
{
	my %data = ();
	while (<$fh>) { chomp;
		next if /^Annex/i;
		next if /^Country\/geographical area/i;
		next if /^\s+Code\s+code/i;
		next if /^NXX :/i;
		next if /^NPA :/i;
		next if /^\s*$/;
		if (/^([^ ].*?)\s{3,}([1-9][0-9()\/]*)\s{3,}/) {
			if (exists $data{country}) {
				unless ($sth->execute($data{country},$data{ccode},$data{mobile})) {
					print STDERR "E: ",$sth->errstr,"\n";
					print STDERR "E: input was '",join("','",$data{country},$data{ccode},$data{mobile}),"'\n";
				}
				%data = ();
			}
			$data{country} = $1;
			$data{ccode} = $2;
			$data{mobile} = substr($_,80);
			$data{mobile} =~ s/^\s+//;
			$data{mobile} =~ s/\s+$//;
			delete $data{mobile} unless $data{mobile};
		} elsif (exists $data{country}) {
			my $field;
			$field = substr($_,0,32); $field =~ s/^\s+//; $field =~ s/\s+$//;
			if ($data{country} =~ /-$/) {
				$data{country} .= $field if $field;
			} else {
				$data{country} = join(' ',$data{country},$field) if $field;
			}
			$field = substr($_,80); $field =~ s/^\s+//; $field =~ s/\s+$//;
			$data{mobile} = join(' ',$data{mobile},$field) if $field;
		}
	}
	if (exists $data{country}) {
		unless ($sth->execute($data{country},$data{ccode},$data{mobile})) {
			print STDERR "E: ",$sth->errstr,"\n";
			print STDERR "E: input was '",join("','",$data{country},$data{ccode},$data{mobile}),"'\n";
		}
		%data = ();
	}
}
close($fh);


$sql = q{
	CREATE TABLE IF NOT EXISTS e164c_notes (
		flag INTEGER PRIMARY KEY,
		note TEXT
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);

$fn = "$progdir/T-SP-E.164C-2011-PDF-E.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO e164c_notes (flag,note) VALUES (?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
{
	my $notes = undef;
	my $flag = undef;
	my @lines = ();
	while (<$fh>) { chomp;
		unless ($notes) {
			next if /^\s*$/;
			next unless /^\s*Notes\s*$/;
			$notes = 1;
			next;
		}
		if (/^\s\s?([0-9]{1,2})\s+(.*)$/) {
			if (defined $flag) {
				unless($sth->execute($flag,join("\n",@lines))) {
					print STDERR "E: ",$sth->errstr,"\n";
					print STDERR "E: input was $flag,",join(' ',@lines),"\n";
				}
				$flag = undef;
				@lines = ();
			}
			$flag = $1;
			push @lines,$2;
			next;
		} elsif (/^\s*$/) {
			if (defined $flag) {
				unless($sth->execute($flag,join("\n",@lines))) {
					print STDERR "E: ",$sth->errstr,"\n";
					print STDERR "E: input was $flag,",join(' ',@lines),"\n";
				}
				$flag = undef;
				@lines = ();
				last;
			}
			next;
		} elsif (/^\s{4,}(.*)$/) {
			if (defined $flag) {
				push @lines, $1;
				next;
			}
		}
		print STDERR "E: can't grok line '$_'\n";
	}
}
close($fh);

$sql = q{
	CREATE TABLE IF NOT EXISTS e164c (
		country TEXT PRIMARY KEY,
		ccode VCHAR(5),
		intpfx VCHAR(3),
		natpfx VCHAR(3),
		nsn VCHAR(20),
		tzone VCHAR(10),
		note CHARACTER(1),
		FOREIGN KEY(country) REFERENCES cnames(country),
		FOREIGN KEY(note) REFERENCES e164c_notes(flag)
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);

$fn = "$progdir/T-SP-E.164C-2011-PDF-E.txt";
print STDERR "I: processing $fn\n";
open($fh,"<:utf8",$fn) or die "can't process $fn";
$sql = 'INSERT OR REPLACE INTO e164c (country,ccode,intpfx,natpfx,nsn,tzone,note) VALUES (?,?,?,?,?,?,?)';
print STDERR "S: $sql\n";
$sth = $dbh->prepare($sql);
{
	my %data = ();
	while (<$fh>) { chomp;
		last if /^\s*Notes\s*$/i;
		next if /^Annex/i;
		next if /^Country\/geographical area/i;
		next if /^\s+Code\s+Prefix/i;
		next if /^____/;
		next if /^\*\s/;
		next if /^\s*$/;
		if (/^([^ ].*?)\s{3,}([1-9][0-9()]*)/) {
			if (exists $data{country}) {
				unless ($sth->execute($data{country},$data{ccode},$data{intpfx},$data{natpfx},$data{nsn},$data{tzone},$data{note})) {
					print STDERR "E: ",$sth->errstr,"\n";
					print STDERR "E: input was '",join("','",$data{country},$data{ccode},$data{intpfx},$data{natpfx},$data{nsn},$data{tzone},$data{note}),"'\n";
				}
				%data = ();
			}
			$data{country} = $1;
			$data{ccode}   = $2;
			$data{intpfx}  = substr($_,43,12); $data{intpfx} =~ s/^\s+//; $data{intpfx} =~ s/\s+$//; delete $data{intpfx} unless $data{intpfx};
			$data{natpfx}  = substr($_,56,12); $data{natpfx} =~ s/^\s+//; $data{natpfx} =~ s/\s+$//; delete $data{natpfx} unless $data{natpfx};
			$data{nsn}     = substr($_,66,20); $data{nsn}    =~ s/^\s+//; $data{nsn}    =~ s/\s+$//; delete $data{nsn} unless $data{nsn};
			$data{tzone}   = substr($_,90,12); $data{tzone}  =~ s/^\s+//; $data{tzone}  =~ s/\s+$//; delete $data{tzone} unless $data{tzone};
			$data{note}    = substr($_,102  ); $data{note}   =~ s/^\s+//; $data{note}   =~ s/\s+$//; delete $data{note} unless $data{note};
			$data{nsn} =~ s/\s*\bdigits\b\s*//g;
			$data{nsn} =~ s/\s*\bto\b\s*/-/g;
			$data{nsn} =~ s/\s*,\s*/,/g;
		} elsif (exists $data{country}) {
			my $field = substr($_,0,28); $field =~ s/^\s+//; $field =~ s/\s+$//;
			$data{country} = join(' ',$data{country},$field) if $field;
		}
	}
	if (exists $data{country}) {
		unless ($sth->execute($data{country},$data{ccode},$data{intpfx},$data{natpfx},$data{nsn},$data{tzone},$data{note})) {
			print STDERR "E: ",$sth->errstr,"\n";
			print STDERR "E: input was '",join("','",$data{country},$data{ccode},$data{intpfx},$data{natpfx},$data{nsn},$data{tzone},$data{note}),"'\n";
		}
		%data = ();
	}
}
close($fh);

$sql = q{
	CREATE TABLE IF NOT EXISTS e212a_notes (
		flag CHARACTER(1) PRIMARY KEY,
		note TEXT
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);

$sql = q{
	CREATE TABLE IF NOT EXISTS e212a (
		mcc VCHAR(3) PRIMARY KEY,
		country TEXT,
		note CHARACTER(1),
		FOREIGN KEY(country) REFERENCES cnames(country),
		FOREIGN KEY(note) REFERENCES e212a_notes(flag)
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);

$sql = q{
	CREATE TABLE IF NOT EXISTS e212b (
		country TEXT,
		network TEXT,
		mcc VCHAR(3),
		mnc VCHAR(3),
		FOREIGN KEY(mcc) REFERENCES e212a(mcc),
		PRIMARY KEY(mcc, mnc),
		FOREIGN KEY(country) REFERENCES cnames(country)
	);
};
print STDERR "S: $sql\n";
$dbh->do($sql);


print STDERR "I: disconnecting database\n";
$dbh->disconnect;
print STDERR "I: database disconnected\n";

exit;

1;

__END__
