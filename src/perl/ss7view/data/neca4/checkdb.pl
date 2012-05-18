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

my $fh = \*INFILE;
my $of = \*OUTFILE;
my ($fn,$fp);

binmode(INFILE,':utf8');
binmode(OUTFILE,':utf8');
binmode(STDERR,':utf8');

my ($dbh,$sth,$sql);

$fn = "$progdir/necadata.sqlite";
print STDERR "I: connecting to database $fn\n";
$dbh = DBI->connect("dbi:SQLite:dbname=$fn", {
		AutoCommit=>0,
	}) or die "can't open $fn";

$sql = q{PRAGMA foreign_keys = OFF;
};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;

my %psdata = ();
my %csdata = ();

$sql = q{SELECT * FROM lergst;
};
print STDERR "S: $sql";
$sth = $dbh->prepare($sql) or die $dbh->errstr;
$sth->execute;
while (my $row = $sth->fetchrow_hashref) {
	$psdata{$row->{ps}} = { %{$row} };
	$csdata{$row->{cs}} = { %{$row} };
}

$fn = "db.fix.sql";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";

$sql = q{SELECT switch,sect FROM switchdata ORDER BY switch;
};
print STDERR "S: $sql";
$sth = $dbh->prepare($sql) or die $dbh->errstr;
$sth->execute;
while (my $row = $sth->fetchrow_hashref) {
	if (length($row->{switch}) != 11) {
		my %rec = %$row;
		{
			$sql = q{SELECT npa,nxx,xxxx,yyyy,sect FROM linedata WHERE switch=?;};
			my $s = $dbh->prepare($sql) or die $dbh->errstr;
			$s->execute($rec{switch});
			while (my $r = $s->fetchrow_hashref) {
				print $of "UPDATE linedata   SET switch='$rec{switch}' WHERE switch=='$rec{switch}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx} AND xxxx='$r->{xxxx}' AND yyyy=='$r->{yyyy}'; -- FIXME: bad clli length ",length($rec{switch})," ($r->{sect})\n";
			}
		}
		{
			$sql = q{SELECT npa,nxx,x,sect FROM pooldata WHERE switch=?;};
			my $s = $dbh->prepare($sql) or die $dbh->errstr;
			$s->execute($rec{switch});
			while (my $r = $s->fetchrow_hashref) {
				print $of "UPDATE pooldata   SET switch='$rec{switch}' WHERE switch=='$rec{switch}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}' AND x=='$r->{x}'; -- FIXME: bad clli length ",length($rec{switch})," ($r->{sect})\n";
			}
		}
		{
			$sql = q{SELECT npa,nxx,sect FROM prefixdata WHERE switch=?;};
			my $s = $dbh->prepare($sql) or die $dbh->errstr;
			$s->execute($rec{switch});
			while (my $r = $s->fetchrow_hashref) {
				print $of "UPDATE prefixdata SET switch='$rec{switch}' WHERE switch=='$rec{switch}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}'; -- FIXME: bad clli length ",length($rec{switch})," ($r->{sect})\n";
			}
		}
		print $of "UPDATE switchdata SET switch='$rec{switch}' WHERE switch=='$rec{switch}'; -- FIXME: bad clli length ",length($rec{switch})," ($rec{sect})\n";
	} elsif (not exists $csdata{substr($row->{switch},4,2)}) {
		my %rec = %$row;
		{
			$sql = q{SELECT npa,nxx,xxxx,yyyy,sect FROM linedata WHERE switch=?;};
			my $s = $dbh->prepare($sql) or die $dbh->errstr;
			$s->execute($rec{switch});
			while (my $r = $s->fetchrow_hashref) {
				print $of "UPDATE linedata   SET switch='$rec{switch}' WHERE switch=='$rec{switch}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}' AND xxxx=='$r->{xxxx}' AND yyyy='$r->{yyyy}; -- FIXME: invalid state/province/territory '",substr($rec{switch},4,2),"' ($r->{sect})\n";
			}
		}
		{
			$sql = q{SELECT npa,nxx,x,sect FROM pooldata WHERE switch=?;};
			my $s = $dbh->prepare($sql) or die $dbh->errstr;
			$s->execute($rec{switch});
			while (my $r = $s->fetchrow_hashref) {
				print $of "UPDATE pooldata   SET switch='$rec{switch}' WHERE switch=='$rec{switch}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}' AND x=='$r->{x}'; -- FIXME: invalid state/province/territory '",substr($rec{switch},4,2),"' ($r->{sect})\n";
			}
		}
		{
			$sql = q{SELECT npa,nxx,sect FROM prefixdata WHERE switch=?;};
			my $s = $dbh->prepare($sql) or die $dbh->errstr;
			$s->execute($rec{switch});
			while (my $r = $s->fetchrow_hashref) {
				print $of "UPDATE prefixdata SET switch='$rec{switch}' WHERE switch=='$rec{switch}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}'; -- FIXME: invalid state/province/territory '",substr($rec{switch},4,2),"' ($r->{sect})\n";
			}
		}
		print $of "UPDATE switchdata SET switch='$rec{switch}' WHERE switch=='$rec{switch}'; -- FIXME: invalid state/province/territory '",substr($rec{switch},4,2),"' ($rec{sect})\n";
	}
}

$fn = "$progdir/../nanpa/nanpdata.sqlite";
print STDERR "I: connecting to database $fn\n";
my $dbh2 = DBI->connect("dbi:SQLite:dbname=$fn", {
		AutoCommit=>0,
	}) or die "can't open $fn";
$sql = q{SELECT npa,assigned,inservice,use,country FROM npadata;
};
print STDERR "S: $sql";
$sth = $dbh2->prepare($sql) or die $dbh2->errstr;
$sth->execute;
my %npas = ();
while (my $row = $sth->fetchrow_hashref) {
	if ($row->{assigned} and $row->{inservice} and $row->{use} eq 'G') {
		$npas{$row->{npa}} = $row->{country} ? $row->{country} : '';
	}
}
$sql = q{SELECT npa,nxx,use,rg,ocn,rc FROM nxxdata;
};
print STDERR "S: $sql";
$sth = $dbh2->prepare($sql) or die $dbh2->errstr;
$sth->execute;
my %nxxs = ();
while (my $row = $sth->fetchrow_hashref) {
	if (($row->{use} eq 'AS' or ($row->{ocn} and $row->{rc})) and $row->{rg}) {
		$nxxs{$row->{npa}}{$row->{nxx}}++;
	}
}
$sth = undef;
print STDERR "I: disconnecting database $fn\n";
$dbh2->disconnect;
undef $dbh2;
print STDERR "I: database $fn disconnected\n";

if (1) {
$sql = q{SELECT npa,nxx,xxxx,yyyy,sect FROM linedata ORDER BY npa,nxx,xxxx;
};
print STDERR "S: $sql";
$sth = $dbh->prepare($sql) or die $dbh->errstr;
$sth->execute;
while (my $row = $sth->fetchrow_hashref) {
	my $assigned = exists $nxxs{$row->{npa}}{$row->{nxx}} or $npas{$row->{npa}} !~ /CA|US/;
	if (not exists $npas{$row->{npa}} and not $assigned) {
		print $of "DELETE FROM linedata   WHERE npa=='$row->{npa}' AND nxx=='$row->{nxx}' AND xxxx=='$row->{xxxx}' AND yyyy=='$row->{yyyy}'; -- NPA $row->{npa} not usable per NANPA ($row->{sect})\n";
	} elsif (not $assigned) {
		print $of "DELETE FROM linedata   WHERE npa=='$row->{npa}' AND nxx=='$row->{nxx}' AND xxxx=='$row->{xxxx}' AND yyyy=='$row->{yyyy}'; -- NPA-NXX $row->{npa}-$row->{nxx} not usable per NANPA ($row->{sect})\n";
	}
}
}

$sql = q{SELECT npa,nxx,x,sect FROM pooldata ORDER BY npa,nxx,x;
};
print STDERR "S: $sql";
$sth = $dbh->prepare($sql) or die $dbh->errstr;
$sth->execute;
while (my $row = $sth->fetchrow_hashref) {
	my $assigned = (exists $nxxs{$row->{npa}}{$row->{nxx}} or ($npas{$row->{npa}} ne 'CA' and $npas{$row->{npa}} ne 'US') or ($row->{nxx} =~ /([2-9]11|976|958|959|310)/));
	if (not exists $npas{$row->{npa}} and not $assigned) {
		print $of "DELETE FROM pooldata   WHERE npa=='$row->{npa}' AND nxx=='$row->{nxx}' AND x=='$row->{x}'; -- NPA $row->{npa} not usable per NANPA ($row->{sect})\n";
	} elsif (not $assigned) {
		print $of "DELETE FROM pooldata   WHERE npa=='$row->{npa}' AND nxx=='$row->{nxx}' AND x=='$row->{x}'; -- NPA-NXX $row->{npa}-$row->{nxx} not usable per NANPA ($row->{sect})\n";
	}
}

$sql = q{SELECT npa,nxx,sect FROM prefixdata ORDER BY npa,nxx;
};
print STDERR "S: $sql";
$sth = $dbh->prepare($sql) or die $dbh->errstr;
$sth->execute;
while (my $row = $sth->fetchrow_hashref) {
	my $assigned = (exists $nxxs{$row->{npa}}{$row->{nxx}} or ($npas{$row->{npa}} ne 'CA' and $npas{$row->{npa}} ne 'US') or ($row->{nxx} =~ /([2-9]11|976|958|959|310)/));
	if (not exists $npas{$row->{npa}} and not $assigned) {
		print $of "DELETE FROM prefixdata WHERE npa=='$row->{npa}' AND nxx=='$row->{nxx}'; -- NPA $row->{npa} not usable per NANPA ($row->{sect})\n";
	} elsif (not $assigned) {
		print $of "DELETE FROM prefixdata WHERE npa=='$row->{npa}' AND nxx=='$row->{nxx}'; -- NPA-NXX $row->{npa}-$row->{nxx} not usable per NANPA ($row->{sect})\n";
	}
}

close($of);

undef $sth;
print STDERR "I: disconnecting database\n";
$dbh->disconnect;
print STDERR "I: database disconnected\n";

exit;

1;

__END__

