#!/usr/bin/perl

eval 'exec /usr/bin/perl -S $0 ${1+"$@"}'
	if $running_under_some_shell;

use strict;

my $program = $0; $program =~ s/^.*\///;
my $progdir = $0; $progdir = '.' unless $progdir =~ /\//; $progdir =~ s/\/[^\/]*$//;
my $datadir = "$progdir/..";
my $telcdir = "$progdir/../telcodata";

my $fh = \*INFILE;

my $nnxx = 0;
my $nxxs = {};
my $nlata = 0;
my $latas = {};
open($fh,"<","$telcdir/latanpanxx.txt");
while (<$fh>) { chomp;
	next unless /^[0-9]+\t/;
	my ($lata,$npa,$nxx) = split(/\t/,$_);
	$nnxx++ unless exists $nxxs->{$npa}{$nxx};
	$nxxs->{$npa}{$nxx} = $lata;
	$nlata++ unless exists $latas->{$lata};
	$latas->{$lata}{$npa}{$nxx} = "$npa-$nxx";
}
close($fh);

my $nclli = 0;
my $cllis = {};
open($fh,"<","$telcdir/cllinpanxx.txt");
while (<$fh>) { chomp;
	next unless /^[A-Z0-9]{11}\t/;
	my ($clli,$npa,$nxx) = split(/\t/,$_);
	$nclli++ unless exists $cllis->{$clli};
	$cllis->{$clli}{$npa}{$nxx} = "$npa-$nxx";
}
close($fh);

my $neca_cllis = do "$datadir/cllis.pm";
$neca_cllis = $neca_cllis->{clli};

my $nstate = 0;
my $states = {};
open($fh,"<","$telcdir/statenpanxx.txt");
while (<$fh>) { chomp;
	next unless /^[A-Z]+\t/;
	my ($state,$npa,$nxx) = split(/\t/,$_);
	$nstate++ unless exists $states->{$state};
	$states->{$state}{$npa}{$nxx} = "$npa-$nxx";
}
close($fh);

my $nrate = 0;
my $rates = {};
open($fh,"<","$telcdir/ratenpanxx.txt");
while (<$fh>) { chomp;
	my ($rate,$npa,$nxx) = split(/\t/,$_);
	$nrate++ unless exists $rates->{$rate};
	$rates->{$rate}{$npa}{$nxx} = "$npa-$nxx";
}
close($fh);

my $nocn = 0;
my $ocns = {};
open($fh,"<","$telcdir/ocnnpanxx.txt");
while (<$fh>) { chomp;
	next unless /^[0-9]+\t/;
	my ($ocn,$npa,$nxx) = split(/\t/,$_);
	$nocn++ unless exists $ocns->{$ocn};
	$ocns->{$ocn}{$npa}{$nxx} = "$npa-$nxx";
}
close($fh);

my $neca_ocns = do "$datadir/ocns.pm";

sub getfn {
	my ($npa,$nxx) = @_;
	my $n1 = int($nxx/100);
	my $n2 = int($nxx/10) % 10;
	my $n3 = $nxx % 10;
	my $dir = sprintf("%03d/%d/%d",$npa,$n1,$n2);
	my $stem = sprintf("%03d%03d",$npa,$nxx);
	my $fn = sprintf("%s/%s.html.xz",$dir,$stem);
	my $url = sprintf("'http://npanxxsource.com/nalennd.php?q=%s'",$stem);
	my $cmd = "mkdir -p $dir; wget -O - $url | xz > $fn";
	return $fn;
}

sub geturl {
	my ($npa,$nxx) = @_;
	my $n1 = int($nxx/100);
	my $n2 = int($nxx/10) % 10;
	my $n3 = $nxx % 10;
	my $dir = sprintf("%03d/%d/%d",$npa,$n1,$n2);
	my $stem = sprintf("%03d%03d",$npa,$nxx);
	my $fn = sprintf("%s/%s.html.xz",$dir,$stem);
	my $url = sprintf("'http://npanxxsource.com/nalennd.php?q=%s'",$stem);
	my $cmd = "mkdir -p $dir; wget -O - $url | xz > $fn";
	return $url;
}

sub getcmd {
	my ($npa,$nxx) = @_;
	my $n1 = int($nxx/100);
	my $n2 = int($nxx/10) % 10;
	my $n3 = $nxx % 10;
	my $dir = sprintf("%03d/%d/%d",$npa,$n1,$n2);
	my $stem = sprintf("%03d%03d",$npa,$nxx);
	my $fn = sprintf("%s/%s.html.xz",$dir,$stem);
	my $url = sprintf("'http://npanxxsource.com/nalennd.php?q=%s'",$stem);
	my $cmd = "mkdir -p $dir; wget -O - $url | xz > $fn";
	return $cmd;
}

sub getstate {
	my $numb = 0;
	foreach my $state (keys %{$states}) {
		my ($found,$first);
		foreach my $npa (keys %{$states->{$state}}) {
			foreach my $nxx (keys %{$states->{$state}{$npa}}) {
				my $fn = getfn($npa,$nxx);
				if (-f $fn) {
					$found = 1;
					last;
				}
				$first = [ $npa, $nxx ] unless $first;
			}
			last if $found;
		}
		if ($found) {
			$numb++;
			next;
		}
		warn "need to fetch $first->[0]-$first->[1] to establish State $state";
		warn "found $numb (of $nstate) States";
		return $first;
	}
	warn "found $numb (of $nstate) States";
	return undef;
}

sub getstatenpa {
	my $numb = 0;
	foreach my $state (keys %{$states}) {
		foreach my $npa (keys %{$states->{$state}}) {
			my ($found,$first);
			foreach my $nxx (keys %{$states->{$state}{$npa}}) {
				my $fn = getfn($npa,$nxx);
				if (-f $fn) {
					$found = 1;
					last;
				}
				$first = [ $npa, $nxx ] unless $first;
			}
			if ($found) {
				$numb++;
				next;
			}
			warn "need to fetch $npa-$first->[1] to establish NPA $npa for State $state";
			warn "found $numb State-NPAs";
			return $first;
		}
	}
	warn "found $numb State-NPAs";
	return undef;
}

sub getnpa {
	my $numb = 0;
	foreach my $npa (keys %{$nxxs}) {
		my ($found,$first);
		foreach my $nxx (keys %{$nxxs->{$npa}}) {
			my $fn = getfn($npa,$nxx);
			if (-f $fn) {
				$found = 1;
				last;
			}
			$first = [ $npa, $nxx ] unless $first;
		}
		if ($found) {
			$numb++;
			next;
		}
		warn "need to fetch $npa-$first->[1] to establish NPA $npa";
		warn "found $numb NPAs";
		return $first;
	}
	warn "found $numb NPAs";
	return undef;
}

sub getlata {
	my $numb = 0;
	foreach my $lata (keys %{$latas}) {
		my ($found,$first);
		foreach my $npa (keys %{$latas->{$lata}}) {
			foreach my $nxx (keys %{$latas->{$lata}{$npa}}) {
				my $fn = getfn($npa,$nxx);
				if (-f $fn) {
					$found = 1;
					last;
				}
				$first = [ $npa, $nxx ] unless $first;
			}
			last if $found;
		}
		if ($found) {
			$numb++;
			next;
		}
		warn "need to fetch $first->[0]-$first->[1] to establish LATA $lata";
		warn "found $numb (of $nlata) LATAs";
		return $first;
	}
	warn "found $numb (of $nlata) LATAs";
	return undef;
}

sub getlatanpa {
	my $numb = 0;
	foreach my $lata (keys %{$latas}) {
		foreach my $npa (keys %{$latas->{$lata}}) {
			my ($found,$first);
			foreach my $nxx (keys %{$latas->{$lata}{$npa}}) {
				my $fn = getfn($npa,$nxx);
				if (-f $fn) {
					$found = 1;
					last;
				}
				$first = [ $npa, $nxx ] unless $first;
			}
			if ($found) {
				$numb++;
				next;
			}
			warn "need to fetch $npa-$first->[1] to establish NPA $npa for LATA $lata";
			warn "found $numb LATA-NPAs";
			return $first;
		}
	}
	warn "found $numb LATA-NPAs";
	return undef;
}

sub getrate {
	my $numb = 0;
	foreach my $rate (keys %{$rates}) {
		my ($found,$first);
		foreach my $npa (keys %{$rates->{$rate}}) {
			foreach my $nxx (keys %{$rates->{$rate}{$npa}}) {
				my $fn = getfn($npa,$nxx);
				if (-f $fn) {
					$found = 1;
					last;
				}
				$first = [ $npa, $nxx ] unless $first;
			}
			last if $found;
		}
		if ($found) {
			$numb++;
			next;
		}
		warn "need to fetch $first->[0]-$first->[1] to establish Rate Center $rate";
		warn "found $numb (of $nrate) RCs";
		return $first;
	}
	warn "found $numb (of $nrate) RCs";
	return undef;
}

sub getocn {
	my $numb = 0;
	foreach my $ocn (keys %{$ocns}) {
		my ($found,$first);
		if (exists $neca_ocns->{$ocn}) {
			$numb++;
			next;
		}
		foreach my $npa (keys %{$ocns->{$ocn}}) {
			foreach my $nxx (keys %{$ocns->{$ocn}{$npa}}) {
				my $fn = getfn($npa,$nxx);
				if (-f $fn) {
					$found = 1;
					last;
				}
				$first = [ $npa, $nxx ] unless $first;
			}
			last if $found;
		}
		if ($found) {
			$numb++;
			next;
		}
		warn "need to fetch $first->[0]-$first->[1] to establish OCN $ocn";
		warn "found $numb (of $nocn) OCNs";
		return $first;
	}
	warn "found $numb (of $nocn) OCNs";
	return undef;
}

sub getclli {
	my $numb = 0;
	foreach my $clli (keys %{$cllis}) {
		my ($found,$first);
		if (exists $neca_cllis->{$clli}) {
			$numb++;
			next;
		}
		foreach my $npa (keys %{$cllis->{$clli}}) {
			foreach my $nxx (keys %{$cllis->{$clli}{$npa}}) {
				my $fn = getfn($npa,$nxx);
				if (-f $fn) {
					$found = 1;
					last;
				}
				$first = [ $npa, $nxx ] unless $first;
			}
			last if $found;
		}
		if ($found) {
			$numb++;
			next;
		}
		warn "need to fetch $first->[0]-$first->[1] to establish CLLI $clli";
		warn "found $numb (of $nclli) CLLIs";
		return $first;
	}
	warn "found $numb (of $nclli) CLLIs";
	return undef;
}

my @npas = ();

open($fh,"<","$datadir/nanp.txt");
while (<$fh>) { chomp;
	next if /^#/;
	next unless /^[0-9][0-9][0-9]\t/;
	my ($npa,$cc,$loc,$geo) = split(/\t/,$_);
	next if $geo;
	push @npas, $npa;
}
close($fh);

my $nnpa = scalar @npas;
my $nnxx = 800;
my $nnum = $nnpa * $nnxx;

print "There are $nnpa NPAs.\n";
print "There are $nnxx NXXs per NPA.\n";
print "There are $nnum NXXs to query.\n";

my $secs = 60 * 60 * 24 * 7;
print "There are $secs seconds in a week.\n";
my $pers = $secs/$nnum;
print "Need to query one per $pers seconds to be done in a week.\n";

while (my $state = getstate()) {
	my ($npa,$nxx) = @{$state};
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	my $sec = 60 + int(rand(120));
	warn "would download $fn from $url";
	warn "using command $cmd";
	warn "sleeping for $sec seconds";
	system($cmd);
	sleep($sec);
}

while (my $lata = getlata()) {
	my ($npa,$nxx) = @{$lata};
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	my $sec = 60 + int(rand(120));
	warn "would download $fn from $url";
	warn "using command $cmd";
	warn "sleeping for $sec seconds";
	system($cmd);
	sleep($sec);
}

while (my $npanxx = getnpa()) {
	my ($npa,$nxx) = @{$npanxx};
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	my $sec = 60 + int(rand(120));
	warn "would download $fn from $url";
	warn "using command $cmd";
	warn "sleeping for $sec seconds";
	system($cmd);
	sleep($sec);
}

while (my $statenpa = getstatenpa()) {
	my ($npa,$nxx) = @{$statenpa};
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	my $sec = 60 + int(rand(120));
	warn "would download $fn from $url";
	warn "using command $cmd";
	warn "sleeping for $sec seconds";
	system($cmd);
	sleep($sec);
}

while (my $latanpa = getlatanpa()) {
	my ($npa,$nxx) = @{$latanpa};
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	my $sec = 60 + int(rand(120));
	warn "would download $fn from $url";
	warn "using command $cmd";
	warn "sleeping for $sec seconds";
	system($cmd);
	sleep($sec);
}

while (my $ocn = getocn()) {
	my ($npa,$nxx) = @{$ocn};
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	my $sec = 60 + int(rand(120));
	warn "would download $fn from $url";
	warn "using command $cmd";
	warn "sleeping for $sec seconds";
	system($cmd);
	sleep($sec);
}

while (my $clli = getclli()) {
	my ($npa,$nxx) = @{$clli};
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	my $sec = 60 + int(rand(120));
	warn "would download $fn from $url";
	warn "using command $cmd";
	warn "sleeping for $sec seconds";
	system($cmd);
	sleep($sec);
}

while (my $rate = getrate()) {
	my ($npa,$nxx) = @{$rate};
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	my $sec = 60 + int(rand(120));
	warn "would download $fn from $url";
	warn "using command $cmd";
	warn "sleeping for $sec seconds";
	system($cmd);
	sleep($sec);
}

while (1) {
	my $npa = $npas[int(rand($nnpa))];
	my $nxx = 200 + int(rand($nnxx));
	next if $nxx % 100 == 11;
	next if $nxx == 555 or $nxx == 700 or $nxx == 950 or $nxx == 958 or $nxx == 959 or $nxx == 976;
	my $fn = getfn($npa,$nxx);
	my $url = geturl($npa,$nxx);
	my $cmd = getcmd($npa,$nxx);
	unless (-f $fn) {
		my $sec = 60 + int(rand(120));
		warn "would download $fn from $url";
		warn "using command $cmd";
		warn "sleeping for $sec seconds";
		system($cmd);
		sleep($sec);
	}
}
