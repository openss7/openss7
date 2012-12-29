#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

my $nflag;
my $bothway;
my $usonly;
my $sections;

while ($ARGV[0] =~ /^-/) {
	$_ = shift;
	last if /^--/;
	if (/^-n/) { $nflag++; next; }
	if (/^-b/) { $bothway++; next; }
	if (/^-u/) { $usonly++; next; }
	if (/^-x/) { $sections++; next; }
	die "I don't recognize this switch: $_";
}

my $fn = shift;
die "require one file argument" unless $fn;
die "no database $fn" unless -f $fn;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir =~ s/\/[^\/]*$//;
my $filedir = $fn; $filedir = "./$filedir" unless $filedir =~ /\//; $filedir =~ s/\/[^\/]*$//;
my $datadir = $filedir;
my $geondir = "$progdir/geonames";
my $nanpdir = "$progdir/nanpa";
my $codedir = "$progdir";

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

sub makechain {
	my @vals = ();
	my %vals = ();
	foreach (@_) {
		next unless length($_);
		push @vals,$_ unless $vals{$_};
		$vals{$_}++;
	}
	return 'NULL' unless @vals;
	return "'".join(',',@vals)."'";
}
sub makesects {
	my @vals = ();
	my %vals = ();
	foreach (@_) {
		if (length($_)) {
			push @vals,$_ unless $vals{$_};
			$vals{$_}++;
		}
		last unless $sections;
	}
	return 'NULL' unless @vals;
	return "'".join(',',@vals)."'";
}
sub makecrefs {
	my @vals = ();
	my %vals = ();
	foreach (@_) {
		if (length($_)) {
			push @vals,$_ unless $vals{$_};
			$vals{$_}++;
		}
	}
	return "'#fixed'" unless @vals;
	return "'".join(',',@vals)."'";
}
sub makeval {
	my $val = shift;
	return 'NULL' unless length($val);
	$val =~ s/'/''/g;
	return "'$val'";
}
sub makevnh {
	my $val = shift;
	return 'NULL' unless length($val);
	return sprintf "'%05d'", int($val);
}
sub maketime {
	my $time = shift;
	my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime($time);
	return sprintf "'%04d-%02d-%02dT%02d:%02d:%02d'",$year+1900,$mon+1,$mday,$hour,$min,$sec;
}

sub addrnorm {
	my $val = shift;
	$val =~ s/[\.,;]//g;
	$val =~ s/-/ /g;
	$val =~ s/\s{2,}/ /g;
	$val =~ s/\broad\b/rd/ig;
	$val =~ s/\bstreet\b/st/ig;
	$val =~ s/\bavenue\b/ave/ig;
	$val = "\U$val\E";
	return $val;
}

sub cmpfields {
	my ($k,$dat,$rec) = @_;
	if ($k eq 'wcaddr') {
		my ($a1,$a2) = (addrnorm($dat),addrnorm($rec));
		return 0 if $a1 eq $a2;
		return 0 if length($a1) < length($a2) and index($a2,$a1) >= 0;
		return 0 if length($a1) > length($a2) and index($a1,$a2) >= 0;
		return 1;
	} elsif ($k =~ /^(wccity|wccounty)$/) {
		return ("\U$dat\E" ne "\U$rec\E");
	} elsif ($k =~ /ocn$/) {
		return 0;
	} elsif ($k =~ /^(wcv|wch)$/) {
		return (abs(int($dat)-int($rec)) > 4);
	} elsif ($k =~ /^(wcv|wch|wclat|wclon|rcv|rch|rclat|rclon)$/) {
		return ($dat != $rec);
	} else {
		return ($dat ne $rec);
	}
}

my $pattern = '^........(';

{
my $fn = "$codedir/matching.txt";
print STDERR "I: reading $fn\n";
open(INFILE,"<:utf8",$fn) or die "can't open $fn";
my $sep = '';
while (<INFILE>) { chomp;
	my ($numb,$pat,$regex) = split(/\t/,$_);
	next unless $numb >= 50 or $pat =~ /[x]/ or ($pat =~ /[an]/ and $numb >= 5);
	next if $pat =~ /[a]/;
	$pattern .= "$sep$regex";
	$sep = '|';
}
close(INFILE);
$pattern .= ')$';
}


my ($dbh,$sth,$sql);

print STDERR "I: connecting to database $fn\n";
$dbh = DBI->connect("dbi:SQLite:$fn") or die "can't open $fn";

$sql = qq{PRAGMA foreign_keys = OFF;\n};
print STDERR "S: $sql";
$dbh->do($sql) or die $dbh->errstr;

my %psdata = ();
my %csdata = ();
my %rgdata = ();
my %ccstdata = ();

$sql = qq{SELECT * FROM lergst;\n};
print STDERR "S: $sql";
$sth = $dbh->prepare($sql) or die $dbh->errstr;
$sth->execute or die $sth->errstr;
while (my $row = $sth->fetchrow_hashref) {
	my $dat = { %$row };
	$psdata{$row->{ps}} = $dat;
	$rgdata{$row->{rg}} = $dat;
	$csdata{$row->{cs}} = $dat;
	$ccstdata{$row->{cc}}{$row->{st}} = $dat;
}

if (0) {
my %npadata = ();

$sql = qq{SELECT * FROM nanpst;\n};
print STDERR "S: $sql";
$sth = $dbh->prepare($sql) or die $dbh->errstr;
$sth->execute or die $sth->errstr;
while (my $row = $sth->fetchrow_hashref) {
	my $dat = { %$row };
	if (my $info = $ccstdata{$row->{cc}}{$row->{st}}) {
		$dat->{rg} = $info->{rg};
		$dat->{cs} = $info->{cs};
		$dat->{pc} = $info->{pc};
		$dat->{ps} = $info->{ps};
	}
	$npadata{$row->{npa}} = $dat;
}
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

#$fn = "$filedir/db.fix.sql";
#print STDERR "I: writing $fn\n";
#open(FIXES,">:utf8",$fn) or die "can't write $fn";

my %badcllis = ();

sub checkclli {
	my ($fld,$cllis) = @_;
	my ($msg,$repl,$wc);
	foreach my $clli (split(/\//,$cllis)) {
		next if exists $badcllis{$clli};
		if ($clli =~ /^([A-Z]{2}[A-Z0-9])[-_]([A-Z]{2}[A-Z0-9]{5})$/) {
			$repl = "$1 $2";
			$wc = substr($repl,0,8);
			$msg = sprintf "FIXED: improper clli format character s/%s/ /", substr($clli,3,1);
		} elsif (
			$clli eq 'XXXXXXXXXXX' or
			$clli =~ /^ASGNIC0[0-9]{3}[0-9A-Z]$/ or
			$clli eq 'ASGNPCSOCNX' or
			$clli =~ /^ASGNPCS[0-9]{3}[0-9A-Z]$/ or
			$clli eq 'RTGETSAGENT' or
			$clli eq 'N/A' or
			$clli eq 'NOCLLIKNOWN' or
			$clli =~ /_/ or
			$clli !~ /^[A-Z]{2}[A-Z0-9][ A-Z0-9][A-Z]{2}[A-Z0-9]*$/
		) {
			$msg = sprintf "FIXME: invalid clli '%s'",$clli;
			$wc = substr($clli,0,8);
		} elsif (length($clli) != 11) {
			$msg = sprintf "FIXME: bad clli length %d", length($clli);
			$wc = substr($clli,0,8);
		} elsif (not exists $csdata{substr($clli,4,2)}) {
			$msg = sprintf "FIXME: invalid state/province/territory '%s'", substr($clli,4,2);
			$wc = substr($clli,0,8);
		} elsif ($clli !~ /$pattern/) {
			$msg = sprintf "FIXME: poor equipment code '%s'", substr($clli,8,3);
		} elsif (($fld eq 'stp1' or $fld eq 'stp2') and $clli !~ /(MS2|W)$/) {
			$msg = sprintf "FIXME: %s clli should end with 'W' instead of '%s'", "\U$fld\E",substr($clli,-3,3);
		} elsif ($fld eq 'tgclli' and $clli !~ /(G[0-9T][0-9]|DC[0-9])$/) {
			$msg = sprintf "FIXME: %s clli should end with 'DC[0-9]' instead of '%s'", "\U$fld\E",substr($clli,-3,3);
		} elsif ($fld =~ /tdm/) {
			if ($fld eq 'tdm911') {
				if ($clli !~ /(ED|T)$/) {
					$msg = sprintf "FIXME: %s clli should end with 'ED' or 'T' instead of '%s'", "\U$fld\E",substr($clli,-3,3);
					{
						my $sql = "SELECT e911sw FROM switchdata WHERE switch==".makeval($clli).";\n";
						if (my $row = $dbh->selectrow_hashref($sql)) {
							$repl = $row->{e911sw} if $row->{e911sw};
						}
					}
				}
			} elsif ($fld eq 'tdmlcl') {
			} elsif ($fld eq 'tdmops') {
				if ($clli !~ /(DS[0-9A-F]|[TB])$/) {
					$msg = sprintf "FIXME: %s clli should end with 'B' or 'T' instead of '%s'", "\U$fld\E",substr($clli,-3,3);
				}
			} elsif ($clli !~ /(DS[0-9A-F]|ED|CT.|C.T|CT|T|G.T|GT|GT.|B)$/) {
				$msg = sprintf "FIXME: %s clli should not end like '%s'","\U$fld\E",substr($clli,-3,3);
			}
		}
		if ($msg) {
			$badcllis{$clli}++;
			last;
		}
	}
	return ($msg,$repl,$wc);
}

sub maxjoin {
	my $hash = shift;
	my @joins = sort keys %$hash;
	if (@joins > 10) { @joins = (splice(@joins,0,10),'...'); }
	return join(',',@joins);
}
sub maxrejoin {
	my $join = shift;
	my %joins = ();
	foreach (split(/,/,$join)) { $joins{$_}++ if $_; }
	return maxjoin(\%joins);
}

my @allswcllis = qw/switch tgclli cls45sw e911sw stp1 stp2 actual agent host mate tdm tdmilt tdmfgc tdmfgd tdmlcl tdmfgb tdmops tdm911 tdmchk tdmblv tdmpvt repl/;
my @allpccllis = qw/switch host cls45sw e911sw stp1 stp2 mate/;

if (exists $tables{switchdata}) {
	$sql = q{SELECT }.join(',',@allswcllis,'sect').q{ FROM switchdata;}.qq{\n};
	print STDERR "S: $sql";
	$sth = $dbh->prepare($sql) or die $dbh->errstr;
	$sth->execute or die $sth->errstr;
	while (my $row = $sth->fetchrow_hashref) {
		foreach my $cf (@allswcllis) {
			#print STDERR "I: testing clli $row->{$cf}\n";
			next unless $row->{$cf};
			my ($msg,$repl,$wc) = checkclli($cf,$row->{$cf});
			if ($msg) {
				$repl = $row->{$cf} unless defined $repl;
				my %rec = %$row;
				if (exists $tables{linedata}) {
					$sql = q{SELECT npa,nxx,xxxx,yyyy,sect FROM linedata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf}) or die $s->errstr;
					my (%sects,%numbs);
					my $gotone;
					while (my $r = $s->fetchrow_hashref) {
						foreach (split(',',$r->{sect})) { next unless $_; $sects{$_}++; }
						$numbs{"$r->{npa}-$r->{nxx}($r->{xxxx}-$r->{yyyy})"}++;
						$gotone++;
					}
					if ($gotone) {
						my $sects = maxjoin(\%sects);
						my $numbs = maxjoin(\%numbs);
						my $sql = sprintf "UPDATE linedata   SET switch=%s WHERE switch==%s; -- %s (%s) [%s]\n",
							makeval($repl),makeval($rec{$cf}),$msg,$sects,$numbs;
						print STDERR "X: $sql";
						print $sql;
					}
				}
				if (exists $tables{pooldata}) {
					$sql = q{SELECT npa,nxx,x,sect FROM pooldata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf}) or die $s->errstr;
					my (%sects,%numbs);
					my $gotone;
					while (my $r = $s->fetchrow_hashref) {
						foreach (split(',',$r->{sect})) { next unless $_; $sects{$_}++; }
						$numbs{"$r->{npa}-$r->{nxx}-$r->{x}"}++;
						$gotone++;
					}
					if ($gotone) {
						my $sects = maxjoin(\%sects);
						my $numbs = maxjoin(\%numbs);
						my $sql = sprintf "UPDATE pooldata   SET switch=%s WHERE switch==%s; -- %s (%s) [%s]\n",
							makeval($repl),makeval($rec{$cf}),$msg,$sects,$numbs;
						print STDERR "X: $sql";
						print $sql;
					}
				}
				if (exists $tables{prefixdata}) {
					$sql = q{SELECT npa,nxx,sect FROM prefixdata WHERE switch=?;};
					my $s = $dbh->prepare($sql) or die $dbh->errstr;
					$s->execute($rec{$cf}) or die $s->errstr;
					my (%sects,%numbs);
					my $gotone;
					while (my $r = $s->fetchrow_hashref) {
						foreach (split(',',$r->{sect})) { next unless $_; $sects{$_}++; }
						$numbs{"$r->{npa}-$r->{nxx}"}++;
						$gotone++;
					}
					if ($gotone) {
						my $sects = maxjoin(\%sects);
						my $numbs = maxjoin(\%numbs);
						my $sql = sprintf "UPDATE prefixdata SET switch=%s WHERE switch==%s; -- %s (%s) [%s]\n",
							makeval($repl),makeval($rec{$cf}),$msg,$sects,$numbs;
						print STDERR "X: $sql";
						print $sql;
					}
				}
				if (exists $tables{pcdata}) {
					foreach my $f (@allpccllis) {
						my %pntcodes = ();
						my %sections = ();
						my $gotone;
						$sql = "SELECT spc,sect FROM pcdata WHERE $f=?;";
						my $s = $dbh->prepare($sql) or die $dbh->errstr;
						$s->execute($rec{$cf}) or die $s->errstr;
						while (my $r = $s->fetchrow_hashref) {
							$pntcodes{$r->{spc}}++;
							foreach (split(/,/,$r->{sect})) { $sections{$_}++ if $_; }
							$gotone++;
						}
						if ($gotone) {
							my $pntcodes = maxjoin(\%pntcodes);
							my $sections = maxjoin(\%sections);
							my $sql = sprintf "UPDATE pcdata SET %s=%s WHERE %s==%s; -- %s (%s) [%s]\n",
								$f,makeval($repl),$f,makeval($rec{$cf}),$msg,$sections,$pntcodes;
							print STDERR "X: $sql";
							print $sql;
						}
					}
				}
				{
					foreach my $f (@allswcllis) {
						my %switches = ();
						my %sections = ();
						my $gotone;
						$sql = "SELECT switch,sect FROM switchdata WHERE $f=?;";
						my $s = $dbh->prepare($sql) or die $dbh->errstr;
						$s->execute($rec{$cf}) or die $s->errstr;
						while (my $r = $s->fetchrow_hashref) {
							$switches{$r->{switch}}++;
							foreach (split(/,/,$r->{sect})) { $sections{$_}++ if $_; }
							$gotone++;
						}
						if ($gotone) {
							my $switches = maxjoin(\%switches);
							my $sections = maxjoin(\%sections);
							my $sql = sprintf "UPDATE switchdata SET %s=%s WHERE %s==%s; -- %s (%s) [%s]\n",
								$f,makeval($repl),$f,makeval($rec{$cf}),$msg,$sections,$switches;
							print STDERR "X: $sql";
							print $sql;
						}
					}
				}
				if (exists $tables{wcdata} and defined $wc) {
					my $sql = sprintf "UPDATE wcdata SET wc=%s WHERE wc==%s; -- %s (%s)\n",makeval($wc),makeval(substr($rec{$cf},0,8)),$msg,maxrejoin($rec{sect});
					print STDERR "X: $sql";
					print $sql;
				}
			}
		}
	}
}

my %npas = ();
my %nxxs = ();

if (exists $tables{linedata} or exists $tables{pooldata} or exists $tables{prefixdata}) {
	$fn = "$nanpdir/nnpadata.sqlite";
	print STDERR "I: connecting to database $fn\n";
	my $dbh2 = DBI->connect("dbi:SQLite:dbname=$fn") or die "can't open $fn";
	$sql = qq{SELECT npa,assigned,inservice,use,country FROM npadata;\n};
	print STDERR "S: $sql";
	$sth = $dbh2->prepare($sql) or die $dbh2->errstr;
	$sth->execute or die $sth->errstr;
	while (my $row = $sth->fetchrow_hashref) {
		if ($row->{assigned} and $row->{inservice} and $row->{use} eq 'G') {
			$npas{$row->{npa}} = $row->{country} ? $row->{country} : '';
		}
	}
	$sql = qq{SELECT npa,nxx,nxxuse,region,ocn,rc,rcshort FROM prefixdata;\n};
	print STDERR "S: $sql";
	$sth = $dbh2->prepare($sql) or die $dbh2->errstr;
	$sth->execute or die $sth->errstr;
	while (my $row = $sth->fetchrow_hashref) {
		if (($row->{nxxuse} eq 'AS' or ($row->{ocn} and ($row->{rc} or $row->{rcshort}))) and $row->{region}) {
			$nxxs{$row->{npa}}{$row->{nxx}} = { %$row };
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
	$sth->execute or die $sth->errstr;
	while (my $row = $sth->fetchrow_hashref) {
		my $assigned = exists $nxxs{$row->{npa}}{$row->{nxx}} or $npas{$row->{npa}} !~ /CA|US/;
		if (not exists $npas{$row->{npa}} and not $assigned) {
			my $per = $npas{$row->{npa}}; $per = 'CANC' if $per eq 'CA'; $per = 'NANPA' if $per eq 'US';
			my $msg = (exists $nxxs{$row->{npa}}{$row->{nxx}}) ? 'not usable' : 'not defined';
			my @tsts = (); push @tsts,"npa=".makeval($row->{npa}); push @tsts,"nxx=".makeval($row->{nxx}); push @tsts,"xxxx=".makeval($row->{xxxx}); push @tsts,"yyyy=".makeval($row->{yyyy});
			my $sql = sprintf "DELETE FROM linedata   WHERE %s; -- NPA %3d %s per %s (%s)\n",
				join(',',@tsts),$row->{npa},$msg,$per,$row->{sect};
			print STDERR "X: $sql";
			print $sql;
		} elsif (not $assigned) {
			my $per = $npas{$row->{npa}}; $per = 'CANC' if $per eq 'CA'; $per = 'NANPA' if $per eq 'US';
			my $msg = (exists $nxxs{$row->{npa}}{$row->{nxx}}) ? 'not usable' : 'not defined';
			my @tsts = (); push @tsts,"npa=".makeval($row->{npa}); push @tsts,"nxx=".makeval($row->{nxx}); push @tsts,"xxxx=".makeval($row->{xxxx}); push @tsts,"yyyy=".makeval($row->{yyyy});
			my $sql = sprintf "DELETE FROM linedata   WHERE %s; -- NPA-NXX %3d-%3d %s per %s (%s)\n",
				join(',',@tsts),$row->{npa},$row->{nxx},$msg,$per,$row->{sect};
			print STDERR "X: $sql";
			print $sql;
		}
	}
}

if (exists $tables{pooldata}) {
	$sql = qq{SELECT npa,nxx,x,sect FROM pooldata ORDER BY npa,nxx,x;\n};
	print STDERR "S: $sql";
	$sth = $dbh->prepare($sql) or die $dbh->errstr;
	$sth->execute or die $sth->errstr;
	while (my $row = $sth->fetchrow_hashref) {
		my $assigned = (exists $nxxs{$row->{npa}}{$row->{nxx}} or $npas{$row->{npa}} !~ /CA|US/ or ($row->{nxx} =~ /([2-9]11|976|958|959|310)/));
		if (not exists $npas{$row->{npa}} and not $assigned) {
			my $per = $npas{$row->{npa}}; $per = 'CANC' if $per eq 'CA'; $per = 'NANPA' if $per eq 'US';
			my $msg = (exists $nxxs{$row->{npa}}{$row->{nxx}}) ? 'not usable' : 'not defined';
			my @tsts = (); push @tsts,"npa=".makeval($row->{npa}); push @tsts,"nxx=".makeval($row->{nxx}); push @tsts,"x=".makeval($row->{x});
			my $sql = sprintf "DELETE FROM pooldata   WHERE %s; -- NPA %3d %s per %s (%s)\n",
				join(',',@tsts),$row->{npa},$msg,$per,$row->{sect};
			print STDERR "X: $sql";
			print $sql;
		} elsif (not $assigned) {
			my $per = $npas{$row->{npa}}; $per = 'CANC' if $per eq 'CA'; $per = 'NANPA' if $per eq 'US';
			my $msg = (exists $nxxs{$row->{npa}}{$row->{nxx}}) ? 'not usable' : 'not defined';
			my @tsts = (); push @tsts,"npa=".makeval($row->{npa}); push @tsts,"nxx=".makeval($row->{nxx}); push @tsts,"x=".makeval($row->{x});
			my $sql = sprintf "DELETE FROM pooldata   WHERE %s; -- NPA-NXX %3d-%3d %s per %s (%s)\n",
				join(',',@tsts),$row->{npa},$row->{nxx},$msg,$per,$row->{sect};
			print STDERR "X: $sql";
			print $sql;
		}
	}
}

if (exists $tables{prefixdata}) {
	$sql = qq{SELECT npa,nxx,sect FROM prefixdata ORDER BY npa,nxx;\n};
	print STDERR "S: $sql";
	$sth = $dbh->prepare($sql) or die $dbh->errstr;
	$sth->execute or die $sth->errstr;
	my %badregions;
	while (my $row = $sth->fetchrow_hashref) {
		my $assigned = (exists $nxxs{$row->{npa}}{$row->{nxx}} or $npas{$row->{npa}} !~ /CA|US/ or ($row->{nxx} =~ /([2-9]11|976|958|959|310)/));
		if (not exists $npas{$row->{npa}} and not $assigned) {
			my $per = $npas{$row->{npa}}; $per = 'CANC' if $per eq 'CA'; $per = 'NANPA' if $per eq 'US';
			my $msg = (exists $nxxs{$row->{npa}}{$row->{nxx}}) ? 'not usable' : 'not defined';
			my @tsts = (); push @tsts,"npa=".makeval($row->{npa}); push @tsts,"nxx=".makeval($row->{nxx});
			my $sql = sprintf "DELETE FROM prefixdata WHERE %s; -- NPA %3d %s per %s (%s)\n",
				join(',',@tsts),$row->{npa},$msg,$per,$row->{sect};
			print STDERR "X: $sql";
			print $sql;
		} elsif (not $assigned) {
			my $per = $npas{$row->{npa}}; $per = 'CANC' if $per eq 'CA'; $per = 'NANPA' if $per eq 'US';
			my $msg = (exists $nxxs{$row->{npa}}{$row->{nxx}}) ? 'not usable' : 'not defined';
			my @tsts = (); push @tsts,"npa=".makeval($row->{npa}); push @tsts,"nxx=".makeval($row->{nxx});
			my $sql = sprintf "DELETE FROM prefixdata WHERE %s; -- NPA-NXX %3d-%3d %s per %s (%s)\n",
				join(',',@tsts),$row->{npa},$row->{nxx},$msg,$per,$row->{sect};
			print STDERR "X: $sql";
			print $sql;
		} else {
			my $newregion;
			my $dat = $nxxs{$row->{npa}}{$row->{nxx}};
			my $rg = $dat->{rg}; my $st = $dat->{st};
			$st = $rgdata{$rg}{ps} if $rg and not $st;
			my (@sets,@tsts);
			if ($row->{st} and $st and $row->{st} ne $st) {
				push @sets,"st=".makeval($st);
				push @tsts,"st==".makeval($row->{st});
			}
			if ($row->{region} and $rg and $row->{region} ne $rg) {
				push @sets,"region=".makeval($rg);
				push @tsts,"region==".makeval($row->{region});
				$newregion++;
			}
			if (@sets) {
				unshift @tsts,"nxx==".makeval($row->{nxx});
				unshift @tsts,"npa==".makeval($row->{npa});
				my $sql = sprintf "UPDATE prefixdata SET %s WHERE %s; -- FIXED: wrong region/state (%s)\n",
					join(',',@sets),join(',',@tsts),$row->{sect};
				print STDERR "X: $sql";
				print $sql;
				if ($newregion) {
					if ($row->{rcshort} and not exists $badregions{rcdata}{$row->{region}}{$row->{rcshort}}) {
						$sql = sprintf "UPDATE OR IGNORE rcdata SET region=%s WHERE region==%s AND rcshort==%s; -- FIXED: wrong region/state (%s)\n",
							makeval($rg),makeval($row->{region}),makeval($row->{rcshort}),$row->{sect};
						print STDERR "X: $sql";
						print $sql;
						$sql = sprintf "DELETE FROM rcdata WHERE region==%s and rcshort==%s; -- FIXED: wrong region/state (%s)\n",
							makeval($row->{region}),makeval($row->{rcshort}),$row->{sect};
						print STDERR "X: $sql";
						print $sql;
						$badregions{rcdata}{$row->{region}}{$row->{rcshort}}++;
					}
					if ($row->{rc} and not exists $badregions{rndata}{$row->{region}}{$row->{rc}}) {
						$sql = sprintf "UPDATE OR IGNORE rndata SET region=%s WHERE region==%s AND rc==%s; -- FIXED: wrong region/state (%s)\n",
							makeval($rg),makeval($row->{region}),makeval($row->{rc}),$row->{sect};
						print STDERR "X: $sql";
						print $sql;
						$sql = sprintf "DELETE FROM rndata WHERE region==%s AND rc==%s; -- FIXED: wrong region/state (%s)\n",
							makeval($row->{region}),makeval($row->{rc}),$row->{sect};
						print STDERR "X: $sql";
						print $sql;
						$badregions{rndata}{$row->{region}}{$row->{rc}}++;
					}
				}
			}
			# check region,st
		}
	}
}

undef $sth;
print STDERR "I: disconnecting database\n";
$dbh->disconnect;
print STDERR "I: database disconnected\n";

exit;

1;

__END__

