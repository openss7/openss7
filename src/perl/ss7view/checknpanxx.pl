#!/usr/bin/perl -w

use strict;

my $fh = \*INFILE;
my $db = {};

my %npas = ();
{
	my $fn = "data/nanp.txt";
	print STDERR "I: reading $fn...\n";
	open($fh,"<",$fn) or die "can't open $fn";
	while(<$fh>) { chomp;
		my ($npa,$country) = split(/\t/,$_);
		$npas{$npa} = $country;
	}
	close($fh);
}

my @nanpa_fields = ();
{
	my $fn = "data/nanpa/db.csv";
	print STDERR "I: reading $fn...\n";
	my $heading = 1;
	open($fh,"<",$fn) or die "can't open $fn";
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@nanpa_fields = @tokens;
			$heading = undef;
			next;
		}
		next if @tokens > 10 and ($tokens[10] eq 'UA');
		next if @tokens > 6  and ($tokens[6]  eq 'XXXXXXXXXX' or $tokens[6] =~ /7 digit service/i);
		next if @tokens > 5  and ($tokens[5]  eq 'Not Available' or $tokens[5] eq 'Assigned');
		next if @tokens > 3  and ($tokens[3]  eq '' or $tokens[3] eq 'MULT');
		my $data = {};
		for (my $i=0;$i<@nanpa_fields;$i++) { $data->{$nanpa_fields[$i]} = $tokens[$i] }
		$db->{nanpa}{$data->{NPA}}{$data->{NXX}}{$data->{X}} = $data;
	}
	close($fh);
}

my @neca4_fields = ();
{
	my $fn = "data/neca4/db.csv";
	print STDERR "I: reading $fn...\n";
	my $heading = 1;
	open ($fh,"<",$fn) or die "can't open $fn";
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@neca4_fields = @tokens;
			$heading = undef;
			next;
		}
		my $data = {};
		for (my $i=0;$i<@neca4_fields;$i++) { $data->{$neca4_fields[$i]} = $tokens[$i] }
		$db->{neca4}{$data->{NPA}}{$data->{NXX}}{$data->{X}} = $data;
	}
	close($fh);
}
my @local_fields = ();
{
	my $fn = "data/localcallingguide/db.csv";
	print STDERR "I: reading $fn...\n";
	my $heading = 1;
	open($fh,"<",$fn) or die "can't open $fn";
	while (<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@local_fields = @tokens;
			$heading = undef;
			next;
		}
		my $data = {};
		for (my $i=0;$i<@local_fields;$i++) { $data->{$local_fields[$i]} = $tokens[$i] }
		$db->{local}{$data->{NPA}}{$data->{NXX}}{$data->{X}} = $data;
	}
	close($fh);
}
my @telco_fields = ();
{
	my $fn = "data/telcodata/db.csv";
	print STDERR "I: reading $fn...\n";
	my $heading = 1;
	open($fh,"<",$fn) or die "can't open $fn";
	while(<$fh>) { chomp;
		s/^"//; s/"$//; my @tokens = split(/","/,$_);
		if ($heading) {
			@telco_fields = @tokens;
			$heading = undef;
			next;
		}
		next if $tokens[4] eq 'N/A' and
			$tokens[5] eq 'N/A' and
			$tokens[6] eq 'N/A';
		my $data = {};
		for (my $i=0;$i<@telco_fields;$i++) { $data->{$telco_fields[$i]} = $tokens[$i] }
		$db->{telco}{$data->{NPA}}{$data->{NXX}}{$data->{X}} = $data;
	}
	close($fh);
}

if (0) {
print STDERR "I: checking neca4 against nanpa...\n";
foreach my $npa (sort keys %{$db->{neca4}}) {
	foreach my $nxx (sort keys %{$db->{neca4}{$npa}}) {
		foreach my $x (sort keys %{$db->{neca4}{$npa}{$nxx}}) {
			unless (exists $db->{nanpa}{$npa}{$nxx}) {
				printf STDERR "E: %-3.3s %-3.3s %-1.1s nanpa assignment does not exist for neca4\n",$npa,$nxx,$x;
				foreach my $k (@neca4_fields) {
					next unless $db->{neca4}{$npa}{$nxx}{$x}{$k};
					printf STDERR "V: %-32.32s: %-32.32s\n", $k,
					$db->{neca4}{$npa}{$nxx}{$x}{$k};
				}
			}
		}
	}
}

print STDERR "I: checking local against nanpa...\n";
foreach my $npa (sort keys %{$db->{local}}) {
	foreach my $nxx (sort keys %{$db->{local}{$npa}}) {
		foreach my $x (sort keys %{$db->{local}{$npa}{$nxx}}) {
			unless (exists $db->{nanpa}{$npa}{$nxx}) {
				printf STDERR "E: %-3.3s %-3.3s %-1.1s nanpa assignment does not exist for local\n",$npa,$nxx,$x;
				foreach my $k (@local_fields) {
					next unless $db->{local}{$npa}{$nxx}{$x}{$k};
					printf STDERR "V: %-32.32s: %-32.32s\n", $k,
					$db->{local}{$npa}{$nxx}{$x}{$k};
				}
			}
		}
	}
}

print STDERR "I: checking neca4 against local...\n";
foreach my $npa (sort keys %{$db->{neca4}}) {
	foreach my $nxx (sort keys %{$db->{neca4}{$npa}}) {
		foreach my $x (sort keys %{$db->{neca4}{$npa}{$nxx}}) {
			unless (exists $db->{local}{$npa}{$nxx}) {
				printf STDERR "E: %-3.3s %-3.3s %-1.1s local assignment does not exist for neca4\n",$npa,$nxx,$x;
				foreach my $k (@neca4_fields) {
					next unless $db->{neca4}{$npa}{$nxx}{$x}{$k};
					printf STDERR "V: %-32.32s: %-32.32s\n", $k,
					$db->{neca4}{$npa}{$nxx}{$x}{$k};
				}
			}
		}
	}
}
}

print STDERR "I: checking...\n";
for (my $npa=200;$npa<1000;$npa++) {
	next if $npa =~ /00$/ or $npa =~ /11$/;
	for (my $nxx=200;$nxx<1000;$nxx++) {
		next if $nxx =~ /11$/;
		next if $nxx eq '700';
		next if $nxx eq '900';
		next if $nxx eq '958';
		next if $nxx eq '959';
		next if $nxx eq '976';
		next if $nxx eq '555';
		my $n = 1 if exists $db->{nanpa}{$npa}{$nxx};
		my $c = undef; # if exists $db->{neca4}{$npa}{$nxx} or (exists $npas{$npa} and $npas{$npa} eq 'CA');
		my $l = 1 if exists $db->{local}{$npa}{$nxx};
		my $t = 1 if exists $db->{telco}{$npa}{$nxx};
		next unless $n or $c or $l or $t;
		$c = 1;
		next if $n and $c and $l and $t;
		if ($n) {
			foreach my $x (sort keys %{$db->{nanpa}{$npa}{$nxx}}) {
				print STDERR "V: --- ---: nanpa\n";
				foreach my $k (@nanpa_fields) {
					next unless $db->{nanpa}{$npa}{$nxx}{$x}{$k};
					printf STDERR "V: %-3.3s %-3.3s: %-32.32s: %-32.32s\n",
					$npa,$nxx,$k, $db->{nanpa}{$npa}{$nxx}{$x}{$k};
				}
			}
		} else {
			printf STDERR "E: %-3.3s %-3.3s: nanpa assignment does not exist\n",$npa,$nxx;
		}
		if (exists $db->{neca4}{$npa}{$nxx}) {
			foreach my $x (sort keys %{$db->{neca4}{$npa}{$nxx}}) {
				print STDERR "V: --- ---: neca4\n";
				foreach my $k (@neca4_fields) {
					next unless $db->{neca4}{$npa}{$nxx}{$x}{$k};
					printf STDERR "V: %-3.3s %-3.3s: %-32.32s: %-32.32s\n",
					$npa,$nxx,$k, $db->{neca4}{$npa}{$nxx}{$x}{$k};
				}
			}
		} else {
			printf STDERR "E: %-3.3s %-3.3s: neca4 assignment does not exist\n",$npa,$nxx;
		}
		if ($l) {
			foreach my $x (sort keys %{$db->{local}{$npa}{$nxx}}) {
				print STDERR "V: --- ---: local\n";
				foreach my $k (@local_fields) {
					next unless $db->{local}{$npa}{$nxx}{$x}{$k};
					printf STDERR "V: %-3.3s %-3.3s: %-32.32s: %-32.32s\n",
					$npa,$nxx,$k, $db->{local}{$npa}{$nxx}{$x}{$k};
				}
			}
		} else {
			printf STDERR "E: %-3.3s %-3.3s: local assignment does not exist\n",$npa,$nxx;
		}
		if ($t) {
			foreach my $x (sort keys %{$db->{telco}{$npa}{$nxx}}) {
				print STDERR "V: --- ---: telco\n";
				foreach my $k (@telco_fields) {
					next unless $db->{telco}{$npa}{$nxx}{$x}{$k};
					printf STDERR "V: %-3.3s %-3.3s: %-32.32s: %-32.32s\n",
					$npa,$nxx,$k, $db->{telco}{$npa}{$nxx}{$x}{$k};
				}
			}
		} else {
			printf STDERR "E: %-3.3s %-3.3s: telco assignment does not exist\n",$npa,$nxx;
		}
	}
}

exit;
