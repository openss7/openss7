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

my @files = `ls *.sqlite` or die "no files";
$fn = $files[0]; chomp $fn;
print STDERR "I: connecting to database $fn\n";
$dbh = DBI->connect("dbi:SQLite:$fn") or die "can't open $fn";

$sql = qq{PRAGMA foreign_keys = OFF;\n};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;

my %psdata = ();
my %csdata = ();

$sql = qq{SELECT * FROM lergst;\n};
print STDERR "S: $sql";
$sth = $dbh->prepare($sql) or die $dbh->errstr;
$sth->execute;
while (my $row = $sth->fetchrow_hashref) {
	$psdata{$row->{ps}} = { %{$row} };
	$csdata{$row->{cs}} = { %{$row} };
}

print STDERR "I: fetching tables\n";
my %tables = ();
$sth = $dbh->table_info('%','%','%','TABLE');
if (my $hash = $sth->fetchall_hashref('TABLE_NAME')) {
	foreach (keys %{$hash}) {
		$tables{$_}++;
	}
} else {
	die "can't fetch tables";
}

$fn = "db.fix.sql";
print STDERR "I: writing $fn\n";
open($of,">:utf8",$fn) or die "can't write $fn";

if (exists $tables{switchdata}) {
	my @fields = qw/switch tgclli cls45sw e911sw stp1 stp2 actual agent host mate tdm tdmilt tdmfgc tdmfgd tdmlcl tdmfgb tdmops tdm911/;
	$sql = q{SELECT }.join(',',@fields,'sect').q{ FROM switchdata;}.qq{\n};
	print STDERR "S: $sql";
	$sth = $dbh->prepare($sql) or die $dbh->errstr;
	$sth->execute;
	while (my $row = $sth->fetchrow_hashref) {
		foreach my $cf (@fields) {
			#print STDERR "I: testing clli $row->{$cf}\n";
			next unless $row->{$cf};
			next if $row->{$cf} eq 'XXXXXXXXXXX';
			next if $row->{$cf} =~ /^ASGNIC0[0-9]{3}[0-9A-Z]$/;
			next if $row->{$cf} eq 'ASGNPCSOCNX';
			next if $row->{$cf} =~ /^ASGNPCS[0-9]{3}[0-9A-Z]$/;
			next if $row->{$cf} eq 'RTGETSAGENT';
			if (length($row->{$cf}) != 11) {
				my %rec = %$row;
				if (exists $tables{linedata}) {
					$sql = q{SELECT npa,nxx,xxxx,yyyy,sect FROM linedata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf});
					while (my $r = $s->fetchrow_hashref) {
						print $of "UPDATE linedata   SET switch='$rec{$cf}' WHERE switch=='$rec{$cf}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx} AND xxxx='$r->{xxxx}' AND yyyy=='$r->{yyyy}'; -- FIXME: bad clli length ",length($rec{$cf})," ($r->{sect})\n";
					}
				}
				if (exists $tables{pooldata}) {
					$sql = q{SELECT npa,nxx,x,sect FROM pooldata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf});
					while (my $r = $s->fetchrow_hashref) {
						print $of "UPDATE pooldata   SET switch='$rec{$cf}' WHERE switch=='$rec{$cf}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}' AND x=='$r->{x}'; -- FIXME: bad clli length ",length($rec{$cf})," ($r->{sect})\n";
					}
				}
				if (exists $tables{prefixdata}) {
					$sql = q{SELECT npa,nxx,sect FROM prefixdata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf});
					while (my $r = $s->fetchrow_hashref) {
						print $of "UPDATE prefixdata SET switch='$rec{$cf}' WHERE switch=='$rec{$cf}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}'; -- FIXME: bad clli length ",length($rec{$cf})," ($r->{sect})\n";
					}
				}
				print $of "UPDATE switchdata SET $cf='$rec{$cf}' WHERE $cf=='$rec{$cf}'; -- FIXME: bad clli length ",length($rec{$cf})," ($rec{sect})\n";
			} elsif (not exists $csdata{substr($row->{$cf},4,2)}) {
				my %rec = %$row;
				if (exists $tables{linedata}) {
					$sql = q{SELECT npa,nxx,xxxx,yyyy,sect FROM linedata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf});
					while (my $r = $s->fetchrow_hashref) {
						print $of "UPDATE linedata   SET switch='$rec{$cf}' WHERE switch=='$rec{$cf}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}' AND xxxx=='$r->{xxxx}' AND yyyy='$r->{yyyy}; -- FIXME: invalid state/province/territory '",substr($rec{$cf},4,2),"' ($r->{sect})\n";
					}
				}
				if (exists $tables{pooldata}) {
					$sql = q{SELECT npa,nxx,x,sect FROM pooldata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf});
					while (my $r = $s->fetchrow_hashref) {
						print $of "UPDATE pooldata   SET switch='$rec{$cf}' WHERE switch=='$rec{$cf}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}' AND x=='$r->{x}'; -- FIXME: invalid state/province/territory '",substr($rec{$cf},4,2),"' ($r->{sect})\n";
					}
				}
				if (exists $tables{prefixdata}) {
					$sql = q{SELECT npa,nxx,sect FROM prefixdata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf});
					while (my $r = $s->fetchrow_hashref) {
						print $of "UPDATE prefixdata SET switch='$rec{$cf}' WHERE switch=='$rec{$cf}' AND npa=='$r->{npa}' AND nxx=='$r->{nxx}'; -- FIXME: invalid state/province/territory '",substr($rec{$cf},4,2),"' ($r->{sect})\n";
					}
				}
				print $of "UPDATE switchdata SET $cf='$rec{$cf}' WHERE $cf=='$rec{$cf}'; -- FIXME: invalid state/province/territory '",substr($rec{$cf},4,2),"' ($rec{sect})\n";
			}
		}
	}
}

my %npas = ();
my %nxxs = ();

if (exists $tables{linedata} or exists $tables{pooldata} or exists $tables{prefixdata}) {
	$fn = "$progdir/../nanpa/nanpdata.sqlite";
	print STDERR "I: connecting to database $fn\n";
	my $dbh2 = DBI->connect("dbi:SQLite:dbname=$fn") or die "can't open $fn";
	$sql = qq{SELECT npa,assigned,inservice,use,country FROM npadata;\n};
	print STDERR "S: $sql";
	$sth = $dbh2->prepare($sql) or die $dbh2->errstr;
	$sth->execute;
	while (my $row = $sth->fetchrow_hashref) {
		if ($row->{assigned} and $row->{inservice} and $row->{use} eq 'G') {
			$npas{$row->{npa}} = $row->{country} ? $row->{country} : '';
		}
	}
	$sql = qq{SELECT npa,nxx,use,rg,ocn,rc FROM nxxdata;\n};
	print STDERR "S: $sql";
	$sth = $dbh2->prepare($sql) or die $dbh2->errstr;
	$sth->execute;
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
}

if (exists $tables{linedata}) {
	$sql = qq{SELECT npa,nxx,xxxx,yyyy,sect FROM linedata ORDER BY npa,nxx,xxxx;\n};
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

if (exists $tables{pooldata}) {
	$sql = qq{SELECT npa,nxx,x,sect FROM pooldata ORDER BY npa,nxx,x;\n};
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
}

if (exists $tables{prefixdata}) {
	$sql = qq{SELECT npa,nxx,sect FROM prefixdata ORDER BY npa,nxx;\n};
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
}

close($of);

undef $sth;
print STDERR "I: disconnecting database\n";
$dbh->disconnect;
print STDERR "I: database disconnected\n";

exit;

1;

__END__

