#!/usr/bin/perl -w

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

use strict;
use DBI;

my $dbh = DBI->connect("DBI:SQLite:pscoast.db") or die "can't open pscoast.db";

my $sql;

my $sth;
my $coords = [];

sub writem {
	$coords->[-1][2] = 0;
	foreach my $point (@$coords) {
		my ($x,$y,$m) = @$point;
		$sth->execute($x,$y,$m) or die $sth->errstr;
	}
	$coords = [];
}

$sql = q[CREATE TABLE IF NOT EXISTS country (lon DOUBLE, lat DOUBLE, more BOOLEAN);];
$dbh->do($sql) or die $dbh->errstr;
$sql = q[INSERT OR FAIL INTO country (lon,lat,more) VALUES(?,?,?);];
$sth = $dbh->prepare($sql) or die $dbh->errstr;

$dbh->begin_work;
open(PSCOAST,"pscoast -Rd -N1 -Dl -m |") or die "can't process pscoast data";
while (<PSCOAST>) { chomp;
	next if /^#/;
	if (/^>/) { writem if (@$coords); } else {
		my ($x,$y) = split(/\t/,$_);
		push @$coords,[$x,$y,1];
	}
}
writem if (@$coords);
$dbh->commit;

$sql = q[CREATE TABLE IF NOT EXISTS state (lon DOUBLE, lat DOUBLE, more BOOLEAN);];
$dbh->do($sql) or die $dbh->errstr;
$sql = q[INSERT OR FAIL INTO state (lon,lat,more) VALUES(?,?,?);];
$sth = $dbh->prepare($sql) or die $dbh->errstr;

$dbh->begin_work;
open(PSCOAST,"pscoast -Rd -N2 -N3 -Di -m |") or die "can't process pscoast data";
while (<PSCOAST>) { chomp;
	next if /^#/;
	if (/^>/) { writem if (@$coords); } else {
		my ($x,$y) = split(/\t/,$_);
		push @$coords,[$x,$y,1];
	}
}
writem if (@$coords);
$dbh->commit;

$sql = q[CREATE TABLE IF NOT EXISTS shore (lon DOUBLE, lat DOUBLE, more BOOLEAN);];
$dbh->do($sql) or die $dbh->errstr;
$sql = q[INSERT OR FAIL INTO shore (lon,lat,more) VALUES(?,?,?);];
$sth = $dbh->prepare($sql) or die $dbh->errstr;

$dbh->begin_work;
open(PSCOAST,"pscoast -Rd -W1 -W2 -W3 -Dl -m |") or die "can't process pscoast data";
while (<PSCOAST>) { chomp;
	next if /^#/;
	if (/^>/) { writem if (@$coords); } else {
		my ($x,$y) = split(/\t/,$_);
		push @$coords,[$x,$y,1];
	}
}
writem if (@$coords);
$dbh->commit;

$sql = q[CREATE TABLE IF NOT EXISTS river (lon DOUBLE, lat DOUBLE, more BOOLEAN);];
$dbh->do($sql) or die $dbh->errstr;
$sql = q[INSERT OR FAIL INTO river (lon,lat,more) VALUES(?,?,?);];
$sth = $dbh->prepare($sql) or die $dbh->errstr;

$dbh->begin_work;
open(PSCOAST,"pscoast -Rd -I1 -I2 -I8 -Di -m |") or die "can't process pscoast data";
while (<PSCOAST>) { chomp;
	next if /^#/;
	if (/^>/) { writem if (@$coords); } else {
		my ($x,$y) = split(/\t/,$_);
		push @$coords,[$x,$y,1];
	}
}
writem if (@$coords);
$dbh->commit;

$dbh->disconnect;

exit;

1;

__END__

-Df = full resolution
-Dh = high resolution
-Di = intermediate resolution
-Dl = low resolution
-Dc = crude resolution

-N1 = National boundaries
-N2 = State boundaries
-N3 = Marine boundaries

-W1 = coastline
-W2 = lakeshore
-W3 = island-in-lake shore
-W4 = lake-in-island-in-lake shore

-I1 = permanent major rivers
-I2 = additional major rivers
-I3 = additional rivers
-I4 = minor rivers
-I5 = intermittent rivers - major
-I6 = intermittent rivers - additional
-I7 = intermittent rivers - minor
-I8 = major canals
-I9 = minor canals
-I10 = irrigation canals
-Ia = all rivers and canals (1-10)
-Ir = all permanent rivers (1-4)
-Ii = all intermittent rivers (5-7)
-Ic = all canals (8-10)
